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
int ErrorCount;
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
int DumptreeI();
int ListHdg();
typedef
  int TableRange;
typedef
  int SetLimit;
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
  int RelCodeArray[32768];
DescriptorRange BusyDescriptorList;
DescriptorRange FreeDescriptorList;
SymbolTableRange ProcedureName;
int OptimizedIf;
int OptimizedLoop;
ProcTableArray ProcTable;
TableRange NoStmnt;
TableRange NoExpr;
HdgRange ExcludedNode;
HdgRange IncludedNode;
int opt_glob;
int NoProc;
int RedundantStmnts;
int RedundantExpr;
int LoopsConverted;
int FoldExp;
int StmntsOutsideLoop;
int ExprOutsideLoop;
int LongStmntOpt;
int OptIndex;
int InitialSize;
int NoNodesTraversed;
int PropagatedExp;
int ExprReduced;
RelCodeArray rel_code;
RelCodeArray sym_range;
SymbolTableRange max_index;
SymbolTableRange max_value;
SetLimit NewWords;
SetLimit NewWordsPlus;
SymbolTableRange NewVars;
int first_time;
int ExitRef;
int ExitDef;
int EntryRef;
int EntryDef;
int Maxused_space;
int s_desc;
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
int OptimizeProg();
int OptdriverI();
int optimize();

struct _ptoc_FRAME_OptdriverI
  {
  char dummy;
  } ;


int OptdriverI()
    {
    struct _ptoc_FRAME_OptdriverI _ptoc_LOCAL;
    DescriptorRange i;
    
    first_time = 1;
    OptimizedIf = 0;
    OptimizedLoop = 0;
    IncludedNode = -1;
    BusyDescriptorList = (struct NodeDescriptorRecord *) (int *) 0;
    FreeDescriptorList = (struct NodeDescriptorRecord *) (int *) 0;
    NoExpr = 0;
    NoStmnt = 0;
    ExcludedNode = -1;
    ExprReduced = 0;
    PropagatedExp = 0;
    FoldExp = 0;
    RedundantStmnts = 0;
    RedundantExpr = 0;
    LoopsConverted = 0;
    StmntsOutsideLoop = 0;
    ExprOutsideLoop = 0;
    LongStmntOpt = 0;
    OptIndex = 0;
    NoNodesTraversed = 0;
    InitialSize = 0;
    }

struct _ptoc_FRAME_statistics
  {
  char dummy;
  } ;


int statistics()
    {
    struct _ptoc_FRAME_statistics _ptoc_LOCAL;
    _iorechd *ptoc_Var0;
    
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s" , "optimization statistics");
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "number expressions reduced           = " , ExprReduced);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "number redundant statements removed  = " , RedundantStmnts);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "number redundant expressions removed = " , RedundantExpr);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "number statements moved outside loop = " , StmntsOutsideLoop);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "number expressions moved outside loop= " , ExprOutsideLoop);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "number long statements optimized     = " , LongStmntOpt);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "SymTab - entries       : " , SymPtr);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "vars and params        : " , max_index);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "size of sets           : " , NewWords);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "max size of sets       : " , NewWordsPlus);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%s%10d" , "used space by optimizer: " , Maxused_space);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    }

struct _ptoc_FRAME_init_relatives
  {
  char dummy;
  } ;


struct _ptoc_FRAME_M_M_init_relatives
  {
  struct _ptoc_FRAME_init_relatives *_ptoc_LINK;
  char dummy;
  } ;


static
int M_M_init_relatives(_ptoc_LINK, a, b)
struct _ptoc_FRAME_init_relatives *_ptoc_LINK;
int a;
int b;
    {
    struct _ptoc_FRAME_M_M_init_relatives _ptoc_LOCAL;
    int _M_M;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (a < b)
        {
        _M_M = a;
        }
    else
        {
        _M_M = b;
        }
    return(_M_M);
    }

int init_relatives()
    {
    struct _ptoc_FRAME_init_relatives _ptoc_LOCAL;
    int _init_relatives;
    int bis;
    int i;
    int delta;
    int stop;
    int ptoc_Var1;
    int ptoc_Var2;
    int ptoc_Var3;
    
    i = give_MrX();
    max_index = 0;
    if (SymPtr > 8191)
        {
        bis = 8191;
        max_value = 8191;
        }
    else
        {
        bis = SymPtr;
        max_value = SymPtr;
        }
    ptoc_Var1 = bis;
    if (0 <= ptoc_Var1)
        {
        i = 0;
        do
            {
            switch (STab[i].SKind)
                {
                case 9 :
                case 14 :
                    rel_code[i] = max_index;
                    sym_range[max_index] = i;
                    max_index += 1;
                    break;
                }
            if (i == ptoc_Var1)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    NewWords = (max_index + 31) / 32;
    stop = 0;
    delta = 2;
    if (!stop)
        {
        ptoc_Var2 = NewWords * 32 - max_index;
        ptoc_Var3 = 8191 - max_value;
        do
            {
            NewVars = ptoc_Var2 + delta * 32;
            NewVars = M_M_init_relatives(&_ptoc_LOCAL, NewVars , ptoc_Var3);
            NewWordsPlus = (max_index + NewVars + 31) / 32;
            if (NewWordsPlus > 256)
                {
                if (delta > 0)
                    {
                    delta += -1;
                    }
                else
                    {
                    stop = 1;
                    _init_relatives = 0;
                    }
                }
            else
                {
                stop = 1;
                _init_relatives = 1;
                }
            }
        while (!stop);
        }
    s_desc = sizeof(struct NodeDescriptorRecord ) + NewWordsPlus * sizeof(int ) * 4;
    EntryRef = 0;
    EntryDef = NewWordsPlus * 4;
    ExitRef = NewWordsPlus * 8;
    ExitDef = NewWordsPlus * 12;
    return(_init_relatives);
    }

struct _ptoc_FRAME_OptProg
  {
  char dummy;
  } ;


int OptProg()
    {
    struct _ptoc_FRAME_OptProg _ptoc_LOCAL;
    HdgRange NodeIndex;
    int last_tian;
    _iorechd *ptoc_Var4;
    
    init_optimizer();
    opt_glob = init_relatives();
    if (!opt_glob)
        {
        goto LAB_1;
        }
    NodeIndex = HdgHead;
    while (NodeIndex != -1)
        {
        if ((field[NodeIndex] & 65535) != 105)
            {
            if (NoProc < 100)
                {
                NoProc += 1;
                }
            ProcedureName = field[NodeIndex + 7];
            ProcTable[NoProc].SymTabInx = ProcedureName;
            ExcludedNode = -1;
            IncludedNode = -1;
            if (control[65])
                {
                OptimizedLoop = 1;
                }
            if (control[66])
                {
                OptimizedIf = 1;
                }
            init_proc();
            last_tian = field[NodeIndex + 7];
            STab[last_tian].SFeld1 = SymPtr + 1;
            OptimizeProg(NodeIndex);
            last_tian = field[NodeIndex + 7];
            STab[last_tian].SFeld2 = SymPtr;
            close_proc();
            }
        NodeIndex = field[NodeIndex + 3];
        }
    LAB_1 :
    close_optimizer();
    if (control[72])
        {
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "");
        sql__writeln(ptoc_Var4);
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "");
        sql__writeln(ptoc_Var4);
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "%s" , "optimized hdg");
        sql__writeln(ptoc_Var4);
        ListHdg(HdgHead);
        }
    if (control[86])
        {
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "");
        sql__writeln(ptoc_Var4);
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "");
        sql__writeln(ptoc_Var4);
        ptoc_Var4 = sql__ut(output);
        fprintf(ptoc_Var4->_fbuf , "%s" , "descriptor table");
        sql__writeln(ptoc_Var4);
        ListDescTab();
        }
    if (control[118])
        {
        statistics();
        }
    }

struct _ptoc_FRAME_optimize
  {
  char dummy;
  } ;


int optimize()
    {
    struct _ptoc_FRAME_optimize _ptoc_LOCAL;
    
    if (ErrorCount == 0)
        {
        gl_InitSym();
        OptProg();
        }
    }
