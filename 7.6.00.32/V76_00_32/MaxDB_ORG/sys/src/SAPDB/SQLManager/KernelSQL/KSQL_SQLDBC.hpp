/*!
  @file           KSQL_SQLDBC.hpp
  @author         D039759
  @ingroup        KSQL
  @brief          Encapsulation of SQLDBC header for use of different namespace.
  @see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (C) 2001-2005 SAP AG

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
#ifndef KSQL_SQLDBC_HPP
#define KSQL_SQLDBC_HPP 

#undef  SQLDBC
#define SQLDBC SQLDBC_Kernel

#define SQLDBC_RUNTIME_IN_NAMESPACE

#ifndef IFR_NAMESPACE
#  define IFR_NAMESPACE SQLDBC_Kernel
#endif

#include "Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "Interfaces/SQLDBC/SQLDBC.h"

#endif
