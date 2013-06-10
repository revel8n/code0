#include "instruction_set.h"

// class CInstructionSet realization

CInstructionSet::CInstructionSet()
{
	//выделить память
	mInstructions=(INSTRUCTION_INFORMATION*)VirtualAlloc(NULL,MAX_INSTRUCTIONS_COUNT*sizeof(INSTRUCTION_INFORMATION),MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
	mInstructionBytes=(INSTRUCTION_BYTE*)VirtualAlloc(NULL,MAX_INSTRUCTION_BYTES_COUNT*sizeof(INSTRUCTION_BYTE),MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
	//инициализировать переменные
	mInstructionsCount=0;
	mInstructionBytesCount=0x100;
}

CInstructionSet::~CInstructionSet()
{
	//освободить память
	VirtualFree(mInstructions,0,MEM_RELEASE);
	VirtualFree(mInstructionBytes,0,MEM_RELEASE);
}

VOID CInstructionSet::CorrectInstruction(INSTRUCTION_INFORMATION& II)
{
	//цикл по операндам
	for(DWORD i=0;i<II.bOperandsCount;++i)
	{
		//если операнд требует байта Mod/RM
		if(II.Operands[i].bSource==OIS_MODRM_RO || II.Operands[i].bSource==OIS_MODRM_RM)
			//поставить флаг
			II.bFlags|=IIF_MODRM;
		//если операнд требует регистровый байт
		if(II.Operands[i].bSize==OIS_REGISTER_BYTE)
		{
			//указать его
			II.bRegisterByteOffset=II.bCodeSize-1;
			//поставить флаг
			II.bFlags|=IIF_REGBYTE;
		}
	}
}

DWORD CInstructionSet::LoadInstruction_OneByte(INSTRUCTION_INFORMATION II,BYTE Code)
{
	//скорректировать инструкцию
	CorrectInstruction(II);
	//добавить инструкцию в набор
	mInstructions[mInstructionsCount]=II;
	
	//записать байт инструкции
	INSTRUCTION_BYTE& IB=mInstructionBytes[Code];
	//если байт инструкции уже присутствует
	if(IB.bFlags)
		MessageBox(NULL,TEXT(""),TEXT(""),0);
	IB.bFlags=IBF_OK | IBF_END;
	IB.bMask=0;
	IB.dwInstructionID=mInstructionsCount;

	//вернуть идентификатор инструкции
	return mInstructionsCount++;
}

DWORD CInstructionSet::LoadInstruction_TwoBytes(INSTRUCTION_INFORMATION II,BYTE Code1,BYTE Code2,BYTE Mask)
{
	DWORD Group;
	//если пространство для группы вторых байтов уже выделено
	if(mInstructionBytes[Code1].bFlags)
		//использовать его
		Group=mInstructionBytes[Code1].dwNextByteGroup;
	else
	{
		//выделить пространство
		Group=mInstructionBytesCount;
		mInstructionBytesCount+=0x100;
	}

	//записать первый байт
	INSTRUCTION_BYTE IB;
	IB.bFlags=IBF_OK;
	IB.bMask=Mask;
	IB.dwNextByteGroup=Group;
	//если байт инструкции уже присутствует
	if(mInstructionBytes[Code1].bFlags & IBF_END)
		MessageBox(NULL,TEXT(""),TEXT(""),0);
	mInstructionBytes[Code1]=IB;
	//записать второй байт
	IB.bFlags=IBF_OK | IBF_END;
	IB.bMask=0;
	IB.dwInstructionID=mInstructionsCount;
	//если байт инструкции уже присутствует
	if(mInstructionBytes[Group | Code2].bFlags)
		MessageBox(NULL,TEXT(""),TEXT(""),0);
	mInstructionBytes[Group | Code2]=IB;

	//скорректировать инструкцию
	CorrectInstruction(II);
	//добавить инструкцию в набор
	mInstructions[mInstructionsCount]=II;

	//вернуть идентификатор инструкции
	return mInstructionsCount++;
}

DWORD CInstructionSet::LoadInstruction_ThreeBytes(INSTRUCTION_INFORMATION II,BYTE Code1,BYTE Code2,BYTE Code3,BYTE Mask1,BYTE Mask2)
{
	DWORD Group1;
	//если пространство для группы вторых байтов уже выделено
	if(mInstructionBytes[Code1].bFlags)
		//использовать его
		Group1=mInstructionBytes[Code1].dwNextByteGroup;
	else
	{
		//выделить пространство
		Group1=mInstructionBytesCount;
		mInstructionBytesCount+=0x100;
	}
	DWORD Group2;
	//если пространство для группы третьих байтов уже выделено
	if(mInstructionBytes[Group1 | Code2].bFlags)
		//использовать его
		Group2=mInstructionBytes[Group1 | Code2].dwNextByteGroup;
	else
	{
		//выделить пространство
		Group2=mInstructionBytesCount;
		mInstructionBytesCount+=0x100;
	}

	//записать первый байт
	INSTRUCTION_BYTE IB;
	IB.bFlags=IBF_OK;
	IB.bMask=Mask1;
	IB.dwNextByteGroup=Group1;
	//если байт инструкции уже присутствует
	if(mInstructionBytes[Code1].bFlags & IBF_END)
		MessageBox(NULL,TEXT(""),TEXT(""),0);
	mInstructionBytes[Code1]=IB;
	//записать второй байт
	IB.bFlags=IBF_OK;
	IB.bMask=Mask2;
	IB.dwNextByteGroup=Group2;
	//если байт инструкции уже присутствует
	if(mInstructionBytes[Group1 | Code2].bFlags & IBF_END)
		MessageBox(NULL,TEXT(""),TEXT(""),0);
	mInstructionBytes[Group1 | Code2]=IB;
	//записать третий байт
	IB.bFlags=IBF_OK | IBF_END;
	IB.bMask=0;
	IB.dwInstructionID=mInstructionsCount;
	//если байт инструкции уже присутствует
	if(mInstructionBytes[Group2 | Code3].bFlags)
		MessageBox(NULL,TEXT(""),TEXT(""),0);
	mInstructionBytes[Group2 | Code3]=IB;

	//скорректировать инструкцию
	CorrectInstruction(II);
	//добавить инструкцию в набор
	mInstructions[mInstructionsCount]=II;

	//вернуть идентификатор инструкции
	return mInstructionsCount++;
}

VOID CInstructionSet::LoadPrefix(BYTE Code,DWORD Flag)
{
	//записать байт инструкции
	INSTRUCTION_BYTE IB;
	IB.bFlags=IBF_OK | IBF_PREFIX;
	IB.bMask=0;
	IB.dwPrefixFlag=Flag;
	mInstructionBytes[Code]=IB;
}

VOID CInstructionSet::SetInstructionFlag(DWORD Flag)
{
	mInstructions[mInstructionsCount-1].bFlags|=Flag;
}

#include <stdio.h>
#include "consts.h"

VOID CInstructionSet::PrintHTML(LPCTSTR szFile,DWORD GroupByte)
{
	//открыть файл
	FILE* f=_tfopen(szFile,TEXT("w"));

	//HTML-заголовок
	fprintf(f,"<HTML><HEAD><TITLE>Таблица опкодов</TITLE></HEAD><BODY><H1>Таблица опкодов</H1>");
	if(GroupByte)
		fprintf(f,"<H2>Первый байт %02X</H2>",GroupByte);
	fprintf(f,"<TABLE style=\"FONT-SIZE: 8pt; FONT-FAMILY: Courier New\" width=\"200%\" border=\"1\" bordercolordark=\"0\" bordercolorlight=\"0\">");

	DWORD Group;
	if(GroupByte)
		Group=mInstructionBytes[GroupByte].dwNextByteGroup;
	else
		Group=0;

	// первая строка
	fprintf(f,"<TR valign=\"middle\" align=\"center\"><TD></TD>");
	for(DWORD x=0;x<0x10;++x)
		fprintf(f,"<TD>%X</TD>",x);
	fprintf(f,"</TR>");

	//строки
	for(DWORD y=0;y<0x10;++y)
	{
		fprintf(f,"<TR valign=\"middle\" align=\"center\"><TD>%X</TD>",y);
		//столбцы
		for(DWORD x=0;x<0x10;++x)
		{
			fprintf(f,"<TD>");

			//байт инструкции
			INSTRUCTION_BYTE IB=mInstructionBytes[Group+y*0x10+x];
			if(IB.bFlags & (IBF_OK | IBF_END))
			{
				//информация об инструкции
				INSTRUCTION_INFORMATION II=mInstructions[IB.dwInstructionID];
				//вывести имя
				fprintf(f,"<B>");
				for(LPCTSTR s=II.szName;*s;s++)
					fprintf(f,"%c",*((LPCSTR)s));
				fprintf(f,"</B><BR>");

				//цикл по операндам
				for(DWORD i=0;i<II.bOperandsCount;++i)
				{
					//операнд
					OPERAND_INFORMATION OI=II.Operands[i];

					if(i)
						fprintf(f,", ");

					//выбор по типу операнда
					switch(OI.bSource)
					{
					case OIS_DATA:
					case OIS_SIGNED_DATA:
						switch(OI.bSize)
						{
						case 1:
							fprintf(f,"ib");
							break;
						case 2:
							fprintf(f,"iw");
							break;
						case 4:
							fprintf(f,"id");
							break;
						}
						break;
					case OIS_REGISTER_BYTE:
						break;
					case OIS_MODRM_RO:
						switch(OI.bRegisterCategory)
						{
						case RC_SEGMENT:
							fprintf(f,"sr");
							break;
						case RC_BYTE:
							fprintf(f,"r8");
							break;
						case RC_WORD:
							fprintf(f,"r16");
							break;
						case RC_DWORD:
							fprintf(f,"r32");
							break;
						}
						break;
					case OIS_MODRM_RM:
						switch(OI.bRegisterCategory)
						{
						case RC_SEGMENT:
							fprintf(f,"sr");
							break;
						case RC_BYTE:
							fprintf(f,"r8");
							break;
						case RC_WORD:
							fprintf(f,"r16");
							break;
						case RC_DWORD:
							fprintf(f,"r32");
							break;
						}
						fprintf(f,"/m%d",OI.bSize*8);
						break;
					case OIS_MEMORY_IM:
						fprintf(f,"[im]");
						break;
					case OIS_CONST_REGISTER:
						{
							for(LPCTSTR s=gcszRegisterNames[REGISTER_ID(OI.bRegisterCategory,OI.bConstRegister)];*s;s++)
								fprintf(f,"%c",*((LPCSTR)s));
						}
						break;
					}
				}
			}
			else
				fprintf(f,"-");
			fprintf(f,"</TD>");

		}
		//конец строки
		fprintf(f,"</TR>");
	}

	//конец файла
	fprintf(f,"</TABLE></BODY></HTML>");

	//закрыть файл
	fclose(f);
}
