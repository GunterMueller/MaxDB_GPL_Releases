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
int CardCount;
int P1m2I();
int ErrorMsg();
int DclError();
int werror();
int FatalErrorMsg();
int WarnStrCmp();
int ConstExp();
int IsPrimConstant();
typedef
  int ConstStackRange;
int ConstStack[11];
ConstStackRange ConstSp;
int itraverse();

struct _ptoc_FRAME_pop
  {
  char dummy;
  } ;


int pop()
    {
    struct _ptoc_FRAME_pop _ptoc_LOCAL;
    
    if (ConstSp == 0)
        {
        ErrorMsg(107 , -1);
        }
    ConstSp += -1;
    }

struct _ptoc_FRAME_push
  {
  char dummy;
  } ;


int push()
    {
    struct _ptoc_FRAME_push _ptoc_LOCAL;
    
    if (ConstSp == 10)
        {
        ErrorMsg(108 , -1);
        }
    ConstSp += 1;
    }

struct _ptoc_FRAME_InitCode
  {
  char dummy;
  } ;


int InitCode(NodeIndex)
int NodeIndex;
    {
    struct _ptoc_FRAME_InitCode _ptoc_LOCAL;
    int i;
    int t;
    int t1;
    int addr;
    int offset;
    
    switch (field[NodeIndex] & 65535)
        {
        case 1 :
            break;
        case 12 :
            t = STab[field[NodeIndex + 1]].SType;
            push();
            ConstStack[ConstSp] = field[NodeIndex + 5];
            break;
        case 94 :
            if (STab[field[NodeIndex + 5]].SKind == 8)
                {
                if (STab[field[NodeIndex + 5]].SFeld3 > 0)
                    {
                    itraverse(STab[field[NodeIndex + 5]].SFeld3);
                    }
                else
                    {
                    push();
                    ConstStack[ConstSp] = STab[field[NodeIndex + 5]].KINDTYPE.C_ConDcl.SCon1val;
                    }
                }
            else
                {
                ErrorMsg(100 , -1);
                }
            break;
        case 4 :
            break;
        case 93 :
            break;
        case 34 :
            ConstStack[ConstSp + -1] = sql__ord((int ) (ConstStack[ConstSp + -1] <= ConstStack[ConstSp]));
            pop();
            break;
        case 32 :
            ConstStack[ConstSp + -1] = sql__ord((int ) (ConstStack[ConstSp + -1] < ConstStack[ConstSp]));
            pop();
            break;
        case 35 :
            ConstStack[ConstSp + -1] = sql__ord((int ) (ConstStack[ConstSp + -1] >= ConstStack[ConstSp]));
            pop();
            break;
        case 33 :
            ConstStack[ConstSp + -1] = sql__ord((int ) (ConstStack[ConstSp + -1] > ConstStack[ConstSp]));
            pop();
            break;
        case 31 :
            ConstStack[ConstSp + -1] = sql__ord((int ) (ConstStack[ConstSp + -1] == ConstStack[ConstSp]));
            pop();
            break;
        case 36 :
            ConstStack[ConstSp + -1] = sql__ord((int ) (ConstStack[ConstSp + -1] != ConstStack[ConstSp]));
            pop();
            break;
        case 24 :
            ConstStack[ConstSp] = -ConstStack[ConstSp];
            break;
        case 19 :
            ConstStack[ConstSp + -1] += ConstStack[ConstSp];
            pop();
            break;
        case 20 :
            ConstStack[ConstSp + -1] -= ConstStack[ConstSp];
            pop();
            break;
        case 21 :
            ConstStack[ConstSp + -1] *= ConstStack[ConstSp];
            pop();
            break;
        case 22 :
            ConstStack[ConstSp + -1] /= ConstStack[ConstSp];
            pop();
            break;
        case 27 :
            ConstStack[ConstSp + -1] = sql__ord((int ) (ConstStack[ConstSp + -1] == sql__ord(1) && ConstStack[ConstSp] == sql__ord(1)));
            pop();
            break;
        case 25 :
            ConstStack[ConstSp + -1] = sql__ord((int ) (ConstStack[ConstSp + -1] == sql__ord(1) || ConstStack[ConstSp] == sql__ord(1)));
            pop();
            break;
        case 28 :
            ConstStack[ConstSp] = sql__ord((int ) (ConstStack[ConstSp] == sql__ord(0)));
            break;
        default :
            ErrorMsg(100 , -1);
            break;
        }
    }

struct _ptoc_FRAME_IcsgTraverse
  {
  char dummy;
  } ;


int IcsgTraverse(NodeIndex)
int NodeIndex;
    {
    struct _ptoc_FRAME_IcsgTraverse _ptoc_LOCAL;
    int i;
    
    switch (shr(field[NodeIndex] , 16))
        {
        case 1 :
            InitCode(NodeIndex);
            break;
        case 4 :
            itraverse(field[NodeIndex + 5]);
            InitCode(NodeIndex);
            break;
        case 5 :
            itraverse(field[NodeIndex + 5]);
            itraverse(field[NodeIndex + 6]);
            InitCode(NodeIndex);
            break;
        case 7 :
            CardCount = field[NodeIndex + 9];
            itraverse(field[NodeIndex + 6]);
            itraverse(field[NodeIndex + 5]);
            InitCode(NodeIndex);
            break;
        case 2 :
            CardCount = field[NodeIndex + 6];
            InitCode(NodeIndex);
            break;
        case 3 :
            CardCount = field[NodeIndex + 6];
            itraverse(field[NodeIndex + 5]);
            InitCode(NodeIndex);
            break;
        }
    }

struct _ptoc_FRAME_itraverse
  {
  char dummy;
  } ;


int itraverse(NodeIndex)
int NodeIndex;
    {
    struct _ptoc_FRAME_itraverse _ptoc_LOCAL;
    int MajorOp;
    
    while (NodeIndex > -1)
        {
        IcsgTraverse(NodeIndex);
        MajorOp = shr(field[NodeIndex] , 16);
        if (MajorOp == 1 || MajorOp == 4 || MajorOp == 5 || MajorOp == 6)
            {
            goto LAB_100;
            }
        NodeIndex = field[NodeIndex + 3];
        }
    LAB_100 :
    ;
    }

struct _ptoc_FRAME_ConstExp
  {
  char dummy;
  } ;


int ConstExp(tree)
int tree;
    {
    struct _ptoc_FRAME_ConstExp _ptoc_LOCAL;
    int _ConstExp;
    
    ConstSp = 0;
    if (tree != 0)
        {
        itraverse(tree);
        }
    _ConstExp = ConstStack[ConstSp];
    return(_ConstExp);
    }

struct _ptoc_FRAME_IsPrimConstant
  {
  char dummy;
  } ;


int IsPrimConstant(NodeIndex)
int NodeIndex;
    {
    struct _ptoc_FRAME_IsPrimConstant _ptoc_LOCAL;
    int _IsPrimConstant;
    
    _IsPrimConstant = (field[NodeIndex] & 65535) == 12 || (field[NodeIndex] & 65535) == 94 && STab[field[NodeIndex + 5]].SKind == 8;
    return(_IsPrimConstant);
    }
