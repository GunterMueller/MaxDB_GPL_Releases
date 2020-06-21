/*!
  @file           vos12u.c
  @author         RaymondR
  @brief          SQLARGS
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
//  MODULE - SQLARGS
//


//
//  INCLUDE FILES
//

#include "gos00.h"
#include "heo46.h"
#include "hsp78_0.h"
#include "hsp83.h"
#include "geo13.h"
#include "heo13.h"
#include "hos13u.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "heo12.h"

//
//  DEFINES
//
#define MOD__  "VOS12UC : "
#define MF__   MOD__"UNDEFINED"


// --- Components defines -----------------------------------------------------

#define COMP_PRECOMPILER        0
#define COMP_PC_RUNTIME         1
#define COMP_EASY               2
#define COMP_QUERY              3
#define COMP_DIALOG             4
#define COMP_APPL               5
#define COMP_LOAD               6
#define COMP_UTILITY            7
#define COMP_AUDIT              8
#define COMP_USER               9
#define COMP_PARAM              10


#define MAX_IDENT_CNT           3
#define OPT_AND_ARG_CNT         25

#define PRECOMPILER_OPTION_STRING                               \
       "CORTXYlcoVWswhiLqpezu:b:d:n:U:r:y:F:P:I:t:m:S:D:M:H:E:G:x:"

#define PRECOMPILER_RUNTIME_OPTION_STRING                       \
       "OTXNWRZfu:d:n:U:y:S:F:I:t:B:Y:L:x:"

#define DIALOG_COMPONENT_OPTION_STRING                          \
       "APVLTcsu:d:n:U:r:b:R:B:O:e:i:S:y:I:t:C:x:"

#define AUDIT_OPTION_STRING                                     \
       "Vu:d:n:U:S:I:t:y:c:l:o:x:N:"

#define ARGL_OPTION_STRING                                      \
        "u:d:r:b:"

#define USERX_OPTION_STRING                                     \
        "Vu:b:"

// Errortexts


//
//  MACROS
//
#ifdef DEBUG_RTE
static char _TmpStr[500 + 1] ;

#define _PToC(_Str) \
           (eo46PtoC( _TmpStr, _Str, min(sizeof(_Str),500)), _TmpStr)
#endif

#define PUTBIT(b,i)             \
        { b [ ( unsigned ) i / 8 ] |=  ( 1 << (( unsigned ) i % 8 )); }
#define CLRBIT(b,i)             \
        { b [ ( unsigned ) i / 8 ] &= ~( 1 << (( unsigned ) i % 8 )); }
#define TSTBIT(b,i)             \
        ( b [ ( unsigned ) i / 8 ] &   ( 1 << (( unsigned ) i % 8 ))  )


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//

typedef struct {
    PSZ                 pszIdent      [ MAX_IDENT_CNT ] ;
    LONG                lIdentLen     [ MAX_IDENT_CNT ] ;
    BOOL                bIdentToUpper [ MAX_IDENT_CNT ] ;
} CTU_REC;             // --- convert to upper structure

typedef  CTU_REC       *PCTU_REC;

typedef struct sql_arg_line_record {
    tsp00_KnlIdentifier     username;
    tsp00_Pw               password;
    SQL_DBNAME             dbname;
    C12                    cmd;
    tsp00_VFilename        filename;
    C40                    aname;
    tsp00_Line             arguments;
} ARG_LINE_REC;
typedef ARG_LINE_REC*           PARG_LINE_REC;

//
//  EXTERNAL VARIABLES
//
extern int        sql80_OptInd;
extern int        sql80_OptErr;
extern char*      sql80_OptArg;



//
//  EXPORTED VARIABLES
//
#if defined(_WIN32)
  int     WinArgc;                // imported and set by ptoclib:_pmain.c
  char**  WinArgv;
#else
  int     os2_argc = 0;           // exported for ptoc
  char**  os2_argv = NULL;        // exported for ptoc
#endif


//
//  LOCAL VARIABLES
//


//
//  LOCAL FUNCTION PROTOTYPES
//

// === LOCAL FUNCTIONS FOR SQLARG3 ============================================

static VOID ArgsInit
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_xuserset       ucXuserType,
                PBOOLEAN            pbEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText     );

static VOID PrecompilerArgsInit
            (   tsp4_args_options  *prcArgsOptions  );

static VOID PrecompilerRunTimeArgsInit
            (   tsp4_args_options  *prcArgsOptions  );

static VOID DialogComponentArgsInit
            (   tsp4_args_options  *prcArgsOptions  );

static VOID AuditArgsInit
            (   tsp4_args_options  *prcArgsOptions  );

static VOID UserXArgsInit
            ( tsp4_args_options  *prcArgsOptions );

static VOID PrecompilerOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgumentCount,
                PCHAR*              apszArguments,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText     );

static VOID PrecompilerRunTimeOptionsAnalysis
            (   tsp4_xuser_record  *prxUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgc,
                PCHAR*              apszArgv,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText     );

static VOID DialogComponentOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgumentCount,
                PCHAR*              apszArguments,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText     );

static VOID UserXOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgumentCount,
                PCHAR*              apszArguments,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText         );

static VOID AuditOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgumentCount,
                PCHAR*              apszArguments,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText     );


static VOID MakeArgs
            (   INT                 nOptionChar,
                tsp4_component      eComponent,
                tsp4_xuser_record  *prcUserParams,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgc,
                PCHAR*              apszArgv,
                BOOLEAN             bEnvDBNameFound,
                ERRORTEXT           acErrorText,
                PBOOLEAN            pbOk            );

static VOID MakeMyArgs
            (   INT                 nOptionChar,
                tsp4_component      eComponent,
                tsp4_xuser_record  *prcUserParams,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgc,
                PCHAR*              apszArgv,
                BOOLEAN             bEnvDBNameFound,
                ERRORTEXT           acErrorText,
                PBOOLEAN            pbOk            );

static VOID MakeUserPassword
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText     );

static VOID MakeDBId
            (   tsp4_xuser_record  *prcUserParams,
                tsp4_xuserset       ucXuserType     );

static VOID MakeDBNode
            (   tsp4_xuser_record  *prcUserParams,
                tsp4_xuserset       ucXuserType     );

static VOID MakeDBLang
            (   tsp4_xuser_record  *prcUserParams,
                tsp4_xuserset       ucXuserType     );

static VOID MakeUserKey
            (   tsp4_xuser_record  *prcUserParams,
                tsp4_xuserset       ucXuserType,
                BOOLEAN             bEnvDBNameFound,
                ERRORTEXT           acErrorText,
                PBOOLEAN            pbOk            );

static VOID MakeSqlMode
            (   tsp4_xuser_record  *prcUserParams,
                tsp4_xuserset       ucXuserType     );

static VOID MakeIsolation
            (   tsp4_xuser_record  *prcUserParams ,
                tsp4_xuserset       ucXuserType     );

static VOID MakeTimeOut
            (   tsp4_xuser_record  *prcUserParams,
                tsp4_xuserset       ucXuserType     );

static VOID MakeCacheLimit
            (   tsp4_xuser_record  *prcUserParams,
                tsp4_xuserset       ucXuserType     );

static VOID MakePrecompilerInputFn
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerTraceFn
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerProgFn
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerMargin
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerDateTime
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerSysTpMon
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerMode
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerCAnsi
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerUnicode
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerPacketSize
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerRunTimeTraceFn
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerRunTimeMFetch
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerRunTimeStmtCnt
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakePrecompilerRunTimeLimit
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeDialogComponentRFn
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeDialogComponentBFn
            (   tsp4_args_options  *prcArgsOptions );

static VOID MakeDialogComponentObj
            (   tsp4_args_options  *prcArgsOptions,
                tsp4_command_mode   OptCommMode,
                INT                 nMaxObjNames,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText     );

static VOID MakeDialogComponentCodeType
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeDialogComponentExport
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeDialogComponentImport
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeAuditCntrl
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeAuditLogfile
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeAuditLogBlockSize
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeAuditOutput
            (   tsp4_args_options  *prcArgsOptions  );

static VOID MakeUserXComponentBFn
            (   tsp4_args_options  *prcArgsOptions  );


// === LOCAL FUNCTIONS FOR SQLARGS, SQLARGL ===================================

static VOID MakeUserPasswordLine
            (   PARG_LINE_REC       prcArgLine,
                PBOOLEAN            pbUserNameFound,
                PBOOLEAN            pbPasswordFound     );

static VOID MakeDBName
            (   PCHAR*              apszArgv,
                PARG_LINE_REC       prcArgLine,
                PBOOLEAN            pbDBNameFound       );

static VOID MakeCmd
            (   PCHAR               pszCmdType,
                PARG_LINE_REC       prcArgLine,
                PBOOLEAN            pbCmdFound,
                PBOOLEAN            pbFileNameFound     );

static VOID MakeArgLine
            (   INT                 nArgc,
                PCHAR*              apszArgv,
                PARG_LINE_REC       prcArgLine,
                PBOOLEAN            pbArgumentsFound,
                PBOOLEAN            pbCmdFound,
                PBOOLEAN            pbFileNameFound,
                INT                 nOptionChar         );

static VOID MakeLine
            (   PCHAR               pszArgIn,
                PINT                pnArgInIndex,
                INT                 nLineLength,
                INT                 nSeparator,
                PINT                pnArgOutIndex,
                PCHAR               pszArgOut           );

static VOID BuildArgLineFromStruct
            (   PCHAR               psArgs,
                PARG_LINE_REC       prcArgLineRec,
                BOOLEAN             bUserNameFound,
                BOOLEAN             bPasswordFound,
                BOOLEAN             bDBNameFound,
                BOOLEAN             bCmdFound,
                BOOLEAN             bFileNameFound,
                BOOLEAN             bArgumentsFound     );


// === OTHER LOCAL FUNCTIONS ==================================================


static VOID MoveToUpper         (   PCHAR       psString,
                                    INT         nStringSize          );

static VOID CheckIdents         (   PCTU_REC    pCtU,
                                    CHAR        cSep,
                                    LONG        lMaxIdent,
                                    PBOOLEAN    pbOk,
                                    ERRORTEXT   acErrorText          );
static INT  StrLen              (   CHAR*       psStr,
                                    INT         nLength              );

static INT  StrChr              (   CHAR*       pszStr,
                                    INT         nCharacter,
                                    INT         nBegin,
                                    INT         nEnd                 );


//
// ========================== GLOBAL FUNCTIONS ================================
//

VOID    sqlarg3 (   tsp4_xuser_record  *prcUserParams,
                    tsp00_Pw            acPassword,
                    tsp4_args_options  *prcArgsOptions,
                    tsp4_xuserset       aucXuserType,
                    ERRORTEXT           acErrorText,
                    PBOOLEAN            pbOk            )
{
    #undef  MF__
    #define MF__ MOD__"sqlarg3"


    tsp4_component eComponent;
    INT            nArgc;
    PCHAR*         apszArgv;
    BOOLEAN        bEnvDBNameFound;


    DBGIN;

    // initialize some of the return parameter
    ArgsInit ( prcUserParams, acPassword,
               aucXuserType, &bEnvDBNameFound, pbOk, acErrorText );

    sql80_OptInd  = 1;
    sql80_OptErr  = 0;
    sql80_OptArg  = NULL;

    // look for the component within the option record
    // it must be in the correct range
    eComponent = prcArgsOptions -> opt_component;

    if ((( INT ) eComponent < COMP_PRECOMPILER ) ||
        (( INT ) eComponent > COMP_PARAM       )) {

        *pbOk = FALSE;

        DBG1 (( MF__, ERRMSG_NO_COMP_SPECIFIED ));
        MSGD  (( ERR_NO_COMP_SPECIFIED ));
        sql46c_build_error_string ( acErrorText, ERRMSG_NO_COMP_SPECIFIED, 0 );

        DBGOUT;
        return;
    }


    #if defined(_WIN32)
      nArgc    = WinArgc;
      apszArgv = WinArgv;
    #else
      // return of the addresses of nArgc and apszArgv
      // ( not the system argv and argc !!!! )
      nArgc    = os2_argc;   // exported from ptoc
      apszArgv = os2_argv;   // exported from ptoc
    #endif

    DBG3 (( MF__, "nArgc = %d", nArgc ));
    if ( nArgc > 0 ) {
        DBG3 (( MF__, "apszArgv [ 0 ] = \"%s\"", apszArgv [ 0 ] ));
        DBG3 (( MF__, "apszArgv [ nArgc - 1 ] = \"%s\"",
                       apszArgv [ nArgc - 1 ] ));
    }


    // jump to the component's option analysis

    DBG3 (( MF__, "Component = %d", eComponent ));

    switch ( eComponent ) {
        case COMP_PRECOMPILER :
            PrecompilerArgsInit ( prcArgsOptions );
            PrecompilerOptionsAnalysis ( prcUserParams, acPassword,
                                         prcArgsOptions, aucXuserType,
                                         nArgc, apszArgv,
                                         bEnvDBNameFound, pbOk, acErrorText );
        break;

        case COMP_PC_RUNTIME :
            PrecompilerRunTimeArgsInit ( prcArgsOptions );
            PrecompilerRunTimeOptionsAnalysis ( prcUserParams, acPassword,
                                                prcArgsOptions, aucXuserType,
                                                nArgc, apszArgv,
                                                bEnvDBNameFound,
                                                pbOk, acErrorText );
        break;

        case COMP_EASY    :
        case COMP_QUERY   :
        case COMP_DIALOG  :
        case COMP_APPL    :
        case COMP_LOAD    :
        case COMP_UTILITY :
        case COMP_PARAM   :
            DialogComponentArgsInit ( prcArgsOptions );
            DialogComponentOptionsAnalysis ( prcUserParams, acPassword,
                                             prcArgsOptions, aucXuserType,
                                             nArgc, apszArgv, bEnvDBNameFound,
                                             pbOk, acErrorText );
        break;

        case COMP_AUDIT :
            AuditArgsInit ( prcArgsOptions );
            AuditOptionsAnalysis ( prcUserParams, acPassword,
                                   prcArgsOptions, aucXuserType,
                                   nArgc, apszArgv,
                                   bEnvDBNameFound, pbOk, acErrorText );
        break;
        case COMP_USER    :
            UserXArgsInit ( prcArgsOptions );
            UserXOptionsAnalysis ( prcUserParams, acPassword,
                                   prcArgsOptions, aucXuserType,
                                   nArgc, apszArgv,
                                   bEnvDBNameFound, pbOk, acErrorText );
        break;
    }

    DBG3 (( MF__, "Password = \"%s\"", _PToC( acPassword )));
    DBG3 (( MF__, "prcUserParams -> xu_serverdb = \"%s\"",
                   _PToC( prcUserParams -> xu_serverdb )));
    DBG3 (( MF__, "prcUserParams -> xu_dblang   = \"%s\"",
                   _PToC( prcUserParams -> xu_dblang   )));

    DBGOUT;
}

//
// ========================== LOCAL FUNCTIONS =================================
//


static VOID ArgsInit    (   tsp4_xuser_record  *prcUserParams,
                            tsp00_Pw            acPassword,
                            tsp4_xuserset       ucXuserType,
                            PBOOLEAN            pbEnvDBNameFound,
                            PBOOLEAN            pbOk,
                            ERRORTEXT           acErrorText     )
{
    #undef  MF__
    #define MF__ MOD__"ArgsInit"


    tsp4_xuser_record   rcLocalUserParams;
    APIRET              rc;
    PSZ                 pszServerDB;

    DBGIN;

    *pbOk = TRUE ;

    sql13u_init_user_params( prcUserParams );

    memset ( acPassword,  ' ',  sizeof ( tsp00_Pw     ));

    memset ( ucXuserType, '\0', sizeof ( tsp4_xuserset )) ;
    PUTBIT ( ucXuserType, sp4xu_sql_usermask );

    // --- xu_key = DEFAULT_USER, check for errors
    SAPDB_memcpy ( rcLocalUserParams.xu_key, DEFAULT_USER,
             sizeof ( rcLocalUserParams.xu_key ));

    DBG3 (( MF__, "rcLocalUserParams.xu_key = \"%s\"",
                   _PToC ( rcLocalUserParams.xu_key )));

    rc = sql13u_getuser ( &rcLocalUserParams, NULL, acErrorText );

    if ( rc == ERROR_ACCESS_DENIED )
        *pbOk = FALSE;


    if ( rc == NO_ERROR ) 
    {
      SAPDB_memcpy ( prcUserParams -> xu_user,
               rcLocalUserParams.xu_user,
               sizeof ( prcUserParams -> xu_user ));
      SAPDB_memcpy ( prcUserParams -> xu_password,
               rcLocalUserParams.xu_password,
               sizeof ( prcUserParams -> xu_password ));
      SAPDB_memcpy ( prcUserParams -> xu_sqlmode,
               rcLocalUserParams.xu_sqlmode,
               sizeof ( prcUserParams -> xu_sqlmode ));
      SAPDB_memcpy ( prcUserParams -> xu_servernode,
               rcLocalUserParams.xu_servernode,
               sizeof ( prcUserParams -> xu_servernode ));
      SAPDB_memcpy ( prcUserParams -> xu_serverdb,
               rcLocalUserParams.xu_serverdb,
               sizeof ( prcUserParams -> xu_serverdb ));
      SAPDB_memcpy ( prcUserParams -> xu_dblang,
               rcLocalUserParams.xu_dblang,
               sizeof ( prcUserParams -> xu_dblang ));

      prcUserParams -> xu_cachelimit = rcLocalUserParams.xu_cachelimit;
      prcUserParams -> xu_timeout    = rcLocalUserParams.xu_timeout;
      prcUserParams -> xu_isolation  = rcLocalUserParams.xu_isolation;

      SAPDB_memcpy ( prcUserParams -> xu_userUCS2,
               rcLocalUserParams.xu_userUCS2,
               sizeof ( prcUserParams -> xu_userUCS2 ));
      SAPDB_memcpy ( prcUserParams -> xu_passwordUCS2,
               rcLocalUserParams.xu_passwordUCS2,
               sizeof ( prcUserParams -> xu_passwordUCS2 ));

      PUTBIT ( ucXuserType, sp4xu_sql_userdefault );
      CLRBIT ( ucXuserType, sp4xu_sql_usermask );

      *pbEnvDBNameFound = FALSE ;
      DBG3 (( MF__, "DBName not used from environment" ));
    }
    else
    {
      *pbEnvDBNameFound = sql01c_get_serverdb ( &pszServerDB );
    }

    if ( *pbEnvDBNameFound ) 
    {
      PUTBIT ( ucXuserType, sp4xu_sql_serverdb );
      DBG3 (( MF__, "DBName in environment" ));
    }

    DBG3 (( MF__, "prcUserParams -> xu_key = \"%s\"",
                   _PToC ( prcUserParams -> xu_key )));

    DBG3 (( MF__, "prcUserParams -> xu_servernode = \"%s\"",
                   _PToC ( prcUserParams -> xu_servernode )));

    DBG3 (( MF__, "prcUserParams -> xu_serverdb = \"%s\"",
                   _PToC ( prcUserParams -> xu_serverdb )));

    DBG3 (( MF__, "prcUserParams -> xu_user = \"%s\"",
                   _PToC ( prcUserParams -> xu_user )));

    DBG3 (( MF__, "Password = \"%s\"",
                   _PToC ( prcUserParams -> xu_password )));

    DBG3 (( MF__, "prcUserParams -> xu_sqlmode = \"%s\"",
                   _PToC ( prcUserParams -> xu_sqlmode )));

    DBG3 (( MF__, "prcUserParams -> xu_cachelimit = %d",
                   prcUserParams -> xu_cachelimit ));

    DBG3 (( MF__, "prcUserParams -> xu_timeout = %d",
                   prcUserParams -> xu_timeout ));

    DBG3 (( MF__, "prcUserParams -> xu_isolation = %d",
                   prcUserParams -> xu_isolation ));
/*
    DBG3 (( MF__, "prcUserParams -> xu_userUCS2 = \"%s\"",
                   _PToC ( prcUserParams -> xu_userUCS2 )));

    DBG3 (( MF__, "PasswordUCS2 = \"%s\"",
                   _PToC ( prcUserParams -> xu_passwordUCS2 )));
 */
    DBGOUT;
}



/*------------------------------*/


static VOID PrecompilerArgsInit (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"PrecompilerArgsInit"


    DBGIN;

    // defaults for the option record PRECOMPILER
    memset ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulefn_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulefn_F ));

    memset ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tracefile_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tracefile_F ));

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_prognamel_F = 0;
    memset ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_progname_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_progname_F ));

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulenamel_F = 0;
    memset ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulename_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulename_F ));

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmonidl_F = 0;
    memset ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmonid_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmonid_F ));

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_begmar_F   = 0;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_endmar_F   = 0;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_prof_F     = 1;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_trace_F    = 0;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_fnind_F    = 0;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 1;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_dyn_F      = 0;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmon_F    = 0;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_mode_F     = 2;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_extern_F   = 1;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_cansi_F    = 0;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_comp_F     = 1;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_packetsize_F= 0;

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_list_F        = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_precom_F      = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_traceswitch_F = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_comment_F     = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_silent_F      = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_nowarn_F      = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_lineno_F      = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_lib_F         = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_quote_F       = TRUE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_decpoint_F    = TRUE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tabformat_F   = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_version_F     = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_help_F        = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_unicode_F     = FALSE;

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_quote_F   = '\'';
    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_decpoint_F = '.';

    memset ( prcArgsOptions->variant.C_sp4co_sql_precompiler.opt_puser_args_F, ' ',
             sizeof ( prcArgsOptions->
                      variant.C_sp4co_sql_precompiler.opt_puser_args_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID PrecompilerRunTimeArgsInit
            (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"PrecompilerRunTimeArgsInit"


    DBGIN;

    // defaults for the option record PRECOMPILER RUNTIME
    memset ( prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F,
             ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F ));

    prcArgsOptions->variant.C_sp4co_sql_pc_runtime.opt_rtrace_F      = 0;
    prcArgsOptions->variant.C_sp4co_sql_pc_runtime.opt_rprof_F       = 0;
    prcArgsOptions->variant.C_sp4co_sql_pc_runtime.opt_rmfetch_F     = 1;
    prcArgsOptions->variant.C_sp4co_sql_pc_runtime.opt_rstmt_cnt_F   = 0;
    prcArgsOptions->variant.C_sp4co_sql_pc_runtime.opt_rtime_limit_F = 0;
    prcArgsOptions->variant.C_sp4co_sql_pc_runtime.opt_rfiller_F     = 0;

    prcArgsOptions ->
    variant.C_sp4co_sql_pc_runtime.opt_rnotracetime_F = FALSE;
    prcArgsOptions ->
    variant.C_sp4co_sql_pc_runtime.opt_rtraceswitch_F = FALSE;
    prcArgsOptions ->
    variant.C_sp4co_sql_pc_runtime.opt_rnosdf_F       = FALSE;
    prcArgsOptions ->
    variant.C_sp4co_sql_pc_runtime.opt_rfiller1_F     = FALSE;

    memset ( prcArgsOptions->
               variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F, ' ',
             sizeof ( prcArgsOptions->
               variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F ));

    DBGOUT;
}



/*------------------------------*/


static VOID DialogComponentArgsInit (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"DialogComponentArgsInit"


    DBGIN;

    // defaults for the option record DIALOG COMPONENTS AND UTILITY
    memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F,
             ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F));

    memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
             ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F));

    memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F,
             ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F));

    memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
             ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F ));

    memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_parameter_F,
             ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_parameter_F ));

    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_append_F    = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_prompt_F    = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_codeonly_F  = FALSE;
    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_utility_session_F  = FALSE;

    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_none;
    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_code_F      = is_codeneutral;

    DBGOUT;
}


/*------------------------------*/


static VOID AuditArgsInit   (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"AuditArgsInit"


    DBGIN;

    // defaults for the option record AUDIT
    memset ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_control_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_control_F ));

    memset ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_logfile_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_logfile_F ));

    memset ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_output_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_output_F ));

    prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_log_blocksize_F = 8 ;
    prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_comm_mode_F = sp4cm_sql_none;

    DBGOUT;
}


/*------------------------------*/


static VOID UserXArgsInit ( tsp4_args_options  *prcArgsOptions )
{
    #undef  MF__
    #define MF__ MOD__"UserXArgsInit"


    DBGIN;

    // defaults for the option record USERX
    memset ( prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_runfile_F, ' ',
             sizeof ( prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_runfile_F ));

    prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F = sp4cm_sql_none;

    DBGOUT;
}



/*------------------------------*/


static VOID PrecompilerOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgumentCount,
                PCHAR*              apszArguments,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText         )
{
    #undef  MF__
    #define MF__ MOD__"PrecompilerOptionsAnalysis"


    BOOLEAN bMoreArgs;
    INT     nOptionChar;


    DBGIN;

    // initialize internel vars
    bMoreArgs   = TRUE;
    nOptionChar = ' ';

    DBG3 (( MF__, "nArgumentCount = %d", nArgumentCount ));

    // parameter existing ?
    if ( nArgumentCount > 1 ) {
        // get options
        while ((( nOptionChar =
                      sqlgetopt ( nArgumentCount, apszArguments,
                                      PRECOMPILER_OPTION_STRING )) != -1 ) &&
                  bMoreArgs                                                &&
                  *pbOk ) {

            DBG3 (( MF__, "Option character = %c", nOptionChar ));
            DBG3 (( MF__, "sql80_OptInd = %d", sql80_OptInd ));

            switch ( nOptionChar ) {
                // general option args
                case 'u' :
                    MakeUserPassword ( prcUserParams,
                                       acPassword, pbOk, acErrorText );
                    PUTBIT ( ucXuserType, sp4xu_sql_userparms );
                    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
                break;

                case 'd' :
                    MakeDBId ( prcUserParams, ucXuserType );
                break;

                case 'n' :
                    MakeDBNode ( prcUserParams, ucXuserType);
                break;

                case 'U' :
                    MakeUserKey ( prcUserParams, ucXuserType,
                                  bEnvDBNameFound, acErrorText, pbOk );
                break;

                case 'S' :
                    MakeSqlMode ( prcUserParams, ucXuserType );
                break;

                case 'I' :
                    MakeIsolation ( prcUserParams, ucXuserType);
                break;

                case 't' :
                    MakeTimeOut ( prcUserParams, ucXuserType );
                break;

                case 'y' :
                    MakeCacheLimit ( prcUserParams, ucXuserType );
                break;

                case 'x' :
                    MakeDBLang ( prcUserParams, ucXuserType);
                break;

                // option args with arguments

                case 'r' :
                    MakePrecompilerInputFn ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'F' :
                    MakePrecompilerTraceFn ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'P' :
                    MakePrecompilerProgFn ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'm' :
                    MakePrecompilerMargin ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'D' :
                    MakePrecompilerDateTime ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'M' :
                    MakePrecompilerSysTpMon ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'H' :
                    MakePrecompilerMode ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'E' :
                    MakePrecompilerCAnsi ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'G' :
                    MakePrecompilerUnicode ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'b' :
                    MakePrecompilerPacketSize ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                // optargs with no arguments
                case 'C' :
                    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_comp_F = 0;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );

                    DBG3 (( MF__, "opt_comp = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_comp_F ));
                break;

                case 'R' :
                    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_prof_F = 0;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_prof = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_prof_F ));
                break;

                case 'O' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_trace_F = 4;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_trace = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_trace_F ));
                break;

                case 'T' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_trace_F = 2;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_trace = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_trace_F ));
                break;

                case 'X' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_trace_F = 3;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_trace = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_trace_F ));
                break;

                case 'Y' :
                    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_dyn_F = 1;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_dyn = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_dyn_F ));
                break;

                case 'l' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_list_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_list = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_list_F ));
                break;

                case 'c' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_precom_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_precom = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_precom_F ));
                break;

                case 'o' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_comment_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_comment = %d",
                                  prcArgsOptions ->
                                  variant.C_sp4co_sql_precompiler.opt_comment_F ));
                break;

                case 'W' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_traceswitch_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_traceswitch = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_traceswitch_F ));
                break;

                case 's' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_silent_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_silent = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_silent_F ));
                break;

                case 'w' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_nowarn_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_nowarn = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_nowarn_F ));
                break;

                case 'i' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_lineno_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_lineno = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_lineno_F ));
                break;

                case 'L' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_lib_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_lib = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_lib_F ));
                break;

                case 'q' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_quote_F = FALSE;
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_quote_F = '\"';
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_quote = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_quote_F ));
                    DBG3 (( MF__, "opt_quo = %c",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_quo_F ));
                break;

                case 'p' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_decpoint_F = FALSE;
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_decpoint_F = ',';
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_decpoint = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_decpoint_F ));
                    DBG3 (( MF__, "opt_point = %c",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_point_F ));
                break;

                case 'z' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_tabformat_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_tabformat = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_tabformat_F ));
                break;
                case 'e' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_extern_F = 0;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_extern = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_extern_F ));
                break;

                case 'V' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_version_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_version = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_version_F ));
                break;

                case 'h' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_help_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_help = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_precompiler.opt_help_F ));
                break;

                // all the other arguments
                case '?' :
                    // --- precompiler don't want to stop at ill option
                    bMoreArgs = FALSE;
                break;
            }   // switch ( nOptionChar )
        }   // while ()

        if (( bMoreArgs ) && ( pbOk )) {
            MakeArgs ( ' ', COMP_PRECOMPILER,
                       prcUserParams, prcArgsOptions, ucXuserType,
                       nArgumentCount, apszArguments,
                       bEnvDBNameFound, acErrorText, pbOk);
        }
    }   // if ( nArgumentCount > 1 )

    DBG3 (( MF__, "ucXuserType [ 0 ] = %d", ucXuserType [ 0 ] ));
    DBG3 (( MF__, "ucXuserType [ 1 ] = %d", ucXuserType [ 1 ] ));

    DBGOUT;
}


/*------------------------------*/


static VOID PrecompilerRunTimeOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgc,
                PCHAR*              apszArgv,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText         )
{
    #undef  MF__
    #define MF__ MOD__"PrecompilerRunTimeOptionsAnalysis"


    BOOLEAN         bMoreArgs;
    INT             nOptionChar;
    INT             nMyArgc;
    PCHAR           apszMyArgv [ OPT_AND_ARG_CNT ];
    PCHAR           pszOptionString;
    INT             nIndex1, nIndex2;
    INT             nLength;
    C132C           szSQLOpt;
    #if defined ( _WIN32 )
     APIRET         rc = NO_ERROR;
     BOOL           fSQLOptEnabled;
    #endif


    DBGIN;

    // initialize internel vars
    bMoreArgs = TRUE;
    nOptionChar = ' ';
    nMyArgc = 0;

    memset ( apszMyArgv, 0, OPT_AND_ARG_CNT );


    // get the precompiler runtime options and build up an apszArguments-array
    // and a nArgumentCount-count by reading the env var SQLOPTIONS
    pszOptionString = NULL;
    if ( sqlIsOptionEncodingUTF8() )
    {
        tsp00_Uint4 bytesWritten;
        tsp00_Uint4 bytesParsed;
        wchar_t     wszSQLOpt[132];
        DWORD EnvLen = GetEnvironmentVariableW(L"SQLOPT", wszSQLOpt, sizeof(wszSQLOpt) );
        if ( EnvLen > sizeof(wszSQLOpt) )
        {
            MSGD (( ERR_ENV_VAR_TO_LONG, "SQLOPT", sizeof(wszSQLOpt)/sizeof(wchar_t) ));
        }
        else if ( EnvLen != 0 )
        {
            // PTS 1109583
            // convert from UCS2 to UTF8...
            tsp78ConversionResult cnvResult;
            cnvResult = sp78convertString ( 
                sp77encodingUTF8,
                szSQLOpt,
                sizeof(szSQLOpt),
                &bytesWritten,
                true,
                sp77encodingUCS2Native,
                wszSQLOpt,
                (tsp00_Uint4)(wcslen(wszSQLOpt) * sizeof(wchar_t)),
                &bytesParsed 
                );
            switch( cnvResult )
            {
            case sp78_TargetExhausted:
            case sp78_TargetNotTerminated:
                MSGD (( ERR_ENV_VAR_TO_LONG, "SQLOPT", sizeof(szSQLOpt) ));
                break;
            case sp78_Ok:
                pszOptionString = szSQLOpt;
                break;
            default:
                MSGD (( ERR_ENV_VAR_TO_LONG, "SQLOPT", 0 ));
                break;
            }
        }
    }
    else
    {
        DWORD EnvLen = GetEnvironmentVariable("SQLOPT", szSQLOpt, sizeof(szSQLOpt) );
        if ( EnvLen > sizeof(szSQLOpt) )
        {
            MSGD (( ERR_ENV_VAR_TO_LONG, "SQLOPT", sizeof(szSQLOpt) ));
        }
        else if ( EnvLen != 0 )
        {
            pszOptionString = szSQLOpt;
        }
    }

    #if defined ( _WIN32 )
     if ( !pszOptionString ) {
          rc = sql13u_get_options_sqlopt ( &fSQLOptEnabled,
                                           szSQLOpt, sizeof(szSQLOpt) - 1);

          if (( rc == NO_ERROR ) && ( fSQLOptEnabled == TRUE ))
              pszOptionString = szSQLOpt;
     }
    #endif

    if ( pszOptionString ) {
        // set apszMyArgv [ 0 ] to something, because you need something
        apszMyArgv [ 0 ] = "precomp_runtime";
        nLength = (INT)strlen ( pszOptionString );

        DBG3 (( MF__, "pszOptionsString = \"%s\"", pszOptionString ));
        DBG3 (( MF__, "nLength = %d", nLength ));
        DBG3 (( MF__, "apszMyArgv [ 0 ] = \"%s\"", apszMyArgv [ 0 ] ));


        // extract every option and argument
        for ( nIndex1 = 0 ; nIndex1 < nLength ; nIndex1++ ) {
            if ( pszOptionString [ nIndex1 ] != ' ' ) {
                nMyArgc++;

                DBG3 (( MF__, "&opt_string [ %d ] = 0x%x",
                               nIndex1, &pszOptionString [ nIndex1 ] ));

                // take over the string
                apszMyArgv [ nMyArgc ] = &pszOptionString [ nIndex1 ];

                // skip string
                for ( nIndex2 = nIndex1; nIndex2 < nLength; nIndex2++ ) {
                    if ( pszOptionString [ nIndex2 ] == ' ' )
                        break;
                }

                // string end
                pszOptionString [ nIndex2 ] = '\0';
                nIndex1 = nIndex2;

                DBG3 (( MF__, "apszMyArgv [ %d ] = \"%s\"",
                               nMyArgc, apszMyArgv [ nMyArgc ] ));
            }   // if ()
        }   // for ()

        nMyArgc++;
    }   // if ()

    DBG3 (( MF__, "nMyArgc = %d", nMyArgc ));

    // parameter existing ?

    if ( nMyArgc > 1 ) {
        // get options
        while ((( nOptionChar =
                      sqlgetopt ( nMyArgc, apszMyArgv,
                                      PRECOMPILER_RUNTIME_OPTION_STRING)) != -1) &&
                  bMoreArgs                                                      &&
                  *pbOk ) {

            DBG3 (( MF__, "nOptionChar = %c", nOptionChar ));
            DBG3 (( MF__, "sql80_OptInd = %d", sql80_OptInd ));

            switch ( nOptionChar ) {
                // general optargs
                case 'u' :
                    MakeUserPassword ( prcUserParams,
                                     acPassword, pbOk, acErrorText );
                    PUTBIT ( ucXuserType, sp4xu_sql_userparms );
                    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
                break;

                case 'd' :
                    MakeDBId ( prcUserParams, ucXuserType );
                break;

                case 'n' :
                    MakeDBNode ( prcUserParams , ucXuserType);
                break;

                case 'U' :
                    MakeUserKey ( prcUserParams, ucXuserType,
                                  bEnvDBNameFound, acErrorText, pbOk );
                break;

                case 'S' :
                    MakeSqlMode ( prcUserParams, ucXuserType );
                break;

                case 'I' :
                    MakeIsolation ( prcUserParams, ucXuserType);
                break;

                case 't' :
                    MakeTimeOut ( prcUserParams, ucXuserType );
                break;

                case 'y' :
                    MakeCacheLimit ( prcUserParams, ucXuserType );
                break;

                case 'x' :
                    MakeDBLang ( prcUserParams, ucXuserType);
                break;

                // optargs with arguments
                case 'F' :
                    MakePrecompilerRunTimeTraceFn ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'B' :
                    MakePrecompilerRunTimeMFetch ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'Y' :
                    MakePrecompilerRunTimeStmtCnt ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'L' :
                    MakePrecompilerRunTimeLimit ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                // opargs with no arguments
                case 'O' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_pc_runtime.opt_rtrace_F = 4;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_rtrace = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F ));
                break;

                case 'T' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_pc_runtime.opt_rtrace_F = 2;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_rtrace = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F ));
                break;

                case 'X' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_pc_runtime.opt_rtrace_F = 3;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_rtrace = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F ));
                break;

                case 'R' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_pc_runtime.opt_rprof_F = 1;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_rprof = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rprof_F ));
                break;

                case 'Z' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_pc_runtime.opt_rprof_F = 2;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_rprof = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rprof_F ));
                break;

                case 'N' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_pc_runtime.opt_rnotracetime_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_rnotraceti = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rnotracetime_F ));
                break;

                case 'W' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_pc_runtime.opt_rtraceswitch_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_rtraceswit = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtraceswitch_F ));
                break;

                case 'f' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_pc_runtime.opt_rnosdf_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_rnosdf = %d",
                                   prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rnosdf_F ));
                break;

                // all the other arguments
                case '?' :
                    // --- precompiler don't want to stop at ill option
                    bMoreArgs = FALSE;
                break;
            }   // switch ( nOptionChar )
        }   // while ()

        // all the other arguments, nOptionChar = -1
        if (( bMoreArgs ) && ( pbOk )) {
            MakeMyArgs ( ' ', COMP_PC_RUNTIME,
                         prcUserParams, prcArgsOptions, ucXuserType,
                         nArgc, apszArgv, bEnvDBNameFound, acErrorText, pbOk );
        }
    }   // if ( nMyArgc > 1 )


    DBG3 (( MF__, "ucXuserType [ 0 ] = %d", ucXuserType [ 0 ] ));
    DBG3 (( MF__, "ucXuserType [ 1 ] = %d", ucXuserType [ 1 ] ));

    DBGOUT;
}


/*------------------------------*/


static VOID DialogComponentOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgumentCount,
                PCHAR*              apszArguments,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText         )
{
    #undef  MF__
    #define MF__ MOD__"DialogComponentOptionsAnalysis"


    BOOLEAN bMoreArgs;
    INT     nOptionChar;


    DBGIN;

    // initialize internel vars
    bMoreArgs   = TRUE;
    nOptionChar = ' ';

    DBG3 (( MF__, "nArgumentCount = %d", nArgumentCount ));

    // parameter existing ?
    if ( nArgumentCount > 1 ) {
        // get options

        while ((( nOptionChar =
                      sqlgetopt ( nArgumentCount, apszArguments,
                                      DIALOG_COMPONENT_OPTION_STRING )) != -1 ) &&
                  bMoreArgs                                                     &&
                  *pbOk ) {

            DBG3 (( MF__, "nOptionChar = %c", nOptionChar ));
            DBG3 (( MF__, "sql80_OptInd = %d", sql80_OptInd ));

            switch ( nOptionChar ) {
                // general optargs
                case 'u' :
                    MakeUserPassword ( prcUserParams,
                                       acPassword, pbOk, acErrorText );
                    PUTBIT ( ucXuserType, sp4xu_sql_userparms );
                    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
                break;

                case 'd' :
                    MakeDBId ( prcUserParams, ucXuserType );
                break;

                case 'n' :
                    MakeDBNode ( prcUserParams , ucXuserType);
                break;

                case 'U' :
                    MakeUserKey ( prcUserParams, ucXuserType,
                                  bEnvDBNameFound, acErrorText, pbOk );
                break;

                case 'S' :
                    MakeSqlMode ( prcUserParams, ucXuserType );
                break;

                case 'I' :
                    MakeIsolation ( prcUserParams, ucXuserType);
                break;

                case 't' :
                    MakeTimeOut ( prcUserParams, ucXuserType );
                break;

                case 'y' :
                    MakeCacheLimit ( prcUserParams, ucXuserType );
                break;

                case 'x' :
                    MakeDBLang ( prcUserParams, ucXuserType);
                break;

                // optargs with arguments
                case 'r' :
                    MakeDialogComponentRFn ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'b' :
                    MakeDialogComponentBFn ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'R' :
                    if (( prcArgsOptions -> opt_component == COMP_DIALOG ) ||
                        ( prcArgsOptions -> opt_component == COMP_APPL   )) {
                        MakeDialogComponentObj ( prcArgsOptions, sp4cm_sql_run_object,
                                                 2, pbOk, acErrorText );
                    }
                    else {
                        MakeDialogComponentObj ( prcArgsOptions, sp4cm_sql_run_object,
                                                 1, pbOk, acErrorText );
                    }

                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'B' :
                    if (( prcArgsOptions -> opt_component == COMP_DIALOG ) ||
                        ( prcArgsOptions -> opt_component == COMP_APPL   )) {
                        MakeDialogComponentObj ( prcArgsOptions, sp4cm_sql_batch_object,
                                                 2, pbOk, acErrorText );
                    }
                    else {
                        MakeDialogComponentObj ( prcArgsOptions, sp4cm_sql_batch_object,
                                                 1, pbOk, acErrorText );
                    }
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'O' :
                    MakeDialogComponentObj ( prcArgsOptions, sp4cm_sql_list, 1,
                                             pbOk, acErrorText );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'C' :
                    MakeDialogComponentCodeType ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'e' :
                    MakeDialogComponentExport ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'i' :
                    MakeDialogComponentImport ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                // optargs with no arguments
                case 'A' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_easy.opt_append_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_append = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_easy.opt_append_F ));
                break;

                case 'P' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_easy.opt_prompt_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_prompt = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_easy.opt_prompt_F ));
                break;

                case 'c' :
                    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_codeonly_F = TRUE;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_codeonly = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_easy.opt_codeonly_F ));
                break;

                case 'V' :
                    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_comp_vers;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_comm_mode = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_easy.opt_comm_mode_F ));
                break;

                case 's' :
                    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_select;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_comm_mode = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_easy.opt_comm_mode_F ));
                break;

                case 'T' :
                    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_utility_session_F = TRUE ;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_comm_mode = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_easy.opt_utility_session_F ));
                break;

                case 'L' :
                    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_list_command;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_comm_mode = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_easy.opt_comm_mode_F ));
                break;

                // all the other arguments
                case '?' :
                    MakeArgs ( '?', COMP_EASY,
                               prcUserParams, prcArgsOptions, ucXuserType,
                               nArgumentCount, apszArguments,
                               bEnvDBNameFound, acErrorText, pbOk );
                    bMoreArgs = FALSE;
                break;
            }   // switch ()
        }   // while ()
        if (( bMoreArgs ) && ( pbOk )) {
            MakeArgs ( ' ', COMP_EASY,
                       prcUserParams, prcArgsOptions, ucXuserType,
                       nArgumentCount, apszArguments,
                       bEnvDBNameFound, acErrorText, pbOk );
        }
    }   // if ()

    DBG3 (( MF__, "ucXuserType [ 0 ] = %d", ucXuserType [ 0 ] ));
    DBG3 (( MF__, "ucXuserType [ 1 ] = %d", ucXuserType [ 1 ] ));

    DBGOUT;
}


/*------------------------------*/


static VOID AuditOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgumentCount,
                PCHAR*              apszArguments,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText         )
{
    #undef  MF__
    #define MF__ MOD__"AuditOptionsAnalysis"


    BOOLEAN bMoreArgs;
    INT     nOptionChar;


    DBGIN;

    // initialize internel vars
    bMoreArgs    = TRUE;
    nOptionChar = ' ';

    DBG3 (( MF__, "nArgumentCount = %d", nArgumentCount ));

    // parameter existing ?
    if ( nArgumentCount > 1 ) {
        // get options
        while ((( nOptionChar =
                      sqlgetopt ( nArgumentCount, apszArguments,
                                      AUDIT_OPTION_STRING )) != - 1 ) &&
                  bMoreArgs                                           &&
                  *pbOk ) {

            DBG3 (( MF__, "nOptionChar = %c", nOptionChar ));
            DBG3 (( MF__, "sql80_OptInd = %d", sql80_OptInd ));

            switch ( nOptionChar ) {
                // general optargs
                case 'u' :
                    MakeUserPassword ( prcUserParams,
                                     acPassword, pbOk, acErrorText );
                    PUTBIT ( ucXuserType, sp4xu_sql_userparms );
                    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
                break;

                case 'd' :
                    MakeDBId ( prcUserParams, ucXuserType );
                break;

                case 'n' :
                    MakeDBNode ( prcUserParams , ucXuserType);
                break;

                case 'U' :
                    MakeUserKey ( prcUserParams, ucXuserType,
                                  bEnvDBNameFound, acErrorText, pbOk );
                break;

                case 'S' :
                    MakeSqlMode ( prcUserParams, ucXuserType );
                break;

                case 'I' :
                    MakeIsolation ( prcUserParams, ucXuserType);
                break;

                case 't' :
                    MakeTimeOut ( prcUserParams, ucXuserType );
                break;

                case 'y' :
                    MakeCacheLimit ( prcUserParams, ucXuserType );
                break;

                case 'x' :
                    MakeDBLang ( prcUserParams, ucXuserType);
                break;

                // optargs with arguments
                case 'c' :
                    MakeAuditCntrl ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'l' :
                    MakeAuditLogfile ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'N' :
                    MakeAuditLogBlockSize ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                case 'o' :
                    MakeAuditOutput ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                // opargs with no arguments
                case 'V' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_audit.opt_aud_comm_mode_F = sp4cm_sql_comp_vers;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_aud_comm_mod = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_audit.opt_aud_comm_mode_F ));
                break;

                // all the other arguments
                case '?' :
                    MakeArgs ( '?', COMP_AUDIT,
                               prcUserParams, prcArgsOptions, ucXuserType,
                               nArgumentCount, apszArguments,
                               bEnvDBNameFound, acErrorText, pbOk );
                    bMoreArgs = FALSE;
                break;
            }   // switch ()
        }   // while ()

        if (( bMoreArgs ) && ( pbOk )) {
            MakeArgs ( ' ', COMP_AUDIT,
                       prcUserParams, prcArgsOptions, ucXuserType,
                       nArgumentCount, apszArguments,
                       bEnvDBNameFound, acErrorText, pbOk );
        }
    }   // if ()

    DBG3 (( MF__, "ucXuserType [ 0 ] = %d", ucXuserType [ 0 ] ));
    DBG3 (( MF__, "ucXuserType [ 1 ] = %d", ucXuserType [ 1 ] ));

    DBGOUT;
}


/*------------------------------*/

static VOID UserXOptionsAnalysis
            (   tsp4_xuser_record  *prcUserParams,
                tsp00_Pw            acPassword,
                tsp4_args_options  *prcArgsOptions,
                tsp4_xuserset       ucXuserType,
                INT                 nArgumentCount,
                PCHAR*              apszArguments,
                BOOLEAN             bEnvDBNameFound,
                PBOOLEAN            pbOk,
                ERRORTEXT           acErrorText         )
{
    #undef  MF__
    #define MF__ MOD__"UserXOptionsAnalysis"


    BOOLEAN bMoreArgs;
    INT     nOptionChar;


    DBGIN;

    // initialize internel vars
    bMoreArgs    = TRUE;
    nOptionChar = ' ';

    DBG3 (( MF__, "nArgumentCount = %d", nArgumentCount ));

    // parameter existing ?
    if ( nArgumentCount > 1 ) {
        // get options
        while ((( nOptionChar =
                      sqlgetopt ( nArgumentCount, apszArguments,
                                      USERX_OPTION_STRING )) != - 1 ) &&
                  bMoreArgs                                           &&
                  *pbOk ) {

            DBG3 (( MF__, "nOptionChar = %c", nOptionChar ));
            DBG3 (( MF__, "sql80_OptInd = %d", sql80_OptInd ));

            switch ( nOptionChar ) {
                // general optargs
                case 'u' :
                    MakeUserPassword ( prcUserParams,
                                     acPassword, pbOk, acErrorText );
                    PUTBIT ( ucXuserType, sp4xu_sql_userparms );
                    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
                break;


                // opargs with no arguments
                case 'V' :
                    prcArgsOptions ->
                    variant.C_sp4co_sql_userx.opt_ux_comm_mode_F = sp4cm_sql_comp_vers;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                    DBG3 (( MF__, "opt_ux_comm_mod = %d",
                                   prcArgsOptions ->
                                   variant.C_sp4co_sql_userx.opt_ux_comm_mode_F ));
                break;

                case 'b' :
                    MakeUserXComponentBFn ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                break;

                // all the other arguments
                case '?' :
                    MakeArgs ( '?', COMP_USER,
                               prcUserParams, prcArgsOptions, ucXuserType,
                               nArgumentCount, apszArguments,
                               bEnvDBNameFound, acErrorText, pbOk );
                    bMoreArgs = FALSE;
                break;
            }   // switch ()
        }   // while ()

        if (( bMoreArgs ) && ( pbOk )) {
            MakeArgs ( ' ', COMP_USER,
                       prcUserParams, prcArgsOptions, ucXuserType,
                       nArgumentCount, apszArguments,
                       bEnvDBNameFound, acErrorText, pbOk );
        }
    }   // if ()

    DBG3 (( MF__, "ucXuserType [ 0 ] = %d", ucXuserType [ 0 ] ));
    DBG3 (( MF__, "ucXuserType [ 1 ] = %d", ucXuserType [ 1 ] ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeArgs    (   INT                 nOptionChar,
                            tsp4_component      eComponent,
                            tsp4_xuser_record  *prcUserParams,
                            tsp4_args_options  *prcArgsOptions,
                            tsp4_xuserset       ucXuserType,
                            INT                 nArgc,
                            PCHAR*              apszArgv,
                            BOOLEAN             bEnvDBNameFound,
                            ERRORTEXT           acErrorText,
                            PBOOLEAN            pbOk            )
{
    #undef  MF__
    #define MF__ MOD__"MakeArgs"


    INT     nTestChar0;
    INT     nTestChar1;
    INT     nLength;
    INT     nArgNumber;
    INT     nOutIndex;
    INT     nSize;
    PCHAR   psArgumentString;
    APIRET  rc;
    BOOLEAN bArgLineExists = FALSE;


    DBGIN;

    if ( nOptionChar == '?' ) {
        // look for option names
        // if found take over
        DBG3 (( MF__, "sql80_OptInd ( nOptionChar = ? ) = %d", sql80_OptInd ));

        nTestChar0 = apszArgv [ sql80_OptInd - 1 ] [ 0 ];
        nTestChar1 = apszArgv [ sql80_OptInd - 1 ] [ 1 ];

        DBG3 (( MF__, "apszArgv [ sql80_OptInd - 1 ] = \"%s\"",
                       apszArgv [ sql80_OptInd - 1 ]  ));

        DBG3 (( MF__, "nTestChar0 = %c", nTestChar0 ));
        DBG3 (( MF__, "nTestChar1 = %c", nTestChar1 ));

        if (( nTestChar0 == '-' || nTestChar0 == '/' ) &&
            ( nTestChar1 == 'R' || nTestChar1 == 'B' ||
              nTestChar1 == 'r' || nTestChar1 == 'b' )) {

            switch ( eComponent ) {
              case COMP_EASY    :
              case COMP_QUERY   :
              case COMP_DIALOG  :
              case COMP_APPL    :
              case COMP_LOAD    :
              case COMP_PARAM   :
              case COMP_UTILITY :

                 // get command, set runfile to STDIN
                 switch ( nTestChar1 ) {
                     case 'r' :
                         prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run;

                         PUTBIT ( ucXuserType, sp4xu_sql_params );

                         SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
                                  "STDIN", 5 );
                     break;

                     case 'b' :
                         prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch;

                         PUTBIT ( ucXuserType, sp4xu_sql_params );

                         SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
                                  "STDIN", 5 );
                     break;
                     case 'B' :

                         memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F,
                                  ' ',
                                  sizeof( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F));

                         memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                                  ' ',
                                  sizeof( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F));

                         memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F,
                                  ' ',
                                  sizeof( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F));

                         prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F =  sp4cm_sql_batch_object;

                         PUTBIT ( ucXuserType, sp4xu_sql_params );

                     break;

                     case 'R' :
                         memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F,
                                  ' ',
                                  sizeof( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F));

                         memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                                  ' ',
                                  sizeof( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F));

                         memset ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F,
                                  ' ',
                                  sizeof( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F));

                         prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F =  sp4cm_sql_run_object;

                         PUTBIT ( ucXuserType, sp4xu_sql_params );

                     break;
                 }
              break;
              case COMP_USER    :
                 switch ( nTestChar1 ) {
                     case 'b' :
                         prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F = sp4cm_sql_batch;

                         PUTBIT ( ucXuserType, sp4xu_sql_params );

                         SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_runfile_F,
                                  "STDIN", 5 );
                     break;
                 }
              break;

            }

            DBGOUT;

            return;
        }
        else {

            // may be there was a 'U'
            if (( nTestChar0 == '-' || nTestChar0 == '/' ) &&
                ( nTestChar1 == 'U' )) {
                // check for error conditions
                if ( TSTBIT ( ucXuserType, sp4xu_sql_userparms  )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_servernode )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_mode       )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_isolation  )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_timeout    )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_cachelimit )) {

                    *pbOk = FALSE;

                    DBG1 (( MF__, ERRMSG_WRONG_OPTION_SEQUENCE ));
                    MSGD  (( ERR_WRONG_OPTION_SEQUENCE ));
                    sql46c_build_error_string ( acErrorText,
                                                ERRMSG_WRONG_OPTION_SEQUENCE,
                                                0 );
                    DBGOUT;

                    return;
                }

                if ( !bEnvDBNameFound && TSTBIT ( ucXuserType, sp4xu_sql_serverdb)) {
                    *pbOk = FALSE;

                    DBG1 (( MF__, ERRMSG_WRONG_OPTION_SEQUENCE ));
                    MSGD  (( ERR_WRONG_OPTION_SEQUENCE ));
                    sql46c_build_error_string ( acErrorText,
                                                ERRMSG_WRONG_OPTION_SEQUENCE,
                                                0 );
                    DBGOUT;
                    return;
                }

                PUTBIT ( ucXuserType, sp4xu_sql_userdefault );
                CLRBIT ( ucXuserType, sp4xu_sql_usermask );

                SAPDB_memcpy ( prcUserParams->xu_key, DEFAULT_USER,
                         sizeof ( prcUserParams->xu_key ));

                rc = sql13u_getuser ( prcUserParams, NULL, acErrorText );

                if ( rc != NO_ERROR )
                    *pbOk = FALSE;

                DBG3 (( MF__, "ucXuserType [ 0 ] = %d", ucXuserType [ 0 ] ));
                DBG3 (( MF__, "ucXuserType [ 1 ] = %d", ucXuserType [ 1 ] ));

                DBG3 (( MF__, "prcUserParams -> xu_key = \"%s\"",
                        _PToC ( prcUserParams -> xu_key )));
                DBG3 (( MF__, "prcUserParams -> xu_servernode = \"%s\"",
                        _PToC ( prcUserParams -> xu_servernode )));
                DBG3 (( MF__, "prcUserParams -> xu_serverdb = \"%s\"",
                        _PToC ( prcUserParams -> xu_serverdb )));
                DBG3 (( MF__, "prcUserParams -> xu_user = \"%s\"",
                        _PToC ( prcUserParams -> xu_user )));
                DBG3 (( MF__, "prcUserParams -> xu_sqlmode = \"%s\"",
                        _PToC ( prcUserParams -> xu_sqlmode )));

                DBG3 (( MF__, "prcUserParams -> xu_cachelimit = %d",
                        prcUserParams -> xu_cachelimit ));
                DBG3 (( MF__, "prcUserParams -> xu_timeout = %d",
                        prcUserParams -> xu_timeout ));
                DBG3 (( MF__, "prcUserParams -> xu_isolation = %d",
                        prcUserParams -> xu_isolation ));

                DBGOUT;

                return;
            }
            else {
                // no regular option, reduce sql80_OptInd but be sure to do this
                // only you have found an '-?' expression
                // ? : means only one char

                if (( sql80_OptInd > 1 )                              &&
                    ( strlen ( apszArgv [ sql80_OptInd - 1 ] ) == 2 ) &&
                    ( nTestChar0 == '-' || nTestChar0 == '/' )) {
                    sql80_OptInd --;
                }
            }
        }
    }

    // take over args into arg line; if an arg line exists
    // but be sure you have an argument
    if ( nArgc > sql80_OptInd ) {
        DBG3 (( MF__, "eComponent = %d", eComponent ));
        DBG3 (( MF__, "sql80_OptInd     = %d", sql80_OptInd ));

        switch ( eComponent ) {
            case COMP_PRECOMPILER  :
                if ( prcArgsOptions->
                       variant.C_sp4co_sql_precompiler.opt_modulefn_F[0] == ' ' ) {
                    sql80_OptArg = apszArgv [ sql80_OptInd++ ];
                    MakePrecompilerInputFn ( prcArgsOptions );
                    PUTBIT ( ucXuserType, sp4xu_sql_params );
                }
                nSize = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_puser_args_F );
                psArgumentString = prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_puser_args_F;
                bArgLineExists = TRUE;

            break;

            case COMP_PC_RUNTIME   :
                nSize = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F );
                psArgumentString = prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F;
                bArgLineExists = TRUE;
            break;

            case COMP_EASY    :
            case COMP_QUERY   :
            case COMP_DIALOG  :
            case COMP_APPL    :
            case COMP_LOAD    :
            case COMP_UTILITY :
                nSize = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_parameter_F );
                psArgumentString = prcArgsOptions -> variant.C_sp4co_sql_easy.opt_parameter_F;
                bArgLineExists = TRUE;
            break;

            default :
                bArgLineExists = FALSE;
            break;
        }

        if ( bArgLineExists ) {
            nOutIndex = 0;

            for ( nArgNumber = sql80_OptInd; nArgNumber < nArgc; nArgNumber++ ) 
            {
                BOOL  ArgumentHasBlanks = false;

                nLength = (INT)strlen ( apszArgv [ nArgNumber ] );

                DBG3 (( MF__, "nLength(apszArgv[nArgNumber] = %d", nLength ));

                if ( nLength > 0 ) 
                {
                    int i;
                    PUTBIT ( ucXuserType, sp4xu_sql_params );

                    for ( i = 0; i < nLength; i++ )
                    {
                        if ( apszArgv[nArgNumber][i] == ' ' )
                        {
                            ArgumentHasBlanks = TRUE;
                            break;
                        }
                    }

                }
                if ( ArgumentHasBlanks )
                {
                    if ( nOutIndex + nLength + 2  <= nSize ) 
                    {
                        psArgumentString[nOutIndex++] = '\"';
                        SAPDB_memcpy ( psArgumentString + nOutIndex,apszArgv [ nArgNumber ], nLength );
                        nOutIndex += nLength;
                        psArgumentString[nOutIndex] = '\"';
                        nOutIndex += 2;
                    }
                    else
                        break;
                }
                else
                {
                    if ( nOutIndex + nLength  <= nSize ) 
                    {
                        SAPDB_memcpy ( psArgumentString + nOutIndex,apszArgv [ nArgNumber ], nLength );
                        nOutIndex += nLength + 1;
                    }
                    else
                        break;
                }
            }
        }
    }

    DBGOUT;
}


/*------------------------------*/


static VOID MakeMyArgs  (   INT                 nOptionChar,
                            tsp4_component      eComponent,
                            tsp4_xuser_record  *prcUserParams,
                            tsp4_args_options  *prcArgsOptions,
                            tsp4_xuserset       ucXuserType,
                            INT                 nArgc,
                            PCHAR*              apszArgv,
                            BOOLEAN             bEnvDBNameFound,
                            ERRORTEXT           acErrorText,
                            PBOOLEAN            pbOk                )
{
    #undef  MF__
    #define MF__ MOD__"MakeMyArgs"


    INT     nTestChar0;
    INT     nTestChar1;
    APIRET  rc;


    DBGIN;

    if ( nOptionChar == '?' ) {
        // look for option names, if found take over
        DBG3 (( MF__, "sql80_OptInd  ( nOptionChar = ? ) = %d", sql80_OptInd ));

        nTestChar0 = apszArgv [ sql80_OptInd - 1 ] [ 0 ];
        nTestChar1 = apszArgv [ sql80_OptInd - 1 ] [ 1 ];

        DBG3 (( MF__,
                "apszArgv [ sql80_OptInd - 1 ] = \"%s\"", apszArgv [ sql80_OptInd - 1 ]  ));
        DBG3 (( MF__, "nTestChar0 = %c", nTestChar0 ));
        DBG3 (( MF__, "nTestChar1 = %c", nTestChar1 ));

        if (( nTestChar0 == '-' || nTestChar0 == '/' ) &&
            ( nTestChar1 == 'r' || nTestChar1 == 'b' )) {

            // get command, set runfile to STDIN
            switch ( nTestChar1 ) {
                case 'r' :
                    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run;

                    PUTBIT ( ucXuserType, sp4xu_sql_params );

                    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
                             "STDIN", 5 );
                break;

                case 'b' :
                    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch;

                    PUTBIT ( ucXuserType, sp4xu_sql_params );

                    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
                             "STDIN", 5 );
                break;
            }

            DBG3 (( MF__, "opt_comm_mode = %d",
                    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F ));
            DBG3 (( MF__, "opt_runfile = \"%s\"",
                    _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F )));

            DBGOUT;

            return;
        }
        else {
            // may be there was a 'U'
            if (( nTestChar0 == '-'  ||  nTestChar0 == '/' ) &&
                ( nTestChar1 == 'U' )) {
                // check for error conditions
                if ( TSTBIT ( ucXuserType, sp4xu_sql_userparms  )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_servernode )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_mode       )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_isolation  )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_timeout    )  |
                     TSTBIT ( ucXuserType, sp4xu_sql_cachelimit )) {

                    *pbOk = FALSE;

                    DBG1 (( MF__, ERRMSG_WRONG_OPTION_SEQUENCE ));
                    MSGD  (( ERR_WRONG_OPTION_SEQUENCE ));
                    sql46c_build_error_string ( acErrorText,
                                                ERRMSG_WRONG_OPTION_SEQUENCE,
                                                0 );

                    DBGOUT;

                    return;
                }

                if ( !bEnvDBNameFound && TSTBIT ( ucXuserType, sp4xu_sql_serverdb )){
                    *pbOk = FALSE;
                    DBG1 (( MF__, ERRMSG_WRONG_OPTION_SEQUENCE ));
                    MSGD  (( ERR_WRONG_OPTION_SEQUENCE ));
                    sql46c_build_error_string ( acErrorText,
                                                ERRMSG_WRONG_OPTION_SEQUENCE,
                                                0 );

                    DBGOUT;

                    return;
                }

                PUTBIT ( ucXuserType, sp4xu_sql_userdefault );
                CLRBIT ( ucXuserType, sp4xu_sql_usermask );

                SAPDB_memcpy ( prcUserParams -> xu_key, DEFAULT_USER, 
                         sizeof ( prcUserParams -> xu_key ));

                rc = sql13u_getuser ( prcUserParams, NULL, acErrorText );

                if ( rc != NO_ERROR )
                    *pbOk = FALSE;

                DBG3 (( MF__, "ucXuserType [ 0 ] = %d", ucXuserType [ 0 ] ));
                DBG3 (( MF__, "ucXuserType [ 1 ] = %d", ucXuserType [ 1 ] ));

                DBG3 (( MF__, "prcUserParams -> xu_key = \"%s\"",
                        _PToC ( prcUserParams -> xu_key )));
                DBG3 (( MF__, "prcUserParams -> xu_servernode = \"%s\"",
                        _PToC ( prcUserParams -> xu_servernode )));
                DBG3 (( MF__, "prcUserParams -> xu_serverdb = \"%s\"",
                        _PToC ( prcUserParams -> xu_serverdb )));
                DBG3 (( MF__, "prcUserParams -> xu_user = \"%s\"",
                        _PToC ( prcUserParams -> xu_user )));
                DBG3 (( MF__, "prcUserParams -> xu_sqlmode = \"%s\"",
                        _PToC ( prcUserParams -> xu_sqlmode )));

                DBG3 (( MF__, "prcUserParams -> xu_cachelimit = %d",
                        prcUserParams -> xu_cachelimit ))
                DBG3 (( MF__, "prcUserParams -> xu_timeout = %d",
                        prcUserParams -> xu_timeout ))

                DBG3 (( MF__, "prcUserParams -> xu_isolation = %d",
                        prcUserParams -> xu_isolation ))

                DBGOUT;

                return;
            }
            else {
                // no regular option, reduce sql80_OptInd but be sure to do this
                // only you have found an '-?' expression
                // ? : means only one char
                if (( sql80_OptInd > 1 )                              &&
                    ( strlen ( apszArgv [ sql80_OptInd - 1 ] ) == 2 ) &&
                    ( nTestChar0 == '-' || nTestChar0 == '/' )) {
                    sql80_OptInd --;
                }
            }
        }
    }

    // take over args into arg line; if an arg line exists
    // but be sure you have an argument

    if ( nArgc > sql80_OptInd ) {
        DBG3 (( MF__, "eComponent = %d", eComponent ));
        DBG3 (( MF__, "sql80_OptInd = %d", sql80_OptInd ));

        switch ( eComponent ) {
            case COMP_PC_RUNTIME   :
                *pbOk = FALSE;

                DBG1 (( MF__, ERRMSG_NO_VALID_OPTION ));
                MSGD  (( ERR_NO_VALID_OPTION ));
                sql46c_build_error_string ( acErrorText,
                                            ERRMSG_NO_VALID_OPTION,
                                            0 );

                DBGOUT;

                return;
        }
    }

    DBGOUT;
}


/*------------------------------*/

/*---------------------------------------
  Function: Ascii8Copy
  Description: Copy native UCS2 pascal string into ascii8 copy
  
  Arguments: destAscii8 [out] destination
             srcUCS2 [in] source
             srcLen [in] length of source and destination buffer
  Return value: true if Ascii8 was ok, false if UCS2 is needed (destAscii8 buffer filled with spaces)
 */
static bool Ascii8Copy(unsigned char *destAscii8, const unsigned short *srcUCS2, int srcLen)
{
    int i;

    memset(destAscii8, ' ', srcLen);
    for ( i = 0; (i < srcLen/2) && (srcUCS2[i] != ' ') ; i++ )
    {
        if ( srcUCS2[i] < 256 )
        {
            destAscii8[i] = (unsigned char)srcUCS2[i];
        }
        else
        {
            memset(destAscii8, ' ', srcLen);
            return false;
        }
    }
    return true;
}


/*---------------------------------------------------------------------------*/

static VOID MakeUserPassword    (   tsp4_xuser_record  *prcUserParams,
                                    tsp00_Pw            acPassword,
                                    PBOOLEAN            pbOk,
                                    ERRORTEXT           acErrorText     )
{
    #undef  MF__
    #define MF__ MOD__"MakeUserPassword"

    CTU_REC         CtU;
    BOOLEAN         bUIdToUpper;
    BOOLEAN         bPasswordToUpper;
    ULONG           nLength;
    PSZ             pszStart1Addr;
    PSZ             pszStart2Addr;
    ULONG           ulLength1;
    ULONG           ulLength2;
    bool            bpureAsciiUser  = true;
    bool            bpureAsciiPassw = true;

    DBGIN;

    // get userid ( only if it exists )
    if ( strlen ( sql80_OptArg ) > 0 ) {
        tsp00_Uint4 fillLength;
        void *      fillPointer;

        // initialize user id, password string and conv_to_upper

        memset ( prcUserParams -> xu_user, ' ', sizeof ( prcUserParams -> xu_user ));
        fillLength  = sizeof(prcUserParams->xu_userUCS2);
        fillPointer = &prcUserParams->xu_userUCS2[0];
        sp77encodingUCS2Native->fillString( &fillPointer, &fillLength, fillLength/2, ' ');

        // check user id and password, maybe you don't need to convert upper
        CheckIdents ( &CtU, ',', 2, pbOk, acErrorText );

        if ( !*pbOk ) {
            DBGOUT;
            return;
        }

        pszStart1Addr    =  CtU.pszIdent     [0];
        ulLength1        =  CtU.lIdentLen    [0];
        bUIdToUpper      =  CtU.bIdentToUpper[0];

        pszStart2Addr    =  CtU.pszIdent     [1];
        ulLength2        =  CtU.lIdentLen    [1];
        bPasswordToUpper =  CtU.bIdentToUpper[1];


        // determinate length of user id
        nLength = sizeof ( tsp00_KnlIdentifier );

        // check length
        if ( ulLength1 > 0 ) 
        {
          tsp00_Uint4 bytesWritten;

          if ( ulLength1 > nLength )
              ulLength1 = nLength;

          /* The caller must use sqlSetOptionEncodingUTF8(true) to enable this... */
          if ( sqlIsOptionEncodingUTF8() )
          {
            tsp00_Uint4 bytesParsed;

            unsigned int byteLength;
            unsigned int charLength;

            if ( sp83UTF8Convert_Success != 
                  sp83UTF8_Bytelen((const tsp00_Byte *)pszStart1Addr, ulLength1, &byteLength, &charLength) )
            {
                  eo46CtoP ( acErrorText,
                             "Username contains non UTF8 character",
                             sizeof(tsp00_ErrText) );
                  *pbOk = false;
                  return;
            }

            if ( charLength > nLength )
            {
                  eo46CtoP ( acErrorText,
                             "username too long for UCS2",
                             sizeof(tsp00_ErrText) );
                  *pbOk = false;
                  return;
            }

            sp78convertBuffer( sp77encodingUCS2Native,
                               prcUserParams->xu_userUCS2,
                               sizeof(prcUserParams->xu_userUCS2),
                              &bytesWritten,
                               sp77encodingUTF8,
                               pszStart1Addr, 
                               ulLength1,
                              &bytesParsed);

            /* Convert xu_userUCS2 into xu_user */
            bpureAsciiUser = Ascii8Copy((unsigned char *)&prcUserParams->xu_user[0], 
                                        (const unsigned short *)&prcUserParams->xu_userUCS2[0],
                                        bytesWritten );
            if ( !bpureAsciiUser )
            {
                memset(&prcUserParams->xu_user[0], ' ', sizeof(tsp00_KnlIdentifier));
            }

            fillLength  = sizeof(tsp00_Pw);
            fillPointer = acPassword;
            sp77encodingUCS2Native->fillString( &fillPointer, &fillLength, fillLength/2, ' ');

          }
          else
          {
            const unsigned char * srcAscii;
            unsigned short      * dstUCS2;
            ULONG                 i;

            bytesWritten = ulLength1 * 2;
            if ( bytesWritten > nLength )
            {
                  eo46CtoP ( acErrorText,
                             "Username too long for UCS2",
                             sizeof(tsp00_ErrText) );
                  *pbOk = false;
                  return;
            }
            dstUCS2 = (unsigned short *)&prcUserParams->xu_userUCS2[0];
            srcAscii = (const unsigned char *)pszStart1Addr;
            for ( i = 0; i < ulLength1; i++ )
            {
              dstUCS2[i] = srcAscii[i];
            }

            SAPDB_memcpy(prcUserParams->xu_user, pszStart1Addr, ulLength1);
          }

          // if necessary convert string to upper
          if ( bUIdToUpper ) 
          {
              if(sp77encodingUCS2Native == sp77encodingUCS2Swapped) {
                  sp81UCS2SwappedStringToupper( &prcUserParams -> xu_userUCS2[0], bytesWritten );
              } else {
                  sp81UCS2StringToupper( &prcUserParams -> xu_userUCS2[0], bytesWritten );
              }
              MoveToUpper( &prcUserParams -> xu_user[0], bytesWritten/2 );
          }

          DBG3 (( MF__, "prcUserParams -> xu_user = \"%s\"",
                          _PToC ( prcUserParams -> xu_user )));

          memset( acPassword, ' ', sizeof(tsp00_Pw) );
        }

        // get password if uLength2 > 0
        if ( ulLength2 > 0 ) 
        {
            tsp00_Uint4 bytesWritten;
            tsp00_Uint4 bytesParsed;

            unsigned int byteLength;
            unsigned int charLength;

            tsp00_Pw tmpPasswordUCS2;
            tsp00_Pw tmpPasswordAscii;

            // determinate length of password
            nLength = sizeof ( tsp00_Pw );

            // check length
            if ( ulLength2 > nLength )
                ulLength2 = nLength;

            /* The caller must use sqlSetOptionEncodingUTF8(true) to enable this... */
            if ( sqlIsOptionEncodingUTF8() )
            {
                if ( sp83UTF8Convert_Success != 
                      sp83UTF8_Bytelen((const tsp00_Byte *)pszStart2Addr, ulLength2, &byteLength, &charLength) )
                {
                      eo46CtoP ( acErrorText,
                                 "Password contains non UTF8 character",
                                 sizeof(tsp00_ErrText) );
                      *pbOk = false;
                      return;
                }

                if ( charLength > nLength )
                {
                      eo46CtoP ( acErrorText,
                                 "password too long for UCS2",
                                 sizeof(tsp00_ErrText) );
                      *pbOk = false;
                      return;
                }

                sp78convertBuffer( sp77encodingUCS2Native,
                                  &tmpPasswordUCS2[0],
                                   sizeof(tsp00_Pw),
                                  &bytesWritten,
                                   sp77encodingUTF8,
                                   pszStart2Addr, 
                                   ulLength2,
                                  &bytesParsed);

                /* Convert xu_userUCS2 into xu_user */
                bpureAsciiPassw = Ascii8Copy((unsigned char *)&tmpPasswordAscii[0], 
                                             (const unsigned short *)&tmpPasswordUCS2[0],
                                             bytesWritten );
                if ( !bpureAsciiPassw || !bpureAsciiUser )
                {
                    fillLength  = sizeof(tsp00_Pw);
                    fillPointer = acPassword;
                    sp77encodingUCS2Native->fillString( &fillPointer, &fillLength, fillLength/2, ' ');
                    memset(&prcUserParams->xu_user[0], ' ', sizeof(tsp00_KnlIdentifier));
                    SAPDB_memcpy ( acPassword, tmpPasswordUCS2 , bytesWritten );
                    if ( bPasswordToUpper ) 
                    {
                        sp81UCS2StringToupper( acPassword, bytesWritten );
                    }
                }
                else
                {
                    memset ( acPassword, ' ', sizeof ( tsp00_Pw ) );
                    SAPDB_memcpy ( acPassword, tmpPasswordAscii , bytesWritten/2 );
                    if ( bPasswordToUpper ) 
                    {
                        MoveToUpper( acPassword, bytesWritten/2 );
                    }
                }

            }
            else
            {
                memset ( acPassword, ' ', sizeof ( tsp00_Pw ) );
                SAPDB_memcpy ( acPassword, pszStart2Addr , ulLength2 );
                if ( bPasswordToUpper ) 
                {
                    MoveToUpper( acPassword, ulLength2 );
                }
            }

            DBG3 (( MF__, "acPassword = \"%s\"", _PToC ( acPassword )));
        }
    }

    DBGOUT;
}


/*------------------------------*/


static VOID MakeDBId    (   tsp4_xuser_record  *prcUserParams,
                            tsp4_xuserset       ucXuserType     )
{
    #undef  MF__
    #define MF__ MOD__"MakeDBId"


    SQL_NODEIDC     acNodeId;
    SQL_DBNAMEC     acDBName;


    DBGIN;

    // get servernode ( only if it exists )
    if ( strlen ( sql80_OptArg )) {
        // initialize serverdb string

        memset ( prcUserParams -> xu_serverdb, ' ', 
                 sizeof ( prcUserParams -> xu_serverdb ));

        // if nessecary split serverdb and servernode
        acNodeId [ 0 ] = (CHAR) 0;             // init the node to empty
        strncpy ( acDBName, sql80_OptArg, sizeof ( SQL_DBNAMEC ));

        DBG3 (( MF__, "acDBName ( original ) = \"%s\"", acDBName ));

        sql17u_split_dbname ( acDBName, acNodeId );

        DBG3 (( MF__, "acDBName ( after splitdbname ) = \"%s\"", acDBName ));
        DBG3 (( MF__, "acNodeId ( after splitdbname ) = \"%s\"", acNodeId ));

        // take over C-names into pascal vars
        eo46CtoP ( prcUserParams -> xu_serverdb,
                      acDBName, 
                      (SHORT) sizeof ( prcUserParams -> xu_serverdb ));

//      MoveToUpper ( prcUserParams -> xu_serverdb, strlen ( acDBName ) );

        if ( strlen ( acNodeId )) {
            // initialize servernode string
            memset ( prcUserParams -> xu_servernode, ' ', 
                     sizeof ( prcUserParams -> xu_servernode ));

            // get servernode
            eo46CtoP ( prcUserParams -> xu_servernode,
                         acNodeId, 
                         (SHORT) sizeof ( prcUserParams -> xu_servernode ));

            // get servernode
            PUTBIT ( ucXuserType, sp4xu_sql_servernode );
        }

        DBG3 (( MF__, "prcUserParams -> xu_serverdb = \"%s\"",
                _PToC ( prcUserParams -> xu_serverdb )));
        DBG3 (( MF__, "prcUserParams -> xu_servernode = \"%s\"",
                _PToC ( prcUserParams -> xu_servernode )));
    }

    PUTBIT ( ucXuserType, sp4xu_sql_serverdb );
    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
    DBGOUT;
}


/*------------------------------*/


static VOID MakeDBNode  (   tsp4_xuser_record  *prcUserParams, 
                            tsp4_xuserset       ucXuserType     )
{
    #undef  MF__
    #define MF__ MOD__"MakeDBNode"


    INT nArgLength;
    INT nLength;


    DBGIN;

    // get servernode
    memset ( prcUserParams -> xu_servernode, ' ', 
             sizeof ( prcUserParams -> xu_servernode ));
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength = sizeof ( prcUserParams -> xu_servernode );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    SAPDB_memcpy ( prcUserParams -> xu_servernode, sql80_OptArg, nArgLength );

    DBG3 (( MF__, "prcUserParams -> xu_servernode = \"%s\"",
            _PToC ( prcUserParams -> xu_servernode )));

    PUTBIT ( ucXuserType, sp4xu_sql_servernode );
    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
    DBGOUT;
}


/*------------------------------*/


static VOID MakeDBLang  (   tsp4_xuser_record  *prcUserParams, 
                            tsp4_xuserset       ucXuserType     )
{
    #undef  MF__
    #define MF__ MOD__"MakeDBLang"


    INT nArgLength;
    INT nLength;


    DBGIN;

    // get dblang
    memset ( prcUserParams -> xu_dblang, ' ', 
             sizeof ( prcUserParams -> xu_dblang ));
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength = sizeof ( prcUserParams -> xu_dblang );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    SAPDB_memcpy ( prcUserParams -> xu_dblang, sql80_OptArg, nArgLength );

    DBG3 (( MF__, "prcUserParams -> xu_dblang = \"%s\"",
            _PToC ( prcUserParams -> xu_dblang )));

    PUTBIT ( ucXuserType, sp4xu_sql_dblang );
    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
    DBGOUT;
}


/*------------------------------*/


static VOID MakeUserKey (   tsp4_xuser_record  *prcUserParams,
                            tsp4_xuserset       ucXuserType,
                            BOOLEAN             bEnvDBNameFound,
                            ERRORTEXT           acErrorText,
                            PBOOLEAN            pbOk                )
{
    #undef  MF__
    #define MF__ MOD__"MakeUserKey"


    INT     nLength;
    INT     nArgLength;
    APIRET  rc;


    DBGIN;

    // get userkey and ucXuserType
    nLength    = sizeof ( prcUserParams -> xu_key );
    nArgLength = (INT)strlen ( sql80_OptArg );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    if ( !strcmp ( sql80_OptArg, "prompt" ) || !strcmp ( sql80_OptArg, "PROMPT" )) {
        PUTBIT ( ucXuserType, sp4xu_sql_usermask );
    }
    else {
        // check for error conditions
        if ( TSTBIT ( ucXuserType, sp4xu_sql_userparms  )  |
             TSTBIT ( ucXuserType, sp4xu_sql_servernode )  |
             TSTBIT ( ucXuserType, sp4xu_sql_mode       )  |
             TSTBIT ( ucXuserType, sp4xu_sql_isolation  )  |
             TSTBIT ( ucXuserType, sp4xu_sql_timeout    )  |
             TSTBIT ( ucXuserType, sp4xu_sql_cachelimit )) {

            *pbOk = FALSE;

            DBG1 (( MF__, ERRMSG_WRONG_OPTION_SEQUENCE ));
            MSGD  (( ERR_WRONG_OPTION_SEQUENCE ));
            SAPDB_memcpy ( acErrorText, ERRMSG_WRONG_OPTION_SEQUENCE, 
                     sizeof ( ERRORTEXT ));
            sql46c_build_error_string ( acErrorText,
                                        ERRMSG_WRONG_OPTION_SEQUENCE, 0 );

            DBGOUT;

            return;
        }

        if ( !bEnvDBNameFound && TSTBIT ( ucXuserType, sp4xu_sql_serverdb )) {
            *pbOk = FALSE;

            DBG1 (( MF__, ERRMSG_WRONG_OPTION_SEQUENCE ));
            MSGD  (( ERR_WRONG_OPTION_SEQUENCE ));
            sql46c_build_error_string ( acErrorText,
                                        ERRMSG_WRONG_OPTION_SEQUENCE, 0 );

            DBGOUT;

            return;
        }

        // copy without '\0'
        if ( BLANKS_ONLY ( sql80_OptArg )) {
            memset ( prcUserParams -> xu_key, ' ', 
                     sizeof ( prcUserParams -> xu_key ));
            SAPDB_memcpy ( prcUserParams -> xu_key, DEFAULT_USER, 
                     sizeof ( prcUserParams -> xu_key ));
            PUTBIT ( ucXuserType, sp4xu_sql_userdefault );
            CLRBIT ( ucXuserType, sp4xu_sql_usermask );
        }
        else {
            memset ( prcUserParams -> xu_key, ' ', 
                     sizeof ( prcUserParams -> xu_key ));
            SAPDB_memcpy ( prcUserParams -> xu_key, sql80_OptArg, nArgLength );

            PUTBIT ( ucXuserType, sp4xu_sql_userkey );
            CLRBIT ( ucXuserType, sp4xu_sql_usermask );
            CLRBIT ( ucXuserType, sp4xu_sql_userdefault );
        }

        // calling getuser because of a specified user key
        rc = sql13u_getuser ( prcUserParams, NULL, acErrorText );

        if ( rc != NO_ERROR )
            *pbOk = FALSE;
//      else
//          MoveToUpper ( prcUserParams -> xu_serverdb, 
//                        sizeof ( prcUserParams -> xu_serverdb ));

        DBG3 (( MF__, "prcUserParams -> xu_key = \"%s\"",
                _PToC ( prcUserParams -> xu_key )));
        DBG3 (( MF__, "prcUserParams -> xu_servernode = \"%s\"",
                _PToC ( prcUserParams -> xu_servernode )));
        DBG3 (( MF__, "prcUserParams -> xu_serverdb = \"%s\"",
                _PToC ( prcUserParams -> xu_serverdb )));
        DBG3 (( MF__, "prcUserParams -> xu_user = \"%s\"",
                _PToC ( prcUserParams -> xu_user )));
        DBG3 (( MF__, "prcUserParams -> xu_sqlmode = \"%s\"",
                _PToC ( prcUserParams -> xu_sqlmode )));

        DBG3 (( MF__, "prcUserParams -> xu_cachelimit = %d",
                       prcUserParams -> xu_cachelimit ));
        DBG3 (( MF__, "prcUserParams -> xu_timeout = %d",
                       prcUserParams -> xu_timeout ));
        DBG3 (( MF__, "prcUserParams -> xu_isolation = %d",
                       prcUserParams -> xu_isolation ));
    }

    DBG3 (( MF__, "ucXuserType [ 0 ] = %d", ucXuserType [ 0 ] ));
    DBG3 (( MF__, "ucXuserType [ 1 ] = %d", ucXuserType [ 1 ] ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeSqlMode (   tsp4_xuser_record  *prcUserParams,
                            tsp4_xuserset       ucXuserType     )
{
    #undef  MF__
    #define MF__ MOD__"MakeSqlMode"


    INT nLength;
    INT nArgLength;


    DBGIN;

    // get sqlmode
    memset ( prcUserParams -> xu_sqlmode, ' ', 
             sizeof ( prcUserParams -> xu_sqlmode ));
    nLength    = sizeof ( prcUserParams -> xu_key );
    nArgLength = (INT)strlen ( sql80_OptArg );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    // copy without '\0'
    SAPDB_memcpy ( prcUserParams -> xu_sqlmode, sql80_OptArg, nArgLength );

    DBG3 (( MF__, "prcUserParams -> xu_sqlmode = \"%s\"",
            _PToC ( prcUserParams -> xu_sqlmode )));

    PUTBIT ( ucXuserType, sp4xu_sql_mode );
    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
    DBGOUT;
}


/*------------------------------*/


static VOID MakeIsolation   (   tsp4_xuser_record  *prcUserParams,
                                tsp4_xuserset       ucXuserType     )
{
    #undef  MF__
    #define MF__ MOD__"MakeIsolation"


    BOOLEAN bNoAlphaFound;
    INT     nArgLength;
    INT     nIndex;


    DBGIN;

    // get isolation level and test for the digits 0 through 9
    nArgLength    = (INT)strlen ( sql80_OptArg );
    bNoAlphaFound = TRUE;

    for ( nIndex = 0; nIndex < nArgLength; nIndex++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
    }

    if ( bNoAlphaFound )
        prcUserParams -> xu_isolation = atoi ( sql80_OptArg );

    DBG3 (( MF__, "prcUserParams -> xu_isolation = %d",
                   prcUserParams -> xu_isolation ));

    PUTBIT ( ucXuserType, sp4xu_sql_isolation );
    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
    DBGOUT;
}


/*------------------------------*/


static VOID MakeTimeOut (   tsp4_xuser_record  *prcUserParams,
                            tsp4_xuserset       ucXuserType     )
{
    #undef  MF__
    #define MF__ MOD__"MakeTimeOut"


    BOOLEAN bNoAlphaFound;
    INT     nArgLength;
    INT     nIndex;
    LONG    lTimeOut;


    DBGIN;

    // get timeout and test for the digits 0 through 9
    nArgLength    = (INT)strlen ( sql80_OptArg );
    bNoAlphaFound = TRUE;

    for ( nIndex = 0; nIndex < nArgLength; nIndex++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
    }

    if ( bNoAlphaFound )
      {
      lTimeOut = atol ( sql80_OptArg );

      if (( lTimeOut < 32768L ) && ( nArgLength <= 5 ))
        prcUserParams -> xu_timeout = (INT2)lTimeOut;
      else
        prcUserParams -> xu_timeout = (INT2)32767;
      }
    else
      prcUserParams -> xu_timeout = (INT2)32767;


    DBG3 (( MF__, "prcUserParams -> xu_timeout = %d",
                   prcUserParams -> xu_timeout ));

    PUTBIT ( ucXuserType, sp4xu_sql_timeout );
    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
    DBGOUT;
}


/*------------------------------*/


static VOID MakeCacheLimit  (   tsp4_xuser_record  *prcUserParams,
                                tsp4_xuserset       ucXuserType     )
{
    #undef  MF__
    #define MF__ MOD__"MakeCacheLimit"


    BOOLEAN bNoAlphaFound;
    INT     nArgLength;
    INT     nIndex;


    DBGIN;

    // get timeout and test for the digits 0 through 9
    nArgLength    = (INT)strlen ( sql80_OptArg );
    bNoAlphaFound = TRUE;

    for ( nIndex = 0; nIndex < nArgLength; nIndex++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
    }

    if ( bNoAlphaFound )
        prcUserParams -> xu_cachelimit = atoi ( sql80_OptArg );

    DBG3 (( MF__, "prcUserParams -> xu_cachelimit = %d",
                   prcUserParams -> xu_cachelimit ));

    PUTBIT ( ucXuserType, sp4xu_sql_cachelimit );
    CLRBIT ( ucXuserType, sp4xu_sql_usermask );
    DBGOUT;
}



/*------------------------------*/


static VOID MakePrecompilerInputFn  (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerInputFn"


    INT             nArgLength;
    INT             nLength;
    INT             nCount;
    PSZ             pszBegin;
    PSZ             pszTmp1;
    PSZ             pszTmp2;
    PSZ             pszEnd;


    DBGIN;

    // get file name
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength    = sizeof ( prcArgsOptions ->
                          variant.C_sp4co_sql_precompiler.opt_modulefn_F );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_fnind_F =
                                                    (INT2) sql80_OptInd - 1;
    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulefn_F,
             sql80_OptArg, nArgLength );

    // extract file name that means: no path and no suffix
    // ( path/modulename.suffix )
    pszEnd   = strrchr ( sql80_OptArg, '.' );
    pszTmp1  = strrchr ( sql80_OptArg, '\\' );
    pszTmp2  = strrchr ( sql80_OptArg, '/' );

    pszBegin = max( pszTmp1, pszTmp2 );


    if ( pszBegin == NULL )
        pszBegin = sql80_OptArg;
    else
        pszBegin++;

    if (( pszEnd == NULL ) || (pszEnd <= pszBegin))
        pszEnd = sql80_OptArg + strlen ( sql80_OptArg );

    nLength = sizeof ( prcArgsOptions ->
                       variant.C_sp4co_sql_precompiler.opt_modulename_F );
    nCount  = (INT)(pszEnd - pszBegin);

    if ( nCount > nLength )
        nCount = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulename_F,
             pszBegin, nCount );

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulenamel_F = nCount;

    DBG3 (( MF__, "opt_modulefn = \"%s\"",
            _PToC ( prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_modulefn_F )));
    DBG3 (( MF__, "opt_modulename = \"%s\"",
            _PToC ( prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_modulename_F )));
    DBG3 (( MF__, "opt_modulenamel= %d",
            prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_modulenamel_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakePrecompilerTraceFn  (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerTraceFn"


    INT nArgLength;
    INT nLength;


    DBGIN;

    // get trace name
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength    = sizeof ( prcArgsOptions ->
                          variant.C_sp4co_sql_precompiler.opt_tracefile_F );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tracefile_F,
             sql80_OptArg, nArgLength );

    DBG3 (( MF__, "opt_tracefile = \"%s\"",
            _PToC ( prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_tracefile_F )));

    DBGOUT;
}


/*------------------------------*/


static VOID MakePrecompilerProgFn   (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerProgFn"


    INT nArgLength;
    INT nLength;


    DBGIN;

    // get prog name and nLength of prog name
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength    = sizeof ( prcArgsOptions ->
                          variant.C_sp4co_sql_precompiler.opt_progname_F );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_prognamel_F = nArgLength;
    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_progname_F,
             sql80_OptArg, nArgLength );

    DBG3 (( MF__, "opt_prognamel = %d",
            prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_prognamel_F ));
    DBG3 (( MF__, "opt_progname = \"%s\"",
            _PToC ( prcArgsOptions ->
                    variant.C_sp4co_sql_precompiler.opt_progname_F )));

    DBGOUT;
}


/*------------------------------*/


static VOID MakePrecompilerMargin   (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerMargin"


    BOOLEAN         bEndMarginFound;
    BOOLEAN         bNoAlphaFound;
    INT             nArgLength;
    INT             nLength;
    INT             nLengthFirst;
    INT             nIndex;
    PSZ             pszStartAddr;
    PSZ             pszCommaAddr ;
    C4              acTmpMargin;


    DBGIN;

    // get begin margin determinate comma position, the start pos of sql80_OptArg
    // and the begin margin string nLength
    bEndMarginFound = TRUE;
    nArgLength      = (INT)strlen ( sql80_OptArg );
    pszCommaAddr    = strchr ( sql80_OptArg, ',' );
    pszStartAddr    = sql80_OptArg;
    nLength         = (INT)(pszCommaAddr  - pszStartAddr);

    // check nLength
    if ( !pszCommaAddr  ) {
        bEndMarginFound = FALSE;
        nLength         = nArgLength;
    }

    // convert string to int and take over it
    nLengthFirst  = nLength + 1;
    bNoAlphaFound = TRUE;

    memset ( acTmpMargin, ' ', sizeof ( acTmpMargin ));

    for ( nIndex = 0; ( nIndex < nLength ) &&
                      ( nIndex < sizeof ( acTmpMargin)); nIndex++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
        else
            acTmpMargin [ nIndex ] = sql80_OptArg [ nIndex ];
    }

    if ( bNoAlphaFound )
        prcArgsOptions ->
        variant.C_sp4co_sql_precompiler.opt_begmar_F = atoi ( acTmpMargin );

    // get end margin ',' if it was found
    if ( bEndMarginFound ) {
        // get the beginning of end margin and determinate nLength of
        // end margin
        pszCommaAddr ++;
        nLength =  (INT)(pszStartAddr  + nArgLength - pszCommaAddr);

        // convert string to int and take over it
        bNoAlphaFound = TRUE;

        memset ( acTmpMargin, ' ', sizeof ( acTmpMargin ));

        for ( nIndex = 0; ( nIndex < nLength ) &&
                          ( nIndex < sizeof ( acTmpMargin )); nIndex++ ) {
            if ( isalpha ( sql80_OptArg [ nIndex + nLengthFirst ] )) {
                bNoAlphaFound = FALSE;

                break;
            }
            else
                acTmpMargin [ nIndex ] = sql80_OptArg [ nIndex + nLengthFirst ];
        }

        if ( bNoAlphaFound )
            prcArgsOptions ->
            variant.C_sp4co_sql_precompiler.opt_endmar_F = atoi ( acTmpMargin );
    }

    DBG3 (( MF__, "opt_begmar = %d",
            prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_begmar_F ));

    DBG3 (( MF__, "opt_endmar = %d",
            prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_endmar_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakePrecompilerDateTime (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerDateTime"


    DBGIN;

    // get date type
    if ( !strcmp ( sql80_OptArg, "iso" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 2;

    if ( !strcmp ( sql80_OptArg, "usa" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 3;

    if ( !strcmp ( sql80_OptArg, "eur" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 4;

    if ( !strcmp ( sql80_OptArg, "jis" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 5;

    DBG3 (( MF__, "opt_datetime = %d",
              prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_datetime_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakePrecompilerSysTpMon (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerSysTpMon"


    BOOLEAN         bTpMonFound;
    INT             nArgLength;
    INT             nLength1;
    INT             nLength2;
    PSZ             pszStartAddr ;
    PSZ             pszCommaAddr ;
    CHAR            acTmpSys [ 9 ];


    DBGIN;

    // get sys, determinate comma position, the start position of sql80_OptArg
    // and the sys string nLength

    bTpMonFound  = TRUE;
    nArgLength   = (INT)strlen ( sql80_OptArg );
    pszCommaAddr = strchr ( sql80_OptArg, ',' );
    pszStartAddr = sql80_OptArg;
    nLength2     = (INT)(pszCommaAddr  - pszStartAddr);

    // check nLength
    if ( !pszCommaAddr  ) {
        bTpMonFound = FALSE;
        nLength2    = nArgLength;
    }

    // copy with '\0'
    if ( nLength2 < sizeof ( acTmpSys )) {
        strncpy ( acTmpSys, sql80_OptArg, ( size_t ) nLength2 );
        acTmpSys [ nLength2 ] = '\0';
    }
    else {
        strncpy ( acTmpSys, sql80_OptArg, sizeof ( acTmpSys ) - 1 );
        acTmpSys [ sizeof ( acTmpSys ) - 1 ] = '\0';
    }

    DBG3 (( MF__, "acTmpSys = \"%s\"", acTmpSys ));

    // determinate tpmon
    if ( !strcmp ( acTmpSys, "cics" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmon_F = 1;

    if ( !strcmp ( acTmpSys, "pccics" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmon_F = 2;

    if ( !strcmp ( acTmpSys, "aim" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmon_F = 3;

    if ( !strcmp ( acTmpSys, "utm" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmon_F = 4;

    DBG3 (( MF__, "opt_tpmon = %d",
            prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmon_F ));

    // get tpmonid if ',' was found
    if ( bTpMonFound ) {
        // get the beginning of tpmonid and the nLength of tpmonid field,
        // determinate nLength of tpmonid
        pszCommaAddr ++;
        nLength1 = sizeof ( prcArgsOptions ->
                            variant.C_sp4co_sql_precompiler.opt_tpmonid_F );
        nLength2 = (INT)(pszStartAddr  + nArgLength - pszCommaAddr);

        // check nLength

        if ( nLength2 > nLength1 )
            nLength2 = nLength1;


        // copy without '\0'
        SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_tpmonid_F,
                 pszCommaAddr , nLength2 );

        DBG3 (( MF__, "opt_tpmonid = \"%s\"",
                _PToC ( prcArgsOptions ->
                        variant.C_sp4co_sql_precompiler.opt_tpmonid_F )));
    }

    DBGOUT;
}


/*------------------------------*/


static VOID MakePrecompilerMode (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerMode"


    DBGIN;

    // get sql mode
    if ( !strcmp ( sql80_OptArg, "syntax" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_mode_F = 1;

    if ( !strcmp ( sql80_OptArg, "nocheck" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_mode_F = 0;

    DBG3 (( MF__, "opt_mode = %d",
            prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_mode_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakePrecompilerCAnsi    (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerCAnsi"


    DBGIN;

    // get sql cansi

    if ( !strcmp ( sql80_OptArg, "cansi" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_cansi_F = 1;
    else
    if ( !strcmp ( sql80_OptArg, "cplus" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_cansi_F = 2;
    else
    if ( !strcmp ( sql80_OptArg, "ansi85" ))
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_cansi_F = 3;

    DBG3 (( MF__, "opt_cansi = %d",
            prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_cansi_F ));
}

/*------------------------------*/

static VOID MakePrecompilerUnicode (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerUnicode"


    DBGIN;

    // get sql mode
   if ( !strcmp ( sql80_OptArg, "unicode" ))
     prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_unicode_F = TRUE ;

   DBG3 (( MF__, "opt_unicode = %d",
           prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_unicode_F ));

    DBGOUT;
}


/*------------------------------*/

static VOID MakePrecompilerPacketSize
            (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerPacketSize"


    BOOLEAN bNoAlphaFound;
    INT     nArgLength;
    INT     nIndex;


    DBGIN;

    nArgLength    = (INT)strlen ( sql80_OptArg );
    bNoAlphaFound = TRUE;

    for ( nIndex = 0; nIndex < nArgLength; nIndex ++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
    }

    if ( bNoAlphaFound )
        prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_packetsize_F = 
                          atoi ( sql80_OptArg );

    DBG3 (( MF__, "opt_packetsize = %d",prcArgsOptions -> variant.C_sp4co_sql_precompiler.opt_packetsize_F ));

    DBGOUT;
}

/*------------------------------*/


static VOID MakePrecompilerRunTimeTraceFn
            (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerRunTimeTraceFn"


    INT nArgLength;
    INT nLength;


    DBGIN;

    // get trace name
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength    = sizeof ( prcArgsOptions ->
                          variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F,
             sql80_OptArg, nArgLength );

    DBG3 (( MF__, "opt_rtracefile = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F )));

    DBGOUT;
}

/*------------------------------*/

static VOID MakePrecompilerRunTimeStmtCnt
            (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerRunTimeStmtCnt"


    BOOLEAN bNoAlphaFound;
    INT     nArgLength;
    INT     nIndex;


    DBGIN;

    nArgLength    = (INT)strlen ( sql80_OptArg );
    bNoAlphaFound = TRUE;

    for ( nIndex = 0; nIndex < nArgLength; nIndex ++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
    }

    if ( bNoAlphaFound )
        prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rstmt_cnt_F = atoi ( sql80_OptArg );

    DBG3 (( MF__, "opt_rstmt_cnt = %d",prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rstmt_cnt_F ));

    DBGOUT;
}

/*------------------------------*/


static VOID MakePrecompilerRunTimeLimit
            (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerRunTimeLimit"


    BOOLEAN bNoAlphaFound;
    INT     nArgLength;
    INT     nIndex;


    DBGIN;

    nArgLength    = (INT)strlen ( sql80_OptArg );
    bNoAlphaFound = TRUE;

    for ( nIndex = 0; nIndex < nArgLength; nIndex ++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
    }

    if ( bNoAlphaFound )
        prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtime_limit_F = atoi ( sql80_OptArg );

    DBG3 (( MF__, "opt_rtime_limit = %d",prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rtime_limit_F ));

    DBGOUT;
}

/*------------------------------*/


static VOID MakePrecompilerRunTimeMFetch
            (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakePrecompilerRunTimeMFetch"


    BOOLEAN bNoAlphaFound;
    INT     nArgLength;
    INT     nIndex;


    DBGIN;

    // get mfetch count and test for the digits 0 through 9
    nArgLength    = (INT)strlen ( sql80_OptArg );
    bNoAlphaFound = TRUE;

    for ( nIndex = 0; nIndex < nArgLength; nIndex ++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
    }

    if ( bNoAlphaFound )
        prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rmfetch_F = atoi ( sql80_OptArg );

    DBG3 (( MF__, "opt_rmfetch = %d",prcArgsOptions -> variant.C_sp4co_sql_pc_runtime.opt_rmfetch_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeDialogComponentRFn  (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeDialogComponentRFn"


    INT nArgLength;
    INT nLength;


    DBGIN;

    // get runfile
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength    = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
             sql80_OptArg, nArgLength );

    // get command mode
    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run;

    DBG3 (( MF__, "opt_runfile = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F )));
    DBG3 (( MF__, "opt_comm_mode = %d",
            prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeDialogComponentBFn  (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeDialogComponentBFn"


    INT nArgLength;
    INT nLength;


    DBGIN;

    // get runfile
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength    = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
             sql80_OptArg, nArgLength );

    // get command mode
    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch;

    DBG3 (( MF__, "opt_runfile = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F )));
    DBG3 (( MF__, "opt_comm_mode = %d",
            prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeDialogComponentObj (   tsp4_args_options  *prcArgsOptions,
                                       tsp4_command_mode   OptCommMode,
                                       INT                 nMaxObjNames,
                                       PBOOLEAN            pbOk,
                                       ERRORTEXT           acErrorText )
{
    #undef  MF__
    #define MF__ MOD__"MakeDialogComponentObj"

    CTU_REC         CtU;
    INT             nLengthObjectOwner;
    INT             nLengthObjectName1;
    INT             nLengthObjectName2;
    INT             nLengthObject;
    INT             nLength1;
    INT             nLength2;

    DBGIN;

    // initialize
    nLengthObjectOwner = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F );
    nLengthObjectName1 = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F );
    nLengthObjectName2 = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F );

    if ( nMaxObjNames == 2 ) {
        CheckIdents ( &CtU, '.', 3, pbOk, acErrorText );

        if ( !*pbOk ) {
            DBGOUT;
            return;
        }

        if ( ( CtU.lIdentLen[0] > 0 ) &&
             ( CtU.lIdentLen[1] > 0 ) &&
             ( CtU.lIdentLen[2] > 0 ) ) {

            if ( CtU.lIdentLen[0] > nLengthObjectOwner )
                nLengthObject = nLengthObjectOwner;
            else
                nLengthObject = CtU.lIdentLen[0];

            if ( CtU.lIdentLen[1] > nLengthObjectName1 )
                nLength1 = nLengthObjectName1;
            else
                nLength1 = CtU.lIdentLen[1];

            if ( CtU.lIdentLen[2] > nLengthObjectName2 )
                nLength2 = nLengthObjectName2;
            else
                nLength2 = CtU.lIdentLen[2];

            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F,
                     CtU.pszIdent[0], nLengthObject );

            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                     CtU.pszIdent[1], nLength1 );

            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F,
                     CtU.pszIdent[2], nLength2 );

            if ( CtU.bIdentToUpper[0] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F,
                              nLengthObject );
            }

            if ( CtU.bIdentToUpper[1] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                              nLength1 );
            }

            if ( CtU.bIdentToUpper[2] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F,
                              nLength2 );
            }
        }
        else if ( ( CtU.lIdentLen[0] >  0 ) &&
                  ( CtU.lIdentLen[1] >  0 ) &&
                  ( CtU.lIdentLen[2] == 0 ) ) {

            if ( CtU.lIdentLen[0] > nLengthObjectName1 )
                nLength1 = nLengthObjectName1;
            else
                nLength1 = CtU.lIdentLen[0];

            if ( CtU.lIdentLen[1] > nLengthObjectName2 )
                nLength2 = nLengthObjectName2;
            else
                nLength2 = CtU.lIdentLen[1];


            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                     CtU.pszIdent[0], nLength1 );

            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F,
                     CtU.pszIdent[1], nLength2 );


            if ( CtU.bIdentToUpper[0] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                              nLength1 );
            }

            if ( CtU.bIdentToUpper[1] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F,
                              nLength2 );
            }
        }
        else if ( ( CtU.lIdentLen[0] >  0 ) &&
                  ( CtU.lIdentLen[1] == 0 ) &&
                  ( CtU.lIdentLen[2] == 0 ) ) {

            if ( CtU.lIdentLen[0] > nLengthObjectName1 )
                nLength1 = nLengthObjectName1;
            else
                nLength1 = CtU.lIdentLen[0];


            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                     CtU.pszIdent[0], nLength1 );


            if ( CtU.bIdentToUpper[0] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                              nLength1 );
            }
        }
        else {
            *pbOk = FALSE;
            SAPDB_memcpy ( acErrorText, ERRMSG_ILLEGAL_DIALOG_OBJ, sizeof ( ERRORTEXT ) );
            sql46c_build_error_string ( acErrorText, ERRMSG_ILLEGAL_DIALOG_OBJ,
                                        0 );
            DBGOUT;
            return;
        }
    }
    // determinate object owner and name1
    else if ( nMaxObjNames == 1 ) {
        CheckIdents ( &CtU, '.', 2, pbOk, acErrorText );

        if ( !*pbOk ) {
            DBGOUT;
            return;
        }

        if ( ( CtU.lIdentLen[0] >  0 ) &&
             ( CtU.lIdentLen[1] >  0 ) &&
             ( CtU.lIdentLen[2] == 0 ) ) {

            if ( CtU.lIdentLen[0] > nLengthObjectOwner )
                nLengthObject = nLengthObjectOwner;
            else
                nLengthObject = CtU.lIdentLen[0];

            if ( CtU.lIdentLen[1] > nLengthObjectName1 )
                nLength1 = nLengthObjectName1;
            else
                nLength1 = CtU.lIdentLen[1];

            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F,
                     CtU.pszIdent[0], nLengthObject );

            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                     CtU.pszIdent[1], nLength1 );

            if ( CtU.bIdentToUpper[0] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F,
                              nLengthObject );
            }

            if ( CtU.bIdentToUpper[1] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                              nLength1 );
            }

        }
        else if ( ( CtU.lIdentLen[0] >  0 ) &&
                  ( CtU.lIdentLen[1] == 0 ) &&
                  ( CtU.lIdentLen[2] == 0 ) ) {

            if ( CtU.lIdentLen[0] > nLengthObjectName1 )
                nLength1 = nLengthObjectName1;
            else
                nLength1 = CtU.lIdentLen[0];


            SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                     CtU.pszIdent[0], nLength1 );


            if ( CtU.bIdentToUpper[0] ) {
                MoveToUpper ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
                              nLength1 );
            }
        }
        else {
            *pbOk = FALSE;
            sql46c_build_error_string ( acErrorText, ERRMSG_ILLEGAL_DIALOG_OBJ,
                                        0 );
            DBGOUT;
            return;
        }
    }
    else {
        *pbOk = FALSE;
        sql46c_build_error_string ( acErrorText, ERRMSG_ILLEGAL_DIALOG_OBJ, 0 );
        DBGOUT;
        return;
    }

    // get command mode
    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = OptCommMode;

    DBG3 (( MF__, "opt_obj_owner = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_owner_F )));
    DBG3 (( MF__, "opt_obj_name1 = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F )));
    DBG3 (( MF__, "opt_obj_name2 = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name2_F )));
    DBG3 (( MF__, "opt_comm_mode = %d",
            prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F ));

    DBGOUT;
    return;
}


/*------------------------------*/


static VOID MakeDialogComponentCodeType
            (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeDialogComponentCodeType"


    DBGIN;

    // get code type
    if ( !strcmp ( sql80_OptArg, "asc" ))
        prcArgsOptions ->
        variant.C_sp4co_sql_easy.opt_code_F = is_ascii;

    if ( !strcmp ( sql80_OptArg, "ebc" ))
        prcArgsOptions ->
        variant.C_sp4co_sql_easy.opt_code_F = is_ebcdic;

    DBG3 (( MF__, "opt_code = %d",
            prcArgsOptions -> variant.C_sp4co_sql_easy.opt_code_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeDialogComponentExport ( tsp4_args_options  *prcArgsOptions )
{
    #undef  MF__
    #define MF__ MOD__"MakeDialogComponentExport"


    BOOLEAN         bFNameFound;
    INT             nArgLen;
    INT             nLength1, nLength2;
    PSZ             pszStartAddr ;
    PSZ             pszCommaAddr ;


    DBGIN;

    // get object name1 :
    // determinate comma position, the start position of sql80_OptArg and the object
    // name1 string length

    bFNameFound  = TRUE;
    nArgLen      = (INT)strlen ( sql80_OptArg );
    pszCommaAddr = strchr ( sql80_OptArg, ',' );
    pszStartAddr = sql80_OptArg;
    nLength2     = (INT)(pszCommaAddr  - pszStartAddr);
    nLength1     = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F );

    // check length
    if ( !pszCommaAddr  ) {
        bFNameFound = FALSE;
        nLength2    = nArgLen;
    }

    if ( nLength2 > nLength1 )
        nLength2 = nLength1;

    // copy without '\0'
    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F,
             sql80_OptArg, nLength2 );

    // get runfile if ',' was found
    if ( bFNameFound ) {
        // get the beginning of runfile and the length of runfile field,
        // determinate length of runfile
        pszCommaAddr ++;
        nLength1 = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F );

        nLength2 = (INT)(pszStartAddr  + nArgLen - pszCommaAddr);

        // check length
        if ( nLength2 > nLength1 )
            nLength2 = nLength1;

        // copy without '\0'
        SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
                 pszCommaAddr , nLength2 );
    }

    // get command mode
    prcArgsOptions ->
    variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_export;

    DBG3 (( MF__, "opt_obj_name1 = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_object_name1_F )));
    DBG3 (( MF__, "opt_runfile = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F )));
    DBG3 (( MF__, "opt_comm_mode = %d",
            prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeDialogComponentImport ( tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeDialogComponentImport"


    INT nArgLen;
    INT nLength;


    DBGIN;

    // get runfile
    nArgLen = (INT)strlen ( sql80_OptArg );
    nLength = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F );

    if ( nArgLen > nLength )
        nArgLen = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_runfile_F,
             sql80_OptArg, nArgLen );

    // get command mode
    prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_import;

    DBG3 (( MF__, "opt_comm_mode = %d",
            prcArgsOptions -> variant.C_sp4co_sql_easy.opt_comm_mode_F ));

    DBG3 (( MF__, "opt_runfile = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_easy.opt_parameter_F )));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeAuditCntrl  (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeAuditCntrl"


    INT nArgLen;
    INT nLength;


    DBGIN;

    // get audit control
    nArgLen = (INT)strlen ( sql80_OptArg );
    nLength = sizeof ( prcArgsOptions ->
                       variant.C_sp4co_sql_audit.opt_aud_control_F );

    if ( nArgLen > nLength )
        nArgLen = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_control_F,
             sql80_OptArg, nArgLen );

    DBG3 (( MF__, "opt_aud_control = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_control_F )));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeAuditLogfile    (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeAuditLogfile"


    INT nArgLen;
    INT nLength;


    DBGIN;

    // get logfile
    nArgLen = (INT)strlen ( sql80_OptArg );
    nLength = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_logfile_F );

    if ( nArgLen > nLength )
        nArgLen = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_logfile_F,
             sql80_OptArg, nArgLen );

    DBG3 (( MF__, "opt_aud_logfile = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_logfile_F )));

    DBGOUT;
}

/*------------------------------*/

static VOID MakeAuditLogBlockSize (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeAuditLogBlockSize"

    BOOLEAN bNoAlphaFound;
    INT     nArgLength;
    INT     nIndex;


    DBGIN;

    // get blocksize for audit logfile, check the digits 0 through 9
    nArgLength    = (INT)strlen ( sql80_OptArg );
    bNoAlphaFound = TRUE;

    for ( nIndex = 0; nIndex < nArgLength; nIndex++ ) {
        if ( isalpha ( sql80_OptArg [ nIndex ] )) {
            bNoAlphaFound = FALSE;

            break;
        }
    }

    if ( bNoAlphaFound )
      prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_log_blocksize_F = 
                        atoi ( sql80_OptArg ) ;

    DBG3 (( MF__, "opt_aud_log_blocksize = \"%d\"",
             prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_log_blocksize_F ));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeAuditOutput (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeAuditOutput"


    INT nArgLen;
    INT nLength;


    DBGIN;

    // get audit output
    nArgLen = (INT)strlen ( sql80_OptArg );
    nLength = sizeof ( prcArgsOptions ->
                       variant.C_sp4co_sql_audit.opt_aud_output_F );

    if ( nArgLen > nLength )
        nArgLen = nLength;

    SAPDB_memcpy ( prcArgsOptions ->
           variant.C_sp4co_sql_audit.opt_aud_output_F, sql80_OptArg, nArgLen );

    DBG3 (( MF__, "opt_aud_output = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_audit.opt_aud_output_F )));

    DBGOUT;
}


/*------------------------------*/

static VOID MakeUserXComponentBFn  (   tsp4_args_options  *prcArgsOptions  )
{
    #undef  MF__
    #define MF__ MOD__"MakeUserXComponentBFn"


    INT nArgLength;
    INT nLength;


    DBGIN;

    // get runfile
    nArgLength = (INT)strlen ( sql80_OptArg );
    nLength    = sizeof ( prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_runfile_F );

    if ( nArgLength > nLength )
        nArgLength = nLength;

    SAPDB_memcpy ( prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_runfile_F,
             sql80_OptArg, nArgLength );

    // get command mode
    prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F = sp4cm_sql_batch;

    DBG3 (( MF__, "opt_ux_runfile = \"%s\"",
            _PToC ( prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_runfile_F )));
    DBG3 (( MF__, "opt_comm_mode = %d",
            prcArgsOptions -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F ));

    DBGOUT;
}

/*------------------------------*/


static VOID MakeUserPasswordLine (  PARG_LINE_REC   prcArgLine,
                                    PBOOLEAN        pbUserNameFound,
                                    PBOOLEAN        pbPasswordFound     )
{
    #undef  MF__
    #define MF__ MOD__"MakeUserPasswordLine"


    INT nSeparatorIndex;
    INT nNull;


    DBGIN;

    // initialize internel vars
    *pbUserNameFound = TRUE;
    *pbPasswordFound = TRUE;

    memset (( PCHAR ) prcArgLine -> username, ' ',
            sizeof ( prcArgLine -> username ));
    memset (( PCHAR ) prcArgLine -> password, ' ',
            sizeof ( prcArgLine -> password ));

    nSeparatorIndex = 0;
    nNull           = 0;

    // get user name
    MakeLine ( sql80_OptArg, &nSeparatorIndex, sizeof ( prcArgLine -> username ),
               ',', &nNull, prcArgLine -> username );

    DBG3 (( MF__, "prcArgLine -> username = \"%s\"",
            _PToC ( prcArgLine -> username )));

    // get password if ',' was found
    if ( sql80_OptArg [ nSeparatorIndex ] == ',' ) {
        nNull = 0;
        nSeparatorIndex++;

        MakeLine ( sql80_OptArg, &nSeparatorIndex, nSeparatorIndex +
                 ( INT ) sizeof ( prcArgLine -> password ),
                   ' ', &nNull, prcArgLine -> password );

        DBG3 (( MF__, "prcArgLine -> password = \"%s\"",
                _PToC ( prcArgLine -> password )));
    }
    else {
        // get password, but first look for ','
        nSeparatorIndex = StrChr  ( sql80_OptArg, ',',
                                    nSeparatorIndex, MAXARGLINELENGTH );

        DBG3 (( MF__, "nSeparatorIndex = %d", nSeparatorIndex ));

        if ( nSeparatorIndex != 0 ) {
            nNull = 0;
            nSeparatorIndex++;
            MakeLine ( sql80_OptArg, &nSeparatorIndex,
                     ( INT ) sizeof ( prcArgLine -> password ) +
                       nSeparatorIndex, ' ', &nNull,
                       prcArgLine -> password );

            DBG3 (( MF__, "prcArgLine -> password = \"%s\"",
                    _PToC ( prcArgLine -> password )));
        }
    }

    DBGOUT;
}


/*------------------------------*/


static VOID MakeDBName  (   PCHAR*          apszArgv,
                            PARG_LINE_REC   prcArgLine,
                            PBOOLEAN        pbDBNameFound   )
{
    #undef  MF__
    #define MF__ MOD__"MakeDBName"


    INT     nNull;
    PSZ     pszBegin;
    PSZ     pszTmp1;
    PSZ     pszTmp2;
    CHAR    acDBNameString [ MX_DBNAME + 1 ];


    DBGIN;

    // initialize vars
    *pbDBNameFound = TRUE;
    nNull = 0;

    memset (( PCHAR ) prcArgLine -> dbname, ' ', sizeof ( prcArgLine -> dbname ));
    memset (( PCHAR ) acDBNameString, ' ', sizeof ( acDBNameString ));

    // get the program name because only utility needs
    // the dbname quoted in "'" ( 'dbname' )
    pszTmp1  = strrchr ( apszArgv [ 0 ], '\\' );
    pszTmp2  = strrchr ( apszArgv [ 0 ], '/' );

    pszBegin = max( pszTmp1, pszTmp2 );

    if ( pszBegin == NULL )
        pszBegin = apszArgv [ 0 ];
    else
        pszBegin++;

    DBG3 (( MF__, "pszBegin = 0x%x", pszBegin ));

    if ( !strcmp ( pszBegin, "utility" )) {
        strcpy  ( acDBNameString, "'" );
        strncat ( acDBNameString, sql80_OptArg, sizeof ( acDBNameString ) - 3 );
        strcat  ( acDBNameString, "'" );

        DBG3 (( MF__, "utility was called" ));
    }
    else {
        strncpy ( acDBNameString, sql80_OptArg, sizeof ( acDBNameString ) - 1 );
        acDBNameString [ sizeof ( acDBNameString ) - 1 ] = 0;

        DBG3 (( MF__, "utility was not called" ));
    }

    DBG3 (( MF__, "acDBNameString = \"%s\"", _PToC (acDBNameString )));

    // get data base name
    MakeLine ( acDBNameString, &nNull, sizeof ( prcArgLine -> dbname ), ' ',
               &nNull, prcArgLine -> dbname );

    MoveToUpper ( prcArgLine -> dbname, sizeof ( prcArgLine -> dbname ));

    DBG3 (( MF__, "prcArgLine -> dbname = \"%s\"",
            _PToC ( prcArgLine -> dbname )));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeCmd (   PCHAR           pszCmdType,
                        PARG_LINE_REC   prcArgLine,
                        PBOOLEAN        pbCmdFound,
                        PBOOLEAN        pbFileNameFound     )
{
    #undef  MF__
    #define MF__ MOD__"MakeCmd"


    INT nNull;


    DBGIN;

    DBG3 (( MF__, "pszCmdType = \"%s\"", pszCmdType ));

    // initialize internel vars
    memset (( PCHAR ) prcArgLine -> cmd, ' ', sizeof ( prcArgLine -> cmd ));
    memset (( PCHAR ) prcArgLine -> filename, ' ', sizeof ( prcArgLine -> filename ));

    nNull            = 0;
    *pbCmdFound      = TRUE;
    *pbFileNameFound = TRUE;

    // get command
    MakeLine ( pszCmdType, &nNull, sizeof ( prcArgLine -> cmd ), ' ',
               &nNull, prcArgLine -> cmd );

    DBG3 (( MF__, "prcArgLine -> cmd = \"%s\"",
            _PToC ( prcArgLine -> cmd )));

    // get file name
    nNull = 0;
    MakeLine ( sql80_OptArg, &nNull, sizeof ( prcArgLine -> filename ), ' ',
               &nNull, prcArgLine -> filename );

    DBG3 (( MF__, "prcArgLine -> filename = \"%s\"",
            _PToC ( prcArgLine -> filename )));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeArgLine (   INT             nArgc,
                            PCHAR*          apszArgv,
                            PARG_LINE_REC   prcArgLine,
                            PBOOLEAN        pbArgumentsFound,
                            PBOOLEAN        pbCmdFound,
                            PBOOLEAN        pbFileNameFound,
                            INT             nOptionChar         )
{
    #undef  MF__
    #define MF__ MOD__"MakeArgLine"


    INT     nTestChar0;
    INT     nTestChar1;
    INT     nArgumentNo;
    INT     nOutIndex;
    INT     nSize;
    INT     nTmpSize;
    INT     nNull;
    PCHAR   pszCmdType;


    DBGIN;

    if ( nOptionChar == '?' ) {
        // look for option names - if found take over
        DBG3 (( MF__, "sql80_OptInd  ( nOptionChar == '?' ) = %d", sql80_OptInd ));

        nTestChar0 = apszArgv [ sql80_OptInd - 1 ] [ 0 ];
        nTestChar1 = apszArgv [ sql80_OptInd - 1 ] [ 1 ];

        DBG3 (( MF__, "argv [ sql80_OptInd - 1 ] = \"%s\"", apszArgv [ sql80_OptInd - 1 ] ));
        DBG3 (( MF__, "nTestChar0          = %c", nTestChar0 ));
        DBG3 (( MF__, "nTestChar1          = %c", nTestChar1 ));


        if (( nTestChar0 == '-' || nTestChar0 == '/' ) &&
            ( nTestChar1 == 'r' || nTestChar1 == 'b' )) {
            *pbCmdFound = TRUE;

            memset (( PCHAR ) prcArgLine -> cmd, ' ',
                    sizeof ( prcArgLine -> cmd ));

            switch ( nTestChar1 ) {
                case 'r' :
                    pszCmdType = "run";
                break;

                case 'b' :
                    pszCmdType = "batch";
                break;
            }

            // get command
            nNull = 0;
            MakeLine ( pszCmdType, &nNull, sizeof ( prcArgLine -> cmd ),
                       ' ', &nNull, prcArgLine -> cmd );

            DBG3 (( MF__, "prcArgLine -> cmd = \"%s\"",
                    _PToC ( prcArgLine -> cmd )));

            // 'STDIN' becomes filename
            *pbFileNameFound = TRUE;
            memset (( PCHAR ) prcArgLine -> filename, ' ',
                    sizeof ( prcArgLine -> filename ));
            nNull = 0;
            MakeLine ( "STDIN", &nNull, sizeof ( prcArgLine -> filename ),
                       ' ', &nNull, prcArgLine -> filename );

            DBG3 (( MF__, "prcArgLine -> filename = \"%s\"",
                    _PToC ( prcArgLine -> filename )));
        }
        else {
            // no regular option, reduce sql80_OptInd but be sure to do this only
            // you have found an '-?' expression
            // ? : means only one character
            if ( sql80_OptInd > 1                              &&
                 strlen ( apszArgv [ sql80_OptInd - 1 ] ) == 2 &&
                 ( nTestChar0 == '-' ||  nTestChar0 == '/' )) {

                sql80_OptInd--;
            }
        }
    }

    // take over args into arg line
    DBG3 (( MF__, "sql80_OptInd = %d", sql80_OptInd ));
    nOutIndex = -1;
    nSize = sizeof ( prcArgLine -> arguments );
    nTmpSize = sizeof ( prcArgLine -> arguments );
    *pbArgumentsFound = TRUE;
    memset (( PCHAR ) prcArgLine -> arguments, ' ',
            sizeof ( prcArgLine -> arguments ));

    for ( nArgumentNo = sql80_OptInd; nArgumentNo < nArgc; nArgumentNo++ ) {
        nNull = 0;
        nOutIndex++;
        MakeLine ( apszArgv [ nArgumentNo ], &nNull, nTmpSize, ' ',
                   &nOutIndex, prcArgLine -> arguments );
        nTmpSize = nSize - nOutIndex + 1;
    }

    DBG3 (( MF__, "prcArgLine -> arguments = \"%s\"",
            _PToC ( prcArgLine -> arguments )));

    DBGOUT;
}


/*------------------------------*/


static VOID MakeLine    (   PCHAR   pszArgIn,
                            PINT    pnArgInIndex,
                            INT     nLineLength,
                            INT     nSeparator,
                            PINT    pnArgOutIndex,
                            PCHAR   pszArgOut       )
{
    #undef  MF__
    #define MF__ MOD__"MakeLine"


    INT nArgLength;
    INT nInIndex;
    INT nOutIndex;


    DBGIN;

    DBG3 (( MF__, "pszArgIn       = \"%s\"",   pszArgIn      ));
    DBG3 (( MF__, "*pnArgInIndex  = %d",  *pnArgInIndex  ));
    DBG3 (( MF__, "nLineLength    = %d",   nLineLength   ));
    DBG3 (( MF__, "nSeparator     = '%c'", nSeparator    ));
    DBG3 (( MF__, "*pnArgOutIndex = %d",  *pnArgOutIndex ));

    // take all the elements you have got and make a line
    // how long is the next element?
    nArgLength = (INT)strlen ( pszArgIn );

    DBG3 (( MF__, "nArgLength = %d", nArgLength ));

    // put one element into the line
    for ( nInIndex = *pnArgInIndex, nOutIndex = *pnArgOutIndex;
          nInIndex < nArgLength && nInIndex < nLineLength &&
          pszArgIn [ nInIndex ] != nSeparator;
          nOutIndex++, nInIndex++ ) {

        pszArgOut [ nOutIndex ] = pszArgIn [ nInIndex ];
    }

    *pnArgOutIndex = nOutIndex;
    *pnArgInIndex  = nInIndex;

    DBG3 (( MF__, "*pnArgInIndex  = %d", *pnArgInIndex ))
    DBG3 (( MF__, "*pnArgOutIndex = %d", *pnArgOutIndex ))
    DBG3 (( MF__, "pszArgOut      = \"%s\"",  pszArgOut ));

    DBGOUT;
}


/*------------------------------*/


static VOID BuildArgLineFromStruct  (   PCHAR           psArgs,
                                        PARG_LINE_REC   prcArgLine,
                                        BOOLEAN         bUserNameFound,
                                        BOOLEAN         bPasswordFound,
                                        BOOLEAN         bDBNameFound,
                                        BOOLEAN         bCmdFound,
                                        BOOLEAN         bFileNameFound,
                                        BOOLEAN         bArgumentsFound     )
{
    #undef  MF__
    #define MF__ MOD__"BuildArgLineFromStruct"


    INT nArgIndex;
    INT nArgCount1;
    INT nArgCount2;
    INT nArgCount3;
    INT nIndex;


    DBGIN;

    DBG3 (( MF__, "prcArgLine -> username  = \"%s\"",
            _PToC ( prcArgLine -> username )));
    DBG3 (( MF__, "prcArgLine -> password  = \"%s\"",
            _PToC ( prcArgLine -> password )));
    DBG3 (( MF__, "prcArgLine -> dbname    = \"%s\"",
            _PToC ( prcArgLine -> dbname )));
    DBG3 (( MF__, "prcArgLine -> cmd       = \"%s\"",
            _PToC ( prcArgLine -> cmd )));
    DBG3 (( MF__, "prcArgLine -> filename  = \"%s\"",
            _PToC ( prcArgLine -> filename )));
    DBG3 (( MF__, "prcArgLine -> arguments = \"%s\"",
            _PToC ( prcArgLine -> arguments )));

    // initialize local vars
    nArgIndex  = 0;
    nArgCount1 = 0;
    nArgCount2 = 0;
    nArgCount3 = 0;

    // build up user-passwd-dbname string
    nArgCount1 = StrLen ( prcArgLine -> username,
                          sizeof ( prcArgLine -> username ));
    nArgCount2 = StrLen ( prcArgLine -> password,
                          sizeof ( prcArgLine -> username ));
    nArgCount3 = StrLen ( prcArgLine -> dbname,
                          sizeof ( prcArgLine -> username ));

    if (( bUserNameFound && nArgCount1 > 0 ) ||
        ( bPasswordFound && nArgCount2 > 0 ) ||
        ( bDBNameFound   && nArgCount3 > 0 )) {

        psArgs [ nArgIndex ] = '(';
        nArgIndex++;

        for ( nIndex = 0;
              nIndex < nArgCount1 && nArgIndex < MAXARGLINELENGTH;
              nIndex++, nArgIndex++ ) {

            psArgs [ nArgIndex ] = prcArgLine -> username [ nIndex ];
        }

        psArgs [ nArgIndex ] = ',';
        nArgIndex++;

        DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));

        for ( nIndex = 0;
              nIndex < nArgCount2 && nArgIndex < MAXARGLINELENGTH;
              nIndex++, nArgIndex++ ) {

            psArgs [ nArgIndex ] = prcArgLine -> password [ nIndex ];
        }

        psArgs [ nArgIndex ] = ',';
        nArgIndex++;

        DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));

        for ( nIndex = 0;
              nIndex < nArgCount3 && nArgIndex < MAXARGLINELENGTH;
              nIndex++, nArgIndex++ ) {

            psArgs [ nArgIndex ] = prcArgLine -> dbname [ nIndex ];
        }

        psArgs [ nArgIndex ] = ')';
        nArgIndex++;

        DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));
    }

    // build up command string
    nArgCount1 = StrLen ( prcArgLine -> cmd,
                          sizeof ( prcArgLine -> cmd ));

    if ( bCmdFound && nArgCount1 > 0 ) {
        if ( nArgIndex > 0 ) {
            DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));

            psArgs [ nArgIndex ] = ' ';
            nArgIndex++;
        }

        for ( nIndex = 0;
              nIndex < nArgCount1 && nArgIndex < MAXARGLINELENGTH;
              nIndex++, nArgIndex++ ) {

            psArgs [ nArgIndex ] = prcArgLine -> cmd [ nIndex ];
        }

        DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));
    }

    // build up filename string
    nArgCount1 = StrLen ( prcArgLine -> filename,
                          sizeof ( prcArgLine -> filename ));

    if ( bFileNameFound && nArgCount1 > 0 ) {
        if ( nArgIndex > 0 ) {
            DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));

            psArgs [ nArgIndex ] = ' ';
            nArgIndex++;
        }

        psArgs [ nArgIndex ] = '\'';
        nArgIndex++;

        for ( nIndex = 0;
              nIndex < nArgCount1 && nArgIndex < MAXARGLINELENGTH;
              nIndex++, nArgIndex++ ) {

            psArgs [ nArgIndex ] = prcArgLine -> filename [ nIndex ];
        }

        psArgs [ nArgIndex ] = '\'';
        nArgIndex++;

        DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));
    }

    // build up argument string
    nArgCount1 = StrLen ( prcArgLine -> arguments,
                          sizeof ( prcArgLine -> arguments ));

    if ( bArgumentsFound && nArgCount1 > 0 ) {
        if ( nArgIndex > 0 ) {
            DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));

            psArgs [ nArgIndex ] = ' ';
            nArgIndex ++;
        }

        for ( nIndex = 0;
              nIndex < nArgCount1 && nArgIndex < MAXARGLINELENGTH;
              nIndex++, nArgIndex ++ ) {

            psArgs [ nArgIndex ] = prcArgLine -> arguments [ nIndex ];
        }

        DBG3 (( MF__, "nArgIndex = %d", nArgIndex ));
    }

    DBGOUT;
}


/*------------------------------*/


static VOID MoveToUpper (   PCHAR   psString,
                            INT     nStringSize     )
{
    #undef  MF__
    #define MF__ MOD__"MoveToUpper"


    INT nIndex;


    DBGIN;

    // convert to upper
    for ( nIndex = 0; nIndex < nStringSize; nIndex++ )
        psString [ nIndex ] = toupper ( psString [ nIndex ] ) ;

    DBGOUT;
}


/*------------------------------*/


static VOID CheckIdents ( PCTU_REC  pCtU,
                          CHAR      cSep,
                          LONG      lMaxIdent,
                          PBOOLEAN  pbOk,
                          ERRORTEXT acErrorText )
{
    #undef  MF__
    #define MF__ MOD__"CheckIdents"
    LONG                    lCnt;
    ULONG                   ulLenArg;
    PSZ                     pszSepAddr;
    PSZ                     pszStart;
    PSZ                     pszEnd;
    BOOL                    bSepFound      = FALSE;
    LONG                    lQuoteCounter  = 0;
    PSZ                     pszTmp         = NULL;
    PSZ                     pszFirstQuote  = NULL;
    PSZ                     pszSecondQuote = NULL;

    DBGIN;

    *pbOk = TRUE;
    memset ( pCtU, 0, sizeof (CTU_REC));

    ulLenArg   = (INT)strlen ( sql80_OptArg );
    pszStart   = sql80_OptArg;
    pszEnd     = pszStart + ulLenArg ;   // --- including "\0"

    DBG3 (( MF__, "pszStart         ( 1 ) = %x", pszStart ));
    DBG3 (( MF__, "pszEnd                 = %x", pszEnd   ));
    DBG3 (( MF__, "ulLenArg               = %d", ulLenArg ));

    pszTmp =  pszStart;

    do {
        pszTmp = strchr ( pszTmp, '\'' );

        if ( pszTmp ) {
            lQuoteCounter ++;
            pszTmp ++;
            }

    } while ( pszTmp );


    if ( lQuoteCounter % 2 ) {
        *pbOk = FALSE;
        sql46c_build_error_string ( acErrorText, ERRMSG_MISSING_QUOTE, 0 );
        DBG3 (( MF__, "lQuoteCounter = %d", lQuoteCounter ));
        DBGOUT;
        return;
    }

    // --- initialize internel parameters again
    pszFirstQuote  = NULL;
    pszSecondQuote = NULL;

    // --- check every part of the argument string
    for ( lCnt = 0;  pszStart != pszEnd ; lCnt ++ ) {

        DBG3 (( MF__, "pszStart ( 2 ) = %x", pszStart ))
        DBG3 (( MF__, "lCnt           = %d", lCnt   ))

        if ( lCnt >= lMaxIdent )  {
            *pbOk = FALSE;
            sql46c_build_error_string ( acErrorText, ERRMSG_ILLEGAL_ARGUMENT,
                                        0 );
            DBGOUT;
            return;
        }

        // --- first sign of the argumnet part == seperator
        //     skip to the next part
        if ( strchr ( pszStart, cSep ) == pszStart ) {
            pszStart ++;
            if ( bSepFound ) {
                *pbOk = FALSE;
                sql46c_build_error_string ( acErrorText,
                                            ERRMSG_DOUBLE_SEPERATOR, 0 );
                DBGOUT;
                return;
            }
            bSepFound = TRUE;
            DBG3 (( MF__, "pszStart ( seperator found ) = %x", pszStart ));
            continue;
        }

        // --- reset bSepFound flag
        bSepFound = FALSE;

        // --- point to '\''
        pszFirstQuote  = strchr ( pszStart, '\'' );

        DBG3 (( MF__, "pszFirstQuote ( 1 ) = %x", pszFirstQuote ));

        // --- be sure you are at the beginning of the part
        if ( pszFirstQuote != pszStart ) pszFirstQuote = NULL;

        if ( pszFirstQuote ) {
            pszSecondQuote = strchr ( pszFirstQuote + 1, '\'');
            DBG3 (( MF__, "pszSecondQuote  = %x", pszSecondQuote  ));
        }


        // --- quote count correct?
        if ( ( pszFirstQuote ) && ( ! pszSecondQuote ) ) {
            *pbOk = FALSE;
            sql46c_build_error_string ( acErrorText,
                                        ERRMSG_MISSING_QUOTE, 0 );
            DBGOUT;
            return;
        }
        //
        // --- get string
        //
        if ( pszFirstQuote ) {
            pCtU->pszIdent      [ lCnt ] = pszFirstQuote + 1;
            pCtU->lIdentLen     [ lCnt ] = (INT)(pszSecondQuote - pszFirstQuote - 1);
            pCtU->bIdentToUpper [ lCnt ] = FALSE;

            DBG3 (( MF__, "pCtU->pszIdent [ lCnt ] ( quoted )      = %x",
                    pCtU->pszIdent [ lCnt ] ))
            DBG3 (( MF__, "pCtU->lIdentLen [ lCnt ] ( quoted )     = %d",
                    pCtU->lIdentLen [ lCnt ] ))
            DBG3 (( MF__, "pCtU->bIdentToUpper [ lCnt ] ( quoted ) = %f",
                    pCtU->bIdentToUpper [ lCnt ] ))


            // ---- find seperator
            if ( ( pszSepAddr = strchr ( pszSecondQuote, cSep ) ) != NULL ) {
                bSepFound = TRUE;
                DBG3 (( MF__, "pszSepAddr ( found, quoted str ) = %x", pszSepAddr ))
            }
            else {
                // --- seperator not found
                bSepFound    = FALSE;
                pszSepAddr   = pszEnd;
                DBG3 (( MF__, "pszSepAddr (not found, quoted str) = %x", pszSepAddr ))

                // --- check error conditions
                if ( pszEnd > ( pszSecondQuote + 1 ) ) {
                    *pbOk = FALSE;
                    sql46c_build_error_string ( acErrorText,
                                                ERRMSG_ILLEGAL_ARGUMENT, 0 );
                    DBGOUT;
                    return;
                }
            }
        }
        else {
            // --- find seperator
            if ( ( pszSepAddr = strchr ( pszStart, cSep ) ) != NULL ) {

                bSepFound = TRUE;
                DBG3 (( MF__, "pszSepAddr ( found, not quoted ) = %x", pszSepAddr ))
            }
            else {
                // --- seperator not found
                bSepFound    = FALSE;
                pszSepAddr   = pszEnd;
                DBG3 (( MF__, "pszSepAddr (not found, not quoted) = %x", pszSepAddr ))
            }


            pCtU->pszIdent      [ lCnt ] = pszStart;
            pCtU->lIdentLen     [ lCnt ] = (INT)(pszSepAddr - pszStart);
            pCtU->bIdentToUpper [ lCnt ] = TRUE;

            DBG3 (( MF__, "pCtU->pszIdent [ lCnt ] ( not quoted )      = %x",
                    pCtU->pszIdent [ lCnt ] ))
            DBG3 (( MF__, "pCtU->lIdentLen [ lCnt ] ( not quoted )     = %d",
                    pCtU->lIdentLen [ lCnt ] ))
            DBG3 (( MF__, "pCtU->bIdentToUpper [ lCnt ] ( not quoted ) = %f",
                    pCtU->bIdentToUpper [ lCnt ] ))
        }

        pszStart = pszSepAddr;
        if ( pszStart != pszEnd ) pszStart ++;
    }

    DBGOUT;
    return;
}

/*------------------------------*/

static int StrLen ( CHAR*   psStr,
                    INT     nLength )
{
    #undef  MF__
    #define MF__ MOD__"StrLen"


    int nIndex;


    DBGIN;

    DBG3 (( MF__, "psStr   = %s", _PToC( psStr) ));
    DBG3 (( MF__, "nLength = %d", nLength ));

    // this function looks for the end of strings which do not ends with \0
    for ( nIndex = nLength - 1; nIndex >= 0; nIndex-- ) {
        if ( psStr [ nIndex ] != ' ' && psStr [ nIndex ] != '\0' )
            break;
    }

    DBG3 (( MF__, "nIndex + 1 = %d", nIndex + 1 ));

    DBGOUT;

    return nIndex + 1;
}


// ----------------------------------------------------------------------------


static INT StrChr   ( CHAR*   pszStr,
                      INT     nCharacter,
                      INT     nBegin,
                      INT     nEnd )
{
    #undef  MF__
    #define MF__ MOD__"StrChr"


    int nIndex;


    DBGIN;

    DBG3 (( MF__, "pszStr     = %s", pszStr     ));
    DBG3 (( MF__, "nCharacter = %c", nCharacter ));
    DBG3 (( MF__, "nBegin     = %d", nBegin     ));
    DBG3 (( MF__, "nEnd       = %d", nEnd       ));

    // StrChr returns the position of the first occurrence in string pszStr
    // of character c
    for ( nIndex = nBegin;
          (nIndex < nEnd) && (pszStr [ nIndex ] != '\0');
          nIndex ++ ) {
        if ( pszStr [ nIndex ] == nCharacter )
            break;
    }

    if ( pszStr [ nIndex ] != nCharacter )
        nIndex = 0;

    DBG3 (( MF__, "nIndex = %d", nIndex ));

    DBGOUT;

    return nIndex;
}

//
// =============================== END ========================================
//
