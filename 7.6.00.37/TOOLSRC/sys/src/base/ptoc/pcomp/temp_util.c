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
  int RelCodeArray[32768];
typedef
  struct Tmolloch *Pmolloch;
struct Tmolloch
  {
  Pmolloch next;
  unsigned char arr[262136];
  int molloch_size;
  } ;
DescriptorRange BusyDescriptorList;
DescriptorRange FreeDescriptorList;
SymbolTableRange ProcedureName;
TableRange NoStmnt;
TableRange NoExpr;
HdgRange ExcludedNode;
HdgRange IncludedNode;
int NoProc;
RelCodeArray rel_code;
RelCodeArray sym_range;
SymbolTableRange max_index;
SymbolTableRange max_value;
SetLimit NewWords;
SymbolTableRange NewVars;
int first_time;
int x_used;
int x_aktadr;
Pmolloch x_heap;
Pmolloch x_aktheap;
int used_space;
int Maxused_space;
int s_desc;
int Anz_desc;
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
int gl_FreeTemps[32];
SymbolTableRange gl_BusyTemps;
int gl_NestLevel;
int gl_TempCount;
long int MALLOC();
int FREE(long int*, int);

struct _ptoc_FRAME_gl_InitTemps
  {
  char dummy;
  } ;


int gl_InitTemps()
    {
    struct _ptoc_FRAME_gl_InitTemps _ptoc_LOCAL;
    int i;
    
    i = 0;
    do
        {
        gl_FreeTemps[i] = -1;
        if (i == 31)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    gl_BusyTemps = -1;
    gl_NestLevel = 0;
    gl_TempCount = 0;
    }

struct _ptoc_FRAME_gl_FindFreeTemp
  {
  char dummy;
  } ;


int gl_FindFreeTemp(TempType)
SymbolTableRange TempType;
    {
    struct _ptoc_FRAME_gl_FindFreeTemp _ptoc_LOCAL;
    int _gl_FindFreeTemp;
    SymbolTableRange i;
    int k;
    int found;
    
    found = 0;
    k = gl_NestLevel;
    while (!found && k >= 0)
        {
        i = gl_FreeTemps[k];
        while (!found && i != -1)
            if (STab[i].SType == TempType)
                {
                found = 1;
                }
            else
                {
                i = STab[i].SLink;
                }
        k += -1;
        }
    _gl_FindFreeTemp = found;
    return(_gl_FindFreeTemp);
    }

struct _ptoc_FRAME_gl_TempAvail
  {
  char dummy;
  } ;


int gl_TempAvail(TempIndex, TempType)
SymbolTableRange TempIndex;
SymbolTableRange TempType;
    {
    struct _ptoc_FRAME_gl_TempAvail _ptoc_LOCAL;
    int _gl_TempAvail;
    
    _gl_TempAvail = gl_NestLevel <= 31 && (TempIndex != -1 || gl_TempCount < 15 || gl_FindFreeTemp(TempType));
    return(_gl_TempAvail);
    }

struct _ptoc_FRAME_gl_GetTemporary
  {
  char dummy;
  } ;


int gl_GetTemporary(TempType)
SymbolTableRange TempType;
    {
    struct _ptoc_FRAME_gl_GetTemporary _ptoc_LOCAL;
    int _gl_GetTemporary;
    SymbolTableRange i;
    SymbolTableRange j;
    int k;
    int save_k;
    int found;
    int diff;
    
    found = 0;
    k = gl_NestLevel;
    while (!found && k >= 0)
        {
        i = gl_FreeTemps[k];
        j = -1;
        while (!found && i != -1)
            if (STab[i].SType == TempType)
                {
                save_k = k;
                found = 1;
                }
            else
                {
                j = i;
                i = STab[i].SLink;
                }
        k += -1;
        }
    if (found)
        {
        if (j == -1)
            {
            gl_FreeTemps[save_k] = STab[i].SLink;
            }
        else
            {
            STab[j].SLink = STab[i].SLink;
            }
        STab[i].SLink = -1;
        }
    else
        {
        if (STab[TempType].SType == -12)
            {
            gl_TempCount += 2;
            }
        else
            {
            gl_TempCount += 1;
            }
        i = GetTemporary(STab[ProcedureName].SType , TempType , 1);
        diff = i - max_value;
        if (NewVars >= diff)
            {
            NewVars -= diff;
            rel_code[i] = max_index;
            sym_range[max_index] = i;
            max_index += 1;
            max_value = i;
            NewWords = (max_index + 31) / 32;
            }
        }
    STab[i].SLink = gl_BusyTemps;
    gl_BusyTemps = i;
    _gl_GetTemporary = i;
    return(_gl_GetTemporary);
    }

struct _ptoc_FRAME_gl_FreeTemp
  {
  char dummy;
  } ;


int gl_FreeTemp(Temp)
SymbolTableRange Temp;
    {
    struct _ptoc_FRAME_gl_FreeTemp _ptoc_LOCAL;
    
    STab[Temp].SLink = gl_FreeTemps[gl_NestLevel];
    gl_FreeTemps[gl_NestLevel] = Temp;
    }

struct _ptoc_FRAME_gl_PushNestLevel
  {
  char dummy;
  } ;


int gl_PushNestLevel()
    {
    struct _ptoc_FRAME_gl_PushNestLevel _ptoc_LOCAL;
    
    gl_NestLevel += 1;
    }

struct _ptoc_FRAME_gl_PopNestLevel
  {
  char dummy;
  } ;


int gl_PopNestLevel(DefSet)
PSet DefSet;
    {
    struct _ptoc_FRAME_gl_PopNestLevel _ptoc_LOCAL;
    int i;
    int j;
    int next_i;
    
    if (gl_NestLevel <= 31)
        {
        i = gl_BusyTemps;
        j = -1;
        while (i != -1)
            {
            next_i = STab[i].SLink;
            if (Member_in_Set(i , DefSet))
                {
                if (j == -1)
                    {
                    gl_BusyTemps = STab[i].SLink;
                    }
                else
                    {
                    STab[j].SLink = STab[i].SLink;
                    }
                gl_FreeTemp(i);
                }
            else
                {
                j = i;
                }
            i = next_i;
            }
        i = gl_FreeTemps[gl_NestLevel + 1];
        gl_FreeTemps[gl_NestLevel + 1] = -1;
        while (i != -1)
            {
            next_i = STab[i].SLink;
            gl_FreeTemp(i);
            i = next_i;
            }
        }
    gl_NestLevel += -1;
    }

struct _ptoc_FRAME_not_SetConst
  {
  char dummy;
  } ;


int not_SetConst(index)
int index;
    {
    struct _ptoc_FRAME_not_SetConst _ptoc_LOCAL;
    int _not_SetConst;
    int ret;
    
    ret = STab[index].KINDTYPE.C_ModuleDcl.SSubClass != 4 || STab[index].KINDTYPE.C_ModuleDcl.SClass != 4 || STab[index].SName != 0 || BasisTyp(STab[index].SType) != -16;
    _not_SetConst = ret;
    return(_not_SetConst);
    }

struct _ptoc_FRAME_x_new
  {
  char dummy;
  } ;


long int x_new(size)
int size;
    {
    struct _ptoc_FRAME_x_new _ptoc_LOCAL;
    long int _x_new;
    long int allsize;
    long int arrsize;
    
    size += 8;
    if ((*(struct Tmolloch *) x_aktheap).next == (struct Tmolloch *) (int *) 0)
        {
        arrsize = (*(struct Tmolloch *) x_aktheap).molloch_size;
        allsize = arrsize + 8;
        (*(struct Tmolloch *) x_aktheap).next = (struct Tmolloch *) (int *) MALLOC(allsize , 5);
        if ((*(struct Tmolloch *) x_aktheap).next != (struct Tmolloch *) (int *) 0)
            {
            (*(struct Tmolloch *) (*(struct Tmolloch *) x_aktheap).next).next = (struct Tmolloch *) (int *) 0;
            (*(struct Tmolloch *) (*(struct Tmolloch *) x_aktheap).next).molloch_size = arrsize;
            }
        else
            {
            PasCerror("no more space ( 1 )                                                                                                                                                                                                                                             ");
            }
        }
    else
        {
        arrsize = (*(struct Tmolloch *) (*(struct Tmolloch *) x_aktheap).next).molloch_size;
        allsize = arrsize + 8;
        if (arrsize < size)
            {
            PasCerror("no more space ( 2 )                                                                                                                                                                                                                                             ");
            }
        }
    used_space += allsize;
    x_aktheap = (*(struct Tmolloch *) x_aktheap).next;
    x_aktadr = sql__ord( &(*(struct Tmolloch *) x_aktheap).arr[0]);
    x_used = sql__ord( &(*(struct Tmolloch *) x_aktheap).arr[arrsize + -1]);
    _x_new = x_aktadr;
    x_aktadr += size;
    return(_x_new);
    }

struct _ptoc_FRAME_init_heapmanager
  {
  char dummy;
  } ;


int init_heapmanager()
    {
    struct _ptoc_FRAME_init_heapmanager _ptoc_LOCAL;
    
    s_desc = sizeof(struct NodeDescriptorRecord );
    x_heap = (struct Tmolloch *) (int *) MALLOC(262144 , 5);
    (*(struct Tmolloch *) x_heap).next = (struct Tmolloch *) (int *) 0;
    (*(struct Tmolloch *) x_heap).molloch_size = 262136;
    x_aktheap = x_heap;
    x_aktadr = sql__ord( &(*(struct Tmolloch *) x_heap).arr[0]);
    x_used = sql__ord( &(*(struct Tmolloch *) x_heap).arr[262135]);
    used_space = 262144;
    Maxused_space = 0;
    }

struct _ptoc_FRAME_free_heap
  {
  char dummy;
  } ;


int free_heap(x)
Pmolloch x;
    {
    struct _ptoc_FRAME_free_heap _ptoc_LOCAL;
    
    if (x != (struct Tmolloch *) (int *) 0)
        {
        free_heap((*(struct Tmolloch *) x).next);
        FREE((long int *)sql__ord( x) , 5);
        }
    }

struct _ptoc_FRAME_close_heapmanager
  {
  char dummy;
  } ;


int close_heapmanager()
    {
    struct _ptoc_FRAME_close_heapmanager _ptoc_LOCAL;
    
    free_heap(x_heap);
    x_heap = (struct Tmolloch *) (int *) 0;
    }

struct _ptoc_FRAME_reset_heapmanager
  {
  char dummy;
  } ;


int reset_heapmanager()
    {
    struct _ptoc_FRAME_reset_heapmanager _ptoc_LOCAL;
    
    x_aktheap = x_heap;
    x_aktadr = sql__ord( &(*(struct Tmolloch *) x_heap).arr[0]);
    x_used = sql__ord( &(*(struct Tmolloch *) x_heap).arr[(*(struct Tmolloch *) x_heap).molloch_size + -1]);
    }

struct _ptoc_FRAME_init_optimizer
  {
  char dummy;
  } ;


int init_optimizer()
    {
    struct _ptoc_FRAME_init_optimizer _ptoc_LOCAL;
    
    init_heapmanager();
    if (first_time)
        {
        first_time = 0;
        NoProc = -1;
        }
    }

struct _ptoc_FRAME_close_optimizer
  {
  char dummy;
  } ;


int close_optimizer()
    {
    struct _ptoc_FRAME_close_optimizer _ptoc_LOCAL;
    
    close_heapmanager();
    }

struct _ptoc_FRAME_init_proc
  {
  char dummy;
  } ;


int init_proc()
    {
    struct _ptoc_FRAME_init_proc _ptoc_LOCAL;
    
    reset_heapmanager();
    Anz_desc = 0;
    BusyDescriptorList = (struct NodeDescriptorRecord *) (int *) 0;
    FreeDescriptorList = (struct NodeDescriptorRecord *) (int *) 0;
    NoExpr = 0;
    NoStmnt = 0;
    ExcludedNode = -1;
    IncludedNode = -1;
    }

struct _ptoc_FRAME_close_proc
  {
  char dummy;
  } ;


int close_proc()
    {
    struct _ptoc_FRAME_close_proc _ptoc_LOCAL;
    
    used_space = used_space - x_used + x_aktadr;
    if (used_space > Maxused_space)
        {
        Maxused_space = used_space;
        }
    }
