/*!
  \file    DBMSrvExpHlp_PreconditionsPart.hpp
  \author  TiloH
  \ingroup Basics for DBMServer commands explain and help
  \brief   definition of a class for accessing the preconditions part
           of an explain string

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#if !defined(DBMSrvExpHlp_PreconditionsPart_hpp)
#define DBMSrvExpHlp_PreconditionsPart_hpp

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp"


//-----------------------------------------------------------------------------
// class DBMSrvExpHlp_PreconditionsPart
//-----------------------------------------------------------------------------

//! \brief a class filtering the preconditions part from an explain string
class DBMSrvExpHlp_PreconditionsPart: public DBMSrvExpHlp_Part
{
  public:
    DBMSrvExpHlp_PreconditionsPart(const char * InfoString)
      :DBMSrvExpHlp_Part("@preconditions", InfoString, false)
    {
    }
};

#endif
