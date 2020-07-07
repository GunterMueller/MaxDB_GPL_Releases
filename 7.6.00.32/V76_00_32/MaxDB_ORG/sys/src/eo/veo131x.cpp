/*!
  @file           veo131x.cpp
  @author         BurkhardD (b)uddig
  @special area   XUSER File C++ Module
  @brief          Program to set/clear/list XUSER file content

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "heo131x.h"
#include "RunTime/Communication/RTEComm_URIBuilder.hpp"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static void PtoC ( char *CStr, tsp00_Byte *PascalStr, int Len);

static void CtoP ( tsp00_Byte *PascalStr, char *CStr, int Len );
/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

void buildServernodeURI( struct tsp4_xuser_record *paramUserRecord,
                                SAPDB_Char *    EncryptionType)
{
  SAPDB_Bool enc = !SAPDB_strcasecmp(EncryptionType, "ssl");
  if (!enc) {
    printf("Warning: Illegal encryption argument specified: %s (ignored)\n", EncryptionType);
    return;
  }
  Msg_List msgList;
  RTEComm_URIBuilder uri;
  RTEComm_URIBuilder::URIBuildRC rc;
  tsp00_NodeIdc szServerNode;
  SAPDB_UTF8 szServerDB[3*sizeof(paramUserRecord->xu_serverdb)];
  PtoC ( szServerNode, (tsp00_Byte*)&paramUserRecord->xu_servernode, sizeof(paramUserRecord->xu_servernode));
  PtoC ( (char*)szServerDB, (tsp00_Byte*)&paramUserRecord->xu_serverdb, sizeof(paramUserRecord->xu_serverdb));
  rc = uri.BuildDatabaseURI(szServerDB, szServerNode, msgList, enc); 
  if (rc == RTEComm_URIBuilder::NoError) {
    const SAPDB_Char* szUri = uri.GetURI();
    SAPDB_UInt4 len = uri.GetURILength();
    if (sizeof(paramUserRecord->xu_servernode) >= len) {
      SAPDB_memcpy(&paramUserRecord->xu_servernode, szUri, len);
      SAPDB_memset(paramUserRecord->xu_servernode+len, ' ', sizeof(paramUserRecord->xu_servernode)-len);
    }
    else {
      printf("FATAL: Uniform Resource Identifier (URI) to long for xuser entry\n");
      exit(2);
    }
  }
  else {
    SAPDB_Char const *const msg=msgList.Message();
    printf("FATAL: %s\n");
    exit(2);
  }
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

static void PtoC ( char       *CStr,
                   tsp00_Byte *PascalStr,
                   int         Len )
{

  teo00_Byte *PtrP = PascalStr + Len - 1 ;
  char *PtrC = CStr + Len ;

  *PtrC-- = '\0';

  while (PtrP >= PascalStr && *PtrP == ' ')
  {
    *PtrC-- = '\0';
    PtrP--;
  }

  while (PtrP >= PascalStr)
    *PtrC-- = *PtrP--;

}

static void CtoP ( tsp00_Byte *PascalStr,
            char       *CStr,
            int         Len )
{
  while (Len-- > 0)
    if (*CStr == '\0')
      *PascalStr++ = ' ';
    else
      *PascalStr++ = *CStr++;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

