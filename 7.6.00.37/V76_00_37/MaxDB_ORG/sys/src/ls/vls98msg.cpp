/*
  -----------------------------------------------------------------------------
  module: vls98msg.cpp
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Server Protokolldateiklasse

  version:      7.6.
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

//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

*/

#ifndef VLS98MSG_CPP
#define VLS98MSG_CPP

#include "gls00.h"
#include "hls98msg.h"
#include "hls01.h"
#include "heo01.h"          // Working path
#include "heo06.h"    
#include "gsp09.h"
#include "heo95.h"
#include "RunTime/RTE_IniFileHandling.h"
#define   REGISTRY true

#ifdef FREEBSD
#include "geo47.h"
#endif

LoaderMessages*         LoaderMessages::_msgsinstance               = NULL;
_TCHAR*                 LoaderMessages::m_LogFileName_c             = NULL;
tsp00_Int4              LoaderMessages::m_LogFile                   = 0;


Loader_RTStatistics*    Loader_RTStatistics::pStatisticsInstance    = NULL;


/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::LoaderMessages()
 -----------------------------------------------------------------------------
*/
LoaderMessages::LoaderMessages()
{
    ls98_Init();
}
/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::ls98_Init()
 -----------------------------------------------------------------------------
*/
void LoaderMessages::ls98_Init()
{
    m_szMessageForFile[0] = '\0';
    m_szMessageForStr[0]  = '\0';
    m_pszMsg              = NULL;
    pszLayer_ls98         = NULL;
}
/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::Instance()
 -----------------------------------------------------------------------------
*/
LoaderMessages* LoaderMessages::Instance()
{
    if  (_msgsinstance == NULL)
    {
         _msgsinstance = new LoaderMessages;
          // Init Protocol Path
         _msgsinstance->ls98InitLoaderPath();
    }
    return _msgsinstance;
};


/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::ls98InitLogFile() 
 -----------------------------------------------------------------------------
*/
int
LoaderMessages::ls98InitLogFile(const char* pszDBName) 
{
    tsp00_Int4          rc = 0;               
    tsp05_RteFileError  RteError;
    RteError.sp5fe_result.becomes(vf_ok);

    if (0 != m_LogFile)
    {
        sqlfclosec(m_LogFile, sp5vf_close_normal, &RteError);
    }

    // Init Log File
    tsp00_VFilenamec   LogFileName_c;

    if ( NULL == getenv ("maxdbloaderpackageprotocol") ) {  
         LogFileName_c.rawAssign("loader.log");
    }
    else {
         LogFileName_c.rawAssign(getenv ("maxdbloaderpackageprotocol"));
    }
    _msgsinstance->ls98SetLogFileName(LogFileName_c);

    // Init Date and Time
    tsp00_Date  Date;
    tsp00_Time  Time;
    sqldattime(Date, Time);
    tsp00_Datec szDate;
    tsp00_Timec szTime;
    szDate.p2c(Date);
    szTime.p2c(Time);

    tsp00_C40c       tmpText;
    char             szOldName[256];
    tsp00_VFilenamec szTempFileName;

    // Because all entries are written as UTF-8 strings we must make sure the file itself is UTF-8 encoded
    ls98_MakeLogFileUTF8();

    const tsp77encoding* pEncoding = sp77encodingUTF8;

    sqlfopenEncodedc(m_LogFileName_c, &pEncoding, pEncoding, sp5vf_append, sp5bk_unbuffered, &m_LogFile, &RteError);
    if (RteError.sp5fe_result != vf_ok && NULL == getenv ("maxdbloaderpackageprotocol"))
    {
        // File could not be opened. Without a log file not operation possible. So open a temporary file.
        m_LogFile = 0;
        // Save error text and file name of original file for an error message
        szOldName[0] = '\0';
        tmpText.p2c(RteError.sp5fe_text);
        ls98GetLogFileName(szOldName);

        // Open temporary file
        szTempFileName.Init();
        sp77sprintf(szTempFileName.asCharp(), szTempFileName.size(), "loader_%s%s.log", szDate.asCharp(), szTime.asCharp());
        ls98SetLogFileName(szTempFileName);
        sqlfopenEncodedc(m_LogFileName_c,
                         &pEncoding,
                         pEncoding,
                         sp5vf_append,
                         sp5bk_unbuffered,
                         &m_LogFile,
                         &RteError);
        rc = -1;    // = temp file opened!!
    }
    //*
    //*     Log version
    //*
    tsp00_Versionc VersionString;
    sp100_GetVersionString( COMP_NAME_REPL_MAN_SP100, s100buildnumber, VersionString );
    ls98PrintMessage(layVFile_els98, msgVersionString_els98, VersionString.asCharp());
#ifdef WIN32
     SAPDB_Char LoaderPath[PATH_MXSP00];
     LoaderPath[0] = '\0';
     int  MaxPathLen = PATH_MXSP00;
     if (GetModuleFileName (NULL, LoaderPath, MaxPathLen) != 0)
         ls98PrintMessage(layVFile_els98, msgModuleFileName_els98, LoaderPath );
#endif
    tsp00_Int4 pos = 0;
    char aText[MAX_REPLY_LENGTH_LS00];
    aText[0] = 0;
    size_t nlen = 0;
    size_t dlen = 0;
    if (NULL != pszDBName)
    {
        dlen = _tcslen(pszDBName);
    }
    //*
    //*     Log server node and server db and start time
    //*
    tsp00_NodeIdc Nodec;
    tsp00_NodeId  Node;
    tsp00_ErrText ErrorText;
    tsp01_CommErr sqlrc;
    sqlhostname(Node, ErrorText, sqlrc);
    if (0 == sqlrc)
    {
        Nodec.p2c(Node);
        nlen = Node.length();
    }
    pos = sp77sprintf(aText, MAX_REPLY_LENGTH_LS00, "at %.*s", nlen, Nodec.asCharp());
    if (0 != dlen)
        sp77sprintf(aText + pos, (MAX_REPLY_LENGTH_LS00 - pos), " for database %.*s", dlen, pszDBName);

    ls98PrintMessage(layVFile_els98, msgInstanceInformation_els98, COMP_NAME_REPL_MAN_SP100, aText);
    ls98PrintMessage(layVFile_els98, msgStartServer_els98, szDate.asCharp(), szTime.asCharp());

    if (-1 == rc)
        ls98PrintMessage(layVFile_els98, errLogFileOpen_els98, szOldName, tmpText.asCharp(), szTempFileName.asCharp());

    return rc;
}
//ls98InitLogFile()


/*
 -----------------------------------------------------------------------------
    function:   ls98_MakeLogFileUTF8()
 -----------------------------------------------------------------------------
*/
void
LoaderMessages::ls98_MakeLogFileUTF8()
{
    // We try to go through it without errors ;-)

    SAPDB_Int4 rc = errOK_els00;

    tsp05_RteFileError  RteError;
    tsp05_RteFileInfo   RteInfo;
    SAPDB_Bool          bCopy = SAPDB_FALSE;

    // Get info on the file loader.log
    // Return ok if
    //  - the file is not writable nor readable. This means some other
    //    Loader has a lock on the file. In this case we open a temporary file anyway
    //    which can always be opened as UTF-8 file.
    //  - the file does not exist.
    sqlfinfoc(m_LogFileName_c, &RteInfo, &RteError);
    if (vf_ok == RteError.sp5fe_result)
    {
        if ( (0 == RteInfo.sp5fi_exists) ||
             ( (1 == RteInfo.sp5fi_exists) &&
               ( (0 == RteInfo.sp5fi_writeable) || (0 == RteInfo.sp5fi_readable) ) ) )
        {
            return;
        }
    }

    //*
    //* Nothing else requested the file --> copy it to a file named loader.ascii.log and
    //* then delete the old loader.log
    //*
    const tsp77encoding* _pEncoding = NULL;

    // Check the encoding
    sqlfopenEncodedc(m_LogFileName_c,
                     &_pEncoding,
                     _pEncoding,
                     sp5vf_read,
                     sp5bk_unbuffered,
                     &m_LogFile,
                     &RteError);
    if (RteError.sp5fe_result == vf_ok)
    {
        if (csp_ascii == _pEncoding->EncodingType)
        {
            bCopy = SAPDB_TRUE;
        }

        // close the file
        sqlfclosec(m_LogFile, sp5vf_close_normal, &RteError);
    }

    // Rename loader.log to loader.ascii.log and delete loader.log
    if (SAPDB_TRUE == bCopy)
    {
        tsp01_RteError RTECommErr;

        size_t _filelen = _tcslen(m_LogFileName_c) + 16*sizeof(char);
        char* _pszAsciiFileName = new char[_filelen];
        _pszAsciiFileName[0] = 0;
        sp77sprintf(_pszAsciiFileName, _filelen, "%s%sloader.ascii.log", m_LoaderLogPath_c, FILENAME_SEPARATOR_LS00);

        sqlfilecopyc(m_LogFileName_c, _pszAsciiFileName, &RTECommErr);
        if (RTE_NO_ERROR_SP01 == RTECommErr.RteErrCode)
        {
            sqlferasec(m_LogFileName_c, &RteError);
        }
    }   // end if (SAPDB_TRUE == bCopy)

    return;
}
// ls98_MakeLogFileUTF8()


/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::~LoaderMessages()
 -----------------------------------------------------------------------------
*/
LoaderMessages::~LoaderMessages()
{
    // This is the end of the loader call at all; we're sure the OS will do the cleanup
    // but must assure that the file is regularly closed to get the unwritten parts flushed.
    if (0 != m_LogFile)
    {
        tsp05_RteFileError  RteError;
        sqlfclosec(m_LogFile, sp5vf_close_normal, &RteError);
        if (vf_ok != RteError.sp5fe_result)
        {
            m_ErrorText.p2c(RteError.sp5fe_text);
        }
        m_LogFile = 0;
    }
}

/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::ls98InitLoaderPath
 -----------------------------------------------------------------------------
*/
void LoaderMessages::ls98InitLoaderPath()
{
    tsp01_RteError Rte1Error;
    SAPDB_Char     LoaderDrive[PATH_MXSP00];
    SAPDB_Char     LoaderPath [PATH_MXSP00];
    m_LoaderDataPath_c[0] = '\0';
    m_LoaderApplPath_c[0] = '\0';
    m_ConfigPath_c[0]     = '\0';
    m_LoaderLogPath_c[0]  = '\0';
    LoaderDrive[0]        = '\0';
    LoaderPath[0]         = '\0';
    //Server
    if ( NULL != getenv ("maxdbloaderserver") ) {   
         tsp00_Pathc LoaderPath;
         sqlGetIndependentWrkPath (LoaderPath, TERM_WITH_DELIMITER_EO01,&Rte1Error); 
         strcpy(m_LoaderLogPath_c, LoaderPath.asCharp());
    }
    //Client transportmode
    else if ( NULL != getenv ("maxdbloaderpackagepath") ) {   
         RTE_Path         HomePath;
         RTE_Path         ApplPath;
         tsp00_ErrTextc   ErrText; 
         // home directory
         RTE_GetMyDocumentsPath(HomePath,TERM_WITH_DELIMITER_EO01,ErrText);
         strcpy(m_LoaderDataPath_c, HomePath );
         strcat(m_LoaderDataPath_c, "loader"FILENAME_SEPARATOR_LS00);
         //log
         strcpy(m_LoaderLogPath_c, getenv ("maxdbloaderpackagepath") );
         strcat(m_LoaderLogPath_c, FILENAME_SEPARATOR_LS00);
         sqlcreate_dirc(m_LoaderLogPath_c, &Rte1Error);
         //packages
         strcpy(m_TransportPath_c, m_LoaderDataPath_c);
         strcat(m_TransportPath_c, "packages");
         sqlcreate_dirc(m_TransportPath_c, &Rte1Error);
         strcat(m_TransportPath_c, FILENAME_SEPARATOR_LS00);
         // application directory
         RTE_GetUserSpecificApplicationDataPath(ApplPath,true,true,ErrText);
         strcpy(m_LoaderApplPath_c, ApplPath );
         strcat(m_LoaderApplPath_c, "loader"FILENAME_SEPARATOR_LS00);
         //log
         strcpy(m_TransportLogPath_c, m_LoaderApplPath_c);
         strcat(m_TransportLogPath_c, "log");
         sqlcreate_dirc(m_TransportLogPath_c, &Rte1Error);
         strcat(m_TransportLogPath_c, FILENAME_SEPARATOR_LS00);
         //config
         strcpy(m_ConfigPath_c, m_LoaderApplPath_c);
         strcat(m_ConfigPath_c, "config");
         sqlcreate_dirc(m_ConfigPath_c, &Rte1Error);
         strcat(m_ConfigPath_c, FILENAME_SEPARATOR_LS00);
    }
    //Client batch and interactive mode
    else
    {
         RTE_Path         HomePath;
         RTE_Path         ApplPath;
         tsp00_ErrTextc   ErrText; 
         // home directory
         RTE_GetMyDocumentsPath(HomePath,TERM_WITH_DELIMITER_EO01,ErrText);
         strcpy(m_LoaderDataPath_c, HomePath );
         strcat(m_LoaderDataPath_c, "loader"FILENAME_SEPARATOR_LS00);
         //log
         strcpy(m_LoaderLogPath_c, m_LoaderDataPath_c);
         strcat(m_LoaderLogPath_c, "log");
         sqlcreate_dirc(m_LoaderLogPath_c, &Rte1Error);
         strcat(m_LoaderLogPath_c, FILENAME_SEPARATOR_LS00);
         //packages
         strcpy(m_TransportPath_c, m_LoaderDataPath_c);
         strcat(m_TransportPath_c, "packages");
         sqlcreate_dirc(m_TransportPath_c, &Rte1Error);
         strcat(m_TransportPath_c, FILENAME_SEPARATOR_LS00);
         // application directory
         RTE_GetUserSpecificApplicationDataPath(ApplPath,true,true,ErrText);
         strcpy(m_LoaderApplPath_c, ApplPath );
         strcat(m_LoaderApplPath_c, "loader"FILENAME_SEPARATOR_LS00);
         //log
         strcpy(m_TransportLogPath_c, m_LoaderApplPath_c);
         strcat(m_TransportLogPath_c, "log");
         sqlcreate_dirc(m_TransportLogPath_c, &Rte1Error);
         strcat(m_TransportLogPath_c, FILENAME_SEPARATOR_LS00);
         //config
         strcpy(m_ConfigPath_c, m_LoaderApplPath_c);
         strcat(m_ConfigPath_c, "config");
         sqlcreate_dirc(m_ConfigPath_c, &Rte1Error);
         strcat(m_ConfigPath_c, FILENAME_SEPARATOR_LS00);
    }
}
/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::ls98GetLoaderPath
 -----------------------------------------------------------------------------
*/
void LoaderMessages::ls98GetLoaderPath(char *pszLoaderPath)
{
    size_t len = _tcslen (m_LoaderDataPath_c);
    SAPDB_memcpy(pszLoaderPath, m_LoaderDataPath_c, len);
    pszLoaderPath[len] = '\0';
}
/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::ls98GetLoaderLogPath
 -----------------------------------------------------------------------------
*/
void LoaderMessages::ls98GetLoaderLogPath(char *pszLoaderPath)
{
    size_t len = _tcslen (m_LoaderLogPath_c);
    SAPDB_memcpy(pszLoaderPath, m_LoaderLogPath_c, len);
    pszLoaderPath[len] = '\0';
}
/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::ls98GetLoaderLogPath
 -----------------------------------------------------------------------------
*/
void LoaderMessages::ls98GetLoaderConfigPath(char *pszLoaderPath)
{
    size_t len = _tcslen (m_ConfigPath_c);
    SAPDB_memcpy(pszLoaderPath, m_ConfigPath_c, len);
    pszLoaderPath[len] = '\0';
}
/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::ls98SetLogFileName
 -----------------------------------------------------------------------------
*/
void LoaderMessages::ls98SetLogFileName(tsp00_VFilenamec pszFileName)
{
    if (NULL != m_LogFileName_c)
    {
        delete [] m_LogFileName_c;
    }

    tsp00_Int4 lLength = pszFileName.length() + _tcslen(_msgsinstance->m_LoaderLogPath_c);
    m_LogFileName_c = new _TCHAR[lLength + 1];
    m_LogFileName_c[0] = '\0';

    sp77sprintf(m_LogFileName_c, lLength+1, "%s%s", _msgsinstance->m_LoaderLogPath_c, pszFileName.asCharp());
}
/*
 -----------------------------------------------------------------------------
    function:   LoaderMessages::ls98GetLogFileName
 -----------------------------------------------------------------------------
*/
void LoaderMessages::ls98GetLogFileName(char *pszLogFileName)
{
    size_t len = _tcslen(m_LogFileName_c);
    SAPDB_memcpy(pszLogFileName, m_LogFileName_c, len);
    pszLogFileName[len] = '\0';
}


void LoaderMessages::ls98GetErrorText(char *pszErrorText)
{
    SAPDB_memcpy(pszErrorText, m_ErrorText.asCharp(), m_ErrorText.length());
    pszErrorText[m_ErrorText.length()] = '\0';
}
// ls98GetErrorText()


int LoaderMessages::ls98ChangeLogFileOwner(tsp00_Addr pszUsername, tsp00_C256c & szErrorText)
{
    tsp00_Int4 rc = errOK_els00;

#ifndef _WIN32

    tsp09_rte_xerror xRteError;

    tsp05_RteFileError  RteError;
    tsp05_RteFileInfo   RteInfo;

    //*     Existiert die datei (sp5fi_exists ist dann auf true gesetzt) ?
    sqlfinfoc (m_LogFileName_c, &RteInfo, &RteError);
    if (vf_ok == RteError.sp5fe_result)
    {
        // Aendere den besitzer der datei
        if (RteInfo.sp5fi_exists)
        {
            tsp00_Pathc myFilename;
            myFilename.rawAssign(m_LogFileName_c);

            sqlxchown(pszUsername, myFilename, &xRteError);
            if (csp9_xrte_notok == xRteError.xe_result)
            {
                szErrorText.rawAssign(xRteError.xe_text);
                rc = errChangeLogFileOwner_ls98;
            }
        }
    }
#endif

    return rc;
}
// ls98ChangeLogFileOwner()




/************  Globale Funktionen    ********************************************/

/*
-----------------------------------------------------------------------------

  function:     ls98PrintMessage

-----------------------------------------------------------------------------

  description:  Gibt einen fehlerstring passend zur gemeldeten fehlernummer
                nur ins logfile aus.
                Die layerinformation wird momentan nicht beachtet.

  arguments:    lLayer         [IN]    - enum parameter des typs LayerNumbers,
                                         der den layer, in dem eine meldung angelegt
                                         wurde, identifiziert.
                                         WIRD Z.Z. NICHT AUSGEWERTET.
                lMessageNumber [IN]    - enum parameter des typs ErrorNumbers
                                         fehler- bzw. meldungsnummer
                weitere param  [IN]    - variabel viele weitere parameter fuer die
                                         meldung

  returnvalue:  No
-----------------------------------------------------------------------------
*/
void LoaderMessages::ls98PrintMessage(const LayerNumbers lLayer, const ErrorNumbers lMessageNumber, ...)
{
    ls98_Init();

    va_list         arglist;
    va_start (arglist, lMessageNumber);

    ls98_FormatMsg(lMessageNumber, true, arglist);

    va_end (arglist);

    ls98_Write();

    return;
}
// ls98PrintMessage()


/*
-----------------------------------------------------------------------------

  function:     ls98MsgToString

-----------------------------------------------------------------------------

  description:  Gibt einen fehlerstring passend zur gemeldeten fehlernummer NUR
                in den angegebenen String aus.
                Die layerinformation wird momentan nicht beachtet.

  arguments:    lLayer         [IN]    - enum parameter des typs LayerNumbers,
                                         der den layer, in dem eine meldung angelegt
                                         wurde, identifiziert.
                                         WIRD Z.Z. NICHT AUSGEWERTET.
                pszDestString  [IN]    - zeiger auf den string, in den der msgtext
                                         kopiert werden soll.
                lMessageNumber [IN]    - enum parameter des typs ErrorNumbers
                                         fehler- bzw. meldungsnummer
                weitere param  [IN]    - variabel viele weitere parameter fuer die
                                         meldung

  returnvalue:  No
-----------------------------------------------------------------------------
*/
void LoaderMessages::ls98MsgToString(const LayerNumbers lLayer,
                                      char* pszDestString,
                                      const ErrorNumbers lMessageNumber,
                                      ...)
{
    ls98_Init();

    va_list     arglist;
    va_start (arglist, lMessageNumber);

    ls98_FormatMsg(lMessageNumber, false, arglist);

    va_end (arglist);

    pszDestString[0] = '\0';
    strcpy(pszDestString, m_szMessageForStr);
}
//  ls98MsgToString()


/*
-----------------------------------------------------------------------------
  function:     ls98AppendStatusToString
-----------------------------------------------------------------------------
*/
void LoaderMessages::ls98AppendStatusToString(const LayerNumbers  lLayer,
                                              char*               pszDestString,
                                              const ErrorNumbers  lMessageNumber,
                                              ...)
{
    // does pszDestSring already contain a message?
    size_t _len = strlen(pszDestString);

    va_list _arglist;
    va_start(_arglist, lMessageNumber);

    bool _bFirstMessage = true;
    if (_len > 0)
    {
        // Check if this is the first status message to append
        if (NULL != strstr(pszDestString, pszStatisticsMsgDelimiter))
        {
            _bFirstMessage = false;
        }
    }
    ls98_AppendMsgToString(lMessageNumber, _bFirstMessage, _arglist);

    va_end(_arglist);

    if (_len > 0)
    {
        sp77sprintf(pszDestString + _len, MAX_REPLY_LENGTH_LS00 - _len - sizeof(char), "\n%s", m_szMessageForStr);
    }
    else
    {
        sp77sprintf(pszDestString + _len, MAX_REPLY_LENGTH_LS00 - _len - sizeof(char), "%s", m_szMessageForStr);
    }
}
//  ls98AppendStatusToString()


/*
-----------------------------------------------------------------------------

  function:     ls98Msg

-----------------------------------------------------------------------------

  description:  Gibt einen fehlerstring passend zur gemeldeten fehlernummer
                ins logfile aus und in den angegebenen String.
                Es wird keine layerinformation mitgegeben.
                Der ausgabestring darf 1024 zeichen nicht ueberschreiten
                (abschliessende NULL eingeschlossen) !!!

  arguments:    lLayer         [IN]    - enum parameter des typs LayerNumbers,
                                         der den layer, in dem eine meldung angelegt
                                         wurde, identifiziert.
                                         WIRD Z.Z. NICHT AUSGEWERTET.
                pszDestString  [IN]    - zeiger auf den string, in den der msgtext
                                         kopiert werden soll.
                lMessageNumber [IN]    - enum parameter des typs ErrorNumbers
                                         fehler- bzw. meldungsnummer
                weitere param  [IN]    - variabel viele weitere parameter fuer die
                                         meldung

  returnvalue:  No
-----------------------------------------------------------------------------
*/
void LoaderMessages::ls98Msg(const LayerNumbers lLayer,
                              char* pszDestString,
                              const ErrorNumbers lMessageNumber, ...)
{
    ls98_Init();

    va_list arglist;
    va_start(arglist, lMessageNumber);

    ls98_FormatMsg(lMessageNumber, false, arglist);

    va_end (arglist);

    ls98_Write();                                       // logfile

    pszDestString[0] = '\0';
    strcpy(pszDestString, m_szMessageForStr);
    return;
}
//  ls98Msg()


/*!
 -----------------------------------------------------------------------------

  function:     ls98EFromRcToFile

 -----------------------------------------------------------------------------

  description:  Verwandelt den Fehlertext in einem rueckgabestring in einen
                Fehlertext fuer die protokolldatei.
                Notwendig, da manchmal nur die rueckgabestrings (insbesondere
                im parser) zurueckgegeben werden und der fehler noch ins logfile
                muss. Text in Rueckgabestring und in protokolldatei unterscheiden
                sich aber in der formatierung.

  attention:    Nur fuer Fehler!

  arguments:    lLayer       [IN]    - enum parameter des typs LayerNumbers,
                                        der den layer, in dem eine meldung angelegt
                                        wurde, identifiziert.
                                        WIRD Z.Z. NICHT AUSGEWERTET.
                pszSrcString [IN]    - zeiger; msgtext fuer rueckgabestring (enthaelt
                                        den zugehoeringen text in bereits formatierter form)
                lMsgNr       [IN]    - enum parameter des typs ErrorNumbers
                                        fehler- bzw. meldungsnummer

  returnvalue:  No
 -----------------------------------------------------------------------------
*/
void LoaderMessages::ls98EFromRcToFile(const LayerNumbers lLayer, char* pszSrcStr, const ErrorNumbers lMsgNr)
{
    ls98_Init();

    if ((ERROR_NUM_BEGIN < lMsgNr ) && ( lMsgNr < ERROR_NUM_END))
    {
         sp77sprintf(m_szMessageForFile, MAX_OI_SIZE_LS98, "%s%6hd:\t%s", pszErrorLineHeader, lMsgNr, &pszSrcStr[7]);
    }
    else if ( (WARNING_NUM_BEGIN < lMsgNr ) && ( lMsgNr < WARNING_NUM_END) )
    {
         sp77sprintf(m_szMessageForFile, MAX_OI_SIZE_LS98, "%s%6hd:\t%s", pszWarningLineHeader, lMsgNr,&pszSrcStr[7]);
    }

    ls98_Write();

    return;
}
//  ls98EFromRcToFile()

void
LoaderMessages::ls98_FormatMsg(const ErrorNumbers lMsgNr,
                                bool  bMsgToFileOnly,
                                va_list arglist)
{
// Die auszugebenden fehlernummern bewegen sich im bereich von -25999 bis -25001.
// Die auszugebenden meldungsnummern bewegen sich im bereich von 25900 bis 25999.
// Die auszugebenden warnungsnummern bewegen sich im bereich von 25600 bis 25699.

    ls98_GetMessageText(lMsgNr);        // hole den fehlertext

    int   pos1  = 0;
    int   pos2  = 0;
    short lmyNr = STATIC_CAST(short, lMsgNr);


    if ((ERROR_NUM_BEGIN < lmyNr) && (lmyNr < ERROR_NUM_END))
    {
        pos1 = sp77sprintf(m_szMessageForFile, MAX_OI_SIZE_LS98, "%s%6hd:\t", pszErrorLineHeader, lmyNr);
    }
    else if ( (INFO_NUM_BEGIN <  lmyNr) && (lmyNr < INFO_NUM_END) )
    {
        pos1 = sp77sprintf(m_szMessageForFile, MAX_OI_SIZE_LS98, "%s", pszInfoLineHeader);
    }
    else if ( INFO_NUM_BEGIN == lmyNr )
    {
        pos1 = sp77sprintf(m_szMessageForFile, MAX_OI_SIZE_LS98, "%s", pszRemarkLineHeader);    //!!LogMessage
    }
    else if ( (WARNING_NUM_BEGIN < lmyNr) && (lmyNr < WARNING_NUM_END) )
    {
        pos1 = sp77sprintf(m_szMessageForFile, MAX_OI_SIZE_LS98, "%s%6hd:\t", pszWarningLineHeader, lmyNr);
    }
    else if (0 == lmyNr)
    {
        m_szMessageForFile[0] = '\0';
    }

    sp77vsprintf(m_szMessageForFile + pos1, (MAX_OI_SIZE_LS98 - pos1), m_pszMsg, arglist);


    if (false == bMsgToFileOnly)
    {
//        if (0 != lmyNr)
        if (lmyNr < 0)      // return strings contain only numbers if errors happened
        {
            pos2 = sp77sprintf(m_szMessageForStr, MAX_REPLY_LENGTH_LS00, "%6hd\n", lmyNr);
        }

        size_t len = _tcslen(m_szMessageForFile) - pos1 + pos2;
        if (len >= MAX_REPLY_LENGTH_LS00)
        {
            strncpy(m_szMessageForStr + pos2, m_szMessageForFile + pos1,  MAX_REPLY_LENGTH_LS00 - pos2 - 1); // leave some space for trailing 0
            m_szMessageForStr[MAX_REPLY_LENGTH_LS00-1] = 0;
        }
        else
        {
            strcpy(m_szMessageForStr + pos2, m_szMessageForFile + pos1);
        }
    }
    return;
}
//ls98_FormatMsg()


void
LoaderMessages::ls98_AppendMsgToString(const ErrorNumbers lMsgNr,
                                       bool  bFirstMsgToAppend,
                                       va_list arglist)
{
// Die auszugebenden fehlernummern   bewegen sich im bereich von -25999 bis -25001.
// Die auszugebenden meldungsnummern bewegen sich im bereich von  25900 bis  25999.
// Die auszugebenden warnungsnummern bewegen sich im bereich von  25600 bis  25699.
    ls98_GetMessageText(lMsgNr);        

    int   pos1  = 0;
    short lmyNr = STATIC_CAST(short, lMsgNr);

    if (true == bFirstMsgToAppend)
    {
        pos1 = sp77sprintf(m_szMessageForStr, MAX_REPLY_LENGTH_LS00, "%s", pszStatisticsMsgDelimiter);
    }

    if (lmyNr < 0)      // return strings contain only numbers if errors happened
    {
        pos1 += sp77sprintf(m_szMessageForStr + pos1, MAX_REPLY_LENGTH_LS00 - pos1, "%6hd\n", lmyNr);
    }

    sp77vsprintf(m_szMessageForStr + pos1, (MAX_REPLY_LENGTH_LS00 - pos1), m_pszMsg, arglist);

    return;
}
// ls98_AppendMsg()


int LoaderMessages::ls98_Write()
{
    tsp00_Int4          rc = 0;
    tsp05_RteFileError  RteError;
    tsp00_Int4          lLength = 0;

    if (0 != m_LogFile)
    {
        sqlfwritec(m_LogFile, m_szMessageForFile, -1, &RteError);
        if (vf_ok != RteError.sp5fe_result)
        {
            m_ErrorText.p2c(RteError.sp5fe_text);
            rc = -1;
        }
    }
    return rc;
}


/********************************************************************************/
/* FUNCTION ls98_GetLayerText - lokale funktion                                 */
/*==============================================================================*/
/* PURPOSE                                                                      */
/*  Ordnet einer layer-enumeration den entsprechenden layertext zu.             */
/*  Der layertext wird in einer klasseninternen variablen gespeichert.          */
/*  Wird im moment nicht verwandt.                                              */
/*==============================================================================*/
/* PARAMETER        IN/OUT      DESCRIPTION                                     */
/*  lLayer          X           enum parameter des typs LayerNumbers            */
/*==============================================================================*/
/* RETURN CODE          DESCRIPTION                                             */
/********************************************************************************/
void LoaderMessages::ls98_GetLayerText(const LayerNumbers Layer)
{
    // Struktur zur Verknüpfung Layernummer - Layertext
    typedef struct
    {
        LayerNumbers    Type;
        char            pszText[5];
    } LayerStruct;


    static LayerStruct LayerDesc[] =
    {
        {layParser_els98,           "PARS"},
        {layScanner_els98,          "SCAN"},
        {laySQLConnect_els98,       "SQLC"},        // funktionalitaet in vls03.cpp fuer db-connect
        {laySQLInterface_els98,     "SQLI"},
        {layGRec_els98,             "GREC"},
        {layBasicConnect_els98,     "BCON"},        // funktionalitaet in vls15.cpp/hls15.cpp
        {layOIHandling_els98,       "OIHA"},        // auftragsschnittstellen-paket-handhabung (vls04.cpp/hls04.h)
        {layUserConnect_els98,      "UCON"},        // vls10.cpp - user connects
        {layDataLoad_els98,         "DLOA"},
        {layVFile_els98,            "VFIL"},
        {layFastload_els98,         "FLOA"},
        {layTableExtract_els98,     "TAEX"},
        {layDataExtract_els98,      "DEXT"},
        {layDispatch_els98,         "DISP"},
        {layCondition_els98,        "COND"},
        {layCursor_els98,           "CURS"},
        {layTableLoad_els98,        "TLOA"},
        {layDataUpdate_els98,       "DUPD"},

        {layEndOfArray_els98,       "UNKN"}         // DO NOT USE
    };

    short i = 0;
    while( (LayerDesc[i].Type != Layer) &&
           (LayerDesc[i].Type != layEndOfArray_els98) )
    {
        i++;
    }
    pszLayer_ls98 = LayerDesc[i].pszText;

    return;
}
//  ls98_GetLayerText()

void LoaderMessages::ls98_GetMessageText(const ErrorNumbers MessageNumber)
{
    // Struktur zur Verknüpfung Fehlernummer - Fehlertext
    typedef struct
    {
        ErrorNumbers    Type;
        char            pszText[1024];
    } ErrorStruct;


    static ErrorStruct ErrorDesc[] =
    {
        //errParseInternal_els98        internal error no specific msg printed
        //errParseNoScanner_els98       internal error no specific msg printed: no scanner assigned in parser
        //errScanNoAction_els98,        internal error no specific msg printed: No action found in scanner
        //errScanNoBuffer_els98,        internal error no specific msg printed: End of buffer missed (in scanner)
        //errScanInputFailed_els98,     internal error no specific msg printed: Input in scanner failed
        //errScanBufOverflow_els98,     internal error no specific msg printed: Input buffer overflow  (in scanner)
        //errUnknownDefault_els98,      internal error no specific msg printed: Unknown db default for special column

        // internal error while converting string/data to unicode or vice versa
        //errToUCS2SourceExhausted_ls98     internal error not specific msg printed
        //errToUCS2TargetExhausted_ls98     internal error not specific msg printed
        //errEncodingSourceExhausted_els98  internal error not specific msg printed
        //errEncodingTargetExhausted_els98  internal error not specific msg printed

        // der platzhalter sollte die funktion enthalten, in der der fehler auftrat. Ausserdem sollte
        {errInternalSQL_els98,                  "Error executing internal sql statement (see log file for statement). Please report to hotline."},
        // Das ist ein etwas allgemein gehaltener fehler, der erst bei der ausgabe naeher spezifieziert werden muss
        // Damit koennen einige fehler behandelt werden, die beim connect oder commit user aufgrund eines fehlers
        // beim fuellen des packets o.ä. auftreten.
        {errConvertPageSize_els98,              "Error[%d] converting page size %d to usable format. Please report to hotline."},
        {errUnknownPart_els98,                  "Error reading data from part. Unknown part kind. Please report to hotline."},
        {errWriteBuf2Part_els98,                "Error writing buffer to part. Not enough space in part. Please report to hotline."},
        {errInitSegment_els98,                  "Error initializing segment in packet. Please report to hotline."},
        {errInitPart_els98,                     "Error initializing part of packet. Please report to hotline."},
        {errFinishPart_els98,                   "Error finishing part. Please report to hotline."},
        {errInitPacket_els98,                   "Initialization of request packet failed (%i). Please report to hotline."},
        // folgender aufbau: [dateiname (in der fehler auftrat), zeile (in dieser datei), fehlernummer]
        // die Fehlernummer kann eine repserverfehlernummer oder auch eine kernfehlernummer sein
        {errInternal_els98,                     "Internal error [%s, %d, %d]; contact technical support"},

        {errTLWrongKernelVersion_els98,         "Error processing IMPORT in PAGE Format; "
                                                "Kernel versions of source table DB and target table DB differ "
                                                "(source table: %d.%d.%.2d, target table: %d.%d.%.2d)"},
        {errTLWrongLoaderVersion_els98,         "Error processing IMPORT in PAGE Format; "
                                                "Loader versions used for EXPORT and IMPORT differ "
                                                "(EXPORT: %d.%d.%.2d, IMPORT: %d.%d.%.2d)"},
        {errTLTableNotEmpty_els98,              "Error restoring table %s; table is not empty"},
        {errTLTableNotExisting_els98,           "Error restoring table %s; table does not exist - table must be created before import"},

        {errUnloadStatistics_els98,             "Error exporting statistics: %s"},
        {errLoadStatistics_els98,               "Error importing statistics: %s"},
    

        // File handling errors
        {errFileWrongEncoding_els98,            "File %s is not encoded as %s"},
        {errFileTruncate_els98,                 "Error truncating file %s: %s."},
        {errFileErase_els98,                    "Error erasing file: <operating system message>."},
        {errFileWrite_els98,                    "Error writing file: '%s': %s."},
        // 'Error reading file '<filename>': <error text>'
        {errFileRead_els98,                     "Error reading file '%s': %s."},
        // 'Error closing file '<filename>': <error text>'
        {errFileClose_els98,                    "Error closing file '%s': %s."},
        // 'Error opening file '<filename>': <error text>'
        {errFileOpen_els98,                     "Error opening file '%s': %s."},

        {errMaxErrorCountReached_els98,         "Errors processing command exceed maximum number of errors to ignore; processing stopped"},
        {errDLDataNotCompletelyLoaded_els98,    "Errors processing data for table %s; data not completely imported/updated"},
        {errDLNoDataLoaded_els98,               "Errors processing data for table %s; no data imported/updated"},

        {errFL_IntegrityViolation_els98,        "Integrity violation (SQL Error %d)%s"},

        //*
        //* Errors when using special ASCII code pages
        //*
        {errInvalidASCIICPValues_els98,         "Invalid ASCII values read or values not sorted ascending; the file might be corrupted"},
        {errInvalidHexInCPFile_els98,           "Invalid HEX constant '%.*s' found in code page file; the file might be corrupted"},
        {errOpen_ReadCPFile_els98,              "Error opening/reading code page file; can't use code page %s for conversion"},
        {errCreate_GrantCPTable_els98,          "Error creating/granting system table SYSDBA.CODEPAGE (%s); using codepage file"},
        {errReadCPTable_els98,                  "Error reading system table SYSDBA.CODEPAGE (%s)."},


        //*
        //*     Sqlstatement fehler
        //*
        // Error during check if table <table name> exists (<SQL error no>: <SQL error text>).
        {errSelectColumnNames_els98,            "Error getting column names from system table COLUMNS: %s"},
        {errParseAgainFailed_els98,             "Catalog of table %s changed while processing %s command; reparse failed (%s); to restart the command append \"%s\" to the infile specification"},
        {errCheckTableExistence_els98,          "Error during check if table %s exists (%s)"},
        {errODBCErrors_els98    ,               "'%s'"},
        {errSQLStmtTooLong_els98,               "SQL statement is too long."},

        {errDEAmbiguousTableName_els98,         "Error generating CREATE TABLE/INDEX statements; table name is ambiguous (different users have a table with this name)."},

        {errConvertingEncoding_els98,           "Error converting data from %s to %s; source contains invalid characters or character sequences"},

        //*
        //*     Fehler beim lesen von werten aus der datendatei
        //*
        {errCheckHeader_els98,                  "Data file corrupted: header  not found "},
        {errCheckTrailer_els98,                 "Data file corrupted: trailer not found "},
        {errTableToLoadNotCurrentTable_els98,   "Names of table to import (%s) and currently processing table in data file (%s) do not match"},
        {errCorruptMetaData_els98,              "Data file corrupted: record length does not match computed length."},
        {errCorruptDataFile_els98,              "Corrupt data file (read data: '%.*s')."},


        //*
        //*     Error processing LONG values
        //*
        {errProcessLongsWithGetVal_els98,       "Error processing LONG value(s): %s"},
        {errProcessLongsWithPutVal_els98,       "Error processing LONG value(s): %s"},
        // Error processing LONG value (column "<column name>"): <return code>
        {errProcessLongs_els98,                 "Error processing LONG value (column \"%s\"): %d."},
        {errLongfileSpecMissing_els98,          "LONGFILE specification missing or incorrect."},
        {errLongFileNumGTMax_els98,             "Spezified maximum number of LONG data files for column \"%s\" exceeded."},
        {errAnalyzeLongSum_els98,               "Errors analyzing long file information. See loader server logfile for more information."},
        {errNoLongFileInfo_els98,               "No long file info given for LONG column \"%s\"."},

        //*
        //*     Errors at load execution
        //*
        {errSchemaMapIdentity,                  "Schemamap id is the default schemamap "},
        {errSchemaMapDefinition,                "Schemamap %s maps source schemas to different target schemas: %s"},
        {errSchemaMapExists_els98,              "Schemamap %s already exists"},
        {errSchemaMapNotExists_els98,           "Schemamap %s does not exist"},
        {errSchemaMapPrivilegesNotExists_els98, "Schemaprivileges for schemas %s defining the schemamap %s do not exist"},
        {errSchemaMapSchemasNotExists_els98,    "Schemas %s defining the schemamap %s do not exist"},
        {errIncompatibleEncodings_els98,        "Conversion from UNICODE data to ASCII data is not supported; this conversion may lose information"},
        // Example: Error extracting SHORT LONG data of table REQUESTS
        {errTEExtractDataOfCertainTable_els98,  "Error exporting %s data of table %s. See loader server logfile for more information."},
        {errTLSrcAndTgtSwapDifferent_els98,     "Different byte swap of source and target machine; Loader must run on same processor memory architecture machine for EXPORT and IMPORT in PAGE Format."},
        {errTLSrcAndTgtEncodingDifferent_els98, "Encoding type of source and target database do not match: source = %s, target = %s."},
        {errTLPacketSizeTooSmall_els98,         "Communication packet size too small to run IMPORT for PAGES of %d kB; increase value of parameter _PACKET_SIZE and restart the database."},
        {errTLSchemeDifferences_els98,          "Definitions of source and target table %s do not match; %s."},
        {errWrongFieldLength_els98,             "Incorrect field length for column \"%s\", minimum length %d."},
        {errNotInSelList_els98,                 "Column name(s) or number(s) do not match column names/numbers in select list: (%s)."},
        {errTLTableNotRestored_els98,           "Error restoring table %s; table exists but source table schema and target table schema are different"},
        {err_DE_GenerateCreateCmd_els98,        "Error generating CREATE TABLE/INDEX statements: %s"},

        {errTLRestartFileOpen_els98,            "Error during restart IMPORT; file %s could not be opened/read"},
        {errTLRestartEntryNotFound_els98,       "Error during restart IMPORT; restart entry %s not found in mapping file %s"},
        {errTLTableDataNotFound_els98,          "Error restoring table %s; no table data found in data file %s"},
        {errTLSingleTabFileMultTab_els98,       "Error restoring multiple tables using data file with single table data; specify data file created using EXPORT DB or EXPORT USER."},
        {errTLMultTabFileSingleTab_els98,       "Error restoring single table %s using data file with multiple table data; specify single table data file."},
        {errTEFileCounterExceeded_els98,        "File counter for data file exceeded maximum value of %d."},
        // Error getting index definitions for table <name> (SQL-Error: <number> = <description>)
        {errGetIndexDefinition_els98,           "Error getting index definitions for table %s (%s)."},
        // Error getting table schema definition for table <name> (SQL-Error: <number> = <description>)
        {errGetTableDefinition_els98,           "Error getting schema definition of table %s (%s)."},
        {errRestrictedStmt_els98,               "This is a restricted statement, user must be SYSDBA."},
        {errWrongTEPageSize_els98,              "The page sizes of the source system and the target system do not match (source: %d, target: %d)."},
        {errWrongTEFileType_els98,              "The given data file '%s' was not generated using EXPORT in PAGE Format (%s)."},
        {errErrorAtDataExtract_els98,           "Some error(s) while extracting data. See loader server logfile for more information."},
        {errAtExtractedRec_els98,               "Error(s) exporting record %d; start position of incorrect record in datafile: %d."},
        {errNumberTruncAtExtract_els98,         "The exported number (column \"%s\") was truncated."},
        {errCompatibilityCheckSum_els98,        "Errors checking compatibility of external and db data types. See loader server logfile for more information."},
        {errLengthCheckSum_els98,               "Errors checking length of column specifications. See loader server logfile for more information."},
        {errIncompatibleNullValue_els98,        "Data type of null value representation for column \"%s\" is incompatible with the column data type."},
        {errTruncatedConditionValue_els98,      "This condition value was truncated and cannot be used: '%s'."},
        {errInvalidNumCondition_els98,          "This condition value is not a valid number: '%s'."},
        // Wird gesetzt, wenn eine spalte beim laden von daten zwar im kommando angegeben wurde
        // aber kein wert dafuer in der datendatei zu finden ist
        {errMissingData_els98,                  "Missing data for specified column \"%s\"."},
        {errSyskeyExceeded_els98,               "Syskey violation (the computed value exceeds the highest possible syskey value)."},
        // "Constant '<data>' must be compatible with column type and length (column: '<column name>')"
        {errIncompatibleConst_els98,            "Constant must be compatible with column type and length (column: '%s')."},
        {errLongColIllegal_els98,               "Table contains at least one column defined as LONG; IMPORT in PAGE format rejected "
                                                "(the only way to load LONG columns is a one-table IMPORT statement in ROW format)"},
        {errIncompatibleDataTypes_els98,        "Column data type of column \"%s\" and external data type are not compatible."},
        {errFractionTooSmall_els98,             "Fractional part too small for column \"%s\", minimum length: %d."},
        {errNumberOverflow_els98,               "Overflow occurred, execution aborted."},
        //  "No Data for NOT NULL - column without default (<column name>).
        //   You either have not specified this column in the command or
        //   there is no data in the data file for this column."
        {errNoDataForNNCol_els98,               "No Data for NOT NULL - column without default (%s)."
                                                "You either have not specified this column in the command or "
                                                "there is no data in the data file for this column."},
        //  "Column (<column name>) must not contain NULL values"
        {errNullNotAllowed_els98,               "Column %s must not contain NULL values."},
        /*"Error inserting values into data page:\nValue  '%.*s'  not recognized as boolean value."*/
        {errInvalidBool_els98,                  "Invalid input for BOOLEAN column: %.*s"},
        {errConstraintViolation_els98,          "CONSTRAINT condition violated."},
        {errInvalidDbyte_els98,                 "Invalid input for DBYTE column."},
        // Die hexkonstante ist als string eingelesen und wird auch so ausgegeben. Die mitgelieferten
        // parameter muessen in der reihenfolge - laenge des werte, wert - uebergeben werden.
        {errInvalidHex_els98,                   "The value is not a valid HEX constant: %.*s."},
        {errKeyLength_els98,                    "Data for key columns exceeds allowed key length."},
        {errNoDataForKeyCol_els98,              "Columns for key (KEY) or search condition missing."},
        {errKeyOrder_els98,                     "Input value does not match key order or range specification."},
        {errInvalidNumber_els98,                "This value is not a valid number: '%.*s'"},
        {errWrongDateTime_els98,                "Invalid input for date or time column: %.*s"},


        //*
        //*     Parser- oder Scannerfehler
        //*
        {errSQLSyntax_els98,                    "Syntax error at position %i (%s: %s); check for typing errors" },

        {errCodepageChangeOnlyBySysdba_els98,   "Code page to set (%s) not found in system table %s.CODEPAGE. The privilege to change the content of this table is restricted to SYSDBA."},
        {errBuildingEncryptionURIFailed_els98,  "Error building URI for encrypted connection: %s"},
        {errWrongIsolationLevel_els98,          "Invalid Isolaton Level specification: possible values are: 0, 1, 10, 15, 2, 20, 3, 30"},
        {errDatastreamFormatnotSupported,       "Error: specify DATA streamformat RECORDS or PAGES" },
        {errCatalogOrDatastreamNeeded ,         "Error: specify CATALOG or  DATA stream " },
        {errCatalogAndDatastreamNeeded,         "Error: specify CATALOG and DATA stream" },
        {errPackagestreamNeeded,                "Error: specify CATALOG, DATA and PACKAGE stream" },
        {errLoaderConstraintViolation_els98,    "Loader CONSTRAINT violation: %s"},
        {errErrorParsingDateTimeMask_els98,     "Error parsing %s mask: %s"},
        {errDBIdentifierTooLong_els98,          "Database identifier too long."},
        {errWrongFullQualTableNameSpec_els98,   "Given table name does not correspond to the specification of a fully qualified table name"},
        {errNoPasswordSupplied_els98,           "Incomplete logon parameters; password missing"},
        {errInvalidInteger_els98,               "Specified value '%s' is not a valid integer"},
        {errACCColsMissing_els98,               "Cannot execute UPDATE COLUMNS command; specify access columns"},
        {errSETColsMissing_els98,               "Cannot execute UPDATE COLUMNS command; specify set columns"},
        {errColOrderFormatted,                  "Some positions overlap."},
        {errColOrderCompressed,                 "Some positions specified more than once."},
        {errEndPosAndFormatted_els98,           "Some column descriptions without end position but file format is FWV."},
        {errEndPosAndCompress_els98,            "Some column descriptions with end position but file format is CSV."},
        {errWrongKeyword_els98,                 "Wrong keyword"},
        {errRTNotAllowed_els98,                 "ROUND or TRUNC specification not allowed for this column."},
        {errCtrlFileSpecMissing_els98,          "Cannot execute command DATAEXTRACT FOR %s; specify an outfile for commands"},
        {errCompareOperator_els98,              "Incorrect comparison operator in condition."},
        {errWrongDateTimeFormat_els98,          "Incorrect format, valid are all patterns accepted in the SET menu."},
        {errMaxConditionCount_els98,            "Specification of conditions exceeds allowed count of conditions."},
        {errMaxColCountExceeded_els98,          "A maximum of 1024 columns can be specified for one table."},
        {errWrongNumberLayout_els98,            "Wrong number specification."},
        {errWrongLangSpec_els98,                "Wrong language specification; specify either ENG or DEU"},
        {errWrongNullSpec_els98,                "Wrong null specification."},
        {errWrongBooleanSpec_els98,             "Wrong boolean specification."},
        {errWrongDecimalSpec_els98,             "Wrong decimal specification."},
        {errWrongDelSepSpec_els98,              "Wrong separator/delimiter specification."},
        {errPosNotAscending_els98,              "Columns have to be specified in left-to-right order (positions ascending)."},
        {errNoColAndFormatted_els98,            "No column specified but file format is FWV."},
        {errColAndCompact_els98,                "Column specified but file format is RECORDS."},
        {errEndPosLTStartPos_els98,             "End position for column %s lower than start position."},
        {errFormattedRequired_els98,            "File option FWV required."},
        {errWrongCodePageSpec_els98,            "Code page specification '%s' incorrect; specification must have following form: ISO-<4 digit number>-<number> or WINDOWS-<number>."},
        {errSepDelFormatted_els98,              "The specification of a separator/delimiter is not allowed together with FWV."},
        {errFormattedCompress_els98,            "Invalid file format specification: given formats rule each other out."},
        {errInvalidTrunc_els98,                 "Invalid TRUNC specification (0 <= TRUNC-value <= 18)."},
        {errInvalidRound_els98,                 "Invalid ROUND specification (0 <= ROUND-value <= 18)."},
        {errInvalidScale_els98,                 "Invalid SCALE specification. Scale range: -127 <= SCALE <= 127."},
        {errInvalidFraction_els98,              "Invalid FRACTION specification (negative values not allowed)."},
        {errInvalidPosition_els98,              "Invalid position specification (negative values not allowed)."},
        {errUsageNotApplicable_els98,           "USAGE value of %d row(s) too high; maximum number of rows possible on a single db page: %d"},
        {errWrongUsageRows_els98,               "USAGE value of '0 rows' not allowed."},
        {errWrongUsagePercent_els98,            "Declare USAGE between 50%%%% and 100%%%%"},
        {errWrongZonedLength_els98,             "Column \"%s\" - ZONED field: 20 bytes maximum (double if HEX)."},
        {errWrongDecimalLength_els98,           "Column \"%s\" - DECIMAL field: 20 bytes maximum (double if HEX)."},
        {errWrongRealLength_els98,              "Column \"%s\" - REAL field: 4 or 8 bytes long (double if HEX)."},
        {errWrongIntegerLength_els98,           "Column \"%s\" - INTEGER field: 1, 2 or 4 bytes long, (double if HEX)."},


        {errNotImplemented_els98,               "%s not implemented."},


        //*
        //*     Interne Fehler des servers
        //*
        {errCancelRequest_els98,                "Received cancel request; stopping execution"},
        {errCurrentUserCmd_els98,               "Error switching connected user to %s: %s"},
        {errCurrentSchemaCmd_els98,             "Error switching current schema to %s: %s"},
        {errNoMoreMemory_els98,                 "Out of memory"},
        {errLogFileOpen_els98,                  "Error opening server log file '%s' (%s). Opened temporary log file '%s'."},
        {errChangeLogFileOwner_ls98,            "Error changing log file owner to current user '%s' (Error: '%s')."},

        {errNoUserConnection_els98,             "No user connection to server db %s established."},
        {errInternSQL_els98,                    "Error executing internal SQL statement '%s': '%s'."},
        // This error message is used to simply pass the SQL error message delivered by the kernel on to
        // other functions and to print it.
        {errSimpleSQL_els98,                    "%s"},
        // Bsp: Failure in db communication (request): <fehlernummer> = <fehlertext>.
        // Fehlernummer und fehlertext werden von der LZU geliefert.
        {errDbComm_els98,                       "Failure in db communication (%s): %i = %.*s."},
        {errPreviousUserRestored_els98,         "Logon failure; previous user's (%s) connection was restored."},


        // Allgemeine fehler
        {errConnectNoServerDB_els98,            "No connection to server db '%s' established."},
        {errSqlConnectUserkey_els98,            "Connect with userkey '%s' failed: %s."},
        // Diese fehlermeldung sollte ausgegeben ungefaehr so aussehen:
        // Error while handling user connection (connect) for user STEFFEN (SQL-Error: 4000 = fehlertext)
        {errSQLConnectUser_els98,               "User (%s) logon to database %s failed: %s"},
        {errSqlConnectDb_els98,                 "A connection to the database could not be established: %i = %.*s."},
        {errDbHost_els98,                       "Getting hostname of database host failed: %i = %.*s."},
        {errDBConnectNoDBName_els98,            "No database connection possible - missing database name; specify the database name in the USE USER command"},

        {msgSimpleMsg_els98,                    "%s"NEW_LINE"// *"},

        //* STATUS MESSAGES IN RETURN STRINGS
        {statInsertedRejectedRows_els98,           "Total number of records imported: %qd%s\nTotal number of records rejected: %qd"},
        {statUpdatedInvalidRows_els98,             "Total number of records updated : %qd\nTotal number of records rejected: %qd"},
        {statNumExtractedRecords_els98,            "Total number of records exported            : %qd\nTotal number of records exported with errors: %qd"},
        {statNumExportedPages_els98,               "Total number of pages exported: %qd (primary data: %qd, long data: %qd)"},
        {statNumImportedPages_els98,               "Total number of pages imported: %qd (primary data: %qd, long data: %qd)"},

        // "Total number of table definitions exported|imported: 100"
        {statNumTablesTransformed_Catalog_els98,   "Total number of tables (definition) %s: %d"},
        // "Total number of tables (data) exported|imported: 100 (excluded: 0, failed: 0)"
        {statNumTablesTransformed_Data_els98,      "Total number of tables (data)       %s: %d (excluded: %d, failed: %d)"},
        {statNULLValuesInserted_els98,             "Total number of NULL values inserted: %qd (NULL representation in data file is '%s')"},

        //*
        //*     WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN
        //*
        {errNILValueTruncated_els98,               "Output of NULL values for column %s will be truncated."},
        {errIntValueOverflow_els98,                "Possible overflow of integer values for column \"%s\"."},
        {errLiteralTruncated_els98,                "Output of literal '%s' will be truncated."},
        {errSurplusLongInfo_els98,                 "Surplus long file info (column name/id '%s') ignored."},
        {errNoDatafileHeader_ls98,                 "No header in binary data file and no record length in command (using computed value of %d bytes for record length)."},
        {wrnTL_LongsButDifferentSwap_ls98,         "Different byte swap of source and target machine; long values may be corrupted after import"},
        {wrnUsageIgnoredPercent_els98,             "Value for usage %d%% to less to fit one row into one database page. Ignoring value."NEW_LINE"// *"},
        {wrnUsageIgnoredRows_els98,                "Value for usage %d rows to high to fit into one database page. Ignoring value."NEW_LINE"// *"},
        {wrnNoRestartFileFound_els98,              "Restart file %s not found; starting import from the beginning"NEW_LINE"// *"},
        {wrnLoaderConstraintViolation_els98,       "Loader CONSTRAINT violation : %s "NEW_LINE"// *"},
        {wrnDEforFLCreatesDLStmt_els98,            "Exported table has LONG column(s) or export format is RECORDS: IMPORT command generated with ROW USAGE instead of PAGE USAGE"NEW_LINE"// *"},
        {wrnConfigurationStreamNotSupported_els98, "CONFIGURATION stream is not supported for this Loader command" },


        //*
        //*     STATUSMELDUNGEN_STATUSMELDUNGEN_STATUSMELDUNGEN_STATUSMELDUNGEN_STATUSMELDUNGEN
        //*
        {msgEnvironmentPath_els98,              "%s: %s"NEW_LINE"// *"}, 
        {msgRemark_els98,                       "%s" NEW_LINE"// *"},  //!!LogMessage
        {msgUserConnectionLog_els98,            "USER:\t'%s'\ton DB:\t'%s'\tISOLATION LEVEL: %d\t%s"NEW_LINE"// *"},
        {msgReleaseUserComm_els98,              "Releasing user connection (USER: '%s')."NEW_LINE"// *"},
        {msgReleaseUserRollback_els98,          "Releasing user connection (USER: '%s'). Open transactions are rolled back (AUTOCOMMIT OFF session)."NEW_LINE"// *"},
        {msgStartCommand_els98,                 "START\t%s\t%s"NEW_LINE"// *"},
        {msgStopCommand_els98,                  "STOP\t%s\t%s"NEW_LINE"// *"},
        {msgCommandExecuting_els98,             "%s"NEW_LINE"// *"},
        {msgLastCommInputLine_els98,            "Last transaction committed at input record %ld"NEW_LINE"// *"},
        {msgLastInsInputLine_els98,             "Last inserted input record : %d"NEW_LINE"// *"},
        {msgTableNotImport_els98,               "Table not imported"NEW_LINE"// *"},
        {msgNoCtrlCmdAtDataextract_els98,       "Exported table \"%s\".\"%s\" is empty. No import/update command generated, no data file created."NEW_LINE"// *"},
        {msgStartServer_els98,                  "Starting protocol at %s%s"NEW_LINE"// *"},
        {msgVersionString_els98,                "%s"NEW_LINE"// *"},
        {msgModuleFileName_els98,               "Module: %s"NEW_LINE"// *"}, 
        {msgTL_InputFile_els98,                 "Input data file used: %s"NEW_LINE"// *"},
        {msgInstanceInformation_els98,          "%s instance started %s"NEW_LINE"// *"},
        {msgLastCommInputLine2_els98,           "Last transaction committed at input line %qd"NEW_LINE"// *"},
        {msgExecuteSystemCommand_els98,         "Execute System Command %s "NEW_LINE"// *"},
        {msgReconnected_els98,                  "Recovered from communication failure; user \"%s\" reconnected"NEW_LINE"// *"},

        {errAtRowWithTabname_els98,             "Error at record %lu in infile for table %s."},
        // Ausgabe der input line number
        {errAtRow_els98,                        "Error at record %lu in infile."},
        {errAtPos_els98,                        "Error at position %lu in infile."},
        {errAtRow2_els98,                       "Error at line %qd in infile."},
        {errAtPos2_els98,                       "Error at position %qd in infile."},

        {msgImportPackage_els98,                "Import    PACKAGE 'x%s'"NEW_LINE"// *"},
        {msgConfigPackage_els98,                "Configure PACKAGE  to transform"NEW_LINE"// *"},
        {msgExportPackage_els98,                "Export    PACKAGE 'x%s'"NEW_LINE"// *"},
        {msgExecSchemaPackage_els98,            "Execute   PACKAGE  to transform  CATALOG"NEW_LINE"/ *"},
        {msgExecDataPackage_els98,              "Execute   PACKAGE  to transform  DATA"NEW_LINE"// *"},
        {msgSequenceRangeExhausted_els98,       "Range  of SEQUENCE %s is exhausted."},
        {msgNumSynonymToTransform_els98,        "Number of SYNONYMS to transform: %d"NEW_LINE"// *"},
        {msgNumSynonymsTransformed_els98,       "Number of SYNONYMS transformed : %d"NEW_LINE"// *"},
        {msgNumViewToTransform_els98,           "Number of VIEWS    to transform: %d"NEW_LINE"// *"},
        {msgNumViewsTransformed_els98,          "Number of VIEWS    transformed : %d"NEW_LINE"// *"},
        {msgNumTabToTransform_els98,            "Number of TABLES   to transform: %d"NEW_LINE"// *"},
        {msgNumTabTransformed_els98,            "Number of TABLES   transformed : %d"NEW_LINE"// *"},
        {msgNumExtractedRecords_els98,          "Number of RECORDS  transformed from table \"%s\".\"%s\": %d, where %d ROWS were transformed with errors."NEW_LINE"// *"},
        {msgNumExtractedRecordsNoTabName_els98, "Number of RECORDS  transformed : %d,  where %d ROWS were transformed with errors."NEW_LINE"// *"},
        {msgNumTabExIncludedTransport_els98,    "Number of TABLES   %s transport: %s"NEW_LINE"// *"},
        {msgNumTabExcludedExport_els98,         "Number of TABLES   excluded from export : %d"NEW_LINE"// *"},
        {msgNumTabExcludedImport_els98,         "Number of TABLES   excluded from import : %d"NEW_LINE"// *"},
        {msgExtractedFailedTables_els98,        "Number of TABLES   exported    : %d, number of TABLES not exported because of errors %d"NEW_LINE"// *"},
        {msgLoadedRejectedTables_els98,         "Number of TABLES   imported    : %d, number of TABLES rejected %d"NEW_LINE"// *"},
        {msgNumTabToLoad_els98,                 "Number of TABLES   to import   : %d"NEW_LINE"// *"},
        {msgInsertedRejectedRows_els98,         "Number of ROWS     inserted    : %d, number of RECORDS rejected : %d"NEW_LINE"// *"},
        {msgInsertedRejectedRows2_els98,        "Number of ROWS     inserted    : %qd,number of RECORDS rejected : %qd"NEW_LINE"// *"},
        {msgUpdatedInvalidRows_els98,           "Number of ROWS     updated     : %d, number of RECORDS rejected : %d"NEW_LINE"// *"},
        {msgTL0Successful_els98,                "IMPORT TABLE  \"%s\".\"%s\" successfully executed"NEW_LINE"// *"},
        {msgTLSuccessful_els98,                 "IMPORT TABLE successfully executed"NEW_LINE"// *"},
        {msgTESuccessful_els98,                 "Export successfully executed"NEW_LINE"// *"},
        {msgDESuccessful_els98,                 "Export successfully executed"NEW_LINE"// *"},
        {msgDLSuccessful_els98,                 "IMPORT TABLE  \"%s\".\"%s\" successfully executed"NEW_LINE"// *"},
        {msgEmptyDataFile_els98,                "No data to import in datastream %s; either the stream is empty or a specified condition does not match any data"},

        // This one is ONLY to copy it to the return string
        {msgErrNo_LT_MaxAdmissibleErrNo_els98,  "Errors processing command (not exceeding MAXERRORCOUNT).\nSee loader server logfile %s for more information."},
        {msgNULLValuesInserted_els98,           "Number of NULL values inserted: %qd (NULL representation in data file is '%s')"NEW_LINE"// *"},

// do NOT use
        {errEndOfArray_els98,                   "Unknown Error - please report to SAP Basis Entwicklung Berlin\n"}
    };

    short i = 0;
    while( (ErrorDesc[i].Type != MessageNumber) &&
           (ErrorDesc[i].Type != errEndOfArray_els98) )
    {
        i++;
    }

    m_pszMsg = ErrorDesc[i].pszText;

    return;
}
// ls98_GetMessageText()



//*
//* class Loader_RTStatistics
//*
Loader_RTStatistics::Loader_RTStatistics()
{
    Init();
}

Loader_RTStatistics*
Loader_RTStatistics::Instance()
{
    if  (pStatisticsInstance == NULL)
    {
         pStatisticsInstance = new Loader_RTStatistics;
    }
    return pStatisticsInstance;
};

void
Loader_RTStatistics::Init()
{
    GatheringInterval           = 0;
    CntCatalogObjTransformed    = 0;
    CntCatalogObjExcluded       = 0;
    CntCatalogObjFailed         = 0;
    CntTablesTransformed        = 0;
    CntTablesExcluded           = 0;
    CntTablesFailed             = 0;
    CntPrimaryData              = 0;
    CntLongData                 = 0;
    CntShortLongData            = 0;
    CntLongLongData             = 0;
    Counter                     = 0;
}


#endif /*VLS98MSG_CPP*/
