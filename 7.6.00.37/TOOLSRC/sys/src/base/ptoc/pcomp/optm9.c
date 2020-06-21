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
  int HdgRange;
long int field[1];
int control[256];
int IsConstant();
int FoldUnary();
int FoldBinary();
int FoldSubscript();
int FoldTree();
int shl();
int shr();
typedef
  int Set[256];
typedef
  Set *PSet;
typedef
  struct ptoc_Type0
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
  struct ptoc_Type1
    {
    SymbolTableRange SymTabInx;
    NodeDescriptorType ProcExitDescr;
    } ProcTableArray[101];
typedef
  unsigned TraversalType;
ProcTableArray ProcTable;
int NoProc;
int InitialSize;
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
int TransformLoop();
int TransformProc();
int OptIfthenelse();
int IntegrateProcedure();
int OptLongStmnt();
int ConstantFolding();
int ConstantPropagation();
int StrengthReduce();
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
int OptimizeProg();
int OTraverse();

struct _ptoc_FRAME_SelectForOptimization
  {
  char dummy;
  } ;


int SelectForOptimization(NodeIndex, LinkNode, traversal)
HdgRange NodeIndex;
HdgRange LinkNode;
TraversalType traversal;
    {
    struct _ptoc_FRAME_SelectForOptimization _ptoc_LOCAL;
    DescriptorRange DescPtr;
    PSet set1;
    
    switch (shr(field[NodeIndex] , 16))
        {
        case 1 :
            break;
        case 2 :
            break;
        case 3 :
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 1);
            break;
        case 4 :
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
            break;
        case 5 :
            if ((field[NodeIndex] & 65535) == 95)
                {
                if (control[80])
                    {
                    FoldSubscript(NodeIndex);
                    }
                }
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
            OTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 0);
            break;
        case 6 :
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
            break;
        case 7 :
            if ((field[NodeIndex] & 65535) == 6)
                {
                if (control[80])
                    {
                    OptLongStmnt(NodeIndex);
                    }
                }
            else
                {
                if ((field[NodeIndex] & 65535) == 40)
                    {
                    if (control[73])
                        {
                        IntegrateProcedure(field[NodeIndex + 7] , NodeIndex);
                        }
                    }
                }
            OTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 0);
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
            break;
        case 8 :
            if (traversal == 1)
                {
                gl_PushNestLevel();
                OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
                OTraverse(field[NodeIndex + 6] , NodeIndex + 6 , traversal);
                if (field[NodeIndex + 7] != -1)
                    {
                    OTraverse(field[NodeIndex + 7] , NodeIndex + 7 , traversal);
                    }
                if (control[66])
                    {
                    OptIfthenelse(NodeIndex , LinkNode , traversal);
                    }
                DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
                gl_PopNestLevel((*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor.def);
                }
            else
                {
                OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
                OTraverse(field[NodeIndex + 6] , NodeIndex + 6 , traversal);
                if (field[NodeIndex + 7] != -1)
                    {
                    OTraverse(field[NodeIndex + 7] , NodeIndex + 7 , traversal);
                    }
                }
            break;
        case 9 :
            gl_PushNestLevel();
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
            OTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 1);
            if (control[83])
                {
                StrengthReduce(NodeIndex , &LinkNode);
                }
            if (control[65])
                {
                TransformLoop(NodeIndex , LinkNode);
                }
            NodeIndex = field[LinkNode];
            while (shr(field[NodeIndex] , 16) == 7)
                {
                LinkNode = NodeIndex + 3;
                NodeIndex = field[NodeIndex + 3];
                }
            if (shr(field[NodeIndex] , 16) == 8)
                {
                if (control[66])
                    {
                    OptIfthenelse(NodeIndex , LinkNode , 1);
                    }
                }
            DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
            gl_PopNestLevel((*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor.def);
            break;
        case 10 :
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
            OTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 1);
            break;
        case 11 :
            break;
        case 12 :
            gl_PushNestLevel();
            OTraverse(field[NodeIndex + 6] , NodeIndex + 6 , 1);
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 0);
            if (control[83])
                {
                StrengthReduce(NodeIndex , &LinkNode);
                }
            if (control[65])
                {
                TransformLoop(NodeIndex , LinkNode);
                }
            DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
            gl_PopNestLevel((*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor.def);
            break;
        case 13 :
            gl_PushNestLevel();
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 1);
            if (control[67])
                {
                TransformProc(NodeIndex , NodeIndex + 5);
                }
            DescPtr = (struct NodeDescriptorRecord *) (int *) field[NodeIndex + 2];
            gl_PopNestLevel((*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor.def);
            if (control[67])
                {
                sql__new(&set1 , sizeof(Set ));
                ProcTable[NoProc].ProcExitDescr.ref = set1;
                sql__new(&set1 , sizeof(Set ));
                ProcTable[NoProc].ProcExitDescr.def = set1;
                ClearDescriptorFull(&ProcTable[NoProc].ProcExitDescr);
                AssignDescriptors(&ProcTable[NoProc].ProcExitDescr , &(*(struct NodeDescriptorRecord *) DescPtr).ExitDescriptor);
                }
            break;
        case 14 :
            OTraverse(field[NodeIndex + 5] , NodeIndex + 5 , 1);
            break;
        }
    }

struct _ptoc_FRAME_OTraverse
  {
  char dummy;
  } ;


int OTraverse(NodeIndex, LinkNode, traversal)
HdgRange NodeIndex;
HdgRange LinkNode;
TraversalType traversal;
    {
    struct _ptoc_FRAME_OTraverse _ptoc_LOCAL;
    HdgRange SaveNextLink;
    
    while (NodeIndex != -1)
        {
        InitialSize += 1;
        SaveNextLink = field[NodeIndex + 3];
        SelectForOptimization(NodeIndex , LinkNode , traversal);
        NodeIndex = field[LinkNode];
        do
            {
            LinkNode = NodeIndex + 3;
            NodeIndex = field[NodeIndex + 3];
            }
        while (!(NodeIndex == SaveNextLink));
        }
    }

struct _ptoc_FRAME_OptimizeProg
  {
  char dummy;
  } ;


int OptimizeProg(NodeIndex)
int NodeIndex;
    {
    struct _ptoc_FRAME_OptimizeProg _ptoc_LOCAL;
    
    gl_InitTemps();
    SelectForOptimization(NodeIndex , -1 , 1);
    if (control[71])
        {
        ConstantPropagation(NodeIndex , -1);
        }
    if (control[70])
        {
        ConstantFolding(NodeIndex , -1);
        }
    }
