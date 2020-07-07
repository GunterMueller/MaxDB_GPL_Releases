/*!**********************************************************************

  module: WDVCAPI_Put.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Put

  see also:

  -----------------------------------------------------------------------

						Copyright (c) 2000-2005 SAP AG



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




************************************************************************/

#ifndef WDVCAPI_PUT_H
#define WDVCAPI_PUT_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "SAPDB/WebDAV/CAPI/WDVCAPI_XMLIndexing.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ZLibIO.h"

/***********************************************************************

	Types

 ***********************************************************************/

typedef struct st_put_handle  *WDVCAPI_PutHandle;

struct st_put_handle {
    SQLHSTMT                    hStmtContainer;
    SQLHSTMT                    hStmtInsertResource;
    SQLHSTMT                    hStmtInode;

    WDVCAPI_Id                  pid;
    SQLLEN                      pIdIndicator;

    WDVCAPI_Id                  cid;
    SQLLEN                      cIdIndicator;

    WDVCAPI_ResourceNameString  name;
    SQLLEN                      nameIndicator;

    char                        firstBlock[8000];
    WDVCAPI_ContentLength       firstBlockLength;
    WDV_Bool                    firstBlockDone;
    SQLLEN                      shortContentIndicator;

    WDV_Bool                    compressionInitialized;
    gzFile                      gzFile;
    WDV_Bool                    compress;

    SQLHSTMT                    hStmtXMLPut;
    SAPDB_UInt8                 xdbDocId;
    WDV_Bool                    isDocForXDB;
    WDVCAPI_InStream            inStream;
    void                       *inStreamUserData;

    WDVCAPI_ContentTypeString   contentTypeString;
    WDVCAPI_ContentLength       contentLength;

    SQLLEN                      bufferLength;

    SQLHSTMT                    hStmtSelectByName;
    WDVCAPI_Id                  selectByNameParentId;
    SQLLEN                      selectByNameParentIdIndicator;
    WDVCAPI_Id                  selectByNameChildId;
    SQLLEN                      selectByNameChildIdIndicator;
    WDVCAPI_ResourceNameString  selectByNameName;
    SQLLEN                      selectByNameNameIndicator;

    WDVCAPI_XmlDocClass         hXmlDocClass;
    WDV_Bool                    asyncIndexing;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool PutCreateHandle( WDVCAPI_WDV  wdv);

WDV_Bool PutDestroyHandle( WDVCAPI_WDV  wdv);

/***********************************************************************

	End

 ***********************************************************************/

#endif
