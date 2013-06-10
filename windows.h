#ifndef QS_WINDOWS_H
#define QS_WINDOWS_H

#define STRICT
#define _WIN32_WINNT 0x0500

//макрос дл€ компил€ции инструмента-дампера
#define CODE0_COMPILE_DUMPING_TOOL

//отключить предупреждени€ об устаревших функци€х
#define _CRT_SECURE_NO_WARNINGS
//отключить предупреждени€ об нестандартных вызовах
#define _CRT_NON_CONFORMING_SWPRINTFS

//макросы дл€ вы€влени€ утечек пам€ти
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#endif
#include <crtdbg.h>

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

// макрос, переопредел€ющий MessageBox
#ifdef MessageBox
#undef MessageBox
#endif
#define MessageBox QsMessageBox
//прототип функции-заменител€ MessageBox
INT QsMessageBox(HWND hWndParent,LPCTSTR szText,LPCTSTR szCaption,UINT Flags);

// полезные макросы

#define SafeDelete(pObject) if(pObject) { delete pObject; pObject=NULL; } else
#define SafeDeleteMassive(pMassive) if(pMassive) { delete [] pMassive; pMassive=NULL; } else
#define SafeRelease(pObject) if(pObject) { pObject->Release(); pObject=NULL; } else
#define SafeVirtualFree(pMemory) if(pMemory) { VirtualFree(pMemory,0,MEM_RELEASE); pMemory=NULL; } else
#define SafeUnmapViewOfFile(pMapping) if(pMapping) { UnmapViewOfFile(pMapping); pMapping=NULL; } else

#endif
