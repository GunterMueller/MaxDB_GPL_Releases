/* @lastChanged: "1998-06-05  13:14"
 * @filename:    veo130.c
 * @purpose:     "XUSER-File API for external application ( R/3inst )"
 * @release:     7.1.0.0
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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "vsp004.h"
#include "gsp00.h"
#include "geo00_0.h"
#include "heo46.h"
#include "heo130.h"
#include "heo102.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

teo00_Longint sql13u_init_user_params ( tsp4_xuser_record  *prcUserParams );


/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static void XUConvEntryInternToExtern ( tsp4_xuser_record   *InternTypes,
                                        XUserEntry_T        *ExternTypes ) ;


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


int XUserOpen ( XUserErrtext_T ErrorText )
{

  #undef  MF__
  #define MF__ MOD__"XUserOpen"

  tsp00_ErrText acErrorText ;
  bool        bOk ;

  sqlxuopenuser( NULL, acErrorText, &bOk) ;

  eo46PtoC( ErrorText, acErrorText, sizeof (acErrorText) );
  
  return ( bOk ) ;
}

/*------------------------------*/

int XUserClose ( XUserErrtext_T ErrorText )
{

  #undef  MF__
  #define MF__ MOD__"XUserClose"

  tsp00_ErrText acErrorText ;
  bool        bOk ;

  sqlxucloseuser( NULL, acErrorText, &bOk) ;

  eo46PtoC( ErrorText, acErrorText, sizeof (acErrorText) );
  
  return ( bOk ) ;
}


/*------------------------------*/

void XUserInitEntry ( XUserEntry_T  *UserEntry )
{

  #undef  MF__
  #define MF__ MOD__"XUserInitEntry"

  tsp4_xuser_record   UserParams;

  sql13u_init_user_params ( &UserParams );

  XUConvEntryInternToExtern( &UserParams, UserEntry ) ;

  return ;
}

/*------------------------------*/

int XUserGetEntry ( XUserEntry_T  *UserEntry,
                    XUserErrtext_T ErrorText )
{

  #undef  MF__
  #define MF__ MOD__"XUserGetEntry"

  tsp4_xuser_record   UserParams;
  tsp00_ErrText         acErrorText ;
  bool                bOk ;

  eo46CtoP(UserParams.xu_key, UserEntry->UserKey, sizeof (UserParams.xu_key));

  sqlgetuser (&UserParams, NULL, acErrorText, &bOk) ;

  if ( bOk )
    XUConvEntryInternToExtern( &UserParams, UserEntry ) ;

  eo46PtoC( ErrorText, acErrorText, sizeof (acErrorText) );
  
  return ( bOk ) ;
}

/*------------------------------*/

int XUserPutEntry ( XUserEntry_T  *UserEntry,
                    XUserErrtext_T ErrorText )
{

  #undef  MF__
  #define MF__ MOD__"XUserPutEntry"

  tsp4_xuser_record   UserParams;
  tsp00_ErrText         acErrorText ;
  bool                bOk ;
  tsp00_Name            passwdBuf;

  void s02applencrypt();

  sql13u_init_user_params ( &UserParams );  /* PTS 1110857 */

  eo46CtoP(UserParams.xu_key , UserEntry->UserKey , sizeof (UserParams.xu_key));
  eo46CtoP(UserParams.xu_user, UserEntry->UserName, sizeof (UserParams.xu_user));

  eo46CtoP(passwdBuf, UserEntry->Password, sizeof (passwdBuf) ) ;
  s02applencrypt(passwdBuf, UserParams.xu_password ) ;

  eo46CtoP( UserParams.xu_servernode, UserEntry->ServerNode, sizeof (UserParams.xu_servernode) );
  eo46CtoP( UserParams.xu_serverdb  , UserEntry->ServerDB  , sizeof (UserParams.xu_serverdb) );
  eo46CtoP( UserParams.xu_sqlmode   , UserEntry->SqlMode   , sizeof (UserParams.xu_sqlmode) );
  eo46CtoP( UserParams.xu_dblang    , UserEntry->DBLocal   , sizeof (UserParams.xu_dblang) );

  UserParams.xu_cachelimit  = UserEntry->CacheLimit     ;
  UserParams.xu_timeout     = UserEntry->Timeout        ;
  UserParams.xu_isolation   = UserEntry->IsolationLevel ;

  sqlputuser (&UserParams, NULL, acErrorText, &bOk) ;

  eo46PtoC( ErrorText, acErrorText, sizeof (acErrorText) );
  
  return ( bOk ) ;
}

/*------------------------------*/

int  XUserRemoveAll ()
{

  #undef  MF__
  #define MF__ MOD__"XUserRemoveAll"

  return ( sqlclearuser(NULL) ) ;
}


/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/


static void XUConvEntryInternToExtern ( tsp4_xuser_record   *InternTypes,
                                        XUserEntry_T        *ExternTypes ) 
{
  eo46PtoC( ExternTypes->UserName, InternTypes->xu_user, sizeof (InternTypes->xu_user) );
  strcpy(ExternTypes->Password,"hidden");
  eo46PtoC( ExternTypes->ServerNode, InternTypes->xu_servernode, sizeof (InternTypes->xu_servernode) );
  eo46PtoC( ExternTypes->ServerDB  , InternTypes->xu_serverdb  , sizeof (InternTypes->xu_serverdb) );
  eo46PtoC( ExternTypes->SqlMode   , InternTypes->xu_sqlmode   , sizeof (InternTypes->xu_sqlmode) );
  eo46PtoC( ExternTypes->DBLocal   , InternTypes->xu_dblang    , sizeof (InternTypes->xu_dblang) );

  ExternTypes->CacheLimit     = InternTypes->xu_cachelimit;
  ExternTypes->Timeout        = InternTypes->xu_timeout   ;
  ExternTypes->IsolationLevel = InternTypes->xu_isolation ;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/