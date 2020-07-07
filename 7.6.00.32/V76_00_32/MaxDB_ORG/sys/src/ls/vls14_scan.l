/*

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
%{
/*
  -----------------------------------------------------------------------------

  module: vls14_scan.l

  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  scanner grammar

  version:      7.6.

  -----------------------------------------------------------------------------
*/

#include "vls14_parse_gen.h"    // by bison generated header-file containing token definitions
#include "hls07.h"
%}


/*
    ALLOWED CHARS FOR IDENTIFIER:

    first_char_in_identifier            a-z
    ------------------------            A-Z
                                        #$@
                                        ASCII (^= ISO 8859/1.2) 192-214, 216-222, 224-246, 248-254

    remaining_chars_in_identifier       a-z
    -----------------------------       A-Z
                                        #$@
                                        _
                                        digits
                                        ASCII (^= ISO 8859/1.2) 192-214, 216-222, 224-246, 248-254


    PASSWORD IDENTIFIER differ from INDENTIFIER in that they allow digits as first character
    Syntax for HOSTNAMES (DNS_NAME) is defined in RFC 1035 DNS_NAME corresponds to dns-domain)

*/

DIGIT           [0-9]
LETTER          [a-zA-Z]
SPECIAL_LETTER  [\200-\377]
HYPHEN          [-]
POINT           [.]
COLON           [:]
SLASH           [\/]


HEX                   ({DIGIT}|"A"|"B"|"C"|"D"|"E"|"F"|"a"|"b"|"c"|"d"|"e"|"f")
URI_ESCAPED           ("%"{HEX}{HEX})
URI_MARK              ("-"|"_"|"."|"!"|"~"|"*"|"'"|"("|")")
URI_UNRESERVED        ({ALPHANUM}|{URI_MARK})
URI_RESERVED          (";"|"\/"|"?"|":"|"@"|"&"|"="|"+"|"$"|",")
URI_LETTER            ({URI_UNRESERVED}|{URI_RESERVED}|{URI_ESCAPED})

URI_SCHEME_LETTER     ({ALPHANUM}|"+"|{HYPHEN}|{POINT})
SCHEME                ({LETTER}({URI_SCHEME_LETTER}*))
URI_SCHEME            (({SCHEME}{COLON}){1,2})
URI_SLASH             ({SLASH}{1,2})
URI_REFERENCE         ({URI_SCHEME}{URI_SLASH}({URI_LETTER}+))


ALPHANUM              ({LETTER}|{DIGIT})
ALPHANUM_HYP          ({ALPHANUM}|{HYPHEN})
DNS_TRAILER           ({ALPHANUM_HYP}*)({ALPHANUM})
DNS_LABEL             ({LETTER})({DNS_TRAILER}*)
DNS_LABEL_POINT       ({POINT}{DNS_LABEL})
DNS_NAME              ({DNS_LABEL}{DNS_LABEL_POINT}*)

DBID_ALPHANUM         ({ALPHANUM}|"#"|"$"|"@"|{SPECIAL_LETTER})
DBID_ALPHANUM_HYP     ({DBID_ALPHANUM}|"_")
DBID_TRAILER          ({DBID_ALPHANUM_HYP}*)({DBID_ALPHANUM})
DBID_LABEL            ({LETTER})({DBID_TRAILER}*)
DBID_LABEL_POINT      ({POINT}{DBID_LABEL})
DBID_NAME             ({DBID_LABEL}{DBID_LABEL_POINT}*)


THREE_DIGITS          ({DIGIT}{1,3})
POI_THR_DIG           ({POINT}{DIGIT}{1,3})
IP_ADDRESS            ({THREE_DIGITS})({POI_THR_DIG})({POI_THR_DIG})({POI_THR_DIG})

HOST_NAME             ({DBID_NAME}|{DNS_NAME}|{IP_ADDRESS})
HOST_NAME_PORT        ({HOST_NAME}({COLON}{DIGIT}+)?)

/* Begin of syntax definition ***********************************************************/
%%

[\/][\/][^\n]*    { /* Skip comments */
                    CopyToken("COMMENT");
                    IncreasePosition();
                }
[\00-\40]       {
                    /* Sonder- und Leerzeichen ueberlesen */
                    IncreasePosition();
                }
'[^']*(''[^']*)*'  {   /*http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1131747*/
                        /* Quotes are escaped by an additional quote. */
    
                            CopyToken("SINGLE_QUOTED_STRING");  
                            ReturnStructure.C_String.strAddr_ls00 = &pLoaderCommand->cmdReqDat_ls00[pLoaderCommand->cmdReqPos_ls00];
                            ReturnStructure.C_String.strLeng_ls00 = yyleng; 
                            IncreasePosition();
                            yylex_return = dtSINGLE_QUOTED_STRING;
                        }
[\"][^"]*(\"\"[^"]*)*[\"]  {    /*http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1131747*/
                                /* Quotes are escaped by an additional quote. */

                                CopyToken("DOUBLE_QUOTED_STRING");
                                ReturnStructure.C_String.strAddr_ls00 = &pLoaderCommand->cmdReqDat_ls00[pLoaderCommand->cmdReqPos_ls00];
                                ReturnStructure.C_String.strLeng_ls00 = yyleng;
                                IncreasePosition();
                                yylex_return = dtDOUBLE_QUOTED_STRING;
                            }
"("             {
                    CopyToken("OPEN");
                    IncreasePosition();
                    ReturnStructure.C_Char = yytext[0];
                    yylex_return = sgnOPEN;
                }
")"             {
                    CopyToken("CLOSE");
                    IncreasePosition();
                    ReturnStructure.C_Char = yytext[0];
                    yylex_return = sgnCLOSE;
                }
","             {
                    CopyToken("COMMA");
                    IncreasePosition();
                    yylex_return = sgnCOMMA;
                }
"."             {
                    CopyToken("POINT");
                    IncreasePosition();
                    yylex_return = sgnPOINT;
                }
":"             {
                    CopyToken("COLON");
                    IncreasePosition();
                    yylex_return = sgnCOLON;
                }
"%"             {
                    CopyToken("PERCENT");
                    IncreasePosition();
                    yylex_return = sgnPERCENT;
                }
"-"             {
                    CopyToken("MINUS");
                    IncreasePosition();
                    yylex_return = opMI;
                }
"="             {
                    CopyToken("EQUAL");
                    IncreasePosition();
                    ReturnStructure.C_Char2[0] = yytext[0];
                    ReturnStructure.C_Char2[1] = BLANK_LS00;
                    yylex_return = opEQ;
                }
"<"             {
                    CopyToken("SMALL");
                    IncreasePosition();
                    ReturnStructure.C_Char2[0] = yytext[0];
                    ReturnStructure.C_Char2[1] = BLANK_LS00;
                    yylex_return = opSM;
                }
">"             {
                    CopyToken("LARGE");
                    IncreasePosition();
                    ReturnStructure.C_Char2[0] = yytext[0];
                    ReturnStructure.C_Char2[1] = BLANK_LS00;
                    yylex_return = opLA;
                }
"<="            {
                    CopyToken("SMALL EQ");
                    IncreasePosition();
                    ReturnStructure.C_Char2[0] = yytext[0];
                    ReturnStructure.C_Char2[1] = yytext[1];
                    yylex_return = opSE;
                }
">="            {
                    CopyToken("LARGE EQ");
                    IncreasePosition();
                    ReturnStructure.C_Char2[0] = yytext[0];
                    ReturnStructure.C_Char2[1] = yytext[1];
                    yylex_return = opLE;
                }
"<>"            {
                    CopyToken("NOT EQ");
                    IncreasePosition();
                    ReturnStructure.C_Char2[0] = yytext[0];
                    ReturnStructure.C_Char2[1] = yytext[1];
                    yylex_return = opNE;
                }
"ADSM"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwADSM;
                }
"TSM"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwADSM;
                }
"ALL"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwALL;
                } 
"AND"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwAND;
                }
"ANSI"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwANSI;
                }
"APPEND"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwAPPEND;
                }
"ASCII"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwASCII;
                }
"AUTOCOMMIT"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwAUTOCOMMIT;
                }
"BACK"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwBACK;
                }
"BACKINT"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwBACK;
                }
"BACKUPTOOL"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwBACKUPTOOL;
                }
"LOB"[\00-\40]+"INSTREAM" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLONGFILE;
                }
"LOB"[\00-\40]+"OUTSTREAM" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLONGFILE;
                }                 
"BOOLEAN"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwBOOLEAN;
                }
"BINARY"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwBINARY;
                }
"BY"            {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwBY;
                }
"CATALOG"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCATALOG;
                }
"CATALOGEXTRACT" {
                     CopyToken("KEYWORD");
                     IncreasePosition();
                     yylex_return = kwCATALOGEXTRACT;
                 }
"CATALOGLOAD"   {
                     CopyToken("KEYWORD");
                     IncreasePosition();
                     yylex_return = kwCATALOGLOAD;
                }
"CHAR"          {
                     CopyToken("KEYWORD");
                     IncreasePosition();
                      yylex_return = kwCHAR;
                }
"CHECKCONSTRAINT" {
                     CopyToken("KEYWORD");
                     IncreasePosition();
                      yylex_return = kwCHECKCONSTRAINT;
                }    
"CODETYPE"      {
                     CopyToken("KEYWORD");
                     IncreasePosition();
                     yylex_return = kwCODETYPE;
                }
"CODEPAGE"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCODEPAGE;
                }
"COMPACT"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwRECORDS;
                }
"COMPRESSED"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCOMPRESSED;
                }
"CONFIGURATION" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCONFIGURATION;
                }                
"COUNT"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCOUNT;
                }
"CSV"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCOMPRESSED;
                }   
"CREATE"[\00-\40]+"SCHEMAMAP" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCREATESCHEMAMAP;
                }
"CURRENT"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCURRENT;
                }
"CURRENT_SCHEMA" {
                     CopyToken("KEYWORD");
                     IncreasePosition();
                     yylex_return = kwCURRENT_SCHEMA;
                 }
"DATA"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATA;
                }
"DATAEXTRACT"   {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATAEXTRACT;
                }
"DATALOAD"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATALOAD;
                }
"DATAUPDATE"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATAUPDATE;
                }
"DATE"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATE;
                }
"DB"            {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwALL;
                }
"DBEXTRACT"     {
                     CopyToken("KEYWORD");
                     IncreasePosition();
                     yylex_return = kwDBEXTRACT;
                }                
"DBLOAD"        {
                     CopyToken("KEYWORD");
                     IncreasePosition();
                     yylex_return = kwDBLOAD;
                }                        
"DB2"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDB2;
                }
"DDL"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDDL;
                }
"DDL"[\00-\40]+"NO"[\00-\40]+"INDEX"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDDLNOINDEX;
                }                                      
"DECIMAL"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDECIMAL;
                }
"DEFAULT"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDEFAULT;
                }
"DELIMITER"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDELIMITER;
                }
"DTL"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDTL;
                }
"DROP"[\00-\40]+"SCHEMAMAP" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDROPSCHEMAMAP;
                }
"DUPLICATES"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDUPLICATES;
                }
"EBCDIC"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEBCDIC;
                }
"EBID"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEBID;
                }
"ENCRYPTION"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwENCRYPTION;
                }
"EUR"             {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEUR;
                  }
"EXCLUDE"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEXCLUDE;
                  }
"EXPORT"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEXPORT;
                  } 
"EXPORT"[\00-\40]+"COLUMNS" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATAEXTRACT;
                  }                                   
"EXPORT"[\00-\40]+"TABLE" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEXPORTTABLE;
                  }                         
"EXTRACT"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEXPORT;
                  }
"EXTRACT"[\00-\40]+"CATALOG" { 
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCATALOGEXTRACT;
                  }
"EXTRACT"[\00-\40]+"TABLE"   { 
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEXPORTTABLE;
                  }                         
"EXTRACT"[\00-\40]+"DATA"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATAEXTRACT;
                  }                  
"EXTRACT"[\00-\40]+"COLUMNS" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATAEXTRACT;
                }
"FAST"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwPAGE;
                }                
"FASTLOAD"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwFASTLOAD;
                }
"FIELDS"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwOUTFIELDS;
                }
"FILE"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwFILE;
                }
"FOR"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwFOR;
                }
"FORMAT"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwFORMAT;
                }                
"FORMATTED"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwFORMATTED;
                }
"FWV"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwFORMATTED;
                }
"HEX"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwHEX;
                }
"HILO"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwHILO;
                }
"IF"            {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwIF;
                }
"IGNORE"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwIGNORE;
                }
"IMPORT"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwIMPORT;
                }
"IMPORT"[\00-\40]+"COLUMNS"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATALOAD;
                }                 
"IMPORT"[\00-\40]+"TABLE"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwIMPORTTABLE;
                } 
"INCLUDE"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwINCLUDE;
                }
"INFILE"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwINFILE;
                }
"INSTREAM"      {   // alternative keyword
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwINFILE;
                }
"INTEGER"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwINTEGER;
                }
"INTERNAL"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwINTERNAL;
                }
"INSTALLATION"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwINSTALLATION;
                }
"ISO"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwISO;
                }
"ISOLATION"[\00-\40]+"LEVEL"   {
                        CopyToken("KEYWORD");
                        IncreasePosition();
                        yylex_return = kwISOLATION_LEVEL;
                    } 
"JIS"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwJIS;
                }
"KEY"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwKEY;
                }
"LANGUAGE"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLANGUAGE;
                }
"LOAD"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwIMPORT;
                }
"LOAD"[\00-\40]+"CATALOG"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwCATALOGLOAD;
                }
"LOAD"[\00-\40]+"TABLE"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwIMPORTTABLE;
                }                 
"LOAD TABLE FAST" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTABLEFASTLOAD;
                }                        
"LOAD"[\00-\40]+"DATA"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATALOAD;
                }
"LOAD"[\00-\40]+"COLUMNS"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATALOAD;
                }                
"LOAD"[\00-\40]+"FAST"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwFASTLOAD;
                }
"LOGOWNER"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLOGOWNER;
                }
"REMARK"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwREMARK;
                }
"LOHI"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLOHI;
                }
"LONGFILE"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLONGFILE;
                }
"LONGSTREAM"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLONGFILE;
                }                

"LONG"[\00-\40]+"INSTREAM" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLONGFILE;
                }
"LONG"[\00-\40]+"OUTSTREAM" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwLONGFILE;
                }  
"MAP"[\00-\40]+"COLUMNS" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwOUTFIELDS;
                }
"MAP"[\00-\40]+"SCHEMAS" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwMAPSCHEMAS;
                }
"MAP"[\00-\40]+"SCHEMA" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwMAPSCHEMAS;
                }
"MAXERRORCOUNT" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwMAXERRORCOUNT;
                }
"MAX"[\00-\40]+"RECORDS" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwMAXRECORDS;
                }
"MAX"[\00-\40]+"ROWS"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwMAXROWS;
                }
"MAXRECORDS"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwMAXRECORDS;
                }
"MAXROWS"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwMAXROWS;
                }
"MESSAGE"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwMESSAGE;
                }
"NOHEADER"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNOHEADER;
                }
"NODATA"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNODATA;
                }
"NODDL"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNODDL;
                }
"NO"[\00-\40]"LOG" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNOLOG;
                }
"NOLOG"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNOLOG;
                }
"NOT"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNOT;
                }
"NSR"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNSR;
                }
"NETWORKER"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNSR;
                }
"NULL"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNULL;
                }
"NUMBER"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwNUMBER;
                }
"ODBCTRACE"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwODBCTRACE;
                }
"OR"            {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwOR;
                }
"OFF"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwOFF;
                }
"ON"            {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwON;
                }
"ORACLE"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwORACLE;
                }
"ORDER"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwORDER;
                }
"OTHERWISE"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwOTHERWISE;
                }
"OUTFILE"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwOUTFILE;
                }
"OUTFIELDS"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwOUTFIELDS;
                }
"OUTSTREAM"     {   // alternative keyword
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwOUTFILE;
                }
"PACKAGE"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwPACKAGE;
                }
"PAGE"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwPAGE;
                }
"PAGES"         {   //alternative keyword
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwPAGES;
                }
"PIPE"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwPIPE;
                }
"POS"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwPOS;
                }
"REAL"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwREAL;
                }
"RECORD"[\00-\40]+"USAGE"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwRECORD;
                }
"RECORDS"[\00-\40]+"USAGE" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwRECORD;
                }   
"RECORDS"       {   // alternative keyword
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwRECORDS;
                }                                
"REJECT"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwREJECT;
                }
"RELEASE"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    pLoaderCommand->cmdSubFunc_ls00 = sfRelease_els00;
                    yylex_return = kwRELEASE;
                }
"RESTART"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwRESTART;
                }
"ROUND"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwROUND;
                }
"ROW"[\00-\40]+"USAGE"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwRECORD;
                }                
"ROWS"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwROWS;
                }
"SCALE"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSCALE;
                }
"SCHEMA"        {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSCHEMA;
                } 
"SCHEMAMAP"     {
                     CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSCHEMAMAP;
                }
"SEPARATOR"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSEPARATOR;
                }
"SEQNO"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSEQNO;
                }
"SERVERDB"      {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSERVERDB;
                }
"SET"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSET;
                }
"SIZE"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSIZE;
                }
"SQLID"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSQLID;
                }
"SQLMODE"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSQLMODE;
                }
"SSL"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSSL;
                }
"STAMP"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSTAMP;
                }
"START"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSTART;
                }
"STATISTICS"    {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSTATISTICS;
                }
"SYSDATE"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSYSDATE;
                }
"SYSTEMATTRIBUTE"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwSYSTEMATTRIBUTE;
                }               
"TRANSPORT"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTRANSPORT;
                }                
"TABLE"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTABLE;
                }
"TABLEEXTRACT"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwEXPORTTABLE;
                }
"TABLELOAD"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwIMPORTTABLE;
                }
"TABLEFASTLOAD" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTABLEFASTLOAD;
                }
"TABLEUNLOAD"   {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTABLEUNLOAD;
                }
"TABLEUPDATE"   {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTABLEUPDATE;
                }
"TAPE"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTAPE;
                }
"TIME"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTIME;
                }
"TIMESTAMP"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTIMESTAMP;
                }
"TO"            {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTO;
                }
"TRANSACTION"   {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTRANSACTION;
                }
"TRUNC"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTRUNC;
                }
"UCS2"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUCS2;
                }
"UID"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUID;
                }
"UNLOAD"[\00-\40]+"TABLE"  {   // Alternative to tableupdate
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTABLEUNLOAD;
                }

"UPDATE"/[\00-\40]*"DUPLICATES" {
/* update wird nur dann als schluesselwort erkannt, wenn ihm beliebig viele sonderzeichen
   und das wort 'duplicates' folgen, wobei die auf update folgenden zeichen zwar
   vorausgelesen aber nicht in yytext gespeichert werden, somit als noch nicht gelesen
   gelten.
   Zusammen ergibt 'update duplicates' einen teil der dataload-anweisung.
   Update allein wird als unbekanntes schluesselwort interpretiert und als sql-kommando
   abgearbeitet
*/
                                    CopyToken("KEYWORD");
                                    IncreasePosition();
                                    yylex_return = kwUPDATE;
                                }
"UPDATE"[\00-\40]+"COLUMNS"   {   // Alternative to dataupdate
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATAUPDATE;
                }
"UPDATE"[\00-\40]+"DATA"   {   // Alternative to dataupdate
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwDATAUPDATE;
                }
"UPDATE"[\00-\40]+"TABLE"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwTABLEUPDATE;
                }
"USA"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUSA;
                }
"USAGE"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUSAGE;
                }
"USE"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUSE;
                }
"USER"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUSER;
                }
"USERGROUP"     {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUSERGROUP;
                }
"USERKEY"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUSERKEY;
                }
"USE"[\00-\40]+"SCHEMAMAP" {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUSESCHEMAMAP;
                }
"UTF8"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwUTF8;
                }
"VERSION"       {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwVERSION;
                }
"WITH"          {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwWITH;
                }
"XML"           {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwXML;
                }
"XML"[\00-\40]+"NO"[\00-\40]+"TABLE"  {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwXMLNOTABLE;
                }                                      
"ZONED"         {
                    CopyToken("KEYWORD");
                    IncreasePosition();
                    yylex_return = kwZONED;
                }
{DIGIT}+        {
                    CopyToken("INTEGER_LITERAL");
                    ReturnStructure.C_String.strAddr_ls00 = &pLoaderCommand->cmdReqDat_ls00[pLoaderCommand->cmdReqPos_ls00];
                    ReturnStructure.C_String.strLeng_ls00 = yyleng;
                    IncreasePosition();
                    yylex_return = dtINTEGER_LITERAL;
                }
[^\00-\40\(\)\,\-<>=\.%]+ {
                    CopyToken("DB_IDENTIFIER");
                    ReturnStructure.C_String.strAddr_ls00 = &pLoaderCommand->cmdReqDat_ls00[pLoaderCommand->cmdReqPos_ls00];
                    ReturnStructure.C_String.strLeng_ls00 = yyleng;
                    IncreasePosition();
                    yylex_return = dtDB_IDENTIFIER;
                }
{URI_REFERENCE}   {
                    CopyToken("URI_REFERENCE");
                    ReturnStructure.C_String.strAddr_ls00 = &pLoaderCommand->cmdReqDat_ls00[pLoaderCommand->cmdReqPos_ls00];
                    ReturnStructure.C_String.strLeng_ls00 = yyleng;
                    IncreasePosition();
                    yylex_return = dtURI_REFERENCE;
                }
{HOST_NAME_PORT}  {
                    CopyToken("DNS_NAME");
                    ReturnStructure.C_String.strAddr_ls00 = &pLoaderCommand->cmdReqDat_ls00[pLoaderCommand->cmdReqPos_ls00];
                    ReturnStructure.C_String.strLeng_ls00 = yyleng;
                    IncreasePosition();
                    yylex_return = dtDNS_NAME;
                }

%%
/* End of syntax definition *************************************************************/
