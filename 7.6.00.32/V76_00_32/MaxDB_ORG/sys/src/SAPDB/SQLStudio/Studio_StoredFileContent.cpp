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

  module: Studio_StoredFileContent.cpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: Studio

  description:  class for storing ssqlstudio/websql objects in database file system

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

#include <stdlib.h>
#include "SAPDB/SQLStudio/Studio_StoredFileContent.hpp"
#include "SAPDB/ToolsCommon/Tools_UTF8Basis.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_Types.h"


#define QPLEXTENSION "qpl"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Studio_StoredFileContent::Studio_StoredFileContent()
{

	//application version and sql studio object versions can have different values

	//version of stored object
	m_nVersion = 72200;
	//SQLStudio/webSQL version which created so object
	m_nAppVersion = 72200;

	m_emObjectType = storedObjectTypeUnknown;

	m_nAutoCommit = SQL_AUTOCOMMIT_ON;
	m_eSQLMode = cpr_kind_internal;
	m_nIsoLevel = SQL_TXN_READ_COMMITTED;

	m_emIconType = iconUnknown;

	

}

Studio_StoredFileContent::~Studio_StoredFileContent()
{

}

SAPDB_Char Studio_StoredFileContent::getTokenSeperator(Studio_TokenSeperator Sep) 
{    
        
    switch (Sep){

        case saveObjectLevel1:
            return (SAPDB_Char)30;
                
        case saveObjectLevel2:
            return (SAPDB_Char)31;
            
        case objectLevel1:
            return (SAPDB_Char)0;
            
        case objectLevel2:
            return (SAPDB_Char)1;
            
        case objectLevel3:
            return (SAPDB_Char)2;
            
        case objectLevel4:
            return (SAPDB_Char)3;
            
        case objectLevel5:
            return (SAPDB_Char)4;
            
		case objectLevel6:
            return (SAPDB_Char)5;
            
		case objectLevel7:
            return (SAPDB_Char)6;
            
        default:
            return (SAPDB_Char)0;
            
    } 
}

SAPDB_Bool Studio_StoredFileContent::getNextToken(Tools_DynamicUTF8String &strTokenList,
		 									 Tools_DynamicUTF8String &strNextToken,
											 SAPDB_Char cTokenSeparator)
{     
	
	strNextToken.Erase();
    
    if (strTokenList.Size() < 1){

        strNextToken.Erase();
        return SAPDB_FALSE;	
	}

	Tools_DynamicUTF8String::BasisElementIndex Pos = 0;
	Tools_DynamicUTF8String strTokenSeparator;

	if (strTokenSeparator.ConvertFromASCII_Latin1(
		(Tools_DynamicUTF8String::ASCIIConstPointer)&cTokenSeparator,
		(Tools_DynamicUTF8String::ASCIIConstPointer)&cTokenSeparator+sizeof (SAPDB_Char)) 
			!= Tools_UTF8Basis::Success) return SAPDB_FALSE;
	
	Pos = strTokenList.Find(strTokenSeparator);
	if (Pos != Tools_DynamicUTF8String::NPos) {

		//strNextToken = Tools_DynamicUTF8String(strTokenList,Pos);
		strNextToken = Tools_DynamicUTF8String(strTokenList.Begin(),strTokenList.GetIteratorAtBasis(Pos));
		strTokenList = strTokenList.SubStr(Pos+1);
	}
	else{
		strNextToken = strTokenList;
		strTokenList.Erase();
	}
     
	return SAPDB_TRUE;
}


SAPDB_Bool Studio_StoredFileContent::setObjectFromBuffer(const Tools_DynamicUTF8String &sData) 
{
	//make a copy of the data to work with
    Tools_DynamicUTF8String sBuffer = sData;

    Tools_DynamicUTF8String sNextToken;
    
	SAPDB_Char* szpConvert = NULL;
	SAPDB_Char  szConvertBuffer [16];

	getNextToken (sBuffer, sNextToken, getTokenSeperator(saveObjectLevel1));
	if (sNextToken.Size() < 1)   return SAPDB_FALSE;
	memset(szConvertBuffer,0,sizeof (szConvertBuffer));	
	sNextToken.ConvertToASCII_Latin1(szConvertBuffer,szConvertBuffer+sizeof (szConvertBuffer),szpConvert);
    m_emIconType = (Studio_IconType)atoi ((char*)szConvertBuffer);

	getNextToken (sBuffer, sNextToken, getTokenSeperator(saveObjectLevel1));
	if (sNextToken.Size() < 1)   return SAPDB_FALSE;
	memset(szConvertBuffer,0,sizeof (szConvertBuffer));
	sNextToken.ConvertToASCII_Latin1(szConvertBuffer,szConvertBuffer+sizeof (szConvertBuffer),szpConvert);
    m_emObjectType = (Studio_StoredObjectType)atoi ((char*)szConvertBuffer);       
       
	if (getNextToken (sBuffer,m_sSQLData, getTokenSeperator(saveObjectLevel1)) == SAPDB_FALSE) return SAPDB_FALSE;    
    
	if (getNextToken (sBuffer,m_sTableData, getTokenSeperator(saveObjectLevel1)) == SAPDB_FALSE) return SAPDB_FALSE;    

	if (getNextToken (sBuffer,m_sJoinData, getTokenSeperator(saveObjectLevel1)) == SAPDB_FALSE) return SAPDB_FALSE;    
    
	if (getNextToken (sBuffer,m_sViewData, getTokenSeperator(saveObjectLevel1)) == SAPDB_FALSE) return SAPDB_FALSE;    

	if (getNextToken (sBuffer,m_sLinkData, getTokenSeperator(saveObjectLevel1)) == SAPDB_FALSE) return SAPDB_FALSE;


	getNextToken (sBuffer, sNextToken, getTokenSeperator(saveObjectLevel1));
	if (sNextToken.Size() < 1)   return SAPDB_FALSE;
	memset(szConvertBuffer,0,sizeof (szConvertBuffer));
	sNextToken.ConvertToASCII_Latin1(szConvertBuffer,szConvertBuffer+sizeof (szConvertBuffer),szpConvert);
    m_nAutoCommit = atoi ((char*)szConvertBuffer);

	getNextToken (sBuffer, sNextToken, getTokenSeperator(saveObjectLevel1));
	if (sNextToken.Size() < 1)   return SAPDB_FALSE;
	memset(szConvertBuffer,0,sizeof (szConvertBuffer));
	sNextToken.ConvertToASCII_Latin1(szConvertBuffer,szConvertBuffer+sizeof (szConvertBuffer),szpConvert);
    m_eSQLMode = (tpr00_SQLModeEnum)atoi ((char*)szConvertBuffer);

	getNextToken (sBuffer, sNextToken, getTokenSeperator(saveObjectLevel1));
	if (sNextToken.Size() < 1)   return SAPDB_FALSE;
	memset(szConvertBuffer,0,sizeof (szConvertBuffer));
	sNextToken.ConvertToASCII_Latin1(szConvertBuffer,szConvertBuffer+sizeof (szConvertBuffer),szpConvert);
    m_nIsoLevel = atoi ((char*)szConvertBuffer);

	getNextToken (sBuffer, sNextToken, getTokenSeperator(saveObjectLevel1));
	if (sNextToken.Size() < 1)   return SAPDB_FALSE;
	memset(szConvertBuffer,0,sizeof (szConvertBuffer));
	sNextToken.ConvertToASCII_Latin1(szConvertBuffer,szConvertBuffer+sizeof (szConvertBuffer),szpConvert);
    m_nVersion = atoi ((char*)szConvertBuffer);
            
    if (m_nVersion > m_nAppVersion)  return SAPDB_FALSE;
    

	return SAPDB_TRUE;                 
}



void Studio_StoredFileContent::getObjectToBuffer(Tools_DynamicUTF8String &sBuffer)
{    
    

	char   sBuf[16];
	memset(sBuf,0,16);       
	sprintf (sBuf,"%d",m_emIconType);
	sBuffer = sBuf;
	sBuffer += getTokenSeperator(saveObjectLevel1) ;


	memset(sBuf,0,16);       
	sprintf (sBuf,"%d",m_emObjectType);
	sBuffer += sBuf;
	sBuffer += getTokenSeperator(saveObjectLevel1) ;


	sBuffer += m_sSQLData;	
	sBuffer += getTokenSeperator(saveObjectLevel1);
	sBuffer +=  m_sTableData; 
	sBuffer +=  getTokenSeperator(saveObjectLevel1);
	sBuffer +=  m_sJoinData;
    sBuffer +=  getTokenSeperator(saveObjectLevel1);
	sBuffer +=  m_sViewData;
	sBuffer +=  getTokenSeperator(saveObjectLevel1);
	sBuffer +=  m_sLinkData;                
	sBuffer +=  getTokenSeperator(saveObjectLevel1);

	memset(sBuf,0,16);       
	sprintf (sBuf,"%d",m_nAutoCommit);
	sBuffer += sBuf;
	sBuffer += getTokenSeperator(saveObjectLevel1) ;


	memset(sBuf,0,16);       
	sprintf (sBuf,"%d",m_eSQLMode);
	sBuffer += sBuf;
	sBuffer += getTokenSeperator(saveObjectLevel1) ;


	memset(sBuf,0,16);       
	sprintf (sBuf,"%d",m_nIsoLevel);
	sBuffer += sBuf;
	sBuffer += getTokenSeperator(saveObjectLevel1) ;

	memset(sBuf,0,16);       
	sprintf (sBuf,"%d",m_nVersion);
	sBuffer += sBuf;
	sBuffer += getTokenSeperator(saveObjectLevel1) ;         

}