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

  module: StudioWeb_SQLWindow.hpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: WebQuery

  description:  class for a sql window

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

#ifndef STUDIOWEB_SQLWINDOW_HPP
#define STUDIOWEB_SQLWINDOW_HPP

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Definitions.hpp"
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateDsql.hpp"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Result.hpp"

#include "SAPDB/SQLStudio/Studio_StoredFileContent.hpp"

//data base file system
#include "SAPDB/SQLStudio/Studio_DBFSObject.hpp"

const SAPDB_Int4  STATEMAXNUMBER = 50;

class StudioWeb_SQLWindow
{

public:
	/*!-------------------------------------------------------------------------
		  function:     StudioWeb_SQLWindow
		  description:  StudioWeb_SQLWindow
		  arguments:	[in] poDBFS, pointer to dbfs object
						[in] bStoreItemsEnabled, flag if stored item tree is used
						
      
		  return value: / 
      --------------------------------------------------------------------------*/
	StudioWeb_SQLWindow(Studio_DBFSObject  *poDBFS,
						SAPDB_Bool		   bStoreItemsEnabled);

	~StudioWeb_SQLWindow();

	/*!-------------------------------------------------------------------------
		  function:     doService      
		  description:  handle web request

		  arguments:    [in]wa   ,WebAgent object
						[in]req  ,Web request object
						[in]rep  ,Web reply object

          return value: true if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool doService (sapdbwa_WebAgent     &wa,
	  				        sapdbwa_HttpRequest  &req,
							sapdbwa_HttpReply    &rep,
							StudioWeb_Result	 *pResult);

private:
	/*!-------------------------------------------------------------------------
		  function:     addStatementToList
		  description:  add a stataement to the statement list

		  arguments:    [in] sStatement, reference to statement 						

          return value: /
      -------------------------------------------------------------------------*/    
	void  addStatementToList(const Tools_DynamicUTF8String& sStatement);

	/*!-------------------------------------------------------------------------
		  function:     getStoredStudioQuery      
		  description:  handle a request for a stored statement from sql studio

		  arguments:    [in] wa   ,web agent object
						[in] req  ,Web request object
						[in] rep  ,Web reply object
						[out] sFileName, filename of stored object

          return value: true if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool getStoredStudioQuery(sapdbwa_WebAgent     &wa,
											sapdbwa_HttpRequest  &req,
											sapdbwa_HttpReply    &rep,
											Tools_DynamicUTF8String &sFileName);
private:	

	//array fo statementstrings
	Tools_DynamicUTF8String	*m_pStatements[STATEMAXNUMBER];
	
	SAPDB_Int2  m_lStatementMax;
	SAPDB_Int2  m_lStatementIndex;
	SAPDB_Int2  m_lStatementRun;

	Studio_StoredFileContent	*m_poSFC;
	Studio_DBFSObject			*m_poDBFS;   
	SAPDB_Bool					m_bStoreItemsEnabled;
	Tools_DynamicUTF8String		m_sLastObjectPath;
	SAPDB_Bool					m_bCloseExecuteConnection;
	
};

#endif