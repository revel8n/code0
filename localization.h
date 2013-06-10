#ifndef QS_LOCALIZATION_H
#define QS_LOCALIZATION_H

#ifdef APSTUDIO_READONLY_SYMBOLS
#ifdef UNICODE
#undef UNICODE
#endif
#ifndef TEXT
#define TEXT(s) s
#endif
#endif

//#define LOCALIZATION_EN
#define LOCALIZATION_RU

#ifdef LOCALIZATION_EN
#define TEXT_EN(s) TEXT(s)
#else
#define TEXT_EN(s) 
#endif

#ifdef LOCALIZATION_RU
#define TEXT_RU(s) TEXT(s)
#else
#define TEXT_RU(s) 
#endif

#endif
