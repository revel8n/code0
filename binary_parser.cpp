#include "binary_parser.h"
#include "interpretations.h"
#include "application.h"

// class CBinaryParser

BOOL CBinaryParser::PrepareAnalize()
{
	//анализ файла не проводится, так как двоичный файл - это и в Африке двоичный файл
	//так что всегда считаем, что файл анализатору подходит

	//указать необходимый размер памяти для буфера файла
	mFileBufferSize=mFileSize;

	//всё
	return TRUE;
}

DWORD CBinaryParser::Analize()
{
	//скопировать файл в буфер
	CopyMemory(mpFileBuffer,mpFileMapping,mFileSize);
	//спроецировать буфер в киберпамять, начиная с нулевого адреса
	mpMemory->Map(0,mpFileBuffer,ALIGN_UP(mFileSize,MEMORY_PAGE_SIZE));

	//создать анализатор кода
	mpCodeParser=new CCodeParser(mpMemory,mpAddressMap,&gApplication.mInstructionSet16,&gApplication.mInstructionSet32);
	//установить 32-битный режим
	mpCodeParser->SetMode(MODE_32BIT);

	//создать метку в начале и конце файла
	mpAddressMap->SetLabel(0,TEXT("FileBegin"));
	mpAddressMap->SetLabel(mFileSize,TEXT("FileEnd"));
	//представить файл в двоичном виде (строки по 16 байт)
	for(CYBER_ADDRESS i=0;i<mFileSize;i+=16)
	{
		//получить размер адреса
		UINT Size=mFileSize-i;
		if(Size>16) Size=16;
		//открыть данные адреса
		CAddressData* pAddressData=mpAddressMap->OpenAddress(i);
		//создать интерпретацию
		CInterpretation* pInterpretation=new CInterpretationData(mpMemory,i,Size);
		//указать интерпретацию
		pAddressData->SetInterpretation(pInterpretation);
		mpAddressMap->SetAddressSize(i,Size);
		//закрыть интерпретацию и данные адреса
		pInterpretation->Release();
		pAddressData->Release();
	}

	//всё!
	return FILE_PARSER_ERROR_SUCCESS;
}

CYBER_ADDRESS CBinaryParser::GetBeginAddress()
{
	//вернуть нулевой начальный адрес
	return 0;
}

VOID CBinaryParser::About(FILE_PARSER_ABOUT* pAbout)
{
	//указать сведения
	_tcscpy(pAbout->szName,TEXT("Binary Files Parser"));
	_tcscpy(pAbout->szDescription,TEXT("This parser no perform automatic analizing and no parse whatever structures. Use this parser with non-standard executable files, or files of unknown formats."));
	pAbout->Version=MAKELONG(0,1);
}
