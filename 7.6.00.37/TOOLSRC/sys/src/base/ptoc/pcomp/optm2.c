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
  int TableRange;
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
  struct ptoc_Type3
    {
    SymbolTableRange SymTabInx;
    NodeDescriptorType ProcExitDescr;
    } ProcTableArray[101];
typedef
  unsigned TraversalType;
int OptimizedIf;
int OptimizedLoop;
int ExprFlag;
int StmntFlag;
ProcTableArray ProcTable;
int StmntChain[1000];
int StmntLink[1000];
int ExprChain[1000];
int ExprLink[1000];
TableRange ExprStmnt[1000];
TableRange NoStmnt;
TableRange NoExpr;
HdgRange ExcludedNode;
HdgRange IncludedNode;
HdgRange OptimizeNode;
int NoProc;
int NoNodesTraversed;
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
int EssentialBit;
int Optm2I();
int BuildExitDescriptors();
int ExitTraverse();
int DeallocateCsgDsc();
int DeallocateDsc();

struct _ptoc_FRAME_Optm2I
  {
  char dummy;
  } ;


int Optm2I()
    {
    struct _ptoc_FRAME_Optm2I _ptoc_LOCAL;
    
    }

struct _ptoc_FRAME_AddToListOfExpressions
  {
  char dummy;
  } ;


int AddToListOfExpressions(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_AddToListOfExpressions _ptoc_LOCAL;
    
    if (ExprFlag && NoExpr < 1000 && NoStmnt < 1000 && IsOptimizible(NodeIndex) && NoStmnt != 0)
        {
        NoExpr += 1;
        ExprChain[NoExpr + -1] = NodeIndex;
        ExprLink[NoExpr + -1] = LinkNode;
        ExprStmnt[NoExpr + -1] = NoStmnt;
        }
    }

struct _ptoc_FRAME_AddToListOfStatements
  {
  char dummy;
  } ;


int AddToListOfStatements(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_AddToListOfStatements _ptoc_LOCAL;
    
    if (StmntFlag && NoStmnt < 1000)
        {
        NoStmnt += 1;
        StmntChain[NoStmnt + -1] = NodeIndex;
        StmntLink[NoStmnt + -1] = LinkNode;
        }
    }

struct _ptoc_FRAME_FindBase
  {
  char dummy;
  } ;


int FindBase(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_FindBase _ptoc_LOCAL;
    int _FindBase;
    
    while (NodeIndex != -1)
        switch (field[NodeIndex] & 65535)
            {
            case 12 :
                _FindBase = -2;
                NodeIndex = -1;
                break;
            case 94 :
            case 98 :
                _FindBase = field[NodeIndex + 5];
                NodeIndex = -1;
                break;
            case 95 :
            case 96 :
                NodeIndex = field[NodeIndex + 5];
                break;
            case 97 :
                _FindBase = -1;
                NodeIndex = -1;
                break;
            default :
                PasCerror("illegal tree - FindBase                                                                                                                                                                                                                                         ");
                break;
            }
    return(_FindBase);
    }

struct _ptoc_FRAME_MakeBaseSet
  {
  char dummy;
  } ;


int MakeBaseSet(NodeIndex, TempSet)
HdgRange NodeIndex;
PSet TempSet;
    {
    struct _ptoc_FRAME_MakeBaseSet _ptoc_LOCAL;
    SymbolTableRange sym;
    
    sym = FindBase(NodeIndex);
    if (sym != -1)
        {
        Clear_this_Set(TempSet);
        if (sym != -2)
            {
            if (STab[sym].SKind != 18)
                {
                Insert_val_into_Set(sym , TempSet);
                }
            else
                {
                Insert_val_into_Set(STab[sym].KINDTYPE.C_ModuleDcl.AddrInfo.KINDTYPE.C_CBlockDcl.SRegister , TempSet);
                }
            }
        }
    else
        {
        Fill_whole_Set(TempSet);
        }
    }

struct _ptoc_FRAME_ConstructDesc
  {
  char dummy;
  } ;


int ConstructDesc(NodeIndex, ExitDesc1)
HdgRange NodeIndex;
NodeDescriptorType *ExitDesc1;
    {
    struct _ptoc_FRAME_ConstructDesc _ptoc_LOCAL;
    int i;
    int inx;
    SymbolTableRange sym;
    int ProcLevel;
    PSet TempSet;
    Set temp_set;
    
    TempSet = (Set *) temp_set;
    ClearDescriptor(&(*ExitDesc1));
    switch (field[NodeIndex] & 65535)
        {
        case 4 :
            MakeBaseSet(field[NodeIndex + 5] , TempSet);
            Union_Sets((*ExitDesc1).ref , (*ExitDesc1).ref , TempSet);
            break;
        case 6 :
            MakeBaseSet(field[NodeIndex + 5] , TempSet);
            Union_Sets((*ExitDesc1).def , (*ExitDesc1).def , TempSet);
            break;
        case 8 :
            if (STab[field[NodeIndex + 1]].SType == -17)
                {
                NodeIndex = field[NodeIndex + 5];
                if ((field[NodeIndex] & 65535) == 13)
                    {
                    MakeBaseSet(field[NodeIndex + 5] , TempSet);
                    Union_Sets((*ExitDesc1).ref , (*ExitDesc1).ref , TempSet);
                    Union_Sets((*ExitDesc1).def , (*ExitDesc1).def , TempSet);
                    }
                else
                    {
                    if ((field[NodeIndex] & 65535) != 14)
                        {
                        Fill_whole_Set(TempSet);
                        Union_Sets((*ExitDesc1).ref , (*ExitDesc1).ref , TempSet);
                        Union_Sets((*ExitDesc1).def , (*ExitDesc1).def , TempSet);
                        }
                    }
                }
            break;
        case 9 :
            MakeBaseSet(field[NodeIndex + 5] , TempSet);
            Union_Sets((*ExitDesc1).ref , (*ExitDesc1).ref , TempSet);
            Union_Sets((*ExitDesc1).def , (*ExitDesc1).def , TempSet);
            break;
        case 86 :
            (*ExitDesc1).continue_ = 1;
            EssentialBit = 0;
            break;
        case 101 :
            (*ExitDesc1).break_ = 1;
            EssentialBit = 0;
            break;
        case 42 :
        case 43 :
            (*ExitDesc1).return_ = 1;
            EssentialBit = 0;
            break;
        case 87 :
        case 98 :
            (*ExitDesc1).goto_label = 1;
            EssentialBit = 0;
            break;
        case 40 :
            sym = FindBase(field[NodeIndex + 5]);
            if (sym > 0)
                {
                inx = 0;
                while (ProcTable[inx].SymTabInx != sym && inx <= 100)
                    inx += 1;
                if (control[67] && inx <= 100 && inx != NoProc)
                    {
                    AssignDescriptors(&(*ExitDesc1) , &ProcTable[inx].ProcExitDescr);
                    }
                else
                    {
                    ProcLevel = STab[sym].SLevel;
                    i = 1;
                    while (i <= SymPtr)
                        {
                        if (STab[i].SLevel <= ProcLevel && (STab[i].SKind == 9 || STab[i].SKind == 14))
                            {
                            Insert_val_into_Set(i , (*ExitDesc1).ref);
                            Insert_val_into_Set(i , (*ExitDesc1).def);
                            }
                        i = STab[i].SNext;
                        }
                    }
                }
            else
                {
                Fill_whole_Set((*ExitDesc1).ref);
                Fill_whole_Set((*ExitDesc1).def);
                }
            break;
        case 62 :
            Insert_val_into_Set(0 , (*ExitDesc1).ref);
            Insert_val_into_Set(0 , (*ExitDesc1).def);
            break;
        case 102 :
        case 103 :
            MakeBaseSet(field[NodeIndex + 5] , TempSet);
            Union_Sets((*ExitDesc1).ref , (*ExitDesc1).ref , TempSet);
            Union_Sets((*ExitDesc1).def , (*ExitDesc1).def , TempSet);
            break;
        case 106 :
            i = 1;
            while (i <= SymPtr)
                {
                Fill_whole_Set((*ExitDesc1).ref);
                Fill_whole_Set((*ExitDesc1).def);
                i = STab[i].SNext;
                }
            (*ExitDesc1).continue_ = 1;
            (*ExitDesc1).break_ = 1;
            (*ExitDesc1).return_ = 1;
            (*ExitDesc1).goto_label = 1;
            EssentialBit = 0;
            break;
        }
    }

struct _ptoc_FRAME_BuildExitDescriptors
  {
  char dummy;
  } ;


int BuildExitDescriptors(NodeIndex, LinkNode, traversal, ExitDesc1)
HdgRange NodeIndex;
HdgRange LinkNode;
TraversalType traversal;
NodeDescriptorType *ExitDesc1;
    {
    struct _ptoc_FRAME_BuildExitDescriptors _ptoc_LOCAL;
    NodeDescriptorType ExitDesc2;
    NodeDescriptorType ExitDesc3;
    Set set1;
    Set set2;
    Set set3;
    Set set4;
    DescriptorRange DescPtr;
    HdgRange CnodeIndex;
    int SaveEssentialBit;
    
    ExitDesc2.ref = (Set *) set1;
    ExitDesc2.def = (Set *) set2;
    ExitDesc3.ref = (Set *) set3;
    ExitDesc3.def = (Set *) set4;
    SaveEssentialBit = EssentialBit;
    if (NodeIndex == ExcludedNode || NodeIndex == -1)
        {
        ClearDescriptor(&(*ExitDesc1));
        goto LAB_1;
        }
    switch (shr(field[NodeIndex] , 16))
        {
        case 1 :
            ConstructDesc(NodeIndex , &(*ExitDesc1));
            (*ExitDesc1).hash_code = field[NodeIndex + 5];
            break;
        case 2 :
            ConstructDesc(NodeIndex , &(*ExitDesc1));
            (*ExitDesc1).hash_code += 1111;
            break;
        case 3 :
            AddToListOfStatements(NodeIndex , LinkNode);
            ConstructDesc(NodeIndex , &(*ExitDesc1));
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 1 , &ExitDesc2);
            (*ExitDesc1).hash_code = ExitDesc2.hash_code + 2222;
            UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
            break;
        case 4 :
            AddToListOfExpressions(NodeIndex , LinkNode);
            ConstructDesc(NodeIndex , &(*ExitDesc1));
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0 , &ExitDesc2);
            (*ExitDesc1).hash_code = ExitDesc2.hash_code + 3333;
            UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
            break;
        case 5 :
            AddToListOfExpressions(NodeIndex , LinkNode);
            ConstructDesc(NodeIndex , &(*ExitDesc1));
            if ((field[NodeIndex] & 65535) == 95)
                {
                EssentialBit = 0;
                }
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0 , &ExitDesc2);
            EssentialBit = SaveEssentialBit;
            if ((field[NodeIndex] & 65535) == 27 || (field[NodeIndex] & 65535) == 25)
                {
                EssentialBit = 0;
                }
            ExitTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 0 , &ExitDesc3);
            (*ExitDesc1).hash_code = ExitDesc2.hash_code + ExitDesc3.hash_code + 4444;
            UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
            UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc3);
            break;
        case 6 :
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0 , &(*ExitDesc1));
            break;
        case 7 :
            if (traversal == 1)
                {
                AddToListOfStatements(NodeIndex , LinkNode);
                }
            else
                {
                AddToListOfExpressions(NodeIndex , LinkNode);
                }
            ConstructDesc(NodeIndex , &(*ExitDesc1));
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0 , &ExitDesc2);
            ExitTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 0 , &ExitDesc3);
            (*ExitDesc1).hash_code = ExitDesc2.hash_code + ExitDesc3.hash_code + 5555;
            UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
            UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc3);
            break;
        case 11 :
            break;
        case 8 :
            if (traversal == 1)
                {
                AddToListOfStatements(NodeIndex , LinkNode);
                }
            else
                {
                AddToListOfExpressions(NodeIndex , LinkNode);
                }
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0 , &(*ExitDesc1));
            if (OptimizedIf && traversal == 1 && ExcludedNode == -1 && NodeIndex != OptimizeNode)
                {
                DescPtr = (struct NodeDescriptorRecord *) field[NodeIndex + 2];
                AssignDescriptors(&(*ExitDesc1) , &(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor);
                }
            else
                {
                EssentialBit = 0;
                ExitTraverse(field[NodeIndex + 6] , NodeIndex + 6 , traversal , &ExitDesc2);
                if (field[NodeIndex + 7] != -1)
                    {
                    ExitTraverse(field[NodeIndex + 7] , NodeIndex + 7 , traversal , &ExitDesc3);
                    (*ExitDesc1).hash_code = ExitDesc2.hash_code + ExitDesc3.hash_code + 6666;
                    UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
                    UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc3);
                    }
                else
                    {
                    (*ExitDesc1).hash_code = ExitDesc2.hash_code + 7777;
                    UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
                    }
                }
            break;
        case 9 :
            if (OptimizedLoop && ExcludedNode == -1 && NodeIndex != OptimizeNode)
                {
                AddToListOfStatements(NodeIndex , LinkNode);
                DescPtr = (struct NodeDescriptorRecord *) field[NodeIndex + 2];
                AssignDescriptors(&(*ExitDesc1) , &(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor);
                }
            else
                {
                AddToListOfStatements(NodeIndex , LinkNode);
                ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0 , &(*ExitDesc1));
                ExitTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 1 , &ExitDesc2);
                ExitDesc2.continue_ = 0;
                ExitDesc2.break_ = 0;
                (*ExitDesc1).hash_code = (*ExitDesc1).hash_code + ExitDesc2.hash_code + 8888;
                UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
                }
            break;
        case 10 :
            AddToListOfStatements(NodeIndex , LinkNode);
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0 , &(*ExitDesc1));
            EssentialBit = 0;
            ExitTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 1 , &ExitDesc2);
            ExitDesc2.break_ = 0;
            (*ExitDesc1).hash_code = (*ExitDesc1).hash_code + ExitDesc2.hash_code + 9999;
            UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
            break;
        case 12 :
            if (OptimizedLoop && ExcludedNode == -1 && NodeIndex != OptimizeNode)
                {
                AddToListOfStatements(NodeIndex , LinkNode);
                DescPtr = (struct NodeDescriptorRecord *) field[NodeIndex + 2];
                AssignDescriptors(&(*ExitDesc1) , &(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor);
                }
            else
                {
                ExitTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 1 , &(*ExitDesc1));
                (*ExitDesc1).continue_ = 0;
                EssentialBit = SaveEssentialBit && !DescriptorFlags(&(*ExitDesc1));
                AddToListOfStatements(NodeIndex , LinkNode);
                ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0 , &ExitDesc2);
                (*ExitDesc1).break_ = 0;
                (*ExitDesc1).hash_code = (*ExitDesc1).hash_code + ExitDesc2.hash_code + 11111;
                UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
                }
            break;
        case 13 :
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 1 , &(*ExitDesc1));
            (*ExitDesc1).return_ = 0;
            (*ExitDesc1).goto_label = 0;
            break;
        case 14 :
            ExitTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 1 , &(*ExitDesc1));
            (*ExitDesc1).return_ = 0;
            (*ExitDesc1).goto_label = 0;
            break;
        }
    EssentialBit = SaveEssentialBit && !DescriptorFlags(&(*ExitDesc1));
    if (shr(field[NodeIndex] , 16) != 1)
        {
        if (ExcludedNode == -1)
            {
            if (IncludedNode == -1 || NodeIndex == IncludedNode)
                {
                if ((int *) field[NodeIndex + 2] != (int *) 0)
                    {
                    DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
                    }
                else
                    {
                    DescPtr = AvailableDescriptor();
                    field[NodeIndex + 2] = sql__ord(DescPtr);
                    }
                AssignDescriptors(&(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor , &(*ExitDesc1));
                (*(struct NodeDescriptorRecord *) DescPtr).essential = EssentialBit;
                }
            else
                {
                if (field[NodeIndex + 2] != sql__ord((int *) 0))
                    {
                    FreeDescriptor((struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2]);
                    field[NodeIndex + 2] = sql__ord((int *) 0);
                    }
                }
            }
        if (control[86])
            {
            PrintDescriptor(NodeIndex , "exit                                                                                                                                                                                                                                                            " , &(*ExitDesc1));
            }
        }
    LAB_1 :
    ;
    }

struct _ptoc_FRAME_ExitTraverse
  {
  char dummy;
  } ;


int ExitTraverse(NodeIndex, LinkNode, traversal, ExitDesc1)
HdgRange NodeIndex;
HdgRange LinkNode;
TraversalType traversal;
NodeDescriptorType *ExitDesc1;
    {
    struct _ptoc_FRAME_ExitTraverse _ptoc_LOCAL;
    NodeDescriptorType ExitDesc2;
    Set set1;
    Set set2;
    
    ExitDesc2.ref = (Set *) set1;
    ExitDesc2.def = (Set *) set2;
    ClearDescriptor(&(*ExitDesc1));
    while (NodeIndex != -1)
        {
        NoNodesTraversed += 1;
        if (NodeIndex != ExcludedNode)
            {
            BuildExitDescriptors(NodeIndex , LinkNode , traversal , &ExitDesc2);
            UnionDescriptors(&(*ExitDesc1) , &(*ExitDesc1) , &ExitDesc2);
            }
        LinkNode = NodeIndex + 3;
        NodeIndex = field[NodeIndex + 3];
        }
    }

struct _ptoc_FRAME_DeallocateDsc
  {
  char dummy;
  } ;


int DeallocateDsc(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_DeallocateDsc _ptoc_LOCAL;
    
    while (NodeIndex != -1)
        {
        DeallocateCsgDsc(NodeIndex);
        NodeIndex = field[NodeIndex + 3];
        }
    }

struct _ptoc_FRAME_DeallocateCsgDsc
  {
  char dummy;
  } ;


int DeallocateCsgDsc(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_DeallocateCsgDsc _ptoc_LOCAL;
    
    switch (shr(field[NodeIndex] , 16))
        {
        case 12 :
            DeallocateDsc(field[NodeIndex + 6]);
            DeallocateDsc(field[NodeIndex + 5]);
            break;
        case 13 :
            DeallocateDsc(field[NodeIndex + 5]);
            break;
        case 14 :
            DeallocateDsc(field[NodeIndex + 5]);
            break;
        case 6 :
            DeallocateDsc(field[NodeIndex + 5]);
            break;
        case 7 :
            DeallocateDsc(field[NodeIndex + 5]);
            DeallocateDsc(field[NodeIndex + 6]);
            break;
        case 11 :
            break;
        case 8 :
            DeallocateDsc(field[NodeIndex + 5]);
            DeallocateDsc(field[NodeIndex + 6]);
            if (field[NodeIndex + 7] != -1)
                {
                DeallocateDsc(field[NodeIndex + 7]);
                }
            break;
        case 9 :
            DeallocateDsc(field[NodeIndex + 5]);
            DeallocateDsc(field[NodeIndex + 6]);
            break;
        case 10 :
            DeallocateDsc(field[NodeIndex + 6]);
            break;
        case 1 :
            break;
        case 2 :
            break;
        case 3 :
            DeallocateDsc(field[NodeIndex + 5]);
            break;
        case 4 :
            DeallocateDsc(field[NodeIndex + 5]);
            break;
        case 5 :
            DeallocateDsc(field[NodeIndex + 5]);
            DeallocateDsc(field[NodeIndex + 6]);
            break;
        default :
            PasCerror("MajorOpcode - DeallocateDsc                                                                                                                                                                                                                                     ");
            break;
        }
    if (shr(field[NodeIndex] , 16) != 1 && shr(field[NodeIndex] , 16) != 12 && shr(field[NodeIndex] , 16) != 8 && shr(field[NodeIndex] , 16) != 9)
        {
        if (field[NodeIndex + 2] != sql__ord((int *) 0))
            {
            FreeDescriptor((struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2]);
            field[NodeIndex + 2] = sql__ord((int *) 0);
            }
        }
    }
