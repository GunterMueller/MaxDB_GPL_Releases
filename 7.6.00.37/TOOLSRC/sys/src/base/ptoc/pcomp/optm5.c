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
long int field[1];
int control[256];
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
  int SetElementType;
typedef
  int Set[256];
typedef
  Set *PSet;
typedef
  struct ptoc_Type2
    {
    PSet ref;
    PSet def;
    int continue_;
    int break_;
    int return_;
    int goto_label;
    int hash_code;
    } NodeDescriptorType;
typedef
  struct NodeDescriptorRecord *DescriptorRange;
struct NodeDescriptorRecord
  {
  DescriptorRange prev;
  DescriptorRange next;
  int essential;
  NodeDescriptorType ExitDescriptor;
  NodeDescriptorType EntryDescriptor;
  } ;
typedef
  unsigned TraversalType;
HdgRange ExcludedNode;
int gl_InitSym();
int PrintDescriptor();
int ListDescTab();
DescriptorRange AvailableDescriptor();
int FreeDescriptor();
int ClearDescriptor();
int ClearDescriptorFull();
int ClearEntryDescriptors();
int ClearExitDescriptors();
int UnionDescriptors();
int AssignDescriptors();
int EqualDescriptors();
int DescriptorFlags();
int IsOptimizible();
int IsLvalOp();
int CreateAddrOfLval();
int CreateAssignmentStatement();
int CreateLoadNode();
int Optm2I();
int BuildExitDescriptors();
int ExitTraverse();
int DeallocateCsgDsc();
int DeallocateDsc();
int BuildEntryDescriptors();
int EntryTraverse();
int Union_Sets();
int Intersect_Sets();
int Insert_val_into_Set();
int Member_in_Set();
int Assign_Set_to_Set();
int Empty_check_Set();
int Equal_two_Sets();
int Clear_this_Set();
int ClearFull();
int Fill_whole_Set();
int PrintSet();
int give_MrX();
int print_name();
int c_freopen();
typedef
  unsigned TransformType;
int identical();
int ConditionsSatisfied();

struct _ptoc_FRAME_IdenticalTypes
  {
  char dummy;
  } ;


int IdenticalTypes(type1, type2)
SymbolTableRange type1;
SymbolTableRange type2;
    {
    struct _ptoc_FRAME_IdenticalTypes _ptoc_LOCAL;
    int _IdenticalTypes;
    SymbolTableRange stype1;
    SymbolTableRange stype2;
    
    if (type1 == type2)
        {
        _IdenticalTypes = 1;
        }
    else
        {
        if (STab[type1].SKind == STab[type2].SKind)
            {
            stype1 = STab[type1].SType;
            stype2 = STab[type2].SType;
            if (stype1 < 0 && stype1 == stype2)
                {
                switch (stype1)
                    {
                    case -1 :
                    case -2 :
                    case -5 :
                    case -6 :
                    case -7 :
                    case -8 :
                    case -9 :
                    case -10 :
                        _IdenticalTypes = STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange == STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange && STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange == STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange;
                        break;
                    case -3 :
                    case -4 :
                    case -11 :
                    case -12 :
                        _IdenticalTypes = 1;
                        break;
                    case -13 :
                        _IdenticalTypes = IdenticalTypes(STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex , STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SIndex) && IdenticalTypes(STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement , STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_ArrayType.SElement);
                        break;
                    case -14 :
                    case -19 :
                        _IdenticalTypes = 0;
                        break;
                    case -15 :
                        _IdenticalTypes = IdenticalTypes(STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent , STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_FileType.SComponent);
                        break;
                    case -16 :
                        _IdenticalTypes = IdenticalTypes(STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase , STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SetType.SBase);
                        break;
                    case -17 :
                        _IdenticalTypes = 1;
                        break;
                    case -18 :
                    case -23 :
                        _IdenticalTypes = 0;
                        break;
                    case -20 :
                        _IdenticalTypes = STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SLength == STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SLength && IdenticalTypes(STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SElem , STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SElem);
                        break;
                    case -22 :
                        _IdenticalTypes = STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_StringType.SLength == STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_BlockType.SBlockLen && STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_BlockType.SElemAlign == STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_BlockType.SElemAlign;
                        break;
                    default :
                        PasCerror("missing case - IdenticalTypes                                                                                                                                                                                                                                   ");
                        break;
                    }
                }
            else
                {
                if (stype1 > 0 && stype2 > 0)
                    {
                    if (STab[type1].SKind == 7)
                        {
                        _IdenticalTypes = STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange == STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange && STab[type1].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange == STab[type2].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange && IdenticalTypes(stype1 , stype2);
                        }
                    else
                        {
                        _IdenticalTypes = IdenticalTypes(stype1 , stype2);
                        }
                    }
                else
                    {
                    _IdenticalTypes = 0;
                    }
                }
            }
        else
            {
            _IdenticalTypes = 0;
            }
        }
    return(_IdenticalTypes);
    }

struct _ptoc_FRAME_IdenticalTraverse
  {
  char dummy;
  } ;


int IdenticalTraverse(NodeIndex, CnodeIndex)
HdgRange NodeIndex;
HdgRange CnodeIndex;
    {
    struct _ptoc_FRAME_IdenticalTraverse _ptoc_LOCAL;
    int _IdenticalTraverse;
    int StillIdentical;
    
    StillIdentical = 1;
    while (StillIdentical && NodeIndex != -1 && CnodeIndex != -1)
        {
        StillIdentical = StillIdentical && identical(NodeIndex , CnodeIndex);
        NodeIndex = field[NodeIndex + 3];
        CnodeIndex = field[CnodeIndex + 3];
        }
    _IdenticalTraverse = StillIdentical && identical(NodeIndex , CnodeIndex);
    return(_IdenticalTraverse);
    }

struct _ptoc_FRAME_identical
  {
  char dummy;
  } ;


int identical(NodeIndex, CnodeIndex)
HdgRange NodeIndex;
HdgRange CnodeIndex;
    {
    struct _ptoc_FRAME_identical _ptoc_LOCAL;
    int _identical;
    int ReturnValue;
    _iorechd *ptoc_Var0;
    
    if (control[86])
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "");
        sql__writeln(ptoc_Var0);
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s%*d%s%*d" , "identical, " , 1 , NodeIndex , ", " , 1 , CnodeIndex);
        sql__writeln(ptoc_Var0);
        }
    if (NodeIndex == -1 || CnodeIndex == -1)
        {
        ReturnValue = NodeIndex == CnodeIndex;
        }
    else
        {
        if (field[NodeIndex] == field[CnodeIndex] && IdenticalTypes(field[NodeIndex + 1] , field[CnodeIndex + 1]))
            {
            switch (shr(field[NodeIndex] , 16))
                {
                case 1 :
                    if (field[NodeIndex + 5] == field[CnodeIndex + 5])
                        {
                        ReturnValue = field[NodeIndex + 6] == field[CnodeIndex + 6];
                        }
                    else
                        {
                        ReturnValue = 0;
                        }
                    break;
                case 2 :
                    ReturnValue = field[NodeIndex + 5] == field[CnodeIndex + 5];
                    break;
                case 3 :
                    ReturnValue = identical(field[NodeIndex + 5] , field[CnodeIndex + 5]);
                    break;
                case 4 :
                    if (field[NodeIndex + 6] == field[CnodeIndex + 6] && field[NodeIndex + 7] == field[CnodeIndex + 7])
                        {
                        ReturnValue = identical(field[NodeIndex + 5] , field[CnodeIndex + 5]);
                        }
                    else
                        {
                        ReturnValue = 0;
                        }
                    break;
                case 5 :
                    if (field[NodeIndex + 7] == field[CnodeIndex + 7] && field[NodeIndex + 8] == field[CnodeIndex + 8] && identical(field[NodeIndex + 5] , field[CnodeIndex + 5]))
                        {
                        ReturnValue = identical(field[NodeIndex + 6] , field[CnodeIndex + 6]);
                        }
                    else
                        {
                        ReturnValue = 0;
                        }
                    break;
                case 6 :
                    ReturnValue = IdenticalTraverse(field[NodeIndex + 5] , field[CnodeIndex + 5]);
                    break;
                case 7 :
                    if (field[NodeIndex + 7] == field[CnodeIndex + 7] && field[NodeIndex + 8] == field[CnodeIndex + 8] && identical(field[NodeIndex + 6] , field[CnodeIndex + 6]))
                        {
                        ReturnValue = identical(field[NodeIndex + 5] , field[CnodeIndex + 5]);
                        }
                    else
                        {
                        ReturnValue = 0;
                        }
                    break;
                case 8 :
                    ReturnValue = identical(field[NodeIndex + 5] , field[CnodeIndex + 5]) && identical(field[NodeIndex + 6] , field[CnodeIndex + 6]) && identical(field[NodeIndex + 7] , field[CnodeIndex + 7]);
                    break;
                default :
                    ReturnValue = 0;
                    break;
                }
            }
        else
            {
            ReturnValue = 0;
            }
        }
    if (control[86])
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s%*d" , "   ReturnValue = " , 1 , sql__ord((int ) ReturnValue));
        sql__writeln(ptoc_Var0);
        }
    _identical = ReturnValue;
    return(_identical);
    }

struct _ptoc_FRAME_ConditionsSatisfied
  {
  HdgRange NodeIndex;
  HdgRange FirstStmnt;
  int *BuiltEntry;
  DescriptorRange DescPtr;
  DescriptorRange DescPtr2;
  DescriptorRange DescPtr3;
  PSet intersection;
  NodeDescriptorType ExitDesc;
  NodeDescriptorType EntryDesc;
  HdgRange SaveNodeIndex;
  int ReturnValue;
  char dummy;
  } ;


struct _ptoc_FRAME_MoveBackwards_ConditionsSatisfied
  {
  struct _ptoc_FRAME_ConditionsSatisfied *_ptoc_LINK;
  char dummy;
  } ;


static
int MoveBackwards_ConditionsSatisfied(_ptoc_LINK)
struct _ptoc_FRAME_ConditionsSatisfied *_ptoc_LINK;
    {
    struct _ptoc_FRAME_MoveBackwards_ConditionsSatisfied _ptoc_LOCAL;
    _iorechd *ptoc_Var1;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[86])
        {
        ptoc_Var1 = sql__ut(output);
        fprintf(ptoc_Var1->_fbuf , "%s" , "MoveBackwards: ");
        ptoc_Var1 = sql__ut(output);
        fprintf(ptoc_Var1->_fbuf , "%s%*d%s" , "NodeIndex = " , 1 , _ptoc_LOCAL._ptoc_LINK->NodeIndex , ", ");
        ptoc_Var1 = sql__ut(output);
        fprintf(ptoc_Var1->_fbuf , "%s%*d" , "essential = " , 1 , sql__ord((int ) (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).essential));
        sql__writeln(ptoc_Var1);
        }
    if ((*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).essential)
        {
        if (!*_ptoc_LOCAL._ptoc_LINK->BuiltEntry)
            {
            ClearEntryDescriptors();
            EntryTraverse(_ptoc_LOCAL._ptoc_LINK->FirstStmnt , -1);
            *_ptoc_LOCAL._ptoc_LINK->BuiltEntry = 1;
            }
        if (control[86])
            {
            PrintDescriptor(-1 , "entry                                                                                                                                                                                                                                                           " , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).EntryDescriptor);
            PrintDescriptor(-1 , "exit                                                                                                                                                                                                                                                            " , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor);
            }
        Intersect_Sets(_ptoc_LOCAL._ptoc_LINK->intersection , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).EntryDescriptor.def , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.ref);
        if (Empty_check_Set(_ptoc_LOCAL._ptoc_LINK->intersection))
            {
            Intersect_Sets(_ptoc_LOCAL._ptoc_LINK->intersection , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).EntryDescriptor.def , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.def);
            if (Empty_check_Set(_ptoc_LOCAL._ptoc_LINK->intersection))
                {
                Intersect_Sets(_ptoc_LOCAL._ptoc_LINK->intersection , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).EntryDescriptor.ref , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.def);
                _ptoc_LOCAL._ptoc_LINK->ReturnValue = Empty_check_Set(_ptoc_LOCAL._ptoc_LINK->intersection);
                }
            }
        }
    }

struct _ptoc_FRAME_MoveOutsideLoop_ConditionsSatisfied
  {
  struct _ptoc_FRAME_ConditionsSatisfied *_ptoc_LINK;
  char dummy;
  } ;


static
int MoveOutsideLoop_ConditionsSatisfied(_ptoc_LINK)
struct _ptoc_FRAME_ConditionsSatisfied *_ptoc_LINK;
    {
    struct _ptoc_FRAME_MoveOutsideLoop_ConditionsSatisfied _ptoc_LOCAL;
    _iorechd *ptoc_Var2;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[86])
        {
        ptoc_Var2 = sql__ut(output);
        fprintf(ptoc_Var2->_fbuf , "%s" , "MoveStmntOutsideLoop:");
        sql__writeln(ptoc_Var2);
        }
    if ((*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).essential && (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr3).essential)
        {
        Intersect_Sets(_ptoc_LOCAL._ptoc_LINK->intersection , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.def , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.ref);
        if (Empty_check_Set(_ptoc_LOCAL._ptoc_LINK->intersection))
            {
            _ptoc_LOCAL._ptoc_LINK->SaveNodeIndex = _ptoc_LOCAL._ptoc_LINK->NodeIndex;
            _ptoc_LOCAL._ptoc_LINK->NodeIndex = _ptoc_LOCAL._ptoc_LINK->FirstStmnt;
            _ptoc_LOCAL._ptoc_LINK->DescPtr2 = (struct NodeDescriptorRecord *) (int *) field[_ptoc_LOCAL._ptoc_LINK->NodeIndex + 2];
            if (control[86])
                {
                ptoc_Var2 = sql__ut(output);
                fprintf(ptoc_Var2->_fbuf , "%s" , "defsp(n0,nj) * ref(nj) = 0");
                sql__writeln(ptoc_Var2);
                PrintDescriptor(_ptoc_LOCAL._ptoc_LINK->SaveNodeIndex , "defsp                                                                                                                                                                                                                                                           " , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr2).ExitDescriptor);
                PrintDescriptor(_ptoc_LOCAL._ptoc_LINK->SaveNodeIndex , "exit                                                                                                                                                                                                                                                            " , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor);
                }
            Intersect_Sets(_ptoc_LOCAL._ptoc_LINK->intersection , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr2).ExitDescriptor.def , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.ref);
            if (Empty_check_Set(_ptoc_LOCAL._ptoc_LINK->intersection))
                {
                ExcludedNode = _ptoc_LOCAL._ptoc_LINK->SaveNodeIndex;
                if (!*_ptoc_LOCAL._ptoc_LINK->BuiltEntry)
                    {
                    ExitTraverse(field[_ptoc_LOCAL._ptoc_LINK->NodeIndex + 6] , -1 , 1 , &_ptoc_LOCAL._ptoc_LINK->EntryDesc);
                    ExitTraverse(field[_ptoc_LOCAL._ptoc_LINK->NodeIndex + 5] , -1 , 0 , &_ptoc_LOCAL._ptoc_LINK->ExitDesc);
                    *_ptoc_LOCAL._ptoc_LINK->BuiltEntry = 1;
                    }
                UnionDescriptors(&_ptoc_LOCAL._ptoc_LINK->EntryDesc , &_ptoc_LOCAL._ptoc_LINK->EntryDesc , &_ptoc_LOCAL._ptoc_LINK->ExitDesc);
                ExcludedNode = -1;
                if (control[86])
                    {
                    ptoc_Var2 = sql__ut(output);
                    fprintf(ptoc_Var2->_fbuf , "%s" , "defex(n0,nj:nj) * def(nj) = 0");
                    sql__writeln(ptoc_Var2);
                    PrintDescriptor(_ptoc_LOCAL._ptoc_LINK->SaveNodeIndex , "defex                                                                                                                                                                                                                                                           " , &_ptoc_LOCAL._ptoc_LINK->EntryDesc);
                    }
                Intersect_Sets(_ptoc_LOCAL._ptoc_LINK->intersection , _ptoc_LOCAL._ptoc_LINK->EntryDesc.def , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.def);
                if (Empty_check_Set(_ptoc_LOCAL._ptoc_LINK->intersection))
                    {
                    ClearEntryDescriptors();
                    EntryTraverse(field[_ptoc_LOCAL._ptoc_LINK->NodeIndex + 6] , -1);
                    if (control[86])
                        {
                        ptoc_Var2 = sql__ut(output);
                        fprintf(ptoc_Var2->_fbuf , "%s" , "def(nj) * refinc(n0,nj) = 0");
                        sql__writeln(ptoc_Var2);
                        PrintDescriptor(_ptoc_LOCAL._ptoc_LINK->SaveNodeIndex , "refinc                                                                                                                                                                                                                                                          " , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).EntryDescriptor);
                        }
                    Intersect_Sets(_ptoc_LOCAL._ptoc_LINK->intersection , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.def , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).EntryDescriptor.ref);
                    if (Empty_check_Set(_ptoc_LOCAL._ptoc_LINK->intersection))
                        {
                        _ptoc_LOCAL._ptoc_LINK->ReturnValue = 1;
                        }
                    }
                }
            }
        }
    }

struct _ptoc_FRAME_NonModify_ConditionsSatisfied
  {
  struct _ptoc_FRAME_ConditionsSatisfied *_ptoc_LINK;
  char dummy;
  } ;


static
int NonModify_ConditionsSatisfied(_ptoc_LINK)
struct _ptoc_FRAME_ConditionsSatisfied *_ptoc_LINK;
    {
    struct _ptoc_FRAME_NonModify_ConditionsSatisfied _ptoc_LOCAL;
    _iorechd *ptoc_Var3;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (control[86])
        {
        ptoc_Var3 = sql__ut(output);
        fprintf(ptoc_Var3->_fbuf , "%s" , "NonModify: ");
        ptoc_Var3 = sql__ut(output);
        fprintf(ptoc_Var3->_fbuf , "%s%*d%s" , "NodeIndex = " , 1 , _ptoc_LOCAL._ptoc_LINK->NodeIndex , ", ");
        sql__writeln(ptoc_Var3);
        ptoc_Var3 = sql__ut(output);
        fprintf(ptoc_Var3->_fbuf , "%s%*d" , "essential = " , 1 , sql__ord((int ) (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).essential));
        sql__writeln(ptoc_Var3);
        }
    Intersect_Sets(_ptoc_LOCAL._ptoc_LINK->intersection , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.def , (*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).ExitDescriptor.ref);
    _ptoc_LOCAL._ptoc_LINK->ReturnValue = Empty_check_Set(_ptoc_LOCAL._ptoc_LINK->intersection);
    }

int ConditionsSatisfied(transformation, NodeIndex, FirstStmnt, BuiltEntry)
TransformType transformation;
HdgRange NodeIndex;
HdgRange FirstStmnt;
int *BuiltEntry;
    {
    struct _ptoc_FRAME_ConditionsSatisfied _ptoc_LOCAL;
    int _ConditionsSatisfied;
    Set intersect;
    Set set1;
    Set set2;
    Set set3;
    Set set4;
    _iorechd *ptoc_Var4;
    
    _ptoc_LOCAL.NodeIndex = NodeIndex;
    _ptoc_LOCAL.FirstStmnt = FirstStmnt;
    _ptoc_LOCAL.BuiltEntry = BuiltEntry;
    _ptoc_LOCAL.intersection = (Set *) intersect;
    _ptoc_LOCAL.ExitDesc.ref = (Set *) set1;
    _ptoc_LOCAL.ExitDesc.def = (Set *) set2;
    _ptoc_LOCAL.EntryDesc.ref = (Set *) set3;
    _ptoc_LOCAL.EntryDesc.def = (Set *) set4;
    if (control[86])
        {
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "");
        sql__writeln(ptoc_Var4);
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "%s%*d%s%*d" , "ConditionsSatisified, " , 1 , _ptoc_LOCAL.NodeIndex , ", " , 1 , _ptoc_LOCAL.FirstStmnt);
        sql__writeln(ptoc_Var4);
        }
    Clear_this_Set(_ptoc_LOCAL.intersection);
    _ptoc_LOCAL.DescPtr = (struct NodeDescriptorRecord *) (int *) field[_ptoc_LOCAL.NodeIndex + 2];
    _ptoc_LOCAL.DescPtr3 = (struct NodeDescriptorRecord *) (int *) field[_ptoc_LOCAL.FirstStmnt + 2];
    _ptoc_LOCAL.ReturnValue = 0;
    switch ((int ) transformation)
        {
        case 0 :
            MoveBackwards_ConditionsSatisfied(&_ptoc_LOCAL);
            break;
        case 1 :
            MoveOutsideLoop_ConditionsSatisfied(&_ptoc_LOCAL);
            break;
        case 2 :
            NonModify_ConditionsSatisfied(&_ptoc_LOCAL);
            break;
        }
    if (control[86])
        {
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "%s%*d" , "   ReturnValue = " , 1 , sql__ord((int ) _ptoc_LOCAL.ReturnValue));
        sql__writeln(ptoc_Var4);
        }
    _ConditionsSatisfied = _ptoc_LOCAL.ReturnValue;
    return(_ConditionsSatisfied);
    }
