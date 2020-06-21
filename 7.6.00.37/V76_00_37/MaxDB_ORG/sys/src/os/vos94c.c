/*!
  @file           vos94c.c
  @author         RaymondR
  @brief          NT Server Configuration
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
*/




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "heo05.h"
#include "gos94.h"

#include           <stdarg.h>

/*
 *  DEFINES
 */
#define MOD__  "VOS94CC : "
#define MF__   MOD__"UNDEFINED"

#define OPTION_STRING             "d:raun:R:qstT:GgA:P:"



/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */
extern int        sql80_OptInd;
extern int        sql80_OptErr;
extern char*      sql80_OptArg;


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

INT main (INT     argc, CHAR   *argv[])
  {
  #undef  MF__
  #define MF__ MOD__"main"
  INT                   OptionChar;
  PSZ                   pszDBRoot         = NULL;
  APIRET                rc                = NO_ERROR;
  PSZ                   pszServerDB       = NULL;
  BOOL                  fCommandError     = FALSE;
  BOOL                  fAutoStart        = FALSE;
  BOOL                  fAddGroup         = FALSE;
  BOOL                  fDelGroup         = FALSE;
  ULONG                 ulRequest         = CREATE_SAPDB_SERVICE;
  PCHAR                 pszNodeName       = NULL;
  ULONG                 ulKernelType      = KERNEL_TYPE_FAST;
  PCHAR                 pszAccountName    = NULL;
  PCHAR                 pszPassword       = NULL;
  PSZ                   pszParameter      = NULL;
  PSZ                   pszValue          = NULL;
  PSZ                   pszKernelName     = NULL;

  DBGIN;
  //
  //
  // --- check the platform id first
  //
  //
  if ( sql02_get_platform_id() != VER_PLATFORM_WIN32_NT )
    {
    // --- Windows 95 - nothing to do !!!!!!!
    DBGOUT;
    return ( NO_ERROR );
    }

  while ( (OptionChar = sql80_GetOpt ( argc, argv, OPTION_STRING )) != -1 )
    {
      switch ( OptionChar )
        {
      case 'd' :
        pszServerDB   = sql80_OptArg;
        break;
      case 'G' :
        if ( fDelGroup )
          fCommandError = TRUE;
        else        
          fAddGroup     = TRUE;
        break;
      case 'g' :
        if ( fAddGroup )
          fCommandError = TRUE;
        else        
          fDelGroup     = TRUE;
        break;
      case 'n' :
        pszNodeName   = sql80_OptArg;
        break;
      case 'R' :
        pszDBRoot     = sql80_OptArg;
        break;
      case 'r' :
        ulRequest     = REMOVE_SAPDB_SERVICE;
        break;
      case 'a' :
        fAutoStart    = TRUE;
        break;
      case 'u' :
        ulRequest     = UPDATE_SAPDB_SERVICE;
        break;
      case 'A' :
        pszAccountName = sql80_OptArg;
        pszPassword    = strrchr ( sql80_OptArg, ',' );
        
        if ( pszPassword != NULL )
        {
          pszPassword[0] = '\0';
          pszPassword++;
        }
        else
          fCommandError = TRUE;
        break;
      case 'P' :
        ulRequest      = UPDATE_SAPDB_SERVICE;
        pszParameter   = sql80_OptArg;
        pszValue       = strrchr ( sql80_OptArg, ',' );
        
        if ( pszValue != NULL )
        {
          pszValue[0] = '\0';
          pszValue++;
        }
        else
          fCommandError = TRUE;
        break;
      case 'T' :
        ulKernelType  = KERNEL_TYPE_TEST;
        pszKernelName = sql80_OptArg;
        break;
      case 't' :
        ulKernelType  = KERNEL_TYPE_TEST;
        break;
      case 'q' :
        ulKernelType  = KERNEL_TYPE_QUICK;
        break;
      case 's' :
        ulKernelType  = KERNEL_TYPE_SLOW;
        break;
      default :
        fCommandError = TRUE;
        break;
      }
    }

  if ( (argc - sql80_OptInd) || (fCommandError == TRUE) )
    {
    printf (SERVICE_USAGE);
    EXITPROCESS(0);
    }

  if ( fAddGroup )
    sql03c_add_SAPDB_operators_group ();
  else if ( fDelGroup )
    sql03c_del_SAPDB_operators_group ();
  else
  {
    if ( pszServerDB == NULL)
      {
      if ( sql01c_get_serverdb ( &pszServerDB  ) == FALSE )
        {
        printf (SERVICE_USAGE);
        DBGOUT;
        EXITPROCESS(2);
        }
      }

    if ( pszDBRoot == NULL )
      {
      /*
       * get DBROOT from Environment
       */
      if ( sql01c_get_dbroot (&pszDBRoot) == FALSE )
        {
        MSGCD ((ERR_DBROOT_NOT_SET));
        DBGOUT;
        EXITPROCESS(2);
        }
      }

    sql94_service_entry ( ulRequest, ulKernelType, fAutoStart,
                          pszNodeName, pszDBRoot, pszServerDB, 
		            				  pszAccountName, pszPassword,
						              pszParameter, pszValue, pszKernelName, NULL );
  }

  DBGOUT;
  EXITPROCESS(0);
  return 0;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*
 * =============================== END ========================================
 */
