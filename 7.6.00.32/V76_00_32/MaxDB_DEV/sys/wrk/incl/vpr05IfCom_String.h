/*!***************************************************************************

  module      : vpr05IfCom_String.h

  -------------------------------------------------------------------------

  responsible : Marco Paskamp (marco.paskamp@sap.com)

  special area:
  description : universal string definition for Interfaces Group

    Defines a general string type based on buffer, length and encoding
    type. Provides fundamental string function for this special string
    type.

  last changed: 2000-08-03  17:16
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

#ifndef VPR05IFCOM_STRING
#define VPR05IFCOM_STRING


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "hsp77.h" 		/* vsp77sprintf & Unicodetypen */
#include "hsp81.h"      /* UCS2 - string routinen      */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*!
  Decleration: tpr05_StringError
  Description: string processing error codes
 */
typedef enum tpr05_StringError
{
  ok_epr05,                 /* Processing sucessfull */
  procError_epr05,          /* error durring string processing*/
  SourceExhausted_epr05,    /* last character only partial in source buffer */
  SourceCorrupted_epr05,    /* source contains invalid chars or char sequences */
  TargetExhausted_epr05,    /* not enaugh room in target for conversion */
  BadSourceEncoding_epr05,  /* unsupported source encoding */
  BadTargetEncoding_epr05,  /* unsupported destination encoding */
  NullArgument_epr05        /* NULL pointer parsed */
} tpr05_StringError;

/*!
  Declaration: tpr05_AllocationType
  Description: type of memory allocation

  unknown_epr05     - allocation type unknown
  allocated_epr05   - dynamic allocated
  constant_epr05    - constant, can not reallocated
 */
typedef enum tpr05_AllocationType
{
  unknown_epr05,    /* allocation type unknown */
  allocated_epr05,  /* dynamic allocated */
  constant_epr05    /* constant, can not reallocated */
} tpr05_AllocationType;

/*!
  Declaration: tpr05_encoding
  Description: type for storing encoding type (sp77encodingUCS2, sp77encodingUTF8 ...)
               defined in hsp77.h
 */
typedef const tsp77encoding* tpr05_StringEncoding;

/*!
  Declaration: tpr05_CharBuffer
  Description: a buffer for strings - independend of its encoding type
               (ASCII, UCS2 ...)
  NOTE: For internal use only!!! The buffer only contains raw string data
        without zero terminator, length and encoding type.
 */
typedef char tpr05_CharBuffer;

/*!
  Declaration: tpr05_String
  Description: universal string type

  tpr05_String is a struct with the following members:
      <UL>
        <LI>tpr05_CharBuffer* rawString         contains the string data encoded by encodingType
                                                without zero terminator
        <LI>tpr05_StringEncoding encodingType   encoding type (ASCII, UCS2...) of rawString
        <LI>tsp00_Uint4 cbLen                    current used length of rawString in Bytes
    	<LI>tsp00_Uint4 cbMaxLen;	            maximum length for rawString
    	<LI>tpr05_AllocationType allocationType type of memory allocation for rawString
    </UL>

	NOTE, that if allocationType is constant the rawString pointer is a fixed length pointer
	and can't realloc automaticly.
 */

typedef struct tpr05_String_struct
{
  tpr05_CharBuffer* rawString;         /* contains the string data encoded by encodingType
                                          without zero terminator */
  tpr05_StringEncoding encodingType;   /* encoding type (ASCII, UCS2...) of rawString*/
  tsp00_Uint4 cbLen;                   /* current used length of rawString in Bytes */
  tsp00_Uint4 cbMaxLen;                /* maximum length for rawString */
  tpr05_AllocationType allocationType; /* type of memory allocation for rawString */
} tpr05_String;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*!
  Function:     pr05IfCom_String_InitString

  see also:

  Description:  initalisizes a given tpr05_String

  Returns a pointer to String. If the given String not exists a new
  will be allocated.

  NOTE The String will be initialized with the parameters - length_in_bytes,
  encoding type and allocation type. If given length_in_bytes is Null the
  rawString of tpr05_String is also null. The default encoding type is
  "sp77encodingAscii" and if no allocation type is set the default
  allocation type is constant.

  Arguments:
    tpr05_String    [in|out] pointer to string that will be initialized
    rawString       [in]     pointer to the source string
    length_in_bytes [in]     length of rawString in bytes
    encodingtype    [in]     encoding type of rawString (z.B. sp77encodingUCS2)
    allocationType  [in]     type of memory allocation

  return value: pointer to the initialized tpr05_String or NULL (in case of error)
 */
tpr05_String* pr05IfCom_String_InitString(tpr05_String* pString,
                                          void *rawString,
                                          tsp00_Uint4 length_in_bytes,
                                          tpr05_StringEncoding encodingType,
                                          tpr05_AllocationType allocationType);

/*!
  Function:     pr05IfCom_String_NewConstString

  see also:

  Description:  Creates and Initializes a new constant tpr05_String

  Allocates and initializes a new tpr05_String that points to a constant
  string. Returns a pointer to the new String.

  NOTE The new tpr05String->rawString points to the given constString. The String
  will be initialized with the parameters - length_in_bytes and encoding type. If given
  the length_in_bytes is Null the  rawString of tpr05_String is also null.
  The default encoding type is "sp77encodingAscii".

  Arguments:
    constString     [in] pointer to a string
    length_in_bytes [in] length of rawString in bytes
    encodingType    [in] encoding type of rawString (z.B. sp77encodingUCS2)

  return value: pointer to the initialized tpr05_String or NULL (in case of error)
 */
tpr05_String* pr05IfCom_String_NewConstString(void *constString,
                                              tsp00_Uint4 length_in_bytes,
                                              tpr05_StringEncoding encodingType);

/*!
  Function:     pr05IfCom_String_NewDynString

  see also:

  Description:  Creates and Initializes a new dynamic allocated tpr05_String

  Allocates and initializes a new tpr05_String. Returns a pointer the new String.

  NOTE The String will be initialized with the parameters - length_in_bytes,
  and encoding type. If given length_in_bytes is Null the rawString of tpr05_String
  is also null. The default encoding type is "sp77encodingAscii" and the allocation type
  is allocated_epr05.

  Arguments:
    length_in_bytes [in]     length of rawString in bytes
    encodingType   [in]     encoding type of rawString (z.B. sp77encodingUCS2)

  return value: pointer to the initialized tpr05_String or NULL (in case of error)
 */
tpr05_String* pr05IfCom_String_NewDynString(tsp00_Uint4 length_in_bytes,
                                            tpr05_StringEncoding encodingType);

/*!
  Function:     pr05IfCom_String_DeleteString

  see also:

  Description:  deletes a tpr05_String and frees memory

  Arguments:
    String [in] pointer to a tpr05_String

  return value: void
 */

void pr05IfCom_String_DeleteString(tpr05_String *String);

/*!
  Function:     pr05IfCom_String_DeleterawString

  see also:

  Description:  deletes the rawString of tpr05_String if needed.

  Arguments:
    String [in] pointer to a tpr05_String

  return value: void
 */

void pr05IfCom_String_DeleterawString(tpr05_String *String);

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
                                             const tpr05_String *source);

/*!
  Function:     pr05IfCom_String_ConvertP

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
                                             tpr05_StringEncoding srcEncoding);
/*!
  Function:     pr05IfCom_String_strlen

  see also:

  Description:  gives length of string

  Arguments:
    string         [in] pointer to a tpr05_String

  return value: length
 */
/* unsigned int pr05IfCom_String_strlen (const tpr05_String *str); */

/*!
  Function:     pr05IfCom_String_strcpy

  see also:

  Description:  copies content of string source to string dest

  Arguments:
    source         [in]  pointer to a tpr05_String
    dest           [out] pointer to a tpr05_String

  return value: error code
 */
tpr05_StringError  pr05IfCom_String_strcpy (      tpr05_String *dest,
                                            const tpr05_String *source);

/*!
  Function:     pr05IfCom_String_strcat

  see also:

  Description:  concatenates strings

  NOTE: In case of overflow an error will be returned.
  The behavior of strcat is undefined if the source and destination strings overlap.

  Arguments:
    dest           [in|out] pointer to a tpr05_String
    str            [in]     pointer to a tpr05_String

  return value: error code
 */
tpr05_StringError pr05IfCom_String_strcat (      tpr05_String *dest,
                                           const tpr05_String *str);
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
                                            tpr05_StringEncoding srcEncoding);

/*!
  Function:     pr05IfCom_String_strcmp

  see also:

  Description:  Compares two given strings

  Arguments:
    string1   [in] pointer to a tpr05_String
    string2   [in] pointer to a tpr05_String

  NOTE, that string2 will be converted if encoding types are different.

  return value:
     return value    0: string1 identical to string2 <BR>
     return value != 0: string1 not identical to string2
 */
tsp00_Int pr05IfCom_String_strcmp( const tpr05_String *string1,
                                   const tpr05_String *string2 );


/*!
  Function:     pr05IfCom_String_RemoveLeadingSpaces

  see also:

  Description:  removes leading spaces from string

  Arguments:
    string         [in|out] pointer to a tpr05_String

  return value: none
 */
void pr05IfCom_String_RemoveLeadingSpaces (tpr05_String *str);

/*!
  Function:     pr05IfCom_String_toupper

  see also:

  Description:  changes all characters to upper

  Arguments:
    string         [in|out] pointer to a tpr05_String

  return value: none
 */
void pr05IfCom_String_toupper (tpr05_String *str);

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
void pr05IfCom_String_toupperBuf (tpr05_CharBuffer *rawString,
                                  tsp00_Uint4 Length,
                                  tpr05_StringEncoding encodingType);

/*!
  Function:     pr05IfCom_String_ClearString

  see also:

  Description:  makes a empty tpr05_String without release memory

  Arguments:
    String [in] pointer to a tpr05_String

  return value: void
 */

void pr05IfCom_String_ClearString(tpr05_String *String);

/*!
  Function: pr05IfCom_errString

  returns: a string representation of a tpr05_StringError
 */
const char * pr05IfCom_errString (tpr05_StringError procResult);

/*!
  Function: pr05IfCom_encTypeString

  returns: a string representation of a tpr05_StringEncoding
 */
const char * pr05IfCom_encTypeString (tpr05_StringEncoding encodingType);

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
                                        tpr05_StringEncoding chrEncType);

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
                                             char *chr);


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
                                    tsp00_Uint4   index);

#endif  /* VPR05IFCOM_STRING */




