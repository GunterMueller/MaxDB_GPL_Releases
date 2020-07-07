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

  module: Studio_DBFSObject.cpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: WebQuery

  description:  class for reading and writing to database file system

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

#include "SAPDB/SQLStudio/Studio_DBFSObject.hpp"
#include "SAPDB/SQLStudio/Studio_Errors.h"

Studio_DBFSObject::Studio_DBFSObject(Studio_Connection* poConnection,
									 SAPDB_UInt2	   nFSPrefix)
									 
:m_nFSPrefix(nFSPrefix),
m_poConnection(poConnection)
{
	m_pDBFS = NULL; 	 
}

Studio_DBFSObject::~Studio_DBFSObject()
{
	sapdbwa_ErrP pError = NULL; 

	//destroy dbfs
	if (m_pDBFS != NULL ) {

		if (wd101DestroyDBFS(m_pDBFS) == 0 ) {
     
			pError = wd101GetErr(m_pDBFS);
			if (sapdbwa_GetErrId(pError) != sapdbwa_Error_NoError) {
				//wa.WriteLogMsg(sapdbwa_GetErrMsg(pError));
			}
		}		 
		m_pDBFS = NULL;
	}
}



SAPDB_Int4 Studio_DBFSObject::readFile(const Tools_DynamicUTF8String	&sFilePath, 
									   SAPDB_UInt4				nMaxFileSize,
									   Tools_DynamicUTF8String	&sContent)
				
{
	//init output
	sContent.Erase();
	Tools_DynamicUTF8String	sCopyOfPath = sFilePath;

	//if there is no database filesystem, get one
	if (m_pDBFS == NULL ){

		if (getValidDBFS(sContent) == SAPDB_FALSE) return -1;		
	}

	twd101DBFile lFile;
	SAPDB_Int4  lByteRead = 0;

	sapdbwa_ErrP pError = NULL;
	Tools_DynamicUTF8String sError;		
		
	
	lFile = wd101Open(m_pDBFS,(const char*)sCopyOfPath.StrPtr(),access_read);
	if (lFile == -1){
		
		if (getDBFSError(sError) == SAPDB_FALSE){
			 
			sContent = sError;
			return -1;
		 }
		 else{
			//open file again
			lFile = wd101Open(m_pDBFS,(const char*)sCopyOfPath.StrPtr(),access_read );
			if (lFile == -1 ) {

				getDBFSError(sError);
				sContent = sError;
				return -1;
			}
		}	
	}	

	char *psBuffer = NULL;
	psBuffer = new char[nMaxFileSize];
	if(psBuffer == NULL) return -1;

	lByteRead = wd101Read(m_pDBFS,lFile,psBuffer,nMaxFileSize);
	if (lByteRead == -1) {
		
		getDBFSError(sError);
		sContent = sError;		
		lByteRead = -1;
	}

	if (wd101Close(m_pDBFS,lFile) == SAPDB_FALSE){

		getDBFSError(sError);
		sContent = sError;
		delete[] psBuffer;
		return -1;
	}

	if(lByteRead > 0)
		sContent = Tools_DynamicUTF8String((Tools_DynamicUTF8String::ConstPointer) psBuffer,lByteRead);
	

	delete[] psBuffer;
	
	return lByteRead;
}

SAPDB_Bool Studio_DBFSObject::readFileBytes(SAPDB_Int2                fileDescriptor, 
									        char                      buf [],
                                            const SAPDB_ULong         bufSize,
									        SAPDB_ULong             & readBytes,
                                            Tools_DynamicUTF8String & error)
				
{
    SAPDB_Bool retval = SAPDB_TRUE;
    		
	error.Erase();
	readBytes = wd101Read(m_pDBFS, fileDescriptor, buf, bufSize);
	if (readBytes == -1) {
    	retval = SAPDB_FALSE;
		getDBFSError(error);
	}

	return retval;
}

SAPDB_Bool Studio_DBFSObject::getDBFSError(Tools_DynamicUTF8String &sOut)
{		    

	if(m_poConnection == NULL){

		sOut = (SAPDB_UTF8*)ST_ERR016;        
		return SAPDB_FALSE;
	}
	//check some pointers
	if (m_poConnection->getDBCHandle() == NULL) {		

		sOut = ST_ERR001;
		return SAPDB_FALSE;
	}
	if (m_pDBFS == NULL) {

		sOut = (SAPDB_UTF8*)ST_ERR002;
		return SAPDB_FALSE;
	}

	SAPDB_UTF8 sBuffer[512];
	memset(sBuffer,0,512);

	//init SAPDB_Bool value
	sapdbwa_Bool connectionDown = SAPDB_TRUE;	
	//get error object
	sapdbwa_ErrP pError = wd101GetErr(m_pDBFS);
	if (pError == NULL) return SAPDB_FALSE;	
	
	if (sapdbwa_CheckDBC(m_poConnection->getDBCHandle(),
					sapdbwa_GetNativeOdbcErr(pError),
					sapdbwa_GetSqlState(pError),
					&connectionDown)) {

		if (connectionDown == SAPDB_FALSE) {			 

			if (sapdbwa_GetErrId(pError) != sapdbwa_Error_NoError) {

	 		   sapdbwa_CopyErrMsg(pError,(char*)sBuffer, sizeof (sBuffer));
			}			
			sOut = Tools_DynamicUTF8String((SAPDB_UTF8*)sBuffer);
			return SAPDB_FALSE;
		}
		else
			sOut = (SAPDB_UTF8*)ST_ERR014;		
	}
	else
		sOut = (SAPDB_UTF8*)ST_ERR015;					
	
	
	return SAPDB_TRUE;
}

SAPDB_Bool  Studio_DBFSObject::getValidDBFS(Tools_DynamicUTF8String &sError)
{
	
	if(m_poConnection == NULL){

		sError = (SAPDB_UTF8*)ST_ERR016;        
		return SAPDB_FALSE;
	}
	if (m_poConnection->getDBCPool() == NULL) {

		sError = (SAPDB_UTF8*)ST_ERR013;        
		return SAPDB_FALSE;
	}
#if defined(WIN32)
	if (m_poConnection->getODBCDriver().Length() < 1) {

		sError = (SAPDB_UTF8*)ST_ERR003;        
		return SAPDB_FALSE;
	}
#endif
	
	if (m_pDBFS != NULL) {

        if (wd101DestroyDBFS(m_pDBFS) == 0 ) {    
         
			getDBFSError(sError);			
			return SAPDB_FALSE;
		}
		m_pDBFS = NULL;		
    }	

	Tools_DynamicUTF8String sCopyODBCDriver =  m_poConnection->getODBCDriver();
	Tools_DynamicUTF8String sCopyDBServer = m_poConnection->getDBServer();
	Tools_DynamicUTF8String sCopyDBName = m_poConnection->getDBName();
	Tools_DynamicUTF8String sCopyUser = m_poConnection->getDBUser();
	Tools_DynamicUTF8String sCopyPWD = m_poConnection->getPassWord();

	m_pDBFS = wd101CreateDbcDBFS(m_poConnection->getDBCPool(),
				     sapdbwa_CreateDBC(),
				     "",
				    (char*)sCopyODBCDriver.StrPtr(),
				    (char*)sCopyDBServer.StrPtr(),
				    (char*)sCopyDBName.StrPtr(),
				    (char*)sCopyUser.StrPtr(),
				    (char*)sCopyPWD.StrPtr(),
				    "",
				    m_nFSPrefix);
	if (m_pDBFS == NULL) {

		sError = (SAPDB_UTF8*)ST_ERR004;        
		return SAPDB_FALSE;
	}		

	return SAPDB_TRUE;

}


SAPDB_Int2  Studio_DBFSObject::openDir(const Tools_DynamicUTF8String &sPath,Tools_DynamicUTF8String &sError)
{

	//if there is no database filesystem, get one
	if (m_pDBFS == NULL ){

		if (getValidDBFS(sError) == SAPDB_FALSE) return -1;		
	}
	
	Tools_DynamicUTF8String		sCopyOfPath = sPath;
	
	SAPDB_Int2 lfno = wd101OpenDir(m_pDBFS,(const char*)sCopyOfPath.StrPtr(),type_asc_name_asc);  
	if ( lfno == -1 ) {    
         
		 if (getDBFSError(sError) == SAPDB_FALSE)			 
			return -1;		 
		 else{
			 //open directory again, for reconnect
			 lfno = wd101OpenDir(m_pDBFS,(const char*)sCopyOfPath.StrPtr(), type_asc_name_asc);  
			 if ( lfno == -1 ) {

				getDBFSError(sError);				
				return -1;
			}
		}
    }

	return lfno;
}

SAPDB_Bool  Studio_DBFSObject::status(Tools_DynamicUTF8String &sPath,st_db_stat &Info,Tools_DynamicUTF8String &sError)
{

	
	Tools_DynamicUTF8String strCopyOfPath = sPath;	
	sError.Erase();

	if ( wd101Stat(m_pDBFS,(const char*)strCopyOfPath.StrPtr(), &Info) == 0 ) {
	    
        getDBFSError(sError);
		return SAPDB_FALSE;			
    }

	return SAPDB_TRUE;
}

SAPDB_Bool  Studio_DBFSObject::closeDir(SAPDB_Int2 lfno,Tools_DynamicUTF8String &sError)
{

	sError.Erase();

	if (wd101CloseDir(m_pDBFS, lfno) == 0 ) {    
         
		 getDBFSError(sError);		
		 return SAPDB_FALSE;
    }        	
	return SAPDB_TRUE;
}

SAPDB_Bool  Studio_DBFSObject::getDirEntry(SAPDB_Int2 lfno,Tools_DynamicUTF8String &sOut)
{

	SAPDB_UTF8 sBuffer[1024];
	memset(sBuffer,0,1024);

	SAPDB_Bool bRet = SAPDB_TRUE;
	sOut.Erase();

    //get directory entries
	SAPDB_Int4 lRet = wd101GetDirEntry(m_pDBFS, lfno,(char*)sBuffer,sizeof(sBuffer), 0);
    if (lRet == -1) {    
         
		 sapdbwa_ErrP pError = wd101GetErr(m_pDBFS);
		 if (sapdbwa_GetErrId(pError) != sapdbwa_Error_NoError &&
			(sapdbwa_GetErrId(pError) != sapdbwa_Error_NoMoreEntries)){

			sapdbwa_CopyErrMsg(pError,(char*)sBuffer,sizeof (sBuffer));						
		 }		 
		 bRet =  SAPDB_FALSE;
	}

	sOut = Tools_DynamicUTF8String((SAPDB_UTF8*)sBuffer);	
	return bRet;
}

SAPDB_Bool Studio_DBFSObject::createFolder(const Tools_DynamicUTF8String &sCompletePath,
										   Tools_DynamicUTF8String &sError)
{
	Tools_DynamicUTF8String strCopyOfPath = sCompletePath;	
	if (wd101MkDir(m_pDBFS,(const char*)strCopyOfPath.StrPtr()) == sapdbwa_False){
    
        getDBFSError(sError);		
		return SAPDB_FALSE;
    }

	return SAPDB_TRUE;
}


SAPDB_Bool Studio_DBFSObject::deleteFolder(const Tools_DynamicUTF8String &sCompletePath,
										   Tools_DynamicUTF8String &sError)
{
	sError.Erase();

	Tools_DynamicUTF8String strCopyOfPath = sCompletePath;	
	if (wd101RmDir(m_pDBFS,(const char*)strCopyOfPath.StrPtr(),sapdbwa_True) == sapdbwa_False){
    
        getDBFSError(sError);		
		return SAPDB_FALSE;
    }

	return SAPDB_TRUE;
}

SAPDB_Bool Studio_DBFSObject::deleteFile(const Tools_DynamicUTF8String &sCompletePath,
										 Tools_DynamicUTF8String &sError)
{

	sError.Erase();

	Tools_DynamicUTF8String strCopyOfPath = sCompletePath;	
	if (wd101Delete(m_pDBFS,(const char*)strCopyOfPath.StrPtr()) == sapdbwa_False){
    
        getDBFSError(sError);		
		return SAPDB_FALSE;
    }

	return SAPDB_TRUE;
}

SAPDB_Bool Studio_DBFSObject::renameItem(const Tools_DynamicUTF8String &sCompleteOldPath,
										 const Tools_DynamicUTF8String &sCompleteNewPath,
										 Tools_DynamicUTF8String &sError)
{
	sError.Erase();

	Tools_DynamicUTF8String strCopyOfNewPath = sCompleteNewPath;
	Tools_DynamicUTF8String strCopyOfOldPath = sCompleteOldPath;
	
	if (wd101Rename(m_pDBFS,(const char*)strCopyOfOldPath.StrPtr(),(const char*)strCopyOfNewPath.StrPtr()) == sapdbwa_False){
    
        getDBFSError(sError);		
		return SAPDB_FALSE;
    }

	return SAPDB_TRUE;
}


SAPDB_Bool Studio_DBFSObject::moveItem(const Tools_DynamicUTF8String &sCompleteOldPath,
									   const Tools_DynamicUTF8String &sCompleteNewPath,
									   Tools_DynamicUTF8String &sError)
{

	sError.Erase();

	Tools_DynamicUTF8String strCopyOfNewPath = sCompleteNewPath;	
	Tools_DynamicUTF8String strCopyOfOldPath = sCompleteOldPath;
	
	// first check if folder exists, because move create a item if it does noit exits
	// here a error code is wanted

	SAPDB_Int2 lfno = openDir(sCompleteNewPath,sError);  
    if ( lfno == -1 )  return SAPDB_FALSE;

	if(closeDir(lfno,sError) == SAPDB_FALSE) return SAPDB_FALSE;
	
	if (wd101Mv(m_pDBFS,(const char*)strCopyOfOldPath.StrPtr(),(const char*)strCopyOfNewPath.StrPtr()) == sapdbwa_False){
    
        getDBFSError(sError);		
		return SAPDB_FALSE;
    }

	return SAPDB_TRUE;
}


SAPDB_Bool Studio_DBFSObject::copyItem(const Tools_DynamicUTF8String &sCompleteOldPath,
										 const Tools_DynamicUTF8String &sCompleteNewPath,
										 Tools_DynamicUTF8String &sError)
{

	
	Tools_DynamicUTF8String strCopyOfNewPath = sCompleteNewPath;
	Tools_DynamicUTF8String strCopyOfOldPath = sCompleteOldPath;
	
	if (wd101Copy(m_pDBFS,(const char*)strCopyOfOldPath.StrPtr(),(const char*)strCopyOfNewPath.StrPtr()) == sapdbwa_False){
    
        getDBFSError(sError);		
		return SAPDB_FALSE;
    }

	return SAPDB_TRUE;
}

SAPDB_Int2 Studio_DBFSObject::createFile(const Tools_DynamicUTF8String &sCompletePath,
										 Tools_DynamicUTF8String &sError)
{
	sError.Erase();
	
	Tools_DynamicUTF8String strCopyOfPath = sCompletePath;	
	SAPDB_Int2 lfno = wd101Create(m_pDBFS,(const char*)strCopyOfPath.StrPtr(),access_write);
	     
	if(lfno == -1) getDBFSError(sError);							

	return lfno;
}


SAPDB_Int2 Studio_DBFSObject::openFile(const Tools_DynamicUTF8String &sCompletePath,
									   Tools_DynamicUTF8String &sError)
{
	return openFile(sCompletePath, sError, access_write);
}

SAPDB_Int2 Studio_DBFSObject::openFile(const Tools_DynamicUTF8String &sCompletePath,
									   Tools_DynamicUTF8String &sError,
                                       twd101OpenFlag accessMode)
{
    SAPDB_Int2 lfno = -1;
    Tools_DynamicUTF8String strCopyOfPath = sCompletePath;
	sError.Erase();
	
	//if there is no database filesystem, get one
	if ( m_pDBFS != NULL || (m_pDBFS == NULL && getValidDBFS(sError) != SAPDB_FALSE) ) {
	    lfno = wd101Open(m_pDBFS,(const char*)strCopyOfPath.StrPtr(),accessMode);
    	     
	    if(lfno == -1) getDBFSError(sError);
    }

	return lfno;
}


SAPDB_Int4 Studio_DBFSObject::writeFile(SAPDB_Int2   lfno,
										void         *buffer,
										SAPDB_Int4   nBufferSize,
										Tools_DynamicUTF8String &sError)
{

	sError.Erase();

	SAPDB_Int4 bytewritten = wd101Write(m_pDBFS,lfno,buffer,nBufferSize);	     
	if(bytewritten == -1) getDBFSError(sError);							

	return bytewritten;
}

SAPDB_Bool Studio_DBFSObject::closeFile(SAPDB_Int2 lfno,Tools_DynamicUTF8String &sError)
{	
	
	sError.Erase();

	if (wd101Close(m_pDBFS,lfno) == sapdbwa_False){
    
        getDBFSError(sError);		
		return SAPDB_FALSE;
    }

	return SAPDB_TRUE;
}