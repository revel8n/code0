#include "file_parser.h"

// class CFileParser

CFileParser::CFileParser()
{
	//обнулить параметры
	mpFileMapping=NULL;
	mpMemory=NULL;
	mpAddressMap=NULL;
	mpCodeParser=NULL;
	mpExtendMap=NULL;
	mpFileBuffer=NULL;
}

CFileParser::~CFileParser()
{
	//удалить объекты
	SafeRelease(mpMemory);
	SafeRelease(mpAddressMap);
	SafeRelease(mpCodeParser);
}

VOID CFileParser::SetFile(PVOID pFileMapping,DWORD Size)
{
	//запомнить параметры
	mpFileMapping=pFileMapping;
	mFileSize=Size;
}

DWORD CFileParser::GetNeededMemorySize()
{
	//вернуть значение переменной
	return mFileBufferSize;
}

VOID CFileParser::SetFileBuffer(PVOID pBuffer)
{
	//запомнить указатель
	mpFileBuffer=pBuffer;
}

BOOL CFileParser::IsAskSettingsAvaliable()
{
	//это заглушка, дополнительных параметров не предвидится
	return FALSE;
}

BOOL CFileParser::AskSettings(HWND hWndParent)
{
	//ничего не спрашивать, это заглушка
	return TRUE;
}

VOID CFileParser::SetEnvironment(CCyberMemory* pMemory,CAddressMap* pAddressMap,std::map<UINT,CObject*>* pExtendMap)
{
	//запомнить параметры
	mpMemory=pMemory;
	mpMemory->AddRef();
	mpAddressMap=pAddressMap;
	mpAddressMap->AddRef();
	mpExtendMap=pExtendMap;
}

CCodeParser* CFileParser::GetCodeParser()
{
	//вернуть анализатор кода
	mpCodeParser->AddRef();
	return mpCodeParser;
}
