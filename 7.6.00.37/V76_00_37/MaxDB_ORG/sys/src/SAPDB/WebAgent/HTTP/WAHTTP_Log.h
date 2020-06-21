/*



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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





*/
#ifndef WAHTTP_LOG_INC
#define WAHTTP_LOG_INC

#ifdef WIN32
#include <WINDOWS.H>
#endif

/*
#include "gsp00.h"
*/



/* Extended error description */
#ifdef DEBUG
    #define WAHTTP_COMPREHENSIVE_ERROR_DESCRIPTION     1
#else
    #define WAHTTP_COMPREHENSIVE_ERROR_DESCRIPTION     0
#endif

#ifdef WAHTTP_COMPREHENSIVE_ERROR_DESCRIPTION
    #define Log_Error(a,b)          Log_WriteError(a,b,__FILE__,__LINE__)
    #define Log_SyscallError(a,b)   Log_WriteSyscallError(a,b,__FILE__,__LINE__)
#else
    #define Log_Error(a,b)          Log_WriteError(a,b,NULL,0)
    #define Log_SyscallError(a,b)   Log_WriteSyscallError(a,b,NULL,0)
#endif

#define Log_Journal(a,b,c)  /*          Log_WriteJournal(a,b,c)*/



typedef struct st_wahttp_log_desc   *WAHTTP_LogDesc;

struct st_wahttp_log_desc {
    int              logFile;
    struct st_excl  *logExcl;
    int              errFile;
    struct st_excl  *errExcl;
};



WAHTTP_LogDesc  Log_GetDesc();



void SetDefLogFilePath
(
 char	*path
);

int InitLog
(
);

void Log
(
 char	text[],
 int	lg
);

void Log_WriteJournal( int   socket,
                       char *text,
                       int   length );

void Log_WriteError( char	    *text,
                     int	     length,
                     char       *file,
                     const int   line );

void Log_WriteSyscallError( char	   *txt,
                            int         err,
                            char       *file,
                            const int   line );

void ExitLog
(
 void
);

typedef struct st_rlog *LogReqHandle;

#ifdef kannweg

enum en_logwhat
{
 lw_request,
 lw_reply
};

enum en_logformat
{
 lf_all,
 lf_raw,
 lf_webber
};

enum en_logoptions
{
 lo_nothing,
 lo_macrosub
};

#endif

#ifdef kannweg
typedef struct st_http		*dummy;
LogReqHandle LogReqInit
(
 struct st_http		*P
);
#endif

#ifdef kannweg

void LogReqLog
(
 enum en_logformat	lf,
 enum en_logwhat	lw,
 LogReqHandle		H,
 char			Line[],
 size_t			Lg
);

void LogReqLogMacro
(
 enum en_logformat	lf,
 enum en_logwhat	lw,
 LogReqHandle		H,
 char			Line[],
 size_t			Lg
);

void LogReqLogNewLine
(
 enum en_logformat	lf,
 enum en_logwhat	lw,
 LogReqHandle		H
);

void LogReqExit
(
 LogReqHandle	H
);

#endif

void SetTraceLevel
(
 int	Level
);

void Trace
(
 int	Level,
 char	*Txt,
 size_t	lg
);

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef WIN32
    DWORD Log_WriteException( LPEXCEPTION_POINTERS ec );

    /*
    #define WA_TRY
    #define WA_CATCH
    */
    #define WA_TRY      \
       __try
    #define WA_CATCH    \
       __except (Log_WriteException(GetExceptionInformation())) {;}
#else
    #define WA_TRY
    #define WA_CATCH

    void Log_SetSignalHandler();
#endif

#if defined(__cplusplus)
}
#endif

#endif