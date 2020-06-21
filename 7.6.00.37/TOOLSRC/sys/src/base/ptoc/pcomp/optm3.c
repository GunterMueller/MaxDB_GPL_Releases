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
int OptimizedIf;
int OptimizedLoop;
HdgRange ExcludedNode;
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
int BuildEntryDescriptors();
int EntryTraverse();

struct _ptoc_FRAME_EntryTraverse
  {
  char dummy;
  } ;


int EntryTraverse(NodeIndex, PrevNode)
HdgRange NodeIndex;
HdgRange PrevNode;
    {
    struct _ptoc_FRAME_EntryTraverse _ptoc_LOCAL;
    
    while (NodeIndex != -1)
        {
        NoNodesTraversed += 1;
        if (NodeIndex != ExcludedNode)
            {
            BuildEntryDescriptors(NodeIndex , PrevNode);
            if (shr(field[NodeIndex] , 16) != 1)
                {
                PrevNode = NodeIndex;
                }
            }
        NodeIndex = field[NodeIndex + 3];
        }
    }

struct _ptoc_FRAME_BuildEntryDescriptors
  {
  HdgRange NodeIndex;
  HdgRange PrevNode;
  DescriptorRange DescPtr;
  DescriptorRange PrevDescPtr;
  char dummy;
  } ;


struct _ptoc_FRAME_ConstructEntryDsc_BuildEntryDescriptors
  {
  struct _ptoc_FRAME_BuildEntryDescriptors *_ptoc_LINK;
  char dummy;
  } ;


static
int ConstructEntryDsc_BuildEntryDescriptors(_ptoc_LINK)
struct _ptoc_FRAME_BuildEntryDescriptors *_ptoc_LINK;
    {
    struct _ptoc_FRAME_ConstructEntryDsc_BuildEntryDescriptors _ptoc_LOCAL;
    int CreatedEmptyDesc;
    _ptoc_LOCAL._ptoc_LINK = _ptoc_LINK;
    if (shr(field[_ptoc_LOCAL._ptoc_LINK->NodeIndex] , 16) != 1)
        {
        _ptoc_LOCAL._ptoc_LINK->DescPtr = (struct NodeDescriptorRecord *) (int *) field[_ptoc_LOCAL._ptoc_LINK->NodeIndex + 2];
        if (_ptoc_LOCAL._ptoc_LINK->PrevNode != -1)
            {
            _ptoc_LOCAL._ptoc_LINK->PrevDescPtr = (struct NodeDescriptorRecord *) (int *) field[_ptoc_LOCAL._ptoc_LINK->PrevNode + 2];
            if (_ptoc_LOCAL._ptoc_LINK->PrevDescPtr != (struct NodeDescriptorRecord *) (int *) 0)
                {
                CreatedEmptyDesc = 0;
                }
            else
                {
                _ptoc_LOCAL._ptoc_LINK->PrevDescPtr = AvailableDescriptor();
                ClearDescriptor(&(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->PrevDescPtr).ExitDescriptor);
                ClearDescriptor(&(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->PrevDescPtr).EntryDescriptor);
                CreatedEmptyDesc = 1;
                }
            }
        else
            {
            _ptoc_LOCAL._ptoc_LINK->PrevDescPtr = AvailableDescriptor();
            ClearDescriptor(&(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->PrevDescPtr).ExitDescriptor);
            ClearDescriptor(&(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->PrevDescPtr).EntryDescriptor);
            CreatedEmptyDesc = 1;
            }
        UnionDescriptors(&(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).EntryDescriptor , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->PrevDescPtr).EntryDescriptor , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->PrevDescPtr).ExitDescriptor);
        if (control[86])
            {
            PrintDescriptor(_ptoc_LOCAL._ptoc_LINK->NodeIndex , "entry                                                                                                                                                                                                                                                           " , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL._ptoc_LINK->DescPtr).EntryDescriptor);
            }
        if (CreatedEmptyDesc)
            {
            FreeDescriptor(_ptoc_LOCAL._ptoc_LINK->PrevDescPtr);
            }
        }
    }

int BuildEntryDescriptors(NodeIndex, PrevNode)
HdgRange NodeIndex;
HdgRange PrevNode;
    {
    struct _ptoc_FRAME_BuildEntryDescriptors _ptoc_LOCAL;
    
    _ptoc_LOCAL.NodeIndex = NodeIndex;
    _ptoc_LOCAL.PrevNode = PrevNode;
    if (_ptoc_LOCAL.NodeIndex == ExcludedNode || _ptoc_LOCAL.NodeIndex == -1)
        {
        goto LAB_1;
        }
    ConstructEntryDsc_BuildEntryDescriptors(&_ptoc_LOCAL);
    switch (shr(field[_ptoc_LOCAL.NodeIndex] , 16))
        {
        case 1 :
            break;
        case 2 :
            break;
        case 3 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            break;
        case 4 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            break;
        case 5 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            if (shr(field[field[_ptoc_LOCAL.NodeIndex + 5]] , 16) != 1)
                {
                EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 6] , field[_ptoc_LOCAL.NodeIndex + 5]);
                }
            else
                {
                EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 6] , _ptoc_LOCAL.PrevNode);
                }
            break;
        case 6 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            break;
        case 7 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 6] , _ptoc_LOCAL.PrevNode);
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            break;
        case 8 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            if (!OptimizedIf)
                {
                EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 6] , field[_ptoc_LOCAL.NodeIndex + 5]);
                if (field[_ptoc_LOCAL.NodeIndex + 7] != -1)
                    {
                    EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 7] , field[_ptoc_LOCAL.NodeIndex + 5]);
                    }
                }
            break;
        case 9 :
            UnionDescriptors(&(*(struct NodeDescriptorRecord *) _ptoc_LOCAL.DescPtr).EntryDescriptor , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL.DescPtr).EntryDescriptor , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL.DescPtr).ExitDescriptor);
            if (!OptimizedLoop)
                {
                EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.NodeIndex);
                EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 6] , _ptoc_LOCAL.NodeIndex);
                }
            break;
        case 10 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 6] , field[_ptoc_LOCAL.NodeIndex + 5]);
            break;
        case 11 :
            break;
        case 12 :
            UnionDescriptors(&(*(struct NodeDescriptorRecord *) _ptoc_LOCAL.DescPtr).EntryDescriptor , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL.DescPtr).EntryDescriptor , &(*(struct NodeDescriptorRecord *) _ptoc_LOCAL.DescPtr).ExitDescriptor);
            if (!OptimizedLoop)
                {
                EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 6] , _ptoc_LOCAL.NodeIndex);
                EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.NodeIndex);
                }
            break;
        case 13 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            break;
        case 14 :
            EntryTraverse(field[_ptoc_LOCAL.NodeIndex + 5] , _ptoc_LOCAL.PrevNode);
            break;
        }
    LAB_1 :
    ;
    }
