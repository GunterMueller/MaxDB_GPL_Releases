/*!
  @file           vos24u.c
  @author         RaymondR
  @brief          sqlexec
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




//
// INCLUDE FILES
//
#include "gos00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo007_2.h"

#include <lm.h>
#include <lmat.h>


//
//  DEFINES
//

#define MOD__  "VOS24UC : "
#define MF__   MOD__"UNDEFINED"

#define NET_API_DLL       "NETAPI32.DLL"

//
//  MACROS
//
#define MK_PA(f)       (NET_API_FUNCTION *lp##f)


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
// EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
// LOCAL VARIABLES
//
static NET_API_STATUS MK_PA(NetScheduleJobAdd)( LPWSTR  Servername,
                                                LPBYTE  Buffer,
                                                LPDWORD JobId );
static HINSTANCE      hinst = NULL;


//
// LOCAL FUNCTION PROTOTYPES
//



//
// ========================== GLOBAL FUNCTIONS ================================
//


VOID  sqlat ( INT4               i4hour,
              INT4               i4minute,
              BOOLEAN            fNow,
              tsp00_ExecArgLine  pCommand,
              tsp00_ExecReturn   *peResult,
              ERRORTEXT          ErrMsg )
  {
  #undef  MF__
  #define MF__ MOD__"sqlat"
  #if defined(_WIN32)
   LONG                  rc;
   DWORD                 JobId;
   AT_INFO               AtInfo;
   ULONG                 ulTime;
   CHAR                  szCommand[sizeof(tsp00_ExecArgLine) + 1];
   WCHAR                 wszCommand[256];
   CHAR                  szServiceDisplayName[128];
   CHAR                  szErrMsg[sizeof(ERRORTEXT) + 1];
   SYSTEMTIME            DateTime;
   ULONG                 ulSeconds = 0;
   REG_ENTRY_REC         RegistryEntries[1];
  #endif

  DBGIN;

  if ( sql02_get_platform_id () != VER_PLATFORM_WIN32_NT )
    {
    sql46c_build_error_string ( ErrMsg, ERRMSG_FUNCTION_NOT_AVAIL, 0 );
    *peResult = ex_notok;
    DBGOUT;
    return;
    }

  // --- load this net function by 'LoadLibrary'
  //     because there is no 'NETAPI32.DLL' available
  //     on Windows95 systems
  //
  hinst = LoadLibrary( NET_API_DLL );

  if ( hinst != NULL )
    lpNetScheduleJobAdd = (VOID*)GetProcAddress( hinst, "NetScheduleJobAdd" );

  if (( hinst == NULL ) || ( lpNetScheduleJobAdd == NULL ))
    {
    rc = GetLastError();
    sql46c_build_error_string ( ErrMsg, ERRMSG_STARTING_FUNC_FROM_DLL, rc );
    MSGD(( ERR_STARTING_FUNC_FOM_DLL, "NetScheduleJobAdd", NET_API_DLL, rc ));
    *peResult = ex_notok;
    DBGOUT;
    return;
    }

  if ( !pCommand || !*pCommand )
    {
    *peResult = ex_notok;

    sql46c_build_error_string ( ErrMsg, ERRMSG_NO_CMD, 0 );
    DBG1  (( MF__,"No Command to execute!" ));
    MSGD  (( ERR_NO_CMD ));

    DBGOUT;
    return;
    }

  #if defined(_WIN32)
   if (( (ULONG)i4hour == 24 ) && ( (ULONG)i4minute == 0 ))
     {
     i4hour   = 0;
     i4minute = 0;
     }

   if ( (ULONG)i4hour > 23)
    {
    sql46c_build_error_string ( ErrMsg, ERRMSG_WRONG_TIME_VALUE, 0 );
    MSGD (( ERR_WRONG_TIME_VALUE, i4hour, i4minute ));

    *peResult = ex_notok;
    DBGOUT;
    return;
    }

   if ( (ULONG)i4minute > 59 )
    {
    sql46c_build_error_string ( ErrMsg, ERRMSG_WRONG_TIME_VALUE, 0 );
    MSGD (( ERR_WRONG_TIME_VALUE, i4hour, i4minute ));

    *peResult = ex_notok;
    DBGOUT;
    return;
    }

   if ( fNow )
    {
    GetLocalTime(&DateTime);

    ulSeconds = DateTime.wSecond;
    i4minute += DateTime.wMinute;
    i4hour   += DateTime.wHour;

    if ( i4minute > 59 )
      {
      i4minute -= 60;
      i4hour++;
      }

    if ( i4hour > 23 )
      i4hour -= 24;
    }

   ulTime  = (( (ULONG)i4hour   * 3600 ) +
              ( (ULONG)i4minute * 60   ) + ulSeconds  ) * 1000;

   eo46PtoC ( szCommand, pCommand, sizeof(tsp00_ExecArgLine));
   MultiByteToWideChar( CP_ACP, 0, szCommand,
                        (ULONG)strlen(szCommand) + 1,
                        wszCommand, sizeof(wszCommand) );

   AtInfo.JobTime       = ulTime;
   AtInfo.DaysOfMonth   = 0;
   AtInfo.DaysOfWeek    = 0;
   AtInfo.Flags         = JOB_NONINTERACTIVE;
   AtInfo.Command       = wszCommand;

   rc = lpNetScheduleJobAdd ( NULL, (LPBYTE)&AtInfo, &JobId  );

   if ( rc != NO_ERROR )
     {
     if ( rc == NERR_ServiceNotInstalled )
       {
       RegistryEntries[0].pszValueName = "DisplayName";
       RegistryEntries[0].pValue       = szServiceDisplayName;
       RegistryEntries[0].ulValueSize  = sizeof(szServiceDisplayName) - 1;
       RegistryEntries[0].ulValueType  = REG_SZ;

       if ( sql50_reg_get_service_values( NULL, "Schedule", 1,
                                          RegistryEntries) != NO_ERROR )
         {
         strcpy ( szServiceDisplayName, "schedule" );
         }

       sprintf ( szErrMsg, ERRMSG_SCHEDULE_SERVICE_NOT_STARTED,
                 szServiceDisplayName );
       sql46c_build_error_string ( ErrMsg, szErrMsg, 0 );
       }
     else
       sql46c_build_error_string ( ErrMsg, ERRMSG_CANNOT_EXECUTE_CMD, rc );

     DBG1  (( MF__, ERRMSG_CANNOT_EXECUTE_CMD, rc ));
     MSGD (( ERR_CANNOT_EXECUTE_CMD, rc ));

     *peResult = ex_notok;
     DBGOUT;
     return;
     }

   *peResult = ex_ok;
  #else
   *peResult = ex_notok;
  #endif

  DBGOUT;
  return;
  }


//
// ========================== LOCAL FUNCTIONS =================================
//


//
// =============================== END ========================================
//
