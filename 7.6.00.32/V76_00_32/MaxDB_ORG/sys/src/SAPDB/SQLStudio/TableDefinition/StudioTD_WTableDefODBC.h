/*!===========================================================================

  module:       StudioTD_WTableDefODBC.h

  ------------------------------------------------------------------------------

  responsible:  WolfgangA

  special area: SQL Studio

  description:  Definition of the ODBC enabled table definition class StudioTD_WTableDefODBC.

  see:          /

  created:      2000-05-29

  updated:      2000-05-29

  ------------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG
  



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




============================================================================*/

#ifndef STUDIOTD_WTABLEDEFODBC_H
#define STUDIOTD_WTABLEDEFODBC_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/TableDefinition/StudioTD_TableDef.h"
#include "SQLStudio/OAL/StudioOAL_WResult.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*!===========================================================================
  class: StudioTD_WTableDefODBC
 
  description: 
    Class for managing a table definition with database connection via ODBC.

    It contains all the attributes of a table definition and methods for
    creating SQL statements (CREATE or ALTER TABLE) or reading an existing
    table definition from the database.
============================================================================*/
class StudioTD_WTableDefODBC : public StudioTD_TableDef
{
public:
  /*!-------------------------------------------------------------------------
    function:     StudioTD_WTableDefODBC
    description:  Constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_WTableDefODBC ();
  /*!-------------------------------------------------------------------------
    function:     StudioTD_WTableDefODBC
    description:  Copy constructor of the class.
  --------------------------------------------------------------------------*/
  StudioTD_WTableDefODBC (const StudioTD_WTableDefODBC& table);
  /*!-------------------------------------------------------------------------
    function:     ~StudioTD_WTableDefODBC
    description:  Destructor of the class.
  --------------------------------------------------------------------------*/
  ~StudioTD_WTableDefODBC();

  /*!-------------------------------------------------------------------------
    function:     setConnection
    description:  Initialize the connection handles used for an ODBC connection
                  to the database.
    arguments:    dbc - database connection handle
                  env - ODBC environment handle
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  SAPDB_Bool      setConnection ( SQLHANDLE hdbc, SQLHENV henv);

private:
  /*!-------------------------------------------------------------------------
    function:     executeSQL
    description:  Execute the given SQL statement.
    arguments:    statement - SQL statement to be executed
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  virtual SAPDB_Bool executeSQL (StudioTD_String& statement);
  /*!-------------------------------------------------------------------------
    function:     readResult
    description:  Reads the result of an executed SQL statement record by record.
    returns:      SAPDB_Bool
  --------------------------------------------------------------------------*/
  virtual SAPDB_Bool readResult (StudioTD_String *record,SAPDB_Bool* bIsNull);
  
      
  StudioOAL_WResult         *mResult;               // Result object
  SQLHENV                   mEnv;                  // ODBC environment handle
  SQLHANDLE                 mDBC;                  // Database connection handle

};  /*! endclass: StudioTD_WTableDefODBC */

#endif   /* StudioTD_WTableDefODBC_H */
