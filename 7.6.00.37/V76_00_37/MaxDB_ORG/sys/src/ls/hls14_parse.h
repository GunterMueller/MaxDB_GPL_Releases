/*
  -----------------------------------------------------------------------------
  module: hls14_parse.h
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  definition of parser class

  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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
#ifndef HLS14_PARSE_H
#define HLS14_PARSE_H

#include "gls00.h"

extern "C" tin_global_in_vars i01g;

template <class T>
void ls14_TemplateNewStruct(T* &parameter)
{
    parameter = new T;
    if (NULL != parameter)
    {
        memset(parameter, DEF_BYTE_LS00, sizeof(T));
    }
}

int ls14ParseLoadCommand(tls00_Command* SvrCmd, tls00_DBInfo*   DBInfo);

/*===========================================================================*
 *    class LoaderParser 
 *===========================================================================*/
class LoaderParser
{
public:
    LoaderParser( tls00_Command*   SvrCmd, tls00_DBInfo*   DBInfo );
   ~LoaderParser();
    int StartParsing(void * & ScanReturnType); 
    int ls14GetErrorCode();

private:

    void        ls14_SetDBNode(tls00_SessionInfo* Session, tls00_String *pDBNode);
    void        ls14_SetDBName(tls00_SessionInfo* Session, tls00_String DBName);
    void        ls14_SetDBNode(tls00_SessionInfo* Session, char* pOldDBNode, tls00_String* pNewDBNode = NULL);
    void        ls14_SetUser(tls00_SessionInfo* Session, tls00_String UserName, tls00_String UserPwd);
    SAPDB_Int4  ls14_BuildDatabaseURI(tls00_SessionInfo* pSession);

    tsp00_Int4  ls14_CheckTransformationStreams(tls00_TransformationCmd *RC );

    tsp00_Int4  ls14_CheckFormat  (tls00_MultColSpec* pMultCol, bool bCompressed, bool bFormatted, bool & bBinary, bool bCompact);
    tsp00_Int4  ls14_CheckFormatEx(tls00_MultColSpec* pMultCol, bool bFormatted , bool & bBinary,  bool bCompact);

    tsp00_Int4  ls14_SpecialDateTimeParse(tls00_DateTimeSpec* pSpec,  short Type);
    int         ls14_ParseSelect(tls00_String*   selectStatement);
    void        ls14_ShowError(const ErrorNumbers lErrorNumber);

    // modules for working on rules
    tsp00_Int4 ls14_FillLongfileStruct(tls00_DeviceType  deviceType,
                                       tsp00_Int4        lColumnId,
                                       tls00_String      *pFileName,
                                       tls00_CodeSpec    *pCodeSpec,
                                       tls00_FileSpec   **pFileSpecifier);

    tsp00_Int4 ls14_FillLongfileStruct(tls00_DeviceType  deviceType,
                                       tls00_String      &pColumnName,
                                       tls00_String      *pFileName,
                                       tls00_CodeSpec    *pCodeSpec,
                                       tls00_FileSpec   **pFileSpecifier);
    
    void       ls14_ChangeUSE_USERString (tls00_String &PwPtr);
    tsp00_Int4 ls14_GetSeparatorDelimiter(char* pszSource, tsp00_Uint4 lSourceLen, char* pszDestination);
    tsp00_Int4 ls14_SeparateIntValues(char*         pszString,
                                      char          SeparatorSign,
                                      tsp00_Int4*   pval1,
                                      tsp00_Int4*   pval2,
                                      bool          bSeparatorIsMandatory = false);

    void       ls14_GetVersionInformation();


    int yyparse(void * &ScanReturnType);

private:
    LoaderScanner      *Scanner;
    LoaderMessages     *MsgInstance_ls14;    
    ErrorNumbers        ParseError;           // Error variable; passed to Scanner, too
    
    tls00_DBInfo        *lcpDBInfo_ls14;
    tls00_Command       *lcpCommand_ls14;

    _TCHAR              *pszMyUserStmt_ls14;  // used to temporarily save 'use user' stmt w/o pw for later output

    static char const   *pszAncientDateTimeParseErrText[13];
    static char const   *pszDateTimeStrings[3];

    bool                 bFileEncodingNotSpecified_ls14;
    bool                 bDataFileEncodingNotSpecified_ls14;

    bool                 bFileFormatNotSpecified_ls14;
    bool                 bDataFileFormatNotSpecified_ls14;
    bool                 bIdentifierIsQuoted_ls14;

    // Variables used by Bison
    int         yychar;     //  the lookahead symbol
    ScanReturn  yylval;     //  the semantic value of the lookahead symbol
    ScanReturn  yylloc;     //  location data for the lookahead symbol
    int         yynerrs;    //  number of parse errors so far
};


//*
//*     All following prototypes (i41...) are for compatibility reasons with the respective
//*     pascal source vin41 where no prototypes exist.
//*
externPascal void i41dparse(tin_date_mask&  DateStruct,
                            int&            lError,
                            tsp00_Uint2&    lErrorPos);

externPascal void i41tparse(tin_date_mask&  TimeStruct,
                            int&            lError,
                            tsp00_Uint2&    lErrorPos);

externPascal void i41tsparse(tin_date_mask& TimeStampStruct,
                             int&           lError,
                             tsp00_Uint2&   lErrorPos);


#endif // HLS14_PARSE_H
