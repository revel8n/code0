#include "application.h"
#include "consts.h"

/*** ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ***/

//указатели на 16-битный и 32-битный наборы инструкций
CInstructionSet* s16;
CInstructionSet* s32;

/*** ОБЩИЕ ФУНКЦИИ ***/

//создать операнд - непосредственные данные
OPERAND_INFORMATION cnst(BOOL bSigned,BYTE bSize,BYTE bDataReference)
{
	//результирующий операнд
	OPERAND_INFORMATION O;

	ZeroMemory(&O,sizeof(O));
	O.bSource=bSigned ? OIS_SIGNED_DATA : OIS_DATA;
	O.bSize=bSize;
	O.bDataReference=bDataReference;

	return O;
}

//создать операнд - регистр из регистрового байта
OPERAND_INFORMATION reg(BYTE bDataReference,BYTE bRegisterCategory)
{
	//результирующий операнд
	OPERAND_INFORMATION O;

	ZeroMemory(&O,sizeof(O));
	O.bSource=OIS_REGISTER_BYTE;
	O.bDataReference=bDataReference;
	O.bRegisterCategory=bRegisterCategory;

	return O;
}

//создать операнд - поле R/O байта ModR/M
OPERAND_INFORMATION ro(BYTE bDataReference,BYTE bRegisterCategory)
{
	//результирующий операнд
	OPERAND_INFORMATION O;

	ZeroMemory(&O,sizeof(O));
	O.bSource=OIS_MODRM_RO;
	O.bDataReference=bDataReference;
	O.bRegisterCategory=bRegisterCategory;

	return O;
}

//создать операнд - поле R/M байта ModR/M
OPERAND_INFORMATION rm(BYTE bSize,BYTE bAddressReference,BYTE bDataReference,BYTE bRegisterCategory)
{
	//результирующий операнд
	OPERAND_INFORMATION O;

	ZeroMemory(&O,sizeof(O));
	O.bSource=OIS_MODRM_RM;
	O.bSize=bSize;
	O.bAddressReference=bAddressReference;
	O.bDataReference=bDataReference;
	O.bRegisterCategory=bRegisterCategory;

	return O;
}

//создать операнд - переменная по константному адресу
OPERAND_INFORMATION memim(BYTE bSize,BYTE bAddressReference,BYTE bDataReference)
{
	//результирующий операнд
	OPERAND_INFORMATION O;

	ZeroMemory(&O,sizeof(O));
	O.bSource=OIS_MEMORY_IM;
	O.bSize=bSize;
	O.bAddressReference=bAddressReference;
	O.bDataReference=bDataReference;

	return O;
}

//создать операнд - постоянный регистр
OPERAND_INFORMATION regcnst(BYTE bDataReference,BYTE bRegisterCategory,BYTE bConstRegister)
{
	//результирующий операнд
	OPERAND_INFORMATION O;

	ZeroMemory(&O,sizeof(O));
	O.bSource=OIS_CONST_REGISTER;
	O.bDataReference=bDataReference;
	O.bRegisterCategory=bRegisterCategory;
	O.bConstRegister=bConstRegister;

	return O;
}

//создать операнд - постоянная константа
OPERAND_INFORMATION cnstdata(BYTE bData)
{
	//результирующий операнд
	OPERAND_INFORMATION O;

	ZeroMemory(&O,sizeof(O));
	O.bSource=OIS_CONST_DATA;
	O.bConstData=bData;

	return O;
}

// функции-операнды

//типы функций-операндов
typedef OPERAND_INFORMATION (*OPFUNCP)(BYTE,BYTE);						//указывается режим и подкод

//операнд, получаемый сдвигом кода на заданное число
template <class op,BYTE offset>
OPERAND_INFORMATION op__offset(BYTE mode,BYTE code)
{
	return op(mode,code+offset);
}

//операнд 1 для инструкций типа ADD rm,r/r,rm			00dw /r
OPERAND_INFORMATION op_rmr_dw1(BYTE mode,BYTE code)
{
	//code == dw
	if(code & 2)
		if(code & 1)
			// d=1, w=1
			return ro(0,mode ? RC_DWORD : RC_WORD);
		else
			// d=1, w=0
			return ro(0,RC_BYTE);
	else
		if(code & 1)
			// d=0, w=1
			return rm(mode ? 4 : 2,OIR_REFERENCE_WRITE,0,mode ? RC_DWORD : RC_WORD);
		else
			// d=0, w=0
			return rm(1,OIR_REFERENCE_WRITE,0,RC_BYTE);
}
//операнд 2 для инструкций того же типа
OPERAND_INFORMATION op_rmr_dw2(BYTE mode,BYTE code)
{
	// d=~d - аналогично предыдущему, но поменять порядок
	code^=2;
	//code == dw
	if(code & 2)
		if(code & 1)
			// d=1, w=1
			return ro(0,mode ? RC_DWORD : RC_WORD);
		else
			// d=1, w=0
			return ro(0,RC_BYTE);
	else
		if(code & 1)
			// d=0, w=1
			return rm(mode ? 4 : 2,OIR_REFERENCE_READ,0,mode ? RC_DWORD : RC_WORD);
		else
			// d=0, w=0
			return rm(1,OIR_REFERENCE_READ,0,RC_BYTE);
}

//////////////////// модификация предыдущего 1-го операнда без записи (для инструкции CMP)
OPERAND_INFORMATION op_rmr_dw1_nowrite(BYTE mode,BYTE code)
{
	//code == dw
	if(code & 2)
		if(code & 1)
			// d=1, w=1
			return ro(0,mode ? RC_DWORD : RC_WORD);
		else
			// d=1, w=0
			return ro(0,RC_BYTE);
	else
		if(code & 1)
			// d=0, w=1
			return rm(mode ? 4 : 2,OIR_REFERENCE_READ,0,mode ? RC_DWORD : RC_WORD);
		else
			// d=0, w=0
			return rm(1,OIR_REFERENCE_READ,0,RC_BYTE);
}
/////////////////////// конец модификации

// операнд 1 для инструкции MOV r,[im] / [im,r]
OPERAND_INFORMATION op_rmim_dw1(BYTE mode,BYTE code)
{
	if(code & 2)
		if(code & 1)
			//dw=11
			return memim(mode ? 4 : 2,OIR_REFERENCE_WRITE,0);
		else
			//dw=10
			return memim(1,OIR_REFERENCE_WRITE,0);
	else
		if(code & 1)
			//dw=01
			return regcnst(0,mode ? RC_DWORD : RC_WORD,0);
		else
			//dw=00
			return regcnst(0,RC_BYTE,0);
}
// операнд 2 для инструкции MOV r,[im] / [im,r]
OPERAND_INFORMATION op_rmim_dw2(BYTE mode,BYTE code)
{
	//то же, что в предыдущем, но поменять порядок и ссылки
	//d=~d
	code^=2;
	if(code & 2)
		if(code & 1)
			//dw=11
			return memim(mode ? 4 : 2,OIR_REFERENCE_READ,0);
		else
			//dw=10
			return memim(1,OIR_REFERENCE_READ,0);
	else
		if(code & 1)
			//dw=01
			return regcnst(0,mode ? RC_DWORD : RC_WORD,0);
		else
			//dw=00
			return regcnst(0,RC_BYTE,0);
}

//операнд ro
OPERAND_INFORMATION op_ro(BYTE mode,BYTE code)
{
	return ro(0,mode ? RC_DWORD : RC_WORD);
}
//операнд ro, 2 варианта (w)
OPERAND_INFORMATION op_ro_w(BYTE mode,BYTE code)
{
	if(code==0)
		return ro(0,RC_BYTE);
	else
		return ro(0,mode ? RC_DWORD : RC_WORD);
}
//операнд ro с заданной категорией
template <int cat>
OPERAND_INFORMATION op_ro_cat(BYTE mode,BYTE code)
{
	return ro(0,cat);
}

//операнд rm для чтения
OPERAND_INFORMATION op_rm_read(BYTE mode,BYTE code)
{
	return rm(mode ? 4 : 2,OIR_REFERENCE_READ,0,mode ? RC_DWORD : RC_WORD);
}
//операнд rm для записи
OPERAND_INFORMATION op_rm_write(BYTE mode,BYTE code)
{
	return rm(mode ? 4 : 2,OIR_REFERENCE_WRITE,0,mode ? RC_DWORD : RC_WORD);
}
//операнд rm для ничего
OPERAND_INFORMATION op_rm_none(BYTE mode,BYTE code)
{
	return rm(mode ? 4 : 2,0,0,mode ? RC_DWORD : RC_WORD);
}

//операнд rm для чтения, 2 варианта (w)
OPERAND_INFORMATION op_rm_w_read(BYTE mode,BYTE code)
{
	if(code==0)
		return rm(1,OIR_REFERENCE_READ,0,RC_BYTE);
	else
		return rm(mode ? 4 : 2,OIR_REFERENCE_READ,0,mode ? RC_DWORD : RC_WORD);
}
//операнд rm для записи, 2 варианта (w)
OPERAND_INFORMATION op_rm_w_write(BYTE mode,BYTE code)
{
	if(code==0)
		return rm(1,OIR_REFERENCE_WRITE,0,RC_BYTE);
	else
		return rm(mode ? 4 : 2,OIR_REFERENCE_WRITE,0,mode ? RC_DWORD : RC_WORD);
}
//операнд rm для чтения и записи, 2 варианта (w)
OPERAND_INFORMATION op_rm_w_readwrite(BYTE mode,BYTE code)
{
	if(code==0)
		return rm(1,OIR_REFERENCE_READWRITE,0,RC_BYTE);
	else
		return rm(mode ? 4 : 2,OIR_REFERENCE_READWRITE,0,mode ? RC_DWORD : RC_WORD);
}
//операнд rm для чтения, содержит абсолютный адрес для выполнения
template <bool call>
OPERAND_INFORMATION op_rm_abspointer(BYTE mode,BYTE code)
{
	return rm(mode ? 4 : 2,OIR_REFERENCE_READ,OIR_REFERENCE_EXECUTE | OIR_ABSOLUTE_POINTER | (call ? OIR_REFERENCE_EXECUTE_CALL : 0),mode ? RC_DWORD : RC_WORD);
}

//операнд rm для чтения, строго задан размер и категория регистров
template <int size,int regcat>
OPERAND_INFORMATION op_rm_read_sz(BYTE mode,BYTE code)
{
	return rm(size,OIR_REFERENCE_READ,0,regcat);
}
//операнд rm для записи, строго задан размер и категория регистров
template <int size,int regcat>
OPERAND_INFORMATION op_rm_write_sz(BYTE mode,BYTE code)
{
	return rm(size,OIR_REFERENCE_WRITE,0,regcat);
}

//операнд ac (аккумулятор)
OPERAND_INFORMATION op_ac(BYTE mode,BYTE code)
{
	return regcnst(0,mode ? RC_DWORD : RC_WORD,0);
}
OPERAND_INFORMATION op_ac_w(BYTE mode,BYTE code)
{
	if(code==0)
		return regcnst(0,RC_BYTE,0);
	else
		return regcnst(0,mode ? RC_DWORD : RC_WORD,0);
}
//операнд - постоянный регистр
template <BYTE regcat,BYTE reg>
OPERAND_INFORMATION op_regcnst(BYTE mode,BYTE code)
{
	return regcnst(0,regcat,reg);
}

//операнд im с указанным размером
template <BYTE size1632_flag>
OPERAND_INFORMATION op_im(BYTE mode,BYTE code)
{
	if(size1632_flag==1)
		return cnst(FALSE,mode ? 4 : 2,0);
	else
		return cnst(FALSE,1,0);
}
//операнд iw (строго задан размер)
OPERAND_INFORMATION op_iw(BYTE mode,BYTE code)
{
	return cnst(FALSE,2,0);
}
//операнд im с различными размерами
OPERAND_INFORMATION op_im_w(BYTE mode,BYTE code)
{
	if(code==0)
		return cnst(FALSE,1,0);
	else
		return cnst(FALSE,mode ? 4 : 2,0);
}
//операнд im с возможным знаком
OPERAND_INFORMATION op_im_sw(BYTE mode,BYTE code)
{
	// s==1
	if(code & 2)
		// w==1
		if(code & 1)
			return cnst(TRUE,1,0);
		// w==0
		else
			return cnst(FALSE,1,0);
	// s==0
	else
		// w==1
		if(code & 1)
			return cnst(FALSE,mode ? 4 : 2,0);
		// w==0
		else
			return cnst(FALSE,1,0);
}
//операнд ib - относительный указатель
OPERAND_INFORMATION op_ib_relpointer(BYTE mode,BYTE code)
{
	return cnst(TRUE,1,OIR_REFERENCE_EXECUTE | OIR_RELATIVE_POINTER);
}
//операнд im - относительный указатель
template <bool call>
OPERAND_INFORMATION op_im_relpointer(BYTE mode,BYTE code)
{
	return cnst(TRUE,mode ? 4 : 2,OIR_REFERENCE_EXECUTE | OIR_RELATIVE_POINTER | (call ? OIR_REFERENCE_EXECUTE_CALL : 0));
}
//операнд im - абсолютный указатель
template <bool call>
OPERAND_INFORMATION op_im_abspointer(BYTE mode,BYTE code)
{
	return cnst(TRUE,4,OIR_REFERENCE_EXECUTE | OIR_ABSOLUTE_POINTER | (call ? OIR_REFERENCE_EXECUTE_CALL : 0));
}

//операнд - постоянный сегментный регистр
template <BYTE reg>
OPERAND_INFORMATION op_regseg_cnst(BYTE mode,BYTE code)
{
	return regcnst(0,RC_SEGMENT,reg);
}
//операнд - сегментный регистр из ro
OPERAND_INFORMATION op_regseg(BYTE mode,BYTE code)
{
	return ro(0,RC_SEGMENT);
}

//операнд - регистр из регистрового байта для команд типа INC reg : 4reg
template <bool large>
OPERAND_INFORMATION op_reg8(BYTE mode,BYTE code)
{
	return regcnst(0,large ? (mode ? RC_DWORD : RC_WORD) : RC_BYTE,code);
}
//операнд - регистр из регистрового байта для команды XCHG eax,e** (со сдвигом кода на 1)
template <bool large>
OPERAND_INFORMATION op_reg8_for_xchg(BYTE mode,BYTE code)
{
	code++;
	return regcnst(0,large ? (mode ? RC_DWORD : RC_WORD) : RC_BYTE,code);
}

//операнд r/rm для инструкций типа ADD r/rm, im : 80sw /0 im
OPERAND_INFORMATION op_rm_sw(BYTE mode,BYTE code)
{
	//такой операнд зависит только от w
	if((code & 1)==0)
		return rm(1,OIR_REFERENCE_WRITE,0,RC_BYTE);
	else
		return rm(mode ? 4 : 2,OIR_REFERENCE_WRITE,0,mode ? RC_DWORD : RC_WORD);
}
//то же, но без записи
OPERAND_INFORMATION op_rm_sw_nowrite(BYTE mode,BYTE code)
{
	//такой операнд зависит только от w
	if((code & 1)==0)
		return rm(1,OIR_REFERENCE_READ,0,RC_BYTE);
	else
		return rm(mode ? 4 : 2,OIR_REFERENCE_READ,0,mode ? RC_DWORD : RC_WORD);
}

//операнд 1 (единица) для команд типа ROL r/m,1
OPERAND_INFORMATION op_cnst1(BYTE mode,BYTE code)
{
	return cnstdata(1);
}

// макросы для упрощения

//сформировать инструкцию
#define DEF_II(CodeSize) \
	INSTRUCTION_INFORMATION II; \
	ZeroMemory(&II,sizeof(II)); \
	II.bCodeSize=CodeSize; \
	_tcscpy(II.szName,szName)
#define DEF_II_NN(CodeSize) \
	INSTRUCTION_INFORMATION II; \
	ZeroMemory(&II,sizeof(II)); \
	II.bCodeSize=CodeSize
#define DEF_IIO(CodeSize,OperandsCount) \
	DEF_II(CodeSize); \
	II.bOperandsCount=OperandsCount
#define DEF_IIO_NN(CodeSize,OperandsCount) \
	DEF_II_NN(CodeSize); \
	II.bOperandsCount=OperandsCount
//загрузить инструкцию
#define LL_16(Code) gpSet16->LoadInstruction_OneByte(II,Code)
#define LL_32(Code) gpSet32->LoadInstruction_OneByte(II,Code)
#define LL_D16(Code1,Code2,Mask) gpSet16->LoadInstruction_TwoBytes(II,Code1,Code2,Mask)
#define LL_D32(Code1,Code2,Mask) gpSet32->LoadInstruction_TwoBytes(II,Code1,Code2,Mask)

// загружающие функции
// i_o <количество операндов>
// i2_o <количество операндов>
// i_o <количество операндов> _r <количество команд>

//однобайтовая команда без операндов
VOID i_o0(LPCTSTR szName,BYTE Code)
{
	DEF_II(1);
	s16->LoadInstruction_OneByte(II,Code);
	s32->LoadInstruction_OneByte(II,Code);
}

//двухбайтовая команда без операндов
VOID i2_o0(LPCTSTR szName,BYTE Code1,BYTE Code2)
{
	DEF_II(2);
	s16->LoadInstruction_TwoBytes(II,Code1,Code2,0xFF);
	s32->LoadInstruction_TwoBytes(II,Code1,Code2,0xFF);
}

//однобайтовая команда без операндов, но с меняющимcя именем
VOID i_o0_n2(LPCTSTR szName16,LPCTSTR szName32,BYTE Code)
{
	DEF_II_NN(1);

	_tcscpy(II.szName,szName16);
	s16->LoadInstruction_OneByte(II,Code);
	_tcscpy(II.szName,szName32);
	s32->LoadInstruction_OneByte(II,Code);
}

//несколько однобайтовых команд с одним операндом
template <BYTE count>
VOID i_o1(LPCTSTR szName,BYTE Code,OPFUNCP op)
{
	DEF_IIO(1,1);

	for(BYTE i=0;i<count;++i)
	{
		II.Operands[0]=op(0,i);
		s16->LoadInstruction_OneByte(II,Code + i);

		II.Operands[0]=op(1,i);
		s32->LoadInstruction_OneByte(II,Code + i);
	}
}

//несколько однобайтовых команд с двумя операндами
template <BYTE count>
VOID i_o2(LPCTSTR szName,BYTE Code,OPFUNCP op1,OPFUNCP op2)
{
	DEF_IIO(1,2);
	for(BYTE i=0;i<count;++i)
	{
		II.Operands[0]=op1(0,i);
		II.Operands[1]=op2(0,i);
		s16->LoadInstruction_OneByte(II,Code + i);

		II.Operands[0]=op1(1,i);
		II.Operands[1]=op2(1,i);
		s32->LoadInstruction_OneByte(II,Code + i);
	}
}

//несколько однобайтовых команд с тремя операндами
template <BYTE count>
VOID i_o3(LPCTSTR szName,BYTE Code,OPFUNCP op1,OPFUNCP op2,OPFUNCP op3)
{
	DEF_IIO(1,3);
	for(BYTE i=0;i<count;++i)
	{
		II.Operands[0]=op1(0,i);
		II.Operands[1]=op2(0,i);
		II.Operands[2]=op3(0,i);
		s16->LoadInstruction_OneByte(II,Code + i);

		II.Operands[0]=op1(1,i);
		II.Operands[1]=op2(1,i);
		II.Operands[2]=op3(1,i);
		s32->LoadInstruction_OneByte(II,Code + i);
	}
}

//16 однобайтных команд с изменением имени (суффикс cond) типа J*
VOID i_o1_cond(LPCTSTR szName,BYTE Code,OPFUNCP op)
{
	DEF_IIO_NN(1,1);

	for(BYTE i=0;i<16;++i)
	{
		_stprintf(II.szName,TEXT("%s%s"),szName,gcszConditionNames[i]);

		II.Operands[0]=op(0,i);
		II.bFlags=IIF_SPACE;
		s16->LoadInstruction_OneByte(II,Code + i);
		II.Operands[0]=op(1,i);
		s32->LoadInstruction_OneByte(II,Code + i);
	}
}
//16 двухбайтных команд с изменением имени (суффикс cond) типа J*
VOID i2_o1_cond(LPCTSTR szName,BYTE Code1,BYTE Code2,OPFUNCP op)
{
	DEF_IIO_NN(2,1);

	for(BYTE i=0;i<16;++i)
	{
		_stprintf(II.szName,TEXT("%s%s"),szName,gcszConditionNames[i]);

		II.Operands[0]=op(0,0);
		II.bFlags=IIF_SPACE;
		s16->LoadInstruction_TwoBytes(II,Code1,Code2 + i,0xFF);
		II.Operands[0]=op(1,0);
		s32->LoadInstruction_TwoBytes(II,Code1,Code2 + i,0xFF);
	}
}

//двухбайтная команда с одним операндом
template <BYTE count>
VOID i2_o1(LPCTSTR szName,BYTE Code1,BYTE Code2,OPFUNCP op)
{
	DEF_IIO(2,1);

	for(BYTE i=0;i<count;++i)
	{
		II.Operands[0]=op(0,i);
		s16->LoadInstruction_TwoBytes(II,Code1,Code2 + i,0xFF);
		II.Operands[0]=op(1,i);
		s32->LoadInstruction_TwoBytes(II,Code1,Code2 + i,0xFF);
	}
}
//двухбайтная команда с двумя операндами
template <BYTE count>
VOID i2_o2(LPCTSTR szName,BYTE Code1,BYTE Code2,OPFUNCP op1,OPFUNCP op2)
{
	DEF_IIO(2,2);

	for(BYTE i=0;i<count;++i)
	{
		II.Operands[0]=op1(0,i);
		II.Operands[1]=op2(0,i);
		s16->LoadInstruction_TwoBytes(II,Code1,Code2 + i,0xFF);
		II.Operands[0]=op1(1,i);
		II.Operands[1]=op2(1,i);
		s32->LoadInstruction_TwoBytes(II,Code1,Code2 + i,0xFF);
	}
}
//двухбайтовая команда с тремя операндами
template <BYTE count>
VOID i2_o3(LPCTSTR szName,BYTE Code1,BYTE Code2,OPFUNCP op1,OPFUNCP op2,OPFUNCP op3)
{
	DEF_IIO(2,3);
	for(BYTE i=0;i<count;++i)
	{
		II.Operands[0]=op1(0,i);
		II.Operands[1]=op2(0,i);
		II.Operands[2]=op3(0,i);
		s16->LoadInstruction_TwoBytes(II,Code1,Code2 + i,0xFF);

		II.Operands[0]=op1(1,i);
		II.Operands[1]=op2(1,i);
		II.Operands[2]=op3(1,i);
		s32->LoadInstruction_TwoBytes(II,Code1,Code2 + i,0xFF);
	}
}
//несколько полуторабайтовых команд с дополнительным кодом в первом байте и одним операндом
template <BYTE count>
VOID i2a1_o1(LPCTSTR szName,BYTE Code,BYTE AdditionalCode,OPFUNCP op)
{
	DEF_IIO(1,1);
	for(BYTE i=0;i<count;++i)
	{
		II.Operands[0]=op(0,i);
		s16->LoadInstruction_TwoBytes(II,Code + i,AdditionalCode<<3,7<<3);

		II.Operands[0]=op(1,i);
		s32->LoadInstruction_TwoBytes(II,Code + i,AdditionalCode<<3,7<<3);
	}
}
//несколько полуторабайтовых команд с дополнительным кодом в первом байте и двумя операндами
template <BYTE count>
VOID i2a1_o2(LPCTSTR szName,BYTE Code,BYTE AdditionalCode,OPFUNCP op1,OPFUNCP op2)
{
	DEF_IIO(1,2);
	for(BYTE i=0;i<count;++i)
	{
		II.Operands[0]=op1(0,i);
		II.Operands[1]=op2(0,i);
		s16->LoadInstruction_TwoBytes(II,Code + i,AdditionalCode<<3,7<<3);

		II.Operands[0]=op1(1,i);
		II.Operands[1]=op2(1,i);
		s32->LoadInstruction_TwoBytes(II,Code + i,AdditionalCode<<3,7<<3);
	}
}
//двухсполовинойбайтовая команда с дополнительным кодом и одним операндом
VOID i3a1_o1(LPCTSTR szName,BYTE Code1,BYTE Code2,BYTE AdditionalCode,OPFUNCP op)
{
	DEF_IIO(2,1);
	II.Operands[0]=op(0,0);
	s16->LoadInstruction_ThreeBytes(II,Code1,Code2,AdditionalCode<<3,0xFF,7<<3);

	II.Operands[0]=op(1,0);
	s32->LoadInstruction_ThreeBytes(II,Code1,Code2,AdditionalCode<<3,0xFF,7<<3);
}

//******************* макросы для загрузки часто встречающихся инструкций
//префикс
#define prefix(code,flag) \
	s16->LoadPrefix(code,flag); \
	s32->LoadPrefix(code,flag)
// конец процедуры
#define endproc() \
	s16->SetInstructionFlag(IIF_ENDPROC); \
	s32->SetInstructionFlag(IIF_ENDPROC)
// разделитель
#define space() \
	s16->SetInstructionFlag(IIF_SPACE); \
	s32->SetInstructionFlag(IIF_SPACE)
// 4 инструкции типа MOV rm,r/r,rm : 88dw /r
#define instruction_4(name,code) \
	i_o2<4>(name,code,op_rmr_dw1,op_rmr_dw2)
// 10 инструкций типа ADD
#define instruction_10(name,code,code_sw,addcode_sw) \
	instruction_4(name,code); \
	i_o2<2>(name,code+4,op_ac_w,op_im_w); \
	i2a1_o2<4>(name,code_sw,addcode_sw,op_rm_sw,op_im_sw)
// модификация предыдущего для CMP ; op_rmr_dw2 - nowrite не надо, так как он и так только для чтения
#define instruction_10_nowrite(name,code,code_sw,addcode_sw) \
	i_o2<4>(name,code,op_rmr_dw1_nowrite,op_rmr_dw2); \
	i_o2<2>(name,code+4,op_ac_w,op_im_w); \
	i2a1_o2<4>(name,code_sw,addcode_sw,op_rm_sw_nowrite,op_im_sw)
// инструкция с одним операндом - сегментным регистром (типа PUSH es)
#define instruction_segreg(name,code,reg) \
	i_o1<1>(name,code,op_regseg_cnst<reg>)
// 2-байтовая инструкция с одним операндом - сегментным регистром (типа PUSH fs)
#define instruction2_segreg(name,code1,code2,reg) \
	i2_o1<1>(name,code1,code2,op_regseg_cnst<reg>)
// 8 инструкций с одним регистром в регистровом байте (типа INC reg)
#define instruction_reg8(name,code) \
	i_o1<8>(name,code,op_reg8<true>)
// инструкция с операндами R, RM
#define instruction_r_rm(name,code) \
	i_o2<1>(name,code,op_ro,op_rm_read)
// инструкция с операндами RM, R
#define instruction_rm_r(name,code) \
	i_o2<1>(name,code,op_rm_write,op_ro)
// 2 инструкции с операндами R, RM
#define instruction_r_rm_w(name,code) \
	i_o2<2>(name,code,op_ro_w,op_rm_w_read)
// 2 инструкции с операндами R, RM для чтения-записи
#define instruction_r_rm_w_readwrite(name,code) \
	i_o2<2>(name,code,op_ro_w,op_rm_w_readwrite)
// инструкция с операндом i8
#define instruction_ib(name,code) \
	i_o1<1>(name,code,op_im<0>)
// инструкция с операндом i16/32
#define instruction_im(name,code) \
	i_o1<1>(name,code,op_im<1>)
// инструкции типа ROL
#define instruction_rolror(name,addcode) \
	i2a1_o2<2>(name,0xD0,addcode,op_rm_w_write,op_cnst1); \
	i2a1_o2<2>(name,0xD2,addcode,op_rm_w_write,op_regcnst<RC_BYTE,1>); \
	i2a1_o2<2>(name,0xC0,addcode,op_rm_w_write,op_im<0>)

//загрузка инструкций
VOID LoadPrivilegedInstructions();
VOID LoadFPUInstructions();
VOID CApplication::LoadInstructions()
{
	//указать указатели на наборы инструкций
	s16=&mInstructionSet16;
	s32=&mInstructionSet32;

	//загрузить префиксы
	prefix(PREFIX_CS,PF_CS);
	prefix(PREFIX_SS,PF_SS);
	prefix(PREFIX_DS,PF_DS);
	prefix(PREFIX_ES,PF_ES);
	prefix(PREFIX_FS,PF_FS);
	prefix(PREFIX_GS,PF_GS);
	prefix(PREFIX_OS,PF_OS);
	prefix(PREFIX_AS,PF_AS);
	prefix(PREFIX_LOCK,PF_LOCK);
	prefix(PREFIX_REPNZ,PF_REPNZ);
	prefix(PREFIX_REP,PF_REP);

	//****************************** область 00 - 3F

	// ADD
	instruction_10(TEXT("add"),0x00,0x80,0);
	// OR
	instruction_10(TEXT("or"),0x08,0x80,1);
	// ADC
	instruction_10(TEXT("adc"),0x10,0x80,2);
	// SBB
	instruction_10(TEXT("sbb"),0x18,0x80,3);
	// AND
	instruction_10(TEXT("and"),0x20,0x80,4);
	// SUB
	instruction_10(TEXT("sub"),0x28,0x80,5);
	// XOR
	instruction_10(TEXT("xor"),0x30,0x80,6);
	// CMP
	instruction_10_nowrite(TEXT("cmp"),0x38,0x80,7);

	// PUSH
	instruction_segreg(TEXT("push"),0x06,REGISTER_ES);
	instruction_segreg(TEXT("push"),0x0E,REGISTER_CS);
	instruction_segreg(TEXT("push"),0x16,REGISTER_SS);
	instruction_segreg(TEXT("push"),0x1E,REGISTER_DS);
	instruction2_segreg(TEXT("push"),0x0F,0xA0,REGISTER_FS);
	instruction2_segreg(TEXT("push"),0x0F,0xA8,REGISTER_GS);
	// POP
	instruction_segreg(TEXT("pop"),0x07,REGISTER_ES);
	instruction_segreg(TEXT("pop"),0x17,REGISTER_SS);
	instruction_segreg(TEXT("pop"),0x1F,REGISTER_DS);
	instruction2_segreg(TEXT("pop"),0x0F,0xA1,REGISTER_FS);
	instruction2_segreg(TEXT("pop"),0x0F,0xA9,REGISTER_GS);

	// DAA
	i_o0(TEXT("daa"),0x27);
	// DAS
	i_o0(TEXT("das"),0x2F);
	// AAA
	i_o0(TEXT("aaa"),0x37);
	// AAS
	i_o0(TEXT("aas"),0x3F);

	//**********************************************

	//****************************** область 40 - 7F

	// INC
	instruction_reg8(TEXT("inc"),0x40);
	// DEC
	instruction_reg8(TEXT("dec"),0x48);
	// PUSH reg
	instruction_reg8(TEXT("push"),0x50);
	// POP reg
	instruction_reg8(TEXT("pop"),0x58);

	// PUSHA/PUSHAD
	i_o0_n2(TEXT("pusha"),TEXT("pushad"),0x60);
	// POPA/POPAD
	i_o0_n2(TEXT("popa"),TEXT("popad"),0x61);

	// BOUND
	instruction_r_rm(TEXT("bound"),0x62);

	// PUSH im
	instruction_im(TEXT("push"),0x68);
	// IMUL r,r,im
	i_o3<1>(TEXT("imul"),0x69,op_ro,op_rm_read,op_im<1>);
	// PUSH ib
	instruction_ib(TEXT("push"),0x6A);
	// IMUL r,r,ib
	i_o3<1>(TEXT("imul"),0x6B,op_ro,op_rm_read,op_im<0>);

	// INSB
	i_o0(TEXT("insb"),0x6C);
	// INSW/INSD
	i_o0_n2(TEXT("insw"),TEXT("insd"),0x6D);
	// OUTSB
	i_o0(TEXT("outsb"),0x6E);
	// OUTSW/OUTSD
	i_o0_n2(TEXT("outsw"),TEXT("outsd"),0x6F);

	// Jcond
	i_o1_cond(TEXT("j"),0x70,op_ib_relpointer);

	//**********************************************

	//****************************** область 80 - BF

	// TEST
	instruction_r_rm_w(TEXT("test"),0x84);
	// XCHG
	instruction_r_rm_w_readwrite(TEXT("xchg"),0x86);

	// MOV
	instruction_4(TEXT("mov"),0x88);
	// MOV rm,sr
	i_o2<1>(TEXT("mov"),0x8C,op_rm_write,op_regseg);
	// LEA r,rm
	i_o2<1>(TEXT("lea"),0x8D,op_ro,op_rm_none);
	// MOV sr,rm
	i_o2<1>(TEXT("mov"),0x8E,op_regseg,op_rm_read);

	// NOP
	i_o0(TEXT("nop"),0x90);
	// XCHG
	i_o2<7>(TEXT("xchg"),0x91,op_ac,op_reg8_for_xchg<true>);

	// CBW
	i_o0(TEXT("cbw"),0x98);
	// CWD/CDQ
	i_o0_n2(TEXT("cwd"),TEXT("cdq"),0x99);

	// CALL far im
	i_o1<1>(TEXT("call far"),0x9A,op_im_abspointer<true>);
	space();

	// PUSHF/PUSHFD
	i_o0_n2(TEXT("pushf"),TEXT("pushfd"),0x9C);
	// POPF/POPFD
	i_o0_n2(TEXT("popf"),TEXT("popfd"),0x9D);

	// SAHF
	i_o0(TEXT("sahf"),0x9E);
	// LAHF
	i_o0(TEXT("lahf"),0x9F);

	// MOV r
	i_o2<4>(TEXT("mov"),0xA0,op_rmim_dw1,op_rmim_dw2);
	// MOVSB
	i_o0(TEXT("movsb"),0xA4);
	// MOVSW/MOVSD
	i_o0_n2(TEXT("movsw"),TEXT("movsd"),0xA5);
	// CMPSB
	i_o0(TEXT("cmpsb"),0xA6);
	// CMPSW/CMPSD
	i_o0_n2(TEXT("cmpsw"),TEXT("cmpsd"),0xA7);

	// TEST
	i_o2<2>(TEXT("test"),0xA8,op_ac_w,op_im_w);

	// STOSB
	i_o0(TEXT("stosb"),0xAA);
	// STOSW/STOSD
	i_o0_n2(TEXT("stosw"),TEXT("stosd"),0xAB);
	// LODSB
	i_o0(TEXT("lodsb"),0xAC);
	// LODSW/LODSD
	i_o0_n2(TEXT("lodsw"),TEXT("lodsd"),0xAD);
	// SCASB
	i_o0(TEXT("scasb"),0xAE);
	// SCASW/SCASD
	i_o0_n2(TEXT("scasw"),TEXT("scasd"),0xAF);

	// MOV r8,i8
	i_o2<8>(TEXT("mov"),0xB0,op_reg8<false>,op_im<0>);
	// MOV r16/32,i16/32
	i_o2<8>(TEXT("mov"),0xB8,op_reg8<true>,op_im<1>);

	//**********************************************

	//****************************** область 80 - BF

	// RETN iw
	i_o1<1>(TEXT("retn"),0xC2,op_iw);
	endproc();
	// RETN
	i_o0(TEXT("retn"),0xC3);
	endproc();

	// LES
	i_o2<1>(TEXT("les"),0xC4,op_ro,op_rm_read);
	// LDS
	i_o2<1>(TEXT("lds"),0xC5,op_ro,op_rm_read);

	// ENTER
	i_o2<1>(TEXT("enter"),0xC8,op_iw,op_im<0>);
	// LEAVE
	i_o0(TEXT("leave"),0xC9);

	// RETF iw
	i_o1<1>(TEXT("retf"),0xCA,op_iw);
	endproc();
	// RETF
	i_o0(TEXT("retf"),0xCB);
	endproc();

	// INT 3
	i_o0(TEXT("int3"),0xCC);
	// INT ib
	i_o1<1>(TEXT("int"),0xCD,op_im<0>);
	// INTO
	i_o0(TEXT("into"),0xCE);
	// IRET
	i_o0(TEXT("iret"),0xCF);
	endproc();

	// AAM
	i_o1<1>(TEXT("aam"),0xD4,op_im<0>);
	// AAD
	i_o1<1>(TEXT("aad"),0xD5,op_im<0>);

	// SALC
	i_o0(TEXT("salc"),0xD6);
	// XLAT
	i_o0(TEXT("xlat"),0xD7);

	// LOOPNE
	i_o1<1>(TEXT("loopne"),0xE0,op_ib_relpointer);
	// LOOPE
	i_o1<1>(TEXT("loope"),0xE1,op_ib_relpointer);
	// LOOP
	i_o1<1>(TEXT("loop"),0xE2,op_ib_relpointer);

	// JCXZ
	i_o1<1>(TEXT("jcxz"),0xE3,op_ib_relpointer);

	// IN
	i_o2<2>(TEXT("in"),0xE4,op_ac_w,op_im<0>);
	// OUT
	i_o2<2>(TEXT("out"),0xE6,op_im<0>,op_ac_w);

	// CALL near im
	i_o1<1>(TEXT("call near"),0xE8,op_im_relpointer<true>);
	space();
	// JMP near im
	i_o1<1>(TEXT("jmp near"),0xE9,op_im_relpointer<false>);
	endproc();
	// JMP far im
	i_o1<1>(TEXT("jmp far"),0xEA,op_im_abspointer<false>);
	endproc();
	// JMP near ib
	i_o1<1>(TEXT("jmp near"),0xEB,op_ib_relpointer);
	endproc();

	// IN ac,DX
	i_o2<2>(TEXT("in"),0xEC,op_ac_w,op_regcnst<RC_WORD,2>);
	// OUT DX,ac
	i_o2<2>(TEXT("out"),0xEE,op_regcnst<RC_WORD,2>,op_ac_w);

	// CMC
	i_o0(TEXT("cmc"),0xF5);

	// CLC
	i_o0(TEXT("clc"),0xF8);
	// STC
	i_o0(TEXT("stc"),0xF9);
	// CLI
	i_o0(TEXT("cli"),0xFA);
	// STI
	i_o0(TEXT("sti"),0xFB);
	// CLD
	i_o0(TEXT("cld"),0xFC);
	// STD
	i_o0(TEXT("std"),0xFD);

	//**********************************************

	//********************* разные команды в алфавитном порядке

	// BSF
	i2_o2<1>(TEXT("bsf"),0x0F,0xBC,op_ro,op_rm_read);
	// BSR
	i2_o2<1>(TEXT("bsr"),0x0F,0xBD,op_ro,op_rm_read);
	// BSWAP
	i2_o1<8>(TEXT("bswap"),0x0F,0xC8,op_reg8<true>);

	// BT
	i2_o2<1>(TEXT("bt"),0x0F,0xA3,op_rm_write,op_ro);
	// и еще несколько BT

	// BTC
	i2_o2<1>(TEXT("btc"),0x0F,0xBB,op_rm_write,op_ro);

	// BTR
	i2_o2<1>(TEXT("btr"),0x0F,0xB3,op_rm_write,op_ro);

	// BTS
	i2_o2<1>(TEXT("bts"),0x0F,0xAB,op_rm_write,op_ro);

	// DIV
	i2a1_o1<2>(TEXT("div"),0xF6,6,op_rm_w_read);

	// IDIV
	i2a1_o1<2>(TEXT("idiv"),0xF6,7,op_rm_w_read);
	// IMUL
	i2a1_o1<2>(TEXT("imul"),0xF6,5,op_rm_w_read);
	i2_o2<1>(TEXT("imul"),0x0F,0xAF,op_ro,op_rm_read);

	// Jcond
	i2_o1_cond(TEXT("j"),0x0F,0x80,op_im_relpointer<false>);

	// MOV
	i2a1_o2<2>(TEXT("mov"),0xC6,0,op_rm_w_write,op_im_w);

	// MOVSX
	i2_o2<2>(TEXT("movsx"),0x0F,0xBE,op_ro_w,op_rm_w_read);
	// MOVZX
	i2_o2<2>(TEXT("movzx"),0x0F,0xB6,op_ro_w,op_rm_w_read);

	// MUL
	i2a1_o1<2>(TEXT("mul"),0xF6,4,op_rm_w_read);

	// NEG
	i2a1_o1<2>(TEXT("neg"),0xF6,3,op_rm_w_write);

	// NOT
	i2a1_o1<2>(TEXT("not"),0xF6,2,op_rm_w_write);

	// ROL
	instruction_rolror(TEXT("rol"),0);
	// ROR
	instruction_rolror(TEXT("ror"),1);
	// RCL
	instruction_rolror(TEXT("rcl"),2);
	// RCR
	instruction_rolror(TEXT("rcr"),3);
	// SAL
	instruction_rolror(TEXT("sal"),4);
	// SHR
	instruction_rolror(TEXT("shr"),5);
	// с кодом 6 - нету?
	// SAR
	instruction_rolror(TEXT("sar"),7);

	// SET*
	i2_o1_cond(TEXT("set"),0x0F,0x90,op_rm_w_write);		// используется op_rm_w_write, который при code=0 даёт как раз нужную байтовую переменную

	// SYSENTER
	i2_o0(TEXT("sysenter"),0x0F,0x34);
	endproc();
	// SYSEXIT
	i2_o0(TEXT("sysexit"),0x0F,0x35);
	endproc();

	// TEST
	i2a1_o2<2>(TEXT("test"),0xF6,0,op_rm_w_read,op_im_w);

	// UD2
	i2_o0(TEXT("ud2"),0x0F,0x0B);

	// XADD
	i2_o2<2>(TEXT("xadd"),0x0F,0xC0,op_rm_w_write,op_ro_w);

	// ************* кодируются через 8F

	// POP m
	i2a1_o1<1>(TEXT("pop"),0x8F,0,op_rm_write);

	// ************* кодируются через 0F

	// CPUID
	i2_o0(TEXT("cpuid"),0x0F,0xA2);

	// SHLD rm,r,im
	i2_o3<1>(TEXT("shld"),0x0F,0xA4,op_rm_write,op_ro,op_im<0>);
	// SHLD rm,r,CL
	i2_o3<1>(TEXT("shld"),0x0F,0xA5,op_rm_write,op_ro,op_regcnst<RC_BYTE,1>);

	// SHRD rm,r,im
	i2_o3<1>(TEXT("shrd"),0x0F,0xAC,op_rm_write,op_ro,op_im<0>);
	// SHRD rm,r,CL
	i2_o3<1>(TEXT("shrd"),0x0F,0xAD,op_rm_write,op_ro,op_regcnst<RC_BYTE,1>);

	// CMPXCHG rm,r
	i2_o2<2>(TEXT("cmpxchg"),0x0F,0xB0,op_rm_w_write,op_ro_w);

	// ************* кодируются через FF

	// INC
	i2a1_o1<2>(TEXT("inc"),0xFE,0,op_rm_w_write);
	// DEC
	i2a1_o1<2>(TEXT("dec"),0xFE,1,op_rm_w_write);

	// CALL near
	i2a1_o1<1>(TEXT("call near"),0xFF,2,op_rm_abspointer<true>);
	space();
	// CALL far
	i2a1_o1<1>(TEXT("call far"),0xFF,3,op_rm_abspointer<true>);
	space();

	// JMP near
	i2a1_o1<1>(TEXT("jmp near"),0xFF,4,op_rm_abspointer<false>);
	endproc();
	// JMP far
	i2a1_o1<1>(TEXT("jmp far"),0xFF,5,op_rm_abspointer<false>);
	endproc();

	// PUSH
	i2a1_o1<1>(TEXT("push"),0xFF,6,op_rm_read);

	//загрузить привилегированные инструкции
	LoadPrivilegedInstructions();

	// загрузить FPU
	LoadFPUInstructions();

	// вывести инструкции в HTML
/*	s16->PrintHTML(TEXT("set16.htm"),0xF6);
	s32->PrintHTML(TEXT("set32.htm"),0xF6);*/
}

//*******************************************************************
//*                      FPU                                        *
//*******************************************************************

//загрузить 3 однобайтовые инструкции типа FADD с 5-битовой маской
template <int size>
VOID fpui1_oper(LPCTSTR szName,BYTE Code,BYTE AdditionalCode,OPFUNCP op)
{
	DEF_IIO(1,1);
	for(BYTE i=0;i<size;++i)
	{
		II.Operands[0]=op(0,i);
		s16->LoadInstruction_TwoBytes(II,Code,(AdditionalCode<<3)+(i<<6),31<<3);

		II.Operands[0]=op(1,i);
		s32->LoadInstruction_TwoBytes(II,Code,(AdditionalCode<<3)+(i<<6),31<<3);
	}
}
//загрузить 3 двухбайтовые инструкции типа FADD с 5-битовой маской
template <int size>
VOID fpui2_oper(LPCTSTR szName,BYTE Code,BYTE AdditionalCode,OPFUNCP op1,OPFUNCP op2)
{
	DEF_IIO(1,2);
	for(BYTE i=0;i<size;++i)
	{
		II.Operands[0]=op1(0,i);
		II.Operands[1]=op2(0,i);
		s16->LoadInstruction_TwoBytes(II,Code,(AdditionalCode<<3)+(i<<6),31<<3);

		II.Operands[0]=op1(1,i);
		II.Operands[1]=op2(1,i);
		s32->LoadInstruction_TwoBytes(II,Code,(AdditionalCode<<3)+(i<<6),31<<3);
	}
}

//загрузить привилегированные инструкции
VOID LoadPrivilegedInstructions()
{
	// LGDT
	i3a1_o1(TEXT("lgdt"),0x0F,0x01,2,op_rm_read_sz<6,RC_DWORD>);
	// SGDT
	i3a1_o1(TEXT("sgdt"),0x0F,0x01,0,op_rm_write_sz<6,RC_DWORD>);

	// LLDT
	i3a1_o1(TEXT("lldt"),0x0F,0x00,2,op_rm_read_sz<2,RC_WORD>);
	// SLDT
	i3a1_o1(TEXT("sldt"),0x0F,0x00,0,op_rm_write_sz<2,RC_WORD>);

	// LTR
	i3a1_o1(TEXT("ltr"),0x0F,0x00,3,op_rm_w_read);
	// STR
	i3a1_o1(TEXT("str"),0x0F,0x00,1,op_rm_w_write);

	// LIDT
	i3a1_o1(TEXT("lidt"),0x0F,0x01,3,op_rm_read_sz<6,RC_DWORD>);
	// SIDT
	i3a1_o1(TEXT("sidt"),0x0F,0x01,1,op_rm_write_sz<6,RC_DWORD>);

	// MOV CRx, r
	i2_o2<1>(TEXT("mov"),0x0F,0x22,op_ro_cat<RC_CR>,op_rm_read_sz<4,RC_DWORD>);
	// MOV r, CRx
	i2_o2<1>(TEXT("mov"),0x0F,0x20,op_rm_write_sz<4,RC_DWORD>,op_ro_cat<RC_CR>);

	// LMSW
	i3a1_o1(TEXT("lmsw"),0x0F,0x01,6,op_rm_read_sz<2,RC_WORD>);
	// SMSW
	i3a1_o1(TEXT("smsw"),0x0F,0x01,4,op_rm_write_sz<2,RC_WORD>);

	// CLTS
	i2_o0(TEXT("clts"),0x0F,0x06);

	// ARPL
	i_o2<1>(TEXT("arpl"),0x63,op_rm_write_sz<2,RC_WORD>,op_ro_cat<RC_WORD>);

	// LAR
	i2_o2<1>(TEXT("lar"),0x0F,0x02,op_ro,op_rm_read);

	// LSL
	i2_o2<1>(TEXT("lsl"),0x0F,0x03,op_ro,op_rm_read);

	// VERR
	i3a1_o1(TEXT("verr"),0x0F,0x00,4,op_rm_read_sz<2,RC_WORD>);
	// VERW
	i3a1_o1(TEXT("verw"),0x0F,0x00,5,op_rm_read_sz<2,RC_WORD>);

	// INVD
	i2_o0(TEXT("invd"),0x0F,0x08);
	// WBINVD
	i2_o0(TEXT("wbinvd"),0x0F,0x09);

	// INVLPG
	i3a1_o1(TEXT("invlpg"),0x0F,0x01,7,op_rm_none);

	// HLT
	i_o0(TEXT("hlt"),0xF4);

	// RDMSR
	i2_o0(TEXT("rdmsr"),0x0F,0x32);

	// RDPMC
	i2_o0(TEXT("rdpmc"),0x0F,0x33);

	// RDTSC
	i2_o0(TEXT("rdtsc"),0x0F,0x31);

	// WRMSR
	i2_o0(TEXT("wrmsr"),0x0F,0x30);
/*
добавить:
RSM
*/
}

//** макросы для загрузки FPU-инструкций
//загрузить инструкцию типа FADD (вместе с парной инструкцией FADDP)
#define fpu_oper(name,code_m32,code_m64,code_p,addcode) \
	fpui1_oper<3>(name,code_m32,addcode,op_rm_read_sz<4,RC_FPU>); \
	fpui1_oper<3>(name,code_m64,addcode,op_rm_read_sz<8,RC_FPU>); \
	fpui2_oper<1>(name,code_m32,0x18 | addcode,op_regcnst<RC_FPU,0>,op_rm_read_sz<10,RC_FPU>); \
	fpui2_oper<1>(name,code_m64,0x18 | addcode,op_rm_write_sz<10,RC_FPU>,op_regcnst<RC_FPU,0>); \
	fpui2_oper<1>(name TEXT("p"),code_p,0x18 | addcode,op_rm_write_sz<10,RC_FPU>,op_regcnst<RC_FPU,0>)

//загрузить FPU-инструкции
VOID LoadFPUInstructions()
{
	// F2XM1
	i2_o0(TEXT("f2xm1"),0xD9,0xF0);
	// FABS
	i2_o0(TEXT("fabs"),0xD9,0xE1);
	// FADD
	fpu_oper(TEXT("fadd"),0xD8,0xDC,0xDE,0);
}
