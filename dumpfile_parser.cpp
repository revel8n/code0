#include "dumpfile_parser.h"
#include "application.h"

// class CDumpFileParser

BOOL CDumpFileParser::PrepareAnalize()
{
	//получить заголовок файла
	if(mFileSize<sizeof(FILE_DUMP_HEADER)) return FALSE;
	mpHeader=(FILE_DUMP_HEADER*)mpFileMapping;
	//проверить сигнатуру файла
	if(mpHeader->Magics[0]!=FILE_DUMP_MAGIC0 || mpHeader->Magics[1]!=FILE_DUMP_MAGIC1) return FALSE;
	//проверить корректность размера файла
	if(mpHeader->FileSize!=mFileSize) return FALSE;

	//получить размеры массивов
	mRegionsCount=mpHeader->RegionsCount;
	mThreadsCount=mpHeader->ThreadsCount;
	mModulesCount=mpHeader->ModulesCount;

	//проверить достаточность размера файла для массивов заголовков регионов, потоков и модулей
	DWORD NeededFileSize=sizeof(FILE_DUMP_HEADER)+mpHeader->RegionsCount*sizeof(FILE_DUMP_REGION)+mpHeader->ThreadsCount*sizeof(FILE_DUMP_THREAD)+mpHeader->ModulesCount*sizeof(FILE_DUMP_MODULE);
	if(mFileSize<NeededFileSize) return FALSE;

	//получить указатели на массивы заголовков
	mpRegions=(FILE_DUMP_REGION*)(mpHeader+1);
	mpThreads=(FILE_DUMP_THREAD*)(mpRegions+mRegionsCount);
	mpModules=(FILE_DUMP_MODULE*)(mpThreads+mThreadsCount);
	//вычислить необходимый размер памяти как сумму размеров регионов
	mFileBufferSize=0;
	for(UINT i=0;i<mRegionsCount;++i)
		if(mpRegions[i].State==MEM_COMMIT)
			mFileBufferSize+=mpRegions[i].Size;
	//проверить достаточность размера файла
	NeededFileSize+=mFileBufferSize;
	if(mFileSize<NeededFileSize) return FALSE;

	//всё, анализатор готов к анализу
	return TRUE;
}

DWORD CDumpFileParser::Analize()
{
	//текущий указатель памяти
	PBYTE pMemory=(PBYTE)mpFileBuffer;
	//цикл по регионам
	UINT i;
	for(i=0;i<mRegionsCount;++i)
	{
		//указатель на строку с состоянием региона
		LPCTSTR szState;
		//выбор по состоянию региона
		switch(mpRegions[i].State)
		{
		case MEM_COMMIT:										//регион зарезервирован, и память передана
			//скопировать память региона
			CopyMemory(pMemory,(PBYTE)mpFileMapping+mpRegions[i].DataOffset,mpRegions[i].Size);
			//спроецировать регион в киберпамять
			mpMemory->Map(mpRegions[i].BaseAddress,pMemory,mpRegions[i].Size);
			//сдвинуть указатель на память в буфере файла
			pMemory+=mpRegions[i].Size;
			//указать строку состояния
			szState=TEXT("commit");
			break;
		case MEM_RESERVE:										//регион зарезервирован
			//указать строку состояния
			szState=TEXT("reserve");
			break;
		case MEM_FREE:											//регион свободен
			//указать строку состояния
			szState=TEXT("free");
			break;
		default:
			szState=TEXT("(unknown state)");
			break;
		}
/*				//указать интерпретацию-данные
				CAddressData* pAddressData=mpAddressMap->OpenAddress(mpRegions[i].BaseAddress);
				CInterpretation* pInterpretation=new CInterpretationData(mpMemory,mpRegions[i].BaseAddress,mpRegions[i].Size);
				pAddressData->SetInterpretation(pInterpretation);
				pInterpretation->Release();
				pAddressData->Release();
				mpAddressMap->SetAddressSize(mpRegions[i].BaseAddress,mpRegions[i].Size);*/
		//сформировать текст для метки
		TCHAR szLabel[0x40];
		_stprintf(szLabel,TEXT("Region state = \'%s\', size = %Xh"),szState,mpRegions[i].Size);
		//указать метку
		mpAddressMap->SetLabel(mpRegions[i].BaseAddress,szLabel);
	}

	//создать анализатор кода
	mpCodeParser=new CCodeParser(mpMemory,mpAddressMap,&gApplication.mInstructionSet16,&gApplication.mInstructionSet32);
	//установить 32-битный режим
	mpCodeParser->SetMode(MODE_32BIT);

	//попытаться найти точки входа модулей и дизассемблировать их
	//цикл по модулям
	for(i=0;i<mModulesCount;++i)
	{
		//получить базовый адрес модуля
		CYBER_ADDRESS BaseAddress=mpModules[i].BaseAddress;
		try
		{
			//считать DOS-заголовок модуля
			IMAGE_DOS_HEADER DosHeader;
			mpMemory->Data(&DosHeader,BaseAddress,sizeof(DosHeader));
			//проверить сигнатуру MZ
			if(DosHeader.e_magic!=IMAGE_DOS_SIGNATURE) continue;
			//получить смещение до new-exe заголовка
			DWORD FileHeaderOffset=BaseAddress+DosHeader.e_lfanew;
			//проверить сигнатуру PE
			if(mpMemory->Dword(FileHeaderOffset)!=IMAGE_NT_SIGNATURE) continue;
			//считать new-exe заголовок
			IMAGE_FILE_HEADER FileHeader;
			FileHeaderOffset+=4;
			mpMemory->Data(&FileHeader,FileHeaderOffset,sizeof(FileHeader));
			//считать опциональный заголовок
			IMAGE_OPTIONAL_HEADER OptionalHeader;
			mpMemory->Data(&OptionalHeader,FileHeaderOffset+sizeof(FileHeader),sizeof(OptionalHeader));
			//дизассемблировать точку входа
			mpCodeParser->ParseEntryPoint(BaseAddress+OptionalHeader.AddressOfEntryPoint);
		}
		catch(CCyberMemoryPageFaultException)
		{
			//возникло исключение при доступе к памяти; значит, что-то неправильно
			//в таком случае надо отказаться от анализа модуля
		}
	}

	//всё!
	return FILE_PARSER_ERROR_SUCCESS;
}

CYBER_ADDRESS CDumpFileParser::GetBeginAddress()
{
	//пока возвращаем 0
	return 0;
}

VOID CDumpFileParser::About(FILE_PARSER_ABOUT* pAbout)
{
	//указать сведения
	_tcscpy(pAbout->szName,TEXT("Code0 Dump Files Parser"));
	_tcscpy(pAbout->szDescription,TEXT("This analizer can be used to load and analize dump files created with Code0 Process Dump tool."));
	pAbout->Version=MAKELONG(0,1);
}
