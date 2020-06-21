/*!
  @file           vos05u.c
  @author         RaymondR
  @brief          Virtual Printer
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

/*
 *  DEFINES
 */
#define MOD__  "VOS05UC : "
#define MF__   MOD__"UNDEFINED"

// --- Virtual printer --------------------------------------------------------

#define VP_NOLF                0
#define VP_ONELF               1
#define VP_TWOLF               2
#define VP_THREELF             3
#define VP_PAGE                4

#define TEMP_NAME   "TMP"       // temp envion. var name
#define SPL_NAME    "spl"       // base name
#define MAX_PID     5           // max digits in PID
#define MAX_DR_ANZ  3           // max digits in job #

#define PR_LF      '\n'         // line feed character
#define PR_FF      '\f'         // form feed character
#define PR_CR      '\r'         // carriage return character


/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 *  EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 *  LOCAL VARIABLES
 */

static CHAR             szPrinterName [ sizeof ( tsp00_PrtName ) + 1 ];
static SHORT            pr_number       = 1;
static HANDLE           hfSpoolFile     = (HANDLE)INVALID_HANDLE_VALUE;
static HANDLE           hfPrtr          = (HANDLE)INVALID_HANDLE_VALUE;
static PATHNAME         szSpoolFile;

/*
 * LOCAL FUNCTION PROTOTYPES
 */

static VOID Spoolname   ( SHORT       cPrint  );


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID    sqlpon  (   tsp00_PrtName printer,
                    BOOLEAN*      vpok        )
{
    #undef  MF__
    #define MF__ MOD__"sqlpon"


    APIRET  rc       = NO_ERROR;
    ULONG   ulAction = 0;


    DBGIN;

    eo46PtoC ( szPrinterName, printer, sizeof ( tsp00_PrtName ));
    DBG3 (( MF__, "Printer name: %s", szPrinterName ));

    if ( hfSpoolFile != ( HANDLE) INVALID_HANDLE_VALUE ) {
        DBG1  (( MF__, "Wrong spool file handle: 0x%x", hfSpoolFile ));
        MSGD  (( ERR_PRINTER_ON ));

        CLOSE_FILE ( hfSpoolFile );
        DELETE_FILE ( szSpoolFile );
    }


    Spoolname ( pr_number );

    #if defined(_WIN32)
     if ((hfSpoolFile = (HANDLE)CreateFile(szSpoolFile,
                                           GENERIC_READ      |
                                           GENERIC_WRITE,
                                           FILE_SHARE_READ,
                                           NULL,
                                           CREATE_ALWAYS,
                                           FILE_ATTRIBUTE_NORMAL,
                                           NULL)) == (HANDLE)INVALID_HANDLE_VALUE)
       {
       rc = GetLastError();
       }
    #else
     rc = DosOpen ( szSpoolFile, &hfSpoolFile, &ulAction, 0,
                    FILE_NORMAL, FILE_TRUNCATE | FILE_CREATE,
                    OPEN_ACCESS_READWRITE |  OPEN_SHARE_DENYWRITE, NULL );
    #endif

    DBG3(( MF__, "Spool file open"));
    if ( rc ) {
        *vpok = FALSE;

        DBG1 (( MF__, "Open spool file" ));
        MSGD (( ERR_OPEN_SPOOLFILE ));
    }
    else {
        #if defined(_WIN32)
         if ((hfPrtr = (HANDLE)CreateFile(szPrinterName,
                                          GENERIC_WRITE,
                                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                                          NULL,
                                          OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL,
                                          NULL)) == (HANDLE)INVALID_HANDLE_VALUE)
           {
           rc = GetLastError();
           }
        #else
         rc = DosOpen ( szPrinterName, &hfPrtr, &ulAction, 0, 0,
                        FILE_CREATE | FILE_OPEN,
                        OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYREADWRITE, NULL);
        #endif

        if ( rc ) {
            *vpok = FALSE;

            CLOSE_FILE ( hfSpoolFile );
            DELETE_FILE ( szSpoolFile );
            hfSpoolFile = ( HANDLE)INVALID_HANDLE_VALUE;

            DBG1 (( MF__, "Open printer" ));
            MSGD (( ERR_OPEN_PRINTER ));
        }
        else {
            ++pr_number;
            *vpok = TRUE;
        }
    }

    DBGOUT;
}


/*------------------------------*/


VOID    sqlprint    (   tsp00_PrtLine     line,
                        INT2              length,
                        tsp00_VpLinefeeds lfeeds,
                        ERRORTEXT         errtext,
                        BOOLEAN*          vpok        )
{
    #undef  MF__
    #define MF__ MOD__"sqlprint"

    CHAR    prstring    [ sizeof (tsp00_PrtLine) + VP_PAGE + 1];
    ULONG   len         = 0;
    APIRET  usError     = 0;
    ULONG   retvalue    = 0;

    DBGIN;

    if ( lfeeds < VP_PAGE ) {
        memset ( prstring, PR_LF, lfeeds );
        len = lfeeds;
    }
    else {
        *prstring = PR_FF;
        len = 1;
    }

    SAPDB_memcpy ( prstring + len, line, length );

    len += length;
    prstring [ len++ ] = PR_CR;

    usError = WRITE_FILE ( hfSpoolFile, prstring, len, &retvalue );

    if (usError || retvalue < len) {
        *vpok = FALSE;

        sql46c_build_error_string ( errtext, ERRMSG_WRITE_SPOOLFILE, usError );

        DBG1 (( MF__, "Write spool file" ));
        MSGD (( ERR_WRITE_SPOOLFILE ));
    }
    else {
        *vpok = TRUE;
    }

    DBGOUT;

}


/*------------------------------*/


VOID    sqlpoff (   BOOLEAN*    print,
                    ERRORTEXT   errtext,
                    BOOLEAN*    vpok    )
{
    #undef  MF__
    #define MF__ MOD__"sqlpoff"

    CHAR    cBuf            [   512 ];
    ULONG   usBytesRead     = 0;
    ULONG   usBytesWritten  = 0;
    APIRET  usError         = 0;
    ULONG   ulPos           = 0;

    DBGIN;

    if ( print ) {
        SET_FILE_PTR ( hfSpoolFile, 0, 0, &ulPos, NULL );

        usError = READ_FILE ( hfSpoolFile, cBuf, ( ULONG ) sizeof ( cBuf ),
                              &usBytesRead );

        if ( usError ) {
            *vpok = FALSE;
            sql46c_build_error_string ( errtext, ERRMSG_READ_SPOOLFILE,
                                        usError );

            CLOSE_FILE ( hfPrtr );
            CLOSE_FILE ( hfSpoolFile );
            DELETE_FILE ( szSpoolFile );
            hfSpoolFile = ( HANDLE)INVALID_HANDLE_VALUE;
            hfPrtr      = ( HANDLE)INVALID_HANDLE_VALUE;

            DBG1 (( MF__, "Read spool file" ));
            MSGD (( ERR_READ_SPOOLFILE ));

            DBGOUT;

            return;
        }

        while ( usError == NO_ERROR && usBytesRead > 0 ) {
            usError = WRITE_FILE ( hfPrtr, cBuf, usBytesRead, &usBytesWritten );

            if ( usError ) {
                *vpok = FALSE;
                sql46c_build_error_string ( errtext, ERRMSG_WRITE_SPOOLFILE,
                                            usError );

                CLOSE_FILE ( hfPrtr );
                CLOSE_FILE ( hfSpoolFile );
                DELETE_FILE ( szSpoolFile );
                hfSpoolFile = ( HANDLE)INVALID_HANDLE_VALUE;
                hfPrtr      = ( HANDLE)INVALID_HANDLE_VALUE;

                DBG1 (( MF__, "Write printer" ));
                MSGD (( ERR_WRITE_PRINTER ));

                DBGOUT;

                return;
            }

            if ( usBytesWritten != usBytesRead ) {
                break;
            }

            usError = READ_FILE ( hfSpoolFile, cBuf,
                                  sizeof ( cBuf ), &usBytesRead );

            if ( usError ) {
                *vpok = FALSE;
                sql46c_build_error_string ( errtext, ERRMSG_READ_SPOOLFILE,
                                            usError );

                CLOSE_FILE ( hfPrtr );
                CLOSE_FILE ( hfSpoolFile );
                DELETE_FILE ( szSpoolFile );
                hfSpoolFile = ( HANDLE)INVALID_HANDLE_VALUE;
                hfPrtr      = ( HANDLE)INVALID_HANDLE_VALUE;

                DBG1 (( MF__, "Write printer" ));
                MSGD (( ERR_READ_SPOOLFILE ));

                DBGOUT;

                return;
            }
        }
    }

    CLOSE_FILE ( hfPrtr );
    CLOSE_FILE ( hfSpoolFile );
    DELETE_FILE ( szSpoolFile );

    hfSpoolFile = ( HANDLE)INVALID_HANDLE_VALUE;
    hfPrtr      = ( HANDLE)INVALID_HANDLE_VALUE;

    *vpok = TRUE;

    DBGOUT;
}


/*
 * ========================== LOCAL FUNCTIONS =================================
 */


//  Diese Funktion ermittelt aus Prozess_id und Anzahl der Druckjobs den Namen
//  fuer die Druckausgabedatei.
//
//  format:  "%TMP%\spl<ppppp>.<ddd>"
//
//  ppppp = process ID
//  ddd   = internal print job #
//  %TMP% = OS/2 TMP environment variable value

static VOID Spoolname   ( SHORT     cPrint )
{
    #undef  MF__
    #define MF__ MOD__"spoolname"

    CHAR    szPid   [ MAX_PID       + 1 ];
    CHAR    szPrint [ MAX_DR_ANZ    + 1 ];
    PCHAR   pch     = NULL;
    PCHAR   pTemp   = NULL;     // ptr to temp variable
    PID     pid;

    DBGIN;

    GETPROCESSID (&pid);

    for ( *(( pch = szPid + MAX_PID )) = '\0', pch--; pch >= szPid; pch-- ) {
        *pch = ( CHAR )( pid % 10 + '0' );
        pid /= 10;
    }

    for ( *(( pch = szPrint + MAX_DR_ANZ )) = '\0', pch--;
                                                    pch >= szPrint; pch-- ) {
        *pch = ( CHAR )( cPrint % 10 + '0' );
        cPrint /= 10;
    }

    pTemp = getenv ( TEMP_NAME );
    if ( pTemp ) {
        strcpy ( szSpoolFile, pTemp );

        if ( *szSpoolFile ) {
            for ( pch = szSpoolFile; *pch; pch++ ) {
                ;
            }

            if ( *( pch - 1 ) != '\\' ) {
                *pch++ = '\\';
                *pch++ = '\0';
            }
        }
    }
    else
        *szSpoolFile = '\0';

    strcat ( szSpoolFile, SPL_NAME );
    strcat ( szSpoolFile, szPid );
    strcat ( szSpoolFile, "." );
    strcat ( szSpoolFile, szPrint );

    DBGOUT;
}


/*
 * =============================== END ========================================
 */
