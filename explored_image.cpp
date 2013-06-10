#include "explored_image.h"
#include "danglewindow_danglegroup.h"
#include "danglewindow_code.h"
#include "dialog_selectfileanalizer.h"

// class CExploredImage

CExploredImage::CExploredImage(LPCTSTR szFileName)
{
	//инициализировать переменные
	mpFileBuffer=NULL;
	mpFileMapping=NULL;
	mpMemory=NULL;
	mpAddressMap=NULL;
	mpCodeParser=NULL;
	//скопировать имя файла
	_tcscpy(mszFileName,szFileName);

	//попробовать загрузить файл
	if(LoadFile(mszFileName))
	{
		//создать адресное киберпространство и карту адресов файла
		mpMemory=new CCyberMemory;
		mpAddressMap=new CAddressMap;
	}
}

CExploredImage::~CExploredImage()
{
	//освободить память файла
	SafeVirtualFree(mpFileBuffer);
	//отключить проецирование файла
	SafeUnmapViewOfFile(mpFileMapping);
	//удалить адресное киберпространство, карту адресов и анализатор кода
	SafeRelease(mpMemory);
	SafeRelease(mpAddressMap);
	SafeRelease(mpCodeParser);

	//очистить карту дополнительных объектов
	for(std::map<UINT,CObject*>::iterator i=mExtendMap.begin();i!=mExtendMap.end();i++)
		(*i).second->Release();
	mExtendMap.clear();
}

BOOL CExploredImage::IsFileOpened()
{
	//вернуть, открыт ли файл
	return mpFileMapping ? TRUE : FALSE;
}

LPCTSTR CExploredImage::GetFileName()
{
	return mszFileName;
}

BOOL CExploredImage::LoadFile(LPCTSTR szFileName)
{
	//открыть файл
	HANDLE hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE) return FALSE;
	mFileMappingSize=GetFileSize(hFile,NULL);
	HANDLE hMapping=CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);
	CloseHandle(hFile);
	if(!hMapping) return FALSE;
	mpFileMapping=MapViewOfFile(hMapping,FILE_MAP_READ,0,0,0);
	CloseHandle(hMapping);
	if(!mpFileMapping) return FALSE;

	return TRUE;
}

DWORD CExploredImage::AnalizeAndPresent(CFileParser* pFileParser,CDangleGroup* pDangleGroup)
{
	//получить необходимый объём памяти
	DWORD BufferSize=pFileParser->GetNeededMemorySize();

	//выделить память
	mpFileBuffer=VirtualAlloc(NULL,BufferSize,MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
	//указать буфер
	pFileParser->SetFileBuffer(mpFileBuffer);
	//указать параметры анализа
	pFileParser->SetEnvironment(mpMemory,mpAddressMap,&mExtendMap);

	//выполнить анализ
	DWORD Result=pFileParser->Analize();
	//если ошибка не критическая
	switch(Result)
	{
	case FILE_PARSER_ERROR_LOADING:
		//критическая ошибка
		break;
	case FILE_PARSER_ERROR_SUCCESS:
	case FILE_PARSER_ERROR_ANALIZING:
		{
			//запомнить указатель на анализатор кода
			mpCodeParser=pFileParser->GetCodeParser();

			//представить файл на экран - открыть окно просмотра кода на точке входа
			CDangleWindow* pDangleWindow=new CDangleWindowCode(this,pFileParser->GetBeginAddress(),mszFileName);
			pDangleGroup->AddWindow(pDangleWindow);
			pDangleWindow->Release();
		}
		break;
	default:
		_ASSERTE(("Unknown file parser error code.",TRUE));
		break;
	}

	//вернуть результат
	return Result;
}

CAddressMap* CExploredImage::GetAddressMap()
{
	mpAddressMap->AddRef();
	return mpAddressMap;
}

CAddressData* CExploredImage::GetAddressData(CYBER_ADDRESS Address)
{
	return mpAddressMap->GetAddress(Address);
}

CCyberMemory* CExploredImage::GetMemory()
{
	mpMemory->AddRef();
	return mpMemory;
}

CCodeParser* CExploredImage::GetCodeParser()
{
	mpCodeParser->AddRef();
	return mpCodeParser;
}

std::map<UINT,CObject*>* CExploredImage::GetExtendMap()
{
	return &mExtendMap;
}

CFileParser* CExploredImage::AskFileParser(HWND hWndParent)
{
	//выбрать анализатор для файла
	CDialogSelectFileAnalizer Dialog;
	return Dialog.Show(hWndParent,mszFileName,mpFileMapping,mFileMappingSize);
}

VOID CExploredImage::SetAddressInterpretationData(CYBER_ADDRESS Address,DWORD Size)
{
	//создать интерпретацию
	CInterpretation* pInterpretation=new CInterpretationData(mpMemory,Address,Size);
	//открыть данные адреса
	CAddressData* pAddressData=mpAddressMap->OpenAddress(Address);
	//указать интерпретацию
	pAddressData->SetInterpretation(pInterpretation);
	//указать размер адреса
	mpAddressMap->SetAddressSize(Address,Size);
	//освободить интерпретацию и данные адреса
	pInterpretation->Release();
	pAddressData->Release();
}

VOID CExploredImage::SetAddressInterpretationCode(CYBER_ADDRESS Address)
{
	//дизассемблировать код
	mpCodeParser->ParseEntryPoint(Address);
}

VOID CExploredImage::SetAddressInterpretationStringUnicode(CYBER_ADDRESS Address)
{
	//создать интерпретацию
	CInterpretationString* pInterpretation=new CInterpretationStringUnicode(mpMemory,Address);
	//открыть данные адреса
	CAddressData* pAddressData=mpAddressMap->OpenAddress(Address);
	//указать интерпретацию
	pAddressData->SetInterpretation(pInterpretation);
	//указать размер адреса
	mpAddressMap->SetAddressSize(Address,pInterpretation->GetStringSize());
	//освободить интерпретацию и данные адреса
	pInterpretation->Release();
	pAddressData->Release();
}

VOID CExploredImage::SetAddressInterpretationStringASCII(CYBER_ADDRESS Address)
{
	//создать интерпретацию
	CInterpretationString* pInterpretation=new CInterpretationStringASCII(mpMemory,Address);
	//открыть данные адреса
	CAddressData* pAddressData=mpAddressMap->OpenAddress(Address);
	//указать интерпретацию
	pAddressData->SetInterpretation(pInterpretation);
	//указать размер адреса
	mpAddressMap->SetAddressSize(Address,pInterpretation->GetStringSize());
	//освободить интерпретацию и данные адреса
	pInterpretation->Release();
	pAddressData->Release();
}

VOID CExploredImage::DeleteAddressIntepretation(CYBER_ADDRESS Address)
{
	//удалить интерпретацию
	CAddressData* pAddressData=mpAddressMap->GetAddress(Address);
	if(pAddressData)
	{
		pAddressData->SetInterpretation(NULL);
		CAddressData* pPreviousAddressData=mpAddressMap->GetAddress(Address+pAddressData->GetSize());
		if(pPreviousAddressData)
		{
			pPreviousAddressData->SetPreviousSize(1);
			pPreviousAddressData->Release();
		}
		pAddressData->SetSize(1);
		pAddressData->Release();
	}
}
