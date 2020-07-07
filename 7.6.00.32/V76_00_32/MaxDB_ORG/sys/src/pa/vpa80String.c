/*!**********************************************************************

  module: vpa80String.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  string manipulating functions

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

#include "hsp78_0.h"
#include "vpa80String.h"


/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/* auxilary functions for tpr05_String, only for ASCII and Unicode */
tpr05_String * pa80EmptyString (tpr05_String *str)
{
    if (str == NULL || str->cbMaxLen < 4)  {
        tpr05_StringEncoding encodingType;

        if (str != NULL) {
            encodingType = str->encodingType;
            pr05IfCom_String_DeleteString (str);
        }
        else {
            encodingType = sp77encodingAscii;
        }
        str = pr05IfCom_String_NewDynString (4, encodingType);
    }
    
    switch (str->encodingType->EncodingType) {
    case csp_ascii:
        strcpy (str->rawString, "");
        str->cbLen = 1;
        break;
    case csp_unicode:
        sp81UCS2strcpy ((tsp81_UCS2Char*) str->rawString, SP81_UCS2EMPTYSTRING);
        str->cbLen = 2;
        break;
    default:
        exit (4);
        /* memcpy (dest->rawString, source->rawString, source->cbLen); */
        break;
    }

    return (str);
}



 /* apmstow() - API miscellaneous function, copy C string to ODBC */

API_RETCODE apmstow ( UCHAR *string,
                      UCHAR *wstring,
                      SWORD  wlen,
                      SWORD *size_ptr )
{
  API_RETCODE retcode;
				  /* set the real size/len */
  if (size_ptr != NULL) {
    *size_ptr = (SWORD) API_STRLEN(string);
  }

  if (wstring == NULL || wlen == 0) {
				  /* can't copy any */
    retcode = API_TRUNCATE;
  }
  else if ((SWORD)API_STRLEN(string)+1 <= wlen) {
				  /* it fits, copy it           */
    API_STRCPY(wstring,string);
    retcode = API_OK;
  }
  else {
				  /* it does not fit, copy some */
    API_MEMCPY(wstring,string,(int) (wlen - 1));
    wstring [wlen - 1] = 0;
    retcode = API_TRUNCATE;
  }

  return(retcode);
} /* apmstow */

/* apmwtos() - API miscellaneous function, copy ODBC string to C */
API_RETCODE apmwtos ( UCHAR *wstring,
                      SWORD  wlen,
                      UCHAR *string,
                      SWORD  slen)
{
  API_RETCODE retcode;
  API_TRACE(API_TR_ENTRY,"apmwtos",0);

  /* is it a NULL terminal string?                    */
  switch (wlen) {
    case (SQL_NULL_DATA):
      if (slen > 1) {
          API_STRCPY(string,(UCHAR FAR *) "");
          retcode = API_OK;
      }
      else {
          retcode = API_NOT_OK;
      }
      break;
    case (SQL_NTS):
      if (wstring == NULL) {
          API_STRCPY(string,(UCHAR FAR *) "");
          retcode = API_OK;
      }
      else {
          if ((SWORD)API_STRLEN(wstring)+1 <= slen) {
              API_STRCPY(string,wstring);
              retcode = API_OK;
          }
          else {
              retcode = API_NOT_OK;
          }
      }
      break;
    default:                      /* use lengths                */
      if (wstring == NULL) {
          API_STRCPY(string,(UCHAR FAR *) "");
          retcode = API_OK;
      }
      else {
          if (wlen+1 <= slen) {
              API_MEMCPY(string,wstring,(int) wlen);
              string [wlen] = 0;
              retcode = API_OK;
          }
          else {
              retcode = API_NOT_OK;
          }
      }
      break;
  }

  API_TRACE(API_TR_EXIT,"apmwtos",0);
  API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);

  return(retcode);
} /* apmwtos */


/* wrapper for sp78convertString: source string contains always
   a terminator, even then, the source buffer length is not sufficient.
*/
API_RETCODE pa80convertString (const tsp77encoding *destEncoding,
                               void                *destBuffer,
                               tsp00_Uint4          destBufferLength,
                               tsp00_Uint4         *destBytesWritten,
                               const tsp77encoding *srcEncoding,
                               const void          *srcBuffer,
                               tsp00_Uint4          srcBufferLength)
{
  tsp78ConversionResult cnvResult;
  API_RETCODE           retcode = API_OK;

  API_TRACE (API_TR_ENTRY, "pa80convertString", 0);

  /* does something fit into destBuffer? If not return length in bytes of source in dest encoding */
  if (destBuffer == NULL || destBufferLength < (tsp00_Uint4) destEncoding->terminatorSize) {
      tsp00_Uint4  byteLen, charLen = 0;
      int          isTerm, isCorr, isExh;

      cnvResult = sp78_TargetExhausted;

      if (srcBuffer != NULL)
          srcEncoding->stringInfo (srcBuffer, srcBufferLength, TRUE, /* length in bytes */ 
                                   &charLen, &byteLen, 
                                   &isTerm, &isCorr, &isExh);

      if (destBytesWritten != NULL)
          *destBytesWritten = charLen * destEncoding->fixedCharacterSize   /* does not work */
                                      / srcEncoding->fixedCharacterSize;   /* with UTF-8    */
  }
  else  {
      tsp00_Uint4  parsedBytes;

      /* reserve space for terminator */
      destBufferLength -= destEncoding->terminatorSize;
  
      cnvResult = sp78convertString (destEncoding,
                                     destBuffer, destBufferLength,
                                     destBytesWritten,
                                     FALSE,
                                     srcEncoding,
                                     srcBuffer,
                                     srcBufferLength,
                                     &parsedBytes);

      /* append always terminator */
      API_MEMCPY (((CHAR*)destBuffer)+*destBytesWritten,
                  destEncoding->charTable->terminator.bytes,
                  destEncoding->charTable->terminator.byteCount);
  }

  if (cnvResult == sp78_TargetExhausted)
      retcode = API_TRUNCATE;
  else if (cnvResult != sp78_Ok)
      retcode = API_NOT_OK;

  API_TRACE (API_TR_EXIT,"pa80convertString",0);

  return retcode;
}



/* pa80ODBCtoEncoding writes tpr05 string into buffer according
   to encoding type */

API_RETCODE pa80ODBCtoEncoding (tpr05_StringEncoding encodingType,
                                PTR *buffer,
                                tsp00_Uint4 maxBufLen,
                                tsp00_Uint4 *usedBufLen,
                                tpr05_String *src)
{

  return pa80convertString (encodingType, buffer, maxBufLen, usedBufLen,
                            src->encodingType, src->rawString, src->cbLen);


/*

  tsp78ConversionResult cnvResult;
  API_RETCODE           retcode = API_OK;
    
  API_TRACE (API_TR_ENTRY, "pa80ODBCtoEncoding", 0);

  if (buffer == NULL) {
      tsp00_Uint4  byteLen, charLen = 0;
      int          isTerm, isCorr, isExh;

      cnvResult = sp78_TargetExhausted;

      if (dest->rawString != NULL && usedBufLen != NULL) {
          encodingType->stringInfo (dest->rawString, dest->cbLen, TRUE, 
                                    &charLen, &byteLen, 
                                    &isTerm, &isCorr, &isExh);
          *usedBufLen = (tsp00_Uint4) charLen;
      }
  }
  else  {
      tsp00_Uint4  parsedBytes;
      tsp00_Uint4  writtenLen;

      * reserve space for terminator *
      maxBufLen -= encodingType->terminatorSize;

      cnvResult = sp78convertString (encodingType,
                                     buffer, maxBufLen,
                                     &writtenLen,
                                     FALSE,
                                     dest->encodingType,
                                     dest->rawString,
                                     dest->cbLen,
                                     &parsedBytes);

      * append always terminator *
      API_MEMCPY (((CHAR*)buffer)+writtenLen,
                  encodingType->charTable->terminator.bytes,
                  encodingType->charTable->terminator.byteCount);

      if (usedBufLen != NULL)
          *usedBufLen = (tsp00_Uint4) writtenLen;
  }

  if (cnvResult == sp78_TargetExhausted)
      retcode = API_TRUNCATE;
  else if (cnvResult != sp78_Ok)
           retcode = API_NOT_OK;
  
  API_TRACE (API_TR_EXIT, "pa80ODBCtoEncoding", 0);
  return (retcode);
*/
}

/* pa80ODBCtoC() - API miscellaneous function,
   copy ODBC string to C (tpr05_String) 
   no terminator is appended */
API_RETCODE pa80ODBCtoC ( tpr05_String        **dest,
                          SQLCHAR                *src,
                          SQLSMALLINT           srcLen)
{
  tsp00_Uint4 len = srcLen;

  API_TRACE (API_TR_ENTRY, "pa80ODBCtoC", 0);

  switch (srcLen) {
  case SQL_NULL_DATA:
      len = 0;
      break;
  case SQL_NTS:
      len = (tsp00_Uint4) API_STRLEN (src);
      break;
  default: ;
  }

  if (*dest == NULL)
      *dest = pr05IfCom_String_NewDynString (len+1, sp77encodingAscii);

  if (len > 0){
      API_STRNCPY ((*dest)->rawString, src, len);
      ((*dest)->rawString)[len] = '\0';
      (*dest)->cbLen = len;
  }
  
  API_TRACE (API_TR_EXIT, "pa80ODBCtoC", 0);
  return (API_OK);
}
API_RETCODE pa80ODBCtoTpr05 ( tpr05_String        *dest,
                              const tsp77encoding *srcEncoding,
                              VOID                *src,
                              SQLINTEGER           srcLen)
{
  API_RETCODE api_retcode = API_OK;
  tsp00_Uint4 byteLen = 0;

  API_TRACE (API_TR_ENTRY, "pa80ODBCtoC", 0);

  switch (srcLen) {

  case SQL_NULL_DATA:
    pr05IfCom_String_ClearString (dest);
    break;

  case SQL_NTS: {
    tsp00_Uint4 charLen;
    int isTerm, isCorr, isExh;
    /* PTS 1114007 */
    if (src != NULL && srcEncoding->stringInfo (src, 1UL<<31, TRUE, 
                                                &charLen, &byteLen, 
                                                &isTerm, &isCorr, &isExh))
      api_retcode = API_NOT_OK;
    break;
  }
  default:
    byteLen = srcLen;
    if (srcLen < 0)
      api_retcode = API_NOT_OK;
  }

  if (api_retcode == API_OK) {
    pr05IfCom_String_ClearString (dest);
    if (pr05IfCom_String_strcatP (dest, src, byteLen, srcEncoding) != ok_epr05)
      api_retcode = API_NOT_OK;
  }

  API_TRACE (API_TR_EXIT, "pa80ODBCtoC", 0);
  return (api_retcode);
}

/* pa80ODBCtoCW() - API miscellaneous function,
   copy ODBC string to C (tpr05_String, UCS2native)
   additional \0 is appended */

API_RETCODE pa80ODBCtoCW ( tpr05_String **dest,
                           SQLWCHAR     *src,
                           SQLSMALLINT   srcLen)
{
  API_TRACE (API_TR_ENTRY, "pa80ODBCtoCW", 0);

  switch (srcLen) {
  case SQL_NULL_DATA:
      srcLen = 0;
      break;
  case SQL_NTS:
      srcLen = (SQLSMALLINT) sp81UCS2strlen ((tsp81_UCS2Char*) src);
      break;
  default: ;
  }

  if (*dest == NULL)
      *dest = pr05IfCom_String_NewDynString (srcLen+2, 
                                             sp77nativeUnicodeEncoding ());

  if (srcLen > 0) {
      sp81UCS2strncpy ((tsp81_UCS2Char*) (*dest)->rawString,
                       (tsp81_UCS2Char*) src, srcLen/2);
      API_MEMCPY (((CHAR*)(*dest)->rawString)+srcLen,
                  (*dest)->encodingType->charTable->terminator.bytes,
                  (*dest)->encodingType->charTable->terminator.byteCount);
      (*dest)->cbLen = srcLen;
  }
  
  API_TRACE (API_TR_EXIT, "pa80ODBCtoCW", 0);
  return (API_OK);
}


/* apmwchs() - API miscellaneous function, check ODBC string */
API_RETCODE apmwchs ( UCHAR FAR * wstring,
  SWORD wlen,
  SWORD *len)

{
   API_RETCODE retcode;
   API_TRACE(API_TR_ENTRY,"apmwchs",0);
   retcode = API_OK;
   switch (wlen) {
      case (SQL_NULL_DATA):
	 *len = 0;
	 break;
      case (SQL_NTS):
	 *len = (SWORD)API_STRLEN(wstring);
	 break;
      default:                      /* use lengths                */
	 if (wstring == NULL)
	    *len = 0;
	 else
	    if (wlen > 0)
	       *len = wlen;
	    else 
	       retcode = API_NOT_OK;
	 break;
  }
  API_TRACE(API_TR_EXIT,"apmwchs",0);
  API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
  
  return(retcode);
} /* apmwchs */


API_RETCODE pa80AllocString (const tsp77encoding *destEncoding,
                             UCHAR  **dest, 
                             UCHAR   *source,
                             UDWORD   length,
                             UDWORD   maxLength)
{
    API_RETCODE retcode = API_OK;
    UDWORD      len = length + destEncoding->terminatorSize;
    tsp00_Uint4 charLen, byteLen;
    int         isTerm, isCorr, isExh;
    
    if (length == SQL_NTS) {
        /* string length plus null-terminator */
      destEncoding->stringInfo (source, maxLength, TRUE,
                                &charLen, &byteLen,
                                &isTerm, &isCorr, &isExh);
      len = isTerm == TRUE ? byteLen : maxLength;
      retcode = API_TRUNCATE;
    } else {
        *dest = apdallo (len + destEncoding->terminatorSize);
        API_MEMCPY (*dest, source, len);
        API_MEMCPY (((CHAR*)dest)+len,
                    destEncoding->charTable->terminator.bytes,
                    destEncoding->charTable->terminator.byteCount);
    }; /* else */

    return retcode;
} /* pa80AllocString */


RETCODE pa80CopyFixedString( UCHAR   *source,
                             SDWORD   sourceLen,
                             SDWORD   maxLen,
                             UCHAR   *dest,
                             SDWORD  *totalLen,
                             UWORD   *sqlStatePtr)
{
    SDWORD strSize;
    RETCODE retcode = SQL_SUCCESS;

    if (API_STRLEN( source ) == 0 ) {
        strSize = 0;
    } else {
        strSize = sourceLen;
        /* ignore blanks at the end */
        while ( (strSize > 0) 
                && (*(source + strSize -1) == ' ') ) {
            strSize--;
        }; /* while */
    }; /* else */
    /* null terminator */
    strSize++;
    if (strSize>maxLen) {
        if (maxLen>0) {
            API_MEMCPY( dest, source, maxLen-1 );
            dest[maxLen-1] = '\0';
        }; /* if */
        *sqlStatePtr = API_ODBC_01004; /* String data right truncated */
        retcode = SQL_SUCCESS_WITH_INFO;
    } else {
        if (strSize>1) {
            API_MEMCPY( dest, source, strSize-1 );
        }; /* if */
        dest[strSize-1] = '\0';
    }; /* else */
    /* string length minus null-terminator */
    if (totalLen != NULL) {
        *totalLen = strSize-1;
    }; /* if */
    
    return retcode;
} /* pa80CopyFixedString */

RETCODE pa80CopyFixedStringEncoding ( const tsp77encoding *destEncoding,
                                      VOID    *dest,
                                      UDWORD   destLen,
                                      UDWORD  *charLength,
                                      UDWORD  *byteLength,
                                      UCHAR   *source,
                                      UDWORD   sourceLen,
                                      UWORD   *sqlStatePtr )
{
    API_RETCODE cnvResult;
    RETCODE     retcode = SQL_SUCCESS;
    tsp00_Uint4 destBytesWritten, charLen, byteLen;
    int         isTerm, isCorr, isExh;

    /* remove padding blanks */
    sourceLen -= sp77encodingUTF8->countPadChars (source, sourceLen, ' ');

    /* convert source to dest */
    cnvResult = pa80convertString (destEncoding,
                                   dest,
                                   destLen,
                                   &destBytesWritten,
                                   sp77encodingUTF8,
                                   source,
                                   sourceLen);

    if (cnvResult == API_TRUNCATE) {
      *sqlStatePtr = API_ODBC_01004; /* String data right truncated */
      retcode = SQL_SUCCESS_WITH_INFO;
    }

    /* fill charLen and byteLen if necessary */
    if (charLength != NULL || byteLength != NULL) {
      destEncoding->stringInfo (dest, destBytesWritten, TRUE,
                                &charLen, &byteLen, &isTerm,
                                &isCorr, &isExh);
      if (charLength != NULL)
        *charLength = charLen;
      if (byteLength != NULL)
        *byteLength = byteLen;
      }

  return retcode;
} /* pa80CopyFixedStringEncoding */

RETCODE pa80CopyStringEncoding (const tsp77encoding *destEncoding,
                                VOID                *dest,
                                tsp00_Uint4          destBufferLength,
                                UDWORD              *charLength,
                                UDWORD              *byteLength,
                                const void          *source,
                                UWORD               *sqlStatePtr)
{
  return pa80CopyStringEncodingExt (destEncoding, dest,
                                    destBufferLength,
                                    charLength, byteLength,
                                    sp77encodingAscii, source,
                                    sqlStatePtr);
} /* pa80CopyStringEncoding */

RETCODE pa80CopyStringEncodingExt (const tsp77encoding *destEncoding,
                                   VOID                *dest,
                                   tsp00_Uint4          destBufferLength,
                                   UDWORD              *charLength,
                                   UDWORD              *byteLength,
                                   const tsp77encoding *sourceEncoding,
                                   const void          *source,
                                   UWORD               *sqlStatePtr)
{
    API_RETCODE cnvResult;
    RETCODE     retcode = SQL_SUCCESS;
    tsp00_Uint4 destBytesWritten, charLen, byteLen;
    unsigned int sourceBytesLen, sourceCharLen;
    int         isTerm, isCorr, isExh;

    /* need lengths of source in bytes and characters */
    sourceEncoding->stringInfo (source, 1UL << 31, TRUE,
                                &sourceCharLen, &sourceBytesLen, &isTerm,
                                &isCorr, &isExh);

    if (dest == NULL || destBufferLength == 0) {
      if (charLength != NULL) 
        *charLength = (UDWORD) sourceCharLen;
      goto exit;
    }

    cnvResult = pa80convertString (destEncoding,
                                   dest,
                                   destBufferLength,
                                   &destBytesWritten,
                                   sourceEncoding,
                                   source,
                                   (tsp00_Uint4) sourceBytesLen);

    if (cnvResult == API_TRUNCATE) {
      *sqlStatePtr = API_ODBC_01004;    /* String data right truncated */
      retcode = SQL_SUCCESS_WITH_INFO;
      /* recalculate lengths for truncated string */
      if (charLength != NULL || byteLength != NULL) {
        destEncoding->stringInfo (dest, destBytesWritten, TRUE,
                                  &charLen, &byteLen, &isTerm,
                                  &isCorr, &isExh);
        if (charLength != NULL)
          *charLength = charLen;
        if (byteLength != NULL)
          *byteLength = byteLen;
      }
    }
    else {
      if (charLength != NULL)
        *charLength = (UDWORD) sourceCharLen;
      if (byteLength != NULL)
        *byteLength = (UDWORD) destBytesWritten;
    }

 exit:
  return retcode;
} /* pa80CopyStringEncoding */


RETCODE pa80CopyString( const char  *source,
                        SWORD   maxLen,
                        char   *dest,
                        SWORD  *totalLen,
                        UWORD  *sqlStatePtr)
{
    SWORD strSize;
    RETCODE retcode = SQL_SUCCESS;
    
    /* string length plus null-terminator */
    strSize = (SWORD) API_STRLEN(source)+1;
    if (strSize>maxLen) {
        if (maxLen>0) {
            API_MEMCPY( dest, source, maxLen-1 );
            dest[maxLen-1] = '\0';
        }; /* if */
        *sqlStatePtr = API_ODBC_01004; /* String data right truncated */
        retcode = SQL_SUCCESS_WITH_INFO;
    } else {
        API_STRCPY( dest, source );
    }; /* else */
    /* string length minus null-terminator */
    if (totalLen != NULL) {
        *totalLen = strSize-1;
    }; /* if */
    
    return retcode;
} /* pa80CopyString */

RETCODE pa80CopyStringD( const char   *source,
                         SDWORD   maxLen,
                         char   *dest,
                         SDWORD  *totalLen,
                         UWORD   *sqlStatePtr)
{
    SDWORD strSize;
    RETCODE retcode = SQL_SUCCESS;
    
    /* string length plus null-terminator */
    strSize = (SDWORD) API_STRLEN((UCHAR*) source)+1;
    if (strSize>maxLen) {
        if (maxLen>0) {
            API_MEMCPY( dest, source, maxLen-1 );
            dest[maxLen-1] = '\0';
        }; /* if */
        *sqlStatePtr = API_ODBC_01004; /* String data right truncated */
        retcode = SQL_SUCCESS_WITH_INFO;
    } else {
        API_STRCPY( dest, source );
    }; /* else */
    /* string length minus null-terminator */
    if (totalLen != NULL) {
        *totalLen = strSize-1;
    }; /* if */
    
    return retcode;
} /* pa80CopyStringD */

/* like pa80CopyString for tpr05 strings. Appends terminator,
   even if string is truncated. */
RETCODE pa80CopyTpr05String( VOID          *dest,
                             UDWORD         maxLen,
                             UDWORD        *totalLen,
                             tpr05_String  *source,
                             UWORD         *sqlStatePtr)
{
    RETCODE retcode = SQL_SUCCESS;
    UDWORD  len;

    len = maxLen - source->encodingType->terminatorSize;

    if (len < source->cbLen)  {
        API_MEMCPY (dest, source->rawString, len);
        API_MEMCPY (((CHAR*)dest)+len,
                    source->encodingType->charTable->terminator.bytes,
                    source->encodingType->charTable->terminator.byteCount);
        *sqlStatePtr = API_ODBC_01004; /* String data right truncated */
        retcode = SQL_SUCCESS_WITH_INFO;
    } /* if */
    else {
        API_MEMCPY (dest, source->rawString, source->cbLen);
        API_MEMCPY (((CHAR*)dest)+source->cbLen,
                    source->encodingType->charTable->terminator.bytes,
                    source->encodingType->charTable->terminator.byteCount);
    }        
    if (totalLen != NULL) {
        *totalLen = source->cbLen;
    }
        
    return retcode;
} /* pa80CopyTpr05String */

int pa80containsWildCard (const void *string,
                         long length,
                         const tsp77encoding *encoding)
{
  if (encoding == sp77encodingAscii) {
    /* there is no official strnchr => null termination with local copy */
    char *lstr = (char*) alloca (length+1);
    const char *p;

    memcpy (lstr, string, length);
    lstr[length] = '\0';

    p = strchr (lstr, '%');

    if (p == 0)
      p = strchr (lstr, '_');

    return p != NULL;
  } else {

    const tsp81_UCS2Char *lstr = (const tsp81_UCS2Char *) string;
    tsp81_UCS2Char  percent;
    tsp00_Byte      c;
    unsigned int    dummy;
    const tsp81_UCS2Char *p;

    c = '%';
    sp81ASCIItoUCS2 ( &percent, 1, encoding == sp77encodingUCS2Swapped,
                      &dummy, &c, 1);

    p = sp81UCS2strnchr (lstr, percent, length);

    if (p == NULL) {
      tsp81_UCS2Char  underscore;
      c = '_';
      sp81ASCIItoUCS2 ( &underscore, 1, encoding == sp77encodingUCS2Swapped,
                        &dummy, &c, 1);

      p = sp81UCS2strnchr (lstr, underscore, length);
    }

    return p != NULL;
  }

  return 0;
}


/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/
