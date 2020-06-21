/*!**********************************************************************

  module: vwd105dynbuf.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Dynamic buffer

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd105dynbuf.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: twd105DynBufP

************************************************************************/

typedef struct st_buf_elem *BufElemP;

struct st_buf_elem
{
    char        *buf;
    sapdbwa_Int4   bufSize;
    sapdbwa_Int4   usedSize;
    BufElemP     nextElem;
}; /* struct st_buf_elem */

sapdbwa_Bool wd105IsEmpty( twd105DynBufP dynBuf );

sapdbwa_Int4 wd105_CalcTotalSize( BufElemP  bufList );

BufElemP wd105_CreateBufElem( sapdbwa_Int4 bufSize );

void wd105_FreeBufList( BufElemP  bufList );

sapdbwa_UInt4 wd105_MakeSize( sapdbwa_UInt4 initialSize,
                            sapdbwa_UInt4 requestedSize );


struct st_dyn_buf
{
    sapdbwa_Int4   initialSize;
    sapdbwa_Int4   actAllocSize;
    
    BufElemP     bufList;
    BufElemP     actBufElem;
}; /* struct st_dyn_buf */


twd105DynBufP wd105CreateDynBuf( sapdbwa_Int4 initialSize )
{
    sapdbwa_Bool     allocatOK = sapdbwa_True;
    twd105DynBufP  newDynBuf = NULL;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_dyn_buf),
                   (sapdbwa_UInt1**) &newDynBuf,
                   &allocatOK );
    if (allocatOK) {
        newDynBuf->initialSize   = initialSize;
        newDynBuf->actAllocSize  = initialSize;
        newDynBuf->bufList       = NULL;
        newDynBuf->actBufElem    = NULL;
    }; /* if */

    return newDynBuf;
} /* wd105CreateDynBuf */


void wd105DestroyDynBuf( twd105DynBufP dynBuf )
{
    if (dynBuf->bufList != NULL) {
        wd105_FreeBufList( dynBuf->bufList );
    }; /* if */
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) dynBuf );
} /* wd105DestroyDynBuf */


sapdbwa_Bool wd105AddToBuf( twd105DynBufP  dynBuf,
                          char          *buf,
                          sapdbwa_UInt4    nBytes )
{
    sapdbwa_Bool  addToBufOK = sapdbwa_True;
    BufElemP    newBufElem = NULL;
    sapdbwa_Int4  bytesPending = 0;
    char       *bufPos       = NULL;
    sapdbwa_Bool  bufferTooSmall = sapdbwa_False;
    sapdbwa_Int4  freeSize = 0;
    
    if (dynBuf->bufList == NULL) {
        /* allocate first bufElem */
        dynBuf->bufList = wd105_CreateBufElem( dynBuf->initialSize );
        addToBufOK = (dynBuf->bufList != NULL);
        dynBuf->actBufElem = dynBuf->bufList;
    }; /* if */

    bufPos         = buf;
    bytesPending   = nBytes;
    bufferTooSmall = sapdbwa_False;
    do {
        freeSize = dynBuf->actBufElem->bufSize - dynBuf->actBufElem->usedSize;
        bufferTooSmall = (freeSize < bytesPending);
        if (bufferTooSmall) {
            /* copy first bytes */
            SAPDB_memcpy( dynBuf->actBufElem->buf + dynBuf->actBufElem->usedSize,
                    bufPos, freeSize );
            dynBuf->actBufElem->usedSize += freeSize;
            bufPos += freeSize;
            bytesPending -= freeSize;
            /* create new bufElem twice as big */
            dynBuf->actAllocSize *= 2;
            newBufElem = wd105_CreateBufElem( dynBuf->actAllocSize );
            if (newBufElem) {
                dynBuf->actBufElem->nextElem = newBufElem;
                dynBuf->actBufElem = newBufElem;
                dynBuf->actBufElem->usedSize = 0;
            } else {
                addToBufOK = sapdbwa_False;
            }; /* else */
        } else {
            SAPDB_memcpy( dynBuf->actBufElem->buf + dynBuf->actBufElem->usedSize,
                    bufPos, bytesPending );
            dynBuf->actBufElem->usedSize += bytesPending;
        }; /* else */
    } while (bufferTooSmall && addToBufOK);
        
    return addToBufOK;
} /* wd105AddToBuf */


sapdbwa_Bool wd105FreeBuf( twd105DynBufP dynBuf )
{
    if (dynBuf->bufList != NULL) {
        /* keep the first elem */
        wd105_FreeBufList( dynBuf->bufList->nextElem );
        dynBuf->actAllocSize = dynBuf->initialSize;
        dynBuf->actBufElem = dynBuf->bufList;
        dynBuf->actBufElem->usedSize = 0;
        dynBuf->bufList->nextElem = NULL;
    }; /* if */

    return sapdbwa_True;
} /* wd105FreeBuf */


sapdbwa_UInt4 wd105GetTotalSize( twd105DynBufP dynBuf )
{
    return wd105_CalcTotalSize( dynBuf->bufList );
} /* wd105GetTotalSize */


SQLRETURN wd105PutDataCompressed( twd105DynBufP   dynBuf,
                                  tpa102_ODCompr  odcompr,
                                  sapdbwa_Int4     *comprErr )
{
    BufElemP  actElem  = dynBuf->bufList;
    SQLRETURN retcode  = SQL_SUCCESS;

    if (wd105IsEmpty( dynBuf )) {
        /* we have to call pa102PutData at least one time to write
         * the header */
        retcode = pa102PutData( odcompr, NULL, 0, comprErr );
    }; /* if */
    while (actElem != NULL &&
           (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        retcode = pa102PutData( odcompr,
                                actElem->buf, actElem->usedSize,
                                comprErr );
        actElem = actElem->nextElem;
    }; /* while */

    return retcode;
} /* wd105PutDataCompressed */

/*!**********************************************************************

  EndChapter: twd105DynBufP

************************************************************************/

sapdbwa_Bool wd105IsEmpty( twd105DynBufP dynBuf )
{
    return (dynBuf->bufList == NULL);
} /* wd105IsEmpty */


sapdbwa_Int4 wd105_CalcTotalSize( BufElemP  bufList )
{
    BufElemP    actElem  = bufList;
    sapdbwa_Int4  totalSize = 0;
    
    while (actElem != NULL) {
        totalSize += actElem->usedSize;
        actElem = actElem->nextElem;
    }; /* while */

    return totalSize;
} /* wd105_CalcTotalSize */


BufElemP wd105_CreateBufElem( sapdbwa_Int4 bufSize )
{
    sapdbwa_Bool allocatOK = sapdbwa_True;
    BufElemP   newBufElem = NULL;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_buf_elem ),
                   (sapdbwa_UInt1**) &newBufElem,
                   &allocatOK );
    if (allocatOK) {
        newBufElem->bufSize   = bufSize;
        newBufElem->usedSize  = 0;        
        newBufElem->nextElem  = NULL;
        sapdbwa_SQLALLOCAT( bufSize,
                       (sapdbwa_UInt1**) &newBufElem->buf,
                       &allocatOK );
        if (!allocatOK) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newBufElem );
        }; /* if */
    }; /* if */

    return (allocatOK ? newBufElem : NULL);
} /* wd105_CreateBufElem */


void wd105_FreeBufList( BufElemP  bufList )
{
    BufElemP  actElem  = bufList;
    BufElemP  nextElem = NULL;

    while (actElem != NULL) {
        nextElem = actElem->nextElem;
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) actElem->buf );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) actElem );
        actElem = nextElem;
    }; /* while */
} /* wd105_FreeBufList */


sapdbwa_UInt4 wd105_MakeSize( sapdbwa_UInt4 initialSize,
                            sapdbwa_UInt4 requestedSize )
{
    sapdbwa_UInt4 allocSize = initialSize;
    
    while (allocSize < requestedSize) {
        allocSize *= 2;
    }; /* while */

    return allocSize;
} /* wd105_MakeSize */
