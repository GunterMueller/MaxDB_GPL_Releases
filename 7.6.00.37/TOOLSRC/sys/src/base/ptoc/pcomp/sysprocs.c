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

int big_goto;
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
int shl();
int shr();
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
int UtilitiesI();
int PasCerror();
int BaseType();
int CopyTree();
int demorgan();
int GetTemporary();
int FreeTemporary();
int ClearTemporarys();
int BasisTyp();
int IsConstant();
int FoldUnary();
int FoldBinary();
int FoldSubscript();
int FoldTree();
typedef
  int ParseStackRange;
typedef
  int ParseStack[1001];
typedef
  int IntExpList[1001];
SymbolTableRange IntegerPtr;
SymbolTableRange BooleanPtr;
SymbolTableRange CharPtr;
SymbolTableRange TextPtr;
SymbolTableRange PacPtr;
SymbolTableRange LongPtr;
SymbolTableRange FloatPtr;
SymbolTableRange DoublePtr;
SymbolTableRange BytePtr;
SymbolTableRange ShortPtr;
SymbolTableRange StringPtr;
int ProcIndex;
ParseStack StartCnt;
ParseStack cnt;
ParseStack g1;
ParseStack TypeType;
ParseStack SubType;
ParseStack CardNum;
ParseStack ColNo;
IntExpList ExpType;
IntExpList ExpStype;
IntExpList ExpColNo;
IntExpList ExpVal;
int CardCount;
int CurrentFile;
int asm_esc;
SymbolTableRange InputPtr;
SymbolTableRange OutputPtr;
SymbolTableRange ErrorPtr;
SymbolTableRange _ArgcPtr;
SymbolTableRange TypeIorechd;
SymbolTableRange TypePtrIorechd;
SymbolTableRange TypeFbuf;
SymbolTableRange ElementFbuf;
SymbolTableRange AnyPtrPtr;
SymbolTableRange PtrToCharPtr;
int OctString;
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
int StartupInit();
int OutputInit();
int TypeSize();
int P1m2I();
int ErrorMsg();
int DclError();
int werror();
int FatalErrorMsg();
int WarnStrCmp();
unsigned char SysSproc[58][20];
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
typedef
  unsigned char String20[20];
typedef
  unsigned char IOcontrolType[200];

struct _ptoc_FRAME_SysprocsI
  {
  char dummy;
  } ;


int SysprocsI()
    {
    struct _ptoc_FRAME_SysprocsI _ptoc_LOCAL;
    
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[0], "reset\0              ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[1], "rewrite\0            ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[2], "read\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[3], "write\0              ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[4], "flush\0              ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[5], "remove\0             ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[6], "message\0            ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[7], "ord\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[8], "chr\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[9], "readln\0             ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[10], "writeln\0            ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[11], "eoln\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[12], "eof\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[13], "argc\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[14], "argv\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[15], "new\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[16], "dispose\0            ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[17], "put\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[18], "get\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[19], "page\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[20], "abs\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[21], "arctan\0             ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[22], "cos\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[23], "exp\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[24], "ln\0                 ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[25], "odd\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[26], "pack\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[27], "pred\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[28], "round\0              ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[29], "sin\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[30], "sqr\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[31], "sqrt\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[32], "succ\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[33], "trunc\0              ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[34], "unpack\0             ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[35], "halt\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[36], "clock\0              ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[37], "sysclock\0           ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[38], "wallclock\0          ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[39], "ptr\0                ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[40], "date\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[41], "time\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[42], "linelimit\0          ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[43], "null\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[44], "card\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[45], "expo\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[46], "random\0             ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[47], "seed\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[48], "undefined\0          ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[49], "length\0             ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[50], "concat\0             ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[51], "copy\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[52], "ord2\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[53], "ord4\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[54], "sizeof\0             ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[55], "close\0              ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[56], "envp\0               ")
    _ptoc_ASSIGN(unsigned char , 20, SysSproc[57], "asm\0                ")
    }

struct _ptoc_FRAME_IsProcedure
  {
  char dummy;
  } ;


int IsProcedure(sysproc)
int sysproc;
    {
    struct _ptoc_FRAME_IsProcedure _ptoc_LOCAL;
    int _IsProcedure;
    
    switch (sysproc)
        {
        case 43 :
        case 42 :
        case 41 :
        case 40 :
        case 35 :
        case 26 :
        case 34 :
        case 19 :
        case 18 :
        case 17 :
        case 55 :
        case 16 :
        case 15 :
        case 14 :
        case 56 :
        case 57 :
        case 5 :
        case 4 :
        case 3 :
        case 10 :
        case 6 :
        case 2 :
        case 9 :
        case 1 :
        case 0 :
            _IsProcedure = 1;
            break;
        default :
            _IsProcedure = 0;
            break;
        }
    return(_IsProcedure);
    }

struct _ptoc_FRAME_CreateEnumBlock
  {
  char dummy;
  } ;


int CreateEnumBlock(enum_typ)
SymbolTableRange enum_typ;
    {
    struct _ptoc_FRAME_CreateEnumBlock _ptoc_LOCAL;
    int _CreateEnumBlock;
    SymbolTableRange i;
    SymbolTableRange enum_block;
    SymbolTableRange enum_block_type;
    SymbolTableRange first_member;
    SymbolTableRange last_member;
    int n;
    int offset;
    int string_size;
    int ptoc_Var0;
    int ptoc_Var1;
    
    StartupInit();
    NewSymbol(0 , -22 , 7 , 1 , -1);
    enum_block_type = SymPtr;
    NewSymbol(0 , enum_block_type , 17 , 1 , -1);
    enum_block = SymPtr;
    STab[enum_block].KINDTYPE.C_ModuleDcl.SClass = 4;
    STab[enum_block].KINDTYPE.C_ModuleDcl.SSubClass = 4;
    STab[enum_block].KINDTYPE.C_ModuleDcl.SParent = -1;
    n = STab[enum_typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange + 1;
    first_member = enum_typ + 1;
    last_member = enum_typ + n;
    OutputInit(enum_block , node3(12 , IntegerPtr , n , 0));
    offset = 32;
    string_size = 0;
    ptoc_Var0 = first_member;
    ptoc_Var1 = last_member;
    if (ptoc_Var0 <= ptoc_Var1)
        {
        i = ptoc_Var0;
        do
            {
            OutputInit(enum_block , UnaryExpression(93 , PtrToCharPtr , node3(14 , PtrToCharPtr , STab[i].SName , 0) , 0 , 0));
            offset += 32;
            string_size = string_size + _GetStringLength(STab[i].SName) + 1;
            if (i == ptoc_Var1)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    STab[enum_block_type].KINDTYPE.C_TypeDcl.C_TypeDcl.C_BlockType.SBlockLen = offset / 8 + string_size;
    STab[enum_block_type].KINDTYPE.C_TypeDcl.C_TypeDcl.C_BlockType.SElemAlign = 32;
    TypeSize(enum_block_type);
    _CreateEnumBlock = enum_block;
    return(_CreateEnumBlock);
    }

struct _ptoc_FRAME_EnterConstantString
  {
  char dummy;
  } ;


int EnterConstantString(ptoc_ptr_strn)
String20 ptoc_ptr_strn;
    {
    struct _ptoc_FRAME_EnterConstantString _ptoc_LOCAL;
    int _EnterConstantString;
    int i;
    String20 strn;
    
    _ptoc_MOVE(char, sizeof(String20 ), strn, ptoc_ptr_strn);
    i = -1;
    do
        i += 1;
    while (!((int ) strn[i] == 32));
    _EnterConstantString = EnterString(&strn[1] , i + -1);
    return(_EnterConstantString);
    }

struct _ptoc_FRAME_EnterNewLine
  {
  char dummy;
  } ;


int EnterNewLine()
    {
    struct _ptoc_FRAME_EnterNewLine _ptoc_LOCAL;
    int _EnterNewLine;
    String20 s;
    
    s[0] = (unsigned char)'\n';
    _EnterNewLine = EnterString(&s[0] , 1);
    return(_EnterNewLine);
    }

struct _ptoc_FRAME_MakeVal
  {
  char dummy;
  } ;


int MakeVal(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_MakeVal _ptoc_LOCAL;
    int _MakeVal;
    
    _MakeVal = UnaryExpression(8 , field[NodeIndex + 1] , NodeIndex , 0 , 0);
    return(_MakeVal);
    }

struct _ptoc_FRAME_MakeRef
  {
  char dummy;
  } ;


int MakeRef(NodeIndex)
HdgRange *NodeIndex;
    {
    struct _ptoc_FRAME_MakeRef _ptoc_LOCAL;
    SymbolTableRange tempvar;
    HdgRange temp;
    struct ptoc_Type1 *ptoc_Var2;
    struct ptoc_Type1 *ptoc_Var3;
    int ptoc_Var4;
    
    if ((field[*NodeIndex] & 65535) == 4)
        {
        SetMinor(*NodeIndex , 9);
        SaveIndex = *NodeIndex;
        *NodeIndex = field[*NodeIndex + 5];
        if ((field[*NodeIndex] & 65535) == 94)
            {
            ptoc_Var2 = &STab[field[*NodeIndex + 5]];
            if (ptoc_Var2->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var2->KINDTYPE.C_ModuleDcl.SClass == 5)
                {
                ptoc_Var2->KINDTYPE.C_ModuleDcl.SClass = 10;
                }
            }
        *NodeIndex = SaveIndex;
        }
    else
        {
        if ((field[*NodeIndex] & 65535) == 12)
            {
            SetMinor(*NodeIndex , 14);
            }
        else
            {
            if ((field[*NodeIndex] & 65535) == 40)
                {
                tempvar = GetTemporary(ProcIndex , field[*NodeIndex + 1] , 0);
                temp = node3(94 , field[*NodeIndex + 1] , tempvar , 0);
                ptoc_Var4 = field[*NodeIndex + 1];
                *NodeIndex = UnaryExpression(9 , ptoc_Var4 , LongStatement(6 , ptoc_Var4 , temp , *NodeIndex , 0 , 0 , -1) , 0 , 0);
                ptoc_Var3 = &STab[tempvar];
                if (ptoc_Var3->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var3->KINDTYPE.C_ModuleDcl.SClass == 5)
                    {
                    ptoc_Var3->KINDTYPE.C_ModuleDcl.SClass = 10;
                    }
                }
            }
        }
    }

struct _ptoc_FRAME_StringElement
  {
  char dummy;
  } ;


int StringElement(NodeIndex, element)
HdgRange NodeIndex;
int element;
    {
    struct _ptoc_FRAME_StringElement _ptoc_LOCAL;
    int _StringElement;
    
    NodeIndex = BinaryExpression(95 , CharPtr , field[NodeIndex + 5] , node3(12 , IntegerPtr , element , 0));
    field[NodeIndex + 8] = 0;
    FoldSubscript(NodeIndex);
    _StringElement = UnaryExpression(4 , CharPtr , NodeIndex , 0 , 0);
    return(_StringElement);
    }

struct _ptoc_FRAME_CallPcstart
  {
  char dummy;
  } ;


int CallPcstart(mode, card)
int mode;
int card;
    {
    struct _ptoc_FRAME_CallPcstart _ptoc_LOCAL;
    int _CallPcstart;
    
    _CallPcstart = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , MakeVal(node3(12 , IntegerPtr , mode , 0)) , -1 , -1) , EnterConstantString("_sql__pmain         ") , 1 , card);
    return(_CallPcstart);
    }

struct _ptoc_FRAME_CallPcexit
  {
  char dummy;
  } ;


int CallPcexit(card)
int card;
    {
    struct _ptoc_FRAME_CallPcexit _ptoc_LOCAL;
    int _CallPcexit;
    
    _CallPcexit = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , MakeVal(node3(12 , IntegerPtr , 0 , 0)) , -1 , -1) , EnterConstantString("_sql__exit          ") , 1 , card);
    return(_CallPcexit);
    }

struct _ptoc_FRAME_CallUnit
  {
  char dummy;
  } ;


int CallUnit(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CallUnit _ptoc_LOCAL;
    int _CallUnit;
    
    _CallUnit = LongStatement(62 , TypePtrIorechd , -1 , MakeList(6 , field[NodeIndex + 1] , NodeIndex , -1 , -1) , EnterConstantString("_sql__ut            ") , 1 , CardCount);
    return(_CallUnit);
    }

struct _ptoc_FRAME_CallFnil
  {
  char dummy;
  } ;


int CallFnil(NodeIndex, ComponentType)
HdgRange NodeIndex;
SymbolTableRange ComponentType;
    {
    struct _ptoc_FRAME_CallFnil _ptoc_LOCAL;
    int _CallFnil;
    int ptoc_Var5;
    
    ptoc_Var5 = field[NodeIndex + 1];
    _CallFnil = UnaryExpression(97 , ComponentType , LongStatement(62 , ptoc_Var5 , -1 , MakeList(6 , ptoc_Var5 , NodeIndex , -1 , -1) , EnterConstantString("_sql__nlf           ") , 1 , CardCount) , 0 , 0);
    return(_CallFnil);
    }

struct _ptoc_FRAME_RefFile
  {
  char dummy;
  } ;


int RefFile(PtrToFile)
SymbolTableRange PtrToFile;
    {
    struct _ptoc_FRAME_RefFile _ptoc_LOCAL;
    int _RefFile;
    
    _RefFile = UnaryExpression(4 , TypePtrIorechd , node3(94 , TypePtrIorechd , PtrToFile , 0) , 0 , 0);
    return(_RefFile);
    }

struct _ptoc_FRAME_RefFileDotFbuf
  {
  char dummy;
  } ;


int RefFileDotFbuf(PtrToFile)
SymbolTableRange PtrToFile;
    {
    struct _ptoc_FRAME_RefFileDotFbuf _ptoc_LOCAL;
    int _RefFileDotFbuf;
    HdgRange FileVar;
    
    FileVar = UnaryExpression(97 , TypeIorechd , UnaryExpression(4 , TypePtrIorechd , node3(94 , TypePtrIorechd , PtrToFile , 0) , 0 , 0) , 0 , 0);
    FileVar = BinaryExpression(96 , TypeFbuf , FileVar , node3(70 , IntegerPtr , ElementFbuf , 0));
    FileVar = UnaryExpression(4 , TypeFbuf , FileVar , 0 , 0);
    _RefFileDotFbuf = FileVar;
    return(_RefFileDotFbuf);
    }

struct _ptoc_FRAME_CallMax
  {
  char dummy;
  } ;


int CallMax(width, reduce, min, col)
HdgRange width;
HdgRange reduce;
HdgRange min;
int col;
    {
    struct _ptoc_FRAME_CallMax _ptoc_LOCAL;
    int _CallMax;
    int width_lit;
    int ptoc_Var6;
    
    if (((ptoc_Var6 = field[width] % 16384) >= 0 ? ptoc_Var6 : ptoc_Var6 + 16384) == 12)
        {
        width_lit = field[width + 5];
        if (width_lit <= 0)
            {
            ErrorMsg(13 , col);
            _CallMax = min;
            }
        else
            {
            width_lit -= field[reduce + 5];
            if (width_lit >= field[min + 5])
                {
                _CallMax = width;
                }
            else
                {
                _CallMax = min;
                }
            }
        }
    else
        {
        _CallMax = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , width , reduce , min) , EnterConstantString("_sql__max           ") , 3 , CardCount);
        }
    return(_CallMax);
    }

struct _ptoc_FRAME_CallNam
  {
  char dummy;
  } ;


int CallNam(item, enumblk)
HdgRange item;
HdgRange enumblk;
    {
    struct _ptoc_FRAME_CallNam _ptoc_LOCAL;
    int _CallNam;
    
    _CallNam = LongStatement(62 , AnyPtrPtr , -1 , MakeList(6 , IntegerPtr , EvalConvert(IntegerPtr , item) , enumblk , -1) , EnterConstantString("_sql__nname         ") , 2 , CardCount);
    return(_CallNam);
    }

struct _ptoc_FRAME_CallStrlen
  {
  char dummy;
  } ;


int CallStrlen(str)
HdgRange str;
    {
    struct _ptoc_FRAME_CallStrlen _ptoc_LOCAL;
    int _CallStrlen;
    
    _CallStrlen = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , AnyPtrPtr , str , -1 , -1) , EnterConstantString("_strlen             ") , 1 , CardCount);
    return(_CallStrlen);
    }

struct _ptoc_FRAME_CallPut
  {
  char dummy;
  } ;


int CallPut(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CallPut _ptoc_LOCAL;
    int _CallPut;
    
    _CallPut = LongStatement(62 , TypePtrIorechd , -1 , MakeList(6 , field[NodeIndex + 1] , NodeIndex , -1 , -1) , EnterConstantString("_sql__put           ") , 1 , CardCount);
    return(_CallPut);
    }

struct _ptoc_FRAME_CallGet
  {
  char dummy;
  } ;


int CallGet(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CallGet _ptoc_LOCAL;
    int _CallGet;
    
    _CallGet = LongStatement(62 , TypePtrIorechd , -1 , MakeList(6 , field[NodeIndex + 1] , NodeIndex , -1 , -1) , EnterConstantString("_sql__get           ") , 1 , CardCount);
    return(_CallGet);
    }

struct _ptoc_FRAME_CallUGet
  {
  char dummy;
  } ;


int CallUGet(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CallUGet _ptoc_LOCAL;
    int _CallUGet;
    
    _CallUGet = LongStatement(62 , TypePtrIorechd , -1 , MakeList(6 , field[NodeIndex + 1] , NodeIndex , -1 , -1) , EnterConstantString("_sql__unget         ") , 1 , CardCount);
    return(_CallUGet);
    }

struct _ptoc_FRAME_CallPage
  {
  char dummy;
  } ;


int CallPage(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CallPage _ptoc_LOCAL;
    int _CallPage;
    
    _CallPage = LongStatement(62 , TypePtrIorechd , -1 , MakeList(6 , field[NodeIndex + 1] , NodeIndex , -1 , -1) , EnterConstantString("_sql__npa           ") , 1 , CardCount);
    return(_CallPage);
    }

struct _ptoc_FRAME_CallCompare
  {
  int variants;
  int gapptr;
  struct ptoc_Type2
    {
    int GCnt;
    int GOffset;
    } gaps[200];
  char dummy;
  } ;
typedef
  struct ptoc_Type2 gaprecord_CallCompare;


struct _ptoc_FRAME_Incrgapptr_CallCompare
  {
  struct _ptoc_FRAME_CallCompare *_ptoc_LINK;
  char dummy;
  } ;


static
int Incrgapptr_CallCompare(_ptoc_LINK)
struct _ptoc_FRAME_CallCompare *_ptoc_LINK;
    {
    struct _ptoc_FRAME_Incrgapptr_CallCompare _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _ptoc_LOCAL._ptoc_LINK->gapptr += 1;
    if (_ptoc_LOCAL._ptoc_LINK->gapptr > 200)
        {
        PasCerror("CallCompare: Stack exceeded                                                                                                                                                                                                                                     ");
        }
    }

struct _ptoc_FRAME_FindGaps_CallCompare
  {
  struct _ptoc_FRAME_CallCompare *_ptoc_LINK;
  char dummy;
  } ;


static
int FindGaps_CallCompare(_ptoc_LINK, p, TotalOffset)
struct _ptoc_FRAME_CallCompare *_ptoc_LINK;
SymbolTableRange p;
int *TotalOffset;
    {
    struct _ptoc_FRAME_FindGaps_CallCompare _ptoc_LOCAL;
    SymbolTableRange Index;
    int RecOffset;
    int RecSize;
    int TotalSize;
    MACHDEP *ptoc_Var7;
    gaprecord_CallCompare *ptoc_Var8;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    RecSize = STab[p].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize + *TotalOffset;
    Index = STab[p].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField;
    _ptoc_LOCAL._ptoc_LINK->variants = STab[p].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant != -1;
    if (STab[STab[Index].SType].SType == -14)
        {
        FindGaps_CallCompare(_ptoc_LOCAL._ptoc_LINK, STab[Index].SType , &(*TotalOffset));
        }
    RecOffset = *TotalOffset;
    TotalSize = *TotalOffset + STab[Index].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize;
    while (STab[Index].KINDTYPE.C_RecordElementDcl.SNextField != -1)
        {
        Index = STab[Index].KINDTYPE.C_RecordElementDcl.SNextField;
        ptoc_Var7 = &STab[Index].KINDTYPE.C_TypeDcl.TypeInfo;
        if (TotalSize < ptoc_Var7->KINDTYPE.C_TypeDcl.SOffset + RecOffset)
            {
            _ptoc_LOCAL._ptoc_LINK->gapptr += 1;
            _ptoc_LOCAL._ptoc_LINK->gaps[_ptoc_LOCAL._ptoc_LINK->gapptr + -1].GCnt = (TotalSize - *TotalOffset) / 8;
            _ptoc_LOCAL._ptoc_LINK->gaps[_ptoc_LOCAL._ptoc_LINK->gapptr + -1].GOffset = *TotalOffset / 8;
            *TotalOffset = ptoc_Var7->KINDTYPE.C_TypeDcl.SOffset + RecOffset;
            if (STab[STab[Index].SType].SType == -14)
                {
                FindGaps_CallCompare(_ptoc_LOCAL._ptoc_LINK, STab[Index].SType , &(*TotalOffset));
                }
            TotalSize = *TotalOffset + ptoc_Var7->KINDTYPE.C_TypeDcl.SSize;
            }
        else
            {
            if (STab[STab[Index].SType].SType == -14)
                {
                FindGaps_CallCompare(_ptoc_LOCAL._ptoc_LINK, STab[Index].SType , &(*TotalOffset));
                }
            TotalSize += ptoc_Var7->KINDTYPE.C_TypeDcl.SSize;
            }
        }
    if (*TotalOffset < RecSize)
        {
        _ptoc_LOCAL._ptoc_LINK->gapptr += 1;
        ptoc_Var8 = &_ptoc_LOCAL._ptoc_LINK->gaps[_ptoc_LOCAL._ptoc_LINK->gapptr + -1];
        ptoc_Var8->GCnt = (TotalSize - *TotalOffset) / 8;
        ptoc_Var8->GOffset = *TotalOffset / 8;
        *TotalOffset = RecSize + RecOffset;
        }
    }

int CallCompare(p1, p2)
SymbolTableRange p1;
SymbolTableRange p2;
    {
    struct _ptoc_FRAME_CallCompare _ptoc_LOCAL;
    int _CallCompare;
    HdgRange Exp1;
    HdgRange Exp2;
    HdgRange len;
    HdgRange temp;
    HdgRange NodeIndex;
    int i;
    int s;
    HdgRange offset;
    HdgRange t1;
    HdgRange t2;
    HdgRange TmpExp1;
    HdgRange TmpExp2;
    struct ptoc_Type1 *ptoc_Var9;
    struct ptoc_Type1 *ptoc_Var10;
    int ptoc_Var11;
    
    Exp1 = g1[p1];
    Exp2 = g1[p2];
    MakeRef(&Exp1);
    MakeRef(&Exp2);
    NodeIndex = Exp1;
    s = EnterConstantString("_sql__compare       ");
    if (STab[TypeType[p1]].SType == -14)
        {
        _ptoc_LOCAL.gapptr = 0;
        _ptoc_LOCAL.variants = 0;
        i = 0;
        FindGaps_CallCompare(&_ptoc_LOCAL, TypeType[p1] , &i);
        if (_ptoc_LOCAL.variants)
            {
            ErrorMsg(41 , ColNo[p2]);
            }
        TmpExp1 = CopyTree(Exp1);
        TmpExp2 = CopyTree(Exp2);
        temp = MakeList(6 , field[NodeIndex + 1] , Exp1 , Exp2 , -1);
        len = node3(12 , IntegerPtr , _ptoc_LOCAL.gaps[0].GCnt , 0);
        offset = node3(12 , IntegerPtr , _ptoc_LOCAL.gaps[0].GOffset , 0);
        AppendList(6 , IntegerPtr , temp , len);
        AppendList(6 , IntegerPtr , temp , offset);
        t1 = LongStatement(62 , IntegerPtr , -1 , temp , s , 4 , CardCount);
        if (_ptoc_LOCAL.gapptr > 1)
            {
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 6)
                {
                Exp1 = UnaryExpression(9 , field[NodeIndex + 1] , field[NodeIndex + 5] , 0 , 0);
                SaveIndex = NodeIndex;
                NodeIndex = Exp1;
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
                }
            else
                {
                Exp1 = TmpExp1;
                }
            NodeIndex = Exp2;
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 6)
                {
                Exp2 = UnaryExpression(9 , field[NodeIndex + 1] , field[NodeIndex + 5] , 0 , 0);
                SaveIndex = NodeIndex;
                NodeIndex = Exp2;
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
                }
            else
                {
                Exp2 = TmpExp2;
                }
            temp = MakeList(6 , field[NodeIndex + 1] , Exp1 , Exp2 , -1);
            ptoc_Var11 = _ptoc_LOCAL.gapptr;
            if (2 <= ptoc_Var11)
                {
                i = 2;
                do
                    {
                    len = node3(12 , IntegerPtr , _ptoc_LOCAL.gaps[i + -1].GCnt , 0);
                    offset = node3(12 , IntegerPtr , _ptoc_LOCAL.gaps[i + -1].GOffset , 0);
                    t2 = CopyTree(temp);
                    AppendList(6 , IntegerPtr , t2 , len);
                    AppendList(6 , IntegerPtr , t2 , offset);
                    t2 = LongStatement(62 , IntegerPtr , -1 , t2 , s , 4 , CardCount);
                    t1 = BinaryExpression(19 , IntegerPtr , t1 , t2);
                    if (i == ptoc_Var11)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                }
            }
        _CallCompare = t1;
        }
    else
        {
        temp = MakeList(6 , field[NodeIndex + 1] , Exp1 , Exp2 , -1);
        if (IsCharArray(field[NodeIndex + 1]))
            {
            len = node3(12 , IntegerPtr , LengthOfCharArray(field[NodeIndex + 1]) , 0);
            }
        else
            {
            len = node3(69 , IntegerPtr , field[NodeIndex + 1] , 0);
            }
        offset = node3(12 , IntegerPtr , 0 , 0);
        AppendList(6 , IntegerPtr , temp , len);
        AppendList(6 , IntegerPtr , temp , offset);
        _CallCompare = LongStatement(62 , IntegerPtr , -1 , temp , s , 4 , CardCount);
        }
    return(_CallCompare);
    }

struct _ptoc_FRAME_CallMath
  {
  char dummy;
  } ;


int CallMath(ptoc_ptr_routine, NodeIndex)
String20 ptoc_ptr_routine;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CallMath _ptoc_LOCAL;
    int _CallMath;
    String20 routine;
    
    _ptoc_MOVE(char, sizeof(String20 ), routine, ptoc_ptr_routine);
    _CallMath = LongStatement(62 , DoublePtr , -1 , MakeList(6 , DoublePtr , EvalConvert(DoublePtr , NodeIndex) , -1 , -1) , EnterConstantString(routine) , 1 , CardCount);
    return(_CallMath);
    }

struct _ptoc_FRAME_CallSetOp
  {
  char dummy;
  } ;


int CallSetOp(op, typ, res, left, right)
int op;
SymbolTableRange typ;
HdgRange res;
HdgRange left;
HdgRange right;
    {
    struct _ptoc_FRAME_CallSetOp _ptoc_LOCAL;
    int _CallSetOp;
    HdgRange size;
    HdgRange temp;
    int routine;
    HdgRange NodeIndex;
    struct ptoc_Type1 *ptoc_Var12;
    
    if (control[110])
        {
        switch (op)
            {
            case 19 :
                routine = EnterConstantString("_sql__psetadd       ");
                break;
            case 20 :
                routine = EnterConstantString("_sql__setsub        ");
                break;
            case 21 :
                routine = EnterConstantString("_sql__psetm         ");
                break;
            }
        }
    else
        {
        switch (op)
            {
            case 19 :
                routine = EnterConstantString("_sql__setadd        ");
                break;
            case 20 :
                routine = EnterConstantString("_sql__setmin        ");
                break;
            case 21 :
                routine = EnterConstantString("_sql__multi         ");
                break;
            }
        }
    res = UnaryExpression(9 , typ , res , 0 , 0);
    SaveIndex = NodeIndex;
    NodeIndex = res;
    NodeIndex = field[NodeIndex + 5];
    if ((field[NodeIndex] & 65535) == 94)
        {
        ptoc_Var12 = &STab[field[NodeIndex + 5]];
        if (ptoc_Var12->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var12->KINDTYPE.C_ModuleDcl.SClass == 5)
            {
            ptoc_Var12->KINDTYPE.C_ModuleDcl.SClass = 10;
            }
        }
    NodeIndex = SaveIndex;
    MakeRef(&left);
    MakeRef(&right);
    size = UnaryExpression(8 , IntegerPtr , node3(69 , IntegerPtr , typ , 0) , 0 , 0);
    temp = MakeList(6 , typ , -1 , -1 , -1);
    AppendList(6 , typ , temp , res);
    AppendList(6 , typ , temp , left);
    AppendList(6 , typ , temp , right);
    AppendList(6 , typ , temp , size);
    _CallSetOp = LongStatement(62 , AnyPtrPtr , -1 , temp , routine , 4 , CardCount);
    return(_CallSetOp);
    }

struct _ptoc_FRAME_CallSetRel
  {
  char dummy;
  } ;


int CallSetRel(op, typ, left, right)
int op;
SymbolTableRange typ;
HdgRange left;
HdgRange right;
    {
    struct _ptoc_FRAME_CallSetRel _ptoc_LOCAL;
    int _CallSetRel;
    HdgRange size;
    HdgRange temp;
    int routine;
    
    if (control[110])
        {
        switch (op)
            {
            case 31 :
                routine = EnterConstantString("_sql__erq           ");
                break;
            case 36 :
                routine = EnterConstantString("_sql__nre           ");
                break;
            case 35 :
                routine = EnterConstantString("_sql__psetge        ");
                break;
            case 33 :
                routine = EnterConstantString("_sql__psetgt        ");
                break;
            case 32 :
                routine = EnterConstantString("_sql__psetlt        ");
                break;
            case 34 :
                routine = EnterConstantString("_sql__psetle        ");
                break;
            }
        }
    else
        {
        switch (op)
            {
            case 31 :
                routine = EnterConstantString("_sql__erq           ");
                break;
            case 36 :
                routine = EnterConstantString("_sql__nre           ");
                break;
            case 35 :
                routine = EnterConstantString("_sql__ger           ");
                break;
            case 33 :
                routine = EnterConstantString("_sql__gtl           ");
                break;
            case 32 :
                routine = EnterConstantString("_sql__ltl           ");
                break;
            case 34 :
                routine = EnterConstantString("_sql__lel           ");
                break;
            }
        }
    MakeRef(&left);
    MakeRef(&right);
    size = UnaryExpression(8 , IntegerPtr , node3(69 , IntegerPtr , typ , 0) , 0 , 0);
    temp = MakeList(6 , typ , -1 , -1 , -1);
    AppendList(6 , typ , temp , size);
    AppendList(6 , typ , temp , left);
    AppendList(6 , typ , temp , right);
    _CallSetRel = LongStatement(62 , BooleanPtr , -1 , temp , routine , 3 , CardCount);
    return(_CallSetRel);
    }

struct _ptoc_FRAME_CallInct
  {
  char dummy;
  } ;


int CallInct(element, ct)
HdgRange element;
HdgRange ct;
    {
    struct _ptoc_FRAME_CallInct _ptoc_LOCAL;
    int _CallInct;
    HdgRange temp;
    
    temp = MakeList(6 , IntegerPtr , -1 , -1 , -1);
    AppendList(6 , IntegerPtr , temp , MakeVal(element));
    LinkLists(temp , ct);
    _CallInct = LongStatement(62 , BooleanPtr , -1 , temp , EnterConstantString("_sql__ic            ") , -1 , CardCount);
    return(_CallInct);
    }

struct _ptoc_FRAME_CallIntor
  {
  char dummy;
  } ;


int CallIntor(element, NodeIndex)
HdgRange element;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CallIntor _ptoc_LOCAL;
    int _CallIntor;
    HdgRange tmp1;
    HdgRange tmp2;
    HdgRange res;
    SymbolTableRange indx_sym;
    
    if (field[NodeIndex + 7] == 0)
        {
        _CallIntor = node3(12 , BooleanPtr , 0 , 0);
        }
    else
        {
        if (!IsLval(element))
            {
            indx_sym = GetTemporary(ProcIndex , IntegerPtr , 0);
            tmp2 = node3(94 , IntegerPtr , indx_sym , 0);
            element = LongStatement(6 , IntegerPtr , tmp2 , element , 0 , 0 , -1);
            }
        else
            {
            indx_sym = 0;
            tmp2 = element;
            }
        NodeIndex = field[NodeIndex + 5];
        res = 0;
        while (NodeIndex != -1)
            {
            if (shr(field[NodeIndex] , 16) == 6)
                {
                tmp1 = FoldBinary(35 , BooleanPtr , element , field[NodeIndex + 5]);
                if (res == 0)
                    {
                    res = tmp1;
                    element = tmp2;
                    tmp2 = FoldBinary(34 , BooleanPtr , element , field[NodeIndex + 6]);
                    res = FoldBinary(27 , BooleanPtr , res , tmp2);
                    }
                else
                    {
                    tmp2 = FoldBinary(34 , BooleanPtr , element , field[NodeIndex + 6]);
                    tmp1 = FoldBinary(27 , BooleanPtr , tmp1 , tmp2);
                    res = FoldBinary(25 , BooleanPtr , res , tmp1);
                    }
                }
            else
                {
                tmp1 = FoldBinary(31 , BooleanPtr , element , NodeIndex);
                if (res == 0)
                    {
                    res = tmp1;
                    element = tmp2;
                    }
                else
                    {
                    res = FoldBinary(25 , BooleanPtr , res , tmp1);
                    }
                }
            NodeIndex = field[NodeIndex + 3];
            }
        if (indx_sym != 0)
            {
            FreeTemporary(indx_sym);
            }
        _CallIntor = res;
        }
    return(_CallIntor);
    }

struct _ptoc_FRAME_CallInt
  {
  char dummy;
  } ;


int CallInt(element, typ, right)
HdgRange element;
SymbolTableRange typ;
HdgRange right;
    {
    struct _ptoc_FRAME_CallInt _ptoc_LOCAL;
    int _CallInt;
    SymbolTableRange sbase;
    SymbolTableRange indx_sym;
    HdgRange lower;
    HdgRange upper;
    HdgRange indx;
    HdgRange left;
    HdgRange _right;
    
    sbase = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase;
    lower = node3(12 , IntegerPtr , STab[sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange , 0);
    upper = node3(12 , IntegerPtr , STab[sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange , 0);
    left = FoldBinary(20 , IntegerPtr , element , lower);
    if (left != element)
        {
        indx_sym = GetTemporary(ProcIndex , IntegerPtr , 0);
        indx = node3(94 , IntegerPtr , indx_sym , 0);
        left = LongStatement(6 , IntegerPtr , indx , left , 0 , 0 , -1);
        indx = FoldUnary(4 , IntegerPtr , indx);
        }
    else
        {
        indx_sym = 0;
        indx = left;
        }
    left = FoldBinary(35 , BooleanPtr , left , node3(12 , IntegerPtr , 0 , 0));
    left = FoldBinary(27 , BooleanPtr , left , FoldBinary(34 , BooleanPtr , indx , upper));
    _right = FoldBinary(30 , IntegerPtr , indx , node3(12 , IntegerPtr , 3 , 0));
    _right = FoldBinary(95 , BytePtr , field[right + 5] , _right);
    _right = FoldUnary(4 , BytePtr , _right);
    _right = FoldBinary(89 , IntegerPtr , _right , FoldBinary(29 , IntegerPtr , node3(12 , IntegerPtr , 1 , 0) , FoldBinary(89 , IntegerPtr , indx , node3(12 , IntegerPtr , 7 , 0))));
    _right = FoldBinary(36 , BooleanPtr , _right , node3(12 , IntegerPtr , 0 , 0));
    _CallInt = FoldBinary(27 , BooleanPtr , left , _right);
    if (indx_sym != 0)
        {
        FreeTemporary(indx_sym);
        }
    return(_CallInt);
    }

struct _ptoc_FRAME_CallCttot
  {
  char dummy;
  } ;


int CallCttot(typ, res, ct)
SymbolTableRange typ;
HdgRange res;
HdgRange ct;
    {
    struct _ptoc_FRAME_CallCttot _ptoc_LOCAL;
    int _CallCttot;
    SymbolTableRange sbase;
    int lower;
    int upper;
    HdgRange temp;
    int routine;
    HdgRange NodeIndex;
    struct ptoc_Type1 *ptoc_Var13;
    
    sbase = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase;
    lower = STab[sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
    upper = STab[sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - lower;
    temp = MakeList(6 , IntegerPtr , -1 , -1 , -1);
    AppendList(6 , IntegerPtr , temp , UnaryExpression(9 , typ , res , 0 , 0));
    SaveIndex = NodeIndex;
    NodeIndex = res;
    if ((field[NodeIndex] & 65535) == 94)
        {
        ptoc_Var13 = &STab[field[NodeIndex + 5]];
        if (ptoc_Var13->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var13->KINDTYPE.C_ModuleDcl.SClass == 5)
            {
            ptoc_Var13->KINDTYPE.C_ModuleDcl.SClass = 10;
            }
        }
    NodeIndex = SaveIndex;
    AppendList(6 , IntegerPtr , temp , MakeVal(node3(12 , IntegerPtr , lower , 0)));
    AppendList(6 , IntegerPtr , temp , MakeVal(node3(12 , IntegerPtr , upper , 0)));
    LinkLists(temp , ct);
    if (control[110])
        {
        routine = EnterConstantString("_sql__pcnst         ");
        }
    else
        {
        routine = EnterConstantString("_sql__scnst         ");
        }
    _CallCttot = LongStatement(62 , AnyPtrPtr , -1 , temp , routine , -1 , CardCount);
    return(_CallCttot);
    }

struct _ptoc_FRAME_CallConvertt
  {
  char dummy;
  } ;


int CallConvertt(res, src)
HdgRange res;
HdgRange src;
    {
    struct _ptoc_FRAME_CallConvertt _ptoc_LOCAL;
    int _CallConvertt;
    SymbolTableRange r_sbase;
    SymbolTableRange r_typ;
    int r_lower;
    int r_upper;
    SymbolTableRange s_sbase;
    SymbolTableRange s_typ;
    int s_lower;
    int s_upper;
    HdgRange temp;
    int routine;
    
    r_typ = field[res + 1];
    r_sbase = STab[r_typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase;
    r_lower = STab[r_sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
    r_upper = STab[r_sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - r_lower;
    s_typ = field[src + 1];
    s_sbase = STab[s_typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase;
    s_lower = STab[s_sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
    s_upper = STab[s_sbase].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - s_lower;
    if (r_lower == s_lower && r_upper == s_upper)
        {
        temp = LongStatement(6 , r_typ , CopyTree(res) , src , 0 , 0 , CardCount);
        temp = MakeList(6 , AnyPtrPtr , temp , FoldUnary(13 , AnyPtrPtr , res) , -1);
        SetMinor(temp , 114);
        _CallConvertt = temp;
        }
    else
        {
        temp = MakeList(6 , IntegerPtr , -1 , -1 , -1);
        AppendList(6 , IntegerPtr , temp , UnaryExpression(9 , r_typ , res , 0 , 0));
        AppendList(6 , IntegerPtr , temp , MakeVal(node3(12 , IntegerPtr , r_lower , 0)));
        AppendList(6 , IntegerPtr , temp , MakeVal(node3(12 , IntegerPtr , r_upper , 0)));
        MakeRef(&src);
        AppendList(6 , IntegerPtr , temp , src);
        AppendList(6 , IntegerPtr , temp , MakeVal(node3(12 , IntegerPtr , s_lower , 0)));
        AppendList(6 , IntegerPtr , temp , MakeVal(node3(12 , IntegerPtr , s_upper , 0)));
        if (control[110])
            {
            routine = EnterConstantString("_sql__psetco        ");
            }
        else
            {
            routine = EnterConstantString("_sql__setco         ");
            }
        _CallConvertt = LongStatement(62 , AnyPtrPtr , -1 , temp , routine , -1 , CardCount);
        }
    return(_CallConvertt);
    }

struct _ptoc_FRAME_CallPclose
  {
  char dummy;
  } ;


int CallPclose()
    {
    struct _ptoc_FRAME_CallPclose _ptoc_LOCAL;
    int _CallPclose;
    
    _CallPclose = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , node3(110 , IntegerPtr , 6 , 0) , -1 , -1) , EnterConstantString("_sql__closea        ") , 1 , CardCount);
    return(_CallPclose);
    }

struct _ptoc_FRAME_CallHalt
  {
  char dummy;
  } ;


int CallHalt(p)
int p;
    {
    struct _ptoc_FRAME_CallHalt _ptoc_LOCAL;
    int _CallHalt;
    int index;
    HdgRange NodeIndex;
    
    if (cnt[p] == 1)
        {
        index = StartCnt[p];
        if (AssgnmntCompatible(IntegerPtr , ExpVal[index]))
            {
            NodeIndex = MakeList(6 , IntegerPtr , UnaryExpression(8 , IntegerPtr , EvalConvert(IntegerPtr , ExpVal[index]) , 0 , 0) , -1 , -1);
            _CallHalt = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__stopp         ") , 0 , CardCount);
            }
        else
            {
            ErrorMsg(1 , ExpColNo[index]);
            _CallHalt = -1;
            }
        }
    else
        {
        NodeIndex = MakeList(6 , IntegerPtr , UnaryExpression(8 , IntegerPtr , node3(12 , IntegerPtr , 0 , 0) , 0 , 0) , -1 , -1);
        _CallHalt = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__stopp         ") , 0 , CardCount);
        }
    return(_CallHalt);
    }

struct _ptoc_FRAME_CallClock
  {
  char dummy;
  } ;


int CallClock()
    {
    struct _ptoc_FRAME_CallClock _ptoc_LOCAL;
    int _CallClock;
    
    _CallClock = LongStatement(62 , IntegerPtr , -1 , -1 , EnterConstantString("_sql__clock         ") , 0 , CardCount);
    return(_CallClock);
    }

struct _ptoc_FRAME_CallSysclock
  {
  char dummy;
  } ;


int CallSysclock()
    {
    struct _ptoc_FRAME_CallSysclock _ptoc_LOCAL;
    int _CallSysclock;
    
    _CallSysclock = LongStatement(62 , IntegerPtr , -1 , -1 , EnterConstantString("_sql__ckl           ") , 0 , CardCount);
    return(_CallSysclock);
    }

struct _ptoc_FRAME_CallWallclock
  {
  char dummy;
  } ;


int CallWallclock()
    {
    struct _ptoc_FRAME_CallWallclock _ptoc_LOCAL;
    int _CallWallclock;
    
    _CallWallclock = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , node3(12 , IntegerPtr , 0 , 0) , -1 , -1) , EnterConstantString("_time               ") , 1 , CardCount);
    return(_CallWallclock);
    }

struct _ptoc_FRAME_CallStringCopy
  {
  char dummy;
  } ;


int CallStringCopy(result, source, first, size)
HdgRange result;
HdgRange source;
HdgRange first;
HdgRange size;
    {
    struct _ptoc_FRAME_CallStringCopy _ptoc_LOCAL;
    int _CallStringCopy;
    HdgRange NodeIndex;
    HdgRange max;
    HdgRange templist;
    struct ptoc_Type1 *ptoc_Var14;
    
    NodeIndex = result;
    result = UnaryExpression(9 , field[NodeIndex + 1] , result , 0 , 0);
    SaveIndex = NodeIndex;
    NodeIndex = result;
    if ((field[NodeIndex] & 65535) == 94)
        {
        ptoc_Var14 = &STab[field[NodeIndex + 5]];
        if (ptoc_Var14->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var14->KINDTYPE.C_ModuleDcl.SClass == 5)
            {
            ptoc_Var14->KINDTYPE.C_ModuleDcl.SClass = 10;
            }
        }
    NodeIndex = SaveIndex;
    max = node3(12 , IntegerPtr , LengthOfString(field[NodeIndex + 1]) , 0);
    MakeRef(&source);
    templist = MakeList(6 , IntegerPtr , -1 , -1 , -1);
    AppendList(6 , IntegerPtr , templist , result);
    AppendList(6 , IntegerPtr , templist , MakeVal(max));
    AppendList(6 , IntegerPtr , templist , source);
    AppendList(6 , IntegerPtr , templist , MakeVal(first));
    AppendList(6 , IntegerPtr , templist , MakeVal(size));
    _CallStringCopy = LongStatement(62 , AnyPtrPtr , -1 , templist , EnterConstantString("_sql__pstrcpy       ") , 5 , CardCount);
    return(_CallStringCopy);
    }

struct _ptoc_FRAME_CallStringConcat
  {
  char dummy;
  } ;


int CallStringConcat(result, left, right)
HdgRange result;
HdgRange left;
HdgRange right;
    {
    struct _ptoc_FRAME_CallStringConcat _ptoc_LOCAL;
    int _CallStringConcat;
    HdgRange max;
    HdgRange NodeIndex;
    HdgRange temp;
    struct ptoc_Type1 *ptoc_Var15;
    
    NodeIndex = result;
    result = UnaryExpression(9 , field[NodeIndex + 1] , result , 0 , 0);
    SaveIndex = NodeIndex;
    NodeIndex = result;
    if ((field[NodeIndex] & 65535) == 94)
        {
        ptoc_Var15 = &STab[field[NodeIndex + 5]];
        if (ptoc_Var15->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var15->KINDTYPE.C_ModuleDcl.SClass == 5)
            {
            ptoc_Var15->KINDTYPE.C_ModuleDcl.SClass = 10;
            }
        }
    NodeIndex = SaveIndex;
    MakeRef(&left);
    MakeRef(&right);
    max = MakeVal(node3(12 , IntegerPtr , LengthOfString(field[NodeIndex + 1]) , 0));
    temp = MakeList(6 , IntegerPtr , -1 , -1 , -1);
    AppendList(6 , IntegerPtr , temp , result);
    AppendList(6 , IntegerPtr , temp , max);
    AppendList(6 , IntegerPtr , temp , left);
    AppendList(6 , IntegerPtr , temp , right);
    _CallStringConcat = LongStatement(62 , AnyPtrPtr , -1 , temp , EnterConstantString("_sql__pstrcat       ") , 4 , CardCount);
    return(_CallStringConcat);
    }

struct _ptoc_FRAME_CallStringCompare
  {
  char dummy;
  } ;


int CallStringCompare(Exp1, Exp2)
HdgRange Exp1;
HdgRange Exp2;
    {
    struct _ptoc_FRAME_CallStringCompare _ptoc_LOCAL;
    int _CallStringCompare;
    HdgRange len;
    HdgRange temp;
    HdgRange NodeIndex;
    
    MakeRef(&Exp1);
    MakeRef(&Exp2);
    temp = MakeList(6 , IntegerPtr , Exp1 , Exp2 , -1);
    _CallStringCompare = LongStatement(62 , IntegerPtr , -1 , temp , EnterConstantString("_sql__pstrcomp      ") , 2 , CardCount);
    return(_CallStringCompare);
    }

struct _ptoc_FRAME_CallRange
  {
  char dummy;
  } ;


int CallRange(value, lower, upper)
HdgRange value;
int lower;
int upper;
    {
    struct _ptoc_FRAME_CallRange _ptoc_LOCAL;
    int _CallRange;
    HdgRange temp;
    
    temp = MakeList(6 , IntegerPtr , MakeVal(value) , MakeVal(node3(12 , IntegerPtr , lower , 0)) , MakeVal(node3(12 , IntegerPtr , upper , 0)));
    _CallRange = LongStatement(62 , IntegerPtr , -1 , temp , EnterConstantString("_sql__setrang       ") , 3 , CardCount);
    return(_CallRange);
    }

struct _ptoc_FRAME_CallCaseErr
  {
  char dummy;
  } ;


int CallCaseErr(p1)
ParseStackRange p1;
    {
    struct _ptoc_FRAME_CallCaseErr _ptoc_LOCAL;
    int _CallCaseErr;
    HdgRange temp;
    
    temp = MakeList(6 , IntegerPtr , MakeVal(node3(14 , PtrToCharPtr , CurrentFile , 0)) , MakeVal(node3(12 , IntegerPtr , CardNum[p1] , 0)) , -1);
    _CallCaseErr = LongStatement(62 , IntegerPtr , -1 , temp , EnterConstantString("_sql__caseerr       ") , 2 , CardNum[p1]);
    return(_CallCaseErr);
    }

struct _ptoc_FRAME_CallNilCheck
  {
  char dummy;
  } ;


int CallNilCheck(p1)
ParseStackRange p1;
    {
    struct _ptoc_FRAME_CallNilCheck _ptoc_LOCAL;
    int _CallNilCheck;
    HdgRange temp;
    
    temp = MakeList(6 , PtrToCharPtr , g1[p1] , MakeVal(node3(14 , PtrToCharPtr , CurrentFile , 0)) , MakeVal(node3(12 , IntegerPtr , CardNum[p1] , 0)));
    _CallNilCheck = LongStatement(62 , IntegerPtr , -1 , temp , EnterConstantString("_sql__nil           ") , 3 , CardNum[p1]);
    return(_CallNilCheck);
    }

struct _ptoc_FRAME_EvalGoto
  {
  char dummy;
  } ;


int EvalGoto(LabIndex)
SymbolTableRange LabIndex;
    {
    struct _ptoc_FRAME_EvalGoto _ptoc_LOCAL;
    int _EvalGoto;
    HdgRange lab;
    HdgRange link;
    
    big_goto = 1;
    control[69] = 0;
    lab = UnaryExpression(13 , AnyPtrPtr , node3(98 , IntegerPtr , LabIndex , CardCount) , 0 , 0);
    link = node3(109 , AnyPtrPtr , LabIndex , 0);
    _EvalGoto = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , AnyPtrPtr , MakeVal(lab) , MakeVal(link) , -1) , EnterConstantString("_sql__goto          ") , 2 , CardCount);
    return(_EvalGoto);
    }

struct _ptoc_FRAME_EvalDefname
  {
  char dummy;
  } ;


int EvalDefname(id)
SymbolTableRange id;
    {
    struct _ptoc_FRAME_EvalDefname _ptoc_LOCAL;
    int _EvalDefname;
    HdgRange FileVar;
    HdgRange FileName;
    HdgRange NameLen;
    HdgRange DataSize;
    HdgRange NodeIndex;
    SymbolTableRange FileTyp;
    struct ptoc_Type1 *ptoc_Var16;
    int ptoc_Var17;
    
    FileTyp = STab[id].SType;
    FileVar = UnaryExpression(9 , FileTyp , node3(94 , FileTyp , id , 0) , 0 , 0);
    SaveIndex = NodeIndex;
    NodeIndex = FileVar;
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
    ptoc_Var17 = STab[id].SName;
    FileName = node3(14 , PacPtr , ptoc_Var17 , _GetStringLength(ptoc_Var17));
    NameLen = node3(12 , IntegerPtr , _GetStringLength(STab[id].SName) , 0);
    if (IsTextFile(FileTyp))
        {
        DataSize = node3(12 , IntegerPtr , 0 , 0);
        }
    else
        {
        DataSize = node3(69 , IntegerPtr , STab[FileTyp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent , 0);
        }
    NodeIndex = MakeList(6 , IntegerPtr , FileVar , FileName , -1);
    AppendList(6 , IntegerPtr , NodeIndex , NameLen);
    AppendList(6 , IntegerPtr , NodeIndex , DataSize);
    NodeIndex = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__dfn           ") , 4 , CardCount);
    _EvalDefname = NodeIndex;
    return(_EvalDefname);
    }

struct _ptoc_FRAME_EvalReset
  {
  char dummy;
  } ;


int EvalReset(ParmIndex)
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalReset _ptoc_LOCAL;
    int _EvalReset;
    int index;
    int count;
    HdgRange NodeIndex;
    HdgRange FileVar;
    HdgRange FileName;
    HdgRange NameLen;
    HdgRange DataSize;
    SymbolTableRange FileTyp;
    SymbolTableRange FileNameTyp;
    
    count = cnt[ParmIndex];
    if (count == 1 || count == 2)
        {
        index = StartCnt[ParmIndex];
        FileTyp = ExpType[index];
        FileVar = ExpVal[index];
        if (IsFile(FileTyp))
            {
            MakeRef(&FileVar);
            if (count == 1)
                {
                FileName = node3(12 , IntegerPtr , 0 , 0);
                NameLen = node3(12 , IntegerPtr , 0 , 0);
                }
            else
                {
                index += 1;
                FileName = ExpVal[index];
                FileNameTyp = ExpType[index];
                if (FileNameTyp == CharPtr && IsConstant(FileName))
                    {
                    CharToPac(FileName);
                    FileNameTyp = PacPtr;
                    }
                if (IsCharArray(FileNameTyp))
                    {
                    if (IsConstant(FileName))
                        {
                        MakeRef(&FileName);
                        NodeIndex = FileName;
                        NameLen = node3(12 , IntegerPtr , _GetStringLength(field[NodeIndex + 5]) , 0);
                        }
                    else
                        {
                        MakeRef(&FileName);
                        NameLen = node3(12 , IntegerPtr , LengthOfCharArray(FileNameTyp) , 0);
                        }
                    }
                else
                    {
                    ErrorMsg(133 , ExpColNo[index]);
                    FileName = node3(12 , IntegerPtr , 0 , 0);
                    NameLen = node3(12 , IntegerPtr , 0 , 0);
                    }
                }
            if (IsTextFile(FileTyp))
                {
                DataSize = node3(12 , IntegerPtr , 0 , 0);
                }
            else
                {
                DataSize = node3(69 , IntegerPtr , STab[FileTyp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent , 0);
                }
            NodeIndex = MakeList(6 , IntegerPtr , FileVar , FileName , -1);
            AppendList(6 , IntegerPtr , NodeIndex , NameLen);
            AppendList(6 , IntegerPtr , NodeIndex , DataSize);
            NodeIndex = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__reset         ") , 4 , CardCount);
            _EvalReset = NodeIndex;
            }
        else
            {
            ErrorMsg(32 , ExpColNo[index]);
            _EvalReset = -1;
            }
        }
    else
        {
        ErrorMsg(8 , -1);
        _EvalReset = -1;
        }
    return(_EvalReset);
    }

struct _ptoc_FRAME_EvalRewrite
  {
  char dummy;
  } ;


int EvalRewrite(ParmIndex)
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalRewrite _ptoc_LOCAL;
    int _EvalRewrite;
    int index;
    int count;
    HdgRange NodeIndex;
    HdgRange FileVar;
    HdgRange FileName;
    HdgRange NameLen;
    HdgRange DataSize;
    SymbolTableRange FileTyp;
    SymbolTableRange FileNameTyp;
    
    count = cnt[ParmIndex];
    if (count == 1 || count == 2)
        {
        index = StartCnt[ParmIndex];
        FileTyp = ExpType[index];
        FileVar = ExpVal[index];
        if (IsFile(FileTyp))
            {
            MakeRef(&FileVar);
            if (count == 1)
                {
                FileName = node3(12 , IntegerPtr , 0 , 0);
                NameLen = node3(12 , IntegerPtr , 0 , 0);
                }
            else
                {
                index += 1;
                FileName = ExpVal[index];
                FileNameTyp = ExpType[index];
                if (FileNameTyp == CharPtr && IsConstant(FileName))
                    {
                    CharToPac(FileName);
                    FileNameTyp = PacPtr;
                    }
                if (IsCharArray(FileNameTyp))
                    {
                    if (IsConstant(FileName))
                        {
                        MakeRef(&FileName);
                        NodeIndex = FileName;
                        NameLen = node3(12 , IntegerPtr , _GetStringLength(field[NodeIndex + 5]) , 0);
                        }
                    else
                        {
                        MakeRef(&FileName);
                        NameLen = node3(12 , IntegerPtr , LengthOfCharArray(FileNameTyp) , 0);
                        }
                    }
                else
                    {
                    ErrorMsg(133 , ExpColNo[index]);
                    FileName = node3(12 , IntegerPtr , 0 , 0);
                    NameLen = node3(12 , IntegerPtr , 0 , 0);
                    }
                }
            if (IsTextFile(FileTyp))
                {
                DataSize = node3(12 , IntegerPtr , 0 , 0);
                }
            else
                {
                DataSize = node3(69 , IntegerPtr , STab[FileTyp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent , 0);
                }
            NodeIndex = MakeList(6 , IntegerPtr , FileVar , FileName , -1);
            AppendList(6 , IntegerPtr , NodeIndex , NameLen);
            AppendList(6 , IntegerPtr , NodeIndex , DataSize);
            NodeIndex = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__rewrite       ") , 4 , CardCount);
            _EvalRewrite = NodeIndex;
            }
        else
            {
            ErrorMsg(32 , ExpColNo[index]);
            _EvalRewrite = -1;
            }
        }
    else
        {
        ErrorMsg(8 , -1);
        _EvalRewrite = -1;
        }
    return(_EvalRewrite);
    }

struct _ptoc_FRAME_EvalClose
  {
  char dummy;
  } ;


int EvalClose(ParmIndex)
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalClose _ptoc_LOCAL;
    int _EvalClose;
    int index;
    HdgRange FileVar;
    SymbolTableRange FileTyp;
    
    if (cnt[ParmIndex] == 1)
        {
        index = StartCnt[ParmIndex];
        FileTyp = ExpType[index];
        FileVar = ExpVal[index];
        if (IsFile(FileTyp))
            {
            MakeRef(&FileVar);
            _EvalClose = LongStatement(62 , TypePtrIorechd , -1 , MakeList(6 , FileTyp , FileVar , -1 , -1) , EnterConstantString("_sql__close         ") , 1 , CardCount);
            }
        else
            {
            ErrorMsg(32 , ExpColNo[index]);
            _EvalClose = -1;
            }
        }
    else
        {
        ErrorMsg(8 , ColNo[ParmIndex]);
        StackError(ParmIndex);
        _EvalClose = -1;
        }
    return(_EvalClose);
    }

struct _ptoc_FRAME_EvalRead
  {
  char dummy;
  } ;


int EvalRead(ProcOrFunc, sysproc, ParmIndex)
int ProcOrFunc;
int sysproc;
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalRead _ptoc_LOCAL;
    int _EvalRead;
    int index;
    int last;
    HdgRange FileVar;
    HdgRange IOitem;
    HdgRange Temp;
    HdgRange TempList;
    HdgRange NodeIndex;
    SymbolTableRange exptype;
    SymbolTableRange FileVarType;
    SymbolTableRange FileTemp;
    SymbolTableRange FileComponentType;
    SymbolTableRange FileComponentStype;
    struct ptoc_Type1 *ptoc_Var18;
    struct ptoc_Type1 *ptoc_Var19;
    int ptoc_Var20;
    
    TempList = -1;
    if (cnt[ParmIndex] == 0)
        {
        index = 0;
        last = -1;
        FileVar = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , InputPtr , 0) , 0 , 0);
        SaveIndex = NodeIndex;
        NodeIndex = FileVar;
        NodeIndex = field[NodeIndex + 5];
        if ((field[NodeIndex] & 65535) == 94)
            {
            ptoc_Var18 = &STab[field[NodeIndex + 5]];
            if (ptoc_Var18->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var18->KINDTYPE.C_ModuleDcl.SClass == 5)
                {
                ptoc_Var18->KINDTYPE.C_ModuleDcl.SClass = 10;
                }
            }
        NodeIndex = SaveIndex;
        FileVarType = TextPtr;
        }
    else
        {
        index = StartCnt[ParmIndex];
        last = index + cnt[ParmIndex] + -1;
        if (IsFile(ExpType[index]) && IsLval(ExpVal[index]))
            {
            FileVar = ExpVal[index];
            FileVarType = ExpType[index];
            MakeRef(&FileVar);
            index += 1;
            }
        else
            {
            FileVar = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , InputPtr , 0) , 0 , 0);
            SaveIndex = NodeIndex;
            NodeIndex = FileVar;
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 94)
                {
                ptoc_Var19 = &STab[field[NodeIndex + 5]];
                if (ptoc_Var19->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var19->KINDTYPE.C_ModuleDcl.SClass == 5)
                    {
                    ptoc_Var19->KINDTYPE.C_ModuleDcl.SClass = 10;
                    }
                }
            NodeIndex = SaveIndex;
            FileVarType = TextPtr;
            }
        }
    FileTemp = GetTemporary(ProcIndex , TypePtrIorechd , 0);
    Temp = LongStatement(6 , TypePtrIorechd , node3(94 , TypePtrIorechd , FileTemp , 0) , CallUnit(FileVar) , 0 , 0 , CardCount);
    TempList = LinkChain(TempList , Temp);
    if (IsTextFile(FileVarType))
        {
        while (index <= last)
            {
            NodeIndex = ExpVal[index];
            if (shr(field[NodeIndex] , 16) == 4 && ((ptoc_Var20 = field[NodeIndex] % 16384) >= 0 ? ptoc_Var20 : ptoc_Var20 + 16384) == 4 && shr(field[field[NodeIndex + 5]] , 16) == 1 && ((ptoc_Var20 = field[field[NodeIndex + 5]] % 16384) >= 0 ? ptoc_Var20 : ptoc_Var20 + 16384) == 94)
                {
                if ((STab[field[field[NodeIndex + 5] + 5]].SFlags & 2048) != 0)
                    {
                    ErrorMsg(145 , ExpColNo[index]);
                    }
                }
            exptype = ExpType[index];
            if ((field[NodeIndex] & 65535) == 4)
                {
                IOitem = field[NodeIndex + 5];
                }
            else
                {
                ErrorMsg(30 , ExpColNo[index]);
                }
            if (exptype == CharPtr)
                {
                Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , -1 , -1);
                Temp = LongStatement(62 , CharPtr , -1 , Temp , EnterConstantString("_sql__1r            ") , 1 , CardCount);
                if (control[114])
                    {
                    Temp = RangeCheck(ExpStype[index] , Temp);
                    }
                Temp = LongStatement(6 , CharPtr , IOitem , Temp , 0 , 0 , CardCount);
                }
            else
                {
                if (IsCharArray(exptype))
                    {
                    IOitem = NodeIndex;
                    MakeRef(&IOitem);
                    Temp = node3(12 , IntegerPtr , LengthOfCharArray(exptype) , 0);
                    Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , IOitem , MakeVal(Temp));
                    Temp = LongStatement(62 , IntegerPtr , -1 , Temp , EnterConstantString("_sql__pr            ") , 3 , CardCount);
                    }
                else
                    {
                    if (exptype == IntegerPtr)
                        {
                        Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , -1 , -1);
                        Temp = LongStatement(62 , IntegerPtr , -1 , Temp , EnterConstantString("_sql__intr          ") , 1 , CardCount);
                        if (control[114])
                            {
                            Temp = RangeCheck(ExpStype[index] , Temp);
                            }
                        Temp = LongStatement(6 , IntegerPtr , IOitem , Temp , 0 , 0 , CardCount);
                        }
                    else
                        {
                        if (exptype == FloatPtr)
                            {
                            Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , -1 , -1);
                            Temp = LongStatement(62 , DoublePtr , -1 , Temp , EnterConstantString("_sql__lor           ") , 1 , CardCount);
                            Temp = LongStatement(6 , FloatPtr , IOitem , EvalConvert(FloatPtr , Temp) , 0 , 0 , CardCount);
                            }
                        else
                            {
                            if (exptype == LongPtr)
                                {
                                Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , -1 , -1);
                                Temp = LongStatement(62 , LongPtr , -1 , Temp , EnterConstantString("_sql__lor           ") , 1 , CardCount);
                                Temp = LongStatement(6 , LongPtr , IOitem , EvalConvert(LongPtr , Temp) , 0 , 0 , CardCount);
                                }
                            else
                                {
                                if (exptype == DoublePtr)
                                    {
                                    Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , -1 , -1);
                                    Temp = LongStatement(62 , DoublePtr , -1 , Temp , EnterConstantString("_sql__lor           ") , 1 , CardCount);
                                    Temp = LongStatement(6 , DoublePtr , IOitem , Temp , 0 , 0 , CardCount);
                                    }
                                else
                                    {
                                    if (IsEnum(exptype) || exptype == BooleanPtr)
                                        {
                                        Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , UnaryExpression(13 , AnyPtrPtr , node3(94 , IntegerPtr , CreateEnumBlock(exptype) , 0) , 0 , 0) , -1);
                                        Temp = LongStatement(62 , exptype , -1 , Temp , EnterConstantString("_sql__er            ") , 1 , CardCount);
                                        Temp = LongStatement(6 , exptype , IOitem , Temp , 0 , 0 , CardCount);
                                        }
                                    else
                                        {
                                        if (IsOrdinal(exptype))
                                            {
                                            Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , -1 , -1);
                                            Temp = LongStatement(62 , IntegerPtr , -1 , Temp , EnterConstantString("_sql__intr          ") , 1 , CardCount);
                                            Temp = EvalConvert(exptype , Temp);
                                            if (control[114])
                                                {
                                                Temp = RangeCheck(ExpStype[index] , Temp);
                                                }
                                            Temp = LongStatement(6 , exptype , IOitem , Temp , 0 , 0 , CardCount);
                                            }
                                        else
                                            {
                                            ErrorMsg(1 , ExpColNo[index]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            TempList = LinkChain(TempList , Temp);
            index += 1;
            }
        if (sysproc == 9)
            {
            if (!IsTextFile(FileVarType))
                {
                ErrorMsg(31 , ExpColNo[StartCnt[ParmIndex]]);
                }
            Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , -1 , -1);
            Temp = LongStatement(62 , IntegerPtr , -1 , Temp , EnterConstantString("_sql__readln        ") , 1 , CardCount);
            TempList = LinkChain(TempList , Temp);
            }
        }
    else
        {
        if (sysproc == 2)
            {
            FileComponentStype = STab[FileVarType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent;
            FileComponentType = PrimType(FileComponentStype);
            while (index <= last)
                {
                NodeIndex = ExpVal[index];
                Temp = GenStore(field[NodeIndex + 5] , ExpType[index] , ExpStype[index] , ExpColNo[index] , UnaryExpression(4 , FileComponentType , CallFnil(RefFile(FileTemp) , FileComponentType) , 0 , 0) , FileComponentType , FileComponentStype , ExpColNo[index] , CardCount);
                TempList = LinkChain(TempList , Temp);
                TempList = LinkChain(TempList , CallGet(RefFile(FileTemp)));
                index += 1;
                }
            }
        else
            {
            ErrorMsg(31 , ExpColNo[StartCnt[ParmIndex]]);
            }
        }
    FreeTemporary(FileTemp);
    _EvalRead = TempList;
    return(_EvalRead);
    }

struct _ptoc_FRAME_EvalWrite
  {
  HdgRange IOwidth;
  HdgRange IOdigits;
  HdgRange TempList;
  int ptoc_Var21;
  IOcontrolType IOctlstring;
  int IOctllen;
  char dummy;
  } ;


struct _ptoc_FRAME_Evalwidthdigits_EvalWrite
  {
  struct _ptoc_FRAME_EvalWrite *_ptoc_LINK;
  char dummy;
  } ;


static
int Evalwidthdigits_EvalWrite(_ptoc_LINK, LengthNode)
struct _ptoc_FRAME_EvalWrite *_ptoc_LINK;
HdgRange LengthNode;
    {
    struct _ptoc_FRAME_Evalwidthdigits_EvalWrite _ptoc_LOCAL;
    SymbolTableRange WidthTemp;
    HdgRange Temp;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    WidthTemp = GetTemporary(ProcIndex , IntegerPtr , 0);
    Temp = LongStatement(6 , IntegerPtr , node3(94 , IntegerPtr , WidthTemp , 0) , _ptoc_LOCAL._ptoc_LINK->IOwidth , 0 , 0 , CardCount);
    _ptoc_LOCAL._ptoc_LINK->TempList = LinkChain(_ptoc_LOCAL._ptoc_LINK->TempList , Temp);
    _ptoc_LOCAL._ptoc_LINK->IOwidth = UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , WidthTemp , 0) , 0 , 0);
    _ptoc_LOCAL._ptoc_LINK->IOdigits = ComplexStmnt(8 , IntegerPtr , BinaryExpression(32 , BooleanPtr , UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , WidthTemp , 0) , 0 , 0) , LengthNode) , UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , WidthTemp , 0) , 0 , 0) , CopyTree(LengthNode) , CardCount);
    FreeTemporary(WidthTemp);
    }

struct _ptoc_FRAME_addcontrol_EvalWrite
  {
  struct _ptoc_FRAME_EvalWrite *_ptoc_LINK;
  char dummy;
  } ;


static
int addcontrol_EvalWrite(_ptoc_LINK, ptoc_ptr_strn)
struct _ptoc_FRAME_EvalWrite *_ptoc_LINK;
String20 ptoc_ptr_strn;
    {
    struct _ptoc_FRAME_addcontrol_EvalWrite _ptoc_LOCAL;
    int i;
    int len;
    String20 strn;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _ptoc_MOVE(char, sizeof(String20 ), strn, ptoc_ptr_strn);
    len = _ptoc_LOCAL._ptoc_LINK->IOctllen;
    i = 0;
    do
        {
        _ptoc_LOCAL._ptoc_LINK->IOctlstring[len] = strn[i];
        len += 1;
        i += 1;
        }
    while (!((int ) strn[i] == 32));
    _ptoc_LOCAL._ptoc_LINK->IOctllen = len;
    }

int EvalWrite(ProcOrFunc, sysproc, ParmIndex)
int ProcOrFunc;
int sysproc;
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalWrite _ptoc_LOCAL;
    int _EvalWrite;
    int i;
    int index;
    int last;
    int IOcount;
    HdgRange FileVar;
    HdgRange IOcontrol;
    HdgRange IOitem;
    HdgRange Temp;
    HdgRange NodeIndex;
    SymbolTableRange exptype;
    SymbolTableRange FileVarType;
    SymbolTableRange FileTemp;
    SymbolTableRange WidthTemp;
    SymbolTableRange ItemTemp;
    SymbolTableRange FileComponentType;
    SymbolTableRange FileComponentStype;
    int octhexflag;
    HdgRange ParamList;
    SymbolTableRange FilePtr;
    struct ptoc_Type1 *ptoc_Var22;
    struct ptoc_Type1 *ptoc_Var23;
    struct ptoc_Type1 *ptoc_Var24;
    struct ptoc_Type1 *ptoc_Var25;
    struct ptoc_Type1 *ptoc_Var26;
    int ptoc_Var27;
    
    _ptoc_LOCAL.TempList = -1;
    if (cnt[ParmIndex] == 0)
        {
        index = 0;
        last = -1;
        if (sysproc == 6)
            {
            FileVar = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , ErrorPtr , 0) , 0 , 0);
            SaveIndex = NodeIndex;
            NodeIndex = FileVar;
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 94)
                {
                ptoc_Var22 = &STab[field[NodeIndex + 5]];
                if (ptoc_Var22->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var22->KINDTYPE.C_ModuleDcl.SClass == 5)
                    {
                    ptoc_Var22->KINDTYPE.C_ModuleDcl.SClass = 10;
                    }
                }
            NodeIndex = SaveIndex;
            }
        else
            {
            FileVar = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , OutputPtr , 0) , 0 , 0);
            SaveIndex = NodeIndex;
            NodeIndex = FileVar;
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 94)
                {
                ptoc_Var23 = &STab[field[NodeIndex + 5]];
                if (ptoc_Var23->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var23->KINDTYPE.C_ModuleDcl.SClass == 5)
                    {
                    ptoc_Var23->KINDTYPE.C_ModuleDcl.SClass = 10;
                    }
                }
            NodeIndex = SaveIndex;
            }
        FileVarType = TextPtr;
        }
    else
        {
        index = StartCnt[ParmIndex];
        last = index + cnt[ParmIndex] + -1;
        if (IsFile(ExpType[index]) && IsLval(ExpVal[index]))
            {
            if (sysproc == 6)
                {
                FileVar = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , ErrorPtr , 0) , 0 , 0);
                SaveIndex = NodeIndex;
                NodeIndex = FileVar;
                NodeIndex = field[NodeIndex + 5];
                if ((field[NodeIndex] & 65535) == 94)
                    {
                    ptoc_Var24 = &STab[field[NodeIndex + 5]];
                    if (ptoc_Var24->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var24->KINDTYPE.C_ModuleDcl.SClass == 5)
                        {
                        ptoc_Var24->KINDTYPE.C_ModuleDcl.SClass = 10;
                        }
                    }
                NodeIndex = SaveIndex;
                FileVarType = TextPtr;
                }
            else
                {
                FileVar = ExpVal[index];
                FileVarType = ExpType[index];
                MakeRef(&FileVar);
                }
            index += 1;
            }
        else
            {
            if (sysproc == 6)
                {
                FileVar = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , ErrorPtr , 0) , 0 , 0);
                SaveIndex = NodeIndex;
                NodeIndex = FileVar;
                NodeIndex = field[NodeIndex + 5];
                if ((field[NodeIndex] & 65535) == 94)
                    {
                    ptoc_Var25 = &STab[field[NodeIndex + 5]];
                    if (ptoc_Var25->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var25->KINDTYPE.C_ModuleDcl.SClass == 5)
                        {
                        ptoc_Var25->KINDTYPE.C_ModuleDcl.SClass = 10;
                        }
                    }
                NodeIndex = SaveIndex;
                }
            else
                {
                FileVar = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , OutputPtr , 0) , 0 , 0);
                SaveIndex = NodeIndex;
                NodeIndex = FileVar;
                NodeIndex = field[NodeIndex + 5];
                if ((field[NodeIndex] & 65535) == 94)
                    {
                    ptoc_Var26 = &STab[field[NodeIndex + 5]];
                    if (ptoc_Var26->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var26->KINDTYPE.C_ModuleDcl.SClass == 5)
                        {
                        ptoc_Var26->KINDTYPE.C_ModuleDcl.SClass = 10;
                        }
                    }
                NodeIndex = SaveIndex;
                }
            FileVarType = TextPtr;
            }
        }
    if (sysproc == 6)
        {
        Temp = LongStatement(62 , IntegerPtr , -1 , -1 , EnterConstantString("_sql__flp           ") , 0 , CardCount);
        _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , Temp);
        }
    FileTemp = GetTemporary(ProcIndex , TypePtrIorechd , 0);
    Temp = LongStatement(6 , TypePtrIorechd , node3(94 , TypePtrIorechd , FileTemp , 0) , CallUnit(FileVar) , 0 , 0 , CardCount);
    _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , Temp);
    if (IsTextFile(FileVarType))
        {
        _ptoc_LOCAL.IOctllen = 0;
        IOcontrol = node3(14 , PtrToCharPtr , 0 , 0);
        IOcount = 2;
        ParamList = MakeList(6 , FileVarType , RefFileDotFbuf(FileTemp) , MakeVal(IOcontrol) , -1);
        while (index <= last)
            {
            NodeIndex = ExpVal[index];
            exptype = ExpType[index];
            if ((field[NodeIndex] & 65535) == 122)
                {
                IOitem = field[NodeIndex + 5];
                _ptoc_LOCAL.IOwidth = field[NodeIndex + 6];
                _ptoc_LOCAL.IOdigits = field[NodeIndex + 7];
                octhexflag = 1;
                }
            else
                {
                if ((field[NodeIndex] & 65535) == 121)
                    {
                    IOitem = field[NodeIndex + 5];
                    _ptoc_LOCAL.IOwidth = field[NodeIndex + 6];
                    _ptoc_LOCAL.IOdigits = field[NodeIndex + 7];
                    octhexflag = 0;
                    }
                else
                    {
                    IOitem = NodeIndex;
                    _ptoc_LOCAL.IOwidth = -1;
                    _ptoc_LOCAL.IOdigits = -1;
                    octhexflag = 0;
                    }
                }
            if (octhexflag)
                {
                if (IsOrdinal(exptype) || IsPointer(exptype))
                    {
                    IOitem = MakeVal(EvalConvert(IntegerPtr , IOitem));
                    if (_ptoc_LOCAL.IOwidth == -1)
                        {
                        if (_ptoc_LOCAL.IOdigits == OctString)
                            {
                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%11o                ");
                            }
                        else
                            {
                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%8x                 ");
                            }
                        }
                    else
                        {
                        if (_ptoc_LOCAL.IOdigits == OctString)
                            {
                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%*o                 ");
                            }
                        else
                            {
                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%*x                 ");
                            }
                        }
                    _ptoc_LOCAL.IOdigits = -1;
                    }
                else
                    {
                    ErrorMsg(30 , ExpColNo[index]);
                    IOitem = -1;
                    }
                }
            else
                {
                if (exptype == CharPtr)
                    {
                    IOitem = MakeVal(EvalConvert(IntegerPtr , IOitem));
                    if (_ptoc_LOCAL.IOwidth == -1)
                        {
                        addcontrol_EvalWrite(&_ptoc_LOCAL, "%c                  ");
                        }
                    else
                        {
                        addcontrol_EvalWrite(&_ptoc_LOCAL, "%*c                 ");
                        }
                    }
                else
                    {
                    if (exptype == IntegerPtr)
                        {
                        IOitem = MakeVal(IOitem);
                        if (_ptoc_LOCAL.IOwidth == -1)
                            {
                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%10d                ");
                            }
                        else
                            {
                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%*d                 ");
                            }
                        }
                    else
                        {
                        if (exptype == FloatPtr)
                            {
                            IOitem = EvalConvert(DoublePtr , IOitem);
                            IOitem = MakeVal(IOitem);
                            if (_ptoc_LOCAL.IOwidth == -1)
                                {
                                addcontrol_EvalWrite(&_ptoc_LOCAL, "%21.13e             ");
                                }
                            else
                                {
                                if (_ptoc_LOCAL.IOdigits == -1)
                                    {
                                    addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*e               ");
                                    _ptoc_LOCAL.IOdigits = CallMax(CopyTree(_ptoc_LOCAL.IOwidth) , node3(12 , IntegerPtr , 8 , 0) , node3(12 , IntegerPtr , 1 , 0) , ExpColNo[index]);
                                    _ptoc_LOCAL.IOwidth = CallMax(_ptoc_LOCAL.IOwidth , node3(12 , IntegerPtr , 0 , 0) , node3(12 , IntegerPtr , 9 , 0) , ExpColNo[index]);
                                    }
                                else
                                    {
                                    addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*f               ");
                                    _ptoc_LOCAL.IOwidth = CallMax(_ptoc_LOCAL.IOwidth , node3(12 , IntegerPtr , 0 , 0) , node3(12 , IntegerPtr , 1 , 0) , ExpColNo[index]);
                                    }
                                }
                            }
                        else
                            {
                            if (exptype == LongPtr)
                                {
                                IOitem = EvalConvert(LongPtr , IOitem);
                                IOitem = MakeVal(IOitem);
                                if (_ptoc_LOCAL.IOwidth == -1)
                                    {
                                    addcontrol_EvalWrite(&_ptoc_LOCAL, "%16ld               ");
                                    }
                                else
                                    {
                                    addcontrol_EvalWrite(&_ptoc_LOCAL, "%*ld                ");
                                    }
                                }
                            else
                                {
                                if (exptype == DoublePtr)
                                    {
                                    IOitem = MakeVal(IOitem);
                                    if (_ptoc_LOCAL.IOwidth == -1)
                                        {
                                        addcontrol_EvalWrite(&_ptoc_LOCAL, "%21.13e             ");
                                        }
                                    else
                                        {
                                        if (_ptoc_LOCAL.IOdigits == -1)
                                            {
                                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*e               ");
                                            _ptoc_LOCAL.IOdigits = CallMax(CopyTree(_ptoc_LOCAL.IOwidth) , node3(12 , IntegerPtr , 8 , 0) , node3(12 , IntegerPtr , 1 , 0) , ExpColNo[index]);
                                            _ptoc_LOCAL.IOwidth = CallMax(_ptoc_LOCAL.IOwidth , node3(12 , IntegerPtr , 0 , 0) , node3(12 , IntegerPtr , 9 , 0) , ExpColNo[index]);
                                            }
                                        else
                                            {
                                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*f               ");
                                            _ptoc_LOCAL.IOwidth = CallMax(_ptoc_LOCAL.IOwidth , node3(12 , IntegerPtr , 0 , 0) , node3(12 , IntegerPtr , 1 , 0) , ExpColNo[index]);
                                            }
                                        }
                                    }
                                else
                                    {
                                    if (exptype == PacPtr)
                                        {
                                        field[IOitem + 1] = PtrToCharPtr;
                                        if (_ptoc_LOCAL.IOwidth == -1)
                                            {
                                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%s                  ");
                                            }
                                        else
                                            {
                                            if (((ptoc_Var27 = field[_ptoc_LOCAL.IOwidth] % 16384) >= 0 ? ptoc_Var27 : ptoc_Var27 + 16384) == 12)
                                                {
                                                if (field[_ptoc_LOCAL.IOwidth + 5] < field[IOitem + 6])
                                                    {
                                                    i = field[_ptoc_LOCAL.IOwidth + 5];
                                                    }
                                                else
                                                    {
                                                    i = field[IOitem + 6];
                                                    }
                                                _ptoc_LOCAL.IOdigits = node3(12 , IntegerPtr , i , 0);
                                                addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*s               ");
                                                }
                                            else
                                                {
                                                Temp = node3(12 , IntegerPtr , field[IOitem + 6] , 0);
                                                Evalwidthdigits_EvalWrite(&_ptoc_LOCAL, Temp);
                                                addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*s               ");
                                                }
                                            }
                                        MakeRef(&IOitem);
                                        IOitem = MakeVal(IOitem);
                                        }
                                    else
                                        {
                                        if (IsCharArray(exptype))
                                            {
                                            if (!control[121] && (!IsPac(exptype) || STab[STab[exptype].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange != 1 || STab[STab[exptype].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange == 1))
                                                {
                                                ErrorMsg(144 , ExpColNo[index]);
                                                }
                                            if (_ptoc_LOCAL.IOwidth == -1)
                                                {
                                                _ptoc_LOCAL.IOdigits = node3(12 , IntegerPtr , LengthOfCharArray(exptype) , 0);
                                                _ptoc_LOCAL.IOwidth = CopyTree(_ptoc_LOCAL.IOdigits);
                                                }
                                            else
                                                {
                                                if (((ptoc_Var27 = field[_ptoc_LOCAL.IOwidth] % 16384) >= 0 ? ptoc_Var27 : ptoc_Var27 + 16384) == 12)
                                                    {
                                                    if (field[_ptoc_LOCAL.IOwidth + 5] < LengthOfCharArray(exptype))
                                                        {
                                                        i = field[_ptoc_LOCAL.IOwidth + 5];
                                                        }
                                                    else
                                                        {
                                                        i = LengthOfCharArray(exptype);
                                                        }
                                                    _ptoc_LOCAL.IOdigits = node3(12 , IntegerPtr , i , 0);
                                                    }
                                                else
                                                    {
                                                    Temp = node3(12 , IntegerPtr , LengthOfCharArray(exptype) , 0);
                                                    Evalwidthdigits_EvalWrite(&_ptoc_LOCAL, Temp);
                                                    }
                                                }
                                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*s               ");
                                            MakeRef(&IOitem);
                                            }
                                        else
                                            {
                                            if (IsString(exptype))
                                                {
                                                if (IsLval(ExpVal[index]))
                                                    {
                                                    ItemTemp = 0;
                                                    }
                                                else
                                                    {
                                                    ItemTemp = GetTemporary(ProcIndex , exptype , 0);
                                                    Temp = EvalStringExp(node3(94 , exptype , ItemTemp , 0) , IOitem);
                                                    _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , Temp);
                                                    IOitem = UnaryExpression(4 , exptype , node3(94 , exptype , ItemTemp , 0) , 0 , 0);
                                                    }
                                                if (_ptoc_LOCAL.IOwidth == -1)
                                                    {
                                                    _ptoc_LOCAL.IOdigits = EvalConvert(IntegerPtr , StringElement(CopyTree(IOitem) , 0));
                                                    _ptoc_LOCAL.IOwidth = EvalConvert(IntegerPtr , StringElement(CopyTree(IOitem) , 0));
                                                    }
                                                else
                                                    {
                                                    if (((ptoc_Var27 = field[_ptoc_LOCAL.IOwidth] % 16384) >= 0 ? ptoc_Var27 : ptoc_Var27 + 16384) == 12)
                                                        {
                                                        if (field[_ptoc_LOCAL.IOwidth + 5] < LengthOfString(exptype))
                                                            {
                                                            i = field[_ptoc_LOCAL.IOwidth + 5];
                                                            }
                                                        else
                                                            {
                                                            i = LengthOfString(exptype);
                                                            }
                                                        _ptoc_LOCAL.IOdigits = node3(12 , IntegerPtr , i , 0);
                                                        }
                                                    else
                                                        {
                                                        Temp = node3(12 , IntegerPtr , LengthOfString(exptype) , 0);
                                                        Evalwidthdigits_EvalWrite(&_ptoc_LOCAL, Temp);
                                                        }
                                                    }
                                                addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*s               ");
                                                if (ItemTemp != 0)
                                                    {
                                                    FreeTemporary(ItemTemp);
                                                    }
                                                IOitem = StringElement(IOitem , 1);
                                                MakeRef(&IOitem);
                                                }
                                            else
                                                {
                                                if (IsEnum(exptype) || exptype == BooleanPtr)
                                                    {
                                                    IOitem = CallNam(IOitem , UnaryExpression(13 , AnyPtrPtr , node3(94 , IntegerPtr , CreateEnumBlock(exptype) , 0) , 0 , 0));
                                                    if (_ptoc_LOCAL.IOwidth == -1)
                                                        {
                                                        addcontrol_EvalWrite(&_ptoc_LOCAL, "%s                  ");
                                                        }
                                                    else
                                                        {
                                                        ItemTemp = GetTemporary(ProcIndex , AnyPtrPtr , 0);
                                                        Temp = LongStatement(6 , AnyPtrPtr , node3(94 , AnyPtrPtr , ItemTemp , 0) , IOitem , 0 , 0 , CardCount);
                                                        _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , Temp);
                                                        IOitem = UnaryExpression(4 , AnyPtrPtr , node3(94 , AnyPtrPtr , ItemTemp , 0) , 0 , 0);
                                                        Temp = CallStrlen(UnaryExpression(4 , AnyPtrPtr , node3(94 , AnyPtrPtr , ItemTemp , 0) , 0 , 0));
                                                        Evalwidthdigits_EvalWrite(&_ptoc_LOCAL, Temp);
                                                        addcontrol_EvalWrite(&_ptoc_LOCAL, "%*.*s               ");
                                                        }
                                                    IOitem = MakeVal(IOitem);
                                                    }
                                                else
                                                    {
                                                    if (IsOrdinal(exptype) || IsPointer(exptype))
                                                        {
                                                        IOitem = EvalConvert(IntegerPtr , IOitem);
                                                        IOitem = MakeVal(IOitem);
                                                        if (_ptoc_LOCAL.IOwidth == -1)
                                                            {
                                                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%10d                ");
                                                            }
                                                        else
                                                            {
                                                            addcontrol_EvalWrite(&_ptoc_LOCAL, "%*d                 ");
                                                            }
                                                        }
                                                    else
                                                        {
                                                        ErrorMsg(1 , ExpColNo[index]);
                                                        IOitem = -1;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            if (IOitem != -1)
                {
                if (_ptoc_LOCAL.IOwidth != -1)
                    {
                    AppendList(6 , IntegerPtr , ParamList , MakeVal(_ptoc_LOCAL.IOwidth));
                    IOcount += 1;
                    }
                if (_ptoc_LOCAL.IOdigits != -1)
                    {
                    AppendList(6 , IntegerPtr , ParamList , MakeVal(_ptoc_LOCAL.IOdigits));
                    IOcount += 1;
                    }
                AppendList(6 , exptype , ParamList , IOitem);
                IOcount += 1;
                }
            index += 1;
            }
        field[IOcontrol + 5] = EnterString(&_ptoc_LOCAL.IOctlstring[0] , _ptoc_LOCAL.IOctllen);
        Temp = LongStatement(62 , IntegerPtr , -1 , ParamList , EnterConstantString("_fprintf            ") , IOcount , CardCount);
        _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , Temp);
        if (sysproc == 10 || sysproc == 6)
            {
            Temp = MakeList(6 , FileVarType , RefFile(FileTemp) , -1 , -1);
            Temp = LongStatement(62 , IntegerPtr , -1 , Temp , EnterConstantString("_sql__writeln       ") , 1 , CardCount);
            _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , Temp);
            }
        }
    else
        {
        FileComponentStype = STab[FileVarType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent;
        FileComponentType = PrimType(FileComponentStype);
        FilePtr = GetTemporary(ProcIndex , IntegerPtr , 0);
        Temp = LongStatement(6 , IntegerPtr , node3(94 , IntegerPtr , FilePtr , 0) , UnaryExpression(4 , IntegerPtr , UnaryExpression(97 , IntegerPtr , UnaryExpression(4 , TypePtrIorechd , node3(94 , TypePtrIorechd , FileTemp , 0) , 0 , 0) , 0 , 0) , 0 , 0) , 0 , 0 , CardCount);
        _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , Temp);
        while (index <= last)
            {
            NodeIndex = ExpVal[index];
            if ((field[NodeIndex] & 65535) == 122 || (field[NodeIndex] & 65535) == 121)
                {
                ErrorMsg(31 , ExpColNo[index]);
                }
            Temp = GenStore(UnaryExpression(97 , FileComponentType , UnaryExpression(4 , IntegerPtr , node3(94 , IntegerPtr , FilePtr , 0) , 0 , 0) , 0 , 0) , FileComponentType , FileComponentStype , ExpColNo[index] , ExpVal[index] , ExpType[index] , ExpStype[index] , ExpColNo[index] , CardCount);
            _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , Temp);
            _ptoc_LOCAL.TempList = LinkChain(_ptoc_LOCAL.TempList , CallPut(RefFile(FileTemp)));
            index += 1;
            }
        if (sysproc != 3)
            {
            ErrorMsg(31 , ExpColNo[StartCnt[ParmIndex]]);
            }
        }
    FreeTemporary(FileTemp);
    _EvalWrite = _ptoc_LOCAL.TempList;
    return(_EvalWrite);
    }

struct _ptoc_FRAME_EvalFlush
  {
  char dummy;
  } ;


int EvalFlush(p)
int p;
    {
    struct _ptoc_FRAME_EvalFlush _ptoc_LOCAL;
    int _EvalFlush;
    HdgRange NodeIndex;
    struct ptoc_Type1 *ptoc_Var28;
    
    if (cnt[p] == 0)
        {
        NodeIndex = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , OutputPtr , 0) , 0 , 0);
        SaveIndex = NodeIndex;
        NodeIndex = field[NodeIndex + 5];
        if ((field[NodeIndex] & 65535) == 94)
            {
            ptoc_Var28 = &STab[field[NodeIndex + 5]];
            if (ptoc_Var28->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var28->KINDTYPE.C_ModuleDcl.SClass == 5)
                {
                ptoc_Var28->KINDTYPE.C_ModuleDcl.SClass = 10;
                }
            }
        NodeIndex = SaveIndex;
        }
    else
        {
        NodeIndex = ExpVal[StartCnt[p]];
        MakeRef(&NodeIndex);
        }
    _EvalFlush = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , field[NodeIndex + 1] , NodeIndex , -1 , -1) , EnterConstantString("_sql__fflush        ") , 1 , CardCount);
    return(_EvalFlush);
    }

struct _ptoc_FRAME_EvalRemove
  {
  char dummy;
  } ;


int EvalRemove(p)
int p;
    {
    struct _ptoc_FRAME_EvalRemove _ptoc_LOCAL;
    int _EvalRemove;
    HdgRange FileName;
    HdgRange NameLen;
    HdgRange NodeIndex;
    SymbolTableRange FileNameTyp;
    int index;
    
    if (cnt[p] != 1)
        {
        ErrorMsg(8 , -1);
        _EvalRemove = -1;
        }
    else
        {
        index = StartCnt[p];
        FileName = ExpVal[index];
        FileNameTyp = ExpType[index];
        if (FileNameTyp == CharPtr && IsConstant(FileName))
            {
            CharToPac(FileName);
            FileNameTyp = PacPtr;
            }
        if (IsCharArray(FileNameTyp))
            {
            if (IsConstant(FileName))
                {
                MakeRef(&FileName);
                NodeIndex = FileName;
                NameLen = node3(12 , IntegerPtr , _GetStringLength(field[NodeIndex + 5]) , 0);
                }
            else
                {
                MakeRef(&FileName);
                NameLen = node3(12 , IntegerPtr , LengthOfCharArray(FileNameTyp) , 0);
                }
            _EvalRemove = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , FileName , NameLen , -1) , EnterConstantString("_sql__del           ") , 2 , CardCount);
            }
        else
            {
            ErrorMsg(133 , ExpColNo[index]);
            _EvalRemove = -1;
            }
        }
    return(_EvalRemove);
    }

struct _ptoc_FRAME_EvalEoln
  {
  char dummy;
  } ;


int EvalEoln(p)
int p;
    {
    struct _ptoc_FRAME_EvalEoln _ptoc_LOCAL;
    int _EvalEoln;
    HdgRange NodeIndex;
    struct ptoc_Type1 *ptoc_Var29;
    
    if (cnt[p] == 0)
        {
        NodeIndex = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , InputPtr , 0) , 0 , 0);
        SaveIndex = NodeIndex;
        NodeIndex = field[NodeIndex + 5];
        if ((field[NodeIndex] & 65535) == 94)
            {
            ptoc_Var29 = &STab[field[NodeIndex + 5]];
            if (ptoc_Var29->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var29->KINDTYPE.C_ModuleDcl.SClass == 5)
                {
                ptoc_Var29->KINDTYPE.C_ModuleDcl.SClass = 10;
                }
            }
        NodeIndex = SaveIndex;
        }
    else
        {
        NodeIndex = ExpVal[StartCnt[p]];
        MakeRef(&NodeIndex);
        }
    if (!IsTextFile(field[NodeIndex + 1]))
        {
        ErrorMsg(31 , ExpColNo[StartCnt[p]]);
        }
    _EvalEoln = LongStatement(62 , BooleanPtr , -1 , MakeList(6 , field[NodeIndex + 1] , NodeIndex , -1 , -1) , EnterConstantString("_sql__endol         ") , 1 , CardCount);
    return(_EvalEoln);
    }

struct _ptoc_FRAME_EvalEof
  {
  char dummy;
  } ;


int EvalEof(p)
int p;
    {
    struct _ptoc_FRAME_EvalEof _ptoc_LOCAL;
    int _EvalEof;
    HdgRange NodeIndex;
    struct ptoc_Type1 *ptoc_Var30;
    
    if (cnt[p] == 0)
        {
        NodeIndex = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , InputPtr , 0) , 0 , 0);
        SaveIndex = NodeIndex;
        NodeIndex = field[NodeIndex + 5];
        if ((field[NodeIndex] & 65535) == 94)
            {
            ptoc_Var30 = &STab[field[NodeIndex + 5]];
            if (ptoc_Var30->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var30->KINDTYPE.C_ModuleDcl.SClass == 5)
                {
                ptoc_Var30->KINDTYPE.C_ModuleDcl.SClass = 10;
                }
            }
        NodeIndex = SaveIndex;
        }
    else
        {
        NodeIndex = ExpVal[StartCnt[p]];
        MakeRef(&NodeIndex);
        }
    _EvalEof = LongStatement(62 , BooleanPtr , -1 , MakeList(6 , field[NodeIndex + 1] , NodeIndex , -1 , -1) , EnterConstantString("_sql__endof         ") , 1 , CardCount);
    return(_EvalEof);
    }

struct _ptoc_FRAME_EvalArgc
  {
  char dummy;
  } ;


int EvalArgc(p)
int p;
    {
    struct _ptoc_FRAME_EvalArgc _ptoc_LOCAL;
    int _EvalArgc;
    int NodeIndex;
    
    NodeIndex = node3(94 , IntegerPtr , _ArgcPtr , 0);
    _EvalArgc = NodeIndex;
    return(_EvalArgc);
    }

struct _ptoc_FRAME_EvalArgv
  {
  char dummy;
  } ;


int EvalArgv(p)
int p;
    {
    struct _ptoc_FRAME_EvalArgv _ptoc_LOCAL;
    int _EvalArgv;
    int index;
    HdgRange temp;
    HdgRange ArgNo;
    HdgRange ArgDest;
    HdgRange DestSize;
    
    index = StartCnt[p];
    ArgNo = ExpVal[index];
    index += 1;
    ArgDest = ExpVal[index];
    if (!AssgnmntCompatible(IntegerPtr , ExpVal[index + -1]))
        {
        ErrorMsg(1 , ExpColNo[index + -1]);
        _EvalArgv = -1;
        }
    else
        {
        if (IsCharArray(ExpType[index]))
            {
            MakeRef(&ArgDest);
            DestSize = node3(12 , IntegerPtr , LengthOfCharArray(ExpType[index]) , 0);
            temp = MakeList(6 , IntegerPtr , ArgNo , ArgDest , -1);
            AppendList(6 , IntegerPtr , temp , DestSize);
            temp = LongStatement(62 , IntegerPtr , -1 , temp , EnterConstantString("_sql__rgv           ") , 3 , CardCount);
            _EvalArgv = temp;
            }
        else
            {
            ErrorMsg(133 , ExpColNo[index]);
            _EvalArgv = -1;
            }
        }
    return(_EvalArgv);
    }

struct _ptoc_FRAME_EvalEnvp
  {
  char dummy;
  } ;


int EvalEnvp(p)
int p;
    {
    struct _ptoc_FRAME_EvalEnvp _ptoc_LOCAL;
    int _EvalEnvp;
    int index;
    HdgRange temp;
    HdgRange EnvName;
    HdgRange EnvNameSize;
    HdgRange EnvDest;
    HdgRange DestSize;
    SymbolTableRange EnvNameTyp;
    int length;
    
    index = StartCnt[p];
    EnvName = ExpVal[index];
    EnvNameTyp = ExpType[index];
    index += 1;
    EnvDest = ExpVal[index];
    if (!IsCharArray(EnvNameTyp) && EnvNameTyp != PacPtr && EnvNameTyp != CharPtr)
        {
        ErrorMsg(1 , ExpColNo[index + -1]);
        _EvalEnvp = -1;
        }
    else
        {
        if (IsCharArray(ExpType[index]))
            {
            MakeRef(&EnvName);
            if (EnvNameTyp == PacPtr || EnvNameTyp == CharPtr)
                {
                EnvNameSize = node3(12 , IntegerPtr , _GetStringLength(field[ExpVal[index + -1] + 5]) , 0);
                }
            else
                {
                EnvNameSize = node3(12 , IntegerPtr , LengthOfCharArray(EnvNameTyp) , 0);
                }
            MakeRef(&EnvDest);
            DestSize = node3(12 , IntegerPtr , LengthOfCharArray(ExpType[index]) , 0);
            temp = MakeList(6 , IntegerPtr , EnvName , EnvNameSize , EnvDest);
            AppendList(6 , IntegerPtr , temp , DestSize);
            temp = LongStatement(62 , IntegerPtr , -1 , temp , EnterConstantString("_sql__env           ") , 4 , CardCount);
            _EvalEnvp = temp;
            }
        else
            {
            ErrorMsg(133 , ExpColNo[index]);
            _EvalEnvp = -1;
            }
        }
    return(_EvalEnvp);
    }

struct _ptoc_FRAME_EvalNew
  {
  char dummy;
  } ;


int EvalNew(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_EvalNew _ptoc_LOCAL;
    int _EvalNew;
    int index;
    HdgRange temp;
    HdgRange NodeIndex;
    HdgRange ItemSize;
    
    if (cnt[p2] > 0)
        {
        index = StartCnt[p2];
        NodeIndex = ExpVal[index];
        if (IsPointer(ExpType[index]) && (field[NodeIndex] & 65535) != 12)
            {
            MakeRef(&NodeIndex);
            ItemSize = node3(69 , IntegerPtr , STab[STab[ExpType[index]].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain].SType , 0);
            temp = MakeList(6 , IntegerPtr , NodeIndex , ItemSize , -1);
            temp = LongStatement(62 , IntegerPtr , -1 , temp , EnterConstantString("_sql__new           ") , 2 , CardCount);
            _EvalNew = temp;
            }
        else
            {
            ErrorMsg(1 , ExpColNo[index]);
            _EvalNew = -1;
            }
        }
    else
        {
        ErrorMsg(8 , ColNo[p1]);
        _EvalNew = -1;
        }
    return(_EvalNew);
    }

struct _ptoc_FRAME_EvalDispose
  {
  char dummy;
  } ;


int EvalDispose(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_EvalDispose _ptoc_LOCAL;
    int _EvalDispose;
    int index;
    HdgRange temp;
    HdgRange NodeIndex;
    HdgRange ItemSize;
    SymbolTableRange ItemTemp;
    struct ptoc_Type1 *ptoc_Var31;
    int ptoc_Var32;
    int ptoc_Var33;
    
    if (cnt[p2] > 0)
        {
        index = StartCnt[p2];
        NodeIndex = ExpVal[index];
        if ((field[NodeIndex] & 65535) == 4)
            {
            ItemTemp = 0;
            temp = -1;
            MakeRef(&NodeIndex);
            }
        else
            {
            ItemTemp = GetTemporary(ProcIndex , field[NodeIndex + 1] , 0);
            ptoc_Var32 = field[NodeIndex + 1];
            temp = LongStatement(6 , ptoc_Var32 , node3(94 , ptoc_Var32 , ItemTemp , 0) , NodeIndex , 0 , 0 , CardCount);
            ptoc_Var33 = field[NodeIndex + 1];
            NodeIndex = UnaryExpression(9 , ptoc_Var33 , node3(94 , ptoc_Var33 , ItemTemp , 0) , 0 , 0);
            SaveIndex = NodeIndex;
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 94)
                {
                ptoc_Var31 = &STab[field[NodeIndex + 5]];
                if (ptoc_Var31->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var31->KINDTYPE.C_ModuleDcl.SClass == 5)
                    {
                    ptoc_Var31->KINDTYPE.C_ModuleDcl.SClass = 10;
                    }
                }
            NodeIndex = SaveIndex;
            }
        ItemSize = node3(69 , IntegerPtr , STab[STab[ExpType[index]].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain].SType , 0);
        NodeIndex = MakeList(6 , IntegerPtr , NodeIndex , ItemSize , -1);
        temp = LinkChain(temp , LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__dispse        ") , 1 , CardCount));
        _EvalDispose = temp;
        }
    else
        {
        ErrorMsg(8 , ColNo[p1]);
        _EvalDispose = -1;
        }
    return(_EvalDispose);
    }

struct _ptoc_FRAME_EvalPut
  {
  char dummy;
  } ;


int EvalPut(p)
int p;
    {
    struct _ptoc_FRAME_EvalPut _ptoc_LOCAL;
    int _EvalPut;
    int index;
    HdgRange FileExp;
    
    index = StartCnt[p];
    FileExp = ExpVal[index];
    MakeRef(&FileExp);
    _EvalPut = CallPut(FileExp);
    return(_EvalPut);
    }

struct _ptoc_FRAME_EvalGet
  {
  char dummy;
  } ;


int EvalGet(p)
int p;
    {
    struct _ptoc_FRAME_EvalGet _ptoc_LOCAL;
    int _EvalGet;
    int index;
    HdgRange FileExp;
    
    index = StartCnt[p];
    FileExp = ExpVal[index];
    MakeRef(&FileExp);
    _EvalGet = CallUGet(FileExp);
    return(_EvalGet);
    }

struct _ptoc_FRAME_EvalPage
  {
  char dummy;
  } ;


int EvalPage(p)
int p;
    {
    struct _ptoc_FRAME_EvalPage _ptoc_LOCAL;
    int _EvalPage;
    int index;
    HdgRange FileExp;
    HdgRange NodeIndex;
    struct ptoc_Type1 *ptoc_Var34;
    
    if (cnt[p] == 0)
        {
        FileExp = UnaryExpression(9 , TextPtr , node3(94 , TextPtr , OutputPtr , 0) , 0 , 0);
        SaveIndex = NodeIndex;
        NodeIndex = FileExp;
        NodeIndex = field[NodeIndex + 5];
        if ((field[NodeIndex] & 65535) == 94)
            {
            ptoc_Var34 = &STab[field[NodeIndex + 5]];
            if (ptoc_Var34->KINDTYPE.C_ModuleDcl.SClass == 1 || ptoc_Var34->KINDTYPE.C_ModuleDcl.SClass == 5)
                {
                ptoc_Var34->KINDTYPE.C_ModuleDcl.SClass = 10;
                }
            }
        NodeIndex = SaveIndex;
        _EvalPage = CallPage(CallUnit(FileExp));
        }
    else
        {
        index = StartCnt[p];
        if (ExpType[index] != TextPtr)
            {
            ErrorMsg(31 , ExpColNo[index]);
            StackError(p);
            _EvalPage = -1;
            }
        else
            {
            FileExp = ExpVal[index];
            MakeRef(&FileExp);
            _EvalPage = CallPage(CallUnit(FileExp));
            }
        }
    return(_EvalPage);
    }

struct _ptoc_FRAME_EvalMath
  {
  char dummy;
  } ;


int EvalMath(sysproc, p1, p2)
int sysproc;
int p1;
int p2;
    {
    struct _ptoc_FRAME_EvalMath _ptoc_LOCAL;
    int routine;
    
    switch (sysproc)
        {
        case 21 :
            routine = EnterConstantString("_sql__atan          ");
            break;
        case 22 :
            routine = EnterConstantString("_sql__cos           ");
            break;
        case 23 :
            routine = EnterConstantString("_sql__exp           ");
            break;
        case 24 :
            routine = EnterConstantString("_sql__ln            ");
            break;
        case 29 :
            routine = EnterConstantString("_sql__sin           ");
            break;
        case 31 :
            routine = EnterConstantString("_sql__sqrt          ");
            break;
        }
    g1[p1] = LongStatement(62 , DoublePtr , -1 , MakeList(6 , DoublePtr , EvalConvert(DoublePtr , ExpVal[StartCnt[p2]]) , -1 , -1) , routine , 1 , CardCount);
    SubType[p1] = DoublePtr;
    TypeType[p1] = DoublePtr;
    }

struct _ptoc_FRAME_EvalPackUnpack
  {
  char dummy;
  } ;


int EvalPackUnpack(sysstring, i, a, z)
int sysstring;
int i;
int a;
int z;
    {
    struct _ptoc_FRAME_EvalPackUnpack _ptoc_LOCAL;
    int _EvalPackUnpack;
    int m;
    int n;
    int u;
    int v;
    SymbolTableRange typ;
    SymbolTableRange typa;
    SymbolTableRange typz;
    HdgRange temp;
    
    typa = ExpType[a];
    typz = ExpType[z];
    if (STab[typa].SType != -13)
        {
        ErrorMsg(21 , ExpColNo[a]);
        }
    if (STab[typz].SType != -13)
        {
        ErrorMsg(21 , ExpColNo[z]);
        }
    if (STab[typa].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement != STab[typz].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement)
        {
        ErrorMsg(0 , ExpColNo[z]);
        }
    temp = MakeList(6 , IntegerPtr , -1 , -1 , -1);
    AppendList(6 , IntegerPtr , temp , ExpVal[i]);
    MakeRef(&ExpVal[a]);
    AppendList(6 , IntegerPtr , temp , ExpVal[a]);
    MakeRef(&ExpVal[z]);
    AppendList(6 , IntegerPtr , temp , ExpVal[z]);
    AppendList(6 , IntegerPtr , temp , node3(69 , IntegerPtr , STab[typa].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , 0));
    typ = STab[typa].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
    if (!AssgnmntCompatible(PrimType(typ) , ExpVal[i]))
        {
        ErrorMsg(1 , ExpColNo[i]);
        }
    m = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
    n = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange;
    typ = STab[ExpType[z]].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
    u = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange;
    v = STab[typ].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange;
    AppendList(6 , IntegerPtr , temp , node3(12 , IntegerPtr , m , 0));
    AppendList(6 , IntegerPtr , temp , node3(12 , IntegerPtr , n - m - (v - u) , 0));
    AppendList(6 , IntegerPtr , temp , node3(69 , IntegerPtr , ExpType[z] , 0));
    _EvalPackUnpack = LongStatement(62 , IntegerPtr , -1 , temp , sysstring , 0 , CardCount);
    return(_EvalPackUnpack);
    }

struct _ptoc_FRAME_EvalDate
  {
  char dummy;
  } ;


int EvalDate(p)
int p;
    {
    struct _ptoc_FRAME_EvalDate _ptoc_LOCAL;
    int _EvalDate;
    HdgRange DateString;
    int index;
    
    if (cnt[p] != 1)
        {
        ErrorMsg(8 , -1);
        _EvalDate = -1;
        }
    else
        {
        index = StartCnt[p];
        DateString = ExpVal[index];
        if (IsCharArray(ExpType[index]))
            {
            if (IsLval(DateString))
                {
                MakeRef(&DateString);
                _EvalDate = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , DateString , -1 , -1) , EnterConstantString("_sql__date          ") , 1 , CardCount);
                }
            else
                {
                ErrorMsg(133 , ExpColNo[index]);
                _EvalDate = -1;
                }
            }
        else
            {
            ErrorMsg(133 , ExpColNo[index]);
            _EvalDate = -1;
            }
        }
    return(_EvalDate);
    }

struct _ptoc_FRAME_EvalTime
  {
  char dummy;
  } ;


int EvalTime(p)
int p;
    {
    struct _ptoc_FRAME_EvalTime _ptoc_LOCAL;
    int _EvalTime;
    HdgRange TimeString;
    int index;
    
    if (cnt[p] != 1)
        {
        ErrorMsg(8 , -1);
        _EvalTime = -1;
        }
    else
        {
        index = StartCnt[p];
        TimeString = ExpVal[index];
        if (IsCharArray(ExpType[index]))
            {
            if (IsConstant(TimeString))
                {
                ErrorMsg(133 , ExpColNo[index]);
                _EvalTime = -1;
                }
            else
                {
                MakeRef(&TimeString);
                _EvalTime = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , TimeString , -1 , -1) , EnterConstantString("_sql__time          ") , 1 , CardCount);
                }
            }
        else
            {
            ErrorMsg(133 , ExpColNo[index]);
            _EvalTime = -1;
            }
        }
    return(_EvalTime);
    }

struct _ptoc_FRAME_EvalLinelimit
  {
  char dummy;
  } ;


int EvalLinelimit(ParmIndex)
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalLinelimit _ptoc_LOCAL;
    int _EvalLinelimit;
    int index;
    HdgRange NodeIndex;
    HdgRange FileVar;
    HdgRange LineLimit;
    
    if (cnt[ParmIndex] == 2)
        {
        index = StartCnt[ParmIndex];
        FileVar = ExpVal[index];
        if (IsTextFile(ExpType[index]))
            {
            MakeRef(&FileVar);
            index += 1;
            if (ExpType[index] == IntegerPtr)
                {
                LineLimit = ExpVal[index];
                NodeIndex = MakeList(6 , IntegerPtr , FileVar , LineLimit , -1);
                NodeIndex = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__lexe          ") , 2 , CardCount);
                _EvalLinelimit = NodeIndex;
                }
            else
                {
                ErrorMsg(0 , ExpColNo[index]);
                _EvalLinelimit = -1;
                }
            }
        else
            {
            ErrorMsg(31 , ExpColNo[index]);
            _EvalLinelimit = -1;
            }
        }
    else
        {
        ErrorMsg(8 , -1);
        _EvalLinelimit = -1;
        }
    return(_EvalLinelimit);
    }

struct _ptoc_FRAME_EvalCard
  {
  char dummy;
  } ;


int EvalCard(ParmIndex)
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalCard _ptoc_LOCAL;
    int _EvalCard;
    int index;
    SymbolTableRange typ;
    SymbolTableRange temp;
    HdgRange Set;
    HdgRange size;
    
    if (cnt[ParmIndex] == 1)
        {
        index = StartCnt[ParmIndex];
        typ = ExpType[index];
        if (IsSet(typ))
            {
            Set = ExpVal[index];
            if (IsLval(Set))
                {
                temp = 0;
                MakeRef(&Set);
                }
            else
                {
                temp = GetTemporary(ProcIndex , typ , 0);
                Set = UnaryExpression(9 , typ , UnaryExpression(97 , typ , EvalSetExp(node3(94 , typ , temp , 0) , Set) , 0 , 0) , 0 , 0);
                }
            size = UnaryExpression(8 , IntegerPtr , node3(69 , IntegerPtr , typ , 0) , 0 , 0);
            _EvalCard = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , Set , size , -1) , EnterConstantString("_sql__ca            ") , 2 , CardCount);
            if (temp != 0)
                {
                FreeTemporary(temp);
                }
            }
        else
            {
            ErrorMsg(0 , ExpColNo[index]);
            _EvalCard = -1;
            }
        }
    else
        {
        ErrorMsg(8 , -1);
        _EvalCard = -1;
        }
    return(_EvalCard);
    }

struct _ptoc_FRAME_EvalExpo
  {
  char dummy;
  } ;


int EvalExpo(ParmIndex)
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalExpo _ptoc_LOCAL;
    int _EvalExpo;
    int index;
    HdgRange NodeIndex;
    
    if (cnt[ParmIndex] == 1)
        {
        index = StartCnt[ParmIndex];
        if (AssgnmntCompatible(DoublePtr , ExpVal[index]))
            {
            NodeIndex = MakeList(6 , IntegerPtr , UnaryExpression(8 , DoublePtr , EvalConvert(DoublePtr , ExpVal[index]) , 0 , 0) , -1 , -1);
            NodeIndex = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__expp          ") , 1 , CardCount);
            _EvalExpo = NodeIndex;
            }
        else
            {
            ErrorMsg(0 , ExpColNo[index]);
            _EvalExpo = -1;
            }
        }
    else
        {
        ErrorMsg(8 , -1);
        _EvalExpo = -1;
        }
    return(_EvalExpo);
    }

struct _ptoc_FRAME_EvalRandom
  {
  char dummy;
  } ;


int EvalRandom(ParmIndex)
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalRandom _ptoc_LOCAL;
    int _EvalRandom;
    int index;
    HdgRange NodeIndex;
    
    if (cnt[ParmIndex] == 1)
        {
        index = StartCnt[ParmIndex];
        if (AssgnmntCompatible(DoublePtr , ExpVal[index]))
            {
            NodeIndex = MakeList(6 , IntegerPtr , UnaryExpression(8 , DoublePtr , EvalConvert(DoublePtr , ExpVal[index]) , 0 , 0) , -1 , -1);
            NodeIndex = LongStatement(62 , DoublePtr , -1 , NodeIndex , EnterConstantString("_sql__rd            ") , 1 , CardCount);
            _EvalRandom = NodeIndex;
            }
        else
            {
            ErrorMsg(0 , ExpColNo[index]);
            _EvalRandom = -1;
            }
        }
    else
        {
        ErrorMsg(8 , -1);
        _EvalRandom = -1;
        }
    return(_EvalRandom);
    }

struct _ptoc_FRAME_EvalSeed
  {
  char dummy;
  } ;


int EvalSeed(ParmIndex)
int ParmIndex;
    {
    struct _ptoc_FRAME_EvalSeed _ptoc_LOCAL;
    int _EvalSeed;
    int index;
    HdgRange NodeIndex;
    
    if (cnt[ParmIndex] == 1)
        {
        index = StartCnt[ParmIndex];
        if (AssgnmntCompatible(IntegerPtr , ExpVal[index]))
            {
            NodeIndex = MakeList(6 , IntegerPtr , UnaryExpression(8 , IntegerPtr , EvalConvert(IntegerPtr , ExpVal[index]) , 0 , 0) , -1 , -1);
            NodeIndex = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__srand         ") , 1 , CardCount);
            _EvalSeed = NodeIndex;
            }
        else
            {
            ErrorMsg(0 , ExpColNo[index]);
            _EvalSeed = -1;
            }
        }
    else
        {
        ErrorMsg(8 , -1);
        _EvalSeed = -1;
        }
    return(_EvalSeed);
    }

struct _ptoc_FRAME_EvalLength
  {
  char dummy;
  } ;


int EvalLength(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_EvalLength _ptoc_LOCAL;
    int index;
    HdgRange NodeIndex;
    
    if (cnt[p2] == 1)
        {
        index = StartCnt[p2];
        if (!AssgnmntCompatible(StringPtr , ExpVal[index]))
            {
            ErrorMsg(0 , ExpColNo[index]);
            StackError(p1);
            goto LAB_100;
            }
        NodeIndex = ExpVal[index];
        if (IsConstant(NodeIndex))
            {
            g1[p1] = node3(12 , IntegerPtr , field[NodeIndex + 6] , 0);
            }
        else
            {
            g1[p1] = EvalConvert(IntegerPtr , StringElement(NodeIndex , 0));
            }
        SubType[p1] = IntegerPtr;
        TypeType[p1] = IntegerPtr;
        }
    else
        {
        ErrorMsg(8 , ColNo[p1]);
        StackError(p1);
        goto LAB_100;
        }
    LAB_100 :
    ;
    }

struct _ptoc_FRAME_EvalConcat
  {
  char dummy;
  } ;


int EvalConcat(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_EvalConcat _ptoc_LOCAL;
    int i;
    int index;
    HdgRange NodeIndex;
    int ptoc_Var35;
    
    if (cnt[p2] >= 2)
        {
        index = StartCnt[p2];
        if (!AssgnmntCompatible(StringPtr , ExpVal[index]))
            {
            ErrorMsg(0 , ExpColNo[index]);
            StackError(p1);
            goto LAB_100;
            }
        NodeIndex = EvalConvert(StringPtr , ExpVal[index]);
        ptoc_Var35 = cnt[p2] + -1;
        if (1 <= ptoc_Var35)
            {
            i = 1;
            do
                {
                if (!AssgnmntCompatible(StringPtr , ExpVal[index + i]))
                    {
                    ErrorMsg(0 , ExpColNo[index + i]);
                    StackError(p1);
                    goto LAB_100;
                    }
                NodeIndex = BinaryExpression(123 , StringPtr , NodeIndex , EvalConvert(StringPtr , ExpVal[index + i]));
                if (i == ptoc_Var35)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        g1[p1] = NodeIndex;
        SubType[p1] = StringPtr;
        TypeType[p1] = StringPtr;
        }
    else
        {
        ErrorMsg(8 , ColNo[p1]);
        StackError(p1);
        goto LAB_100;
        }
    LAB_100 :
    ;
    }

struct _ptoc_FRAME_EvalCopy
  {
  char dummy;
  } ;


int EvalCopy(p1, p2)
ParseStackRange p1;
ParseStackRange p2;
    {
    struct _ptoc_FRAME_EvalCopy _ptoc_LOCAL;
    int index;
    
    if (cnt[p2] == 3)
        {
        index = StartCnt[p2];
        if (!AssgnmntCompatible(StringPtr , ExpVal[index]))
            {
            ErrorMsg(0 , ExpColNo[index]);
            StackError(p1);
            goto LAB_100;
            }
        if (!AssgnmntCompatible(IntegerPtr , ExpVal[index + 1]))
            {
            ErrorMsg(0 , ExpColNo[index + 1]);
            StackError(p1);
            goto LAB_100;
            }
        if (!AssgnmntCompatible(IntegerPtr , ExpVal[index + 2]))
            {
            ErrorMsg(0 , ExpColNo[index + 2]);
            StackError(p1);
            goto LAB_100;
            }
        g1[p1] = UnaryExpression(124 , StringPtr , EvalConvert(StringPtr , ExpVal[index]) , EvalConvert(IntegerPtr , ExpVal[index + 1]) , EvalConvert(IntegerPtr , ExpVal[index + 2]));
        SubType[p1] = StringPtr;
        TypeType[p1] = StringPtr;
        }
    else
        {
        ErrorMsg(8 , -1);
        StackError(p1);
        }
    LAB_100 :
    ;
    }

struct _ptoc_FRAME_EvalPred
  {
  char dummy;
  } ;


int EvalPred(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_EvalPred _ptoc_LOCAL;
    int index;
    SymbolTableRange typ;
    SymbolTableRange styp;
    HdgRange NodeIndex;
    int ptoc_Var36;
    
    index = StartCnt[p2];
    typ = ExpType[index];
    styp = ExpStype[index];
    if (typ == FloatPtr || typ == LongPtr || typ == DoublePtr)
        {
        ErrorMsg(9 , ExpColNo[index]);
        StackError(p1);
        }
    else
        {
        NodeIndex = EvalConvert(IntegerPtr , ExpVal[index]);
        if (IsConstant(NodeIndex))
            {
            if (control[114] && field[NodeIndex + 5] == STab[styp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange)
                {
                ErrorMsg(148 , ColNo[p1]);
                }
            else
                {
                field[NodeIndex + 5] += -1;
                }
            }
        else
            {
            if (control[114])
                {
                NodeIndex = MakeList(6 , IntegerPtr , NodeIndex , node3(12 , IntegerPtr , STab[styp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange , 0) , node3(12 , IntegerPtr , STab[styp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange , 0));
                AppendList(6 , IntegerPtr , NodeIndex , node3(14 , PtrToCharPtr , CurrentFile , 0));
                AppendList(6 , IntegerPtr , NodeIndex , node3(12 , IntegerPtr , CardNum[p1] , 0));
                NodeIndex = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__pred          ") , 5 , CardCount);
                }
            else
                {
                NodeIndex = BinaryExpression(20 , IntegerPtr , NodeIndex , node3(12 , IntegerPtr , 1 , 0));
                }
            }
        g1[p1] = EvalConvert(typ , NodeIndex);
        ptoc_Var36 = ExpType[index];
        SubType[p1] = ptoc_Var36;
        TypeType[p1] = ptoc_Var36;
        }
    }

struct _ptoc_FRAME_EvalSucc
  {
  char dummy;
  } ;


int EvalSucc(p1, p2)
int p1;
int p2;
    {
    struct _ptoc_FRAME_EvalSucc _ptoc_LOCAL;
    int index;
    SymbolTableRange typ;
    SymbolTableRange styp;
    HdgRange NodeIndex;
    int ptoc_Var37;
    
    index = StartCnt[p2];
    typ = ExpType[index];
    styp = ExpStype[index];
    if (typ == FloatPtr || typ == LongPtr || typ == DoublePtr)
        {
        ErrorMsg(9 , ExpColNo[index]);
        StackError(p1);
        }
    else
        {
        NodeIndex = EvalConvert(IntegerPtr , ExpVal[index]);
        if (IsConstant(NodeIndex))
            {
            if (control[114] && field[NodeIndex + 5] == STab[styp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange)
                {
                ErrorMsg(147 , ColNo[p1]);
                }
            else
                {
                field[NodeIndex + 5] += 1;
                }
            }
        else
            {
            if (control[114])
                {
                NodeIndex = MakeList(6 , IntegerPtr , NodeIndex , node3(12 , IntegerPtr , STab[styp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange , 0) , node3(12 , IntegerPtr , STab[styp].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange , 0));
                AppendList(6 , IntegerPtr , NodeIndex , node3(14 , PtrToCharPtr , CurrentFile , 0));
                AppendList(6 , IntegerPtr , NodeIndex , node3(12 , IntegerPtr , CardNum[p1] , 0));
                NodeIndex = LongStatement(62 , IntegerPtr , -1 , NodeIndex , EnterConstantString("_sql__succ          ") , 5 , CardCount);
                }
            else
                {
                NodeIndex = BinaryExpression(19 , IntegerPtr , NodeIndex , node3(12 , IntegerPtr , 1 , 0));
                }
            }
        g1[p1] = EvalConvert(typ , NodeIndex);
        ptoc_Var37 = ExpType[index];
        SubType[p1] = ptoc_Var37;
        TypeType[p1] = ptoc_Var37;
        }
    }

struct _ptoc_FRAME_EvalSys
  {
  char dummy;
  } ;


int EvalSys(ProcOrFunc, sysproc, p1, p2)
int ProcOrFunc;
int sysproc;
int p1;
int p2;
    {
    struct _ptoc_FRAME_EvalSys _ptoc_LOCAL;
    int index;
    SymbolTableRange typ;
    SymbolTableRange tmpvar;
    HdgRange NodeIndex;
    HdgRange temp;
    int ptoc_Var38;
    int ptoc_Var39;
    
    ptoc_Var38 = StartCnt[p2];
    ptoc_Var39 = StartCnt[p2] + cnt[p2] + -1;
    if (ptoc_Var38 <= ptoc_Var39)
        {
        index = ptoc_Var38;
        do
            {
            if (ExpVal[index] != -1 && ((ptoc_Var38 = field[ExpVal[index]] % 16384) >= 0 ? ptoc_Var38 : ptoc_Var38 + 16384) == 40)
                {
                CheckCall(ExpVal[index] , ExpColNo[index]);
                }
            if (index == ptoc_Var39)
                {
                break;
                }
            index += 1;
            }
        while (!0);
        }
    switch (sysproc)
        {
        case 0 :
            g1[p1] = EvalReset(p2);
            break;
        case 1 :
            g1[p1] = EvalRewrite(p2);
            break;
        case 9 :
            g1[p1] = EvalRead(ProcOrFunc , sysproc , p2);
            break;
        case 2 :
            if (cnt[p2] == 0)
                {
                ErrorMsg(8 , ColNo[p1]);
                g1[p1] = -1;
                }
            else
                {
                if (cnt[p2] == 1)
                    {
                    index = StartCnt[p2];
                    if (IsFile(ExpType[index]))
                        {
                        ErrorMsg(8 , ColNo[p2]);
                        g1[p1] = -1;
                        }
                    else
                        {
                        g1[p1] = EvalRead(ProcOrFunc , sysproc , p2);
                        }
                    }
                else
                    {
                    g1[p1] = EvalRead(ProcOrFunc , sysproc , p2);
                    }
                }
            break;
        case 10 :
        case 6 :
            g1[p1] = EvalWrite(ProcOrFunc , sysproc , p2);
            break;
        case 3 :
            if (cnt[p2] == 0)
                {
                ErrorMsg(8 , ColNo[p1]);
                g1[p1] = -1;
                }
            else
                {
                if (cnt[p2] == 1)
                    {
                    index = StartCnt[p2];
                    if (IsFile(ExpType[index]))
                        {
                        ErrorMsg(8 , ColNo[p2]);
                        g1[p1] = -1;
                        }
                    else
                        {
                        g1[p1] = EvalWrite(ProcOrFunc , sysproc , p2);
                        }
                    }
                else
                    {
                    g1[p1] = EvalWrite(ProcOrFunc , sysproc , p2);
                    }
                }
            break;
        case 4 :
            g1[p1] = EvalFlush(p2);
            break;
        case 5 :
            g1[p1] = EvalRemove(p2);
            break;
        case 7 :
        case 53 :
            if (cnt[p2] == 1)
                {
                index = StartCnt[p2];
                if (IsOrdinal(ExpType[index]) || IsPointer(ExpType[index]))
                    {
                    g1[p1] = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , IntegerPtr , FoldUnary(93 , IntegerPtr , ExpVal[index]) , -1 , -1) , EnterConstantString("_sql__ord           ") , 1 , CardCount);
                    SubType[p1] = IntegerPtr;
                    TypeType[p1] = IntegerPtr;
                    }
                else
                    {
                    ErrorMsg(9 , ExpColNo[index]);
                    StackError(p1);
                    }
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 52 :
            if (cnt[p2] == 1)
                {
                index = StartCnt[p2];
                if (IsOrdinal(ExpType[index]))
                    {
                    g1[p1] = FoldUnary(93 , ShortPtr , ExpVal[index]);
                    SubType[p1] = ShortPtr;
                    TypeType[p1] = ShortPtr;
                    }
                else
                    {
                    ErrorMsg(9 , ExpColNo[index]);
                    StackError(p1);
                    }
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 8 :
            index = StartCnt[p2];
            if (IsOrdinal(ExpType[index]))
                {
                g1[p1] = FoldUnary(93 , CharPtr , ExpVal[StartCnt[p2]]);
                SubType[p1] = CharPtr;
                TypeType[p1] = CharPtr;
                }
            else
                {
                ErrorMsg(1 , ExpColNo[index]);
                g1[p1] = FoldUnary(93 , CharPtr , node3(12 , IntegerPtr , 0 , 0));
                SubType[p1] = CharPtr;
                TypeType[p1] = CharPtr;
                }
            break;
        case 11 :
            g1[p1] = EvalEoln(p2);
            SubType[p1] = BooleanPtr;
            TypeType[p1] = BooleanPtr;
            break;
        case 12 :
            g1[p1] = EvalEof(p2);
            SubType[p1] = BooleanPtr;
            TypeType[p1] = BooleanPtr;
            break;
        case 13 :
            g1[p1] = EvalArgc(p2);
            SubType[p1] = IntegerPtr;
            TypeType[p1] = IntegerPtr;
            break;
        case 14 :
            if (cnt[p2] == 2)
                {
                g1[p1] = EvalArgv(p2);
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 56 :
            if (cnt[p2] == 2)
                {
                g1[p1] = EvalEnvp(p2);
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 57 :
            index = StartCnt[p2];
            if (cnt[p2] != 1)
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            else
                {
                if (ExpType[index] == PacPtr)
                    {
                    asm_esc = 1;
                    temp = special(106 , 0 , field[ExpVal[index] + 5] , ExpColNo[index]);
                    NodeIndex = special(106 , 0 , EnterNewLine() , ExpColNo[index]);
                    g1[p1] = LinkChain(temp , NodeIndex);
                    }
                else
                    {
                    ErrorMsg(1 , ExpColNo[index]);
                    g1[p1] = -1;
                    }
                }
            break;
        case 15 :
            g1[p1] = EvalNew(p1 , p2);
            TypeType[p1] = -21;
            SubType[p1] = -21;
            break;
        case 16 :
            g1[p1] = EvalDispose(p1 , p2);
            TypeType[p1] = -21;
            SubType[p1] = -21;
            break;
        case 17 :
            if (cnt[p2] == 1)
                {
                g1[p1] = EvalPut(p2);
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 18 :
            if (cnt[p2] == 1)
                {
                g1[p1] = EvalGet(p2);
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 19 :
            g1[p1] = EvalPage(p2);
            break;
        case 25 :
            index = StartCnt[p2];
            if (IsOrdinal(ExpType[index]))
                {
                g1[p1] = UnaryExpression(93 , BooleanPtr , BinaryExpression(89 , IntegerPtr , UnaryExpression(93 , IntegerPtr , ExpVal[StartCnt[p2]] , 0 , 0) , node3(12 , IntegerPtr , 1 , 0)) , 0 , 0);
                SubType[p1] = BooleanPtr;
                TypeType[p1] = BooleanPtr;
                }
            else
                {
                ErrorMsg(1 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 27 :
            if (cnt[p2] == 1)
                {
                EvalPred(p1 , p2);
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 32 :
            if (cnt[p2] == 1)
                {
                EvalSucc(p1 , p2);
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 20 :
            index = StartCnt[p2];
            typ = ExpType[index];
            switch (STab[typ].SType)
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
                    if (IsConstant(ExpVal[index]))
                        {
                        NodeIndex = ExpVal[index];
                        if (IsNegative(NodeIndex))
                            {
                            NodeIndex = FoldUnary(24 , typ , NodeIndex);
                            }
                        g1[p1] = NodeIndex;
                        }
                    else
                        {
                        tmpvar = GetTemporary(ProcIndex , typ , 0);
                        NodeIndex = UnaryExpression(4 , typ , node3(94 , typ , tmpvar , 0) , 0 , 0);
                        temp = ComplexStmnt(8 , typ , BinaryExpression(35 , typ , CopyTree(NodeIndex) , node3(12 , typ , 0 , 0)) , NodeIndex , UnaryExpression(24 , typ , CopyTree(NodeIndex) , 0 , 0) , CardCount);
                        g1[p1] = MakeList(6 , typ , LongStatement(6 , typ , node3(94 , typ , tmpvar , 0) , ExpVal[index] , 0 , 0 , CardCount) , temp , -1);
                        FreeTemporary(tmpvar);
                        }
                    TypeType[p1] = typ;
                    SubType[p1] = typ;
                    break;
                default :
                    ErrorMsg(1 , ColNo[p1]);
                    g1[p1] = -1;
                    TypeType[p1] = -21;
                    SubType[p1] = -21;
                    break;
                }
            break;
        case 21 :
        case 22 :
        case 23 :
        case 24 :
        case 29 :
        case 31 :
            if (cnt[p2] == 1)
                {
                EvalMath(sysproc , p1 , p2);
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 28 :
            if (cnt[p2] == 1)
                {
                index = StartCnt[p2];
                typ = ExpType[index];
                if (typ != FloatPtr && typ != LongPtr && typ != DoublePtr)
                    {
                    ErrorMsg(9 , ExpColNo[index]);
                    StackError(p1);
                    }
                else
                    {
                    g1[p1] = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , DoublePtr , EvalConvert(DoublePtr , ExpVal[index]) , -1 , -1) , EnterConstantString("_sql__round         ") , 1 , CardCount);
                    SubType[p1] = IntegerPtr;
                    TypeType[p1] = IntegerPtr;
                    }
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 30 :
            if (cnt[p2] == 1)
                {
                index = StartCnt[p2];
                typ = ExpType[index];
                NodeIndex = ExpVal[index];
                switch (STab[typ].SType)
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
                        g1[p1] = FoldBinary(21 , typ , CopyTree(NodeIndex) , NodeIndex);
                        TypeType[p1] = typ;
                        SubType[p1] = typ;
                        break;
                    default :
                        ErrorMsg(1 , ColNo[p1]);
                        g1[p1] = -1;
                        TypeType[p1] = -21;
                        SubType[p1] = -21;
                        break;
                    }
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 33 :
            if (cnt[p2] == 1)
                {
                index = StartCnt[p2];
                typ = ExpType[index];
                if (typ != FloatPtr && typ != LongPtr && typ != DoublePtr)
                    {
                    ErrorMsg(9 , ExpColNo[index]);
                    StackError(p1);
                    }
                else
                    {
                    g1[p1] = LongStatement(62 , IntegerPtr , -1 , MakeList(6 , DoublePtr , EvalConvert(DoublePtr , ExpVal[index]) , -1 , -1) , EnterConstantString("_sql__trunc         ") , 1 , CardCount);
                    SubType[p1] = IntegerPtr;
                    TypeType[p1] = IntegerPtr;
                    }
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 26 :
            index = StartCnt[p2];
            if (cnt[p2] == 3)
                {
                g1[p1] = EvalPackUnpack(EnterConstantString("_sql__pack          ") , index + 1 , index , index + 2);
                }
            else
                {
                ErrorMsg(8 , ExpColNo[index + cnt[p2] + -1]);
                StackError(p1);
                }
            break;
        case 34 :
            index = StartCnt[p2];
            if (cnt[p2] == 3)
                {
                g1[p1] = EvalPackUnpack(EnterConstantString("_sql__unpack        ") , index + 2 , index + 1 , index);
                }
            else
                {
                ErrorMsg(8 , ExpColNo[index + cnt[p2] + -1]);
                StackError(p1);
                }
            break;
        case 35 :
            if (cnt[p2] > 1)
                {
                ErrorMsg(8 , ExpColNo[StartCnt[p2] + 1]);
                g1[p1] = -1;
                }
            else
                {
                g1[p1] = CallHalt(p2);
                }
            break;
        case 36 :
            g1[p1] = CallClock();
            SubType[p1] = IntegerPtr;
            TypeType[p1] = IntegerPtr;
            break;
        case 37 :
            g1[p1] = CallSysclock();
            SubType[p1] = IntegerPtr;
            TypeType[p1] = IntegerPtr;
            break;
        case 38 :
            g1[p1] = CallWallclock();
            SubType[p1] = IntegerPtr;
            TypeType[p1] = IntegerPtr;
            break;
        case 39 :
            if (cnt[p2] == 1)
                {
                g1[p1] = FoldUnary(93 , AnyPtrPtr , ExpVal[StartCnt[p2]]);
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            SubType[p1] = AnyPtrPtr;
            TypeType[p1] = AnyPtrPtr;
            break;
        case 40 :
            g1[p1] = EvalDate(p2);
            break;
        case 41 :
            g1[p1] = EvalTime(p2);
            break;
        case 42 :
            g1[p1] = EvalLinelimit(p2);
            break;
        case 43 :
            g1[p1] = -1;
            break;
        case 44 :
            g1[p1] = EvalCard(p2);
            SubType[p1] = IntegerPtr;
            TypeType[p1] = IntegerPtr;
            break;
        case 45 :
            g1[p1] = EvalExpo(p2);
            SubType[p1] = IntegerPtr;
            TypeType[p1] = IntegerPtr;
            break;
        case 46 :
            g1[p1] = EvalRandom(p2);
            SubType[p1] = DoublePtr;
            TypeType[p1] = DoublePtr;
            break;
        case 47 :
            g1[p1] = EvalSeed(p2);
            SubType[p1] = IntegerPtr;
            TypeType[p1] = IntegerPtr;
            break;
        case 48 :
            g1[p1] = node3(12 , BooleanPtr , 0 , 0);
            SubType[p1] = BooleanPtr;
            TypeType[p1] = BooleanPtr;
            break;
        case 49 :
            EvalLength(p1 , p2);
            break;
        case 50 :
            EvalConcat(p1 , p2);
            break;
        case 51 :
            EvalCopy(p1 , p2);
            break;
        case 54 :
            if (cnt[p2] == 1)
                {
                index = StartCnt[p2];
                NodeIndex = ExpVal[index];
                if ((field[NodeIndex] & 65535) == 4)
                    {
                    NodeIndex = field[NodeIndex + 5];
                    }
                if ((field[NodeIndex] & 65535) == 94)
                    {
                    typ = PrimType(STab[field[NodeIndex + 5]].SType);
                    }
                else
                    {
                    typ = ExpType[index];
                    }
                g1[p1] = node3(69 , IntegerPtr , typ , 0);
                SubType[p1] = IntegerPtr;
                TypeType[p1] = IntegerPtr;
                }
            else
                {
                ErrorMsg(8 , ColNo[p1]);
                StackError(p1);
                }
            break;
        case 55 :
            g1[p1] = EvalClose(p2);
            break;
        default :
            ErrorMsg(27 , -1);
            break;
        }
    }
