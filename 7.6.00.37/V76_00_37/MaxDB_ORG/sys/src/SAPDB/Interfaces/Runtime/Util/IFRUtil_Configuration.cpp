/*!
  @file           IFRUtil_Configuration.cpp
  @author         D039759
  @ingroup        IFR_Common
  @brief          Configuration settings.
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

#include "Interfaces/Runtime/Util/IFRUtil_Configuration.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"

#include <stdio.h>

#ifndef WIN32
#include <unistd.h>
#define GETCWD getcwd
#else
#include <direct.h>
#define GETCWD _getcwd
#endif


#define SQLDBC_CONFIGFILENAME  SAPDB_RUNTIMES_INI_FILE
#define SQLDBC_CONFIGSECTION   "SQLDBC"

#define SQLDBC_TRACEFILENAME_KEY     "TraceFileName"
#define SQLDBC_PROFILEFILENAME_KEY   "ProfileFileName"
#define SQLDBC_TRACEFLAGS_KEY        "TraceFlags"
#define SQLDBC_TRACESHAREDMEM_KEY    "TraceSharedMemory"

#define SQLDBC_DFEFAULT_TRACEFILENAME "sqldbctrace-%p.prt"
#define SQLDBC_DFEFAULT_PROFILEFILENAME  "sqldbcprofile-%p.prt"
//----------------------------------------------------------------------
static
IFR_Retcode innerGetFileName(const char *user, char *filename, IFR_size_t length, const char* regKey)
{
    tsp00_ErrTextc      ignoredErrorText;
    RTE_IniFileResult   configResult;


    RTE_GetUserConfigString(user,
                            SQLDBC_CONFIGFILENAME,
                            SQLDBC_CONFIGSECTION,
                            regKey,
                            filename,
                            (SAPDB_Int4)length,
                            ignoredErrorText,
                            configResult);

    switch(configResult) {
    case SAPDB_INIFILE_RESULT_OK:
        return IFR_OK;
    case SAPDB_INIFILE_RESULT_TRUNCATED:
        return IFR_DATA_TRUNC;
    }

    // nasty fallback - look in the global configuration
    RTE_GetGlobalConfigString(SQLDBC_CONFIGFILENAME,
                              SQLDBC_CONFIGSECTION,
                              regKey,
                              filename,
                              (SAPDB_Int4)length,
                              ignoredErrorText,
                              configResult);
    
    



    return IFR_NOT_OK;
}

//----------------------------------------------------------------------
static
IFR_Retcode
internalgetFileName(const char *user, char *filename, IFR_size_t length, 
                                        char *errtext, IFR_size_t errtextlength, 
                                        const char* regKey, const char* defaultFileName)
{

if(length) *filename='\0';
    
    tsp00_ErrTextc      ignoredErrorText;
    RTE_IniFileResult   configResult;

    if(innerGetFileName(user, filename, length, regKey) == IFR_OK) {
        return IFR_OK;
    }

    RTE_PutUserConfigString(user,
                            SQLDBC_CONFIGFILENAME,
                            SQLDBC_CONFIGSECTION,
                            regKey,
                            defaultFileName,
                            ignoredErrorText,
                            configResult);

    if(configResult == SAPDB_INIFILE_RESULT_OK) {
        return innerGetFileName(user, filename, length, regKey);
    } else {
        strncpy(errtext, ignoredErrorText, errtextlength);
        errtext[errtextlength-1]='\0';
        return IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::getTraceFileName(const char *user, char *filename, IFR_size_t length, 
                                        char *errtext, IFR_size_t errtextlength)
{
    return internalgetFileName(user, filename, length, 
                               errtext, errtextlength, 
                               SQLDBC_TRACEFILENAME_KEY, SQLDBC_DFEFAULT_TRACEFILENAME);
}
//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::getProfileFileName(const char *user, char *filename, IFR_size_t length, 
                                        char *errtext, IFR_size_t errtextlength)
{
    return internalgetFileName(user, filename, length, 
                               errtext, errtextlength, 
                               SQLDBC_PROFILEFILENAME_KEY, SQLDBC_DFEFAULT_PROFILEFILENAME);
}
//----------------------------------------------------------------------

static
IFR_Retcode
internalsetTraceFileName(const char *user, char *filename, char *errortext,
                                        IFR_size_t errortextlength, const char* regKey)
{
    if(filename==0 || strlen(filename) >= 1024) {
        return IFR_NOT_OK;
    }

    tsp00_ErrTextc      ignoredErrorText;
    RTE_IniFileResult   configResult;

    RTE_PutUserConfigString(user,
                            SQLDBC_CONFIGFILENAME,
                            SQLDBC_CONFIGSECTION,
                            regKey,
                            filename,
                            ignoredErrorText,
                            configResult);

    if(configResult == SAPDB_INIFILE_RESULT_OK) {
        return IFR_OK;
    } else {
        strncpy(errortext, ignoredErrorText, errortextlength);
        errortext[errortextlength-1]='\0';
        return IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::setTraceFileName(const char *user, char *filename, char *errortext,
                                        IFR_size_t errortextlength)
{
   return internalsetTraceFileName(user, filename, errortext,
                                   errortextlength, SQLDBC_TRACEFILENAME_KEY);
}
//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::setProfileFileName(const char *user, char *filename, char *errortext,
                                        IFR_size_t errortextlength)
{
   return internalsetTraceFileName(user, filename, errortext,
     errortextlength, SQLDBC_PROFILEFILENAME_KEY);
}
//----------------------------------------------------------------------
static IFR_Bool
fileNameIsAbsolute(char *filename)
{
#ifdef WIN32
    if(*filename && isalpha(*filename) && filename[1]==':') {
        return true;
    }
    if(filename[0]=='\\' && filename[1]=='\\') {
        return true;
    }
    return false;
#else
    if(filename[0]=='/') {
        return true;
    }
    return false;
#endif
}

//----------------------------------------------------------------------
static IFR_Bool
fileNameIsRelative(char *filename)
{
    if(*filename && *filename=='.') {
        return filename[1]=='/' || filename[1]=='\\';
    } else {
        return false;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::getFullFileName(const char *user, 
                                       char *tmpfilename, 
                                       char *filename, 
                                       IFR_size_t length,
                                       char *errortext,
                                       IFR_size_t errortextlength)
{
    if(fileNameIsAbsolute(tmpfilename)) {
        strcpy(filename, tmpfilename);
        return IFR_OK;
    } else if(fileNameIsRelative(tmpfilename)) {
        char fileLocation[512];
        GETCWD(fileLocation, 512);
        if(strlen(fileLocation) + strlen(tmpfilename) + 1 > length) {
            return IFR_NOT_OK;
        }
        strcpy(filename, fileLocation);
        char endc = filename[strlen(filename) - 1];
        if(endc == '/' 
#ifdef WIN32
           || endc == '\\'
#endif
            ) {
            strcat(filename, tmpfilename + 2);
        } else {
#ifdef WIN32
            strcat(filename, "\\");
#else
            strcat(filename, "/");
#endif
            strcat(filename, tmpfilename + 2);
        }
        return IFR_OK;
    } else {
        RTE_Path            fileLocation;
        tsp00_ErrTextc      errtext;

        if(RTE_GetUserSpecificConfigPath(fileLocation, true, errtext)) {
            if(strlen(fileLocation) + strlen(tmpfilename) + 1 > length) {
                return IFR_NOT_OK;
            }
            strcpy(filename, fileLocation);
            strcat(filename, tmpfilename);
            return IFR_OK;
        } else {
            strncpy(errortext, errtext, errortextlength);
            errortext[errortextlength-1]='\0';
            return IFR_NOT_OK;
        }
    }
    
    }
//----------------------------------------------------------------------
static
IFR_Retcode
internalgetFullFileName(const char *user, char *filename, IFR_size_t length,
                                            char *errortext, IFR_size_t errortextlength,
                                            const char* regKey, const char* defaultFileName)
{
    char *tmpfilename = (char *)alloca(length);
    IFR_Retcode rc=internalgetFileName(user, tmpfilename, length, errortext, errortextlength, regKey, defaultFileName);
    if(rc != IFR_OK) {
        return IFR_NOT_OK;
    }
    if(fileNameIsAbsolute(tmpfilename)) {
        strcpy(filename, tmpfilename);
        return IFR_OK;
    } else if(fileNameIsRelative(tmpfilename)) {
        char fileLocation[512];
        GETCWD(fileLocation, 512);
        if(strlen(fileLocation) + strlen(tmpfilename) + 1 > length) {
            return IFR_NOT_OK;
        }
        strcpy(filename, fileLocation);
        char endc = filename[strlen(filename) - 1];
#ifdef WIN32
        if(endc == '/' || endc == '\\') {
#else
        if(endc == '/') {
#endif
            strcat(filename, tmpfilename + 2);
        } else {
#ifdef WIN32
            strcat(filename, "\\");
#else
            strcat(filename, "/");
#endif
            strcat(filename, tmpfilename + 2);
        }
        return IFR_OK;
    } else {
        RTE_Path            fileLocation;
        tsp00_ErrTextc      errtext;

        if(RTE_GetUserSpecificConfigPath(fileLocation, true, errtext)) {
            if(strlen(fileLocation) + strlen(tmpfilename) + 1 > length) {
                return IFR_NOT_OK;
            }
            strcpy(filename, fileLocation);
            strcat(filename, tmpfilename);
            return IFR_OK;
        } else {
            strncpy(errortext, errtext, errortextlength);
            errortext[errortextlength-1]='\0';
            return IFR_NOT_OK;
        }
    }
}
//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::getFullTraceFileName(const char *user, char *filename, IFR_size_t length,
                                            char *errortext, IFR_size_t errortextlength)
{
  return internalgetFullFileName(user, filename, length, errortext, errortextlength,
                                 SQLDBC_TRACEFILENAME_KEY, SQLDBC_DFEFAULT_TRACEFILENAME);
}
//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::getFullProfileFileName(const char *user, char *filename, IFR_size_t length,
                                            char *errortext, IFR_size_t errortextlength)
{
  return internalgetFullFileName(user, filename, length, errortext, errortextlength,
    SQLDBC_PROFILEFILENAME_KEY, SQLDBC_DFEFAULT_PROFILEFILENAME);
}

//----------------------------------------------------------------------
static IFR_Retcode internalGetSharedMemoryName(const char *user, char *filename,
                                               IFR_size_t length,
                                               char *errortext,
                                               IFR_size_t errortextlength)
{
    tsp00_ErrTextc      ignoredErrorText;
    RTE_IniFileResult   configResult;

    RTE_GetUserConfigString(user,
                            SQLDBC_CONFIGFILENAME,
                            SQLDBC_CONFIGSECTION,
                            SQLDBC_TRACESHAREDMEM_KEY,
                            filename,
                            (SAPDB_Int4)length,
                            ignoredErrorText,
                            configResult);

    switch(configResult) {
    case SAPDB_INIFILE_RESULT_OK:
        return IFR_OK;
    case SAPDB_INIFILE_RESULT_TRUNCATED:
        return IFR_DATA_TRUNC;
    }
    
    strncpy(errortext, ignoredErrorText, errortextlength);
    errortext[errortextlength-1]='\0';
    
    return IFR_NOT_OK;
}



//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::getSharedMemoryName(const char *user, char *filename, IFR_size_t length, 
                                           char *errortext, IFR_size_t errortextlength)
{
    if(length) *filename='\0';

    tsp00_ErrTextc      ignoredErrorText;
    RTE_IniFileResult   configResult;

    if(internalGetSharedMemoryName(user, filename, length, errortext, errortextlength) == IFR_OK) {
        return IFR_OK;
    }

    RTE_PutUserConfigString(user,
                            SQLDBC_CONFIGFILENAME,
                            SQLDBC_CONFIGSECTION,
                            SQLDBC_TRACESHAREDMEM_KEY,
                            "sqldbctrace.shm",
                            ignoredErrorText,
                            configResult);

    if(configResult == SAPDB_INIFILE_RESULT_OK) {
        return internalGetSharedMemoryName(user, filename, length, errortext, errortextlength);
    } else {
        strncpy(errortext, ignoredErrorText, errortextlength);
        errortext[errortextlength-1]='\0';
        return IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
    IFRUtil_Configuration::getFullSharedMemoryName(const char *user, char *sharedmemoryname, IFR_size_t length,
                                                   char *errortext, IFR_size_t errortextlength)
{
    char *tmpfilename = (char *)alloca(length);
    IFR_Retcode rc=getSharedMemoryName(user, tmpfilename, length, errortext, errortextlength);
    if(rc != IFR_OK) {
        return IFR_NOT_OK;
    }
    if(fileNameIsAbsolute(tmpfilename)) {
        strcpy(sharedmemoryname, tmpfilename);
        return IFR_OK;
    } else {
        RTE_Path           fileLocation;
        tsp00_ErrTextc     errtext;

        if(RTE_GetUserSpecificConfigPath(fileLocation, true, errtext)) {
            if(strlen(fileLocation) + strlen(tmpfilename) + 1 > length) {
                return IFR_NOT_OK;
            }
            strcpy(sharedmemoryname, fileLocation);
            strcat(sharedmemoryname, tmpfilename);
            return IFR_OK;
        } else {
            strncpy(errortext, errtext, errortextlength);
            errortext[errortextlength-1]='\0';
            return IFR_NOT_OK;
        }
    }
}


//----------------------------------------------------------------------
static IFR_Retcode internalGetTraceFlags(const char *user,
                                         char *flags,
                                         IFR_size_t length,
                                         char *errortext,
                                         IFR_size_t errortextlength)
{
    tsp00_ErrTextc      ignoredErrorText;
    RTE_IniFileResult   configResult;

    RTE_GetUserConfigString(user,
                            SQLDBC_CONFIGFILENAME,
                            SQLDBC_CONFIGSECTION,
                            SQLDBC_TRACEFLAGS_KEY,
                            flags,
                            (SAPDB_Int4)length,
                            ignoredErrorText,
                            configResult);

    switch(configResult) {
    case SAPDB_INIFILE_RESULT_OK:
        return IFR_OK;
    case SAPDB_INIFILE_RESULT_TRUNCATED:
        return IFR_DATA_TRUNC;
    }
    strncpy(errortext, ignoredErrorText, errortextlength);
    errortext[errortextlength-1]='\0';
    return IFR_NOT_OK;
}


//----------------------------------------------------------------------
IFR_Retcode
    IFRUtil_Configuration::getTraceFlags(const char *user, char *flags, IFR_size_t length,
                                         char *errortext, IFR_size_t errortextlength)
{
    if(length) *flags='\0';

    tsp00_ErrTextc      ignoredErrorText;
    RTE_IniFileResult   configResult;

    if(internalGetTraceFlags(user, flags, length, errortext, errortextlength) == IFR_OK) {
        return IFR_OK;
    }

    RTE_PutUserConfigString(user,
                            SQLDBC_CONFIGFILENAME,
                            SQLDBC_CONFIGSECTION,
                            SQLDBC_TRACEFLAGS_KEY,
                            "",
                            ignoredErrorText,
                            configResult);

    if(configResult == SAPDB_INIFILE_RESULT_OK) {
        return internalGetTraceFlags(user, flags, length, errortext, errortextlength);
    } else {
        strncpy(errortext,ignoredErrorText, errortextlength);
        errortext[errortextlength - 1]='\0';
        return IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_Configuration::setTraceFlags(const char *user, char *flags, 
                                         char *errortext, 
                                         IFR_size_t errortextlength)
{
    tsp00_ErrTextc      ignoredErrorText;
    RTE_IniFileResult   configResult;

    RTE_PutUserConfigString(user,
                            SQLDBC_CONFIGFILENAME,
                            SQLDBC_CONFIGSECTION,
                            SQLDBC_TRACEFLAGS_KEY,
                            flags,
                            ignoredErrorText,
                            configResult);
    
    if(configResult == SAPDB_INIFILE_RESULT_OK) {
        return IFR_OK;
    } else {
        strncpy(errortext, ignoredErrorText, errortextlength);
        errortext[errortextlength - 1]='\0';
        return IFR_NOT_OK;
    }
}

