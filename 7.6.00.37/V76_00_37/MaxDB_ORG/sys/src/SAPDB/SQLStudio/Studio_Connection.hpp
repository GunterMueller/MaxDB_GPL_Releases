/*
    Copyright (c) 2000-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



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

/*!**********************************************************************

  module: Studio_Connection.hpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: SQLStudio

  description:  class for handling database connection				

  see also:		/


************************************************************************/

#ifndef STUDIO_CONNECTION_HPP
#define STUDIO_CONNECTION_HPP

#include "gpr00.h"

//must be included before vpa00..
#include <WINDOWS.H>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "vpa00sqlod32.h"

#include "hwd08dbc.h"

#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"

class Studio_Connection
{

public:

	  /*!-------------------------------------------------------------------------
		  function:     Studio_Connection
		  description:  Constructor
		  arguments:    [in] pDBCPool, pointer to DB Pool

		  return value: / 
      --------------------------------------------------------------------------*/	  
	  Studio_Connection(const sapdbwa_DBCPoolP pDbcPool,sapdbwa_DBCExclType nPoolingMode);

	  /*!-------------------------------------------------------------------------
		  function:     Studio_Connection
		  description:  Constructor
		  arguments:    [in] oCon, ref to connection object

		  return value: / 
      --------------------------------------------------------------------------*/	  
	  Studio_Connection(const Studio_Connection& oCon);

	  /*!-------------------------------------------------------------------------
		  function:     Studio_Connection
		  description:  Destructor
		  arguments:    /

		  return value: / 
      --------------------------------------------------------------------------*/	  
	  ~Studio_Connection();

	  /*!-------------------------------------------------------------------------
      function:     getDBServer
      description:  get database server
      arguments:    /

	  return value: m_sDBServer, database server 
      --------------------------------------------------------------------------*/
	  const Tools_DynamicUTF8String & getDBServer() const {return m_sDBServer;}

	  /*!-------------------------------------------------------------------------
      function:     getDBName
      description:  get db name
      arguments:    /

	  return value: m_sDBName, database name 
      --------------------------------------------------------------------------*/
	  const Tools_DynamicUTF8String & getDBName() const {return m_sDBName;}
	  
	  /*!-------------------------------------------------------------------------
      function:     getDBUser
      description:  get database user
      arguments:    /

	  return value: m_sDBUser, database user 
      --------------------------------------------------------------------------*/
	  const Tools_DynamicUTF8String & getDBUser() const {return m_sDBUser;}

	  /*!-------------------------------------------------------------------------

		  function:     getPassWord
		  description:  get password
		  arguments:    /

		  return value: m_sPassWord, passowrd of user 
      --------------------------------------------------------------------------*/
	  const Tools_DynamicUTF8String & getPassWord() const {return m_sPassWord;}

	  
	  /*!-------------------------------------------------------------------------
      function:     getODBCDriver
      description:  get ODBC driver
      arguments:    /

	  return value: m_sODBCDriver, odbc driver 
      --------------------------------------------------------------------------*/
	  const Tools_DynamicUTF8String & getODBCDriver() const { return m_sODBCDriver;}


	  /*!-------------------------------------------------------------------------
      function:     getDBCPool
      description:  get pointer of dbc pool
      arguments:    /

	  return value: m_pDbcPool,pointer of dbc pool
      --------------------------------------------------------------------------*/
	  const sapdbwa_DBCPoolP getDBCPool() const { return m_pDbcPool;}

	  /*!-------------------------------------------------------------------------
      function:     getDbcHandle
      description:  get pointer of db connection
      arguments:    /

	  return value: m_pDbcHandle,pointer of db connection handle
      --------------------------------------------------------------------------*/
	  const sapdbwa_DBCP getDBCHandle() const { return m_pDbcHandle;}
	  
	  /*!-------------------------------------------------------------------------
      function:     setDBServer
      description:  set database server
      arguments:    [in] sDBServer, database server

	  return value: / 
      --------------------------------------------------------------------------*/
	  void setDBServer(const Tools_DynamicUTF8String &sDBServer){m_sDBServer = sDBServer;}


	  /*!-------------------------------------------------------------------------
      function:     setDBName
      description:  set db name
      arguments:    [in] sDBName, database name

	  return value: / 
      --------------------------------------------------------------------------*/
	  void setDBName(const Tools_DynamicUTF8String &sDBName){m_sDBName = sDBName;}

	  /*!-------------------------------------------------------------------------
      function:     setDBUser
      description:  set database user
      arguments:    [in] sDBUser, database user

	  return value: / 
      --------------------------------------------------------------------------*/
	  void setDBUser(const Tools_DynamicUTF8String &sDBUser){m_sDBUser = sDBUser;}


	  /*!-------------------------------------------------------------------------

		  function:     setPassWord
		  description:  set password
		  arguments:    [in] sPassWord, passowrd of user

		  return value: / 
      --------------------------------------------------------------------------*/
	  void setPassWord(const Tools_DynamicUTF8String &sPassWord){m_sPassWord = sPassWord;}
	  
	  /*!-------------------------------------------------------------------------
      function:     setODBCDriver
      description:  set ODBC driver
      arguments:    [in] sODBCDriver, odbc driver

	  return value: / 
      --------------------------------------------------------------------------*/
	  void setODBCDriver(const Tools_DynamicUTF8String &sODBCDriver){m_sODBCDriver = sODBCDriver;}

	  /*!-------------------------------------------------------------------------
      function:     dbLogOff
      description:  log out of database
      arguments:    [out] sError, possible error

	  return value: true if success
      --------------------------------------------------------------------------*/
	  SAPDB_Bool dbLogOff(Tools_DynamicUTF8String&	sError);


	  /*!-------------------------------------------------------------------------
      function:     dbLogOn
      description:  log on of database
      arguments:    [out] sError, possible error

	  return value: true if success
      --------------------------------------------------------------------------*/
	  SAPDB_Bool dbLogOn(Tools_DynamicUTF8String&	sError);


	  /*!-------------------------------------------------------------------------
      function:     operator=()
      description:  copy constructor
      arguments:    [in] oCon Studio_Connection to be copied

	  return value: true if success
      --------------------------------------------------------------------------*/
	  //Studio_Connection& operator=(const Studio_Connection& oCon);

	  /*!-------------------------------------------------------------------------
    function:     setAutoCommit
    description:  set auto commit type of stored object
	arguments:	  [in] nAutoCommit, set auto commit type of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setAutoCommit(const SQLSMALLINT nAutoCommit){m_nAutoCommit = nAutoCommit;}


	/*!-------------------------------------------------------------------------
    function:     getAutoCommit
    description:  get auto commit type of stored object
	arguments:	  /
				  
				  
    return value: m_nAutoCommit, get auto commit type of stored object
    --------------------------------------------------------------------------*/
	SQLSMALLINT getAutoCommit(){return m_nAutoCommit;}

	/*!-------------------------------------------------------------------------
    function:     setSQLMode
    description:  set object type of stored object
	arguments:	  [in] emSQLMode, object type of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setSQLMode(const tpr00_SQLModeEnum eSQLMode){m_eSQLMode = eSQLMode;}

	/*!-------------------------------------------------------------------------
    function:     getSQLMode
    description:  get object type of stored object
	arguments:	  /
				  
				  
    return value: m_emSQLMode, object type of stored object
    --------------------------------------------------------------------------*/
	const tpr00_SQLModeEnum getSQLMode(){return m_eSQLMode;}

	/*!-------------------------------------------------------------------------
    function:     setIsoLevel
    description:  set isolation level of stored object
	arguments:	  [in] emIsoLevel,isolation level type of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setIsoLevel(const SQLSMALLINT nIsoLevel){m_nIsoLevel = nIsoLevel;}		


	/*!-------------------------------------------------------------------------
    function:     getIsoLevel
    description:  get isolation level of stored object
	arguments:	  /
				  
				  
    return value: m_emIsoLevel,isolation level type of stored object
    --------------------------------------------------------------------------*/
	const SQLSMALLINT getIsoLevel(){return m_nIsoLevel;}		

private:

	//data base login data
   Tools_DynamicUTF8String	m_sDBServer;
   Tools_DynamicUTF8String	m_sDBName;
   Tools_DynamicUTF8String	m_sDBUser;
   Tools_DynamicUTF8String	m_sPassWord;

   Tools_DynamicUTF8String	m_sODBCDriver;

   //data base connection handle
   sapdbwa_DBCP					m_pDbcHandle;      
   
   //data base connection pool
   sapdbwa_DBCPoolP m_pDbcPool;


   SQLSMALLINT			m_nAutoCommit;
   tpr00_SQLModeEnum	m_eSQLMode;
   SQLSMALLINT			m_nIsoLevel;

   sapdbwa_DBCExclType	m_nPoolingMode;

};


#endif