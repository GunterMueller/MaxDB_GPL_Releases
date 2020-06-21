/*!**********************************************************************

  module: vwd25log.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Logging routines

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <stdio.h>
#include <time.h>

#include "hwd25log.h"
#include "hwd27excl.h"

/*!**********************************************************************

  Chapter: twd25LogP

************************************************************************/

struct st_wa_log {
    char            filename[MAX_INI_PAR_LEN_WD00+1];
    FILE           *logFd;
    twd27ExclP      logExcl;
    sapdbwa_Bool    withInfo;
}; /* struct st_wa_log */

const char* wd25GetLogFilename( twd25LogP log )
{

	if ( !log || !log->filename )
		return "";

	return log->filename;

}

void wd25SetLogInfoFlag( twd25LogP log, sapdbwa_Bool withInfo )
{

	if (!log)
		return;

    if (withInfo == sapdbwa_True) {
	    log->withInfo = sapdbwa_True;
    } else {
	    log->withInfo = sapdbwa_False;
    }

}

twd25LogP wd25CreateLog( char *filename )
{
    twd25LogP  newLog;
    sapdbwa_Bool allocatOK;

    if (strlen(filename)>=MAX_INI_PAR_LEN_WD00) {
        /* log-filename too long */
        return NULL;
    }; /* if */
    sapdbwa_SQLALLOCAT( sizeof(struct st_wa_log),
                   (sapdbwa_UInt1**) &newLog,
                   &allocatOK );
    if (!allocatOK) {
        /* memory allocation error */
        return NULL;
    }; /* if */
    if (!wd27InitExcl( &newLog->logExcl )) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) newLog );
        return NULL;
    }; /* if */
    strcpy( newLog->filename, filename );

    newLog->withInfo = sapdbwa_False;
    
    return newLog;
} /* wd25CreateLog */


void wd25DestroyLog( twd25LogP log )
{
    wd27ExitExcl( log->logExcl );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) log );
} /* wd25DestroyLog */


void wd25EmptyLog( twd25LogP log, sapdbwa_Bool makeCopy )
{

    FILE    *logFile;
    FILE    *backupFile;
    char     backupFilename[1024] = "";
    size_t   readLen = 0;
    char     buffer[1024] = "";

    if (makeCopy == sapdbwa_True) {
        sp77sprintf(backupFilename, 1024, "%s.bak", log->filename);

        /* Copy log file */
        logFile = fopen( log->filename, "r" );
        if (logFile) {
            backupFile = fopen(backupFilename, "w" );
            if (backupFile) {
                while(!feof(logFile)) {
                    readLen = fread(buffer, 1, 1024, logFile);
                    if (readLen > 0) {
                        fwrite(buffer, 1, readLen, backupFile);
                    }
                }

                fclose(backupFile);
            }

            fclose(logFile);
        };
    }

    /* Empty log file */
    logFile = fopen( log->filename, "w" );
    if (logFile) {
        fclose(logFile);
    };

}


const char *wd25GetFileName( twd25LogP log )
{
    return log->filename;
} /* wd25GetLogFileName */


void wd25_MakeTimeString( tsp00_Date date,  tsp00_Time time, char *timeStr );

void wd25WriteLogInfoMsg( twd25LogP log, char *msg, sapdbwa_Bool withHeader )
{
    char	     tdbuffer[100];
    tsp00_Date   date;
    tsp00_Time   time;

	/* Checks */
	if ( !log || !msg || !log->withInfo)
		return;
    
    if (!wd27BegExcl( log->logExcl )) {
        return;
    }; /* if */

    if (withHeader) {
        sqldattime( date, time );
        wd25_MakeTimeString( date, time, tdbuffer );
    }

    log->logFd = fopen( log->filename, "a" );
    if (log->logFd) {
        if (withHeader) {
            fprintf( log->logFd, tdbuffer );
            fprintf( log->logFd, "[Info]: " );
        } else {
            fprintf( log->logFd, "                             " );
        }

        fprintf( log->logFd, msg );
        fprintf( log->logFd, "\n" );

        fclose( log->logFd );
    };
    
    wd27EndExcl( log->logExcl );
}

void wd25WriteLogMsg( twd25LogP log, char *msg )
{
    char	     tdbuffer[100];
    tsp00_Date   date;
    tsp00_Time   time;

	/* Checks */
	if ( !log || !msg )
		return;
    
    if (!wd27BegExcl( log->logExcl )) {
        return;
    }; /* if */

    sqldattime( date, time );
    wd25_MakeTimeString( date, time, tdbuffer );
    log->logFd = fopen( log->filename, "a" );
    if (log->logFd) {
        fprintf( log->logFd, tdbuffer );
        fprintf( log->logFd, " " );
        fprintf( log->logFd, msg );
        fclose( log->logFd );
    }; /* if */
    
    wd27EndExcl( log->logExcl );
} /* wd25WriteLogMsg */


void wd25WriteLnLogMsg( twd25LogP log, char *msg )
{
    char	     tdbuffer[100];
    tsp00_Date   date;
    tsp00_Time   time;
    
    if (!wd27BegExcl( log->logExcl )) {
        return;
    }; /* if */

    sqldattime( date, time );
    wd25_MakeTimeString( date, time, tdbuffer );
    log->logFd = fopen( log->filename, "a" );
    if (log->logFd) {
        fprintf( log->logFd, tdbuffer );
        fprintf( log->logFd, " " );
        fprintf( log->logFd, msg );
        fprintf( log->logFd, "\n" );        
        fclose( log->logFd );
    }; /* if */
    
    wd27EndExcl( log->logExcl );
} /* wd25WriteLnLogMsg */

void sapdbwa_WriteTraceLine( const char *module,
							 const char *function,
							 const char *trace	)
{

#ifdef WIN32
	const char	FN[] = "C:\\WebAgent.dbg";
#else
	const char	FN[] = "/tmp/WebAgent.dbg";
#endif

    char	     tdbuffer[100];
    tsp00_Date   date;
    tsp00_Time   time;
	FILE		*file;
/*    
    if (!wd27BegExcl( log->logExcl )) {
        return;
    };
*/

    sqldattime( date, time );
    wd25_MakeTimeString( date, time, tdbuffer );
    file = fopen( FN, "a" );
    if ( file ) {
        fprintf( file, tdbuffer );
        fprintf( file, "\n" );
        fprintf( file, "Module: " );
        fprintf( file, module );
        fprintf( file, "\n" );        
        fprintf( file, "Function: " );
        fprintf( file, function );
        fprintf( file, "\n" );        
        fprintf( file, trace );
        fprintf( file, "\n" );        
        fclose( file );
    };
    
/*
    wd27EndExcl( log->logExcl );
	*/
}


void wd25_MakeTimeString( tsp00_Date date,  tsp00_Time time, char *timeStr )
{
    sapdbwa_UInt2 pos = 0;
    
    timeStr[pos++] = '[';
    timeStr[pos++] = date[0];
    timeStr[pos++] = date[1];
    timeStr[pos++] = date[2];
    timeStr[pos++] = date[3];
    timeStr[pos++] = '/';
    timeStr[pos++] = date[4];
    timeStr[pos++] = date[5];
    timeStr[pos++] = '/';
    timeStr[pos++] = date[6];
    timeStr[pos++] = date[7];
    timeStr[pos++] = ' ';

    timeStr[pos++] = time[2];
    timeStr[pos++] = time[3];
    timeStr[pos++] = ':';
    timeStr[pos++] = time[4];
    timeStr[pos++] = time[5];
    timeStr[pos++] = ':';
    timeStr[pos++] = time[6];
    timeStr[pos++] = time[7];
    timeStr[pos++] = ']';
    timeStr[pos++] = '\0';
} /* wd25_MakeTimeString */

sapdbwa_Bool wd25IsLogFileEmpty( twd25LogP log )
{
    char	     tdbuffer[100];
    sapdbwa_Bool   rc = sapdbwa_True;
    
    if (!wd27BegExcl( log->logExcl )) {
        return rc;
    }; /* if */

    log->logFd = fopen( log->filename, "r" );
    if (log->logFd) {
		if ( fread( tdbuffer, 1, sizeof(tdbuffer), log->logFd ) > 0 ) rc = sapdbwa_False;
        fclose( log->logFd );
    }; /* if */
    
    wd27EndExcl( log->logExcl );

	return rc;
} /* wd25IsLogFileEmpty */

/*!**********************************************************************

  EndChapter: twd25LogP
  
************************************************************************/
