/*!
  @file           DBMSrvKnl_SQLField.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to Kernel Communication - Specification

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef _DBMSrvKnl_SQLField_HPP_
#define _DBMSrvKnl_SQLField_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "vsp001.h"
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"

/*! @brief field value */
class DBMSrvKnl_SQLField {
private:
  friend class DBMSrvKnl_SQLRecord;
  friend class DBMSrvKnl_Session;

  /*! @brief   data type */
  typedef enum {
    typeInt,
    typeString
  } DataType;

  /*! @ brief constructor */
  DBMSrvKnl_SQLField
    ( tsp1_param_info * aInfo,
      DataType          aType);

  /*! @brief member for paraminfo */
  tsp1_param_info         oInfo;
  /*! @ brief member fro requested type */
  DataType                oType;
  /*! @ brief member for integer */
  SAPDB_Int4              iValue;
  /*! @ brief member for string */
  Tools_DynamicUTF8String sValue;
  /*! @ brief null flag */
  bool                    bNull;

}; // end class DBMSrvKnl_SQLField

#endif /* _DBMSrvKnl_SQLField_HPP_ */
