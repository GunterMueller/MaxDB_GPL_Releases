/*! 
  -----------------------------------------------------------------------------
  module: hls40cli.h
  -----------------------------------------------------------------------------
  responsible:  SteffenS
 
  special area: SAP DB Loader
 
  description:  SAP DB Loader CommandLineInterface (LoaderCLI)

  version:      7.6
  -----------------------------------------------------------------------------
    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

#ifndef HLS40CLI
#define HLS40CLI

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#if defined (_WIN32)
  #include <windows.h>
#else
  #define WINAPI
#endif

#ifndef MAX_CRYPT
  #define MAX_CRYPT csp_cryptname
#endif

static const char   *C_COMPONENT  = "loadercli                                                      ";

const char* const  CMD_SQLDEFAULT = "SQLMODE INTERNAL;";
const char* const  REPMAN_ERR_STR = "ERR";
const char* const  REPMAN_CON_STR = "CON";
const char         NULLCHAR       = '\0';

#define PRG_OK               0
#define PRG_FATAL           -1
#define PRG_VERYFATAL       -2
#define PRG_SHOW            -3      // used to release option parser (only help or version requested)
#define PRG_SERVER_ERROR    -4
#define PRG_CMD_ERROR       -5


// Answer header size from server containing either 'ERR\n' or 'OK \n'
#define REPLY_HEADER_SIZE_LS40   4

// Defines read buffer size when reading from batch file
const unsigned long MAXBUFFERSIZE_LS40 = LINELENGTH_MXSP00;


#define MAX_PARAM_LS40  9

// definition corresponds to the biggest possible message buffer
// that can be exchanged between server and db server
#define MAX_COMMAND_LENGTH_LS40 128*1024

#define MAX_CMD_SEPARATOR_LENGTH_LS40     2
#define DEFAULT_CMD_SEPARATOR_LENGTH_LS40 2

typedef enum
{
    rncLocal_els40,
    rncDBNode_els40,
    rncDifferent_els40
} tls40_RepmNodeLoc;

typedef struct tls40_CmdSeparator
{
    bool  csDefaultSeparator;
    char  csCmdSeparator_ls40[3];

} tls40_CmdSeparator;

typedef struct tls40_Options
{
    char              *application;
    tsp00_Pathc        szDBRoot_ls40;
    tls40_RepmNodeLoc  NodeLoc;
    const char        *repm_node;
    tsp00_Int4         lIsoLevel_ls40;

    tls00_VFile        VirtualFile_ls40;

    char              *pszOutfileName;
    FILE              *outfile;
    void              *session;

    _TCHAR            *pszCommBuffer_ls40;
    _TCHAR            *pszCmdBuffer;
    tsp00_Longint      CmdBufferLen;
    tls40_CmdSeparator CmdSeparator_ls40;
    _TCHAR            *pszReplyBuffer;
    tsp00_Int4         ReplyBufferLen;

    tsp00_Int4         CmdExecError;
    tsp00_Int4         BatchErrorCnt_ls40;
    _TCHAR            *pszParamList_ls40[MAX_PARAM_LS40];
    bool               bNoParam_ls40;
    bool               showServerVersion;
    bool               bSuppressOutput_ls40;
    bool               bStaticServer_ls40;
    bool               bLoaderExtension_ls40;
    // source
    const char*                 dbnode;
    char                        dbname[128];
    char                        dbuser[128];
    char                        dbuserkey[128];
    char                        dbschema[128];
    tls00_EncryptionProtocol    EncryptionProtocol;
    // target
    const char*                 targetdbnode;
    char                        targetdbname[128];
    char                        targetdbuser[128];
    char                        targetdbuserkey[128];
    char                        targetdbschema[128];
    tls00_EncryptionProtocol    targetEncryptionProtocol;

    // transport parameter
    char               executionkind[128];
    char               transformationtype[24];
    char               transformationpackage[256];
    char               transformationpart[128];
    char               transformationstreams[48];
    char               streammedium[24];
    char               streamformat[24];
    char               streamformat_target[24];
    char               streamcharencoding[24];
    char               streamlob[24];
    char               startposition[24];

    bool               bInteractive_ls40;
    bool               bRelease_ls40;

    BOOLEAN            bCancelByte_ls40;
} tls40_Options;


#endif /* HLS40CLI */
