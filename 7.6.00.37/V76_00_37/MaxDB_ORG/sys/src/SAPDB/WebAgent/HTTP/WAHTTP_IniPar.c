/*!**********************************************************************

  module: WAHTTP_IniPar.c

  -----------------------------------------------------------------------

  responsible:  Tony Guepin,
                Markus Oezgen

  special area: SAP DB WWW

  description:  Registry, INI Files, Settings

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



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





************************************************************************/

#define MODULE_NAME "WAHTTP_IniPar.c"

/****************************************************************************
 ***
 *** Includes
 ***
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Entries.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_IniPar.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Server.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_HTTP.h"
#include "SAPDB/Registry/Reg_Registry.h"

/* webdav */
#define RESOURCE_FILE
#include "gsp100.h"
#include "hsp100.h"
#include "hsp77.h"

#define INI_PAR_NF          "Iniparameter not found"
#define INI_MIMETYPEFILE    "mimetypefile"
#define MIMETYPE_NOT_FOUND  "Mimetype not found"

#define OPEN_FILE_ERR       "File open error "

#define MAX_INI_PAR_LENGTH      100
#define MAX_FILNAMELENGTH       MAXURLSIZE

#define INIPAR_SECTION  "SapDbPort80"

/*
static  unsigned int    ncur_inipars = 0;
static  st_entry    *inipars = NULL;
*/
static  unsigned int     ncur_mimetypes = 0;
static  st_entry        *mimetypes = NULL;


/****************************************************************************
 ***
 *** Registry
 ***
 ****************************************************************************/

#ifdef REG_WIN32
    #define WAHTTP_REGISTRY                 "SOFTWARE\\SAP\\SAP DBTech\\WebAgent75"
    #define WAHTTP_REG_SECTION_HTTP         WAHTTP_REGISTRY"\\HTTP"
#else
    #define WAHTTP_REGISTRY                 "/usr/spool/sql/ini/WebAgent75.ini"
    #define WAHTTP_REG_SECTION_HTTP         "HTTP"
#endif

#define WAHTTP_REG_KEY_PORT                 "Port"
#define WAHTTP_REG_KEY_THREADING            "Threading"
#define WAHTTP_REG_KEY_BROWSABLE            "Browsable"
#define WAHTTP_REG_KEY_LOG_REQUESTS         "LogRequests"
#define WAHTTP_REG_KEY_LOG_DIRECTORY        "LogDirectory"
#define WAHTTP_REG_KEY_DOCUMENT_ROOT        "DocumentRoot"
#define WAHTTP_REG_KEY_DEFAULT_DOCUMENT     "DefaultDocument"
#define WAHTTP_REG_KEY_MIME_TYPE_FILE       "MimeTypeFile"
#define WAHTTP_REG_KEY_MAX_THREADS          "MaxThreads"
#define WAHTTP_REG_KEY_WEBDAV               "enableWebDAV"

#define WAHTTP_REG_DEFVAL_DEFAULT_DOCUMENT  "default.htm"

typedef struct st_wahttp_ini {
    sapdbwa_UInt4   port;
    sapdbwa_Bool    threading;
    sapdbwa_Bool    browseable;
    sapdbwa_Bool    logRequests;
    char            logDirectory[MAX_FILENAME_LEN+1];
    char            documentRoot[MAX_FILENAME_LEN+1];
    char            defaultdocument[MAX_FILENAME_LEN+1];
    char            mimeTypeFile[MAX_FILENAME_LEN+1];
    sapdbwa_UInt4   maxThreads;
	sapdbwa_Bool	enableWebDAV;
} WAHTTP_Ini;

static WAHTTP_Ini   iniValues;

sapdbwa_Bool ReadConf ( char            *exePath,
                        char            *confPath,
                        sapdbwa_UInt2    confPathSize,
                        char            *errorText,
                        sapdbwa_UInt2    errorTextSize );

/****************************************************************************
 ***
 *** Methods
 ***
 ****************************************************************************/

sapdbwa_UInt4 GetPort(){
    return iniValues.port;
}

sapdbwa_Bool GetThreading(){
    return iniValues.threading;
}

sapdbwa_Bool GetBrowsable(){
    return iniValues.browseable;
}

sapdbwa_Bool GetLogRequests(){
    return iniValues.logRequests;
}

char* GetLogDirectory(){
    return iniValues.logDirectory;
}

char* GetDocumentRoot(){
    return iniValues.documentRoot;
}

char* GetDefaultDocument(){
    return iniValues.defaultdocument;
}

char* GetMimeTypeFile(){
    return iniValues.mimeTypeFile;
}

sapdbwa_UInt4 GetMaxThreads(){
    return iniValues.maxThreads;
}

sapdbwa_Bool GetEnableWebDav(){
    return iniValues.enableWebDAV;
}

/****************************************************************************
 ***
 *** ReadConf
 ***
 *** Reads the wahttp.conf file to get the path for the setting descriptions.
 *** Depends on the plattform these settings are described in the registry
 *** or in an .ini file. You can specify in the wahttp.conf file where the
 *** web server can find its settings. With this feature you can start 
 *** many web servers with different settings.
 *** You can also specify the path for the settings with the command line
 *** option -f <my_settings_path>.
 *** 
 ****************************************************************************/

sapdbwa_Bool ReadConf ( char            *exePath,
                        char            *confPath,
                        sapdbwa_UInt2    confPathSize,
                        char            *errorText,
                        sapdbwa_UInt2    errorTextSize )
{

    static const BUF_SIZE = 4095;

    sapdbwa_Int4         file = -1;
    tsp05_RteFileError   fError;
    tsp00_Longint          readLen = 0;
    char                 fileName[MAX_FILENAME_LEN+1] = "";
    char                *slash = NULL;

    /* Checks   */
    if ( !exePath || !confPath ) {
        return sapdbwa_False;
    }

    /* Build filename   */
    slash = exePath+strlen(exePath)-1;
    if (*slash==DIRPATH_SEP) {
        sp77sprintf( fileName, MAX_FILENAME_LEN, "%swahttp.conf", exePath );
    } else {
        sp77sprintf( fileName, MAX_FILENAME_LEN, "%s%cwahttp.conf", exePath, DIRPATH_SEP );
    }

    /* Open file for reading in binary mode */
    sqlfopenc( fileName, sp5vf_text, sp5vf_read, sp5bk_buffered, &file, &fError );
    if ( fError.sp5fe_result != vf_ok ) {
        sp77sprintf( errorText, errorTextSize, "Cannot open file: %s", fileName );

        return sapdbwa_False;
    }

    /* Read file into buffer    */
    sqlfreadc ( file, confPath, confPathSize, &readLen, &fError );
    confPath[confPathSize-1] = '\0';

    /* Close file   */
    sqlfclosec( file, sp5vf_close_normal, &fError );

    return sapdbwa_True;

}

/****************************************************************************
 ***
 *** 
 ***
 ****************************************************************************/

/*
int IniReadPar
(
 char       Parameter[],
 unsigned short ValueSize,
 char       Value[],
 unsigned short ErrorTextSize,
 char       *ErrorText
)
{
 char   *tmp = NULL;

 tmp = EntryWertCase(ncur_inipars, inipars, Parameter);
 if (tmp)
 {
    strcpymax(ValueSize, Value, tmp);
    return(TRUE);
 }
 else
 {
    strcpymax(ErrorTextSize, ErrorText, INI_PAR_NF);
    strcatmax(ErrorTextSize, ErrorText, Parameter);
    return(FALSE);
 } 
}
*/

static int ReadIniPars
(
 char       IniPath[],
 unsigned int   *ncur_inipars,
 st_entry   **inipars,
 unsigned short ErrorTextSize,
 char       *ErrorText
)
/* Find parameter value pair in the ini file with parameter.
 * If there is a section for WEBDB, check it, otherwise ignore.
 *
 * Parameter/Value pairs are one-liners, separated by an equal(=) sign.
 */
{
 char       *path = IniPath;
 FILE       *fin = NULL;
 int        found = FALSE, error = FALSE;
 char       fParameter[MAX_INI_PAR_LENGTH+1];
 char       tmpParameter[MAX_INI_PAR_LENGTH+1];
 char       Value[MAX_INI_PAR_LENGTH+1];
 int        ValueSize = sizeof(Value);
 int        i = 0;
 char       c;
 int        l = 0;
 char       *tmp = NULL;
 int        ThisIsWebDb = TRUE;
 size_t     tmpParameterLg = 0;
 int        SectionStart = FALSE;

 fin = fopen(IniPath, "r");

 if (!fin) {
    if (ErrorText)
    {
        strcpymax(ErrorTextSize, ErrorText, OPEN_FILE_ERR);
        strcatmax(ErrorTextSize, ErrorText, " ");
        strcatmax(ErrorTextSize, ErrorText, IniPath);
        strcatmax(ErrorTextSize, ErrorText, " (");
        strcatmax(ErrorTextSize, ErrorText, strerror(errno));
        strcatmax(ErrorTextSize, ErrorText, ")");
    }
    return(FALSE);
 }

 found = FALSE;
 while (!error && !found && !feof(fin))
 {

    SectionStart = FALSE;
     /* skip white spaces */
    while (!error && !feof(fin))
    {
        if (EOF == (c = fgetc(fin)))
            break;
        SectionStart = c == '[';
        if (!isspace(c))
            if (EOF == ungetc(c, fin))
            {
                error = TRUE;
                break;
            }
         else
             break;
     }

     /* Parameter */
     fParameter[0] = '\0';
     i = 0;
     while (!error && !feof(fin)) {
        if (EOF == (fParameter[i] = fgetc(fin)))
        {
             fParameter[i] = '\0';
             break;
        }
        if (SectionStart && fParameter[i] == ']')
        {
            SectionStart = FALSE;
            fParameter[i+1] = '\0';
            break;
        }
        if (!SectionStart &&
             (fParameter[i] == '=' /*||
              isspace(fParameter[i])*/)) {
            if (fParameter[i] == '=')
                if (EOF == ungetc(fParameter[i], fin))
                {
                    error = TRUE;
                    break;
                }
            fParameter[i] = '\0';
            break;
         }
         else {
             fParameter[i+1] = '\0';
             i++;
         }
     }

    strcpy(tmpParameter, fParameter);
    tmpParameterLg = strlen(tmpParameter);
    if (tmpParameterLg > 2 &&
        tmpParameter[0] == '[' && 
        tmpParameter[tmpParameterLg-1] == ']')
    {
        tmpParameter[tmpParameterLg-1] = '\0';
        trim(&tmpParameter[1]);
        if (strcasecmp(&tmpParameter[1], INIPAR_SECTION))
        {
            error = TRUE;
            break;
        }
        ThisIsWebDb = TRUE;
        continue;
    }

     /* skip white spaces */
     while (!error && !feof(fin)) {
         if (EOF == (c = fgetc(fin)))
             break;
         if (!isspace(c))
             if (EOF == ungetc(c, fin)) {
                error = TRUE;
                break;
             }
         else
             break;
     }

     /* = */
     if (!error && !feof(fin)) {
         if (EOF == (c = fgetc(fin)))
             break;
         if (c != '=') {
            error = TRUE;
         }
     }

     /* skip white spaces */
     while (!error && !feof(fin)) {
         if (EOF == (c = fgetc(fin)))
             break;
         if (!isspace(c))
             if (EOF == ungetc(c, fin)) {
                error = TRUE;
                break;
             }
         else
             break;
     }

    /* Value (until eoln)*/
    i = 0;
    while (i < ValueSize-1 && !error && !feof(fin))
    {
        if (EOF == (c = fgetc(fin)))
        {
             break;
        }
        if (c== '\n') {
/*      if (isspace(c)) {   allow spaces */
            break;
        }
        else
        {
            Value[i] = c;
            i++;
        }
    }
    Value[i] = '\0';

    if (!error)
    {
        if (!SetEntryWert(ncur_inipars, inipars,
                  strlen(fParameter), fParameter,
                  strlen(Value), Value,
                  ErrorTextSize, ErrorText))
            return(FALSE);
    }
 }

 if (fin) fclose(fin);

 if (error)
 {
    strcpymax(ErrorTextSize, ErrorText, "ADAWWW_INI_WRONG_FORMAT");
    return(FALSE);
 }
 else
    return(TRUE);
}


void ExitIniPar
(
 void
)
{
/*
 DeleteEntries(ncur_inipars, inipars);
*/
}


/*
int IniSetPar
(
 char       Parameter[],
 char       Value[],
 unsigned short ErrorTextSize,
 char       *ErrorText
)
{
 if (!UpdEntryWert(&ncur_inipars, &inipars,
           strlen(Parameter), Parameter,
           strlen(Value), Value,
           ErrorTextSize, ErrorText))
    return(FALSE);
 else
    return(TRUE);
}
*/

int ReadMimeTypes( char           *errorText,
                   unsigned short  errorTextSize  )
{

 char       Line[MAX_INI_PAR_LENGTH+1];
 char       *l = 0, *end = 0, *eoln_or_cr = 0;
 char       *tmpMimeType = 0;
 int        error = FALSE;
 char       MimeTypeFilename[MAX_FILENAME_LEN+1];
 FILE       *fin = NULL;
 size_t     length = 0, i = 0;

    strncpy(MimeTypeFilename, GetMimeTypeFile(), MAX_FILENAME_LEN );
    MimeTypeFilename[MAX_FILENAME_LEN] = '\0';

/*
 ncur_mimetypes = 0;
 mimetypes = NULL;
 return(TRUE);
*/

/*
 if (!IniReadPar(INI_MIMETYPEFILE,
         sizeof(MimeTypeFilename), MimeTypeFilename,
         ErrorTextSize, ErrorText))
    return(FALSE);
*/

 if ((strlen(MimeTypeFilename) == 0) ||
     (NULL == (fin = fopen(MimeTypeFilename, "r"))))
 {
    strcpymax(errorTextSize, errorText, OPEN_FILE_ERR);
    strcpymax(errorTextSize, errorText, MimeTypeFilename);
    return(FALSE);
 }

 while (!error && fgets(Line, sizeof(Line), fin)) {
    tmpMimeType = 0;
    l = Line;

    while (eoln_or_cr = strpbrk(l, "\r\n"))
        *eoln_or_cr = ' ';  /* replace the eoln with space */
    trim(l);

    /* skip white spaces */
    while (*l != '\0' && (*l == ' ' || *l == '\t')) l++;
    if (*l == '\0') continue;

    /* Check fo comment */
    if (*l == '#')  continue;

    end = strpbrk(l, " \t");

    if (!end) continue; /* Just one word in the line */
    *end = '\0';        /* terminate the mimetype */
    tmpMimeType = l;
    l = end + 1;

    while (!error && *l) {
        /* skip white spaces */
        while (*l != '\0' && (*l == ' ' || *l == '\t')) l++;
        if (*l == '\0') continue;

        /* Check fo comment */
        if (*l == '#')  continue;

        end = strpbrk(l, " \t");

        if (end)
            *end = '\0';        /* terminate the FileType */

        length = strlen(l);
        for (i=0; i<length; i++) l[i] = tolower(l[i]);

        if (!SetEntryWert(&ncur_mimetypes, &mimetypes,
                  strlen(l), l,
                  strlen(tmpMimeType), tmpMimeType,
                  errorTextSize, errorText))
        {
            error = TRUE;
            break;
        }
        if (end)
            l = end + 1;
        else
            break;
    }   
 }

 fclose(fin);

 if (error)
 {
    DeleteEntries(ncur_mimetypes, mimetypes);
 }

 return(!error);
}

int IniGetMimeType
(
 unsigned int   MimeTypeSize,
 char       MimeType[],
 char       FileExtension[],
 unsigned short ErrorTextSize,
 char       *ErrorText
)
{
 char   *tmp = NULL;

 tmp = EntryWertCase(ncur_mimetypes, mimetypes, FileExtension);
 if (tmp)
 {
    strcpymax(MimeTypeSize, MimeType, tmp);
    return(TRUE);
 }
 else
 {
    strcpymax(ErrorTextSize, ErrorText, MIMETYPE_NOT_FOUND);
    strcatmax(ErrorTextSize, ErrorText, " ");
    strcatmax(ErrorTextSize, ErrorText, FileExtension);
    return(FALSE);
 } 
}

/*========================================================================*/

int InitIniPar ( char                                iniPath[],
                 struct wahttp_alternate_settings   *alternateSettings,
                 char                               *usedIniFile,
                 char                               *errorText,
                 sapdbwa_UInt2                       errorTextSize )
{

    char            valueBuf[REG_MAX_VALUE_LEN+1];
    char            usedSection[REG_MAX_SECTION_LEN+1];
    Reg_RegistryP   registry = NULL;

    /* Is option -f used?   */
    if (strlen(alternateSettings->iniFile)) {
        strncpy(usedIniFile, alternateSettings->iniFile, MAX_FILENAME_LEN);
        usedIniFile[MAX_FILENAME_LEN] = '\0';
    } else {
        /*                                                          */
        /* Get ini file to use from wahttp.conf                     */
        /*                                                          */
        /* On Windows systems ini file means registry path,         */
        /* whereby the main key HKEY_LOCAL_MACHINE is predefined    */
        /*                                                          */
        /* On Unix systems ini file means ini file,                 */
        /* with its whole path.                                     */
        /*                                                          */
        if ( !ReadConf( iniPath, usedIniFile, MAX_FILENAME_LEN, errorText, errorTextSize ) ) {
            return 0;
        }
    }

    /* If ini file is not described by wahttp.conf use default ini file */
    if ( strlen(usedIniFile) < 1 ) {
        strcpy(usedIniFile, WAHTTP_REGISTRY);
        strcpy(usedSection, WAHTTP_REG_SECTION_HTTP);
    } else {
#ifdef WIN32
        sp77sprintf(usedSection, REG_MAX_SECTION_LEN, "%s\\%s", usedIniFile, "HTTP");
#else
        strcpy(usedSection, WAHTTP_REG_SECTION_HTTP);
#endif
    }

    /* Init ini values  */
    iniValues.port = 80;
    iniValues.threading = sapdbwa_True;
    iniValues.browseable = sapdbwa_False;
    iniValues.logRequests = sapdbwa_True;
    iniValues.logDirectory[0] = '\0';
    iniValues.documentRoot[0] = '\0';
    iniValues.defaultdocument[0] = '\0';
    iniValues.mimeTypeFile[0] = '\0';
	iniValues.enableWebDAV = sapdbwa_False;

    /* Open registry */
    if ( !Reg_OpenRegistry( &registry, usedIniFile )) {
        sp77sprintf( errorText, errorTextSize, "Registry could not be read: %s", usedIniFile );
        return 0;
    }

    /* Get port */
    if (strlen(alternateSettings->port)) {
        iniValues.port = atol(alternateSettings->port);
    } else {
        if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_PORT, valueBuf, REG_MAX_VALUE_LEN )) {
            Reg_CloseRegistry( registry );
            sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_PORT );
            return 0;
        } else {
            if ( valueBuf ) {
                iniValues.port = atol( valueBuf );
            }
        }
    }

    /* Get threading value  *
    if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_THREADING, valueBuf, REG_MAX_VALUE_LEN )) {
        Reg_CloseRegistry( registry );
        sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_THREADING );
        return 0;
    } else {
        if ( strcmp( valueBuf, "0" ) == 0 ) {
            iniValues.threading = sapdbwa_False;
        } else {
            iniValues.threading = sapdbwa_True;
        }
    }
    */

    /* Get browsable value  */
    if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_BROWSABLE, valueBuf, REG_MAX_VALUE_LEN )) {
        Reg_CloseRegistry( registry );
        sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_BROWSABLE );
        return 0;
    } else {
        if ( strcmp( valueBuf, "0" ) == 0 ) {
            iniValues.browseable = sapdbwa_False;
        } else {
            iniValues.browseable = sapdbwa_True;
        }
    }

    /* Get logRequests value    */
    if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_LOG_REQUESTS, valueBuf, REG_MAX_VALUE_LEN )) {
        Reg_CloseRegistry( registry );
        sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_LOG_REQUESTS );
        return 0;
    } else {
        if ( strcmp( valueBuf, "0" ) == 0 ) {
            iniValues.logRequests = sapdbwa_False;
        } else {
            iniValues.logRequests = sapdbwa_True;
        }
    }

    /* Get logDirectory value   */
    if (strlen(alternateSettings->logDir)) {
        strncpy(iniValues.logDirectory, alternateSettings->logDir, sizeof(iniValues.logDirectory));
        iniValues.logDirectory[MAX_FILENAME_LEN] = '\0';
    } else {
        if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_LOG_DIRECTORY, valueBuf, REG_MAX_VALUE_LEN )) {
            Reg_CloseRegistry( registry );
            sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_LOG_DIRECTORY );
            return 0;
        } else {
            strncpy( iniValues.logDirectory, valueBuf, sizeof(iniValues.logDirectory) );
            iniValues.logDirectory[MAX_FILENAME_LEN] = '\0';
        }
    }

    /* Get documentRoot value   */
    if (strlen(alternateSettings->docRoot)) {
        strncpy(iniValues.documentRoot, alternateSettings->docRoot, sizeof(iniValues.documentRoot));
        iniValues.documentRoot[MAX_FILENAME_LEN] = '\0';
    } else {
        if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_DOCUMENT_ROOT, valueBuf, REG_MAX_VALUE_LEN )) {
            Reg_CloseRegistry( registry );
            sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_DOCUMENT_ROOT );
            return 0;
        } else {
            strncpy( iniValues.documentRoot, valueBuf, sizeof(iniValues.documentRoot) );
            iniValues.documentRoot[MAX_FILENAME_LEN] = '\0';
        }
    }

    /* Get defaultdocument value    */
    if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_DEFAULT_DOCUMENT, valueBuf, REG_MAX_VALUE_LEN )) {
        sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_DEFAULT_DOCUMENT );
        strcpy( iniValues.defaultdocument, WAHTTP_REG_DEFVAL_DEFAULT_DOCUMENT );
    } else {
        strncpy( iniValues.defaultdocument, valueBuf, sizeof(iniValues.defaultdocument) );
        iniValues.defaultdocument[MAX_FILENAME_LEN] = '\0';
    }

    /* Get mimeTypeFile value   */
    if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_MIME_TYPE_FILE, valueBuf, REG_MAX_VALUE_LEN )) {
        Reg_CloseRegistry( registry );
        sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_MIME_TYPE_FILE );
        return 0;
    } else {
        strncpy( iniValues.mimeTypeFile, valueBuf, sizeof(iniValues.mimeTypeFile) );
        iniValues.mimeTypeFile[MAX_FILENAME_LEN] = '\0';
    }

    /* Get maxThreads value */
    if (strlen(alternateSettings->threads)) {
        iniValues.maxThreads = atol(alternateSettings->threads);
    } else {
        if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_MAX_THREADS, valueBuf, REG_MAX_VALUE_LEN )) {
            Reg_CloseRegistry( registry );
            sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_MAX_THREADS );
            return 0;
        } else {
            iniValues.maxThreads = atol( valueBuf );
        }
    }

    /* Get enableWebDAV value  */
    if ( !Reg_GetRegistryKey( registry, usedSection, WAHTTP_REG_KEY_WEBDAV, valueBuf, REG_MAX_VALUE_LEN )) {
        Reg_CloseRegistry( registry );
        sp77sprintf( errorText, errorTextSize, "Registry value '%s' could not be read!", WAHTTP_REG_KEY_WEBDAV );
        return 0;
    } else {
        if ( strcmp( valueBuf, "0" ) == 0 ) {
            iniValues.enableWebDAV = sapdbwa_False;
        } else {
            iniValues.enableWebDAV = sapdbwa_True;
        }
    }

    /* Close registry */
    Reg_CloseRegistry( registry );

    /*
    if (!ReadIniPars(iniPath, &ncur_inipars, &inipars, sizeof(ErrorText), ErrorText)) {
        printf("%s\n", ErrorText);
        *  Log(ErrorText, -1);*
        return(FALSE);
    }
    */

    return(TRUE);
}

/*========================================================================*/

int IniReadParFile
(
 char   IPAR[],
 DynStr *Str
)
{
 char   filename[100];
 FILE   *fin = NULL;
 DynStr tmp;
 size_t lg = 0;

/*
 if (IniReadPar(IPAR, sizeof(filename), filename, 0, NULL))
 {
    tmp = DSinit();
    fin = fopen(filename, "r");
    if (!fin)
    {
        char tmp[100];
        sprintf(tmp, "Could not open %s", filename);
        Log(tmp, -1);
        return(FALSE);
    }
    else
    {
        do
        {
            DSextent(&tmp, 100);
            lg = fread(&tmp->str[tmp->lg], 1, 100, fin);
            tmp->lg += lg;
        }
        while (lg >= 100);
        fclose(fin);
        *Str = tmp;
    }
    return(TRUE);
 }
 else
*/
    return(FALSE);
}

/* webdav */
/* function for reading version and build informations */
extern void wahttp_get_info(char* wahttp_name, char *wahttp_version, char *wahttp_build) {

	sp77sprintf(wahttp_name,100,"%s",THIS_SERVER);
	wahttp_build = s100buildnumber();
	sp77sprintf(wahttp_version,100,"%s.%s.%s",MAJOR_VERSION_NO_STR_SP100,
											  MINOR_VERSION_NO_STR_SP100,
											  CORRECTION_LEVEL_STR_SP100);
}
