/*


    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
#include <ptoc.h>

typedef
  unsigned char STRING[256];
int sql__strlen();
typedef
  int SymbolTableRange;
typedef
  int PRIMTYPE;
typedef
  int KINDTYPE;
typedef
  int CLASSTYPE;
typedef
  int SUBCLASSTYPE;
typedef
  struct ptoc_Type0
    {
    union  
      {
      struct  
        {
        int dummy1;
        int dummy2;
        int dummy3;
        } C_DummyDcl;
      struct  
        {
        int SAlign;
        int SSize;
        int SOffset;
        } C_TypeDcl;
      struct  
        {
        int STempIndex;
        } C_BlockDcl;
      struct  
        {
        int SRegister;
        } C_CBlockDcl;
      } KINDTYPE;
    } MACHDEP;
SymbolTableRange SymPtr;
struct ptoc_Type1
  {
  int SNext;
  int SName;
  int SLink;
  int SFile;
  int SType;
  int SFlags;
  int SCard;
  int SLevel;
  int STname;
  int SOrder;
  int SFeld1;
  int SFeld2;
  int SRecVar;
  int SFeld3;
  int SFeld4;
  int SFeld5;
  int STmyname;
  KINDTYPE SKind;
  union  
    {
    struct  
      {
      MACHDEP DummyInfo;
      } C_DummyDcl;
    struct  
      {
      MACHDEP TypeInfo;
      union  
        {
        struct  
          {
          int SLowRange;
          int SHighRange;
          SymbolTableRange SFirstEnum;
          } C_SubRangeType;
        struct  
          {
          char dummy;
          } C_FloatType;
        struct  
          {
          SymbolTableRange SIndex;
          SymbolTableRange SElement;
          } C_ArrayType;
        struct  
          {
          SymbolTableRange SFirstField;
          SymbolTableRange SFirstVariant;
          SymbolTableRange SNextVariant;
          } C_RecordType;
        struct  
          {
          SymbolTableRange SComponent;
          } C_FileType;
        struct  
          {
          SymbolTableRange SBase;
          } C_SetType;
        struct  
          {
          SymbolTableRange SDomain;
          } C_PointerType;
        struct  
          {
          SymbolTableRange SResult;
          SymbolTableRange SParmCnt;
          SymbolTableRange SBlock;
          } C_ProcedureType;
        struct  
          {
          int SBlockLen;
          int SElemAlign;
          } C_BlockType;
        struct  
          {
          int SLength;
          SymbolTableRange SElem;
          } C_StringType;
        } C_TypeDcl;
      } C_TypeDcl;
    struct  
      {
      MACHDEP AddrInfo;
      CLASSTYPE SClass;
      SUBCLASSTYPE SSubClass;
      SymbolTableRange SParent;
      } C_ModuleDcl;
    struct  
      {
      MACHDEP AllocInfo;
      SymbolTableRange SFuncBlock;
      SymbolTableRange SLastEntry;
      } C_BlockDcl;
    struct  
      {
      MACHDEP ElementInfo;
      SymbolTableRange SNextField;
      } C_RecordElementDcl;
    struct  
      {
      int SCon1val;
      int SCon2val;
      SymbolTableRange SNextEnum;
      } C_ConDcl;
    struct  
      {
      MACHDEP SysProcInfo;
      int SSysSpecific;
      } C_SysProcDcl;
    struct  
      {
      MACHDEP TypeNameInfo;
      } C_TypeNameDcl;
    } KINDTYPE;
  } STab[32768];
typedef
  int HdgRange;
int field[1];
int control[256];
int FillForVar;
int shl();
int shr();
int StrTable[32768];
int StabI();
int EnterString();
int GetString();
unsigned char *_GetStringPtr();
int _GetStringLength();
int _GetStringNo();
unsigned char _GetStringChar();
unsigned char _GetFirstChar();
int SetMinor();
int node3();
int UnaryExpression();
int BinaryExpression();
int SetBinaryParameters();
int AppendList();
int InsertList();
int LinkLists();
int MakeList();
int ExtractList();
int ComplexStmnt();
int CaseStmnt();
int special();
int ShortStatement();
int LongStatement();
int ProcNode();
int LinkChain();
int IsConstant();
int FoldUnary();
int FoldBinary();
int FoldSubscript();
int FoldTree();
int UtilitiesI();
int PasCerror();
int BaseType();
int CopyTree();
int demorgan();
int GetTemporary();
int FreeTemporary();
int ClearTemporarys();
int BasisTyp();
typedef
  int ParseStackRange;
typedef
  int ParseStack[1001];
typedef
  int ExpListRange;
typedef
  int IntExpList[1001];
typedef
  struct ptoc_Type2
    {
    int LIndex;
    int DefLevel;
    int RefLevel;
    } LabelStackType[101];
SymbolTableRange IntegerPtr;
SymbolTableRange BooleanPtr;
SymbolTableRange CharPtr;
SymbolTableRange PacPtr;
SymbolTableRange LongPtr;
SymbolTableRange FloatPtr;
SymbolTableRange DoublePtr;
SymbolTableRange StringPtr;
int ProcMark;
int ProcIndex;
int LevelNo;
int mp;
ParseStack StartCnt;
ParseStack cnt;
ParseStack g1;
ParseStack g2;
ParseStack val;
ParseStack TypeType;
ParseStack SubType;
ParseStack CardNum;
ParseStack ColNo;
ParseStack WithRecs;
ParseStack NameofConst;
IntExpList ExpType;
IntExpList ExpStype;
IntExpList ExpColNo;
IntExpList ExpVal;
int IdPtr;
int IdList[1001];
int CardCount;
int CurrentFile;
HdgRange CaseRoot;
LabelStackType LabelStack;
int LabelPtr;
int LabelScopePtr;
int ForWithLevel;
SymbolTableRange InputPtr;
SymbolTableRange OutputPtr;
SymbolTableRange AnyPtrPtr;
SymbolTableRange PtrToCharPtr;
SymbolTableRange AnyPacPtr;
SymbolTableRange AnyPacLenPtr;
SymbolTableRange AnyFilePtr;
SymbolTableRange AnySetPtr;
int SymtabI();
int PrimType();
int ChkEqualType();
int IdPtrIncr();
int ParmPtrIncr();
int IncrExpPtr();
int IncrConstPtr();
int IncrLabelPtr();
int MoveStacks();
int PopScope();
int upscope();
int CloseRec();
int OpenRec();
int NewSymbol();
int FindSymbol();
int FindQSymbol();
int EntSymbol();
int EntModule();
int EntBlock();
int EntConstant();
int EntVariable();
int LabelName();
int EntField();
int EntVariant();
int EntLabel();
int InitSystem();
int ChkTypeDefn();
int TypName();
int CheckType();
int CheckForwardDefn();
int DclProc();
int EntParameterList();
int FormalProcType();
int EntProcParm();
int IdLookup();
int QidLookup();
int FindIntset();
int CheckCaseConst();
HdgRange SaveIndex;
int SysprocsI();
int CallPcstart();
int CallPcexit();
int CallFnil();
int CallCompare();
int CallSetOp();
int CallSetRel();
int CallInct();
int CallIntor();
int CallInt();
int CallCttot();
int CallConvertt();
int CallStringCopy();
int CallStringConcat();
int CallStringCompare();
int CallRange();
int CallNilCheck();
int EvalGoto();
int EvalDefname();
int IsProcedure();
int EvalSys();
int TypeSize();
int StackError();
int IsOrdinal();
int IsIntegral();
int IsEnum();
int IsFile();
int IsTextFile();
int IsPac();
int IsCharFile();
int IsCharArray();
int LengthOfCharArray();
int IsLval();
int IsSet();
int IsPointer();
int IsStructured();
int IsCallRange();
int SkipCallRange();
int CharToPac();
int IsString();
int LengthOfString();
int PacToString();
int CharToString();
int EvalConvert();
int CheckCall();
int CompatibleTypes();
int ContainsFile();
int FileInRec();
int IsNegative();
int FillVar();
int ConstExp();
int IsPrimConstant();
int alloc_name_space();
int P1m2I();
int ErrorMsg();
int DclError();
int werror();
int FatalErrorMsg();
int WarnStrCmp();
int EvalBinary();
int EvalDivide();
int EvalModDiv();
int EvalRelational();
int EvalIn();
int AssgnmntCompatible();
int EvalUnary();
int EvalSetExp();
int EvalStringExp();
int EvalSetConstructor();
int RangeCheck();
int GenStore();
int GenCall();
int GenAddress();
int AccessVariable();
int IndexedVar();
int EvalWith();
int EndWith();
int QualifiedVar();
int RefVar();
int DerefVar();
int BuildCaseConst();
int BuildCase();
int BuildDefault();
int LabDef();
int LabRef();
int EvalTo();
int EvalDownto();
int EvalFor();
int EvalProgramParameters();
int EvalAssert();
int IsConstantExpr();
int CsgPrint();
int NoSideEffects();
int FreeSetTempFlag;
int SetTempCount;
int SetTempStack[101];
int FreeStringTempFlag;
int StringTempCount;
int StringTempStack[101];
int process_initializer();
int GenConDclName();
int GenConstName();

struct _ptoc_FRAME_InitSemantics
  {
  char dummy;
  } ;


int InitSemantics()
    {
    struct _ptoc_FRAME_InitSemantics _ptoc_LOCAL;
    
    FreeSetTempFlag = 1;
    SetTempCount = -1;
    FreeStringTempFlag = 1;
    StringTempCount = -1;
    }

struct _ptoc_FRAME_ClearLabel
  {
  char dummy;
  } ;


int ClearLabel()
    {
    struct _ptoc_FRAME_ClearLabel _ptoc_LOCAL;
    int i;
    struct ptoc_Type2 *ptoc_Var0;
    
    i = LabelScopePtr + 1;
    while (i <= LabelPtr)
        {
        ptoc_Var0 = &LabelStack[i];
        if (ptoc_Var0->DefLevel == ForWithLevel)
            {
            ptoc_Var0->DefLevel = 0;
            }
        if (ptoc_Var0->RefLevel == ForWithLevel)
            {
            ptoc_Var0->RefLevel += -1;
            }
        i += 1;
        }
    ForWithLevel += -1;
    }

struct _ptoc_FRAME_SetResultType
  {
  char dummy;
  } ;


int SetResultType(typ1, typ2)
SymbolTableRange typ1;
SymbolTableRange typ2;
    {
    struct _ptoc_FRAME_SetResultType _ptoc_LOCAL;
    int _SetResultType;
    SymbolTableRange typ;
    SymbolTableRange btyp1;
    SymbolTableRange btyp2;
    SymbolTableRange ptyp1;
    SymbolTableRange ptyp2;
    int low;
    int high;
    int lowlow;
    int highhigh;
    
    if (typ1 == typ2)
        {
        _SetResultType = typ1;
        }
    else
        {
        if (typ1 == AnySetPtr)
            {
            _SetResultType = typ2;
            }
        else
            {
            if (typ2 == AnySetPtr)
                {
                _SetResultType = typ1;
                }
            else
                {
                if (typ1 == FindIntset())
                    {
                    _SetResultType = typ2;
                    }
                else
                    {
                    if (typ2 == FindIntset())
                        {
                        _SetResultType = typ1;
                        }
                    else
                        {
                        btyp1 = STab[typ1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase;
                        btyp2 = STab[typ2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase;
                        ptyp1 = PrimType(btyp1);
                        ptyp2 = PrimType(btyp2);
                        if (CompatibleTypes(ptyp1 , ptyp2))
                            {
                            if (STab[btyp1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange < STab[btyp2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange)
                                {
                                lowlow = STab[btyp1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
                                low = 1;
                                }
                            else
                                {
                                lowlow = STab[btyp2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
                                low = 2;
                                }
                            if (STab[btyp1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange > STab[btyp2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange)
                                {
                                highhigh = STab[btyp1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange;
                                high = 1;
                                }
                            else
                                {
                                highhigh = STab[btyp2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange;
                                high = 2;
                                }
                            switch (low * 2 + high)
                                {
                                case 3 :
                                    _SetResultType = typ1;
                                    break;
                                case 6 :
                                    _SetResultType = typ2;
                                    break;
                                case 4 :
                                case 5 :
                                    if (ptyp1 == ptyp2)
                                        {
                                        NewSymbol(0 , ptyp1 , 7 , LevelNo , -1);
                                        }
                                    else
                                        {
                                        NewSymbol(0 , IntegerPtr , 7 , LevelNo , -1);
                                        }
                                    typ = SymPtr;
                                    STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = lowlow;
                                    STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = highhigh;
                                    TypeSize(typ);
                                    NewSymbol(0 , -16 , 7 , LevelNo , -1);
                                    STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase = typ;
                                    TypeSize(SymPtr);
                                    _SetResultType = SymPtr;
                                    break;
                                }
                            }
                        else
                            {
                            _SetResultType = -21;
                            }
                        }
                    }
                }
            }
        }
    return(_SetResultType);
    }

struct _ptoc_FRAME_ResultType
  {
  char dummy;
  } ;


int ResultType(exp1, exp2, col1, col2)
HdgRange exp1;
HdgRange exp2;
int col1;
int col2;
    {
    struct _ptoc_FRAME_ResultType _ptoc_LOCAL;
    int _ResultType;
    SymbolTableRange typ1;
    SymbolTableRange typ2;
    PRIMTYPE t1;
    PRIMTYPE t2;
    SymbolTableRange typ;
    int ptoc_Var1;
    
    if (exp1 != -1 && exp2 != -1)
        {
        typ1 = field[exp1 + 1];
        typ2 = field[exp2 + 1];
        t1 = STab[typ1].SType;
        t2 = STab[typ2].SType;
        switch (t1)
            {
            case -8 :
            case -9 :
            case -1 :
            case -2 :
            case -5 :
                switch (t2)
                    {
                    case -8 :
                    case -9 :
                    case -1 :
                    case -2 :
                    case -5 :
                        _ResultType = IntegerPtr;
                        break;
                    case -4 :
                    case -3 :
                        _ResultType = LongPtr;
                        break;
                    case -11 :
                        _ResultType = FloatPtr;
                        break;
                    case -12 :
                        if (((ptoc_Var1 = field[exp2] % 16384) >= 0 ? ptoc_Var1 : ptoc_Var1 + 16384) == 12)
                            {
                            _ResultType = FloatPtr;
                            }
                        else
                            {
                            _ResultType = DoublePtr;
                            }
                        break;
                    default :
                        ErrorMsg(0 , col1);
                        ErrorMsg(0 , col2);
                        _ResultType = -21;
                        break;
                    }
                break;
            case -11 :
                switch (t2)
                    {
                    case -8 :
                    case -9 :
                    case -1 :
                    case -2 :
                    case -5 :
                    case -4 :
                    case -3 :
                    case -11 :
                        _ResultType = FloatPtr;
                        break;
                    case -12 :
                        if (((ptoc_Var1 = field[exp2] % 16384) >= 0 ? ptoc_Var1 : ptoc_Var1 + 16384) == 12)
                            {
                            _ResultType = FloatPtr;
                            }
                        else
                            {
                            _ResultType = DoublePtr;
                            }
                        break;
                    default :
                        ErrorMsg(0 , col1);
                        ErrorMsg(0 , col2);
                        _ResultType = -21;
                        break;
                    }
                break;
            case -4 :
            case -3 :
                switch (t2)
                    {
                    case -8 :
                    case -9 :
                    case -1 :
                    case -2 :
                    case -5 :
                    case -4 :
                    case -3 :
                    case -11 :
                        _ResultType = LongPtr;
                        break;
                    case -12 :
                        if (((ptoc_Var1 = field[exp2] % 16384) >= 0 ? ptoc_Var1 : ptoc_Var1 + 16384) == 12)
                            {
                            _ResultType = LongPtr;
                            }
                        else
                            {
                            _ResultType = DoublePtr;
                            }
                        break;
                    default :
                        ErrorMsg(0 , col1);
                        ErrorMsg(0 , col2);
                        _ResultType = -21;
                        break;
                    }
                break;
            case -12 :
                switch (t2)
                    {
                    case -8 :
                    case -9 :
                    case -1 :
                    case -2 :
                    case -5 :
                    case -4 :
                    case -3 :
                    case -11 :
                        if (((ptoc_Var1 = field[exp1] % 16384) >= 0 ? ptoc_Var1 : ptoc_Var1 + 16384) == 12)
                            {
                            _ResultType = FloatPtr;
                            }
                        else
                            {
                            _ResultType = DoublePtr;
                            }
                        break;
                    case -12 :
                        _ResultType = DoublePtr;
                        break;
                    default :
                        ErrorMsg(0 , col1);
                        ErrorMsg(0 , col2);
                        _ResultType = -21;
                        break;
                    }
                break;
            case -7 :
            case -24 :
            case -10 :
                if (typ1 == typ2)
                    {
                    _ResultType = typ1;
                    }
                else
                    {
                    ErrorMsg(0 , col1);
                    ErrorMsg(0 , col2);
                    _ResultType = -21;
                    }
                break;
            case -13 :
            case -20 :
                if (typ2 == typ1)
                    {
                    _ResultType = typ1;
                    }
                else
                    {
                    if (IsCharArray(typ1) && IsCharArray(typ2))
                        {
                        if (typ1 == PacPtr)
                            {
                            _ResultType = typ2;
                            }
                        else
                            {
                            if (typ2 == PacPtr)
                                {
                                _ResultType = typ1;
                                }
                            else
                                {
                                if (LengthOfCharArray(typ1) == LengthOfCharArray(typ2))
                                    {
                                    _ResultType = typ1;
                                    }
                                else
                                    {
                                    ErrorMsg(0 , col1);
                                    ErrorMsg(0 , col2);
                                    _ResultType = -21;
                                    }
                                }
                            }
                        }
                    else
                        {
                        if (IsString(typ1))
                            {
                            if (typ2 == PacPtr || IsString(typ2))
                                {
                                _ResultType = typ1;
                                }
                            else
                                {
                                ErrorMsg(0 , col1);
                                ErrorMsg(0 , col2);
                                _ResultType = -21;
                                }
                            }
                        else
                            {
                            if (IsString(typ2))
                                {
                                if (typ1 == PacPtr || IsString(typ1))
                                    {
                                    _ResultType = typ2;
                                    }
                                else
                                    {
                                    ErrorMsg(0 , col1);
                                    ErrorMsg(0 , col2);
                                    _ResultType = -21;
                                    }
                                }
                            else
                                {
                                ErrorMsg(1 , col2);
                                _ResultType = -21;
                                }
                            }
                        }
                    }
                break;
            case -14 :
                if (typ2 == typ1)
                    {
                    _ResultType = typ1;
                    }
                else
                    {
                    ErrorMsg(1 , col2);
                    _ResultType = -21;
                    }
                break;
            case -16 :
                if (t2 == -16)
                    {
                    typ = SetResultType(typ1 , typ2);
                    if (typ != -21)
                        {
                        _ResultType = typ;
                        }
                    else
                        {
                        ErrorMsg(0 , col2);
                        _ResultType = -21;
                        }
                    }
                else
                    {
                    ErrorMsg(0 , col2);
                    _ResultType = -21;
                    }
                break;
            case -17 :
                if (typ1 == typ2)
                    {
                    _ResultType = typ1;
                    }
                else
                    {
                    if (typ1 == AnyPtrPtr)
                        {
                        _ResultType = typ2;
                        }
                    else
                        {
                        if (typ2 == AnyPtrPtr)
                            {
                            _ResultType = typ1;
                            }
                        else
                            {
                            ErrorMsg(0 , col2);
                            _ResultType = -21;
                            }
                        }
                    }
                break;
            default :
                ErrorMsg(1 , col1);
                _ResultType = -21;
                break;
            }
        }
    else
        {
        _ResultType = -21;
        }
    return(_ResultType);
    }

struct _ptoc_FRAME_ChkOpType
  {
  char dummy;
  } ;


int ChkOpType(op, typ, p)
int op;
SymbolTableRange typ;
ParseStackRange p;
    {
    struct _ptoc_FRAME_ChkOpType _ptoc_LOCAL;
    int _ChkOpType;
    PRIMTYPE t;
    
    t = STab[typ].SType;
    switch (op)
        {
        case 19 :
        case 20 :
        case 21 :
            switch (t)
                {
                case -5 :
                case -4 :
                case -3 :
                case -11 :
                case -12 :
                case -16 :
                    _ChkOpType = t;
                    break;
                default :
                    ErrorMsg(1 , ColNo[p]);
                    _ChkOpType = -21;
                    break;
                }
            break;
        case 24 :
            switch (t)
                {
                case -5 :
                case -4 :
                case -3 :
                case -11 :
                case -12 :
                    _ChkOpType = t;
                    break;
                default :
                    ErrorMsg(1 , ColNo[p]);
                    _ChkOpType = -21;
                    break;
                }
            break;
        case 22 :
        case 104 :
            switch (t)
                {
                case -4 :
                case -3 :
                case -5 :
                    _ChkOpType = t;
                    break;
                default :
                    ErrorMsg(1 , ColNo[p]);
                    _ChkOpType = -21;
                    break;
                }
            break;
        case 31 :
        case 36 :
            switch (t)
                {
                case -5 :
                case -4 :
                case -3 :
                case -11 :
                case -12 :
                case -7 :
                case -24 :
                case -10 :
                case -13 :
                case -14 :
                case -16 :
                case -17 :
                case -20 :
                    _ChkOpType = t;
                    break;
                default :
                    ErrorMsg(1 , ColNo[p]);
                    _ChkOpType = -21;
                    break;
                }
            break;
        case 32 :
        case 33 :
        case 34 :
        case 35 :
            switch (t)
                {
                case -5 :
                case -4 :
                case -3 :
                case -11 :
                case -12 :
                case -7 :
                case -24 :
                case -10 :
                case -13 :
                case -16 :
                case -20 :
                    _ChkOpType = t;
                    break;
                default :
                    ErrorMsg(1 , ColNo[p]);
                    _ChkOpType = -21;
                    break;
                }
            break;
        case 27 :
        case 25 :
        case 28 :
            switch (t)
                {
                case -7 :
                    _ChkOpType = t;
                    break;
                case -4 :
                case -3 :
                case -5 :
                    _ChkOpType = -22;
                    break;
                default :
                    ErrorMsg(1 , ColNo[p]);
                    _ChkOpType = -21;
                    break;
                }
            break;
        case 123 :
        case 124 :
            switch (t)
                {
                case -20 :
                    _ChkOpType = t;
                    break;
                default :
                    ErrorMsg(1 , ColNo[p]);
                    _ChkOpType = -21;
                    break;
                }
            break;
        default :
            ErrorMsg(27 , -1);
            _ChkOpType = -21;
            break;
        }
    return(_ChkOpType);
    }

struct _ptoc_FRAME_AssgnmntCompatible
  {
  char dummy;
  } ;


int AssgnmntCompatible(typ1, exp2)
SymbolTableRange typ1;
SymbolTableRange exp2;
    {
    struct _ptoc_FRAME_AssgnmntCompatible _ptoc_LOCAL;
    int _AssgnmntCompatible;
    PRIMTYPE t1;
    PRIMTYPE t2;
    SymbolTableRange typ2;
    SymbolTableRange base1;
    SymbolTableRange base2;
    int ptoc_Var2;
    
    if (typ1 == -21 || exp2 == -1)
        {
        _AssgnmntCompatible = 0;
        }
    else
        {
        typ2 = field[exp2 + 1];
        if (typ1 == typ2)
            {
            _AssgnmntCompatible = 1;
            }
        else
            {
            t1 = STab[typ1].SType;
            t2 = STab[typ2].SType;
            switch (t1)
                {
                case -8 :
                case -9 :
                case -1 :
                case -2 :
                case -5 :
                    switch (t2)
                        {
                        case -8 :
                        case -9 :
                        case -1 :
                        case -2 :
                        case -4 :
                        case -3 :
                        case -5 :
                            _AssgnmntCompatible = 1;
                            break;
                        default :
                            _AssgnmntCompatible = 0;
                            break;
                        }
                    break;
                case -11 :
                case -4 :
                case -3 :
                case -12 :
                    switch (t2)
                        {
                        case -8 :
                        case -9 :
                        case -1 :
                        case -2 :
                        case -5 :
                        case -4 :
                        case -3 :
                        case -11 :
                        case -12 :
                            _AssgnmntCompatible = 1;
                            break;
                        default :
                            _AssgnmntCompatible = 0;
                            break;
                        }
                    break;
                case -13 :
                    if (IsPac(typ1))
                        {
                        _AssgnmntCompatible = IsPac(typ2) && LengthOfCharArray(typ1) == LengthOfCharArray(typ2) || IsIntegral(PrimType(STab[typ1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex)) && typ2 == PacPtr && ((ptoc_Var2 = field[exp2] % 16384) >= 0 ? ptoc_Var2 : ptoc_Var2 + 16384) == 12 && (control[121] || LengthOfCharArray(typ1) == _GetStringLength(field[exp2 + 5])) || IsIntegral(PrimType(STab[typ1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex)) && typ2 == CharPtr && IsConstantExpr(exp2) && (control[121] || LengthOfCharArray(typ1) == 1);
                        }
                    else
                        {
                        _AssgnmntCompatible = IsCharArray(typ1) && (typ2 == PacPtr && ((ptoc_Var2 = field[exp2] % 16384) >= 0 ? ptoc_Var2 : ptoc_Var2 + 16384) == 12 && LengthOfCharArray(typ1) == _GetStringLength(field[exp2 + 5]) || typ2 == CharPtr && IsConstantExpr(exp2) && LengthOfCharArray(typ1) == 1 || IsCharArray(typ2) && LengthOfCharArray(typ1) == LengthOfCharArray(typ2));
                        }
                    break;
                case -20 :
                    _AssgnmntCompatible = t2 == -20 || typ2 == PacPtr || typ2 == CharPtr && IsConstantExpr(exp2);
                    break;
                case -16 :
                    _AssgnmntCompatible = typ2 == AnySetPtr || t2 == -16 && CompatibleTypes(PrimType(STab[typ1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase) , PrimType(STab[typ2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase));
                    break;
                case -17 :
                    _AssgnmntCompatible = typ2 == AnyPtrPtr;
                    break;
                default :
                    _AssgnmntCompatible = 0;
                    break;
                }
            }
        }
    return(_AssgnmntCompatible);
    }

struct _ptoc_FRAME_ChkPtrOpType
  {
  char dummy;
  } ;


int ChkPtrOpType(op, exp1, exp2)
int op;
HdgRange exp1;
HdgRange exp2;
    {
    struct _ptoc_FRAME_ChkPtrOpType _ptoc_LOCAL;
    int _ChkPtrOpType;
    SymbolTableRange typ1;
    SymbolTableRange typ2;
    
    if (exp1 != -1 && exp2 != -1)
        {
        typ1 = field[exp1 + 1];
        typ2 = field[exp2 + 1];
        switch (op)
            {
            case 20 :
                if (IsPointer(typ1) && typ1 == typ2 && typ1 != AnyPtrPtr)
                    {
                    _ChkPtrOpType = IntegerPtr;
                    }
                else
                    {
                    if (IsPointer(typ1) && IsIntegral(typ2) && typ1 != AnyPtrPtr)
                        {
                        _ChkPtrOpType = typ1;
                        }
                    else
                        {
                        _ChkPtrOpType = -21;
                        }
                    }
                break;
            case 19 :
                if (IsPointer(typ1) && IsIntegral(typ2) && typ1 != AnyPtrPtr)
                    {
                    _ChkPtrOpType = typ1;
                    }
                else
                    {
                    if (IsPointer(typ2) && IsIntegral(typ1) && typ2 != AnyPtrPtr)
                        {
                        _ChkPtrOpType = typ2;
                        }
                    else
                        {
                        _ChkPtrOpType = -21;
                        }
                    }
                break;
            default :
                _ChkPtrOpType = -21;
                break;
            }
        }
    else
        {
        _ChkPtrOpType = -21;
        }
    return(_ChkPtrOpType);
    }

struct _ptoc_FRAME_EvalBinary
  {
  char dummy;
  } ;


int EvalBinary(p1, p2, op)
ParseStackRange p1;
ParseStackRange p2;
int op;
    {
    struct _ptoc_FRAME_EvalBinary _ptoc_LOCAL;
    SymbolTableRange ResTyp;
    PRIMTYPE typ;
    
    ResTyp = ChkPtrOpType(op , g1[p1] , g1[p2]);
    if (control[112] && ResTyp != -21)
        {
        g1[p1] = FoldBinary(op , ResTyp , g1[p1] , g1[p2]);
        }
    else
        {
        ResTyp = ResultType(g1[p1] , g1[p2] , ColNo[p1] , ColNo[p2]);
        if (ResTyp != -21)
            {
            typ = ChkOpType(op , ResTyp , p2);
            switch (typ)
                {
                case -5 :
                case -4 :
                case -3 :
                case -11 :
                case -12 :
                case -7 :
                    g1[p1] = FoldBinary(op , ResTyp , EvalConvert(ResTyp , g1[p1]) , EvalConvert(ResTyp , g1[p2]));
                    TypeType[p1] = ResTyp;
                    SubType[p1] = ResTyp;
                    break;
                case -22 :
                    switch (op)
                        {
                        case 27 :
                            op = 89;
                            break;
                        case 25 :
                            op = 90;
                            break;
                        }
                    g1[p1] = FoldBinary(op , ResTyp , EvalConvert(ResTyp , g1[p1]) , EvalConvert(ResTyp , g1[p2]));
                    TypeType[p1] = ResTyp;
                    SubType[p1] = ResTyp;
                    break;
                case -16 :
                    if (field[g1[p1] + 1] == FindIntset())
                        {
                        field[g1[p1] + 1] = ResTyp;
                        }
                    else
                        {
                        if (field[g1[p2] + 1] == FindIntset())
                            {
                            field[g1[p2] + 1] = ResTyp;
                            }
                        }
                    g1[p1] = BinaryExpression(op , ResTyp , g1[p1] , g1[p2]);
                    TypeType[p1] = ResTyp;
                    SubType[p1] = ResTyp;
                    break;
                case -21 :
                    g1[p1] = -1;
                    TypeType[p1] = -21;
                    SubType[p1] = -21;
                    break;
                default :
                    ErrorMsg(27 , -1);
                    g1[p1] = -1;
                    TypeType[p1] = -21;
                    SubType[p1] = -21;
                    break;
                }
            }
        else
            {
            g1[p1] = -1;
            TypeType[p1] = -21;
            SubType[p1] = -21;
            }
        }
    }

struct _ptoc_FRAME_EvalDivide
  {
  char dummy;
  } ;


int EvalDivide(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_EvalDivide _ptoc_LOCAL;
    SymbolTableRange ResTyp;
    PRIMTYPE typ;
    
    ResTyp = ResultType(g1[p1] , g1[p2] , ColNo[p1] , ColNo[p2]);
    if (ResTyp != -21)
        {
        typ = ChkOpType(21 , ResTyp , p2);
        if (ResTyp == IntegerPtr)
            {
            ResTyp = FloatPtr;
            }
        switch (typ)
            {
            case -5 :
            case -4 :
            case -3 :
            case -11 :
            case -12 :
                g1[p1] = FoldBinary(22 , ResTyp , EvalConvert(ResTyp , g1[p1]) , EvalConvert(ResTyp , g1[p2]));
                TypeType[p1] = ResTyp;
                SubType[p1] = ResTyp;
                break;
            case -21 :
                g1[p1] = -1;
                TypeType[p1] = -21;
                SubType[p1] = -21;
                break;
            default :
                ErrorMsg(27 , -1);
                g1[p1] = -1;
                TypeType[p1] = -21;
                SubType[p1] = -21;
                break;
            }
        }
    else
        {
        g1[p1] = -1;
        TypeType[p1] = -21;
        SubType[p1] = -21;
        }
    }

struct _ptoc_FRAME_TransformModulo
  {
  char dummy;
  } ;


int TransformModulo(ResTyp, p1, p2)
SymbolTableRange ResTyp;
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_TransformModulo _ptoc_LOCAL;
    int _TransformModulo;
    HdgRange tmp1;
    HdgRange tmp2;
    HdgRange tmp3;
    HdgRange op1;
    HdgRange op2;
    SymbolTableRange Temp1;
    SymbolTableRange Temp2;
    
    Temp2 = 0;
    if (NoSideEffects(g1[p2]))
        {
        op2 = EvalConvert(ResTyp , g1[p2]);
        tmp2 = CopyTree(op2);
        }
    else
        {
        Temp2 = GetTemporary(ProcIndex , ResTyp , 0);
        tmp2 = LongStatement(6 , ResTyp , node3(94 , ResTyp , Temp2 , 0) , EvalConvert(ResTyp , g1[p2]) , 0 , 0 , 0);
        op2 = FoldUnary(4 , ResTyp , node3(94 , ResTyp , Temp2 , 0));
        }
    op1 = EvalConvert(ResTyp , g1[p1]);
    Temp1 = GetTemporary(ProcIndex , ResTyp , 0);
    tmp2 = FoldBinary(88 , ResTyp , op1 , tmp2);
    tmp2 = LongStatement(6 , ResTyp , node3(94 , ResTyp , Temp1 , 0) , tmp2 , 0 , 0 , 0);
    op1 = FoldUnary(4 , ResTyp , node3(94 , ResTyp , Temp1 , 0));
    tmp1 = FoldBinary(35 , BooleanPtr , CopyTree(op2) , node3(12 , IntegerPtr , 0 , 0));
    tmp3 = FoldUnary(24 , ResTyp , CopyTree(op2));
    tmp1 = ComplexStmnt(8 , ResTyp , tmp1 , op2 , tmp3 , CardCount);
    tmp3 = FoldBinary(19 , ResTyp , CopyTree(op1) , CopyTree(tmp1));
    tmp1 = FoldBinary(35 , BooleanPtr , CopyTree(tmp2) , node3(12 , IntegerPtr , 0 , 0));
    tmp1 = ComplexStmnt(8 , ResTyp , tmp1 , op1 , tmp3 , CardCount);
    _TransformModulo = FoldTree(tmp1);
    if (Temp1 != 0)
        {
        FreeTemporary(Temp1);
        }
    if (Temp2 != 0)
        {
        FreeTemporary(Temp2);
        }
    return(_TransformModulo);
    }

struct _ptoc_FRAME_EvalModDiv
  {
  char dummy;
  } ;


int EvalModDiv(p1, p2, op)
ParseStackRange p1;
ParseStackRange p2;
int op;
    {
    struct _ptoc_FRAME_EvalModDiv _ptoc_LOCAL;
    SymbolTableRange ResTyp;
    PRIMTYPE typ;
    
    ResTyp = ResultType(g1[p1] , g1[p2] , ColNo[p1] , ColNo[p2]);
    if (ResTyp != -21)
        {
        typ = ChkOpType(op , ResTyp , p2);
        switch (typ)
            {
            case -4 :
            case -3 :
            case -5 :
                if (op == 104)
                    {
                    g1[p1] = TransformModulo(ResTyp , p1 , p2);
                    }
                else
                    {
                    g1[p1] = FoldBinary(op , ResTyp , EvalConvert(ResTyp , g1[p1]) , EvalConvert(ResTyp , g1[p2]));
                    }
                TypeType[p1] = ResTyp;
                SubType[p1] = ResTyp;
                break;
            case -11 :
            case -12 :
            case -21 :
                g1[p1] = -1;
                TypeType[p1] = -21;
                SubType[p1] = -21;
                break;
            default :
                ErrorMsg(27 , -1);
                g1[p1] = -1;
                TypeType[p1] = -21;
                SubType[p1] = -21;
                break;
            }
        }
    else
        {
        g1[p1] = -1;
        TypeType[p1] = -21;
        SubType[p1] = -21;
        }
    }

struct _ptoc_FRAME_EvalUnary
  {
  char dummy;
  } ;


int EvalUnary(p1, op)
ParseStackRange p1;
int op;
    {
    struct _ptoc_FRAME_EvalUnary _ptoc_LOCAL;
    SymbolTableRange ResTyp;
    PRIMTYPE typ;
    int ptoc_Var3;
    int ptoc_Var4;
    
    ptoc_Var3 = g1[p1];
    ptoc_Var4 = ColNo[p1];
    ResTyp = ResultType(ptoc_Var3 , ptoc_Var3 , ptoc_Var4 , ptoc_Var4);
    if (ResTyp != -21)
        {
        typ = ChkOpType(op , ResTyp , p1);
        if (op == 28)
            {
            if (typ == -7)
                {
                g1[p1] = demorgan(EvalConvert(ResTyp , g1[p1]));
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                }
            else
                {
                if (typ == -22)
                    {
                    g1[p1] = FoldUnary(92 , ResTyp , EvalConvert(ResTyp , g1[p1]));
                    if (IsConstant(g1[p1]))
                        {
                        NameofConst[p1] = GenConstName(field[g1[p1] + 5]);
                        }
                    }
                else
                    {
                    g1[p1] = -1;
                    TypeType[p1] = -21;
                    SubType[p1] = -21;
                    }
                }
            }
        else
            {
            switch (typ)
                {
                case -5 :
                case -4 :
                case -3 :
                case -11 :
                case -12 :
                    g1[p1] = FoldUnary(op , ResTyp , EvalConvert(ResTyp , g1[p1]));
                    if (IsConstant(g1[p1]) && op == 24)
                        {
                        NameofConst[p1] = GenConstName(field[g1[p1] + 5]);
                        }
                    break;
                case -21 :
                    g1[p1] = -1;
                    TypeType[p1] = -21;
                    SubType[p1] = -21;
                    break;
                default :
                    ErrorMsg(27 , -1);
                    g1[p1] = -1;
                    TypeType[p1] = -21;
                    SubType[p1] = -21;
                    break;
                }
            }
        }
    }

struct _ptoc_FRAME_OptimSetExp
  {
  char dummy;
  } ;


int OptimSetExp(Node, Result)
HdgRange Node;
HdgRange *Result;
    {
    struct _ptoc_FRAME_OptimSetExp _ptoc_LOCAL;
    int _OptimSetExp;
    HdgRange n1;
    HdgRange n2;
    SymbolTableRange id1;
    SymbolTableRange id2;
    int ptoc_Var5;
    
    n1 = -1;
    n2 = -1;
    id1 = 0;
    id2 = 0;
    _OptimSetExp = 0;
    if (shr(field[Node] , 16) == 6)
        {
        n1 = field[Node + 5];
        if (n1 != -1)
            {
            n2 = field[n1 + 3];
            if (n2 != -1)
                {
                if (field[n2 + 3] != -1)
                    {
                    n1 = -1;
                    }
                }
            }
        }
    if (n1 != -1 && n2 != -1)
        {
        if (((ptoc_Var5 = field[n2] % 16384) >= 0 ? ptoc_Var5 : ptoc_Var5 + 16384) == 13)
            {
            if (((ptoc_Var5 = field[field[n2 + 5]] % 16384) >= 0 ? ptoc_Var5 : ptoc_Var5 + 16384) == 94)
                {
                id2 = field[field[n2 + 5] + 5];
                }
            }
        if (((ptoc_Var5 = field[n1] % 16384) >= 0 ? ptoc_Var5 : ptoc_Var5 + 16384) == 6)
            {
            if (((ptoc_Var5 = field[field[n1 + 5]] % 16384) >= 0 ? ptoc_Var5 : ptoc_Var5 + 16384) == 94 && IsLval(field[n1 + 6]))
                {
                id1 = field[field[n1 + 5] + 5];
                }
            }
        if (id1 != 0 && id2 != 0 && id1 == id2)
            {
            _OptimSetExp = 1;
            *Result = field[n1 + 6];
            }
        }
    return(_OptimSetExp);
    }

struct _ptoc_FRAME_EvalSetExp
  {
  char dummy;
  } ;


int EvalSetExp(ResultIndex, ExprIndex)
HdgRange ResultIndex;
HdgRange ExprIndex;
    {
    struct _ptoc_FRAME_EvalSetExp _ptoc_LOCAL;
    int _EvalSetExp;
    SymbolTableRange ResultType;
    SymbolTableRange ExprType;
    SymbolTableRange TempTemp;
    SymbolTableRange LeftTemp;
    SymbolTableRange RightTemp;
    HdgRange HIndex;
    HdgRange TempIndex;
    HdgRange LeftIndex;
    HdgRange RightIndex;
    int SaveFreeSet;
    int ptoc_Var6;
    
    ResultType = field[ResultIndex + 1];
    ExprType = field[ExprIndex + 1];
    switch (shr(field[ExprIndex] , 16))
        {
        case 7 :
            if (((ptoc_Var6 = field[ExprIndex] % 16384) >= 0 ? ptoc_Var6 : ptoc_Var6 + 16384) == 40)
                {
                field[ExprIndex + 1] = AnyPtrPtr;
                _EvalSetExp = CallConvertt(ResultIndex , UnaryExpression(4 , ExprType , UnaryExpression(97 , ExprType , ExprIndex , 0 , 0) , 0 , 0));
                }
            else
                {
                ErrorMsg(27 , -1);
                _EvalSetExp = -1;
                }
            break;
        case 4 :
            if (((ptoc_Var6 = field[ExprIndex] % 16384) >= 0 ? ptoc_Var6 : ptoc_Var6 + 16384) == 4)
                {
                _EvalSetExp = CallConvertt(ResultIndex , ExprIndex);
                }
            else
                {
                ErrorMsg(27 , -1);
                _EvalSetExp = -1;
                }
            break;
        case 5 :
            SaveFreeSet = FreeSetTempFlag;
            FreeSetTempFlag = 0;
            if (ResultType == ExprType)
                {
                TempTemp = 0;
                TempIndex = ResultIndex;
                }
            else
                {
                if (STab[STab[ResultType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange < STab[STab[ExprType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange || STab[STab[ResultType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange > STab[STab[ExprType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange)
                    {
                    TempTemp = 0;
                    TempIndex = ResultIndex;
                    LeftTemp = GetTemporary(ProcIndex , ResultType , 0);
                    LeftIndex = UnaryExpression(4 , ResultType , UnaryExpression(97 , ResultType , EvalSetExp(node3(94 , ResultType , LeftTemp , 0) , field[ExprIndex + 5]) , 0 , 0) , 0 , 0);
                    RightTemp = GetTemporary(ProcIndex , ResultType , 0);
                    RightIndex = UnaryExpression(4 , ResultType , UnaryExpression(97 , ResultType , EvalSetExp(node3(94 , ResultType , RightTemp , 0) , field[ExprIndex + 6]) , 0 , 0) , 0 , 0);
                    TempIndex = CallSetOp((ptoc_Var6 = field[ExprIndex] % 16384) >= 0 ? ptoc_Var6 : ptoc_Var6 + 16384 , ResultType , TempIndex , LeftIndex , RightIndex);
                    }
                else
                    {
                    TempTemp = GetTemporary(ProcIndex , ExprType , 0);
                    TempIndex = node3(94 , ExprType , TempTemp , 0);
                    }
                }
            if (STab[STab[ResultType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange >= STab[STab[ExprType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange && STab[STab[ResultType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange <= STab[STab[ExprType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange)
                {
                if (ExprType == field[field[ExprIndex + 5] + 1] && IsLval(field[ExprIndex + 5]))
                    {
                    LeftTemp = 0;
                    LeftIndex = field[ExprIndex + 5];
                    }
                else
                    {
                    LeftTemp = GetTemporary(ProcIndex , ExprType , 0);
                    LeftIndex = EvalSetExp(node3(94 , ExprType , LeftTemp , 0) , field[ExprIndex + 5]);
                    if (OptimSetExp(LeftIndex , &HIndex))
                        {
                        LeftIndex = HIndex;
                        }
                    else
                        {
                        LeftIndex = UnaryExpression(4 , ExprType , UnaryExpression(97 , ExprType , LeftIndex , 0 , 0) , 0 , 0);
                        }
                    }
                if (ExprType == field[field[ExprIndex + 6] + 1] && IsLval(field[ExprIndex + 6]))
                    {
                    RightTemp = 0;
                    RightIndex = field[ExprIndex + 6];
                    }
                else
                    {
                    RightTemp = GetTemporary(ProcIndex , ExprType , 0);
                    RightIndex = EvalSetExp(node3(94 , ExprType , RightTemp , 0) , field[ExprIndex + 6]);
                    if (OptimSetExp(RightIndex , &HIndex))
                        {
                        RightIndex = HIndex;
                        }
                    else
                        {
                        RightIndex = UnaryExpression(4 , ExprType , UnaryExpression(97 , ExprType , RightIndex , 0 , 0) , 0 , 0);
                        }
                    }
                TempIndex = CallSetOp((ptoc_Var6 = field[ExprIndex] % 16384) >= 0 ? ptoc_Var6 : ptoc_Var6 + 16384 , ExprType , TempIndex , LeftIndex , RightIndex);
                }
            FreeSetTempFlag = SaveFreeSet;
            if (FreeSetTempFlag)
                {
                while (SetTempCount >= 0)
                    {
                    FreeTemporary(SetTempStack[SetTempCount]);
                    SetTempCount += -1;
                    }
                }
            if (TempTemp != 0)
                {
                TempIndex = CallConvertt(ResultIndex , UnaryExpression(4 , ExprType , UnaryExpression(97 , ExprType , TempIndex , 0 , 0) , 0 , 0));
                if (FreeSetTempFlag)
                    {
                    FreeTemporary(TempTemp);
                    }
                else
                    {
                    SetTempCount += 1;
                    SetTempStack[SetTempCount] = TempTemp;
                    }
                }
            if (LeftTemp != 0)
                {
                if (FreeSetTempFlag)
                    {
                    FreeTemporary(LeftTemp);
                    }
                else
                    {
                    SetTempCount += 1;
                    SetTempStack[SetTempCount] = LeftTemp;
                    }
                }
            if (RightTemp != 0)
                {
                if (FreeSetTempFlag)
                    {
                    FreeTemporary(RightTemp);
                    }
                else
                    {
                    SetTempCount += 1;
                    SetTempStack[SetTempCount] = RightTemp;
                    }
                }
            _EvalSetExp = TempIndex;
            break;
        case 6 :
            if (((ptoc_Var6 = field[ExprIndex] % 16384) >= 0 ? ptoc_Var6 : ptoc_Var6 + 16384) == 126)
                {
                _EvalSetExp = CallCttot(ResultType , ResultIndex , ExprIndex);
                }
            else
                {
                LeftTemp = GetTemporary(ProcIndex , 0 , 0);
                STab[SymPtr].SType = ResultType;
                STab[SymPtr].KINDTYPE.C_ModuleDcl.SClass = 4;
                STab[SymPtr].KINDTYPE.C_ModuleDcl.SSubClass = 4;
                process_initializer(LeftTemp , ExprIndex , CardCount);
                ExprIndex = FoldUnary(4 , ResultType , node3(94 , ResultType , LeftTemp , 0));
                _EvalSetExp = CallConvertt(ResultIndex , ExprIndex);
                }
            break;
        default :
            ErrorMsg(27 , -1);
            _EvalSetExp = -1;
            break;
        }
    return(_EvalSetExp);
    }

struct _ptoc_FRAME_EvalSetConstructor
  {
  char dummy;
  } ;


int EvalSetConstructor(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_EvalSetConstructor _ptoc_LOCAL;
    HdgRange pairList;
    HdgRange singList;
    HdgRange mainList;
    HdgRange index;
    HdgRange temp;
    int PairCnt;
    int SingCnt;
    SymbolTableRange SetPtr;
    HdgRange NodeIndex;
    int low;
    int lowlow;
    int high;
    int highhigh;
    int x;
    int ptoc_Var7;
    
    mainList = g1[p2];
    if (field[mainList + 5] == -1)
        {
        SetPtr = AnySetPtr;
        }
    else
        {
        if (IsIntegral(field[mainList + 1]))
            {
            SetPtr = FindIntset();
            if (((ptoc_Var7 = field[mainList] % 16384) >= 0 ? ptoc_Var7 : ptoc_Var7 + 16384) != 126)
                {
                NodeIndex = field[mainList + 5];
                lowlow = 2147483647;
                highhigh = -2147483647;
                while (NodeIndex != -1)
                    {
                    if (shr(field[NodeIndex] , 16) == 6)
                        {
                        index = field[NodeIndex + 5];
                        low = ConstExp(index);
                        index = field[index + 3];
                        high = ConstExp(index);
                        if (low <= high)
                            {
                            if (low < lowlow)
                                {
                                lowlow = low;
                                }
                            if (high > highhigh)
                                {
                                highhigh = high;
                                }
                            }
                        }
                    else
                        {
                        x = ConstExp(NodeIndex);
                        if (x > highhigh)
                            {
                            highhigh = x;
                            }
                        if (x < lowlow)
                            {
                            lowlow = x;
                            }
                        }
                    NodeIndex = field[NodeIndex + 3];
                    }
                if (lowlow < STab[SetPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange || highhigh > STab[SetPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange)
                    {
                    NewSymbol(0 , field[mainList + 1] , 7 , LevelNo , -1);
                    SetPtr = SymPtr;
                    STab[SetPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = lowlow;
                    STab[SetPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = highhigh;
                    TypeSize(SetPtr);
                    NewSymbol(0 , -16 , 7 , LevelNo , -1);
                    STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase = SetPtr;
                    STab[SymPtr].SFlags |= 4096;
                    TypeSize(SymPtr);
                    SetPtr = SymPtr;
                    }
                }
            }
        else
            {
            NewSymbol(0 , -16 , 7 , LevelNo , -1);
            STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase = field[mainList + 1];
            TypeSize(SymPtr);
            SetPtr = SymPtr;
            }
        }
    if (((ptoc_Var7 = field[mainList] % 16384) >= 0 ? ptoc_Var7 : ptoc_Var7 + 16384) == 126)
        {
        pairList = MakeList(6 , 0 , -1 , -1 , -1);
        singList = MakeList(6 , 0 , -1 , -1 , -1);
        PairCnt = 0;
        SingCnt = 0;
        index = ExtractList(mainList);
        while (index != -1)
            {
            if (shr(field[index] , 16) == 6 && ((ptoc_Var7 = field[index] % 16384) >= 0 ? ptoc_Var7 : ptoc_Var7 + 16384) != 114)
                {
                PairCnt += 1;
                temp = ExtractList(index);
                AppendList(6 , 0 , pairList , FoldUnary(8 , field[index + 1] , ExtractList(index)));
                AppendList(6 , 0 , pairList , FoldUnary(8 , field[temp + 1] , temp));
                }
            else
                {
                SingCnt += 1;
                AppendList(6 , 0 , singList , index);
                }
            index = ExtractList(mainList);
            }
        LinkLists(pairList , singList);
        InsertList(113 , 0 , pairList , FoldUnary(8 , IntegerPtr , node3(12 , IntegerPtr , SingCnt , 0)));
        InsertList(113 , 0 , pairList , FoldUnary(8 , IntegerPtr , node3(12 , IntegerPtr , PairCnt , 0)));
        SetMinor(pairList , 126);
        field[pairList + 1] = SetPtr;
        g1[p1] = pairList;
        TypeType[p1] = SetPtr;
        SubType[p1] = SetPtr;
        }
    else
        {
        field[mainList + 1] = SetPtr;
        g1[p1] = mainList;
        TypeType[p1] = SetPtr;
        SubType[p1] = SetPtr;
        }
    }

struct _ptoc_FRAME_EvalStringExp
  {
  char dummy;
  } ;


int EvalStringExp(ResultIndex, ExprIndex)
HdgRange ResultIndex;
HdgRange ExprIndex;
    {
    struct _ptoc_FRAME_EvalStringExp _ptoc_LOCAL;
    int _EvalStringExp;
    SymbolTableRange typ;
    SymbolTableRange LeftTemp;
    SymbolTableRange RightTemp;
    HdgRange NodeIndex;
    HdgRange LeftIndex;
    HdgRange RightIndex;
    int SaveFreeString;
    int ptoc_Var8;
    
    NodeIndex = ExprIndex;
    switch (field[NodeIndex] & 65535)
        {
        case 4 :
        case 12 :
            _EvalStringExp = CallStringCopy(ResultIndex , ExprIndex , node3(12 , IntegerPtr , 1 , 0) , node3(12 , IntegerPtr , 255 , 0));
            break;
        case 40 :
            typ = field[NodeIndex + 1];
            field[NodeIndex + 1] = AnyPtrPtr;
            _EvalStringExp = CallStringCopy(ResultIndex , UnaryExpression(4 , typ , UnaryExpression(97 , typ , NodeIndex , 0 , 0) , 0 , 0) , node3(12 , IntegerPtr , 1 , 0) , node3(12 , IntegerPtr , 255 , 0));
            break;
        case 123 :
            SaveFreeString = FreeStringTempFlag;
            FreeStringTempFlag = 0;
            if (IsConstant(field[NodeIndex + 5]))
                {
                LeftTemp = 0;
                LeftIndex = EvalConvert(StringPtr , field[NodeIndex + 5]);
                }
            else
                {
                if (IsLval(field[NodeIndex + 5]))
                    {
                    LeftTemp = 0;
                    LeftIndex = field[NodeIndex + 5];
                    }
                else
                    {
                    LeftTemp = GetTemporary(ProcIndex , field[NodeIndex + 1] , 0);
                    ptoc_Var8 = field[NodeIndex + 1];
                    LeftIndex = UnaryExpression(4 , ptoc_Var8 , UnaryExpression(97 , ptoc_Var8 , EvalStringExp(node3(94 , ptoc_Var8 , LeftTemp , 0) , field[NodeIndex + 5]) , 0 , 0) , 0 , 0);
                    }
                }
            if (IsConstant(field[NodeIndex + 6]))
                {
                RightTemp = 0;
                RightIndex = EvalConvert(StringPtr , field[NodeIndex + 6]);
                }
            else
                {
                if (IsLval(field[NodeIndex + 6]))
                    {
                    RightTemp = 0;
                    RightIndex = field[NodeIndex + 6];
                    }
                else
                    {
                    RightTemp = GetTemporary(ProcIndex , field[NodeIndex + 1] , 0);
                    ptoc_Var8 = field[NodeIndex + 1];
                    RightIndex = UnaryExpression(4 , ptoc_Var8 , UnaryExpression(97 , ptoc_Var8 , EvalStringExp(node3(94 , ptoc_Var8 , RightTemp , 0) , field[NodeIndex + 6]) , 0 , 0) , 0 , 0);
                    }
                }
            _EvalStringExp = CallStringConcat(ResultIndex , LeftIndex , RightIndex);
            FreeStringTempFlag = SaveFreeString;
            if (FreeStringTempFlag)
                {
                while (StringTempCount >= 0)
                    {
                    FreeTemporary(StringTempStack[StringTempCount]);
                    StringTempCount += -1;
                    }
                }
            if (LeftTemp != 0)
                {
                if (FreeStringTempFlag)
                    {
                    FreeTemporary(LeftTemp);
                    }
                else
                    {
                    StringTempCount += 1;
                    StringTempStack[StringTempCount] = LeftTemp;
                    }
                }
            if (RightTemp != 0)
                {
                if (FreeStringTempFlag)
                    {
                    FreeTemporary(RightTemp);
                    }
                else
                    {
                    StringTempCount += 1;
                    StringTempStack[StringTempCount] = RightTemp;
                    }
                }
            break;
        case 124 :
            SaveFreeString = FreeStringTempFlag;
            FreeStringTempFlag = 0;
            if (IsConstant(field[NodeIndex + 5]))
                {
                LeftTemp = 0;
                LeftIndex = EvalConvert(StringPtr , field[NodeIndex + 5]);
                }
            else
                {
                if (IsLval(field[NodeIndex + 5]))
                    {
                    LeftTemp = 0;
                    LeftIndex = field[NodeIndex + 5];
                    }
                else
                    {
                    LeftTemp = GetTemporary(ProcIndex , StringPtr , 0);
                    LeftIndex = UnaryExpression(4 , StringPtr , UnaryExpression(97 , StringPtr , EvalStringExp(node3(94 , field[NodeIndex + 1] , LeftTemp , 0) , field[NodeIndex + 5]) , 0 , 0) , 0 , 0);
                    }
                }
            _EvalStringExp = CallStringCopy(ResultIndex , LeftIndex , field[NodeIndex + 6] , field[NodeIndex + 7]);
            FreeStringTempFlag = SaveFreeString;
            if (FreeStringTempFlag)
                {
                while (StringTempCount >= 0)
                    {
                    FreeTemporary(StringTempStack[StringTempCount]);
                    StringTempCount += -1;
                    }
                }
            if (LeftTemp != 0)
                {
                if (FreeStringTempFlag)
                    {
                    FreeTemporary(LeftTemp);
                    }
                else
                    {
                    StringTempCount += 1;
                    StringTempStack[StringTempCount] = LeftTemp;
                    }
                }
            break;
        default :
            ErrorMsg(27 , -1);
            _EvalStringExp = -1;
            break;
        }
    return(_EvalStringExp);
    }

struct _ptoc_FRAME_EvalRelational
  {
  char dummy;
  } ;


int EvalRelational(p1, p2, op)
ParseStackRange p1;
ParseStackRange p2;
int op;
    {
    struct _ptoc_FRAME_EvalRelational _ptoc_LOCAL;
    SymbolTableRange ResTyp;
    SymbolTableRange LeftTemp;
    SymbolTableRange RightTemp;
    SymbolTableRange IndexTemp;
    PRIMTYPE typ;
    HdgRange NodeIndex;
    int leftlen;
    int rightlen;
    int SaveFreeSet;
    int SaveFreeString;
    HdgRange HIndex;
    HdgRange RightIndex;
    HdgRange LeftIndex;
    
    ResTyp = ResultType(g1[p1] , g1[p2] , ColNo[p1] , ColNo[p2]);
    if (ResTyp != -21)
        {
        typ = ChkOpType(op , ResTyp , p2);
        switch (typ)
            {
            case -5 :
            case -4 :
            case -3 :
            case -11 :
            case -12 :
            case -17 :
                g1[p1] = FoldBinary(op , BooleanPtr , EvalConvert(ResTyp , g1[p1]) , EvalConvert(ResTyp , g1[p2]));
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                break;
            case -10 :
            case -7 :
                if (control[113])
                    {
                    g1[p1] = FoldBinary(op , BooleanPtr , EvalConvert(IntegerPtr , g1[p1]) , EvalConvert(IntegerPtr , g1[p2]));
                    }
                else
                    {
                    g1[p1] = FoldBinary(op , BooleanPtr , EvalConvert(ResTyp , g1[p1]) , EvalConvert(ResTyp , g1[p2]));
                    }
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                break;
            case -24 :
                g1[p1] = FoldBinary(op , BooleanPtr , EvalConvert(IntegerPtr , g1[p1]) , EvalConvert(IntegerPtr , g1[p2]));
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                break;
            case -13 :
                if (TypeType[p1] == PacPtr)
                    {
                    if (TypeType[p2] == PacPtr)
                        {
                        NodeIndex = g1[p1];
                        leftlen = field[NodeIndex + 6];
                        NodeIndex = g1[p2];
                        rightlen = field[NodeIndex + 6];
                        if (leftlen != rightlen)
                            {
                            WarnStrCmp(CardNum[p1]);
                            }
                        NewSymbol(0 , IntegerPtr , 7 , STab[ProcIndex].SLevel + 1 , 0);
                        STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 1;
                        if (leftlen > rightlen)
                            {
                            STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = leftlen;
                            }
                        else
                            {
                            STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = rightlen;
                            }
                        TypeSize(SymPtr);
                        NewSymbol(0 , -13 , 7 , STab[ProcIndex].SLevel + 1 , 0);
                        STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex = SymPtr + -1;
                        STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = CharPtr;
                        TypeSize(SymPtr);
                        ResTyp = SymPtr;
                        NodeIndex = g1[p1];
                        field[NodeIndex + 1] = ResTyp;
                        NodeIndex = g1[p2];
                        field[NodeIndex + 1] = ResTyp;
                        }
                    else
                        {
                        NodeIndex = g1[p1];
                        field[NodeIndex + 1] = TypeType[p2];
                        leftlen = field[NodeIndex + 6];
                        rightlen = LengthOfCharArray(field[g1[p2] + 1]);
                        if (leftlen != rightlen)
                            {
                            WarnStrCmp(CardNum[p1]);
                            }
                        }
                    }
                else
                    {
                    if (TypeType[p2] == PacPtr)
                        {
                        NodeIndex = g1[p2];
                        field[NodeIndex + 1] = TypeType[p1];
                        rightlen = field[NodeIndex + 6];
                        leftlen = LengthOfCharArray(field[g1[p1] + 1]);
                        if (leftlen != rightlen)
                            {
                            WarnStrCmp(CardNum[p1]);
                            }
                        }
                    }
                leftlen = LengthOfCharArray(field[g1[p1] + 1]);
                rightlen = LengthOfCharArray(field[g1[p2] + 1]);
                if (leftlen != rightlen)
                    {
                    WarnStrCmp(CardNum[p1]);
                    }
                g1[p1] = BinaryExpression(op , BooleanPtr , EvalConvert(ResTyp , g1[p1]) , EvalConvert(ResTyp , g1[p2]));
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                break;
            case -14 :
                g1[p1] = BinaryExpression(op , BooleanPtr , CallCompare(p1 , p2) , node3(12 , IntegerPtr , 0 , 0));
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                break;
            case -16 :
                SaveFreeSet = FreeSetTempFlag;
                FreeSetTempFlag = 0;
                if ((ResTyp == field[g1[p1] + 1] || STab[ResTyp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase == STab[field[g1[p1] + 1]].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase) && IsLval(g1[p1]))
                    {
                    LeftTemp = 0;
                    }
                else
                    {
                    LeftTemp = GetTemporary(ProcIndex , ResTyp , 0);
                    LeftIndex = EvalSetExp(node3(94 , ResTyp , LeftTemp , 0) , g1[p1]);
                    if (OptimSetExp(LeftIndex , &HIndex))
                        {
                        g1[p1] = HIndex;
                        }
                    else
                        {
                        g1[p1] = UnaryExpression(4 , ResTyp , UnaryExpression(97 , ResTyp , LeftIndex , 0 , 0) , 0 , 0);
                        }
                    }
                if ((ResTyp == field[g1[p2] + 1] || STab[ResTyp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase == STab[field[g1[p2] + 1]].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase) && IsLval(g1[p2]))
                    {
                    RightTemp = 0;
                    }
                else
                    {
                    RightTemp = GetTemporary(ProcIndex , ResTyp , 0);
                    RightIndex = EvalSetExp(node3(94 , ResTyp , RightTemp , 0) , g1[p2]);
                    if (OptimSetExp(RightIndex , &HIndex))
                        {
                        g1[p2] = HIndex;
                        }
                    else
                        {
                        g1[p2] = UnaryExpression(4 , ResTyp , UnaryExpression(97 , ResTyp , RightIndex , 0 , 0) , 0 , 0);
                        }
                    }
                g1[p1] = CallSetRel(op , ResTyp , g1[p1] , g1[p2]);
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                FreeSetTempFlag = SaveFreeSet;
                if (FreeSetTempFlag)
                    {
                    while (SetTempCount >= 0)
                        {
                        FreeTemporary(SetTempStack[SetTempCount]);
                        SetTempCount += -1;
                        }
                    }
                if (LeftTemp != 0)
                    {
                    if (FreeSetTempFlag)
                        {
                        FreeTemporary(LeftTemp);
                        }
                    else
                        {
                        SetTempCount += 1;
                        SetTempStack[SetTempCount] = LeftTemp;
                        }
                    }
                if (RightTemp != 0)
                    {
                    if (FreeSetTempFlag)
                        {
                        FreeTemporary(RightTemp);
                        }
                    else
                        {
                        SetTempCount += 1;
                        SetTempStack[SetTempCount] = RightTemp;
                        }
                    }
                break;
            case -20 :
                SaveFreeString = FreeStringTempFlag;
                FreeStringTempFlag = 0;
                if (IsConstant(g1[p1]))
                    {
                    LeftTemp = 0;
                    g1[p1] = EvalConvert(StringPtr , g1[p1]);
                    }
                else
                    {
                    if (IsLval(g1[p1]))
                        {
                        LeftTemp = 0;
                        }
                    else
                        {
                        LeftTemp = GetTemporary(ProcIndex , ResTyp , 0);
                        g1[p1] = UnaryExpression(4 , ResTyp , UnaryExpression(97 , ResTyp , EvalStringExp(node3(94 , ResTyp , LeftTemp , 0) , g1[p1]) , 0 , 0) , 0 , 0);
                        }
                    }
                if (IsConstant(g1[p2]))
                    {
                    RightTemp = 0;
                    g1[p2] = EvalConvert(StringPtr , g1[p2]);
                    }
                else
                    {
                    if (IsLval(g1[p2]))
                        {
                        RightTemp = 0;
                        }
                    else
                        {
                        RightTemp = GetTemporary(ProcIndex , ResTyp , 0);
                        g1[p2] = UnaryExpression(4 , ResTyp , UnaryExpression(97 , ResTyp , EvalStringExp(node3(94 , ResTyp , RightTemp , 0) , g1[p2]) , 0 , 0) , 0 , 0);
                        }
                    }
                g1[p1] = BinaryExpression(op , BooleanPtr , CallStringCompare(g1[p1] , g1[p2]) , node3(12 , IntegerPtr , 0 , 0));
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                FreeStringTempFlag = SaveFreeString;
                if (FreeStringTempFlag)
                    {
                    while (StringTempCount >= 0)
                        {
                        FreeTemporary(StringTempStack[StringTempCount]);
                        StringTempCount += -1;
                        }
                    }
                if (LeftTemp != 0)
                    {
                    if (FreeStringTempFlag)
                        {
                        FreeTemporary(LeftTemp);
                        }
                    else
                        {
                        StringTempCount += 1;
                        StringTempStack[StringTempCount] = LeftTemp;
                        }
                    }
                if (RightTemp != 0)
                    {
                    if (FreeStringTempFlag)
                        {
                        FreeTemporary(RightTemp);
                        }
                    else
                        {
                        StringTempCount += 1;
                        StringTempStack[StringTempCount] = RightTemp;
                        }
                    }
                break;
            case -21 :
                g1[p1] = -1;
                TypeType[p1] = -21;
                SubType[p1] = -21;
                break;
            default :
                ErrorMsg(27 , -1);
                g1[p1] = -1;
                TypeType[p1] = -21;
                SubType[p1] = -21;
                break;
            }
        }
    else
        {
        g1[p1] = -1;
        TypeType[p1] = -21;
        SubType[p1] = -21;
        }
    }

struct _ptoc_FRAME_EvalIn
  {
  char dummy;
  } ;


int EvalIn(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_EvalIn _ptoc_LOCAL;
    SymbolTableRange ResTyp;
    SymbolTableRange RightType;
    SymbolTableRange RightTemp;
    HdgRange element;
    HdgRange NodeIndex;
    
    RightType = TypeType[p2];
    if (IsSet(RightType))
        {
        if (CompatibleTypes(TypeType[p1] , PrimType(STab[RightType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase)))
            {
            element = EvalConvert(IntegerPtr , g1[p1]);
            NodeIndex = g1[p2];
            if (shr(field[NodeIndex] , 16) == 6 && (field[NodeIndex] & 65535) == 126)
                {
                g1[p1] = CallInct(element , NodeIndex);
                TypeType[p1] = BooleanPtr;
                SubType[p1] = BooleanPtr;
                }
            else
                {
                if (shr(field[NodeIndex] , 16) == 6 && (field[NodeIndex] & 65535) == 113 && field[NodeIndex + 7] <= 32)
                    {
                    g1[p1] = CallIntor(element , NodeIndex);
                    TypeType[p1] = BooleanPtr;
                    SubType[p1] = BooleanPtr;
                    }
                else
                    {
                    if (IsLval(NodeIndex))
                        {
                        RightTemp = 0;
                        }
                    else
                        {
                        if (shr(field[NodeIndex] , 16) == 6 && (field[NodeIndex] & 65535) == 113)
                            {
                            RightTemp = GetTemporary(ProcIndex , 0 , 0);
                            STab[SymPtr].SType = RightType;
                            STab[SymPtr].KINDTYPE.C_ModuleDcl.SClass = 4;
                            STab[SymPtr].KINDTYPE.C_ModuleDcl.SSubClass = 4;
                            process_initializer(RightTemp , NodeIndex , CardCount);
                            NodeIndex = FoldUnary(4 , RightType , node3(94 , RightType , RightTemp , 0));
                            RightTemp = 0;
                            }
                        else
                            {
                            RightTemp = GetTemporary(ProcIndex , RightType , 0);
                            NodeIndex = UnaryExpression(4 , RightType , UnaryExpression(97 , RightType , EvalSetExp(node3(94 , RightType , RightTemp , 0) , NodeIndex) , 0 , 0) , 0 , 0);
                            }
                        }
                    g1[p1] = CallInt(element , RightType , NodeIndex);
                    TypeType[p1] = BooleanPtr;
                    SubType[p1] = BooleanPtr;
                    if (RightTemp != 0)
                        {
                        FreeTemporary(RightTemp);
                        }
                    }
                }
            }
        else
            {
            ErrorMsg(0 , ColNo[p2]);
            g1[p1] = -1;
            TypeType[p1] = -21;
            SubType[p1] = -21;
            }
        }
    else
        {
        ErrorMsg(1 , ColNo[p2]);
        g1[p1] = -1;
        TypeType[p1] = -21;
        SubType[p1] = -21;
        }
    }

struct _ptoc_FRAME_RangeCheck
  {
  char dummy;
  } ;


int RangeCheck(typ, NodeIndex)
SymbolTableRange typ;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_RangeCheck _ptoc_LOCAL;
    int _RangeCheck;
    int exp_val;
    int low_lim;
    int high_lim;
    
    if (typ != IntegerPtr && IsOrdinal(PrimType(typ)))
        {
        if (!IsConstantExpr(NodeIndex))
            {
            if (IsCallRange(NodeIndex))
                {
                _RangeCheck = NodeIndex;
                }
            else
                {
                _RangeCheck = EvalConvert(field[NodeIndex + 1] , CallRange(NodeIndex , STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange , STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange));
                }
            }
        else
            {
            low_lim = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
            high_lim = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange;
            exp_val = ConstExp(NodeIndex);
            if (exp_val < low_lim || exp_val > high_lim)
                {
                ErrorMsg(146 , -1);
                _RangeCheck = -1;
                }
            else
                {
                _RangeCheck = EvalConvert(field[NodeIndex + 1] , NodeIndex);
                }
            }
        }
    else
        {
        _RangeCheck = NodeIndex;
        }
    return(_RangeCheck);
    }

struct _ptoc_FRAME_GenStore
  {
  char dummy;
  } ;


int GenStore(lhs, ltype, lstype, lcol, rhs, rtype, rstype, rcol, card)
HdgRange lhs;
SymbolTableRange ltype;
SymbolTableRange lstype;
int lcol;
HdgRange rhs;
SymbolTableRange rtype;
SymbolTableRange rstype;
int rcol;
int card;
    {
    struct _ptoc_FRAME_GenStore _ptoc_LOCAL;
    int _GenStore;
    HdgRange NodeIndex;
    
    if (AssgnmntCompatible(ltype , rhs))
        {
        if (IsString(ltype))
            {
            rhs = EvalConvert(ltype , rhs);
            _GenStore = EvalStringExp(lhs , rhs);
            }
        else
            {
            if (IsSet(ltype) && (!IsLval(rhs) || ltype != rtype))
                {
                _GenStore = EvalSetExp(lhs , rhs);
                }
            else
                {
                if (IsFile(ltype) && IsFile(rtype))
                    {
                    ErrorMsg(39 , rcol);
                    _GenStore = -1;
                    }
                else
                    {
                    if (FileInRec(ltype))
                        {
                        ErrorMsg(136 , rcol);
                        }
                    else
                        {
                        if (IsCharArray(ltype))
                            {
                            NodeIndex = rhs;
                            if ((field[NodeIndex] & 65535) == 12 && field[NodeIndex + 6] > LengthOfCharArray(ltype))
                                {
                                ErrorMsg(38 , rcol);
                                }
                            }
                        }
                    rhs = EvalConvert(ltype , rhs);
                    if (control[114])
                        {
                        rhs = RangeCheck(lstype , rhs);
                        }
                    NodeIndex = lhs;
                    if ((field[NodeIndex] & 65535) == 94 && (STab[field[NodeIndex + 5]].SFlags & 2048) != 0)
                        {
                        ErrorMsg(142 , lcol);
                        _GenStore = -1;
                        }
                    else
                        {
                        _GenStore = LongStatement(6 , ltype , lhs , rhs , 0 , 0 , card);
                        }
                    }
                }
            }
        }
    else
        {
        ErrorMsg(0 , rcol);
        _GenStore = -1;
        }
    return(_GenStore);
    }

struct _ptoc_FRAME_RefParm
  {
  char dummy;
  } ;


int RefParm(ParmInx, actual, actual_list)
SymbolTableRange ParmInx;
ExpListRange actual;
HdgRange actual_list;
    {
    struct _ptoc_FRAME_RefParm _ptoc_LOCAL;
    int _RefParm;
    HdgRange NodeIndex;
    SymbolTableRange atyp;
    SymbolTableRange astyp;
    SymbolTableRange fstyp;
    int len;
    struct ptoc_Type1 *ptoc_Var9;
    struct ptoc_Type1 *ptoc_Var10;
    
    atyp = ExpType[actual];
    astyp = ExpStype[actual];
    fstyp = STab[ParmInx].SType;
    NodeIndex = ExpVal[actual];
    if (shr(field[NodeIndex] , 16) == 4 && (field[NodeIndex] & 65535) == 4)
        {
        NodeIndex = field[NodeIndex + 5];
        }
    if (shr(field[NodeIndex] , 16) == 1 && (field[NodeIndex] & 65535) == 94 && (STab[field[NodeIndex + 5]].SFlags & 2048) != 0)
        {
        ErrorMsg(145 , ExpColNo[actual]);
        }
    if (fstyp == astyp || fstyp == AnyFilePtr && IsFile(atyp))
        {
        NodeIndex = ExpVal[actual];
        if ((field[NodeIndex] & 65535) == 4)
            {
            SetMinor(NodeIndex , 9);
            SaveIndex = NodeIndex;
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 94)
                {
                ptoc_Var9 = &STab[field[NodeIndex + 5]];
                if (ptoc_Var9->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var9->KINDTYPE.C_ModuleDcl.SClass == 5)
                    {
                    ptoc_Var9->KINDTYPE.C_ModuleDcl.SClass = 10;
                    }
                }
            NodeIndex = SaveIndex;
            AppendList(6 , IntegerPtr , actual_list , NodeIndex);
            _RefParm = 0;
            }
        else
            {
            if (IsCallRange(NodeIndex))
                {
                NodeIndex = SkipCallRange(NodeIndex);
                SetMinor(NodeIndex , 9);
                AppendList(6 , IntegerPtr , actual_list , NodeIndex);
                _RefParm = 0;
                }
            else
                {
                ErrorMsg(9 , ExpColNo[actual]);
                _RefParm = 0;
                }
            }
        }
    else
        {
        if ((fstyp == AnyPacPtr || fstyp == AnyPacLenPtr) && (atyp == CharPtr || IsCharArray(atyp)))
            {
            NodeIndex = ExpVal[actual];
            if ((field[NodeIndex] & 65535) == 4)
                {
                if (atyp == CharPtr)
                    {
                    len = 1;
                    }
                else
                    {
                    len = LengthOfCharArray(atyp);
                    }
                SetMinor(NodeIndex , 9);
                SaveIndex = NodeIndex;
                NodeIndex = field[NodeIndex + 5];
                if ((field[NodeIndex] & 65535) == 94)
                    {
                    ptoc_Var10 = &STab[field[NodeIndex + 5]];
                    if (ptoc_Var10->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var10->KINDTYPE.C_ModuleDcl.SClass == 5)
                        {
                        ptoc_Var10->KINDTYPE.C_ModuleDcl.SClass = 10;
                        }
                    }
                NodeIndex = SaveIndex;
                AppendList(6 , IntegerPtr , actual_list , NodeIndex);
                if (fstyp == AnyPacLenPtr)
                    {
                    AppendList(6 , IntegerPtr , actual_list , node3(12 , IntegerPtr , len , 0));
                    }
                _RefParm = 0;
                }
            else
                {
                if ((field[NodeIndex] & 65535) == 12)
                    {
                    if (atyp == CharPtr)
                        {
                        CharToPac(NodeIndex);
                        len = 1;
                        }
                    else
                        {
                        len = field[NodeIndex + 6];
                        }
                    SetMinor(NodeIndex , 14);
                    field[NodeIndex + 1] = PtrToCharPtr;
                    AppendList(6 , IntegerPtr , actual_list , NodeIndex);
                    if (fstyp == AnyPacLenPtr)
                        {
                        AppendList(6 , IntegerPtr , actual_list , node3(12 , IntegerPtr , len , 0));
                        }
                    _RefParm = 0;
                    }
                else
                    {
                    ErrorMsg(9 , ExpColNo[actual]);
                    _RefParm = 0;
                    }
                }
            }
        else
            {
            ErrorMsg(9 , ExpColNo[actual]);
            _RefParm = 0;
            }
        }
    return(_RefParm);
    }

struct _ptoc_FRAME_ValueParm
  {
  char dummy;
  } ;


int ValueParm(ParmInx, actual, actual_list)
SymbolTableRange ParmInx;
ExpListRange actual;
HdgRange actual_list;
    {
    struct _ptoc_FRAME_ValueParm _ptoc_LOCAL;
    int _ValueParm;
    SymbolTableRange FormalType;
    SymbolTableRange ActualType;
    SymbolTableRange temp;
    HdgRange NodeIndex;
    HdgRange ActualExp;
    int ptoc_Var11;
    
    FormalType = PrimType(STab[ParmInx].SType);
    ActualType = ExpType[actual];
    ActualExp = ExpVal[actual];
    if (ActualExp != -1 && ((ptoc_Var11 = field[ActualExp] % 16384) >= 0 ? ptoc_Var11 : ptoc_Var11 + 16384) == 40)
        {
        CheckCall(ActualExp , ExpColNo[actual]);
        }
    if (AssgnmntCompatible(FormalType , ActualExp))
        {
        if (IsString(ActualType))
            {
            temp = GetTemporary(ProcIndex , FormalType , 0);
            NodeIndex = UnaryExpression(97 , FormalType , EvalStringExp(node3(94 , FormalType , temp , 0) , EvalConvert(FormalType , ActualExp)) , 0 , 0);
            NodeIndex = UnaryExpression(8 , FormalType , NodeIndex , 0 , 0);
            }
        else
            {
            if (IsSet(ActualType) && (!IsLval(ActualExp) || ActualType != FormalType))
                {
                temp = GetTemporary(ProcIndex , FormalType , 0);
                NodeIndex = UnaryExpression(97 , FormalType , EvalSetExp(node3(94 , FormalType , temp , 0) , ActualExp) , 0 , 0);
                NodeIndex = UnaryExpression(8 , FormalType , NodeIndex , 0 , 0);
                }
            else
                {
                temp = 0;
                NodeIndex = EvalConvert(FormalType , ActualExp);
                if (control[114])
                    {
                    NodeIndex = RangeCheck(STab[ParmInx].SType , NodeIndex);
                    }
                NodeIndex = UnaryExpression(8 , FormalType , NodeIndex , 0 , 0);
                }
            }
        AppendList(6 , IntegerPtr , actual_list , NodeIndex);
        _ValueParm = temp;
        }
    else
        {
        ErrorMsg(9 , ExpColNo[actual]);
        _ValueParm = 0;
        }
    return(_ValueParm);
    }

struct _ptoc_FRAME_ProcParm
  {
  char dummy;
  } ;


int ProcParm(ParmInx, actual, actual_list)
SymbolTableRange ParmInx;
HdgRange actual;
HdgRange actual_list;
    {
    struct _ptoc_FRAME_ProcParm _ptoc_LOCAL;
    SymbolTableRange formal_type;
    HdgRange proc_address;
    HdgRange static_link;
    HdgRange NodeIndex;
    int ptoc_Var12;
    
    formal_type = PrimType(STab[ParmInx].SType);
    NodeIndex = actual;
    if ((field[NodeIndex] & 65535) == 40 && field[NodeIndex + 6] == -1)
        {
        NodeIndex = field[NodeIndex + 5];
        if ((field[NodeIndex] & 65535) == 94)
            {
            proc_address = UnaryExpression(13 , formal_type , NodeIndex , 0 , 0);
            static_link = node3(109 , AnyPtrPtr , field[NodeIndex + 5] , 0);
            }
        else
            {
            NodeIndex = field[NodeIndex + 5];
            NodeIndex = field[NodeIndex + 5];
            proc_address = UnaryExpression(4 , formal_type , NodeIndex , 0 , 0);
            static_link = UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , field[NodeIndex + 5] + 1 , 0) , 0 , 0);
            if (STab[field[NodeIndex + 5]].SLevel <= STab[ProcIndex].SLevel)
                {
                ptoc_Var12 = field[NodeIndex + 5];
                STab[ptoc_Var12].SFeld4 = 1;
                STab[ptoc_Var12 + 1].SFeld4 = 1;
                }
            }
        AppendList(6 , IntegerPtr , actual_list , UnaryExpression(8 , formal_type , proc_address , 0 , 0));
        if (control[120])
            {
            AppendList(6 , IntegerPtr , actual_list , UnaryExpression(8 , IntegerPtr , static_link , 0 , 0));
            }
        }
    else
        {
        ErrorMsg(9 , -1);
        }
    }

struct _ptoc_FRAME_FetchParm
  {
  char dummy;
  } ;


int FetchParm(ParmInx, actual, actual_list)
SymbolTableRange ParmInx;
ExpListRange actual;
HdgRange actual_list;
    {
    struct _ptoc_FRAME_FetchParm _ptoc_LOCAL;
    int _FetchParm;
    
    if (STab[ParmInx].KINDTYPE.C_ModuleDcl.SSubClass == 1)
        {
        _FetchParm = ValueParm(ParmInx , actual , actual_list);
        }
    else
        {
        _FetchParm = RefParm(ParmInx , actual , actual_list);
        }
    return(_FetchParm);
    }

struct _ptoc_FRAME_congruent
  {
  char dummy;
  } ;


int congruent(actual, formali)
SymbolTableRange actual;
SymbolTableRange formali;
    {
    struct _ptoc_FRAME_congruent _ptoc_LOCAL;
    int _congruent;
    SymbolTableRange act;
    SymbolTableRange frm;
    
    if (STab[actual].SKind == 20 || STab[actual].SKind == 19)
        {
        act = STab[actual].SType;
        frm = STab[formali].SType;
        if (STab[act].SType == STab[frm].SType)
            {
            act = STab[act].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain;
            frm = STab[frm].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain;
            _congruent = STab[act].SType == STab[frm].SType && STab[act].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult == STab[frm].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
            }
        else
            {
            _congruent = 0;
            }
        }
    else
        {
        _congruent = STab[actual].KINDTYPE.C_ModuleDcl.SSubClass == STab[formali].KINDTYPE.C_ModuleDcl.SSubClass && STab[actual].SType == STab[formali].SType;
        }
    return(_congruent);
    }

struct _ptoc_FRAME_GenParameters
  {
  char dummy;
  } ;


int GenParameters(symp, p, return_cnt, actual_list)
SymbolTableRange symp;
ParseStackRange p;
int *return_cnt;
HdgRange *actual_list;
    {
    struct _ptoc_FRAME_GenParameters _ptoc_LOCAL;
    int i;
    int formal_cnt;
    int actual_cnt;
    int first;
    int last;
    SymbolTableRange k;
    HdgRange head;
    HdgRange NodeIndex;
    int j;
    SymbolTableRange act_type;
    SymbolTableRange procparm_type;
    int ok;
    int actprocparcnt;
    int ptoc_Var13;
    int ptoc_Var14;
    
    if (p == -1)
        {
        actual_cnt = 0;
        }
    else
        {
        actual_cnt = cnt[p];
        }
    formal_cnt = STab[symp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
    *return_cnt = formal_cnt;
    if (actual_cnt <= formal_cnt)
        {
        if (actual_cnt == 0)
            {
            *actual_list = -1;
            }
        else
            {
            first = StartCnt[p];
            last = first + actual_cnt + -1;
            head = MakeList(6 , 0 , -1 , -1 , -1);
            i = first;
            k = STab[symp].SNext;
            while (i <= last && formal_cnt > 0)
                if (STab[k].SKind == 14)
                    {
                    ExpType[i] = FetchParm(k , i , head);
                    i += 1;
                    if (STab[k].SType == AnyPacLenPtr)
                        {
                        k = STab[k].SNext;
                        k = STab[k].SNext;
                        formal_cnt += -2;
                        }
                    else
                        {
                        k = STab[k].SNext;
                        formal_cnt += -1;
                        }
                    }
                else
                    {
                    procparm_type = STab[STab[k].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain;
                    NodeIndex = ExpVal[i];
                    NodeIndex = field[NodeIndex + 5];
                    if (NodeIndex != -1)
                        {
                        act_type = field[NodeIndex + 1];
                        if (STab[act_type].SType != STab[procparm_type].SType)
                            {
                            if (STab[procparm_type].SType == -23)
                                {
                                ErrorMsg(26 , ExpColNo[i]);
                                }
                            else
                                {
                                ErrorMsg(34 , ExpColNo[i]);
                                }
                            }
                        actprocparcnt = STab[act_type].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
                        if (actprocparcnt != STab[procparm_type].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt)
                            {
                            ErrorMsg(8 , ExpColNo[i]);
                            }
                        if (STab[act_type].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult != STab[procparm_type].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult)
                            {
                            ErrorMsg(9 , ExpColNo[i]);
                            }
                        ok = 1;
                        j = 0;
                        while (ok && j < actprocparcnt)
                            {
                            act_type = STab[act_type].SNext;
                            procparm_type = STab[procparm_type].SNext;
                            j += 1;
                            ok = congruent(act_type , procparm_type);
                            }
                        if (!ok)
                            {
                            ErrorMsg(9 , ExpColNo[i]);
                            }
                        ProcParm(k , ExpVal[i] , head);
                        ExpType[i] = 0;
                        i += 1;
                        k = STab[k].SNext;
                        if (control[120])
                            {
                            k = STab[k].SNext;
                            formal_cnt += -2;
                            }
                        else
                            {
                            formal_cnt += -1;
                            }
                        }
                    else
                        {
                        ExpType[i] = 0;
                        i += 1;
                        k = STab[k].SNext;
                        if (control[120])
                            {
                            k = STab[k].SNext;
                            formal_cnt += -2;
                            }
                        else
                            {
                            formal_cnt += -1;
                            }
                        }
                    }
            if (i == last + 1 && formal_cnt == 0)
                {
                ptoc_Var13 = first;
                ptoc_Var14 = last;
                if (ptoc_Var13 <= ptoc_Var14)
                    {
                    i = ptoc_Var13;
                    do
                        {
                        if (ExpType[i] != 0)
                            {
                            FreeTemporary(ExpType[i]);
                            }
                        if (i == ptoc_Var14)
                            {
                            break;
                            }
                        i += 1;
                        }
                    while (!0);
                    }
                *actual_list = head;
                }
            else
                {
                ErrorMsg(8 , ColNo[mp]);
                *return_cnt = 0;
                *actual_list = -1;
                }
            }
        }
    else
        {
        ErrorMsg(8 , ColNo[mp]);
        *return_cnt = 0;
        *actual_list = -1;
        }
    }

struct _ptoc_FRAME_GenCall
  {
  char dummy;
  } ;


int GenCall(symp, p1, p2)
SymbolTableRange symp;
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_GenCall _ptoc_LOCAL;
    SymbolTableRange t;
    SymbolTableRange t1;
    HdgRange actual_list;
    int actual_count;
    
    switch (STab[symp].SKind)
        {
        case 3 :
        case 5 :
            t1 = STab[symp].SType;
            GenParameters(t1 , p2 , &actual_count , &actual_list);
            g1[p1] = LongStatement(40 , IntegerPtr , node3(94 , t1 , symp , 0) , actual_list , node3(109 , AnyPtrPtr , symp , 0) , actual_count , CardNum[p1]);
            break;
        case 4 :
        case 6 :
            t1 = STab[symp].SType;
            GenParameters(t1 , p2 , &actual_count , &actual_list);
            g1[p1] = LongStatement(40 , TypeType[p1] , node3(94 , t1 , symp , 0) , actual_list , node3(109 , AnyPtrPtr , symp , 0) , actual_count , CardNum[p1]);
            break;
        case 20 :
            t = PrimType(STab[symp].SType);
            t1 = PrimType(STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain);
            GenParameters(t1 , p2 , &actual_count , &actual_list);
            g1[p1] = LongStatement(40 , IntegerPtr , UnaryExpression(97 , t1 , UnaryExpression(4 , t , node3(94 , t , symp , 0) , 0 , 0) , 0 , 0) , actual_list , UnaryExpression(4 , AnyPtrPtr , node3(94 , AnyPtrPtr , symp + 1 , 0) , 0 , 0) , actual_count , CardNum[p1]);
            if (STab[symp].SLevel <= STab[ProcIndex].SLevel)
                {
                STab[symp].SFeld4 = 1;
                STab[symp + 1].SFeld4 = 1;
                }
            break;
        case 19 :
            t = PrimType(STab[symp].SType);
            t1 = PrimType(STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain);
            GenParameters(t1 , p2 , &actual_count , &actual_list);
            g1[p1] = LongStatement(40 , TypeType[p1] , UnaryExpression(97 , t1 , UnaryExpression(4 , t , node3(94 , t , symp , 0) , 0 , 0) , 0 , 0) , actual_list , UnaryExpression(4 , AnyPtrPtr , node3(94 , AnyPtrPtr , symp + 1 , 0) , 0 , 0) , actual_count , CardNum[p1]);
            if (STab[symp].SLevel <= STab[ProcIndex].SLevel)
                {
                STab[symp].SFeld4 = 1;
                STab[symp + 1].SFeld4 = 1;
                }
            break;
        }
    }

struct _ptoc_FRAME_GenAddress
  {
  char dummy;
  } ;


int GenAddress(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_GenAddress _ptoc_LOCAL;
    HdgRange NodeIndex;
    SymbolTableRange typ;
    int ptoc_Var15;
    struct ptoc_Type1 *ptoc_Var16;
    
    NodeIndex = g1[p2];
    if (NodeIndex != -1)
        {
        switch (field[NodeIndex] & 65535)
            {
            case 4 :
                if (shr(field[field[NodeIndex + 5]] , 16) == 1 && ((ptoc_Var15 = field[field[NodeIndex + 5]] % 16384) >= 0 ? ptoc_Var15 : ptoc_Var15 + 16384) == 94 && STab[field[field[NodeIndex + 5] + 5]].SKind == 16)
                    {
                    ErrorMsg(1 , ColNo[p2]);
                    }
                else
                    {
                    typ = AnyPtrPtr;
                    SetMinor(NodeIndex , 13);
                    field[NodeIndex + 1] = typ;
                    SaveIndex = NodeIndex;
                    NodeIndex = field[NodeIndex + 5];
                    if ((field[NodeIndex] & 65535) == 94)
                        {
                        ptoc_Var16 = &STab[field[NodeIndex + 5]];
                        if (ptoc_Var16->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var16->KINDTYPE.C_ModuleDcl.SClass == 5)
                            {
                            ptoc_Var16->KINDTYPE.C_ModuleDcl.SClass = 10;
                            }
                        }
                    NodeIndex = SaveIndex;
                    }
                break;
            case 93 :
            case 62 :
                if (IsCallRange(NodeIndex))
                    {
                    NodeIndex = SkipCallRange(NodeIndex);
                    SetMinor(NodeIndex , 13);
                    typ = AnyPtrPtr;
                    field[NodeIndex + 1] = typ;
                    }
                else
                    {
                    ErrorMsg(28 , ColNo[p2]);
                    typ = -21;
                    NodeIndex = -1;
                    }
                break;
            case 40 :
                NodeIndex = field[NodeIndex + 5];
                NodeIndex = UnaryExpression(13 , AnyPtrPtr , NodeIndex , 0 , 0);
                typ = AnyPtrPtr;
                break;
            case 12 :
                typ = TypeType[p2];
                if (typ == CharPtr)
                    {
                    CharToPac(NodeIndex);
                    typ = PacPtr;
                    }
                if (IsCharArray(typ))
                    {
                    typ = AnyPtrPtr;
                    SetMinor(NodeIndex , 14);
                    field[NodeIndex + 1] = typ;
                    }
                else
                    {
                    ErrorMsg(28 , ColNo[p2]);
                    typ = -21;
                    NodeIndex = -1;
                    }
                break;
            default :
                ErrorMsg(28 , ColNo[p2]);
                typ = -21;
                NodeIndex = -1;
                break;
            }
        g1[p1] = NodeIndex;
        TypeType[p1] = typ;
        SubType[p1] = typ;
        }
    else
        {
        g1[p1] = -1;
        TypeType[p1] = -21;
        SubType[p1] = -21;
        }
    }
int UseSymbolicConst();

struct _ptoc_FRAME_AccessVariable
  {
  char dummy;
  } ;


int AccessVariable(symp, p1, p2)
SymbolTableRange symp;
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_AccessVariable _ptoc_LOCAL;
    struct ptoc_Type1 *ptoc_Var17;
    struct ptoc_Type1 *ptoc_Var18;
    
    if (symp == -1)
        {
        g1[p1] = -1;
        TypeType[p1] = IntegerPtr;
        SubType[p1] = IntegerPtr;
        }
    else
        {
        switch (STab[symp].SKind)
            {
            case 8 :
                if (UseSymbolicConst() && IsOrdinal(PrimType(STab[symp].SType)))
                    {
                    g1[p1] = node3(94 , TypeType[p1] , symp , 0);
                    }
                else
                    {
                    g1[p1] = node3(12 , TypeType[p1] , STab[symp].KINDTYPE.C_ConDcl.SCon1val , STab[symp].KINDTYPE.C_ConDcl.SCon2val);
                    }
                NameofConst[p1] = GenConDclName(symp);
                break;
            case 9 :
            case 14 :
            case 16 :
                ptoc_Var17 = &STab[symp];
                if (ptoc_Var17->SLevel <= STab[ProcIndex].SLevel && (ptoc_Var17->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var17->KINDTYPE.C_ModuleDcl.SClass == 5))
                    {
                    ptoc_Var17->KINDTYPE.C_ModuleDcl.SClass = 10;
                    }
                ptoc_Var18 = &STab[symp];
                if (ptoc_Var18->SLevel <= STab[ProcIndex].SLevel && (ptoc_Var18->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var18->KINDTYPE.C_ModuleDcl.SClass == 10 || ptoc_Var18->KINDTYPE.C_ModuleDcl.SClass == 5 || ptoc_Var18->KINDTYPE.C_ModuleDcl.SClass == 7))
                    {
                    ptoc_Var18->SFeld4 = 1;
                    }
                g1[p1] = node3(94 , TypeType[p1] , symp , 0);
                break;
            case 11 :
                g1[p1] = BinaryExpression(96 , TypeType[p1] , CopyTree(WithRecs[STab[symp].SLevel]) , node3(70 , IntegerPtr , symp , 0));
                break;
            default :
                ErrorMsg(18 , ColNo[p1]);
                break;
            }
        }
    }

struct _ptoc_FRAME_IndexedVar
  {
  char dummy;
  } ;


int IndexedVar(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_IndexedVar _ptoc_LOCAL;
    SymbolTableRange typ;
    SymbolTableRange ityp;
    SymbolTableRange etyp;
    int upper;
    int lower;
    HdgRange itree;
    HdgRange NodeIndex;
    KINDTYPE kind;
    
    if (val[p1] == -1)
        {
        ErrorMsg(21 , ColNo[p1]);
        StackError(p1);
        goto LAB_100;
        }
    kind = STab[val[p1]].SKind;
    if (kind == 8)
        {
        ErrorMsg(100 , ColNo[p1]);
        StackError(p1);
        goto LAB_100;
        }
    typ = TypeType[p1];
    if (STab[typ].SType == -13)
        {
        ityp = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
        upper = STab[ityp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange;
        lower = STab[ityp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
        ityp = PrimType(ityp);
        etyp = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement;
        }
    else
        {
        if (STab[typ].SType == -20)
            {
            upper = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SLength + -1;
            lower = 0;
            ityp = IntegerPtr;
            etyp = CharPtr;
            }
        else
            {
            ErrorMsg(21 , ColNo[p1]);
            StackError(p1);
            goto LAB_100;
            }
        }
    if (AssgnmntCompatible(ityp , g1[p2]))
        {
        itree = EvalConvert(IntegerPtr , g1[p2]);
        }
    else
        {
        ErrorMsg(11 , ColNo[p2]);
        StackError(p1);
        goto LAB_100;
        }
    if (control[114])
        {
        itree = RangeCheck(STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex , itree);
        }
    SubType[p1] = etyp;
    TypeType[p1] = PrimType(etyp);
    NodeIndex = g1[p1];
    NodeIndex = BinaryExpression(95 , etyp , g1[p1] , itree);
    field[NodeIndex + 8] = -lower;
    FoldSubscript(NodeIndex);
    g1[p1] = NodeIndex;
    LAB_100 :
    ;
    }

struct _ptoc_FRAME_EvalWith
  {
  char dummy;
  } ;


int EvalWith(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_EvalWith _ptoc_LOCAL;
    SymbolTableRange typ;
    SymbolTableRange tmptyp;
    SymbolTableRange tmp;
    HdgRange NodeIndex;
    
    typ = TypeType[p2];
    if (STab[typ].SType != -14)
        {
        ErrorMsg(19 , ColNo[p2]);
        }
    else
        {
        EntSymbol(0 , -17 , 7 , -1);
        STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = typ;
        tmptyp = SymPtr;
        tmp = GetTemporary(ProcIndex , tmptyp , 0);
        NodeIndex = LongStatement(6 , IntegerPtr , node3(94 , tmptyp , tmp , 0) , UnaryExpression(13 , tmptyp , g1[p2] , 0 , 0) , 0 , 0 , CardNum[p2]);
        upscope(-1);
        WithRecs[LevelNo] = UnaryExpression(97 , typ , UnaryExpression(4 , tmptyp , node3(94 , tmptyp , tmp , 0) , 0 , 0) , 0 , 0);
        val[p1] += 1;
        g1[p1] = LinkChain(g1[p1] , NodeIndex);
        }
    }

struct _ptoc_FRAME_EndWith
  {
  char dummy;
  } ;


int EndWith(p1)
int p1;
    {
    struct _ptoc_FRAME_EndWith _ptoc_LOCAL;
    int i;
    int count;
    HdgRange NodeIndex;
    int ptoc_Var19;
    
    count = val[p1];
    ptoc_Var19 = count;
    if (1 <= ptoc_Var19)
        {
        i = 1;
        do
            {
            NodeIndex = WithRecs[LevelNo];
            NodeIndex = field[NodeIndex + 5];
            NodeIndex = field[NodeIndex + 5];
            FreeTemporary(field[NodeIndex + 5]);
            PopScope(-1);
            if (i == ptoc_Var19)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    ClearLabel();
    }

struct _ptoc_FRAME_QualifiedVar
  {
  char dummy;
  } ;


int QualifiedVar(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_QualifiedVar _ptoc_LOCAL;
    int i;
    
    i = QidLookup(p1 , p2);
    if (i != -1)
        {
        g1[p1] = BinaryExpression(96 , SubType[p2] , g1[p1] , node3(70 , IntegerPtr , i , 0));
        TypeType[p1] = TypeType[p2];
        SubType[p1] = SubType[p2];
        }
    }

struct _ptoc_FRAME_RefVar
  {
  char dummy;
  } ;


int RefVar(p1)
int p1;
    {
    struct _ptoc_FRAME_RefVar _ptoc_LOCAL;
    
    ErrorMsg(27 , -1);
    }

struct _ptoc_FRAME_DerefVar
  {
  char dummy;
  } ;


int DerefVar(p1)
int p1;
    {
    struct _ptoc_FRAME_DerefVar _ptoc_LOCAL;
    SymbolTableRange t;
    SymbolTableRange typ;
    HdgRange NodeIndex;
    struct ptoc_Type1 *ptoc_Var20;
    
    typ = TypeType[p1];
    if (STab[typ].SType == -17)
        {
        if (control[114])
            {
            g1[p1] = CallNilCheck(p1);
            }
        t = STab[STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain].SType;
        g1[p1] = UnaryExpression(97 , t , g1[p1] , 0 , 0);
        }
    else
        {
        if (STab[typ].SType == -15)
            {
            t = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent;
            SetMinor(g1[p1] , 9);
            NodeIndex = g1[p1];
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 94)
                {
                ptoc_Var20 = &STab[field[NodeIndex + 5]];
                if (ptoc_Var20->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var20->KINDTYPE.C_ModuleDcl.SClass == 5)
                    {
                    ptoc_Var20->KINDTYPE.C_ModuleDcl.SClass = 10;
                    }
                }
            g1[p1] = CallFnil(g1[p1] , PrimType(t));
            }
        else
            {
            ErrorMsg(29 , ColNo[p1]);
            t = -21;
            g1[p1] = -1;
            }
        }
    SubType[p1] = t;
    TypeType[p1] = PrimType(t);
    }

struct _ptoc_FRAME_CheckScalar
  {
  char dummy;
  } ;


int CheckScalar(p1)
int p1;
    {
    struct _ptoc_FRAME_CheckScalar _ptoc_LOCAL;
    
    switch (STab[TypeType[p1]].SType)
        {
        case -4 :
        case -3 :
        case -7 :
        case -5 :
        case -24 :
        case -10 :
            break;
        default :
            ErrorMsg(1 , ColNo[p1]);
            break;
        }
    }

struct _ptoc_FRAME_BuildCaseConst
  {
  char dummy;
  } ;


int BuildCaseConst(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_BuildCaseConst _ptoc_LOCAL;
    
    CheckScalar(p1);
    if (p2 != p1)
        {
        CheckScalar(p2);
        if (val[p1] > val[p2])
            {
            ErrorMsg(130 , ColNo[p2]);
            }
        if (SubType[p1] != SubType[p2])
            {
            ErrorMsg(1 , ColNo[p2]);
            }
        }
    g2[p1] = g1[p2];
    }

struct _ptoc_FRAME_BuildCase
  {
  int j;
  int t;
  char dummy;
  } ;


struct _ptoc_FRAME_search_BuildCase
  {
  struct _ptoc_FRAME_BuildCase *_ptoc_LINK;
  char dummy;
  } ;


static
int search_BuildCase(_ptoc_LINK, low, high, p, h)
struct _ptoc_FRAME_BuildCase *_ptoc_LINK;
int low;
int high;
int *p;
int *h;
    {
    struct _ptoc_FRAME_search_BuildCase _ptoc_LOCAL;
    int p1;
    int p2;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (*p == -1)
        {
        *p = _ptoc_LOCAL._ptoc_LINK->t;
        *h = 1;
        field[*p + 5] = low;
        field[*p + 6] = high;
        field[*p + 7] = -1;
        field[*p + 8] = -1;
        field[*p + 10] = 0;
        }
    else
        {
        if (high < field[*p + 5] || high >= field[*p + 5] && low < field[*p + 5])
            {
            if (high >= field[*p + 5])
                {
                ErrorMsg(131 , ExpColNo[_ptoc_LOCAL._ptoc_LINK->j + 1]);
                }
            search_BuildCase(_ptoc_LOCAL._ptoc_LINK, low , high , &field[*p + 7] , &(*h));
            if (*h)
                {
                switch (field[*p + 10])
                    {
                    case 1 :
                        field[*p + 10] = 0;
                        *h = 0;
                        break;
                    case 0 :
                        field[*p + 10] = -1;
                        break;
                    case -1 :
                        p1 = field[*p + 7];
                        if (field[p1 + 10] == -1)
                            {
                            field[*p + 7] = field[p1 + 8];
                            field[p1 + 8] = *p;
                            field[*p + 10] = 0;
                            *p = p1;
                            }
                        else
                            {
                            p2 = field[p1 + 8];
                            field[p1 + 8] = field[p2 + 7];
                            field[p2 + 7] = p1;
                            field[*p + 7] = field[p2 + 8];
                            field[p2 + 8] = *p;
                            if (field[p2 + 10] == -1)
                                {
                                field[*p + 10] = 1;
                                }
                            else
                                {
                                field[*p + 10] = 0;
                                }
                            if (field[p2 + 10] == 1)
                                {
                                field[p1 + 10] = -1;
                                }
                            else
                                {
                                field[p1 + 10] = 0;
                                }
                            *p = p2;
                            }
                        field[*p + 10] = 0;
                        *h = 0;
                        break;
                    }
                }
            }
        else
            {
            if (low > field[*p + 6] || low <= field[*p + 6] && high > field[*p + 6])
                {
                if (low <= field[*p + 6])
                    {
                    ErrorMsg(131 , ExpColNo[_ptoc_LOCAL._ptoc_LINK->j + 1]);
                    }
                search_BuildCase(_ptoc_LOCAL._ptoc_LINK, low , high , &field[*p + 8] , &(*h));
                if (*h)
                    {
                    switch (field[*p + 10])
                        {
                        case -1 :
                            field[*p + 10] = 0;
                            *h = 0;
                            break;
                        case 0 :
                            field[*p + 10] = 1;
                            break;
                        case 1 :
                            p1 = field[*p + 8];
                            if (field[p1 + 10] == 1)
                                {
                                field[*p + 8] = field[p1 + 7];
                                field[p1 + 7] = *p;
                                field[*p + 10] = 0;
                                *p = p1;
                                }
                            else
                                {
                                p2 = field[p1 + 7];
                                field[p1 + 7] = field[p2 + 8];
                                field[p2 + 8] = p1;
                                field[*p + 8] = field[p2 + 7];
                                field[p2 + 7] = *p;
                                if (field[p2 + 10] == 1)
                                    {
                                    field[*p + 10] = -1;
                                    }
                                else
                                    {
                                    field[*p + 10] = 0;
                                    }
                                if (field[p2 + 10] == -1)
                                    {
                                    field[p1 + 10] = 1;
                                    }
                                else
                                    {
                                    field[p1 + 10] = 0;
                                    }
                                *p = p2;
                                }
                            field[*p + 10] = 0;
                            *h = 0;
                            break;
                        }
                    }
                }
            else
                {
                if (low < field[*p + 5] && high > field[*p + 6])
                    {
                    p1 = *p;
                    *p = _ptoc_LOCAL._ptoc_LINK->t;
                    field[*p + 5] = low;
                    field[*p + 6] = high;
                    field[*p + 7] = field[p1 + 7];
                    field[*p + 8] = field[p1 + 8];
                    field[*p + 10] = field[p1 + 10];
                    ErrorMsg(131 , ExpColNo[_ptoc_LOCAL._ptoc_LINK->j + 1]);
                    }
                ErrorMsg(131 , ExpColNo[_ptoc_LOCAL._ptoc_LINK->j + 1]);
                *h = 0;
                }
            }
        }
    }

int BuildCase(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_BuildCase _ptoc_LOCAL;
    int _BuildCase;
    int i;
    int TempCase;
    int flag;
    int ptoc_Var21;
    
    TempCase = -1;
    _ptoc_LOCAL.j = StartCnt[p1];
    ptoc_Var21 = cnt[p1];
    if (1 <= ptoc_Var21)
        {
        i = 1;
        do
            {
            _ptoc_LOCAL.t = CaseStmnt(11 , 99 , IntegerPtr , ConstExp(ExpVal[_ptoc_LOCAL.j]) , ConstExp(ExpVal[_ptoc_LOCAL.j + 1]) , CardNum[p1]);
            flag = 0;
            search_BuildCase(&_ptoc_LOCAL, field[_ptoc_LOCAL.t + 5] , field[_ptoc_LOCAL.t + 6] , &CaseRoot , &flag);
            TempCase = LinkChain(TempCase , _ptoc_LOCAL.t);
            _ptoc_LOCAL.j += 2;
            if (i == ptoc_Var21)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    TempCase = LinkChain(TempCase , g1[p2]);
    _ptoc_LOCAL.t = special(101 , 0 , 0 , CardNum[p1]);
    TempCase = LinkChain(TempCase , _ptoc_LOCAL.t);
    _BuildCase = TempCase;
    return(_BuildCase);
    }

struct _ptoc_FRAME_BuildDefault
  {
  char dummy;
  } ;


int BuildDefault(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_BuildDefault _ptoc_LOCAL;
    int _BuildDefault;
    int t;
    int TempCase;
    
    TempCase = CaseStmnt(11 , 100 , IntegerPtr , 0 , 0 , CardNum[p1]);
    TempCase = LinkChain(TempCase , g1[p2]);
    t = special(101 , 0 , 0 , CardNum[p1]);
    TempCase = LinkChain(TempCase , t);
    _BuildDefault = TempCase;
    return(_BuildDefault);
    }

struct _ptoc_FRAME_LabDef
  {
  char dummy;
  } ;


int LabDef(p1)
int p1;
    {
    struct _ptoc_FRAME_LabDef _ptoc_LOCAL;
    int _LabDef;
    int lab;
    int len;
    STRING s;
    int ptr;
    struct ptoc_Type2 *ptoc_Var22;
    
    lab = val[p1];
    if (lab < 0 || lab > 9999)
        {
        ErrorMsg(28 , ColNo[p1]);
        _LabDef = -1;
        }
    else
        {
        LabelName(lab , s , &len);
        lab = EnterString(&s[0] , len);
        if (StrTable[lab + 3] == -1)
            {
            alloc_name_space(lab);
            }
        lab = FindSymbol(lab);
        if (lab == -1)
            {
            ErrorMsg(3 , ColNo[p1]);
            _LabDef = -1;
            }
        else
            {
            if (lab < ProcMark)
                {
                ErrorMsg(6 , ColNo[p1]);
                _LabDef = -1;
                }
            else
                {
                if ((STab[lab].SFlags & 64) == 0)
                    {
                    ErrorMsg(36 , ColNo[p1]);
                    _LabDef = -1;
                    }
                else
                    {
                    STab[lab].SFlags &= -65;
                    _LabDef = special(98 , 0 , lab , CardNum[p1]);
                    ptr = STab[lab].KINDTYPE.C_ModuleDcl.SParent;
                    ptoc_Var22 = &LabelStack[ptr];
                    ptoc_Var22->DefLevel = ForWithLevel;
                    if (ptoc_Var22->RefLevel > 0 && ptoc_Var22->RefLevel != ptoc_Var22->DefLevel)
                        {
                        ErrorMsg(138 , ColNo[p1]);
                        }
                    }
                }
            }
        }
    return(_LabDef);
    }

struct _ptoc_FRAME_LabRef
  {
  char dummy;
  } ;


int LabRef(p1)
int p1;
    {
    struct _ptoc_FRAME_LabRef _ptoc_LOCAL;
    int _LabRef;
    int lab;
    int len;
    STRING s;
    int ptr;
    struct ptoc_Type2 *ptoc_Var23;
    
    lab = val[p1];
    if (lab < 0 || lab > 9999)
        {
        ErrorMsg(35 , ColNo[p1]);
        _LabRef = -1;
        goto LAB_100;
        }
    LabelName(lab , s , &len);
    lab = EnterString(&s[0] , len);
    lab = FindSymbol(lab);
    if (lab == -1)
        {
        ErrorMsg(3 , ColNo[p1]);
        _LabRef = -1;
        goto LAB_100;
        }
    if (lab >= ProcMark)
        {
        _LabRef = special(87 , 0 , lab , CardNum[p1]);
        }
    else
        {
        _LabRef = EvalGoto(lab);
        }
    ptr = STab[lab].KINDTYPE.C_ModuleDcl.SParent;
    ptoc_Var23 = &LabelStack[ptr];
    if (ptoc_Var23->RefLevel > 0)
        {
        if (ForWithLevel < ptoc_Var23->RefLevel)
            {
            ptoc_Var23->RefLevel = ForWithLevel;
            }
        }
    else
        {
        ptoc_Var23->RefLevel = ForWithLevel;
        }
    if ((STab[lab].SFlags & 64) == 0 && ptoc_Var23->DefLevel == 0)
        {
        ErrorMsg(139 , ColNo[p1]);
        }
    LAB_100 :
    return(_LabRef);
    }

struct _ptoc_FRAME_EvalTo
  {
  char dummy;
  } ;


int EvalTo(f, v, e1, e2)
ParseStackRange f;
ParseStackRange v;
ParseStackRange e1;
ParseStackRange e2;
    {
    struct _ptoc_FRAME_EvalTo _ptoc_LOCAL;
    SymbolTableRange i;
    SymbolTableRange typ;
    SymbolTableRange t1;
    SymbolTableRange t2;
    HdgRange tln1;
    HdgRange tln2;
    HdgRange temp;
    HdgRange ForStmnt;
    int ptoc_Var24;
    int ptoc_Var25;
    
    i = IdLookup(v);
    if (i != -1)
        {
        typ = TypeType[v];
        if ((STab[i].SFlags & 2048) != 0)
            {
            ErrorMsg(142 , ColNo[v]);
            }
        if (STab[i].SKind != 9 && STab[i].SKind != 14)
            {
            ErrorMsg(18 , ColNo[v]);
            g1[f] = -1;
            }
        else
            {
            if (STab[i].SKind != 9 || STab[i].SLevel <= STab[ProcIndex].SLevel)
                {
                ErrorMsg(134 , ColNo[v]);
                g1[f] = -1;
                }
            else
                {
                if (!IsOrdinal(typ))
                    {
                    ErrorMsg(1 , ColNo[v]);
                    g1[f] = -1;
                    }
                else
                    {
                    if (!AssgnmntCompatible(typ , g1[e1]))
                        {
                        ErrorMsg(0 , ColNo[e1]);
                        g1[f] = -1;
                        }
                    else
                        {
                        if (!AssgnmntCompatible(typ , g1[e2]))
                            {
                            ErrorMsg(0 , ColNo[e2]);
                            g1[f] = -1;
                            }
                        else
                            {
                            if (IsConstant(g1[e2]) && IsConstant(g1[e1]) && field[g1[e1] + 5] > field[g1[e2] + 5])
                                {
                                g1[f] = -1;
                                }
                            else
                                {
                                if (control[116])
                                    {
                                    if (IsConstant(g1[e1]))
                                        {
                                        t1 = -1;
                                        tln1 = EvalConvert(typ , g1[e1]);
                                        }
                                    else
                                        {
                                        t1 = GetTemporary(ProcIndex , typ , 0);
                                        tln1 = UnaryExpression(4 , typ , node3(94 , typ , t1 , 0) , 0 , 0);
                                        }
                                    if (IsConstant(g1[e2]))
                                        {
                                        t2 = -1;
                                        tln2 = EvalConvert(typ , g1[e2]);
                                        }
                                    else
                                        {
                                        t2 = GetTemporary(ProcIndex , typ , 0);
                                        tln2 = UnaryExpression(4 , typ , node3(94 , typ , t2 , 0) , 0 , 0);
                                        }
                                    temp = LongStatement(102 , typ , node3(94 , typ , i , 0) , node3(12 , typ , 1 , 0) , 0 , 19 , CardNum[f]);
                                    ForStmnt = FoldBinary(34 , BooleanPtr , UnaryExpression(4 , typ , node3(94 , typ , i , 0) , 0 , 0) , tln2);
                                    ForStmnt = ComplexStmnt(9 , 0 , ForStmnt , temp , -1 , CardNum[f]);
                                    g2[f] = ForStmnt;
                                    temp = LongStatement(6 , typ , node3(94 , typ , i , 0) , tln1 , 0 , 0 , CardNum[f]);
                                    ForStmnt = LinkChain(temp , ForStmnt);
                                    temp = -1;
                                    ptoc_Var24 = t1 != -1;
                                    if (ptoc_Var24)
                                        {
                                        temp = LinkChain(temp , LongStatement(6 , typ , node3(94 , typ , t1 , 0) , g1[e1] , 0 , 0 , CardNum[f]));
                                        }
                                    if (t2 != -1)
                                        {
                                        temp = LinkChain(temp , LongStatement(6 , typ , node3(94 , typ , t2 , 0) , g1[e2] , 0 , 0 , CardNum[f]));
                                        }
                                    ForStmnt = LinkChain(temp , ForStmnt);
                                    g1[f] = ForStmnt;
                                    TypeType[f] = t2;
                                    if (ptoc_Var24)
                                        {
                                        FreeTemporary(t1);
                                        }
                                    }
                                else
                                    {
                                    if (IsConstant(g1[e1]))
                                        {
                                        t1 = -1;
                                        tln1 = EvalConvert(IntegerPtr , g1[e1]);
                                        }
                                    else
                                        {
                                        t1 = GetTemporary(ProcIndex , IntegerPtr , 0);
                                        tln1 = UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , t1 , 0) , 0 , 0);
                                        }
                                    if (IsConstant(g1[e2]))
                                        {
                                        t2 = -1;
                                        tln2 = EvalConvert(IntegerPtr , g1[e2]);
                                        }
                                    else
                                        {
                                        t2 = GetTemporary(ProcIndex , IntegerPtr , 0);
                                        tln2 = UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , t2 , 0) , 0 , 0);
                                        }
                                    temp = FoldBinary(31 , BooleanPtr , UnaryExpression(4 , typ , node3(94 , typ , i , 0) , 0 , 0) , EvalConvert(typ , CopyTree(tln2)));
                                    ForStmnt = ComplexStmnt(8 , 0 , temp , special(101 , 0 , 0 , CardNum[f]) , -1 , CardNum[f]);
                                    temp = LongStatement(102 , typ , node3(94 , typ , i , 0) , node3(12 , typ , 1 , 0) , 0 , 19 , CardNum[f]);
                                    if (control[114])
                                        {
                                        temp = RangeCheck(SubType[v] , temp);
                                        }
                                    ForStmnt = LinkChain(ForStmnt , temp);
                                    ForStmnt = ComplexStmnt(12 , 0 , node3(12 , BooleanPtr , 0 , 0) , ForStmnt , -1 , CardNum[f]);
                                    g2[f] = ForStmnt;
                                    temp = EvalConvert(typ , CopyTree(tln1));
                                    if (control[114])
                                        {
                                        temp = RangeCheck(SubType[v] , temp);
                                        }
                                    temp = LongStatement(6 , typ , node3(94 , typ , i , 0) , temp , 0 , 0 , CardNum[f]);
                                    ForStmnt = LinkChain(temp , ForStmnt);
                                    temp = FoldBinary(34 , BooleanPtr , CopyTree(tln1) , CopyTree(tln2));
                                    ForStmnt = ComplexStmnt(8 , 0 , temp , ForStmnt , -1 , CardNum[f]);
                                    ForStmnt = FoldTree(ForStmnt);
                                    temp = -1;
                                    ptoc_Var25 = t1 != -1;
                                    if (ptoc_Var25)
                                        {
                                        temp = LinkChain(temp , LongStatement(6 , IntegerPtr , node3(94 , IntegerPtr , t1 , 0) , g1[e1] , 0 , 0 , CardNum[f]));
                                        }
                                    if (t2 != -1)
                                        {
                                        temp = LinkChain(temp , LongStatement(6 , IntegerPtr , node3(94 , IntegerPtr , t2 , 0) , g1[e2] , 0 , 0 , CardNum[f]));
                                        }
                                    ForStmnt = LinkChain(temp , ForStmnt);
                                    g1[f] = ForStmnt;
                                    TypeType[f] = t2;
                                    if (ptoc_Var25)
                                        {
                                        FreeTemporary(t1);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    else
        {
        g1[f] = -1;
        }
    SubType[f] = i;
    ForWithLevel += 1;
    if (g1[f] != -1)
        {
        STab[i].SFlags |= 2048;
        }
    }

struct _ptoc_FRAME_EvalDownto
  {
  char dummy;
  } ;


int EvalDownto(f, v, e1, e2)
ParseStackRange f;
ParseStackRange v;
ParseStackRange e1;
ParseStackRange e2;
    {
    struct _ptoc_FRAME_EvalDownto _ptoc_LOCAL;
    SymbolTableRange i;
    SymbolTableRange typ;
    SymbolTableRange t1;
    SymbolTableRange t2;
    HdgRange tln1;
    HdgRange tln2;
    HdgRange temp;
    HdgRange ForStmnt;
    int ptoc_Var26;
    int ptoc_Var27;
    
    i = IdLookup(v);
    if (i != -1)
        {
        typ = TypeType[v];
        if ((STab[i].SFlags & 2048) != 0)
            {
            ErrorMsg(142 , ColNo[v]);
            }
        if (STab[i].SKind != 9 && STab[i].SKind != 14)
            {
            ErrorMsg(18 , ColNo[v]);
            g1[f] = -1;
            }
        else
            {
            if (STab[i].SKind != 9 || STab[i].SLevel <= STab[ProcIndex].SLevel)
                {
                ErrorMsg(134 , ColNo[v]);
                g1[f] = -1;
                }
            else
                {
                if (!IsOrdinal(typ))
                    {
                    ErrorMsg(1 , ColNo[v]);
                    g1[f] = -1;
                    }
                else
                    {
                    if (!AssgnmntCompatible(typ , g1[e1]))
                        {
                        ErrorMsg(0 , ColNo[e1]);
                        g1[f] = -1;
                        }
                    else
                        {
                        if (!AssgnmntCompatible(typ , g1[e2]))
                            {
                            ErrorMsg(0 , ColNo[e2]);
                            g1[f] = -1;
                            }
                        else
                            {
                            if (IsConstant(g1[e2]) && IsConstant(g1[e1]) && field[g1[e1] + 5] < field[g1[e2] + 5])
                                {
                                g1[f] = -1;
                                }
                            else
                                {
                                if (control[116])
                                    {
                                    if (IsConstant(g1[e1]))
                                        {
                                        t1 = -1;
                                        tln1 = EvalConvert(typ , g1[e1]);
                                        }
                                    else
                                        {
                                        t1 = GetTemporary(ProcIndex , typ , 0);
                                        tln1 = UnaryExpression(4 , typ , node3(94 , typ , t1 , 0) , 0 , 0);
                                        }
                                    if (IsConstant(g1[e2]))
                                        {
                                        t2 = -1;
                                        tln2 = EvalConvert(typ , g1[e2]);
                                        }
                                    else
                                        {
                                        t2 = GetTemporary(ProcIndex , typ , 0);
                                        tln2 = UnaryExpression(4 , typ , node3(94 , typ , t2 , 0) , 0 , 0);
                                        }
                                    temp = LongStatement(102 , typ , node3(94 , typ , i , 0) , node3(12 , typ , 1 , 0) , 0 , 20 , CardNum[f]);
                                    ForStmnt = FoldBinary(35 , BooleanPtr , UnaryExpression(4 , typ , node3(94 , typ , i , 0) , 0 , 0) , tln2);
                                    ForStmnt = ComplexStmnt(9 , 0 , ForStmnt , temp , -1 , CardNum[f]);
                                    g2[f] = ForStmnt;
                                    temp = LongStatement(6 , typ , node3(94 , typ , i , 0) , tln1 , 0 , 0 , CardNum[f]);
                                    ForStmnt = LinkChain(temp , ForStmnt);
                                    temp = -1;
                                    ptoc_Var26 = t1 != -1;
                                    if (ptoc_Var26)
                                        {
                                        temp = LinkChain(temp , LongStatement(6 , typ , node3(94 , typ , t1 , 0) , g1[e1] , 0 , 0 , CardNum[f]));
                                        }
                                    if (t2 != -1)
                                        {
                                        temp = LinkChain(temp , LongStatement(6 , typ , node3(94 , typ , t2 , 0) , g1[e2] , 0 , 0 , CardNum[f]));
                                        }
                                    ForStmnt = LinkChain(temp , ForStmnt);
                                    g1[f] = ForStmnt;
                                    TypeType[f] = t2;
                                    if (ptoc_Var26)
                                        {
                                        FreeTemporary(t1);
                                        }
                                    }
                                else
                                    {
                                    if (IsConstant(g1[e1]))
                                        {
                                        t1 = -1;
                                        tln1 = EvalConvert(IntegerPtr , g1[e1]);
                                        }
                                    else
                                        {
                                        t1 = GetTemporary(ProcIndex , IntegerPtr , 0);
                                        tln1 = UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , t1 , 0) , 0 , 0);
                                        }
                                    if (IsConstant(g1[e2]))
                                        {
                                        t2 = -1;
                                        tln2 = EvalConvert(IntegerPtr , g1[e2]);
                                        }
                                    else
                                        {
                                        t2 = GetTemporary(ProcIndex , IntegerPtr , 0);
                                        tln2 = UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , t2 , 0) , 0 , 0);
                                        }
                                    temp = FoldBinary(31 , BooleanPtr , UnaryExpression(4 , typ , node3(94 , typ , i , 0) , 0 , 0) , EvalConvert(typ , CopyTree(tln2)));
                                    ForStmnt = ComplexStmnt(8 , 0 , temp , special(101 , 0 , 0 , CardNum[f]) , -1 , CardNum[f]);
                                    temp = LongStatement(102 , typ , node3(94 , typ , i , 0) , node3(12 , typ , 1 , 0) , 0 , 20 , CardNum[f]);
                                    if (control[114])
                                        {
                                        temp = RangeCheck(SubType[v] , temp);
                                        }
                                    ForStmnt = LinkChain(ForStmnt , temp);
                                    ForStmnt = ComplexStmnt(12 , 0 , node3(12 , BooleanPtr , 0 , 0) , ForStmnt , -1 , CardNum[f]);
                                    g2[f] = ForStmnt;
                                    temp = EvalConvert(typ , CopyTree(tln1));
                                    if (control[114])
                                        {
                                        temp = RangeCheck(SubType[v] , temp);
                                        }
                                    temp = LongStatement(6 , typ , node3(94 , typ , i , 0) , temp , 0 , 0 , CardNum[f]);
                                    ForStmnt = LinkChain(temp , ForStmnt);
                                    temp = FoldBinary(35 , BooleanPtr , CopyTree(tln1) , CopyTree(tln2));
                                    ForStmnt = ComplexStmnt(8 , 0 , temp , ForStmnt , -1 , CardNum[f]);
                                    ForStmnt = FoldTree(ForStmnt);
                                    temp = -1;
                                    ptoc_Var27 = t1 != -1;
                                    if (ptoc_Var27)
                                        {
                                        temp = LinkChain(temp , LongStatement(6 , IntegerPtr , node3(94 , IntegerPtr , t1 , 0) , g1[e1] , 0 , 0 , CardNum[f]));
                                        }
                                    if (t2 != -1)
                                        {
                                        temp = LinkChain(temp , LongStatement(6 , IntegerPtr , node3(94 , IntegerPtr , t2 , 0) , g1[e2] , 0 , 0 , CardNum[f]));
                                        }
                                    ForStmnt = LinkChain(temp , ForStmnt);
                                    g1[f] = ForStmnt;
                                    TypeType[f] = t2;
                                    if (ptoc_Var27)
                                        {
                                        FreeTemporary(t1);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    else
        {
        g1[f] = -1;
        }
    SubType[f] = i;
    ForWithLevel += 1;
    if (g1[f] != -1)
        {
        STab[i].SFlags |= 2048;
        }
    }

struct _ptoc_FRAME_EvalFor
  {
  char dummy;
  } ;


int EvalFor(p1, p2)
HdgRange p1;
HdgRange p2;
    {
    struct _ptoc_FRAME_EvalFor _ptoc_LOCAL;
    HdgRange NodeIndex;
    
    if (g1[p1] != -1)
        {
        NodeIndex = g2[p1];
        if (control[116])
            {
            NodeIndex = field[NodeIndex + 5];
            NodeIndex = field[field[NodeIndex + 5] + 5];
            STab[field[NodeIndex + 5]].SFlags &= -2049;
            NodeIndex = g2[p1];
            field[NodeIndex + 6] = LinkChain(g1[p2] , field[NodeIndex + 6]);
            }
        else
            {
            NodeIndex = field[NodeIndex + 6];
            NodeIndex = field[NodeIndex + 5];
            NodeIndex = field[field[NodeIndex + 5] + 5];
            STab[field[NodeIndex + 5]].SFlags &= -2049;
            NodeIndex = g2[p1];
            field[NodeIndex + 6] = LinkChain(g1[p2] , field[NodeIndex + 6]);
            }
        if (TypeType[p1] != -1)
            {
            FreeTemporary(TypeType[p1]);
            }
        }
    if (control[103] && SubType[p1] != -1)
        {
        NodeIndex = LongStatement(6 , IntegerPtr , node3(94 , IntegerPtr , SubType[p1] , 0) , node3(12 , IntegerPtr , FillForVar , 0) , 0 , 0 , CardNum[p1]);
        g1[p1] = LinkChain(g1[p1] , NodeIndex);
        }
    ClearLabel();
    }

struct _ptoc_FRAME_EvalProgramParameters
  {
  char dummy;
  } ;


int EvalProgramParameters(p1)
ParseStackRange p1;
    {
    struct _ptoc_FRAME_EvalProgramParameters _ptoc_LOCAL;
    int _EvalProgramParameters;
    int i;
    int k;
    SymbolTableRange id;
    HdgRange NodeIndex;
    
    NodeIndex = -1;
    k = 1;
    while (k <= cnt[p1])
        {
        i = StartCnt[p1] + k;
        id = FindSymbol(IdList[i]);
        if (id != -1)
            {
            if (id != InputPtr && id != OutputPtr)
                {
                if (IsFile(STab[id].SType))
                    {
                    NodeIndex = LinkChain(NodeIndex , EvalDefname(id));
                    }
                else
                    {
                    DclError(IdList[i] , CardNum[p1] , CurrentFile , 1);
                    }
                }
            }
        else
            {
            DclError(IdList[i] , CardNum[p1] , CurrentFile , 3);
            }
        k += 1;
        }
    IdPtr = StartCnt[p1];
    _EvalProgramParameters = NodeIndex;
    return(_EvalProgramParameters);
    }

struct _ptoc_FRAME_EvalAssert
  {
  char dummy;
  } ;


int EvalAssert(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_EvalAssert _ptoc_LOCAL;
    int _EvalAssert;
    
    _EvalAssert = -1;
    return(_EvalAssert);
    }
