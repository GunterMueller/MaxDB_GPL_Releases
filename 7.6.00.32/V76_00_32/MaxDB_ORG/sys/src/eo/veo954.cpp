/*!
  @file           veo954.c
  @author         JoergM
  @special area   application startup program
  @brief          special tools to determine special installation apthes
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

#include "geo002.h"
#include "heo95.h"
#include "heo46.h"
#include "gsp09.h"
#include "RunTime/RTE_GetDBRootOfDB.h"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"

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
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

#define PRINT_RTE_ERROR(RteError)\
    { fprintf ( stderr, "OS_ERROR  %d: %s\n" , RteError.OsErrCode,  RteError.OsErrText);\
      fprintf ( stderr, "RTE_ERROR %d: %s\n" , RteError.RteErrCode, RteError.RteErrText);\
	}

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*-----------------------------------------------------------*/
int eo954PrintIndepDataPath ( PROG_ID *PrintIndepDataPath, 
			           	      int      num_args, 
				              char   **args )
/*-----------------------------------------------------------*/
{
  tsp00_Pathc    IndepDataPath ;
  tsp01_RteError RteError ;
  int            call_ret ;

  call_ret = sqlGetIndependentDataPath(IndepDataPath,0,&RteError) ? TRUE : NOTOK ;
  if ( call_ret != NOTOK ) 
    printf ("%-20s: %s\n", "IndepData",IndepDataPath.asCharp() );
  else
    PRINT_RTE_ERROR(RteError) ;

  return ( call_ret ) ;
}
 
/*-----------------------------------------------------------*/
int eo954PrintIndepProgramsPath ( PROG_ID *PrintIndepProgramsPath, 
			           	          int      num_args, 
				                  char   **args )
/*-----------------------------------------------------------*/
{
  tsp00_Pathc    IndepProgramsPath ;
  tsp01_RteError RteError ;
  int            call_ret ;

  call_ret = sqlGetIndependentProgramsPath(IndepProgramsPath,0,&RteError) ? TRUE : NOTOK ;
  if ( call_ret != NOTOK ) 
    printf ("%-20s: %s\n", "IndepPrograms",IndepProgramsPath.asCharp() );
  else
    PRINT_RTE_ERROR(RteError) ;

  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int eo954PrintDBRootOfDB ( PROG_ID *PrintDBRootOfDB,
			           	   int      num_args, 
				           char   **args )
/*-----------------------------------------------------------*/
{
  tsp00_Pathc    DBRootOfDB ;
  int            call_ret ;

  if ( num_args == 1 )
  {
    call_ret = RTE_GetDBRootOfDB(args[0], DBRootOfDB, sizeof(DBRootOfDB)) ? TRUE : NOTOK ;
    if ( call_ret != NOTOK ) 
	{
	  tsp00_Pathc KernelVersion ;
      tsp00_ErrText ErrText;

      tsp00_DbNamec DBNamec;
      DBNamec.rawAssign(args[0]);

      tsp00_C256c   KernelVersionParamNamec;
      KernelVersionParamNamec.rawAssign(PAN_PARAM_KERNELVERSION);

      printf ("%-20s: %s\n", "InstallationPath",DBRootOfDB.asCharp() );
      if ( RTEConf_ParamaterAccessGetParam ( DBNamec , KernelVersionParamNamec, KernelVersion, ErrText ))
        printf ("%-20s: %s\n", "Kernelversion",KernelVersion.asCharp() );
	}
    else
      printf("Installationpath of serverdb <%s> not found!\n", args[0]);
  }
  else
  {
    printf("Usage: %s <SAP DB / liveCache name >\n", PrintDBRootOfDB->progname);
    call_ret = NOTOK ;
  }

  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int eo954PrintRundirOfDB ( PROG_ID *PrintRundirOfDB,
			           	   int      num_args, 
				           char   **args )
/*-----------------------------------------------------------*/
{
  SAPDB_UTF8              RundirOfDB[RTECONF_MAXSTRINGLENGTH+1] ;
  int                     call_ret ;

  if ( num_args == 1 )
  {
    RTEConf_Parameter parameterSet( args[0] );
    Msg_List err;

    call_ret = parameterSet.GetRundirectoryAbsolutePath ( &RundirOfDB[0],err )
        ? TRUE : NOTOK ;
    if ( call_ret != NOTOK ) 
      printf ("%-20s: %s\n","Rundirectory", RundirOfDB );
    else
      RTE_Message(err);
  }
  else
  {
    printf("Usage: %s <SAP DB / liveCache name >\n", PrintRundirOfDB->progname);
    call_ret = NOTOK ;
  }

  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int eo954PrintInstInfo ( PROG_ID *PrintInstInfo,
			             int      num_args, 
				         char   **args )
/*-----------------------------------------------------------*/
{


  int            call_ret ;

  call_ret = eo954PrintIndepDataPath    ( PrintInstInfo, num_args, args ) ;
  call_ret = eo954PrintIndepProgramsPath( PrintInstInfo, num_args, args ) ;

  if ( num_args == 1 )
  {
    call_ret = eo954PrintDBRootOfDB ( PrintInstInfo, num_args, args ) ;
    call_ret = eo954PrintRundirOfDB ( PrintInstInfo, num_args, args ) ;
  }
  return ( call_ret ) ;
}


/*-----------------------------------------------------------*/
int eo954PrintSysRC ( PROG_ID *PrintSysRC,
  		              int      num_args, 
				      char   **args )
/*-----------------------------------------------------------*/
{


  int            call_ret = TRUE;

  if ( num_args == 1 )
  {
    char ErrText [ 100 ] ;
    int ErrCode = atoi ( args[0] ) ;

    eo46_system_error_text(ErrCode, ErrText, sizeof (ErrText) ) ;
    printf("\nErrortext for errorcode %d:\n'%s'\n\n", ErrCode, ErrText ) ;
  }
  else
    printf("Usage: <%s> <Error Number>\n", PrintSysRC->progid ) ;

  return ( call_ret ) ;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
