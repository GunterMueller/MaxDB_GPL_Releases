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

  module: StudioWeb_StoredQueryTree.cpp

  -----------------------------------------------------------------------

  special area: WebQuery

  mail to:  sapdb-general@sap.com

  description:  class for stored webqueries from sqlstudio

  see also:        /

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG


************************************************************************/

//html code fragments
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_HtmlFragments.h"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_StoredQueryTree.hpp"
//error messages
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Error.h"

#include "SAPDBCommon/SAPDB_string.h"




#define SQL1         "SELECT TABLENAME FROM DOMAIN.TABLES WHERE OWNER ="
#define SQL2         " AND TABLENAME ='"
#define SYSDBA         "SYSDBA"
#define SIMPLEQUOTE     "'"
#define TMPNAME "B1k7f4I0ki3bf10Ud4Ry0j8uj2hD84yrJd7b"

#define EXPORTFILE_BUFLEN 81920
#define MAX_CONTENT_DISPOSITION_LEN 8192

StudioWeb_StoredQueryTree::StudioWeb_StoredQueryTree(Studio_DBFSObject            *poDBFS)
:m_poDBFS(poDBFS)
{    
    m_sRefreshPath.Erase(); 
}

StudioWeb_StoredQueryTree::~StudioWeb_StoredQueryTree()
{
    
}

SAPDB_Bool StudioWeb_StoredQueryTree::doService (sapdbwa_WebAgent     & wa,
                                                 sapdbwa_HttpRequest  & req,
                                                 sapdbwa_HttpReply    & rep)
{
    SAPDB_Bool retval;
    // in these two cases we initate a download and don't do the 
    // usual preparations for HTTP and HTML headers , etc. :
    if(isExportFolderRequest(req)) {
        retval = exportFolder(wa,req,rep);
    }
    else if(isExportFileRequest(req)) {
        retval = exportFile(wa,req,rep);
    }
    else {
    
        //init refresh path
        m_sRefreshPath.Erase();

        sendPageBegin(rep,WQ_MAINWINDOW_TAG);

        if(m_poDBFS == NULL){

            sendErrorMessageBox(wa,rep,(const char*)WQ_ERR026);
            sendPageEnd (rep);    
            return SAPDB_FALSE;
        }

        if(m_poDBFS->getConnection() == NULL){

            sendErrorMessageBox(wa,rep,(const char*)WQ_ERR009);
            sendPageEnd (rep);    
            return SAPDB_FALSE;
        }

        if(isCreateFolderRequest(req)){

            if(createFolder(wa,req,rep) == SAPDB_FALSE) return SAPDB_FALSE;
        }
        else if(isDeleteFolderRequest (req)){

            if(deleteFolder(wa,req,rep) == SAPDB_FALSE) return SAPDB_FALSE;        
        }
        else if(isDeleteFileRequest (req)){

            if(deleteFile(wa,req,rep) == SAPDB_FALSE) return SAPDB_FALSE;        
        }
        else if(isRenameItemRequest (req)){

            if(renameItem(wa,req,rep) == SAPDB_FALSE) return SAPDB_FALSE;        
        }
        else if(isMoveToFolderRequest (req)){

            if(moveToFolder(wa,req,rep) == SAPDB_FALSE) return SAPDB_FALSE;        
        }
        else if(isImportFolderRequest (req)){

            if(importFolder(wa,req,rep) == SAPDB_FALSE) return SAPDB_FALSE;        
        }
        else if(isImportFileRequest (req)){

            if(importFile(wa,req,rep) == SAPDB_FALSE) return SAPDB_FALSE;        
        }
        else{
            sapdbwa_StringSeq paramNames;
            if (req.GetParameterNames(paramNames)) {

                if(paramNames[0] != NULL){
                    //request for storing a dsql object
                    if (strcmp((char *)paramNames[0],WQ_PATHTOSTORE) == 0){

                        if(saveFile(wa,req,rep) == SAPDB_FALSE) return SAPDB_FALSE;                            
                    }
                }
            }
        }
        
        Tools_DynamicUTF8String sTree;
        SAPDB_Bool bTreeExists = buildTree(sTree);        

        if (bTreeExists) {
            
            Tools_DynamicUTF8String sUser = m_poDBFS->getConnection()->getDBUser();
            if(!sUser.Empty()){
                Tools_DynamicUTF8String tmp = sUser.SubStrElem(0,1);
                if(!tmp.Empty()){
                    if(tmp == (SAPDB_Char*)"\""){
                        sUser = sUser.SubStrElem(1,sUser.Size()-2);
                    }
                }
            }
            Tools_DynamicUTF8String sName = sUser;
            sName += m_poDBFS->getConnection()->getDBName();

            rep.SendBody(WQ_JS_BEGIN);        
            //rep.SendBody(WQ_JS_NAVONE);
            rep.SendBody(WQ_JS_CLEAR_TREE);            
            rep.SendBody(WQ_JS_ADDNAV);
            //name
            rep.SendBody(sName.StrPtr());
            rep.SendBody("\", \"");

            //rootLabel
            sName = sUser;
            sName += WQ_WSTEXT_ON;
            sName += m_poDBFS->getConnection()->getDBName();
            rep.SendBody(sName.StrPtr());        

            //frameName, picturesURL, backgroundPicturesURL, withPlus, withToolTip, cookieDays        
            rep.SendBody("\", \"tree\", \"/WARoot/Images/\", \"\", true, true, 1));\n");            
            rep.SendBody(WQ_MARKSELECTEDITEM);        
            rep.SendBody(WQ_HREFASFUNCTION);        
        }        

        rep.SendBody(sTree.StrPtr(),sTree.BasisSize());

        if (bTreeExists) {        

            if(m_sRefreshPath.Size() > 0){

                rep.SendBody("parent.dsql.refreshDSQLStoredStatement('");
                escapeAsUTF8String(m_sRefreshPath);
                rep.SendBody(m_sRefreshPath.StrPtr());
                rep.SendBody("');");
            }        
            
            rep.SendBody(WQ_JS_END);
        }

        rep.SendBody(WQ_BODYOPEN2);
        rep.SendBody(WQ_BG_COLOR);
        if (bTreeExists){
            
            rep.SendBody(WQ_SHOWNAVIGATION);
        }
        
        rep.SendBody(WQ_CLOSETAG);

        sendPageEnd (rep);    

        retval = SAPDB_TRUE;
    }
    return retval;
}


SAPDB_Bool StudioWeb_StoredQueryTree::checkForQueryTree()
{

    if(m_poDBFS == NULL) return SAPDB_FALSE;    
    if(m_poDBFS->getConnection() == NULL) return SAPDB_FALSE;    

    //check if inode table is installed

    SAPDB_Bool bFrameWithTree = checkUserNotExlusive(m_poDBFS->getConnection()->getDBCHandle(),m_poDBFS->getConnection()->getDBUser());
    if (bFrameWithTree == SAPDB_TRUE) {
        bFrameWithTree = checkForSystemTable2(m_poDBFS->getConnection()->getDBCHandle(),(SAPDB_UTF8*)"SYSDBFILESYSTEM",(SAPDB_UTF8*)"MY_INODE");
        if (bFrameWithTree == SAPDB_TRUE) {
            //check if container table is installed
            bFrameWithTree = checkForSystemTable2(m_poDBFS->getConnection()->getDBCHandle(),(SAPDB_UTF8*)"SYSDBFILESYSTEM",(SAPDB_UTF8*)"MY_CONTAINER");
		}else{
			//check for old system tables
	        bFrameWithTree = checkForSystemTable(m_poDBFS->getConnection()->getDBCHandle(),(SAPDB_UTF8*)SYSDBA,(SAPDB_UTF8*)"WA_MY_INODE");
			if (bFrameWithTree == SAPDB_TRUE) {
				//check if container table is installed
			    bFrameWithTree = checkForSystemTable(m_poDBFS->getConnection()->getDBCHandle(),(SAPDB_UTF8*)SYSDBA,(SAPDB_UTF8*)"WA_MY_CONTAINER");
			}
		}
    }

    return bFrameWithTree;
}

SAPDB_Bool StudioWeb_StoredQueryTree::buildTree(Tools_DynamicUTF8String &sResponse)
{
    //varibles
    Tools_DynamicUTF8String sSubTree;    
    if (readFirstLevelTree(sSubTree) == SAPDB_FALSE){

        // error occurred, sSubTree contains error message
        sResponse = sSubTree;
        return SAPDB_FALSE;
    }
    else
        sResponse += sSubTree;        
     
 
    return SAPDB_TRUE;   
}


SAPDB_Bool StudioWeb_StoredQueryTree::readFirstLevelTree(Tools_DynamicUTF8String &sResponse)
{
    
    //variables/////////////////////////////////////////////////////////////
    SAPDB_Int2 lfno      = 0;
    SAPDB_Int2 nNodeCount = 0;
    sResponse.Erase();

    Tools_DynamicUTF8String sFolder = (SAPDB_UTF8*)WQ_STOREDQUERY_FOLDER;
    lfno = m_poDBFS->openDir(sFolder,sResponse);  
    if ( lfno == -1 )  return SAPDB_FALSE;
    ////////////////////////////////////////////////////////////////////////

    //variables/////////////////////////////////////////////////////////////
    Tools_DynamicUTF8String sTreeItem;
    Tools_DynamicUTF8String sSubTree;
    //path in db file system
    Tools_DynamicUTF8String sPath;
    Tools_DynamicUTF8String sBuffer;
    Tools_DynamicUTF8String sError;

    //node path in java script
    Tools_DynamicUTF8String sJSPath = (SAPDB_UTF8*)WQ_JSROOT;        
    SAPDB_Bool bRet     = SAPDB_FALSE;
    st_db_stat  Info;    
    ////////////////////////////////////////////////////////////////////////

    bRet = m_poDBFS->getDirEntry(lfno,sBuffer);          
          
    while (bRet == SAPDB_TRUE )
    {   

        sPath = (SAPDB_UTF8*)WQ_STOREDQUERY_FOLDER;
        //if it is not root
        if (sPath != (SAPDB_UTF8*)"/")  sPath += (SAPDB_UTF8*)("/");        

        sPath += sBuffer; 

        if (m_poDBFS->status(sPath,Info,sError) == SAPDB_FALSE) break;

        //increase nodecount
        nNodeCount ++;

        //entry is a directory
        if (Info.mode == 1) {            

            // Define the items for the top-level of the tree menu.
            //parent.Navigations.items[0].addNode(new node("Test"));
            sResponse += sJSPath;
            sResponse += WQ_JS3;                                    
            sResponse += sBuffer;
            sResponse += (SAPDB_UTF8*) "\"));\n";
                        
            //reinit
            sSubTree.Erase();
            //read directory and add items to tree
            if (readSubDir(sPath,sJSPath,nNodeCount,sSubTree) == SAPDB_FALSE){

                sResponse = sSubTree;
                return SAPDB_FALSE;
            }
            else
                sResponse += sSubTree;
        }
        else{
            sTreeItem.Erase();
            makeTreeItem((SAPDB_UTF8*)WQ_STOREDQUERY_FOLDER,
                         sJSPath,
                         sBuffer, 
                         -1,
                         sTreeItem);
            sResponse += sTreeItem;                    
        }        
        bRet = m_poDBFS->getDirEntry(lfno,sBuffer);                  
    }    
      
    m_poDBFS->closeDir(lfno,sBuffer);

    return SAPDB_TRUE;       
}



SAPDB_Bool StudioWeb_StoredQueryTree::makeTreeItem(const Tools_DynamicUTF8String &sPath,
                                                   const Tools_DynamicUTF8String &sJSPath,
                                                   const Tools_DynamicUTF8String &sFileName,                  
                                                   SAPDB_Int2 nLevel,
                                                   Tools_DynamicUTF8String &sResponse)
{
     
    

    char sBuf   [16];        
    Tools_DynamicUTF8String sCopyFileName;
    Tools_DynamicUTF8String sCopyPath;

    //init
    sResponse = sJSPath;
    if (nLevel > -1){

        sResponse += WQ_JS2;     
        sprintf (sBuf,"%d",nLevel);
        sResponse +=  (SAPDB_UTF8*)sBuf;        
        sResponse += (SAPDB_UTF8*) "].";
    }
    sResponse += WQ_JS3;                
    sCopyFileName = sFileName;
    escapeAsUTF8String(sCopyFileName);
    sResponse += sCopyFileName; 
    sResponse += (SAPDB_UTF8*) "\",\"";
    sCopyPath = sPath;
    escapeAsUTF8String(sCopyPath);
    sResponse +=  sCopyPath;
    if(sPath != (SAPDB_UTF8*) "/")  sResponse += (SAPDB_UTF8*) "/";
    sCopyFileName = sFileName;        
    escapeAsUTF8String(sCopyFileName);
    sResponse +=  sCopyFileName;
    sResponse += (SAPDB_UTF8*) "\",\"";
    sResponse += WQ_DSQL_WINDOW;
    sResponse += (SAPDB_UTF8*) "\"));\r\n";
    
    return SAPDB_TRUE;
}





SAPDB_Bool StudioWeb_StoredQueryTree::readSubDir(const Tools_DynamicUTF8String &sParentPath,
                                                  Tools_DynamicUTF8String &sJSPath,
                                                  const SAPDB_Int2 nNodeCount,
                                                  Tools_DynamicUTF8String &sSubTree)
{    
    Tools_DynamicUTF8String sSubTree2;
    Tools_DynamicUTF8String sTreeItem;
    Tools_DynamicUTF8String    sPath;
    Tools_DynamicUTF8String    sBuffer;
    Tools_DynamicUTF8String sError;
    

    //make a copy for recursion
    Tools_DynamicUTF8String    sJSPath2 = sJSPath;
    
    char sBuf   [16];    
    SAPDB_Bool bRet    = SAPDB_FALSE;
    SAPDB_Int2 lfno    = 0;
    SAPDB_Int2 nNodeCount2 = 0;
    
    st_db_stat  Info;    
    
    lfno = m_poDBFS->openDir(sParentPath,sError);  
    if ( lfno == -1 )  return SAPDB_FALSE;      
    
    bRet = m_poDBFS->getDirEntry(lfno,sBuffer);    
      
    while (bRet == SAPDB_TRUE){       
        
        nNodeCount2 ++;

        sPath = sParentPath;        
        //if it is not root
        if (sPath != (SAPDB_UTF8*)"/")  sPath += (SAPDB_UTF8*)"/";        
        
        sPath += sBuffer;
        //get some properties of the dir content
        
        if (m_poDBFS->status(sPath,Info,sError) == SAPDB_FALSE) break;

        //entry is a directory
        if (Info.mode == 1) {                
            
            //e.g.: parent.Navigations.items[0].nodes[0].nodes[0].nodes[1].addNode(new parent.node(\"SP\", \"waadmin\", \"Details\"));*/
            //build the new java script node path

            //set the start node in every loop
            sJSPath2 = sJSPath;
            sJSPath2 += WQ_JS2;
            sprintf (sBuf,"%d",(nNodeCount-1));
            sJSPath2 += (SAPDB_UTF8*)sBuf;
            sJSPath2 += (SAPDB_UTF8*) "].";

            sSubTree +=    sJSPath2;
            sSubTree += WQ_JS3;                        
            sSubTree += sBuffer;
            sSubTree += (SAPDB_UTF8*) "\"));\n";

            //reinit
            sSubTree2.Erase();
            if (readSubDir(sPath,sJSPath2,nNodeCount2,sSubTree2) == SAPDB_FALSE){

                sSubTree = sSubTree2;
                return SAPDB_FALSE;
            }
            else                
                sSubTree +=sSubTree2;    //e.g. "parent.Navigations.items[0].nodes[2].nodes[0].addNode(new parent.node(\"Hallo\"));";
        }
        else{    
            sTreeItem.Erase();
            makeTreeItem(sParentPath,
                         sJSPath,
                         sBuffer, 
                         nNodeCount-1,
                         sTreeItem);
            sSubTree += sTreeItem;            
        }
        
        bRet = m_poDBFS->getDirEntry(lfno,sBuffer);
    } 
    return (m_poDBFS->closeDir(lfno,sError));    
}



SAPDB_Bool StudioWeb_StoredQueryTree::createFolder(sapdbwa_WebAgent     &wa,
                                                   sapdbwa_HttpRequest  &req,
                                                   sapdbwa_HttpReply    &rep)
{

    const char        *queryString = req.GetQueryString();

    if (queryString == NULL) return SAPDB_FALSE;    
    
    Tools_DynamicUTF8String sName;
    getParameterValue(WQ_ACTIONTOTAKE,req,sName); 

    if(sName.Empty()) return SAPDB_FALSE;

    Tools_DynamicUTF8String sError;
    if(m_poDBFS->createFolder(sName,sError) == SAPDB_FALSE){

        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;    
}



SAPDB_Bool StudioWeb_StoredQueryTree::deleteFolder(sapdbwa_WebAgent     &wa,
                                                   sapdbwa_HttpRequest  &req,
                                                   sapdbwa_HttpReply    &rep)
{

    const char        *queryString = req.GetQueryString();

    if (queryString == NULL) return SAPDB_FALSE;    
    
    Tools_DynamicUTF8String sName;
    getParameterValue(WQ_ACTIONTOTAKE,req,sName); 

    if(sName.Empty()) return SAPDB_FALSE;

    Tools_DynamicUTF8String sError;
    if(m_poDBFS->deleteFolder(sName,sError) == SAPDB_FALSE){

        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;    
}

SAPDB_Bool StudioWeb_StoredQueryTree::renameItem(sapdbwa_WebAgent     &wa,
                                                 sapdbwa_HttpRequest  &req,
                                                 sapdbwa_HttpReply    &rep)
{

    const char        *queryString = req.GetQueryString();

    if (queryString == NULL) return SAPDB_FALSE;    
    
    Tools_DynamicUTF8String sOldName;
    Tools_DynamicUTF8String sNewName;

    getParameterValue(WQ_ACTIONTOTAKE,req,sOldName); 
    if(sOldName.Empty()) return SAPDB_FALSE;

    getParameterValue(WQ_NEWITEM,req,sNewName); 
    if(sNewName.Empty()) return SAPDB_FALSE;    

    //determine file type
    st_db_stat Info;
    Tools_DynamicUTF8String sError;
    if(m_poDBFS->status(sOldName,Info,sError) == SAPDB_FALSE){
            
        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }
    
    //now change file name
    if(m_poDBFS->renameItem(sOldName,sNewName,sError) == SAPDB_FALSE){

        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }

    if(Info.mode == DB_FIL)
        //set the rename path for refreshing the sql window
        m_sRefreshPath = sNewName;

    
    return SAPDB_TRUE;    
}

SAPDB_Bool StudioWeb_StoredQueryTree::deleteFile(sapdbwa_WebAgent     &wa,
                                                 sapdbwa_HttpRequest  &req,
                                                 sapdbwa_HttpReply    &rep)
{

    const char        *queryString = req.GetQueryString();

    if (queryString == NULL) return SAPDB_FALSE;    
    
    Tools_DynamicUTF8String sName;
    getParameterValue(WQ_ACTIONTOTAKE,req,sName); 
    
    if(sName.Empty()) return SAPDB_FALSE;
    

    Tools_DynamicUTF8String sError;
    if(m_poDBFS->deleteFile(sName,sError) == SAPDB_FALSE){

        sendUtf8ErrorMessageBox(wa,rep,sError);        
        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;    
}

SAPDB_Bool StudioWeb_StoredQueryTree::moveToFolder(sapdbwa_WebAgent     &wa,
                                                 sapdbwa_HttpRequest  &req,
                                                 sapdbwa_HttpReply    &rep)
{

    const char        *queryString = req.GetQueryString();

    if (queryString == NULL) return SAPDB_FALSE;
    
    
    Tools_DynamicUTF8String sOldName;
    Tools_DynamicUTF8String sNewName;

    getParameterValue(WQ_ACTIONTOTAKE,req,sOldName); 
    if(sOldName.Empty()) return SAPDB_FALSE;

    getParameterValue(WQ_NEWITEM,req,sNewName); 
    if(sNewName.Empty()) return SAPDB_FALSE;
    
    //determine file type
    st_db_stat Info;
    Tools_DynamicUTF8String sError;
    if(m_poDBFS->status(sOldName,Info,sError) == SAPDB_FALSE){
            
        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }

    if(m_poDBFS->moveItem(sOldName,sNewName,sError) == SAPDB_FALSE){

        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }
    
    return SAPDB_TRUE;    
}



SAPDB_Bool StudioWeb_StoredQueryTree::copyToFolder(sapdbwa_WebAgent     &wa,
                                                 sapdbwa_HttpRequest  &req,
                                                 sapdbwa_HttpReply    &rep)
{

    const char        *queryString = req.GetQueryString();

    if (queryString == NULL) return SAPDB_FALSE;    
    
    Tools_DynamicUTF8String sOldName;
    Tools_DynamicUTF8String sNewName;
    
    getParameterValue(WQ_ACTIONTOTAKE,req,sOldName); 
    if(sOldName.Empty()) return SAPDB_FALSE;

    getParameterValue(WQ_NEWITEM,req,sNewName); 
    if(sNewName.Empty()) return SAPDB_FALSE;    

    Tools_DynamicUTF8String sError;
    if(m_poDBFS->copyItem(sOldName,sNewName,sError) == SAPDB_FALSE) {

        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }
    
    return SAPDB_TRUE;    
}


SAPDB_Bool StudioWeb_StoredQueryTree::saveFile(sapdbwa_WebAgent     &wa,
                                               sapdbwa_HttpRequest  &req,
                                               sapdbwa_HttpReply    &rep)
{

    const char        *queryString = req.GetQueryString();

    if (queryString == NULL) return SAPDB_FALSE;    
    
    Tools_DynamicUTF8String  sStatement;
    getParameterValue(WQ_SQLQUERYTEXT,req,sStatement);
    
    Tools_DynamicUTF8String  sCompletePath;
    getParameterValue(WQ_PATHTOSTORE,req,sCompletePath);
    if(sCompletePath.Empty()){

        sendErrorMessageBox(wa,rep,(const char*)WQ_ERR027);
        return SAPDB_FALSE;    
    }

    Tools_DynamicUTF8String  sOverWrite;
    getParameterValue(WQ_OVERWRITE,req,sOverWrite);
    if(sOverWrite.Empty()){

        sendErrorMessageBox(wa,rep,(const char*)WQ_ERR034);
        return SAPDB_FALSE;    
    }
        
    //check if file exists
    st_db_stat Info;
    SAPDB_Bool bOverWrite = SAPDB_FALSE;
    Tools_DynamicUTF8String sError;
    if(m_poDBFS->status(sCompletePath,Info,sError) == SAPDB_TRUE){
        
        if(Info.mode == DB_FIL){
            
            if(sOverWrite == WQ_NO){

                sendErrorMessageBox(wa,rep,(const char*)WQ_ERR001);
                return SAPDB_FALSE;
            }
            bOverWrite = SAPDB_TRUE;        
        }
    }

    Studio_StoredFileContent *poSFC = new Studio_StoredFileContent;
    if(poSFC == NULL){

        sendErrorMessageBox(wa,rep,(const char*)WQ_ERR026);
        return SAPDB_FALSE;
    }

    if(bOverWrite == SAPDB_TRUE){

        // first read stored sql statement to get information like 
        // isolevel,outocommit off ...
        // this information would be lost if just the sql information is stored
        Tools_DynamicUTF8String  sBuffer;
        SAPDB_Int4 lBytesRead = m_poDBFS->readFile(sCompletePath,4096,sBuffer);
        if (lBytesRead < 0){

            sendUtf8ErrorMessageBox(wa,rep,sBuffer);
            return SAPDB_FALSE;
        }

        if (poSFC->setObjectFromBuffer(sBuffer) == 0){

            sBuffer = (SAPDB_UTF8*)WQ_ERR012;
            sBuffer += sCompletePath;
            sendUtf8ErrorMessageBox(wa,rep,sBuffer);
            return SAPDB_FALSE;
        }                

        if(poSFC->getObjectType() != Studio_StoredFileContent::storedObjectTypeStatement){

            sBuffer = (SAPDB_UTF8*)WQ_ERR035;
            sendUtf8ErrorMessageBox(wa,rep,sBuffer);
            return SAPDB_FALSE;
        }
    }

    //the procedure to store the tree items is compatible to SQLStudio
    //items can be read/written in SQLStudio and webSQL
    poSFC->setIconType(Studio_StoredFileContent::iconStoredDSQL);
    poSFC->setObjectType(Studio_StoredFileContent::storedObjectTypeStatement);
    initSOFromRequest(poSFC,req);

    SAPDB_Bool bRet = storeDSQLObject(wa,rep,sCompletePath,poSFC,bOverWrite);

    //set the rename path for refreshing the sql window if a new file was stored
    if(bRet == SAPDB_TRUE && bOverWrite == SAPDB_FALSE) m_sRefreshPath = sCompletePath;

    //delete SO object
    delete poSFC;

    return bRet;
        
}




void printbuf(const char * buf, sapdbwa_ULong len) {
    sapdbwa_ULong pos;
    putchar('{'); printf("%d", len); printf("::");
    for(pos = 0; pos < len; pos++) {
        if(buf[pos] == '\r') {
            putchar('<'); putchar('C'); putchar('R'); putchar('>');
        }
        else if(buf[pos] == '\n') {
            putchar('<'); putchar('L');putchar('F'); putchar('>'); putchar('\r'); putchar('\n');
        }
        else {
            putchar(buf[pos]);
        }
    }
    putchar('}');
    fflush(stdout);
}

/*
 * Compares the character sequence in 'buf' against the sequences given in 'upper' resp. 'lower'.
 * a character at a certain position in 'buf' matches, if if matches the character in 'lower'
 * _or_ in 'upper' at that position.
 * This allows e.g. case insensitive comparisons.
 * Parameter 'upper' and 'lower' must have the same length of data but need not be 
 * null terminated.
 * Care must be taken of that parameter len does not exceed the data length of
 * 'buf', 'lower', or 'upper'.
 * Comparison is done byte-wise, encodings are not respected.
 * True is returned if a matching can be achieved, false otherwise.
 */
SAPDB_Bool compareBuf(const char        * buf,
                      const char        * lower,
                      const char        * upper,
                      SAPDB_ULong   len) {
    SAPDB_ULong idx;
    SAPDB_Bool retval = SAPDB_TRUE;

    for(idx = 0; idx < len; idx++) {
        if(buf[idx] != lower[idx] && buf[idx] != upper[idx]) {
            retval = SAPDB_FALSE;
            break;
        }
    }
    return retval;
}

/*
 * returns offset and length of the basename of a file or directory in a full path.
 * If the path ends with '/'es or '\'es , these are ignored.
 * The character sequence of the full path is given in 'buf' with offset 'filePathStart'
 * and length 'filePathLen'.
 * 
 */
void trimFileName(const char  * buf, 
                  SAPDB_ULong   filePathStart, 
                  SAPDB_ULong   filePathLen, 
                  SAPDB_ULong & fileNameStart, 
                  SAPDB_ULong & fileNameLen) {

    SAPDB_ULong idx;
    SAPDB_ULong len;

    for(len = filePathLen; len > 0; len--) {
        if(buf[filePathStart+len-1] == '/' || buf[filePathStart+len-1] == '\\') {
            len--;
        }
        else {
            break;
        }
    }
    for(idx = 0; idx < len; idx++) {
        if(buf[filePathStart+len-idx-1] == '/' || buf[filePathStart+len-idx-1] == '\\') {
            break;
        }
    }
    fileNameStart = filePathStart+len-idx;
    fileNameLen = idx;
}

/*
 * scans a part header line for 
 * "Content-Disposition: form-data; name="import" filename="<path to a file>" .
 * Returns true, or false if an error occurred.
 */
SAPDB_Bool getFileInput(const char  * buf, 
                        SAPDB_ULong   buflen, 
                        SAPDB_Bool  & found,
                        SAPDB_ULong & filePathStart,
                        SAPDB_ULong & filePathLen,
                        SAPDB_ULong & fileNameStart,
                        SAPDB_ULong & fileNameLen) {

    SAPDB_Bool    retval                  = SAPDB_TRUE;
    sapdbwa_Bool  haveContentDisposition  = SAPDB_FALSE;
    SAPDB_ULong   contentDispositionStart = 0;
    SAPDB_ULong   contentDispositionLen   = 0;
    sapdbwa_Bool  haveName                = SAPDB_FALSE;
    SAPDB_ULong   nameStart               = 0;
    SAPDB_ULong   nameLen                 = 0;
    sapdbwa_Bool  haveFilePath            = SAPDB_FALSE;


    found = SAPDB_FALSE;
                            
    haveContentDisposition = sapdbwa_HttpRequest::ExtractValue(buf, buflen, "CONTENT-DISPOSITION: ", "content-disposition: ", 
                                                               contentDispositionStart, contentDispositionLen);
    if(haveContentDisposition == SAPDB_TRUE &&
       compareBuf(buf + contentDispositionStart, "FORM-DATA", "form-data", contentDispositionLen)) {
           haveName = sapdbwa_HttpRequest::ExtractValue(buf, buflen, "NAME=", "name=", nameStart, nameLen);
        if(haveName == SAPDB_TRUE) {
            if(compareBuf(buf + nameStart, WQ_IMPORT, WQ_IMPORT, nameLen)) {
                haveFilePath = sapdbwa_HttpRequest::ExtractValue(buf, buflen, "FILENAME=", "filename=", filePathStart, filePathLen);
                if(haveFilePath == SAPDB_TRUE) {
                    trimFileName(buf, filePathStart, filePathLen, fileNameStart, fileNameLen);
                    //printbuf(buf+fileNameStart, fileNameLen);
                    found = SAPDB_TRUE;
                }
                else {
                    retval = SAPDB_FALSE;
                }
            }
        }
    }
    return retval;
}

/*
 * scans a part header line for 
 * "content-type: form-data; name="import" boundary="<mime boundary>" .
 * Returns true, or false if an error occurred.
 */
SAPDB_Bool getIsMultipart(const char  * buf, 
                          SAPDB_ULong   buflen, 
                          SAPDB_Bool  & found,
                          char        * subBoundary) {

    SAPDB_Bool    retval                  = SAPDB_TRUE;
    sapdbwa_Bool  haveContentType         = SAPDB_FALSE;
    SAPDB_ULong   contentTypeStart        = 0;
    SAPDB_ULong   contentTypeLen          = 0;
    sapdbwa_Bool  haveBoundary            = SAPDB_FALSE;
    SAPDB_ULong   boundaryStart           = 0;
    SAPDB_ULong   boundaryLen             = 0;


    found = SAPDB_FALSE;
                            
    haveContentType = sapdbwa_HttpRequest::ExtractValue(buf, buflen, "CONTENT-TYPE: ", "content-type: ", 
                                                              contentTypeStart, contentTypeLen);
    if(haveContentType == SAPDB_TRUE &&
       compareBuf(buf + contentTypeStart, "FORM-DATA", "form-data", contentTypeLen)) {
        haveBoundary = sapdbwa_HttpRequest::ExtractValue(buf, buflen, "BOUNDARY=", "boundary=", boundaryStart, boundaryLen);
        if(haveBoundary == SAPDB_TRUE) {
            //printbuf(buf+boundaryStart, boundaryStart);
            found = SAPDB_TRUE;
        }
        else {
            retval = SAPDB_FALSE;
        }
    }
    return retval;
}

/*
 * scans a part header line for 
 * "Content-Disposition: form-data; name="<inputElementName>" .
 * Returns true, or false if an error occurred.
 */
SAPDB_Bool getInputElement(const char  * buf,
                           SAPDB_ULong   buflen,
                           const char  * inputElementName,
                           SAPDB_Bool  & found) {

    SAPDB_Bool    retval                  = SAPDB_TRUE;
    sapdbwa_Bool  haveContentDisposition  = SAPDB_FALSE;
    SAPDB_ULong   contentDispositionStart = 0;
    SAPDB_ULong   contentDispositionLen   = 0;
    sapdbwa_Bool  haveName                = SAPDB_FALSE;
    SAPDB_ULong   nameStart               = 0;
    SAPDB_ULong   nameLen                 = 0;


    found = SAPDB_FALSE;
                            
    haveContentDisposition = sapdbwa_HttpRequest::ExtractValue(buf, buflen, "CONTENT-DISPOSITION: ", "content-disposition: ", 
                                                              contentDispositionStart, contentDispositionLen);
    if(haveContentDisposition == SAPDB_TRUE &&
       compareBuf(buf + contentDispositionStart, "FORM-DATA", "form-data", contentDispositionLen)) {
           haveName = sapdbwa_HttpRequest::ExtractValue(buf, buflen, "NAME=", "name=", nameStart, nameLen);
        if(haveName == SAPDB_TRUE) {
            if(compareBuf(buf+nameStart, inputElementName, inputElementName, nameLen)) {
                //printbuf(buf+nameStart, nameLen);
                found = SAPDB_TRUE;
            }
        }
    }
    return retval;
}
/*
 * HTML file upload is generally done using the encoding type 'multipart/form-data' for all of the form data.
 * For example, if one has the following HTML form :
 *
 * <form name='frmImport' METHOD='POST' action='SOMEURL' ENCTYPE='multipart/form-data'>
 *          <input class='input_field' type='file' name='import'></input>
 *          <input type='hidden' name='actiontotake' value='/'></input>   
 *  </form>
 *
 * Then the Content-Type HTTP header is (e.g.) of the form 
 *
 * content type=multipart/form-data; boundary=---------------------------7d3541c073e
 * 
 * and the request body looks like this (where a "/r/n" sequence is denoted by <CR><LF> ) :
 * 
 * -----------------------------7d3541c073e<CR><LF>
 * Content-Disposition: form-data; name="import"; filename="D:\SAPDevelop\VTools\src\SQLStudio\Main\SQLStudio\Independent\Local Folder\cnt"<CR><LF>
 * Content-Type: application/octet-stream<CR><LF>
 * <CR><LF>
 * 13?1?select count(*) from "REPMAN"."REPMAN_SENDORDER"<CR>?????1?1?2?72200?<CR><LF>
 * -----------------------------7d3541c073e<CR><LF>
 * Content-Disposition: form-data; name="actiontotake"<CR><LF>
 * <CR><LF>
 * /<CR><LF>
 * -----------------------------7d3541c073e--<CR><LF>
 * 
 * In RFC speak this is a 'multipart entity consisting of two parts'
 * (according to the input elements in the HTML form).
 * The method 'storeMimeParts' does not rely on the web browser preserving the 
 * order of the HTML input elements in the multipart entity.
 */
SAPDB_Bool StudioWeb_StoredQueryTree::storeMimeParts(sapdbwa_HttpRequest & req,
                                                     Tools_DynamicUTF8String & sError,
                                                     const char          * boundary) {

    const SAPDB_ULong    BUFLEN              = 8192;
    const SAPDB_ULong    HDRBUFLEN           = 8192;

    char                 buf[BUFLEN];
    SAPDB_ULong          read_len            = 0;

    char                 hdr_buf[HDRBUFLEN];
    SAPDB_ULong          hdr_read_len        = 0;

    SAPDB_Bool           retval                 = SAPDB_TRUE;
    sapdbwa_Bool         Ok                     = SAPDB_FALSE;
    sapdbwa_Bool         moreParts              = SAPDB_TRUE;
    sapdbwa_Bool         moreData               = SAPDB_FALSE;
    sapdbwa_Bool         moreHeaderFields       = SAPDB_FALSE;

    SAPDB_Bool           found                = SAPDB_FALSE;
    SAPDB_Bool           fileInputFound       = SAPDB_FALSE;
    SAPDB_ULong          filePathStart        = 0;
    SAPDB_ULong          filePathLen          = 0;
    SAPDB_ULong          fileNameStart        = 0;
    SAPDB_ULong          fileNameLen          = 0;

    char                 subBoundary[100];
    SAPDB_Bool           isMultipartFound  = SAPDB_FALSE;
    SAPDB_Bool           actiontotakeFound = SAPDB_FALSE;

    SAPDB_Int2           fileDescriptor;
    SAPDB_Int4           writtenBytes;

    Tools_DynamicUTF8String   dest;
    Tools_DynamicUTF8String   destdir;
    Tools_DynamicUTF8String   destpath = "";
    Tools_DynamicUTF8String   *tmpName = new Tools_DynamicUTF8String(TMPNAME);
    SAPDB_ULong               destfileLen    = 0;
    SAPDB_ULong               destfileStart  = 0;

    Tools_DynamicUTF8String   filename;
    Tools_DynamicUTF8String   temppath;

    Tools_DynamicUTF8String   sDelError;
    struct st_db_stat         fileStatus;

    Ok = req.MimeMultipartBody_NextPart(boundary, moreParts);
    while(Ok == SAPDB_TRUE && moreParts == SAPDB_TRUE) {
        do {
            Ok = req.MimeMultipartBody_PartHeaderField(hdr_buf, 
                                                       HDRBUFLEN, 
                                                       hdr_read_len, 
                                                       moreData, 
                                                       moreHeaderFields);
            if(Ok != SAPDB_TRUE) break;

            Ok = getFileInput(hdr_buf, 
                              hdr_read_len, 
                              found,
                              filePathStart,
                              filePathLen,
                              fileNameStart,
                              fileNameLen);
            if(Ok != SAPDB_TRUE) break;
            if(found == SAPDB_TRUE) {
                fileInputFound = SAPDB_TRUE;
                filename.AssignRaw((Tools_DynamicUTF8String::ConstPointer)hdr_buf+fileNameStart, fileNameLen);
            }

            Ok = getIsMultipart(hdr_buf, 
                                hdr_read_len, 
                                found,
                                subBoundary);
            if(Ok != SAPDB_TRUE) break;
            isMultipartFound = (found == SAPDB_TRUE || isMultipartFound == SAPDB_TRUE) ? SAPDB_TRUE : SAPDB_FALSE;

            Ok = getInputElement(hdr_buf, 
                                 hdr_read_len,
                                 WQ_ACTIONTOTAKE,
                                 found);
            if(Ok != SAPDB_TRUE) break;
            actiontotakeFound = (found == SAPDB_TRUE || actiontotakeFound == SAPDB_TRUE) ? SAPDB_TRUE : SAPDB_FALSE;


        } while(moreHeaderFields == SAPDB_TRUE && Ok == SAPDB_TRUE);

        if(Ok != SAPDB_TRUE) break;

        if(fileInputFound  == SAPDB_TRUE) {
            /* store the file(s) at a temporary location until we know the dbfs path */
            m_poDBFS->deleteFile(*tmpName, sDelError);
            fileDescriptor = m_poDBFS->createFile(*tmpName, sError);
            if(fileDescriptor == -1)  {
                Ok = SAPDB_FALSE;
                break;
            }
            do {
                Ok = req.MimeMultipartBody_PartBody(boundary, 
                                                    buf, 
                                                    BUFLEN, 
                                                    read_len, 
                                                    moreData, 
                                                    moreParts);
                //printbuf(buf, read_len);
                if(Ok == SAPDB_TRUE) {
                    writtenBytes = m_poDBFS->writeFile(fileDescriptor,
						                               buf,
						                               read_len,
						                               sError);
                    if(writtenBytes < 0) Ok = SAPDB_FALSE;
                }
            } while(moreData == SAPDB_TRUE && Ok == SAPDB_TRUE);
            if(Ok != SAPDB_TRUE) break;
            m_poDBFS->closeFile(fileDescriptor, sError);
            m_poDBFS->status(*tmpName, fileStatus, sDelError);
            if(fileStatus.size == 0) {
                sError = "Import File is Empty.";
                m_poDBFS->deleteFile(*tmpName, sDelError);
                Ok = SAPDB_FALSE;
                break;
            }
        }
        else if(actiontotakeFound  == SAPDB_TRUE) {
            /* the path to store the uploaded file in the dbfs */
            Ok = req.MimeMultipartBody_PartBody(boundary, 
                                                buf, 
                                                BUFLEN, 
                                                read_len, 
                                                moreData, 
                                                moreParts);
            /* we don't support paths longer than BUFLEN (minus a few byte)
               (8k should be enough) : */
            if(moreData == SAPDB_TRUE) Ok = SAPDB_FALSE;
            if(Ok != SAPDB_TRUE) break;
            dest.AssignRaw((Tools_DynamicUTF8String::ConstPointer)buf, read_len);
            m_poDBFS->status(dest, fileStatus, sDelError);
            if(fileStatus.mode == DB_DIR) {
                destdir = dest;
            }
            else {
                trimFileName(buf, 0, read_len, destfileStart, destfileLen);
                destdir.AssignRaw((Tools_DynamicUTF8String::ConstPointer)buf, read_len-destfileLen);
            }
            if(destdir.FindLastOf("/") != destdir.Size()-1) {
                destdir += "/";
            }
        }
        else {
            Ok = req.MimeMultipartBody_NextPart(boundary, moreParts);
            if(Ok != SAPDB_TRUE) break;
        }
        fileInputFound     = SAPDB_FALSE;
        actiontotakeFound  = SAPDB_FALSE;
    } /* while(Ok == SAPDB_TRUE && moreParts == SAPDB_TRUE) { ... */
    if(Ok == SAPDB_TRUE && dest.Size() != 0 && filename.Size() != 0) {
            Ok = m_poDBFS->moveItem(*tmpName,
						            destdir,
						            sError);
            if(Ok != SAPDB_TRUE) {
                m_poDBFS->deleteFile(*tmpName, sDelError);
            }
            else {
                temppath = destdir;
                destpath = destdir;
                temppath += *tmpName;
                destpath += filename;
                Ok = m_poDBFS->renameItem(temppath,
						                  destpath,
						                  sError);
                if(Ok != SAPDB_TRUE) {
                    m_poDBFS->deleteFile(temppath, sDelError);
                }
            }
    }
    else {
        Ok = SAPDB_FALSE;
    }
    if(Ok != SAPDB_TRUE) {
        retval = SAPDB_FALSE;
        if(sError.Size() == 0) {
            sError = "General Error in importing Object.";
        }
    }
    return retval;
}

SAPDB_Bool printBody(sapdbwa_HttpRequest & req) {

    const SAPDB_ULong    BUFLEN              = 81920;

    char                 buf[BUFLEN];
    SAPDB_ULong          read_len            = 0;

    SAPDB_Bool           retval                 = SAPDB_TRUE;
    sapdbwa_Bool         Ok                     = SAPDB_FALSE;

    read_len = req.ReadBody(buf, BUFLEN);
    printbuf(buf, read_len);
    return retval;
}

/**/
SAPDB_Bool StudioWeb_StoredQueryTree::importItem(sapdbwa_WebAgent     &wa,
                                                 sapdbwa_HttpRequest  &req,
                                                 sapdbwa_HttpReply    &rep) {
    SAPDB_Bool                bRet = SAPDB_TRUE;
    sapdbwa_Bool              haveBoundary = SAPDB_FALSE;
    SAPDB_ULong               boundaryStart = 0;
    SAPDB_ULong               boundaryLen = 0;
    const char              * contentType = NULL;
    char                      boundary[100];
    Tools_DynamicUTF8String   sError;


    contentType = req.GetContentType();
    haveBoundary = sapdbwa_HttpRequest::ExtractValue(contentType, 
                                                     strlen(contentType), 
                                                     "BOUNDARY=", 
                                                     "boundary=", 
                                                     boundaryStart, 
                                                     boundaryLen);
    if(haveBoundary == SAPDB_TRUE && boundaryLen < 100) {
        SAPDB_memmove(boundary, contentType + boundaryStart, boundaryLen);
        boundary[boundaryLen] = 0;
        bRet = storeMimeParts(req, sError, boundary);
        if(bRet == SAPDB_FALSE) {
            sendUtf8ErrorMessageBox(wa,rep,sError);
        }
    }
    return bRet;
}

/*/
SAPDB_Bool StudioWeb_StoredQueryTree::importItem(sapdbwa_WebAgent &wa, sapdbwa_HttpRequest &req, sapdbwa_HttpReply &rep) {
    const char              * contentType = NULL;

    contentType = req.GetContentType();
    printf("content type=%s\r\n",contentType);
    printf("#############################################\r\n");

    printBody(req);

    printf("#############################################\r\n");

    return SAPDB_TRUE;
}

/**/
/**/
SAPDB_Bool StudioWeb_StoredQueryTree::exportItem(sapdbwa_WebAgent     &wa,
                                                 sapdbwa_HttpRequest  &req,
                                                 sapdbwa_HttpReply    &rep) {
    SAPDB_Bool               retval = SAPDB_FALSE;
    Tools_DynamicUTF8String  filePath;
    char                     saneFilePath[MAX_CONTENT_DISPOSITION_LEN/4];
    SAPDB_ULong              saneFilePathLen;
    SAPDB_ULong              fileNameStart;
    SAPDB_ULong              fileNameLen;
    Tools_DynamicUTF8String  error;
    SAPDB_Int2               fileDescriptor;

    char                 buf[EXPORTFILE_BUFLEN];
    SAPDB_ULong          readBytes;
    struct st_db_stat    fileStatus;
    char                 contentLength[70];
    char                 content_disposition[MAX_CONTENT_DISPOSITION_LEN];

    SAPDB_UInt4          writtenBytes;
    
    getParameterValue(WQ_ACTIONTOTAKE, req, filePath); 
    if(filePath.Empty() == SAPDB_FALSE && filePath.Size() < MAX_CONTENT_DISPOSITION_LEN/4-20) {
        m_poDBFS->status(filePath, fileStatus, error);
        if(fileStatus.mode == DB_FIL) {
            fileDescriptor = m_poDBFS->openFile(filePath, error, access_read);
            if(fileDescriptor >= 0)  {
                sprintf(contentLength, "%d", fileStatus.size);
                // internet explorer (at least <= 6.0) has a bug which prevents the
                // 'filename' parameter in the 'content-disposition' and the mime type
                // given by 'content-type' from being used in the 'save as' dialog
                // as preselections. Instead ie displays the last path component of 
                // the request URI as filename suggestion and preselects the '.html' suffix
                // in the dialog.
                // As a workaround the request url is modified to contain the filename
                // as trailing part of the URI.
                // This is done using browser-side scripting.
                SAPDB_strcpy(saneFilePath, filePath.CharPtr());
                saneFilePathLen = strlen(saneFilePath);
                trimFileName(saneFilePath, 0, saneFilePathLen, fileNameStart, fileNameLen);
                // remove trailing separator (cf description of 'trimFileName') :
                if(fileNameStart + fileNameLen < saneFilePathLen) {
                    saneFilePath[fileNameStart + fileNameLen] = 0;
                }
                sprintf(content_disposition, "attachment; filename=%s", saneFilePath+fileNameStart);
                rep.InitHeader(sapdbwa_Status_Ok,
                                NULL,
                                NULL,
                                NULL, // const char *lastModified
                                NULL, // const char *expires
                                NULL ); // const char *location
                rep.SetHeader ("Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
                rep.SetHeader ("Content-Length", contentLength);
                rep.SetHeader ("Content-Type", "application/octet-stream");
                rep.SetHeader ("Content-Disposition", content_disposition);
                rep.SetHeader ("Content-Transfer-Encoding", "binary");
                rep.SendHeader();
                do {
                    retval = m_poDBFS->readFileBytes(fileDescriptor, buf, EXPORTFILE_BUFLEN, readBytes, error);
                    //printbuf(buf, read_len);
                        if(retval == SAPDB_TRUE && readBytes > 0) {
                            writtenBytes = rep.SendBody(buf, readBytes);
                        }
                } while(readBytes > 0 && retval == SAPDB_TRUE);
            }
        }
    }
    if(retval != SAPDB_TRUE) {
        retval = SAPDB_FALSE;
        if(error.Size() == 0) {
            error = "General Error in exporting Object.";
        }
    }
    return retval;
}

/*
SAPDB_Bool StudioWeb_StoredQueryTree::exportItem(sapdbwa_WebAgent &wa, sapdbwa_HttpRequest &req, sapdbwa_HttpReply &rep) {
    const char              * contentType = NULL;
    const char              * queryString = NULL;

    contentType = req.GetContentType();
    printf("content type=%s\r\n",contentType);
    queryString = req.GetQueryString();
    printf("query string=%s\r\n",queryString);
    printf("#############################################\r\n");

    printBody(req);

    printf("#############################################\r\n");

    return SAPDB_TRUE;
}

*/

SAPDB_Bool StudioWeb_StoredQueryTree::importFolder(sapdbwa_WebAgent     &wa,
                                                   sapdbwa_HttpRequest  &req,
                                                   sapdbwa_HttpReply    &rep) {
    SAPDB_Bool bRet = importItem(wa, req, rep);
    return bRet;
}

SAPDB_Bool StudioWeb_StoredQueryTree::importFile(sapdbwa_WebAgent     &wa,
                                                 sapdbwa_HttpRequest  &req,
                                                 sapdbwa_HttpReply    &rep) {
    SAPDB_Bool bRet = importItem(wa, req, rep);
    return bRet;
}

SAPDB_Bool StudioWeb_StoredQueryTree::exportFolder(sapdbwa_WebAgent     &wa,
                                                   sapdbwa_HttpRequest  &req,
                                                   sapdbwa_HttpReply    &rep) {
    SAPDB_Bool bRet = SAPDB_FALSE;
    //bRet = exportItem(wa, req, rep);

    sendErrorMessageBox(wa,rep,(const char*)"Export of Folders Is Not Implemented.");

    return bRet;
}

SAPDB_Bool StudioWeb_StoredQueryTree::exportFile(sapdbwa_WebAgent     &wa,
                                                 sapdbwa_HttpRequest  &req,
                                                 sapdbwa_HttpReply    &rep) {
    SAPDB_Bool bRet = exportItem(wa, req, rep);
    return bRet;
}


        


SAPDB_Bool StudioWeb_StoredQueryTree::storeDSQLObject(sapdbwa_WebAgent     &wa,
                                                      sapdbwa_HttpReply    &rep,
                                                      const Tools_DynamicUTF8String  &sCompletePath,
                                                      Studio_StoredFileContent *poSFC,
                                                      SAPDB_Bool bOverWrite)
{

    //check if file exists
    Tools_DynamicUTF8String sError;

    SAPDB_Int2 lfno = -1;
    if(bOverWrite){

        lfno = m_poDBFS->openFile(sCompletePath,sError);
    }
    else{
        lfno = m_poDBFS->createFile(sCompletePath,sError);
    }
    if(lfno == -1){

        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }
    
    Tools_DynamicUTF8String  sBuffer;    
    poSFC->getObjectToBuffer(sBuffer);
    
    SAPDB_Int4 byteswritten = m_poDBFS->writeFile(lfno,
                                                  (void*)sBuffer.StrPtr(),
                                                  sBuffer.BasisSize(),
                                                  sError);

    if(byteswritten < 0){

        m_poDBFS->closeFile(lfno,sError);
        sendUtf8ErrorMessageBox(wa,rep,sError);        
        return SAPDB_FALSE;

    }

    if(m_poDBFS->closeFile(lfno,sError) == SAPDB_FALSE){

        sendUtf8ErrorMessageBox(wa,rep,sError);
        return SAPDB_FALSE;
    }
    
    return SAPDB_TRUE;
}

