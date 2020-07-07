/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebCheck.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  the "Check" section of "the" DBMWeb Class
  remarks:      ATTENTION! This not a full qualified header file! 
                It contains only some private member function for the 
                class DBMWeb_DBMWeb and must only included within the 
                class declaration in DBMWeb_DBMWeb.hpp.
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef _DBMWEB_DBMWEBSTATE_HPP_
#define _DBMWEB_DBMWEBSTATE_HPP_

SAPDB_Bool dbState ( sapdbwa_WebAgent    & wa,
                     sapdbwa_HttpRequest & request,
                     sapdbwa_HttpReply   & reply );

#endif // _DBMWEB_DBMWEBSTATE_HPP_
