#include "pe_parser.h"
#include "dialog_peparsersettings.h"
#include "application.h"
#include "image_extend_object.h"
#include "image_list_imports.h"
#include "image_list_exports.h"
#include <bitset>
#include <winnt.h>

// class CPEParser

BOOL CPEParser::IsAskSettingsAvaliable()
{
	return TRUE;
}

BOOL CPEParser::AskSettings(HWND hWndParent)
{
	//выполнить диалог
	CDialogPEParserSettings Dialog;
	return Dialog.Show(hWndParent,&mSettings);
}

BOOL CPEParser::PrepareAnalize()
{
	//получить заголовки файла
	if(!RetrieveHeaders()) return FALSE;

	//если предыдущая операция удалась, можно считать,
	//что файл действительно формата PE.

	//указать требуемый размер буфера файла
	mFileBufferSize=mpImageOptionalHeader->SizeOfImage;

	//указать настройки анализатора по умолчанию
	//базовый адрес загрузки
	mSettings.PreferredBaseAddress=mpImageOptionalHeader->ImageBase;
	mSettings.BaseAddress=mSettings.PreferredBaseAddress;
	//флаги: использовать предпочтительный адрес загрузки, дизассемблировать экспорт
	mSettings.Flags=PEPSF_USE_PREFERRED_BASE_ADDRESS | PEPSF_ANALIZE_EXPORTS_CODE;

	return TRUE;
}

DWORD CPEParser::Analize()
{
	// первичный анализ должен быть уже выполнен вызовом PrepareAnalize,
	// и заголовки файла получены

	//скорректировать настройки
	// если используется предпочтительный базовый адрес загрузки, то запомнить его
	if(mSettings.Flags & PEPSF_USE_PREFERRED_BASE_ADDRESS) mSettings.BaseAddress=mSettings.PreferredBaseAddress;

	//спроецировать файл в киберпамять
	if(!MapToCyberMemory()) return FILE_PARSER_ERROR_LOADING;
	//проанализировать разделы файла
	AnalizeGeneral();
	AnalizeImport();
	AnalizeExport();
	//дизассемблировать код
	if(!ParseCode()) return FILE_PARSER_ERROR_ANALIZING;

	return FILE_PARSER_ERROR_SUCCESS;
}

BOOL CPEParser::RetrieveHeaders()
{
	//проверить достаточность размера для DOS-заголовка
	if(mFileSize < sizeof(IMAGE_DOS_HEADER)) return FALSE;
	//получить DOS-заголовок
	mpImageDosHeader=(PIMAGE_DOS_HEADER)mpFileMapping;
	//проверить сигнатуру MZ
	if(mpImageDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) return FALSE;

	//проверить корректность смещения до PE-заголовка
	if(mFileSize < mpImageDosHeader->e_lfanew+4+sizeof(IMAGE_FILE_HEADER)) return FALSE;
	//проверить сигнатуру PE
	if(*(DWORD*)((PBYTE)mpFileMapping+mpImageDosHeader->e_lfanew)!=IMAGE_NT_SIGNATURE) return FALSE;
	//получить PE-заголовок
	mpImageFileHeader=(PIMAGE_FILE_HEADER)((PBYTE)mpFileMapping+mpImageDosHeader->e_lfanew+4);

	//проверить корректность указанного размера опционального заголовка
	if(mpImageFileHeader->SizeOfOptionalHeader < sizeof(IMAGE_OPTIONAL_HEADER)) return FALSE;
	//проверить достаточность размера для опционального заголовка
	if(mFileSize < ( (PBYTE)mpImageFileHeader-(PBYTE)mpFileMapping+sizeof(IMAGE_FILE_HEADER)+mpImageFileHeader->SizeOfOptionalHeader )) return FALSE;
	//получить опциональный заголовок
	mpImageOptionalHeader=(PIMAGE_OPTIONAL_HEADER)(mpImageFileHeader+1);

	//проверить достаточность размера для всех заголовков и таблицы секций
	if(mFileSize < (
		(PBYTE)mpImageOptionalHeader-(PBYTE)mpFileMapping
		+ mpImageFileHeader->SizeOfOptionalHeader
		+ mpImageFileHeader->NumberOfSections*sizeof(IMAGE_SECTION_HEADER)
		) ) return FALSE;

	//проверить корректность указанного размера заголовков
	if(mpImageOptionalHeader->SizeOfHeaders < ( (PBYTE)mpImageOptionalHeader-(PBYTE)mpFileMapping+mpImageFileHeader->SizeOfOptionalHeader+mpImageFileHeader->NumberOfSections*sizeof(IMAGE_SECTION_HEADER) ) ) return FALSE;

	//получить указатель на таблицу секций
	mpImageSections=(PIMAGE_SECTION_HEADER)((PBYTE)mpImageOptionalHeader+mpImageFileHeader->SizeOfOptionalHeader);

	//получить указатель на последнюю секцию
//	PIMAGE_SECTION_HEADER pLastHeader=mpImageSections+mpImageFileHeader->NumberOfSections-1;
	//проверить корректность указанного размера образа
//	if(mpImageOptionalHeader->SizeOfImage > ALIGN_UP(pLastHeader->VirtualAddress+pLastHeader->Misc.VirtualSize,0x1000)) return FALSE;

	//вроде всё.
	return TRUE;
}

BOOL CPEParser::MapToCyberMemory()
{
	//спроецировать буфер в киберпамять
	if(!mpMemory->Map(mSettings.BaseAddress,mpFileBuffer,ALIGN_UP(mpImageOptionalHeader->SizeOfImage,MEMORY_PAGE_SIZE))) return FALSE;

	//скопировать заголовки
	CopyMemory(mpFileBuffer,mpFileMapping,mpImageOptionalHeader->SizeOfHeaders);

	//получить количество секций
	DWORD SectionsCount=mpImageFileHeader->NumberOfSections;
	//получить величины выравнивания
//	DWORD FileAlignment=mpImageOptionalHeader->FileAlignment;
	DWORD SectionAlignment=mpImageOptionalHeader->SectionAlignment;
	//цикл по секциям файла
	for(DWORD i=0;i<SectionsCount;++i)
	{
		//получить заголовок секции
		PIMAGE_SECTION_HEADER pSection=mpImageSections+i;

		//получить физический и виртуальный размеры секции
		DWORD PhysicalSize=pSection->SizeOfRawData;
		DWORD VirtualSize=pSection->Misc.VirtualSize;

		//если виртуальный размер равен 0
		if(!VirtualSize)
			//использовать в его качестве физический
			VirtualSize=ALIGN_UP(PhysicalSize,SectionAlignment);
		//иначе если виртуальный размер меньше физического
		else if(VirtualSize<PhysicalSize && !VirtualSize)
			//в качестве физического размера использовать виртуальный
			PhysicalSize=VirtualSize;

		//проверить корректность физического размера
		if(mFileSize < pSection->PointerToRawData+PhysicalSize) return FALSE;

		//скопировать инициализированные данные секции
		CopyMemory((PBYTE)mpFileBuffer+pSection->VirtualAddress,(PBYTE)mpFileMapping+pSection->PointerToRawData,PhysicalSize);
	}

	//всё!
	return TRUE;
}

BOOL CPEParser::AnalizeGeneral()
{
	//добавить метку на точку входа
	mpAddressMap->SetLabel(mSettings.BaseAddress+mpImageOptionalHeader->AddressOfEntryPoint,TEXT("EntryPoint"));
	//создать в точке входа функцию
	{
		CAddressData* pAddressData=mpAddressMap->OpenAddress(mSettings.BaseAddress+mpImageOptionalHeader->AddressOfEntryPoint);
		pAddressData->CreateSubroutine();
		pAddressData->Release();
	}
	//добавить метку на DOS-заглушку
	mpAddressMap->SetLabel(mSettings.BaseAddress+sizeof(IMAGE_DOS_HEADER),TEXT("DosStub"));
	//создать в DOS-заглушке функцию
	{
		CAddressData* pAddressData=mpAddressMap->OpenAddress(mSettings.BaseAddress+sizeof(IMAGE_DOS_HEADER));
		pAddressData->CreateSubroutine();
		pAddressData->Release();
	}
	return TRUE;
}

BOOL CPEParser::AnalizeImport()
{
	//получить директорию импорта
	PIMAGE_DATA_DIRECTORY pDataDirectory=mpImageOptionalHeader->DataDirectory+IMAGE_DIRECTORY_ENTRY_IMPORT;
	//если импорта нет, закончить
	if(!pDataDirectory->VirtualAddress || !pDataDirectory->Size) return TRUE;
	//считать дескриптор импорта
	CYBER_ADDRESS DescriptorAddress=mSettings.BaseAddress+pDataDirectory->VirtualAddress;
	IMAGE_IMPORT_DESCRIPTOR Descriptor;
	try
	{
		mpMemory->Data(&Descriptor,DescriptorAddress,sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	catch(CCyberMemoryPageFaultException)
	{
		goto error;
	}

	//создать список импорта
	CImageListImports* pImportsList=new CImageListImports;
	//добавить его в список дополнительных объектов
	mpExtendMap->insert(std::make_pair(IMAGE_EXTEND_LIST_IMPORTS,pImportsList));
	//получить список библиотек
	std::list<CImageListImports::CImportLibrary*>* pLibrariesList=pImportsList->GetList();

	//цикл по импортируемым библиотекам
	while(Descriptor.Name)
	{
		//получить таблицу адресов (IAT)
		CYBER_ADDRESS IATAddress=mSettings.BaseAddress+Descriptor.FirstThunk;

		//импортируемая библиотека
		CImageListImports::CImportLibrary* pImportLibrary;
		{
			//считать имя (в ASCII)
			LPSTR szName;
			try
			{
				szName=mpMemory->ReadASCIIZ(mSettings.BaseAddress+Descriptor.Name);
			}
			catch(CCyberMemoryPageFaultException)
			{
				goto error;
			}
#ifdef UNICODE
			//преобразовать строку в Unicode
			UINT Length=strlen(szName)+1;
			LPTSTR szUnicodeName=new TCHAR[Length];
			MultiByteToWideChar(CP_ACP,0,szName,-1,szUnicodeName,Length);
			//создать импортируемую библиотеку
			pImportLibrary=new CImageListImports::CImportLibrary(szUnicodeName,IATAddress);
			SafeDeleteMassive(szUnicodeName);
#else
			//создать импортируемую библиотеку
			pImportLibrary=new CImageListImports::CImportLibrary(szName,IATAddress);
#endif
			SafeDeleteMassive(szName);
		}
		//добавить библиотеку в список библиотек
		pLibrariesList->push_back(pImportLibrary);
		//получить список функций
		std::list<CImageListImports::CImportFunction*>* pFunctionsList=pImportLibrary->GetList();

		//получить первую импортируемую функцию
		CYBER_ADDRESS ThunkAddress=mSettings.BaseAddress+Descriptor.OriginalFirstThunk;
		DWORD Thunk;
		try
		{
			Thunk=mpMemory->Dword(ThunkAddress);
		}
		catch(CCyberMemoryPageFaultException)
		{
			goto error;
		}
		//счетчик функций
		UINT i=0;
		//цикл по функциям
		while(Thunk)
		{
			//если функция импортируется по номеру
			if(Thunk & 0x80000000)
			{
			}
			//иначе функция импортируется по имени
			else
			{
				//считать имя импортируемой функции (в секции импорта используется ASCII)
				LPSTR szName;
				try
				{
					szName=mpMemory->ReadASCIIZ(mSettings.BaseAddress+Thunk+2);
				}
				catch(CCyberMemoryPageFaultException)
				{
					goto error;
				}
#ifdef UNICODE
				//преобразовать строку в Unicode
				UINT Length=strlen(szName)+1;
				LPTSTR szUnicodeName=new TCHAR[Length];
				MultiByteToWideChar(CP_ACP,0,szName,-1,szUnicodeName,Length);
				//составить имя для метки
				LPTSTR szLabel=new TCHAR[Length+6];
				_stprintf(szLabel,TEXT("__imp_%s"),szUnicodeName);
				SafeDeleteMassive(szName);
#else
				//составить имя для метки
				LPTSTR szLabel=new TCHAR[Length+6];
				_stprintf(szLabel,TEXT("__imp_%s"),szName);
#endif
				//установить тип интерпретации как данные
				CAddressData* pAddressData=mpAddressMap->OpenAddress(IATAddress+i*sizeof(DWORD));
				CInterpretation* pInterpretation=new CInterpretationData(mpMemory,IATAddress+i*sizeof(DWORD),4);
				pAddressData->SetInterpretation(pInterpretation);
				mpAddressMap->SetAddressSize(IATAddress+i*sizeof(DWORD),4);
				pInterpretation->Release();
				//создать метку
				CLabel* pLabel=new CLabel(szLabel);
				SafeDeleteMassive(szLabel);
				pAddressData->SetLabel(pLabel);
				pLabel->Release();
				pAddressData->Release();

				//добавить импортируемую функцию в список функций
#ifdef UNICODE
				pFunctionsList->push_back(new CImageListImports::CImportFunction(szUnicodeName,IATAddress+i*sizeof(DWORD)));
				SafeDeleteMassive(szUnicodeName);
#else
				pFunctionsList->push_back(new CImageListImports::CImportFunction(szName,IATAddress+i*sizeof(DWORD)));
				SafeDeleteMassive(szName);
#endif
			}

			//увеличить счетчик функций
			i++;
			//перейти к следующей функции
			ThunkAddress+=sizeof(Thunk);
			try
			{
				Thunk=mpMemory->Dword(ThunkAddress);
			}
			catch(CCyberMemoryPageFaultException)
			{
				goto error;
			}
		}

		//перейти к следующей библиотеке
		DescriptorAddress+=sizeof(IMAGE_IMPORT_DESCRIPTOR);
		try
		{
			mpMemory->Data(&Descriptor,DescriptorAddress,sizeof(IMAGE_IMPORT_DESCRIPTOR));
		}
		catch(CCyberMemoryPageFaultException)
		{
			goto error;
		}
	}

	//анализ завершен успешно
	return TRUE;

	//ошибка
error:
	//раздел импорта искажен
	MessageBox(NULL,TEXT("The import section in this file is non-readable.\nPE Parser coudn't parse import."),TEXT("Import parsing"),MB_ICONSTOP);

	return FALSE;
}

BOOL CPEParser::AnalizeExport()
{
	//флаги наличия имени у функции (определяется заблаговременно, чтобы не было warning'а из-за goto).
	static std::bitset<0x10000> NamesFlags;
	NamesFlags.reset();

	//получить директорию экспорта
	PIMAGE_DATA_DIRECTORY pDataDirectory=mpImageOptionalHeader->DataDirectory+IMAGE_DIRECTORY_ENTRY_EXPORT;
	//если экспорта нет, закончить
	if(!pDataDirectory->VirtualAddress || pDataDirectory->Size<sizeof(IMAGE_EXPORT_DIRECTORY)) return TRUE;
	//считать дескриптор экспорта
	CYBER_ADDRESS DescriptorAddress=mSettings.BaseAddress+pDataDirectory->VirtualAddress;
	IMAGE_EXPORT_DIRECTORY Descriptor;
	try
	{
		mpMemory->Data(&Descriptor,DescriptorAddress,sizeof(IMAGE_EXPORT_DIRECTORY));
	}
	catch(CCyberMemoryPageFaultException)
	{
		goto error;
	}

	//создать список экспорта
	CImageListExports* pExportsList=new CImageListExports;
	//добавить его в список дополнительных объектов
	mpExtendMap->insert(std::make_pair(IMAGE_EXTEND_LIST_EXPORTS,pExportsList));
	//получить список функций
	std::list<CImageListExports::CExportFunction*>* pFunctionsList=pExportsList->GetList();

	//получить количество функций и имен
	UINT FunctionsCount=Descriptor.NumberOfFunctions;
	if(FunctionsCount>0x10000) goto error;
	UINT NamesCount=Descriptor.NumberOfNames;
	if(NamesCount>0x10000) goto error;
	//получить таблицы имен, адресов и ординалов
	CYBER_ADDRESS AddressTable=mSettings.BaseAddress+Descriptor.AddressOfFunctions;
	CYBER_ADDRESS NamesTable=mSettings.BaseAddress+Descriptor.AddressOfNames;
	CYBER_ADDRESS OrdinalsTable=mSettings.BaseAddress+Descriptor.AddressOfNameOrdinals;

	//цикл по именам
	for(UINT i=0;i<NamesCount;++i)
	{
		//считать имя, ординал, адрес и форвард функции
		LPSTR szName;
		LPSTR szForward;
		WORD Ordinal;
		CYBER_ADDRESS Address;
		//флаг форварда
		BOOL IsForward=FALSE;
		try
		{
			//считать имя
			szName=mpMemory->ReadASCIIZ(mSettings.BaseAddress+mpMemory->Dword(NamesTable+i*sizeof(DWORD)));
			//считать ординал
			Ordinal=mpMemory->Word(OrdinalsTable+i*sizeof(WORD));
			//считать адрес
			Address=mpMemory->Dword(AddressTable+Ordinal*sizeof(DWORD));

			//если адрес находится в пределах таблицы экспорта, значит, это форвард
			if(Address>=pDataDirectory->VirtualAddress && Address<pDataDirectory->VirtualAddress+pDataDirectory->Size)
				IsForward=TRUE;
			Address+=mSettings.BaseAddress;

			if(IsForward)
				//считать имя форвард-функции
				szForward=mpMemory->ReadASCIIZ(Address);
		}
		catch(CCyberMemoryPageFaultException)
		{
			goto error;
		}

		//указать флаг наличия имени
		NamesFlags[Ordinal]=true;

#ifdef UNICODE
		//преобразовать строки в Unicode
		UINT NameLength=strlen(szName)+1;
		LPTSTR szUnicodeName=new TCHAR[NameLength];
		MultiByteToWideChar(CP_ACP,0,szName,-1,szUnicodeName,NameLength);
		SafeDeleteMassive(szName);

		UINT ForwardLength;
		LPTSTR szUnicodeForward;
		if(IsForward)
		{
			ForwardLength=strlen(szForward)+1;
			szUnicodeForward=new TCHAR[ForwardLength];
			MultiByteToWideChar(CP_ACP,0,szForward,-1,szUnicodeForward,ForwardLength);
			SafeDeleteMassive(szForward);
		}
#define szName szUnicodeName
#define szForward szUnicodeForward
#endif

		//добавить функцию в список
		if(IsForward)
			pFunctionsList->push_back(new CImageListExports::CExportFunction(szName,Ordinal,szForward));
		else
			pFunctionsList->push_back(new CImageListExports::CExportFunction(szName,Ordinal,Address));

		//если функция локальная
		if(!IsForward)
		{
			//сформировать имя и установить метку
			LPTSTR szLabelName=new TCHAR[NameLength+6];
			_stprintf(szLabelName,TEXT("__exp_%s"),szName);
			CAddressData* pAddressData=mpAddressMap->OpenAddress(Address);
			CLabel* pLabel=new CLabel(szLabelName);
			SafeDeleteMassive(szLabelName);
			pAddressData->SetLabel(pLabel);
			pLabel->Release();
			//создать функцию
			pAddressData->CreateSubroutine();
			pAddressData->Release();
		}

		//освободить память
		SafeDeleteMassive(szName);
		if(IsForward)
			SafeDeleteMassive(szForward);

#ifdef UNICODE
#undef szName
#undef szForward
#endif
	}

	//цикл по функциям
	for(UINT i=0;i<FunctionsCount;++i)
	{
		//если функция имеет имя, значит, она уже обработана
		if(NamesFlags[i]) continue;

		//считать адрес
		CYBER_ADDRESS Address;
		try
		{
			Address=mSettings.BaseAddress+mpMemory->Dword(AddressTable+i*sizeof(DWORD));
		}
		catch(CCyberMemoryPageFaultException)
		{
			goto error;
		}

		//добавить функцию в список
		pFunctionsList->push_back(new CImageListExports::CExportFunction(NULL,i,Address));
	}

	return TRUE;

	//ошибка
error:
	//раздел экспорта искажен
	MessageBox(NULL,TEXT("The export section in this file is non-readable.\nPE Parser coudn't parse export."),TEXT("Export parsing"),MB_ICONSTOP);

	return FALSE;
}

BOOL CPEParser::ParseCode()
{
	//создать анализатор кода
	mpCodeParser=new CCodeParser(mpMemory,mpAddressMap,&gApplication.mInstructionSet16,&gApplication.mInstructionSet32);

	//если необходимо, дизассемблировать DOS-заглушку
	if(mSettings.Flags & PEPSF_ANALIZE_DOS_STUB)
	{
		//установить 16-битный режим
		mpCodeParser->SetMode(MODE_16BIT);
		//выполнить разбор кода DOS-заглушки
		mpCodeParser->ParseEntryPoint(mSettings.BaseAddress+sizeof(IMAGE_DOS_HEADER));
	}
	//установить 32-битный режим
	mpCodeParser->SetMode(MODE_32BIT);
	//выполнить разбор кода, начиная с точки входа
	mpCodeParser->ParseEntryPoint(mSettings.BaseAddress+mpImageOptionalHeader->AddressOfEntryPoint);

	//если необходимо, дизассемблировать экспорт
	if(mSettings.Flags & PEPSF_ANALIZE_EXPORTS_CODE)
	{
		//получить список экспорта, подготовленный на стадии AnalizeImport
		std::map<UINT,CObject*>::iterator ListIterator=mpExtendMap->find(IMAGE_EXTEND_LIST_EXPORTS);
		if(ListIterator!=mpExtendMap->end())
		{
			std::list<CImageListExports::CExportFunction*>* pExportsList=((CImageListExports*)(*ListIterator).second)->GetList();
			//установить 32-битный режим
			mpCodeParser->SetMode(MODE_32BIT);
			//цикл по экспорту
			for(std::list<CImageListExports::CExportFunction*>::iterator i=pExportsList->begin();i!=pExportsList->end();i++)
			{
				//получить функцию
				CImageListExports::CExportFunction* pFunction=*i;
				//если функция локальная
				if(pFunction->IsLocal())
				{
					//дизассемблировать функцию
					mpCodeParser->ParseEntryPoint(pFunction->GetAddress());
				}
			}
		}
	}

	//всё!
	return TRUE;
}

CYBER_ADDRESS CPEParser::GetBeginAddress()
{
	//вернуть точку входа
	return mSettings.BaseAddress+mpImageOptionalHeader->AddressOfEntryPoint;
}

VOID CPEParser::About(FILE_PARSER_ABOUT* pAbout)
{
	//указать сведения
	_tcscpy(pAbout->szName,TEXT("PE Win32 Files Parser"));
	_tcscpy(pAbout->szDescription,TEXT("This parser perform initial analize for standard Microsoft 32-bit PE files (usually \"*.exe\" or \"*.dll\" name extension)."));
	pAbout->Version=MAKELONG(0,1);
}
