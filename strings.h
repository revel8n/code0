#ifndef QS_STRINGS_H
#define QS_STRINGS_H

/*
‘айл содержит некоторые вспомогательные функции дл€ работы со строками.

ќкончание I в конце названи€ функции означает, что функци€ выполн€ет сравнение
без учета регистра символов.
*/

#include "windows.h"

//вы€снить, содержитс€ ли строка-фильтр в заданной строке
BOOL IsAgreeFilter(LPCTSTR szString,LPCTSTR szFilter);
BOOL IsAgreeFilterI(LPCTSTR szString,LPCTSTR szFilter);

#endif
