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
SymbolTableRange gl_IntegerPtr;
SymbolTableRange gl_AnyPtrPtr;
DescriptorRange BusyDescriptorList;
DescriptorRange FreeDescriptorList;
int ExitRef;
int ExitDef;
int EntryRef;
int EntryDef;
int x_used;
long int x_aktadr;
long int s_desc;
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
long int x_new();
int not_SetConst();
int BumpSymPtr();
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

struct _ptoc_FRAME_gl_InitSym
  {
  char dummy;
  } ;


struct _ptoc_FRAME_NewOptimizerSymbol_gl_InitSym
  {
  struct _ptoc_FRAME_gl_InitSym *_ptoc_LINK;
  char dummy;
  } ;


static
int NewOptimizerSymbol_gl_InitSym(_ptoc_LINK, id, typ, kind, level, card)
struct _ptoc_FRAME_gl_InitSym *_ptoc_LINK;
int id;
int typ;
KINDTYPE kind;
int level;
int card;
    {
    struct _ptoc_FRAME_NewOptimizerSymbol_gl_InitSym _ptoc_LOCAL;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    BumpSymPtr();
    STab[SymPtr].SName = id;
    STab[SymPtr].SType = typ;
    STab[SymPtr].SKind = kind;
    STab[SymPtr].SLevel = level;
    STab[SymPtr].SCard = card;
    STab[SymPtr].KINDTYPE.C_DummyDcl.DummyInfo.KINDTYPE.C_DummyDcl.dummy1 = -1;
    STab[SymPtr].KINDTYPE.C_DummyDcl.DummyInfo.KINDTYPE.C_DummyDcl.dummy2 = -1;
    STab[SymPtr].KINDTYPE.C_DummyDcl.DummyInfo.KINDTYPE.C_DummyDcl.dummy3 = -1;
    }

struct _ptoc_FRAME_InitSym_gl_InitSym
  {
  struct _ptoc_FRAME_gl_InitSym *_ptoc_LINK;
  char dummy;
  } ;


static
int InitSym_gl_InitSym(_ptoc_LINK, kind, t)
struct _ptoc_FRAME_gl_InitSym *_ptoc_LINK;
KINDTYPE kind;
int t;
    {
    struct _ptoc_FRAME_InitSym_gl_InitSym _ptoc_LOCAL;
    int _InitSym;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    NewOptimizerSymbol_gl_InitSym(_ptoc_LOCAL._ptoc_LINK, 0 , t , kind , 0 , 0);
    _InitSym = SymPtr;
    return(_InitSym);
    }

int gl_InitSym()
    {
    struct _ptoc_FRAME_gl_InitSym _ptoc_LOCAL;
    
    gl_IntegerPtr = InitSym_gl_InitSym(&_ptoc_LOCAL, 7 , -5);
    STab[gl_IntegerPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SLowRange = -2147483647;
    STab[gl_IntegerPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_SubRangeType.SHighRange = 2147483647;
    STab[gl_IntegerPtr].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SAlign = 32;
    STab[gl_IntegerPtr].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize = 32;
    gl_AnyPtrPtr = InitSym_gl_InitSym(&_ptoc_LOCAL, 7 , -17);
    STab[gl_AnyPtrPtr].KINDTYPE.C_TypeDcl.C_TypeDcl.C_PointerType.SDomain = gl_IntegerPtr;
    STab[gl_AnyPtrPtr].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SAlign = 32;
    STab[gl_AnyPtrPtr].KINDTYPE.C_TypeDcl.TypeInfo.KINDTYPE.C_TypeDcl.SSize = 32;
    }

struct _ptoc_FRAME_PrintDescriptor
  {
  char dummy;
  } ;


int PrintDescriptor(NodeIndex, ptoc_ptr_msg, desc1)
HdgRange NodeIndex;
STRING ptoc_ptr_msg;
NodeDescriptorType *desc1;
    {
    struct _ptoc_FRAME_PrintDescriptor _ptoc_LOCAL;
    _iorechd *ptoc_Var0;
    STRING msg;
    
    _ptoc_MOVE(char, sizeof(STRING ), msg, ptoc_ptr_msg);
    if (NodeIndex == -1)
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s%*.*s%s" , "*** " , 20 , 20 , msg , " = (");
        }
    else
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s%*.*s%c%10d%s" , "*** " , 20 , 20 , msg , 40 , NodeIndex , ") = (");
        }
    PrintSet((*desc1).def);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%c" , 59);
    PrintSet((*desc1).ref);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%c" , 41);
    if ((*desc1).continue_)
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s" , " cont");
        }
    if ((*desc1).break_)
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s" , " brk");
        }
    if ((*desc1).return_)
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s" , " ret");
        }
    if ((*desc1).goto_label)
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%s" , " lab");
        }
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    sql__fflush(output);
    }

struct _ptoc_FRAME_ListDescTab
  {
  char dummy;
  } ;


int ListDescTab()
    {
    struct _ptoc_FRAME_ListDescTab _ptoc_LOCAL;
    DescriptorRange DescPtr;
    _iorechd *ptoc_Var1;
    
    ptoc_Var1 = sql__ut(output);
    fprintf(ptoc_Var1->_fbuf , "");
    sql__writeln(ptoc_Var1);
    ptoc_Var1 = sql__ut(output);
    fprintf(ptoc_Var1->_fbuf , "%s" , "ListDescTab called");
    sql__writeln(ptoc_Var1);
    ptoc_Var1 = sql__ut(output);
    fprintf(ptoc_Var1->_fbuf , "");
    sql__writeln(ptoc_Var1);
    }

struct _ptoc_FRAME_temp_list
  {
  char dummy;
  } ;


int temp_list(list)
DescriptorRange list;
    {
    struct _ptoc_FRAME_temp_list _ptoc_LOCAL;
    int i;
    _iorechd *ptoc_Var2;
    
    i = 0;
    while (list != (struct NodeDescriptorRecord *) (int *) 0)
        {
        i += 1;
        ptoc_Var2 = sql__ut(output);
        fprintf(ptoc_Var2->_fbuf , "%10d" , sql__ord(list));
        if (i == 8)
            {
            ptoc_Var2 = sql__ut(output);
            fprintf(ptoc_Var2->_fbuf , "");
            sql__writeln(ptoc_Var2);
            i = 0;
            }
        list = (*(struct NodeDescriptorRecord *) list).next;
        }
    if (i != 0)
        {
        ptoc_Var2 = sql__ut(output);
        fprintf(ptoc_Var2->_fbuf , "");
        sql__writeln(ptoc_Var2);
        }
    }

struct _ptoc_FRAME_AvailableDescriptor
  {
  char dummy;
  } ;


DescriptorRange AvailableDescriptor()
    {
    struct _ptoc_FRAME_AvailableDescriptor _ptoc_LOCAL;
    DescriptorRange _AvailableDescriptor;
    DescriptorRange t;
    
    if (FreeDescriptorList == (struct NodeDescriptorRecord *) (int *) 0)
        {
        t = (struct NodeDescriptorRecord *) x_aktadr;
        x_aktadr += s_desc;
        if (x_aktadr > x_used)
            {
            t = (struct NodeDescriptorRecord *) x_new(s_desc);
            }
        (*(struct NodeDescriptorRecord *) t).ExitDescriptor.ref = (Set *) (int *) (sql__ord(t) + sizeof(struct NodeDescriptorRecord ) + ExitRef);
        (*(struct NodeDescriptorRecord *) t).ExitDescriptor.def = (Set *) (int *) (sql__ord(t) + sizeof(struct NodeDescriptorRecord ) + ExitDef);
        (*(struct NodeDescriptorRecord *) t).EntryDescriptor.ref = (Set *) (int *) (sql__ord(t) + sizeof(struct NodeDescriptorRecord ) + EntryRef);
        (*(struct NodeDescriptorRecord *) t).EntryDescriptor.def = (Set *) (int *) (sql__ord(t) + sizeof(struct NodeDescriptorRecord ) + EntryDef);
        }
    else
        {
        t = FreeDescriptorList;
        FreeDescriptorList = (*(struct NodeDescriptorRecord *) FreeDescriptorList).next;
        }
    if (BusyDescriptorList != (struct NodeDescriptorRecord *) (int *) 0)
        {
        (*(struct NodeDescriptorRecord *) BusyDescriptorList).prev = t;
        }
    (*(struct NodeDescriptorRecord *) t).next = BusyDescriptorList;
    BusyDescriptorList = t;
    _AvailableDescriptor = t;
    return(_AvailableDescriptor);
    }

struct _ptoc_FRAME_FreeDescriptor
  {
  char dummy;
  } ;


int FreeDescriptor(desc)
DescriptorRange desc;
    {
    struct _ptoc_FRAME_FreeDescriptor _ptoc_LOCAL;
    
    if (desc == BusyDescriptorList)
        {
        BusyDescriptorList = (*(struct NodeDescriptorRecord *) desc).next;
        }
    else
        {
        (*(struct NodeDescriptorRecord *) (*(struct NodeDescriptorRecord *) desc).prev).next = (*(struct NodeDescriptorRecord *) desc).next;
        if ((*(struct NodeDescriptorRecord *) desc).next != (struct NodeDescriptorRecord *) (int *) 0)
            {
            (*(struct NodeDescriptorRecord *) (*(struct NodeDescriptorRecord *) desc).next).prev = (*(struct NodeDescriptorRecord *) desc).prev;
            }
        }
    (*(struct NodeDescriptorRecord *) desc).next = FreeDescriptorList;
    FreeDescriptorList = desc;
    }

struct _ptoc_FRAME_ClearDescriptor
  {
  char dummy;
  } ;


int ClearDescriptor(descriptor)
NodeDescriptorType *descriptor;
    {
    struct _ptoc_FRAME_ClearDescriptor _ptoc_LOCAL;
    
    Clear_this_Set((*descriptor).ref);
    Clear_this_Set((*descriptor).def);
    (*descriptor).continue_ = 0;
    (*descriptor).break_ = 0;
    (*descriptor).return_ = 0;
    (*descriptor).goto_label = 0;
    (*descriptor).hash_code = 0;
    }

struct _ptoc_FRAME_ClearDescriptorFull
  {
  char dummy;
  } ;


int ClearDescriptorFull(descriptor)
NodeDescriptorType *descriptor;
    {
    struct _ptoc_FRAME_ClearDescriptorFull _ptoc_LOCAL;
    
    ClearFull((*descriptor).ref);
    ClearFull((*descriptor).def);
    (*descriptor).continue_ = 0;
    (*descriptor).break_ = 0;
    (*descriptor).return_ = 0;
    (*descriptor).goto_label = 0;
    (*descriptor).hash_code = 0;
    }

struct _ptoc_FRAME_ClearEntryDescriptors
  {
  char dummy;
  } ;


int ClearEntryDescriptors()
    {
    struct _ptoc_FRAME_ClearEntryDescriptors _ptoc_LOCAL;
    DescriptorRange i;
    
    i = BusyDescriptorList;
    while (i != (struct NodeDescriptorRecord *) (int *) 0)
        {
        ClearDescriptor(&(*(struct NodeDescriptorRecord *) i).EntryDescriptor);
        i = (*(struct NodeDescriptorRecord *) i).next;
        }
    }

struct _ptoc_FRAME_ClearExitDescriptors
  {
  char dummy;
  } ;


int ClearExitDescriptors()
    {
    struct _ptoc_FRAME_ClearExitDescriptors _ptoc_LOCAL;
    DescriptorRange i;
    
    PasCerror("ClearExitDescriptors called                                                                                                                                                                                                                                     ");
    }

struct _ptoc_FRAME_UnionDescriptors
  {
  char dummy;
  } ;


int UnionDescriptors(desc, desc1, desc2)
NodeDescriptorType *desc;
NodeDescriptorType *desc1;
NodeDescriptorType *desc2;
    {
    struct _ptoc_FRAME_UnionDescriptors _ptoc_LOCAL;
    
    Union_Sets((*desc).def , (*desc1).def , (*desc2).def);
    Union_Sets((*desc).ref , (*desc1).ref , (*desc2).ref);
    (*desc).continue_ = (*desc1).continue_ || (*desc2).continue_;
    (*desc).break_ = (*desc1).break_ || (*desc2).break_;
    (*desc).return_ = (*desc1).return_ || (*desc2).return_;
    (*desc).goto_label = (*desc1).goto_label || (*desc2).goto_label;
    (*desc).hash_code = (*desc1).hash_code + (*desc2).hash_code;
    }

struct _ptoc_FRAME_AssignDescriptors
  {
  char dummy;
  } ;


int AssignDescriptors(desc1, desc2)
NodeDescriptorType *desc1;
NodeDescriptorType *desc2;
    {
    struct _ptoc_FRAME_AssignDescriptors _ptoc_LOCAL;
    
    Assign_Set_to_Set((*desc1).def , (*desc2).def);
    Assign_Set_to_Set((*desc1).ref , (*desc2).ref);
    (*desc1).continue_ = (*desc2).continue_;
    (*desc1).break_ = (*desc2).break_;
    (*desc1).return_ = (*desc2).return_;
    (*desc1).goto_label = (*desc2).goto_label;
    (*desc1).hash_code = (*desc2).hash_code;
    }

struct _ptoc_FRAME_EqualDescriptors
  {
  char dummy;
  } ;


int EqualDescriptors(desc1, desc2)
NodeDescriptorType *desc1;
NodeDescriptorType *desc2;
    {
    struct _ptoc_FRAME_EqualDescriptors _ptoc_LOCAL;
    int _EqualDescriptors;
    
    if (Equal_two_Sets((*desc1).def , (*desc2).def) && Equal_two_Sets((*desc1).ref , (*desc2).ref) && (*desc1).continue_ == (*desc2).continue_ && (*desc1).break_ == (*desc2).break_ && (*desc1).return_ == (*desc2).return_ && (*desc1).goto_label == (*desc2).goto_label)
        {
        _EqualDescriptors = 1;
        }
    else
        {
        _EqualDescriptors = 0;
        }
    return(_EqualDescriptors);
    }

struct _ptoc_FRAME_DescriptorFlags
  {
  char dummy;
  } ;


int DescriptorFlags(desc1)
NodeDescriptorType *desc1;
    {
    struct _ptoc_FRAME_DescriptorFlags _ptoc_LOCAL;
    int _DescriptorFlags;
    
    _DescriptorFlags = (*desc1).continue_ || (*desc1).break_ || (*desc1).return_ || (*desc1).goto_label;
    return(_DescriptorFlags);
    }

struct _ptoc_FRAME_IsOptimizible
  {
  char dummy;
  } ;


int IsOptimizible(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_IsOptimizible _ptoc_LOCAL;
    int _IsOptimizible;
    
    switch (field[NodeIndex] & 65535)
        {
        case 4 :
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 94)
                {
                _IsOptimizible = 0;
                }
            else
                {
                _IsOptimizible = 1;
                }
            break;
        case 13 :
            NodeIndex = field[NodeIndex + 5];
            if ((field[NodeIndex] & 65535) == 94 || (field[NodeIndex] & 65535) == 95 || (field[NodeIndex] & 65535) == 96 || (field[NodeIndex] & 65535) == 97)
                {
                _IsOptimizible = 0;
                }
            else
                {
                _IsOptimizible = 1;
                }
            break;
        case 94 :
        case 8 :
        case 9 :
            _IsOptimizible = 0;
            break;
        case 95 :
        case 96 :
        case 97 :
            _IsOptimizible = 0;
            break;
        case 93 :
            switch (STab[field[NodeIndex + 1]].SType)
                {
                case -5 :
                case -3 :
                case -6 :
                case -4 :
                case -17 :
                    switch (STab[field[field[NodeIndex + 5] + 1]].SType)
                        {
                        case -5 :
                        case -3 :
                        case -6 :
                        case -4 :
                        case -17 :
                            _IsOptimizible = 0;
                            break;
                        default :
                            _IsOptimizible = 1;
                            break;
                        }
                    break;
                default :
                    _IsOptimizible = 1;
                    break;
                }
            break;
        default :
            _IsOptimizible = 1;
            break;
        }
    return(_IsOptimizible);
    }

struct _ptoc_FRAME_IsLvalOp
  {
  char dummy;
  } ;


int IsLvalOp(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_IsLvalOp _ptoc_LOCAL;
    int _IsLvalOp;
    
    switch (field[NodeIndex] & 65535)
        {
        case 94 :
        case 95 :
        case 96 :
        case 97 :
            _IsLvalOp = 1;
            break;
        default :
            _IsLvalOp = 0;
            break;
        }
    return(_IsLvalOp);
    }

struct _ptoc_FRAME_CreateAddrOfLval
  {
  char dummy;
  } ;


int CreateAddrOfLval(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_CreateAddrOfLval _ptoc_LOCAL;
    int _CreateAddrOfLval;
    
    if ((field[NodeIndex] & 65535) == 97)
        {
        _CreateAddrOfLval = field[NodeIndex + 5];
        }
    else
        {
        _CreateAddrOfLval = UnaryExpression(13 , field[NodeIndex + 1] , NodeIndex , 0 , 0);
        }
    return(_CreateAddrOfLval);
    }

struct _ptoc_FRAME_CreateAssignmentStatement
  {
  char dummy;
  } ;


int CreateAssignmentStatement(PtrToExpr, NextStmnt, target, IndexExp)
HdgRange PtrToExpr;
HdgRange NextStmnt;
SymbolTableRange target;
HdgRange IndexExp;
    {
    struct _ptoc_FRAME_CreateAssignmentStatement _ptoc_LOCAL;
    int _CreateAssignmentStatement;
    HdgRange NodeIndex;
    int ptoc_Var3;
    
    ptoc_Var3 = STab[target].SType;
    NodeIndex = LongStatement(6 , ptoc_Var3 , node3(94 , ptoc_Var3 , target , 0) , PtrToExpr , 0 , 0 , -1);
    field[NodeIndex + 3] = NextStmnt;
    _CreateAssignmentStatement = NodeIndex;
    return(_CreateAssignmentStatement);
    }

struct _ptoc_FRAME_CreateLoadNode
  {
  char dummy;
  } ;


int CreateLoadNode(source)
SymbolTableRange source;
    {
    struct _ptoc_FRAME_CreateLoadNode _ptoc_LOCAL;
    int _CreateLoadNode;
    int ptoc_Var4;
    
    ptoc_Var4 = STab[source].SType;
    _CreateLoadNode = UnaryExpression(4 , ptoc_Var4 , node3(94 , ptoc_Var4 , source , 0) , 0 , 0);
    return(_CreateLoadNode);
    }

struct _ptoc_FRAME_EndTraverse
  {
  char dummy;
  } ;


int EndTraverse(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_EndTraverse _ptoc_LOCAL;
    int _EndTraverse;
    
    if (NodeIndex == -1)
        {
        _EndTraverse = 1;
        }
    else
        {
        switch (field[NodeIndex] & 65535)
            {
            case 98 :
            case 87 :
            case 86 :
            case 101 :
                _EndTraverse = 1;
                break;
            default :
                _EndTraverse = 0;
                break;
            }
        }
    return(_EndTraverse);
    }

struct _ptoc_FRAME_NextCase
  {
  char dummy;
  } ;


int NextCase(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_NextCase _ptoc_LOCAL;
    int _NextCase;
    
    while (NodeIndex != -1 && shr(field[NodeIndex] , 16) != 11)
        NodeIndex = field[NodeIndex + 3];
    _NextCase = NodeIndex;
    if (NodeIndex != -1)
        {
        NodeIndex = field[NodeIndex + 3];
        while (NodeIndex != -1 && shr(field[NodeIndex] , 16) == 11)
            {
            _NextCase = NodeIndex;
            NodeIndex = field[NodeIndex + 3];
            }
        }
    return(_NextCase);
    }
