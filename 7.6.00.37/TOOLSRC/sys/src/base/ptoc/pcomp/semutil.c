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
HdgRange HdgFillList;
int control[256];
int FillWord;
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
int StabI();
int EnterString();
int GetString();
unsigned char *_GetStringPtr();
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
int shl();
int shr();
typedef
  int ParseStackRange;
typedef
  int ParseStack[1001];
SymbolTableRange IntegerPtr;
SymbolTableRange CharPtr;
SymbolTableRange TextPtr;
SymbolTableRange PacPtr;
SymbolTableRange LongPtr;
SymbolTableRange FloatPtr;
SymbolTableRange DoublePtr;
SymbolTableRange StringPtr;
ParseStack g1;
ParseStack TypeType;
ParseStack SubType;
SymbolTableRange AnyPacPtr;
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
int P1m2I();
int ErrorMsg();
int DclError();
int werror();
int FatalErrorMsg();
int WarnStrCmp();
int StartupInit();
int OutputInit();
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

struct _ptoc_FRAME_StackError
  {
  char dummy;
  } ;


int StackError(p)
ParseStackRange p;
    {
    struct _ptoc_FRAME_StackError _ptoc_LOCAL;
    
    g1[p] = -1;
    SubType[p] = -21;
    TypeType[p] = -21;
    }

struct _ptoc_FRAME_IsOrdinal
  {
  char dummy;
  } ;


int IsOrdinal(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_IsOrdinal _ptoc_LOCAL;
    int _IsOrdinal;
    
    switch (STab[t].SType)
        {
        case -24 :
        case -8 :
        case -9 :
        case -1 :
        case -2 :
        case -5 :
        case -7 :
        case -10 :
            _IsOrdinal = 1;
            break;
        default :
            _IsOrdinal = 0;
            break;
        }
    return(_IsOrdinal);
    }

struct _ptoc_FRAME_IsIntegral
  {
  char dummy;
  } ;


int IsIntegral(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_IsIntegral _ptoc_LOCAL;
    int _IsIntegral;
    
    switch (STab[t].SType)
        {
        case -8 :
        case -9 :
        case -1 :
        case -2 :
        case -4 :
        case -3 :
        case -5 :
            _IsIntegral = 1;
            break;
        default :
            _IsIntegral = 0;
            break;
        }
    return(_IsIntegral);
    }

struct _ptoc_FRAME_IsEnum
  {
  char dummy;
  } ;


int IsEnum(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_IsEnum _ptoc_LOCAL;
    int _IsEnum;
    
    _IsEnum = STab[t].SType == -10;
    return(_IsEnum);
    }

struct _ptoc_FRAME_IsFile
  {
  char dummy;
  } ;


int IsFile(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_IsFile _ptoc_LOCAL;
    int _IsFile;
    
    _IsFile = STab[t].SType == -15;
    return(_IsFile);
    }

struct _ptoc_FRAME_IsTextFile
  {
  char dummy;
  } ;


int IsTextFile(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_IsTextFile _ptoc_LOCAL;
    int _IsTextFile;
    
    _IsTextFile = t == TextPtr;
    return(_IsTextFile);
    }

struct _ptoc_FRAME_IsCharFile
  {
  char dummy;
  } ;


int IsCharFile(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_IsCharFile _ptoc_LOCAL;
    int _IsCharFile;
    
    _IsCharFile = STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent == CharPtr;
    return(_IsCharFile);
    }

struct _ptoc_FRAME_SubRange
  {
  char dummy;
  } ;


int SubRange(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_SubRange _ptoc_LOCAL;
    int _SubRange;
    SymbolTableRange lastt;
    
    lastt = t;
    if (STab[t].SType > 0)
        {
        while (STab[t].SType > 0)
            {
            lastt = t;
            t = STab[t].SType;
            }
        if (STab[lastt].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange == 0 && STab[lastt].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange == 0)
            {
            _SubRange = 0;
            }
        else
            {
            _SubRange = 1;
            }
        }
    else
        {
        _SubRange = 0;
        }
    return(_SubRange);
    }

struct _ptoc_FRAME_IsPac
  {
  char dummy;
  } ;


int IsPac(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_IsPac _ptoc_LOCAL;
    int _IsPac;
    
    _IsPac = STab[t].SType == -13 && PrimType(STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement) == CharPtr && !SubRange(STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement) && (STab[t].SFlags & 1024) != 0;
    return(_IsPac);
    }

struct _ptoc_FRAME_IsCharArray
  {
  char dummy;
  } ;


int IsCharArray(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_IsCharArray _ptoc_LOCAL;
    int _IsCharArray;
    
    _IsCharArray = STab[t].SType == -13 && STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement == CharPtr;
    return(_IsCharArray);
    }

struct _ptoc_FRAME_LengthOfCharArray
  {
  char dummy;
  } ;


int LengthOfCharArray(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_LengthOfCharArray _ptoc_LOCAL;
    int _LengthOfCharArray;
    
    t = STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
    _LengthOfCharArray = STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1;
    return(_LengthOfCharArray);
    }

struct _ptoc_FRAME_IsLval
  {
  char dummy;
  } ;


int IsLval(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_IsLval _ptoc_LOCAL;
    int _IsLval;
    
    _IsLval = (field[NodeIndex] & 65535) == 4;
    return(_IsLval);
    }

struct _ptoc_FRAME_IsSet
  {
  char dummy;
  } ;


int IsSet(typ)
SymbolTableRange typ;
    {
    struct _ptoc_FRAME_IsSet _ptoc_LOCAL;
    int _IsSet;
    
    _IsSet = STab[typ].SType == -16;
    return(_IsSet);
    }

struct _ptoc_FRAME_IsPointer
  {
  char dummy;
  } ;


int IsPointer(typ)
SymbolTableRange typ;
    {
    struct _ptoc_FRAME_IsPointer _ptoc_LOCAL;
    int _IsPointer;
    
    _IsPointer = STab[typ].SType == -17;
    return(_IsPointer);
    }

struct _ptoc_FRAME_IsStructured
  {
  char dummy;
  } ;


int IsStructured(typ)
SymbolTableRange typ;
    {
    struct _ptoc_FRAME_IsStructured _ptoc_LOCAL;
    int _IsStructured;
    
    switch (STab[typ].SType)
        {
        case -13 :
        case -14 :
        case -15 :
        case -16 :
        case -20 :
            _IsStructured = 1;
            break;
        case -8 :
        case -9 :
        case -1 :
        case -2 :
        case -5 :
        case -4 :
        case -3 :
        case -7 :
        case -24 :
        case -10 :
        case -11 :
        case -12 :
        case -17 :
            _IsStructured = 0;
            break;
        default :
            PasCerror("unexpected type - IsStructured                                                                                                                                                                                                                                  ");
            break;
        }
    return(_IsStructured);
    }

struct _ptoc_FRAME_IsCallRange
  {
  char dummy;
  } ;


int IsCallRange(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_IsCallRange _ptoc_LOCAL;
    int _IsCallRange;
    STRING s;
    
    _ptoc_ASSIGN(unsigned char , 256, s, "sql__setrang                                                                                                                                                                                                                                                    ")
    if ((field[NodeIndex] & 65535) == 93)
        {
        NodeIndex = field[NodeIndex + 5];
        }
    _IsCallRange = (field[NodeIndex] & 65535) == 62 && field[NodeIndex + 7] == EnterString(&s[0] , 12);
    return(_IsCallRange);
    }

struct _ptoc_FRAME_SkipCallRange
  {
  char dummy;
  } ;


int SkipCallRange(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_SkipCallRange _ptoc_LOCAL;
    int _SkipCallRange;
    
    if ((field[NodeIndex] & 65535) == 93)
        {
        NodeIndex = field[NodeIndex + 5];
        }
    NodeIndex = field[NodeIndex + 6];
    NodeIndex = field[NodeIndex + 5];
    NodeIndex = field[NodeIndex + 5];
    if ((field[NodeIndex] & 65535) == 93)
        {
        NodeIndex = field[NodeIndex + 5];
        }
    _SkipCallRange = NodeIndex;
    return(_SkipCallRange);
    }

struct _ptoc_FRAME_CharToPac
  {
  char dummy;
  } ;


int CharToPac(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CharToPac _ptoc_LOCAL;
    STRING s;
    
    s[0] = (unsigned char ) ConstExp(NodeIndex);
    field[NodeIndex] = shl(1 , 16) + 12;
    field[NodeIndex + 3] = -1;
    field[NodeIndex + 4] = -1;
    field[NodeIndex + 1] = PacPtr;
    field[NodeIndex + 5] = EnterString(&s[0] , 1);
    field[NodeIndex + 6] = 1;
    }

struct _ptoc_FRAME_IsString
  {
  char dummy;
  } ;


int IsString(typ)
SymbolTableRange typ;
    {
    struct _ptoc_FRAME_IsString _ptoc_LOCAL;
    int _IsString;
    
    _IsString = STab[typ].SType == -20;
    return(_IsString);
    }

struct _ptoc_FRAME_LengthOfString
  {
  char dummy;
  } ;


int LengthOfString(t)
SymbolTableRange t;
    {
    struct _ptoc_FRAME_LengthOfString _ptoc_LOCAL;
    int _LengthOfString;
    
    _LengthOfString = STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SLength + -1;
    return(_LengthOfString);
    }

struct _ptoc_FRAME_PacToString
  {
  char dummy;
  } ;


int PacToString(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_PacToString _ptoc_LOCAL;
    int i;
    int len;
    STRING s;
    int ptoc_Var0;
    int ptoc_Var1;
    
    GetString(field[NodeIndex + 5] , &s[0]);
    len = field[NodeIndex + 6];
    ptoc_Var0 = len;
    if (ptoc_Var0 >= 1)
        {
        i = ptoc_Var0;
        do
            {
            s[i] = s[i + -1];
            if (i == 1)
                {
                break;
                }
            i -= 1;
            }
        while (!0);
        }
    s[0] = (unsigned char ) len;
    field[NodeIndex + 1] = StringPtr;
    ptoc_Var1 = len + 1;
    field[NodeIndex + 5] = EnterString(&s[0] , ptoc_Var1);
    field[NodeIndex + 6] = ptoc_Var1;
    }

struct _ptoc_FRAME_CharToString
  {
  char dummy;
  } ;


int CharToString(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CharToString _ptoc_LOCAL;
    STRING s;
    
    s[0] = (unsigned char)'\001';
    s[1] = (unsigned char ) ConstExp(NodeIndex);
    field[NodeIndex] = shl(1 , 16) + 12;
    field[NodeIndex + 3] = -1;
    field[NodeIndex + 4] = -1;
    field[NodeIndex + 1] = StringPtr;
    field[NodeIndex + 5] = EnterString(&s[0] , 2);
    field[NodeIndex + 6] = 2;
    }

struct _ptoc_FRAME_EvalConvert
  {
  char dummy;
  } ;


int EvalConvert(NewType, NodeIndex)
SymbolTableRange NewType;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_EvalConvert _ptoc_LOCAL;
    int _EvalConvert;
    SymbolTableRange OldType;
    
    if (NodeIndex == -1)
        {
        _EvalConvert = NodeIndex;
        }
    else
        {
        OldType = field[NodeIndex + 1];
        if (OldType == NewType)
            {
            _EvalConvert = NodeIndex;
            }
        else
            {
            switch (STab[OldType].SType)
                {
                case -17 :
                    _EvalConvert = FoldUnary(93 , NewType , NodeIndex);
                    break;
                case -24 :
                    if (IsCharArray(NewType))
                        {
                        CharToPac(NodeIndex);
                        field[NodeIndex + 1] = NewType;
                        _EvalConvert = NodeIndex;
                        }
                    else
                        {
                        if (IsString(NewType))
                            {
                            CharToString(NodeIndex);
                            field[NodeIndex + 1] = NewType;
                            _EvalConvert = NodeIndex;
                            }
                        else
                            {
                            _EvalConvert = FoldUnary(93 , NewType , NodeIndex);
                            }
                        }
                    break;
                case -13 :
                    if (OldType == PacPtr)
                        {
                        switch (STab[NewType].SType)
                            {
                            case -13 :
                                if (STab[STab[NewType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange != 1 && !control[121])
                                    {
                                    ErrorMsg(143 , -1);
                                    }
                                field[NodeIndex + 1] = NewType;
                                _EvalConvert = NodeIndex;
                                break;
                            case -20 :
                                PacToString(NodeIndex);
                                _EvalConvert = NodeIndex;
                                break;
                            default :
                                _EvalConvert = NodeIndex;
                                break;
                            }
                        }
                    else
                        {
                        _EvalConvert = NodeIndex;
                        }
                    break;
                case -20 :
                    _EvalConvert = NodeIndex;
                    break;
                case -15 :
                case -14 :
                case -16 :
                    PasCerror("illegal type - EvalConvert                                                                                                                                                                                                                                      ");
                    break;
                default :
                    _EvalConvert = FoldUnary(93 , NewType , NodeIndex);
                    break;
                }
            }
        }
    return(_EvalConvert);
    }

struct _ptoc_FRAME_CheckCall
  {
  char dummy;
  } ;


int CheckCall(NodeIndex, col)
HdgRange NodeIndex;
int col;
    {
    struct _ptoc_FRAME_CheckCall _ptoc_LOCAL;
    
    if (field[NodeIndex + 6] == -1)
        {
        if (field[NodeIndex + 8] != 0)
            {
            ErrorMsg(8 , col);
            }
        }
    else
        {
        if (field[field[NodeIndex + 6] + 7] != field[NodeIndex + 8])
            {
            ErrorMsg(8 , col);
            }
        }
    }

struct _ptoc_FRAME_CompatibleTypes
  {
  char dummy;
  } ;


int CompatibleTypes(ptyp1, ptyp2)
SymbolTableRange ptyp1;
SymbolTableRange ptyp2;
    {
    struct _ptoc_FRAME_CompatibleTypes _ptoc_LOCAL;
    int _CompatibleTypes;
    
    if (ptyp1 == ptyp2)
        {
        _CompatibleTypes = 1;
        }
    else
        {
        switch (STab[ptyp1].SType)
            {
            case -8 :
            case -9 :
            case -1 :
            case -2 :
            case -4 :
            case -3 :
            case -5 :
                switch (STab[ptyp2].SType)
                    {
                    case -8 :
                    case -9 :
                    case -1 :
                    case -2 :
                    case -4 :
                    case -3 :
                    case -5 :
                        _CompatibleTypes = 1;
                        break;
                    default :
                        _CompatibleTypes = 0;
                        break;
                    }
                break;
            default :
                _CompatibleTypes = 0;
                break;
            }
        }
    return(_CompatibleTypes);
    }

struct _ptoc_FRAME_ContainsFile
  {
  char dummy;
  } ;


int ContainsFile(p)
SymbolTableRange p;
    {
    struct _ptoc_FRAME_ContainsFile _ptoc_LOCAL;
    int _ContainsFile;
    int retval;
    int rtype;
    int rkind;
    
    retval = 0;
    if (p > 0)
        {
        rtype = STab[p].SType;
        rkind = STab[p].SKind;
        if (rtype == -14)
            {
            if (rkind == 7)
                {
                retval = (STab[p].SFlags & 128) != 0;
                }
            else
                {
                if (rkind == 12)
                    {
                    retval = (STab[p].SFlags & 128) != 0 || ContainsFile(STab[p].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SNextVariant);
                    }
                }
            }
        else
            {
            if (IsFile(p) || ContainsFile(rtype))
                {
                retval = 1;
                }
            else
                {
                if (rkind == 11)
                    {
                    retval = ContainsFile(STab[p].KINDTYPE.C_RecordElementDcl.SNextField);
                    }
                }
            }
        }
    _ContainsFile = retval;
    return(_ContainsFile);
    }

struct _ptoc_FRAME_FileInRec
  {
  char dummy;
  } ;


int FileInRec(p)
SymbolTableRange p;
    {
    struct _ptoc_FRAME_FileInRec _ptoc_LOCAL;
    int _FileInRec;
    
    if (STab[p].SType == -14 && (STab[p].SFlags & 128) != 0)
        {
        _FileInRec = 1;
        }
    else
        {
        _FileInRec = 0;
        }
    return(_FileInRec);
    }

struct _ptoc_FRAME_IsNegative
  {
  char dummy;
  } ;


int IsNegative(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_IsNegative _ptoc_LOCAL;
    int _IsNegative;
    struct ptoc_Type2
      {
      union  
        {
        struct  
          {
          double db;
          } C_1;
        struct  
          {
          int lo;
          int hi;
          } C_2;
        struct  
          {
          long l;
          } C_3;
        } integer;
      } cast;
    
    _IsNegative = 0;
    switch (STab[field[NodeIndex + 1]].SType)
        {
        case -8 :
        case -9 :
        case -1 :
        case -2 :
        case -5 :
            if (field[NodeIndex + 5] < 0)
                {
                _IsNegative = 1;
                }
            break;
        case -3 :
        case -4 :
            cast.integer.C_2.lo = field[NodeIndex + 5];
            cast.integer.C_2.hi = field[NodeIndex + 6];
            if (cast.integer.C_3.l < (long ) 0)
                {
                _IsNegative = 1;
                }
            break;
        case -11 :
        case -12 :
            cast.integer.C_2.lo = field[NodeIndex + 5];
            cast.integer.C_2.hi = field[NodeIndex + 6];
            if (cast.integer.C_1.db < (double ) 0.00000000000000000000e+00)
                {
                _IsNegative = 1;
                }
            break;
        }
    return(_IsNegative);
    }

struct _ptoc_FRAME_FillVar
  {
  char dummy;
  } ;


int FillVar(six)
SymbolTableRange six;
    {
    struct _ptoc_FRAME_FillVar _ptoc_LOCAL;
    HdgRange t;
    
    if (IsStructured(PrimType(STab[six].SType)) || STab[six].SType == FloatPtr || STab[six].SType == LongPtr || STab[six].SType == DoublePtr)
        {
        t = node3(94 , STab[six].SType , six , 0);
        t = UnaryExpression(93 , AnyPacPtr , t , 0 , 0);
        HdgFillList = LinkChain(HdgFillList , t);
        STab[SymPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
        }
    else
        {
        StartupInit();
        t = node3(12 , IntegerPtr , FillWord , 0);
        t = UnaryExpression(93 , STab[six].SType , t , 0 , 0);
        OutputInit(six , t);
        STab[SymPtr].KINDTYPE.C_ModuleDcl.SSubClass = 4;
        }
    }
