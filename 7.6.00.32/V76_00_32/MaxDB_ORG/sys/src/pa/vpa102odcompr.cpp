/*!**********************************************************************

  module: vpa102odcompr.cpp

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC

  description:  C wrapper for ODBC extension to compress/decompress data

  see also: odcompr.h for specification

  -----------------------------------------------------------------------

                         Copyright (c) 1999-2005 SAP AG




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

#include "hpa111ODCompr2.h"
#include "hpa102odcompr.h"

/*****************************************************************
 * tpa102_ODCompr
 *****************************************************************/

struct st_odcompr {
    st_odcompr( tsp00_Int4 maxBlockSize )
        : odcompr( maxBlockSize ) { };
    tpa111_ODCompr2 odcompr;
};

tsp00_Bool pa102AllocCompr( tpa102_ODCompr *compr, tsp00_Int4 maxBlockSize )
{
    *compr = new st_odcompr( maxBlockSize );

    return (compr != NULL);
}; // pa102AllocCompr


void pa102FreeCompr( tpa102_ODCompr compr )
{
    delete compr;
}; // pa102FreeODCompr


SQLRETURN pa102InitCompr( tpa102_ODCompr  compr,
                          SQLHSTMT        hstmt,
                          tsp00_Int4      totalDataSize,
                          tsp00_Int4     *comprErr )
{
    tsp00_Int4 written = 0;

    return compr->odcompr.InitCompr( hstmt,
                                     totalDataSize,
                                     &written,
                                     comprErr );
}; // pa102InitCompr

SQLRETURN pa102CompressBuffer( tpa102_ODCompr  compr,
                               SQLPOINTER      buffer,
                               SQLINTEGER      bufferLen,
                               tsp00_Int4     *bytesReadPtr,
                               tsp00_Int4     *bytesCompressedPtr,
                               tsp00_Bool     *allBytesCompressedPtr,
                               tsp00_Int4     *comprErr )
{
    return compr->odcompr.CompressBuffer( buffer,
                                          bufferLen,
                                          bytesReadPtr,
                                          bytesCompressedPtr,
                                          allBytesCompressedPtr,
                                          comprErr );
}; // pa102CompressBuffer

    
SQLRETURN pa102FlushBuffer( tpa102_ODCompr compr )
{
    return compr->odcompr.FlushBuffer();
}; // pa102FlushBuffer


SQLRETURN pa102PutData( tpa102_ODCompr  compr,
                        SQLPOINTER      buffer,
                        SQLINTEGER      strLenOrIndi,
                        tsp00_Int4     *comprErr )
{
    return compr->odcompr.PutData( buffer, strLenOrIndi, comprErr );
}; // pa102PutData


tsp00_Int4 pa102GetLen( tpa102_ODCompr compr, unsigned char *buffer )
{
    return 0; //... compr->odcompr.GetLen( buffer );
}; // pa102GetLen

/*****************************************************************
 * tpa102_ODDecompr
 *****************************************************************/

struct st_oddecompr {
    tpa111_ODDecompr2 oddecompr;
};

tsp00_Bool pa102AllocDecompr( tpa102_ODDecompr *decompr )
{
    *decompr = new struct st_oddecompr;
    return (decompr != NULL);
}; // pa102AllocDecompr


void pa102FreeDecompr( tpa102_ODDecompr decompr )
{
    delete decompr;
}; // pa102FreeDecompr


void pa102InitDecompr( tpa102_ODDecompr  decompr,
                       SQLHSTMT          hstmt,
                       SQLUSMALLINT      colNo )
{
    decompr->oddecompr.InitDecompr( hstmt, colNo );
}; // pa102InitDecompr

    
SQLRETURN pa102GetData( tpa102_ODDecompr  decompr,
                        SQLPOINTER        buffer,
                        SQLINTEGER        bufferLen_in,
                        SQLINTEGER       *strLenOrIndiPtr_in,
                        tsp00_Int4       *comprErr )
{
    SQLLEN     bufferLen, dummy;
    SQLLEN    *strLenOrIndiPtr;
    SQLRETURN  retVal;

    bufferLen = bufferLen_in;
    if (strLenOrIndiPtr_in == NULL)
        strLenOrIndiPtr = NULL;
    else {
        strLenOrIndiPtr = &dummy;
        *strLenOrIndiPtr = *strLenOrIndiPtr_in;
    }

    retVal = decompr->oddecompr.GetData( buffer,
                                         bufferLen,
                                         strLenOrIndiPtr,
                                         comprErr );

    if (strLenOrIndiPtr_in != NULL)
        *strLenOrIndiPtr_in = (SQLINTEGER) *strLenOrIndiPtr;

    return retVal;
}; // pa102GetData
