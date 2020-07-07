/*! 
  -----------------------------------------------------------------------------
 
  module: hcn54.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBM Server
 
  description:  specification of the TGZ Class

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
 

#ifndef _HCN54_H_
#define _HCN54_H_

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "gsp00.h"

/*!  
  -----------------------------------------------------------------------------
  EndChapter: Macros and Constants
  -----------------------------------------------------------------------------
 */


/*
  -----------------------------------------------------------------------------
  Class:        cn54DBMTgz
  -----------------------------------------------------------------------------

  description:  TGZ Class

  -----------------------------------------------------------------------------
*/
class cn54DBMTgz {

public:
/*!
  -------------------------------------------------------------------------
  function:     cn54DBMTgz
  -------------------------------------------------------------------------
  description:  constructor

                Create the archive.

  arguments:    szArchive [IN] - the name of the archive 
                                 This name will exented with a ".tar" for the
                                 uncompressed version and a ".tgz" for the compressed
                                 version.

                                 This constructor overwrites existing file <szArchive>.tar.

  -------------------------------------------------------------------------
 */
  cn54DBMTgz (const tsp00_Pathc &  szArchive);
  
/*!
  -------------------------------------------------------------------------
  function:     ~cn54DBMTgz
  -------------------------------------------------------------------------
  description:  destructor
  -------------------------------------------------------------------------
 */
  ~cn54DBMTgz ();

/*!
  -------------------------------------------------------------------------
  function:     append
  -------------------------------------------------------------------------
  description:  append a file to the compressed tgz archive

  arguments:    szFile   [IN] - the name of the original file
                szTarget [IN] - the name of the file in the archive

  return value: success or not (ask lastEvent() for more information)

  prototypes:   2
  -------------------------------------------------------------------------
 */
  bool append(const tsp00_Pathc &  szFile);
  bool append(const tsp00_Pathc &  szFile,
              const tsp00_Pathc &  szTarget);
/*!
  -------------------------------------------------------------------------
  function:     compressDirect
  -------------------------------------------------------------------------
  description:  converts the source file to a compressed gz file

                This function overwrites existing file <szTarget>.gz and 
                removes the source (if requested) after reading and before 
                writing. If there a error during writing the function makes a
                restore attempt of the source but if this failes the source
                may be lost.

                The function needs memory for the whole source file.

  arguments:    szTarget      [IN]  - the target file
                szSource      [IN]  - the source file
                oEvent        [OUT] - for indication of errors
                bRemoveSource [IN]  - flag for removing the source after reading
  
  return value: success or not (ask lastEvent() for more information)
  -------------------------------------------------------------------------
 */
  static bool compressDirect(const tsp00_Pathc &  szTarget,
                             const tsp00_Pathc &  szSource,
                             Msg_List  &  oEvent,
                             bool                 bRemoveSource = true);

/*!
  -------------------------------------------------------------------------
  function:     lastEvent
  -------------------------------------------------------------------------
  description:  return the last event

  return value: the event list object
  -------------------------------------------------------------------------
 */
  const Msg_List & lastEvent() {return m_oEvent;}

private:

  static void restoreFile (const tsp00_Pathc & szSource,
                           const char        * pBuffer,
                           tsp00_Longint       nLen);

  typedef struct {
	  char name[100];
	  char mode[8];
	  char uid[8];
	  char gid[8];
	  char size[12];
	  char mtime[12];
	  char chksum[8];
	  char typeflag[1];	
	  char linkname[100];
	  char magic[6];
	  char version[2];
	  char uname[32];
	  char gname[32];
	  char devmajor[8];
	  char devminor[8];
	  char prefix[155];
  } tar_hdr_t;

  tsp00_Pathc m_szTgz;

  Msg_List m_oEvent;
  unsigned char *  m_oBuffer;

};

/* EndClass: cn54DBMTgz */

#endif  // _HCN54_H_
