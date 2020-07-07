/*!
  @file           veo131.c
  @author         J. Mensing (jrg)
  @special area   XUSER File
  @brief          Program to set/clear/list XUSER file content
PTS 1109583
  @first created  000-10-18  9:06

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include "gsp00.h"
#include "heo12.h" /* sqlgetopt() */
#if defined(WIN32)
extern int   sql80_OptInd;
#define optind sql80_OptInd
extern char* sql80_OptArg;
#define optarg sql80_OptArg
#else
extern char *optarg;
extern int   optind;
#endif

#include "geo13.h"
#include "heo13.h"
#include "hsp02.h"
#include "hsp77.h"
#include "hsp78_0.h"
#include "hsp83.h"
#include "hsp100.h"
#if defined(AIX) || defined(SUN) || defined(SOLARIS) || defined(OSF1) || defined(HPUX) || defined(NMP) || defined(LINUX) || defined(FREEBSD)
#include <wchar.h>
#endif /* AIX || SUN || SOLARIS || OSF1 || HPUX || NMP || LINUX || FREEBSD*/
#if !defined(_WIN32)
static wchar_t *localFgetws(wchar_t *WString, int Number, FILE *Stream);
#define fgetws localFgetws
#endif
/* PTS 1109894 */
#if defined(LINUX) || defined(HPUX)
static int      wprintf(const wchar_t *format, ...);
#endif
#include "SAPDBCommon/SAPDB_string.h"
#include "heo131x.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define STRING_BUFFER_SIZE 1024 /* Maximum size of an input line when using batchfiles */
#define TEXT_FILE_UCS2_INTRO ((const unsigned short)0xFEFFU) /* Introduction added by notepad in text mode */
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

#if defined(WIN32)
typedef wchar_t UCS2CHAR;
#else
typedef unsigned short UCS2CHAR;
#endif

enum xuser_action { action_set = 0, action_list = 1, action_clear = 2 };

enum batch_file_format {oldVersion=0, Version76=1};

static int               beVerbose = 0;
static int               usePureAscii8 = 0;
static int               entriesFound = 0;
static int               recursiveCall = 0;
static tsp4_xuser_record allEntries[REL611_MX_XUSER_ENTRIES+1];

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*---------------------------------------
  Function: c_main
  Description: Main program
  Arguments: argc [in] Number of arguments
             argv [in] Array of UTF8 or ascii arguments
 */
static int c_main(int argc, char *argv[]);
/*---------------------------------------
  Function: Usage
  Description: Show usage information
  Arguments: longHelp [in] flag to indicate long text wanted
 */
static void Usage(int longHelp);
/*---------------------------------------
  Function: showVersion
  Description: Show XUSER record version
 */
static void showVersion();
/*---------------------------------------
  Function: parseString
  Description: Parse a 7bit ASCII string and copy it padded with blanks
  Arguments: explain [in] Name of parameter for error output
             dest [out] destination (fill with blanks)
             maxChar [in] size of destination
 */
static void parseString( const char *explain,
                         unsigned char *dest,
                         int maxChars);
/*---------------------------------------
  Function: parseNumber
  Description: Parse a decimal number string
  All negativ numbers are returned as '-1'.

  Arguments: explain [in] Name of parameter for error output
             maxNumber [in] maximum number value allowed
 */
static int parseNumber( const char *explain,
                        unsigned int maxNumber );

/*---------------------------------------
  Function: parseStringC
  Description: Parse a 7bit ASCII string and copy it to dest. If NULL is given for dest the
               length of the parameter to be parsed is determined.
               Usually this function is called twice:
               First call: Determine length
               Alloc memory
               Second Call: Copy paramter to memory location 
  Arguments: explain [in] Name of parameter for error output
             dest [out] destination or NULL, if the length of the pararmter should be determined

  Return: Length of the parsed String
 */

static SAPDB_UInt4 parseStringC(SAPDB_Char const * explain,
                                SAPDB_Char *       dest);

/*---------------------------------------
  Function: parseUsernamePassword
  Description: Parse a username/password combination

  Return username and password arguments.

  Arguments: username [out] username found (UCS2 encoded)
             password [out] password found (UCS2 encoded)
 */
static void parseUsernamePassword(tsp00_KnlIdentifier username,
                                  tsp00_KnlIdentifier password);

/*---------------------------------------
  Function: updatePasswordRecord
  Description: Fill Ascii Username and crypt passwords

  Arguments: passwordUCS2 [in] UCS2 encoded password
             paramUserRecord [in] The XUSER record to use
 */
static void updatePasswordRecord( tsp00_KnlIdentifier passwordUCS2,
                                 struct tsp4_xuser_record *paramUserRecord );
/*---------------------------------------
  Function: listSingleEntry
  Description: List only one entry identified by key

  All entries are read in first. Then the key is search and if found entry is shown.

  Arguments: key [in] Key of entry to be cleared
 */
static void listSingleEntry (tsp4_xuserkey   key,
                             SAPDB_Char*     accountName);
/*---------------------------------------
  Function: listAllEntries
  Description: List all entries

  All entries are read in first. Then each entry is shown.
 */
static void listAllEntries  (SAPDB_Char*     accountName);
/*---------------------------------------
  Function: clearSingleEntry
  Description: Clear only one entry, leaving all other entries

  Since clearing entries can be done only for all entries, the entries are
  read in first. Then the XUSER file is cleared and a new XUSER file is
  filled using all but the entry to be cleared.

  Arguments: key [in] Key of entry to be cleared
 */
static void clearSingleEntry(tsp4_xuserkey  key,
                             SAPDB_Char*    accountName );
/*---------------------------------------
  Function: readAllEntries
  Description: Read all XUSER entries into own memory buffer

  The global variables entriesFound and allEntries are filled.
 */
static void readAllEntries  (SAPDB_Char*    accountName);
/*---------------------------------------
  Function: printEntry
  Description: Print a XUSER entry in readable form (except password...)
  Arguments: which [in] index of entry
             pEntry [in] The entry to print
 */
static void printEntry( int which, tsp4_xuser_record *pEntry );
/*---------------------------------------
  Function: parseBatchFile
  Description: Read entries in from given file

  All entries are read in first and verified. Only valid entries are put
  into XUSER information. If an invalid entry was found a warning is given.

  Arguments: filename [in] The file with the entry information
             accountName [in]       Account of the user to be updated
 */
static void parseBatchFile(char const *    filename,
                           SAPDB_Char *    accountName);
/*---------------------------------------
  Function: checkBatchFormat
  Description: Checks if the given file contains xuser command lines command or the old fixed entries

  Arguments: filename [in] The file with the entry or command line information
  Return: The type of the batch file format.
 */

static enum batch_file_format checkBatchFormat(char const *     filename);

/*---------------------------------------
  Function: executeBatchCommands
  Description: Read the argument lines from given file and execute it by calling it selfs

  All entries are in the form o a normal xuser command line.

  Arguments: filename [in] The file with the entry information
 */

static void executeBatchCommands(char const *    filename);

/*---------------------------------------
  Function: fgetUCS2s
  Description: Read a string of UCS2 character using fgetws

    On systems where wchar_t is larger than UCS2CHAR , first wchar_t array is used and the result
    is copied.

  Arguments: buffer [in] UCS2 character buffer
             maxChar [in] size of buffer
             fp [in] file to read from
  Return value: NULL if fgetUCS2s failed
 */
static wchar_t *fgetUCS2s( bool isUCS2, UCS2CHAR *buffer, FILE *fp );

/*---------------------------------------
  Function: readString
  Description: Read a 7bit ASCII string and copy it padded with blanks
  Arguments: isUCS2 [in] Flag needed to distiguish between UCS2 and Ascii Batchfiles
             fp [in] file to read from
             pReadResult [inout] former / returned read result
             pInsertEntry [out] flag for invalid entry (updated only if needed)
             explain [in] Name of parameter for error output
             dest [out] destination (fill with blanks)
             maxChar [in] size of destination
 */
static void readString( bool isUCS2,
                       FILE *fp,
                  wchar_t **pReadResult,
                  int *pInsertEntry,
                  const char *explain,
                  unsigned char *dest,
                  int maxChar );
/*---------------------------------------
  Function: readNumber
  Description: Read a decimal number string
  All negativ numbers are returned as '-1'.

  Arguments: isUCS2 [in] Flag needed to distiguish between UCS2 and Ascii Batchfiles
             fp [in] file to read from
             pReadResult [inout] former / returned read result
             pInsertEntry [out] flag for invalid entry (updated only if needed)
             explain [in] Name of parameter for error output
             maxNumber [in] maximum number value allowed
 */
static int readNumber( bool isUCS2,
                       FILE *fp,
                       wchar_t **pReadResult,
                       int *pInsertEntry,
                       const char *explain,
                       unsigned int maxNumber );
/*---------------------------------------
  Function: readUsernamePassword
  Description: Read a username/password combination

  Return username and password arguments.

  Arguments: isUCS2 [in] Flag needed to distiguish between UCS2 and Ascii Batchfiles
             fp [in] file to read from
             pReadResult [inout] former / returned read result
             pInsertEntry [out] flag for invalid entry (updated only if needed)
             username [out] username found (terminated by ',')
             password [out] password found (terminated by '\0')
 */
static void readUsernamePassword( bool isUCS2,
                                  FILE *fp,
                                  wchar_t **pReadResult,
                                  int *pInsertEntry,
                                  tsp00_KnlIdentifier username,
                                  tsp00_KnlIdentifier password );
/*---------------------------------------
  Function: updateUserRecord
  Description: Verify the given entry and update XUSER information

  The entry is verified (isolation level and sqlmodname) are checked.
  If everything want ok so far, the update of XUSER information is done.

  Arguments: paramUserRecord [in]   The XUSER record to use
             accountName [in]       Account of the user to be updated
 */
static void updateUserRecord (struct tsp4_xuser_record *    paramUserRecord,
                              SAPDB_Char *                  accountName);
/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

/*
  Function: [w]main
  Description: Main entry routine

  The read parameter are converted into UTF8 character strings and parsed
  to c_main. This function does the work.

  Arguments: argc [in] Argument count
             wargv [in] UCS2 encoded argument array (WindowsNT)
             argv[in] UTF8 or ascii encoded argument array (UNIX)
 */
#if defined(WIN32)
int wmain(int argc, UCS2CHAR *wargv[])
{
  char **returnedUTF8argv;
  /* Convert into UTF8 parameter array... */
  if ( !sp83UTF8ArrayFromUCS2Array ( argc,
                                     (tsp81_UCS2Char **)wargv,
                                     &returnedUTF8argv ) )
  {
    printf("Failed to convert to UTF8 argument array\n");
    Usage(0);
  }

  return c_main(argc, returnedUTF8argv);
}
#else
int main(int argc, char *argv[])
{
  return c_main(argc, argv);
}
#endif

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/
/*---------------------------------------
  Function: c_main
  Description: Main program
  Arguments: argc [in] Number of arguments
             argv [in] Array of UTF8 or ascii arguments
 */
static int c_main(int argc, char *argv[])
{
  int                      nextOption;
  enum xuser_action        paramAction = action_set;
  char const *             paramBatchFile = 0;
  SAPDB_Char *             paramEncryption = 0;
  tsp00_KnlIdentifier      paramPassword;
  int                      gotVersion = 0;
  int                      gotUserKey = 0;
  int                      gotAnythingElse = 0;
  struct tsp4_xuser_record paramUserRecord;
  SAPDB_Char *             pAccountName = NULL;
  SAPDB_UInt4              lenAccountName = 0;

  /* Initialize paramUserRecord */
  SAPDB_memset(paramPassword, ' ', sizeof(tsp00_KnlIdentifier));
  SAPDB_memset(&paramUserRecord, 0, sizeof(tsp4_xuser_record));
  sql13u_init_user_params( &paramUserRecord );

  if ( argc == 1 )
  {
    Usage(0);
  }

  do
  {
    nextOption = sqlgetopt(argc, argv, "zVAab:c:e:U:u:d:n:S:y:t:I:x:?h");
    switch( nextOption )
    {
    case '?':
	  Usage(0);
	  break;
    case 'h':
	  Usage(1);
	  break;
    case 'A':
    case 'a':
	  usePureAscii8 = 1;
	  break;
    case 'V':
	  gotVersion = 1;
	  break;
    case 'z':
	  beVerbose = 1;
	  break;
    case 'b':
          gotAnythingElse = 1;
	  paramBatchFile = optarg;
          if (recursiveCall) {
            printf("FATAL: Could not use option -b within batchfiles\n");
            return -1;
          }
	  break;
    case 'e':
          gotAnythingElse = 1;
          paramEncryption = optarg;
          break;
    case 'c':
        gotAnythingElse = 1;
        lenAccountName = parseStringC("Account Name",
                                      pAccountName);
        if (0 != lenAccountName)
        {
            pAccountName = (SAPDB_Char *)alloca (lenAccountName + 1);
            if (NULL == pAccountName)
            {
                printf("FATAL: Could not allocate memory to hold account name\n");
                return -1;
            }

            parseStringC ("Account Name", pAccountName);
        }

        break;
    case 'U':
          gotAnythingElse = 1;
	  parseString( "UserKey",
		       paramUserRecord.xu_key,
		       sizeof(tsp4_xuserkey) );
          gotUserKey = 1;
	  break;
    case 'u':
          gotAnythingElse = 1;
	  parseUsernamePassword( paramUserRecord.xu_userUCS2, paramPassword );
      updatePasswordRecord( paramPassword, &paramUserRecord );
	  break;
    case 'd':
          gotAnythingElse = 1;
	  parseString( "Dbname",
		       paramUserRecord.xu_serverdb,
		       sizeof(tsp00_DbName) );
	  break;
    case 'n':
          gotAnythingElse = 1;
	  parseString( "Nodename",
		       paramUserRecord.xu_servernode,
		       sizeof(tsp00_NodeId) );
	  break;
    case 'S':
          gotAnythingElse = 1;
	  parseString( "Sqlmodename",
		       paramUserRecord.xu_sqlmode,
		       sizeof(tsp4_sqlmode_name) );
	  break;
    case 'y':
          gotAnythingElse = 1;
	  paramUserRecord.xu_cachelimit =
		  parseNumber( "Cachelimit", 2147483647U );
	  break;
    case 't':
          gotAnythingElse = 1;
	  paramUserRecord.xu_timeout =
		  parseNumber( "Timeout", 32767U );
	  break;
    case 'I':
          gotAnythingElse = 1;
	  paramUserRecord.xu_isolation =
		  parseNumber( "Isolation", 30U );
	  break;
    case 'x':
          gotAnythingElse = 1;
	  parseString( "Charsetname",
		       paramUserRecord.xu_dblang,
		       sizeof(tsp00_KnlIdentifier) );
	  break;
    case -1:
	  break; /* no more options */
    default:
	  printf( "Illegal option\n" );
	  Usage(0);
	  break;
    }
  } while ( nextOption != -1 );

  /* first non option argument is action argument */
  if ( optind < argc )
  {
    if ( strcmp("list", argv[optind] ) == 0 )
    {
  	paramAction = action_list;
    } else if ( strcmp("clear", argv[optind] ) == 0 ) {
  	paramAction = action_clear;
    } else if ( strcmp("set", argv[optind] ) == 0 ) {
  	paramAction = action_set;
    } else {
        printf("Unknown action: %s\n", argv[optind] );
        Usage(1);
    }
  }

  if ( beVerbose )
  {
    printf("Options:\n");
    if ( gotVersion ) printf("Got Version option\n");
    if ( usePureAscii8 ) printf("Using pure ascii8\n");
    if ( gotUserKey ) printf("Got Userkey '%8.8s'\n", paramUserRecord.xu_key );
    if ( paramBatchFile ) printf("Got Batchfile '%s'\n", paramBatchFile );
    if ( paramEncryption ) printf("Got Encryption '%s'\n", paramEncryption );
    if ( gotAnythingElse ) {
        tsp00_KnlIdentifier userUCS2buffer;
        int  UCS2index;
        printf("Username    '%64.64s'\n", paramUserRecord.xu_user );
        for ( UCS2index = 0; UCS2index < sizeof(tsp00_KnlIdentifier); UCS2index++ )
        {
          userUCS2buffer[UCS2index] = paramUserRecord.xu_userUCS2[UCS2index];
          if ( userUCS2buffer[UCS2index] == 0 )
          {
              userUCS2buffer[UCS2index] = '.';
          }
        }
        printf("UsernameUCS2'%64.64s'\n", userUCS2buffer );
        printf("Password     %s\n", paramUserRecord.xu_password[0] != 0 ? "set" : "not set" );
        printf("PasswordUCS2 %s\n", paramUserRecord.xu_passwordUCS2[0] != 0 ? "set" : "not set" );
        printf("Dbname      '%18.18s'\n", paramUserRecord.xu_serverdb );
        printf("Nodename    '%64.64s'\n", paramUserRecord.xu_servernode );
        printf("Sqlmodename '%8.8s'\n", paramUserRecord.xu_sqlmode );
        printf("Cachelimit   %d\n", paramUserRecord.xu_cachelimit );
        printf("Timeout      %d\n", paramUserRecord.xu_timeout );
        printf("Isolation    %d\n", paramUserRecord.xu_isolation );
        printf("Charsetname '%18.18s'\n", paramUserRecord.xu_dblang );
    }
  }

  switch( paramAction )
  {
  case action_list:
    if ( gotUserKey )
    {
      if ( beVerbose ) printf("Action ListSingleEntry (%8.8s)\n", paramUserRecord.xu_key );
      listSingleEntry (paramUserRecord.xu_key, pAccountName);
    }
    else
    {
      if ( beVerbose ) printf("Action ListAllEntries\n" );
      listAllEntries (pAccountName);
    }
    break;

  case action_clear:
    if ( gotUserKey )
    {
      if ( beVerbose ) printf("Action ClearSingleEntry (%8.8s)\n", paramUserRecord.xu_key );
      clearSingleEntry (paramUserRecord.xu_key, pAccountName);
    }
    else
    {
      if ( beVerbose ) printf("Action ClearAllEntries\n" );
      sqlclearuser (pAccountName);
    }
    break;

  default:
    if ( gotAnythingElse )
    {
      if ( paramBatchFile )
      {
        if ( beVerbose ) printf("Parsing Batchfile %s\n", paramBatchFile );
        if (checkBatchFormat(paramBatchFile) == Version76) {
          if ( beVerbose ) printf("Executing Batchfile %s\n", paramBatchFile );
          executeBatchCommands(paramBatchFile);
        }
        else {
          parseBatchFile( paramBatchFile, pAccountName);
        }
      }
      else
      {
        if ( beVerbose ) printf("Update user record\n");
        if ( paramEncryption ) {
          buildServernodeURI(&paramUserRecord, paramEncryption );
        }
        /* Fill password and check also sqlmode,isolation */
        updateUserRecord( &paramUserRecord, pAccountName);
      }
    }
    break;
  }

  if ( gotVersion )
  {
     showVersion();
  }

  return 0;
}

/*---------------------------------------
  Function: Usage
  Description: Show usage information
  Arguments: longHelp [in] flag to indicate long text wanted
 */



static void Usage(int longHelp)
{
  printf("Syntax: xuser [{options}] [action]\n");
  showVersion();
  printf("Usage: List, clear or setup user specific connection information\n");
  printf("Description: This tool is used to predefine and manage default values\n");
  printf("             for user data and connection options. Every data set\n");
  printf("             is stored with an XUSER key. When connecting to the\n");
  printf("             database without specifying an XUSER key, the key DEFAULT\n");
  printf("             is used. Additionally specified command line options\n");
  printf("             override the XUSER data.\n");
  printf("Options:\n");
  printf("    -h show long helptext\n");
  if ( !longHelp )
  {
    printf("    -V -A -b file -c account -U key -u name,password -d dbname -n nodename\n");
    printf("    -e encryption -S sqlmode -t timeout -I isolation\n");
  }
  else
  {
    printf("    -V           show version of XUSER program\n");
    printf("    -A ASCII8    interpret environment and batchfiles as pure ASCII8\n");
    printf("    -b file      set user information via batch file\n");
    printf("                 The batch file format is analogous to the way you enter\n");
    printf("                 options and commands on the command line. Each line in\n");
    printf("                 the batch file must contain an option list followed by\n");
    printf("                 a command. The lines are processed in the order they\n");
    printf("                 occur in the batch file. You can mix 'set' and 'clear'\n");
    printf("                 commands in one batch file. You can't use the 'list'\n");
    printf("                 command or the option -b. If an error occurs, the batch\n");
    printf("                 execution stops at the respective line of the batch file.\n");
    printf("    -c account_name       user account <domain>\\<user> for which the xuser\n");
    printf("                          command should be executed (only available for\n");
    printf("                          administrators)\n");
    printf("       string (no restriction to the length), default '' = current user\n");
    printf("    -U user_key           user key\n");
    printf("       string (max. 18 characters), default 'DEFAULT'\n");
    printf("    -u user_name,password user name/password combination\n");
    printf("       2 strings (max. 18 characters), default '',''\n");
    printf("    -d database_name      database name to connect to\n");
    printf("       string (max. 18 character), default ''\n");
    printf("    -n database_server    database server name\n");
    printf("       string (max. 64 characters), default '' = local server\n");
    printf("    -e encryption         Encryption type valid encryption types\n");
    printf("                          are 'SSL'\n");
    printf("    -S SQL_mode           SQL mode\n");
    printf("       string (max. 8 characters), default ''\n");
    printf("       valid modes are 'SAPDB', 'INTERNAL', 'ANSI',\n");
    printf("                       'DB2', 'ORACLE' and 'SAPR3'\n");
    printf("    -t timeout            connection timeout in seconds\n");
    printf("       number (default -1 = default timeout value set in the database\n");
    printf("                        0 = no timeout)\n");
    printf("    -I isolation_level    isolation level\n");
    printf("       number (default -1, valid values are -1,0,1,2,3,9,10,15,20,30)\n");
  }
  printf("  The following actions are defined (must always be the LAST argument):\n");
  printf("   list    list user connection information (default all user keys)\n");
  printf("           ignores all options, except -U\n");
  printf("   clear   clear user connection information (default all user keys)\n");
  printf("           ignores all options, except -U\n");
  printf("   set     (default) set user information\n");
  exit(1);
}

/*---------------------------------------
  Function: showVersion
  Description: Show XUSER record version
 */
static void showVersion()
{
  tsp00_Versionc VersionString;
  sp100_GetVersionString ( "xuser    ", s100buildnumber, VersionString);
  printf("%s (record format version: %d)\n", VersionString, XUSER_RECORD_VERSION);
}

/*---------------------------------------
  Function: parseString
  Description: Parse a 7bit ASCII string and copy it padded with blanks
  Arguments: explain [in] Name of parameter for error output
             dest [out] destination (fill with blanks)
             maxChar [in] size of destination
 */
static void parseString( const char *explain,
                  unsigned char *dest,
                  int maxChar )
{
  int i;
  SAPDB_memset(dest, ' ', maxChar);

  for ( i = 0; (i < maxChar) && (optarg[i] != 0); i++ )
  {
     if ( (optarg[i] & 0x7f) != optarg[i] )
     {
	printf("Warning: Non Ascii7 parameter '0x%x' for '%s' (parameter ignored)\n",
          optarg[i], explain);
        SAPDB_memset(dest, ' ', maxChar);
        break;
     }
     dest[i] = optarg[i];
  }
}

/*---------------------------------------
  Function: parseNumber
  Description: Parse a decimal number string
  All negativ numbers are returned as '-1'.

  Arguments: explain [in] Name of parameter for error output
             maxNumber [in] maximum number value allowed
 */
static int parseNumber( const char *explain,
                        unsigned int maxNumber )
{
  unsigned int i;
  int negativ = 0;
  unsigned long number;

  for ( i = 0; i < strlen(optarg); i++ )
  {
    if ( optarg[i] != '*' )
	break;
  }

  /* empty or '*****...' argument found */
  if ( optarg[i] == 0 )
  {
    return -1;
  }

  for ( i = 0; optarg[i] == ' ' || optarg[i] == '\t'; i++ )
	;

  if ( optarg[i] == '+' )
  {
     for ( ++i; optarg[i] == ' ' || optarg[i] == '\t'; i++ )
	;
  } else if ( optarg[i] == '-' )
  {
    negativ = 1;
  }

  for ( number = 0UL; number < maxNumber && optarg[i] != 0; i++ )
  {
     switch(optarg[i])
     {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		number = number * 10 + optarg[i] - '0';
		break;
	default:
		printf("Warning: Nondigit parameter %s for '%s' (ignored)\n",
			optarg, explain);
                negativ = 1;
		break;
     }
  }

  if ( number > maxNumber )
  {
    printf("Warning: Parameter %s out of range (max %d) for '%s' (ignored)\n",
			optarg, maxNumber, explain);
    negativ = 1;
  }

  return ( negativ ? -1 : (int)number );
}

/*---------------------------------------
  Function: parseStringC
  Description: Parse a 7bit ASCII string and copy it to dest. If NULL is given for dest the
               length of the parameter to be parsed is determined.
               Usually this function is called twice:
               First call: Determine length
               Alloc memory
               Second Call: Copy paramter to memory location 

               !!! C a u t i o n !!!
               If dest is unequal to NULL it is assumed that sufficient memory
               is allocated to hold the whole string including the terminating null byte.

  Arguments: explain [in] Name of parameter for error output
             dest [out] destination or NULL, if the length of the pararmter should be determined

  Return: Length of the parsed String
 */

static SAPDB_UInt4 parseStringC(SAPDB_Char const * explain,
                                SAPDB_Char *       dest)
{
  SAPDB_UInt4   idx;

  for (idx = 0; (optarg[idx] != 0); idx++ )
  {
     if ( (optarg[idx] & 0x7f) != optarg[idx] )
     {
	    printf("Warning: Non Ascii7 parameter '0x%x' for '%s' (parameter ignored)\n",
                optarg[idx], explain);
        idx= 0;
        break;
     }

     if (NULL != dest)
     {
        dest[idx] = optarg[idx];
     }
  }

  if (NULL != dest)
  {
     dest[idx] = '\0';
  }

  return idx;
}

/* PTS 1111170 */
/*---------------------------------------
  Function: GetQuotedArgumentUppercase
  Description: fill given buffer with uppercase converted input argument

  Return pointer to buffer

  Arguments: argumentBuffer [out] argument buffer
 */
static char *GetQuotedArgumentUppercase(char *argumentBuffer)
{
    int isQuoted = 0;

    if ( usePureAscii8 )
    {
        char *nextChar = optarg;
        char *nextArg = argumentBuffer;
        while ( ( 0 != *nextChar )
             && ( nextArg < (argumentBuffer + 2 * sizeof(tsp00_KnlIdentifier) + 2) ) )
        {
            if ( 0 != isQuoted )
            {
                if ( *nextChar == isQuoted )
                {
                    isQuoted = 0;
                }
                else
                {
                    *nextArg++ = *nextChar;
                }
            }
            else
            {
                if ( *nextChar == '"' )
                {
                    isQuoted = '"';
                }
                else if ( *nextChar == '\'' )
                {
                    isQuoted = '\'';
                }
                else
                {
                    tsp00_Uint2 keyToMap = *nextChar;
                    *nextArg++ = (char)sp81UCS2ToUpper(keyToMap);
                }
            }
            ++nextChar;
        }
        *nextArg = 0;
    }
    else
    {
        tsp00_Uint4 bytesWritten;
        tsp00_Uint4 bytesParsed;
        tsp78ConversionResult convResult;
        tsp00_Uint2 ucs2argumentBuffer[ 2 * sizeof(tsp00_KnlIdentifier) + 2 ];
        tsp00_Uint2 *nextUCS2Char;
        tsp00_Uint2 *nextUCS2Arg;

        convResult = sp78convertBuffer( sp77encodingUCS2Native,
                           ucs2argumentBuffer,
                           2 * sizeof(tsp00_KnlIdentifier) + 2,
                           &bytesWritten,
                           sp77encodingUTF8,
                           optarg,
                           (SAPDB_UInt4)strlen(optarg)+1,
                           &bytesParsed );
        if ( sp78_Ok != convResult )
        {
            printf("Username,Password conversion UTF8->UCS2 failed:%d\n", (int)convResult);
            exit(1);
        }

        nextUCS2Char = nextUCS2Arg = &ucs2argumentBuffer[0];

        while ( ( 0 != *nextUCS2Char )
             && ( nextUCS2Arg < (ucs2argumentBuffer + 2 * sizeof(tsp00_KnlIdentifier) + 2) ) )
        {
            if ( 0 != isQuoted )
            {
                if ( *nextUCS2Char == isQuoted )
                {
                    isQuoted = 0;
                }
                else
                {
                    *nextUCS2Arg++ = *nextUCS2Char;
                }
            }
            else
            {
                if ( *nextUCS2Char == '"' )
                {
                    isQuoted = '"';
                }
                else if ( *nextUCS2Char == '\'' )
                {
                    isQuoted = '\'';
                }
                else
                {
                    tsp00_Uint2 keyToMap = *nextUCS2Char;
                    *nextUCS2Arg++ = sp81UCS2ToUpper(keyToMap);
                }
            }
            ++nextUCS2Char;
        }
        *nextUCS2Arg = 0;

        convResult = sp78convertBuffer( sp77encodingUTF8,
                           argumentBuffer,
                           2 * sizeof(tsp00_KnlIdentifier) + 2,
                           &bytesWritten,
                           sp77encodingUCS2Native,
                           ucs2argumentBuffer,
                           (SAPDB_UInt4)((nextUCS2Arg - ucs2argumentBuffer) * sizeof(tsp00_Uint2) + 2),
                           &bytesParsed );
        if ( sp78_Ok != convResult )
        {
            printf("Username,Password conversion UCS2->UTF8 failed:%d\n", (int)convResult);
            exit(1);
        }
    }

    if ( 0 != isQuoted )
    {
        printf("Username,Password missing quote '%c'\n", (char)isQuoted);
        exit(1);
    }
    return argumentBuffer;
}
/*---------------------------------------
  Function: parseUsernamePassword
  Description: Parse a username/password combination

  Return username and password arguments.

  Arguments: username [out] username found (terminated by ',')
             password [out] password found (terminated by '\0')
 */
static void parseUsernamePassword( tsp00_KnlIdentifier username,
                                   tsp00_KnlIdentifier password )
{
  char const * pSearch;
  tsp00_Uint4 bytesWritten;
  tsp00_Uint4 bytesParsed;
  tsp78ConversionResult convResult;
  void *fillBuffer;
  tsp00_Uint4 fillLength;
  char const * pSeparator = (char const *)0;
  char argumentBuffer[2*sizeof(tsp00_KnlIdentifier)+2];
  char *pArg;

  pArg = GetQuotedArgumentUppercase(&argumentBuffer[0]);

  pSearch = pArg;

  while ( *pSearch )
  {
    if ( *pSearch == ',' )
    {
        pSeparator = pSearch;
        break;
    }
    ++pSearch;
  }

  if ( pSeparator != 0 )
  {
      if ( !usePureAscii8 )
      {
          unsigned int byteLength;
          unsigned int charLength;
          if ( sp83UTF8Convert_Success !=
              sp83UTF8_Bytelen((const tsp00_Byte *)pArg,
                               (SAPDB_UInt4)(pSeparator - pArg),
                               &byteLength,
                               &charLength) )
          {
              printf("Username contains non UTF8 character\n");
              exit(1);
          }
      }
      convResult = sp78convertBuffer( sp77encodingUCS2Native,
                         username,
                         sizeof(tsp00_KnlIdentifier),
                         &bytesWritten,
                         usePureAscii8 ? sp77encodingAscii : sp77encodingUTF8,
                         pArg,
                         (SAPDB_UInt4)(pSeparator - pArg),
                         &bytesParsed );
      if ( sp78_Ok != convResult )
      {
          printf("Username conversion UTF8->UCS2 failed:%d\n", (int)convResult);
          exit(1);
      }

      if ( bytesWritten > sizeof(tsp00_KnlIdentifier) )
      {
          printf("Username too long: Maximum %d characters allowed\n",
              sizeof(tsp00_KnlIdentifier)/2 );
          exit(1);
      }
      fillBuffer = &username[bytesWritten];
      fillLength = sizeof(tsp00_KnlIdentifier) - bytesWritten;
      sp77encodingUCS2Native->fillString(&fillBuffer, &fillLength, fillLength/2, ' ');

      /* Skip ',' */
      ++pSeparator;

      if ( !usePureAscii8 )
      {
          unsigned int byteLength;
          unsigned int charLength;
          if ( sp83UTF8Convert_Success !=
              sp83UTF8_Bytelen((const tsp00_Byte *)pArg, (SAPDB_UInt4)strlen(pSeparator), &byteLength, &charLength) )
          {
              printf("Password contains non UTF8 character\n");
              exit(1);
          }
      }
      convResult = sp78convertBuffer( sp77encodingUCS2Native,
                         password,
                         sizeof(tsp00_KnlIdentifier),
                         &bytesWritten,
                         usePureAscii8 ? sp77encodingAscii : sp77encodingUTF8,
                         pSeparator,
                         (SAPDB_UInt)strlen(pSeparator),
                         &bytesParsed );
      if ( sp78_Ok != convResult )
      {
          printf("Password conversion UTF8->UCS2 failed\n", (int)convResult);
          exit(1);
      }
      if ( bytesWritten > sizeof(tsp00_KnlIdentifier) )
      {
          printf("Password too long: Maximum %d characters allowed\n",
              sizeof(tsp00_KnlIdentifier)/2 );
          exit(1);
      }
      fillBuffer = &password[bytesWritten];
      fillLength = sizeof(tsp00_KnlIdentifier) - bytesWritten;
      sp77encodingUCS2Native->fillString(&fillBuffer, &fillLength, fillLength/2, ' ');
  }
  else
  {
      /* only username found... */
      if ( !usePureAscii8 )
      {
          unsigned int byteLength;
          unsigned int charLength;
          if ( sp83UTF8Convert_Success !=
              sp83UTF8_Bytelen((const tsp00_Byte *)pArg, (SAPDB_UInt4)(pSearch - pArg), &byteLength, &charLength) )
          {
              printf("Username contains non UTF8 character\n");
              exit(1);
          }
      }
      convResult = sp78convertBuffer( sp77encodingUCS2Native,
                         username,
                         sizeof(tsp00_KnlIdentifier),
                         &bytesWritten,
                         usePureAscii8 ? sp77encodingAscii : sp77encodingUTF8,
                         pArg,
                         (SAPDB_UInt4)(pSearch - pArg),
                         &bytesParsed );
      if ( sp78_Ok != convResult )
      {
          printf("Username conversion UTF8->UCS2 failed:%d\n", (int)convResult);
          exit(1);
      }
      if ( bytesWritten > sizeof(tsp00_KnlIdentifier) )
      {
          printf("Username too long: Maximum %d characters allowed\n",
              sizeof(tsp00_KnlIdentifier)/2 );
          exit(1);
      }
      fillBuffer = &username[bytesWritten];
      fillLength = sizeof(tsp00_KnlIdentifier) - bytesWritten;
      sp77encodingUCS2Native->fillString(&fillBuffer, &fillLength, fillLength/2, ' ');

      /* fill in empty password */
      fillBuffer = &password[0];
      fillLength = sizeof(tsp00_KnlIdentifier);
      sp77encodingUCS2Native->fillString(&fillBuffer, &fillLength, fillLength/2, ' ');
  }
  return;
}

/*---------------------------------------
  Function: clearSingleEntry
  Description: Clear only one entry, leaving all other entries

  Since clearing entries can be done only for all entries, the entries are
  read in first. Then the XUSER file is cleared and a new XUSER file is
  filled using all but the entry to be cleared.

  Arguments: key [in] Key of entry to be cleared
 */
static void clearSingleEntry(tsp4_xuserkey  key,
                             SAPDB_Char*    accountName )
{
  int errorCount = 0;
  int i;
  int entryToDelete = -1;
  tsp00_Bool ok = 1;
  tsp00_ErrText errText;

  SAPDB_memset( errText, 0, sizeof(tsp00_ErrText));

  readAllEntries(accountName);

  for ( i = 0; i < entriesFound; i++ )
  {
    if ( memcmp(allEntries[i].xu_key, key, sizeof(tsp4_xuserkey)) == 0 )
    {
      entryToDelete = i;
      break;
    }
  }

  if ( entryToDelete == -1 )
  {
    printf("Entry for userKey %18.18s not found\n", key);
  }
  else
  {
      sqlclearuser(accountName);/*Löscht den Key mit*/

      sqlxuopenuser( accountName, errText, &ok );
      if ( !ok )
      {
          printf("FATAL: Reopen xuser entry file failed: %40.40s\n", errText);
          printf("xuser entry file lost\n");
          exit(2);
      }

      for ( i = 0; i < entriesFound; i++ )
      {
          if ( i != entryToDelete )
          {
              sqlputuser( &allEntries[i], accountName, errText, &ok );
              if ( !ok )
              {
                  printf("FATAL: Put old xuser entry failed: %40.40s\n", errText);
                  printf("FATAL: xuser entry for key %18.18s may be lost\n",
                      allEntries[i].xu_key);
                  ok = 1;
                  errorCount++;
              }
          }
      }

      sqlxucloseuser(accountName, errText, &ok);
      if ( !ok )
      {
          printf("FATAL: Closing xuser entry file failed: %40.40s\n", errText);
          printf("xuser entry file may be lost\n");
          exit(2);
      }

      if ( errorCount > 0 )
      {
          printf("FATAL: %d errors while putting old entries\n", errorCount);
          exit(2);
      }
  }
}

/*---------------------------------------
  Function: listSingleEntry
  Description: List only one entry identified by key

  All entries are read in first. Then the key is search and if found entry is shown.

  Arguments: key [in] Key of entry to be cleared
 */
static void listSingleEntry(tsp4_xuserkey   key,
                             SAPDB_Char*     accountName)
{
  int entryFound = -1;
  int i;

  readAllEntries(accountName);

  for ( i = 0; i < entriesFound; i++ )
  {
    if ( memcmp( allEntries[i].xu_key, key, sizeof(tsp4_xuserkey) ) == 0 )
    {
      entryFound = i;
      break;
    }
  }

  if ( entryFound == -1 )
  {
    printf("No xuser entry for key %18.18s\n", key );
  }
  else
  {
    printEntry( entryFound, &allEntries[entryFound] );
  }
}

/*---------------------------------------
  Function: listAllEntries
  Description: List all entries

  All entries are read in first. Then each entry is shown.
 */
static void listAllEntries(SAPDB_Char*     accountName)
{
  int i;

  readAllEntries(accountName);

  for ( i = 0; i < entriesFound; i++ )
  {
    printEntry( i, &allEntries[i] );
  }
}

/*---------------------------------------
  Function: printEntry
  Description: Print a XUSER entry in readable form (except password...)
  Arguments: pEntry [in] The entry to print
 */
static void printEntry(int i, tsp4_xuser_record *pEntry)
{
  printf("-----------------------------------------------------------------\n");
  printf("XUSER Entry %2d\n", i+1);
  printf("--------------\n");

  printf("Key         :%18.18s\n", pEntry->xu_key);

  printf("Username    :%-64.64s\n",
      pEntry->xu_user[0] == ' ' ? "<unspecified>" : (const char *)&pEntry->xu_user[0] );

  if ( ((const unsigned short)' ') == *(const unsigned short *)&pEntry->xu_userUCS2[0] )
  {
    printf("UsernameUCS2:<unspecified>\n");
  }
  else
  {
    tsp00_KnlIdentifier userUCS2buffer;
    int  UCS2index;
    for ( UCS2index = 0; UCS2index < sizeof(tsp00_KnlIdentifier); UCS2index++ )
    {
      userUCS2buffer[UCS2index] = pEntry->xu_userUCS2[UCS2index];
      if ( userUCS2buffer[UCS2index] == 0 )
      {
          userUCS2buffer[UCS2index] = '.';
      }
    }
    printf("UsernameUCS2:%-64.64s\n", userUCS2buffer );
  }

  printf("Password    :%s\n",
      pEntry->xu_password[0] ==  0  ? "<unspecified>" : "?????????" );

  printf("PasswordUCS2:%s\n",
      (*(const unsigned short *)&pEntry->xu_passwordUCS2[0]) == 0 ? "<unspecified>" : "?????????" );

  printf("Dbname      :%-18.18s\n",
      pEntry->xu_serverdb[0]   == ' ' ? "<unspecified>" : (char *)&pEntry->xu_serverdb[0] );

  printf("Nodename    :%-64.64s\n",
      pEntry->xu_servernode[0] == ' ' ? "<unspecified>" : (char *)&pEntry->xu_servernode[0] );

  if ( pEntry->xu_sqlmode[0] == ' ' ) {
    printf("Sqlmode     :<unspecified>\n");
  } else {
    printf("Sqlmode     :%-8.8s\n",
      pEntry->xu_sqlmode );
  }

  printf("Cachelimit  :%d\n",
      pEntry->xu_cachelimit );

  printf("Timeout     :%d\n",
      pEntry->xu_timeout );

  printf("Isolation   :%d\n",
      pEntry->xu_isolation );

  printf("Charset     :%-18.18s\n",
      pEntry->xu_dblang[0] == ' ' ? "<unspecified>" : (char *)&pEntry->xu_dblang[0] );
}

/*---------------------------------------
  Function: readAllEntries
  Description: Read all XUSER entries into own memory buffer

  The global variables entriesFound and allEntries are filled.
 */
static void readAllEntries(SAPDB_Char *    accountName)
{
  int i;
  tsp00_Bool ok;
  tsp00_ErrText errText;

  SAPDB_memset( errText, 0, sizeof(tsp00_ErrText));

  sqlxuopenuser(accountName, errText, &ok );
  if ( !ok )
  {
    if ( errText[0] != ' ' )
    {
      printf("Warning: Open xuser entry file failed: %40.40s\n", errText);
    }
    entriesFound = 0;
    return;
  }

  /* Create a backup copy of all entries */
  entriesFound = REL611_MX_XUSER_ENTRIES;
  for ( i = 0; i < REL611_MX_XUSER_ENTRIES; i++ )
  {
    sqlindexuser((tsp00_Int2)(i+1), &allEntries[i], accountName, errText, &ok);
    if ( !ok )
    {
      if ( errText[0] != ' ' )
      {
        printf("Warning: Error reading xuser entry %d: %40.40s\n", i, errText);
      }
      else
      {
        entriesFound = i;
        break;
      }
    }
  }

  sqlxucloseuser(accountName, errText, &ok);
  if ( !ok )
  {
    printf("Warning: Error closing xuser file: %40.40s\n", errText);
  }
}

/*---------------------------------------
  Function: skipUCS2intro
  Description: Since some text editor put an introduction sequence 'FFFE' as unicode file header, it
  is necessary to skip these first two bytes if they exist.

  Arguments: fp [in] already opened batch file
  Return value: false if no valid file found, true everything is fine
 */
static bool skipUCS2intro(FILE *fp, bool *isUCS2)
{
  int c1;
  int c2;
  union swapTester {
      struct {
          unsigned char c1;
          unsigned char c2;
      } s;
      unsigned short ucs2code;
  } swapTest;

  c1 = fgetc(fp);
  if ( EOF == c1 )
  {
      if ( beVerbose ) printf("file is empty\n");
      clearerr(fp);
      return true;
  }
  swapTest.s.c1 = (unsigned char)(c1&0x0FF);

  c2 = fgetc(fp);
  if ( EOF == c2 )
  {
      if ( beVerbose ) printf("file contains only a singled character\n");
      return false;
  }
  swapTest.s.c2 = (unsigned char)(c2&0x0FF);

  *isUCS2 = ( swapTest.ucs2code == TEXT_FILE_UCS2_INTRO );

  if ( !*isUCS2 )
  {
      if ( EOF == ungetc(c2,fp) )
      {
          if ( beVerbose ) printf("not able to push back intro character 2\n");
          return false;
      }

      if ( EOF == ungetc(c1,fp) )
      {
          if ( beVerbose ) printf("not able to push back intro character 1\n");
          return false;
      }
      *isUCS2 = ( c1 == 0 || c2 == 0 );
  }

  return true;
}

/*---------------------------------------
  Function: parseBatchFile
  Description:  Read entries in from given file. Does not support the '-c' option for giving an
                account name.

  All entries are read in first and verified. Only valid entries are put
  into XUSER information. If an invalid entry was found a warning is given.

  Arguments: filename [in] The file with the entry information
 */
static void parseBatchFile(char const *     filename,
                           SAPDB_Char *     accountName)
{
  FILE *                    fp;
  tsp4_xuser_record         paramUserRecord;
  tsp00_KnlIdentifier       paramPassword;


  /* readResult (reset on first read failure. '8' forces non error on first read check... */
  wchar_t *readResult = (wchar_t *)8;
  /* Each entry found will increase the entryIndex by 1 */
  int     entryIndex = 0;
  /* Flag to indicate entry is insertable, updated on each content check! */
  int     insertEntry;
  /* Flag to indicate UCS2 batch file was found */
  bool    isUCS2;
  /* Flag to indicate newEntry, set to true after key was succefully read */
  bool    newEntry;

  SAPDB_memset(paramPassword, ' ', sizeof(tsp00_KnlIdentifier));

#if defined(_WIN32)
  fp = fopen(filename, "rb");
#else
  fp = fopen(filename, "r");
#endif
  if ( !fp )
  {
    printf("Cannot open file '%s'\n", filename);
    Usage(0);
  }

  if ( !skipUCS2intro(fp, &isUCS2) )
  {
    printf("Cannot skip intro of file '%s'\n", filename);
    Usage(0);
  }

  /* mercy check on Ascii file .... */

  do {
    entryIndex++;
    insertEntry = true;

    SAPDB_memset(&paramUserRecord, 0, sizeof(tsp4_xuser_record));
    sql13u_init_user_params( &paramUserRecord );


    /* 1-UserKey */
    readString( isUCS2, fp, &readResult, &insertEntry, "UserKey",
                 paramUserRecord.xu_key, sizeof(tsp4_xuserkey) );
    newEntry = ( readResult != NULL );
    /* 2-UserName 3-Password */
    readUsernamePassword( isUCS2, fp, &readResult, &insertEntry, paramUserRecord.xu_userUCS2, paramPassword );
    updatePasswordRecord( paramPassword, &paramUserRecord );
    /* 4-Dbname */
    readString( isUCS2, fp, &readResult, &insertEntry, "Dbname",
                 paramUserRecord.xu_serverdb, sizeof(tsp00_DbName) );
    /* 5-NodeName */
    readString( isUCS2, fp, &readResult, &insertEntry, "Nodename",
                 paramUserRecord.xu_servernode, sizeof(tsp00_NodeId) );
    /* 6-SqlModeName */
    readString( isUCS2, fp, &readResult, &insertEntry, "Sqlmodename",
                 paramUserRecord.xu_sqlmode, sizeof(tsp4_sqlmode_name) );
    /* 7-CacheLimit */
    paramUserRecord.xu_cachelimit = readNumber( isUCS2, fp, &readResult, &insertEntry,
                  "Cachelimit", 2147483647U );
    /* 8-TimeOut */
    paramUserRecord.xu_timeout = readNumber( isUCS2, fp, &readResult, &insertEntry,
                  "Timeout", 32767U );
    /* 9-Isolation */
    paramUserRecord.xu_isolation = readNumber( isUCS2, fp, &readResult, &insertEntry,
		  "Isolation", 30U );
    /* 10-Charsetname */
    readString( isUCS2, fp, &readResult, &insertEntry, "Charsetname",
                 paramUserRecord.xu_dblang, sizeof(tsp00_KnlIdentifier) );

    if ( insertEntry )
    {
        printf("Updating entry %d for Userkey '%18.18s'\n",
            entryIndex, &paramUserRecord.xu_key[0] );
        updateUserRecord(&paramUserRecord, accountName);
    }
    else
    {
        if ( readResult != NULL )
        {
            printf("Entry %d discarded. Given Userkey was '%18.18s'\n",
                entryIndex, &paramUserRecord.xu_key[0] );
        }
        else
        {
            if ( newEntry )
            {
                printf("Entry %d discarded (incomplete). Given Userkey was '%18.18s'\n",
                    entryIndex, &paramUserRecord.xu_key[0] );
            }
        }
    }
  } while ( readResult != NULL );

  fclose(fp);
}

static enum batch_file_format checkBatchFormat(char const *     filename)
{
  enum batch_file_format batchFormat = oldVersion;
  FILE *                    fp;
  /* readResult (reset on first read failure. '8' forces non error on first read check... */
  wchar_t *readResult = (wchar_t *)8;
  /* Flag to indicate UCS2 batch file was found */
  bool    isUCS2;
  unsigned int charLength;
  unsigned int byteLength;
  int  isTerminated;
  int isCorrupted;
  int isExhausted;

  UCS2CHAR buffer[STRING_BUFFER_SIZE+1];
  UCS2CHAR *c;

#if defined(_WIN32)
  fp = fopen(filename, "rb");
#else
  fp = fopen(filename, "r");
#endif
  if ( !fp )
  {
    printf("Cannot open file '%s'\n", filename);
    Usage(0);
  }

  if ( !skipUCS2intro(fp, &isUCS2) )
  {
    printf("Cannot skip intro of file '%s'\n", filename);
    Usage(0);
  }
  readResult = fgetUCS2s( isUCS2, buffer, fp);
  if (readResult) {
    sp77encodingUCS2Native->stringInfo(buffer, sizeof(buffer), true,
                                       &charLength, &byteLength, &isTerminated, &isCorrupted, &isExhausted);
    c = buffer;
    while (sp77encodingUCS2Native->isSpace(c)) {
      c++;
    }
    if (*c=='-') {
      batchFormat=Version76;
    }
  }
  fclose(fp);
  return batchFormat;
}

/*---------------------------------------
  Function: executeBatchCommands
  Description: Read the argument lines from given file and execute it by calling it selfs

  All entries are in the form o a normal xuser command line.

  Arguments: filename [in] The file with the entry information
 */

static void executeBatchCommands(char const *     filename)
{
  FILE *                    fp;
  /* readResult (reset on first read failure. '8' forces non error on first read check... */
  wchar_t *readResult = (wchar_t *)8;
  /* Flag to indicate UCS2 batch file was found */
  bool    isUCS2;
  unsigned int charLength;
  unsigned int byteLength;
  int  isTerminated;
  int isCorrupted;
  int isExhausted;
  const tsp77encoding *encoding;
  UCS2CHAR buffer[STRING_BUFFER_SIZE+1];
  UCS2CHAR *c;
  UCS2CHAR *wargv[20];
  int argc=0;
#if defined(_WIN32)
  fp = fopen(filename, "rb");
#else
  fp = fopen(filename, "r");
#endif
  if ( !fp )
  {
    printf("Cannot open file '%s'\n", filename);
    Usage(0);
  }

  if ( !skipUCS2intro(fp, &isUCS2) )
  {
    printf("Cannot skip intro of file '%s'\n", filename);
    Usage(0);
  }
  encoding = sp77encodingUCS2Native;
  
  readResult = fgetUCS2s( isUCS2, buffer, fp);
  while (readResult) {
    argc=0;
    wargv[argc]=calloc(1, sizeof(UCS2CHAR));
    argc++;

    if (readResult) {
      encoding->stringInfo(buffer, sizeof(buffer), true,
                           &charLength, &byteLength, &isTerminated, &isCorrupted, &isExhausted);
      c = buffer;
      while (encoding->isSpace(c) && *c != 0) {
        c++;
      }
      while(*c != 0) {
        UCS2CHAR *optbeg = NULL;
        if (*c == '-') {
          optbeg = c;
          while (!encoding->isSpace(c) && *c != 0) {
            c++;
          }
          wargv[argc]=calloc(c-optbeg+1, sizeof(UCS2CHAR));
          memcpy(wargv[argc],optbeg,(c-optbeg)*sizeof(UCS2CHAR));
          *(wargv[argc]+(c-optbeg)) = 0;
          argc++;
        }
        if (encoding->isSpace(c)) {
          c++;
          continue;
        }
        if (*c == '"' || *c == '\'') {
          UCS2CHAR qoutec = *c;
          optbeg = c;
          do {
            c++;
            if (encoding->isSpace(c))
              if (*c-1 == qoutec && buffer <= c-2 && *c-2 == qoutec )
                continue;
              else
                break;
          } while(*c != 0 );
          wargv[argc]=calloc(c-optbeg+1, sizeof(UCS2CHAR));
          memcpy(wargv[argc],optbeg,(c-optbeg)*sizeof(UCS2CHAR));
          *(wargv[argc]+(c-optbeg)) = 0;
          argc++;
        }
        else {
          if (!encoding->isSpace(c)) {
            optbeg = c;
            while (!encoding->isSpace(c) && *c != 0) {
              c++;
            }
            wargv[argc]=calloc(c-optbeg+1, sizeof(UCS2CHAR));
            memcpy(wargv[argc],optbeg,(c-optbeg)*sizeof(UCS2CHAR));
            *(wargv[argc]+(c-optbeg)) = 0;
            argc++;
          }
        }
      }
    }
    {
      int rc;
      char **returnedUTF8argv;
      optind = 1;
      recursiveCall = 1;
      sp83UTF8ArrayFromUCS2Array ( argc,
                                   (tsp81_UCS2Char **)wargv,
                                   &returnedUTF8argv );
      rc = c_main(argc, returnedUTF8argv); 
      for (argc--;argc >= 0; argc--) {
        free(returnedUTF8argv[argc]);
      }
      if (!rc) 
        readResult = fgetUCS2s( isUCS2, buffer, fp);
      else {
        printf("Processing batchfile failed\n");
        exit(1);
      }
    }
    for (argc--;argc >= 0; argc--) {
      free(wargv[argc]);
    }
  }
  fclose(fp);
}

/*---------------------------------------
  Function: fgetUCS2s
  Description: Read a string of UCS2 character using fgetws

    On systems where wchar_t is larger than UCS2CHAR , first wchar_t array is used and the result
    is copied.

  Arguments: buffer [in] UCS2 character buffer
             maxChar [in] size of buffer
             fp [in] file to read from
  Return value: NULL if fgetUCS2s failed
 */
static wchar_t *fgetUCS2s( bool isUCS2, UCS2CHAR *buffer, FILE *fp )
{
  wchar_t *result;
  int i;
  bool pureAscii8;

  SAPDB_memset(buffer, 0, STRING_BUFFER_SIZE*sizeof(UCS2CHAR));

#if defined(_WIN32)
  pureAscii8 = !isUCS2;
#else
  pureAscii8 = usePureAscii8;
#endif

  if ( pureAscii8 )
  {
        tsp00_Byte charBuffer[STRING_BUFFER_SIZE+1];

        SAPDB_memset(charBuffer, 0, sizeof(charBuffer));
        result = (wchar_t *)fgets((char *)&charBuffer[0], STRING_BUFFER_SIZE, fp);
        if ( result )
        {
            for ( i = 0; i < STRING_BUFFER_SIZE; i++ )
            {
              buffer[i] = charBuffer[i];
              if ( buffer[i] == '\x0d' || buffer[i] == '\x0a' || buffer[i] == 0 )
              {
                buffer[i] = 0;
                break;
              }
            }
        }
  }
  else
  {
      if ( sizeof(wchar_t) != sizeof(UCS2CHAR) )
      {
        wchar_t wcharBuffer[STRING_BUFFER_SIZE+1];

        if ( beVerbose ) printf("wchar_t does not match UCS2CHAR\n");
        SAPDB_memset(wcharBuffer, 0, sizeof(wcharBuffer));
        result = fgetws(wcharBuffer, STRING_BUFFER_SIZE, fp);
        if ( result )
        {
            for ( i = 0; i < STRING_BUFFER_SIZE; i++ )
            {
              buffer[i] = wcharBuffer[i];
              if ( buffer[i] == '\x0d' || buffer[i] == '\x0a' || buffer[i] == 0 )
              {
                buffer[i] = 0;
                break;
              }
            }
        }
      }
      else
      {
        if ( beVerbose ) printf("wchar_t matches UCS2CHAR\n");
        result = fgetws((wchar_t *)buffer, STRING_BUFFER_SIZE, fp);
        if ( result )
        {
            for ( i = 0; i < STRING_BUFFER_SIZE; i++ )
            {
              if ( buffer[i] == '\x0d' || buffer[i] == '\x0a' || buffer[i] == 0 )
              {
                buffer[i] = 0;
                break;
              }
            }
        }
      }
  }

  return result;
}

/*---------------------------------------
  Function: readString
  Description: Read a 7bit ASCII string and copy it padded with blanks
  Arguments: fp [in] file to read from
             pReadResult [inout] former / returned read result
             pInsertEntry [out] flag for invalid entry (updated only if needed)
             explain [in] Name of parameter for error output
             dest [out] destination (fill with blanks)
             maxChar [in] size of destination
 */
static void readString( bool isUCS2,
                  FILE *fp,
                  wchar_t **pReadResult,
                  int *pInsertEntry,
                  const char *explain,
                  unsigned char *dest,
                  int maxChar )
{
  int i;
  UCS2CHAR buffer[STRING_BUFFER_SIZE+1];

  if ( !*pReadResult ) { *pInsertEntry = false; return; }
  SAPDB_memset(buffer, 0, sizeof(buffer));
  *pReadResult = fgetUCS2s( isUCS2, buffer, fp);
  if ( !*pReadResult ) { *pInsertEntry = false; return; }
  SAPDB_memset(dest, ' ', maxChar);
  for ( i = 0; (i < maxChar) && (buffer[i] != 0); i++ )
  {
    if ( (buffer[i] & 0x7f) != buffer[i] )
    {
      printf("Warning: Non ascii7 parameter ");
      wprintf(L"%ls", buffer);
      printf(" for '%s' (parameter ignored)\n", explain );
      *pInsertEntry = false;
      break;
    }
    dest[i] = (char)(buffer[i] & 0x7f);
  }
  if ( beVerbose ) printf("readString(%s): %*.*s\n", explain, maxChar, maxChar, dest );
}

/*---------------------------------------
  Function: readNumber
  Description: Read a decimal number string
  All negativ numbers are returned as '-1'.

  Arguments: fp [in] file to read from
             pReadResult [inout] former / returned read result
             pInsertEntry [out] flag for invalid entry (updated only if needed)
             explain [in] Name of parameter for error output
             maxNumber [in] maximum number value allowed
 */
static int readNumber( bool isUCS2,
                       FILE *fp,
                       wchar_t **pReadResult,
                       int *pInsertEntry,
                       const char *explain,
                       unsigned int maxNumber )
{
  unsigned int i;
  int negativ = 0;
  unsigned long number;
  UCS2CHAR buffer[STRING_BUFFER_SIZE+1];
  char    asciiBuffer[STRING_BUFFER_SIZE+1];

  if ( !*pReadResult ) { *pInsertEntry = false; return -1; }
  SAPDB_memset(buffer, 0, sizeof(buffer));
  *pReadResult = fgetUCS2s( isUCS2, &buffer[0], fp);
  if ( !*pReadResult ) { *pInsertEntry = false; return -1; }

  for ( i = 0; buffer[i] != 0 && i < STRING_BUFFER_SIZE; i++ )
  {
    if ( (buffer[i] & 0x7f) != buffer[i] )
    {
      printf("Warning: Non ascii7 parameter ");
      wprintf(L"%ls", buffer);
      printf(" for '%s' (parameter ignored)\n", explain );
      return -1;
    }
    asciiBuffer[i] = (char)(buffer[i] & 0x7f);
  }
  asciiBuffer[i] = 0;

  if ( beVerbose ) printf("readNumber(%s): %s\n", explain, asciiBuffer);

  for ( i = 0; i < strlen(asciiBuffer); i++ )
  {
    if ( asciiBuffer[i] != '*' )
	break;
  }

  /* empty or '*****...' argument found */
  if ( asciiBuffer[i] == 0 )
  {
    return -1;
  }

  for ( i = 0; (asciiBuffer[i] == ' ') || (asciiBuffer[i] == '\t'); i++ )
	;

  if ( asciiBuffer[i] == '+' )
  {
     for ( ++i; (asciiBuffer[i] == ' ') || (asciiBuffer[i] == '\t'); i++ )
	;
  } else if ( asciiBuffer[i] == '-' )
  {
    negativ = 1;
    for ( ++i; (asciiBuffer[i] == ' ') || (asciiBuffer[i] == '\t'); i++ )
	;
  }

  for ( number = 0UL; (number < maxNumber) && (asciiBuffer[i] != 0); i++ )
  {
     switch(asciiBuffer[i])
     {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		number = number * 10 + asciiBuffer[i] - '0';
		break;
	default:
                printf("Warning: Non digit parameter 0x%x for '%s' (parameter ignored)\n",
	            asciiBuffer[i], explain );
		*pInsertEntry = false;
                return -1;
     }
  }

  if ( number > maxNumber )
  {
    *pInsertEntry = false;
    return -1;
  }

  return ( negativ ? -1 : (int)number );
}

/*---------------------------------------
  Function: readUsernamePassword
  Description: Read a username/password combination

  Return username and password arguments.

  Arguments: fp [in] file to read from
             pReadResult [inout] former / returned read result
             pInsertEntry [out] flag for invalid entry (updated only if needed)
             pUsername [out] username found (terminated by ',')
             pPassword [out] password found (terminated by '\0')
 */
static void readUsernamePassword( bool isUCS2,
                                  FILE *fp,
                                  wchar_t **pReadResult,
                                  int *pInsertEntry,
                                  tsp00_KnlIdentifier username,
                                  tsp00_KnlIdentifier password )
{
  int i;
  void *fillBuffer;
  tsp00_Uint4 fillLength;
  UCS2CHAR buffer[STRING_BUFFER_SIZE+1];

  if ( !*pReadResult ) { *pInsertEntry = false; return; }
  SAPDB_memset(buffer, 0, sizeof(buffer));
  *pReadResult = fgetUCS2s( isUCS2, &buffer[0], fp);
  if ( !*pReadResult ) {
      *pInsertEntry = false;
      *pReadResult = fgetUCS2s( isUCS2, &buffer[0], fp);
      return;
  }

  SAPDB_memcpy(username, buffer, sizeof(tsp00_KnlIdentifier));

  for ( i = 0; i < (sizeof(tsp00_KnlIdentifier)/sizeof(UCS2CHAR)); i++ )
      if ( 0 == buffer[i] )
          break;
  if ( i > (sizeof(tsp00_KnlIdentifier)/2) )
  {
      printf("Warning:Username too long: Maximum %d characters allowed\n",
              sizeof(tsp00_KnlIdentifier)/2 );
      *pInsertEntry = false;
      *pReadResult = fgetUCS2s( isUCS2, &buffer[0], fp);
      return;
  }
  fillBuffer = &username[i*sizeof(UCS2CHAR)];
  fillLength = sizeof(tsp00_KnlIdentifier) - (i*sizeof(UCS2CHAR));
  sp77encodingUCS2Native->fillString(&fillBuffer, &fillLength, fillLength/2, ' ');

  SAPDB_memset(buffer, 0, sizeof(buffer));
  *pReadResult = fgetUCS2s( isUCS2, &buffer[0], fp);
  if ( !*pReadResult ) { *pInsertEntry = false; return; }

  SAPDB_memcpy(password, buffer, sizeof(tsp00_KnlIdentifier));

  for ( i = 0; i < (sizeof(tsp00_KnlIdentifier)/sizeof(UCS2CHAR)); i++ )
      if ( 0 == buffer[i] )
          break;
  if ( i > (sizeof(tsp00_KnlIdentifier)/2) )
  {
      printf("Warning:Password too long: Maximum %d characters allowed\n",
              sizeof(tsp00_KnlIdentifier)/2 );
      *pInsertEntry = false;
      return;
  }
  fillBuffer = &password[i*sizeof(UCS2CHAR)];
  fillLength = sizeof(tsp00_KnlIdentifier) - (i*sizeof(UCS2CHAR));
  sp77encodingUCS2Native->fillString(&fillBuffer, &fillLength, fillLength/2, ' ');

  return;
}

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

    SAPDB_memset(destAscii8, ' ', srcLen);
    for ( i = 0; (i < srcLen/2) && (srcUCS2[i] != ' ') ; i++ )
    {
        if ( srcUCS2[i] < 256 )
        {
            destAscii8[i] = (unsigned char)srcUCS2[i];
        }
        else
        {
            SAPDB_memset(destAscii8, ' ', srcLen);
            return false;
        }
    }
    return true;
}

/*---------------------------------------
  Function: updatePasswordRecord
  Description: Fill Ascii Username and crypt passwords

  Arguments: passwordUCS2 [in] UCS2 encoded password
             paramUserRecord [in] The XUSER record to use
 */
static void updatePasswordRecord( tsp00_KnlIdentifier passwordUCS2,
                                  struct tsp4_xuser_record *paramUserRecord )
{
    bool UCS2only = false;
    tsp00_KnlIdentifier passwordAscii8;

    if ( !Ascii8Copy(&paramUserRecord->xu_user[0], (const unsigned short *)&paramUserRecord->xu_userUCS2[0], sizeof(tsp00_KnlIdentifier)) )
    {
        UCS2only = true;
    }
    else if ( !Ascii8Copy(&passwordAscii8[0], (const unsigned short *)&passwordUCS2[0], sizeof(tsp00_KnlIdentifier)) )
    {
        UCS2only = true;
    }
    else
    {
        if ( passwordAscii8[0] != ' ' && passwordAscii8[0] != 0 )
        {
            s02applencrypt ( passwordAscii8, paramUserRecord->xu_password );
        }
        else
        {
            SAPDB_memset(paramUserRecord->xu_password, 0, sizeof(tsp00_CryptPw));
        }

        if ( (*(unsigned short *)&passwordUCS2[0] != ((unsigned short)' '))
          && (*(unsigned short *)&passwordUCS2[0] != 0) )
        {
            s02applencrypt ( passwordUCS2, paramUserRecord->xu_passwordUCS2 );
        }
        else
        {
            SAPDB_memset(paramUserRecord->xu_passwordUCS2, 0, sizeof(tsp00_CryptPw));
        }
    }

    if ( UCS2only )
    {
        SAPDB_memset(paramUserRecord->xu_user, ' ', sizeof(tsp00_KnlIdentifier));
        SAPDB_memset(paramUserRecord->xu_password, 0, sizeof(tsp00_CryptPw));
        if ( (*(unsigned short *)&passwordUCS2[0] != ((unsigned short)' '))
          && (*(unsigned short *)&passwordUCS2[0] != 0) )
        {
            s02applencrypt ( passwordUCS2, paramUserRecord->xu_passwordUCS2 );
        }
        else
        {
            SAPDB_memset(paramUserRecord->xu_passwordUCS2, 0, sizeof(tsp00_CryptPw));
        }
    }
}

/*---------------------------------------
  Function: updateUserRecord
  Description: Verify the given entry and update XUSER information

  The entry is verified (isolation level and sqlmodname) and username,password
  are checked and crypted. If everything want ok so far, the update of XUSER
  information is done.

  Arguments: passwordUCS2 [in] UCS2 encoded password
             paramUserRecord [in] The XUSER record to use
 */
static void updateUserRecord( struct tsp4_xuser_record *    paramUserRecord,
                              SAPDB_Char *                  accountName)
{
  tsp00_Bool    ok;
  tsp00_ErrText errText;

  SAPDB_memset( errText, 0, sizeof(tsp00_ErrText));

  /* Replace empty key by DEFAULT */
  if ( paramUserRecord->xu_key[0] == ' ' || paramUserRecord->xu_key[0] == 0 )
  {
      SAPDB_memset( paramUserRecord->xu_key, ' ', sizeof(tsp4_xuserkey) );
      SAPDB_memcpy( paramUserRecord->xu_key, "DEFAULT", strlen("DEFAULT") );
  }

  /* Check valid sqlmodenames */
  if ( memcmp( paramUserRecord->xu_sqlmode, "ADABAS  ", 8 ) == 0
    || memcmp( paramUserRecord->xu_sqlmode, "INTERNAL", 8 ) == 0
    || memcmp( paramUserRecord->xu_sqlmode, "SQL-DB  ", 8 ) == 0
    || memcmp( paramUserRecord->xu_sqlmode, "SAPDB   ", 8 ) == 0 )
  {
    SAPDB_memcpy( paramUserRecord->xu_sqlmode, "INTERNAL", 8);
  } else if ( !( memcmp( paramUserRecord->xu_sqlmode, "ANSI    ", 8 ) == 0
              || memcmp( paramUserRecord->xu_sqlmode, "DB2     ", 8 ) == 0
              || memcmp( paramUserRecord->xu_sqlmode, "ORACLE  ", 8 ) == 0
              || memcmp( paramUserRecord->xu_sqlmode, "SAPR3   ", 8 ) == 0
              || memcmp( paramUserRecord->xu_sqlmode, "ADABAS  ", 8 ) == 0
              || memcmp( paramUserRecord->xu_sqlmode, "        ", 8 ) == 0 ) )
  {
    printf("Warning: Illegal sqlmode specified: %8.8s (ignored)\n",
      paramUserRecord->xu_sqlmode);
    SAPDB_memcpy( paramUserRecord->xu_sqlmode, "        ", 8);
  }

  /* Check valid isolation levels */
  switch( paramUserRecord->xu_isolation )
  {
  case -1:
  case 0:
  case 1:
  case 2:
  case 3:
  case 9:
  case 10:
  case 15:
  case 20:
  case 30:
    break;
  default:
    printf("Warning: Illegal isolation level specified: %d (ignored)\n",
      paramUserRecord->xu_isolation);
    paramUserRecord->xu_isolation = -1;
    break;
  }

  sqlxuopenuser (accountName, errText, &ok );
  if ( !ok )
  {
    printf("FATAL: Open xuser entry file failed: %40.40s\n", errText);
    exit(2);
  }

  sqlputuser (paramUserRecord, accountName, errText, &ok);
  if ( !ok )
  {
    printf("FATAL: Put xuser entry failed: %40.40s\n", errText);
    sqlxucloseuser ( accountName, errText, &ok );
    exit(2);
  }

  sqlxucloseuser (accountName, errText, &ok );
  if ( !ok )
  {
    printf("FATAL: Close xuser entry failed: %40.40s\n", errText);
    exit(2);
  }

  return;
}


/*---------------------------------------------------------------------------*/

#if !defined(_WIN32)

/*
 Function: fgetws

 Description: On all UNIX platforms this routine replaces fgetws

  This makes sure UTF8 character input works as wanted...
  Read fails if fgets fails or non UTF8 characters are detected.
  Input is hard limited to 1024 characters.

 Arguments: WString [out] wchar_t buffer to fill
            Number  [in] size of buffer in wchar_t
            String  [in] the input stream
 Return value: NULL if read failed
 */
static wchar_t *fgetws(wchar_t *WString, int Number, FILE *Stream)
{
#define MAX_FGETWS_BUFFER 1024
    char            buffer[MAX_FGETWS_BUFFER];
    UCS2CHAR        ucs2Buffer[MAX_FGETWS_BUFFER];
    char           *result;
    tsp00_Uint4 bytesWritten;
    tsp00_Uint4 bytesParsed;
    tsp78ConversionResult convResult;
    unsigned int byteLength;
    unsigned int charLength;

    /* Read UTF8 Byte Input Stream */
    errno = 0;
    result = fgets(buffer, sizeof(buffer), Stream);
    if ( result == NULL )
    {
#if !defined(OSF1)
        if ( errno == EOVERFLOW )
        {
            printf("Failed to read from input stream (OVERFLOW) Maximum %d chars\n", MAX_FGETWS_BUFFER);
        } else
#endif
            if ( errno != 0 )
        {
            printf("Failed to read from input stream (%d)\n", errno );
        }
        return (wchar_t *)0;
    }

    /* Convert into UCS2 buffer */
    if ( sp83UTF8Convert_Success !=
        sp83UTF8_Bytelen((const tsp00_Byte *)buffer, strlen(buffer)+1, &byteLength, &charLength) )
    {
        printf("Input stream contains non UTF8 character\n");
        return NULL;
    }
    convResult = sp78convertBuffer( sp77encodingUCS2Native,
                                    ucs2Buffer,
                                    Number,
                                    &bytesWritten,
                                    sp77encodingUTF8,
                                    buffer,
                                    strlen(buffer)+1,
                                    &bytesParsed );
    if ( sp78_Ok != convResult )
    {
        printf("UTF8->UCS2 conversion failed:%d\n", (int)convResult);
        return NULL;
    }
    else
    {
        /* Convert into wchar_t buffer */
        int i;
        for ( i = 0; i < (bytesWritten/2); i++ )
        {
            WString[i] = ucs2Buffer[i];
        }
        return WString;
    }
}
#endif /* !WIN32 */
/*---------------------------------------------------------------------------*/

#if defined(LINUX) || defined(HPUX)
/* primitive replacement for LINUX and HPUX */
static int      wprintf(const wchar_t *format, ...)
{
  /* ROUTINE_DBG_MSP00 ("wprintf"); */
  int             i;
  int             result;
  char            buffer[1024];
  char            asciiFormat[1024];
  va_list         args;

  for ( i = 0; format[i]; i++ )
  {
      asciiFormat[i] = (char)(format[i] & 0x0FF);
  }
  asciiFormat[i] = 0;

  va_start (args, format);
  result = sp77vsprintfUnicode( sp77encodingUTF8, buffer, sizeof(buffer), asciiFormat, args);
  va_end (args);

  if ( result >= 0 )
  {
      (void)printf("%s", buffer);
  }
  return result;
}
#endif /* LINUX || HPUX */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

