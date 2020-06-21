/*!
  @file           geo54.h
  @author         JoergM
  @special area   Kernel I/O definitions
  @brief          Kernel I/O definitions
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/



#ifndef GEO54_H
#define GEO54_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*
 * This offset is used to be able to work with AIX Logical Volume Manager
 * The LVM uses the first 512 bytes for its own information and who knows
 * how much the next version will take away... So I choosed 2K offset for
 * the LZU Page0 Information 
 */
#define PAGE_0_RECORD_OFFSET_EO54  2048

#define NO_BAD_PAGE_MARKED_EO54      ((tsp00_PageNo)0)
#define DEVSPACE_LABEL_EO54          "SAPDB DEVSPACE"
#define OLD_UNIX_DEVSPACE_LABEL_EO54 "SQL-DB DEVSPACE"
#define SIZEOF_TSP_VF_LABEL_EO54     132

/*  The magic header is introduced to make sure we really read our header... */
#define MAGIC_HEADER_EO54          "RTE HEADER Joerg,Franki,Raymond"
/* Don't exceed MAGIC_HEADER_MXEO54 12345678901234567890123456789012 */
#define MAGIC_HEADER_MXEO54        32

#define NUMBER_OF_IO_RETRIES       3

#ifdef AIX
/* 
   The Logical Volume Manager forces us not to use the first 512 bytes...
   To keep them 'alive' these bytes are read before write
 */
#define PAGE_0_PRESERVE

#endif /* AIX */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*
 * Needed for upward compatibility ( < Kernel Version 7.2 )
 */
typedef struct teo54OldUnixPage0Info
{
    char         filler0_Ueo54;   /* begin of page_0 (no OFFSET!) */
    char         filler1_Ueo54;
    char         blockSizeByte1_Ueo54; /* offset 2   */
    char         blockSizeByte0_Ueo54; /* offset 3   */
    tsp00_PageNo badMarkedPage_Ueo54;  /* offset 4-7 */
    char         label_Ueo54[SIZEOF_TSP_VF_LABEL_EO54+1]; /* offset 8... */
} teo54OldUnixPage0Info;

/*
 * For documentation only
 */
typedef struct teo54Win32Page0Info
{
    char         filler0_Weo54;        /* begin of page_0 (no OFFSET!) */
    char         filler1_Weo54;
    char         blockSizeByte1_Weo54; /* offset 2     */
    char         blockSizeByte0_Weo54; /* offset 3     */
    char         blockSizeByte3_Weo54; /* offset 4     */
    char         blockSizeByte2_Weo54; /* offset 5     */
    char         filler2_Weo54;        /* offset 6     */
    char         filler3_Weo54;        /* offset 7     */
    char         productID_Weo54[6];   /* offset 8...  */
    char         devspaceID_Weo54[242];/* offset 14... */
    tsp00_Int4   maxSizePos_Weo54;     /* offset 256   */
    tsp00_Int4   sizePos_Weo54;
    tsp00_Int4   badMarkedPage_Weo54; 
    tsp00_Int4   reintegrationInfo_Weo54; 
} teo54Win32Page0Info;


typedef struct teo54Page0Info
{
    char         magicHeader_eo54[MAGIC_HEADER_MXEO54]; /* identification */
    tsp00_Int4   blockSize_eo54;        /* block size in bytes for device */
    tsp00_Int4   maxSize_eo54;          /* maximum device size in block sizes */
    tsp00_Int4   actualSize_eo54;       /* UNIX:NYI, NT actual size */
    tsp00_PageNo badMarkedPage_eo54;    /* Flag */
    char         label_eo54[SIZEOF_TSP_VF_LABEL_EO54+1]; /* user settable */
    tsp00_PageNo reintegrationInfo_eo54;/* Page */
} teo54Page0Info;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*
 * Normalized Number are written to disk
 */
externC tsp00_PageNo eo54SavablePageNo(tsp00_PageNo);
externC tsp00_Int4   eo54SavableInt4(tsp00_Int4);
/*
 * Convert number read from disk into internal format
 */
externC tsp00_PageNo eo54RestoredPageNo(tsp00_PageNo);
externC tsp00_Int4   eo54RestoredInt4(tsp00_Int4);

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO54_H */
