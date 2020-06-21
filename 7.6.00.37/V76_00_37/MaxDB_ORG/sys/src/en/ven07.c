/*!
  @file           ven07.c
  @author         JoergM
  @brief          Client RunTime: sqlexec
  @see            


  Starting Of Other Programs

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
*/

#define MOD__ "ven07.c:"
#include "gen00.h"
#include "geo007_1.h"
#include "hen40.h"
/* ***** external procedures/functions ************************************ */

extern char *           getenv () ;

/* ***** extern vars ****************************************************** */

extern  int sql01_vt_clear ;

/* ***** global vars ****************************************************** */

typedef  char                           exec_line_3 [ MX_EXECLINE + 3 ] ;
static   SIGNALFUNCTYPE                 ( * oldsigcld ) ( ) ;

/* ***** error text ******************************************************* */

#define sync_same_no_support    "'sync_same' is not supported           "
#define system_error            "C-lib function 'system' fails          "

#define signal_error            "cannot set signal handling             "
#define reset_signal_error      "cannot reset signal handling           "

/* ***** forward declarations ********************************************* */

static void     make_cmd_string ( char          * comstring ,
								  tsp00_ExecMode   mode ,
								  tsp00_Int4        cmd_str_len );

/* ***** macro definitions ************************************************ */
#define  ERR_SQLEXEC_SET_SIGNAL                       11451, ERR_TYPE,"SQL_EXEC","signal set err, %s"
#define  ERR_SQLEXEC_RESET_SIGNAL                     11451, ERR_TYPE,"SQL_EXEC","signal reset err, %s"
#define  ERR_SQLEXEC_SYSTEM_ERROR                     11451, ERR_TYPE,"SQL_EXEC","system error, %s"

/* ************************************************************************ */
/*==========================================================================*/

 
#undef MF__
#define MF__ MOD__"set_signal"
static  int     set_signal ( )
{
	tsp00_Bool         do_again ;

	DBGIN;

	/* ***** assign SIG_DFL to signal SIGCLD.           ***** */
	/* ***** You cannot assign SIG_IGN to that signal.  ***** */
	/* ***** It will be crash because the invoked shell ***** */
	/* ***** (system do that) only expects SIG_DFL.     ***** */
	/* ***** If SIG_IGN occurs you will get errno 10 by ***** */
	/* ***** calling "system".                          ***** */

	do_again = FALSE ;
	do
	{
		if ((long)( oldsigcld  = signal ( SIGCLD  , SIG_DFL )) < 0 )
		{
			if  (  errno == EINTR )
				do_again = TRUE ;
			else
			{
				MSGD (( ERR_SQLEXEC_SET_SIGNAL, sqlerrs() )) ;
							    
				DBG1 (( MF__,"error, %s\n", sqlerrs() ));
				return ( -1 ) ;
			}
		}
	}
	while ( do_again ) ;

    DBGOUT;

	return ( 0 ) ;
}

 
/*==========================================================================*/
 
#undef MF__
#define MF__ MOD__"reset_signal"
static  int    reset_signal ( )
{
	tsp00_Bool         do_again ;


	DBGIN;

	/* ***** assign oldsigcld for signal SIGCLD ***** */

	do_again = FALSE ;
	do
	{
		if ( ( long ) ( signal ( SIGCLD  , oldsigcld ) ) < 0 )
			if  (  errno == EINTR )
				do_again = TRUE ;
			else
			{
				MSGD (( ERR_SQLEXEC_RESET_SIGNAL, sqlerrs() ));
				DBG1 (( MF__,"error, %s\n", sqlerrs() ))
				return ( - 1 ) ;
			}
	}
	while ( do_again ) ;

    DBGOUT;

	return ( 0 ) ;
}
 
#undef MF__
#define MF__ MOD__"sqlexec"
void sqlexec    ( 
tsp00_ExecLine      command     , /* arrays always delivered by reference */
tsp00_ExecMode     mode ,
tsp00_ExecReturn   * exec_ret ,
tsp00_ErrText         err_text ,    /* arrays always delivered by reference */
tsp00_Int2          * commandresult )
{
	exec_line_3     local_command_string ;
	tsp00_Bool         do_again ;
	int             rc ;

	DBGIN;
	DBG1 (( MF__,"command                      = %.132s\n",
				 command ));
	DBG1 (( MF__,"mode                         = %d\n", mode ));


	/* ***** initialize return parameters ***** */

	* commandresult = 0 ;
	SAPDB_memset ( err_text , ' ' , MAXERRORTEXT ) ;
	* exec_ret = E_OK ;


	/* ***** initialize local vars ***** */

	SAPDB_memset ( local_command_string , ' ' , MX_EXECLINE + 3  ) ;

	/* ***** only "SYNC_NEW_SESSION" and "ASYNC" are supported ***** */

	if  ( mode != SYNC_SAME_SESSION )
	{

		/* ***** initialize extern vars ***** */

		sql01_vt_clear = 1 ;


		/* ***** extract progname and args and        ***** */
		/* ***** make a cmommand string for "system"  ***** */

		(void) strncpy ( local_command_string , (char *)command , MX_EXECLINE ) ;
		make_cmd_string ( local_command_string , mode , MX_EXECLINE );

		/* ***** execute the command by using the C-lib   ***** */
		/* ***** function "system"                        ***** */

		do_again = FALSE ;
		do
		{
			/* ***** assign SIG_DFL or         ***** */
			/* ***** SIG_IGN for signal SIGCLD ***** */

			rc =  set_signal ( ) ;

			DBG1 (( MF__,"set_signal, rc = %d\n", rc ));

			if ( rc < 0 )
			{
			       * exec_ret = E_NOTOK ;
			       SAPDB_memcpy ( err_text , signal_error ,
				      MAXERRORTEXT ) ;
			       return ;
			}


			if ( ( rc = system ( local_command_string ) ) < 0 )
			{
				DBG1 (( MF__,"system,                   rc = %d\n", rc ));
				if  (  errno == EINTR )
					do_again = TRUE ;
				else
				{
					MSGD (( ERR_SQLEXEC_SYSTEM_ERROR, sqlerrs() )) ;
					* exec_ret = E_NOTOK ;
					SAPDB_memcpy ( err_text , system_error ,
					       MAXERRORTEXT ) ;
					return ;
				}
			}

			/* ***** reset signals ***** */

			if (  reset_signal ( ) < 0 )
			{
			       * exec_ret = E_NOTOK ;
			       SAPDB_memcpy ( err_text , reset_signal_error ,
				      MAXERRORTEXT ) ;
			       return ;
			}
		}
		while ( do_again ) ;

		DBG1 (( MF__,"system returncode,        rc = %d\n", rc ));


		/* ***** sytem always returns a value :     ***** */
		/* ***** > 0 if something happens           ***** */
		/* *****   0 if the process works in the    ***** */
		/* *****     background or nothing happens  ***** */
		/* ***** < 0 if system fails                ***** */

		* commandresult = ( tsp00_Int2 ) rc ;

	}
	else
	{
		* exec_ret = E_NO_SYNC_SAME ;
		SAPDB_memcpy ( err_text , sync_same_no_support , MAXERRORTEXT );
	}
}
 
/*==========================================================================*/
 
#undef MF__
#define MF__ MOD__"make_cmd_string"
static void     make_cmd_string ( 
char            * comstring ,
tsp00_ExecMode   mode ,
tsp00_Int4        cmd_str_len )
{
	int             len ;
	int             ind ;
	tsp00_Bool         no_ampersand ;

	DBGIN;

	/* ***** look for string's end ***** */

	DBG1 (( MF__,"make_cmd_string : comstring  ( 1 )             = %.132s \n",
				 comstring ))

	for ( ind = cmd_str_len - 1 ;
	      ( comstring [ ind ] == ' ' ) && ( ind >= 0 ) ; ind -- ) ;


	/* ***** string must be terminate with \0 ***** */

	if ( ( ind == 0 ) || ( ind == cmd_str_len ) )
		comstring [ ind ] = '\0' ;
	else
		comstring [ ind + 1 ] = '\0' ;

	DBG1 (( MF__,"make_cmd_string : ind                          = %d \n", ind ))
	DBG1 (( MF__,"make_cmd_string : comstring  ( 2 )             = %s \n",
				 comstring ))


	/* ***** looking for "&" at the string's end ( os specific ) ***** */

	no_ampersand = FALSE ;
	len = strlen ( comstring ) ;

	DBG1 (( MF__,"make_cmd_string : no_ampersand ( 1 )           = %d \n",
				 no_ampersand ))

	if ( comstring [ len - 1 ] != '&' )
		no_ampersand = TRUE ;

	DBG1 (( MF__,"make_cmd_string : no_ampersand ( 2 )           = %d \n",
				 no_ampersand ))
	DBG1 (( MF__,"make_cmd_string : len                          = %d \n",
				 len      ))
	DBG1 (( MF__,"make_cmd_string : comstring [ len - 1 ]        = %c \n",
				 comstring [ len - 1 ] ))


	/* ***** append a ' &' at the string's end ***** */
	/* ***** if mode = ASYNC  and no ampersand ***** */
	/* ***** is found at the string's end      ***** */

	if ( ( mode == ASYNC ) && ( no_ampersand ) )
	{
		(void) strncat ( comstring , " &" , 3 ) ;

		DBG1 (( MF__,"make_cmd_string : comstring  ( 3 ) with &      = %s\n",
					 comstring ))
	}

    DBGOUT;
}
