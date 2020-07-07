/*!**********************************************************************

  module: vpa31DiagRec.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  diagnostic record

  see also:     http://www.microsoft.com/data/odbc/

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

#include "vpa31DiagRec.h"
#include "vpa80String.h"
#include "hsp78_0.h"

#ifdef ODBC_STATIC        /* PTS 1109265 */
/* LoadString surrogate for static linking of ODBC driver */
unsigned int LoadString_static (SQLUSMALLINT sqlState,
                                UCHAR        *buffer,
                                unsigned int length);
#endif

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

/*!
  Function: pa31GetMessage

  description: Returns the message text of an diagnostic record.

  arguments:
    tpa31DiagRec *diagRec      [in]  Pointer to diagnostic record
    UDWORD        odbcVersion  [in]
    UCHAR        *messsageText [out] Target buffer for error text.
    SWORD         textLen      [in]  Length of *messageText.
    SWORD         charLength   [out] Target for total length of the error
                                     message in char count.
    SWORD         byteLength   [out] Target for total length of the error
                                     message in byte count.
    tsp77encoding *nativeEnc   [in]  encoding if char data are written to
                                     messageText
                                
  return value: RETCODE
    - SQL_SUCCESS for success
    - SQL_ERROR for failure
    - SQL_SUCCES_WITH_INFO if the message text was longer than textLen.

  If the data source produced an error than the error message from the
  data source is returned else the dafault error text for the sql state
  is returned. The generated text also contains the vendor specific
  information ([SAP AG][SQLOD32]...).
  */

SQLRETURN pa31GetMessage( tpa31DiagRec *diagRec,
                          UDWORD        odbcVersion,
                          UCHAR        *messageText,
                          SWORD         textLength,
                          SWORD        *charLength,
                          SWORD        *byteLength,
                          const tsp77encoding *nativeEncoding);


/*!**********************************************************************
  Function: pa31GetSqlStateString

  description: Returns the five character sql state string and the
               textual message for that state.

  arguments:
    UDWORD   odbcVersion  [in]  SQL_OV_ODBC2 or SQL_OV_ODBC3
    UWORD    sqlState     [in]  sql state id (API_ODBC_...)
    UCHAR   *stateString  [out] Target buffer for sql state string
                                (must be at least 6 characters long)
    UCHAR   *messsageText [out] Target buffer for error text.
    SWORD    textLen      [in]  Length of *messageText.
    SWORD    totalLength  [out] Target for total length of the error
                                message.
                                
  return value: RETCODE
    - SQL_SUCCESS for success
    - SQL_ERROR for failure
    - SQL_SUCCES_WITH_INFO if the message text was longer than textLen.

  If odbcVersion is SQL_OV_ODBC2 then 2.x states are mapped to 3.x states.
    */  

SQLRETURN pa31GetSqlStateString( UDWORD   odbcVersion,
                                 UWORD    sqlState,
                                 UCHAR   *stateString,
                                 UCHAR   *messageText,
                                 SWORD    textLen,
                                 SWORD   *totalLength );

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

SQLRETURN pa31GetDiagField( tpa31DiagRec *diagRec,
                            UDWORD        odbcVersion,
                            SWORD         diagId,
                            PTR           valuePtr,
                            SWORD         bufferLength,
                            SWORD        *strLenPtr,
                            const tsp77encoding *nativeEncoding)
{
    SQLRETURN retcode = SQL_SUCCESS;
    UWORD     sqlState; /* dummy */
    UCHAR     stateBuffer[SQL_SQLSTATE_SIZE+1];
    UDWORD    strLenInBytes;

    API_TRACE( API_TR_ENTRY, "pa31GetDiagField", 0);
    API_TRACE( API_TR_PTR, "diagRec", &diagRec );
    API_TRACE( API_TR_UDWORD, "odbcVersion", &odbcVersion );
    API_TRACE( API_TR_SWORD, "diagId", &diagId );
    API_TRACE( API_TR_PTR, "valuePtr", &valuePtr );
    API_TRACE( API_TR_SWORD, "bufferLength", &bufferLength );
    API_TRACE( API_TR_PTR, "strLengthPtr", &strLenPtr );
    
    switch (diagId) {
    case SQL_DIAG_CLASS_ORIGIN:
        switch(diagRec->sqlState) {
        case API_ODBC_IM001:
        case API_ODBC_IM002:
        case API_ODBC_IM003:
        case API_ODBC_IM004:
        case API_ODBC_IM005:
        case API_ODBC_IM006:
        case API_ODBC_IM007:
        case API_ODBC_IM008:
        case API_ODBC_IM009:
        case API_ODBC_IM010:
        case API_ODBC_IM011:
        case API_ODBC_IM012:
        case API_ODBC_IM013:
        case API_ODBC_IM014:
        case API_ODBC_IM015:
            retcode = pa80CopyStringEncoding (nativeEncoding,
                                              valuePtr,
                                              bufferLength,
                                              NULL,           /* char count */
                                              &strLenInBytes, /* byte count */
                                              "ODBC 3.0",
                                              &sqlState );
            if (strLenPtr != NULL)
              *strLenPtr = (SWORD) strLenInBytes;
            break;
        default:
            retcode = pa80CopyStringEncoding (nativeEncoding,
                                              valuePtr,
                                              bufferLength,
                                              NULL,           /* char count */
                                              &strLenInBytes, /* byte count */
                                              "ISO 9075",
                                              &sqlState );
            if (strLenPtr != NULL)
              *strLenPtr = (SWORD) strLenInBytes;
            break;
        }; /* switch */
        break;
    case SQL_DIAG_COLUMN_NUMBER:
        *((SDWORD*) valuePtr) = SQL_COLUMN_NUMBER_UNKNOWN;
        break;
    case SQL_DIAG_MESSAGE_TEXT:
        retcode = pa31GetMessage( diagRec,
                                  odbcVersion,
                                  valuePtr,
                                  bufferLength,
                                  NULL,           /* char count */
                                  strLenPtr,      /* byte count */
                                  nativeEncoding);
        break;
    case SQL_DIAG_NATIVE:
        *((SDWORD*) valuePtr) = diagRec->nativeErr;
        break;
    case SQL_DIAG_ROW_NUMBER:
        *((SDWORD*) valuePtr) = SQL_ROW_NUMBER_UNKNOWN;
        break;
    case SQL_DIAG_SQLSTATE:
        retcode = pa31GetSqlStateString( odbcVersion,
                                         diagRec->sqlState,
                                         stateBuffer,
                                         NULL, 0, NULL );
        if (retcode != SQL_ERROR) {
            retcode = pa80CopyStringEncoding (nativeEncoding,
                                              valuePtr,
                                              bufferLength,
                                              NULL,           /* char count */
                                              &strLenInBytes, /* byte count */
                                              (char*) stateBuffer,
                                              &sqlState );
            if (strLenPtr != NULL)
              *strLenPtr = (SWORD) strLenInBytes;
        }; /* if */
        break;
    case SQL_DIAG_SUBCLASS_ORIGIN:
        switch(diagRec->sqlState) {
        case API_ODBC_IM001:
        case API_ODBC_IM002:
        case API_ODBC_IM003:
        case API_ODBC_IM004:
        case API_ODBC_IM005:
        case API_ODBC_IM006:
        case API_ODBC_IM007:
        case API_ODBC_IM008:
        case API_ODBC_IM010:
        case API_ODBC_IM011:
        case API_ODBC_IM012:
        case API_ODBC_01S00:
        case API_ODBC_01S01:
        case API_ODBC_01S02:
        case API_ODBC_01S06:
        case API_ODBC_01S07:
        case API_ODBC_07S01:
        case API_ODBC_08S01:
        case API_ODBC_21S01:
        case API_ODBC_21S02:
        case API_ODBC_25S01:
        case API_ODBC_25S02:
        case API_ODBC_25S03:
        case API_ODBC_42S01:
        case API_ODBC_42S02:
        case API_ODBC_42S11:
        case API_ODBC_42S12:
        case API_ODBC_42S21:
        case API_ODBC_42S22:
        case API_ODBC_HY098:
        case API_ODBC_HY099:
        case API_ODBC_HY100:
        case API_ODBC_HY101:
        case API_ODBC_HY105:
        case API_ODBC_HY107:
        case API_ODBC_HY109:
        case API_ODBC_HY110:
        case API_ODBC_HY111:
        case API_ODBC_HYT00:
        case API_ODBC_HYT01:
            retcode = pa80CopyStringEncoding (nativeEncoding,
                                              valuePtr,
                                              bufferLength,
                                              NULL,            /* char count */
                                              &strLenInBytes,  /* byte count */
                                              "ODBC 3.0",
                                              &sqlState );
            if (strLenPtr != NULL)
              *strLenPtr = (SWORD) strLenInBytes;
            break;
        default:
            retcode = pa80CopyStringEncoding (nativeEncoding,
                                              valuePtr,
                                              bufferLength,
                                              NULL,            /* char count */
                                              &strLenInBytes,  /* byte count */
                                              "ISO 9075",
                                              &sqlState );
            if (strLenPtr != NULL)
              *strLenPtr = (SWORD) strLenInBytes;
            break;
        }; /* switch */
        break;
    default:
        retcode = SQL_ERROR;
        break;
    }; /* switch */
    
    API_TRACE( API_TR_EXIT, "pa31GetDiagField", 0 );
    API_TRACE( API_TR_RETCODE, "retcode", &retcode );
    
    return retcode;
} /* pa31GetDiagField */


SQLRETURN pa31GetDiagRec( tpa31DiagRec *diagRec,
                          UDWORD        odbcVersion,
                          UCHAR        *sqlStatePtr,
                          SDWORD       *nativeErrPtr,
                          UCHAR        *messageText,
                          SWORD         bufferLength,
                          SWORD        *textLengthPtr,
                          const tsp77encoding *nativeEncoding)
{
    SQLRETURN retcode = SQL_SUCCESS;
    
    API_TRACE( API_TR_ENTRY, "pa31GetDiagRec", 0);
    API_TRACE( API_TR_PTR, "diagRec", &diagRec );
    API_TRACE( API_TR_PTR, "sqlStatePtr", &sqlStatePtr );
    API_TRACE( API_TR_PTR, "nativeErrPtr", &nativeErrPtr );
    API_TRACE( API_TR_PTR, "messageText", &messageText );
    API_TRACE( API_TR_SWORD, "bufferLength", &bufferLength );
    API_TRACE( API_TR_PTR, "textLengthPtr", &textLengthPtr );

    if (nativeErrPtr!=NULL) {
        *nativeErrPtr = diagRec->nativeErr;
    }; /* if */
    if (sqlStatePtr!=NULL) {
        retcode = pa31GetSqlStateString( odbcVersion,
                                         diagRec->sqlState,
                                         sqlStatePtr,
                                         NULL, 0, NULL );

        /* convert sqlState */
        if (nativeEncoding != sp77encodingAscii) {
          SQLWCHAR  textBuffer[6];
          tsp00_Uint4 destBytesWritten, srcBytesParsed;

          API_MEMCPY (textBuffer, sqlStatePtr, 5);
          sp78convertString (nativeEncoding,
                             sqlStatePtr,
                             6 * sizeof (SQLWCHAR), /* 2*5 characters UCS2 + terminator */
                             &destBytesWritten,
                             TRUE,
                             sp77encodingAscii,
                             textBuffer,
                             5,
                             &srcBytesParsed);
        }

    }; /* if */
    if (retcode != SQL_ERROR) {
        retcode = pa31GetMessage( diagRec,
                                  odbcVersion,
                                  messageText,
                                  bufferLength,
                                  textLengthPtr,      /* char count */
                                  NULL,               /* byte count */
                                  nativeEncoding);
    }; /* if */
    
    API_TRACE( API_TR_EXIT, "pa31GetDiagRec", 0 );
    API_TRACE_LEN( API_TR_ODBC_STRING, "sqlStatePtr", sqlStatePtr, SQL_NTS );
    API_TRACE_LEN( API_TR_ODBC_STRING, "messageText", messageText, SQL_NTS );
    API_TRACE( API_TR_SWORD, "*nativeErrPtr", nativeErrPtr );
    API_TRACE( API_TR_SWORD, "*textLengthPtr", textLengthPtr );
    API_TRACE( API_TR_RETCODE, "retcode", &retcode );
    
    return retcode;
} /* pa31GetDiagRec */


API_RETCODE pa31InitDiagRec( tpa31DiagRec *diagRec,
                             UWORD         sqlState,
                             SDWORD        nativeErr,
                             UCHAR        *messageText,
                             SDWORD        colNumber,
                             SDWORD        rowNumber )
{
    API_RETCODE apiRetcode = API_OK;
    
    API_TRACE( API_TR_ENTRY, "pa31InitDiagRec", 0);
    API_TRACE( API_TR_PTR, "diagRec", &diagRec );
    API_TRACE( API_TR_UWORD, "sqlState", &sqlState );
    API_TRACE( API_TR_SDWORD, "nativeErrPtr", &nativeErr );
    API_TRACE( API_TR_UCHAR, "messageText", &messageText );

    diagRec->sqlState = sqlState;
    diagRec->nativeErr = nativeErr;
    API_STRCPY( diagRec->messageText, messageText );
    diagRec->colNumber = colNumber;
    diagRec->rowNumber = rowNumber;
    
    API_TRACE( API_TR_EXIT, "pa31InitDiagRec", 0 );
    API_TRACE( API_TR_API_RETCODE, "apiRetcode", &apiRetcode );
    
    return apiRetcode;
} /* InitDiagRec */

/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

#ifdef WIN32
extern  HANDLE s_hModule;
#endif


/* pa31ConvErr23 - Convert 2.x SQL-states to 3.x SQL-states */

#define API_ODBC_MAPPING_SPECIAL   -1 /* Mapping is not one to one       */

UWORD pa31ConvErr23( UWORD key2x )
{
    /* For the mapping from ODBC 2.x states to ODBC 3.x states the
     * following table is used */
    
    static UWORD state_mapping_tab[] =
    {
        /* ODBC 2.x State, ODBC 3.x State */
        /* ============================== */
        /* API_ODBC_00000 */ API_ODBC_00000,
        /* API_ODBC_01002 */ API_ODBC_01002,
        /* API_ODBC_01004 */ API_ODBC_01004,
        /* API_ODBC_01006 */ API_ODBC_01006,
        /* API_ODBC_01S00 */ API_ODBC_01S00,
        /* API_ODBC_07001 */ API_ODBC_07001,
        /* API_ODBC_07006 */ API_ODBC_07006,
        /* API_ODBC_08001 */ API_ODBC_08001,
        /* API_ODBC_08002 */ API_ODBC_08002,
        /* API_ODBC_08003 */ API_ODBC_08003,
        /* API_ODBC_08004 */ API_ODBC_08004,
        /* API_ODBC_08007 */ API_ODBC_08007,
        /* API_ODBC_08S01 */ API_ODBC_08S01,
        /* API_ODBC_21S01 */ API_ODBC_21S01,
        /* API_ODBC_21S02 */ API_ODBC_21S02,
        /* API_ODBC_22001 */ API_ODBC_22001,
        /* API_ODBC_22003 */ API_ODBC_22003, /* API_ODBC_HY019, */
        /* API_ODBC_22005 */ API_ODBC_22018,
        /* API_ODBC_22008 */ API_ODBC_22007,
        /* API_ODBC_22012 */ API_ODBC_22012,
        /* API_ODBC_22026 */ API_ODBC_22026,
        /* API_ODBC_23000 */ API_ODBC_23000,
        /* API_ODBC_24000 */ API_ODBC_24000,
        /* API_ODBC_25000 */ API_ODBC_25000,
        /* API_ODBC_28000 */ API_ODBC_28000,
        /* API_ODBC_34000 */ API_ODBC_34000,
        /* API_ODBC_37000 */ API_ODBC_42000,
        /* API_ODBC_3C000 */ API_ODBC_3C000,
        /* API_ODBC_40001 */ API_ODBC_40001,
        /* API_ODBC_42000 */ API_ODBC_42000,
        /* API_ODBC_70100 */ API_ODBC_HY018,
        /* API_ODBC_IM001 */ API_ODBC_IM001,
        /* API_ODBC_IM002 */ API_ODBC_IM002,
        /* API_ODBC_IM003 */ API_ODBC_IM003,
        /* API_ODBC_IM004 */ API_ODBC_IM004,
        /* API_ODBC_IM005 */ API_ODBC_IM005,
        /* API_ODBC_IM006 */ API_ODBC_IM006,
        /* API_ODBC_IM007 */ API_ODBC_IM007,
        /* API_ODBC_IM008 */ API_ODBC_IM008,
        /* API_ODBC_IM009 */ API_ODBC_IM009,
        /* API_ODBC_S0001 */ API_ODBC_42S01,
        /* API_ODBC_S0002 */ API_ODBC_42S02,
        /* API_ODBC_S0011 */ API_ODBC_42S11,
        /* API_ODBC_S0012 */ API_ODBC_42S12,
        /* API_ODBC_S0021 */ API_ODBC_42S21,
        /* API_ODBC_S0022 */ API_ODBC_42S22,
        /* API_ODBC_S1000 */ API_ODBC_HY000,
        /* API_ODBC_S1001 */ API_ODBC_HY001,
        /* API_ODBC_S1002 */ API_ODBC_07009, /* API_ODBC_MAPPING_SPECIAL */
        /* API_ODBC_S1003 */ API_ODBC_HY003,
        /* API_ODBC_S1004 */ API_ODBC_HY004,
        /* API_ODBC_S1008 */ API_ODBC_HY008,
        /* API_ODBC_S1009 */ API_ODBC_HY024, /* API_ODBC_MAPPING_SPECIAL */
        /* API_ODBC_S1010 */ API_ODBC_HY010, /* API_ODBC_MAPPING_SPECIAL */
        /* API_ODBC_S1012 */ API_ODBC_HY012,
        /* API_ODBC_S1015 */ API_ODBC_HY015,
        /* API_ODBC_S1090 */ API_ODBC_HY090,
        /* API_ODBC_S1091 */ API_ODBC_HY091,
        /* API_ODBC_S1092 */ API_ODBC_HY092,
        /* API_ODBC_S1093 */ API_ODBC_07009,
        /* API_ODBC_S1094 */ API_ODBC_S1094,
        /* API_ODBC_S1095 */ API_ODBC_HY095,
        /* API_ODBC_S1096 */ API_ODBC_HY096,
        /* API_ODBC_S1097 */ API_ODBC_HY097,
        /* API_ODBC_S1098 */ API_ODBC_HY098,
        /* API_ODBC_S1099 */ API_ODBC_HY099,
        /* API_ODBC_S1100 */ API_ODBC_HY100,
        /* API_ODBC_S1101 */ API_ODBC_HY101,
        /* API_ODBC_S1102 */ API_ODBC_S1102,
        /* API_ODBC_S1103 */ API_ODBC_HY103,
        /* API_ODBC_S1106 */ API_ODBC_HY106,
        /* API_ODBC_S1107 */ API_ODBC_HY107,
        /* API_ODBC_S1108 */ API_ODBC_HY108,
        /* API_ODBC_S1109 */ API_ODBC_HY109,
        /* API_ODBC_S1110 */ API_ODBC_HY110,
        /* API_ODBC_S1C00 */ API_ODBC_HYC00,
        /* API_ODBC_S1DE0 */ API_ODBC_S1DE0,
        /* API_ODBC_S1T00 */ API_ODBC_HYT00,
        /* API_ODBC_01S02 */ API_ODBC_01S02,
        /* API_ODBC_S1011 */ API_ODBC_HY011,
        /* API_ODBC_01000 */ API_ODBC_01000,
        /* API_ODBC_01S01 */ API_ODBC_01S01,
        /* API_ODBC_01S03 */ API_ODBC_01001,
        /* API_ODBC_01S04 */ API_ODBC_01001,
        /* API_ODBC_01S05 */ API_ODBC_01S05
    }; /* state_mapping_tab */    

    if (key2x<=API_STATES_20_MAX) {
        return state_mapping_tab[key2x];
    } else {
        return key2x;
    }; /* else */
} /* pa31ConvErr23 */

SQLRETURN pa31GetMessage( tpa31DiagRec *diagRec,
                          UDWORD        odbcVersion,
                          UCHAR        *messageText,
                          SWORD         textLength,
                          SWORD        *charLength,
                          SWORD        *byteLength,
                          const tsp77encoding *nativeEncoding)
{
    SQLRETURN retcode = SQL_SUCCESS;
    UCHAR messageBuffer [ API_MESSAGE_MAX_LEN + PA_ERROR_TEXT_LEN + 200 ];
    UCHAR stateMessage[ API_MESSAGE_MAX_LEN-1 ];
    UCHAR sqlStateDummy[ SQL_SQLSTATE_SIZE+1 ];
    SWORD lengthDummy;
    UCHAR numberBuffer[ 20 ];
    UWORD sqlState;
    UDWORD charLen, byteLen, *charLenP, *byteLenP;
    
    API_TRACE(API_TR_ENTRY,"pa31GetMessage",0);

    retcode = pa31GetSqlStateString( odbcVersion,
                                     diagRec->sqlState,
                                     sqlStateDummy,
                                     stateMessage,
                                     sizeof(stateMessage),
                                     &lengthDummy );
    if (retcode != SQL_ERROR) {
        /* check is ESQ error or if ODBC error */
        if ( diagRec->nativeErr == 0 ) {
            /* not from ESQ */
            API_STRCPY( (UCHAR*) messageBuffer, (UCHAR*) API_MSG_PREFIX );
            API_STRCAT( (UCHAR*) messageBuffer, (UCHAR*) stateMessage );
        } else {
            API_STRCPY( (UCHAR*) messageBuffer, (UCHAR*) API_MSG_PREFIX );
            API_STRCAT( (UCHAR*) messageBuffer, (UCHAR*) "["API_PROD_NAME"]");
            API_STRCAT( (UCHAR*) messageBuffer, (UCHAR*) stateMessage );
            API_SPRINTF( (char*) numberBuffer, (char*) ";%d ",
                         diagRec->nativeErr );
            API_STRCAT( (UCHAR*) messageBuffer, numberBuffer );
            API_STRCAT( (UCHAR*) messageBuffer, diagRec->messageText );
        }; /* else */
    }; /* else */

    /* call copy function with same NULL pointers if any */
    charLenP = charLength == NULL ? NULL : &charLen;
    byteLenP = byteLength == NULL ? NULL : &byteLen;

    retcode = pa80CopyStringEncodingExt (nativeEncoding,
                                         (char*) messageText,
                                         textLength,
                                         charLenP,
                                         byteLenP,
                                         sp77encodingUTF8,
                                         (char*) messageBuffer,
                                         &sqlState );
    if (charLength != NULL)
      *charLength = (SWORD) charLen;
    if (byteLength != NULL)
      *byteLength = (SWORD) byteLen;
    
    API_TRACE( API_TR_EXIT, "pa31GetMessage",0);
    API_TRACE( API_TR_RETCODE, "retcode",&retcode);

    return retcode;
} /* pa31GetMessage */

SQLRETURN pa31GetSqlStateString( UDWORD   odbcVersion,
                                 UWORD    sqlState,
                                 UCHAR   *stateString,
                                 UCHAR   *messageText,
                                 SWORD    textLen,
                                 SWORD   *totalLength )
{
    SQLRETURN retcode;
    UCHAR buffer [API_MESSAGE_MAX_LEN];
    SWORD length;
    UWORD sqlStateDummy;
    
    API_TRACE(API_TR_ENTRY,"pa31GetSqlStateString",0);
    if (odbcVersion == SQL_OV_ODBC3) {
        /* do mapping between 2.x and 3.x states */
        sqlState = pa31ConvErr23(sqlState);
    }; /* if */
#ifdef WIN32
#ifdef ODBC_STATIC        /* PTS 1109265 */
    length = LoadString_static (sqlState,
                                buffer,
                                sizeof (buffer));
#else    
    length = LoadString( s_hModule,
                         sqlState,
                         (UCHAR FAR *) buffer,
                         sizeof(buffer));
#endif    
#else
    {
        static api_text_tab textTab [ ] =
        {
            API_ODBC_00000, API_TEXT_00000,
            API_ODBC_01002, API_TEXT_01002,
            API_ODBC_01004, API_TEXT_01004,
            API_ODBC_01006, API_TEXT_01006,
            API_ODBC_01S00, API_TEXT_01S00,
            API_ODBC_07001, API_TEXT_07001,
            API_ODBC_07006, API_TEXT_07006,
            API_ODBC_08001, API_TEXT_08001,
            API_ODBC_08002, API_TEXT_08002,
            API_ODBC_08003, API_TEXT_08003,
            API_ODBC_08004, API_TEXT_08004,
            API_ODBC_08007, API_TEXT_08007,
            API_ODBC_08S01, API_TEXT_08S01,
            API_ODBC_21S01, API_TEXT_21S01,
            API_ODBC_21S02, API_TEXT_21S02,
            API_ODBC_22001, API_TEXT_22001,
            API_ODBC_22003, API_TEXT_22003,
            API_ODBC_22005, API_TEXT_22005,
            API_ODBC_22008, API_TEXT_22008,
            API_ODBC_22012, API_TEXT_22012,
            API_ODBC_22026, API_TEXT_22026,
            API_ODBC_23000, API_TEXT_23000,
            API_ODBC_24000, API_TEXT_24000,
            API_ODBC_25000, API_TEXT_25000,
            API_ODBC_28000, API_TEXT_28000,
            API_ODBC_34000, API_TEXT_34000,
            API_ODBC_37000, API_TEXT_37000,
            API_ODBC_3C000, API_TEXT_3C000,
            API_ODBC_40001, API_TEXT_40001,
            API_ODBC_42000, API_TEXT_42000,
            API_ODBC_70100, API_TEXT_70100,
            API_ODBC_IM001, API_TEXT_IM001,
            API_ODBC_IM002, API_TEXT_IM002,
            API_ODBC_IM003, API_TEXT_IM003,
            API_ODBC_IM004, API_TEXT_IM004,
            API_ODBC_IM005, API_TEXT_IM005,
            API_ODBC_IM006, API_TEXT_IM006,
            API_ODBC_IM007, API_TEXT_IM007,
            API_ODBC_IM008, API_TEXT_IM008,
            API_ODBC_IM009, API_TEXT_IM009,
            API_ODBC_S0001, API_TEXT_S0001,
            API_ODBC_S0002, API_TEXT_S0002,
            API_ODBC_S0011, API_TEXT_S0011,
            API_ODBC_S0012, API_TEXT_S0012,
            API_ODBC_S0021, API_TEXT_S0021,
            API_ODBC_S0022, API_TEXT_S0022,
            API_ODBC_S1000, API_TEXT_S1000,
            API_ODBC_S1001, API_TEXT_S1001,
            API_ODBC_S1002, API_TEXT_S1002,
            API_ODBC_S1003, API_TEXT_S1003,
            API_ODBC_S1004, API_TEXT_S1004,
            API_ODBC_S1008, API_TEXT_S1008,
            API_ODBC_S1009, API_TEXT_S1009,
            API_ODBC_S1010, API_TEXT_S1010,
            API_ODBC_S1012, API_TEXT_S1012,
            API_ODBC_S1015, API_TEXT_S1015,
            API_ODBC_S1090, API_TEXT_S1090,
            API_ODBC_S1091, API_TEXT_S1091,
            API_ODBC_S1092, API_TEXT_S1092,
            API_ODBC_S1093, API_TEXT_S1093,
            API_ODBC_S1094, API_TEXT_S1094,
            API_ODBC_S1095, API_TEXT_S1095,
            API_ODBC_S1096, API_TEXT_S1096,
            API_ODBC_S1097, API_TEXT_S1097,
            API_ODBC_S1098, API_TEXT_S1098,
            API_ODBC_S1099, API_TEXT_S1099,
            API_ODBC_S1100, API_TEXT_S1100,
            API_ODBC_S1101, API_TEXT_S1101,
            API_ODBC_S1102, API_TEXT_S1102,
            API_ODBC_S1103, API_TEXT_S1103,
            API_ODBC_S1106, API_TEXT_S1106,
            API_ODBC_S1107, API_TEXT_S1107,
            API_ODBC_S1108, API_TEXT_S1108,
            API_ODBC_S1109, API_TEXT_S1109,
            API_ODBC_S1110, API_TEXT_S1110,
            API_ODBC_S1C00, API_TEXT_S1C00,
            API_ODBC_S1DE0, API_TEXT_S1DE0,
            API_ODBC_S1T00, API_TEXT_S1T00,
            API_ODBC_01S02, API_TEXT_01S02,
            API_ODBC_S1011, API_TEXT_S1011,
            API_ODBC_01000, API_TEXT_01000,
            API_ODBC_01S01, API_TEXT_01S01,
            API_ODBC_01S03, API_TEXT_01S03,
            API_ODBC_01S04, API_TEXT_01S04,
            API_ODBC_01S05, API_TEXT_01S05,
            /* ODBC 3.x states */
            API_ODBC_01001,       API_TEXT_01001,
            API_ODBC_HY019,       API_TEXT_HY019,
            API_ODBC_22007,       API_TEXT_22007,
            API_ODBC_22018,       API_TEXT_22018,
            API_ODBC_07005,       API_TEXT_07005,
            API_ODBC_HY018,       API_TEXT_HY018,
            API_ODBC_42S01,       API_TEXT_42S01,
            API_ODBC_42S02,       API_TEXT_42S02,
            API_ODBC_42S11,       API_TEXT_42S11,
            API_ODBC_42S12,       API_TEXT_42S12,
            API_ODBC_42S21,       API_TEXT_42S21,
            API_ODBC_42S22,       API_TEXT_42S22,
            API_ODBC_HY000,       API_TEXT_HY000,
            API_ODBC_HY001,       API_TEXT_HY001,
            API_ODBC_07009,       API_TEXT_07009,
            API_ODBC_HY003,       API_TEXT_HY003,
            API_ODBC_HY004,       API_TEXT_HY004,
            API_ODBC_HY008,       API_TEXT_HY008,
            API_ODBC_HY009,       API_TEXT_HY009,
            API_ODBC_HY024,       API_TEXT_HY024,
            API_ODBC_HY092,       API_TEXT_HY092,
            API_ODBC_HY007,       API_TEXT_HY007,
            API_ODBC_HY010,       API_TEXT_HY010,
            API_ODBC_HY011,       API_TEXT_HY011,
            API_ODBC_HY012,       API_TEXT_HY012,
            API_ODBC_HY090,       API_TEXT_HY090,
            API_ODBC_HY091,       API_TEXT_HY091,
            API_ODBC_HY096,       API_TEXT_HY096,
            API_ODBC_HY097,       API_TEXT_HY097,
            API_ODBC_HY098,       API_TEXT_HY098,
            API_ODBC_HY099,       API_TEXT_HY099,
            API_ODBC_HY100,       API_TEXT_HY100,
            API_ODBC_HY101,       API_TEXT_HY101,
            API_ODBC_HY103,       API_TEXT_HY103,
            API_ODBC_HY104,       API_TEXT_HY104,
            API_ODBC_HY105,       API_TEXT_HY105,
            API_ODBC_HY106,       API_TEXT_HY106,
            API_ODBC_HY107,       API_TEXT_HY107,
            API_ODBC_HY109,       API_TEXT_HY109,
            API_ODBC_HY110,       API_TEXT_HY110,
            API_ODBC_HY111,       API_TEXT_HY111,
            API_ODBC_HYC00,       API_TEXT_HYC00,
            API_ODBC_HYT00,       API_TEXT_HYT00,
            API_ODBC_01S07,       API_TEXT_01S07,
            API_ODBC_01S08,       API_TEXT_01S08,
            API_ODBC_01S09,       API_TEXT_01S09,
            API_ODBC_07002,       API_TEXT_07002,
            API_ODBC_07S01,       API_TEXT_07S01,
            API_ODBC_22015,       API_TEXT_22015,
            API_ODBC_22019,       API_TEXT_22019,
            API_ODBC_22025,       API_TEXT_22025,
            API_ODBC_25S01,       API_TEXT_25S01,
            API_ODBC_25S02,       API_TEXT_25S02,
            API_ODBC_25S03,       API_TEXT_25S03,
            API_ODBC_40002,       API_TEXT_40002,
            API_ODBC_40003,       API_TEXT_40003,
            API_ODBC_44000,       API_TEXT_44000,
            API_ODBC_HY013,       API_TEXT_HY013,
            API_ODBC_HY014,       API_TEXT_HY014,
            API_ODBC_HY015,       API_TEXT_HY015,
            API_ODBC_HY016,       API_TEXT_HY016,
            API_ODBC_HY017,       API_TEXT_HY017,
            API_ODBC_HY020,       API_TEXT_HY020,
            API_ODBC_HY021,       API_TEXT_HY021,
            API_ODBC_HY095,       API_TEXT_HY095,
            API_ODBC_HYT01,       API_TEXT_HYT01,
            API_ODBC_HY108,       API_TEXT_HY108,
            API_ODBC_IM010,       API_TEXT_IM010,
            API_ODBC_IM011,       API_TEXT_IM011,
            API_ODBC_IM012,       API_TEXT_IM012,
            API_ODBC_IM013,       API_TEXT_IM013,
            API_ODBC_IM014,       API_TEXT_IM014,
            API_ODBC_IM015,       API_TEXT_IM015,
            API_ODBC_01S06,       API_TEXT_01S06,
            API_ODBC_01S10,       API_TEXT_01S10,
            API_ODBC_22002,       API_TEXT_22002
        };
        /* does not totally check for */
        /* an access beyond the array!*/
#ifdef DEBUG
        {
            UWORD i;
            for(i=0;i<(UWORD)(sizeof(text_tab)/sizeof(api_text_tab));i++) {
                if (i != textTab[ i ].key) {
                    API_TRACE(API_TR_UWORD, "invalid text_tab in apdmsg", &i);
                    API_EXIT(99);
                }; /* if */
            }; /* for */
        };
#endif /* DEBUG */
        if (sqlState == textTab [ sqlState ].key ) {
            API_STRCPY( (UCHAR FAR *) buffer,
                        (UCHAR FAR *) textTab [ sqlState ] .text);
            length = (SWORD) API_STRLEN(buffer);
        } else {
            length = 0;
        }; /* else */
    };
#endif /* WIN32 */
    if (length > 0) {
        retcode = SQL_SUCCESS;
        API_MEMCPY((UCHAR*) stateString,
                   (UCHAR*) buffer,
                   SQL_SQLSTATE_SIZE);
        *(stateString + SQL_SQLSTATE_SIZE) = '\0';
        if (messageText!=NULL) {
            retcode = pa80CopyString( (char*) (buffer + SQL_SQLSTATE_SIZE),
                                      textLen,
                                      (char*) messageText,
                                      totalLength,
                                      &sqlStateDummy );
        } else {
            retcode = SQL_SUCCESS_WITH_INFO;
            if (totalLength != NULL) {
                *totalLength = length-SQL_SQLSTATE_SIZE;
            }; /* if */
        }; /* else */
    } else {
        retcode = SQL_ERROR;
    }; /* else */
    
    API_TRACE( API_TR_EXIT, "pa31GetSqlStateString",0);
    API_TRACE( API_TR_RETCODE, "retcode",&retcode);

    return (retcode);
} /* pa31GetSqlStateString */
                       
/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
