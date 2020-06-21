/*!
  @file         Kernel_Initialization.hpp
  @author       UweH
  @brief        This calls all components initializers when the kernel is before state ADMIN
  @since        2004-11-03
*/
/*!
\if EMIT_LICENCE

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/
#ifndef KERNEL_INITIALIZATION_H
#define KERNEL_INITIALIZATION_H
#include "ggg00.h" // tgg00_TransContext
class RTETask_ITask;
class Trans_Context;
class Msg_List;
class SAPDBMem_RawAllocator;
struct tak_all_command_glob;
/// creates a new sub allocator for the kernel task context
SAPDBMem_RawAllocator* Kernel_CreateTransContextAllocator (RTETask_ITask &task);
/// deletes a sub allocator for the kernel task context
void Kernel_DeleteTransContextAllocator(Trans_Context &trans);
/// This allocates some session buffer
bool Kernel_AllocateSessionBuffers (tak_all_command_glob &acv);
/// This deallocates the previously allocated session buffers
void Kernel_DeallocateSessionBuffers ( tak_all_command_glob &acv,
                                       bool                  free_packets );
/// initializes the session context and assigns the SQLMan ErrorList to the transcontext.
void Kernel_InitSqlContext (tak_all_command_glob &acv,
                                tgg00_TransContext   &trans);
/// This starts a non user task, called by Kernel_Main
void Kernel_NonUserTask ( RTETask_ITask &task,
                          Trans_Context &context,
                          Msg_List      &errlist );
/// This is called by the runtime before any kernel task is executed.
void Kernel_Init(RTETask_ITask &task);
#endif // KERNEL_INITIALIZATION_H