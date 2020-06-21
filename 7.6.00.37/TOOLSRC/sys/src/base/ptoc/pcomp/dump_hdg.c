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
int control2[256];
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
double double_to_double();
long double_to_long();
int DumptreeI();
int ListHdg();
typedef
  unsigned char String20[20];
String20 opcodes[120];
int NestLevel;

struct _ptoc_FRAME_DumptreeI
  {
  char dummy;
  } ;


int DumptreeI()
    {
    struct _ptoc_FRAME_DumptreeI _ptoc_LOCAL;
    int i;
    int j;
    
    NestLevel = 0;
    i = 1;
    do
        {
        _ptoc_ASSIGN(unsigned char , 20, opcodes[i + -1], "<<unknown>>         ")
        opcodes[i + -1][11] = (unsigned char)'\0';
        if (i == 120)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    _ptoc_ASSIGN(unsigned char , 20, opcodes[0], "nop                 ")
    opcodes[0][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[2], "lno                 ")
    opcodes[2][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[3], "load                ")
    opcodes[3][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[5], "store               ")
    opcodes[5][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[7], "storep              ")
    opcodes[7][6] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[8], "storepadr           ")
    opcodes[8][9] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[11], "lit                 ")
    opcodes[11][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[12], "loadadr             ")
    opcodes[12][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[13], "loadstr             ")
    opcodes[13][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[18], "add                 ")
    opcodes[18][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[19], "subt                ")
    opcodes[19][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[20], "mult                ")
    opcodes[20][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[21], "div                 ")
    opcodes[21][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[22], "rnge                ")
    opcodes[22][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[23], "uminus              ")
    opcodes[23][6] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[24], "or                  ")
    opcodes[24][2] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[25], "xor                 ")
    opcodes[25][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[26], "and                 ")
    opcodes[26][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[27], "not                 ")
    opcodes[27][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[28], "shl                 ")
    opcodes[28][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[29], "shr                 ")
    opcodes[29][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[30], "eq                  ")
    opcodes[30][2] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[31], "lt                  ")
    opcodes[31][2] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[32], "gt                  ")
    opcodes[32][2] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[33], "le                  ")
    opcodes[33][2] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[34], "ge                  ")
    opcodes[34][2] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[35], "ne                  ")
    opcodes[35][2] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[39], "call                ")
    opcodes[39][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[41], "rtrn                ")
    opcodes[41][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[42], "rtrnx               ")
    opcodes[42][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[57], "proc                ")
    opcodes[57][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[61], "sys0                ")
    opcodes[61][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[68], "LitSize             ")
    opcodes[68][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[69], "LitOffset           ")
    opcodes[69][9] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[75], "addi                ")
    opcodes[75][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[76], "subti               ")
    opcodes[76][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[81], "addix               ")
    opcodes[81][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[82], "subtix              ")
    opcodes[82][6] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[83], "multx               ")
    opcodes[83][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[85], "continue            ")
    opcodes[85][8] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[86], "goto                ")
    opcodes[86][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[87], "mod                 ")
    opcodes[87][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[88], "b_and               ")
    opcodes[88][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[89], "b_or                ")
    opcodes[89][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[90], "b_xor               ")
    opcodes[90][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[91], "b_not               ")
    opcodes[91][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[92], "convert             ")
    opcodes[92][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[93], "id                  ")
    opcodes[93][2] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[94], "subscript           ")
    opcodes[94][9] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[95], "element             ")
    opcodes[95][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[96], "deref               ")
    opcodes[96][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[97], "label               ")
    opcodes[97][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[98], "case                ")
    opcodes[98][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[99], "default             ")
    opcodes[99][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[100], "break               ")
    opcodes[100][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[101], "prefix              ")
    opcodes[101][6] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[102], "postfix             ")
    opcodes[102][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[103], "modulo              ")
    opcodes[103][6] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[104], "file                ")
    opcodes[104][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[105], "asm                 ")
    opcodes[105][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[106], "repeat              ")
    opcodes[106][6] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[107], "complex             ")
    opcodes[107][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[108], "staticlink          ")
    opcodes[108][10] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[109], "reg                 ")
    opcodes[109][3] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[110], "entry               ")
    opcodes[110][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[111], "storeadr            ")
    opcodes[111][8] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[112], "list                ")
    opcodes[112][4] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[113], "comma               ")
    opcodes[113][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[114], "push                ")
    opcodes[114][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[115], "pop                 ")
    opcodes[115][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[116], "pushadr             ")
    opcodes[116][7] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[117], "popadr              ")
    opcodes[117][6] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[118], "begin               ")
    opcodes[118][5] = (unsigned char)'\0';
    _ptoc_ASSIGN(unsigned char , 20, opcodes[119], "end                 ")
    opcodes[119][3] = (unsigned char)'\0';
    }

struct _ptoc_FRAME_indent
  {
  char dummy;
  } ;


int indent(NoBlanks, NodeIndex, ptoc_ptr_str)
int NoBlanks;
int NodeIndex;
String20 ptoc_ptr_str;
    {
    struct _ptoc_FRAME_indent _ptoc_LOCAL;
    int i;
    _iorechd *ptoc_Var0;
    int ptoc_Var1;
    String20 str;
    
    _ptoc_MOVE(char, sizeof(String20 ), str, ptoc_ptr_str);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%c%*d%s" , 40 , 4 , NodeIndex , ") ");
    ptoc_Var1 = NoBlanks;
    if (1 <= ptoc_Var1)
        {
        i = 1;
        do
            {
            ptoc_Var0 = sql__ut(output);
            fprintf(ptoc_Var0->_fbuf , "%c" , 32);
            if (i == ptoc_Var1)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    i = -1;
    do
        i += 1;
    while (!((int ) str[i] == 32 || (int ) str[i] == 0));
    if (i > 0)
        {
        ptoc_Var0 = sql__ut(output);
        ptoc_Var1 = i;
        fprintf(ptoc_Var0->_fbuf , "%*.*s" , ptoc_Var1 , ptoc_Var1 < 20 ? ptoc_Var1 : 20 , str);
        }
    if (control[105] == 0)
        {
        sql__fflush(output);
        }
    }

struct _ptoc_FRAME_PrintName
  {
  char dummy;
  } ;


int PrintName(index)
int index;
    {
    struct _ptoc_FRAME_PrintName _ptoc_LOCAL;
    int i;
    STRING s;
    _iorechd *ptoc_Var2;
    int ptoc_Var3;
    
    GetString(STab[index].SName , &s[0]);
    i = _GetStringLength(STab[index].SName);
    if (i > 0)
        {
        ptoc_Var2 = sql__ut(output);
        ptoc_Var3 = i;
        fprintf(ptoc_Var2->_fbuf , "%*.*s%s%*d%c" , ptoc_Var3 , ptoc_Var3 < 256 ? ptoc_Var3 : 256 , s , " [" , 1 , index , 93);
        }
    else
        {
        ptoc_Var2 = sql__ut(output);
        fprintf(ptoc_Var2->_fbuf , "%s%*d%c" , "name is null [" , 1 , index , 93);
        }
    }

struct _ptoc_FRAME_ListString
  {
  char dummy;
  } ;


int ListString(index)
int index;
    {
    struct _ptoc_FRAME_ListString _ptoc_LOCAL;
    int i;
    STRING s;
    _iorechd *ptoc_Var4;
    int ptoc_Var5;
    
    GetString(index , &s[0]);
    i = _GetStringLength(index);
    if (i > 0)
        {
        ptoc_Var4 = sql__ut(output);
        ptoc_Var5 = i;
        fprintf(ptoc_Var4->_fbuf , "%*.*s%s%*d%c" , ptoc_Var5 , ptoc_Var5 < 256 ? ptoc_Var5 : 256 , s , " [" , 1 , index , 93);
        }
    else
        {
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "%s%*d%c" , "string is null [" , 1 , index , 93);
        }
    }

struct _ptoc_FRAME_LstCode
  {
  char dummy;
  } ;


int LstCode(code, NodeIndex, parm1, parm2)
int code;
int NodeIndex;
int parm1;
int parm2;
    {
    struct _ptoc_FRAME_LstCode _ptoc_LOCAL;
    STRING s;
    _iorechd *ptoc_Var6;
    int ptoc_Var7;
    
    if (code == 3)
        {
        indent(NestLevel , NodeIndex , opcodes[code + -1]);
        }
    else
        {
        ptoc_Var6 = sql__ut(output);
        ptoc_Var7 = sql__strlen(&opcodes[code + -1][0]);
        fprintf(ptoc_Var6->_fbuf , "%c%*.*s%s" , 45 , ptoc_Var7 , ptoc_Var7 < 20 ? ptoc_Var7 : 20 , opcodes[code + -1] , ":  ");
        }
    switch (code)
        {
        case 3 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%c%*d" , 32 , 1 , parm1);
            break;
        case 69 :
        case 70 :
        case 94 :
            PrintName(parm1);
            break;
        case 14 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s" , " (string) ");
            ListString(parm1);
            break;
        case 12 :
            switch (STab[field[NodeIndex + 1]].SType)
                {
                case -17 :
                case -5 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%c%*d%s" , 32 , 1 , parm1 , " (integer)");
                    break;
                case -9 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%c%*c%s%*d%s" , 32 , 1 , (int ) (unsigned char ) parm1 , ", " , 1 , parm1 , " (uchar)");
                    break;
                case -3 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%c%*ld%s" , 32 , 1 , double_to_long(parm1 , parm2) , " (long)");
                    break;
                case -4 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%c%*ld%s" , 32 , 1 , double_to_long(parm1 , parm2) , " (unsigned long)");
                    break;
                case -11 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%c%*.*e%s" , 32 , 9 , 1 , double_to_double(parm1 , parm2) , " (float)");
                    break;
                case -12 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%c%*.*e%s" , 32 , 9 , 1 , double_to_double(parm1 , parm2) , " (double)");
                    break;
                case -24 :
                case -10 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%c%*d%s" , 91 , 1 , parm1 , "]  ");
                    break;
                case -7 :
                    if (parm1 == 0)
                        {
                        ptoc_Var6 = sql__ut(output);
                        fprintf(ptoc_Var6->_fbuf , "%s" , " false");
                        }
                    else
                        {
                        ptoc_Var6 = sql__ut(output);
                        fprintf(ptoc_Var6->_fbuf , "%s" , " true");
                        }
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " (boolean)");
                    break;
                case -13 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " (string) ");
                    ListString(parm1);
                    break;
                case 0 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (SubRangeType !!)");
                    break;
                case -1 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (ShortType !!)");
                    break;
                case -2 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (UshortType !!)");
                    break;
                case -6 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (UnsignedType !!)");
                    break;
                case -8 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%c%*c%s%*d%s" , 32 , 1 , (int ) (unsigned char ) parm1 , ", " , 1 , parm1 , " (CharType)");
                    break;
                case -14 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (RecordType !!)");
                    break;
                case -15 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (FileType !!)");
                    break;
                case -16 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (SetType !!)");
                    break;
                case -18 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (FunctionType !!)");
                    break;
                case -19 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (UnionType !!)");
                    break;
                case -20 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (StringType !!)");
                    break;
                case -21 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (VoidType !!)");
                    break;
                case -22 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (BlockType !!)");
                    break;
                case -23 :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (ProcedureType !!)");
                    break;
                default :
                    ptoc_Var6 = sql__ut(output);
                    fprintf(ptoc_Var6->_fbuf , "%s" , " ..... (unknown type !!)");
                    break;
                }
            break;
        case 62 :
            ListString(parm1);
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s%*d" , ", no. parameters = " , 1 , parm2);
            break;
        case 95 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%*d%s%*d" , 1 , parm1 , ", " , 1 , parm2);
            break;
        case 99 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%*d%s%*d" , 1 , parm1 , ", " , 1 , parm2);
            break;
        case 111 :
        case 115 :
        case 116 :
        case 117 :
        case 118 :
        case 87 :
        case 98 :
            PrintName(parm1);
            break;
        case 105 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s" , "  ");
            ListString(parm1);
            break;
        case 106 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s" , "  ");
            ListString(parm1);
            break;
        case 119 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s" , "  ");
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%*d" , 1 , parm1);
            break;
        case 120 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s" , "  ");
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%*d" , 1 , parm1);
            break;
        case 109 :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s%*d" , "  " , 1 , parm1);
            break;
        default :
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%c" , 32);
            break;
        }
    }
int HdgPrint();

struct _ptoc_FRAME_CsgPrint
  {
  char dummy;
  } ;


struct _ptoc_FRAME_PrintType_CsgPrint
  {
  struct _ptoc_FRAME_CsgPrint *_ptoc_LINK;
  char dummy;
  } ;


static
int PrintType_CsgPrint(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_CsgPrint *_ptoc_LINK;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_PrintType_CsgPrint _ptoc_LOCAL;
    int i;
    STRING s;
    _iorechd *ptoc_Var8;
    int ptoc_Var9;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    GetString(STab[field[NodeIndex + 1]].SName , &s[0]);
    i = _GetStringLength(STab[field[NodeIndex + 1]].SName);
    if (i > 0)
        {
        ptoc_Var8 = sql__ut(output);
        ptoc_Var9 = i;
        fprintf(ptoc_Var8->_fbuf , "%s%*.*s%s%*d%c" , ", type = " , ptoc_Var9 , ptoc_Var9 < 256 ? ptoc_Var9 : 256 , s , " [" , 1 , field[NodeIndex + 1] , 93);
        }
    else
        {
        ptoc_Var8 = sql__ut(output);
        fprintf(ptoc_Var8->_fbuf , "%s%*d%c" , ", type = unnamed [" , 1 , field[NodeIndex + 1] , 93);
        }
    }

struct _ptoc_FRAME_PrintDscPtr_CsgPrint
  {
  struct _ptoc_FRAME_CsgPrint *_ptoc_LINK;
  char dummy;
  } ;


static
int PrintDscPtr_CsgPrint(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_CsgPrint *_ptoc_LINK;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_PrintDscPtr_CsgPrint _ptoc_LOCAL;
    _iorechd *ptoc_Var10;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    ptoc_Var10 = sql__ut(output);
    fprintf(ptoc_Var10->_fbuf , "%s%*d%c" , "   {" , 1 , field[NodeIndex + 2] , 125);
    }

int CsgPrint(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CsgPrint _ptoc_LOCAL;
    STRING s;
    HdgRange SaveIndex;
    _iorechd *ptoc_Var11;
    int ptoc_Var12;
    
    if (NodeIndex != -1)
        {
        NestLevel += 3;
        switch (shr(field[NodeIndex] , 16))
            {
            case 1 :
                indent(NestLevel , NodeIndex , "node                ");
                LstCode(field[NodeIndex] & 65535 , NodeIndex , field[NodeIndex + 5] , field[NodeIndex + 6]);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                break;
            case 2 :
                LstCode(3 , NodeIndex , field[NodeIndex + 6] , 0);
                indent(NestLevel , NodeIndex , "spec                ");
                LstCode(field[NodeIndex] & 65535 , NodeIndex , field[NodeIndex + 5] , 0);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                break;
            case 3 :
                LstCode(3 , NodeIndex , field[NodeIndex + 6] , 0);
                indent(NestLevel , NodeIndex , "ShortStmnt          ");
                LstCode(shr(field[NodeIndex] , 16) , NodeIndex , 0 , 0);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                CsgPrint(field[NodeIndex + 5]);
                break;
            case 4 :
                indent(NestLevel , NodeIndex , "UnaryExp            ");
                LstCode(field[NodeIndex] & 65535 , NodeIndex , field[NodeIndex + 6] , field[NodeIndex + 7]);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                CsgPrint(field[NodeIndex + 5]);
                break;
            case 5 :
                indent(NestLevel , NodeIndex , "BinaryExp           ");
                LstCode(field[NodeIndex] & 65535 , NodeIndex , field[NodeIndex + 7] , field[NodeIndex + 8]);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                CsgPrint(field[NodeIndex + 5]);
                CsgPrint(field[NodeIndex + 6]);
                break;
            case 6 :
                indent(NestLevel , NodeIndex , "ExpList             ");
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                SaveIndex = NodeIndex;
                NodeIndex = field[NodeIndex + 5];
                while (NodeIndex != -1)
                    {
                    indent(NestLevel , SaveIndex , "ListElement         ");
                    CsgPrint(NodeIndex);
                    NodeIndex = field[NodeIndex + 3];
                    }
                break;
            case 7 :
                LstCode(3 , NodeIndex , field[NodeIndex + 9] , 0);
                indent(NestLevel , NodeIndex , "LongStmnt           ");
                LstCode(field[NodeIndex] & 65535 , NodeIndex , field[NodeIndex + 7] , field[NodeIndex + 8]);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                if ((field[NodeIndex] & 65535) == 40)
                    {
                    indent(NestLevel , NodeIndex , "Ls1parameter        ");
                    CsgPrint(field[NodeIndex + 7]);
                    }
                indent(NestLevel , NodeIndex , "Ls1operand          ");
                if ((field[NodeIndex] & 65535) != 62)
                    {
                    CsgPrint(field[NodeIndex + 5]);
                    }
                indent(NestLevel , NodeIndex , "Ls2operand          ");
                CsgPrint(field[NodeIndex + 6]);
                break;
            case 8 :
                LstCode(3 , NodeIndex , field[NodeIndex + 8] , 0);
                indent(NestLevel , NodeIndex , "ifthenelse          ");
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                indent(NestLevel , NodeIndex , "ifcond              ");
                CsgPrint(field[NodeIndex + 5]);
                indent(NestLevel , NodeIndex , "iftrue              ");
                HdgPrint(field[NodeIndex + 6]);
                if (field[NodeIndex + 7] != -1)
                    {
                    indent(NestLevel , NodeIndex , "iffalse             ");
                    HdgPrint(field[NodeIndex + 7]);
                    }
                break;
            case 9 :
                LstCode(3 , NodeIndex , field[NodeIndex + 8] , 0);
                indent(NestLevel , NodeIndex , "loop                ");
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                indent(NestLevel , NodeIndex , "whiletest           ");
                CsgPrint(field[NodeIndex + 5]);
                indent(NestLevel , NodeIndex , "whilebody           ");
                HdgPrint(field[NodeIndex + 6]);
                break;
            case 10 :
                LstCode(3 , NodeIndex , field[NodeIndex + 8] , 0);
                indent(NestLevel , NodeIndex , "Switch              ");
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                indent(NestLevel , NodeIndex , "SwitchTest          ");
                CsgPrint(field[NodeIndex + 5]);
                indent(NestLevel , NodeIndex , "SwitchBody          ");
                HdgPrint(field[NodeIndex + 6]);
                break;
            case 11 :
                LstCode(3 , NodeIndex , field[NodeIndex + 9] , 0);
                indent(NestLevel , NodeIndex , "Case                ");
                LstCode(field[NodeIndex] & 65535 , NodeIndex , field[NodeIndex + 5] , field[NodeIndex + 6]);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                break;
            case 12 :
                LstCode(3 , NodeIndex , field[NodeIndex + 8] , 0);
                indent(NestLevel , NodeIndex , "RepeatUntil         ");
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                indent(NestLevel , NodeIndex , "repeatbody          ");
                HdgPrint(field[NodeIndex + 6]);
                indent(NestLevel , NodeIndex , "repeattest          ");
                CsgPrint(field[NodeIndex + 5]);
                break;
            case 13 :
                ptoc_Var11 = sql__ut(output);
                fprintf(ptoc_Var11->_fbuf , "");
                sql__writeln(ptoc_Var11);
                ptoc_Var11 = sql__ut(output);
                fprintf(ptoc_Var11->_fbuf , "");
                sql__writeln(ptoc_Var11);
                indent(NestLevel , NodeIndex , "Proc                ");
                GetString(STab[field[NodeIndex + 7]].SName , &s[0]);
                ptoc_Var11 = sql__ut(output);
                ptoc_Var12 = _GetStringLength(STab[field[NodeIndex + 7]].SName);
                fprintf(ptoc_Var11->_fbuf , "%s%*.*s%s%*d%c" , ":  " , ptoc_Var12 , ptoc_Var12 < 256 ? ptoc_Var12 : 256 , s , " [" , 1 , field[NodeIndex + 7] , 93);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                HdgPrint(field[NodeIndex + 6]);
                HdgPrint(field[NodeIndex + 5]);
                break;
            case 14 :
                indent(NestLevel , NodeIndex , "OpenProc            ");
                GetString(STab[field[NodeIndex + 7]].SName , &s[0]);
                ptoc_Var11 = sql__ut(output);
                ptoc_Var12 = _GetStringLength(STab[field[NodeIndex + 7]].SName);
                fprintf(ptoc_Var11->_fbuf , "%s%*.*s%s%*d%c" , ":  " , ptoc_Var12 , ptoc_Var12 < 256 ? ptoc_Var12 : 256 , s , " [" , 1 , field[NodeIndex + 7] , 93);
                PrintType_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                PrintDscPtr_CsgPrint(&_ptoc_LOCAL, NodeIndex);
                HdgPrint(field[NodeIndex + 5]);
                break;
            case 15 :
                indent(NestLevel , NodeIndex , "InitialProc - no further analysis");
                break;
            default :
                ptoc_Var11 = sql__ut(output);
                fprintf(ptoc_Var11->_fbuf , "%s%10d%10d" , "CsgPrint: unexpected MajorOp / NodeIndex " , shr(field[NodeIndex] , 16) , NodeIndex);
                sql__writeln(ptoc_Var11);
                break;
            }
        NestLevel += -3;
        }
    }

struct _ptoc_FRAME_HdgPrint
  {
  char dummy;
  } ;


int HdgPrint(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_HdgPrint _ptoc_LOCAL;
    
    while (NodeIndex != -1 && NodeIndex != 0)
        {
        CsgPrint(NodeIndex);
        NodeIndex = field[NodeIndex + 3];
        }
    }

struct _ptoc_FRAME_DumpHdg
  {
  char dummy;
  } ;


int DumpHdg(NodeIndex)
int NodeIndex;
    {
    struct _ptoc_FRAME_DumpHdg _ptoc_LOCAL;
    _iorechd *ptoc_Var13;
    
    DumptreeI();
    ptoc_Var13 = sql__ut(output);
    fprintf(ptoc_Var13->_fbuf , "");
    sql__writeln(ptoc_Var13);
    ptoc_Var13 = sql__ut(output);
    fprintf(ptoc_Var13->_fbuf , "");
    sql__writeln(ptoc_Var13);
    ptoc_Var13 = sql__ut(output);
    fprintf(ptoc_Var13->_fbuf , "%s" , "dump hdg");
    ptoc_Var13 = sql__ut(output);
    fprintf(ptoc_Var13->_fbuf , "");
    sql__writeln(ptoc_Var13);
    ptoc_Var13 = sql__ut(output);
    fprintf(ptoc_Var13->_fbuf , "");
    sql__writeln(ptoc_Var13);
    NestLevel = -3;
    HdgPrint(NodeIndex);
    ptoc_Var13 = sql__ut(output);
    fprintf(ptoc_Var13->_fbuf , "");
    sql__writeln(ptoc_Var13);
    ptoc_Var13 = sql__ut(output);
    fprintf(ptoc_Var13->_fbuf , "");
    sql__writeln(ptoc_Var13);
    }
int HdgMark();
int markT();

struct _ptoc_FRAME_Mark
  {
  char dummy;
  } ;


int Mark(Idx)
int Idx;
    {
    struct _ptoc_FRAME_Mark _ptoc_LOCAL;
    
    STab[Idx].SFeld5 = 1;
    }
int DontReduceAnything();

struct _ptoc_FRAME_IsMarked
  {
  char dummy;
  } ;


int IsMarked(Idx)
int Idx;
    {
    struct _ptoc_FRAME_IsMarked _ptoc_LOCAL;
    int _IsMarked;
    
    _IsMarked = STab[Idx].SFeld5 == 1 || DontReduceAnything();
    return(_IsMarked);
    }

struct _ptoc_FRAME_TypeOfVar
  {
  char dummy;
  } ;


int TypeOfVar(Idx)
int Idx;
    {
    struct _ptoc_FRAME_TypeOfVar _ptoc_LOCAL;
    int _TypeOfVar;
    
    if (STab[Idx].STname > 0)
        {
        _TypeOfVar = STab[Idx].STname;
        }
    else
        {
        _TypeOfVar = STab[Idx].SType;
        }
    return(_TypeOfVar);
    }

struct _ptoc_FRAME_IsConstDcl
  {
  char dummy;
  } ;


int IsConstDcl(Idx)
int Idx;
    {
    struct _ptoc_FRAME_IsConstDcl _ptoc_LOCAL;
    int _IsConstDcl;
    struct ptoc_Type1 *ptoc_Var14;
    
    ptoc_Var14 = &STab[Idx];
    _IsConstDcl = ptoc_Var14->SKind == 8 && ptoc_Var14->KINDTYPE.C_ConDcl.SNextEnum == 0;
    return(_IsConstDcl);
    }

struct _ptoc_FRAME_SubTree
  {
  char dummy;
  } ;


int SubTree(Idx)
int Idx;
    {
    struct _ptoc_FRAME_SubTree _ptoc_LOCAL;
    int _SubTree;
    
    _SubTree = STab[Idx].SFeld3;
    return(_SubTree);
    }

struct _ptoc_FRAME_MarkId
  {
  char dummy;
  } ;


int MarkId(Idx)
int Idx;
    {
    struct _ptoc_FRAME_MarkId _ptoc_LOCAL;
    
    Mark(Idx);
    markT(TypeOfVar(Idx));
    if (IsConstDcl(Idx))
        {
        if (SubTree(Idx) != 0)
            {
            HdgMark(SubTree(Idx));
            }
        }
    }

struct _ptoc_FRAME_CsgMark
  {
  char dummy;
  } ;


int CsgMark(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CsgMark _ptoc_LOCAL;
    STRING s;
    HdgRange SaveIndex;
    
    if (NodeIndex != -1)
        {
        switch (shr(field[NodeIndex] , 16))
            {
            case 1 :
                switch (field[NodeIndex] & 65535)
                    {
                    case 94 :
                        MarkId(field[NodeIndex + 5]);
                        break;
                    default :
                        break;
                    }
                break;
            case 2 :
                break;
            case 3 :
                CsgMark(field[NodeIndex + 5]);
                break;
            case 4 :
                switch (field[NodeIndex] & 65535)
                    {
                    case 97 :
                        markT(field[NodeIndex + 1]);
                        break;
                    default :
                        break;
                    }
                CsgMark(field[NodeIndex + 5]);
                break;
            case 5 :
                CsgMark(field[NodeIndex + 5]);
                CsgMark(field[NodeIndex + 6]);
                break;
            case 6 :
                SaveIndex = NodeIndex;
                NodeIndex = field[NodeIndex + 5];
                while (NodeIndex != -1)
                    {
                    CsgMark(NodeIndex);
                    NodeIndex = field[NodeIndex + 3];
                    }
                break;
            case 7 :
                if ((field[NodeIndex] & 65535) == 40)
                    {
                    CsgMark(field[NodeIndex + 7]);
                    }
                if ((field[NodeIndex] & 65535) != 62)
                    {
                    CsgMark(field[NodeIndex + 5]);
                    }
                CsgMark(field[NodeIndex + 6]);
                break;
            case 8 :
                CsgMark(field[NodeIndex + 5]);
                HdgMark(field[NodeIndex + 6]);
                if (field[NodeIndex + 7] != -1)
                    {
                    HdgMark(field[NodeIndex + 7]);
                    }
                break;
            case 9 :
                CsgMark(field[NodeIndex + 5]);
                HdgMark(field[NodeIndex + 6]);
                break;
            case 10 :
                CsgMark(field[NodeIndex + 5]);
                HdgMark(field[NodeIndex + 6]);
                break;
            case 11 :
                break;
            case 12 :
                HdgMark(field[NodeIndex + 6]);
                CsgMark(field[NodeIndex + 5]);
                break;
            case 13 :
                HdgMark(field[NodeIndex + 6]);
                HdgMark(field[NodeIndex + 5]);
                break;
            case 14 :
                HdgMark(field[NodeIndex + 5]);
                break;
            }
        }
    }

struct _ptoc_FRAME_HdgMark
  {
  char dummy;
  } ;


int HdgMark(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_HdgMark _ptoc_LOCAL;
    
    while (NodeIndex != -1)
        {
        CsgMark(NodeIndex);
        NodeIndex = field[NodeIndex + 3];
        }
    }

struct _ptoc_FRAME_IsEnumDcl
  {
  char dummy;
  } ;


int IsEnumDcl(Idx)
int Idx;
    {
    struct _ptoc_FRAME_IsEnumDcl _ptoc_LOCAL;
    int _IsEnumDcl;
    struct ptoc_Type1 *ptoc_Var15;
    
    ptoc_Var15 = &STab[Idx];
    _IsEnumDcl = ptoc_Var15->SKind == 7 && ptoc_Var15->SType == -10;
    return(_IsEnumDcl);
    }

struct _ptoc_FRAME_M_RecordType
  {
  char dummy;
  } ;
static
  int List_RecordElements_M_RecordType();
static
  int List_RecordVariants_M_RecordType();
static
  int M_RecordElement_M_RecordType();
static
  int M_RecordVariant_M_RecordType();


struct _ptoc_FRAME_List_RecordElements_M_RecordType
  {
  struct _ptoc_FRAME_M_RecordType *_ptoc_LINK;
  char dummy;
  } ;


static
int List_RecordElements_M_RecordType(_ptoc_LINK, Idx)
struct _ptoc_FRAME_M_RecordType *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_List_RecordElements_M_RecordType _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    while (Idx != -1)
        {
        M_RecordElement_M_RecordType(_ptoc_LOCAL._ptoc_LINK, Idx);
        Idx = STab[Idx].KINDTYPE.C_RecordElementDcl.SNextField;
        }
    }

struct _ptoc_FRAME_List_RecordVariants_M_RecordType
  {
  struct _ptoc_FRAME_M_RecordType *_ptoc_LINK;
  char dummy;
  } ;


static
int List_RecordVariants_M_RecordType(_ptoc_LINK, Idx)
struct _ptoc_FRAME_M_RecordType *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_List_RecordVariants_M_RecordType _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    while (Idx != -1)
        {
        M_RecordVariant_M_RecordType(_ptoc_LOCAL._ptoc_LINK, Idx);
        Idx = STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SNextVariant;
        }
    }

struct _ptoc_FRAME_M_RecordElement_M_RecordType
  {
  struct _ptoc_FRAME_M_RecordType *_ptoc_LINK;
  char dummy;
  } ;


static
int M_RecordElement_M_RecordType(_ptoc_LINK, Idx)
struct _ptoc_FRAME_M_RecordType *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_M_RecordElement_M_RecordType _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (!IsEnumDcl(STab[Idx].SType))
        {
        markT(TypeOfVar(Idx));
        }
    }

struct _ptoc_FRAME_M_RecordVariant_M_RecordType
  {
  struct _ptoc_FRAME_M_RecordType *_ptoc_LINK;
  char dummy;
  } ;


static
int M_RecordVariant_M_RecordType(_ptoc_LINK, Idx)
struct _ptoc_FRAME_M_RecordType *_ptoc_LINK;
int Idx;
    {
    struct _ptoc_FRAME_M_RecordVariant_M_RecordType _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    List_RecordElements_M_RecordType(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField);
    List_RecordVariants_M_RecordType(_ptoc_LOCAL._ptoc_LINK, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant);
    }

int M_RecordType(Idx)
int Idx;
    {
    struct _ptoc_FRAME_M_RecordType _ptoc_LOCAL;
    
    List_RecordElements_M_RecordType(&_ptoc_LOCAL, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstField);
    List_RecordVariants_M_RecordType(&_ptoc_LOCAL, STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_RecordType.SFirstVariant);
    }

struct _ptoc_FRAME_BreakPointerType
  {
  char dummy;
  } ;


int BreakPointerType()
    {
    struct _ptoc_FRAME_BreakPointerType _ptoc_LOCAL;
    int _BreakPointerType;
    
    _BreakPointerType = control2[98];
    return(_BreakPointerType);
    }

struct _ptoc_FRAME_DelayedParCopy
  {
  char dummy;
  } ;


int DelayedParCopy()
    {
    struct _ptoc_FRAME_DelayedParCopy _ptoc_LOCAL;
    int _DelayedParCopy;
    
    _DelayedParCopy = !control2[100];
    return(_DelayedParCopy);
    }

struct _ptoc_FRAME_DeleteUnusedVar
  {
  char dummy;
  } ;


int DeleteUnusedVar()
    {
    struct _ptoc_FRAME_DeleteUnusedVar _ptoc_LOCAL;
    int _DeleteUnusedVar;
    
    _DeleteUnusedVar = !control2[118];
    return(_DeleteUnusedVar);
    }

struct _ptoc_FRAME_DeleteUnusedType
  {
  char dummy;
  } ;


int DeleteUnusedType()
    {
    struct _ptoc_FRAME_DeleteUnusedType _ptoc_LOCAL;
    int _DeleteUnusedType;
    
    _DeleteUnusedType = !control2[116];
    return(_DeleteUnusedType);
    }

struct _ptoc_FRAME_DeleteUnusedEnumElement
  {
  char dummy;
  } ;


int DeleteUnusedEnumElement()
    {
    struct _ptoc_FRAME_DeleteUnusedEnumElement _ptoc_LOCAL;
    int _DeleteUnusedEnumElement;
    
    _DeleteUnusedEnumElement = DeleteUnusedType();
    return(_DeleteUnusedEnumElement);
    }

struct _ptoc_FRAME_DeleteUnusedConst
  {
  char dummy;
  } ;


int DeleteUnusedConst()
    {
    struct _ptoc_FRAME_DeleteUnusedConst _ptoc_LOCAL;
    int _DeleteUnusedConst;
    
    _DeleteUnusedConst = DeleteUnusedType();
    return(_DeleteUnusedConst);
    }

struct _ptoc_FRAME_UseSymbolicConst
  {
  char dummy;
  } ;


int UseSymbolicConst()
    {
    struct _ptoc_FRAME_UseSymbolicConst _ptoc_LOCAL;
    int _UseSymbolicConst;
    
    _UseSymbolicConst = control[99];
    return(_UseSymbolicConst);
    }

struct _ptoc_FRAME_DontReduceAnything
  {
  char dummy;
  } ;


int DontReduceAnything()
    {
    struct _ptoc_FRAME_DontReduceAnything _ptoc_LOCAL;
    int _DontReduceAnything;
    
    _DontReduceAnything = !DeleteUnusedType() && !DeleteUnusedVar();
    return(_DontReduceAnything);
    }

struct _ptoc_FRAME_markT
  {
  char dummy;
  } ;


int markT(Idx)
int Idx;
    {
    struct _ptoc_FRAME_markT _ptoc_LOCAL;
    
    if (!IsMarked(Idx))
        {
        Mark(Idx);
        if (STab[Idx].SKind == 7)
            {
            switch (STab[Idx].SType)
                {
                case -14 :
                    M_RecordType(Idx);
                    break;
                case -17 :
                    if (!BreakPointerType())
                        {
                        markT(STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain);
                        }
                    break;
                case -13 :
                    markT(STab[Idx].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement);
                    break;
                case -10 :
                    if (STab[Idx].STmyname > 0)
                        {
                        markT(STab[Idx].STmyname);
                        }
                    break;
                default :
                    break;
                }
            }
        else
            {
            if (STab[Idx].SKind == 16 || STab[Idx].SKind == 23)
                {
                markT(STab[Idx].SType);
                }
            }
        }
    }

struct _ptoc_FRAME_UnmarkedType
  {
  char dummy;
  } ;


int UnmarkedType(Idx)
int Idx;
    {
    struct _ptoc_FRAME_UnmarkedType _ptoc_LOCAL;
    int _UnmarkedType;
    
    _UnmarkedType = (STab[Idx].SKind == 16 || STab[Idx].SKind == 7) && !IsMarked(Idx);
    return(_UnmarkedType);
    }

struct _ptoc_FRAME_UnmarkedVar
  {
  char dummy;
  } ;


int UnmarkedVar(Idx)
int Idx;
    {
    struct _ptoc_FRAME_UnmarkedVar _ptoc_LOCAL;
    int _UnmarkedVar;
    
    _UnmarkedVar = (STab[Idx].SKind == 9 || STab[Idx].SKind == 14) && !IsMarked(Idx);
    return(_UnmarkedVar);
    }
