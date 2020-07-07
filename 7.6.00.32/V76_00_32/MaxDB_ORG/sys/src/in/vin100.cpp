/*! 
  -----------------------------------------------------------------------------
 
  module: vin100.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: log class
 
  description:  implementation of a log class

  version:      min 7.2.

  Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




  -----------------------------------------------------------------------------
*/
 

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "hin100.h"
#include "gcn00.h"

#include "DBM/Srv/Message/DBMSrvMsg_Info.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/* 
  -----------------------------------------------------------------------------
  implementation class tin100_Log
  -----------------------------------------------------------------------------
*/

/* 
  -----------------------------------------------------------------------------
  public constructor tin100_Log :: tin100_Log
  -----------------------------------------------------------------------------
*/
tin100_Log :: tin100_Log (SAPDBErr_MessageList * pEventList)
            : m_pFileAnchor(NULL)
{

} // end tin100_Log :: tin100_Log

/* 
  -----------------------------------------------------------------------------
  public destructor tin100_Log :: ~tin100_Log
  -----------------------------------------------------------------------------
*/
tin100_Log :: ~tin100_Log ()
{

  tin105_LogFile * pCurrent = m_pFileAnchor;
  tin105_LogFile * pDelete  = NULL;

  // destroy logfiles
  while (pCurrent != NULL) {

    pDelete  = pCurrent;
    pCurrent = pCurrent->next();

    if (pDelete->isToDestroy()) {
      delete pDelete;
    } // end if
  } // end while

} // end tin100_Log :: tin100_Log

/* 
  -----------------------------------------------------------------------------
  public function tin100_Log :: writeEntry
  -----------------------------------------------------------------------------
*/
bool tin100_Log :: writeEntry (const SAPDBErr_MessageList & EventList)
{
  tin105_LogFile * pCurrent = m_pFileAnchor;
  bool success(true);

  while (pCurrent != NULL) {
    
    if( !pCurrent->writeEvents (EventList) )
        success = false;
    
    pCurrent = pCurrent->next();
  } // end while
 
  return success;
} // tin100_Log :: writeEntry
/* 
  -----------------------------------------------------------------------------
*/
bool tin100_Log :: writeEntry 
      ( const char             * szFormat,
        const char             * p0,
        const char             * p1,
        const char             * p2,
        const char             * p3,
        const char             * p4,
        const char             * p5,
        const char             * p6,
        const char             * p7,
        const char             * p8,
        const char             * p9)
{
  DBMSrvMsg_Info aEvent( szFormat, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
  return writeEntry(aEvent);
} // tin100_Log :: writeEntry

/* 
  -----------------------------------------------------------------------------
  public function tin100_Log :: addFile
  -----------------------------------------------------------------------------
*/
void tin100_Log :: addFile
      ( tin105_LogFile * pLogFile,
        const bool       bSelfDestroy)
{
  
  tin105_LogFile * pCurrent = m_pFileAnchor;
  
  if (pLogFile != NULL) {
    pLogFile->setToDestroy(bSelfDestroy);
  } // end if

  if (pCurrent == NULL) {
    m_pFileAnchor = pLogFile;
  } else {
    while (pCurrent->next() != NULL) {
      pCurrent = pCurrent->next();
    } // end while
    pCurrent->setNext(pLogFile);
  } // end if

} // end tin100_Log :: addFile
 
/* 
  -----------------------------------------------------------------------------
  implementation class tin100_GlobalLog
  -----------------------------------------------------------------------------
*/

tin100_GlobalLog* tin100_GlobalLog :: m_pMySelf = NULL;

/* 
  -----------------------------------------------------------------------------
  public function tin100_GlobalLog :: createObject
  -----------------------------------------------------------------------------
*/
tin100_GlobalLog * tin100_GlobalLog :: createObject(SAPDBErr_MessageList * pEvtList)
{
  FUNCTION_DBG_MCN00_1("tin100_GlobalLog :: createObject");

  if (m_pMySelf == NULL) {
    m_pMySelf = new tin100_GlobalLog(pEvtList);
  } // end if
  if (m_pMySelf == NULL && pEvtList != NULL) {
    *pEvtList = DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM);
  } // end if

   return m_pMySelf;
} // end tin100_GlobalLog :: createObject

/* 
  -----------------------------------------------------------------------------
  public function tin100_GlobalLog :: deleteObject
  -----------------------------------------------------------------------------
*/
void tin100_GlobalLog :: deleteObject()
{

  if (m_pMySelf != NULL) {
    delete m_pMySelf;
    m_pMySelf = NULL;
  } // end if

} // end tin100_GlobalLog :: deleteObject

/* 
  -----------------------------------------------------------------------------
  public function tin100_GlobalLog :: getReference
  -----------------------------------------------------------------------------
*/
tin100_GlobalLog * tin100_GlobalLog :: getReference(const bool bCreateIt)
{

  if ((m_pMySelf == NULL) && bCreateIt) {
    createObject();
  } // end if

   return m_pMySelf;
} // end tin100_GlobalLog :: createObject


/* 
  -----------------------------------------------------------------------------
  public function tin100_GlobalLog :: writeEntry
  -----------------------------------------------------------------------------
*/
bool tin100_GlobalLog :: writeEntry (const SAPDBErr_MessageList & EventList)
{
  tin100_GlobalLog * pLog = tin100_GlobalLog :: getReference();
  
  bool success(false);

  if (pLog != NULL) {
    
    success = pLog->tin100_Log::writeEntry(EventList);

  } // end if
 
  return success;
} // tin100_GlobalLog :: writeEntry

/* 
  -----------------------------------------------------------------------------
  protected constructor tin100_GlobalLog :: tin100_GlobalLog
  -----------------------------------------------------------------------------
*/
tin100_GlobalLog :: tin100_GlobalLog
      ( SAPDBErr_MessageList * pEvtList )
      : tin100_Log (pEvtList)
{

} // end tin100_GlobalLog :: tin100_GlobalLog

