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
#include <stdio.h>

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
typedef
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
    } SYMTAB;
SymbolTableRange SymPtr;
SYMTAB STab[32768];
typedef
  int HdgRange;
HdgRange HdgHead;
HdgRange ptrg;
int field[1];
HdgRange HdgFillList;
int ErrorCount;
int control[256];
int control2[256];
unsigned char FillByte;
int FillWord;
int FillForVar;
int WOptSize;
int CommonInitial();
typedef
  unsigned char char_;
typedef
  char_ *STRINGPTRTYPE;
int StrTable[32768];
int StabI();
int EnterString();
int GetString();
STRINGPTRTYPE _GetStringPtr();
int _GetStringLength();
int _GetStringNo();
unsigned char _GetStringChar();
unsigned char _GetFirstChar();
int UtilitiesI();
int PasCerror();
int BaseType();
int CopyTree();
int demorgan();
int GetTemporary();
int FreeTemporary();
int ClearTemporarys();
int BasisTyp();
int DumpSymbolTable();
int DumptreeI();
int ListHdg();
typedef
  int ParseStackRange;
typedef
  int ParseStack[1001];
typedef
  int ParmTableRange;
typedef
  int ParmTable[1001];
typedef
  int ExpListRange;
typedef
  int IntExpList[1001];
typedef
  STRING ProcNameType;
typedef
  int ConstListRange;
typedef
  int TagListRange;
typedef
  int IntConstList[501];
typedef
  int IntTagList[51];
typedef
  int LabelStackRange;
typedef
  struct ptoc_Type2
    {
    int LIndex;
    int DefLevel;
    int RefLevel;
    } LabelRecord;
typedef
  LabelRecord LabelStackType[101];
SymbolTableRange IntegerPtr;
SymbolTableRange BooleanPtr;
SymbolTableRange CharPtr;
SymbolTableRange TextPtr;
SymbolTableRange FilePtr;
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
ProcNameType ProcName;
int ProcMark;
int ProcIndex;
int parct;
int ReturnedType;
int LevelNo;
int mp;
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
IntExpList ExpType;
IntExpList ExpStype;
IntExpList ExpColNo;
IntExpList ExpVal;
int IdPtr;
int IdList[1001];
int HeadFieldLink;
int TailFieldLink;
int CardCount;
int CurrentFile;
HdgRange CaseRoot;
IntConstList ConstVal;
IntConstList TagType;
IntConstList StartConst;
int ConstPtr;
int TagPtr;
LabelStackType LabelStack;
int LabelPtr;
int LabelScopePtr;
int ForWithLevel;
int asm_esc;
int PasclInitial();
int OpenInput();
int initialize();
int getline();
int yyparse();
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
int TypeSize();
int P1m2I();
int ErrorMsg();
int DclError();
int werror();
int FatalErrorMsg();
int WarnStrCmp();
int sql__exit();
int OptimizerInitial();
int OptdriverI();
int optimize();
int OpenObjFile();
#include <stdio.h>
FILE * ObjFile;
int *datafile;
int datacnt;
int yflag;
int preprocessed;
int first_time;
int OldSymPtr;
STRING argstring;
int ProcessFillOpt();
int ProcessFillForVar();
int atoi();
int InitSemantics();
int ResetHdgArray();
int ToSymtabIdx();
int InitGenC();
int GenC();


struct _ptoc_FRAME_XformAll
  {
  char dummy;
  } ;


int XformAll(enclosing_block, end_func)
long int enclosing_block;
int end_func;
    {
    struct _ptoc_FRAME_XformAll _ptoc_LOCAL;
    SymbolTableRange SaveSymPtr;
    HdgRange NodeIndex;
    int i;
    int b;
    
    if (ErrorCount == 0 && end_func)
        {
        SaveSymPtr = SymPtr;
        if (control[79])
            {
            if (asm_esc)
                {
                werror("File contains asms - Optimizer not called\0                                                                                                                                                                                                                      ");
                }
            else
                {
                optimize();
                }
            }
        first_time = 0;
        if (control[84])
            {
            DumpSymbolTable(2);
            }
        if (control[72])
            {
            ListHdg(HdgHead);
            }
        GenC(HdgHead , OldSymPtr , SaveSymPtr , ToSymtabIdx(enclosing_block) , 0 , 0 , control[120]);
        OldSymPtr = STab[SymPtr].SNext;
        HdgHead = -1;
        ResetHdgArray();
        }
    }

struct _ptoc_FRAME_ParseProgram
  {
  char dummy;
  } ;


int ParseProgram()
    {
    struct _ptoc_FRAME_ParseProgram _ptoc_LOCAL;
    int i;
    char_ c;
    STRING ObjFileName;
    int argno;
    int argcount;
    SymbolTableRange SaveSymPtr;
    HdgRange NodeIndex;
    _iorechd *ptoc_Var0;
    
    c = (unsigned char)'\0';
    do
        {
        control[(int ) c] = 0;
        if (c == (unsigned char)'\177')
            {
            break;
            }
        c += (unsigned char)'\001';
        }
    while (!0);
    control[109] = 1;
    first_time = 1;
    OldSymPtr = 0;
    HdgHead = -1;
    CaseRoot = -1;
    argcount = _argc;
    argno = 1;
    while (argno < argcount)
        {
        sql__rgv(argno , argstring , 256);
        if ((int ) argstring[0] == 45)
            {
            switch ((int ) argstring[1])
                {
                case 111 :
                    control[111] = 1;
                    argno += 1;
                    sql__rgv(argno , ObjFileName , 256);
                    i = -1;
                    do
                        i += 1;
                    while (!((int ) ObjFileName[i] == 32));
                    ObjFileName[i] = (unsigned char)'\0';
                    OpenObjFile(ObjFileName);
                    if (ObjFile == 0)
                        {
                        FatalErrorMsg(132 , -1);
                        }
                    break;
                case 113 :
                    control[(int ) argstring[1]] = 1;
                    if ((int ) argstring[2] != 32)
                        {
                        control[(int ) argstring[2]] = 1;
                        }
                    break;
                case 88 :
                    i = 2;
                    while ((int ) argstring[i] != 32)
                        {
                        control[(int ) argstring[i]] = 1;
                        i += 1;
                        }
                    break;
                case 75 :
                    i = 2;
                    while ((int ) argstring[i] != 32)
                        {
                        control2[(int ) argstring[i]] = 1;
                        i += 1;
                        }
                    break;
                case 121 :
                    if ((int ) argstring[2] == 112 && (int ) argstring[3] == 99)
                        {
                        control[121] = 1;
                        }
                    break;
                case 102 :
                    ProcessFillOpt(argstring);
                    control[102] = 1;
                    break;
                case 103 :
                    ProcessFillForVar(argstring);
                    control[103] = 1;
                    break;
                case 87 :
                    WOptSize = atoi(&argstring[2]);
                    control[87] = 1;
                    break;
                default :
                    control[(int ) argstring[1]] = 1;
                    break;
                }
            }
        else
            {
            i = -1;
            do
                i += 1;
            while (!((int ) argstring[i] == 32));
            argstring[i] = (unsigned char)'\0';
            OpenInput(&argstring[0]);
            }
        argno += 1;
        }
    if (control[115] && control[121])
        {
        control[115] = 0;
        }
    if (control[81])
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s" , "Options :  \"");
        c = (unsigned char)'\0';
        do
            {
            if (control[(int ) c])
                {
                ptoc_Var0 = sql__ut(output);
                fprintf(ptoc_Var0->_fbuf , "%c" , (int ) c);
                }
            if (c == (unsigned char)'\177')
                {
                break;
                }
            c += (unsigned char)'\001';
            }
        while (!0);
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%c" , 34);
        sql__writeln(ptoc_Var0);
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s" , "Options2 : \"");
        c = (unsigned char)'\0';
        do
            {
            if (control2[(int ) c])
                {
                ptoc_Var0 = sql__ut(output);
                fprintf(ptoc_Var0->_fbuf , "%c" , (int ) c);
                }
            if (c == (unsigned char)'\177')
                {
                break;
                }
            c += (unsigned char)'\001';
            }
        while (!0);
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%c" , 34);
        sql__writeln(ptoc_Var0);
        }
    initialize();
    InitSystem();
    asm_esc = 0;
    InitSemantics();
    InitGenC();
    i = yyparse();
    if (ErrorCount == 0)
        {
        XformAll(0 , 1);
        }
    else
        {
        if (control[84])
            {
            DumpSymbolTable(1);
            }
        if (control[72])
            {
            ListHdg(NodeIndex);
            }
        sql__exit(1);
        }
    }

int main(_argc_, _argv_, _envp_)
int _argc_;
int *_argv_;
int *_envp_;
    {
        {
        _argc = _argc_;
        _argv = _argv_;
        _envp = _envp_;
        sql__pmain(0);
        }
        {
        CommonInitial();
        PasclInitial();
        OptimizerInitial();
        ParseProgram();
        sql__exit(0);
        }
    }

