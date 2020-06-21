/*!
  @file           veo951.c
  @author         JoergM
  @special area   application startup program
  @brief          param
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp01.h"
#define XPARAM_NAMES_ONLY
#include "geo002.h"
#include "heo95.h"
#include "heo06.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"

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



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static char *expand_env_vars  ( char *path );
/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*-----------------------------------------------------------*/
char *e951_getparam ( char  *dbname, 
                      char  *param_id, 
                      char **param )
/*-----------------------------------------------------------*/
{
  tsp00_ErrText           ErrText;
  tsp00_C256c             ParamVal;
 
  DBG0 (( "e951_getparam for db <%s> called\n", dbname ));

  if(RTEConf_ParamaterAccessGetParam ( dbname , param_id, ParamVal, ErrText ))
  {
    expand_env_vars ( ParamVal ) ;
    *param = (char *) strdup ( ParamVal ) ;
    DBG0 (( "e951_getparam <%s>:\n", param_id,ParamVal ));
  }
  else
  {
    /* PTS 1107204 */
     DBG0 (( "\ne951_getparam: %s\n", ErrText )) ;
     *param = NULL ;
  }

  return ( *param ) ;
}

/*-----------------------------------------------------------*/
int e951_f_access ( char *path, 
                    int   mode, 
                    int   pri_err )
/*-----------------------------------------------------------*/
{
  struct stat buf;
  int access_ok ;

  access_ok = stat ( path, &buf )  == 0 && (buf.st_mode & mode) == mode ;
  DBG0 (( "trying to access path <%s> with mode %02x\n", path, mode ));
  if ( !access_ok && pri_err )
    {
      perror ( "\nstat error" ) ;
      printf ( "can't access path  >%s<\n", path ) ;
    }

  return ( access_ok ) ;
}


/*-----------------------------------------------------------*/
char *e951_toupper_str ( char *str )
/*-----------------------------------------------------------*/
{
  char *base = str ;
  for ( ; *str != '\0'; str++ )
    if ( islower ( *str ) )
      *str = (char )toupper ( *str ) ;
  return ( base );
}

/*-----------------------------------------------------------*/
int e951_create_path ( char          *path_to_create, 
                       unsigned short mode )
/*-----------------------------------------------------------*/
{
  int create_ok = TRUE ;
  char *act_pos = path_to_create , *end_pos;

#ifdef WIN32
     if ( act_pos[1] == ':' ) act_pos += 2;
#endif
  if ( *act_pos == PATH_DELIMITER_EO01 ) act_pos++ ;

  do
    { if ( (end_pos = (char *)strchr ( act_pos, PATH_DELIMITER_EO01 )) != CNULL )
        *end_pos = '\0' ;
      if ( !e951_f_access ( path_to_create , S_IFDIR, FALSE ) )
        { DBG0 (( "\ncreating directory <%s>", path_to_create ));
#ifdef WIN32
          if ( !(create_ok = CreateDirectory ( path_to_create, NULL ) != 0 ) )
#else
          if ( !(create_ok = mkdir ( path_to_create, mode ) == 0 ) )
#endif
            perror ( "can't create directory" ) ;
        }
      if ( end_pos != CNULL )
        { *end_pos = PATH_DELIMITER_EO01; act_pos = end_pos+1 ; }
    }
  while ( create_ok && end_pos != CNULL ) ;
  return ( create_ok ) ;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/
/*-----------------------------------------------------------*/
static char *expand_env_vars ( char *path )
/*-----------------------------------------------------------*/
{
  int len ;
  tsp00_Pathc TmpTmpFile,exp_path ;
  char *beg_env_var, *end_env_var, *env_var_val,
       *act_pos, *exp_path_pos;

  DBG0 (( "expand_env_vars: before expand <%s>\n", path ));
  
  do                                             /* PTS 1000713 */
  { 
    beg_env_var  = end_env_var = CNULL ;
    act_pos      = path;
    exp_path_pos = exp_path ;

    strcpy( TmpTmpFile, path ) ;
    while ( ( beg_env_var = strchr ( act_pos      , BEG_ENV_VAR ) ) != CNULL &&
          ( end_env_var = strchr ( beg_env_var+1, END_ENV_VAR ) ) != CNULL )
    { *end_env_var = '\0' ;
      if ( ( env_var_val = (char *)getenv ( beg_env_var+1 ) ) != CNULL )
        { len = (int ) (beg_env_var - act_pos) ;
#ifndef WIN32
          if ( * ( beg_env_var - 1 ) == '$' ) len-- ;
#endif
          if ( len > 0 )
            strncpy ( exp_path_pos, act_pos, len ) ;
          strcpy ( exp_path_pos + len, env_var_val );
          exp_path_pos += len + strlen ( env_var_val ) ;
          act_pos = end_env_var + 1 ;
        }
    }

    if ( end_env_var != CNULL || beg_env_var != CNULL )
    {
      strcpy ( exp_path_pos, act_pos ) ;
      strcpy ( path, exp_path ) ;
    }
    
  }
  while ( strcmp( TmpTmpFile, path ) != 0 ) ;

  DBG0 (( "expand_env_vars: after expand <%s>\n", path ));
  return ( path ) ;
}



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/