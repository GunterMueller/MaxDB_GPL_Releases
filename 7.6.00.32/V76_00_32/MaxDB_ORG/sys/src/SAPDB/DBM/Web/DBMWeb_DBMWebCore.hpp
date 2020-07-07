/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebCore.hpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: Database Manager WebAgent Extension
  description:  the "Core" section of "the" DBMWeb Class
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

#ifndef _DBMWeb_DBMWebCore_HPP_
#define _DBMWeb_DBMWebCore_HPP_

SAPDB_Bool sendFrame        ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool pageHeader       ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool pageMenu         ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool pageWizMenu      ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool pageWizInfo      ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool pageSize         ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool pageVersion      ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool dbmLogon         ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     
 
SAPDB_Bool dbmLogoff        ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool enumDatabases    ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool sendNotImpl      ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );
     
SAPDB_Bool dbmWizard        ( sapdbwa_WebAgent      &wa,     
                              sapdbwa_HttpRequest   &request,     
                              sapdbwa_HttpReply     &reply );     

SAPDB_Bool sendMsgListError ( sapdbwa_WebAgent           & wa,
                              sapdbwa_HttpRequest        & request,     
                              sapdbwa_HttpReply          & reply,
                              const Msg_List & oMsgList,
                              const DBMCli_String        & sDatabase,
                              const char                 * sAction = NULL);

#endif // _DBMWeb_DBMWebCore_HPP_
