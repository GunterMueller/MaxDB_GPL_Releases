/*! 
  -----------------------------------------------------------------------------
  module: gls00.h
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893 

  special area: SAP DB LOADER
 
  description:  "Fundamental definition module of the LS layer"
                
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
#ifndef GLS00
#define GLS00

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RunTime/System/RTESys_MicroTime.h"
#if defined(WIN32)
#include <strstream> 
using namespace std;
#endif
#if defined(PA20W)
#include <fstream.h>
#else
#include <fstream>   
using namespace std;
#endif

#include "heo02.h"
#include "heo03.h"
#include "heo06.h"
#include "hsp77.h"
#include "hsp81.h"
#include "vsp007.h"

#include "ToolsCommon/Tools_DynamicUTF8String.hpp"                  // unicode support
#include "hwd07dbc.h" 
#include "hls33_backup.h"

#include "SAPDBCommon/SAPDB_string.h"

#include "vin00.h"                                                  // time and date conversions and parse-ID

#include "geo47.h"
#include "gsp100.h"
#include "hsp100.h"
#include "ggg00.h"

#include "RunTime/RTE_Configure.h"  /* dep=RTE_Configure.pl */      // definition of swap kind


/* START OF GENERAL DEFINITIONS: *******************************************************/

#ifndef UNICODE
#define UNICODE
#endif
//#define ODBC_PREPARED


#if defined(_WIN32)
#define NEW_LINE_LS00 "\r\n"
const short NEW_LINE_LEN_LS00 = 2;
#else
#define NEW_LINE_LS00 "\n"
const short NEW_LINE_LEN_LS00 = 1;
#endif

#if defined(_WIN32)
#define FILENAME_SEPARATOR_LS00 "\\"
#else
#define FILENAME_SEPARATOR_LS00 "/"
#endif


// define swapping of the server
#if defined (SDB_BYTE_ORDER_MOST_SIGNIFICANT_BYTE_FIRST)
const tsp00_SwapKind_Enum SERVER_SWAPPING_LS00 = sw_normal;
#else
const tsp00_SwapKind_Enum SERVER_SWAPPING_LS00 = sw_full_swapped;
#endif

const unsigned char UNDEF_BYTE_LS00         = STATIC_CAST(unsigned char, csp_undef_byte);
const char          DEF_BYTE_LS00           = csp_defined_byte;
const char          DEF_BYTE_ASCII_LS00     = '\x20';
const char          DEF_BYTE_EBCDIC_LS00    = '\x40';
const char          DEF_BYTE_UNICODE_LS00   = csp_unicode_def_byte;
const char          BLANK_LS00              = ' ';
const int           SLASH_LS00              = '/';
const int           BACKSLASH_LS00          = '\\';
const char          EOC_LS00                = STATIC_CAST(char,0x3B);

const char          QUOTE_LS00              = '"';
const char          DOUBLE_QUOTE_LS00       = '"';
const char          SINGLE_QUOTE_LS00       = '\'';
const char          COMMA_LS00              = ',';
const char          POINT_LS00              = '.';
const char          NO_THSD_GROUPS_LS00     = 'N';      // zur einstellung der tausendergruppen bei dezimalzahlen

const char          ProtOkHeader_ls00[5]    = "OK \n";
const char          ProtErrHeader_ls00[5]   = "ERR\n";
const int           ProtHeaderLength_ls00   = 4;

const tsp00_Int4    PROTOCOL_SIZE_LS00      = 4*sizeof(char);

const int           MAX_STMT_LENGTH_LS00    = 4*PATH_MXSP00+ 1028*4 +1028;
const int           MAX_PARAM_LS00          = 10;
const int           EOF_LS00                = -1;

const int           MAX_NUMERICLEN_LS00     = csp_fixed;
const int           MAX_SHORTREAL_LS00      = 4;
const int           MAX_LONGREAL_LS00       = 8;

const short         MAX_BOOLVALUE_LENGTH_LS00    = 10;
const short         MAX_DEFNULL_SPEC_LENGTH_LS00 = 20;

const int           MAX_PARSEID_LENGTH_LS00 = mxin_parsid;



#define             UNDEFINED_LS00  -2
#define             NULL_VALUE   "?"

// fuer tableextract/tableload
const tsp00_Uint1   MY_PAGE_LS00            = 255;

#ifdef UNICODE
// using this size value in char string definitions we have to consider the trailing NULL
// and the fact that names could be decorated with double quotes which cause the name
// to be 2 byte longer; the kernel however does not pay any attention to the additional bytes
//
// When using this size for kernel identifiers add 5 to be on the safe site
const short         KNL_IDENTIFIER_SIZE = sizeof(tsp00_KnlIdentifier);
#else
const short         KNL_IDENTIFIER_SIZE = sizeof(tsp00_KnlIdentifier)/2;
#endif

const int           MAX_REPLY_LENGTH_LS00      = 1024;

const char          COMP_NAME_LS00[65]         = "SAPDB LOADER (SERVER COMPONENT)                                 ";
const char          APPL_NAME_LS00[4]          =  csp_comp_load;

const char          HEADER_IDENTIFIER_LS00[33] = "REPLICATION SERVER SAP DB       ";

#define EXISTS_TABLE_STMT_LS00     _T("EXISTS TABLE \"%s\".\"%s\"")

#define INSERT_EMPTY_VALUE -10
#define INSERT_DEFAULT     -11
#define INSERT_NULL        -12
#define INSERT_ASSIGNED    -13

#define STOP_NOMORE_DATA_LS00     -111
#define STOP_LAST_DATA_LS00       -222
#define STOP_NO_DATA_LS00         -333

#define CMD_SEPARATOR_TOKEN_LS00       "//"
#define CMD_SEPARATOR_TOKEN_LEN_LS00   2

// EXTRACT TABLE


// Maximale LONG Daten Länge im Eingabedatenstrom: 'Filename [von-bis]'
#define DEF_MAX_LONG_VALUE_LS00 1024

#define SHORT_STMT_LENGTH_LS00     4*sizeof(tsp00_KnlIdentifier)

//  header sizes of an sql packet of the order interface
static tsp00_Int4 BlockHSize_ls00   = sizeof(tsp1_packet_header) + sizeof(tsp1_segment_header) + sizeof(tsp1_part_header);
static tsp00_Int4 PartHSize_ls00    = sizeof(tsp1_part_header);
static tsp00_Int4 SegmentHSize_ls00 = sizeof(tsp1_segment_header);
static tsp00_Int4 PacketHSize_ls00  = sizeof(tsp1_packet_header);

static tsp00_Int4 LongDescSize_ls00 = sizeof(tsp00_LongDescriptor);

/* Possible db data types

 0         1          2          3           4         5           6            7           8             9
 dfixed,   dfloat,    dcha,      dche,       dchb,     drowid,     dstra,       dstre,      dstrb,        dstrdb,
 10
 ddate,    dtime,     dvfloat,   dtimestamp, dunknown, dnumber,    dnonumber,   dduration,  ddbyteebcdic, dlonga,
 20
 dlonge,   dlongb,    dlongdb,   dboolean,   dunicode, ddtfiller1, ddtfiller2,  ddtfiller3, ddtfiller4,   dsmallint,
 30
 dinteger, dvarchara, dvarchare, dvarcharb,  dstruni,  dlonguni,   dvarcharuni, dudt,       dabaptabhandle
*/

// char data types
static char
szMap_CharDataType_ls00[39] =
    {0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0,0,1,0,0};

// long data types
static char
szMap_LongDataType_ls00[39] =
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0};


// number data types
static char
szMap_NumberDataType_ls00[39] =
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0};


// special constant data types
static char
szMap_SpecConstDataType_ls00[39] =
    {0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


// blob data types
static char
szMap_BlobDataType_ls00[39] =
    {0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0};


// unicode (string) data types
static char
szMap_UnicodeDataType_ls00[39] =
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0};


// Defines the necessity to send data of the particular data type as UCS2 when order interface
// is unicode (replication server works with unicode order interface if database is unicode)
static bool
bUnicodeOverOI_ls00[39] = {false,false,true ,true ,false,false,true ,true ,false,false,
                           true ,true ,false,true ,false,false,false,false,false,false,
                           false,false,false,false,true ,false,false,false,false,false,
                           false,true ,true ,false,true ,true ,true ,false,false};


//  leading headers of an sql packet of the order interface
typedef char tls00_PktBlock [sizeof(tsp1_packet_header) + sizeof(tsp1_segment_header) + sizeof(tsp1_part_header)];


// Laenge des headers einer datendatei bei TABLEEXTRACT
#define TE_FILE_HEADER_LENGTH_LS00   1024

// DB return codes
typedef enum{
    dbNotLoadedCharset_els00   = -4025,
    dbUnknownTableName_els00   = -4004,
    dbRowNotFound_els00        = 100    
} tls00_DBReturnCodes;



//* Resulttypes of syntax parser
//* ACHTUNG: tls00_ScanResult und ScanResultName_ls00 sind miteinander korreliert!!!
typedef enum {
    srUnknown_els00,
    srRelease_els00,
    srFctTransport_els00,
    srFctCatExtract_els00,
    srFctCatLoad_els00,
    srFctColExtract_els00,
    srFctColLoad_els00,
    srFctColUpdate_els00,
    srFctDBExtr_els00,
    srFctDBLoad_els00,
    srFctTabExtract_els00,
    srFctTabLoad_els00,
    srFctPartExtract_els00,
    srFctPartLoad_els00,
    srFctSqlStmt_els00,
    srCmdSession_els00,
    srFctRemark_els00,
    srFctCreateSchemaMap_els00,
    srFctUseSchemaMap_els00,
    srFctDropSchemaMap_els00
} tls00_ScanResult;

//* Namen fuer die oben definierten resultattypen
static const char* ScanResultName_ls00[] = {
//    "UNKNOWN",
    "RELEASE",
    "TRANSPORT",
    "CATALOGEXTRACT",
    "CATALOGLOAD",
    "DATAEXTRACT",
    "DATALOAD",
    "DATAUPDATE",
    "DBEXTRACT",
    "DBLOAD",
    "TABLEEXTRACT",
    "TABLELOAD",
    "PARTEXTRACT",
    "PARTLOAD",
    "SQL_STMT",
    "SESSION_STMT",
    "REMARK"  ,
    "CREATESCHEMAMAP",
    "USESCHEMAMAP",
    "DROPSCHEMAMAP",
    "EXTERN_STMT"
};

/* Modi for PackageConfiguration */
typedef enum {
    ctInclude_els00,
    ctExclude_els00
} tls00_ConfigurationType;

static char const *pszConfigurationTypeName_ls00[2] = {"included to","excluded from"} ;


/* Modi zur zusaetzlichen klassifikation des parse-resultats */
typedef enum
{
    sfUnknown_els00,
    sfAutoCommitON_els00,
    sfUseUser_els00,
    sfRelease_els00
} tls00_SubFuncClass;

/* Modi for copying string to packet (ls04StringToPart) */
typedef enum {
    smUnknown_els00,
    smVarLeng_els00,
    smDefLeng_els00,
    smFixLeng_els00
} tls00_StrMode;


/* Character codes */
/* The order of the values MUST remain; if new values are necessary add them at the end*/
typedef enum {
    ctUnknown_els00,
    ctAscii_els00,
    ctEbcdic_els00,
    ctUTF8_els00,
    ctIgnore_els00,         // corresponds to BYTE in db and binary in data file struct
    ctUCS2_els00,           // not swapped UCS2 (high byte, low byte)
    ctUCS2Swapped_els00,    // swapped UCS2 (low byte, high byte)
    ctUCS2Native_els00,     // swapped/not swapped UCS2 depending on machine server running on
    ctCodeSet_els00
} tls00_CodeType;

static char const *pszCodeTypeName_ls00   [9] = {  " "      ,   " ASCII ",   " EBCDIC ",   " UTF8 ",   " BINARY ",   " UCS2 ",   " UCS2 ",   " UCS2 ",   " CODESET " };
static char const *pszCodeTypeValue_ls00  [9] = {  "'ASCII'",  " 'ASCII' ", " 'EBCDIC' ", " 'UTF8' ", " 'BINARY' ", " 'UCS2' ", " 'UCS2' ", " 'UCS2' ", " 'CODESET' " };
static char const *pszAppend_ls00         [2] = {  " "      ,   " APPEND " };

/* Encodings; directly related to tls00_CodeType */
static const tsp77encoding *pEncodings_ls00[9] = {NULL,
                                           sp77encodingAscii,
                                           NULL,                    // EBCDIC
                                           sp77encodingUTF8,
                                           NULL,                    // IGNORE = BINARY
                                           sp77encodingUCS2,
                                           sp77encodingUCS2Swapped,
                                           sp77encodingUCS2Native,
                                           NULL};                   // Code set


//*
//*     Code page related values
//*

// default code page (ASCII->UCS2 mapping stored in system table CODEPAGE upon DB installation)
static char const *pszDefaultCodePageName_ls00 = "ISO-8859-1";

// default code page map = ISO-8859-1
static const tsp00_Uint2 lDefaultCodePageMap_ls00[256] =
{0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,
 0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F,
 0x0010,0x0011,0x0012,0x0013,0x0014,0x0015,0x0016,0x0017,
 0x0018,0x0019,0x001A,0x001B,0x001C,0x001D,0x001E,0x001F,
 0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,
 0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
 0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,
 0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
 0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,
 0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
 0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,
 0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,
 0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,
 0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
 0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,
 0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x007F,
 0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,
 0x0088,0x0089,0x008A,0x008B,0x008C,0x008D,0x008E,0x008F,
 0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,
 0x0098,0x0099,0x009A,0x009B,0x009C,0x009D,0x009E,0x009F,
 0x00A0,0x00A1,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7,
 0x00A8,0x00A9,0x00AA,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
 0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,
 0x00B8,0x00B9,0x00BA,0x00BB,0x00BC,0x00BD,0x00BE,0x00BF,
 0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,
 0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
 0x00D0,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x00D7,
 0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x00DD,0x00DE,0x00DF,
 0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,
 0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
 0x00F0,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x00F7,
 0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x00FD,0x00FE,0x00FF
};

/* Representation of date & time */
typedef enum {
    dtUnknown_els00 = -1,
    dtFreeMask_els00,
    dtISO_els00,
    dtUSA_els00,
    dtEUR_els00,
    dtJIS_els00,
    dtInternal_els00
} tls00_DateTime;

static char const *pszDateTimeName_ls00[6] = { " FREEMASK ",   " ISO ",   " USA ",   " EUR ",   " JIS ",   " INTERNAL " };

/* Errorcodes */
typedef enum {
    errOK_els00,
    errNoMemory_els00,
    errInitPckt_els00,
    errInitSegm_els00,
    errStr2Part_els00,
    errChr2Part_els00,
    errBuf2Part_els00,
    errFromPart_els00,
    errConvNumber_els00,
    errMoveRange_els00,
    errYaccAlloc_els00,
    errParseSyntax_els00,
    errFastLoad_els00,
    errVFClose_els00
} tls00_ErrorCodes;


typedef enum
{
    tptUnknown_els00 = -1,
    tptHeader_els00,
    tptData_els00,
    tptTableDescription_els00,
    tptDefault_els00,
    tptConstraint_els00,
    tptIndexDescription_els00,
    tptIndex_els00,
    tptLongString_els00,
    tptShortString_els00,
    tptEOF_els00,
    tptEOT_els00
} tls00_TEPageType;


typedef enum
{
    dbuUnknown_els00 = 0,
    dbuSysdba_els00,
    dbuDba_els00,
    dbuResource_els00,
    dbuColdUser_els00
} tls00_DBUserMode;

/* External data types */
typedef enum {
    ioChar_els00,
    ioDecimal_els00,
    ioZoned_els00,
    ioInteger_els00,
    ioReal_els00,
    ioOracleNumber_els00,
    ioOracleDate_els00
} tls00_InOutType;

/* Boolean Operators */
typedef enum
{
    boUnknown_els00,
    boAND_els00,
    boOR_els00,
    boNOT_els00
} tls00_BoolOpType;

/* Numerical Operators */
typedef enum {
    opUnknown_els00,
    opEqual_els00,
    opSmaller_els00,
    opLarger_els00,
    opSmallerEqual_els00,
    opLargerEqual_els00,
    opNotEqual_els00
} tls00_OpType;

/* Simple column condition */
typedef struct tls00_Condition
{
    tsp00_Int2      conStart_ls00;
    tsp00_Int2      conEnd_ls00;
    tls00_InOutType conFormat_ls00;
    bool            conIsHex_ls00;
    tsp00_Int2      conFrac_ls00;
    void            *conBinCompVal_ls00;
    tsp00_Addr      conCompVal_ls00;
    tsp00_Int4      conValLen_ls00;
    tls00_OpType    conCompOp_ls00;
    tsp00_Int2      conColNo_ls00;
} tls00_Condition;

/* Node of complex column condition */
typedef struct tls00_CondSpec
{
    tls00_BoolOpType    cosBoolOp_ls00;
    bool                cosOtherwise_ls00;
    bool                cosDefNull_ls00;
    bool                cosCondValue_ls00;      
    tls00_Condition*    cosCond_ls00;
    tls00_CondSpec*     cosLeft_ls00;
    tls00_CondSpec*     cosRight_ls00;
} tls00_CondSpec;

typedef enum
{
    stUnknown_els00,
    stConfiguration_els00,
    stCatalog_els00,
	stData_els00,
    stPackage_els00
} tls00_StreamType;

typedef enum
{
    epUnknown_els00,
    epSSL_els00
} tls00_EncryptionProtocol;

static char const *pszEncryptionProtName_ls00[2] = {"NONE" , "SSL"};

/* Packagepart Spec */
typedef enum
{
    ptUnknown_els00,
    ptColumn_els00,
    ptTable_els00,
	ptSchema_els00,
    ptUser_els00,
    ptAll_els00
} tls00_PartType;

static char const *pszPartTypeName_ls00[6]     = { "NONE" ,   "COLUMN" ,  "TABLE",    "SCHEMA"     , "USER"             , "ALL" };
static char const *pszPartTypeValue_ls00[6]    = {"'NONE'",  "'COLUMN'", "'TABLE'",  "'SCHEMA'"    ,"'USER'"            , "'ALL'"};
static char const *pszPackageOwner_ls00[6]     = { " "    ,  " "       ,   " "    , " "            ," "                 , "\"OWNER\"      1 "};
static char const *pszPackageSchema_ls00[6]    = { " "    ,  " "       ,   " "    , " "            ,"\"SCHEMANAME\" 1 " , "\"SCHEMANAME\" 1 "};

typedef struct tls00_PartSpec
{
	tls00_PartType           psPartType_ls00;
	tsp00_Addr               psFullTabName_ls00;
	tsp00_KnlIdentifierc     psTabOwner_ls00;
	tsp00_KnlIdentifierc     psTabSchema_ls00;
    tsp00_KnlIdentifierc     psTabName_ls00;
} tls00_PartSpec;

/* Tablepart Spec */
typedef struct tls00_TableSpec
{
	tsp00_Addr            tsFullTabName_ls00;
    tsp00_KnlIdentifierc  tsTabOwner_ls00;
    tsp00_KnlIdentifierc  tsTabSchema_ls00;
	tsp00_KnlIdentifierc  tsTabName_ls00;
    tls00_CondSpec       *tsCondSpec_ls00;
} tls00_TableSpec;


/* Some more often used structures */
struct tls00_Performance
{
    SAPDB_UInt8             perRequest_ls00 ;
    SAPDB_UInt8             perReceive_ls00 ;
    SAPDB_UInt8             perIO_ls00;
    SAPDB_UInt8             perLog_ls00;
};

typedef struct tls00_Reconnect
{
    SAPDB_UInt8             rcoTimeOut_ls00;
    SAPDB_UInt8             rcoLastSuccessfulKnlContact_ls00;
    SAPDB_UInt8             rcoThreshold_ls00;
    SAPDB_Bool              rcoDoReconnect_ls00;
} tls00_Reconnect;


typedef struct tls00_String
{
    tsp00_Addr              strAddr_ls00;       // char pointer
    tsp00_Int4              strLeng_ls00;
}tls00_String;


typedef struct tls00_FieldBasics
{
    tsp00_DataType          fbDataType_ls00;        // sp7fi_data_type
    teo00_Int1              fbFraction_ls00;        // sp7fi_frac           DER TYP MUSS BLEIBEN, DA SIGNED NOTWENDIG
    tsp00_Int2              fbLength_ls00;          // sp7fi_len
    tsp00_Int2              fbInOutLength_ls00;     // sp7fi_in_out_l
} tls00_FieldBasics;

typedef struct tls00_FieldInfo              //tsp7_ffield_info
{
    tsp00_Int2          fiStmtColNo_ls00;       // sp7fi_stmt_col_no
    tls00_FieldBasics   fiBasics_ls00;
    tsp00_Int2          fiDefColNo_ls00;        // sp7fi_def_col_no
    tsp00_ColMode       fiColMode_ls00;         // sp7fi_col_mode
    tsp7_len_type       fiLengthType_ls00;      // sp7fi_len_type
    tsp7_default        fiDefault_ls00;         // sp7fi_default
    tsp00_C3            fiFiller_ls00;          // sp7fi_filler
} tls00_FieldInfo;


// hervorgegangen aus tsp1_param_info !! Bei aenderungen an der kernstruktur muss diese struktur also
// angepasst werden !!
typedef struct tls00_ParamInfo
{
    tsp1_param_opt_set  piMode_ls00;          // bit combination: mandatory, optional, default, escape_char,
    tsp1_param_io_type  piIoType_ls00;       // enum type: input, output, inout,
    tls00_FieldBasics   piBasicInfo_ls00;
    tsp00_Int4          piBufpos_ls00;
} tls00_ParamInfo;


// Die folgende struktur ist eine nachbildung der struktur tsp7_table_description aus vsp007.
// Dabei wurde allerdings auf die union verzichtet und die struktur tsp7_table_description_false
// nicht genutzt, da damit sonst sofort 80k speicher belegt werden wuerden.
// Ausserdem ist zusaetzlich eine struktur aufgenommen worden, die die spaltennamen der
// tabelle enthalten kann. Diese muss aber von der jeweiligen funktionalitaet nach eigenen
// erfordernissen gefuellt werden.

typedef struct tls00_TableDescription
{
    tsp00_KnlIdentifierc    tdAuthid_ls00;
    tsp00_KnlIdentifierc    tdTablename_ls00;

    tsp00_KnlIdentifierc   *tdColName_ls00[csp_max_fields];    // array of pointers

    tsp00_Int2              tdFieldCount_ls00;          // sp7t_field_cnt_F
    tsp00_SwapKind          tdKernelSwap_ls00;          // sp7t_kernel_swap_F
    tsp00_Uint1             tdFiller_ls00;              // sp7t_filler_F
    tsp00_Int2              tdKernelLoc_ls00;           // sp7t_kernel_loc_F
    tsp00_Int2              tdConstraintCount_ls00;     // sp7t_constraint_cnt_F

    union
    {
        tls00_FieldInfo     *tdFieldsArray_ls00[csp_max_fields];        // sp7t_fields_F    csp_max_fields = 1024
        tls00_ParamInfo     *tdParamArray_ls00[csp_max_fields];
    };
} tls00_TableDescription;


typedef struct tls00_Parts4InternalSql
{
    tsp00_Int2                  pfiCount_ls00;
    tsp1_part_kind_Enum*        pfiPartKind_ls00;
    tls00_String*               pfiData_ls00;
} tls00_Parts4InternalSql;


typedef struct tls00_BDInfo
{
    SAPDB_Int4                  bdiLength_ls00;
    SAPDB_Char                  szbdiData_ls00[128];         // being on the safe site with 128 Bytes?!
}
tls00_BDInfo;


typedef enum
{
    btTypeUnknown_els00,
    btTypeADSM_ls00,
    btTypeBACK_ls00,
    btTypeNSR_ls00

} tls00_BackupTool;

static char const *pszBackupToolName_ls00 [4]  = {  "NONE",    "ADSM",    "BACK",    "NSR"  };
static char const *pszBackupToolValue_ls00[4]  = {" 'NONE' "," 'ADSM' "," 'BACK' "," 'NSR' "};



/***************************************************************************************/
typedef struct tls00_DBSqlCmd
{
    tsp00_Addr  dscSqlStmt_ls00;
    tsp00_Int4  dscStmtLen_ls00;
} tls00_DBSqlCmd;

/* Structure for LOAD-SESSION context */
typedef struct tls00_SessionInfo
{
    // db connection attributes
    char*                       si_pDBNode_ls00;        // 64 byte character
    tsp00_DbNamec               siDBName_ls00;          // 18 byte character null terminated
    tsp00_KnlIdentifierc        siUserName_ls00;        // 18 byte (terminated 0) character array
    tsp00_CryptPw               siUserCPwd_ls00;        // 24 byte character
    tsp00_Namec                 siUserPwd_ls00;         
    tsp00_KnlIdentifier         siUserGroup_ls00;       // 18 byte character; group user above belongs to
    tsp00_Addr                  siUserKey_ls00;
	char                        siSchemaName_ls00   [KNL_IDENTIFIER_SIZE];  
    char                        siSchemaMapName_ls00[KNL_IDENTIFIER_SIZE];  

    bool                        siDBConnection_ls00;    // db connection established?
    bool                        siSqlStat_ls00;
    bool                        siAuthentication_ls00;
    tls00_EncryptionProtocol    siEncryptionProtocol_ls00;
    tls00_Reconnect             siReconnect_ls00;

    tsp1_sqlmode_Enum           siSQLMode_ls00;
    tsp00_Int2                  siDBSessionIsoLevel_ls00;
    bool                        siNolog_ls00;
    bool                        siAutoCommit_ls00;
    bool                        siMessageOn_ls00;
    bool                        siInstallOn_ls00;
    bool                        siODBCTrace_ls00;
    bool                        siCheckConstraint_ls00;
    bool                        siSystemAttribute_ls00;

    tls00_ConfigurationType     siConfigurationType_ls00;
    tls00_BackupTool            siBackupTool_ls00;

    // codeset conversions attributes
    tsp00_Ctable                siConversionTables_ls00[6];
    tls00_CodeType              siCodeType_ls00;
    tsp81_CodePage              siCodePage_ls00;

    tls00_DateTime              siFormat_ls00;
    tls00_DateTime              siDateType_ls00; 
    tls00_DateTime              siTimeType_ls00; 
    tls00_DateTime              siStampType_ls00; 
    char                        siThsdChar_ls00;
    char                        siDecPoint_ls00;
    char                        siSeperat_ls00[2];      // 1 byte sign and the null terminator
    char                        siDelimit_ls00[2];      // 1 byte sign and the null terminator

    tsp00_C20c                  siNullSpec_ls00;
    
    // default value for bool-values,  10 bytes long + terminating 0
    char                        siTrueVal_ls00[MAX_BOOLVALUE_LENGTH_LS00 + 1];
    char                        siFalseVal_ls00[MAX_BOOLVALUE_LENGTH_LS00 + 1];
    tsp00_C3                    siLanguage_ls00;
    tsp00_C1                    siFiller_ls00;

    tsp00_Int4                  siTASize_ls00;          // transaction size may be given by the user
    tsp00_Int4                  siMaxAdmissibleErrors;

    bool*                       sipCancelByte_ls00;
} tls00_SessionInfo;

/* Structure for DATABASE-SESSION context */
typedef struct tls00_DBInfo
{
    tls00_String                dbiCommand_ls00;    // command to be continued
    tsp00_C3                    dbiFiller_ls00;
    tsp00_TermId                dbiSqlTerm_ls00;       // 18 byte character
    tsp01_Service               dbiSqlService_ls00;    // enum type: sql_user, sql_async_user, sql_utility, sqlDistribution_esp00
    tsp01_CommErr               dbiSqlRc_ls00;         // signed char (vsp001c.h), Communication Error
    tsp00_SwapKind              dbiOwnSwap_ls00;       // swap behavior (little endian, big endian); enum type: sw_dummy, sw_normal, sw_full_swapped, sw_part_swapped,
    tsp00_Int2                  dbiPktCount_ls00;      // Number of packets
    tsp00_Int4                  dbiPktRc_ls00;         // Return code for sending packet
    tls00_CodeType              dbiEncoding_ls00;      // encoding of db server connected to (unicode, ascii)
    tls00_CodeType              dbiOI_Encoding_ls00;
    
    tsp100_VersionID1           dbiDBVersion_ls00;
    tsp00_KnlIdentifierc        dbiSYSDBA_Name_ls00;
    tsp00_Int4                  dbiPageSize_ls00;
    tgg00_DateTimeFormat_Enum   dbiDateTimeFormat_ls00;

    //oi connection
    tls00_SessionInfo           dbiSession_ls00;       // see structure above
    tsp00_Int4                  dbiPktSize_ls00;       // Size of message packet
    tsp1_packet_list            dbiPktSndList_ls00;    // Pointer to tsp1_packet
    tsp1_packet_list            dbiPktRcvList_ls00;    // Pointer to tsp1_packet
    tsp00_ErrText               dbiSqlErr_ls00;        // 40 byte character error text
    tsp00_Int4                  dbiSqlRef_ls00;
    tls00_PktBlock              dbiPktInitBlock_ls00;  // packet-, segment- and part header at once;  see above for declaration
    tsp00_Int2                  dbiLoaderSessionIsoLevel_ls00;

    //odbc connection
    sapdbwa_DBCPool *           dbiOdbcConnectionPool;
    sapdbwa_DBC                 dbiOdbcConnection; 

} tls00_DBInfo;


/* Structure for CLIENT/SERVER communication */
typedef struct tls00_Command
{
    tsp00_Addr                  cmdReqDat_ls00;
    tsp00_Int4                  cmdReqPos_ls00;
    tsp00_Int4                  cmdReqErr_ls00;

    tsp00_Addr                  cmdScnDat_ls00;
    tsp00_Addr                  cmdRpyDat_ls00;         // wird in ls01-modul dynamisch mit 1024 byte allokiert und an die
                                                // funktionen weitergereicht
    tls00_ScanResult            cmdScanRC_ls00;
    tls00_SubFuncClass          cmdSubFunc_ls00;
    bool                        cmdInternal_ls00;
    bool                        cmdLogging_ls00;

    tsp00_C2                    cmdFiller_ls00;
} tls00_Command;

typedef struct tls00_Buffer
{
    // size values correspond to the values buf len and buf size in part header of the order interface
    void*                       pBufPtr_ls00;
    tsp00_Int4                  lBufSize_ls00;     // specifies the still available count of bytes in buffer - on creation time == the whole buffer
    tsp00_Int4                  lBufLen_ls00;      // specifies count of used bytes in buffer - on creation time = 0
} tls00_Buffer;

typedef struct tls00_Buffer2
{
    void*                       pBufPtr_ls00;
    tsp00_Longint               lBufLen_ls00;      // buffer length, fix - determined when creating pBufPtr
    tsp00_Longint               lBufPos_ls00;      // used to navigate through the buffer
} tls00_Buffer2;

typedef struct tls00_LargeNumber
{
    tsp00_Longint               lnu2GB_Blocks_ls00;
    tsp00_Longint               lnuRemainder_ls00;
} tls00_LargeNumber;

typedef enum
{
    dtypeUnknown_els00,
    dtypeFile_ls00,
    dtypePipe_ls00,
    dtypeTape_ls00

} tls00_DeviceType;

typedef enum
{
    butTypeUnknown_els00,
    butTypeEBID_ls00
} tls00_BackupId;

typedef struct tls00_BackupIdName
{
    tls00_BackupId   bBackupId_ls00;
    tsp00_Addr       bBackupIdName_ls00;
} tls00_BackupIdName;

static char const *pszDeviceTypeName_ls00[4]   = { " FILE " , " FILE " , " PIPE ",  " TAPE " };
static char const *pszDeviceTypeValue_ls00[4]  = {" 'FILE' "," 'FILE' "," 'PIPE' "," 'TAPE' "};


/* Structure for FILE access */
typedef struct tls00_VFile
{
    tsp00_Addr               vfFileName_ls00;               // null terminated string
    Tools_DynamicUTF8String  vfName_ls00;
    Tools_DynamicUTF8String  vfPathName_ls00;
    tsp00_Int4               vfFileHndl_ls00;
    tls00_DeviceType         vfFileType_ls00;
    tls00_CodeType           vfFileEncoding_ls00;           // possible values: UCS2, ASCII, UTF8
    tls00_Buffer2            vfMagicBytesCache_ls00;

    BackupControllerFactory  vfBackupControllerFactory;     // stream medium tape
    BackupController        *vfpBackupController_ls00;
    tls00_BackupTool         vfBackupTool_ls00;
    Tools_DynamicUTF8String  vfNodeName_ls00;
    Tools_DynamicUTF8String  vfDBName_ls00;
    Tools_DynamicUTF8String  vfBackupId_ls00;
    tsp05_RteDataKind        vfDataType_ls00;               // stream format binary
    tsp05_RteFileMode        vfFileMode_ls00;
    tsp05_RteBufferingKind   vfFileBuf_ls00;
    tsp05_RteCloseOption     vfFileCOpt_ls00;
    tls00_LargeNumber        vfFilePos_ls00;
    tls00_LargeNumber        vfFileLen_ls00;                // in Bytes
    tsp00_Int8               vfFileLineNum_ls00;
} tls00_VFile;


typedef struct tls00_Restart
{
    Tools_DynamicUTF8String     rstPackageid_ls00;     // guid char byte 24 encoded in utf8
    tls00_VFile*                rstRestartFile_ls00;
    tsp00_Longint               rstRestartFilePos_ls00;
    tsp00_Longint               rstRestartFileLen_ls00;
    tsp00_Longint               rstRestartCommRow_ls00;
    tsp00_Addr                  rstRestartDataFileName_ls00;
    tsp00_Addr                  rstRestartEntry_ls00;
    tls00_LargeNumber           rstRestartPos_ls00;
    bool                        rstRestart_ls00;
} tls00_Restart;


/* Special structure for predefined default values. Used in FastLoadRecord structure
   and probably in other load structure. These defaults are delivered by the
   tls00_SessionInfo Structure. */
typedef struct tls00_PredefDefault
{
    tsp00_Addr                  pdUserDefault;
    tsp00_Addr                  pdUserGroupDefault;
    tsp00_Number                pdOracleUid_ls00;
    tsp00_C9                    pdStamp_ls00;      // stamp length is actually 8 bytes + the undef/def byte, contains last inserted stamp value
    tsp00_C6                    pdFiller1_ls00;
    tsp00_C1                    pdFiller2_ls00;
} tls00_PredefDefault;

//*
//*     Structure used ONLY for fastload. Contains information about
//*     the page layout necessary to know if we want to fill the page.
//*
typedef struct tls00_PageInfo
{
    tsp00_Int4                  pgiDataStartPos;
    tsp00_Int4                  pgiDataEndPos;
    tsp00_Int4                  pgiDataSpace;
} tls00_PageInfo;

typedef struct tls00_ParamInfo_LONG       // LONG Paramterinfo (verwendet bei dataload und dataextract)
{
    bool                        loiIsLong_ls00;          // TRUE: IS LONG COLUMN
    bool                        loiWithLongInfo_ls00;    // TRUE: Der Spalte wurde ein LONGFILE Parameter zugeordnet
    bool                        loiPart_ls00;            // bool, flagging if long is part of file or the whole file
    bool                        loibFileNumbered_ls00;   // nur beim Dataextract genutzt: wird die longfiledatei durchnumeriert?
    tsp00_Int2                  loiNumberSize_ls00;      // nur beim Dataextract genutzt: wieviel ziffern beim durchnumerieren
                                                         // bzw. fuer positionsangaben verfuegbar
    tsp00_Int4                  loiGreatestNum_ls00;     // nur bei DE, enthaelt groesste moegliche zahl fuer durchnumerierte datei

    tsp00_Addr                  loiLongFileName_ls00;    // Command-Filename 

    tsp00_Addr                  loiFileName_ls00;        // Inputfilename 
    tsp00_Longint               loiLength_ls00;          // 0: Laenge unbekannt, d.h. gesamter Fileinhalt ist signifikant 
    tls00_LargeNumber           loiFileOffset_ls00;      // File offset of the long data in the used long data file
    tsp00_Longint               loiCountOfSendData_ls00; // Anzahl bereits gesendeter Daten an den SQL Server

    tls00_VFile*                loiFileStruct_ls00;      // IO Fileinfo's 
    tls00_CodeType              loiFileEncoding_ls00;
} tls00_ParamInfo_LONG;


typedef struct tls00_DataSource 
{
    tls00_Buffer2               dasInputBuffer;
    tls00_String*               dasConvertedData;
    tsp00_Uint4                 lReadFieldsCnt_ls00;
    tsp00_Uint4*                lFillIndex_ls00;
    tsp00_Uint4*                plDataToColIndex_ls00;
    tsp00_Uint4*                plDataToParamIndex_ls00;
    tsp00_Uint4*                lReadPos_ls00;

    tls00_CodeType              dasCodeType_ls00;
    tsp81_CodePage*             pdasCodePage_ls00;
} tls00_DataSource;


// pattern is build from separators and delimiters; single separators do
// not happen but single delimiters must be handled; this is why we have
// patDelimit here
typedef struct tls00_Pattern
{
    tsp00_Addr          ppatPattern_ls00;
    tsp00_Longint       lpatPatternLen_ls00;
    tsp00_Addr          ppatDelimit_ls00;
    tsp00_Longint       lpatDelimiterLen_ls00;
    tsp00_Int4         *plpatFailure_ls00;
} tls00_Pattern;


typedef struct tls00_ColumnData
{
    tsp00_Longint       cdPosInStmt_ls00;
    tsp00_Addr          cdPtrToData_ls00;
    tsp00_Addr          cdDataBuffer_ls00;
    tsp00_Longint       cdDataBufferLen_ls00;
    tsp00_Longint       cdDataLen_ls00;
    tsp00_Longint       cdLONGLen_ls00;             // length of LONG value if column is of type long
    bool                cdPtrUsed_ls00;
    tsp00_DataType      cdSQLDataType_ls00;
    tsp00_Longint       cdSQLLen_ls00;
    tls00_InOutType     cdExtDataType_ls00;
    tls00_CodeType      cdExtEncoding_ls00;
    tls00_CodeType      cdExtLONGEncoding_ls00;
    bool                cdExtHexRepresentation_ls00;
    bool                cdIsNILValue_ls00;
    tsp00_C1            cdFiller_ls00;
} tls00_ColumnData;


typedef struct tls00_NullValueCounter                       // introduced with http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1138384
{
    SAPDB_Bool              nvcCount_ls00;
    SAPDB_Int8              nvcCounter_ls00;
    SAPDB_Bool*             nvcColumnAllowsNull_ls00;       // The following both are for FASTLOAD only!
    tls00_CondSpec*         nvcAuxConditionSpec_ls00;
} tls00_NullValueCounter;


typedef struct tls00_MetaData
{
    tls00_VFile          *pmdMetaFile_ls00;          // not used; data file itself contains meta data 
    tsp00_Uint4           mdFieldCount_ls00;         // Count of columns in data file
    tsp00_Uint4           mdLongFieldCount_ls00;     // Count of columns of type long in data file
    tsp00_Uint4          *pmdLongFieldPos_ls00;       // array of positions of columns of type long in a single record
    tsp00_Int2           *pmdLengthArray_ls00;       // used to read meta data from file (length info of the data in file); may be negative to indicate a NULL value
    tsp00_Uint4           mdLengthArrayLen_ls00;     // length of length array
    tsp00_Uint2           mdLengthArrayPos_ls00;     // not used
    tsp00_Uint4           mdRecordLength_ls00;
    tls00_LargeNumber     mdPosOfNextLengthArray_ls00;  // used for data with LONGs in case of error and MAXERRORCOUNT > 1
    tsp00_SwapKind_Enum   mdSwapKind_ls00;           // swap kind of data in data file
} tls00_MetaData;

//*
//*     Structure for FASTLOAD execution --> FastLoadRecord
//*
typedef struct tls00_FloadRec
{
    bool                     flrTableSwitched_ls00;
    tls00_TableDescription   flrTableInfo_ls00;
    
    tls00_VFile             *flrpInfile_ls00;
    
    tls00_DataSource         flrDataSource_ls00;
    tls00_Pattern            flrPattern_ls00;
    
    tls00_BDInfo             flrBdInfo_ls00;

    tsp00_Key                flrLstKeyAcp_ls00;      /* Last key accepted */
    tsp00_Int2               flrLenKeyAcp_ls00;
    
    //* Special treatment for known default values. Delivered by tls00_SessionInfo
    tls00_PredefDefault      flrDefStruct_ls00;
    tls00_String            *flrDefValue_ls00[csp_max_fields];  // array of tls00_String pointers

    tsp00_Int2               flrPgPerPckt_ls00;          /* number of possible pages per packet */
    tsp00_Int2               flrRecordLen_ls00;          /* length of one record in a table */
    tsp00_Int2               flrFixColLen_ls00;          /* length of all fix columns together */
    tsp00_Int2               flrVarColCnt_ls00;          /* number of columns with variable length */
    tsp00_Int2               flrDefColCnt_ls00;          /* number of columns given in the fastload statement,
                                                       if no columns are given ALL columns will be used */
    tls00_PageInfo           flrPageInfo_ls00;

    //*
    //* The following 2 values are used to summarize the number of pages and the number of records loaded
    //* during one fastload run into a dedicated table. This information is used in the ENDLOAD command
    //* after loading the pages has successfully finished
    //*
    tsp00_Int4               flrPageCount_ls00;      // count of built pages sent to kernel; limited by kernel to MAX_INT4_SP00
    tsp00_Int8               flrRecordCount_ls00;    // count of processed records
    
    tsp00_Int8               flrLastCommLine_ls00;   // last committed input line
    tsp00_Int4               flrLRejected_ls00;      // count of rejected records
    tls00_NullValueCounter   flrNullValueCounter_ls00;

    tsp00_SwapKind           flrSwapKind_ls00;
    bool                     flrIsSyskey_ls00;       // syskey column (if no user defined key exists)
    bool                     flrStampDef_ls00;

    bool                     flrTabEmpty_ls00;
    bool                     flrTabLoaded_ls00;      // indicates fullnes   of table
} tls00_FloadRec;



typedef struct tls00_LongExecInfo
{
    tsp00_Int2              *dliNNLongs_ls00;               // array of long column indexes in single record which are not null
    tsp00_Int2               dliCntNNLongs_ls00;            // Count of not null long cols in single record
    tsp00_Int2               dliFinishedNNLongs_ls00;       // Count of not null long cols in single records already processed
    tls00_Buffer2            dliConversionBuffer_ls00;      // Used (as a workaround) for converting LONG values between
                                                            //  different encodings and hexadecimal representation while
                                                            //  inserting
} tls00_LongExecInfo;


typedef enum
{
    tsUnknown_ls00,
    tsExport_ls00,
    tsImport_ls00,
    tsTransport_ls00,
    tsUpdate_ls00
} tls00_TransSpec;

static char const *pszTransSpecValue_ls00[5] = {"'NONE'", "'EXPORT'", "'IMPORT'", "'TRANSPORT'","'UPDATE'"};

//*
//*     Structure for DATALOAD execution --> DataLoadRecord
//*

/* Modi for dataload */
typedef enum
{
    dupUnknown_els00,
    dupIgnore_els00,
    dupReject_els00,
    dupUpdate_els00
} tls00_Duplicate;


typedef struct tls00_OI_DataPartCache
{
    SAPDB_Int4                  dpc_RecordLength_ls00;
    tls00_Buffer                dpc_Buffer_ls00;
    SAPDB_Int4                  dpc_lRowCount_ls00;

    // free space management and recovery in case of error in order interface buffer
    SAPDB_Int4*                 dpc_RecordNumbers_ls00;         // record numbers in mass insert/update OI packet buffer
    SAPDB_Int4*                 dpc_RecordSpace_ls00;           // position/space of records in mass insert/update OI packet buffer
} tls00_OI_DataPartCache;


typedef struct tls00_AlternatingBuffers
{
    SAPDB_Int4                  ab_lSndPcktNo_ls00;
    SAPDB_Int4                  ab_lRcvPcktNo_ls00;
    SAPDB_UInt4                 ab_LastSuccessful_ls00;                         // Last successfully sent record no
    tls00_OI_DataPartCache*     ab_pCache_ls00;                                 // the real cache
} tls00_AlternatingBuffers;


typedef struct tls00_TransformationRTE
{
    // databaseInfo
    tsp00_SwapKind           rte_SwapKind_ls00;

    // data source 
    tls00_VFile             *rte_pDataFile_ls00;
    tls00_Pattern            rte_Pattern_ls00;          // defines data in data file
    
    tls00_DataSource         rte_DataSource_ls00;       // necessary for reading from data file; mostly kind of cache

    // table data: describe processed table
    tls00_TableDescription   rte_TableInfo_ls00;        // sp1pk_shortinfo
    SAPDB_Int4               rte_LongColCnt_ls00;       // count of LONGs in table to process
    
    // variables used for communication with kernel:
    // primary data part
    char                     rte_ParseId_ls00[MAX_PARSEID_LENGTH_LS00 + 1];    // sp1pk_parsid
    char*                    rte_pStmtParsed_ls00;
    unsigned char            rte_ResultCount_ls00[sizeof(csp_rescnt_unknown)]; // sp1pk_resultcount
    tsp1_part_attributes     rte_PartAttr_ls00;                                // sp1pa_first_packet, sp1pa_next_packet, sp1pa_last_packet
    SAPDB_Int4               rte_MaxRecordsInPacket_ls00;                      // max number of rows in OI packet frueher: MaxRowsPPckt

    // variables controlling processing of LONG values : 
    // long data part
    tls00_LongExecInfo       rte_LongExecInfo_ls00;
    tsp00_LongDescriptor    *rte_pLongDescriptorContainer_ls00;

    //*
    //* variables controlling processing :
    //*
    // orderInterface header block; reused for every packet
    tls00_String             rte_OI_Reuse_PacketHeader_ls00;    // Consists of packet-, segment-header, parseid-part,
                                                                // resultcount-part, data-part-header to init a mass insert
                                                                // packet quickly
    // buffer management
    tls00_AlternatingBuffers rte_OI_DataPart_Cache_ls00;

    // logging and recovery
    SAPDB_Int4              rte_Cnt_ProcessedRecords_ls00;    // number of rows
    SAPDB_Int4              rte_Cnt_CommittedRecords_ls00;    // number of successfully sent (updated/inserted) rows
    
    SAPDB_Int4              rte_LastCommLine_ls00;            // last commited input line
    SAPDB_Int4              rte_Cnt_RejectedRecords_ls00;     // number of rejected rows
    //*
    //* special variables only for DATALOAD
    //*
    tls00_PartSpec           rte_PartSpec_ls00;
    bool                     rte_TableSwitched_ls00;

    tls00_NullValueCounter   rte_NullValueCounter_ls00;         // temporarily; used during import into single table to determine whether null values
                                                                // have been inserted if no columns were specified in the command

    // this structure is only used to process data in external format RECORDS
    tls00_MetaData           rte_MetaData_ls00;

    bool                     rte_TabEmpty_ls00;       // indicates that table is empty
    bool                     rte_TabLoaded_ls00;      // indicates that table is completely loaded
    bool                     rte_bRestart_ls00;
}
tls00_TransformationRTE;



//*
//*     Structure for DATAEXTRACT execution --> DataExtractRecord
//*

/* Modi for dataextract */
typedef enum
{
    resUnknown_els00,
    resDataload_els00,
    resFastload_els00,
    resDataUpdt_els00,
    resDataloadNoDDL_els00,
    resFastloadNoDDL_els00,
    resDataloadNoDATA_els00,
    resFastloadNoDATA_els00,
    resTableload_els00,
    resTableFastload_els00,
    resTableUpdt_els00
} tls00_Restore;

typedef struct tls27_DataSink
{
    tls00_Buffer2    dskMetaLine_ls27;
    tls00_Buffer2    dskOutLine_ls27;
} tls27_DataSink;


typedef struct tls00_DextractRec
{
    // session
    tls00_PredefDefault       derDefStruct_ls00;
    tsp00_SwapKind            derSwapKind_ls00;

    //tableextract all/user/table records 
    tls00_PartSpec            derPartSpec_ls00;
    tls00_Restore             derRestore_ls00;

    tsp00_Int4                lNameCount_ls00;               // used for allocation of char-resources
    SAPDB_Bool                derUserSwitched_ls00;
    bool                      derTableSwitched_ls00;
    tsp00_Longint             derExtractedRows_ls00;         // number of extracted rows
    tsp00_Longint             derLastCommLine_ls00;          // last committed extracted line
    tsp00_Longint             derLoadedRows_ls00;

    // transformation file
    tls00_VFile              *derCmdFile_ls00;               // command file 
    tls00_VFile              *derDataFile_ls00;              // data file

    // table data
    tls00_TableDescription    derTableInfo_ls00;

    tls00_DataSource          derDataSource_ls00;
    tls27_DataSink            derDataSink_ls00;
    tls00_Pattern            *pderPattern_ls00;               // pattern structure used when extracting to external format COMPRESSED
    tls00_ColumnData         *pderColumnData_ls00;

    char                      derParseId_ls00[MAX_PARSEID_LENGTH_LS00 + 1];
    tsp00_Addr                derRTableName_ls00;         // result table name
    
    char                      derFillValue_ls00;
    tsp00_Int4              (*derExtLenFrac_ls00)[2];     // 2-dim-array; 1.wert = externe laenge, 2. wert = fraction part
    tls00_LongExecInfo        derLongInfo_ls00;           // info on longs processing/processed in a single record
    tsp00_LongDescriptor     *derLongDescArray_ls00;      // one-dim. array of long descriptors
    bool                      derUseGetvalPckt_ls00;      // indicates necessity of using getval packets to extract longs
    tsp00_Addr                derRawResultCount_ls00;     // result count in internal number representation
    tsp00_Int4                derResultCount_ls00;        // result count as integer value

    tsp00_Int2                derRecordLen_ls00;          // length of one record in a table
    tsp00_Int4                derMaxRowsPPckt_ls00;       // max. count of records in a single fetch packet
    tsp00_Int4                derRecordCount_ls00;        // count of extracted records

    tsp00_Int4                derErrNumInRecord_ls00;
    tsp00_Int4                derLRejected_ls00;
    tsp00_Int4                derLongColCnt_ls00;         // count of long columns in table to process
    
    // transformation table
    tls00_LargeNumber         derHeaderFilePos_ls00;
    tsp00_Int4                derStartPos_ls00;
    _TCHAR                    derpStartPos_ls00[256];

    tsp00_Longint             derMediumSize_ls00;
    tsp00_Int4                derMediumLongIntBlocks_ls00 ;
    
    tsp00_Int4                derNumTabInFile_ls00;
    bool                      derTabEmpty_ls00;              // indicates emptyness of table
    bool                      derTabExtracted_ls00;          // indicates fullnes   of table

} tls00_DextractRec;


//*
//*     Structure for TABLEEXTRACT execution --> TableExtractRecord
//*
typedef struct tls00_TextractRec
{
    // session
    tls00_BDInfo                terBdInfo_ls00;
    tls00_BDInfo                terLongBdInfo_ls00;

    tsp00_Int2                  terPgPerPckt_ls00;      // number of possible pages per packet
    tsp00_Int4                  terPageSize_ls00;
    tsp00_Int4                  terPageCount_ls00;

    tsp00_Int4                  terPageNo_ls00;

    SAPDB_Char*                 terpPageBuffer_ls00;

    //tableextract all/user/table pages 
    tls00_PartSpec              terPartSpec_ls00;
    tls00_Restore               terRestore_ls00;

    tsp00_Int4                  lNameCount_ls00;               
    SAPDB_Bool                  terUserSwitched_ls00;
    bool                        terTableSwitched_ls00;

    // transformation file
    tls00_VFile*                terOutFile_ls00;
    tsp00_Int4                  terFileCounter_ls00;

    // table data
    tsp00_Int2                  terConstraintCnt_ls00;
    tsp00_Int2                  terDefaultCnt_ls00;         // user defined defaults
    tsp00_Int2                  terLongColCnt_ls00;
    tls00_TEPageType            terPageTypeExtracting_ls00;
    tls00_LargeNumber           terMediumSize_ls00;
    tsp00_Int4                  terCount;                  // gibt an, wieviele datenseiten pro speichermedium genutzt werden sollen //TODOTODO: gehoert eigentlich in tabextractcmd-struktur und muss dann hier ausgewertet werden mit der folgenden variablen
    Tools_DynamicUTF8String     terExtrStmt_ls00;
  
    tsp00_Int4                  terNumTabInFile_ls00;
    
    // transformation table
    tls00_LargeNumber           terHeaderFilePos_ls00;

    bool                        terTabEmpty_ls00;
    bool                        terTabExtracted_ls00;
} tls00_TextractRec;


typedef struct tls00_TloadRec
{
    // session
    tsp00_SwapKind              tlrSwapFromDb_ls00;
    tsp00_SwapKind              tlrTargetSwap_ls00;     // swap of the target db kernel
    tsp00_SwapKind              tlrSwapFromTE_ls00;

    tls00_BDInfo                tlrBdInfo_ls00;
    tls00_BDInfo                tlrLongBdInfo_ls00;

    tsp00_Int2                  tlrPgPerPckt_ls00;      // number of possible pages per packet to send
    tsp00_Int4                  tlrPageSize_ls00;
    tsp00_Int4                  tlrPageCount_ls00;

    tsp00_Int4                  tlrSndPcktNo_ls00;      // sent packet number
    tsp00_Int4                  tlrRcvPcktNo_ls00;      // received packet number

    tsp00_Int4                  tlrPageNo_ls00;

    //tableload all/user/table pages
    tls00_PartSpec              tlrPartSpec_ls00;     

    bool                        tlrTableSwitched_ls00;

    // transformation file
    tls00_VFile*                tlrMappingfile_ls00;
    tls00_VFile*                tlrInfile_ls00;
    tls00_CodeType              tlrSourceEncoding_ls00;            // encoding of the source db; read from data file header
    
    tls00_Restart               tlrRestart_ls00;

    // table data
    tls00_String                tlrTableInfo_ls00;
    tls00_String                tlrExtrTableInfo_ls00;
    tls00_String                tlrExtrIndexInfo_ls00;
    tls00_TEPageType            tlrPageTypeExtracting_ls00;
    tsp00_Int4                  tlrCount_ls00;                  // gibt an, wieviele datenseiten pro speichermedium beim te genutzt wurden

    bool                        tlrMultTabInFile_ls00;

    bool                        tlrTabEmpty_ls00;       // indicates emptyness of table
    bool                        tlrTabLoaded_ls00;      // indicates fullnes   of table
} tls00_TloadRec;


// Special abstract types derived from the known datatypes
typedef enum
{
    atUnknown_els00,
    atNumber_els00,
    atSpecConstant_els00,
    atLong_els00
} tls00_AbstractType;

/* Column assign values */
typedef enum
{
    asUnknown_els00,
    asLiteral_els00,
    asSeqno_els00,
    asUser_els00,
    asUsergroup_els00,
    asStamp_els00,
    asDate_els00,
    asTime_els00,
    asTimeStamp_els00,
    asDB2Sqlid_els00,
    asDB2Date_els00,
    asDB2Time_els00,
    asDB2Timestamp_els00,
    asOraSysdate_els00,
    asOraUid_els00
} tls00_Assign;


/* TABLE SPECIFICATION: ****************************************************************/

/* Position of a column */
typedef struct tls00_FieldPos
{
    tsp00_Int2          fpoStart_ls00;
    tsp00_Int2          fpoEnd_ls00;
} tls00_ColField;


/* External format of a column */
typedef struct tls00_FieldFormat
{
    tls00_InOutType     ffoFormat_ls00;
    bool                ffoIsHex_ls00;
    tsp00_Int2          ffoFrac_ls00;
} tls00_FieldFormat;


/* Transforming options of a column */
typedef struct tls00_FieldOption
{
    bool       fopScale_ls00;
    bool       fopTrunc_ls00;
    bool       fopRound_ls00;
    bool       fopAnyOperation_ls00;     // To optimize some numeric operation during fastload
    tsp00_Int4 fopScVal_ls00;
    tsp00_Int4 fopRTVal_ls00;
} tls00_FieldOption;


/* Internal combination of 2 structures */
typedef struct tls00_FormatSpec
{
    tls00_FieldFormat   fspFormat_ls00;
    tls00_FieldOption   fspOption_ls00;
} tls00_FormatSpec;


/* Assign value definition of a column */
typedef struct tls00_FieldAsgn
{
    tsp00_Addr      fasAsgnVal_ls00;
    tsp00_Int2      fasAsgnLen_ls00;
    tls00_Assign    fasAsgnTyp_ls00;
    tsp00_C1        fasFiller_ls00;
    tsp00_Int4      fasSeqNo_ls00;
    tsp00_Int4      fasSeqInc_ls00;
} tls00_FieldAsgn;


/* Structure of column specification */
typedef struct tls00_Column
{
    tsp00_KnlIdentifierc colName_ls00;  /* 32/64 byte in ascii/unicode */
    tsp00_Int2           colNumber_ls00;
    bool                 colIsLit_ls00;
    bool                 colIsKey_ls00;
    bool                 colIsSet_ls00;
    tls00_AbstractType   colAbstractType_ls00;      // Enum --> 1 byte; wird in abh. vom db-datentyp gesetzt
    bool                 colIsWhere_ls00;
    tls00_FieldPos       colFPos_ls00;
    tls00_FieldFormat    colFormat_ls00;
    tls00_FieldOption    colOption_ls00;
    tls00_FieldAsgn      colAssign_ls00;
    tls00_CondSpec*      colNullCond_ls00;
    tls00_ParamInfo_LONG colLongInfo;
    tsp00_C2             colFiller_ls00;
} tls00_Column;

/* List of columns */
typedef struct tls00_MultColSpec
{
    tsp00_Int4          mcsCount_ls00;
    tls00_Column*       mcsColumn_ls00[csp_max_fields];
} tls00_MultColSpec;

typedef struct tls00_SchemaMap
{
    tsp00_KnlIdentifierc smSourceSchema_ls00;  /* 32/64 byte in ascii/unicode */
    tsp00_KnlIdentifierc smTargetSchema_ls00;  /* 32/64 byte in ascii/unicode */
} tls00_SchemaMap;

typedef struct tls00_MultSchemaSpec
{
    tsp00_Int4           msCount_ls00;
    tsp00_KnlIdentifierc msSchemaMapName_ls00;
    tls00_SchemaMap*     msSchemaMap_ls00[30];
} tls00_MultSchemaSpec;


/* FILE SPECIFICATION: *****************************************************************/


/* Some file option specifications ... */
typedef struct tls00_CodeSpec
{
    tls00_CodeType  cdsCodeType_ls00;
    tsp00_C1        cdsFiller_ls00;
} tls00_CodeSpec;

/* The following structure is only used in vls11.ycc for parsing */
typedef struct tls00_DateTimeSpec
{
    tsp00_Addr      dtsMask_ls00;
    tls00_DateTime  dtsType_ls00;
    tsp00_C3        dtsFiller_ls00;
    tin_date_mask   dtsSpecialDateMask_ls00;    // This mask is needed only to be able to use date conversion
                                                // functions in vin41 (vin32)
} tls00_DateTimeSpec;

typedef struct tls00_ComprSpec
{
    tsp00_C1        cpsSeperat_ls00;
    tsp00_C1        cpsDelimit_ls00;
    bool            cpsIsFormat_ls00;
    bool            cpsIsCompr_ls00;
} tls00_ComprSpec;


//*
//*     Only (ONLY) used for parsing the command. The values are delivered to 
//*     structure tls00_FileFormat for later use.
//*
typedef struct tls00_BoolSpec
{
    tsp00_Int2      bspTValLen_ls00;
    tsp00_Addr      bspTrueVal_ls00;

    tsp00_Int2      bspFValLen_ls00;
    tsp00_Addr      bspFalseVal_ls00;
} tls00_BoolSpec;

typedef struct tls00_Usage
{
    tsp00_Int4  uUsageValue_ls00;
    bool        uUsingRows_ls00;
    bool        uFastMode_ls00;
    bool        uKeySequence_ls00;
    bool        uNoLog_ls00;
    char        uFiller[3];
} tls00_Usage;


/* Structure of file format options */
/* CAUTION:
   This structure is saved in some data files (using tableextract ... records and should
   therefore not be changed as this would cause severe incompatibilities
*/
typedef struct tls00_FileFormat
{
    tls00_CodeType      ffoCodeType_ls00;       // Code type of the data file

    //*** date values given as in/outfile specification
    tls00_DateTime      ffoDateType_ls00;
    _TCHAR              ffoDateMask_ls00[128];           // null terminated string
    tin_date_mask       ffoSpecialDateMask_ls00;    // This mask is needed only to be able to use date conversion
                                                    // functions in vin41 (vin32)
    //*** time values given as in/outfile specification
    tls00_DateTime      ffoTimeType_ls00; 
    _TCHAR              ffoTimeMask_ls00[128];           // null terminated string
    tin_date_mask       ffoSpecialTimeMask_ls00;    // This mask is needed only to be able to use date conversion

    //*** time stamp values given as in/outfile specification
    tls00_DateTime      ffoTimeStampType_ls00; 
    _TCHAR              ffoTimeStampMask_ls00[128];          // null terminated string
    tin_date_mask       ffoSpecialTimeStampMask_ls00;   // This mask is needed only to be able to use date conversion

    //*** Decimal point specifier and 'thousand-separator' as in 1.000.000,00
    char                ffoThsdChar_ls00;
    char                ffoDecPoint_ls00;

    char                ffoSeperat_ls00[2];     // 1 byte sign and the null terminator
    char                ffoDelimit_ls00[2];     // 1 byte sign and the null terminator

    _TCHAR              ffoCodeSet_ls00[64];

    tsp00_Int2          ffoTValLen_ls00;        // boolean value TRUE - so far max 10 chars long
    _TCHAR              ffoTrueVal_ls00[11];

    tsp00_Int2          ffoFValLen_ls00;        // boolean value FALSE - so far max 10 chars long
    _TCHAR              ffoFalseVal_ls00[11];

    _TCHAR              ffoNullSpec_ls00[21];
    tsp00_Int4          ffoCntSpec_ls00; 

    tsp00_Int2          ffoCSetLen_ls00;
    tsp00_Int2          ffoNSpecLen_ls00;
    tsp00_SwapKind      ffoIntSpec_ls00; 

    //*** Values that describe the file format
    //*     The value ffoCompress_ls00 is so far only used for navigating within the
    //*     parser. Whithout that one could not decide what to do if COMPRESS and FORMATTED
    //*     are specified. In the program control flow only the value of ffoFormatted is
    //*     is used - if true the data is formatted, if false the data is compressed.
    bool                ffoBinary_ls00;         // angeschaltet liegt datei binaer vor, ansonsten in textformat
    bool                ffoFormatted_ls00;
    bool                ffoCompress_ls00;
    bool                ffoPage_ls00;     
    bool                ffoCompact_ls00;  
    bool                ffoDdl_ls00;
    bool                ffoDdlNoIndex_ls00;
    bool                ffoDtl_ls00;
    tls00_Usage         ffoUsage_ls00;
    bool                ffoXml_ls00;
    bool                ffoXmlNoTable_ls00;
    tsp00_Int4          ffoBytesPerLine_ls00;

} tls00_FileFormat;

typedef struct tls00_FileExtract
{
    tls00_LargeNumber feStart_ls00;
    tls00_LargeNumber feEnd_ls00;
} tls00_FileExtract;

/* File specification */
typedef struct tls00_FileSpec
{
    tsp00_Addr          fsFileName_ls00;        // null terminierter string
    tls00_FileFormat    fsFormat_ls00;
    tls00_DeviceType    fsDeviceType_ls00;
    tls00_BackupIdName  fsBackupId_ls00;
    tls00_FileExtract   fsExtract_ls00;
    bool                fsAppend_ls00;
    bool                fsFromOra_ls00;
    tsp00_Int2          fsLongColID_ls00;
    tsp00_Addr          fsLongColName_ls00;     // null terminierter string
    bool                fsCurrentStream_ls00;
} tls00_FileSpec;

typedef struct tls00_FileInfo
{
    tls00_VFile              vFile;
    tls00_FileSpec           fileSpec;
} tls00_FileInfo ;

/* List of file specifications */
typedef struct tls00_MultFileSpec
{
    tsp00_Int4      mfsCount_ls00;
    tls00_FileSpec* mfsFiles_ls00[csp_max_fields];
} tls00_MultFileSpec;


/* Loader Types ************************************************************/

typedef struct tls00_RemarkCmd   
{
    tsp00_Addr  dscRemark_ls00;
    tsp00_Int4  dscRemarkLen_ls00;
} tls00_RemarkCmd;

typedef struct tls00_TransformationCmd
{
    Tools_DynamicUTF8String trPackageid_ls00;         // guid char byte 24 encoded in utf8
    tls00_TransSpec         trTransSpec_ls00;   
    tls00_MultSchemaSpec    trSchemaMap_ls00;
    tls00_PartSpec          trPartSpec_ls00;          
    tls00_TableSpec         trTable_ls00;
    tls00_Duplicate         trDplMode_ls00;
    tls00_MultColSpec       trColSpec_ls00;
    tls00_MultFileSpec      trColLongSpec_ls00;
    
    tls00_FileSpec          trConfigurationFile_ls00; 
    tls00_FileSpec          trCatalogFile_ls00;       
    tls00_FileSpec          trDataFile_ls00;
    tls00_FileSpec*         trStatisticFile_ls00;
    tls00_FileSpec          trPackageFile_ls00;       
    
    tls00_Restart           trRestart_ls00;           
    bool                    trInternal_ls00;          
    bool                    trExclude_ls00;           
    tsp00_Addr              trSelOrdStr_ls00;
    tsp00_Int4              trSetColIndex_ls00;    
    tls00_FileSpec          trCmdFile_ls00;
    tls00_Restore           trRestore_ls00;
    tsp00_Addr              trFullUserInput_ls00;
    tls00_Usage             trUsage_ls00;          
    bool                    trOracle_ls00;
} tls00_TransformationCmd;

/*!
  -----------------------------------------------------------------------------
 
  function:     ls00IsDBUnicode

  -----------------------------------------------------------------------------

  description:  Tests if db is UNICODE. To do so at least a connection 
                to the db must be established.
                The default encoding type is ASCII.
                The right value is set after connecting a user to the particular
                db (only then the db releases the information about its
                encoding type).

  arguments:    DBInfo          [IN]    - structure with all info on the running
                                           session and the database connected to

  returnvalue:  true    - db is unicode
                false   - the opposite
  -----------------------------------------------------------------------------
*/
inline bool
ls00IsDBUnicode(tls00_DBInfo *pDBInfo)
{
    return (ctUCS2_els00 == pDBInfo->dbiEncoding_ls00);
}

inline bool
ls00UseUnicodeOI(tls00_DBInfo *pDBInfo)
{
    return ( (ctUCS2_els00        == pDBInfo->dbiOI_Encoding_ls00) ||
             (ctUCS2Swapped_els00 == pDBInfo->dbiOI_Encoding_ls00) ||
             (ctUCS2Native_els00  == pDBInfo->dbiOI_Encoding_ls00));
}

inline bool
ls00IsUCS2(tls00_CodeType *pEncoding)
{
    return ( (ctUCS2_els00        == *pEncoding) ||
             (ctUCS2Swapped_els00 == *pEncoding) ||
             (ctUCS2Native_els00  == *pEncoding) );
}

#endif /*GLS00*/

