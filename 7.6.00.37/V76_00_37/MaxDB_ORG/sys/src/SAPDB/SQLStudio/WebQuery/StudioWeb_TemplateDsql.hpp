/*    

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

  module: StudioWeb_Template.hpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  sql window template class for WebQuery

************************************************************************/

#ifndef STUDIOWEB_TEMPLATEDSQL_HPP
#define STUDIOWEB_TEMPLATEDSQL_HPP

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "ToolsCommon/Tools_Template.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_HtmlFragments.h"
#include "SAPDB/SQLStudio/Studio_StoredFileContent.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        StudioWeb_TemplateDsql
  -----------------------------------------------------------------------------
  description:  An instance of this class represents a HTML-Page with a 
                Info Table.
  -----------------------------------------------------------------------------
*/
class StudioWeb_TemplateDsql : public Tools_Template 
{
  public:

     /*!-------------------------------------------------------------------------
		  function:     StudioWeb_WebQuery
		  description:  Constructor
      
		  return value: / 
      
		  arguments:    [in] wa,  webagent instance
      
      -------------------------------------------------------------------------*/
     StudioWeb_TemplateDsql (sapdbwa_WebAgent & wa);

	 /*!-------------------------------------------------------------------------
		  function:     setNextDisabled
		  description:  set next button disabled
      
		  return value: / 
      
		  arguments:    [in] bDisabled, is button disabled
      
      -------------------------------------------------------------------------*/
	 void setNextDisabled(const SAPDB_Bool &bDisabled){m_bNextDisabled = bDisabled;}

	 /*!-------------------------------------------------------------------------
		  function:     setPrevDisabled
		  description:  set previous button disabled
      
		  return value: / 
      
		  arguments:    [in] bDisabled, is button disabled
      
      -------------------------------------------------------------------------*/
	 void setPrevDisabled(const SAPDB_Bool &bDisabled){m_bPrevDisabled = bDisabled;}

	 /*!-------------------------------------------------------------------------
		  function:     setStoreItemVisible
		  description:  set the store item button visible
      
		  return value: / 
      
		  arguments:    [in] bVisible, is button disabled
      
      -------------------------------------------------------------------------*/
	 void setStoreItemVisible(const SAPDB_Bool &bVisible){m_bStoreItemVisible = bVisible;}

	 /*!-------------------------------------------------------------------------
		  function:     setSQLStatement
		  description:  set sql statement for template
      
		  return value: / 
      
		  arguments:    [in] sDSqlText, sql statement for template
      
      -------------------------------------------------------------------------*/
	 void setSQLStatement(const Tools_DynamicUTF8String &sDSqlText){m_sDSqlText = sDSqlText;}

	 /*!-------------------------------------------------------------------------
		  function:     setStoredObjectPath
		  description:  set path of sql statement for template

		  arguments:    [in] sSOPath, path of sql statement for template
      
		  return value: / 
      
      -------------------------------------------------------------------------*/
	 void setStoredObjectPath(const Tools_DynamicUTF8String &sSOPath){m_sSOPath = sSOPath;}


	 /*!-------------------------------------------------------------------------
		  function:     setStoredObjectAutoCommit
		  description:  set the autocommit mode of the dsql window

		  arguments:    [in] nAutoCommitOn, autocommit mode of the stored statement 
      
		  return value: /       
		  
      
      -------------------------------------------------------------------------*/
	 void setStoredObjectAutoCommit(SQLSMALLINT nAutoCommitOn){m_nAutoCommitOn = nAutoCommitOn;}


	 /*!-------------------------------------------------------------------------
		function:     setSQLMode
		description:  set sql mode of dsql window
		arguments:	  [in] emSQLMode, sql mode of dsql window
					  
					  
		return value: /
     --------------------------------------------------------------------------*/
	 void setSQLMode(const tpr00_SQLModeEnum eSQLMode){m_eSQLMode = eSQLMode;}


	/*!-------------------------------------------------------------------------
    function:     setIsoLevel
    description:  set isolation level of dsql window
	arguments:	  [in] emIsoLevel,isolation level type of dsql window
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setIsoLevel(const SQLSMALLINT nIsoLevel){m_nIsoLevel = nIsoLevel;}		


	/*!-------------------------------------------------------------------------
    function:     setObjectType
    description:  set object type of sql window
	arguments:	  [in] emObjectType, object type of sql window
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setObjectType(const Studio_StoredFileContent::Studio_StoredObjectType emObjectType){m_emObjectType = emObjectType;}

	/*!-------------------------------------------------------------------------
    function:     enableSave
    description:  enable the save button of dsql window
	arguments:	  [in] bEnableSave, flag to enable the save button of dsql window
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void enableSave(SAPDB_Bool bEnableSave){m_bEnableSave = bEnableSave;}

	


  private:
    virtual SAPDB_Int2            askForWriteCount(const Tools_DynamicUTF8String &szName);
    virtual Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String &szName);
                                              
                                                  
    SAPDB_Bool m_bNextDisabled;
    SAPDB_Bool m_bPrevDisabled;
	SAPDB_Bool m_bStoreItemVisible;

    Tools_DynamicUTF8String  m_sDSqlText;
	Tools_DynamicUTF8String  m_sSOPath;

	SQLSMALLINT m_nAutoCommitOn;
	tpr00_SQLModeEnum	m_eSQLMode;
	SQLSMALLINT			m_nIsoLevel;

	//is button save or save as
	SAPDB_Bool			m_bEnableSave;

	Studio_StoredFileContent::Studio_StoredObjectType m_emObjectType;

	Tools_DynamicUTF8String m_sBuffer;
	

}; 



#endif 