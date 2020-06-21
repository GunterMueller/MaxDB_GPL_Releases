/**************************************************************************/
/*!

  @file         Kernel_Timeout.hpp
  @author       UweH
  @brief        This implements the class Kernel_Timeout which is the realization of the timeout task.
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
#ifndef KERNEL_TIMEOUT_HPP
#define KERNEL_TIMEOUT_HPP
class Trans_Context;
/// This class relizes the timeout task
class Kernel_Timeout
{
public:
    void Run(Trans_Context &context);
private:
};
#endif /*KERNEL_TIMEOUT_HPP*/
