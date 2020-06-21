# --------------------------------------------------------------------------
# description: Container within SAPDB Kernel
#
# author:	stefan.baier@sap.com, till.luhmann@sap.com
# --------------------------------------------------------------------------
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

regex(.*\.cpp$)    -except

#
#-------------------------------------------------------------------------------
# common classes
#
#
Container_Node.hpp                  inc=
Container_NodeIterator.hpp          inc=
Container_DoubleNode.hpp            inc=
Container_DoubleNodeIterator.hpp    inc=Container_NodeIterator.hpp

#-------------------------------------------------------------------------------
# class Container_Vector
#
Container_Vector.hpp                inc=SAPDBErr_Assertions.hpp,\
                                        SAPDB_Types.hpp,\
                                        SAPDBMem_IRawAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp

Container_SortableVector.hpp		inc=Container_Vector.hpp,\
                                        SAPDBAlgo_QuickSort.hpp

#
#-------------------------------------------------------------------------------
# class Container_Stack
#
Container_Stack.hpp                 inc=Container_Vector.hpp

#
#-------------------------------------------------------------------------------
# class Container_SingleList
#
Container_List.hpp                  inc=SAPDBErr_Assertions.hpp,\
                                        SAPDB_Types.hpp,\
                                        SAPDBMem_IRawAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        Container_Node.hpp,\
                                        Container_NodeIterator.hpp

#-------------------------------------------------------------------------------
# class Container_DoubleList
#
Container_DoubleList.hpp            inc=SAPDBErr_Assertions.hpp,\
                                        SAPDB_Types.hpp,\
                                        SAPDBMem_IRawAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        Container_DoubleNode.hpp,\
                                        Container_DoubleNodeIterator.hpp

#-------------------------------------------------------------------------------
# class Container_CircularList
#
Container_CircularList.hpp          inc=SAPDBErr_Assertions.hpp,\
                                        SAPDB_Types.hpp,\
                                        SAPDBMem_IRawAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        Container_DoubleNode.hpp,\
                                        Container_DoubleNodeIterator.hpp
#-------------------------------------------------------------------------------
# class Container_Hash
#
Container_Hash.hpp				          inc=SAPDBErr_Assertions.hpp,\
                                        SAPDB_Types.hpp,\
                                        SAPDBMem_IRawAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp

#-------------------------------------------------------------------------------
# class Container_AVLTree
#
Container_AVLTree.hpp		inc=SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp,\
					SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
					geo00.h

#
#-------------------------------------------------------------------------------
# Test programs
#
Container_VectorTest.cpp            inc=SAPDBMem_DefaultBlockAllocator.hpp,\
                                        SAPDBMem_DefaultRawAllocator.hpp,\
                                        SAPDBMem_IncrementalBufferAllocator.hpp,\
                                        SAPDBMem_BuddyAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        Container_Vector.hpp

Container_ListTest.cpp              inc=SAPDBMem_DefaultBlockAllocator.hpp,\
                                        SAPDBMem_DefaultRawAllocator.hpp,\
                                        SAPDBMem_IncrementalRawAllocator.hpp,\
                                        SAPDBMem_BuddyAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        Container_List.hpp

Container_DoubleListTest.cpp        inc=SAPDBMem_DefaultRawAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        Container_DoubleList.hpp

Container_CircularListTest.cpp      inc=SAPDBMem_DefaultRawAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        Container_CircularList.hpp

Container_HashTest.cpp				      inc=SAPDBMem_DefaultRawAllocator.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        Container_Hash.hpp



