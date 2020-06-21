/*!
  @file         Kernel_Main.hpp
  @author       UweH
  @brief        This is called by the RTE to call the kernel code
  @since        2004-11-09
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
#ifndef KERNEL_MAIN_H
#define KERNEL_MAIN_H
class RTETask_ITask;
/// This calls all init functions of all kernel components
void Kernel_Init(RTETask_ITask &task);
/// This is called by the rte and executes all kernel tasks code.
void Kernel_Main(RTETask_ITask &task);
#endif // KERNEL_MAIN_H