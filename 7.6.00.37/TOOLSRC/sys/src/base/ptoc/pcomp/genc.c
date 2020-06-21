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
HdgRange HdgHead;
int field[1];
int control[256];
unsigned char FillByte;
int WOptSize;
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
int DumptreeI();
int ListHdg();
int version_writing();
int rediroutput();
int GenTyp();
int GenIfTyp();
int double_to_string();
int long_to_string();
int GenName();
int GenPName();
int BuildUnique();
int BuildTypeName();
int BuildL0Name();
int Ref_STab_datafile();
int DataNode();
int TypeSize();
int BumpSymPtr();
int UnCKeyword();
int SpecFunction();
int NoSideEffects();
int IsOrdinal();
int PrimType();
int exit();
int setbit();
int clearbit();
int testbit();
int PAR();
int UnmarkedType();
int UnmarkedVar();
int HdgMark();
int IsMarked();
int markT();
int TypeOfVar();
int DeleteUnusedVar();
int DeleteUnusedType();
int DeleteUnusedEnumElement();
int DeleteUnusedConst();
int BreakPointerType();
int DelayedParCopy();
int UseSymbolicConst();
int IsEnumDcl();
int IsConstDcl();
int DumpFlag;
int IDX_PTR;
int IDX_DATA;
int nummer;
int CurrentLevel;
int CurrentProc;
unsigned char outfile[1140];
int BlankString;

struct _ptoc_FRAME_GenC
  {
  int Node;
  int to_stab;
  int MaxPos;
  int StatLink;
  int Idx_name;
  int Idx_typespec;
  char dummy;
  } ;
typedef
  unsigned char String20_GenC[20];
static
  int TYPE_SPECIFIER_GenC();
static
  int DECLARATOR_GenC();
static
  int ABSTRACT_DECLARATOR_GenC();
static
  int EXPRESSION_GenC();
static
  int BLOCK_GenC();
static
  int STATEMENT_GenC();
static
  int FUNCTION_DEFINITION_GenC();
static
  int ComplexType_GenC();
static
  int CaseEmptyStatement_GenC();
static
  int EmptyStatement_GenC();
static
  int IsInFrameParam_GenC();
static
  int IsInFrame_GenC();
static
  int Typ_GenC();
static
  int DefineConstant_GenC();
static
  int Single_Definition_GenC();
static
  int ANSIDeklFormPar_GenC();


struct _ptoc_FRAME_Space_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Space_GenC(_ptoc_LINK)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
    {
    struct _ptoc_FRAME_Space_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var0;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var0 = sql__ut(outfile);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    }

struct _ptoc_FRAME_Abbruch_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Abbruch_GenC(_ptoc_LINK)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
    {
    struct _ptoc_FRAME_Abbruch_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var1;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var1 = sql__ut(outfile);
    fprintf(ptoc_Var1->_fbuf , "%s" , "--- Abbruch ---");
    sql__writeln(ptoc_Var1);
    Space_GenC(_ptoc_LOCAL._ptoc_LINK);
    exit(1);
    }

struct _ptoc_FRAME_indent_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int indent_GenC(_ptoc_LINK, kennz, index, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int kennz;
int index;
int inc;
    {
    struct _ptoc_FRAME_indent_GenC _ptoc_LOCAL;
    int i;
    _iorechd *ptoc_Var2;
    int ptoc_Var3;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (inc >= 0)
        {
        ptoc_Var2 = sql__ut(outfile);
        fprintf(ptoc_Var2->_fbuf , "");
        sql__writeln(ptoc_Var2);
        if (DumpFlag == 1)
            {
            switch (kennz)
                {
                case 1 :
                    ptoc_Var2 = sql__ut(outfile);
                    fprintf(ptoc_Var2->_fbuf , "%c%*d%s" , 91 , 6 , index , "]    ");
                    break;
                case 2 :
                    ptoc_Var2 = sql__ut(outfile);
                    fprintf(ptoc_Var2->_fbuf , "%c%*d%s" , 40 , 6 , index , ")    ");
                    break;
                }
            }
        ptoc_Var3 = inc;
        if (1 <= ptoc_Var3)
            {
            i = 1;
            do
                {
                ptoc_Var2 = sql__ut(outfile);
                fprintf(ptoc_Var2->_fbuf , "%c" , 32);
                if (i == ptoc_Var3)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        }
    }

struct _ptoc_FRAME_Mistake_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Mistake_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_Mistake_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var4;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var4 = sql__ut(outfile);
    fprintf(ptoc_Var4->_fbuf , "%s%10d%s" , "*** Mistake:" , Idx , " *** ");
    }

struct _ptoc_FRAME_Missing_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Missing_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_Missing_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var5;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var5 = sql__ut(outfile);
    fprintf(ptoc_Var5->_fbuf , "%s%10d%s" , "*** Missing:" , Idx , " *** ");
    }

struct _ptoc_FRAME_Fehler_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Fehler_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_Fehler_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var6;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var6 = sql__ut(outfile);
    fprintf(ptoc_Var6->_fbuf , "%s%10d%s" , " *** Fehler:" , Node , " *** ");
    }

struct _ptoc_FRAME_Error_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Error_GenC(_ptoc_LINK, inc, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int inc;
int Node;
    {
    struct _ptoc_FRAME_Error_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var7;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
    ptoc_Var7 = sql__ut(outfile);
    fprintf(ptoc_Var7->_fbuf , "%s%10d%s" , "======  Fehler:" , Node , "  ======");
    }

struct _ptoc_FRAME_DumpPutString_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DumpPutString_GenC(_ptoc_LINK, index, anf)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int index;
int anf;
    {
    struct _ptoc_FRAME_DumpPutString_GenC _ptoc_LOCAL;
    int len;
    int i;
    STRING s;
    int ptoc_Var8;
    int ptoc_Var9;
    _iorechd *ptoc_Var10;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    GetString(index , &s[0]);
    len = _GetStringLength(index);
    ptoc_Var8 = anf;
    ptoc_Var9 = len + -1;
    if (ptoc_Var8 <= ptoc_Var9)
        {
        i = ptoc_Var8;
        do
            {
            switch ((int ) s[i])
                {
                case 35 :
                case 46 :
                case 36 :
                    ptoc_Var10 = sql__ut(output);
                    fprintf(ptoc_Var10->_fbuf , "%c" , 95);
                    break;
                default :
                    ptoc_Var10 = sql__ut(output);
                    fprintf(ptoc_Var10->_fbuf , "%c" , (int ) s[i]);
                    break;
                }
            if (i == ptoc_Var9)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_PutString_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int PutString_GenC(_ptoc_LINK, index, anf)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int index;
int anf;
    {
    struct _ptoc_FRAME_PutString_GenC _ptoc_LOCAL;
    int len;
    int i;
    STRING s;
    _iorechd *ptoc_Var11;
    int ptoc_Var12;
    int ptoc_Var13;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var11 = sql__ut(output);
        fprintf(ptoc_Var11->_fbuf , "%*.*s" , 30 , 12 , "PutString : ");
        DumpPutString_GenC(_ptoc_LOCAL._ptoc_LINK, index , anf);
        ptoc_Var11 = sql__ut(output);
        fprintf(ptoc_Var11->_fbuf , "");
        sql__writeln(ptoc_Var11);
        }
    GetString(index , &s[0]);
    len = _GetStringLength(index);
    ptoc_Var12 = anf;
    ptoc_Var13 = len + -1;
    if (ptoc_Var12 <= ptoc_Var13)
        {
        i = ptoc_Var12;
        do
            {
            switch ((int ) s[i])
                {
                case 35 :
                case 46 :
                case 36 :
                    ptoc_Var11 = sql__ut(outfile);
                    fprintf(ptoc_Var11->_fbuf , "%c" , 95);
                    break;
                default :
                    ptoc_Var11 = sql__ut(outfile);
                    fprintf(ptoc_Var11->_fbuf , "%c" , (int ) s[i]);
                    break;
                }
            if (i == ptoc_Var13)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_DumpAsmString_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DumpAsmString_GenC(_ptoc_LINK, index)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int index;
    {
    struct _ptoc_FRAME_DumpAsmString_GenC _ptoc_LOCAL;
    int len;
    int i;
    STRING s;
    int ptoc_Var14;
    _iorechd *ptoc_Var15;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    GetString(index , &s[0]);
    len = _GetStringLength(index);
    ptoc_Var14 = len + -2;
    if (0 <= ptoc_Var14)
        {
        i = 0;
        do
            {
            ptoc_Var15 = sql__ut(outfile);
            fprintf(ptoc_Var15->_fbuf , "%c" , (int ) s[i]);
            if (i == ptoc_Var14)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_DumpDumpChar_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DumpDumpChar_GenC(_ptoc_LINK, c)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
unsigned char c;
    {
    struct _ptoc_FRAME_DumpDumpChar_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var16;
    int ptoc_Var17;
    int ptoc_Var18;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (sql__ord((int ) c))
        {
        case 0 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\0");
            break;
        case 10 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\n");
            break;
        case 9 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\t");
            break;
        case 8 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\b");
            break;
        case 13 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\r");
            break;
        case 12 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\f");
            break;
        case 92 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\\\");
            break;
        case 39 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\\'");
            break;
        case 34 :
            ptoc_Var16 = sql__ut(output);
            fprintf(ptoc_Var16->_fbuf , "%s" , "\\\"");
            break;
        default :
            if ((int ) c < 32 || (int ) c > 126)
                {
                ptoc_Var16 = sql__ut(output);
                ptoc_Var18 = (int ) c;
                fprintf(ptoc_Var16->_fbuf , "%c%*d%*d%*d" , 92 , 1 , sql__ord(ptoc_Var18) / 64 , 1 , (ptoc_Var17 = sql__ord(ptoc_Var18) / 8 % 8) >= 0 ? ptoc_Var17 : ptoc_Var17 + 8 , 1 , (ptoc_Var17 = sql__ord(ptoc_Var18) % 8) >= 0 ? ptoc_Var17 : ptoc_Var17 + 8);
                }
            else
                {
                ptoc_Var16 = sql__ut(output);
                fprintf(ptoc_Var16->_fbuf , "%c" , (int ) c);
                }
            break;
        }
    }

struct _ptoc_FRAME_DumpDumpString_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DumpDumpString_GenC(_ptoc_LINK, index)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int index;
    {
    struct _ptoc_FRAME_DumpDumpString_GenC _ptoc_LOCAL;
    int len;
    int i;
    STRING s;
    int ptoc_Var19;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    GetString(index , &s[0]);
    len = _GetStringLength(index);
    ptoc_Var19 = len + -1;
    if (0 <= ptoc_Var19)
        {
        i = 0;
        do
            {
            DumpDumpChar_GenC(_ptoc_LOCAL._ptoc_LINK, s[i]);
            if (i == ptoc_Var19)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_DumpChar_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DumpChar_GenC(_ptoc_LINK, c)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
unsigned char c;
    {
    struct _ptoc_FRAME_DumpChar_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var20;
    int ptoc_Var21;
    int ptoc_Var22;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (sql__ord((int ) c))
        {
        case 0 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\0");
            break;
        case 10 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\n");
            break;
        case 9 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\t");
            break;
        case 8 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\b");
            break;
        case 13 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\r");
            break;
        case 12 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\f");
            break;
        case 92 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\\\");
            break;
        case 39 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\\'");
            break;
        case 34 :
            ptoc_Var20 = sql__ut(outfile);
            fprintf(ptoc_Var20->_fbuf , "%s" , "\\\"");
            break;
        default :
            if ((int ) c < 32 || (int ) c > 126)
                {
                ptoc_Var20 = sql__ut(outfile);
                ptoc_Var22 = (int ) c;
                fprintf(ptoc_Var20->_fbuf , "%c%*d%*d%*d" , 92 , 1 , sql__ord(ptoc_Var22) / 64 , 1 , (ptoc_Var21 = sql__ord(ptoc_Var22) / 8 % 8) >= 0 ? ptoc_Var21 : ptoc_Var21 + 8 , 1 , (ptoc_Var21 = sql__ord(ptoc_Var22) % 8) >= 0 ? ptoc_Var21 : ptoc_Var21 + 8);
                }
            else
                {
                ptoc_Var20 = sql__ut(outfile);
                fprintf(ptoc_Var20->_fbuf , "%c" , (int ) c);
                }
            break;
        }
    }

struct _ptoc_FRAME_DumpString_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DumpString_GenC(_ptoc_LINK, index)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int index;
    {
    struct _ptoc_FRAME_DumpString_GenC _ptoc_LOCAL;
    int len;
    int i;
    STRING s;
    int ptoc_Var23;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    GetString(index , &s[0]);
    len = _GetStringLength(index);
    ptoc_Var23 = len + -1;
    if (0 <= ptoc_Var23)
        {
        i = 0;
        do
            {
            DumpChar_GenC(_ptoc_LOCAL._ptoc_LINK, s[i]);
            if (i == ptoc_Var23)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_PrintString_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int PrintString_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_PrintString_GenC _ptoc_LOCAL;
    struct ptoc_Type1 *ptoc_Var24;
    int ptoc_Var25;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var24 = &STab[Idx];
    if (ptoc_Var24->SName == 0)
        {
        ptoc_Var24->SName = -GenName(Idx);
        }
    else
        {
        if (ptoc_Var24->SName > 0)
            {
            ptoc_Var24->SName = UnCKeyword(ptoc_Var24->SName);
            if (ptoc_Var24->SKind == 16 || ptoc_Var24->SKind == 23)
                {
                ptoc_Var24->SName = -BuildTypeName(Idx);
                }
            if (ptoc_Var24->SLevel == 0 && ptoc_Var24->SKind == 8)
                {
                ptoc_Var24->SName = -BuildL0Name(Idx);
                }
            }
        }
    PutString_GenC(_ptoc_LOCAL._ptoc_LINK, (ptoc_Var25 = ptoc_Var24->SName , ptoc_Var25 >= 0 ? ptoc_Var25 : -ptoc_Var25) , 0);
    }

struct _ptoc_FRAME_DumpPrintString_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DumpPrintString_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_DumpPrintString_GenC _ptoc_LOCAL;
    struct ptoc_Type1 *ptoc_Var26;
    int ptoc_Var27;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var26 = &STab[Idx];
    if (ptoc_Var26->SName == 0)
        {
        ptoc_Var26->SName = -GenName(Idx);
        }
    else
        {
        if (ptoc_Var26->SName > 0)
            {
            ptoc_Var26->SName = UnCKeyword(ptoc_Var26->SName);
            if (ptoc_Var26->SKind == 16 || ptoc_Var26->SKind == 23)
                {
                ptoc_Var26->SName = -BuildTypeName(Idx);
                }
            if (ptoc_Var26->SLevel == 0 && ptoc_Var26->SKind == 8)
                {
                ptoc_Var26->SName = -BuildL0Name(Idx);
                }
            }
        }
    DumpPutString_GenC(_ptoc_LOCAL._ptoc_LINK, (ptoc_Var27 = ptoc_Var26->SName , ptoc_Var27 >= 0 ? ptoc_Var27 : -ptoc_Var27) , 0);
    }

struct _ptoc_FRAME_MakeUnique_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int MakeUnique_GenC(_ptoc_LINK, Idx, Suffix)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int Suffix;
    {
    struct _ptoc_FRAME_MakeUnique_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (STab[Idx].SName > 0)
        {
        if (Suffix > 0)
            {
            STab[Idx].SName = -BuildUnique(Idx , Suffix);
            }
        else
            {
            STab[Idx].SName = -UnCKeyword(STab[Idx].SName);
            }
        }
    }

struct _ptoc_FRAME_STAT_params_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int STAT_params_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_STAT_params_GenC _ptoc_LOCAL;
    int i;
    int MoreParams;
    _iorechd *ptoc_Var28;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (STab[Idx].SLevel > 1)
        {
        ptoc_Var28 = sql__ut(outfile);
        fprintf(ptoc_Var28->_fbuf , "%s" , "_ptoc_LINK");
        if (STab[STab[Idx].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt > 0)
            {
            ptoc_Var28 = sql__ut(outfile);
            fprintf(ptoc_Var28->_fbuf , "%s" , ", ");
            }
        }
    }

struct _ptoc_FRAME_print_TypeName_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int print_TypeName_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_print_TypeName_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var29;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    ptoc_Var29 = sql__ut(outfile);
    fprintf(ptoc_Var29->_fbuf , "%c" , 32);
    }

struct _ptoc_FRAME_IsTagName_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsTagName_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsTagName_GenC _ptoc_LOCAL;
    int _IsTagName;
    int idx1;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsTagName = STab[Idx].SKind == 23;
    return(_IsTagName);
    }

struct _ptoc_FRAME_IsBitfeld_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsBitfeld_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsBitfeld_GenC _ptoc_LOCAL;
    int _IsBitfeld;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsBitfeld = STab[Idx].SKind == 13;
    return(_IsBitfeld);
    }

struct _ptoc_FRAME_IsValueArray_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsValueArray_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsValueArray_GenC _ptoc_LOCAL;
    int _IsValueArray;
    struct ptoc_Type1 *ptoc_Var30;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var30 = &STab[Idx];
    _IsValueArray = ptoc_Var30->SKind == 14 && ptoc_Var30->KINDTYPE.C_ModuleDcl.SSubClass == 1 && STab[ptoc_Var30->SType].SType == -13;
    return(_IsValueArray);
    }

struct _ptoc_FRAME_IsArray_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsArray_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsArray_GenC _ptoc_LOCAL;
    int _IsArray;
    int t;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    t = STab[STab[Idx].SType].SType;
    _IsArray = t == -13 || t == -15 || t == -16 || t == -20;
    return(_IsArray);
    }

struct _ptoc_FRAME_PrintProcName_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int PrintProcName_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_PrintProcName_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    MakeUnique_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].KINDTYPE.C_ModuleDcl.SParent);
    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    }

struct _ptoc_FRAME_STAT_PrintString_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int STAT_PrintString_GenC(_ptoc_LINK, Idx, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_STAT_PrintString_GenC _ptoc_LOCAL;
    int VarLevel;
    int index;
    int par;
    _iorechd *ptoc_Var31;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    VarLevel = STab[Idx].SLevel;
    par = 0;
    if (VarLevel > 1)
        {
        index = CurrentLevel - VarLevel + 1;
        if (IsInFrame_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) || IsInFrameParam_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
            {
            par = PAR(Oop , Side , 96);
            if (par)
                {
                ptoc_Var31 = sql__ut(outfile);
                fprintf(ptoc_Var31->_fbuf , "%c" , 40);
                }
            ptoc_Var31 = sql__ut(outfile);
            fprintf(ptoc_Var31->_fbuf , "%s" , "_ptoc_LOCAL.");
            while (index > 0)
                {
                ptoc_Var31 = sql__ut(outfile);
                fprintf(ptoc_Var31->_fbuf , "%s" , "_ptoc_LINK->");
                index += -1;
                }
            }
        }
    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    if (par)
        {
        ptoc_Var31 = sql__ut(outfile);
        fprintf(ptoc_Var31->_fbuf , "%c" , 41);
        }
    }

struct _ptoc_FRAME_STAT_Identifier_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int STAT_Identifier_GenC(_ptoc_LINK, Idx, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_STAT_Identifier_GenC _ptoc_LOCAL;
    int par;
    _iorechd *ptoc_Var32;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (STab[Idx].SKind)
        {
        case 9 :
            STAT_PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , Oop , Side);
            break;
        case 14 :
            if (STab[Idx].KINDTYPE.C_ModuleDcl.SSubClass == 2)
                {
                if (IsArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                    {
                    STAT_PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , Oop , Side);
                    }
                else
                    {
                    par = PAR(Oop , Side , 97);
                    if (par)
                        {
                        ptoc_Var32 = sql__ut(outfile);
                        fprintf(ptoc_Var32->_fbuf , "%c" , 40);
                        }
                    ptoc_Var32 = sql__ut(outfile);
                    fprintf(ptoc_Var32->_fbuf , "%c" , 42);
                    STAT_PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 97 , 2);
                    if (par)
                        {
                        ptoc_Var32 = sql__ut(outfile);
                        fprintf(ptoc_Var32->_fbuf , "%c" , 41);
                        }
                    }
                }
            else
                {
                if (IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) && !DelayedParCopy())
                    {
                    par = PAR(Oop , Side , 96);
                    if (par)
                        {
                        ptoc_Var32 = sql__ut(outfile);
                        fprintf(ptoc_Var32->_fbuf , "%c" , 40);
                        }
                    STAT_PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 96 , 1);
                    ptoc_Var32 = sql__ut(outfile);
                    fprintf(ptoc_Var32->_fbuf , "%s" , ".data");
                    if (par)
                        {
                        ptoc_Var32 = sql__ut(outfile);
                        fprintf(ptoc_Var32->_fbuf , "%c" , 41);
                        }
                    }
                else
                    {
                    STAT_PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , Oop , Side);
                    }
                }
            break;
        case 3 :
        case 4 :
        case 5 :
        case 6 :
            PrintProcName_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            break;
        default :
            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            break;
        }
    }

struct _ptoc_FRAME_Identifier_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Identifier_GenC(_ptoc_LINK, Idx, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_Identifier_GenC _ptoc_LOCAL;
    int par;
    _iorechd *ptoc_Var33;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (STab[Idx].SKind)
        {
        case 9 :
            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            break;
        case 14 :
            if (STab[Idx].KINDTYPE.C_ModuleDcl.SSubClass == 2)
                {
                if (IsArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                    {
                    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                    }
                else
                    {
                    par = PAR(Oop , Side , 97);
                    if (par)
                        {
                        ptoc_Var33 = sql__ut(outfile);
                        fprintf(ptoc_Var33->_fbuf , "%c" , 40);
                        }
                    ptoc_Var33 = sql__ut(outfile);
                    fprintf(ptoc_Var33->_fbuf , "%c" , 42);
                    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                    if (par)
                        {
                        ptoc_Var33 = sql__ut(outfile);
                        fprintf(ptoc_Var33->_fbuf , "%c" , 41);
                        }
                    }
                }
            else
                {
                if (IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) && !DelayedParCopy())
                    {
                    par = PAR(Oop , Side , 96);
                    if (par)
                        {
                        ptoc_Var33 = sql__ut(outfile);
                        fprintf(ptoc_Var33->_fbuf , "%c" , 40);
                        }
                    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                    ptoc_Var33 = sql__ut(outfile);
                    fprintf(ptoc_Var33->_fbuf , "%s" , ".data");
                    if (par)
                        {
                        ptoc_Var33 = sql__ut(outfile);
                        fprintf(ptoc_Var33->_fbuf , "%c" , 41);
                        }
                    }
                else
                    {
                    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                    }
                }
            break;
        case 3 :
        case 4 :
        case 5 :
        case 6 :
            PrintProcName_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            break;
        default :
            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            break;
        }
    }

struct _ptoc_FRAME_FUNC_HEAD_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int FUNC_HEAD_GenC(_ptoc_LINK, Idx1)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx1;
    {
    struct _ptoc_FRAME_FUNC_HEAD_GenC _ptoc_LOCAL;
    int Idx2;
    int FunctionName;
    int ParAnz;
    int ParIdx;
    int i;
    _iorechd *ptoc_Var34;
    int ptoc_Var35;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx2 = STab[Idx1].SType;
    FunctionName = STab[Idx1].SName;
    ParAnz = STab[Idx2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
    Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , 40 , 1);
    ptoc_Var34 = sql__ut(outfile);
    fprintf(ptoc_Var34->_fbuf , "%c" , 40);
    if (_ptoc_LOCAL._ptoc_LINK->StatLink)
        {
        STAT_params_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1);
        }
    ParIdx = Idx2;
    ptoc_Var35 = ParAnz;
    if (1 <= ptoc_Var35)
        {
        i = 1;
        do
            {
            ParIdx = STab[ParIdx].SNext;
            if (IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, ParIdx) && DelayedParCopy())
                {
                ptoc_Var34 = sql__ut(outfile);
                fprintf(ptoc_Var34->_fbuf , "%s" , "ptoc_ptr_");
                }
            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, ParIdx);
            if (i != ParAnz)
                {
                ptoc_Var34 = sql__ut(outfile);
                fprintf(ptoc_Var34->_fbuf , "%s" , ", ");
                }
            if (i == ptoc_Var35)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    ptoc_Var34 = sql__ut(outfile);
    fprintf(ptoc_Var34->_fbuf , "%c" , 41);
    }

struct _ptoc_FRAME_ANSI_FUNC_HEAD_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ANSI_FUNC_HEAD_GenC(_ptoc_LINK, Idx1)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx1;
    {
    struct _ptoc_FRAME_ANSI_FUNC_HEAD_GenC _ptoc_LOCAL;
    int Idx2;
    int FunctionName;
    int ParAnz;
    int ParIdx;
    int i;
    _iorechd *ptoc_Var36;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , 40 , 1);
    ptoc_Var36 = sql__ut(outfile);
    fprintf(ptoc_Var36->_fbuf , "%c" , 40);
    ANSIDeklFormPar_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1);
    ptoc_Var36 = sql__ut(outfile);
    fprintf(ptoc_Var36->_fbuf , "%c" , 41);
    }

struct _ptoc_FRAME_STRUCT_DEC_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int STRUCT_DEC_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_STRUCT_DEC_GenC _ptoc_LOCAL;
    int tn;
    _iorechd *ptoc_Var37;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc);
    DECLARATOR_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , tn);
    if (IsBitfeld_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SType))
        {
        TypeSize(Idx);
        ptoc_Var37 = sql__ut(outfile);
        fprintf(ptoc_Var37->_fbuf , "%s" , " : ");
        ptoc_Var37 = sql__ut(outfile);
        fprintf(ptoc_Var37->_fbuf , "%*d" , 1 , STab[Idx].KINDTYPE.C_RecordElementDcl.ElementInfo.KINDTYPE.C_TypeDcl.SSize);
        }
    }

struct _ptoc_FRAME_AppendUnionElement_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int AppendUnionElement_GenC(_ptoc_LINK, x, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int *x;
int Idx;
    {
    struct _ptoc_FRAME_AppendUnionElement_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (*x == -1)
        {
        *x = Idx;
        }
    else
        {
        AppendUnionElement_GenC(_ptoc_LOCAL._ptoc_LINK, &STab[*x].KINDTYPE.C_RecordElementDcl.SNextField , Idx);
        }
    }

struct _ptoc_FRAME_GenStructElement_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int GenStructElement_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_GenStructElement_GenC _ptoc_LOCAL;
    int _GenStructElement;
    int el;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (Idx == -1)
        {
        _GenStructElement = -1;
        }
    else
        {
        BumpSymPtr();
        el = SymPtr;
        STab[el].SName = STab[Idx].STname;
        STab[el].SType = Idx;
        STab[el].SKind = 11;
        STab[el].KINDTYPE.C_RecordElementDcl.SNextField = GenStructElement_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SNextVariant);
        _GenStructElement = el;
        }
    return(_GenStructElement);
    }

struct _ptoc_FRAME_InitSRecVar_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int InitSRecVar_GenC(_ptoc_LINK, Idx, vorg)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int vorg;
    {
    struct _ptoc_FRAME_InitSRecVar_GenC _ptoc_LOCAL;
    int el;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (STab[Idx].SType < 0)
        {
        switch (STab[Idx].SType)
            {
            case -14 :
            case -19 :
                el = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField;
                while (el != -1)
                    {
                    InitSRecVar_GenC(_ptoc_LOCAL._ptoc_LINK, el , vorg);
                    el = STab[el].KINDTYPE.C_RecordElementDcl.SNextField;
                    }
                break;
            default :
                break;
            }
        }
    else
        {
        STab[Idx].SRecVar = vorg;
        InitSRecVar_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SType , Idx);
        }
    }

struct _ptoc_FRAME_CompleteVariantRecord_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int CompleteVariantRecord_GenC(_ptoc_LINK, Idx, erg)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int *erg;
    {
    struct _ptoc_FRAME_CompleteVariantRecord_GenC _ptoc_LOCAL;
    int first;
    int el1;
    int el2;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    first = GenStructElement_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant);
    BumpSymPtr();
    el1 = SymPtr;
    STab[el1].SName = BlankString;
    STab[el1].SType = -19;
    STab[el1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField = first;
    STab[el1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant = -1;
    BumpSymPtr();
    el2 = SymPtr;
    STab[el2].SName = STab[Idx].STname;
    STab[el2].SType = el1;
    STab[el2].SKind = 11;
    STab[el2].KINDTYPE.C_RecordElementDcl.SNextField = -1;
    AppendUnionElement_GenC(_ptoc_LOCAL._ptoc_LINK, &STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField , el2);
    *erg = el2;
    }

struct _ptoc_FRAME_DummyStructDec_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DummyStructDec_GenC(_ptoc_LINK, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int inc;
    {
    struct _ptoc_FRAME_DummyStructDec_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var38;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , 0 , inc);
    ptoc_Var38 = sql__ut(outfile);
    fprintf(ptoc_Var38->_fbuf , "%s" , "char dummy;");
    }

struct _ptoc_FRAME_STRUCT_SPEC_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int STRUCT_SPEC_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_STRUCT_SPEC_GenC _ptoc_LOCAL;
    int Idx1;
    int erg;
    _iorechd *ptoc_Var39;
    int ptoc_Var40;
    int ptoc_Var41;
    int ptoc_Var42;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    switch (STab[Idx].SType)
        {
        case -14 :
            ptoc_Var39 = sql__ut(outfile);
            fprintf(ptoc_Var39->_fbuf , "%s" , "struct");
            break;
        case -19 :
            ptoc_Var39 = sql__ut(outfile);
            fprintf(ptoc_Var39->_fbuf , "%s" , "union");
            break;
        }
    ptoc_Var39 = sql__ut(outfile);
    fprintf(ptoc_Var39->_fbuf , "%c" , 32);
    if (STab[Idx].STmyname != 0)
        {
        if (IsTagName_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].STmyname))
            {
            MakeUnique_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].STmyname , CurrentProc);
            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].STmyname);
            }
        else
            {
            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            }
        }
    else
        {
        PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
        }
    if (!testbit(STab[Idx].SFeld4 , 4))
        {
        setbit(&STab[Idx].SFeld4 , 4);
        if (STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant != -1)
            {
            CompleteVariantRecord_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , &erg);
            }
        ptoc_Var41 = inc + 2;
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , ptoc_Var41);
        ptoc_Var39 = sql__ut(outfile);
        fprintf(ptoc_Var39->_fbuf , "%c" , 123);
        ptoc_Var42 = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField;
        if (ptoc_Var42 == -1)
            {
            DummyStructDec_GenC(_ptoc_LOCAL._ptoc_LINK, inc + 2);
            }
        Idx1 = ptoc_Var42;
        if (Idx1 != -1)
            {
            ptoc_Var40 = inc + 2;
            do
                {
                STRUCT_DEC_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , ptoc_Var40);
                Idx1 = STab[Idx1].KINDTYPE.C_RecordElementDcl.SNextField;
                ptoc_Var39 = sql__ut(outfile);
                fprintf(ptoc_Var39->_fbuf , "%c" , 59);
                }
            while (!(Idx1 == -1));
            }
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , ptoc_Var41);
        ptoc_Var39 = sql__ut(outfile);
        fprintf(ptoc_Var39->_fbuf , "%s" , "} ");
        if (STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant != -1)
            {
            InitSRecVar_GenC(_ptoc_LOCAL._ptoc_LINK, STab[erg].SType , erg);
            }
        }
    else
        {
        ptoc_Var39 = sql__ut(outfile);
        fprintf(ptoc_Var39->_fbuf , "%c" , 32);
        }
    }

struct _ptoc_FRAME_NumberOfMarkedEnumElements_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int NumberOfMarkedEnumElements_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_NumberOfMarkedEnumElements_GenC _ptoc_LOCAL;
    int _NumberOfMarkedEnumElements;
    int Idx1;
    int anz;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    anz = 0;
    Idx1 = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SFirstEnum;
    while (Idx1 != -1)
        {
        if (IsMarked(Idx1))
            {
            anz += 1;
            }
        Idx1 = STab[Idx1].KINDTYPE.C_ConDcl.SNextEnum;
        }
    _NumberOfMarkedEnumElements = anz;
    return(_NumberOfMarkedEnumElements);
    }

struct _ptoc_FRAME_DefineEnum_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DefineEnum_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_DefineEnum_GenC _ptoc_LOCAL;
    int Idx1;
    int suppress;
    _iorechd *ptoc_Var43;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (DeleteUnusedEnumElement() && NumberOfMarkedEnumElements_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) == 0)
        {
        }
    else
        {
        if (UseSymbolicConst())
            {
            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
            if (!testbit(STab[Idx].SFeld4 , 4))
                {
                setbit(&STab[Idx].SFeld4 , 4);
                ptoc_Var43 = sql__ut(outfile);
                fprintf(ptoc_Var43->_fbuf , "%s" , "enum {");
                Idx1 = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SFirstEnum;
                while (Idx1 != -1)
                    {
                    suppress = 0;
                    if (DeleteUnusedEnumElement())
                        {
                        if (!IsMarked(Idx1))
                            {
                            suppress = 1;
                            }
                        }
                    if (!suppress)
                        {
                        MakeUnique_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , CurrentProc);
                        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc + 2);
                        PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1);
                        ptoc_Var43 = sql__ut(outfile);
                        fprintf(ptoc_Var43->_fbuf , "%c%*d%s" , 61 , 1 , STab[Idx1].KINDTYPE.C_ConDcl.SCon1val , ", ");
                        }
                    Idx1 = STab[Idx1].KINDTYPE.C_ConDcl.SNextEnum;
                    }
                indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
                ptoc_Var43 = sql__ut(outfile);
                fprintf(ptoc_Var43->_fbuf , "%s" , "};");
                }
            }
        }
    }

struct _ptoc_FRAME_ENUM_SPEC_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ENUM_SPEC_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_ENUM_SPEC_GenC _ptoc_LOCAL;
    int Idx1;
    _iorechd *ptoc_Var44;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    TypeSize(Idx);
    if (STab[Idx].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize == 8)
        {
        ptoc_Var44 = sql__ut(outfile);
        fprintf(ptoc_Var44->_fbuf , "%s" , "unsigned char ");
        }
    else
        {
        if (STab[Idx].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize == 16)
            {
            ptoc_Var44 = sql__ut(outfile);
            fprintf(ptoc_Var44->_fbuf , "%s" , "unsigned short ");
            }
        else
            {
            ptoc_Var44 = sql__ut(outfile);
            fprintf(ptoc_Var44->_fbuf , "%s" , "unsigned ");
            }
        }
    }

struct _ptoc_FRAME_Unlink_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Unlink_GenC(_ptoc_LINK, Idx, save)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int *save;
    {
    struct _ptoc_FRAME_Unlink_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (IsTagName_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
        {
        *save = testbit(STab[STab[Idx].SType].SFeld4 , 4);
        setbit(&STab[STab[Idx].SType].SFeld4 , 4);
        }
    }

struct _ptoc_FRAME_Link_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Link_GenC(_ptoc_LINK, Idx, save)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int save;
    {
    struct _ptoc_FRAME_Link_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (IsTagName_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
        {
        if (save)
            {
            setbit(&STab[STab[Idx].SType].SFeld4 , 4);
            }
        else
            {
            clearbit(&STab[STab[Idx].SType].SFeld4 , 4);
            }
        }
    }

struct _ptoc_FRAME_GenArray_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int GenArray_GenC(_ptoc_LINK, Idx, size)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int size;
    {
    struct _ptoc_FRAME_GenArray_GenC _ptoc_LOCAL;
    int Idx2;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    BumpSymPtr();
    Idx2 = SymPtr;
    STab[Idx].SType = -13;
    STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex = Idx2;
    STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = 15;
    STab[Idx2].SType = 1;
    STab[Idx2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 1;
    STab[Idx2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = size / 8;
    }

struct _ptoc_FRAME_GenCBlock_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int GenCBlock_GenC(_ptoc_LINK, ObjIdx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int ObjIdx;
    {
    struct _ptoc_FRAME_GenCBlock_GenC _ptoc_LOCAL;
    int _GenCBlock;
    int Idx1;
    int Idx2;
    int Idx3;
    int Idx4;
    int Idx5;
    int Idx6;
    int size;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    size = STab[ObjIdx].SFeld2 - STab[ObjIdx].SFeld1;
    BumpSymPtr();
    Idx1 = SymPtr;
    BumpSymPtr();
    Idx2 = SymPtr;
    BumpSymPtr();
    Idx3 = SymPtr;
    BumpSymPtr();
    Idx4 = SymPtr;
    BumpSymPtr();
    Idx5 = SymPtr;
    BumpSymPtr();
    Idx6 = SymPtr;
    STab[Idx1].SName = 0;
    STab[Idx1].SType = -14;
    STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField = Idx2;
    STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant = -1;
    STab[Idx2].SName = 0;
    STab[Idx2].SType = 1;
    STab[Idx2].SKind = 11;
    STab[Idx2].KINDTYPE.C_RecordElementDcl.SNextField = Idx3;
    STab[Idx3].SName = 0;
    STab[Idx3].SType = Idx4;
    STab[Idx3].SKind = 11;
    STab[Idx3].KINDTYPE.C_RecordElementDcl.SNextField = -1;
    STab[Idx4].SType = -13;
    STab[Idx4].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex = Idx5;
    STab[Idx4].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = Idx6;
    STab[Idx5].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = 1;
    STab[Idx5].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = size;
    STab[Idx6].SType = -17;
    STab[Idx6].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = 15;
    _GenCBlock = Idx1;
    return(_GenCBlock);
    }

struct _ptoc_FRAME_GetTypeName_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int GetTypeName_GenC(_ptoc_LINK, ObjIdx, TypIdx, DontUseBackRef)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int ObjIdx;
int TypIdx;
int DontUseBackRef;
    {
    struct _ptoc_FRAME_GetTypeName_GenC _ptoc_LOCAL;
    int _GetTypeName;
    int tnam;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    tnam = 0;
    if (ObjIdx != 0)
        {
        if (STab[ObjIdx].SType == TypIdx)
            {
            if (!DontUseBackRef && STab[ObjIdx].SKind == 16)
                {
                tnam = ObjIdx;
                }
            else
                {
                if (STab[ObjIdx].STname > 0)
                    {
                    tnam = STab[ObjIdx].STname;
                    }
                }
            }
        }
    if (tnam == 0 && !DontUseBackRef)
        {
        tnam = STab[TypIdx].STmyname;
        }
    if (tnam > 0)
        {
        if (STab[tnam].SLevel == 0 && !ComplexType_GenC(_ptoc_LOCAL._ptoc_LINK, TypIdx) || STab[tnam].SKind != 16 || !testbit(STab[tnam].SFeld4 , 4))
            {
            tnam = 0;
            }
        }
    _GetTypeName = tnam;
    return(_GetTypeName);
    }

struct _ptoc_FRAME_TYPE_SPECIFIER_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int TYPE_SPECIFIER_GenC(_ptoc_LINK, ObjIdx, TypIdx, DontUseBackRef, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int ObjIdx;
int TypIdx;
int DontUseBackRef;
int inc;
    {
    struct _ptoc_FRAME_TYPE_SPECIFIER_GenC _ptoc_LOCAL;
    int _TYPE_SPECIFIER;
    int Idx_h;
    int i;
    int ST_Idx;
    int tname;
    int tn;
    int save;
    _iorechd *ptoc_Var45;
    struct ptoc_Type1 *ptoc_Var46;
    struct ptoc_Type1 *ptoc_Var47;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _TYPE_SPECIFIER = TypIdx;
    tname = GetTypeName_GenC(_ptoc_LOCAL._ptoc_LINK, ObjIdx , TypIdx , DontUseBackRef);
    if (tname != 0)
        {
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , ObjIdx , inc);
        PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, tname);
        ptoc_Var45 = sql__ut(outfile);
        fprintf(ptoc_Var45->_fbuf , "%c" , 32);
        ptoc_Var46 = &STab[ObjIdx];
        if (ptoc_Var46->SKind == 19 || ptoc_Var46->SKind == 20 && tname == ptoc_Var46->STname)
            {
            _TYPE_SPECIFIER = STab[STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
            }
        ptoc_Var47 = &STab[TypIdx];
        if (ptoc_Var47->SType == -15 || ptoc_Var47->SType == -20 || ptoc_Var47->SType == -16)
            {
            TypeSize(TypIdx);
            GenArray_GenC(_ptoc_LOCAL._ptoc_LINK, TypIdx , STab[TypIdx].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize);
            }
        }
    else
        {
        if (STab[TypIdx].SType < 0)
            {
            switch (STab[TypIdx].SType)
                {
                case -11 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "float ");
                    break;
                case -12 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "double ");
                    break;
                case -21 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "void ");
                    break;
                case -5 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "int ");
                    break;
                case -6 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "unsigned ");
                    break;
                case -3 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "_ptoc_long ");
                    break;
                case -4 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "unsigned _ptoc_long ");
                    break;
                case -1 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "short ");
                    break;
                case -2 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "unsigned short ");
                    break;
                case -8 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "char ");
                    break;
                case -9 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "unsigned char ");
                    break;
                case -7 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    if (control[113])
                        {
                        ptoc_Var45 = sql__ut(outfile);
                        fprintf(ptoc_Var45->_fbuf , "%s" , "char ");
                        }
                    else
                        {
                        ptoc_Var45 = sql__ut(outfile);
                        fprintf(ptoc_Var45->_fbuf , "%s" , "int ");
                        }
                    break;
                case -24 :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    ptoc_Var45 = sql__ut(outfile);
                    fprintf(ptoc_Var45->_fbuf , "%s" , "unsigned char ");
                    break;
                case -17 :
                    if (!IsMarked(STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain))
                        {
                        if (BreakPointerType())
                            {
                            STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = 1;
                            }
                        }
                    Unlink_GenC(_ptoc_LOCAL._ptoc_LINK, STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain , &save);
                    if (testbit(STab[TypIdx].SFeld4 , 2))
                        {
                        STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = 1;
                        }
                    setbit(&STab[TypIdx].SFeld4 , 2);
                    _TYPE_SPECIFIER = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain , 0 , inc);
                    clearbit(&STab[TypIdx].SFeld4 , 2);
                    Link_GenC(_ptoc_LOCAL._ptoc_LINK, STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain , save);
                    break;
                case -14 :
                case -19 :
                    STRUCT_SPEC_GenC(_ptoc_LOCAL._ptoc_LINK, TypIdx , inc);
                    break;
                case -13 :
                    _TYPE_SPECIFIER = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , 0 , inc);
                    break;
                case -23 :
                    if (control[97])
                        {
                        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                        ptoc_Var45 = sql__ut(outfile);
                        fprintf(ptoc_Var45->_fbuf , "%s" , "void ");
                        _TYPE_SPECIFIER = STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
                        }
                    else
                        {
                        _TYPE_SPECIFIER = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult , 0 , inc);
                        }
                    break;
                case -18 :
                    _TYPE_SPECIFIER = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , STab[TypIdx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult , 0 , inc);
                    break;
                case -10 :
                    ENUM_SPEC_GenC(_ptoc_LOCAL._ptoc_LINK, TypIdx , inc);
                    break;
                case -16 :
                case -15 :
                case -20 :
                    TypeSize(TypIdx);
                    GenArray_GenC(_ptoc_LOCAL._ptoc_LINK, TypIdx , STab[TypIdx].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize);
                    _TYPE_SPECIFIER = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, ObjIdx , TypIdx , 0 , inc);
                    break;
                case -22 :
                    tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , GenCBlock_GenC(_ptoc_LOCAL._ptoc_LINK, ObjIdx) , 0 , inc);
                    break;
                default :
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , TypIdx , inc);
                    Missing_GenC(_ptoc_LOCAL._ptoc_LINK, TypIdx);
                    break;
                }
            }
        else
            {
            if (TypIdx == 0)
                {
                Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, TypIdx);
                }
            else
                {
                _TYPE_SPECIFIER = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, TypIdx , STab[TypIdx].SType , 0 , inc);
                }
            }
        }
    return(_TYPE_SPECIFIER);
    }

struct _ptoc_FRAME_AbstrDeklarator_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int AbstrDeklarator_GenC(_ptoc_LINK, Idx, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_AbstrDeklarator_GenC _ptoc_LOCAL;
    int s_index;
    int par;
    _iorechd *ptoc_Var48;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (Idx == -1)
        {
        }
    else
        {
        switch (STab[Idx].SType)
            {
            case -17 :
                par = PAR(Oop , Side , 97);
                if (par)
                    {
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%c" , 40);
                    }
                ptoc_Var48 = sql__ut(outfile);
                fprintf(ptoc_Var48->_fbuf , "%c" , 42);
                AbstrDeklarator_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain , 97 , 2);
                if (par)
                    {
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%c" , 41);
                    }
                break;
            case -13 :
                par = PAR(Oop , Side , 95);
                if (par)
                    {
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%c" , 40);
                    }
                AbstrDeklarator_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , 95 , 1);
                ptoc_Var48 = sql__ut(outfile);
                fprintf(ptoc_Var48->_fbuf , "%c" , 91);
                s_index = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
                ptoc_Var48 = sql__ut(outfile);
                fprintf(ptoc_Var48->_fbuf , "%*d" , 1 , STab[s_index].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[s_index].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1);
                ptoc_Var48 = sql__ut(outfile);
                fprintf(ptoc_Var48->_fbuf , "%c" , 93);
                if (par)
                    {
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%c" , 41);
                    }
                break;
            case -23 :
            case -18 :
                par = PAR(Oop , Side , 40);
                if (par)
                    {
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%c" , 40);
                    }
                AbstrDeklarator_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult , 40 , 1);
                if (control[97])
                    {
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%c" , 40);
                    ANSIDeklFormPar_GenC(_ptoc_LOCAL._ptoc_LINK, _ptoc_LOCAL._ptoc_LINK->Idx_name);
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%c" , 41);
                    }
                else
                    {
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%s" , "()");
                    }
                if (par)
                    {
                    ptoc_Var48 = sql__ut(outfile);
                    fprintf(ptoc_Var48->_fbuf , "%c" , 41);
                    }
                break;
            default :
                Mistake_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                break;
            }
        }
    }

struct _ptoc_FRAME_Aufbau_AbstrDekl_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Aufbau_AbstrDekl_GenC(_ptoc_LINK, Idx, vorg)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int vorg;
    {
    struct _ptoc_FRAME_Aufbau_AbstrDekl_GenC _ptoc_LOCAL;
    int orig_son;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (Idx == _ptoc_LOCAL._ptoc_LINK->Idx_typespec)
        {
        AbstrDeklarator_GenC(_ptoc_LOCAL._ptoc_LINK, vorg , 1 , 0);
        }
    else
        {
        switch (STab[Idx].SType)
            {
            case -17 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = vorg;
                Aufbau_AbstrDekl_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = orig_son;
                break;
            case -13 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = vorg;
                Aufbau_AbstrDekl_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = orig_son;
                break;
            case -23 :
            case -18 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = vorg;
                Aufbau_AbstrDekl_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = orig_son;
                break;
            default :
                if (STab[Idx].SType > 0)
                    {
                    Aufbau_AbstrDekl_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SType , vorg);
                    }
                else
                    {
                    AbstrDeklarator_GenC(_ptoc_LOCAL._ptoc_LINK, vorg , 1 , 0);
                    }
                break;
            }
        }
    }

struct _ptoc_FRAME_ABSTRACT_DECLARATOR_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ABSTRACT_DECLARATOR_GenC(_ptoc_LINK, Idx, namedtype)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int namedtype;
    {
    struct _ptoc_FRAME_ABSTRACT_DECLARATOR_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _ptoc_LOCAL._ptoc_LINK->Idx_typespec = namedtype;
    Aufbau_AbstrDekl_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , -1);
    }

struct _ptoc_FRAME_TYPE_NAME_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int TYPE_NAME_GenC(_ptoc_LINK, Idx, DontUseBackRef)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int DontUseBackRef;
    {
    struct _ptoc_FRAME_TYPE_NAME_GenC _ptoc_LOCAL;
    int tn;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , Idx , DontUseBackRef , -1);
    ABSTRACT_DECLARATOR_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , tn);
    }

struct _ptoc_FRAME_TypeCast_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int TypeCast_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_TypeCast_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var49;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var49 = sql__ut(outfile);
    fprintf(ptoc_Var49->_fbuf , "%c" , 40);
    TYPE_NAME_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 1);
    ptoc_Var49 = sql__ut(outfile);
    fprintf(ptoc_Var49->_fbuf , "%s" , ") ");
    }

struct _ptoc_FRAME_PointerTypeCast_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int PointerTypeCast_GenC(_ptoc_LINK, TypIdx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int TypIdx;
    {
    struct _ptoc_FRAME_PointerTypeCast_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    STab[IDX_PTR].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = TypIdx;
    TypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, IDX_PTR);
    }

struct _ptoc_FRAME_StructArray_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int StructArray_GenC(_ptoc_LINK, TypIdx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int TypIdx;
    {
    struct _ptoc_FRAME_StructArray_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var50;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    STab[IDX_DATA].SType = TypIdx;
    ptoc_Var50 = sql__ut(outfile);
    fprintf(ptoc_Var50->_fbuf , "%s" , "struct { ");
    STRUCT_DEC_GenC(_ptoc_LOCAL._ptoc_LINK, IDX_DATA , -1);
    ptoc_Var50 = sql__ut(outfile);
    fprintf(ptoc_Var50->_fbuf , "%s" , ";} ");
    }

struct _ptoc_FRAME_Func_Konstruktor_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Func_Konstruktor_GenC(_ptoc_LINK, Idx, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_Func_Konstruktor_GenC _ptoc_LOCAL;
    int s_index;
    int par;
    _iorechd *ptoc_Var51;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (Idx == _ptoc_LOCAL._ptoc_LINK->Idx_name)
        {
        if (control[97])
            {
            ANSI_FUNC_HEAD_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            }
        else
            {
            FUNC_HEAD_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            }
        }
    else
        {
        switch (STab[Idx].SType)
            {
            case -17 :
                par = PAR(Oop , Side , 97);
                if (par)
                    {
                    ptoc_Var51 = sql__ut(outfile);
                    fprintf(ptoc_Var51->_fbuf , "%c" , 40);
                    }
                ptoc_Var51 = sql__ut(outfile);
                fprintf(ptoc_Var51->_fbuf , "%c" , 42);
                Func_Konstruktor_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain , 97 , 2);
                if (par)
                    {
                    ptoc_Var51 = sql__ut(outfile);
                    fprintf(ptoc_Var51->_fbuf , "%c" , 41);
                    }
                break;
            case -13 :
                par = PAR(Oop , Side , 95);
                if (par)
                    {
                    ptoc_Var51 = sql__ut(outfile);
                    fprintf(ptoc_Var51->_fbuf , "%c" , 40);
                    }
                Func_Konstruktor_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , 95 , 1);
                ptoc_Var51 = sql__ut(outfile);
                fprintf(ptoc_Var51->_fbuf , "%c" , 91);
                s_index = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
                ptoc_Var51 = sql__ut(outfile);
                fprintf(ptoc_Var51->_fbuf , "%*d" , 1 , STab[s_index].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[s_index].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1);
                ptoc_Var51 = sql__ut(outfile);
                fprintf(ptoc_Var51->_fbuf , "%c" , 93);
                if (par)
                    {
                    ptoc_Var51 = sql__ut(outfile);
                    fprintf(ptoc_Var51->_fbuf , "%c" , 41);
                    }
                break;
            case -23 :
            case -18 :
                Func_Konstruktor_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult , Oop , Side);
                break;
            default :
                Mistake_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                break;
            }
        }
    }

struct _ptoc_FRAME_Aufbau_Func_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Aufbau_Func_GenC(_ptoc_LINK, Idx, vorg)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int vorg;
    {
    struct _ptoc_FRAME_Aufbau_Func_GenC _ptoc_LOCAL;
    int orig_son;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (Idx == _ptoc_LOCAL._ptoc_LINK->Idx_typespec)
        {
        Func_Konstruktor_GenC(_ptoc_LOCAL._ptoc_LINK, vorg , 1 , 0);
        }
    else
        {
        switch (STab[Idx].SType)
            {
            case -17 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = vorg;
                Aufbau_Func_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = orig_son;
                break;
            case -13 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = vorg;
                Aufbau_Func_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = orig_son;
                break;
            case -23 :
            case -18 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = vorg;
                Aufbau_Func_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = orig_son;
                break;
            default :
                if (STab[Idx].SType > 0)
                    {
                    Aufbau_Func_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SType , vorg);
                    }
                else
                    {
                    Func_Konstruktor_GenC(_ptoc_LOCAL._ptoc_LINK, vorg , 1 , 0);
                    }
                break;
            }
        }
    }

struct _ptoc_FRAME_FUNC_DEC_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int FUNC_DEC_GenC(_ptoc_LINK, Idx, namedtype)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int namedtype;
    {
    struct _ptoc_FRAME_FUNC_DEC_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _ptoc_LOCAL._ptoc_LINK->Idx_name = Idx;
    _ptoc_LOCAL._ptoc_LINK->Idx_typespec = namedtype;
    Aufbau_Func_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SType , Idx);
    }

struct _ptoc_FRAME_Deklarator_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Deklarator_GenC(_ptoc_LINK, Idx, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_Deklarator_GenC _ptoc_LOCAL;
    int s_index;
    int par;
    _iorechd *ptoc_Var52;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var52 = sql__ut(output);
        fprintf(ptoc_Var52->_fbuf , "%*.*s" , 30 , 13 , "Deklarator : ");
        }
    if (Idx == _ptoc_LOCAL._ptoc_LINK->Idx_name)
        {
        if (control[81])
            {
            ptoc_Var52 = sql__ut(output);
            fprintf(ptoc_Var52->_fbuf , "%s" , "Identifier");
            sql__writeln(ptoc_Var52);
            }
        Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , Oop , Side);
        }
    else
        {
        switch (STab[Idx].SType)
            {
            case -17 :
                if (control[81])
                    {
                    ptoc_Var52 = sql__ut(output);
                    fprintf(ptoc_Var52->_fbuf , "%s" , "PointerType");
                    sql__writeln(ptoc_Var52);
                    }
                par = PAR(Oop , Side , 97);
                if (par)
                    {
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%c" , 40);
                    }
                ptoc_Var52 = sql__ut(outfile);
                fprintf(ptoc_Var52->_fbuf , "%c" , 42);
                Deklarator_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain , 97 , 2);
                if (par)
                    {
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%c" , 41);
                    }
                break;
            case -13 :
                if (control[81])
                    {
                    ptoc_Var52 = sql__ut(output);
                    fprintf(ptoc_Var52->_fbuf , "%s" , "ArrayType");
                    sql__writeln(ptoc_Var52);
                    }
                par = PAR(Oop , Side , 95);
                if (par)
                    {
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%c" , 40);
                    }
                Deklarator_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , 95 , 1);
                ptoc_Var52 = sql__ut(outfile);
                fprintf(ptoc_Var52->_fbuf , "%c" , 91);
                s_index = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
                ptoc_Var52 = sql__ut(outfile);
                fprintf(ptoc_Var52->_fbuf , "%*d" , 1 , STab[s_index].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[s_index].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1);
                ptoc_Var52 = sql__ut(outfile);
                fprintf(ptoc_Var52->_fbuf , "%c" , 93);
                if (par)
                    {
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%c" , 41);
                    }
                break;
            case -23 :
            case -18 :
                if (control[81])
                    {
                    ptoc_Var52 = sql__ut(output);
                    fprintf(ptoc_Var52->_fbuf , "%s" , "PprocedureType/FunctionType");
                    sql__writeln(ptoc_Var52);
                    }
                par = PAR(Oop , Side , 40);
                if (par)
                    {
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%c" , 40);
                    }
                Deklarator_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult , 40 , 1);
                if (control[97])
                    {
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%c" , 40);
                    ANSIDeklFormPar_GenC(_ptoc_LOCAL._ptoc_LINK, _ptoc_LOCAL._ptoc_LINK->Idx_name);
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%c" , 41);
                    }
                else
                    {
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%s" , "()");
                    }
                if (par)
                    {
                    ptoc_Var52 = sql__ut(outfile);
                    fprintf(ptoc_Var52->_fbuf , "%c" , 41);
                    }
                break;
            default :
                Mistake_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                break;
            }
        }
    }

struct _ptoc_FRAME_Aufbau_Dekl_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Aufbau_Dekl_GenC(_ptoc_LINK, Idx, vorg)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int vorg;
    {
    struct _ptoc_FRAME_Aufbau_Dekl_GenC _ptoc_LOCAL;
    int orig_son;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (Idx == _ptoc_LOCAL._ptoc_LINK->Idx_typespec)
        {
        Deklarator_GenC(_ptoc_LOCAL._ptoc_LINK, vorg , 1 , 0);
        }
    else
        {
        switch (STab[Idx].SType)
            {
            case -17 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = vorg;
                Aufbau_Dekl_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = orig_son;
                break;
            case -13 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = vorg;
                Aufbau_Dekl_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement = orig_son;
                break;
            case -23 :
            case -18 :
                orig_son = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = vorg;
                Aufbau_Dekl_GenC(_ptoc_LOCAL._ptoc_LINK, orig_son , Idx);
                STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult = orig_son;
                break;
            default :
                if (STab[Idx].SType > 0)
                    {
                    Aufbau_Dekl_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SType , vorg);
                    }
                else
                    {
                    Deklarator_GenC(_ptoc_LOCAL._ptoc_LINK, vorg , 1 , 0);
                    }
                break;
            }
        }
    }

struct _ptoc_FRAME_DECLARATOR_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DECLARATOR_GenC(_ptoc_LINK, Idx, namedtype)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int namedtype;
    {
    struct _ptoc_FRAME_DECLARATOR_GenC _ptoc_LOCAL;
    int Idx1;
    int Idx2;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _ptoc_LOCAL._ptoc_LINK->Idx_name = Idx;
    _ptoc_LOCAL._ptoc_LINK->Idx_typespec = namedtype;
    Idx1 = STab[Idx].SType;
    Aufbau_Dekl_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , Idx);
    }

struct _ptoc_FRAME_DEC_LIST_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DEC_LIST_GenC(_ptoc_LINK, Idx, namedtype)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int namedtype;
    {
    struct _ptoc_FRAME_DEC_LIST_GenC _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    DECLARATOR_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , namedtype);
    }

struct _ptoc_FRAME_TYPE_DECL_LIST_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int TYPE_DECL_LIST_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_TYPE_DECL_LIST_GenC _ptoc_LOCAL;
    int tn;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc);
    DEC_LIST_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , tn);
    }

struct _ptoc_FRAME_IsSelfRef_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsSelfRef_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsSelfRef_GenC _ptoc_LOCAL;
    int _IsSelfRef;
    int Idx1;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsSelfRef = 0;
    Idx1 = STab[Idx].SType;
    if (STab[Idx1].SType == -17)
        {
        if (STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain == Idx)
            {
            _IsSelfRef = 1;
            }
        }
    return(_IsSelfRef);
    }

struct _ptoc_FRAME_Unnamed_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Unnamed_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_Unnamed_GenC _ptoc_LOCAL;
    int _Unnamed;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _Unnamed = STab[Idx].SName == 0;
    return(_Unnamed);
    }

struct _ptoc_FRAME_TYPE_DEFINITION_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int TYPE_DEFINITION_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_TYPE_DEFINITION_GenC _ptoc_LOCAL;
    int Idx1;
    int scard;
    int tn;
    _iorechd *ptoc_Var53;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var53 = sql__ut(output);
        fprintf(ptoc_Var53->_fbuf , "%*.*s" , 30 , 20 , "TYPE_DEFINITION   : ");
        DumpDumpString_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SName);
        ptoc_Var53 = sql__ut(output);
        fprintf(ptoc_Var53->_fbuf , "");
        sql__writeln(ptoc_Var53);
        }
    if (!Unnamed_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
        {
        if (IsSelfRef_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
            {
            STab[STab[Idx].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = 1;
            }
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
        ptoc_Var53 = sql__ut(outfile);
        fprintf(ptoc_Var53->_fbuf , "%s" , "typedef");
        tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 1 , inc + 2);
        DEC_LIST_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , tn);
        ptoc_Var53 = sql__ut(outfile);
        fprintf(ptoc_Var53->_fbuf , "%c" , 59);
        setbit(&STab[Idx].SFeld4 , 4);
        }
    }

struct _ptoc_FRAME_DECL_STAT_params_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DECL_STAT_params_GenC(_ptoc_LINK, Idx0, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx0;
int inc;
    {
    struct _ptoc_FRAME_DECL_STAT_params_GenC _ptoc_LOCAL;
    int Idx;
    _iorechd *ptoc_Var54;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx = Idx0;
    Idx = STab[Idx].KINDTYPE.C_ModuleDcl.SParent;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    ptoc_Var54 = sql__ut(outfile);
    fprintf(ptoc_Var54->_fbuf , "%s" , "struct _ptoc_FRAME_");
    Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 1 , 0);
    ptoc_Var54 = sql__ut(outfile);
    fprintf(ptoc_Var54->_fbuf , "%s" , " *_ptoc_LINK");
    }

struct _ptoc_FRAME_DeklFormPar_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DeklFormPar_GenC(_ptoc_LINK, Idx1, inc, inframe)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx1;
int inc;
int inframe;
    {
    struct _ptoc_FRAME_DeklFormPar_GenC _ptoc_LOCAL;
    int Idx2;
    int Idx;
    int paranz;
    int i;
    int tn;
    _iorechd *ptoc_Var55;
    static
      struct ptoc_Dummy0
        {
        int ptoc_Elem0;
        unsigned char *ptoc_Elem1[2];
        } ptoc_CBlock0 = 
      {
      2, 
      (unsigned char *) "false", 
      (unsigned char *) "true"
      };
    int ptoc_Var56;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var55 = sql__ut(output);
        fprintf(ptoc_Var55->_fbuf , "%*.*s%s%s%s" , 30 , 14 , "DeklFormPar : " , "inframe = " , sql__nname((int ) inframe , &ptoc_CBlock0) , ", ");
        sql__writeln(ptoc_Var55);
        }
    if (_ptoc_LOCAL._ptoc_LINK->StatLink && STab[Idx1].KINDTYPE.C_ModuleDcl.SParent != -1)
        {
        DECL_STAT_params_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , inc);
        ptoc_Var55 = sql__ut(outfile);
        fprintf(ptoc_Var55->_fbuf , "%c" , 59);
        }
    Idx2 = STab[Idx1].SType;
    paranz = STab[Idx2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
    Idx = Idx2;
    ptoc_Var56 = paranz;
    if (1 <= ptoc_Var56)
        {
        i = 1;
        do
            {
            Idx = STab[Idx].SNext;
            if (control[81])
                {
                ptoc_Var55 = sql__ut(output);
                fprintf(ptoc_Var55->_fbuf , "%*.*s%s" , 30 , 14 , "DeklFormPar : " , "Parameter ");
                DumpPrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                ptoc_Var55 = sql__ut(output);
                fprintf(ptoc_Var55->_fbuf , "");
                sql__writeln(ptoc_Var55);
                }
            switch (STab[Idx].SKind)
                {
                case 14 :
                case 19 :
                case 20 :
                    if (!inframe || IsInFrameParam_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                        {
                        if (control[87])
                            {
                            if ((STab[Idx].KINDTYPE.C_ModuleDcl.SSubClass != 2 || IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx)) && STab[STab[Idx].SType].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize > WOptSize * 8)
                                {
                                ptoc_Var55 = sql__ut(output);
                                fprintf(ptoc_Var55->_fbuf , "%s" , "Warning : Sizeof Val.P. \"");
                                DumpPrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                                ptoc_Var55 = sql__ut(output);
                                fprintf(ptoc_Var55->_fbuf , "%s" , "\" in \"");
                                DumpPrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1);
                                ptoc_Var55 = sql__ut(output);
                                fprintf(ptoc_Var55->_fbuf , "%s%*d" , "\" = " , 5 , STab[STab[Idx].SType].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize / 8);
                                sql__writeln(ptoc_Var55);
                                }
                            }
                        if (IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                            {
                            if (DelayedParCopy())
                                {
                                tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc);
                                ptoc_Var55 = sql__ut(outfile);
                                fprintf(ptoc_Var55->_fbuf , "%s" , "ptoc_ptr_");
                                PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                                }
                            else
                                {
                                indent_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , _ptoc_LOCAL._ptoc_LINK->Node , inc);
                                StructArray_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SType);
                                PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                                }
                            }
                        else
                            {
                            TYPE_DECL_LIST_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
                            }
                        ptoc_Var55 = sql__ut(outfile);
                        fprintf(ptoc_Var55->_fbuf , "%c" , 59);
                        }
                    break;
                default :
                    break;
                }
            if (i == ptoc_Var56)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_ANSIDeklFormPar_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ANSIDeklFormPar_GenC(_ptoc_LINK, Idx1)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx1;
    {
    struct _ptoc_FRAME_ANSIDeklFormPar_GenC _ptoc_LOCAL;
    int Idx2;
    int Idx;
    int paranz;
    int i;
    int tn;
    _iorechd *ptoc_Var57;
    int ptoc_Var58;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx2 = STab[Idx1].SType;
    paranz = STab[Idx2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
    if (_ptoc_LOCAL._ptoc_LINK->StatLink && STab[Idx1].KINDTYPE.C_ModuleDcl.SParent != -1)
        {
        DECL_STAT_params_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , 4);
        if (paranz > 0)
            {
            ptoc_Var57 = sql__ut(outfile);
            fprintf(ptoc_Var57->_fbuf , "%c" , 44);
            }
        }
    Idx = Idx2;
    ptoc_Var58 = paranz;
    if (1 <= ptoc_Var58)
        {
        i = 1;
        do
            {
            Idx = STab[Idx].SNext;
            switch (STab[Idx].SKind)
                {
                case 14 :
                case 19 :
                case 20 :
                    if (control[87])
                        {
                        if ((STab[Idx].KINDTYPE.C_ModuleDcl.SSubClass != 2 || IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx)) && STab[STab[Idx].SType].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize > WOptSize * 8)
                            {
                            ptoc_Var57 = sql__ut(output);
                            fprintf(ptoc_Var57->_fbuf , "%s" , "Warning : Sizeof Val.P. \"");
                            DumpPrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                            ptoc_Var57 = sql__ut(output);
                            fprintf(ptoc_Var57->_fbuf , "%s" , "\" in \"");
                            DumpPrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1);
                            ptoc_Var57 = sql__ut(output);
                            fprintf(ptoc_Var57->_fbuf , "%s%*d" , "\" = " , 5 , STab[STab[Idx].SType].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize / 8);
                            sql__writeln(ptoc_Var57);
                            }
                        }
                    if (IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                        {
                        if (DelayedParCopy())
                            {
                            tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , 4);
                            ptoc_Var57 = sql__ut(outfile);
                            fprintf(ptoc_Var57->_fbuf , "%s" , "ptoc_ptr_");
                            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                            }
                        else
                            {
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , _ptoc_LOCAL._ptoc_LINK->Node , 4);
                            StructArray_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SType);
                            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                            }
                        }
                    else
                        {
                        TYPE_DECL_LIST_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 4);
                        }
                    break;
                default :
                    break;
                }
            if (i != paranz)
                {
                ptoc_Var57 = sql__ut(outfile);
                fprintf(ptoc_Var57->_fbuf , "%c" , 44);
                }
            if (i == ptoc_Var58)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_InitTypIfTyp_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int InitTypIfTyp_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_InitTypIfTyp_GenC _ptoc_LOCAL;
    int next;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    next = field[NodeIndex + 3];
    field[NodeIndex + 3] = -1;
    GenTyp(Node);
    GenIfTyp(Node);
    field[NodeIndex + 3] = next;
    }

struct _ptoc_FRAME_Max_Positions_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Max_Positions_GenC(_ptoc_LINK, IdxVar, anz)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int IdxVar;
int anz;
    {
    struct _ptoc_FRAME_Max_Positions_GenC _ptoc_LOCAL;
    int _Max_Positions;
    int Idx;
    int elemtyp;
    int range;
    int ptoc_Var59;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _Max_Positions = _ptoc_LOCAL._ptoc_LINK->MaxPos;
    if (_ptoc_LOCAL._ptoc_LINK->MaxPos == 0)
        {
        _Max_Positions = 1;
        Idx = STab[IdxVar].SType;
        switch (STab[Idx].SType)
            {
            case -13 :
                Idx = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
                range = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1;
                if (((ptoc_Var59 = anz % range) >= 0 ? ptoc_Var59 : ptoc_Var59 + (range >= 0 ? range : -range)) != 0)
                    {
                    Mistake_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                    }
                elemtyp = STab[STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement].SType;
                if (elemtyp <= -13)
                    {
                    _Max_Positions = anz / range;
                    }
                else
                    {
                    _Max_Positions = 8;
                    }
                break;
            default :
                break;
            }
        }
    return(_Max_Positions);
    }

struct _ptoc_FRAME_INITIALIZER_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int INITIALIZER_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_INITIALIZER_GenC _ptoc_LOCAL;
    int entry_;
    int Node;
    int i;
    int pos;
    int maxpos;
    int von;
    int bis;
    _iorechd *ptoc_Var60;
    int ptoc_Var61;
    int ptoc_Var62;
    int ptoc_Var63;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var60 = sql__ut(output);
        fprintf(ptoc_Var60->_fbuf , "%*.*s%10d" , 30 , 14 , "INITIALIZER : " , STab[STab[Idx].SType].SType);
        sql__writeln(ptoc_Var60);
        }
    von = STab[Idx].SFeld1;
    bis = STab[Idx].SFeld2;
    if (STab[STab[Idx].SType].SType != -13 && von == bis)
        {
        entry_ = STab[Idx].SFeld1;
        Node = DataNode(entry_);
        InitTypIfTyp_GenC(_ptoc_LOCAL._ptoc_LINK, Node);
        EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, Node , 6 , 2);
        }
    else
        {
        maxpos = Max_Positions_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , bis - von + 1);
        pos = 0;
        ptoc_Var63 = inc + 2;
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , ptoc_Var63);
        ptoc_Var60 = sql__ut(outfile);
        fprintf(ptoc_Var60->_fbuf , "%c" , 123);
        ptoc_Var61 = von;
        ptoc_Var62 = bis;
        if (ptoc_Var61 <= ptoc_Var62)
            {
            i = ptoc_Var61;
            do
                {
                pos += 1;
                Node = DataNode(i);
                InitTypIfTyp_GenC(_ptoc_LOCAL._ptoc_LINK, Node);
                if (((ptoc_Var61 = (pos + -1) % maxpos) >= 0 ? ptoc_Var61 : ptoc_Var61 + (maxpos >= 0 ? maxpos : -maxpos)) == 0)
                    {
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc + 2);
                    }
                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, Node , 4 , 0);
                if (i != bis)
                    {
                    ptoc_Var60 = sql__ut(outfile);
                    fprintf(ptoc_Var60->_fbuf , "%s" , ", ");
                    }
                if (i == ptoc_Var62)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , ptoc_Var63);
        ptoc_Var60 = sql__ut(outfile);
        fprintf(ptoc_Var60->_fbuf , "%c" , 125);
        }
    }

struct _ptoc_FRAME_INIT_DEC_LIST_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int INIT_DEC_LIST_GenC(_ptoc_LINK, Idx, inc, namedtype)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
int namedtype;
    {
    struct _ptoc_FRAME_INIT_DEC_LIST_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var64;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    DECLARATOR_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , namedtype);
    if (STab[Idx].KINDTYPE.C_ModuleDcl.SSubClass == 4)
        {
        ptoc_Var64 = sql__ut(outfile);
        fprintf(ptoc_Var64->_fbuf , "%s" , " = ");
        INITIALIZER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
        }
    }

struct _ptoc_FRAME_DECLARATION_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DECLARATION_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_DECLARATION_GenC _ptoc_LOCAL;
    int tn;
    _iorechd *ptoc_Var65;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    ptoc_Var65 = sql__ut(outfile);
    fprintf(ptoc_Var65->_fbuf , "%s" , "extern");
    tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc + 2);
    DEC_LIST_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , tn);
    ptoc_Var65 = sql__ut(outfile);
    fprintf(ptoc_Var65->_fbuf , "%c" , 59);
    }

struct _ptoc_FRAME_E_DATA_DEFINITION_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int E_DATA_DEFINITION_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_E_DATA_DEFINITION_GenC _ptoc_LOCAL;
    int tn;
    _iorechd *ptoc_Var66;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (STab[Idx].KINDTYPE.C_ModuleDcl.SClass)
        {
        case 3 :
            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
            ptoc_Var66 = sql__ut(outfile);
            fprintf(ptoc_Var66->_fbuf , "%s" , "static");
            tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc + 2);
            break;
        default :
            tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc);
            break;
        }
    if (STab[Idx].SKind != 16 && STab[Idx].SKind != 23)
        {
        INIT_DEC_LIST_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , tn);
        }
    ptoc_Var66 = sql__ut(outfile);
    fprintf(ptoc_Var66->_fbuf , "%c" , 59);
    }

struct _ptoc_FRAME_I_DATA_DEFINITION_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int I_DATA_DEFINITION_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_I_DATA_DEFINITION_GenC _ptoc_LOCAL;
    int tn;
    _iorechd *ptoc_Var67;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (STab[Idx].KINDTYPE.C_ModuleDcl.SClass)
        {
        case 3 :
        case 4 :
            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
            ptoc_Var67 = sql__ut(outfile);
            fprintf(ptoc_Var67->_fbuf , "%s" , "static");
            tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc + 2);
            break;
        case 5 :
            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
            ptoc_Var67 = sql__ut(outfile);
            fprintf(ptoc_Var67->_fbuf , "%s" , "register");
            tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc + 2);
            break;
        default :
            tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].SType , 0 , inc);
            break;
        }
    if (STab[Idx].SKind != 16 && STab[Idx].SKind != 23)
        {
        INIT_DEC_LIST_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , tn);
        }
    ptoc_Var67 = sql__ut(outfile);
    fprintf(ptoc_Var67->_fbuf , "%c" , 59);
    }

struct _ptoc_FRAME_GetNode_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int GetNode_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_GetNode_GenC _ptoc_LOCAL;
    int _GetNode;
    HdgRange NodeIndex;
    int found;
    _iorechd *ptoc_Var68;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    found = 0;
    NodeIndex = HdgHead;
    while (NodeIndex != -1)
        {
        if (shr(field[NodeIndex] , 16) == 13)
            {
            if (field[NodeIndex + 7] == Idx)
                {
                found = 1;
                _GetNode = NodeIndex;
                }
            }
        NodeIndex = field[NodeIndex + 3];
        }
    if (found == 0)
        {
        Space_GenC(_ptoc_LOCAL._ptoc_LINK);
        ptoc_Var68 = sql__ut(outfile);
        fprintf(ptoc_Var68->_fbuf , "%s" , "Fehler: Hdg-Node fuer ProcDcl/FuncDcl nicht gefunden!");
        sql__writeln(ptoc_Var68);
        ptoc_Var68 = sql__ut(outfile);
        fprintf(ptoc_Var68->_fbuf , "%s%*d" , "Idx = " , 1 , Idx);
        sql__writeln(ptoc_Var68);
        Abbruch_GenC(_ptoc_LOCAL._ptoc_LINK);
        }
    return(_GetNode);
    }

struct _ptoc_FRAME_DefineConstant_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DefineConstant_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_DefineConstant_GenC _ptoc_LOCAL;
    int t;
    STRING dblstr;
    int anz_bytes;
    _iorechd *ptoc_Var69;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var69 = sql__ut(outfile);
    fprintf(ptoc_Var69->_fbuf , "");
    sql__writeln(ptoc_Var69);
    ptoc_Var69 = sql__ut(outfile);
    fprintf(ptoc_Var69->_fbuf , "%s" , "PTOC_CONST(");
    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    ptoc_Var69 = sql__ut(outfile);
    fprintf(ptoc_Var69->_fbuf , "%s" , ", ");
    InitTypIfTyp_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SFeld3);
    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SFeld3 , 1 , 0);
    ptoc_Var69 = sql__ut(outfile);
    fprintf(ptoc_Var69->_fbuf , "%s" , ");");
    }

struct _ptoc_FRAME_IsInFrameParam_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsInFrameParam_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsInFrameParam_GenC _ptoc_LOCAL;
    int _IsInFrameParam;
    struct ptoc_Type1 *ptoc_Var70;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (_ptoc_LOCAL._ptoc_LINK->StatLink)
        {
        ptoc_Var70 = &STab[Idx];
        _IsInFrameParam = (ptoc_Var70->SKind == 14 || ptoc_Var70->SKind == 20 || ptoc_Var70->SKind == 19) && testbit(ptoc_Var70->SFeld4 , 1);
        }
    else
        {
        _IsInFrameParam = 0;
        }
    return(_IsInFrameParam);
    }

struct _ptoc_FRAME_IsInFrame_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsInFrame_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsInFrame_GenC _ptoc_LOCAL;
    int _IsInFrame;
    struct ptoc_Type1 *ptoc_Var71;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (_ptoc_LOCAL._ptoc_LINK->StatLink)
        {
        ptoc_Var71 = &STab[Idx];
        _IsInFrame = ptoc_Var71->SKind == 9 && ptoc_Var71->KINDTYPE.C_ModuleDcl.SSubClass == 0 && ptoc_Var71->KINDTYPE.C_ModuleDcl.SClass == 10 && testbit(ptoc_Var71->SFeld4 , 1);
        }
    else
        {
        _IsInFrame = 0;
        }
    return(_IsInFrame);
    }

struct _ptoc_FRAME_IsGlobalLocal_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsGlobalLocal_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsGlobalLocal_GenC _ptoc_LOCAL;
    int _IsGlobalLocal;
    struct ptoc_Type1 *ptoc_Var72;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var72 = &STab[Idx];
    _IsGlobalLocal = ptoc_Var72->SKind == 7 || ptoc_Var72->SKind == 16 || ptoc_Var72->SKind == 5 || ptoc_Var72->SKind == 6 || ptoc_Var72->SKind == 9 && (ptoc_Var72->KINDTYPE.C_ModuleDcl.SClass == 0 || ptoc_Var72->KINDTYPE.C_ModuleDcl.SClass == 3 || ptoc_Var72->KINDTYPE.C_ModuleDcl.SClass == 4) || ptoc_Var72->SKind == 8;
    return(_IsGlobalLocal);
    }

struct _ptoc_FRAME_IsEnumvalueDcl_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsEnumvalueDcl_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsEnumvalueDcl_GenC _ptoc_LOCAL;
    int _IsEnumvalueDcl;
    struct ptoc_Type1 *ptoc_Var73;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var73 = &STab[Idx];
    _IsEnumvalueDcl = ptoc_Var73->SKind == 8 && ptoc_Var73->KINDTYPE.C_ConDcl.SNextEnum != 0;
    return(_IsEnumvalueDcl);
    }

struct _ptoc_FRAME_IsCharArray_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsCharArray_GenC(_ptoc_LINK, Idx0)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx0;
    {
    struct _ptoc_FRAME_IsCharArray_GenC _ptoc_LOCAL;
    int _IsCharArray;
    int Idx;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsCharArray = 0;
    Idx = STab[Idx0].SType;
    if (STab[Idx].SType == -13)
        {
        _IsCharArray = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement == 15;
        }
    return(_IsCharArray);
    }

struct _ptoc_FRAME_FillInteger_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int FillInteger_GenC(_ptoc_LINK, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int inc;
    {
    struct _ptoc_FRAME_FillInteger_GenC _ptoc_LOCAL;
    int Idx;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    BumpSymPtr();
    Idx = SymPtr;
    STab[Idx].SName = 0;
    STab[Idx].SType = 1;
    STab[Idx].SKind = 9;
    STab[Idx].KINDTYPE.C_ModuleDcl.SClass = 1;
    STab[Idx].KINDTYPE.C_ModuleDcl.SSubClass = 0;
    Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , 0 , 0);
    }

struct _ptoc_FRAME_IsForwardDcl_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsForwardDcl_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsForwardDcl_GenC _ptoc_LOCAL;
    int _IsForwardDcl;
    struct ptoc_Type1 *ptoc_Var74;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var74 = &STab[Idx];
    _IsForwardDcl = ptoc_Var74->SKind == 5 || ptoc_Var74->SKind == 6;
    return(_IsForwardDcl);
    }

struct _ptoc_FRAME_IsFuncDcl_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsFuncDcl_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsFuncDcl_GenC _ptoc_LOCAL;
    int _IsFuncDcl;
    struct ptoc_Type1 *ptoc_Var75;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var75 = &STab[Idx];
    _IsFuncDcl = ptoc_Var75->SKind == 4 || ptoc_Var75->SKind == 3 || ptoc_Var75->SKind == 2;
    return(_IsFuncDcl);
    }

struct _ptoc_FRAME_IsBlockDcl_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsBlockDcl_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsBlockDcl_GenC _ptoc_LOCAL;
    int _IsBlockDcl;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsBlockDcl = STab[Idx].SKind == 21;
    return(_IsBlockDcl);
    }

struct _ptoc_FRAME_LevelNext_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int LevelNext_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_LevelNext_GenC _ptoc_LOCAL;
    int _LevelNext;
    int next;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    next = STab[Idx].SNext;
    if (IsBlockDcl_GenC(_ptoc_LOCAL._ptoc_LINK, next))
        {
        _LevelNext = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, STab[next].KINDTYPE.C_BlockDcl.SLastEntry);
        }
    else
        {
        _LevelNext = next;
        }
    return(_LevelNext);
    }

struct _ptoc_FRAME_Single_Definition_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Single_Definition_GenC(_ptoc_LINK, Idx, inc, pulloutside, ismodul)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
int pulloutside;
int ismodul;
    {
    struct _ptoc_FRAME_Single_Definition_GenC _ptoc_LOCAL;
    int Idx1;
    _iorechd *ptoc_Var76;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var76 = sql__ut(output);
        fprintf(ptoc_Var76->_fbuf , "%*.*s" , 30 , 20 , "Single_Definition : ");
        if (control[89])
            {
            if (STab[Idx].SKind == 9)
                {
                if (control[81])
                    {
                    ptoc_Var76 = sql__ut(output);
                    fprintf(ptoc_Var76->_fbuf , "%s" , "Variable Defition ");
                    if (STab[Idx].KINDTYPE.C_ModuleDcl.SClass == 2)
                        {
                        ptoc_Var76 = sql__ut(output);
                        fprintf(ptoc_Var76->_fbuf , "%s" , "(global !) ");
                        }
                    }
                }
            }
        DumpDumpString_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SName);
        ptoc_Var76 = sql__ut(output);
        fprintf(ptoc_Var76->_fbuf , "");
        sql__writeln(ptoc_Var76);
        }
    switch (STab[Idx].SKind)
        {
        case 9 :
        case 6 :
        case 5 :
        case 17 :
            if (control[89])
                {
                if (STab[Idx].SKind == 9)
                    {
                    if (STab[Idx].KINDTYPE.C_ModuleDcl.SClass == 2)
                        {
                        ptoc_Var76 = sql__ut(outfile);
                        fprintf(ptoc_Var76->_fbuf , "");
                        sql__writeln(ptoc_Var76);
                        ptoc_Var76 = sql__ut(outfile);
                        fprintf(ptoc_Var76->_fbuf , "");
                        sql__writeln(ptoc_Var76);
                        ptoc_Var76 = sql__ut(outfile);
                        fprintf(ptoc_Var76->_fbuf , "%s" , "/*size printf (\"");
                        DumpString_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SName);
                        ptoc_Var76 = sql__ut(outfile);
                        fprintf(ptoc_Var76->_fbuf , "%s" , "   %d\\n\" , sizeof (");
                        STab[Idx].SName = GenPName(Idx);
                        DumpString_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SName);
                        ptoc_Var76 = sql__ut(outfile);
                        fprintf(ptoc_Var76->_fbuf , "%s" , ")); */");
                        sql__writeln(ptoc_Var76);
                        ptoc_Var76 = sql__ut(outfile);
                        fprintf(ptoc_Var76->_fbuf , "");
                        sql__writeln(ptoc_Var76);
                        ptoc_Var76 = sql__ut(outfile);
                        fprintf(ptoc_Var76->_fbuf , "%s" , "extern ");
                        if (control[81])
                            {
                            ptoc_Var76 = sql__ut(output);
                            fprintf(ptoc_Var76->_fbuf , "%*.*s%s" , 30 , 20 , "Single_Definition : " , "New Name = ");
                            DumpDumpString_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SName);
                            ptoc_Var76 = sql__ut(output);
                            fprintf(ptoc_Var76->_fbuf , "");
                            sql__writeln(ptoc_Var76);
                            }
                        }
                    }
                }
            if (STab[Idx].KINDTYPE.C_ModuleDcl.SClass == 0)
                {
                DECLARATION_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
                }
            else
                {
                if (pulloutside && STab[Idx].KINDTYPE.C_ModuleDcl.SClass != 3)
                    {
                    MakeUnique_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , CurrentProc);
                    }
                if (ismodul)
                    {
                    E_DATA_DEFINITION_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
                    }
                else
                    {
                    I_DATA_DEFINITION_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
                    }
                }
            break;
        case 16 :
            if (pulloutside)
                {
                MakeUnique_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , CurrentProc);
                }
            TYPE_DEFINITION_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
            break;
        case 7 :
            Idx1 = STab[Idx].STmyname;
            if (Idx1 != 0)
                {
                if (IsTagName_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1))
                    {
                    if (pulloutside)
                        {
                        MakeUnique_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , CurrentProc);
                        }
                    if (ismodul)
                        {
                        E_DATA_DEFINITION_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , inc);
                        }
                    else
                        {
                        I_DATA_DEFINITION_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , inc);
                        }
                    }
                }
            break;
        case 2 :
        case 4 :
        case 3 :
            FUNCTION_DEFINITION_GenC(_ptoc_LOCAL._ptoc_LINK, GetNode_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) , inc);
            break;
        default :
            break;
        }
    }

struct _ptoc_FRAME_Outside_Definitions_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Outside_Definitions_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_Outside_Definitions_GenC _ptoc_LOCAL;
    int last;
    int x;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    last = STab[Idx].KINDTYPE.C_BlockDcl.SLastEntry;
    x = Idx;
    x = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, x);
    while (x <= last)
        {
        if (IsEnumDcl(x))
            {
            DefineEnum_GenC(_ptoc_LOCAL._ptoc_LINK, x , inc);
            }
        x = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, x);
        }
    x = Idx;
    x = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, x);
    while (x <= last)
        {
        if (IsGlobalLocal_GenC(_ptoc_LOCAL._ptoc_LINK, x) && !IsEnumDcl(x) && !IsConstDcl(x))
            {
            Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, x , inc , 1 , 0);
            }
        else
            {
            if (UseSymbolicConst() && IsConstDcl(x) && IsOrdinal(PrimType(STab[x].SType)) && !IsEnumvalueDcl_GenC(_ptoc_LOCAL._ptoc_LINK, x))
                {
                MakeUnique_GenC(_ptoc_LOCAL._ptoc_LINK, x , CurrentProc);
                DefineConstant_GenC(_ptoc_LOCAL._ptoc_LINK, x);
                }
            }
        x = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, x);
        }
    }

struct _ptoc_FRAME_Frame_Definitions_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Frame_Definitions_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_Frame_Definitions_GenC _ptoc_LOCAL;
    int last;
    int level;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    level = STab[Idx].SLevel;
    last = STab[Idx].KINDTYPE.C_BlockDcl.SLastEntry;
    Idx = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    while (Idx <= last)
        {
        if (level == STab[Idx].SLevel && IsInFrame_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
            {
            if (IsCharArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                {
                FillInteger_GenC(_ptoc_LOCAL._ptoc_LINK, inc);
                }
            Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , 0 , 0);
            }
        Idx = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
        }
    }

struct _ptoc_FRAME_Local_Definitions_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Local_Definitions_GenC(_ptoc_LINK, Idx_Defs, inc, minlevel)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx_Defs;
int inc;
int minlevel;
    {
    struct _ptoc_FRAME_Local_Definitions_GenC _ptoc_LOCAL;
    int Idx;
    int last;
    int level;
    int first;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx = Idx_Defs;
    level = STab[Idx].SLevel;
    if (level >= minlevel)
        {
        last = STab[Idx].KINDTYPE.C_BlockDcl.SLastEntry;
        Idx = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
        while (Idx <= last)
            {
            if (!IsInFrame_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) && !IsGlobalLocal_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) && !IsFuncDcl_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                {
                Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , 0 , 0);
                }
            Idx = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
            }
        Idx = STab[Idx_Defs].KINDTYPE.C_BlockDcl.SFuncBlock;
        first = STab[Idx].SFeld1;
        last = STab[Idx].SFeld2;
        while (first <= last && first != 0 && last != 0)
            {
            Idx = first;
            Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , 0 , 0);
            first += 1;
            }
        }
    }

struct _ptoc_FRAME_Global_Definitions_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Global_Definitions_GenC(_ptoc_LINK, Idx_Defs, inc, minlevel)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx_Defs;
int inc;
int minlevel;
    {
    struct _ptoc_FRAME_Global_Definitions_GenC _ptoc_LOCAL;
    int Idx;
    int last;
    int level;
    int x;
    int first;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx = Idx_Defs;
    level = STab[Idx].SLevel;
    if (level >= minlevel)
        {
        last = STab[Idx].KINDTYPE.C_BlockDcl.SLastEntry;
        x = Idx;
        x = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, x);
        while (x <= last)
            {
            if (IsEnumDcl(x))
                {
                DefineEnum_GenC(_ptoc_LOCAL._ptoc_LINK, x , inc);
                }
            x = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, x);
            }
        x = Idx;
        x = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, x);
        while (x <= last)
            {
            if (!IsEnumDcl(x) && !IsConstDcl(x) && !IsFuncDcl_GenC(_ptoc_LOCAL._ptoc_LINK, x))
                {
                Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, x , inc , 0 , 0);
                }
            else
                {
                if (UseSymbolicConst() && IsConstDcl(x) && IsOrdinal(PrimType(STab[x].SType)) && !IsEnumvalueDcl_GenC(_ptoc_LOCAL._ptoc_LINK, x))
                    {
                    DefineConstant_GenC(_ptoc_LOCAL._ptoc_LINK, x);
                    }
                }
            x = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, x);
            }
        Idx = STab[Idx_Defs].KINDTYPE.C_BlockDcl.SFuncBlock;
        first = STab[Idx].SFeld1;
        last = STab[Idx].SFeld2;
        while (first <= last && first != 0 && last != 0)
            {
            Idx = first;
            Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , 0 , 0);
            first += 1;
            }
        }
    }

struct _ptoc_FRAME_Modul_Definitions_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Modul_Definitions_GenC(_ptoc_LINK, Idx_Defs, inc, minlevel)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx_Defs;
int inc;
int minlevel;
    {
    struct _ptoc_FRAME_Modul_Definitions_GenC _ptoc_LOCAL;
    int Idx;
    int Idx1;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx1 = Idx_Defs;
    while (Idx1 <= SymPtr && STab[Idx1].SLevel < minlevel)
        Idx1 = STab[Idx1].SNext;
    Idx = Idx1;
    while (Idx <= _ptoc_LOCAL._ptoc_LINK->to_stab)
        {
        if (IsEnumDcl(Idx))
            {
            if (IsMarked(Idx))
                {
                DefineEnum_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
                }
            }
        Idx = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
        }
    Idx = Idx1;
    while (Idx <= _ptoc_LOCAL._ptoc_LINK->to_stab)
        {
        if (!IsConstDcl(Idx) && !IsEnumDcl(Idx))
            {
            if (UnmarkedType(Idx))
                {
                if (DeleteUnusedType())
                    {
                    }
                else
                    {
                    Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , 0 , 1);
                    }
                }
            else
                {
                if (DeleteUnusedVar() && UnmarkedVar(Idx))
                    {
                    }
                else
                    {
                    Single_Definition_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc , 0 , 1);
                    }
                }
            }
        else
            {
            if (UseSymbolicConst() && IsConstDcl(Idx) && IsOrdinal(PrimType(STab[Idx].SType)) && !IsEnumvalueDcl_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                {
                if (!IsMarked(Idx))
                    {
                    if (DeleteUnusedConst())
                        {
                        }
                    else
                        {
                        DefineConstant_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                        }
                    }
                else
                    {
                    DefineConstant_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
                    }
                }
            }
        Idx = LevelNext_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
        }
    }

struct _ptoc_FRAME_BINOP_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int BINOP_GenC(_ptoc_LINK, Op)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Op;
    {
    struct _ptoc_FRAME_BINOP_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var77;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (Op)
        {
        case 21 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " * ");
            break;
        case 22 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " / ");
            break;
        case 88 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " % ");
            break;
        case 19 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " + ");
            break;
        case 20 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " - ");
            break;
        case 30 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " >> ");
            break;
        case 29 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " << ");
            break;
        case 32 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " < ");
            break;
        case 33 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " > ");
            break;
        case 34 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " <= ");
            break;
        case 35 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " >= ");
            break;
        case 31 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " == ");
            break;
        case 36 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " != ");
            break;
        case 89 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " & ");
            break;
        case 91 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " ^ ");
            break;
        case 90 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " | ");
            break;
        case 27 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " && ");
            break;
        case 25 :
            ptoc_Var77 = sql__ut(outfile);
            fprintf(ptoc_Var77->_fbuf , "%s" , " || ");
            break;
        }
    }

struct _ptoc_FRAME_ASGNOP_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ASGNOP_GenC(_ptoc_LINK, Op)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Op;
    {
    struct _ptoc_FRAME_ASGNOP_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var78;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (Op)
        {
        case 19 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " += ");
            break;
        case 20 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " -= ");
            break;
        case 21 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " *= ");
            break;
        case 22 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " /= ");
            break;
        case 88 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " %= ");
            break;
        case 30 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " >>= ");
            break;
        case 29 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " <<= ");
            break;
        case 89 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " &= ");
            break;
        case 91 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " ^= ");
            break;
        case 90 :
            ptoc_Var78 = sql__ut(outfile);
            fprintf(ptoc_Var78->_fbuf , "%s" , " |= ");
            break;
        }
    }

struct _ptoc_FRAME_DruPostfix_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DruPostfix_GenC(_ptoc_LINK, Op)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Op;
    {
    struct _ptoc_FRAME_DruPostfix_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var79;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (Op)
        {
        case 19 :
            ptoc_Var79 = sql__ut(outfile);
            fprintf(ptoc_Var79->_fbuf , "%s" , "++");
            break;
        case 20 :
            ptoc_Var79 = sql__ut(outfile);
            fprintf(ptoc_Var79->_fbuf , "%s" , "--");
            break;
        }
    }

struct _ptoc_FRAME_FillBlanks_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int FillBlanks_GenC(_ptoc_LINK, Idx, index)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int index;
    {
    struct _ptoc_FRAME_FillBlanks_GenC _ptoc_LOCAL;
    int i;
    int len;
    int typelen;
    int s_index;
    int ptoc_Var80;
    int ptoc_Var81;
    _iorechd *ptoc_Var82;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    len = _GetStringLength(index);
    s_index = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
    typelen = STab[s_index].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[s_index].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1;
    ptoc_Var80 = len + 1;
    ptoc_Var81 = typelen;
    if (ptoc_Var80 <= ptoc_Var81)
        {
        i = ptoc_Var80;
        do
            {
            ptoc_Var82 = sql__ut(outfile);
            fprintf(ptoc_Var82->_fbuf , "%c" , 32);
            if (i == ptoc_Var81)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_CONSTANT_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int CONSTANT_GenC(_ptoc_LINK, Node, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_CONSTANT_GenC _ptoc_LOCAL;
    int anz;
    int level;
    HdgRange NodeIndex;
    int anz_bytes;
    int Idx;
    int i;
    STRING dblstr;
    _iorechd *ptoc_Var83;
    int ptoc_Var84;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    switch (shr(field[NodeIndex] , 16))
        {
        case 1 :
            switch (field[NodeIndex] & 65535)
                {
                case 12 :
                    if (control[81])
                        {
                        ptoc_Var83 = sql__ut(output);
                        fprintf(ptoc_Var83->_fbuf , "%s" , "LitOp");
                        sql__writeln(ptoc_Var83);
                        }
                    switch (STab[field[NodeIndex + 1]].SType)
                        {
                        case -3 :
                        case -4 :
                            TypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1]);
                            long_to_string(field[NodeIndex + 5] , field[NodeIndex + 6] , dblstr , &anz_bytes);
                            ptoc_Var83 = sql__ut(outfile);
                            ptoc_Var84 = anz_bytes;
                            fprintf(ptoc_Var83->_fbuf , "%*.*s" , ptoc_Var84 , ptoc_Var84 < 256 ? ptoc_Var84 : 256 , dblstr);
                            break;
                        case -11 :
                        case -12 :
                            TypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1]);
                            double_to_string(field[NodeIndex + 5] , field[NodeIndex + 6] , dblstr , &anz_bytes);
                            ptoc_Var83 = sql__ut(outfile);
                            ptoc_Var84 = anz_bytes;
                            fprintf(ptoc_Var83->_fbuf , "%*.*s" , ptoc_Var84 , ptoc_Var84 < 256 ? ptoc_Var84 : 256 , dblstr);
                            break;
                        case -13 :
                            if (control[97])
                                {
                                ptoc_Var83 = sql__ut(outfile);
                                fprintf(ptoc_Var83->_fbuf , "%s" , "(unsigned char *)\"");
                                }
                            else
                                {
                                ptoc_Var83 = sql__ut(outfile);
                                fprintf(ptoc_Var83->_fbuf , "%c" , 34);
                                }
                            DumpString_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                            FillBlanks_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1] , field[NodeIndex + 5]);
                            ptoc_Var83 = sql__ut(outfile);
                            fprintf(ptoc_Var83->_fbuf , "%c" , 34);
                            break;
                        case -17 :
                            TypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1]);
                            ptoc_Var83 = sql__ut(outfile);
                            fprintf(ptoc_Var83->_fbuf , "%*d" , 1 , field[NodeIndex + 5]);
                            break;
                        case -9 :
                        case -24 :
                            ptoc_Var83 = sql__ut(outfile);
                            fprintf(ptoc_Var83->_fbuf , "%s" , "(unsigned char)\'");
                            DumpChar_GenC(_ptoc_LOCAL._ptoc_LINK, (unsigned char ) field[NodeIndex + 5]);
                            ptoc_Var83 = sql__ut(outfile);
                            fprintf(ptoc_Var83->_fbuf , "%c" , 39);
                            break;
                        case -8 :
                            ptoc_Var83 = sql__ut(outfile);
                            fprintf(ptoc_Var83->_fbuf , "%c" , 39);
                            DumpChar_GenC(_ptoc_LOCAL._ptoc_LINK, (unsigned char ) field[NodeIndex + 5]);
                            ptoc_Var83 = sql__ut(outfile);
                            fprintf(ptoc_Var83->_fbuf , "%c" , 39);
                            break;
                        default :
                            ptoc_Var83 = sql__ut(outfile);
                            fprintf(ptoc_Var83->_fbuf , "%*d" , 1 , field[NodeIndex + 5]);
                            break;
                        }
                    break;
                case 14 :
                    if (control[81])
                        {
                        ptoc_Var83 = sql__ut(output);
                        fprintf(ptoc_Var83->_fbuf , "%s" , "LoadstrOp");
                        sql__writeln(ptoc_Var83);
                        }
                    ptoc_Var83 = sql__ut(outfile);
                    fprintf(ptoc_Var83->_fbuf , "%c" , 34);
                    DumpString_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                    ptoc_Var83 = sql__ut(outfile);
                    fprintf(ptoc_Var83->_fbuf , "%c" , 34);
                    break;
                case 69 :
                    if (control[81])
                        {
                        ptoc_Var83 = sql__ut(output);
                        fprintf(ptoc_Var83->_fbuf , "%s" , "LitSizeOp");
                        sql__writeln(ptoc_Var83);
                        }
                    Idx = field[NodeIndex + 5];
                    ptoc_Var83 = sql__ut(outfile);
                    fprintf(ptoc_Var83->_fbuf , "%s" , "sizeof(");
                    TYPE_NAME_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 0);
                    ptoc_Var83 = sql__ut(outfile);
                    fprintf(ptoc_Var83->_fbuf , "%c" , 41);
                    break;
                case 109 :
                    if (control[81])
                        {
                        ptoc_Var83 = sql__ut(output);
                        fprintf(ptoc_Var83->_fbuf , "%s" , "StaticLinkOp");
                        sql__writeln(ptoc_Var83);
                        }
                    level = STab[field[NodeIndex + 5]].SLevel;
                    if (level <= 1)
                        {
                        ptoc_Var83 = sql__ut(outfile);
                        fprintf(ptoc_Var83->_fbuf , "%s" , " 0 ");
                        }
                    else
                        {
                        level = CurrentLevel - level + 1;
                        if (level == 0)
                            {
                            if (PAR(Oop , Side , 13))
                                {
                                ptoc_Var83 = sql__ut(outfile);
                                fprintf(ptoc_Var83->_fbuf , "%s" , "(&_ptoc_LOCAL)");
                                }
                            else
                                {
                                ptoc_Var83 = sql__ut(outfile);
                                fprintf(ptoc_Var83->_fbuf , "%s" , "&_ptoc_LOCAL");
                                }
                            }
                        else
                            {
                            ptoc_Var83 = sql__ut(outfile);
                            fprintf(ptoc_Var83->_fbuf , "%s" , "_ptoc_LOCAL._ptoc_LINK");
                            while (level > 1)
                                {
                                ptoc_Var83 = sql__ut(outfile);
                                fprintf(ptoc_Var83->_fbuf , "%s" , "->_ptoc_LINK");
                                level += -1;
                                }
                            }
                        }
                    break;
                default :
                    Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                    break;
                }
            break;
        default :
            Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
            break;
        }
    }

struct _ptoc_FRAME_FillSRecVar_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int FillSRecVar_GenC(_ptoc_LINK, Idx, typ)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int typ;
    {
    struct _ptoc_FRAME_FillSRecVar_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var85;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (STab[Idx].SRecVar > 0)
        {
        if (STab[STab[Idx].SRecVar].SType != typ)
            {
            FillSRecVar_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SRecVar , typ);
            ptoc_Var85 = sql__ut(outfile);
            fprintf(ptoc_Var85->_fbuf , "%c" , 46);
            }
        }
    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    }

struct _ptoc_FRAME_IDENTIFIER_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IDENTIFIER_GenC(_ptoc_LINK, Node, typ)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int typ;
    {
    struct _ptoc_FRAME_IDENTIFIER_GenC _ptoc_LOCAL;
    HdgRange NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    switch (shr(field[NodeIndex] , 16))
        {
        case 1 :
            switch (field[NodeIndex] & 65535)
                {
                case 70 :
                    if (STab[field[NodeIndex + 5]].SRecVar > 0)
                        {
                        FillSRecVar_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , typ);
                        }
                    else
                        {
                        PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                        }
                    break;
                default :
                    Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                    break;
                }
            break;
        default :
            Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
            break;
        }
    }

struct _ptoc_FRAME_IsNodeId_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsNodeId_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_IsNodeId_GenC _ptoc_LOCAL;
    int _IsNodeId;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsNodeId = shr(field[NodeIndex] , 16) == 1 && (field[NodeIndex] & 65535) == 94;
    return(_IsNodeId);
    }

struct _ptoc_FRAME_IsParam_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsParam_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsParam_GenC _ptoc_LOCAL;
    int _IsParam;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsParam = STab[Idx].SKind == 14;
    return(_IsParam);
    }

struct _ptoc_FRAME_IsRecordElement_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsRecordElement_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_IsRecordElement_GenC _ptoc_LOCAL;
    int _IsRecordElement;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsRecordElement = shr(field[NodeIndex] , 16) == 5 && (field[NodeIndex] & 65535) == 96;
    return(_IsRecordElement);
    }

struct _ptoc_FRAME_IsPointerExpression_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsPointerExpression_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_IsPointerExpression_GenC _ptoc_LOCAL;
    int _IsPointerExpression;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsPointerExpression = shr(field[NodeIndex] , 16) == 5 && (field[NodeIndex] & 65535) != 95;
    return(_IsPointerExpression);
    }

struct _ptoc_FRAME_Typ_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Typ_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_Typ_GenC _ptoc_LOCAL;
    int _Typ;
    int t;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    t = field[Node + 1];
    while (STab[t].SKind == 16)
        t = STab[t].SType;
    _Typ = t;
    return(_Typ);
    }

struct _ptoc_FRAME_ElementTyp_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ElementTyp_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_ElementTyp_GenC _ptoc_LOCAL;
    int _ElementTyp;
    int t;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    t = Typ_GenC(_ptoc_LOCAL._ptoc_LINK, Node);
    if (STab[t].SType == -13)
        {
        t = STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement;
        while (STab[t].SKind == 16)
            t = STab[t].SType;
        }
    else
        {
        t = 0;
        }
    _ElementTyp = t;
    return(_ElementTyp);
    }

struct _ptoc_FRAME_DerefTyp_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DerefTyp_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_DerefTyp_GenC _ptoc_LOCAL;
    int _DerefTyp;
    int t;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    t = Typ_GenC(_ptoc_LOCAL._ptoc_LINK, Node);
    if (STab[t].SType == -17)
        {
        t = STab[t].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain;
        while (STab[t].SKind == 16)
            t = STab[t].SType;
        }
    else
        {
        t = 0;
        }
    _DerefTyp = t;
    return(_DerefTyp);
    }

struct _ptoc_FRAME_EqualSizeTypes_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int EqualSizeTypes_GenC(_ptoc_LINK, typ_1, typ_2)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int typ_1;
int typ_2;
    {
    struct _ptoc_FRAME_EqualSizeTypes_GenC _ptoc_LOCAL;
    int _EqualSizeTypes;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    while (typ_1 > 0)
        typ_1 = STab[typ_1].SType;
    while (typ_2 > 0)
        typ_2 = STab[typ_2].SType;
    if (typ_1 == typ_2)
        {
        _EqualSizeTypes = 1;
        }
    else
        {
        if (typ_1 == -8 || typ_1 == -9 || typ_1 == -24)
            {
            _EqualSizeTypes = typ_2 == -8 || typ_2 == -9 || typ_2 == -24;
            }
        else
            {
            if (typ_1 == -1 || typ_1 == -2)
                {
                _EqualSizeTypes = typ_2 == -1 || typ_2 == -2;
                }
            else
                {
                if (typ_1 == -3 || typ_1 == -4)
                    {
                    _EqualSizeTypes = typ_2 == -3 || typ_2 == -4;
                    }
                else
                    {
                    if (typ_1 == -5 || typ_1 == -6)
                        {
                        _EqualSizeTypes = typ_2 == -5 || typ_2 == -6;
                        }
                    else
                        {
                        _EqualSizeTypes = 0;
                        }
                    }
                }
            }
        }
    return(_EqualSizeTypes);
    }

struct _ptoc_FRAME_IsDerefOp_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsDerefOp_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_IsDerefOp_GenC _ptoc_LOCAL;
    int _IsDerefOp;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsDerefOp = shr(field[NodeIndex] , 16) == 4 && (field[NodeIndex] & 65535) == 97;
    return(_IsDerefOp);
    }

struct _ptoc_FRAME_actual_paramlist_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int actual_paramlist_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_actual_paramlist_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var86;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (NodeIndex != -1)
        {
        NodeIndex = field[NodeIndex + 5];
        while (NodeIndex != -1)
            {
            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , 4 , 0);
            NodeIndex = field[NodeIndex + 3];
            if (NodeIndex != -1)
                {
                ptoc_Var86 = sql__ut(outfile);
                fprintf(ptoc_Var86->_fbuf , "%s" , " , ");
                }
            }
        }
    }

struct _ptoc_FRAME_TypeofStaticLink_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int TypeofStaticLink_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_TypeofStaticLink_GenC _ptoc_LOCAL;
    int _TypeofStaticLink;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (NodeIndex != -1)
        {
        if (shr(field[NodeIndex] , 16) == 1 && (field[NodeIndex] & 65535) == 109)
            {
            if (STab[field[NodeIndex + 5]].SLevel > 1)
                {
                _TypeofStaticLink = 1;
                }
            else
                {
                _TypeofStaticLink = 0;
                }
            }
        else
            {
            _TypeofStaticLink = 2;
            }
        }
    else
        {
        _TypeofStaticLink = 0;
        }
    return(_TypeofStaticLink);
    }

struct _ptoc_FRAME_IsSpecSys_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsSpecSys_GenC(_ptoc_LINK, index)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int index;
    {
    struct _ptoc_FRAME_IsSpecSys_GenC _ptoc_LOCAL;
    int _IsSpecSys;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsSpecSys = SpecFunction(index) == 3;
    return(_IsSpecSys);
    }

struct _ptoc_FRAME_SpecSys_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int SpecSys_GenC(_ptoc_LINK, index)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int index;
    {
    struct _ptoc_FRAME_SpecSys_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var87;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    switch (SpecFunction(index))
        {
        case 3 :
            ptoc_Var87 = sql__ut(outfile);
            fprintf(ptoc_Var87->_fbuf , "%s" , "exit(1)");
            break;
        default :
            break;
        }
    }

struct _ptoc_FRAME_IsArrayExpression_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsArrayExpression_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_IsArrayExpression_GenC _ptoc_LOCAL;
    int _IsArrayExpression;
    int t;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    t = STab[field[NodeIndex + 1]].SType;
    _IsArrayExpression = t == -13 || t == -15 || t == -16 || t == -20;
    return(_IsArrayExpression);
    }

struct _ptoc_FRAME_IsFuncExpression_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsFuncExpression_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_IsFuncExpression_GenC _ptoc_LOCAL;
    int _IsFuncExpression;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsFuncExpression = STab[field[NodeIndex + 1]].SType == -23 || STab[field[NodeIndex + 1]].SType == -18;
    return(_IsFuncExpression);
    }

struct _ptoc_FRAME_TERM_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int TERM_GenC(_ptoc_LINK, Node, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_TERM_GenC _ptoc_LOCAL;
    int anz;
    int i;
    HdgRange NodeIndex;
    HdgRange NodeId;
    int par;
    _iorechd *ptoc_Var88;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var88 = sql__ut(output);
        fprintf(ptoc_Var88->_fbuf , "%*.*s" , 30 , 7 , "TERM : ");
        }
    NodeIndex = Node;
    switch (shr(field[NodeIndex] , 16))
        {
        case 4 :
            switch (field[NodeIndex] & 65535)
                {
                case 4 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "UnaryExp, LoadOp -> TERM");
                        sql__writeln(ptoc_Var88);
                        }
                    TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , Oop , Side);
                    break;
                case 13 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "UnaryExp, LoadadrOp");
                        sql__writeln(ptoc_Var88);
                        }
                    if (IsDerefOp_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]))
                        {
                        if (field[NodeIndex + 1] != field[field[field[NodeIndex + 5] + 5] + 1])
                            {
                            par = PAR(Oop , Side , 93);
                            if (par)
                                {
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                }
                            TypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1]);
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[field[NodeIndex + 5] + 5] , 93 , 2);
                            if (par)
                                {
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                }
                            }
                        else
                            {
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[field[NodeIndex + 5] + 5] , Oop , Side);
                            }
                        }
                    else
                        {
                        if (!IsPointerExpression_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]) || IsRecordElement_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]))
                            {
                            if (!IsArrayExpression_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]) && !IsFuncExpression_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]))
                                {
                                par = PAR(Oop , Side , 13);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                    }
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 38);
                                TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 13 , Side);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                    }
                                }
                            else
                                {
                                TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , Oop , Side);
                                }
                            }
                        else
                            {
                            if (field[NodeIndex + 1] != field[field[NodeIndex + 5] + 1])
                                {
                                par = PAR(Oop , Side , 93);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                    }
                                TypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1]);
                                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 93 , 2);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                    }
                                }
                            else
                                {
                                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , Oop , Side);
                                }
                            }
                        }
                    break;
                case 97 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "UnaryExp, DerefOp");
                        sql__writeln(ptoc_Var88);
                        }
                    par = PAR(Oop , Side , 97);
                    if (par)
                        {
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                        }
                    ptoc_Var88 = sql__ut(outfile);
                    fprintf(ptoc_Var88->_fbuf , "%c" , 42);
                    if (Typ_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex) != DerefTyp_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]))
                        {
                        PointerTypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1]);
                        }
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 97 , 2);
                    if (par)
                        {
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                        }
                    break;
                case 9 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "UnaryExp, StorepadrOp");
                        sql__writeln(ptoc_Var88);
                        }
                    if (IsDerefOp_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]))
                        {
                        EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[field[NodeIndex + 5] + 5] , 4 , 0);
                        }
                    else
                        {
                        if (!IsArrayExpression_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]) && !IsFuncExpression_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]))
                            {
                            ptoc_Var88 = sql__ut(outfile);
                            fprintf(ptoc_Var88->_fbuf , "%c" , 38);
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 13 , 0);
                            }
                        else
                            {
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 4 , 0);
                            }
                        }
                    break;
                default :
                    Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                    break;
                }
            break;
        case 1 :
            switch (field[NodeIndex] & 65535)
                {
                case 94 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "node, IdOp");
                        sql__writeln(ptoc_Var88);
                        }
                    if (_ptoc_LOCAL._ptoc_LINK->StatLink)
                        {
                        STAT_Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , Oop , Side);
                        }
                    else
                        {
                        Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , Oop , Side);
                        }
                    break;
                case 110 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "node, RegOp");
                        sql__writeln(ptoc_Var88);
                        }
                    ptoc_Var88 = sql__ut(outfile);
                    fprintf(ptoc_Var88->_fbuf , "%s" , "sql__gc()");
                    break;
                default :
                    Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                    break;
                }
            break;
        case 7 :
            switch (field[NodeIndex] & 65535)
                {
                case 40 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "LongStmnt, CallOp");
                        sql__writeln(ptoc_Var88);
                        }
                    if (_ptoc_LOCAL._ptoc_LINK->StatLink)
                        {
                        switch (TypeofStaticLink_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7]))
                            {
                            case 0 :
                                par = PAR(Oop , Side , 40);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                    }
                                TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 40 , 1);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                actual_paramlist_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                    }
                                break;
                            case 1 :
                                par = PAR(Oop , Side , 40);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                    }
                                TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 40 , 1);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7] , 4 , 0);
                                if (field[NodeIndex + 6] != -1)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%s" , ", ");
                                    }
                                actual_paramlist_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                    }
                                break;
                            case 2 :
                                par = PAR(Oop , Side , 121);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                    }
                                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7] , 121 , 1);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%s" , " ? ");
                                TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 40 , 1);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7] , 4 , 0);
                                if (field[NodeIndex + 6] != -1)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%s" , ", ");
                                    }
                                actual_paramlist_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%s" , " : ");
                                TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 40 , 1);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                                actual_paramlist_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                                ptoc_Var88 = sql__ut(outfile);
                                fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                if (par)
                                    {
                                    ptoc_Var88 = sql__ut(outfile);
                                    fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                                    }
                                break;
                            default :
                                break;
                            }
                        }
                    else
                        {
                        par = PAR(Oop , Side , 40);
                        if (par)
                            {
                            ptoc_Var88 = sql__ut(outfile);
                            fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                            }
                        TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 40 , 1);
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                        actual_paramlist_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                        if (par)
                            {
                            ptoc_Var88 = sql__ut(outfile);
                            fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                            }
                        }
                    break;
                case 62 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "LongStmnt, SysOp");
                        sql__writeln(ptoc_Var88);
                        }
                    if (IsSpecSys_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7]))
                        {
                        SpecSys_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7]);
                        }
                    else
                        {
                        par = PAR(Oop , Side , 40);
                        if (par)
                            {
                            ptoc_Var88 = sql__ut(outfile);
                            fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                            }
                        PutString_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7] , 0);
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                        actual_paramlist_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                        if (par)
                            {
                            ptoc_Var88 = sql__ut(outfile);
                            fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                            }
                        }
                    break;
                case 103 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "LongStmnt, PostfixOp -> EXPRESSION");
                        sql__writeln(ptoc_Var88);
                        }
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , Oop , Side);
                    break;
                case 6 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "LongStmnt, StoreOp -> EXPRESSION");
                        sql__writeln(ptoc_Var88);
                        }
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , Oop , Side);
                    break;
                default :
                    Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                    break;
                }
            break;
        case 5 :
            switch (field[NodeIndex] & 65535)
                {
                case 95 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "BinaryExp, SubscriptOp");
                        sql__writeln(ptoc_Var88);
                        }
                    par = PAR(Oop , Side , 95);
                    if (par)
                        {
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                        }
                    if (!EqualSizeTypes_GenC(_ptoc_LOCAL._ptoc_LINK, Typ_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex) , ElementTyp_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5])))
                        {
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                        PointerTypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1]);
                        TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 93 , 2);
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                        }
                    else
                        {
                        TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 95 , 1);
                        }
                    ptoc_Var88 = sql__ut(outfile);
                    fprintf(ptoc_Var88->_fbuf , "%c" , 91);
                    if (field[NodeIndex + 6] != -1)
                        {
                        if (field[NodeIndex + 8] != 0)
                            {
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 19 , 1);
                            ptoc_Var88 = sql__ut(outfile);
                            fprintf(ptoc_Var88->_fbuf , "%s%*d" , " + " , 1 , field[NodeIndex + 8]);
                            }
                        else
                            {
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 4 , 0);
                            }
                        }
                    else
                        {
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%*d" , 1 , field[NodeIndex + 8]);
                        }
                    ptoc_Var88 = sql__ut(outfile);
                    fprintf(ptoc_Var88->_fbuf , "%c" , 93);
                    if (par)
                        {
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                        }
                    break;
                case 96 :
                    if (control[81])
                        {
                        ptoc_Var88 = sql__ut(output);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "BinaryExp, ElementOp");
                        sql__writeln(ptoc_Var88);
                        }
                    par = PAR(Oop , Side , 96);
                    if (par)
                        {
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 40);
                        }
                    if (IsDerefOp_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]) && DerefTyp_GenC(_ptoc_LOCAL._ptoc_LINK, field[field[NodeIndex + 5] + 5]) == Typ_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]))
                        {
                        TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[field[NodeIndex + 5] + 5] , 96 , 1);
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%s" , "->");
                        }
                    else
                        {
                        TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 96 , 1);
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 46);
                        }
                    IDENTIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , Typ_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]));
                    if (par)
                        {
                        ptoc_Var88 = sql__ut(outfile);
                        fprintf(ptoc_Var88->_fbuf , "%c" , 41);
                        }
                    break;
                default :
                    Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                    break;
                }
            break;
        default :
            Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
            break;
        }
    }

struct _ptoc_FRAME_IsArrayAssign_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsArrayAssign_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_IsArrayAssign_GenC _ptoc_LOCAL;
    int _IsArrayAssign;
    int t;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (shr(field[NodeIndex] , 16) == 7 && (field[NodeIndex] & 65535) == 6)
        {
        t = STab[field[NodeIndex + 1]].SType;
        _IsArrayAssign = t == -13 || t == -16 || t == -20 || t == -15;
        }
    else
        {
        _IsArrayAssign = 0;
        }
    return(_IsArrayAssign);
    }

struct _ptoc_FRAME_IsStringOperation_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsStringOperation_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_IsStringOperation_GenC _ptoc_LOCAL;
    int _IsStringOperation;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    _IsStringOperation = STab[Typ_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5])].SType == -13;
    return(_IsStringOperation);
    }

struct _ptoc_FRAME_StringOperation_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int StringOperation_GenC(_ptoc_LINK, Node, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_StringOperation_GenC _ptoc_LOCAL;
    int NodeIndex;
    int Idx;
    int Idx1;
    int par;
    _iorechd *ptoc_Var89;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    Idx = field[field[NodeIndex + 5] + 1];
    par = PAR(Oop , Side , 121);
    if (par)
        {
        ptoc_Var89 = sql__ut(outfile);
        fprintf(ptoc_Var89->_fbuf , "%c" , 40);
        }
    ptoc_Var89 = sql__ut(outfile);
    fprintf(ptoc_Var89->_fbuf , "%s" , "_ptoc_MEMCMP(");
    TYPE_NAME_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , 0);
    ptoc_Var89 = sql__ut(outfile);
    fprintf(ptoc_Var89->_fbuf , "%s" , ", ");
    Idx1 = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
    ptoc_Var89 = sql__ut(outfile);
    fprintf(ptoc_Var89->_fbuf , "%*d" , 1 , STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1);
    ptoc_Var89 = sql__ut(outfile);
    fprintf(ptoc_Var89->_fbuf , "%s" , ", ");
    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 6 , 1);
    ptoc_Var89 = sql__ut(outfile);
    fprintf(ptoc_Var89->_fbuf , "%s" , " , ");
    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 6 , 2);
    ptoc_Var89 = sql__ut(outfile);
    fprintf(ptoc_Var89->_fbuf , "%c" , 41);
    switch (field[NodeIndex] & 65535)
        {
        case 32 :
            ptoc_Var89 = sql__ut(outfile);
            fprintf(ptoc_Var89->_fbuf , "%s" , " < 0 ? 1 : 0");
            break;
        case 33 :
            ptoc_Var89 = sql__ut(outfile);
            fprintf(ptoc_Var89->_fbuf , "%s" , " > 0 ? 1 : 0");
            break;
        case 34 :
            ptoc_Var89 = sql__ut(outfile);
            fprintf(ptoc_Var89->_fbuf , "%s" , " > 0 ? 0 : 1");
            break;
        case 35 :
            ptoc_Var89 = sql__ut(outfile);
            fprintf(ptoc_Var89->_fbuf , "%s" , " < 0 ? 0 : 1");
            break;
        case 31 :
            ptoc_Var89 = sql__ut(outfile);
            fprintf(ptoc_Var89->_fbuf , "%s" , " == 0 ? 1 : 0");
            break;
        case 36 :
            ptoc_Var89 = sql__ut(outfile);
            fprintf(ptoc_Var89->_fbuf , "%s" , " == 0 ? 0 : 1");
            break;
        default :
            Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
            break;
        }
    if (par)
        {
        ptoc_Var89 = sql__ut(outfile);
        fprintf(ptoc_Var89->_fbuf , "%c" , 41);
        }
    }

struct _ptoc_FRAME_ArrayAssign_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ArrayAssign_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
int inc;
    {
    struct _ptoc_FRAME_ArrayAssign_GenC _ptoc_LOCAL;
    int Idx;
    int Idx1;
    int t;
    int NodeIndex;
    _iorechd *ptoc_Var90;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    Idx = field[NodeIndex + 1];
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 0 , Node , inc);
    ptoc_Var90 = sql__ut(outfile);
    fprintf(ptoc_Var90->_fbuf , "%s" , "_ptoc_ASSIGN(");
    t = STab[Idx].SType;
    if (t == -16 || t == -15 || t == -20)
        {
        TypeSize(Idx);
        GenArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize);
        }
    TYPE_NAME_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , 0);
    ptoc_Var90 = sql__ut(outfile);
    fprintf(ptoc_Var90->_fbuf , "%s" , ", ");
    Idx1 = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
    ptoc_Var90 = sql__ut(outfile);
    fprintf(ptoc_Var90->_fbuf , "%*d" , 1 , STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1);
    ptoc_Var90 = sql__ut(outfile);
    fprintf(ptoc_Var90->_fbuf , "%s" , ", ");
    TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 6 , 1);
    ptoc_Var90 = sql__ut(outfile);
    fprintf(ptoc_Var90->_fbuf , "%s" , ", ");
    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 6 , 2);
    ptoc_Var90 = sql__ut(outfile);
    fprintf(ptoc_Var90->_fbuf , "%c" , 41);
    }

struct _ptoc_FRAME_ArrayMove_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ArrayMove_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_ArrayMove_GenC _ptoc_LOCAL;
    int Idx;
    int Idx1;
    int t;
    int NodeIndex;
    _iorechd *ptoc_Var91;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    Idx = field[NodeIndex + 1];
    ptoc_Var91 = sql__ut(outfile);
    fprintf(ptoc_Var91->_fbuf , "%s" , "_ptoc_MOVE(");
    t = STab[Idx].SType;
    if (t == -16 || t == -15 || t == -20)
        {
        TypeSize(Idx);
        GenArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , STab[Idx].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize);
        }
    TYPE_NAME_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , 0);
    ptoc_Var91 = sql__ut(outfile);
    fprintf(ptoc_Var91->_fbuf , "%s" , ", ");
    Idx1 = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex;
    ptoc_Var91 = sql__ut(outfile);
    fprintf(ptoc_Var91->_fbuf , "%*d" , 1 , STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange - STab[Idx1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange + 1);
    ptoc_Var91 = sql__ut(outfile);
    fprintf(ptoc_Var91->_fbuf , "%s" , ", ");
    TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 6 , 1);
    ptoc_Var91 = sql__ut(outfile);
    fprintf(ptoc_Var91->_fbuf , "%s" , ", ");
    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 6 , 2);
    ptoc_Var91 = sql__ut(outfile);
    fprintf(ptoc_Var91->_fbuf , "%c" , 41);
    }

struct _ptoc_FRAME_PadVariable_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int PadVariable_GenC(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_PadVariable_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var92;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var92 = sql__ut(outfile);
    fprintf(ptoc_Var92->_fbuf , "%s" , "_ptoc_MEMSET(&");
    Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1 , 0);
    ptoc_Var92 = sql__ut(outfile);
    fprintf(ptoc_Var92->_fbuf , "%s" , ", (unsigned char)\'");
    DumpChar_GenC(_ptoc_LOCAL._ptoc_LINK, FillByte);
    ptoc_Var92 = sql__ut(outfile);
    fprintf(ptoc_Var92->_fbuf , "%s" , "\', sizeof(");
    Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1 , 0);
    ptoc_Var92 = sql__ut(outfile);
    fprintf(ptoc_Var92->_fbuf , "%s" , "))");
    }

struct _ptoc_FRAME_EXPRESSION_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int EXPRESSION_GenC(_ptoc_LINK, Node, Oop, Side)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int Oop;
int Side;
    {
    struct _ptoc_FRAME_EXPRESSION_GenC _ptoc_LOCAL;
    HdgRange NodeIndex;
    int par;
    _iorechd *ptoc_Var93;
    int ptoc_Var94;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var93 = sql__ut(output);
        fprintf(ptoc_Var93->_fbuf , "%*.*s" , 30 , 13 , "EXPRESSION : ");
        }
    NodeIndex = Node;
    if (field[NodeIndex + 2] == 1)
        {
        if (control[81])
            {
            ptoc_Var93 = sql__ut(output);
            fprintf(ptoc_Var93->_fbuf , "%s" , "CONSTANT, ");
            }
        CONSTANT_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , Oop , Side);
        }
    else
        {
        if (field[NodeIndex + 2] == 2 || field[NodeIndex + 2] == 3)
            {
            if (control[81])
                {
                ptoc_Var93 = sql__ut(output);
                fprintf(ptoc_Var93->_fbuf , "%s" , "TERM");
                sql__writeln(ptoc_Var93);
                }
            TERM_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , Oop , Side);
            }
        else
            {
            switch (shr(field[NodeIndex] , 16))
                {
                case 5 :
                    if (control[81])
                        {
                        ptoc_Var93 = sql__ut(output);
                        fprintf(ptoc_Var93->_fbuf , "%s" , "BinaryExp");
                        sql__writeln(ptoc_Var93);
                        }
                    if (IsStringOperation_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex))
                        {
                        StringOperation_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , Oop , Side);
                        }
                    else
                        {
                        par = PAR(Oop , Side , field[NodeIndex] & 65535);
                        if (par)
                            {
                            ptoc_Var93 = sql__ut(outfile);
                            fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                            }
                        EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , field[NodeIndex] & 65535 , 1);
                        ptoc_Var94 = field[NodeIndex] & 65535;
                        BINOP_GenC(_ptoc_LOCAL._ptoc_LINK, ptoc_Var94);
                        EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , ptoc_Var94 , 2);
                        if (par)
                            {
                            ptoc_Var93 = sql__ut(outfile);
                            fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                            }
                        }
                    break;
                case 7 :
                    if (control[81])
                        {
                        ptoc_Var93 = sql__ut(output);
                        fprintf(ptoc_Var93->_fbuf , "%s" , "LongStmnt, ");
                        }
                    switch (field[NodeIndex] & 65535)
                        {
                        case 6 :
                            if (control[81])
                                {
                                ptoc_Var93 = sql__ut(output);
                                fprintf(ptoc_Var93->_fbuf , "%s" , "StoreOp");
                                sql__writeln(ptoc_Var93);
                                }
                            if (IsArrayAssign_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex))
                                {
                                ArrayMove_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                                }
                            else
                                {
                                par = PAR(Oop , Side , 6);
                                if (par)
                                    {
                                    ptoc_Var93 = sql__ut(outfile);
                                    fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                                    }
                                TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 6 , 1);
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%s" , " = ");
                                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 6 , 2);
                                if (par)
                                    {
                                    ptoc_Var93 = sql__ut(outfile);
                                    fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                                    }
                                }
                            break;
                        case 102 :
                            if (control[81])
                                {
                                ptoc_Var93 = sql__ut(output);
                                fprintf(ptoc_Var93->_fbuf , "%s" , "PrefixOp");
                                sql__writeln(ptoc_Var93);
                                }
                            par = PAR(Oop , Side , 6);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                                }
                            TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 6 , 1);
                            ASGNOP_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 8]);
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 6 , 2);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                                }
                            break;
                        case 103 :
                            if (control[81])
                                {
                                ptoc_Var93 = sql__ut(output);
                                fprintf(ptoc_Var93->_fbuf , "%s" , "PostfixOp");
                                sql__writeln(ptoc_Var93);
                                }
                            par = PAR(Oop , Side , 103);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                                }
                            TERM_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 103 , 1);
                            DruPostfix_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 8]);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                                }
                            break;
                        default :
                            Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                            break;
                        }
                    break;
                case 8 :
                    if (control[81])
                        {
                        ptoc_Var93 = sql__ut(output);
                        fprintf(ptoc_Var93->_fbuf , "%s" , "ifthenelse");
                        sql__writeln(ptoc_Var93);
                        }
                    par = PAR(Oop , Side , 121);
                    if (par)
                        {
                        ptoc_Var93 = sql__ut(outfile);
                        fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                        }
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 121 , 0);
                    ptoc_Var93 = sql__ut(outfile);
                    fprintf(ptoc_Var93->_fbuf , "%s" , " ? ");
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 121 , 0);
                    ptoc_Var93 = sql__ut(outfile);
                    fprintf(ptoc_Var93->_fbuf , "%s" , " : ");
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7] , 121 , 0);
                    if (par)
                        {
                        ptoc_Var93 = sql__ut(outfile);
                        fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                        }
                    break;
                case 4 :
                    if (control[81])
                        {
                        ptoc_Var93 = sql__ut(output);
                        fprintf(ptoc_Var93->_fbuf , "%s" , "UnaryExp, ");
                        }
                    switch (field[NodeIndex] & 65535)
                        {
                        case 93 :
                            if (control[81])
                                {
                                ptoc_Var93 = sql__ut(output);
                                fprintf(ptoc_Var93->_fbuf , "%s" , "ConvertOp");
                                sql__writeln(ptoc_Var93);
                                }
                            if (STab[field[NodeIndex + 1]].SType == -21)
                                {
                                PadVariable_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                                }
                            else
                                {
                                par = PAR(Oop , Side , 93);
                                if (par)
                                    {
                                    ptoc_Var93 = sql__ut(outfile);
                                    fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                                    }
                                TypeCast_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 1]);
                                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 93 , 2);
                                if (par)
                                    {
                                    ptoc_Var93 = sql__ut(outfile);
                                    fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                                    }
                                }
                            break;
                        case 24 :
                            if (control[81])
                                {
                                ptoc_Var93 = sql__ut(output);
                                fprintf(ptoc_Var93->_fbuf , "%s" , "UminusOp");
                                sql__writeln(ptoc_Var93);
                                }
                            par = PAR(Oop , Side , 24);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                                }
                            ptoc_Var93 = sql__ut(outfile);
                            fprintf(ptoc_Var93->_fbuf , "%c" , 45);
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 24 , 2);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                                }
                            break;
                        case 28 :
                            if (control[81])
                                {
                                ptoc_Var93 = sql__ut(output);
                                fprintf(ptoc_Var93->_fbuf , "%s" , "NotOp");
                                sql__writeln(ptoc_Var93);
                                }
                            par = PAR(Oop , Side , 28);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                                }
                            ptoc_Var93 = sql__ut(outfile);
                            fprintf(ptoc_Var93->_fbuf , "%c" , 33);
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 28 , 2);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                                }
                            break;
                        case 92 :
                            if (control[81])
                                {
                                ptoc_Var93 = sql__ut(output);
                                fprintf(ptoc_Var93->_fbuf , "%s" , "BNotOp");
                                sql__writeln(ptoc_Var93);
                                }
                            par = PAR(Oop , Side , 92);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                                }
                            ptoc_Var93 = sql__ut(outfile);
                            fprintf(ptoc_Var93->_fbuf , "%c" , 126);
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 92 , 2);
                            if (par)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                                }
                            break;
                        case 8 :
                            if (control[81])
                                {
                                ptoc_Var93 = sql__ut(output);
                                fprintf(ptoc_Var93->_fbuf , "%s" , "StoreOp");
                                sql__writeln(ptoc_Var93);
                                }
                            if (STab[Typ_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5])].SType == -13)
                                {
                                if (DelayedParCopy())
                                    {
                                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 93 , 2);
                                    }
                                else
                                    {
                                    ptoc_Var93 = sql__ut(outfile);
                                    fprintf(ptoc_Var93->_fbuf , "%s" , "*(");
                                    StructArray_GenC(_ptoc_LOCAL._ptoc_LINK, Typ_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]));
                                    ptoc_Var93 = sql__ut(outfile);
                                    fprintf(ptoc_Var93->_fbuf , "%s" , " *)");
                                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 93 , 2);
                                    }
                                }
                            else
                                {
                                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 8 , 1);
                                }
                            break;
                        default :
                            Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                            break;
                        }
                    break;
                case 6 :
                    NodeIndex = field[NodeIndex + 5];
                    if (Oop == 1 && field[field[NodeIndex + 3] + 3] == -1 && NoSideEffects(field[NodeIndex + 3]))
                        {
                        EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , Oop , Side);
                        }
                    else
                        {
                        if (1)
                            {
                            ptoc_Var93 = sql__ut(outfile);
                            fprintf(ptoc_Var93->_fbuf , "%c" , 40);
                            }
                        while (NodeIndex != -1)
                            {
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , 114 , 1);
                            NodeIndex = field[NodeIndex + 3];
                            if (NodeIndex != -1)
                                {
                                ptoc_Var93 = sql__ut(outfile);
                                fprintf(ptoc_Var93->_fbuf , "%s" , " , ");
                                }
                            }
                        if (1)
                            {
                            ptoc_Var93 = sql__ut(outfile);
                            fprintf(ptoc_Var93->_fbuf , "%c" , 41);
                            }
                        }
                    break;
                default :
                    Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                    break;
                }
            }
        }
    }

struct _ptoc_FRAME_empty_statement_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int empty_statement_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
    {
    struct _ptoc_FRAME_empty_statement_GenC _ptoc_LOCAL;
    int _empty_statement;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    _empty_statement = NodeIndex == -1;
    return(_empty_statement);
    }

struct _ptoc_FRAME_is_label_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int is_label_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
    {
    struct _ptoc_FRAME_is_label_GenC _ptoc_LOCAL;
    int _is_label;
    HdgRange NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    _is_label = shr(field[NodeIndex] , 16) == 2 && (field[NodeIndex] & 65535) == 98;
    return(_is_label);
    }

struct _ptoc_FRAME_next_statement_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int next_statement_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
    {
    struct _ptoc_FRAME_next_statement_GenC _ptoc_LOCAL;
    int _next_statement;
    HdgRange NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    switch (shr(field[NodeIndex] , 16))
        {
        case 2 :
            switch (field[NodeIndex] & 65535)
                {
                case 98 :
                    while (is_label_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex))
                        NodeIndex = field[NodeIndex + 3];
                    if (!EmptyStatement_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex))
                        {
                        NodeIndex = next_statement_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                        }
                    break;
                case 119 :
                    NodeIndex = field[NodeIndex + 4];
                    NodeIndex = field[NodeIndex + 3];
                    break;
                default :
                    NodeIndex = field[NodeIndex + 3];
                    break;
                }
            break;
        default :
            NodeIndex = field[NodeIndex + 3];
            break;
        }
    _next_statement = NodeIndex;
    return(_next_statement);
    }

struct _ptoc_FRAME_CASE_STATEMENT_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int CASE_STATEMENT_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int inc;
    {
    struct _ptoc_FRAME_CASE_STATEMENT_GenC _ptoc_LOCAL;
    HdgRange NodeIndex;
    _iorechd *ptoc_Var95;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
    ptoc_Var95 = sql__ut(outfile);
    fprintf(ptoc_Var95->_fbuf , "%c" , 123);
    if (shr(field[NodeIndex] , 16) == 2 && (field[NodeIndex] & 65535) == 119)
        {
        NodeIndex = field[NodeIndex + 3];
        }
    while (NodeIndex != -1)
        {
        switch (shr(field[NodeIndex] , 16))
            {
            case 11 :
                switch (field[NodeIndex] & 65535)
                    {
                    case 99 :
                        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , NodeIndex , inc);
                        ptoc_Var95 = sql__ut(outfile);
                        fprintf(ptoc_Var95->_fbuf , "%s" , "case ");
                        if (field[NodeIndex + 5] != field[NodeIndex + 6])
                            {
                            Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                            }
                        else
                            {
                            ptoc_Var95 = sql__ut(outfile);
                            fprintf(ptoc_Var95->_fbuf , "%*d%s" , 1 , field[NodeIndex + 5] , " :");
                            }
                        break;
                    case 100 :
                        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , NodeIndex , inc);
                        ptoc_Var95 = sql__ut(outfile);
                        fprintf(ptoc_Var95->_fbuf , "%s" , "default :");
                        CaseEmptyStatement_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 3] , inc + 4);
                        break;
                    default :
                        Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                        break;
                    }
                break;
            default :
                STATEMENT_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , inc + 4);
                break;
            }
        NodeIndex = next_statement_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
        }
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
    ptoc_Var95 = sql__ut(outfile);
    fprintf(ptoc_Var95->_fbuf , "%c" , 125);
    }

struct _ptoc_FRAME_IsField_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsField_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_IsField_GenC _ptoc_LOCAL;
    int _IsField;
    int NodeIndex;
    int major;
    int minor;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    major = shr(field[NodeIndex] , 16);
    minor = field[NodeIndex] & 65535;
    if (major >= 1 && major <= 15 && minor >= 0 && minor <= 120)
        {
        _IsField = 1;
        }
    else
        {
        _IsField = 0;
        }
    return(_IsField);
    }

struct _ptoc_FRAME_TestField_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int TestField_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_TestField_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var96;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (Node != -1)
        {
        if (!IsField_GenC(_ptoc_LOCAL._ptoc_LINK, Node))
            {
            Space_GenC(_ptoc_LOCAL._ptoc_LINK);
            ptoc_Var96 = sql__ut(outfile);
            fprintf(ptoc_Var96->_fbuf , "%s" , "Fehler in TestField!");
            sql__writeln(ptoc_Var96);
            Abbruch_GenC(_ptoc_LOCAL._ptoc_LINK);
            }
        }
    }

struct _ptoc_FRAME_statement_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int statement_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int inc;
    {
    struct _ptoc_FRAME_statement_GenC _ptoc_LOCAL;
    HdgRange NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    TestField_GenC(_ptoc_LOCAL._ptoc_LINK, Node);
    NodeIndex = Node;
    if (NodeIndex == -1)
        {
        STATEMENT_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
        }
    else
        {
        if (field[NodeIndex + 3] == -1)
            {
            STATEMENT_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
            }
        else
            {
            BLOCK_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc , -1);
            }
        }
    }

struct _ptoc_FRAME_LastNode_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int LastNode_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_LastNode_GenC _ptoc_LOCAL;
    int _LastNode;
    int NodeIndex;
    int tiefe;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    tiefe = 0;
    do
        {
        switch (shr(field[NodeIndex] , 16))
            {
            case 2 :
                switch (field[NodeIndex] & 65535)
                    {
                    case 119 :
                        tiefe += 1;
                        break;
                    case 120 :
                        tiefe += -1;
                        break;
                    default :
                        break;
                    }
                break;
            default :
                break;
            }
        NodeIndex = field[NodeIndex + 3];
        }
    while (!(tiefe == 0));
    _LastNode = NodeIndex;
    return(_LastNode);
    }

struct _ptoc_FRAME_ElseConflict_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ElseConflict_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_ElseConflict_GenC _ptoc_LOCAL;
    int _ElseConflict;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    _ElseConflict = 0;
    if (field[NodeIndex + 7] != -1)
        {
        _ElseConflict = 1;
        }
    return(_ElseConflict);
    }

struct _ptoc_FRAME_EmptyStatement_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int EmptyStatement_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_EmptyStatement_GenC _ptoc_LOCAL;
    int _EmptyStatement;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _EmptyStatement = 0;
    NodeIndex = Node;
    if (NodeIndex == -1)
        {
        _EmptyStatement = 1;
        }
    else
        {
        if (shr(field[NodeIndex] , 16) == 2 && (field[NodeIndex] & 65535) == 120 && field[NodeIndex + 3] == -1 || shr(field[NodeIndex] , 16) == 11)
            {
            _EmptyStatement = 1;
            }
        }
    return(_EmptyStatement);
    }

struct _ptoc_FRAME_CaseEmptyStatement_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int CaseEmptyStatement_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
int inc;
    {
    struct _ptoc_FRAME_CaseEmptyStatement_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var97;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (EmptyStatement_GenC(_ptoc_LOCAL._ptoc_LINK, Node))
        {
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
        ptoc_Var97 = sql__ut(outfile);
        fprintf(ptoc_Var97->_fbuf , "%c" , 59);
        }
    }

struct _ptoc_FRAME_ANWEISUNG_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ANWEISUNG_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int inc;
    {
    struct _ptoc_FRAME_ANWEISUNG_GenC _ptoc_LOCAL;
    HdgRange NodeIndex;
    _iorechd *ptoc_Var98;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    if (empty_statement_GenC(_ptoc_LOCAL._ptoc_LINK, Node))
        {
        if (control[81])
            {
            ptoc_Var98 = sql__ut(output);
            fprintf(ptoc_Var98->_fbuf , "%s" , "empty_statement");
            sql__writeln(ptoc_Var98);
            }
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
        ptoc_Var98 = sql__ut(outfile);
        fprintf(ptoc_Var98->_fbuf , "%c" , 59);
        }
    else
        {
        if (field[NodeIndex + 2] > 0 && field[NodeIndex + 2] <= 4)
            {
            if (control[81])
                {
                ptoc_Var98 = sql__ut(output);
                fprintf(ptoc_Var98->_fbuf , "%s" , "EXPRESSION");
                sql__writeln(ptoc_Var98);
                }
            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, Node , 1 , 0);
            ptoc_Var98 = sql__ut(outfile);
            fprintf(ptoc_Var98->_fbuf , "%c" , 59);
            }
        else
            {
            switch (shr(field[NodeIndex] , 16))
                {
                case 2 :
                    switch (field[NodeIndex] & 65535)
                        {
                        case 42 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "RtrnOp");
                                sql__writeln(ptoc_Var98);
                                }
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "return;");
                            break;
                        case 101 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "BreakOp");
                                sql__writeln(ptoc_Var98);
                                }
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "break;");
                            break;
                        case 86 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "ContinueOp");
                                sql__writeln(ptoc_Var98);
                                }
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "continue;");
                            break;
                        case 87 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "GotoOp");
                                sql__writeln(ptoc_Var98);
                                }
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "goto ");
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "LAB_");
                            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%c" , 59);
                            break;
                        case 119 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "BeginOp");
                                sql__writeln(ptoc_Var98);
                                }
                            BLOCK_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc , LastNode_GenC(_ptoc_LOCAL._ptoc_LINK, Node));
                            break;
                        case 98 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "LabelOp");
                                sql__writeln(ptoc_Var98);
                                }
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "LAB_");
                            PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , " :");
                            if (EmptyStatement_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 3]))
                                {
                                CaseEmptyStatement_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 3] , inc);
                                }
                            else
                                {
                                STATEMENT_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 3] , inc);
                                }
                            break;
                        case 120 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "EndOp");
                                sql__writeln(ptoc_Var98);
                                }
                            break;
                        case 106 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "AsmOp");
                                sql__writeln(ptoc_Var98);
                                }
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "asm(\"");
                            DumpAsmString_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "\");");
                            break;
                        default :
                            Error_GenC(_ptoc_LOCAL._ptoc_LINK, inc , NodeIndex);
                            break;
                        }
                    break;
                case 7 :
                    switch (field[NodeIndex] & 65535)
                        {
                        case 43 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "RtrnxOp");
                                sql__writeln(ptoc_Var98);
                                }
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "return(");
                            EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 4 , 0);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , ");");
                            break;
                        default :
                            Error_GenC(_ptoc_LOCAL._ptoc_LINK, inc , NodeIndex);
                            break;
                        }
                    break;
                case 8 :
                    if (control[81])
                        {
                        ptoc_Var98 = sql__ut(output);
                        fprintf(ptoc_Var98->_fbuf , "%s" , "ifthenelse");
                        sql__writeln(ptoc_Var98);
                        }
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%s" , "if (");
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 4 , 0);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%c" , 41);
                    BLOCK_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , inc + 4 , -1);
                    if (field[NodeIndex + 7] != -1)
                        {
                        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                        ptoc_Var98 = sql__ut(outfile);
                        fprintf(ptoc_Var98->_fbuf , "%s" , "else");
                        BLOCK_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7] , inc + 4 , -1);
                        }
                    break;
                case 10 :
                    if (control[81])
                        {
                        ptoc_Var98 = sql__ut(output);
                        fprintf(ptoc_Var98->_fbuf , "%s" , "Switch");
                        sql__writeln(ptoc_Var98);
                        }
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%s" , "switch (");
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 4 , 0);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%c" , 41);
                    CASE_STATEMENT_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , inc + 4);
                    break;
                case 12 :
                    if (control[81])
                        {
                        ptoc_Var98 = sql__ut(output);
                        fprintf(ptoc_Var98->_fbuf , "%s" , "RepeatUntil");
                        sql__writeln(ptoc_Var98);
                        }
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%s" , "do");
                    statement_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , inc + 4);
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%s" , "while (!");
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 28 , 2);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%s" , ");");
                    break;
                case 9 :
                    if (control[81])
                        {
                        ptoc_Var98 = sql__ut(output);
                        fprintf(ptoc_Var98->_fbuf , "%s" , "loop");
                        sql__writeln(ptoc_Var98);
                        }
                    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%s" , "while (");
                    EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 4 , 0);
                    ptoc_Var98 = sql__ut(outfile);
                    fprintf(ptoc_Var98->_fbuf , "%c" , 41);
                    statement_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , inc + 4);
                    break;
                case 11 :
                    switch (field[NodeIndex] & 65535)
                        {
                        case 99 :
                            if (control[81])
                                {
                                ptoc_Var98 = sql__ut(output);
                                fprintf(ptoc_Var98->_fbuf , "%s" , "CaseOp");
                                sql__writeln(ptoc_Var98);
                                }
                            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
                            ptoc_Var98 = sql__ut(outfile);
                            fprintf(ptoc_Var98->_fbuf , "%s" , "case ");
                            if (field[NodeIndex + 5] != field[NodeIndex + 6])
                                {
                                Fehler_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                                }
                            else
                                {
                                ptoc_Var98 = sql__ut(outfile);
                                fprintf(ptoc_Var98->_fbuf , "%*d%s" , 1 , field[NodeIndex + 5] , " :");
                                }
                            break;
                        default :
                            Error_GenC(_ptoc_LOCAL._ptoc_LINK, inc , NodeIndex);
                            break;
                        }
                    break;
                default :
                    Error_GenC(_ptoc_LOCAL._ptoc_LINK, inc , NodeIndex);
                    break;
                }
            }
        }
    }

struct _ptoc_FRAME_IsFunctionVar_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsFunctionVar_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsFunctionVar_GenC _ptoc_LOCAL;
    int _IsFunctionVar;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsFunctionVar = STab[Idx].SKind == 9 && STab[Idx].SLevel == 2 && STab[Idx].KINDTYPE.C_ModuleDcl.SClass == 4;
    return(_IsFunctionVar);
    }

struct _ptoc_FRAME_ComplReturnStmnt_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ComplReturnStmnt_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_ComplReturnStmnt_GenC _ptoc_LOCAL;
    int _ComplReturnStmnt;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    if (empty_statement_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex))
        {
        _ComplReturnStmnt = 0;
        }
    else
        {
        if (shr(field[NodeIndex] , 16) != 7 || (field[NodeIndex] & 65535) != 43)
            {
            _ComplReturnStmnt = 0;
            }
        else
            {
            NodeIndex = field[NodeIndex + 6];
            if (shr(field[NodeIndex] , 16) != 4 || (field[NodeIndex] & 65535) != 13)
                {
                _ComplReturnStmnt = 0;
                }
            else
                {
                NodeIndex = field[NodeIndex + 5];
                if (shr(field[NodeIndex] , 16) != 1 || (field[NodeIndex] & 65535) != 94)
                    {
                    _ComplReturnStmnt = 0;
                    }
                else
                    {
                    if (!IsFunctionVar_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]))
                        {
                        _ComplReturnStmnt = 0;
                        }
                    else
                        {
                        _ComplReturnStmnt = 1;
                        }
                    }
                }
            }
        }
    return(_ComplReturnStmnt);
    }

struct _ptoc_FRAME_SpecFileStmnt_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int SpecFileStmnt_GenC(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_SpecFileStmnt_GenC _ptoc_LOCAL;
    int _SpecFileStmnt;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    _SpecFileStmnt = shr(field[NodeIndex] , 16) == 2 && (field[NodeIndex] & 65535) == 105;
    return(_SpecFileStmnt);
    }

struct _ptoc_FRAME_STATEMENT_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int STATEMENT_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int inc;
    {
    struct _ptoc_FRAME_STATEMENT_GenC _ptoc_LOCAL;
    int NodeIndex;
    _iorechd *ptoc_Var99;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var99 = sql__ut(output);
        fprintf(ptoc_Var99->_fbuf , "%*.*s" , 30 , 22 , "----------STATEMENT : ");
        }
    if (ComplReturnStmnt_GenC(_ptoc_LOCAL._ptoc_LINK, Node) || SpecFileStmnt_GenC(_ptoc_LOCAL._ptoc_LINK, Node))
        {
        if (control[81])
            {
            ptoc_Var99 = sql__ut(output);
            fprintf(ptoc_Var99->_fbuf , "%s" , "ComplReturnStmnt or SpecFileStmnt");
            sql__writeln(ptoc_Var99);
            }
        }
    else
        {
        if (empty_statement_GenC(_ptoc_LOCAL._ptoc_LINK, Node))
            {
            if (control[81])
                {
                ptoc_Var99 = sql__ut(output);
                fprintf(ptoc_Var99->_fbuf , "%s" , "empty_statemennt -> ANWEISUNG : ");
                }
            ANWEISUNG_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
            }
        else
            {
            NodeIndex = Node;
            if (ComplReturnStmnt_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 3]))
                {
                if (control[81])
                    {
                    ptoc_Var99 = sql__ut(output);
                    fprintf(ptoc_Var99->_fbuf , "%s" , "ComplReturnStmnt");
                    sql__writeln(ptoc_Var99);
                    }
                indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , field[NodeIndex + 3] , inc);
                ptoc_Var99 = sql__ut(outfile);
                fprintf(ptoc_Var99->_fbuf , "%s" , "return(");
                EXPRESSION_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 4 , 0);
                ptoc_Var99 = sql__ut(outfile);
                fprintf(ptoc_Var99->_fbuf , "%s" , ");");
                }
            else
                {
                if (IsArrayAssign_GenC(_ptoc_LOCAL._ptoc_LINK, Node))
                    {
                    if (control[81])
                        {
                        ptoc_Var99 = sql__ut(output);
                        fprintf(ptoc_Var99->_fbuf , "%s" , "IsArrayAssign -> ArrayAssign");
                        sql__writeln(ptoc_Var99);
                        }
                    ArrayAssign_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
                    }
                else
                    {
                    if (control[81])
                        {
                        ptoc_Var99 = sql__ut(output);
                        fprintf(ptoc_Var99->_fbuf , "%s" , "ANWEISUNG : ");
                        }
                    ANWEISUNG_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
                    }
                }
            }
        }
    }

struct _ptoc_FRAME_Define_LOCAL_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int Define_LOCAL_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
int inc;
    {
    struct _ptoc_FRAME_Define_LOCAL_GenC _ptoc_LOCAL;
    int NodeIndex;
    int Idx;
    _iorechd *ptoc_Var100;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    Idx = field[NodeIndex + 5];
    Idx = STab[Idx].KINDTYPE.C_BlockDcl.SFuncBlock;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
    ptoc_Var100 = sql__ut(outfile);
    fprintf(ptoc_Var100->_fbuf , "%s" , "struct _ptoc_FRAME_");
    Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 1 , 0);
    ptoc_Var100 = sql__ut(outfile);
    fprintf(ptoc_Var100->_fbuf , "%s" , " _ptoc_LOCAL;");
    }

struct _ptoc_FRAME_CopySingleParam_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int CopySingleParam_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_CopySingleParam_GenC _ptoc_LOCAL;
    int Idx1;
    _iorechd *ptoc_Var101;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    ptoc_Var101 = sql__ut(outfile);
    fprintf(ptoc_Var101->_fbuf , "%s" , "_ptoc_LOCAL.");
    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    ptoc_Var101 = sql__ut(outfile);
    fprintf(ptoc_Var101->_fbuf , "%s" , " = ");
    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    ptoc_Var101 = sql__ut(outfile);
    fprintf(ptoc_Var101->_fbuf , "%c" , 59);
    }

struct _ptoc_FRAME_CopyParams_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int CopyParams_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
int inc;
    {
    struct _ptoc_FRAME_CopyParams_GenC _ptoc_LOCAL;
    int NodeIndex;
    int Idx;
    int i;
    int paranz;
    int level;
    _iorechd *ptoc_Var102;
    int ptoc_Var103;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    Idx = field[NodeIndex + 5];
    Idx = STab[Idx].KINDTYPE.C_BlockDcl.SFuncBlock;
    level = STab[Idx].SLevel;
    Idx = STab[Idx].SType;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    if (level > 1)
        {
        ptoc_Var102 = sql__ut(outfile);
        fprintf(ptoc_Var102->_fbuf , "%s" , "_ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;");
        }
    paranz = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
    ptoc_Var103 = paranz;
    if (1 <= ptoc_Var103)
        {
        i = 1;
        do
            {
            Idx = STab[Idx].SNext;
            switch (STab[Idx].SKind)
                {
                case 14 :
                case 19 :
                case 20 :
                    if (IsInFrameParam_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                        {
                        CopySingleParam_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
                        }
                    break;
                default :
                    break;
                }
            if (i == ptoc_Var103)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_CopySingleValueArray_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int CopySingleValueArray_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_CopySingleValueArray_GenC _ptoc_LOCAL;
    int Idx1;
    _iorechd *ptoc_Var104;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    ptoc_Var104 = sql__ut(outfile);
    fprintf(ptoc_Var104->_fbuf , "%s" , "_ptoc_MOVE(char, sizeof(");
    TYPE_NAME_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 0);
    ptoc_Var104 = sql__ut(outfile);
    fprintf(ptoc_Var104->_fbuf , "%s" , "), ");
    Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 1 , 0);
    ptoc_Var104 = sql__ut(outfile);
    fprintf(ptoc_Var104->_fbuf , "%s" , ", ptoc_ptr_");
    PrintString_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    ptoc_Var104 = sql__ut(outfile);
    fprintf(ptoc_Var104->_fbuf , "%s" , ");");
    }

struct _ptoc_FRAME_CopyValueArrays_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int CopyValueArrays_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
int inc;
    {
    struct _ptoc_FRAME_CopyValueArrays_GenC _ptoc_LOCAL;
    int NodeIndex;
    int Idx;
    int i;
    int paranz;
    int ptoc_Var105;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    Idx = field[NodeIndex + 5];
    Idx = STab[Idx].KINDTYPE.C_BlockDcl.SFuncBlock;
    Idx = STab[Idx].SType;
    paranz = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
    ptoc_Var105 = paranz;
    if (1 <= ptoc_Var105)
        {
        i = 1;
        do
            {
            Idx = STab[Idx].SNext;
            switch (STab[Idx].SKind)
                {
                case 14 :
                case 19 :
                case 20 :
                    if (IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                        {
                        CopySingleValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
                        }
                    break;
                default :
                    break;
                }
            if (i == ptoc_Var105)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_DefineSingleValueArray_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DefineSingleValueArray_GenC(_ptoc_LINK, Idx, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
int inc;
    {
    struct _ptoc_FRAME_DefineSingleValueArray_GenC _ptoc_LOCAL;
    int Idx1;
    _iorechd *ptoc_Var106;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx , inc);
    TYPE_NAME_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 0);
    Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 1 , 0);
    ptoc_Var106 = sql__ut(outfile);
    fprintf(ptoc_Var106->_fbuf , "%c" , 59);
    }

struct _ptoc_FRAME_DefineValueArrays_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DefineValueArrays_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int inc;
    {
    struct _ptoc_FRAME_DefineValueArrays_GenC _ptoc_LOCAL;
    int NodeIndex;
    int Idx;
    int i;
    int paranz;
    int ptoc_Var107;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    Idx = field[NodeIndex + 5];
    Idx = STab[Idx].KINDTYPE.C_BlockDcl.SFuncBlock;
    Idx = STab[Idx].SType;
    paranz = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SParmCnt;
    ptoc_Var107 = paranz;
    if (1 <= ptoc_Var107)
        {
        i = 1;
        do
            {
            Idx = STab[Idx].SNext;
            switch (STab[Idx].SKind)
                {
                case 14 :
                case 19 :
                case 20 :
                    if (IsValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
                        {
                        DefineSingleValueArray_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , inc);
                        }
                    break;
                default :
                    break;
                }
            if (i == ptoc_Var107)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_BLOCK_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int BLOCK_GenC(_ptoc_LINK, Node, inc, last)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int inc;
int last;
    {
    struct _ptoc_FRAME_BLOCK_GenC _ptoc_LOCAL;
    HdgRange NodeIndex;
    _iorechd *ptoc_Var108;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
    ptoc_Var108 = sql__ut(outfile);
    fprintf(ptoc_Var108->_fbuf , "%c" , 123);
    NodeIndex = Node;
    if (shr(field[NodeIndex] , 16) == 2 && (field[NodeIndex] & 65535) == 119)
        {
        if (_ptoc_LOCAL._ptoc_LINK->StatLink)
            {
            if (CurrentLevel > 0)
                {
                Define_LOCAL_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
                Local_Definitions_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , inc , 2);
                if (DelayedParCopy())
                    {
                    DefineValueArrays_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
                    }
                CopyParams_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
                }
            }
        else
            {
            Local_Definitions_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , inc , 2);
            if (DelayedParCopy())
                {
                DefineValueArrays_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
                }
            }
        if (DelayedParCopy())
            {
            CopyValueArrays_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
            }
        NodeIndex = field[NodeIndex + 3];
        }
    while (NodeIndex != last)
        {
        STATEMENT_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex , inc);
        NodeIndex = next_statement_GenC(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
        }
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
    ptoc_Var108 = sql__ut(outfile);
    fprintf(ptoc_Var108->_fbuf , "%c" , 125);
    }

struct _ptoc_FRAME_DummyDekl_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DummyDekl_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
int inc;
    {
    struct _ptoc_FRAME_DummyDekl_GenC _ptoc_LOCAL;
    _iorechd *ptoc_Var109;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
    ptoc_Var109 = sql__ut(outfile);
    fprintf(ptoc_Var109->_fbuf , "%s" , "char dummy;");
    }

struct _ptoc_FRAME_DefineBlock_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DefineBlock_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Node;
int inc;
    {
    struct _ptoc_FRAME_DefineBlock_GenC _ptoc_LOCAL;
    int NodeIndex;
    int Idx;
    _iorechd *ptoc_Var110;
    int ptoc_Var111;
    int ptoc_Var112;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    if (shr(field[NodeIndex] , 16) != 13)
        {
        ptoc_Var110 = sql__ut(outfile);
        fprintf(ptoc_Var110->_fbuf , "%s" , "Fehler in DefineBlock!");
        sql__writeln(ptoc_Var110);
        Abbruch_GenC(_ptoc_LOCAL._ptoc_LINK);
        }
    Idx = field[NodeIndex + 7];
    NodeIndex = field[NodeIndex + 5];
    if (shr(field[NodeIndex] , 16) == 2 && (field[NodeIndex] & 65535) == 119)
        {
        Space_GenC(_ptoc_LOCAL._ptoc_LINK);
        if (_ptoc_LOCAL._ptoc_LINK->StatLink)
            {
            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , inc);
            ptoc_Var110 = sql__ut(outfile);
            fprintf(ptoc_Var110->_fbuf , "%s" , "struct _ptoc_FRAME_");
            Identifier_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , 1 , 0);
            ptoc_Var111 = inc + 2;
            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , ptoc_Var111);
            ptoc_Var110 = sql__ut(outfile);
            fprintf(ptoc_Var110->_fbuf , "%c" , 123);
            DeklFormPar_GenC(_ptoc_LOCAL._ptoc_LINK, Idx , ptoc_Var111 , 1);
            Frame_Definitions_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , inc + 2);
            ptoc_Var112 = inc + 2;
            DummyDekl_GenC(_ptoc_LOCAL._ptoc_LINK, Node , ptoc_Var112);
            indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , ptoc_Var112);
            ptoc_Var110 = sql__ut(outfile);
            fprintf(ptoc_Var110->_fbuf , "%s" , "} ;");
            }
        Outside_Definitions_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , inc);
        }
    }

struct _ptoc_FRAME_NextEntry_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int NextEntry_GenC(_ptoc_LINK, Idx0)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx0;
    {
    struct _ptoc_FRAME_NextEntry_GenC _ptoc_LOCAL;
    int _NextEntry;
    int Idx;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx = Idx0;
    switch (STab[Idx].SKind)
        {
        case 6 :
        case 5 :
        case 4 :
        case 3 :
            _NextEntry = STab[STab[STab[STab[Idx].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock].KINDTYPE.C_BlockDcl.SLastEntry].SNext;
            break;
        default :
            _NextEntry = STab[Idx].SNext;
            break;
        }
    return(_NextEntry);
    }

struct _ptoc_FRAME_InsideFunctions_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int InsideFunctions_GenC(_ptoc_LINK, Idx0, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx0;
int inc;
    {
    struct _ptoc_FRAME_InsideFunctions_GenC _ptoc_LOCAL;
    int Idx;
    int last;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx = Idx0;
    Idx = STab[Idx].SType;
    Idx = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock;
    last = STab[Idx].KINDTYPE.C_BlockDcl.SLastEntry;
    Idx = NextEntry_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    while (Idx < last)
        {
        if (IsFuncDcl_GenC(_ptoc_LOCAL._ptoc_LINK, Idx))
            {
            FUNCTION_DEFINITION_GenC(_ptoc_LOCAL._ptoc_LINK, GetNode_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) , inc);
            }
        Idx = NextEntry_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
        }
    }

struct _ptoc_FRAME_FUNCTION_DEFINITION_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int FUNCTION_DEFINITION_GenC(_ptoc_LINK, Node, inc)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
HdgRange Node;
int inc;
    {
    struct _ptoc_FRAME_FUNCTION_DEFINITION_GenC _ptoc_LOCAL;
    HdgRange NodeIndex;
    int Idx1;
    int Idx2;
    int Idx3;
    int FunctionType;
    int tn;
    _iorechd *ptoc_Var113;
    int ptoc_Var114;
    int ptoc_Var115;
    int ptoc_Var116;
    int ptoc_Var117;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[89])
        {
        ptoc_Var113 = sql__ut(outfile);
        fprintf(ptoc_Var113->_fbuf , "");
        sql__writeln(ptoc_Var113);
        ptoc_Var113 = sql__ut(outfile);
        fprintf(ptoc_Var113->_fbuf , "");
        sql__writeln(ptoc_Var113);
        ptoc_Var113 = sql__ut(outfile);
        fprintf(ptoc_Var113->_fbuf , "%s" , "/*SOFU*/");
        sql__writeln(ptoc_Var113);
        }
    NodeIndex = Node;
    ptoc_Var116 = field[NodeIndex + 7];
    CurrentProc = ptoc_Var116;
    CurrentLevel = STab[ptoc_Var116].SLevel;
    if (STab[ptoc_Var116].SOrder > 0)
        {
        ptoc_Var114 = field[NodeIndex + 7];
        STab[ptoc_Var114].SName = STab[STab[ptoc_Var114].SOrder].SName;
        }
    DefineBlock_GenC(_ptoc_LOCAL._ptoc_LINK, Node , inc);
    Space_GenC(_ptoc_LOCAL._ptoc_LINK);
    InsideFunctions_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7] , inc);
    Space_GenC(_ptoc_LOCAL._ptoc_LINK);
    ptoc_Var117 = field[NodeIndex + 7];
    CurrentProc = ptoc_Var117;
    CurrentLevel = STab[ptoc_Var117].SLevel;
    Idx1 = ptoc_Var117;
    if (STab[Idx1].KINDTYPE.C_ModuleDcl.SClass == 3)
        {
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx1 , inc);
        ptoc_Var113 = sql__ut(outfile);
        fprintf(ptoc_Var113->_fbuf , "%s" , "static");
        }
    Idx2 = STab[Idx1].SType;
    Idx3 = STab[Idx2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult;
    if (control[97] && STab[Idx2].SType == -23)
        {
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 1 , Idx1 , inc);
        ptoc_Var113 = sql__ut(outfile);
        fprintf(ptoc_Var113->_fbuf , "%s" , "void ");
        }
    else
        {
        tn = TYPE_SPECIFIER_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , Idx3 , 0 , inc);
        }
    FUNC_DEC_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , tn);
    if (!control[97])
        {
        DeklFormPar_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1 , inc , 0);
        }
    if (field[NodeIndex + 6] > 0)
        {
        ptoc_Var114 = inc + 4;
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , ptoc_Var114);
        ptoc_Var113 = sql__ut(outfile);
        fprintf(ptoc_Var113->_fbuf , "%c" , 123);
        ptoc_Var115 = inc + 8;
        BLOCK_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , ptoc_Var115 , -1);
        BLOCK_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , ptoc_Var115 , -1);
        indent_GenC(_ptoc_LOCAL._ptoc_LINK, 2 , Node , ptoc_Var114);
        ptoc_Var113 = sql__ut(outfile);
        fprintf(ptoc_Var113->_fbuf , "%c" , 125);
        }
    else
        {
        BLOCK_GenC(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , inc + 4 , -1);
        }
    }

struct _ptoc_FRAME_ComplexType_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int ComplexType_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_ComplexType_GenC _ptoc_LOCAL;
    int _ComplexType;
    int typ;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    typ = STab[Idx].SType;
    _ComplexType = typ == -14 || typ == -19 || typ == -10;
    return(_ComplexType);
    }

struct _ptoc_FRAME_DifferentLevels_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int DifferentLevels_GenC(_ptoc_LINK, Idx1, Idx2)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx1;
int Idx2;
    {
    struct _ptoc_FRAME_DifferentLevels_GenC _ptoc_LOCAL;
    int _DifferentLevels;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _DifferentLevels = STab[Idx1].SLevel != STab[Idx2].SLevel;
    return(_DifferentLevels);
    }

struct _ptoc_FRAME_IsCTagName_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int IsCTagName_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_IsCTagName_GenC _ptoc_LOCAL;
    int _IsCTagName;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsCTagName = 0;
    return(_IsCTagName);
    }

struct _ptoc_FRAME_single_TypeNameDcl_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int single_TypeNameDcl_GenC(_ptoc_LINK, Idx)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_single_TypeNameDcl_GenC _ptoc_LOCAL;
    int Idx1;
    _iorechd *ptoc_Var118;
    struct ptoc_Type1 *ptoc_Var119;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[81])
        {
        ptoc_Var118 = sql__ut(output);
        fprintf(ptoc_Var118->_fbuf , "%*.*s%s%10d" , 30 , 21 , "single_TypeNameDcl : " , "Idx = " , Idx);
        sql__writeln(ptoc_Var118);
        }
    if (STab[Idx].SKind == 16)
        {
        Idx1 = STab[Idx].SType;
        ptoc_Var119 = &STab[Idx1];
        if (ptoc_Var119->SKind == 7)
            {
            if (control[81])
                {
                ptoc_Var118 = sql__ut(output);
                fprintf(ptoc_Var118->_fbuf , "%*.*s%s" , 30 , 21 , "single_TypeNameDcl : " , "SKind = TypeDcl : ");
                DumpDumpString_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx].SName);
                ptoc_Var118 = sql__ut(output);
                fprintf(ptoc_Var118->_fbuf , "%s" , "  (");
                DumpDumpString_GenC(_ptoc_LOCAL._ptoc_LINK, STab[Idx1].SName);
                ptoc_Var118 = sql__ut(output);
                fprintf(ptoc_Var118->_fbuf , "%c" , 41);
                sql__writeln(ptoc_Var118);
                }
            if (ComplexType_GenC(_ptoc_LOCAL._ptoc_LINK, Idx1))
                {
                if (control[81])
                    {
                    ptoc_Var118 = sql__ut(output);
                    fprintf(ptoc_Var118->_fbuf , "%*.*s%s" , 30 , 21 , "single_TypeNameDcl : " , "ComplexType");
                    sql__writeln(ptoc_Var118);
                    }
                if (IsCTagName_GenC(_ptoc_LOCAL._ptoc_LINK, Idx) || Idx < Idx1 && ptoc_Var119->STmyname == 0)
                    {
                    if (control[81])
                        {
                        ptoc_Var118 = sql__ut(output);
                        fprintf(ptoc_Var118->_fbuf , "%*.*s%s" , 30 , 21 , "single_TypeNameDcl : " , "STab[Idx].SKind := TagNameDcl");
                        sql__writeln(ptoc_Var118);
                        }
                    STab[Idx].SKind = 23;
                    }
                if (ptoc_Var119->SLevel == 0)
                    {
                    setbit(&STab[Idx].SFeld4 , 4);
                    }
                }
            if (ptoc_Var119->STmyname <= 0)
                {
                ptoc_Var119->STmyname = Idx;
                }
            }
        }
    }

struct _ptoc_FRAME_init_SymtabExtensions_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int init_SymtabExtensions_GenC(_ptoc_LINK, from_stab, to_stab)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int from_stab;
int to_stab;
    {
    struct _ptoc_FRAME_init_SymtabExtensions_GenC _ptoc_LOCAL;
    int Idx;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    Idx = from_stab;
    single_TypeNameDcl_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
    while (Idx != to_stab)
        {
        Idx = STab[Idx].SNext;
        single_TypeNameDcl_GenC(_ptoc_LOCAL._ptoc_LINK, Idx);
        }
    }

struct _ptoc_FRAME_STabMark_GenC
  {
  struct _ptoc_FRAME_GenC *_ptoc_LINK;
  char dummy;
  } ;


static
int STabMark_GenC(_ptoc_LINK, from_stab, to_stab)
struct _ptoc_FRAME_GenC *_ptoc_LINK;
int from_stab;
int to_stab;
    {
    struct _ptoc_FRAME_STabMark_GenC _ptoc_LOCAL;
    int i;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    i = from_stab;
    while (i <= SymPtr && STab[i].SLevel < 1)
        i = STab[i].SNext;
    while (i <= to_stab)
        {
        switch (STab[i].SKind)
            {
            case 9 :
                if (DeleteUnusedType())
                    {
                    if (DeleteUnusedVar())
                        {
                        if (!IsMarked(i) && STab[i].SLevel == 1)
                            {
                            }
                        else
                            {
                            markT(TypeOfVar(i));
                            }
                        }
                    else
                        {
                        markT(TypeOfVar(i));
                        }
                    }
                else
                    {
                    markT(TypeOfVar(i));
                    }
                break;
            case 14 :
                markT(TypeOfVar(i));
                break;
            case 4 :
                markT(STab[STab[i].SType].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SResult);
                break;
            default :
                break;
            }
        i = STab[i].SNext;
        }
    }

int GenC(Node, from_stab, to_stab, enc_block, DFlag, MaxPos, StatLink)
int Node;
int from_stab;
int to_stab;
int enc_block;
int DFlag;
int MaxPos;
int StatLink;
    {
    struct _ptoc_FRAME_GenC _ptoc_LOCAL;
    int NestLevel;
    int save1;
    int save2;
    
    _ptoc_LOCAL.Node = Node;
    _ptoc_LOCAL.to_stab = to_stab;
    _ptoc_LOCAL.MaxPos = MaxPos;
    _ptoc_LOCAL.StatLink = StatLink;
    init_SymtabExtensions_GenC(&_ptoc_LOCAL, from_stab , _ptoc_LOCAL.to_stab);
    Ref_STab_datafile();
    GenTyp(_ptoc_LOCAL.Node);
    GenIfTyp(_ptoc_LOCAL.Node);
    HdgMark(_ptoc_LOCAL.Node);
    STabMark_GenC(&_ptoc_LOCAL, from_stab , _ptoc_LOCAL.to_stab);
    if (enc_block > 0)
        {
        Global_Definitions_GenC(&_ptoc_LOCAL, STab[enc_block].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock , 0 , 1);
        FUNCTION_DEFINITION_GenC(&_ptoc_LOCAL, GetNode_GenC(&_ptoc_LOCAL, STab[STab[enc_block].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ProcedureType.SBlock].KINDTYPE.C_BlockDcl.SFuncBlock) , 0);
        }
    else
        {
        Modul_Definitions_GenC(&_ptoc_LOCAL, from_stab , 0 , 1);
        }
    Space_GenC(&_ptoc_LOCAL);
    }

struct _ptoc_FRAME_gen_idx_ptr
  {
  char dummy;
  } ;


int gen_idx_ptr()
    {
    struct _ptoc_FRAME_gen_idx_ptr _ptoc_LOCAL;
    
    BumpSymPtr();
    STab[SymPtr].SKind = 7;
    STab[SymPtr].SType = -17;
    IDX_PTR = SymPtr;
    }

struct _ptoc_FRAME_gen_idx_data
  {
  char dummy;
  } ;


int gen_idx_data()
    {
    struct _ptoc_FRAME_gen_idx_data _ptoc_LOCAL;
    STRING s;
    
    BumpSymPtr();
    _ptoc_ASSIGN(unsigned char , 256, s, "data                                                                                                                                                                                                                                                            ")
    STab[SymPtr].SName = EnterString(&s[0] , 4);
    STab[SymPtr].SKind = 11;
    IDX_DATA = SymPtr;
    }

struct _ptoc_FRAME_InitGenC
  {
  char dummy;
  } ;


int InitGenC()
    {
    struct _ptoc_FRAME_InitGenC _ptoc_LOCAL;
    _iorechd *ptoc_Var120;
    
    if (control[81])
        {
        version_writing();
        }
    sql__rewrite(outfile , 0 , 0 , 0);
    rediroutput(outfile);
    ptoc_Var120 = sql__ut(outfile);
    fprintf(ptoc_Var120->_fbuf , "%s" , "#include <ptoc.h>");
    sql__writeln(ptoc_Var120);
    if (control[68])
        {
        DumpFlag = 1;
        }
    else
        {
        DumpFlag = 0;
        }
    gen_idx_ptr();
    gen_idx_data();
    nummer = 0;
    CurrentProc = 0;
    CurrentLevel = 0;
    }
