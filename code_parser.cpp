#include "code_parser.h"
#include "instruction.h"
#include "interpretations.h"

#include "consts.h"
#include <list>

/*
Файл, который реализует разбор инструкций.
*/

CCodeParser::CCodeParser(CCyberMemory* pMemory,CAddressMap* pAddressMap,CInstructionSet* pSet16,CInstructionSet* pSet32)
{
	//создать объект для доступа к киберпамяти
	mpMemory=new CCyberMemoryAccessor(pMemory);

	//сформировать структуры режимов
	mModes[MODE_16BIT].pInstructionSet=pSet16;
	pSet16->AddRef();
	mModes[MODE_16BIT].NormalAddressOperandSize=2;
	mModes[MODE_16BIT].RedefinedAddressOperandSize=4;

	mModes[MODE_32BIT].pInstructionSet=pSet32;
	pSet32->AddRef();
	mModes[MODE_32BIT].NormalAddressOperandSize=4;
	mModes[MODE_32BIT].RedefinedAddressOperandSize=2;

	//запомнить указатель на хранилище адресной информации
	mpAddressMap=pAddressMap;
	mpAddressMap->AddRef();
}

CCodeParser::~CCodeParser()
{
	//удалить объект для доступа к памяти
	mpMemory->Release();
	//освободить карту адресов
	mpAddressMap->Release();
	//освободить наборы инструкций
	mModes[MODE_16BIT].pInstructionSet->Release();
	mModes[MODE_32BIT].pInstructionSet->Release();
}

VOID CCodeParser::UpdateMode()
{
	//если присутствует префикс изменения разрядности операнда
	if(mPrefixFlags & PF_OS)
		//указать обратный режим
		mpCurrentMode=mModes+!mMode;
	else
		//режим не меняется
		mpCurrentMode=mModes+mMode;
	//если присутствует префикс изменения разрядности адреса
	if(mPrefixFlags & PF_AS)
		//указать текущий размер адреса
		mAddressOperandSize=mModes[mMode].RedefinedAddressOperandSize;
	else
		mAddressOperandSize=mModes[mMode].NormalAddressOperandSize;
}

//функция, выполняющая разбор поля ModR/M
/*
        Формат поля ModRM:
          [ ][ ][ ][ ][ ][ ][ ][ ]
           7                    0
          [MOD ][  R/O  ][  R/M  ]
        Формат поля SIB:
          [ ][ ][ ][ ][ ][ ][ ][ ]
           7                    0
          [ S  ][   I   ][   B   ]
*/
VOID CCodeParser::ParseModRM(MODRM* pModRM)
{
	//считать байт ModR/M
	BYTE bModRM=mpMemory->CurrentByte();

	//получить поле R/M
	BYTE bRM=(bModRM & 0x7);
	//получить поле R/O
	BYTE bRO=((bModRM>>3) & 0x7);
	//получить поле MOD
	BYTE bMOD=((bModRM>>6) & 0x7);

	//обнулить флаги RM
	pModRM->RM.bFlags=0;

	//сформировать операнд R/O, который - всегда регистр
	pModRM->RO.bRORegister=bRO;

	//далее будет идти разбор только операнда R/M

	//если используется регистровая адресация
	if(bMOD==3)
	{
		//поле R/M описывает регистр; сформировать операнд R/M
		pModRM->RM.bRegister=bRM;
		//всё!
		return;
	}

	//иначе операнд R/M находится в памяти; MOD может иметь значения 0, 1, 2

	//указать тип операнда
	pModRM->RM.bFlags=MODRM_RM_MEMORY;

	//определить размер значения смещения с помощью поля MOD
	BYTE bOffsetLength;
	//если MOD=0, то смещения нет
	if(bMOD==0) bOffsetLength=0;
	//иначе если MOD=1, то смещение - байтовое
	else if(bMOD==1) bOffsetLength=1;
	//иначе MOD=2, и смещение занимает 2 или 4 байта в зависимости от текущего режима
	else
		bOffsetLength=mAddressOperandSize;

	//далее действия зависят от режима процессора
	if(mAddressOperandSize==4)		// 32-битный режим
	{
		//если поле R/M не равно 4, то SIB не используется
		if(bRM!=4)
		{
			//если R/M=5 и MOD=0, то используется только 32-битное смещение (в отличие от остальных случаев MOD=0), и не используется регистр
			if((bRM==5) && (bMOD==0))
			{
				//исправить размер значения смещения
				bOffsetLength=4;
				//установить флаги операнда-переменной
				pModRM->RM.bFlags|=MODRM_RM_OFFSET;
			}
			else
			{
				//получить базовый регистр для адресации
				pModRM->RM.bBaseRegister=REGISTER_ID(RC_DWORD,bRM);
				//установить флаги
				pModRM->RM.bFlags|=OMF_BASE_REGISTER | (bOffsetLength ? MODRM_RM_OFFSET : 0);
			}
			//считать смещение
			pModRM->RM.dwOffset=mpMemory->CurrentSignedData(bOffsetLength);
			//всё!
			return;
		}

		//теперь остался только случай, когда R/M=4, т.е. используется SIB

		//обработка SIB

		//считать байт SIB
		BYTE bSIB=mpMemory->CurrentByte();

		//получить базовый регистр из поля B
		pModRM->RM.bBaseRegister=bSIB & 0x7;
		//(базовый регистр не используется, если B=5 и MOD=0
		if(!((pModRM->RM.bBaseRegister==5) && (bMOD==0)))
		{
			//добавить флаг использования базового регистра
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER;
			//сформировать идентификатор регистра
			pModRM->RM.bBaseRegister=REGISTER_ID(RC_DWORD,pModRM->RM.bBaseRegister);
		}
		//иначе действительно B=5 и MOD=0
		else
			//в таком случае базового регистра нет, но есть 32-битное смещение
			bOffsetLength=4;

		//получить индексный регистр из поля I
		pModRM->RM.bIndexRegister=(bSIB >> 3) & 0x7;
		//(индексный регистр не используется, если I=4)
		if(pModRM->RM.bIndexRegister!=4)
		{
			//добавить флаг использования индексного регистра
			pModRM->RM.bFlags|=MODRM_RM_INDEX_REGISTER;
			//сформировать идентификатор регистра
			pModRM->RM.bIndexRegister=REGISTER_ID(RC_DWORD,pModRM->RM.bIndexRegister);
		}

		//получить множитель из поля S
		pModRM->RM.bMultiplier=1<<(bSIB >> 6);
		//добавить флаг использования множителя
		if(pModRM->RM.bMultiplier>1) pModRM->RM.bFlags|=MODRM_RM_MULTIPLIER;

		//считать смещение
		pModRM->RM.dwOffset=mpMemory->CurrentSignedData(bOffsetLength);
		//указать флаг наличия смещения операнда-переменной
		pModRM->RM.bFlags|=((bOffsetLength && pModRM->RM.dwOffset) ? MODRM_RM_OFFSET : 0);
	}
	else		// 16-битный режим
	{
		//если MOD=00 и R/M=6, то используется только смещение
		if((bMOD==0) && (bRM==6))
		{
			//установить флаги
			pModRM->RM.bFlags|=MODRM_RM_OFFSET;
			//исправить размер значения смещения
			bOffsetLength=2;
			//считать смещение
			pModRM->RM.dwOffset=mpMemory->CurrentSignedData(bOffsetLength);
			//всё!
			return;
		}

		//теперь осталось только разобрать случаи R/M
		switch(bRM)
		{
		case 0:					// [BX+SI]
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER | MODRM_RM_INDEX_REGISTER;
			pModRM->RM.bBaseRegister=REGISTER_ID(RC_WORD,3);
			pModRM->RM.bIndexRegister=REGISTER_ID(RC_WORD,6);
			break;
		case 1:					// [BX+DI]
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER | MODRM_RM_INDEX_REGISTER;
			pModRM->RM.bBaseRegister=REGISTER_ID(RC_WORD,3);
			pModRM->RM.bIndexRegister=REGISTER_ID(RC_WORD,7);
			break;
		case 2:					// [BP+SI]
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER | MODRM_RM_INDEX_REGISTER;
			pModRM->RM.bBaseRegister=REGISTER_ID(RC_WORD,5);
			pModRM->RM.bIndexRegister=REGISTER_ID(RC_WORD,6);
			break;
		case 3:					// [BP+DI]
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER | MODRM_RM_INDEX_REGISTER;
			pModRM->RM.bBaseRegister=REGISTER_ID(RC_WORD,5);
			pModRM->RM.bIndexRegister=REGISTER_ID(RC_WORD,7);
			break;
		case 4:					// [SI]
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER;
			pModRM->RM.bBaseRegister=REGISTER_ID(RC_WORD,6);
			break;
		case 5:					// [DI]
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER;
			pModRM->RM.bBaseRegister=REGISTER_ID(RC_WORD,7);
			break;
		case 6:					// [BP]
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER;
			pModRM->RM.bBaseRegister=REGISTER_BP;
			break;
		case 7:					// [BX]
			pModRM->RM.bFlags|=MODRM_RM_BASE_REGISTER;
			pModRM->RM.bBaseRegister=REGISTER_ID(RC_WORD,3);
			break;
		}

		//добавить смещение
		if(bOffsetLength)
		{
			//добавить флаг
			pModRM->RM.bFlags|=MODRM_RM_OFFSET;
			//считать смещение
			pModRM->RM.dwOffset=mpMemory->CurrentSignedData(bOffsetLength);
		}
	}

	//всё!
}

//обработать очередной байт инструкции
DWORD CCodeParser::ParseInstructionByte(DWORD Group,BYTE Mask)
{
	//считать текущий байт и наложить на него маску
	BYTE Byte=mpMemory->CurrentByte() & Mask;

	//получить информацию о байте инструкции
	INSTRUCTION_BYTE IB=mpCurrentMode->pInstructionSet->mInstructionBytes[Group+Byte];

	//если этот байт определён
	if(IB.bFlags & IBF_OK)
		//если этот байт - префикс
		if(IB.bFlags & IBF_PREFIX)
		{
			//установить соотвеcтвующий флаг префикса
			mPrefixFlags|=IB.dwPrefixFlag;
			//сдвинуть киберуказатель инструкции, чтобы собственно инструкция начиналась позже
			mInstructionAddress++;
			//продолжить разбор байтов
			return ParseInstructionByte(Group,Mask);
		}
		//иначе если этот байт - конечный
		else if(IB.bFlags & IBF_END)
			//разбор закончен, вернуть идентификатор инструкции
			return IB.dwInstructionID;
		//иначе этот байт не последний
		else
			//рекурсивно продолжить разбор
			return ParseInstructionByte(IB.dwNextByteGroup,IB.bMask);
	//иначе этот байт не определён
	else
		//вернуть ошибку
		return INSTRUCTION_ID_ERROR;
}

CInstruction* CCodeParser::ParseInstruction(CYBER_ADDRESS Address)
{
	//запомнить адрес текущей инструкции
	mInstructionAddress=Address;
	//установить текущий адрес
	mpMemory->SetPointer(Address);

	//сбросить флаги префиксов
	mPrefixFlags=0;

	//выполнить разбор байтов инструкции и получить номер инструкции
	DWORD InstructionID=ParseInstructionByte(0,0xFF);

	//далее вместо Address используется mInstructionAddress, так как это значение
	//должно быть с учётом сдвига из-за наличия префиксов (обрабатывается в
	//ParseInstructionByte).

	//если инструкция ошибочна
	if(InstructionID==INSTRUCTION_ID_ERROR)
		//вернуть ошибку
		return NULL;

	//обновить текущий режим
	UpdateMode();

	//кэшировать информацию об инструкции
	INSTRUCTION_INFORMATION InstructionInformation=mpCurrentMode->pInstructionSet->mInstructions[InstructionID];

	//если инструкции требуется регистровый байт
	BYTE ByteRegister;
	{
		CCyberMemory* pMemory;
		try
		{
			if(InstructionInformation.bFlags & IIF_REGBYTE)
			{
				//считать его и извлечь номер регистра
				pMemory=mpMemory->Memory();
				ByteRegister=(pMemory->Byte(mInstructionAddress+InstructionInformation.bRegisterByteOffset) & 0x7);
				pMemory->Release();
			}
		}
		catch(CCyberMemoryPageFaultException)
		{
			//освободить память
			pMemory->Release();
			//передать исключение дальше
			throw;
		}
	}

	//если присутствует префикс переопределения сегмента
	BYTE Segment;
	BOOL IsSegment=FALSE;
	if(mPrefixFlags & PREFIXES_SEGMENT)
	{
		//указать флаг наличия сегмента
		IsSegment=TRUE;
		//получить его
		BYTE j=0;
		for(BYTE i=1;i<=PREFIXES_SEGMENT;i<<=1,j++)
			if(mPrefixFlags & i)
			{
				Segment=j;
				break;
			}
	}

	//установить текущий указатель памяти на операнды
	mpMemory->SetPointer(mInstructionAddress+InstructionInformation.bCodeSize);

	//если инструкции требуется байт ModRM
	MODRM ModRM;
	//блока try нет, так как ресурсы не выделяются
	if(InstructionInformation.bFlags & IIF_MODRM)
		//получить и обработать его
		ParseModRM(&ModRM);

	//создать инструкцию
	CInstruction* pInstruction=new CInstruction(mpCurrentMode->pInstructionSet,InstructionID,mPrefixFlags);

	//список операндов для последующей корректировки
	std::list<COperand*> OperandsList;

	//цикл по операндам
	for(BYTE i=0;i<InstructionInformation.bOperandsCount;++i)
	{
		//кэшировать информацию об операнде
		OPERAND_INFORMATION OperandInformation=InstructionInformation.Operands[i];

		//блок перехвата исключений чтения памяти
		try
		{
			//выбор по источнику операнда
			COperand* pOperand;
			switch(OperandInformation.bSource)
			{
			case OIS_DATA:												//константа
				//создать операнд - константу
				pOperand=new COperandData(mpMemory->CurrentData(OperandInformation.bSize));
				break;
			case OIS_SIGNED_DATA:									//константа со знаком
				//создать операнд - константу
				pOperand=new COperandData(mpMemory->CurrentSignedData(OperandInformation.bSize));
				break;
			case OIS_REGISTER_BYTE:								//регистр из регистрового байта
				//создать операнд - регистр
				pOperand=new COperandRegister(ByteRegister);
				break;
			case OIS_MODRM_RO:										//регистр из поля RO байта ModR/M
				//создать операнд - регистр
				pOperand=new COperandRegister(ModRM.RO.bRORegister);
				break;
			case OIS_MODRM_RM:										//регистр или переменная из поля RM байта ModR/M
				//если операнд - переменная
				if(ModRM.RM.bFlags & MODRM_RM_MEMORY)
					//создать операнд - переменную
					pOperand=new COperandMemory(ModRM);
				//иначе операнд - регистр
				else
					//создать операнд - регистр
					pOperand=new COperandRegister(ModRM.RM.bRegister);
				break;
			case OIS_MEMORY_IM:										//переменная по константному адресу
				//создать операнд - переменную (изменить размер считываемого адреса, если нужно)
				//ERROR FIXED
//				pOperand=new COperandMemory(mpMemory->CurrentData(OperandInformation.bSize>1 ? mAddressOperandSize : 1));
				pOperand=new COperandMemory(mpMemory->CurrentData(mAddressOperandSize));
				break;
			case OIS_CONST_REGISTER:							//постоянный регистр
				//создать операнд - регистр
				pOperand=new COperandRegister(OperandInformation.bConstRegister);
				break;
			case OIS_CONST_DATA:									//постоянная константа
				//создать операнд - константу
				pOperand=new COperandData(OperandInformation.bConstData);
				break;
			}
			//добавить операнд в список инструкции и в локальный список
			pInstruction->AddOperand(pOperand);
			OperandsList.push_back(pOperand);
		}
		catch(CCyberMemoryPageFaultException)
		{
			//не удалось считать какую-либо переменную

			//удалить уже распознанные операнды
			for(std::list<COperand*>::iterator i=OperandsList.begin();i!=OperandsList.end();i++)
				(*i)->Release();
			OperandsList.clear();
			//удалить инструкцию
			pInstruction->Release();

			//передать исключение дальше
			throw;
		}
	}

	//скорректировать операнды
	//корректировка выполняется после обработки всех операндов, чтобы выяснить длину инструкции
	DWORD j=0;
	for(std::list<COperand*>::iterator i=OperandsList.begin();i!=OperandsList.end();i++)
	{
		//получить указатель на операнд
		COperand* pOperand=*i;
		//кэшировать информацию об операнде
		OPERAND_INFORMATION OperandInformation=InstructionInformation.Operands[j++];
		//выбор по типу операнда
		switch(pOperand->GetType())
		{
		case OT_DATA:
			{
				//флаг того, что нужно создавать ссылку
				BOOL bNeedLink=FALSE;
				//если данные рассматриваются как абсолютный указатель
				if(OperandInformation.bDataReference & OIR_ABSOLUTE_POINTER)
				{
					bNeedLink=TRUE;
					//в 16-битном режиме 4-х байтный адрес - это far-ссылка (сегмент:смещение)
					if(mMode==MODE_16BIT && OperandInformation.bSize==4)
						//лучше наверно не транслировать far-вызовы
						((COperandData*)pOperand)->mReferenceFlags&=~OIR_ABSOLUTE_POINTER;
//						((COperandData*)pOperand)->mData=(((COperandData*)pOperand)->mData & 0xFFFF) + ((((COperandData*)pOperand)->mData & 0xFFFF0000)>>12);
				}
				//иначе если данные рассматриваются как относительный указатель
				else if(OperandInformation.bDataReference & OIR_RELATIVE_POINTER)
				{
					bNeedLink=TRUE;
					//скорректировать указатель
					//в 16-битном режиме смещение сдвигает только вдоль сегмента (не всегда работает)
					if(mMode==MODE_16BIT && OperandInformation.bSize==2)
					{
						WORD offset=(WORD)((COperandData*)pOperand)->mData + (WORD)(mpMemory->GetPointer() & 0xFFFF);
						((COperandData*)pOperand)->mData=(mpMemory->GetPointer() & 0xFFFF0000) | offset;
					}
					else
						((COperandData*)pOperand)->mData+=mpMemory->GetPointer();
				}
				//если необходимо создать ссылку
				if(bNeedLink)
				{
					//запомнить тип ссылки данных
					((COperandData*)pOperand)->mReferenceFlags=OperandInformation.bDataReference;
					//создать перекрестную ссылку
					AddXReference(Address,((COperandData*)pOperand)->mData,OperandInformation.bDataReference);
				}
			}
			break;
		case OT_REGISTER:
			//скорректировать идентификатор регистра с помощью категории регистра
			((COperandRegister*)pOperand)->mRegister=REGISTER_ID(OperandInformation.bRegisterCategory,((COperandRegister*)pOperand)->mRegister);
			break;
		case OT_MEMORY:
			//запомнить размер операнда
			((COperandMemory*)pOperand)->mSize=OperandInformation.bSize;
			//если есть префикс переопределения сегмента
			if(IsSegment)
				//указать сегмент
				((COperandMemory*)pOperand)->SetSegment(Segment);
			//если переменная задаётся только смещением
			if(((COperandMemory*)pOperand)->mFlags==OMF_OFFSET)
				//создать перекрестную ссылку
				AddXReference(Address,((COperandMemory*)pOperand)->mOffset,OperandInformation.bAddressReference,OperandInformation.bSize);
			break;
		}

		//освободить операнд
		pOperand->Release();
	}

	//очистить список указателей на операнды
	OperandsList.clear();

	//всё!
	return pInstruction;
};

VOID CCodeParser::AddXReference(CYBER_ADDRESS AddressFrom,CYBER_ADDRESS AddressTo,BYTE ReferenceFlags,BYTE Size)
{
	//добавить перекрестную ссылку в хранилище
	mpAddressMap->AddXReference(AddressFrom,AddressTo,ReferenceFlags);

	//получить данные адреса
	CAddressData* pAddressData=mpAddressMap->OpenAddress(AddressTo);

	//сформировать имя метки
	TCHAR szLabel[0x10];

	//если ссылка ссылается для выполнения
	if(ReferenceFlags & OIR_REFERENCE_EXECUTE)
	{
		//если ссылка ссылается как вызов процедуры
		if(ReferenceFlags & OIR_REFERENCE_EXECUTE_CALL)
		{
			//добавить адрес в очередь дизассемблирования
			mParsingQueue.insert(std::make_pair(AddressTo,AddressTo));
			_stprintf(szLabel,TEXT("sub_%X"),AddressTo);
			//кроме того, создать функцию
			pAddressData->CreateSubroutine();
		}
		else
		{
			//добавить адрес в очередь дизассемблирования
			mParsingQueue.insert(std::make_pair(AddressTo,mSubroutineAddress));
			_stprintf(szLabel,TEXT("loc_%X"),AddressTo);
		}
	}
	//иначе ссылка ссылается на данные
	else
	{
		//указать метку
		_stprintf(szLabel,TEXT("data_%X"),AddressTo);
		//установить интерпретацию-данные для адреса, если её нет
		CInterpretation* pInterpretation=pAddressData->GetInterpretation();
		if(pInterpretation)
			pInterpretation->Release();
		else
		{
			CCyberMemory* pCyberMemory=mpMemory->Memory();
			pInterpretation=new CInterpretationData(pCyberMemory,AddressTo,Size);
			pCyberMemory->Release();
			pAddressData->SetInterpretation(pInterpretation);
			mpAddressMap->SetAddressSize(AddressTo,Size);
			pInterpretation->Release();
		}
	}

	//освободить данные адреса
	pAddressData->Release();

	//указать метку
	mpAddressMap->SetLabel(AddressTo,szLabel);
}

VOID CCodeParser::SetMode(DWORD Mode)
{
	//установить текущий режим
	mMode=Mode;
	mpCurrentMode=mModes+mMode;
}

VOID CCodeParser::ParseEntryPoint(CYBER_ADDRESS EntryPoint)
{
	//добавить адрес в очередь дизассемблирования
	mParsingQueue.insert(std::make_pair(EntryPoint,EntryPoint));

	//дизассемблировать кодовые блоки
	std::set<std::pair<CYBER_ADDRESS,CYBER_ADDRESS> >::iterator i;
	while((i=mParsingQueue.begin())!=mParsingQueue.end())
	{
		//получить адрес начала
		CYBER_ADDRESS Address=(*i).first;
		mSubroutineAddress=(*i).second;
		//удалить кодовый блок
		mParsingQueue.erase(i);

		//цикл дизассемблирования
		for(;;)
		{
			//открыть данные адреса
			CAddressData* pAddressData=mpAddressMap->OpenAddress(Address);
			//получить уже существующую интерпретацию, если она есть
			CInterpretation* pExistInterpretation=pAddressData->GetInterpretation();
			if(pExistInterpretation)
			{
				//если тип интерпретации - код, то закончить
				if(pExistInterpretation->GetInterpretationType()==ADEIT_CODE)
				{
					pExistInterpretation->Release();
					pAddressData->Release();
					break;
				}
				pExistInterpretation->Release();
			}

			CInterpretationCode* pInterpretation;
			CInstruction* pInstruction;
			//дизассемблировать инструкцию
			try
			{
				pInstruction=ParseInstruction(Address);
			}
			catch(CCyberMemoryPageFaultException)
			{
				//мы дошли до нечитаемой памяти
				//освободить данные адреса
				pAddressData->Release();
				//закончить дизассемблирование блока
				break;
			}
			//создать интерпретацию
			pInterpretation=new CInterpretationCode(pInstruction);

			//указать интерпретацию для данных адреса
			pAddressData->SetInterpretation(pInterpretation);

			//указать адрес начала процедуры
			pAddressData->SetBlockAddress(mSubroutineAddress);

			//размер адреса
			CYBER_ADDRESS Size;
			//указать размер адреса
			if(pInstruction)
				Size=mpMemory->GetPointer()-Address;
			else
				Size=1;
			mpAddressMap->SetAddressSize(Address,Size);

			//если инструкция распознана
			if(pInstruction)
			{
				//получить идентификатор инструкции
				DWORD InstructionID=pInstruction->mInstructionID;
				//освободить инструкцию
				pInstruction->Release();
				//если инструкция является окончанием блока кода
				if(mpCurrentMode->pInstructionSet->mInstructions[InstructionID].bFlags & IIF_ENDPROC)
				{
					//установить флаг конца процедуры
					pInterpretation->SetFlags(ICF_SUBROUTINE_END);
					//освободить интерпретацию и данные адреса
					pInterpretation->Release();
					pAddressData->Release();
					//закончить дизассемблирование
					break;
				}
				//иначе если инструкция является разделителем
				else if(mpCurrentMode->pInstructionSet->mInstructions[InstructionID].bFlags & IIF_SPACE)
					//установить флаг разделителя
					pInterpretation->SetFlags(ICF_SPACE);
			}

			//освободить интерпретацию и данные адреса
			pInterpretation->Release();
			pAddressData->Release();

			//переместиться к следующей инструкции
			Address+=Size;
		}
	}
}
