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
  unsigned TraversalType;
SymbolTableRange gl_AnyPtrPtr;
int ExprFlag;
int StmntFlag;
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
int RedundantStmnts;
int RedundantExpr;
int LoopsConverted;
int StmntsOutsideLoop;
int ExprOutsideLoop;
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
int EssentialBit;
int Optm2I();
int BuildExitDescriptors();
int ExitTraverse();
int DeallocateCsgDsc();
int DeallocateDsc();
typedef
  unsigned TransformType;
int identical();
int ConditionsSatisfied();
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
int gl_InitTemps();
int gl_TempAvail();
int gl_GetTemporary();
int gl_FreeTemp();
int gl_PushNestLevel();
int gl_PopNestLevel();
int init_optimizer();
int close_optimizer();
int init_proc();
int close_proc();
int reset_heapmanager();
int not_SetConst();
int TransformLoop();
int TransformProc();
int OptIfthenelse();

struct _ptoc_FRAME_NoPack
  {
  char dummy;
  } ;


int NoPack(typ)
SymbolTableRange typ;
    {
    struct _ptoc_FRAME_NoPack _ptoc_LOCAL;
    int _NoPack;
    
    while (typ > 0 && STab[typ].SKind == 13)
        typ = STab[typ].SType;
    _NoPack = typ;
    return(_NoPack);
    }

struct _ptoc_FRAME_RemoveExpressions
  {
  char dummy;
  } ;


int RemoveExpressions(FirstStmnt, LinkToFirstStmnt, index1, index2, traversal, temporary, BuiltEntry)
HdgRange FirstStmnt;
HdgRange LinkToFirstStmnt;
HdgRange index1;
HdgRange index2;
TraversalType traversal;
SymbolTableRange *temporary;
int *BuiltEntry;
    {
    struct _ptoc_FRAME_RemoveExpressions _ptoc_LOCAL;
    int _RemoveExpressions;
    HdgRange NodeIndex;
    HdgRange CnodeIndex;
    HdgRange LinkNode;
    NodeDescriptorType ExitDesc;
    Set set1;
    Set set2;
    int MovingLval;
    SymbolTableRange TempType;
    _iorechd *ptoc_Var0;
    int ptoc_Var1;
    
    ExitDesc.ref = (Set *) set1;
    ExitDesc.def = (Set *) set2;
    NodeIndex = ExprChain[index1 + -1];
    CnodeIndex = ExprChain[index2 + -1];
    MovingLval = IsLvalOp(NodeIndex);
    if (MovingLval)
        {
        TempType = gl_AnyPtrPtr;
        }
    else
        {
        if (shr(field[NodeIndex] , 16) == 8)
            {
            TempType = NoPack(field[field[NodeIndex + 6] + 1]);
            }
        else
            {
            TempType = NoPack(field[NodeIndex + 1]);
            }
        }
    if (gl_TempAvail(*temporary , TempType) && ConditionsSatisfied(2 , NodeIndex , FirstStmnt , &(*BuiltEntry)) && ConditionsSatisfied(0 , NodeIndex , FirstStmnt , &(*BuiltEntry)) && ConditionsSatisfied(0 , CnodeIndex , FirstStmnt , &(*BuiltEntry)))
        {
        if (*temporary == -1)
            {
            *temporary = gl_GetTemporary(TempType);
            if (MovingLval)
                {
                FirstStmnt = CreateAssignmentStatement(CreateAddrOfLval(NodeIndex) , FirstStmnt , *temporary , -1);
                }
            else
                {
                FirstStmnt = CreateAssignmentStatement(NodeIndex , FirstStmnt , *temporary , -1);
                }
            LinkNode = LinkToFirstStmnt;
            field[LinkNode] = FirstStmnt;
            field[ExprLink[index1 + -1]] = CreateLoadNode(*temporary);
            if (MovingLval)
                {
                ptoc_Var1 = ExprLink[index1 + -1];
                field[ptoc_Var1] = UnaryExpression(97 , field[NodeIndex + 1] , field[ptoc_Var1] , 0 , 0);
                }
            field[field[ExprLink[index1 + -1]] + 3] = field[NodeIndex + 3];
            field[NodeIndex + 3] = -1;
            }
        field[ExprLink[index2 + -1]] = CreateLoadNode(*temporary);
        if (MovingLval)
            {
            ptoc_Var1 = ExprLink[index2 + -1];
            field[ptoc_Var1] = UnaryExpression(97 , field[NodeIndex + 1] , field[ptoc_Var1] , 0 , 0);
            }
        field[field[ExprLink[index2 + -1]] + 3] = field[CnodeIndex + 3];
        field[CnodeIndex + 3] = -1;
        if (control[86])
            {
            ptoc_Var0 = sql__ut(output);
            fprintf(ptoc_Var0->_fbuf , "%s%10d%c%10d" , "redundant expressions found: " , NodeIndex , 44 , CnodeIndex);
            sql__writeln(ptoc_Var0);
            }
        _RemoveExpressions = 1;
        }
    else
        {
        if (control[86])
            {
            ptoc_Var0 = sql__ut(output);
            fprintf(ptoc_Var0->_fbuf , "%s" , "conditions for redundant expressions failed");
            sql__writeln(ptoc_Var0);
            }
        _RemoveExpressions = 0;
        }
    return(_RemoveExpressions);
    }

struct _ptoc_FRAME_TransformExpressionList
  {
  char dummy;
  } ;


int TransformExpressionList(FirstStmnt, LinkNode, traversal, success)
HdgRange FirstStmnt;
HdgRange LinkNode;
TraversalType traversal;
int *success;
    {
    struct _ptoc_FRAME_TransformExpressionList _ptoc_LOCAL;
    HdgRange CurrentStatement;
    HdgRange CurrentLink;
    HdgRange NodeIndex;
    HdgRange CnodeIndex;
    TableRange index1;
    TableRange index2;
    TableRange StatementIndex;
    SymbolTableRange temporary;
    NodeDescriptorType ExitDesc;
    Set set1;
    Set set2;
    int NodeRemoved;
    int BuiltEntry;
    DescriptorRange DescPtr;
    DescriptorRange CdescPtr;
    _iorechd *ptoc_Var2;
    int ptoc_Var3;
    int ptoc_Var4;
    int ptoc_Var5;
    
    ExitDesc.ref = (Set *) set1;
    ExitDesc.def = (Set *) set2;
    if (control[86])
        {
        ptoc_Var2 = sql__ut(output);
        fprintf(ptoc_Var2->_fbuf , "");
        sql__writeln(ptoc_Var2);
        ptoc_Var2 = sql__ut(output);
        fprintf(ptoc_Var2->_fbuf , "%s%*d" , "TransformExpressionList: FirstStmnt = " , 1 , FirstStmnt);
        sql__writeln(ptoc_Var2);
        }
    LAB_100 :
    if (*success)
        {
        ExprFlag = 1;
        StmntFlag = 1;
        NoExpr = 0;
        NoStmnt = 0;
        EssentialBit = 1;
        ExitTraverse(FirstStmnt , LinkNode , traversal , &ExitDesc);
        *success = 0;
        ExprFlag = 0;
        StmntFlag = 1;
        }
    NodeRemoved = 0;
    ptoc_Var3 = NoExpr;
    if (1 <= ptoc_Var3)
        {
        index1 = 1;
        do
            {
            StatementIndex = ExprStmnt[index1 + -1];
            CurrentStatement = StmntChain[StatementIndex + -1];
            CurrentLink = StmntLink[StatementIndex + -1];
            BuiltEntry = 0;
            temporary = -1;
            NodeIndex = ExprChain[index1 + -1];
            DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
            ptoc_Var4 = index1 + 1;
            ptoc_Var5 = NoExpr;
            if (ptoc_Var4 <= ptoc_Var5)
                {
                index2 = ptoc_Var4;
                do
                    {
                    CnodeIndex = ExprChain[index2 + -1];
                    CdescPtr = (struct NodeDescriptorRecord *) (int *) field[CnodeIndex + 2];
                    if (control[86])
                        {
                        ptoc_Var2 = sql__ut(output);
                        fprintf(ptoc_Var2->_fbuf , "%s%10d%c%10d" , "compare expressions: " , NodeIndex , 44 , CnodeIndex);
                        sql__writeln(ptoc_Var2);
                        }
                    if (CdescPtr != (struct NodeDescriptorRecord *) (int *) 0)
                        {
                        if ((*(struct NodeDescriptorRecord *) DescPtr).essential && (*(struct NodeDescriptorRecord *) CdescPtr).essential && (*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor.hash_code == (*(struct NodeDescriptorRecord *) CdescPtr).ExitDescriptor.hash_code)
                            {
                            if (EqualDescriptors(&(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor , &(*(struct NodeDescriptorRecord *) CdescPtr).ExitDescriptor))
                                {
                                if (identical(NodeIndex , CnodeIndex))
                                    {
                                    if (RemoveExpressions(CurrentStatement , CurrentLink , index1 , index2 , traversal , &temporary , &BuiltEntry))
                                        {
                                        NodeRemoved = 1;
                                        RedundantExpr += 1;
                                        DeallocateDsc(CnodeIndex);
                                        }
                                    }
                                }
                            }
                        }
                    if (index2 == ptoc_Var5)
                        {
                        break;
                        }
                    index2 += 1;
                    }
                while (!0);
                }
            if (NodeRemoved)
                {
                *success = 1;
                FirstStmnt = field[LinkNode];
                goto LAB_100;
                }
            if (index1 == ptoc_Var3)
                {
                break;
                }
            index1 += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_RemoveRedundantStatement
  {
  char dummy;
  } ;


int RemoveRedundantStatement(index1, index2, BuiltEntry)
HdgRange index1;
HdgRange index2;
int *BuiltEntry;
    {
    struct _ptoc_FRAME_RemoveRedundantStatement _ptoc_LOCAL;
    int _RemoveRedundantStatement;
    HdgRange NodeIndex;
    HdgRange FirstStmnt;
    _iorechd *ptoc_Var6;
    
    if (control[86])
        {
        ptoc_Var6 = sql__ut(output);
        fprintf(ptoc_Var6->_fbuf , "");
        sql__writeln(ptoc_Var6);
        ptoc_Var6 = sql__ut(output);
        fprintf(ptoc_Var6->_fbuf , "%s" , "RemoveRedundantStatements");
        sql__writeln(ptoc_Var6);
        }
    NodeIndex = StmntChain[index2 + -1];
    FirstStmnt = StmntChain[index1];
    if (ConditionsSatisfied(2 , NodeIndex , FirstStmnt , &(*BuiltEntry)) && ConditionsSatisfied(0 , NodeIndex , FirstStmnt , &(*BuiltEntry)))
        {
        field[StmntLink[index2 + -1]] = field[NodeIndex + 3];
        if (control[86])
            {
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s%10d%s" , "redundant statement: " , NodeIndex , " removed");
            sql__writeln(ptoc_Var6);
            }
        _RemoveRedundantStatement = 1;
        }
    else
        {
        if (control[86])
            {
            ptoc_Var6 = sql__ut(output);
            fprintf(ptoc_Var6->_fbuf , "%s" , "conditions for redundant statements failed");
            sql__writeln(ptoc_Var6);
            }
        _RemoveRedundantStatement = 0;
        }
    return(_RemoveRedundantStatement);
    }

struct _ptoc_FRAME_TransformStatementList
  {
  char dummy;
  } ;


int TransformStatementList(FirstStmnt, LinkToFirstStmnt, traversal, success)
HdgRange FirstStmnt;
HdgRange LinkToFirstStmnt;
TraversalType traversal;
int *success;
    {
    struct _ptoc_FRAME_TransformStatementList _ptoc_LOCAL;
    int index1;
    int index2;
    HdgRange NodeIndex;
    HdgRange CnodeIndex;
    NodeDescriptorType ExitDesc;
    Set set1;
    Set set2;
    int StmntRemoved;
    int BuiltEntry;
    DescriptorRange DescPtr;
    DescriptorRange CdescPtr;
    _iorechd *ptoc_Var7;
    int ptoc_Var8;
    int ptoc_Var9;
    int ptoc_Var10;
    
    ExitDesc.ref = (Set *) set1;
    ExitDesc.def = (Set *) set2;
    if (control[86])
        {
        ptoc_Var7 = sql__ut(output);
        fprintf(ptoc_Var7->_fbuf , "");
        sql__writeln(ptoc_Var7);
        ptoc_Var7 = sql__ut(output);
        fprintf(ptoc_Var7->_fbuf , "%s%*d" , "TransformStatementList: FirstStmnt = " , 1 , FirstStmnt);
        sql__writeln(ptoc_Var7);
        }
    LAB_100 :
    if (*success)
        {
        ExprFlag = 1;
        StmntFlag = 1;
        NoExpr = 0;
        NoStmnt = 0;
        EssentialBit = 1;
        ExitTraverse(FirstStmnt , LinkToFirstStmnt , traversal , &ExitDesc);
        *success = 0;
        ExprFlag = 1;
        StmntFlag = 0;
        }
    StmntRemoved = 0;
    ptoc_Var8 = NoStmnt;
    if (1 <= ptoc_Var8)
        {
        index1 = 1;
        do
            {
            BuiltEntry = 0;
            NodeIndex = StmntChain[index1 + -1];
            DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
            ptoc_Var9 = index1 + 1;
            ptoc_Var10 = NoStmnt;
            if (ptoc_Var9 <= ptoc_Var10)
                {
                index2 = ptoc_Var9;
                do
                    {
                    CnodeIndex = StmntChain[index2 + -1];
                    CdescPtr = (struct NodeDescriptorRecord *) (int *) field[CnodeIndex + 2];
                    if (control[86])
                        {
                        ptoc_Var7 = sql__ut(output);
                        fprintf(ptoc_Var7->_fbuf , "%s%10d%c%10d" , "compare stmnts: " , NodeIndex , 44 , CnodeIndex);
                        sql__writeln(ptoc_Var7);
                        }
                    if ((*(struct NodeDescriptorRecord *) DescPtr).essential && (*(struct NodeDescriptorRecord *) CdescPtr).essential && (*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor.hash_code == (*(struct NodeDescriptorRecord *) CdescPtr).ExitDescriptor.hash_code)
                        {
                        if (EqualDescriptors(&(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor , &(*(struct NodeDescriptorRecord *) CdescPtr).ExitDescriptor))
                            {
                            if (identical(NodeIndex , CnodeIndex))
                                {
                                if (RemoveRedundantStatement(index1 , index2 , &BuiltEntry))
                                    {
                                    StmntRemoved = 1;
                                    RedundantStmnts += 1;
                                    DeallocateDsc(CnodeIndex);
                                    }
                                }
                            }
                        }
                    if (index2 == ptoc_Var10)
                        {
                        break;
                        }
                    index2 += 1;
                    }
                while (!0);
                }
            if (StmntRemoved)
                {
                *success = 1;
                goto LAB_100;
                }
            if (index1 == ptoc_Var8)
                {
                break;
                }
            index1 += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_ConvertLoop
  {
  char dummy;
  } ;


int ConvertLoop(NodeIndex, LinkNode, NewLinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
HdgRange *NewLinkNode;
    {
    struct _ptoc_FRAME_ConvertLoop _ptoc_LOCAL;
    HdgRange CnodeIndex;
    _iorechd *ptoc_Var11;
    
    if (control[86])
        {
        ptoc_Var11 = sql__ut(output);
        fprintf(ptoc_Var11->_fbuf , "");
        sql__writeln(ptoc_Var11);
        ptoc_Var11 = sql__ut(output);
        fprintf(ptoc_Var11->_fbuf , "%s" , "ConvertLoop into RepeatUntil");
        sql__writeln(ptoc_Var11);
        }
    CnodeIndex = ComplexStmnt(8 , 0 , CopyTree(field[NodeIndex + 5]) , NodeIndex , -1 , -1);
    field[LinkNode] = CnodeIndex;
    field[CnodeIndex + 3] = field[NodeIndex + 3];
    field[NodeIndex + 3] = -1;
    *NewLinkNode = CnodeIndex + 6;
    field[NodeIndex + 5] = demorgan(field[NodeIndex + 5]);
    field[NodeIndex] = shl(12 , 16) + 1;
    LoopsConverted += 1;
    }

struct _ptoc_FRAME_MoveStatement
  {
  char dummy;
  } ;


int MoveStatement(index1, LoopIndex, LinkNode)
int index1;
HdgRange *LoopIndex;
HdgRange *LinkNode;
    {
    struct _ptoc_FRAME_MoveStatement _ptoc_LOCAL;
    HdgRange NodeIndex;
    
    NodeIndex = StmntChain[index1 + -1];
    field[StmntLink[index1 + -1]] = field[NodeIndex + 3];
    if (field[NodeIndex + 3] == StmntChain[index1])
        {
        StmntLink[index1] = StmntLink[index1 + -1];
        }
    field[*LinkNode] = NodeIndex;
    field[NodeIndex + 3] = *LoopIndex;
    *LinkNode = NodeIndex + 3;
    }

struct _ptoc_FRAME_MoveStmntOutsideLoop
  {
  char dummy;
  } ;


int MoveStmntOutsideLoop(LoopIndex, LinkNode, success)
HdgRange LoopIndex;
HdgRange *LinkNode;
int *success;
    {
    struct _ptoc_FRAME_MoveStmntOutsideLoop _ptoc_LOCAL;
    HdgRange NodeIndex;
    HdgRange CnodeIndex;
    HdgRange NewLinkNode;
    int index1;
    NodeDescriptorType ExitDesc;
    Set set1;
    Set set2;
    int BuiltEntry;
    _iorechd *ptoc_Var12;
    int ptoc_Var13;
    
    ExitDesc.ref = (Set *) set1;
    ExitDesc.def = (Set *) set2;
    if (control[86])
        {
        ptoc_Var12 = sql__ut(output);
        fprintf(ptoc_Var12->_fbuf , "");
        sql__writeln(ptoc_Var12);
        ptoc_Var12 = sql__ut(output);
        fprintf(ptoc_Var12->_fbuf , "%s" , "MoveStmntOutsideLoop");
        sql__writeln(ptoc_Var12);
        }
    LAB_100 :
    if (*success)
        {
        ExprFlag = 1;
        StmntFlag = 1;
        NoExpr = 0;
        NoStmnt = 0;
        EssentialBit = 1;
        BuildExitDescriptors(LoopIndex , -1 , 1 , &ExitDesc);
        *success = 0;
        ExprFlag = 0;
        StmntFlag = 0;
        }
    BuiltEntry = 0;
    CnodeIndex = LoopIndex;
    ptoc_Var13 = NoStmnt;
    if (1 <= ptoc_Var13)
        {
        index1 = 1;
        do
            {
            NodeIndex = StmntChain[index1 + -1];
            if (NodeIndex != LoopIndex && ConditionsSatisfied(1 , NodeIndex , LoopIndex , &BuiltEntry))
                {
                if (control[86])
                    {
                    ptoc_Var12 = sql__ut(output);
                    fprintf(ptoc_Var12->_fbuf , "");
                    sql__writeln(ptoc_Var12);
                    ptoc_Var12 = sql__ut(output);
                    fprintf(ptoc_Var12->_fbuf , "%s%10d%s" , "move stmnt " , NodeIndex , " outside loop");
                    sql__writeln(ptoc_Var12);
                    }
                if (shr(field[CnodeIndex] , 16) == 9)
                    {
                    ConvertLoop(LoopIndex , *LinkNode , &NewLinkNode);
                    *LinkNode = NewLinkNode;
                    }
                *success = 1;
                MoveStatement(index1 , &LoopIndex , &(*LinkNode));
                StmntsOutsideLoop += 1;
                goto LAB_100;
                }
            if (index1 == ptoc_Var13)
                {
                break;
                }
            index1 += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_MoveExprOutsideLoop
  {
  char dummy;
  } ;


struct _ptoc_FRAME_SubExpression_MoveExprOutsideLoop
  {
  struct _ptoc_FRAME_MoveExprOutsideLoop *_ptoc_LINK;
  char dummy;
  } ;


static
int SubExpression_MoveExprOutsideLoop(_ptoc_LINK, expr1, expr2)
struct _ptoc_FRAME_MoveExprOutsideLoop *_ptoc_LINK;
HdgRange expr1;
HdgRange expr2;
    {
    struct _ptoc_FRAME_SubExpression_MoveExprOutsideLoop _ptoc_LOCAL;
    int _SubExpression;
    int NodeIndex;
    int flag;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (expr1 == expr2)
        {
        _SubExpression = 1;
        }
    else
        {
        if (expr2 == -1)
            {
            _SubExpression = 0;
            }
        else
            {
            NodeIndex = expr2;
            switch (shr(field[NodeIndex] , 16))
                {
                case 1 :
                    _SubExpression = 0;
                    break;
                case 4 :
                    _SubExpression = SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , field[NodeIndex + 5]);
                    break;
                case 5 :
                    if (SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , field[NodeIndex + 5]))
                        {
                        _SubExpression = 1;
                        }
                    else
                        {
                        _SubExpression = SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , field[NodeIndex + 6]);
                        }
                    break;
                case 6 :
                    NodeIndex = field[NodeIndex + 5];
                    do
                        {
                        flag = SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , NodeIndex);
                        NodeIndex = field[NodeIndex + 3];
                        }
                    while (!(flag || NodeIndex == -1));
                    _SubExpression = flag;
                    break;
                case 7 :
                    if (SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , field[NodeIndex + 5]))
                        {
                        _SubExpression = 1;
                        }
                    else
                        {
                        _SubExpression = SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , field[NodeIndex + 6]);
                        }
                    break;
                case 8 :
                    if (SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , field[NodeIndex + 5]))
                        {
                        _SubExpression = 1;
                        }
                    else
                        {
                        if (SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , field[NodeIndex + 6]))
                            {
                            _SubExpression = 1;
                            }
                        else
                            {
                            _SubExpression = field[NodeIndex + 7] != -1 && SubExpression_MoveExprOutsideLoop(_ptoc_LOCAL._ptoc_LINK, expr1 , field[NodeIndex + 7]);
                            }
                        }
                    break;
                case 2 :
                case 3 :
                case 9 :
                case 10 :
                case 11 :
                case 12 :
                case 13 :
                case 14 :
                    PasCerror("unexpected MajorOperator in SubExpression                                                                                                                                                                                                                       ");
                    break;
                }
            }
        }
    return(_SubExpression);
    }

int MoveExprOutsideLoop(LoopIndex, LinkNode, success)
HdgRange LoopIndex;
HdgRange *LinkNode;
int *success;
    {
    struct _ptoc_FRAME_MoveExprOutsideLoop _ptoc_LOCAL;
    HdgRange NodeIndex;
    HdgRange CnodeIndex;
    HdgRange NewLinkNode;
    HdgRange SaveNextLink;
    int index1;
    int index2;
    SymbolTableRange temporary;
    NodeDescriptorType ExitDesc;
    Set set1;
    Set set2;
    int BuiltEntry;
    int MovingLval;
    SymbolTableRange TempType;
    _iorechd *ptoc_Var14;
    int ptoc_Var15;
    int ptoc_Var16;
    
    ExitDesc.ref = (Set *) set1;
    ExitDesc.def = (Set *) set2;
    if (control[86])
        {
        ptoc_Var14 = sql__ut(output);
        fprintf(ptoc_Var14->_fbuf , "");
        sql__writeln(ptoc_Var14);
        ptoc_Var14 = sql__ut(output);
        fprintf(ptoc_Var14->_fbuf , "%s%*d%c" , "MoveExprOutsideLoop (" , 1 , LoopIndex , 41);
        sql__writeln(ptoc_Var14);
        }
    LAB_100 :
    if (*success)
        {
        ExprFlag = 1;
        StmntFlag = 1;
        NoExpr = 0;
        NoStmnt = 0;
        EssentialBit = 1;
        BuildExitDescriptors(LoopIndex , -1 , 1 , &ExitDesc);
        *success = 0;
        ExprFlag = 0;
        StmntFlag = 0;
        }
    BuiltEntry = 0;
    CnodeIndex = LoopIndex;
    ptoc_Var15 = NoExpr;
    if (1 <= ptoc_Var15)
        {
        index1 = 1;
        do
            {
            NodeIndex = ExprChain[index1 + -1];
            MovingLval = IsLvalOp(NodeIndex);
            if (MovingLval)
                {
                TempType = gl_AnyPtrPtr;
                }
            else
                {
                if (shr(field[NodeIndex] , 16) == 8)
                    {
                    TempType = NoPack(field[field[NodeIndex + 6] + 1]);
                    }
                else
                    {
                    TempType = NoPack(field[NodeIndex + 1]);
                    }
                }
            if (gl_TempAvail(-1 , TempType) && ConditionsSatisfied(1 , NodeIndex , LoopIndex , &BuiltEntry))
                {
                if (shr(field[CnodeIndex] , 16) == 9 && !SubExpression_MoveExprOutsideLoop(&_ptoc_LOCAL, NodeIndex , field[CnodeIndex + 5]))
                    {
                    ConvertLoop(LoopIndex , *LinkNode , &NewLinkNode);
                    *LinkNode = NewLinkNode;
                    }
                *success = 1;
                BuiltEntry = 0;
                if (control[86])
                    {
                    ptoc_Var14 = sql__ut(output);
                    fprintf(ptoc_Var14->_fbuf , "%s%10d%s" , "   moved expression " , NodeIndex , " outside loop");
                    sql__writeln(ptoc_Var14);
                    }
                SaveNextLink = field[NodeIndex + 3];
                field[NodeIndex + 3] = -1;
                temporary = gl_GetTemporary(TempType);
                if (MovingLval)
                    {
                    NodeIndex = CreateAssignmentStatement(CreateAddrOfLval(NodeIndex) , LoopIndex , temporary , -1);
                    }
                else
                    {
                    NodeIndex = CreateAssignmentStatement(NodeIndex , LoopIndex , temporary , -1);
                    }
                field[*LinkNode] = NodeIndex;
                *LinkNode = NodeIndex + 3;
                field[ExprLink[index1 + -1]] = CreateLoadNode(temporary);
                if (MovingLval)
                    {
                    ptoc_Var16 = ExprLink[index1 + -1];
                    field[ptoc_Var16] = UnaryExpression(97 , field[NodeIndex + 1] , field[ptoc_Var16] , 0 , 0);
                    }
                field[field[ExprLink[index1 + -1]] + 3] = SaveNextLink;
                ExprOutsideLoop += 1;
                goto LAB_100;
                }
            if (index1 == ptoc_Var15)
                {
                break;
                }
            index1 += 1;
            }
        while (!0);
        }
    }

struct _ptoc_FRAME_ClearUnnecessaryDsc
  {
  char dummy;
  } ;


int ClearUnnecessaryDsc(NodeIndex, traversal)
HdgRange NodeIndex;
TraversalType traversal;
    {
    struct _ptoc_FRAME_ClearUnnecessaryDsc _ptoc_LOCAL;
    NodeDescriptorType ExitDsc;
    Set set1;
    Set set2;
    
    ExitDsc.ref = (Set *) set1;
    ExitDsc.def = (Set *) set2;
    IncludedNode = NodeIndex;
    EssentialBit = 1;
    BuildExitDescriptors(NodeIndex , -1 , traversal , &ExitDsc);
    IncludedNode = -1;
    }

struct _ptoc_FRAME_TransformLoop
  {
  char dummy;
  } ;


int TransformLoop(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_TransformLoop _ptoc_LOCAL;
    int success;
    DescriptorRange DescPtr;
    _iorechd *ptoc_Var17;
    
    if (control[86])
        {
        ptoc_Var17 = sql__ut(output);
        fprintf(ptoc_Var17->_fbuf , "");
        sql__writeln(ptoc_Var17);
        ptoc_Var17 = sql__ut(output);
        fprintf(ptoc_Var17->_fbuf , "");
        sql__writeln(ptoc_Var17);
        ptoc_Var17 = sql__ut(output);
        fprintf(ptoc_Var17->_fbuf , "%s" , "#### optimize loop ####");
        sql__writeln(ptoc_Var17);
        ptoc_Var17 = sql__ut(output);
        fprintf(ptoc_Var17->_fbuf , "");
        sql__writeln(ptoc_Var17);
        }
    IncludedNode = -1;
    ExcludedNode = -1;
    OptimizeNode = NodeIndex;
    success = 1;
    TransformStatementList(field[NodeIndex + 6] , NodeIndex + 6 , 1 , &success);
    TransformExpressionList(field[NodeIndex + 6] , NodeIndex + 6 , 1 , &success);
    success = 1;
    MoveStmntOutsideLoop(NodeIndex , &LinkNode , &success);
    MoveExprOutsideLoop(NodeIndex , &LinkNode , &success);
    ClearUnnecessaryDsc(NodeIndex , 1);
    if (control[86])
        {
        DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
        PrintDescriptor(NodeIndex , "loop                                                                                                                                                                                                                                                            " , &(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor);
        }
    OptimizeNode = -1;
    }

struct _ptoc_FRAME_TransformProc
  {
  char dummy;
  } ;


int TransformProc(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_TransformProc _ptoc_LOCAL;
    int success;
    DescriptorRange DescPtr;
    _iorechd *ptoc_Var18;
    
    if (control[86])
        {
        ptoc_Var18 = sql__ut(output);
        fprintf(ptoc_Var18->_fbuf , "");
        sql__writeln(ptoc_Var18);
        ptoc_Var18 = sql__ut(output);
        fprintf(ptoc_Var18->_fbuf , "");
        sql__writeln(ptoc_Var18);
        ptoc_Var18 = sql__ut(output);
        fprintf(ptoc_Var18->_fbuf , "%s" , "#### optimize procedure ####");
        sql__writeln(ptoc_Var18);
        ptoc_Var18 = sql__ut(output);
        fprintf(ptoc_Var18->_fbuf , "");
        sql__writeln(ptoc_Var18);
        }
    IncludedNode = -1;
    ExcludedNode = -1;
    OptimizeNode = NodeIndex;
    success = 1;
    TransformStatementList(field[NodeIndex + 5] , NodeIndex + 5 , 1 , &success);
    TransformExpressionList(field[NodeIndex + 5] , NodeIndex + 5 , 1 , &success);
    ClearUnnecessaryDsc(NodeIndex , 1);
    if (control[86])
        {
        DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
        PrintDescriptor(NodeIndex , "proc                                                                                                                                                                                                                                                            " , &(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor);
        }
    OptimizeNode = -1;
    }

struct _ptoc_FRAME_OptIfthenelse
  {
  char dummy;
  } ;


int OptIfthenelse(NodeIndex, LinkNode, traversal)
HdgRange NodeIndex;
HdgRange LinkNode;
TraversalType traversal;
    {
    struct _ptoc_FRAME_OptIfthenelse _ptoc_LOCAL;
    int success;
    DescriptorRange DescPtr;
    _iorechd *ptoc_Var19;
    
    if (control[86])
        {
        ptoc_Var19 = sql__ut(output);
        fprintf(ptoc_Var19->_fbuf , "");
        sql__writeln(ptoc_Var19);
        ptoc_Var19 = sql__ut(output);
        fprintf(ptoc_Var19->_fbuf , "");
        sql__writeln(ptoc_Var19);
        ptoc_Var19 = sql__ut(output);
        fprintf(ptoc_Var19->_fbuf , "%s" , "#### optimize ifthenelse ####");
        sql__writeln(ptoc_Var19);
        ptoc_Var19 = sql__ut(output);
        fprintf(ptoc_Var19->_fbuf , "");
        sql__writeln(ptoc_Var19);
        }
    OptimizeNode = NodeIndex;
    success = 1;
    TransformStatementList(field[NodeIndex + 6] , NodeIndex + 6 , traversal , &success);
    TransformExpressionList(field[NodeIndex + 6] , NodeIndex + 6 , traversal , &success);
    if (field[NodeIndex + 7] != -1)
        {
        success = 1;
        TransformStatementList(field[NodeIndex + 7] , NodeIndex + 7 , traversal , &success);
        TransformExpressionList(field[NodeIndex + 7] , NodeIndex + 7 , traversal , &success);
        }
    ClearUnnecessaryDsc(NodeIndex , traversal);
    if (control[86])
        {
        DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
        PrintDescriptor(NodeIndex , "ifthenelse                                                                                                                                                                                                                                                      " , &(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor);
        }
    OptimizeNode = -1;
    }
