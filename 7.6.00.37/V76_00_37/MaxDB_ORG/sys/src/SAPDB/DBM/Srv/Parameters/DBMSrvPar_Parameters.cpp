/*!
  @file           DBMSrvPar_Parameters.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer database parameters handling - Implementation

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp"
#include "DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"

#include "hcn20.h"
#include "hcn29.h"

/*
  -----------------------------------------------------------------------------
  values
  -----------------------------------------------------------------------------
 */
const DBMSrvPar_Parameters::size_type DBMSrvPar_Parameters::m_HashBase = 1001;

/*-----------------------------------------------------------------------------*/
/*! @brief public constructor */
DBMSrvPar_Parameters::DBMSrvPar_Parameters
    ( const char * szDBNameIN )
    : pFirst(NULL),           
      bParamFileExist(false),
      bKernelDirty(false),   
      bDevspaceDirty(false),
      szDBName(_Tools_UTF8Ptr(szDBNameIN))
{
  for (int i=0;i< m_HashBase + 2;++i) (*this)[i].pPointer = NULL;
} // end DBMSrvPar_Parameters::DBMSrvPar_Parameters

/*-----------------------------------------------------------------------------*/
/*! @brief public destructor */
DBMSrvPar_Parameters::~DBMSrvPar_Parameters
    ( )
{
  int i = 0;
  for (i = 0; i < (*this).size();++i) {
    if ((*this)[i].pPointer != NULL) {
      delete (*this)[i].pPointer;
      (*this)[i].pPointer = NULL;
    } // end if
  } // end for
} // end DBMSrvPar_Parameters::~DBMSrvPar_Parameters

/*-----------------------------------------------------------------------------*/
/*! @brief public member function DBMSrvPar_Parameters::Put */
void DBMSrvPar_Parameters::Put
    ( DBMSrvPar_Parameter * pParameter )
{
  // compute hash value
  int nHash = GetHash(pParameter->szParameterName);

  while(((*this)[nHash].pPointer != NULL) && (strcmp(pParameter->szParameterName, (*this)[nHash].pPointer->szParameterName ) != 0) ) {
    ++nHash;
  } // end while
 
  if (((*this)[nHash].pPointer != NULL) && ((*this)[nHash].pPointer != pParameter)) {
    delete (*this)[nHash].pPointer;
  } // end if

  (*this)[nHash].pPointer = pParameter;

} // end DBMSrvPar_Parameters::Put

/*-----------------------------------------------------------------------------*/
/*! @brief public member function DBMSrvPar_Parameters::Get */
DBMSrvPar_Parameter * DBMSrvPar_Parameters::Get
    ( const char * szParameter ) const
{
  int nHash = GetHash(szParameter);

  while( (nHash < (*this).size()) && ((*this)[nHash].pPointer != NULL) ) {
    if (strcmp(szParameter, (*this)[nHash].pPointer->szParameterName ) == 0) {
      return (*this)[nHash].pPointer;
    } // end if
    ++nHash;
  } // end while

  return NULL;
} // end DBMSrvPar_Parameters::Get

/*-----------------------------------------------------------------------------*/
/*! @brief public member function DBMSrvPar_Parameters::Delete */
void DBMSrvPar_Parameters::Delete
    ( const DBMSrvPar_Parameter * pParameter )
{
  Delete(pParameter->szParameterName);
} // end DBMSrvPar_Parameters::Remove

/*-----------------------------------------------------------------------------*/
/*! @brief public member function DBMSrvPar_Parameters::Delete */
void DBMSrvPar_Parameters::Delete
    ( const char * szParameter )
{
  int nHash = GetHash(szParameter);

  while( (nHash < (*this).size()) && ((*this)[nHash].pPointer != NULL) ) {
    if (strcmp(szParameter, (*this)[nHash].pPointer->szParameterName ) == 0) {
      delete (*this)[nHash].pPointer;
      (*this)[nHash].pPointer = NULL;
      return;
    } // end if
    ++nHash;
  } // end while

} // end DBMSrvPar_Parameters::Delete

/*-----------------------------------------------------------------------------*/
/*! @brief public member function DBMSrvPar_Parameters::GetInstance */
DBMSrvPar_Parameters * DBMSrvPar_Parameters::GetInstance
     ( const char      * szDBName,
       bool              bReadOnly,
       DBMSrvMsg_Error & oError )
{
  DBMSrvInfrReply_ReplyBufferWritingIntoMsgList oMsgListReply(oError);
  DBMSrvPar_Parameters * pXParamData = NULL;

  if(oMsgListReply.isValid())
  {
    tsp00_DbNamec oDBName;
    oDBName.rawAssign(szDBName);
    cn29ParamStartSession( oDBName, 
                            pXParamData, 
                            oMsgListReply.getData(), 
                            oMsgListReply.getLength(), 
                            oMsgListReply.getMaximalLength(), 
                            bReadOnly);

    oMsgListReply.fillMessageList();
  }

  return pXParamData;
} // end  DBMSrvPar_Parameters::GetInstance

/*-----------------------------------------------------------------------------*/
/*! @brief public member SetValueWithCheck */
bool DBMSrvPar_Parameters::SetValueWithCheck
     ( const char                       * szParameter,
       const char                       * szValue,
       bool                               bIgnoreReadOnly,
       DBMSrvPar_Parameter::XPWriteMode   oWriteMode,
       DBMSrvMsg_Error                  & oError )
{
  oError.ClearMessageList();

  DBMSrvInfrReply_ReplyBufferWritingIntoMsgList oMsgListReply(oError);

  tcn00_Error rc = cn29SoftPutIntern(this, szParameter, szValue, "", bIgnoreReadOnly, oWriteMode);

  if (rc != OK_CN00) {
    oMsgListReply.getReply().startWithError(rc);
    oMsgListReply.fillMessageList();
  } // end if

  return oError.IsEmpty();
} // end DBMSrvPar_Parameters::SetValue

/*-----------------------------------------------------------------------------*/
/*! @brief public member CheckAll */
bool DBMSrvPar_Parameters::CheckAll
     ( DBMSrvMsg_Error & oError )
{
  oError.ClearMessageList();

  DBMSrvInfrReply_ReplyBufferWritingIntoMsgList oMsgListReply(oError);

  if(oMsgListReply.isValid())  {
    tsp00_DbNamec          szDbNameOut;
    szDbNameOut.rawAssign(szDBName.CharPtr());

    cn20ParamCheckAll( szDbNameOut,
                       this,
                        "",
                        oMsgListReply.getData(), 
                        oMsgListReply.getLength(), 
                        oMsgListReply.getMaximalLength());

    oMsgListReply.fillMessageList();
  } else {
    oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM);
  } // end if

  return oError.IsEmpty();
} // end DBMSrvPar_Parameters::CheckAll

/*-----------------------------------------------------------------------------*/
/*! @brief public member Commit */
bool DBMSrvPar_Parameters::Commit
     ( DBMSrvMsg_Error & oError )
{
  oError.ClearMessageList();

  DBMSrvInfrReply_ReplyBufferWritingIntoMsgList oMsgListReply(oError);

  if(oMsgListReply.isValid()) {
    tsp00_DbNamec          szDbNameOut;
    szDbNameOut.rawAssign(szDBName.CharPtr());
    cn20ParamCommitSession( szDbNameOut,
                            this,
                            oMsgListReply.getData(), 
                            oMsgListReply.getLength(), 
                            oMsgListReply.getMaximalLength());

    oMsgListReply.fillMessageList();
  } else {
    oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM);
  } // end if

  return oError.IsEmpty();
} // end DBMSrvPar_Parameters::Commit

/*-----------------------------------------------------------------------------*/
/*! @brief private member function DBMSrvPar_Parameters::GetHash */
int DBMSrvPar_Parameters::GetHash
    ( const char * szParameter ) const
{
  int nHash = 0;
  int i     = 0;
  for (i = 0; szParameter[i] != 0;++i) {
    nHash += szParameter[i];
  } // end for
  return (nHash % m_HashBase);
} // end DBMSrvPar_Parameters::GetHash

