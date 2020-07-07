/*! 
  -----------------------------------------------------------------------------
 
  module: DBMSrvBHist_ListProducer.hpp
 
  -----------------------------------------------------------------------------
 
  responsible:  TiloH
 
  special area: backup history handling by the DBMServer
   
  description:  a class suppling basic constants for creating an output list

  version:      7.4.4 and higher 
  -----------------------------------------------------------------------------
 
                          Copyright (c) 2002-2005 SAP AG
 
  -----------------------------------------------------------------------------



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




*/

#if !defined(DBMSRVBHIST_LISTPRODUCER_HPP)
#define DBMSRVBHIST_LISTPRODUCER_HPP


//
//-----------------------------------------------------------------------------
// class DBMSrvBHist_ListProducer
//-----------------------------------------------------------------------------
//

class DBMSrvBHist_ListProducer
{
  protected:
    static const char * CONT_FLAG_END;
    static const char * CONT_FLAG_CONTINUE;
};

#endif
