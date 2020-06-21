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
#ifndef STUDIO_STOREDFILECONTENT_HPP
#define STUDIO_STOREDFILECONTENT_HPP

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
*/

/*!**********************************************************************

  module: Studio_StoredFileContent.hpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: Studio

  description:  class for storing ssqlstudio/websql objects in database file system

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2004 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/


#include "gpr00.h"

//must be included before vpa00..
#include <WINDOWS.H>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "vpa00sqlod32.h"

#include "ToolsCommon/Tools_DynamicUTF8String.hpp"




class Studio_StoredFileContent  
{
public:
	Studio_StoredFileContent();
	virtual ~Studio_StoredFileContent();

	enum Studio_TokenSeperator
	{
		saveObjectLevel1 = 1,
		saveObjectLevel2 = 2 ,
		objectLevel1 = 3,
		objectLevel2 = 4,
		objectLevel3 = 5,
		objectLevel4 = 6,
		objectLevel5 = 7,
		objectLevel6 = 8,
		objectLevel7 = 9
	};

	
	//this definition is compatiblew with SQLStudio, see  SAPDBGC.GlobalCom
	enum  Studio_IconType
	{
		iconUnknown = 0,
		iconTable = 1,
		iconView = 2,
		iconResult = 3,
		iconSystem = 4,
		iconSnapshot = 5,
		iconSynonym = 6,
		iconUser = 7,
		iconUserGroup = 8,
		iconTableOwner = 9,
		iconWebFolder = 11,
		iconWebItem = 12,
		iconStoredDSQL = 13,
		iconStoredVIQ = 14,
		iconStoredQBE = 15,
		iconIndex = 16,
		iconSequence = 17,
		iconSOFolder = 18,
		iconStoredDraft = 19,
		iconLinkDSQL = 20,
		iconLinkVIQ = 21,
		iconLinkQBE = 22,
		iconLinkDraft = 23
	};



	enum Studio_StoredObjectType{

		storedObjectTypeUnknown = 0,
		storedObjectTypeStatement = 1,
		storedObjectTypeVisualQuery = 2,
		//don't change this value, because it was stored in Objects
		storedObjectTypeQueryByExample = 5 ,
		storedObjectTypeDraftTable = 6,
		storedObjectTypeDraftView = 7,
		storedObjectTypeDraftIndex = 8
	};

	/*!-------------------------------------------------------------------------
    function:     setObjectFromBuffer
    description:  set the description of a sql studio object
	arguments:	  [in] sData, data to fill sql studio object				  
				  
				  
    return value: SAPDB_TRUE, if success
    --------------------------------------------------------------------------*/
	SAPDB_Bool setObjectFromBuffer(const Tools_DynamicUTF8String &sData);

	/*!-------------------------------------------------------------------------
    function:     getSQLData
    description:  return sql data
	arguments:	  /
				  
				  
    return value: sql data as const reference
    --------------------------------------------------------------------------*/
	const Tools_DynamicUTF8String &getSQLData() const {return m_sSQLData;} 

	/*!-------------------------------------------------------------------------
    function:     getObjectToBuffer
    description:  stream a stored object to a buffer
	arguments:	  [in] sBuffer, buffer to fill
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void getObjectToBuffer(Tools_DynamicUTF8String &sBuffer);
	
	/*!-------------------------------------------------------------------------
    function:     setSQLData
    description:  set sql data of stored object
	arguments:	  [in] sSQLData,sql data of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setSQLData(const Tools_DynamicUTF8String &sSQLData){m_sSQLData = sSQLData;}

	/*!-------------------------------------------------------------------------
    function:     setTableData
    description:  set table data of stored object
	arguments:	  [in] sTableData,table data of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setTableData(const Tools_DynamicUTF8String &sTableData){m_sTableData = sTableData;}

	/*!-------------------------------------------------------------------------
    function:     setJoinData
    description:  set join data of stored object
	arguments:	  [in] sJoinData,join data of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setJoinData(const Tools_DynamicUTF8String &sJoinData){m_sJoinData = sJoinData;}

	/*!-------------------------------------------------------------------------
    function:     setViewData
    description:  set view data of stored object
	arguments:	  [in] sViewData,view data of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setViewData(const Tools_DynamicUTF8String &sViewData){m_sViewData = sViewData;}


	/*!-------------------------------------------------------------------------
    function:     setLinkData
    description:  set link data of stored object
	arguments:	  [in] sLinkData,link data of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setLinkData(const Tools_DynamicUTF8String &sLinkData){m_sLinkData = sLinkData;}


	/*!-------------------------------------------------------------------------
    function:     setVersion
    description:  set version of stored object
	arguments:	  [in] nVersion,version of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setVersion(const SAPDB_UInt nVersion){m_nVersion = nVersion;}


	/*!-------------------------------------------------------------------------
    function:     setAppVersion
    description:  set application version of stored object
	arguments:	  [in] nAppVersion,application version of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setAppVersion(const SAPDB_UInt nAppVersion){m_nAppVersion = nAppVersion;}


	/*!-------------------------------------------------------------------------
    function:     setObjectType
    description:  set object type of stored object
	arguments:	  [in] emObjectType, object type of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setObjectType(const Studio_StoredObjectType emObjectType){m_emObjectType = emObjectType;}

	/*!-------------------------------------------------------------------------
    function:     getObjectType
    description:  get object type of stored object
	arguments:	  /
				  
				  
    return value: m_emObjectType, object type of stored object
    --------------------------------------------------------------------------*/
	Studio_StoredObjectType getObjectType(){return m_emObjectType;}


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


	/*!-------------------------------------------------------------------------
    function:     getTokenSeperator
    description:  return token seperator for a sql studio object	token level
	arguments:	  [in] Sep, SQL Studio TokenSeperator
				  
				  
    return value: SAPDB_Char, a SAPDB_Char between hex value 0 -6 or 1E- 1F
    --------------------------------------------------------------------------*/
	SAPDB_Char getTokenSeperator(Studio_TokenSeperator Sep);

	/*!-------------------------------------------------------------------------
    function:     setIconType
    description:  set icon type of stored object
	arguments:	  [in] nIconType, set icon type of stored object
				  
				  
    return value: /
    --------------------------------------------------------------------------*/
	void setIconType(const Studio_IconType emIconType){m_emIconType = emIconType;}
				

private:

	//private functions

	/*!-------------------------------------------------------------------------
    function:     getNextToken
    description:  return toke seperator for a sql studio object	token level
	arguments:	  [in/out] strTokenList, token stream, will be reduced with next token
				  [out] strNextToken, found token 
				  [in] cTokenSeparator, token seperator
				  
				  
    return value: SAPDB_TRUE, if success
    --------------------------------------------------------------------------*/
	SAPDB_Bool getNextToken(Tools_DynamicUTF8String &strTokenList,
							Tools_DynamicUTF8String &strNextToken,
							SAPDB_Char cTokenSeparator);

	//private varibles
	Tools_DynamicUTF8String m_sSQLData;       
	Tools_DynamicUTF8String m_sTableData;     
	Tools_DynamicUTF8String m_sJoinData;      
	Tools_DynamicUTF8String m_sViewData;      
	Tools_DynamicUTF8String m_sLinkData;      
	SAPDB_UInt m_nVersion;
	SAPDB_UInt m_nAppVersion;
	
	Studio_StoredObjectType m_emObjectType;

	SQLSMALLINT			m_nAutoCommit;
	tpr00_SQLModeEnum	m_eSQLMode;
	SQLSMALLINT			m_nIsoLevel;
	Studio_IconType		m_emIconType;	

};


#endif