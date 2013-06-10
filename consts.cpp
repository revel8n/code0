#include "consts.h"

LPCTSTR gcszDataDirectoryNames[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]={
	TEXT("EXPORT"),
	TEXT("IMPORT"),
	TEXT("RESOURCE"),
	TEXT("EXCEPTION"),
	TEXT("SECURITY"),
	TEXT("BASERELOC"),
	TEXT("DEBUG"),
	TEXT("ARCHITECTURE"),
	TEXT("GLOBALPTR"),
	TEXT("TLS"),
	TEXT("LOAD_CONFIG"),
	TEXT("BOUND_IMPORT"),
	TEXT("IAT"),
	TEXT("DELAY_IMPORT"),
	TEXT("COM_DESCRIPTOR"),
	TEXT("RESERVED")
};

LPCTSTR gcszRegisterNames[REGISTERS_COUNT]={
	TEXT("ES"),			TEXT("CS"),			TEXT("SS"),			TEXT("DS"),			TEXT("FS"),			TEXT("GS"),			TEXT("n/d"),		TEXT("n/d"),
	TEXT("AL"),			TEXT("CL"),			TEXT("DL"),			TEXT("BL"),			TEXT("AH"),			TEXT("CH"),			TEXT("DH"),			TEXT("BH"),
	TEXT("AX"),			TEXT("CX"),			TEXT("DX"),			TEXT("BX"),			TEXT("SP"),			TEXT("BP"),			TEXT("SI"),			TEXT("DI"),
	TEXT("EAX"),		TEXT("ECX"),		TEXT("EDX"),		TEXT("EBX"),		TEXT("ESP"),		TEXT("EBP"),		TEXT("ESI"),		TEXT("EDI"),
	TEXT("ST(0)"),	TEXT("ST(1)"),	TEXT("ST(2)"),	TEXT("ST(3)"),	TEXT("ST(4)"),	TEXT("ST(5)"),	TEXT("ST(6)"),	TEXT("ST(7)"),
	TEXT("MM0"),		TEXT("MM1"),		TEXT("MM2"),		TEXT("MM3"),		TEXT("MM4"),		TEXT("MM5"),		TEXT("MM6"),		TEXT("MM7"),
	TEXT("XMM0"),		TEXT("XMM1"),		TEXT("XMM2"),		TEXT("XMM3"),		TEXT("XMM4"),		TEXT("XMM5"),		TEXT("XMM6"),		TEXT("XMM7"),
	TEXT("CR0"),		TEXT("CR1"),		TEXT("CR2"),		TEXT("CR3"),		TEXT("CR4"),		TEXT("CR_invalid"),	TEXT("CR_invalid"),	TEXT("CR_invalid"),
	TEXT("DR0"),		TEXT("DR1"),		TEXT("DR2"),		TEXT("DR3"),		TEXT("DR4"),		TEXT("DR5"),		TEXT("DR6"),		TEXT("DR7"),
	TEXT("TR0"),		TEXT("TR1"),		TEXT("TR2"),		TEXT("TR3"),		TEXT("TR4"),		TEXT("TR5"),		TEXT("TR6"),		TEXT("TR7")
};

LPCTSTR gcszPrefixNames[PREFIX_COUNT]={
	TEXT("cs"), TEXT("ss"), TEXT("ds"), TEXT("es"), TEXT("fs"), TEXT("gs"), TEXT("os"), TEXT("as"),
	TEXT("lock"), TEXT("repnz"), TEXT("rep")
};

LPCTSTR gcszConditionNames[CONDITIONS_COUNT]={
	TEXT("o"), TEXT("no"), TEXT("c"), TEXT("nc"), TEXT("z"), TEXT("nz"), TEXT("na"), TEXT("a"), TEXT("s"), TEXT("ns"), TEXT("p"), TEXT("np"), TEXT("l"), TEXT("nl"), TEXT("ng"), TEXT("g")
};
