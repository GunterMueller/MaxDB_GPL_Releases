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
SymbolTableRange gl_IntegerPtr;
SymbolTableRange ProcedureName;
HdgRange ExcludedNode;
int NoNodesTraversed;
int ExprReduced;
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
typedef
  unsigned TransformType;
int identical();
int ConditionsSatisfied();
int ConstantPropagation();
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
int x_new();
int not_SetConst();
int StrengthReduce();
typedef
  int STableRange;
typedef
  struct ptoc_Type3
    {
    struct ptoc_Type4
      {
      HdgRange lval;
      int ConstExp;
      HdgRange StmntIndex;
      } candidate[80];
    STableRange sentinel;
    } STableType;
typedef
  int StackRange;
typedef
  struct ptoc_Type5
    {
    int StackArray[6];
    StackRange StackPtr;
    } StackType;
typedef
  int PatrnType[6];
HdgRange LoopStmnt;
HdgRange LoopTailLink;
HdgRange PrevLink;
STableType STable;
PatrnType patrn;
StackType args;

struct _ptoc_FRAME_InitSTable
  {
  char dummy;
  } ;


int InitSTable(one)
int one;
    {
    struct _ptoc_FRAME_InitSTable _ptoc_LOCAL;
    
    STable.sentinel = one;
    }

struct _ptoc_FRAME_PrintSTable
  {
  char dummy;
  } ;


int PrintSTable()
    {
    struct _ptoc_FRAME_PrintSTable _ptoc_LOCAL;
    STableRange i;
    _iorechd *ptoc_Var0;
    int ptoc_Var1;
    
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s" , "{ ");
    ptoc_Var1 = STable.sentinel + -1;
    if (1 <= ptoc_Var1)
        {
        i = 1;
        do
            {
            ptoc_Var0 = sql__ut(output);
            fprintf(ptoc_Var0->_fbuf , "%10d%c%10d%c%10d%s" , STable.candidate[i + -1].lval , 58 , STable.candidate[i + -1].ConstExp , 58 , STable.candidate[i + -1].StmntIndex , ", ");
            if (i == ptoc_Var1)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s" , " }");
    sql__writeln(ptoc_Var0);
    sql__fflush(output);
    }

struct _ptoc_FRAME_SearchTable
  {
  char dummy;
  } ;


STableRange SearchTable(lval)
HdgRange lval;
    {
    struct _ptoc_FRAME_SearchTable _ptoc_LOCAL;
    STableRange _SearchTable;
    STableRange i;
    
    i = 1;
    STable.candidate[STable.sentinel + -1].lval = lval;
    while (!identical(STable.candidate[i + -1].lval , lval))
        i += 1;
    _SearchTable = i;
    return(_SearchTable);
    }

struct _ptoc_FRAME_AddTable
  {
  char dummy;
  } ;


int AddTable(lval, NodeIndex, constant)
HdgRange lval;
HdgRange NodeIndex;
int constant;
    {
    struct _ptoc_FRAME_AddTable _ptoc_LOCAL;
    STableRange i;
    
    if (STable.sentinel < 80)
        {
        i = SearchTable(lval);
        if (i == STable.sentinel)
            {
            STable.candidate[i + -1].lval = lval;
            STable.candidate[i + -1].ConstExp = constant;
            STable.candidate[i + -1].StmntIndex = NodeIndex;
            STable.sentinel += 1;
            }
        }
    }

struct _ptoc_FRAME_RmTable
  {
  char dummy;
  } ;


int RmTable(j)
STableRange j;
    {
    struct _ptoc_FRAME_RmTable _ptoc_LOCAL;
    
    if (j < STable.sentinel)
        {
        STable.sentinel += -1;
        while (j < STable.sentinel)
            {
            STable.candidate[j + -1] = STable.candidate[j];
            j += 1;
            }
        }
    }

struct _ptoc_FRAME_ConstLit
  {
  char dummy;
  } ;


int ConstLit(NodeIndex, literal)
HdgRange NodeIndex;
int *literal;
    {
    struct _ptoc_FRAME_ConstLit _ptoc_LOCAL;
    int _ConstLit;
    
    if (shr(field[NodeIndex] , 16) == 1 && (field[NodeIndex] & 65535) == 12)
        {
        *literal = field[NodeIndex + 5];
        _ConstLit = 1;
        }
    else
        {
        _ConstLit = 0;
        }
    return(_ConstLit);
    }

struct _ptoc_FRAME_LiteralNode
  {
  char dummy;
  } ;


int LiteralNode(literal)
int literal;
    {
    struct _ptoc_FRAME_LiteralNode _ptoc_LOCAL;
    int _LiteralNode;
    
    _LiteralNode = node3(12 , gl_IntegerPtr , literal , 0);
    return(_LiteralNode);
    }

struct _ptoc_FRAME_CrtInductnStmnt
  {
  char dummy;
  } ;


int CrtInductnStmnt(temp, ConstA, ConstC)
SymbolTableRange temp;
int ConstA;
int ConstC;
    {
    struct _ptoc_FRAME_CrtInductnStmnt _ptoc_LOCAL;
    int _CrtInductnStmnt;
    int ptoc_Var2;
    
    ptoc_Var2 = STab[temp].SType;
    _CrtInductnStmnt = LongStatement(102 , ptoc_Var2 , node3(94 , ptoc_Var2 , temp , 0) , LiteralNode(ConstA * ConstC) , 0 , 19 , -1);
    return(_CrtInductnStmnt);
    }

struct _ptoc_FRAME_FindVars
  {
  char dummy;
  } ;


int FindVars(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_FindVars _ptoc_LOCAL;
    int literal;
    
    while (NodeIndex != -1)
        {
        switch (field[NodeIndex] & 65535)
            {
            case 102 :
                if (STab[field[NodeIndex + 1]].SType == -5)
                    {
                    switch (field[NodeIndex + 8])
                        {
                        case 19 :
                            if (ConstLit(field[NodeIndex + 6] , &literal))
                                {
                                AddTable(field[NodeIndex + 5] , NodeIndex , literal);
                                }
                            break;
                        case 20 :
                            if (ConstLit(field[NodeIndex + 6] , &literal))
                                {
                                AddTable(field[NodeIndex + 5] , NodeIndex , -literal);
                                }
                            break;
                        }
                    }
                break;
            case 6 :
                FindVars(field[NodeIndex + 5]);
                break;
            }
        NodeIndex = field[NodeIndex + 3];
        }
    if (control[86])
        {
        PrintSTable();
        }
    }

struct _ptoc_FRAME_ScreenVars
  {
  char dummy;
  } ;


int ScreenVars(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_ScreenVars _ptoc_LOCAL;
    PSet intersection;
    Set intersect;
    Set set1;
    Set set2;
    Set set3;
    Set set4;
    NodeDescriptorType LoopExitDesc;
    NodeDescriptorType CandidateExitDesc;
    STableRange i;
    
    intersection = (Set *) intersect;
    CandidateExitDesc.ref = (Set *) set1;
    CandidateExitDesc.def = (Set *) set2;
    LoopExitDesc.ref = (Set *) set3;
    LoopExitDesc.def = (Set *) set4;
    i = 1;
    while (i < STable.sentinel)
        {
        BuildExitDescriptors(STable.candidate[i + -1].StmntIndex , -1 , 1 , &CandidateExitDesc);
        ExcludedNode = STable.candidate[i + -1].StmntIndex;
        BuildExitDescriptors(NodeIndex , -1 , 1 , &LoopExitDesc);
        Intersect_Sets(intersection , CandidateExitDesc.ref , LoopExitDesc.def);
        if (!Empty_check_Set(intersection))
            {
            RmTable(i);
            }
        else
            {
            Intersect_Sets(intersection , CandidateExitDesc.ref , LoopExitDesc.ref);
            if (Empty_check_Set(intersection))
                {
                RmTable(i);
                }
            else
                {
                i += 1;
                }
            }
        }
    if (control[86])
        {
        PrintSTable();
        }
    }

struct _ptoc_FRAME_PopArgs
  {
  char dummy;
  } ;


int PopArgs()
    {
    struct _ptoc_FRAME_PopArgs _ptoc_LOCAL;
    int _PopArgs;
    
    if (args.StackPtr > 0)
        {
        _PopArgs = args.StackArray[args.StackPtr];
        args.StackPtr += -1;
        }
    else
        {
        PasCerror("module StrRedux -- stack underflow                                                                                                                                                                                                                              ");
        }
    return(_PopArgs);
    }

struct _ptoc_FRAME_PushArgs
  {
  char dummy;
  } ;


int PushArgs(value)
int value;
    {
    struct _ptoc_FRAME_PushArgs _ptoc_LOCAL;
    
    if (args.StackPtr < 5)
        {
        args.StackPtr += 1;
        args.StackArray[args.StackPtr] = value;
        }
    else
        {
        PasCerror("module StrRedux -- stack overflow                                                                                                                                                                                                                               ");
        }
    }

struct _ptoc_FRAME_ClearArgs
  {
  char dummy;
  } ;


int ClearArgs()
    {
    struct _ptoc_FRAME_ClearArgs _ptoc_LOCAL;
    
    args.StackPtr = 0;
    }

struct _ptoc_FRAME_CreatePatrns
  {
  char dummy;
  } ;


int CreatePatrns()
    {
    struct _ptoc_FRAME_CreatePatrns _ptoc_LOCAL;
    HdgRange opr1;
    HdgRange opr2;
    
    opr1 = CreateLoadNode(0);
    opr2 = LiteralNode(0);
    patrn[0] = BinaryExpression(21 , 0 , opr2 , opr1);
    patrn[1] = BinaryExpression(19 , 0 , patrn[0] , opr2);
    patrn[2] = BinaryExpression(20 , 0 , patrn[0] , opr2);
    patrn[3] = BinaryExpression(21 , 0 , opr1 , opr2);
    patrn[4] = BinaryExpression(19 , 0 , patrn[3] , opr2);
    patrn[5] = BinaryExpression(20 , 0 , patrn[3] , opr2);
    }

struct _ptoc_FRAME_InsrtInductnStmnt
  {
  char dummy;
  } ;


int InsrtInductnStmnt(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_InsrtInductnStmnt _ptoc_LOCAL;
    
    if (field[LinkNode] == -1)
        {
        field[LoopTailLink] = NodeIndex;
        }
    field[NodeIndex + 3] = field[LinkNode];
    field[LinkNode] = NodeIndex;
    }

struct _ptoc_FRAME_InsrtInitStmnt
  {
  char dummy;
  } ;


int InsrtInitStmnt(temp, ExprPtr)
SymbolTableRange temp;
HdgRange ExprPtr;
    {
    struct _ptoc_FRAME_InsrtInitStmnt _ptoc_LOCAL;
    HdgRange NodeIndex;
    
    NodeIndex = CreateAssignmentStatement(ExprPtr , LoopStmnt , temp , -1);
    field[PrevLink] = NodeIndex;
    PrevLink = NodeIndex + 3;
    }

struct _ptoc_FRAME_RepLinearExp
  {
  char dummy;
  } ;


int RepLinearExp(temp, LinkNode)
SymbolTableRange temp;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_RepLinearExp _ptoc_LOCAL;
    HdgRange NodeIndex;
    
    NodeIndex = CreateLoadNode(temp);
    field[LinkNode] = NodeIndex;
    }

struct _ptoc_FRAME_IdentExp
  {
  char dummy;
  } ;


int IdentExp(NodeIndex, CnodeIndex)
HdgRange NodeIndex;
HdgRange CnodeIndex;
    {
    struct _ptoc_FRAME_IdentExp _ptoc_LOCAL;
    int _IdentExp;
    int ReturnValue;
    _iorechd *ptoc_Var3;
    
    if (control[86])
        {
        ptoc_Var3 = sql__ut(output);
        fprintf(ptoc_Var3->_fbuf , "");
        sql__writeln(ptoc_Var3);
        ptoc_Var3 = sql__ut(output);
        fprintf(ptoc_Var3->_fbuf , "%s%*d%s%*d" , "IdentExp, " , 1 , NodeIndex , ", " , 1 , CnodeIndex);
        sql__writeln(ptoc_Var3);
        }
    if (NodeIndex == -1 || CnodeIndex == -1)
        {
        ReturnValue = NodeIndex == CnodeIndex;
        }
    else
        {
        if (field[NodeIndex] == field[CnodeIndex])
            {
            switch (shr(field[NodeIndex] , 16))
                {
                case 1 :
                    switch (field[NodeIndex] & 65535)
                        {
                        case 12 :
                            PushArgs(field[NodeIndex + 5]);
                            ReturnValue = 1;
                            break;
                        default :
                            ReturnValue = 0;
                            break;
                        }
                    break;
                case 4 :
                    switch (field[NodeIndex] & 65535)
                        {
                        case 4 :
                            if (SearchTable(field[NodeIndex + 5]) < STable.sentinel)
                                {
                                PushArgs(field[NodeIndex + 5]);
                                ReturnValue = 1;
                                }
                            else
                                {
                                ReturnValue = 0;
                                }
                            break;
                        default :
                            ReturnValue = 0;
                            break;
                        }
                    break;
                case 5 :
                    if (IdentExp(field[NodeIndex + 5] , field[CnodeIndex + 5]))
                        {
                        ReturnValue = IdentExp(field[NodeIndex + 6] , field[CnodeIndex + 6]);
                        }
                    else
                        {
                        ReturnValue = 0;
                        }
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
        ptoc_Var3 = sql__ut(output);
        fprintf(ptoc_Var3->_fbuf , "%s%*d" , "   ReturnValue = " , 1 , sql__ord((int ) ReturnValue));
        sql__writeln(ptoc_Var3);
        }
    _IdentExp = ReturnValue;
    return(_IdentExp);
    }

struct _ptoc_FRAME_MatchPatrn
  {
  char dummy;
  } ;


int MatchPatrn(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_MatchPatrn _ptoc_LOCAL;
    int _MatchPatrn;
    int i;
    int j;
    int ConstA;
    int ConstB;
    int ConstC;
    SymbolTableRange temp;
    HdgRange NewStmnt;
    HdgRange InduxStmntLink;
    HdgRange InductnVar;
    
    i = 1;
    do
        {
        ClearArgs();
        if (gl_TempAvail(-1 , field[NodeIndex + 1]) && IdentExp(NodeIndex , patrn[i + -1]))
            {
            switch (i)
                {
                case 1 :
                    ConstB = 0;
                    InductnVar = PopArgs();
                    ConstA = PopArgs();
                    break;
                case 2 :
                    ConstB = PopArgs();
                    InductnVar = PopArgs();
                    ConstA = PopArgs();
                    break;
                case 3 :
                    ConstB = -PopArgs();
                    InductnVar = PopArgs();
                    ConstA = PopArgs();
                    break;
                case 4 :
                    ConstB = 0;
                    ConstA = PopArgs();
                    InductnVar = PopArgs();
                    break;
                case 5 :
                    ConstB = PopArgs();
                    ConstA = PopArgs();
                    InductnVar = PopArgs();
                    break;
                case 6 :
                    ConstB = -PopArgs();
                    ConstA = PopArgs();
                    InductnVar = PopArgs();
                    break;
                }
            temp = gl_GetTemporary(field[NodeIndex + 1]);
            j = SearchTable(InductnVar);
            ConstC = STable.candidate[j + -1].ConstExp;
            InduxStmntLink = STable.candidate[j + -1].StmntIndex + 3;
            NewStmnt = CrtInductnStmnt(temp , ConstA , ConstC);
            InsrtInductnStmnt(NewStmnt , InduxStmntLink);
            InsrtInitStmnt(temp , NodeIndex);
            RepLinearExp(temp , LinkNode);
            _MatchPatrn = 1;
            goto LAB_100;
            }
        if (i == 6)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    _MatchPatrn = 0;
    LAB_100 :
    return(_MatchPatrn);
    }

struct _ptoc_FRAME_force_register
  {
  char dummy;
  } ;


int force_register(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_force_register _ptoc_LOCAL;
    SymbolTableRange id;
    
    id = field[NodeIndex + 5];
    if (STab[id].SLevel == STab[ProcedureName].SLevel + 1 && STab[id].KINDTYPE.C_ModuleDcl.SClass == 1)
        {
        STab[id].KINDTYPE.C_ModuleDcl.SClass = 5;
        }
    }
int ETraverse();

struct _ptoc_FRAME_LinearExpSearch
  {
  char dummy;
  } ;


int LinearExpSearch(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_LinearExpSearch _ptoc_LOCAL;
    
    switch (shr(field[NodeIndex] , 16))
        {
        case 1 :
            break;
        case 2 :
            break;
        case 3 :
            ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
            break;
        case 4 :
            ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
            break;
        case 5 :
            if (MatchPatrn(NodeIndex , LinkNode))
                {
                ExprReduced += 1;
                }
            else
                {
                ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
                ETraverse(field[NodeIndex + 6] , NodeIndex + 6);
                }
            break;
        case 6 :
            ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
            NodeIndex = field[NodeIndex + 5];
            while (field[NodeIndex + 3] != -1)
                {
                ETraverse(field[NodeIndex + 3] , NodeIndex + 3);
                NodeIndex = field[NodeIndex + 3];
                }
            break;
        case 7 :
            ETraverse(field[NodeIndex + 6] , NodeIndex + 6);
            ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
            break;
        case 8 :
            ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
            ETraverse(field[NodeIndex + 6] , NodeIndex + 6);
            if (field[NodeIndex + 7] != -1)
                {
                ETraverse(field[NodeIndex + 7] , NodeIndex + 7);
                }
            break;
        case 10 :
            ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
            ETraverse(field[NodeIndex + 6] , NodeIndex + 6);
            break;
        case 11 :
            break;
        case 9 :
            ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
            ETraverse(field[NodeIndex + 6] , NodeIndex + 6);
            break;
        case 12 :
            ETraverse(field[NodeIndex + 6] , NodeIndex + 6);
            ETraverse(field[NodeIndex + 5] , NodeIndex + 5);
            break;
        case 13 :
            break;
        case 14 :
            break;
        }
    }

struct _ptoc_FRAME_ETraverse
  {
  char dummy;
  } ;


int ETraverse(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange LinkNode;
    {
    struct _ptoc_FRAME_ETraverse _ptoc_LOCAL;
    
    while (NodeIndex != -1)
        {
        NoNodesTraversed += 1;
        LinearExpSearch(NodeIndex , LinkNode);
        LinkNode = NodeIndex + 3;
        NodeIndex = field[NodeIndex + 3];
        }
    }

struct _ptoc_FRAME_StrengthReduce
  {
  char dummy;
  } ;


int StrengthReduce(NodeIndex, LinkNode)
HdgRange NodeIndex;
HdgRange *LinkNode;
    {
    struct _ptoc_FRAME_StrengthReduce _ptoc_LOCAL;
    
    LoopStmnt = NodeIndex;
    PrevLink = *LinkNode;
    LoopTailLink = field[NodeIndex + 6] + 4;
    if (control[71])
        {
        ConstantPropagation(LoopStmnt , -1);
        }
    CreatePatrns();
    InitSTable(1);
    switch (shr(field[NodeIndex] , 16))
        {
        case 9 :
            FindVars(field[NodeIndex + 5]);
            FindVars(field[NodeIndex + 6]);
            break;
        case 12 :
            FindVars(field[NodeIndex + 6]);
            FindVars(field[NodeIndex + 5]);
            break;
        }
    ScreenVars(LoopStmnt);
    LinearExpSearch(LoopStmnt , -1);
    *LinkNode = PrevLink;
    }
