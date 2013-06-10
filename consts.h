#ifndef QS_CONSTS_H
#define QS_CONSTS_H

#include "windows.h"

/*** макросы ***/

/* Ќомер - указываетс€ в машинных командах дл€ указани€ регистра.
*/

//получить идентификатор регистра по категории и номеру
#define REGISTER_ID(Category,Number)								( ((Category)<<3) | (Number) )
//получить номер регистра по идентификатору
#define REGISTER_NUMBER(ID)													( (ID) & 0x7 )

//категории регистров
#define RC_SEGMENT						0
#define RC_BYTE								1
#define RC_WORD								2
#define RC_DWORD							3
#define RC_FPU								4
#define RC_MMX								5
#define RC_SSE								6
#define RC_CR									7
#define RC_DR									8
#define RC_TR									9

//количество категорий регистров
#define RC_COUNT							10
//количество регистров
#define REGISTERS_COUNT				(RC_COUNT*8)

//номера регистров процессора

//сегментные регистры
#define REGISTER_ES						REGISTER_ID(RC_SEGMENT,0)
#define REGISTER_CS						REGISTER_ID(RC_SEGMENT,1)
#define REGISTER_SS						REGISTER_ID(RC_SEGMENT,2)
#define REGISTER_DS						REGISTER_ID(RC_SEGMENT,3)
#define REGISTER_FS						REGISTER_ID(RC_SEGMENT,4)
#define REGISTER_GS						REGISTER_ID(RC_SEGMENT,5)

//регистры - байт
#define REGISTER_AL						REGISTER_ID(RC_BYTE,0)
#define REGISTER_CL						REGISTER_ID(RC_BYTE,1)
#define REGISTER_DL						REGISTER_ID(RC_BYTE,2)
#define REGISTER_BL						REGISTER_ID(RC_BYTE,3)
#define REGISTER_AH						REGISTER_ID(RC_BYTE,4)
#define REGISTER_CH						REGISTER_ID(RC_BYTE,5)
#define REGISTER_DH						REGISTER_ID(RC_BYTE,6)
#define REGISTER_BH						REGISTER_ID(RC_BYTE,7)

//регистры - слово
#define REGISTER_AX						REGISTER_ID(RC_WORD,0)
#define REGISTER_CX						REGISTER_ID(RC_WORD,1)
#define REGISTER_DX						REGISTER_ID(RC_WORD,2)
#define REGISTER_BX						REGISTER_ID(RC_WORD,3)
#define REGISTER_SP						REGISTER_ID(RC_WORD,4)
#define REGISTER_BP						REGISTER_ID(RC_WORD,5)
#define REGISTER_SI						REGISTER_ID(RC_WORD,6)
#define REGISTER_DI						REGISTER_ID(RC_WORD,7)

//регистры - двойное слово
#define REGISTER_EAX					REGISTER_ID(RC_DWORD,0)
#define REGISTER_ECX					REGISTER_ID(RC_DWORD,1)
#define REGISTER_EDX					REGISTER_ID(RC_DWORD,2)
#define REGISTER_EBX					REGISTER_ID(RC_DWORD,3)
#define REGISTER_ESP					REGISTER_ID(RC_DWORD,4)
#define REGISTER_EBP					REGISTER_ID(RC_DWORD,5)
#define REGISTER_ESI					REGISTER_ID(RC_DWORD,6)
#define REGISTER_EDI					REGISTER_ID(RC_DWORD,7)

//регистры FPU
#define REGISTER_ST(id)				REGISTER_ID(RC_FPU,(id))

//регистры MMX
#define REGISTER_MM(id)				REGISTER_ID(RC_MMX,(id))

//регистры SSE
#define REGISTER_XMM(id)			REGISTER_ID(RC_SSE,(id))

//количество префиксов
#define PREFIX_COUNT					11

/*** флаги префиксов ***/
#define PF_CS									1
#define PF_SS									2
#define PF_DS									4
#define PF_ES									8
#define PF_FS									16
#define PF_GS									32
#define PF_OS									64
#define PF_AS									128

#define PF_LOCK								256
#define PF_REPNZ							512
#define PF_REP								1024

#define PREFIXES_COUNT				11

//маска префиксов переопределени€ сегмента
#define PREFIXES_SEGMENT			(PF_CS | PF_SS | PF_DS | PF_ES | PF_FS | PF_GS)
//маска выводимых префиксов
#define PREFIXES_VISIBLE			(PF_LOCK | PF_REPNZ | PF_REP)

/*** префиксы ***/
#define PREFIX_CS							0x2e
#define PREFIX_SS							0x36
#define PREFIX_DS							0x3e
#define PREFIX_ES							0x26
#define PREFIX_FS							0x64
#define PREFIX_GS							0x65
#define PREFIX_OS							0x66
#define PREFIX_AS							0x67

#define PREFIX_LOCK						0xF0
#define PREFIX_REPNZ					0xF2
#define PREFIX_REP						0xF3

//количество условий дл€ условных инструкций
#define CONDITIONS_COUNT			16

/*** общие константные строки ***/

extern LPCTSTR gcszDataDirectoryNames[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
extern LPCTSTR gcszRegisterNames[REGISTERS_COUNT];
extern LPCTSTR gcszPrefixNames[PREFIX_COUNT];
extern LPCTSTR gcszConditionNames[CONDITIONS_COUNT];

#endif
