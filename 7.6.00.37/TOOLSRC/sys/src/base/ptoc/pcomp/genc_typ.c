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
int BumpSymPtr();

struct _ptoc_FRAME_GenTyp
  {
  int NestLevel;
  char dummy;
  } ;
typedef
  unsigned char String20_GenTyp[20];
static
  int HdgGenTyp_GenTyp();


struct _ptoc_FRAME_TYP_GenTyp
  {
  struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int TYP_GenTyp(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_TYP_GenTyp _ptoc_LOCAL;
    int _TYP;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (NodeIndex == -1)
        {
        _TYP = 0;
        }
    else
        {
        _TYP = field[NodeIndex + 2];
        }
    return(_TYP);
    }

struct _ptoc_FRAME_max_GenTyp
  {
  struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int max_GenTyp(_ptoc_LINK, a, b)
struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
int a;
int b;
    {
    struct _ptoc_FRAME_max_GenTyp _ptoc_LOCAL;
    int _max;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (a > b)
        {
        _max = a;
        }
    else
        {
        _max = b;
        }
    return(_max);
    }

struct _ptoc_FRAME_Block_GenTyp
  {
  struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int Block_GenTyp(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_Block_GenTyp _ptoc_LOCAL;
    int _Block;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _Block = field[NodeIndex + 3] != -1;
    return(_Block);
    }

struct _ptoc_FRAME_CsgGenTyp_GenTyp
  {
  struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int CsgGenTyp_GenTyp(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CsgGenTyp_GenTyp _ptoc_LOCAL;
    STRING s;
    HdgRange SaveIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (NodeIndex != -1)
        {
        _ptoc_LOCAL._ptoc_LINK->NestLevel += 3;
        switch (shr(field[NodeIndex] , 16))
            {
            case 1 :
                switch (field[NodeIndex] & 65535)
                    {
                    case 94 :
                        field[NodeIndex + 2] = 2;
                        break;
                    case 110 :
                        field[NodeIndex + 2] = 2;
                        break;
                    case 12 :
                        field[NodeIndex + 2] = 1;
                        break;
                    case 14 :
                        field[NodeIndex + 2] = 1;
                        break;
                    case 70 :
                        field[NodeIndex + 2] = 2;
                        break;
                    case 69 :
                        field[NodeIndex + 2] = 1;
                        break;
                    case 109 :
                        field[NodeIndex + 2] = 1;
                        break;
                    default :
                        field[NodeIndex + 2] = 0;
                        break;
                    }
                break;
            case 2 :
                switch (field[NodeIndex] & 65535)
                    {
                    case 119 :
                        field[NodeIndex + 2] = 5;
                        break;
                    case 101 :
                    case 86 :
                    case 87 :
                    case 98 :
                        field[NodeIndex + 2] = 6;
                        break;
                    default :
                        field[NodeIndex + 2] = 0;
                        break;
                    }
                break;
            case 3 :
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                break;
            case 4 :
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                switch (field[NodeIndex] & 65535)
                    {
                    case 8 :
                    case 93 :
                    case 92 :
                    case 28 :
                    case 24 :
                        field[NodeIndex + 2] = 4;
                        break;
                    case 9 :
                    case 97 :
                    case 4 :
                    case 13 :
                        field[NodeIndex + 2] = 3;
                        break;
                    default :
                        field[NodeIndex + 2] = TYP_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                        break;
                    }
                break;
            case 5 :
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                switch (field[NodeIndex] & 65535)
                    {
                    case 19 :
                    case 27 :
                    case 89 :
                    case 90 :
                    case 91 :
                    case 22 :
                    case 31 :
                    case 35 :
                    case 33 :
                    case 34 :
                    case 32 :
                    case 88 :
                    case 21 :
                    case 36 :
                    case 25 :
                    case 29 :
                    case 30 :
                    case 20 :
                        field[NodeIndex + 2] = 4;
                        break;
                    case 96 :
                    case 95 :
                        field[NodeIndex + 2] = 3;
                        break;
                    default :
                        field[NodeIndex + 2] = max_GenTyp(_ptoc_LOCAL._ptoc_LINK, TYP_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]) , TYP_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]));
                        break;
                    }
                break;
            case 6 :
                SaveIndex = NodeIndex;
                NodeIndex = field[NodeIndex + 5];
                while (NodeIndex != -1)
                    {
                    CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
                    NodeIndex = field[NodeIndex + 3];
                    }
                NodeIndex = SaveIndex;
                field[NodeIndex + 2] = 4;
                break;
            case 7 :
                if ((field[NodeIndex] & 65535) == 40)
                    {
                    CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7]);
                    }
                if ((field[NodeIndex] & 65535) != 62)
                    {
                    CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                    }
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                switch (field[NodeIndex] & 65535)
                    {
                    case 40 :
                    case 62 :
                        field[NodeIndex + 2] = 3;
                        break;
                    case 103 :
                    case 102 :
                    case 6 :
                    case 112 :
                        field[NodeIndex + 2] = 4;
                        break;
                    case 43 :
                        field[NodeIndex + 2] = 6;
                        break;
                    default :
                        field[NodeIndex + 2] = 0;
                        break;
                    }
                break;
            case 8 :
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                HdgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                if (field[NodeIndex + 7] != -1)
                    {
                    HdgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7]);
                    }
                field[NodeIndex + 2] = max_GenTyp(_ptoc_LOCAL._ptoc_LINK, TYP_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]) , TYP_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7]));
                if (field[NodeIndex + 2] <= 4)
                    {
                    field[NodeIndex + 2] = 4;
                    }
                else
                    {
                    field[NodeIndex + 2] = 6;
                    }
                if (field[NodeIndex + 6] == -1 || field[NodeIndex + 7] == -1)
                    {
                    field[NodeIndex + 2] = 6;
                    }
                if (Block_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]) || Block_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7]))
                    {
                    field[NodeIndex + 2] = 6;
                    }
                break;
            case 9 :
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                HdgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                field[NodeIndex + 2] = 6;
                break;
            case 10 :
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                HdgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                field[NodeIndex + 2] = 6;
                break;
            case 11 :
                field[NodeIndex + 2] = 0;
                break;
            case 12 :
                HdgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                field[NodeIndex + 2] = 6;
                break;
            case 13 :
                HdgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                HdgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                field[NodeIndex + 2] = 0;
                break;
            case 14 :
                HdgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                field[NodeIndex + 2] = 0;
                break;
            }
        _ptoc_LOCAL._ptoc_LINK->NestLevel += -3;
        }
    }

struct _ptoc_FRAME_HdgGenTyp_GenTyp
  {
  struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int HdgGenTyp_GenTyp(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenTyp *_ptoc_LINK;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_HdgGenTyp_GenTyp _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    while (NodeIndex != -1)
        {
        CsgGenTyp_GenTyp(_ptoc_LOCAL._ptoc_LINK, NodeIndex);
        NodeIndex = field[NodeIndex + 3];
        }
    }

int GenTyp(Node)
int Node;
    {
    struct _ptoc_FRAME_GenTyp _ptoc_LOCAL;
    int nummer;
    
    _ptoc_LOCAL.NestLevel = -3;
    HdgGenTyp_GenTyp(&_ptoc_LOCAL, Node);
    }

struct _ptoc_FRAME_GenIfTyp
  {
  int NestLevel;
  char dummy;
  } ;
typedef
  unsigned char String20_GenIfTyp[20];
static
  int WithCompletion_GenIfTyp();
static
  int HdgGenIfTyp_GenIfTyp();


struct _ptoc_FRAME_TYP_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int TYP_GenIfTyp(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
int NodeIndex;
    {
    struct _ptoc_FRAME_TYP_GenIfTyp _ptoc_LOCAL;
    int _TYP;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (NodeIndex == -1)
        {
        _TYP = 0;
        }
    else
        {
        _TYP = field[NodeIndex + 2];
        }
    return(_TYP);
    }

struct _ptoc_FRAME_Block_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int Block_GenIfTyp(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_Block_GenIfTyp _ptoc_LOCAL;
    int _Block;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _Block = field[NodeIndex + 3] != -1;
    return(_Block);
    }

struct _ptoc_FRAME_CsgGenIfTyp_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int CsgGenIfTyp_GenIfTyp(_ptoc_LINK, NodeIndex, inc)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
HdgRange NodeIndex;
int inc;
    {
    struct _ptoc_FRAME_CsgGenIfTyp_GenIfTyp _ptoc_LOCAL;
    STRING s;
    HdgRange SaveIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (NodeIndex != -1)
        {
        _ptoc_LOCAL._ptoc_LINK->NestLevel += 3;
        switch (shr(field[NodeIndex] , 16))
            {
            case 1 :
                break;
            case 2 :
                break;
            case 3 :
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1);
                break;
            case 4 :
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1);
                break;
            case 5 :
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1);
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 1);
                break;
            case 6 :
                SaveIndex = NodeIndex;
                NodeIndex = field[NodeIndex + 5];
                while (NodeIndex != -1)
                    {
                    CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, NodeIndex , 1);
                    NodeIndex = field[NodeIndex + 3];
                    }
                break;
            case 7 :
                if ((field[NodeIndex] & 65535) == 40)
                    {
                    CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7] , 1);
                    }
                if ((field[NodeIndex] & 65535) != 62)
                    {
                    CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1);
                    }
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6] , 1);
                break;
            case 8 :
                if (inc == 0)
                    {
                    field[NodeIndex + 2] = 6;
                    }
                else
                    {
                    field[NodeIndex + 2] = 4;
                    }
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1);
                HdgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                if (field[NodeIndex + 7] != -1)
                    {
                    HdgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 7]);
                    }
                break;
            case 9 :
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1);
                HdgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                break;
            case 10 :
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1);
                HdgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                break;
            case 11 :
                break;
            case 12 :
                HdgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5] , 1);
                break;
            case 13 :
                HdgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]);
                HdgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                break;
            case 14 :
                HdgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]);
                break;
            }
        _ptoc_LOCAL._ptoc_LINK->NestLevel += -3;
        }
    }

struct _ptoc_FRAME_HdgGenIfTyp_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int HdgGenIfTyp_GenIfTyp(_ptoc_LINK, NodeIndex)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_HdgGenIfTyp_GenIfTyp _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    while (NodeIndex != -1)
        {
        CsgGenIfTyp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, NodeIndex , 0);
        NodeIndex = field[NodeIndex + 3];
        }
    }

struct _ptoc_FRAME_IsAnyPointerType_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int IsAnyPointerType_GenIfTyp(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_IsAnyPointerType_GenIfTyp _ptoc_LOCAL;
    int _IsAnyPointerType;
    int NodeIndex;
    int Idx;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsAnyPointerType = 0;
    NodeIndex = Node;
    if (shr(field[NodeIndex] , 16) == 1 && (field[NodeIndex] & 65535) == 94)
        {
        Idx = field[NodeIndex + 5];
        if (STab[Idx].SName == 0 && STab[Idx].SType == 40)
            {
            _IsAnyPointerType = 1;
            }
        }
    return(_IsAnyPointerType);
    }

struct _ptoc_FRAME_IsLoadadrOp_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int IsLoadadrOp_GenIfTyp(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_IsLoadadrOp_GenIfTyp _ptoc_LOCAL;
    int _IsLoadadrOp;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsLoadadrOp = 0;
    NodeIndex = Node;
    if (shr(field[NodeIndex] , 16) == 4 && (field[NodeIndex] & 65535) == 13)
        {
        _IsLoadadrOp = 1;
        }
    return(_IsLoadadrOp);
    }

struct _ptoc_FRAME_IsWithStatement_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int IsWithStatement_GenIfTyp(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_IsWithStatement_GenIfTyp _ptoc_LOCAL;
    int _IsWithStatement;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    _IsWithStatement = 0;
    NodeIndex = Node;
    if (shr(field[NodeIndex] , 16) == 7 && (field[NodeIndex] & 65535) == 6)
        {
        if (IsAnyPointerType_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 5]) && IsLoadadrOp_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, field[NodeIndex + 6]))
            {
            _IsWithStatement = 1;
            }
        }
    return(_IsWithStatement);
    }

struct _ptoc_FRAME_Complete_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int Complete_GenIfTyp(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_Complete_GenIfTyp _ptoc_LOCAL;
    int Idx1;
    int Idx2;
    int Idx_neu;
    int NodeIndex;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NodeIndex = Node;
    NodeIndex = field[NodeIndex + 5];
    Idx1 = field[NodeIndex + 5];
    NodeIndex = Node;
    NodeIndex = field[NodeIndex + 6];
    NodeIndex = field[NodeIndex + 5];
    Idx2 = field[NodeIndex + 1];
    BumpSymPtr();
    Idx_neu = SymPtr;
    STab[Idx_neu].SType = -17;
    STab[Idx_neu].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = Idx2;
    STab[Idx1].SType = Idx_neu;
    }

struct _ptoc_FRAME_WithCompletion_GenIfTyp
  {
  struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
  char dummy;
  } ;


static
int WithCompletion_GenIfTyp(_ptoc_LINK, Node)
struct _ptoc_FRAME_GenIfTyp *_ptoc_LINK;
int Node;
    {
    struct _ptoc_FRAME_WithCompletion_GenIfTyp _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (IsWithStatement_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, Node))
        {
        Complete_GenIfTyp(_ptoc_LOCAL._ptoc_LINK, Node);
        }
    }

int GenIfTyp(Node)
int Node;
    {
    struct _ptoc_FRAME_GenIfTyp _ptoc_LOCAL;
    
    _ptoc_LOCAL.NestLevel = -3;
    HdgGenIfTyp_GenIfTyp(&_ptoc_LOCAL, Node);
    }

struct _ptoc_FRAME_IsConstantExpr
  {
  char dummy;
  } ;


int IsConstantExpr(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_IsConstantExpr _ptoc_LOCAL;
    int _IsConstantExpr;
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
                        _IsConstantExpr = STab[field[NodeIndex + 5]].SKind == 8;
                        break;
                    case 12 :
                    case 14 :
                        _IsConstantExpr = 1;
                        break;
                    default :
                        _IsConstantExpr = 0;
                        break;
                    }
                break;
            case 4 :
                switch (field[NodeIndex] & 65535)
                    {
                    case 93 :
                    case 92 :
                    case 28 :
                    case 24 :
                    case 97 :
                    case 4 :
                    case 13 :
                        _IsConstantExpr = IsConstantExpr(field[NodeIndex + 5]);
                        break;
                    default :
                        _IsConstantExpr = 0;
                        break;
                    }
                break;
            case 5 :
                switch (field[NodeIndex] & 65535)
                    {
                    case 19 :
                    case 27 :
                    case 89 :
                    case 90 :
                    case 91 :
                    case 22 :
                    case 31 :
                    case 35 :
                    case 33 :
                    case 34 :
                    case 32 :
                    case 88 :
                    case 21 :
                    case 36 :
                    case 25 :
                    case 29 :
                    case 30 :
                    case 20 :
                        _IsConstantExpr = IsConstantExpr(field[NodeIndex + 5]) && IsConstantExpr(field[NodeIndex + 6]);
                        break;
                    default :
                        _IsConstantExpr = 0;
                        break;
                    }
                break;
            default :
                _IsConstantExpr = 0;
                break;
            }
        }
    else
        {
        _IsConstantExpr = 0;
        }
    return(_IsConstantExpr);
    }
