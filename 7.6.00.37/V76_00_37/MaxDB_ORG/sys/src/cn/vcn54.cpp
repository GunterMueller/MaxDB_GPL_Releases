/*! 
  -----------------------------------------------------------------------------
 
  module: vcn54.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBM Server
 
  description:  implementation of the TGZ Class

  version:      min 7.2.
 
  Copyright (c) 2001-2005 SAP AG
 
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "heo06.h"
#include "gcn00.h"
#include "hcn54.h"
#include "extern/zlib/zlib.h"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
#define TRANS_BLOCK_SIZE   10485760 // 10 MB
#define BLOCK_SIZE         512
#define TGZ_EXTENSION      ".tgz"
#define GZ_EXTENSION       ".gz"

/*
  -------------------------------------------------------------------------
  implemenation class cn54DBMTgz
  -------------------------------------------------------------------------
*/

/*
  -------------------------------------------------------------------------
  public constructor cn54DBMTgz :: cn54DBMTgz
  -------------------------------------------------------------------------
*/
cn54DBMTgz :: cn54DBMTgz 
      ( const tsp00_Pathc &  szArchive ) 
{
  FUNCTION_DBG_MCN00_1("cn54DBMTgz :: cn54DBMTgz");

  m_szTgz = szArchive;
  if (m_szTgz.length() > (int) strlen(TGZ_EXTENSION)) {
    if (strcmp(&m_szTgz[m_szTgz.length() - strlen(TGZ_EXTENSION)], TGZ_EXTENSION) == 0) {
      m_szTgz[m_szTgz.length() - strlen(TGZ_EXTENSION)] = 0;
    } // end if
  } // end if
  strcat(m_szTgz, TGZ_EXTENSION);

  m_oEvent.ClearMessageList();

  // open tgz
  gzFile              hTgz;

  hTgz = gzopen(m_szTgz, "wb");
  if (hTgz != NULL) {
    gzclose(hTgz);
  } // end if
  if (hTgz == NULL) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN,
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME,m_szTgz)));
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
  } else {
    m_oBuffer = new unsigned char [TRANS_BLOCK_SIZE];
    if (m_oBuffer == NULL) {
      m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
    } // end if
  } // end if

} // end cn54DBMTgz :: cn54DBMTgz

/*
  -------------------------------------------------------------------------
  public function cn54DBMTgz :: ~cn54DBMTgz
  -------------------------------------------------------------------------
*/
cn54DBMTgz :: ~cn54DBMTgz()
{
  if (m_oBuffer != NULL) {
    delete [] m_oBuffer;
  } // end if
} // end cn54DBMTgz :: ~cn54DBMTgz

/*
  -------------------------------------------------------------------------
  public function cn54DBMTgz :: append
  -------------------------------------------------------------------------
*/
bool cn54DBMTgz :: append 
      ( const tsp00_Pathc &  szFile )
{
  FUNCTION_DBG_MCN00_1("cn54DBMTgz :: append");

  tsp00_Pathc szTarget;
  
  szTarget.Init();

  return append(szFile, szTarget);
} // end cn54DBMTgz :: append 
/*
  -------------------------------------------------------------------------
*/
bool cn54DBMTgz :: append 
      ( const tsp00_Pathc &  szFile,
        const tsp00_Pathc &  szTarget )
{
  FUNCTION_DBG_MCN00_1("cn54DBMTgz :: append");

  tar_hdr_t           oHeader;
  tsp05_RteFileInfo   rteFileInfo;
  tsp05_RteFileError  rteFileErr;
  struct tm           oModified;
  int                 nDate;
  int                 nTime;
  tsp00_Datec         szDate;
  tsp00_Timec         szTime;
  tsp00_Int4          hFile;
  gzFile              hTgz;
  unsigned char    *  oBuffer = m_oBuffer;
  tsp00_Longint       nOutLen   = 0;
  int                 nChecksum = 0;
  int                 i         = 0;

  m_oEvent.ClearMessageList();

  // get info about file
  sqlfinfoc (szFile.asCharp(), &rteFileInfo, &rteFileErr );
  if (rteFileErr.sp5fe_result != vf_ok) {
    m_oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    return m_oEvent.IsEmpty();
  } // end if

  if (rteFileInfo.sp5fi_exists) {
    // compute time
    szDate.p2c(rteFileInfo.sp5fi_date_modified);
    szTime.p2c(rteFileInfo.sp5fi_time_modified);
    nDate = atoi(szDate.asCharp());
    nTime = atoi(szTime.asCharp());
    oModified.tm_year   = (nDate / 10000) - 1900;
    oModified.tm_mon    = ((nDate % 10000) / 100) - 1;
    oModified.tm_mday   = nDate % 100;
    oModified.tm_hour   = nTime / 10000;
    oModified.tm_min    = (nTime % 10000) / 100;
    oModified.tm_sec    = nTime % 100;

    // fill header
    memset(&oHeader, 0, sizeof(oHeader));
    if (szTarget.length() > 0) { 
      strncpy(oHeader.name, szTarget.asCharp(), szTarget.length());   // name of file
    } else {
      strncpy(oHeader.name, szFile.asCharp(), szFile.length());       // name of file
    } // end if
    strncpy(oHeader.mode, "644", 4);                                  // acess mode of file
    //oHeader.uid
    //oHeader.gid
    sprintf(oHeader.size, "%lo", (long) rteFileInfo.sp5fi_size);      // real size
    sprintf(oHeader.mtime, "%lo", (long) mktime( &oModified ));       // modified time
    //oHeader.chksum
    strncpy(oHeader.typeflag, "0", 1);                                // filetype - 0 for regular file
    //oHeader.linkname
    //oHeader.magic
    //oHeader.version
    //oHeader.uname
    //oHeader.gname
    //oHeader.devmajor
    //oHeader.devminor
    //oHeader.prefix
  } // end if

  // compute checksum
  for (i = 0; i < BLOCK_SIZE; ++ i) {
   nChecksum = nChecksum + oHeader.name[i];
  } // end for
  sprintf(oHeader.chksum, "%o", nChecksum + (8 * 32));                // save checksum of header

  // open file
  sqlfopenc (szFile.asCharp(), sp5vf_binary, sp5vf_read, sp5bk_buffered, &hFile, &rteFileErr );
  if (rteFileErr.sp5fe_result != vf_ok) {
    m_oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    return m_oEvent.IsEmpty();
  } // end if

  // open archive
  hTgz = gzopen(m_szTgz, "ab");
  if (hTgz == NULL) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN,
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME,m_szTgz)));
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
    return m_oEvent.IsEmpty();
  } // end if

  // move header to archive
  if (gzwrite(hTgz, &oHeader, BLOCK_SIZE) != BLOCK_SIZE) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEWRITE,
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FILEWRITE__FILENAME,m_szTgz)));
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
    gzclose(hTgz);
    return m_oEvent.IsEmpty();
  } // end if

  // move file content to archive
  sqlfreadc(hFile, &oBuffer[0], TRANS_BLOCK_SIZE, &nOutLen, &rteFileErr);
  while (rteFileErr.sp5fe_result == vf_ok) {
    rteFileErr.sp5fe_text[rteFileErr.sp5fe_text.length() - 1] = 0;
    if (nOutLen%BLOCK_SIZE == 0) {
      if (gzwrite(hTgz, &oBuffer[0], (unsigned int) nOutLen) != nOutLen) {
        rteFileErr.sp5fe_result.becomes(vf_notok);
        SAPDB_strcpy(rteFileErr.sp5fe_text, "write tgz error");
      } // end if
    } else {
      if (gzwrite(hTgz, &oBuffer[0], (unsigned int)  (BLOCK_SIZE * (nOutLen/BLOCK_SIZE + 1))) != (BLOCK_SIZE * (nOutLen/BLOCK_SIZE + 1))) {
        rteFileErr.sp5fe_result.becomes(vf_notok);
        SAPDB_strcpy(rteFileErr.sp5fe_text, "write tgz error");
      } // end if
    } // end if
    if (rteFileErr.sp5fe_result == vf_ok) {
      sqlfreadc(hFile, &oBuffer[0], TRANS_BLOCK_SIZE, &nOutLen, &rteFileErr);
    } // end if
  } // end while
  if ((rteFileErr.sp5fe_result != vf_ok) && (rteFileErr.sp5fe_result != vf_eof)) {
    m_oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
    gzclose(hTgz);
    return m_oEvent.IsEmpty();
  } // end if

  // close archive
  if (gzclose(hTgz) != Z_OK) {
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECLOSE,
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FILECLOSE__FILENAME,m_szTgz)));
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
    return m_oEvent.IsEmpty();
  } // end if

  // close file
  sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
  if (rteFileErr.sp5fe_result != vf_ok) {
    m_oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
    m_oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    return m_oEvent.IsEmpty();
  } // end if

  return m_oEvent.IsEmpty();
} // end cn54DBMTgz :: append 

/*
  -------------------------------------------------------------------------
  public function cn54DBMTgz :: compressDirect
  -------------------------------------------------------------------------
*/
bool cn54DBMTgz :: compressDirect 
      (const tsp00_Pathc &  szTarget,
       const tsp00_Pathc &  szSource,
       Msg_List  &  oEvent,
       bool                 bRemoveSource)
{
  FUNCTION_DBG_MCN00_1("cn54DBMTgz :: compressDirect");

  tsp00_Int4          hSource;
  tsp00_Pathc         szGz;
  gzFile              hTgz;
  tsp05_RteFileError  rteFileErr;
  tsp05_RteFileInfo   rteFileInfo;
  tsp00_Longint       nOutLen   = 0;
  char              * pBuffer = NULL;
  
  oEvent.ClearMessageList();

  // handle filename
  szGz = szTarget;
  if (szGz.length() > (int) strlen(GZ_EXTENSION)) {
    if (strcmp(&szGz[szGz.length() - strlen(GZ_EXTENSION)], GZ_EXTENSION) != 0) {
      strcat(szGz, GZ_EXTENSION);
    } // end if
  } // end if

  // get info about source
  sqlfinfoc (szSource.asCharp(), &rteFileInfo, &rteFileErr );
  if (rteFileErr.sp5fe_result != vf_ok) {
    oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    return oEvent.IsEmpty();
  } // end if
  if (rteFileInfo.sp5fi_exists) {
    pBuffer = new char[rteFileInfo.sp5fi_size];
    if (pBuffer == NULL) {
      oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
    } // end if
  } // end if

  // open source
  sqlfopenc (szSource.asCharp(), sp5vf_binary, sp5vf_read, sp5bk_buffered, &hSource, &rteFileErr );
  if (rteFileErr.sp5fe_result != vf_ok) {
    oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    delete [] pBuffer;
    return oEvent.IsEmpty();
  } // end if

  // read source
  sqlfreadc(hSource, pBuffer, rteFileInfo.sp5fi_size, &nOutLen, &rteFileErr);
  if ((rteFileErr.sp5fe_result != vf_ok) || (rteFileInfo.sp5fi_size != nOutLen)) {
    oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    delete [] pBuffer;
    sqlfclosec ( hSource, sp5vf_close_normal, &rteFileErr );
    return oEvent.IsEmpty();
  } // end if

  // close the source
  sqlfclosec ( hSource, sp5vf_close_normal, &rteFileErr );
  if (rteFileErr.sp5fe_result != vf_ok) {
    oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    delete [] pBuffer;
    return oEvent.IsEmpty();
  } // end if

  // remove source
  if (bRemoveSource) {
    sqlferasec (szSource.asCharp(), &rteFileErr );
    if (rteFileErr.sp5fe_result != vf_ok) {
      oEvent.Overrule(DBMSrvMsg_RTEError(rteFileErr));
      oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
      delete [] pBuffer;
      return oEvent.IsEmpty();
    } // end if
  } // end if

  // open the target
  hTgz = gzopen(szGz, "wb");
  if (hTgz == NULL) {
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN,
	                                  Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME,szGz)));
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    if (bRemoveSource) {
      restoreFile(szSource, pBuffer, rteFileInfo.sp5fi_size);
    } // end if
    delete [] pBuffer;
    return oEvent.IsEmpty();
  } // end if

  // move archive content to tgz
  if (gzwrite(hTgz, pBuffer, (unsigned int) nOutLen) != nOutLen) {
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEWRITE,
	                                  Msg_Arg(SDBMSGTAG_DBMSRV_FILEWRITE__FILENAME,szGz)));
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    if (bRemoveSource) {
      restoreFile(szSource, pBuffer, rteFileInfo.sp5fi_size);
    } // end if
    delete [] pBuffer;
    gzclose(hTgz);
    return oEvent.IsEmpty();
  } // end if

  // close tgz
  if (gzclose(hTgz) != Z_OK) {
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECLOSE,
	                                  Msg_Arg(SDBMSGTAG_DBMSRV_FILECLOSE__FILENAME,szGz)));
    oEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    if (bRemoveSource) {
      restoreFile(szSource, pBuffer, rteFileInfo.sp5fi_size);
    } // end if
    return oEvent.IsEmpty();
  } // end if

  delete [] pBuffer;

  return oEvent.IsEmpty();
} // end cn54DBMTgz :: compressDirect

/*
  -------------------------------------------------------------------------
  private function cn54DBMTgz :: restoreFile
  -------------------------------------------------------------------------
*/
void cn54DBMTgz :: restoreFile 
      (const tsp00_Pathc & szSource,
       const char        * pBuffer,
       tsp00_Longint       nLen)
{
  tsp00_Int4          hSource;
  tsp05_RteFileError  rteFileErr;
  
  // restor source
  sqlfopenc (szSource.asCharp(), sp5vf_binary, sp5vf_write, sp5bk_buffered, &hSource, &rteFileErr );
  if (rteFileErr.sp5fe_result == vf_ok) {
    sqlfwritec (hSource, pBuffer, nLen, &rteFileErr );
    sqlfclosec ( hSource, sp5vf_close_normal, &rteFileErr );
  } // end if

} // end cn54DBMTgz :: restoreFile 
