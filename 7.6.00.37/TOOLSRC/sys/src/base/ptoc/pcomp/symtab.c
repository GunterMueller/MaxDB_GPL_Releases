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
int IsConstant();
int FoldUnary();
int FoldBinary();
int FoldSubscript();
int FoldTree();
int StrTable[32768];
int StabI();
int EnterString();
int GetString();
unsigned char *_GetStringPtr();
int _GetStringLength();
int _GetStringNo();
unsigned char _GetStringChar();
unsigned char _GetFirstChar();
int shl();
int shr();
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
  int ParmTable[1001];
typedef
  int IntExpList[1001];
typedef
  int IntConstList[501];
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
SymbolTableRange TextPtr;
SymbolTableRange PacPtr;
SymbolTableRange LongPtr;
SymbolTableRange FloatPtr;
SymbolTableRange DoublePtr;
SymbolTableRange UbytePtr;
SymbolTableRange UshortPtr;
SymbolTableRange BytePtr;
SymbolTableRange ShortPtr;
SymbolTableRange StringPtr;
SymbolTableRange WFormatPtr;
int IntsetIndex;
int ProcMark;
int ProcIndex;
int parct;
int ReturnedType;
int LevelNo;
ParseStack StartCnt;
ParseStack cnt;
ParseStack g1;
ParseStack g2;
ParseStack val;
ParseStack TypeType;
ParseStack SubType;
ParseStack SaveReturnedType;
ParseStack SaveParct;
ParseStack SaveProcmark;
ParseStack SaveProcindex;
ParseStack CardNum;
ParseStack ColNo;
ParseStack WithRecs;
ParseStack NameofType;
ParseStack NameofConst;
int ParmPtr;
ParmTable ParmId;
ParmTable ParmType;
ParmTable ParmNameofType;
unsigned char ParmPassedBy[1001];
int ExpPtr;
IntExpList ExpStype;
IntExpList ExpColNo;
IntExpList ExpVal;
int IdPtr;
int IdList[1001];
int HeadFieldLink;
int TailFieldLink;
int CurrentFile;
IntConstList ConstVal;
IntConstList TagType;
IntConstList StartConst;
int ConstPtr;
int TagPtr;
LabelStackType LabelStack;
int LabelPtr;
int LabelScopePtr;
int ForWithLevel;
int ConstExp();
int IsPrimConstant();
int P1m2I();
int ErrorMsg();
int DclError();
int werror();
int FatalErrorMsg();
int WarnStrCmp();
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
unsigned char SysSproc[58][20];
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
int alloc_name_space();
int BumpSymPtr();
int IsConstantExpr();
int GenConstName();
SymbolTableRange InputPtr;
SymbolTableRange OutputPtr;
SymbolTableRange ErrorPtr;
SymbolTableRange _ArgcPtr;
SymbolTableRange _ArgvPtr;
SymbolTableRange _EnvpPtr;
SymbolTableRange ArgcPtr;
SymbolTableRange ArgvPtr;
SymbolTableRange EnvpPtr;
SymbolTableRange TypeIorechd;
SymbolTableRange TypePtrIorechd;
SymbolTableRange TypeFbuf;
SymbolTableRange ElementFbuf;
SymbolTableRange AnyPtrPtr;
SymbolTableRange PtrToCharPtr;
SymbolTableRange AnyPacPtr;
SymbolTableRange AnyPacLenPtr;
SymbolTableRange AnyFilePtr;
SymbolTableRange AnySetPtr;
int BlankString;
int OctString;
int HexString;
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
struct ptoc_Type3
  {
  SymbolTableRange symbol;
  SymbolTableRange field;
  } NameSpace[32768];
int ExternalString;
int ForwardString;
int StringString;
SymbolTableRange UnsignedPtr;
int TypNmBottom;
int TypNmTop;
SymbolTableRange MainPtr;

struct _ptoc_FRAME_SymtabI
  {
  char dummy;
  } ;


int SymtabI()
    {
    struct _ptoc_FRAME_SymtabI _ptoc_LOCAL;
    int i;
    
    SymPtr = -1;
    ProcIndex = -1;
    ProcMark = 0;
    LevelNo = 0;
    WithRecs[LevelNo] = -1;
    ConstPtr = -1;
    TagPtr = -1;
    ForWithLevel = 1;
    }

struct _ptoc_FRAME_PrimType
  {
  char dummy;
  } ;


int PrimType(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_PrimType _ptoc_LOCAL;
    int _PrimType;
    
    if (t > 0 && t <= SymPtr && STab[t].SKind == 7)
        {
        while (STab[t].SType > -1)
            t = STab[t].SType;
        _PrimType = t;
        }
    else
        {
        _PrimType = t;
        }
    return(_PrimType);
    }

struct _ptoc_FRAME_ChkEqualType
  {
  char dummy;
  } ;


int ChkEqualType(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_ChkEqualType _ptoc_LOCAL;
    
    if (TypeType[p1] != TypeType[p2])
        {
        ErrorMsg(0 , ColNo[p2]);
        }
    }

struct _ptoc_FRAME_IdPtrIncr
  {
  char dummy;
  } ;


int IdPtrIncr()
    {
    struct _ptoc_FRAME_IdPtrIncr _ptoc_LOCAL;
    
    IdPtr += 1;
    if (IdPtr > 1000)
        {
        PasCerror("identifier list overflow                                                                                                                                                                                                                                        ");
        }
    }

struct _ptoc_FRAME_ParmPtrIncr
  {
  char dummy;
  } ;


int ParmPtrIncr()
    {
    struct _ptoc_FRAME_ParmPtrIncr _ptoc_LOCAL;
    
    ParmPtr += 1;
    if (ParmPtr > 1000)
        {
        PasCerror("parameter list overflow                                                                                                                                                                                                                                         ");
        }
    }

struct _ptoc_FRAME_IncrExpPtr
  {
  char dummy;
  } ;


int IncrExpPtr()
    {
    struct _ptoc_FRAME_IncrExpPtr _ptoc_LOCAL;
    
    ExpPtr += 1;
    if (ExpPtr > 1000)
        {
        PasCerror("expression list overflow                                                                                                                                                                                                                                        ");
        }
    }

struct _ptoc_FRAME_IncrConstPtr
  {
  char dummy;
  } ;


int IncrConstPtr()
    {
    struct _ptoc_FRAME_IncrConstPtr _ptoc_LOCAL;
    
    ConstPtr += 1;
    if (ConstPtr > 500)
        {
        PasCerror("constant list overflow                                                                                                                                                                                                                                          ");
        }
    }

struct _ptoc_FRAME_IncrLabelPtr
  {
  char dummy;
  } ;


int IncrLabelPtr()
    {
    struct _ptoc_FRAME_IncrLabelPtr _ptoc_LOCAL;
    
    LabelPtr += 1;
    if (LabelPtr > 100)
        {
        PasCerror("Label stack overflow                                                                                                                                                                                                                                            ");
        }
    }

struct _ptoc_FRAME_MoveStacks
  {
  char dummy;
  } ;


int MoveStacks(From, To)
int From;
int To;
    {
    struct _ptoc_FRAME_MoveStacks _ptoc_LOCAL;
    
    g1[To] = g1[From];
    g2[To] = g2[From];
    val[To] = val[From];
    SubType[To] = SubType[From];
    TypeType[To] = TypeType[From];
    StartCnt[To] = StartCnt[From];
    cnt[To] = cnt[From];
    ColNo[To] = ColNo[From];
    CardNum[To] = CardNum[From];
    SaveProcmark[To] = SaveProcmark[From];
    SaveProcindex[To] = SaveProcindex[From];
    SaveParct[To] = SaveParct[From];
    SaveReturnedType[To] = SaveReturnedType[From];
    NameofType[To] = NameofType[From];
    NameofConst[To] = NameofConst[From];
    }

struct _ptoc_FRAME_PopScope
  {
  char dummy;
  } ;


int PopScope(p)
int p;
    {
    struct _ptoc_FRAME_PopScope _ptoc_LOCAL;
    int i;
    int ss_name;
    struct ptoc_Type1 *ptoc_Var0;
    
    if (p != -1)
        {
        i = ProcMark;
        while (i <= SymPtr)
            {
            ptoc_Var0 = &STab[i];
            if (ptoc_Var0->SLevel == LevelNo)
                {
                ss_name = StrTable[ptoc_Var0->SName + 3];
                if (ss_name != -1 && NameSpace[ss_name].symbol == i)
                    {
                    NameSpace[ss_name].symbol = ptoc_Var0->SLink;
                    if ((STab[i].SFlags & 64) != 0)
                        {
                        DclError(ptoc_Var0->SName , ptoc_Var0->SCard , ptoc_Var0->SFile , 3);
                        }
                    if (ptoc_Var0->SKind == 10)
                        {
                        ptoc_Var0->KINDTYPE.C_ModuleDcl.SParent = -1;
                        }
                    }
                }
            else
                {
                if (STab[i].SKind == 16 && (STab[i].SFlags & 64) != 0)
                    {
                    DclError(ptoc_Var0->SName , ptoc_Var0->SCard , ptoc_Var0->SFile , 3);
                    }
                }
            i = STab[i].SNext;
            }
        if (STab[ProcIndex].SKind != 7)
            {
            STab[STab[STab[ProcIndex].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock].KINDTYPE.C_BlockDcl.SLastEntry = SymPtr;
            }
        ProcMark = SaveProcmark[p];
        ProcIndex = SaveProcindex[p];
        parct = SaveParct[p];
        ReturnedType = SaveReturnedType[p];
        LabelPtr = LabelScopePtr + -1;
        LabelScopePtr = LabelStack[LabelScopePtr].LIndex;
        }
    WithRecs[LevelNo] = -1;
    LevelNo += -1;
    }

struct _ptoc_FRAME_upscope
  {
  char dummy;
  } ;


int upscope(p)
int p;
    {
    struct _ptoc_FRAME_upscope _ptoc_LOCAL;
    
    if (p != -1)
        {
        SaveProcmark[p] = ProcMark;
        SaveProcindex[p] = ProcIndex;
        SaveParct[p] = parct;
        SaveReturnedType[p] = ReturnedType;
        ProcMark = STab[SymPtr].SNext;
        ProcIndex = SymPtr;
        IncrLabelPtr();
        LabelStack[LabelPtr].LIndex = LabelScopePtr;
        LabelScopePtr = LabelPtr;
        }
    LevelNo += 1;
    WithRecs[LevelNo] = -1;
    }

struct _ptoc_FRAME_CloseRec
  {
  char dummy;
  } ;


int CloseRec()
    {
    struct _ptoc_FRAME_CloseRec _ptoc_LOCAL;
    int i;
    int ss_name;
    
    i = ProcMark;
    while (i <= SymPtr)
        {
        if (STab[i].SLevel == LevelNo)
            {
            ss_name = StrTable[STab[i].SName + 3];
            if (NameSpace[ss_name].field == i)
                {
                NameSpace[ss_name].field = STab[i].SLink;
                }
            }
        i = STab[i].SNext;
        }
    WithRecs[LevelNo] = -1;
    LevelNo += -1;
    }

struct _ptoc_FRAME_OpenRec
  {
  char dummy;
  } ;


int OpenRec()
    {
    struct _ptoc_FRAME_OpenRec _ptoc_LOCAL;
    
    LevelNo += 1;
    WithRecs[LevelNo] = -1;
    }

struct _ptoc_FRAME_NewSymbol
  {
  char dummy;
  } ;


int NewSymbol(id, typ, kind, level, card)
int id;
int typ;
KINDTYPE kind;
int level;
int card;
    {
    struct _ptoc_FRAME_NewSymbol _ptoc_LOCAL;
    
    BumpSymPtr();
    STab[SymPtr].SName = id;
    STab[SymPtr].SLink = -1;
    STab[SymPtr].SCard = card;
    STab[SymPtr].SFile = CurrentFile;
    STab[SymPtr].SType = typ;
    STab[SymPtr].SLevel = level;
    STab[SymPtr].SFlags = 0;
    STab[SymPtr].SKind = kind;
    STab[SymPtr].STname = 0;
    STab[SymPtr].STmyname = 0;
    STab[SymPtr].SOrder = 0;
    STab[SymPtr].SRecVar = 0;
    STab[SymPtr].SFeld1 = 0;
    STab[SymPtr].SFeld2 = 0;
    STab[SymPtr].SFeld3 = 0;
    STab[SymPtr].SFeld4 = 0;
    STab[SymPtr].KINDTYPE.C_DummyDcl.DummyInfo.KINDTYPE.C_DummyDcl.dummy1 = -1;
    STab[SymPtr].KINDTYPE.C_DummyDcl.DummyInfo.KINDTYPE.C_DummyDcl.dummy2 = -1;
    STab[SymPtr].KINDTYPE.C_DummyDcl.DummyInfo.KINDTYPE.C_DummyDcl.dummy3 = -1;
    }

struct _ptoc_FRAME_LinkName
  {
  char dummy;
  } ;


int LinkName(symp, StrPtr)
int symp;
int StrPtr;
    {
    struct _ptoc_FRAME_LinkName _ptoc_LOCAL;
    int ss_name;
    
    ss_name = StrTable[StrPtr + 3];
    STab[symp].SLink = NameSpace[ss_name].symbol;
    NameSpace[ss_name].symbol = symp;
    }

struct _ptoc_FRAME_FindSymbol
  {
  char dummy;
  } ;


int FindSymbol(id)
int id;
    {
    struct _ptoc_FRAME_FindSymbol _ptoc_LOCAL;
    int _FindSymbol;
    int level;
    HdgRange NodeIndex;
    int entry_;
    int strsem;
    
    strsem = StrTable[id + 3];
    if (strsem != -1)
        {
        if (WithRecs[LevelNo] == -1)
            {
            _FindSymbol = NameSpace[strsem].symbol;
            }
        else
            {
            level = LevelNo;
            NodeIndex = WithRecs[level];
            do
                {
                entry_ = FindQSymbol(field[NodeIndex + 1] , id);
                if (entry_ != -1)
                    {
                    STab[entry_].SLevel = level;
                    NodeIndex = -1;
                    }
                else
                    {
                    level += -1;
                    NodeIndex = WithRecs[level];
                    }
                }
            while (!(NodeIndex == -1));
            if (entry_ != -1)
                {
                _FindSymbol = entry_;
                }
            else
                {
                _FindSymbol = NameSpace[strsem].symbol;
                }
            }
        }
    else
        {
        _FindSymbol = -1;
        }
    return(_FindSymbol);
    }

struct _ptoc_FRAME_FindQSymbol
  {
  char dummy;
  } ;


int FindQSymbol(rec, id)
int rec;
int id;
    {
    struct _ptoc_FRAME_FindQSymbol _ptoc_LOCAL;
    int _FindQSymbol;
    int i;
    int j;
    
    i = STab[rec].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField;
    while (i != -1)
        {
        if (STab[i].SName == id)
            {
            _FindQSymbol = i;
            goto LAB_100;
            }
        i = STab[i].KINDTYPE.C_RecordElementDcl.SNextField;
        }
    j = STab[rec].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant;
    while (j != -1)
        {
        i = FindQSymbol(j , id);
        if (i != -1)
            {
            _FindQSymbol = i;
            goto LAB_100;
            }
        j = STab[j].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SNextVariant;
        }
    _FindQSymbol = -1;
    LAB_100 :
    return(_FindQSymbol);
    }

struct _ptoc_FRAME_duplicate
  {
  char dummy;
  } ;


int duplicate(id, col)
int id;
int col;
    {
    struct _ptoc_FRAME_duplicate _ptoc_LOCAL;
    int _duplicate;
    int i;
    
    if (id == 0)
        {
        _duplicate = 0;
        }
    else
        {
        i = FindSymbol(id);
        if (i == MainPtr && LevelNo == 1)
            {
            ErrorMsg(140 , col);
            _duplicate = 0;
            }
        else
            {
            if (i >= ProcMark)
                {
                ErrorMsg(102 , col);
                _duplicate = 1;
                }
            else
                {
                _duplicate = 0;
                }
            }
        }
    return(_duplicate);
    }

struct _ptoc_FRAME_EntSymbol
  {
  char dummy;
  } ;


int EntSymbol(id, typ, kind, card)
int id;
int typ;
KINDTYPE kind;
int card;
    {
    struct _ptoc_FRAME_EntSymbol _ptoc_LOCAL;
    
    NewSymbol(id , typ , kind , LevelNo , card);
    if (id != 0)
        {
        LinkName(SymPtr , id);
        }
    }

struct _ptoc_FRAME_EntModule
  {
  char dummy;
  } ;


int EntModule(p, ModuleKind)
ParseStackRange p;
KINDTYPE ModuleKind;
    {
    struct _ptoc_FRAME_EntModule _ptoc_LOCAL;
    int _EntModule;
    STRING s;
    int id;
    int TypePtr;
    struct ptoc_Type1 *ptoc_Var1;
    struct ptoc_Type1 *ptoc_Var2;
    struct ptoc_Type1 *ptoc_Var3;
    struct ptoc_Type1 *ptoc_Var4;
    struct ptoc_Type1 *ptoc_Var5;
    
    _ptoc_ASSIGN(unsigned char , 256, s, "main                                                                                                                                                                                                                                                            ")
    id = EnterString(&s[0] , 4);
    if (StrTable[id + 3] == -1)
        {
        alloc_name_space(id);
        }
    EntSymbol(id , 0 , ModuleKind , CardNum[p]);
    STab[SymPtr].SFlags |= 32;
    ptoc_Var1 = &STab[SymPtr];
    ptoc_Var1->SType = ptoc_Var1->SNext;
    ptoc_Var1->KINDTYPE.C_ModuleDcl.SClass = 2;
    ptoc_Var1->KINDTYPE.C_ModuleDcl.SSubClass = 0;
    ptoc_Var1->KINDTYPE.C_ModuleDcl.SParent = -1;
    MainPtr = SymPtr;
    upscope(p);
    EntSymbol(0 , -23 , 7 , CardNum[p]);
    ptoc_Var2 = &STab[SymPtr];
    ptoc_Var2->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = IntegerPtr;
    ptoc_Var2->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt = 3;
    ptoc_Var2->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock = -1;
    TypePtr = SymPtr;
    _ptoc_ASSIGN(unsigned char , 256, s, ".argc.                                                                                                                                                                                                                                                          ")
    id = EnterString(&s[0] , 6);
    if (StrTable[id + 3] == -1)
        {
        alloc_name_space(id);
        }
    EntSymbol(id , IntegerPtr , 14 , CardNum[p]);
    ptoc_Var3 = &STab[SymPtr];
    ptoc_Var3->KINDTYPE.C_ModuleDcl.SClass = 7;
    ptoc_Var3->KINDTYPE.C_ModuleDcl.SSubClass = 1;
    ptoc_Var3->KINDTYPE.C_ModuleDcl.SParent = -1;
    ArgcPtr = SymPtr;
    _ptoc_ASSIGN(unsigned char , 256, s, ".argv.                                                                                                                                                                                                                                                          ")
    id = EnterString(&s[0] , 6);
    if (StrTable[id + 3] == -1)
        {
        alloc_name_space(id);
        }
    EntSymbol(id , AnyPtrPtr , 14 , CardNum[p]);
    ptoc_Var4 = &STab[SymPtr];
    ptoc_Var4->KINDTYPE.C_ModuleDcl.SClass = 7;
    ptoc_Var4->KINDTYPE.C_ModuleDcl.SSubClass = 1;
    ptoc_Var4->KINDTYPE.C_ModuleDcl.SParent = -1;
    ArgvPtr = SymPtr;
    _ptoc_ASSIGN(unsigned char , 256, s, ".envp.                                                                                                                                                                                                                                                          ")
    id = EnterString(&s[0] , 6);
    if (StrTable[id + 3] == -1)
        {
        alloc_name_space(id);
        }
    EntSymbol(id , AnyPtrPtr , 14 , CardNum[p]);
    ptoc_Var5 = &STab[SymPtr];
    ptoc_Var5->KINDTYPE.C_ModuleDcl.SClass = 7;
    ptoc_Var5->KINDTYPE.C_ModuleDcl.SSubClass = 1;
    ptoc_Var5->KINDTYPE.C_ModuleDcl.SParent = -1;
    EnvpPtr = SymPtr;
    _EntModule = TypePtr;
    return(_EntModule);
    }

struct _ptoc_FRAME_EntBlock
  {
  char dummy;
  } ;


int EntBlock(p)
ParseStackRange p;
    {
    struct _ptoc_FRAME_EntBlock _ptoc_LOCAL;
    
    EntSymbol(0 , 0 , 21 , CardNum[p]);
    STab[SymPtr].KINDTYPE.C_BlockDcl.SFuncBlock = ProcIndex;
    STab[STab[ProcIndex].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock = SymPtr;
    }

struct _ptoc_FRAME_EntConstant
  {
  char dummy;
  } ;


int EntConstant(id, p)
int id;
int p;
    {
    struct _ptoc_FRAME_EntConstant _ptoc_LOCAL;
    int NodeIndex;
    int t;
    
    if (!duplicate(id , -1))
        {
        EntSymbol(id , TypeType[p] , 8 , CardNum[p]);
        NodeIndex = g1[p];
        if (NodeIndex != -1)
            {
            NodeIndex = FoldTree(NodeIndex);
            STab[SymPtr].SFeld3 = NodeIndex;
            if (IsConstant(NodeIndex))
                {
                STab[SymPtr].KINDTYPE.C_ConDcl.SCon1val = field[NodeIndex + 5];
                STab[SymPtr].KINDTYPE.C_ConDcl.SCon2val = field[NodeIndex + 6];
                }
            else
                {
                if (!IsOrdinal(PrimType(STab[SymPtr].SType)) || !IsConstantExpr(NodeIndex))
                    {
                    ErrorMsg(100 , ColNo[p]);
                    }
                }
            }
        }
    }

struct _ptoc_FRAME_EntVariable
  {
  char dummy;
  } ;


int EntVariable(p1, p2, p3)
int p1;
int p2;
int p3;
    {
    struct _ptoc_FRAME_EntVariable _ptoc_LOCAL;
    int i;
    int id;
    SymbolTableRange typ;
    int card;
    int ptoc_Var6;
    int ptoc_Var7;
    int ptoc_Var8;
    
    if (STab[TypeType[p2]].SType == -21)
        {
        ErrorMsg(1 , ColNo[p2]);
        }
    typ = SubType[p2];
    card = CardNum[p2];
    ptoc_Var8 = StartCnt[p1];
    ptoc_Var6 = ptoc_Var8 + 1;
    ptoc_Var7 = ptoc_Var8 + cnt[p1];
    if (ptoc_Var6 <= ptoc_Var7)
        {
        i = ptoc_Var6;
        do
            {
            id = IdList[i];
            if (!duplicate(id , ColNo[p1]))
                {
                EntSymbol(id , typ , 9 , card);
                STab[SymPtr].STname = NameofType[p2];
                if (LevelNo == 1)
                    {
                    STab[SymPtr].KINDTYPE.C_ModuleDcl.SClass = 2;
                    STab[SymPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
                    }
                else
                    {
                    STab[SymPtr].KINDTYPE.C_ModuleDcl.SClass = 1;
                    if (control[102])
                        {
                        FillVar(SymPtr);
                        }
                    else
                        {
                        STab[SymPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
                        }
                    }
                STab[SymPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
                }
            if (i == ptoc_Var7)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_EntField
  {
  char dummy;
  } ;


int EntField(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_EntField _ptoc_LOCAL;
    int i;
    int k;
    int id;
    int ss_id;
    
    k = 1;
    while (k <= cnt[p1])
        {
        i = StartCnt[p1] + k;
        k += 1;
        id = IdList[i];
        ss_id = StrTable[id + 3];
        i = NameSpace[ss_id].field;
        if (i >= ProcMark)
            {
            ErrorMsg(102 , ColNo[p1]);
            goto LAB_100;
            }
        NewSymbol(id , SubType[p2] , 11 , LevelNo , CardNum[p2]);
        STab[SymPtr].KINDTYPE.C_RecordElementDcl.SNextField = -1;
        STab[SymPtr].STname = NameofType[p2];
        STab[SymPtr].SLink = NameSpace[ss_id].field;
        NameSpace[ss_id].field = SymPtr;
        if (HeadFieldLink == -1)
            {
            HeadFieldLink = SymPtr;
            TailFieldLink = SymPtr;
            }
        else
            {
            STab[TailFieldLink].KINDTYPE.C_RecordElementDcl.SNextField = SymPtr;
            TailFieldLink = SymPtr;
            }
        }
    LAB_100 :
    ;
    }

struct _ptoc_FRAME_EntVariant
  {
  char dummy;
  } ;


int EntVariant(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_EntVariant _ptoc_LOCAL;
    int ptoc_Var9;
    
    NewSymbol(BlankString , -14 , 12 , LevelNo , CardNum[p2]);
    ptoc_Var9 = g1[p2];
    STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField = ptoc_Var9;
    STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant = g2[p2];
    STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SNextVariant = -1;
    STab[SymPtr].STname = NameofConst[p1];
    if (ContainsFile(ptoc_Var9))
        {
        STab[SymPtr].SFlags |= 128;
        }
    if (HeadFieldLink == -1)
        {
        HeadFieldLink = SymPtr;
        TailFieldLink = SymPtr;
        }
    else
        {
        STab[TailFieldLink].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SNextVariant = SymPtr;
        TailFieldLink = SymPtr;
        }
    }

struct _ptoc_FRAME_LabelName
  {
  char dummy;
  } ;


int LabelName(lab, name, len)
int lab;
STRING name;
int *len;
    {
    struct _ptoc_FRAME_LabelName _ptoc_LOCAL;
    int i;
    int place;
    int ptoc_Var10;
    
    place = 10;
    while (place <= lab)
        place *= 10;
    i = 0;
    do
        {
        place /= 10;
        name[i] = (unsigned char ) (lab / place + sql__ord(48));
        lab = (ptoc_Var10 = lab % place) >= 0 ? ptoc_Var10 : ptoc_Var10 + (place >= 0 ? place : -place);
        i += 1;
        }
    while (!(place == 1));
    *len = i;
    }

struct _ptoc_FRAME_EntLabel
  {
  char dummy;
  } ;


int EntLabel(p1)
int p1;
    {
    struct _ptoc_FRAME_EntLabel _ptoc_LOCAL;
    int lab;
    int len;
    STRING s;
    struct ptoc_Type2 *ptoc_Var11;
    
    lab = val[p1];
    if (lab < 0 || lab > 9999)
        {
        ErrorMsg(35 , ColNo[p1]);
        }
    else
        {
        LabelName(lab , s , &len);
        lab = EnterString(&s[0] , len);
        if (StrTable[lab + 3] == -1)
            {
            alloc_name_space(lab);
            }
        if (!duplicate(lab , ColNo[p1]))
            {
            EntSymbol(lab , 0 , 10 , CardNum[p1]);
            if (LevelNo == 1)
                {
                STab[SymPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
                }
            else
                {
                STab[SymPtr].KINDTYPE.C_ModuleDcl.SClass = 4;
                }
            STab[SymPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
            IncrLabelPtr();
            STab[SymPtr].KINDTYPE.C_ModuleDcl.SParent = LabelPtr;
            ptoc_Var11 = &LabelStack[LabelPtr];
            ptoc_Var11->LIndex = SymPtr;
            ptoc_Var11->DefLevel = 0;
            ptoc_Var11->RefLevel = 0;
            STab[SymPtr].SFlags |= 64;
            }
        }
    }

struct _ptoc_FRAME_InitSystem
  {
  int isymid;
  char dummy;
  } ;
typedef
  unsigned char String20_InitSystem[20];


struct _ptoc_FRAME_InitSym_InitSystem
  {
  struct _ptoc_FRAME_InitSystem *_ptoc_LINK;
  char dummy;
  } ;


static
int InitSym_InitSystem(_ptoc_LINK, ptoc_ptr_str, kind, t)
struct _ptoc_FRAME_InitSystem *_ptoc_LINK;
String20_InitSystem ptoc_ptr_str;
KINDTYPE kind;
int t;
    {
    struct _ptoc_FRAME_InitSym_InitSystem _ptoc_LOCAL;
    int _InitSym;
    int i;
    String20_InitSystem str;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _ptoc_MOVE(char, sizeof(String20_InitSystem ), str, ptoc_ptr_str);
    i = -1;
    do
        i += 1;
    while (!((int ) str[i] == 32 || (int ) str[i] == 0));
    _ptoc_LOCAL._ptoc_LINK->isymid = EnterString(&str[0] , i);
    NewSymbol(_ptoc_LOCAL._ptoc_LINK->isymid , t , kind , LevelNo , 0);
    if (_ptoc_LOCAL._ptoc_LINK->isymid != 0)
        {
        if (StrTable[_ptoc_LOCAL._ptoc_LINK->isymid + 3] == -1)
            {
            alloc_name_space(_ptoc_LOCAL._ptoc_LINK->isymid);
            }
        LinkName(SymPtr , _ptoc_LOCAL._ptoc_LINK->isymid);
        }
    _InitSym = SymPtr;
    return(_InitSym);
    }

int InitSystem()
    {
    struct _ptoc_FRAME_InitSystem _ptoc_LOCAL;
    SymbolTableRange i;
    SymbolTableRange j;
    SymbolTableRange id;
    SymbolTableRange SaveSymPtr;
    SymbolTableRange TypeFileptr;
    String20_InitSystem s;
    int nflag;
    
    nflag = control[110];
    _ptoc_ASSIGN(unsigned char , 20, s, "external            ")
    ExternalString = EnterString(&s[0] , 8);
    TypNmBottom = ExternalString;
    _ptoc_ASSIGN(unsigned char , 20, s, "forward             ")
    ForwardString = EnterString(&s[0] , 7);
    _ptoc_ASSIGN(unsigned char , 20, s, "string              ")
    StringString = EnterString(&s[0] , 6);
    _ptoc_ASSIGN(unsigned char , 20, s, "oct                 ")
    OctString = EnterString(&s[0] , 3);
    _ptoc_ASSIGN(unsigned char , 20, s, "hex                 ")
    HexString = EnterString(&s[0] , 3);
    _ptoc_ASSIGN(unsigned char , 20, s, "                    ")
    BlankString = EnterString(&s[0] , 1);
    NewSymbol(0 , 0 , 0 , 0 , 0);
    IntegerPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#integer            " , 7 , -5);
    STab[IntegerPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = -2147483647;
    STab[IntegerPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 2147483647;
    TypeSize(IntegerPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "longint             " , 16 , IntegerPtr);
    UnsignedPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#unsigned           " , 7 , -6);
    STab[UnsignedPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
    STab[UnsignedPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = -1;
    TypeSize(UnsignedPtr);
    ShortPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#short              " , 7 , -1);
    STab[ShortPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = -32768;
    STab[ShortPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 32767;
    TypeSize(ShortPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "shortint            " , 16 , ShortPtr);
    UshortPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#ushort             " , 7 , -2);
    STab[UshortPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
    STab[UshortPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 65535;
    TypeSize(UshortPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "integer             " , 16 , IntegerPtr);
    BytePtr = InitSym_InitSystem(&_ptoc_LOCAL, "#byte               " , 7 , -8);
    STab[BytePtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = -128;
    STab[BytePtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 127;
    TypeSize(BytePtr);
    UbytePtr = InitSym_InitSystem(&_ptoc_LOCAL, "#ubyte              " , 7 , -9);
    STab[UbytePtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
    STab[UbytePtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 255;
    TypeSize(UbytePtr);
    BooleanPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#boolean            " , 7 , -7);
    STab[BooleanPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
    STab[BooleanPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 1;
    TypeSize(BooleanPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "false               " , 8 , BooleanPtr);
    STab[i].KINDTYPE.C_ConDcl.SCon1val = 0;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "true                " , 8 , BooleanPtr);
    STab[i].KINDTYPE.C_ConDcl.SCon1val = 1;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "boolean             " , 16 , BooleanPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "Boolean             " , 16 , BooleanPtr);
    CharPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#char               " , 7 , -24);
    STab[CharPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
    STab[CharPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 255;
    TypeSize(CharPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "char                " , 16 , CharPtr);
    FloatPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#float              " , 7 , -11);
    TypeSize(FloatPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "float               " , 16 , FloatPtr);
    DoublePtr = InitSym_InitSystem(&_ptoc_LOCAL, "#double             " , 7 , -12);
    TypeSize(DoublePtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "double              " , 16 , DoublePtr);
    LongPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#long               " , 7 , -3);
    TypeSize(LongPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "long                " , 16 , LongPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "real                " , 16 , FloatPtr);
    TextPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#text               " , 7 , -15);
    STab[TextPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent = CharPtr;
    TypeSize(TextPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "text                " , 16 , TextPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#pac_range          " , 7 , IntegerPtr);
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = -1;
    PacPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#pac                " , 7 , -13);
    STab[PacPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex = i;
    STab[PacPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = CharPtr;
    TypeSize(PacPtr);
    AnyPacPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#anypac             " , 7 , -21);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "anyPAC              " , 16 , AnyPacPtr);
    AnyPacLenPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#anypaclen          " , 7 , -21);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "anyPAClen           " , 16 , AnyPacLenPtr);
    AnyFilePtr = InitSym_InitSystem(&_ptoc_LOCAL, "#anyfile            " , 7 , -21);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "anyfile             " , 16 , AnyFilePtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#alfa_range         " , 7 , IntegerPtr);
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 1;
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 10;
    j = InitSym_InitSystem(&_ptoc_LOCAL, "#alfa               " , 7 , -13);
    STab[j].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex = i;
    STab[j].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = CharPtr;
    TypeSize(j);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "alfa                " , 16 , j);
    StringPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#string             " , 7 , -20);
    STab[StringPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SLength = 256;
    TypeSize(StringPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#intset_base        " , 7 , IntegerPtr);
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 127;
    TypeSize(i);
    j = InitSym_InitSystem(&_ptoc_LOCAL, "#intset             " , 7 , -16);
    STab[j].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase = i;
    TypeSize(j);
    _ptoc_ASSIGN(unsigned char , 20, s, "intset              ")
    IntsetIndex = EnterString(&s[0] , 6);
    if (StrTable[IntsetIndex + 3] == -1)
        {
        alloc_name_space(IntsetIndex);
        }
    i = InitSym_InitSystem(&_ptoc_LOCAL, s , 16 , j);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#anyset_base        " , 7 , IntegerPtr);
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 0;
    TypeSize(i);
    j = InitSym_InitSystem(&_ptoc_LOCAL, "#anyset             " , 7 , -16);
    STab[j].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase = i;
    TypeSize(j);
    AnySetPtr = j;
    AnyPtrPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#anyptrptr          " , 7 , -17);
    STab[AnyPtrPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = IntegerPtr;
    TypeSize(AnyPtrPtr);
    PtrToCharPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#ptrtocharptr       " , 7 , -17);
    STab[PtrToCharPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = CharPtr;
    TypeSize(PtrToCharPtr);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "maxint              " , 8 , IntegerPtr);
    STab[i].KINDTYPE.C_ConDcl.SCon1val = 2147483647;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "minint              " , 8 , IntegerPtr);
    STab[i].KINDTYPE.C_ConDcl.SCon1val = -2147483647;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "maxchar             " , 8 , CharPtr);
    if (nflag)
        {
        STab[i].KINDTYPE.C_ConDcl.SCon1val = 255;
        }
    else
        {
        STab[i].KINDTYPE.C_ConDcl.SCon1val = 127;
        }
    i = InitSym_InitSystem(&_ptoc_LOCAL, "minchar             " , 8 , CharPtr);
    STab[i].KINDTYPE.C_ConDcl.SCon1val = 0;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "bell                " , 8 , CharPtr);
    STab[i].KINDTYPE.C_ConDcl.SCon1val = 7;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "tab                 " , 8 , CharPtr);
    STab[i].KINDTYPE.C_ConDcl.SCon1val = 9;
    InputPtr = InitSym_InitSystem(&_ptoc_LOCAL, "sql__input          " , 9 , TextPtr);
    STab[InputPtr].SFlags |= 32;
    STab[InputPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
    STab[InputPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    STab[InputPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
    InputPtr = InitSym_InitSystem(&_ptoc_LOCAL, "input               " , 9 , TextPtr);
    STab[InputPtr].SFlags |= 32;
    STab[InputPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
    STab[InputPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    STab[InputPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
    OutputPtr = InitSym_InitSystem(&_ptoc_LOCAL, "sql__output         " , 9 , TextPtr);
    STab[OutputPtr].SFlags |= 32;
    STab[OutputPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
    STab[OutputPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    STab[OutputPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
    OutputPtr = InitSym_InitSystem(&_ptoc_LOCAL, "output              " , 9 , TextPtr);
    STab[OutputPtr].SFlags |= 32;
    STab[OutputPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
    STab[OutputPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    STab[OutputPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
    TypNmTop = _ptoc_LOCAL.isymid;
    ErrorPtr = InitSym_InitSystem(&_ptoc_LOCAL, "sql__err            " , 9 , TextPtr);
    STab[ErrorPtr].SFlags |= 32;
    STab[ErrorPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
    STab[ErrorPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    STab[ErrorPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
    _ArgcPtr = InitSym_InitSystem(&_ptoc_LOCAL, "_argc               " , 9 , IntegerPtr);
    STab[_ArgcPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
    STab[_ArgcPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    STab[_ArgcPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
    STab[_ArgcPtr].SFlags |= 32;
    _ArgvPtr = InitSym_InitSystem(&_ptoc_LOCAL, "_argv               " , 9 , AnyPtrPtr);
    STab[_ArgvPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
    STab[_ArgvPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    STab[_ArgvPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
    STab[_ArgvPtr].SFlags |= 32;
    _EnvpPtr = InitSym_InitSystem(&_ptoc_LOCAL, "_envp               " , 9 , AnyPtrPtr);
    STab[_EnvpPtr].KINDTYPE.C_ModuleDcl.SClass = 0;
    STab[_EnvpPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    STab[_EnvpPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
    STab[_EnvpPtr].SFlags |= 32;
    TypeFileptr = InitSym_InitSystem(&_ptoc_LOCAL, "                    " , 7 , -17);
    STab[TypeFileptr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = CharPtr;
    TypeSize(TypeFileptr);
    TypeFbuf = InitSym_InitSystem(&_ptoc_LOCAL, "                    " , 7 , -17);
    STab[TypeFbuf].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = IntegerPtr;
    TypeSize(TypeFbuf);
    SaveSymPtr = SymPtr + 1;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#fileptr            " , 11 , TypeFileptr);
    STab[i].KINDTYPE.C_RecordElementDcl.SNextField = SymPtr + 1;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#lcount             " , 11 , IntegerPtr);
    STab[i].KINDTYPE.C_RecordElementDcl.SNextField = SymPtr + 1;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#llimit             " , 11 , IntegerPtr);
    STab[i].KINDTYPE.C_RecordElementDcl.SNextField = SymPtr + 1;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#fbuf               " , 11 , SaveSymPtr + -1);
    STab[i].KINDTYPE.C_RecordElementDcl.SNextField = -1;
    ElementFbuf = i;
    i = InitSym_InitSystem(&_ptoc_LOCAL, "                    " , 7 , -14);
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField = SaveSymPtr;
    STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant = -1;
    TypeIorechd = i;
    TypeSize(TypeIorechd);
    i = InitSym_InitSystem(&_ptoc_LOCAL, "#iorechd            " , 16 , TypeIorechd);
    TypePtrIorechd = InitSym_InitSystem(&_ptoc_LOCAL, "                    " , 7 , -17);
    STab[TypePtrIorechd].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = i;
    TypeSize(TypePtrIorechd);
    WFormatPtr = InitSym_InitSystem(&_ptoc_LOCAL, "#wformat            " , 7 , ShortPtr);
    STab[WFormatPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 1;
    STab[WFormatPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 4096;
    i = 0;
    do
        {
        id = InitSym_InitSystem(&_ptoc_LOCAL, SysSproc[i] , 15 , IntegerPtr);
        STab[id].KINDTYPE.C_SysProcDcl.SSysSpecific = i;
        if (i == 57)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    ProcMark = STab[SymPtr].SNext;
    LevelNo = 1;
    WithRecs[LevelNo] = -1;
    }

struct _ptoc_FRAME_ChkTypeDefn
  {
  char dummy;
  } ;


struct _ptoc_FRAME_EntTsym_ChkTypeDefn
  {
  struct _ptoc_FRAME_ChkTypeDefn *_ptoc_LINK;
  char dummy;
  } ;


static
int EntTsym_ChkTypeDefn(_ptoc_LINK, typ, card)
struct _ptoc_FRAME_ChkTypeDefn *_ptoc_LINK;
int typ;
int card;
    {
    struct _ptoc_FRAME_EntTsym_ChkTypeDefn _ptoc_LOCAL;
    int _EntTsym;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NewSymbol(0 , typ , 7 , LevelNo , card);
    _EntTsym = SymPtr;
    return(_EntTsym);
    }

int ChkTypeDefn(t, p1, p2, p3)
int t;
ParseStackRange p1;
ParseStackRange p2;
ParseStackRange p3;
    {
    struct _ptoc_FRAME_ChkTypeDefn _ptoc_LOCAL;
    int t1;
    int t2;
    int i;
    int j;
    int k;
    int id;
    int c1;
    int c2;
    SymbolTableRange SaveSymPtr;
    int ptoc_Var12;
    
    switch (t)
        {
        case -10 :
            SaveSymPtr = EntTsym_ChkTypeDefn(&_ptoc_LOCAL, t , CardNum[p1]);
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 0;
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = cnt[p2] + -1;
            STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SFirstEnum = STab[SymPtr].SNext;
            ptoc_Var12 = cnt[p2];
            if (1 <= ptoc_Var12)
                {
                k = 1;
                do
                    {
                    i = StartCnt[p2] + k;
                    id = IdList[i];
                    if (!duplicate(id , -1))
                        {
                        EntSymbol(id , SaveSymPtr , 8 , CardNum[p1]);
                        STab[SymPtr].KINDTYPE.C_ConDcl.SCon1val = k + -1;
                        STab[SymPtr].KINDTYPE.C_ConDcl.SNextEnum = STab[SymPtr].SNext;
                        }
                    if (k == ptoc_Var12)
                        {
                        break;
                        }
                    k += 1;
                    }
                while (!0);
                }
            STab[SymPtr].KINDTYPE.C_ConDcl.SNextEnum = -1;
            break;
        case 0 :
            c1 = ConstExp(g1[p1]);
            c2 = ConstExp(g1[p2]);
            val[p1] = c1;
            val[p2] = c2;
            ChkEqualType(p1 , p2);
            t1 = TypeType[p1];
            t1 = STab[t1].SType;
            if (t1 == -5)
                {
                if (control[113])
                    {
                    if (c1 < 0)
                        {
                        if (-128 <= c1 && c2 < 128)
                            {
                            TypeType[p1] = BytePtr;
                            }
                        else
                            {
                            if (-32768 <= c1 && c2 < 32768)
                                {
                                TypeType[p1] = ShortPtr;
                                }
                            }
                        }
                    else
                        {
                        if (c2 < 256)
                            {
                            TypeType[p1] = UbytePtr;
                            }
                        else
                            {
                            if (c2 < 32768)
                                {
                                TypeType[p1] = UshortPtr;
                                }
                            }
                        }
                    }
                }
            else
                {
                if (t1 != -10 && t1 != -24 && t1 != -7)
                    {
                    ErrorMsg(1 , ColNo[p1]);
                    }
                }
            if (c2 < c1)
                {
                ErrorMsg(1 , ColNo[p2]);
                }
            SaveSymPtr = EntTsym_ChkTypeDefn(&_ptoc_LOCAL, TypeType[p1] , CardNum[p1]);
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = c1;
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = c2;
            break;
        case -13 :
            if (!IsOrdinal(TypeType[p1]))
                {
                ErrorMsg(1 , ColNo[p1]);
                }
            SaveSymPtr = EntTsym_ChkTypeDefn(&_ptoc_LOCAL, t , CardNum[p1]);
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex = SubType[p1];
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = SubType[p2];
            if (val[p3] == 1)
                {
                STab[SaveSymPtr].SFlags |= 1024;
                }
            break;
        case -14 :
            SaveSymPtr = EntTsym_ChkTypeDefn(&_ptoc_LOCAL, t , CardNum[p1]);
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField = g1[p2];
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant = g2[p2];
            if (g2[p2] != -1)
                {
                STab[SaveSymPtr].STname = STab[NameofType[p2]].SName;
                }
            if (ContainsFile(g1[p2]) || ContainsFile(g2[p2]))
                {
                STab[SaveSymPtr].SFlags |= 128;
                }
            break;
        case -15 :
            SaveSymPtr = EntTsym_ChkTypeDefn(&_ptoc_LOCAL, -15 , CardNum[p1]);
            t1 = PrimType(SubType[p2]);
            if (IsFile(t1) || FileInRec(t1))
                {
                ErrorMsg(137 , ColNo[p2]);
                }
            STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent = SubType[p2];
            break;
        case -17 :
            id = val[p2];
            i = FindSymbol(id);
            if (i == -1 || i < ProcIndex && (STab[i].SFlags & 512) == 0)
                {
                EntSymbol(id , 0 , 16 , CardNum[p2]);
                if (i == -1)
                    {
                    STab[SymPtr].SFlags |= 64;
                    }
                else
                    {
                    if (STab[i].SKind == 16)
                        {
                        STab[SymPtr].SType = STab[i].SType;
                        }
                    else
                        {
                        STab[SymPtr].SFlags |= 64;
                        }
                    }
                STab[SymPtr].SFlags |= 512;
                i = SymPtr;
                }
            if (STab[i].SKind == 16)
                {
                SaveSymPtr = EntTsym_ChkTypeDefn(&_ptoc_LOCAL, -17 , CardNum[p2]);
                STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = i;
                }
            else
                {
                ErrorMsg(2 , ColNo[p2]);
                SaveSymPtr = IntegerPtr;
                }
            break;
        case -16 :
            if (IsOrdinal(TypeType[p2]))
                {
                if (SubType[p2] == IntegerPtr || SubType[p2] == ShortPtr || SubType[p2] == UshortPtr)
                    {
                    SaveSymPtr = FindIntset();
                    }
                else
                    {
                    SaveSymPtr = EntTsym_ChkTypeDefn(&_ptoc_LOCAL, -16 , CardNum[p1]);
                    STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase = SubType[p2];
                    }
                }
            else
                {
                ErrorMsg(1 , ColNo[p2]);
                SaveSymPtr = IntegerPtr;
                }
            break;
        case -20 :
            if (val[p1] == StringString)
                {
                if (TypeType[p2] == IntegerPtr)
                    {
                    i = ConstExp(g1[p2]);
                    if (i >= 1 && i <= 255)
                        {
                        SaveSymPtr = EntTsym_ChkTypeDefn(&_ptoc_LOCAL, -20 , CardNum[p1]);
                        STab[SaveSymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SLength = i + 1;
                        }
                    else
                        {
                        ErrorMsg(100 , ColNo[p2]);
                        SaveSymPtr = StringPtr;
                        }
                    }
                else
                    {
                    ErrorMsg(100 , ColNo[p2]);
                    SaveSymPtr = StringPtr;
                    }
                }
            else
                {
                ErrorMsg(1 , ColNo[p1]);
                SaveSymPtr = IntegerPtr;
                }
            break;
        }
    TypeSize(SaveSymPtr);
    SubType[p1] = SaveSymPtr;
    TypeType[p1] = PrimType(SaveSymPtr);
    NameofType[p1] = 0;
    }

struct _ptoc_FRAME_TypName
  {
  char dummy;
  } ;


int TypName(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_TypName _ptoc_LOCAL;
    int i;
    int id;
    int symp;
    
    id = val[p1];
    symp = SubType[p2];
    i = FindSymbol(id);
    if (i < ProcMark)
        {
        EntSymbol(id , symp , 16 , CardNum[p1]);
        STab[SymPtr].STname = NameofType[p2];
        }
    else
        {
        if (STab[i].SKind == 16)
            {
            if ((STab[i].SFlags & 512) != 0)
                {
                STab[i].SFlags &= -513;
                STab[i].SFlags &= -65;
                STab[i].SType = symp;
                STab[i].STname = NameofType[p2];
                }
            else
                {
                ErrorMsg(102 , ColNo[p1]);
                }
            }
        else
            {
            ErrorMsg(102 , ColNo[p2]);
            }
        }
    }

struct _ptoc_FRAME_CheckType
  {
  char dummy;
  } ;


int CheckType(p)
int p;
    {
    struct _ptoc_FRAME_CheckType _ptoc_LOCAL;
    int i;
    int s;
    
    i = FindSymbol(val[p]);
    s = i;
    if (i == -1)
        {
        ErrorMsg(2 , ColNo[p]);
        i = IntegerPtr;
        }
    else
        {
        if (STab[i].SKind != 16)
            {
            ErrorMsg(2 , ColNo[p]);
            i = IntegerPtr;
            }
        else
            {
            i = STab[i].SType;
            if (STab[i].SKind != 7)
                {
                ErrorMsg(2 , ColNo[p]);
                i = IntegerPtr;
                }
            }
        }
    SubType[p] = i;
    TypeType[p] = PrimType(i);
    NameofType[p] = s;
    }

struct _ptoc_FRAME_CheckForwardDefn
  {
  char dummy;
  } ;


int CheckForwardDefn(p)
int p;
    {
    struct _ptoc_FRAME_CheckForwardDefn _ptoc_LOCAL;
    
    if (val[p] == ExternalString)
        {
        if (STab[ProcIndex].SLevel == 1)
            {
            STab[ProcIndex].SFlags |= 32;
            }
        else
            {
            ErrorMsg(122 , -1);
            }
        }
    else
        {
        if (val[p] == ForwardString)
            {
            STab[ProcIndex].SFlags |= 64;
            }
        else
            {
            ErrorMsg(123 , ColNo[p]);
            }
        }
    if (STab[ProcIndex].SKind == 3)
        {
        STab[ProcIndex].SKind = 5;
        }
    else
        {
        if (STab[ProcIndex].SKind == 4)
            {
            STab[ProcIndex].SKind = 6;
            }
        else
            {
            ErrorMsg(102 , -1);
            }
        }
    }

struct _ptoc_FRAME_ProcDuplicate
  {
  char dummy;
  } ;


int ProcDuplicate(id, kind, result, p2)
int id;
KINDTYPE kind;
SymbolTableRange result;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_ProcDuplicate _ptoc_LOCAL;
    int _ProcDuplicate;
    int i;
    int j;
    int k;
    int proc;
    int count;
    int ptoc_Var13;
    int ptoc_Var14;
    int ptoc_Var15;
    
    _ProcDuplicate = 0;
    if (id == 0)
        {
        goto LAB_100;
        }
    proc = FindSymbol(id);
    count = cnt[p2];
    if (proc >= ProcMark)
        {
        if (STab[proc].SKind != 5 && STab[proc].SKind != 6)
            {
            ErrorMsg(102 , -1);
            goto LAB_100;
            }
        if (STab[proc].SKind == 5)
            {
            if (kind != 3)
                {
                ErrorMsg(101 , -1);
                }
            }
        else
            {
            if (kind != 4)
                {
                ErrorMsg(101 , -1);
                }
            }
        i = STab[proc].SType;
        if (result != STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult || count != 0 && count != STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt)
            {
            ErrorMsg(101 , -1);
            }
        if (LevelNo != STab[proc].SLevel)
            {
            ErrorMsg(6 , -1);
            }
        STab[proc].SFlags &= -65;
        NameSpace[StrTable[id + 3]].symbol = STab[proc].SLink;
        if (count == STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt)
            {
            j = STab[i].SNext;
            ptoc_Var15 = StartCnt[p2];
            ptoc_Var13 = ptoc_Var15 + 1;
            ptoc_Var14 = ptoc_Var15 + count;
            if (ptoc_Var13 <= ptoc_Var14)
                {
                k = ptoc_Var13;
                do
                    {
                    if (STab[j].SName != ParmId[k] || STab[j].SType != ParmType[k] || STab[j].KINDTYPE.C_ModuleDcl.SSubClass != sql__ord((int ) ParmPassedBy[k]))
                        {
                        ErrorMsg(101 , -1);
                        }
                    j = STab[j].SNext;
                    if (k == ptoc_Var14)
                        {
                        break;
                        }
                    k += 1;
                    }
                while (!0);
                }
            }
        else
            {
            if (count == 0)
                {
                count = STab[i].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
                cnt[p2] = count;
                j = STab[i].SNext;
                ptoc_Var14 = count;
                if (1 <= ptoc_Var14)
                    {
                    k = 1;
                    do
                        {
                        ParmPtrIncr();
                        ParmId[ParmPtr] = STab[j].SName;
                        ParmType[ParmPtr] = STab[j].SType;
                        ParmPassedBy[ParmPtr] = (unsigned char ) STab[j].KINDTYPE.C_ModuleDcl.SSubClass;
                        ParmNameofType[ParmPtr] = STab[j].STname;
                        j = STab[j].SNext;
                        if (k == ptoc_Var14)
                            {
                            break;
                            }
                        k += 1;
                        }
                    while (!0);
                    }
                }
            }
        _ProcDuplicate = proc;
        }
    LAB_100 :
    return(_ProcDuplicate);
    }

struct _ptoc_FRAME_DclParms
  {
  char dummy;
  } ;


int DclParms(p)
ParseStackRange p;
    {
    struct _ptoc_FRAME_DclParms _ptoc_LOCAL;
    int i;
    int id;
    int parm_kind;
    SymbolTableRange typ;
    int ptoc_Var16;
    int ptoc_Var17;
    struct ptoc_Type1 *ptoc_Var18;
    int ptoc_Var19;
    
    ptoc_Var19 = StartCnt[p];
    ptoc_Var16 = ptoc_Var19 + 1;
    ptoc_Var17 = ptoc_Var19 + cnt[p];
    if (ptoc_Var16 <= ptoc_Var17)
        {
        i = ptoc_Var16;
        do
            {
            id = ParmId[i];
            typ = ParmType[i];
            if (!duplicate(id , -1))
                {
                if (STab[typ].SType == -17)
                    {
                    switch (STab[STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain].SType)
                        {
                        case -18 :
                            parm_kind = 19;
                            break;
                        case -23 :
                            parm_kind = 20;
                            break;
                        default :
                            parm_kind = 14;
                            break;
                        }
                    }
                else
                    {
                    parm_kind = 14;
                    }
                EntSymbol(id , typ , parm_kind , CardNum[p]);
                STab[SymPtr].STname = ParmNameofType[i];
                ptoc_Var18 = &STab[SymPtr];
                ptoc_Var18->KINDTYPE.C_ModuleDcl.SClass = 7;
                ptoc_Var18->KINDTYPE.C_ModuleDcl.SSubClass = sql__ord((int ) ParmPassedBy[i]);
                ptoc_Var18->KINDTYPE.C_ModuleDcl.SParent = -1;
                }
            if (i == ptoc_Var17)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_DclProc
  {
  char dummy;
  } ;


int DclProc(p1, p2, kind, result)
ParseStackRange p1;
ParseStackRange p2;
KINDTYPE kind;
SymbolTableRange result;
    {
    struct _ptoc_FRAME_DclProc _ptoc_LOCAL;
    int _DclProc;
    int id;
    int TypePtr;
    SymbolTableRange fwp;
    struct ptoc_Type1 *ptoc_Var20;
    struct ptoc_Type1 *ptoc_Var21;
    
    id = val[p1];
    fwp = ProcDuplicate(id , kind , result , p2);
    EntSymbol(id , 0 , kind , CardNum[p1]);
    ptoc_Var20 = &STab[SymPtr];
    ptoc_Var20->SOrder = fwp;
    ptoc_Var20->SType = ptoc_Var20->SNext;
    if (LevelNo <= 1)
        {
        ptoc_Var20->KINDTYPE.C_ModuleDcl.SClass = 2;
        ptoc_Var20->KINDTYPE.C_ModuleDcl.SParent = -1;
        }
    else
        {
        ptoc_Var20->KINDTYPE.C_ModuleDcl.SClass = 3;
        ptoc_Var20->KINDTYPE.C_ModuleDcl.SParent = ProcIndex;
        }
    ptoc_Var20->KINDTYPE.C_ModuleDcl.SSubClass = 0;
    upscope(p1);
    if (kind == 3)
        {
        EntSymbol(0 , -23 , 7 , CardNum[p1]);
        }
    else
        {
        EntSymbol(0 , -18 , 7 , CardNum[p1]);
        }
    ptoc_Var21 = &STab[SymPtr];
    ptoc_Var21->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = result;
    ptoc_Var21->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt = cnt[p2];
    ptoc_Var21->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock = -1;
    TypePtr = SymPtr;
    parct = cnt[p2];
    SubType[p1] = result;
    TypeType[p1] = PrimType(result);
    NameofType[p1] = 0;
    ReturnedType = TypeType[p1];
    DclParms(p2);
    _DclProc = TypePtr;
    return(_DclProc);
    }

struct _ptoc_FRAME_EntParameterList
  {
  char dummy;
  } ;


struct _ptoc_FRAME_NewParm_EntParameterList
  {
  struct _ptoc_FRAME_EntParameterList *_ptoc_LINK;
  char dummy;
  } ;


static
int NewParm_EntParameterList(_ptoc_LINK, id, typ, tnam, PassedBy)
struct _ptoc_FRAME_EntParameterList *_ptoc_LINK;
int id;
int typ;
int tnam;
int PassedBy;
    {
    struct _ptoc_FRAME_NewParm_EntParameterList _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ParmPtrIncr();
    ParmId[ParmPtr] = id;
    ParmType[ParmPtr] = typ;
    ParmNameofType[ParmPtr] = tnam;
    ParmPassedBy[ParmPtr] = (unsigned char ) PassedBy;
    }

int EntParameterList(p1, p2, p3, col)
ParseStackRange p1;
ParseStackRange p2;
ParseStackRange p3;
int col;
    {
    struct _ptoc_FRAME_EntParameterList _ptoc_LOCAL;
    int _EntParameterList;
    int i;
    int j;
    int id;
    int PassedBy;
    STRING s;
    int typ;
    int tnam;
    int ptoc_Var22;
    int ptoc_Var23;
    int ptoc_Var24;
    int ptoc_Var25;
    
    PassedBy = TypeType[p1];
    typ = SubType[p3];
    tnam = NameofType[p3];
    ptoc_Var25 = StartCnt[p2];
    ptoc_Var22 = ptoc_Var25 + 1;
    ptoc_Var23 = ptoc_Var25 + cnt[p2];
    if (ptoc_Var22 <= ptoc_Var23)
        {
        i = ptoc_Var22;
        ptoc_Var24 = PassedBy == 1;
        do
            {
            NewParm_EntParameterList(&_ptoc_LOCAL, IdList[i] , typ , tnam , PassedBy);
            if (typ == AnyPacLenPtr)
                {
                GetString(IdList[i] , &s[0]);
                j = _GetStringLength(IdList[i]);
                s[j] = (unsigned char)'.';
                s[j + 1] = (unsigned char)'l';
                s[j + 2] = (unsigned char)'e';
                s[j + 3] = (unsigned char)'n';
                s[j + 4] = (unsigned char)'\0';
                id = EnterString(&s[0] , i + 4);
                if (StrTable[id + 3] == -1)
                    {
                    alloc_name_space(id);
                    }
                NewParm_EntParameterList(&_ptoc_LOCAL, id , IntegerPtr , 0 , 1);
                }
            if (ptoc_Var24 && (FileInRec(typ) || IsFile(typ)))
                {
                ErrorMsg(137 , col);
                }
            if (i == ptoc_Var23)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    if (typ == AnyPacLenPtr)
        {
        _EntParameterList = cnt[p2] * 2;
        }
    else
        {
        _EntParameterList = cnt[p2];
        }
    return(_EntParameterList);
    }

struct _ptoc_FRAME_FormalProcType
  {
  char dummy;
  } ;


int FormalProcType(p1, p2, prim_type, tnam, res_typ)
ParseStackRange p1;
ParseStackRange p2;
SymbolTableRange prim_type;
SymbolTableRange tnam;
SymbolTableRange res_typ;
    {
    struct _ptoc_FRAME_FormalProcType _ptoc_LOCAL;
    SymbolTableRange SaveSymPtr;
    struct ptoc_Type1 *ptoc_Var26;
    
    EntSymbol(0 , prim_type , 7 , CardNum[p1]);
    ptoc_Var26 = &STab[SymPtr];
    ptoc_Var26->STname = tnam;
    ptoc_Var26->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = res_typ;
    ptoc_Var26->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt = cnt[p2];
    ptoc_Var26->KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock = -1;
    SaveSymPtr = SymPtr;
    upscope(p1);
    DclParms(p2);
    PopScope(p1);
    EntSymbol(0 , -17 , 7 , CardNum[p1]);
    STab[SymPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = SaveSymPtr;
    TypeSize(SymPtr);
    TypeType[p1] = SymPtr;
    }

struct _ptoc_FRAME_EntProcParm
  {
  char dummy;
  } ;


int EntProcParm(p1)
ParseStackRange p1;
    {
    struct _ptoc_FRAME_EntProcParm _ptoc_LOCAL;
    int _EntProcParm;
    int i;
    int id;
    STRING s;
    SymbolTableRange SaveSymPtr;
    
    id = val[p1];
    ParmPtrIncr();
    ParmId[ParmPtr] = id;
    ParmType[ParmPtr] = TypeType[p1];
    ParmNameofType[ParmPtr] = 0;
    ParmPassedBy[ParmPtr] = (unsigned char)'\001';
    if (control[120])
        {
        GetString(id , &s[0]);
        i = _GetStringLength(id);
        s[i] = (unsigned char)'.';
        s[i + 1] = (unsigned char)'s';
        s[i + 2] = (unsigned char)'s';
        s[i + 3] = (unsigned char)'l';
        s[i + 4] = (unsigned char)'\0';
        id = EnterString(&s[0] , i + 4);
        if (StrTable[id + 3] == -1)
            {
            alloc_name_space(id);
            }
        ParmPtrIncr();
        ParmId[ParmPtr] = id;
        ParmType[ParmPtr] = IntegerPtr;
        ParmNameofType[ParmPtr] = 0;
        ParmPassedBy[ParmPtr] = (unsigned char)'\001';
        _EntProcParm = 2;
        }
    else
        {
        _EntProcParm = 1;
        }
    return(_EntProcParm);
    }

struct _ptoc_FRAME_IdLookup
  {
  char dummy;
  } ;


int IdLookup(p)
int p;
    {
    struct _ptoc_FRAME_IdLookup _ptoc_LOCAL;
    int _IdLookup;
    int i;
    
    i = FindSymbol(val[p]);
    if (i > -1)
        {
        switch (STab[i].SKind)
            {
            case 8 :
            case 9 :
            case 14 :
            case 11 :
                val[p] = i;
                SubType[p] = STab[i].SType;
                TypeType[p] = PrimType(SubType[p]);
                _IdLookup = i;
                break;
            case 4 :
            case 6 :
                val[p] = i;
                SubType[p] = STab[STab[i].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
                TypeType[p] = PrimType(SubType[p]);
                _IdLookup = i;
                break;
            case 19 :
                val[p] = i;
                SubType[p] = STab[STab[STab[i].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
                TypeType[p] = PrimType(SubType[p]);
                _IdLookup = i;
                break;
            case 3 :
            case 5 :
            case 20 :
                val[p] = i;
                SubType[p] = -21;
                TypeType[p] = -21;
                _IdLookup = i;
                break;
            case 16 :
                val[p] = i;
                SubType[p] = -21;
                TypeType[p] = -21;
                _IdLookup = i;
                break;
            default :
                val[p] = i;
                SubType[p] = -21;
                TypeType[p] = -21;
                _IdLookup = i;
                break;
            }
        }
    else
        {
        ErrorMsg(3 , ColNo[p]);
        val[p] = -1;
        TypeType[p] = -21;
        SubType[p] = -21;
        _IdLookup = -1;
        }
    return(_IdLookup);
    }

struct _ptoc_FRAME_QidLookup
  {
  char dummy;
  } ;


int QidLookup(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_QidLookup _ptoc_LOCAL;
    int _QidLookup;
    int i;
    
    if (STab[TypeType[p1]].SType != -14)
        {
        ErrorMsg(19 , ColNo[p1]);
        TypeType[p2] = IntegerPtr;
        }
    else
        {
        i = FindQSymbol(TypeType[p1] , val[p2]);
        if (i > -1)
            {
            val[p2] = i;
            SubType[p2] = STab[i].SType;
            TypeType[p2] = PrimType(SubType[p2]);
            _QidLookup = i;
            }
        else
            {
            ErrorMsg(3 , ColNo[p2]);
            TypeType[p2] = IntegerPtr;
            _QidLookup = -1;
            }
        }
    return(_QidLookup);
    }

struct _ptoc_FRAME_FindIntset
  {
  char dummy;
  } ;


int FindIntset()
    {
    struct _ptoc_FRAME_FindIntset _ptoc_LOCAL;
    int _FindIntset;
    SymbolTableRange i;
    SymbolTableRange j;
    
    i = NameSpace[StrTable[IntsetIndex + 3]].symbol;
    j = STab[i].SType;
    while (STab[j].SType != -16)
        {
        i = STab[i].SLink;
        j = STab[i].SType;
        }
    _FindIntset = j;
    return(_FindIntset);
    }

struct _ptoc_FRAME_CheckCaseConst
  {
  char dummy;
  } ;


int CheckCaseConst(p1)
int p1;
    {
    struct _ptoc_FRAME_CheckCaseConst _ptoc_LOCAL;
    int cval;
    int t1;
    int t2;
    int i;
    int j;
    int p;
    int NodeIndex;
    int low;
    int high;
    int ptoc_Var27;
    int ptoc_Var28;
    int ptoc_Var29;
    
    t1 = TagType[TagPtr];
    low = STab[t1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
    high = STab[t1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange;
    p = StartCnt[p1];
    ptoc_Var27 = cnt[p1];
    if (1 <= ptoc_Var27)
        {
        i = 1;
        do
            {
            t2 = ExpStype[p];
            if (!CompatibleTypes(PrimType(t1) , PrimType(t2)))
                {
                ErrorMsg(1 , ExpColNo[p]);
                }
            else
                {
                NodeIndex = ExpVal[p];
                cval = ConstExp(NodeIndex);
                if (!IsPrimConstant(NodeIndex))
                    {
                    NameofConst[p1] = GenConstName(cval);
                    }
                if (cval < low || cval > high)
                    {
                    ErrorMsg(135 , ExpColNo[p]);
                    }
                ptoc_Var28 = StartConst[TagPtr] + 1;
                ptoc_Var29 = ConstPtr;
                if (ptoc_Var28 <= ptoc_Var29)
                    {
                    j = ptoc_Var28;
                    do
                        {
                        if (cval == ConstVal[j])
                            {
                            ErrorMsg(131 , ExpColNo[p]);
                            }
                        if (j == ptoc_Var29)
                            {
                            break;
                            }
                        j += 1;
                        }
                    while (!0);
                    }
                IncrConstPtr();
                ConstVal[ConstPtr] = cval;
                }
            p += 2;
            if (i == ptoc_Var27)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }
