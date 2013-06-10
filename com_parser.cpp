#include "com_parser.h"
#include "dialog_comparsersettings.h"
#include "application.h"

// class CCOMParser

BOOL CCOMParser::IsAskSettingsAvaliable()
{
	return TRUE;
}

BOOL CCOMParser::AskSettings(HWND hWndParent)
{
	//выполнить диалог
	CDialogCOMParserSettings Dialog;
	return Dialog.Show(hWndParent,&mSettings);
}

BOOL CCOMParser::PrepareAnalize()
{
	//особого анализа формата файла проводить не нужно, так как COM-файл нельзя определить
	//проверить только, что первые два байта файла - не MZ
	//(такой алгоритм использует MS-DOS)

	if(mFileSize>=2)
		if(*(WORD*)mpFileMapping==IMAGE_DOS_SIGNATURE)
			return FALSE;

	//вычислить необходимый размер памяти с учётом выравнивания (на одну страницу больше)
	mFileBufferSize=mFileSize+MEMORY_PAGE_SIZE;

	//указать настройки анализатора по умолчанию
	mSettings.BaseAddress=0x100;
	mSettings.EntryPointAddress=0x100;
	mSettings.Flags=0;

	//всё, проверки закончены
	return TRUE;
}

DWORD CCOMParser::Analize()
{
	//получить адрес начала проецирования в киберпамять
	CYBER_ADDRESS MappingBegin=ALIGN_DOWN(mSettings.BaseAddress,MEMORY_PAGE_SIZE);
	//скопировать файл в буфер с учётом сдвига
	CopyMemory((PBYTE)mpFileBuffer+mSettings.BaseAddress-MappingBegin,mpFileMapping,mFileSize);
	//спроецировать буфер в киберпамять, начиная с заданного адреса
	if(!mpMemory->Map(MappingBegin,mpFileBuffer,ALIGN_UP(mFileSize,MEMORY_PAGE_SIZE))) return FILE_PARSER_ERROR_LOADING;

	//создать анализатор кода
	mpCodeParser=new CCodeParser(mpMemory,mpAddressMap,&gApplication.mInstructionSet16,&gApplication.mInstructionSet32);
	//установить нужный режим
	mpCodeParser->SetMode((mSettings.Flags & COMPSF_32BIT) ? MODE_32BIT : MODE_16BIT);
	//создать метку в точке входа
	mpAddressMap->SetLabel(mSettings.EntryPointAddress,TEXT("EntryPoint"));
	CAddressData* pAddress=mpAddressMap->OpenAddress(mSettings.EntryPointAddress);
	pAddress->CreateSubroutine();
	pAddress->Release();
	//выполнить разбор кода, начиная с точки входа
	mpCodeParser->ParseEntryPoint(mSettings.EntryPointAddress);

	//если указано разбирать таблицу прерываний
	if(mSettings.Flags & COMPSF_INTERRUPTTABLE)
		for(unsigned i=0;i<256;++i)
			try
			{
				DWORD address=mpMemory->Dword(i*4);
				address=(address & 0xFFFF) + (address >> 12);
				if(address)
				{
					//создать метку для элемента таблицы
					WCHAR buffer[0x30];
					_stprintf(buffer,TEXT("int_%02xh_entry"),i+1);
					mpAddressMap->SetLabel(i*4,buffer);
					//отметить как данные
					CAddressData* pAddressData=mpAddressMap->OpenAddress(i*4);
					CInterpretation* pInterpretation=new CInterpretationData(mpMemory,i*4,4);
					pAddressData->SetInterpretation(pInterpretation);
					mpAddressMap->SetAddressSize(i*4,4);
					pInterpretation->Release();
					pAddressData->Release();

					//создать метку для кода
					_stprintf(buffer,TEXT("int_%02xh_handler"),i+1);
					mpAddressMap->SetLabel(address,buffer);
					//создать подпрограмму
					pAddressData=mpAddressMap->OpenAddress(address);
					pAddressData->CreateSubroutine();
					pAddressData->Release();
					//выполнить разбор кода
					mpCodeParser->ParseEntryPoint(address);
				}
			}
			catch(CCyberMemoryPageFaultException)
			{
			}

	//всё!
	return FILE_PARSER_ERROR_SUCCESS;
}

CYBER_ADDRESS CCOMParser::GetBeginAddress()
{
	//вернуть начальный адрес
	return mSettings.EntryPointAddress;
}

VOID CCOMParser::About(FILE_PARSER_ABOUT* pAbout)
{
	//указать сведения
	_tcscpy(pAbout->szName,TEXT("MS-DOS .COM Files Parser"));
	_tcscpy(pAbout->szDescription,TEXT("This parser is used to disassembly standard COM files (usually \"*.com\" file mask), such consist of only 16-bit code. Be sure that your file is COM file because no checks are performed."));
	pAbout->Version=MAKELONG(0,1);
}
