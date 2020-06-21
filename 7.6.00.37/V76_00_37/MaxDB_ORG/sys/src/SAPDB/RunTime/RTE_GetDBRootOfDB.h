/*!---------------------------------------------------------------------
  @file           RTE_GetDBRootOfDB.h
  @author         RobinW, RaymondR
  @brief          DBM Support: SAPDB Instance and Database Registration and Management Interfaces
  @see            

\if EMIT_LICENCE

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



\endif
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifndef externC
# if defined(__cplusplus)
#define externC extern "C" 
#  else
#define externC extern
# endif
#endif

/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/

#ifdef _WIN32

/*!--------------------------------------------------------------------
   @brief          Lookup dbroot from registry
   @param          pszServerDB [in] Full(!) name of database.               Full means: If the database is not registered for speed fast,               one of the speeds it is registered for has to follow the database names               in brackets. For example: DB74 (slow)
   @param          pszNodeName [in] name of the node the database is on. NULL if the database is local.
   @param          ulServerType [in] type of the database. Values are:              SERVER_TYPE_SERVERDB             (0)               SERVER_TYPE_GATEWAY              (1)              SERVER_TYPE_ADABAS_SERVERDB      (2)              SERVER_TYPE_ADABAS_GATEWAY       (3)
   @Return value   the DBRoot of the database. NULL if none was found.

 --------------------------------------------------------------------*/

externC SAPDB_Char *RTE_getDBRootFromRegistry ( SAPDB_Char  *pszNodeName,
                                        SAPDB_ULong ulServerType,
                                        SAPDB_Char  *pszServerDB );

#endif

/*!--------------------------------------------------------------------
   @brief          get the DBRoot of the database given by DBName
   @param          DBName [in] Name of database
   @param          DBRootOfDB [out] Pointer to a buffer that is filled with DBRoot of database
   @param          MaxDBRootLen [in] size of the buffer
   @Return value   true if call suceeded, false if not

 --------------------------------------------------------------------*/

externC SAPDB_Bool RTE_GetDBRootOfDB          ( SAPDB_Char *DBName,
                                        SAPDB_Char *DBRootOfDB,
                                        SAPDB_Int  MaxDBRootLen);

/*!--------------------------------------------------------------------
   @brief          Check if DBRoot points to a registered installation
   @param          DBRoot [in] path to dbroot
   @Return value   true if installation found, false if not
 --------------------------------------------------------------------*/
externC SAPDB_Bool RTE_CheckIfInstallationRegistered( SAPDB_Char *DBRoot);

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
