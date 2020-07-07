/*!
  @file           geo13.h
  @author         JoergM
  @special area   XUSER
  @brief          XUser file and registry API
  @first created  000-10-10  9:12
  @see            example.html ...


  Internal used typedefinitions and constants

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



#ifndef GEO13_H
#define GEO13_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "vsp004.h"
#include "gsp100.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MX_XUSER_ENTRIES            32

/***************************************************************/
#if !defined(_WIN32) && !defined(OS2) 
/***************************************************************/
#define ERROR_ACCESS_DENIED         -2
#define INFO_OK                     0
#define MX_XUSER_ENTRIES            32
#define DEFAULT_USER            "DEFAULT           "
#define ERROR_FILE_NOT_FOUND        -2
#define EOF_XUSER_PAGE             -99

#define GET_SERVERDB sql13c_get_serverdb
#define GET_DBLANG   sql13c_get_dblang

#define ERRMSG_DATA_NEWER_THAN_COMPONENT    "USER data newer than component"
#define ERRMSG_ILLEGAL_USER_INDEX           "illegal USER index"
#define ERRMSG_BLANK_USERKEY                "blank user key not allowed"
#define ERRMSG_NO_XUSER_ENTRY_FOUND         "the USERKEY is unknown"
#define ERRMSG_NO_SPACE_LEFT                "space for USER entries exhausted"
#define ERRMSG_READ_XUSER_REG_DATA          "could not read USER data"
#define ERRMSG_WRONG_XUSER_REG_DATA_LEN     "wrong USER data length"
#define ERRMSG_DIFFERENT_SIDS               "incorrect USER data owner"

/***************************************************************/
#else
/***************************************************************/

#define GET_SERVERDB sql01c_get_serverdb
#define GET_DBLANG   sql01c_get_dblang

/***************************************************************/
#endif
/***************************************************************/

/*
//  --- R E L 6 1 1   -  DEFINES, TYPE AND STRUCT DEFINITIONS
*/
#define REL611_MX_XUSER_ENTRIES     32
#define REL611_REG_VN_XUSER         "XUData"

#define XUSER_RECORD_VERSION        3

#define XUSER_DATA_SIZE ( sizeof(XUSER_INFO_PAGE) + \
                          sizeof(XUSER_PAGE) * MX_XUSER_ENTRIES )

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

typedef struct rel611_xuser_params_record {
    tsp4_xuserkey                 xu_key;
    tsp00_Int2                    xu_fill;
    tsp00_NodeId                  xu_servernode;
    tsp00_DbName                  xu_serverdb;
    tsp00_C18                     xu_user;
    tsp00_CryptPw                 xu_password;
    tsp4_sqlmode_name             xu_sqlmode;
    tsp00_Int4                    xu_cachelimit;
    tsp00_Int2                    xu_timeout;
    tsp00_Int2                    xu_isolation;
} REL611_XUSER_PARAMS_REC;

typedef struct rel611_xuser_page {
    tsp00_Int4                    rec_no;
    REL611_XUSER_PARAMS_REC       params;
} REL611_XUSER_PAGE;

typedef struct rel611_effective_user_id_record {
    tsp00_Int4    euid_no;
    tsp00_C20     euid_field [ 6 ];
} REL611_EFFECTIVE_USER_ID_REC;

typedef union {
    REL611_XUSER_PAGE             xu_rec;
    REL611_EFFECTIVE_USER_ID_REC  euid_rec;
} REL611_XUSER_PAGE_UNION;

typedef REL611_XUSER_PAGE_UNION   *PREL611_XUSER_PAGE_UNION;

#define REL611_XUSER_PAGE_BUFFER_SIZE \
          ( sizeof(REL611_XUSER_PAGE_UNION) * (REL611_MX_XUSER_ENTRIES + 1))

/*= PTS 1105361 =*/
#ifdef OSF1
/*==================*/
/* FH problems with 32 Bit and 64 Bit xuser-access on the same maschine */
typedef struct xuser_info_page {
    teo00_Longint                 ulInfoPageSize;
    teo00_Longuint                ulXUserVersionID;
    tsp100_VersionID1		  RTEVersionID;
    teo00_Longuint                ulPages;
    teo00_Longuint                ulPageSize;
    tsp00_C20                     uidField [ 6 ];
} XUSER_INFO_PAGE;
/*==================*/
#else
/*==================*/
typedef struct xuser_info_page {
    teo00_Uint4                   ulInfoPageSize;
    teo00_Uint4                   ulXUserVersionID;
    tsp100_VersionID1	          RTEVersionID;
    teo00_Uint4                   ulPages;
    teo00_Uint4                   ulPageSize;
    tsp00_C20                     uidField [ 6 ];
} XUSER_INFO_PAGE;
/*==================*/
#endif
/*==================*/

typedef XUSER_INFO_PAGE           *PXUSER_INFO_PAGE;
typedef tsp4_xuser_record         XUSER_PAGE;
typedef XUSER_PAGE                *PXUSER_PAGE;

#define XUSER_DATA_SIZE ( sizeof(XUSER_INFO_PAGE) + \
                          sizeof(XUSER_PAGE) * MX_XUSER_ENTRIES )

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

tsp00_Int4    
sql13u_remove_xuser_entries (SAPDB_Char *              accountName, 
                             tsp00_ErrText             acErrorText );

tsp00_Int4    
sql13u_get_all_entries      (SAPDB_Char *              accountName, 
                             char                      **pcDataBuffer,
                             tsp00_Int4                *lDataLen,
                             PREL611_XUSER_PAGE_UNION  *pRel611XUserPages,
                             tsp00_ErrText             errtext ) ;

tsp00_Int4    
sql13u_write_xuser_entries  (SAPDB_Char *              accountName,
                             char                      *cDataBuffer,
                             PXUSER_INFO_PAGE          pXUserInfoPage,
                             tsp00_ErrText             acErrorText ) ;

#if defined(_WIN32) 

BOOLEAN sql13u_user_is_guest        ( void );

#else
externC int sql13c_get_serverdb ( char **serverdb );

externC int sql13c_get_dblang ( char **dblang );

externC teo00_Bool sql13c_only_blanks ( char * str );

#endif

void   
sql13u_init_user_params ( tsp4_xuser_record  *prcUserParams );

tsp00_Int4 
sql13u_getuser (tsp4_xuser_record  *prcUserParams,
                SAPDB_Char *        accountName,
                tsp00_ErrText       acErrorText );

#endif  /* GEO13_H */
