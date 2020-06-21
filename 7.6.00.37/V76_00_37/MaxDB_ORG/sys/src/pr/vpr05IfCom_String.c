/****************************************************************************

  module      : pr05IfCom_String.c

  -------------------------------------------------------------------------

  responsible : Marco Paskamp (marco.paskamp@sap.com)

  special area:
  description : universal String definition of Interfaces

  last changed: 2000-09-08  9:04
  see also    :

  -------------------------------------------------------------------------


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


\endif
*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "vpr05IfCom_String.h"
#include "gpr03m.h"
#include "gpr05.h"
#include "hsp78_0.h"
#include "hsp83.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*    gehoert eigentlich in die LZU        */ 
tsp00_SwapKind_Enum sqlLocalMachineSwapKind;
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*!
  Function:     pr05IfCom_String_mapErrorCode

  Description:  Maps error code from tsp78ConversionResult to corresponding
                error code in tpr05_StringError
    
  Arguments:
    tsp78ConversionResult [in] source error code

  return value: result of mapping to tpr05_StringError
 */
tpr05_StringError pr05IfCom_String_mapErrorCode ( tsp78ConversionResult ConvRes )
{
switch (ConvRes)
    {
      case (sp78_Ok):
      /* Conversion sucessfull */
      {
          return ok_epr05;
      }    
      case (sp78_SourceExhausted):
      /* last character only partial in source buffer */
      {
          return SourceExhausted_epr05;
      }
      case (sp78_SourceCorrupted):
      /* source contains invalid chars or char sequences */
      {
          return SourceCorrupted_epr05;
      }  
      case (sp78_TargetExhausted):
      /* not enaugh room in target for conversion */
      {
        return TargetExhausted_epr05;
      }
      case (sp78_BadSourceEncoding):
      /* unsupported source encoding */
      {
        return BadSourceEncoding_epr05;
      }   
      case (sp78_BadTargetEncoding):
      /* unsupported destination encoding */
      {
        return BadTargetEncoding_epr05;
      }         
      case (sp78_NullArgument):
      /* NULL pointer parsed */
      {
          return NullArgument_epr05;
      }         
      default :
      {
          return procError_epr05;
      }
    }  /*end switch*/
}
/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
  Function:     pr05IfCom_InitString

  see also: 

  Description:  initalisizes a given tpr05_String
    
  NOTE The String will be initialized with the parameters - length_in_bytes,
  encoding type and allocation type. If given length_in_bytes is Null the 
  rawString of tpr05_String is also null. The default encoding type is
  "sp77encodingAscii" and if no allocation type is set the default 
  allocation type is constant.
  
  Arguments:
    tpr05_String    [in|out] pointer to string that will be initialized 
    length_in_bytes [in]     length of rawString in bytes  
    encoding type   [in]     encoding type of rawString (z.B. sp77encodingUCS2)
    tpr05_AllocationType allocationType [in] type of memory allocation
    
  return value: pointer to the initialized tpr05_String or NULL (in case of error)
 */
tpr05_String* pr05IfCom_String_InitString(tpr05_String* pString, void *rawString, tsp00_Uint4 length_in_bytes, tpr05_StringEncoding encodingType, tpr05_AllocationType allocationType)
{
  if (pString) 
  {
    pString->encodingType = (encodingType) ? encodingType : sp77encodingAscii;
    
    switch (allocationType)
    {
      case (constant_epr05):
      { 
        if (pString->allocationType == allocated_epr05)
        /*string was allocated free it*/
        {
          pr03mFree(pString->rawString, "IfCom_String->rawString");
        }
        pString->rawString  = rawString;
        pString->cbMaxLen = pString->cbLen = length_in_bytes;
        pString->allocationType = constant_epr05;
        break;
      }
      case (allocated_epr05):
      {
        if (pString->allocationType ==  constant_epr05)
        /* string was a constant then alloc memory*/
        {    
          pString->rawString = pr03mAllocat((length_in_bytes*sizeof(char)), "IfCom_String->rawString");
          pString->cbMaxLen = length_in_bytes;
        }
        else if (pString->cbMaxLen < length_in_bytes)
        /* string was dynamic allocated but is too small -> realloc*/
        {
          pString->rawString = pr03mReallocat(pString->rawString, (length_in_bytes*sizeof(char)), "IfCom_String->rawString");
          pString->cbMaxLen = length_in_bytes;
        }    
        pString->cbLen          = 0;
        pString->allocationType = allocated_epr05;
        if (rawString)
        {
          memcpy (pString->rawString, rawString, length_in_bytes );   
          pString->cbLen = length_in_bytes;                      
        }
        break;
      }    
    } /*end switch*/
  }
  return pString;
}
/*****************************************************************************/
/*!
  Function:     pr05IfCom_String_NewConstString

  see also: 

  Description:  Creates and Initializes new constant tpr05_String
    
  Allocates and initializes a new tpr05_String  that points to a constant
  string. Returns a pointer to the new String.
  
  NOTE The new tpr05String->rawString points to the given constString. The String 
  will be initialized with the parameters - length_in_bytes, encoding type and allocation 
  type. If given length_in_bytes is Null the  rawString of tpr05_String is also null. 
  The default encoding type is "sp77encodingAscii" and the allocation type is allocated_epr05.
  
  Arguments:
    constString     [in] pointer to a string
    length_in_bytes [in] length of rawString in bytes  
    encoding type   [in] encoding type of rawString (z.B. sp77encodingUCS2)

  return value: pointer to the initialized tpr05_String or NULL (in case of error)
 */
tpr05_String* pr05IfCom_String_NewConstString(void *constString, tsp00_Uint4 length_in_bytes, tpr05_StringEncoding encodingType)
{
  tpr05_String* String = pr03mAllocat(sizeof(tpr05_String), "IfCom_String");
  if (String) {
    String->encodingType   = (encodingType) ? encodingType : sp77encodingAscii;
    String->allocationType = constant_epr05; /* const string do not realloc */
    String->rawString      = constString;
    String->cbMaxLen = String->cbLen = length_in_bytes;
  }
  return String;
}

/*****************************************************************************/
/*!
  Function:     pr05IfCom_String_NewDynString

  see also: 

  Description:  Creates and Initializes new dynamic allocated tpr05_String
    
  Allocates and initializes a new tpr05_String. Returns a pointer the new String.
  
  NOTE The String will be initialized with the parameters - length_in_bytes,
  encoding type and allocation type. If given length_in_bytes is Null the 
  rawString of tpr05_String is also null. The default encoding type is
  "sp77encodingAscii" and the allocation type is allocated_epr05.
  
  Arguments:
    length_in_bytes [in]     length of rawString in bytes  
    encoding type   [in]     encoding type of rawString (z.B. sp77encodingUCS2)

  return value: pointer to the initialized tpr05_String or NULL (in case of error)
 */
tpr05_String* pr05IfCom_String_NewDynString(tsp00_Uint4 length_in_bytes, tpr05_StringEncoding encodingType)
{
  tpr05_String *String = pr03mAllocat(sizeof(tpr05_String), "IfCom_String");
  if (String)
  {
    if(encodingType != NULL)
    {
      String->encodingType = encodingType;
    }
    else
    /* default encoding type is ASCII*/
    {
      String->encodingType = sp77encodingAscii;
    }
    String->rawString      = NULL;
    String->cbLen          = 0;
    String->allocationType = allocated_epr05;
            
    if(length_in_bytes > 0)
    {
      String->rawString = pr03mAllocat((length_in_bytes*sizeof(char)), "IfCom_String->rawString");
    }
    if(String->rawString)
    {
      String->cbMaxLen = length_in_bytes;
    }
    else
    {
      String->cbMaxLen = 0;
    }
  }
  return String;
}
/**********************************************************************/


/*!
  Function:     pr05IfCom_String_DeleteString

  see also: 

  Description:  deletes a tpr05_String and free memory
    
  Arguments:
    String [in] pointer to a tpr05_String 
  
  return value: void
 */
void pr05IfCom_String_DeleteString(tpr05_String *String)
{
  if (String) {
    pr05IfCom_String_DeleterawString(String);
    pr03mFree(String, "IfCom_String");
  }
}

/*!
  Function:     pr05IfCom_String_DeleterawString

  see also: 

  Description:  deletes the rawString of tpr05_String if needed.
    
  Arguments:
    String [in] pointer to a tpr05_String 
  
  return value: void
 */
void pr05IfCom_String_DeleterawString(tpr05_String *String)
{
  if (String) 
  {
    if (   String->rawString 
        && String->allocationType == allocated_epr05) 
    {
        pr03mFree(String->rawString, "IfCom_String->rawString");
    }
    String->rawString  = NULL;
    String->cbLen = String->cbMaxLen = 0;
  }
}

/*****************************************************************************/

/*!
  Function:     pr05IfCom_String_Convert

  see also: 

  Description:  convert content of string source to string dest
    
  Arguments:
    dest           [out] pointer to the destination tpr05_String 
    source         [in]  pointer to the source tpr05_String 
  
  return value: error code
 */
tpr05_StringError pr05IfCom_String_Convert (       tpr05_String *dest,
                                             const tpr05_String *source)
{
   return pr05IfCom_String_ConvertP (dest,
                                     source->rawString, 
                                     source->cbLen,
                                     source->encodingType);
}
/*****************************************************************************/

/*!
  Function:     pr05IfCom_String_Convert

  see also: 

  Description:  convert content of string source (given by buffer, length and 
                encoding) to string dest
    
  Arguments:
    dest        [in|out] pointer to a tpr05_String 
    srcBuf      [in]     pointer to a char * buffer containing source string 
    srcLength   [in]     length in bytes of source string
    srcEncoding [in]     encoding type of source string
  
  return value: error code
 */
tpr05_StringError pr05IfCom_String_ConvertP (tpr05_String *dest,
                                             const char   *srcBuf, 
                                             tsp00_Uint4   srcLength,
                                             tpr05_StringEncoding srcEncoding)
{
  tsp78ConversionResult ConvRes          = sp78_Ok;
  tsp00_Uint4           destBytesWritten = 0;
  tsp00_Uint4           srcBytesParsed   = 0;
  ConvRes = sp78convertString( dest->encodingType,
                               dest->rawString,
                               dest->cbMaxLen,
                               &destBytesWritten,
                               FALSE,
                               srcEncoding,
                               srcBuf,
                               srcLength,
                               &srcBytesParsed);

  dest->cbLen = destBytesWritten;
  return pr05IfCom_String_mapErrorCode (ConvRes);
}

/**********************************************************************/
/*!
  Function:     pr05IfCom_String_strlen

  see also: 

  Description:  gives length of string
    
  Arguments:
    string         [in] pointer to a tpr05_String 
  
  return value: length of string in bytes
 */
/* unsigned int pr05IfCom_String_strlen (const tpr05_String *str) 
   {*/
   /*tsp00_Uint4 destLengthBytes, destLengthChar;
     tsp00_Int4  isTerminated, isCorrupted, isExhausted;
      
     str->encodingType->stringInfo( str->rawString,
                                  (tsp00_Uint4) str->cbLen,
                                   TRUE,
                                   &destLengthChar,
                                   &destLengthBytes,
                                   &isTerminated,
                                   &isCorrupted,
                                   &isExhausted);
   return (destLengthBytes);   */                             
  /*   return str->cbLen;
       }*/
/*****************************************************************************/

/*!
  Function:     pr05IfCom_String_strcpy

  see also: 

  Description:  copies content of string source to string dest 
  
  The copy includes all components of tpr05_String (encoding type, length,
  max length, rawString and allocation type)
    
  Arguments:
    dest           [out] pointer to the destination tpr05_String 
    source         [in]  pointer to the source tpr05_String 
  
  return value: error code
 */
tpr05_StringError pr05IfCom_String_strcpy (      tpr05_String *dest,
                                           const tpr05_String *source)
{
    if (dest == source)
        return (ok_epr05);

    switch(source->allocationType)
    { 
        case(allocated_epr05):
        {
            if (   dest->allocationType == unknown_epr05
                || dest->allocationType == constant_epr05 )
            {
                dest->rawString = pr03mAllocat((source->cbMaxLen*sizeof(char)), "IfCom_String->rawString");
            }
            else if(dest->cbMaxLen != source->cbMaxLen )
            /*allocation type must be allocated_epr05*/
            {
                dest->rawString = pr03mReallocat(dest->rawString, (source->cbMaxLen*sizeof(char)), "IfCom_String->rawString");
            }
            memcpy (dest->rawString, source->rawString, source->cbLen);
            break;
        }
        case(constant_epr05):
        {
            if (dest->allocationType == allocated_epr05)
            {
                if (dest->rawString)
                  pr03mFree(dest->rawString, "IfCom_String->rawString");
            }
            dest->rawString = source->rawString;
            break;
        }
        default:
        {
            return (BadSourceEncoding_epr05);   
        }
    
    }/*end switch*/

    dest->allocationType = source->allocationType;
    dest->encodingType   = source->encodingType;
    dest->cbLen          = source->cbLen;
    dest->cbMaxLen       = source->cbMaxLen;

    return (ok_epr05);
}

/*****************************************************************************/
/*!
  Function:     pr05IfCom_String_strcat

  see also: 

  Description:  concatenates strings
    
  Arguments:
    dest           [in|out] pointer to a tpr05_String 
    str            [in]     pointer to a tpr05_String 
  
  return value: error code
 */
tpr05_StringError pr05IfCom_String_strcat (      tpr05_String *dest,
                                           const tpr05_String *source)
{
  return pr05IfCom_String_strcatP (dest,
                                   source->rawString, 
                                   source->cbLen,
                                   source->encodingType);
}


/*!
  Function:     pr05IfCom_String_strcatP

  see also: 

  Description:  concatenates a tpr05_Strings and (normal) a string 
                given by pointer, length and encoding type
  
  NOTE: In case of overflow an error will be returned. 
  The behavior of strcat is undefined if the source and destination strings overlap.
    
  Arguments:
    dest        [in|out] pointer to a tpr05_String 
    srcBuf      [in]     pointer to a char * buffer containing source string 
    srcLength   [in]     length in bytes of source string
    srcEncoding [in]     encoding type of source string
  
  return value: error code
 */
 tpr05_StringError pr05IfCom_String_strcatP (tpr05_String *dest,
                                             const char *srcBuf, 
                                             tsp00_Uint4 srcLength,
                                             tpr05_StringEncoding srcEncoding)
{
    tsp78ConversionResult ConvRes          = sp78_Ok;
    tsp00_Uint4           destBytesWritten = 0;
    tsp00_Uint4           srcBytesParsed   = 0;
    
    /*copy & convert*/
    ConvRes = sp78convertString( dest->encodingType,
                                 &dest->rawString[dest->cbLen],
                                 (dest->cbMaxLen-dest->cbLen),
                                 &destBytesWritten,
                                 FALSE,
                                 srcEncoding,
                                 srcBuf,
                                 srcLength,
                                 &srcBytesParsed);

   dest->cbLen += destBytesWritten; 
   return pr05IfCom_String_mapErrorCode (ConvRes);
}

/*****************************************************************************/
/*!
  Function:     pr05IfCom_String_strcmp

  see also: 

  Description:  Compares two given strings
    
  Arguments:
    string1   [in] pointer to a tpr05_String 
    string2   [in] pointer to a tpr05_String 
  
  NOTE, that string2 will be converted if encoding types are different.
  
  return value: 
     return value    0: string1 identical to string2 
     return value != 0: string1 not identical to string2 
 */
tsp00_Int pr05IfCom_String_strcmp( const tpr05_String *string1, 
                                   const tpr05_String *string2 )
{
    if (string1->encodingType == string2->encodingType)
    {
        if (string1->cbLen != string2->cbLen)
        {
            return -2;
        }
        return  memcmp(string1->rawString, string2->rawString, string1->cbLen);   
    }    
    else
    /*different encoding types, string must be converted*/
    {
        tpr05_String *tmpString = NULL;
        tpr05_StringError convErg;
	tsp00_Int res;
        
        if (   string1->encodingType != sp77encodingAscii  
            && string2->encodingType != sp77encodingUTF8 )
        { 
             /*covert string1 into tmpString*/
             tmpString = pr05IfCom_String_NewDynString(string2->cbLen, string2->encodingType);
             convErg = pr05IfCom_String_Convert ( tmpString, string1);
             if (convErg == ok_epr05)
             {
                 res = memcmp(tmpString->rawString, string2->rawString, tmpString->cbLen);
             }
             else   
             {
                 res = -3;
             }
	     pr05IfCom_String_DeleteString (tmpString);
	     return res;
        }
        else
        {
             /*covert string2 into tmpString*/
             tmpString = pr05IfCom_String_NewDynString(string1->cbLen, string1->encodingType);
             convErg = pr05IfCom_String_Convert ( tmpString, string2);
             if (convErg == ok_epr05)
             {
                 res = memcmp(tmpString->rawString, string1->rawString, tmpString->cbLen);
             }
             else   
             {
                 res = -3;
             }  
	     pr05IfCom_String_DeleteString (tmpString);
	     return res;
        }
    }
}
/*****************************************************************************/
/*!
  Function:     pr05IfCom_String_RemoveLeadingSpaces

  see also: 

  Description:  removes leading spaces from string
    
  Arguments:
    string         [in|out] pointer to a tpr05_String 
  
  return value: none
 */
void pr05IfCom_String_RemoveLeadingSpaces (tpr05_String *source)
{
   tsp00_Int4  Offset = 0;
      
   while (  source->encodingType->isSpace(&source->rawString[Offset]) )
   {   
      Offset += source->encodingType->terminatorSize;
   }                                
   if (Offset != 0)
   {
     memmove( source->rawString , &source->rawString[Offset], source->cbLen-Offset );
     source->cbLen -= Offset;
   }
}

/*!
  Function:     pr05IfCom_String_toupper

  see also: 

  Description:  changes all characters to upper 
    
  Arguments:
    string         [in|out] pointer to a tpr05_String 
  
  return value: none
 */
void pr05IfCom_String_toupper (tpr05_String *str)
{
   pr05IfCom_String_toupperBuf (str->rawString, 
                                str->cbLen, 
                                str->encodingType);
}

/*!
  Function:     pr05IfCom_String_toupperBuf

  see also: 

  Description:  changes all characters in given buffer to upper 
    
  Arguments:
    rawString    [in|out] pointer to a buffer containing string
    Length       [in]     length of buffer
    encodingType [in]     encoding type of buffer
  
  return value: none
 */
void pr05IfCom_String_toupperBuf (tpr05_CharBuffer* rawString, 
                                  tsp00_Uint4 cbLen, 
                                  tpr05_StringEncoding encodingType)
{
    tsp00_Uint4 i;

/*NOTE the toUpper Function from hsp77.h will be inserted later instead of this coding*/    
    if (encodingType == sp77encodingAscii)
    {
            for (i=0; i<cbLen; i++) 
            {
                rawString[i] = toupper (rawString[i]);
            }    
    }       
    else if (encodingType == sp77encodingUCS2 )
    {
        sp81UCS2StringToupper( rawString, cbLen);
    }
    else if (encodingType == sp77encodingUCS2Swapped)
    {
        sp81UCS2SwappedStringToupper( rawString, cbLen);
    }
    else if (encodingType == sp77encodingUTF8)
    {
        tsp00_Byte *pByte;
        tsp83UTF8_ConversionResult erg = sp83UTF8StringToUpper( (tsp00_Byte*) rawString, 
                                                                &pByte,
                                                                (unsigned int) cbLen );   
    }
    else
        sqlabort ();
}
/*!
  Function:     pr05IfCom_String_ClearString

  see also: 

  Description:  makes a empty tpr05_String without release memory
    
  Arguments:
    String [in] pointer to a tpr05_String 
  
  return value: void
 */

void pr05IfCom_String_ClearString(tpr05_String *String)
{
   if (String->allocationType = allocated_epr05)
   {
     memset (String->rawString,0,String->cbLen);     
   }
   else 
   {
     String->rawString = NULL; 
     String->cbMaxLen = 0;
   }
   String->cbLen = 0; 
}

/*!
  Function: pr05IfCom_errString

  returns: a string representation of a tpr05_StringError
 */
const char * pr05IfCom_errString (tpr05_StringError procResult)
{
    switch (procResult)
    {
        case (ok_epr05):
          return        "Ok";
        case (procError_epr05):
          return        "ProcessError";
        case (SourceExhausted_epr05):
          return        "SourceExhausted";
        case (SourceCorrupted_epr05):
          return        "SourceCorrupted";
        case (TargetExhausted_epr05):
          return        "TargetExhausted";
        case (BadSourceEncoding_epr05):
          return        "BadSourceEncoding";
        case (BadTargetEncoding_epr05):
          return        "BadTargetEncoding";
        case (NullArgument_epr05):
          return        "NullArgument";
        default:
          return        "unknownError";
    };
}


/*!
  Function: pr05IfCom_encTypeString

  returns: a string representation of a tpr05_StringEncoding
 */
const char * pr05IfCom_encTypeString (tpr05_StringEncoding encodingType)
{
    if (encodingType == sp77encodingAscii)
      return "ASCII";
    else if (encodingType == sp77encodingUCS2)
      return "UCS2";
    else if (encodingType == sp77encodingUCS2Swapped)
      return "UCS2Swapped";
    else if (encodingType == sp77encodingUTF8)
      return "UTF8";
    else
      return "unkown";  
}

/*!
  Function:     pr05IfCom_String_CharCompare

  see also: 

  Description:  compares unicode character with specified character in tpr05_String
    
  Arguments:
    String       [in]     pointer to a tpr05_String
    index        [in]     index of unicode character in String
    chr          [in]     unicode character to compare
    chrEncType   [in]     encoding type of unicode character
  
  return value: 
     return value    0: character String[index] identical to chr 
     return value != 0: character String[index] not identical to chr 
 */
tsp00_Int pr05IfCom_String_CharCompare (tpr05_String *String, 
                                        tsp00_Uint4 index,
                                        char *chr, 
                                        tpr05_StringEncoding chrEncType)
{
  int cmpres;
  tsp00_Uint4 offset;
  char *ip;

  if (String->encodingType == sp77encodingAscii)
    offset = 1;
  else if ((String->encodingType == sp77encodingUCS2) ||
           (String->encodingType == sp77encodingUCS2Swapped))
    offset = 2;
  else
    return -2;
  
  if (chrEncType == sp77encodingUTF8)
    return -2;

  if (offset*index >= String->cbLen)
    return -3;

  ip = &String->rawString[offset*index];

  cmpres = 0;
  if (String->encodingType == chrEncType) {
    cmpres = memcmp (ip, chr, offset);
  } else if (chrEncType == sp77encodingAscii) {
    if (String->encodingType == sp77encodingUCS2Swapped) {
      if (ip[1] == '\0') {
        cmpres = memcmp (ip, chr, sizeof (char));
      } else
        cmpres = -1;
    } else {
      if (ip[0] == '\0') {
        cmpres = memcmp (ip+1, chr, sizeof (char));
      } else
        cmpres = -1;
    }
  } else if (chrEncType == sp77encodingUCS2) {
    if (String->encodingType == sp77encodingUCS2Swapped) {
      char map[2];
      map[0] = ip[1], map[1] = ip[0];
      cmpres = memcmp (map, chr, offset);
    } else {
      if (chr[1] == '\0') {
        cmpres = memcmp (ip, chr, sizeof (char));
      } else
        cmpres = -1;
    }
  } else if (chrEncType == sp77encodingUCS2Swapped) {
    if (String->encodingType == sp77encodingUCS2) {
      char map[2];
      map[0] = ip[1], map[1] = ip[0];
      cmpres = memcmp (map, chr, offset);
    } else {
      if (chr[1] == '\0') {
        cmpres = memcmp (ip, chr, sizeof (char));
      } else
        cmpres = -1;
    }
  }

  return cmpres;
}

/*!
  Function:     pr05IfCom_String_AsciiCharCompare

  see also: 

  Description:  compares ascii character with specified character in tpr05_String
    
  Arguments:
    String       [in]     pointer to a tpr05_String
    index        [in]     index of unicode character in String
    chr          [in]     ascii character to compare
  
  return value: 
     return value    0: character String[index] identical to ascii chr 
     return value != 0: character String[index] not identical to ascii chr 
 */
tsp00_Int pr05IfCom_String_AsciiCharCompare (tpr05_String *String, 
                                             tsp00_Uint4 index, 
                                             char *chr) 
{
  return pr05IfCom_String_CharCompare (String, index, chr, sp77encodingAscii);
}


/*!
  Function:     pr05IfCom_String_isSpace

  see also: 

  Description:  checks, whether specified character is a space character
    
  Arguments:
    String       [in]     pointer to a tpr05_String
    index        [in]     index of unicode character in String
  
  return value: 
     return value    0: character String[index] is space character 
     return value != 0: character String[index] is no space character
 */
tsp00_Int pr05IfCom_String_isSpace (tpr05_String *String, 
                                    tsp00_Uint4   index)
{
  const tsp00_Uint4 charLen = String->encodingType->fixedCharacterSize;

  if (charLen == 0)      /* variable character size not implemented */
    return -2;
  
  if (charLen*index > String->cbLen)
    return -3;

  return (String->encodingType->isSpace 
          ( &((char*)String->rawString)[charLen*index] ));
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
