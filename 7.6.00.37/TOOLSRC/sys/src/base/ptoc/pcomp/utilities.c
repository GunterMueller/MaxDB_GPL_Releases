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
int shl();
int shr();
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
int cerror();
int BumpSymPtr();
int TypeSize();
int UtilitiesI();
int PasCerror();
int BaseType();
int CopyTree();
int demorgan();
int GetTemporary();
int FreeTemporary();
int ClearTemporarys();
int BasisTyp();
SymbolTableRange FreeTemps;
int TempNo;

struct _ptoc_FRAME_UtilitiesI
  {
  char dummy;
  } ;


int UtilitiesI()
    {
    struct _ptoc_FRAME_UtilitiesI _ptoc_LOCAL;
    
    FreeTemps = -1;
    TempNo = 0;
    }

struct _ptoc_FRAME_PasCerror
  {
  char dummy;
  } ;


int PasCerror(ptoc_ptr_message)
STRING ptoc_ptr_message;
    {
    struct _ptoc_FRAME_PasCerror _ptoc_LOCAL;
    int i;
    STRING message;
    
    _ptoc_MOVE(char, sizeof(STRING ), message, ptoc_ptr_message);
    i = 255;
    while ((int ) message[i] == 32)
        i += -1;
    message[i + 1] = (unsigned char)'\0';
    cerror(message);
    }

struct _ptoc_FRAME_BaseType
  {
  char dummy;
  } ;


int BaseType(typ)
SymbolTableRange typ;
    {
    struct _ptoc_FRAME_BaseType _ptoc_LOCAL;
    int _BaseType;
    
    if (typ == -1)
        {
        _BaseType = -1;
        }
    else
        {
        if (STab[typ].SKind == 16)
            {
            _BaseType = BaseType(STab[typ].SType);
            }
        else
            {
            _BaseType = typ;
            }
        }
    return(_BaseType);
    }

struct _ptoc_FRAME_CopyTree
  {
  char dummy;
  } ;


int CopyTree(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CopyTree _ptoc_LOCAL;
    int _CopyTree;
    HdgRange Tree;
    int maj;
    int typ;
    HdgRange SaveIndex;
    
    if (NodeIndex != -1)
        {
        switch (shr(field[NodeIndex] , 16))
            {
            case 1 :
                Tree = node3(field[NodeIndex] & 65535 , field[NodeIndex + 1] , field[NodeIndex + 5] , field[NodeIndex + 6]);
                break;
            case 4 :
                Tree = UnaryExpression(field[NodeIndex] & 65535 , field[NodeIndex + 1] , CopyTree(field[NodeIndex + 5]) , field[NodeIndex + 6] , field[NodeIndex + 7]);
                break;
            case 5 :
                Tree = BinaryExpression(field[NodeIndex] & 65535 , field[NodeIndex + 1] , CopyTree(field[NodeIndex + 5]) , CopyTree(field[NodeIndex + 6]));
                SetBinaryParameters(Tree , field[NodeIndex + 7] , field[NodeIndex + 8]);
                break;
            case 7 :
                Tree = LongStatement(field[NodeIndex] & 65535 , field[NodeIndex + 1] , CopyTree(field[NodeIndex + 5]) , CopyTree(field[NodeIndex + 6]) , field[NodeIndex + 7] , field[NodeIndex + 8] , field[NodeIndex + 9]);
                break;
            case 8 :
            case 9 :
            case 12 :
            case 10 :
                Tree = ComplexStmnt(shr(field[NodeIndex] , 16) , field[NodeIndex + 1] , CopyTree(field[NodeIndex + 5]) , CopyTree(field[NodeIndex + 6]) , CopyTree(field[NodeIndex + 7]) , field[NodeIndex + 8]);
                break;
            case 13 :
                Tree = ProcNode(13 , field[NodeIndex + 1] , CopyTree(field[NodeIndex + 5]) , CopyTree(field[NodeIndex + 6]) , field[NodeIndex + 7] , field[NodeIndex + 8]);
                break;
            case 11 :
                Tree = CaseStmnt(shr(field[NodeIndex] , 16) , field[NodeIndex] & 65535 , field[NodeIndex + 1] , field[NodeIndex + 5] , field[NodeIndex + 6] , field[NodeIndex + 9]);
                break;
            case 2 :
                Tree = special(field[NodeIndex] & 65535 , field[NodeIndex + 1] , field[NodeIndex + 5] , field[NodeIndex + 6]);
                break;
            case 3 :
                Tree = ShortStatement(shr(field[NodeIndex] , 16) , field[NodeIndex + 1] , CopyTree(field[NodeIndex + 5]) , field[NodeIndex + 6]);
                break;
            case 6 :
                Tree = MakeList(shr(field[NodeIndex] , 16) , field[NodeIndex + 1] , -1 , -1 , -1);
                maj = shr(field[NodeIndex] , 16);
                typ = field[NodeIndex + 1];
                SaveIndex = NodeIndex;
                NodeIndex = field[NodeIndex + 5];
                while (NodeIndex != -1)
                    {
                    AppendList(maj , typ , Tree , CopyTree(NodeIndex));
                    NodeIndex = field[NodeIndex + 3];
                    }
                NodeIndex = SaveIndex;
                break;
            }
        _CopyTree = Tree;
        }
    else
        {
        _CopyTree = NodeIndex;
        }
    return(_CopyTree);
    }

struct _ptoc_FRAME_demorgan
  {
  char dummy;
  } ;


int demorgan(NodeIndex)
int NodeIndex;
    {
    struct _ptoc_FRAME_demorgan _ptoc_LOCAL;
    int _demorgan;
    int op;
    int typ;
    int op1;
    int op2;
    
    switch (shr(field[NodeIndex] , 16))
        {
        case 5 :
            op = field[NodeIndex] & 65535;
            typ = field[NodeIndex + 1];
            op1 = field[NodeIndex + 5];
            op2 = field[NodeIndex + 6];
            switch (op)
                {
                case 25 :
                case 27 :
                    op = 52 - op;
                    op1 = demorgan(op1);
                    op2 = demorgan(op2);
                    _demorgan = BinaryExpression(op , typ , op1 , op2);
                    goto LAB_100;
                    break;
                case 31 :
                case 32 :
                case 33 :
                case 34 :
                case 35 :
                case 36 :
                    op = 67 - op;
                    _demorgan = BinaryExpression(op , typ , op1 , op2);
                    goto LAB_100;
                    break;
                default :
                    break;
                }
            break;
        case 4 :
            op = field[NodeIndex] & 65535;
            typ = field[NodeIndex + 1];
            op1 = field[NodeIndex + 5];
            if (op == 28)
                {
                _demorgan = op1;
                goto LAB_100;
                }
            break;
        case 1 :
            if ((field[NodeIndex] & 65535) == 12)
                {
                if (field[NodeIndex + 5] == 0)
                    {
                    field[NodeIndex + 5] = 1;
                    }
                else
                    {
                    field[NodeIndex + 5] = 0;
                    }
                _demorgan = NodeIndex;
                goto LAB_100;
                }
            break;
        default :
            break;
        }
    _demorgan = UnaryExpression(28 , field[NodeIndex + 1] , NodeIndex , 0 , 0);
    LAB_100 :
    return(_demorgan);
    }

struct _ptoc_FRAME_is_equal_set
  {
  char dummy;
  } ;


int is_equal_set(typ_1, typ_2)
SymbolTableRange typ_1;
SymbolTableRange typ_2;
    {
    struct _ptoc_FRAME_is_equal_set _ptoc_LOCAL;
    int _is_equal_set;
    
    if (STab[typ_1].SType == -16 && STab[typ_2].SType == -16)
        {
        TypeSize(typ_1);
        TypeSize(typ_2);
        if (STab[typ_1].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize == STab[typ_2].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize && STab[typ_1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange == STab[typ_2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange && STab[typ_1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange == STab[typ_2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange)
            {
            STab[typ_1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase = STab[typ_2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase;
            _is_equal_set = 1;
            }
        else
            {
            _is_equal_set = 0;
            }
        }
    else
        {
        _is_equal_set = 0;
        }
    return(_is_equal_set);
    }

struct _ptoc_FRAME_GetTemporary
  {
  char dummy;
  } ;


int GetTemporary(ProcedureName, TempType, TempList)
SymbolTableRange ProcedureName;
SymbolTableRange TempType;
int TempList;
    {
    struct _ptoc_FRAME_GetTemporary _ptoc_LOCAL;
    int _GetTemporary;
    SymbolTableRange i;
    SymbolTableRange j;
    int found;
    
    i = FreeTemps;
    j = -1;
    found = 0;
    while (!found && i != -1)
        if (STab[i].SType == TempType || is_equal_set(STab[i].SType , TempType))
            {
            found = 1;
            }
        else
            {
            j = i;
            i = STab[i].SName;
            }
    if (found)
        {
        if (j == -1)
            {
            FreeTemps = STab[i].SName;
            }
        else
            {
            STab[j].SName = STab[i].SName;
            }
        STab[i].SName = 0;
        _GetTemporary = i;
        }
    else
        {
        TempNo += 1;
        BumpSymPtr();
        STab[SymPtr].SName = 0;
        STab[SymPtr].SLink = -1;
        STab[SymPtr].SCard = -1;
        STab[SymPtr].SFile = 0;
        STab[SymPtr].SType = TempType;
        STab[SymPtr].SLevel = STab[ProcedureName].SLevel + 1;
        STab[SymPtr].SFlags = 0;
        STab[SymPtr].SKind = 9;
        STab[SymPtr].KINDTYPE.C_ModuleDcl.SClass = 1;
        STab[SymPtr].KINDTYPE.C_ModuleDcl.SSubClass = 0;
        STab[SymPtr].KINDTYPE.C_ModuleDcl.SParent = -1;
        STab[SymPtr].KINDTYPE.C_ModuleDcl.AddrInfo.KINDTYPE.C_CBlockDcl.SRegister = -1;
        STab[SymPtr].KINDTYPE.C_ModuleDcl.AddrInfo.KINDTYPE.C_TypeDcl.SOffset = -1;
        if (TempList)
            {
            if (STab[ProcedureName].KINDTYPE.C_BlockDcl.AllocInfo.KINDTYPE.C_BlockDcl.STempIndex == -1)
                {
                STab[ProcedureName].KINDTYPE.C_BlockDcl.AllocInfo.KINDTYPE.C_BlockDcl.STempIndex = shl(SymPtr , 4);
                }
            else
                {
                STab[ProcedureName].KINDTYPE.C_BlockDcl.AllocInfo.KINDTYPE.C_BlockDcl.STempIndex += 1;
                }
            }
        _GetTemporary = SymPtr;
        }
    return(_GetTemporary);
    }

struct _ptoc_FRAME_FreeTemporary
  {
  char dummy;
  } ;


int FreeTemporary(TempIndex)
SymbolTableRange TempIndex;
    {
    struct _ptoc_FRAME_FreeTemporary _ptoc_LOCAL;
    
    STab[TempIndex].SName = FreeTemps;
    FreeTemps = TempIndex;
    }

struct _ptoc_FRAME_ClearTemporarys
  {
  char dummy;
  } ;


int ClearTemporarys()
    {
    struct _ptoc_FRAME_ClearTemporarys _ptoc_LOCAL;
    SymbolTableRange i;
    SymbolTableRange j;
    
    i = FreeTemps;
    while (i != -1)
        {
        j = STab[i].SName;
        STab[i].SName = 0;
        i = j;
        }
    FreeTemps = -1;
    }

struct _ptoc_FRAME_BasisTyp
  {
  char dummy;
  } ;


int BasisTyp(index)
int index;
    {
    struct _ptoc_FRAME_BasisTyp _ptoc_LOCAL;
    int _BasisTyp;
    
    while (STab[index].SType > 0)
        index = STab[index].SType;
    _BasisTyp = STab[index].SType;
    return(_BasisTyp);
    }
