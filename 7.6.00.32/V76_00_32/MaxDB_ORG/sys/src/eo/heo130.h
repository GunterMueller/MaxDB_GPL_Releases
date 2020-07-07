/* @lastChanged: "1998-06-08  14:26"
 * @filename:    heo130.h
 * @purpose:     "C-Types/Prototypes for external usage of XUSER Entries"
 * @release:     7.1.1.1
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

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



\endif

 */


#ifndef HEO130_H
#define HEO130_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  TYPES                                                      *
 *===========================================================================*/

#ifdef __cplusplus
    #define externC      extern "C"
#else
    #define externC
#endif

#define XU_MAX_USER_ENTRIES  32

#define XU_MAX_ERRORTEXT_LEN 40+1
#define XU_MAX_KEY_LEN       18+1
#define XU_MAX_NAME_LEN      64+1
#define XU_MAX_PWD_LEN       18+1
#define XU_MAX_SQLMODE_LEN    8+1
#define XU_MAX_DBNAME_LEN    18+1
#define XU_MAX_DBNODE_LEN    64+1
#define XU_MAX_DBLOCAL_LEN   64+1

typedef char XUserErrtext_T   [ XU_MAX_ERRORTEXT_LEN ];
typedef char XUserKey_T       [ XU_MAX_KEY_LEN       ];
typedef char XUserName_T      [ XU_MAX_NAME_LEN      ];
typedef char XUserPassWord_T  [ XU_MAX_PWD_LEN       ];
typedef char XUserSqlMode_T   [ XU_MAX_SQLMODE_LEN   ];
typedef char XUserDBName_T    [ XU_MAX_DBNAME_LEN    ];
typedef char XUserDBNode_T    [ XU_MAX_DBNODE_LEN    ];
typedef char XUserDBLocal_T   [ XU_MAX_DBLOCAL_LEN   ];

typedef  struct XUserData_T {
  XUserKey_T      UserKey;          
  XUserName_T     UserName;
  XUserPassWord_T Password;
  XUserDBNode_T   ServerNode;
  XUserDBName_T   ServerDB;
  XUserSqlMode_T  SqlMode;
  XUserDBLocal_T  DBLocal;
  int             CacheLimit;
  short           Timeout;
  short           IsolationLevel;
} XUserEntry_T;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC int  XUserOpen      ( XUserErrtext_T  ErrorText );

externC int  XUserClose     ( XUserErrtext_T  ErrorText );

externC void XUserInitEntry ( XUserEntry_T   *UserEntry );

externC int  XUserGetEntry  ( XUserEntry_T   *UserEntry,
                              XUserErrtext_T  ErrorText );

externC int  XUserPutEntry  ( XUserEntry_T   *UserEntry,
                              XUserErrtext_T  ErrorText );

externC int  XUserRemoveAll ();


#endif  /* HEO130_H */
