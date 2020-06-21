  /*!***************************************************************************

  module:       Tools_UTF8Basis.hpp

  ------------------------------------------------------------------------------

  responsible:  JoergM
  original author:  Till Luhmann (D026414 - till.luhmann@sap.com)

  Joerg Mensing added
  
    - KernelConvertFromUTF16,KernelConvertToUTF16
    - Bytelen
    - ConvertToASCII,ConvertFromASCII
    - ASCII specific types

    KernelConvert{From,To}UTF16 was needed due to unaligned UCS2
    characters with externally defined swap type!!!!

    Bytelen allows to get byte length and length in number of 
    UTF8 character for a given UTF8 string with a given limit
    to the number of characters parsed. (Needed for sp77sprintfUnicode)

  special area: SAPDB Common

  description:  UTF-8 basis functionality

  see also:     

  ------------------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG




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




  *****************************************************************************/



#ifndef Tools_UTF8BASIS_HPP
#define Tools_UTF8BASIS_HPP


#include "hsp81.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*!*****************************************************************************

    class: Tools_UTF8Basis

    description: Static class providing utf-8 basis functionality

*******************************************************************************/
class Tools_UTF8Basis
{
public:
    /*!-------------------------------------------------------------------------
    declaration : UTF8Pointer
    description:  Pointer to utf-8 element
    --------------------------------------------------------------------------*/
    typedef SAPDB_UTF8*                                 UTF8Pointer;
    /*!-------------------------------------------------------------------------
    declaration : UTF8ConstPointer
    description:  Iterator to constant utf-8 element
    --------------------------------------------------------------------------*/
    typedef const SAPDB_UTF8*                           UTF8ConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : UTF8Iterator
    description:  Iterator of utf-8 elements
    --------------------------------------------------------------------------*/
    typedef UTF8Pointer                                 UTF8Iterator;
    /*!-------------------------------------------------------------------------
    declaration : UTF8ConstIterator
    description:  Iterator of constant utf-8 elements
    --------------------------------------------------------------------------*/
    typedef UTF8ConstPointer                            UTF8ConstIterator;

    /*!-------------------------------------------------------------------------
    declaration : UTF16Pointer
    description:  Pointer to utf-16 element
    --------------------------------------------------------------------------*/
    typedef SAPDB_UTF16*                                UTF16Pointer;
    /*!-------------------------------------------------------------------------
    declaration : UTF16ConstPointer
    description:  Pointer to constant utf-16 element
    --------------------------------------------------------------------------*/
    typedef const SAPDB_UTF16*                          UTF16ConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : UTF16Iterator
    description:  Iterator of utf-16 elements
    --------------------------------------------------------------------------*/
    typedef UTF16Pointer                                UTF16Iterator;
    /*!-------------------------------------------------------------------------
    declaration : UTF16ConstIterator
    description:  Iterator of constant utf-16 elements
    --------------------------------------------------------------------------*/
    typedef UTF16ConstPointer                           UTF16ConstIterator;

    /*!-------------------------------------------------------------------------
    declaration : UCS2Pointer
    description:  Pointer to ucs-2 element
    --------------------------------------------------------------------------*/
    typedef SAPDB_UCS2*                                 UCS2Pointer;
    /*!-------------------------------------------------------------------------
    declaration : UCS2ConstPointer
    description:  Pointer to constant ucs-2 element
    --------------------------------------------------------------------------*/
    typedef const SAPDB_UCS2*                           UCS2ConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : UCS2Iterator
    description:  Iterator of ucs-2 elements
    --------------------------------------------------------------------------*/
    typedef UCS2Pointer                                 UCS2Iterator;
    /*!-------------------------------------------------------------------------
    declaration : UCS2ConstIterator
    description:  Iterator of constant ucs-2 elements
    --------------------------------------------------------------------------*/
    typedef UCS2ConstPointer                            UCS2ConstIterator;

    /*!-------------------------------------------------------------------------
    declaration : UCS4Pointer
    description:  Pointer to ucs-4 element
    --------------------------------------------------------------------------*/
    typedef SAPDB_UCS4*                                 UCS4Pointer;
    /*!-------------------------------------------------------------------------
    declaration : UCS4ConstPointer
    description:  Pointer to constant ucs-4 element
    --------------------------------------------------------------------------*/
    typedef const SAPDB_UCS4*                           UCS4ConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : UCS4Iterator
    description:  Iterator of ucs-4 elements
    --------------------------------------------------------------------------*/
    typedef UCS4Pointer                                 UCS4Iterator;
    /*!-------------------------------------------------------------------------
    declaration : UCS4ConstIterator
    description:  Iterator of constant ucs-4 elements
    --------------------------------------------------------------------------*/
    typedef UCS4ConstPointer                            UCS4ConstIterator;
    /*!-------------------------------------------------------------------------
    declaration : UCS2Char
    description:  UCS2 character element
    --------------------------------------------------------------------------*/
//    union UCS2Char { SAPDB_UInt2 s; SAPDB_Byte c[2]; };
    typedef tsp81_UCS2Char                              UCS2Char;
    //---------------------------------------------------------------------------
    // Joerg Mensings extensions begin here....
    //---------------------------------------------------------------------------

    /*!-------------------------------------------------------------------------
    declaration : ASCIIPointer
    description:  Pointer to 8-Bit ASCII(ISO8859-1) element
    --------------------------------------------------------------------------*/
    typedef SAPDB_Char*                                 ASCIIPointer;
    /*!-------------------------------------------------------------------------
    declaration : ASCIIConstPointer
    description:  Pointer to constant 8-Bit ASCII(ISO8859-1) element
    --------------------------------------------------------------------------*/
    typedef const SAPDB_Char*                           ASCIIConstPointer;
    /*!-------------------------------------------------------------------------
    declaration : ASCIIIterator
    description:  Iterator of 8-Bit ASCII(ISO8859-1) element
    --------------------------------------------------------------------------*/
    typedef ASCIIPointer                                ASCIIIterator;
    /*!-------------------------------------------------------------------------
    declaration : ASCIIConstIterator
    description:  constant Iterator of 8-Bit ASCII(ISO8859-1) element
    --------------------------------------------------------------------------*/
    typedef ASCIIConstPointer                           ASCIIConstIterator;

    //---------------------------------------------------------------------------
    // Joerg Mensings extensions end here....
    //---------------------------------------------------------------------------


    typedef enum 
    {
	    Success,            // conversion successful
	    SourceExhausted,    // partial character in source, but hit end
        SourceCorrupted,    // source contains invalid chars or char sequences
	    TargetExhausted	    // insuff. room in target for conversion
    } 
        ConversionResult;

    /*!-------------------------------------------------------------------------
    function:     CharacterSize()
    description:  Returns the size of the next utf8 character

                  - The size in bytes of the utf8 character located
                    at 'ptr' is returned. 
                  - The pointer must be positioned at the first byte of an 
                    utf8 character.
    arguments:    ptr   [in]    pointer to utf8 character
    return value: size in bytes
    --------------------------------------------------------------------------*/
    static SAPDB_UInt CharacterSize
        (UTF8ConstPointer               ptr)
    {
        // may not be a trailing byte in multibyte utf8 character:
        // SAPDBERR_ASSERT_ARGUMENT(ElementSize[*ptr] != 0);
        return ElementSize[*ptr];
    }

    /*!-------------------------------------------------------------------------
    function:     ReverseCharacterSize()
    description:  Returns the size of the previous utf8 character

                  - The size in bytes of the utf8 character ending
                    before 'ptr' is returned.
                  - The pointer must be positioned behind the last byte of the
                    utf8 character.
    arguments:    ptr   [in]    pointer behind utf8 character
    return value: size in bytes
    --------------------------------------------------------------------------*/
    static SAPDB_UInt ReverseCharacterSize
        (UTF8ConstPointer               ptr)
    {
        --ptr;
        // may not be leading byte in multibyte utf8 character:
        // SAPDBERR_ASSERT_ARGUMENT(ElementSize[*ptr] < 2);

        while (ElementSize[*ptr] == 0)          // while trailing byte
            --ptr;

        return ElementSize[*ptr];
    }

    /* -----------------------------------------------------------------------*/
    /*! chapter: Conversion functions                                         */
    /*!-------------------------------------------------------------------------
    function:     ConvertFromUTF16()
    description:  Converts utf-16 string to utf-8 string

                  - Source string is located between 'srcBeg' end 'srcEnd'.
                  - Destination is written to location behind 'destBeg'.
                  - On function return, 'srcAt' and 'destAt' point behind the last
                    character processed in source and destination, respectively.

    arguments:    srcBeg  [in]  pointer to begin of source field
                  scrEnd  [in]  pointer behind end of source field
                  srcAt   [out] pointer to position in source after function returned
                  destBeg [in]  pointer to begin of destination buffer
                  destEnd [in]  pointer behind end of destination buffer
                  destAt  [out] pointer to position in dest buffer after function returned
    return value: (ConversionResult)
	              - Success:            conversion successful
	              - SourceExhausted:    partial character in source, but hit end
	              - TargetExhausted:    insuffient space in target for conversion
    --------------------------------------------------------------------------*/
    static ConversionResult ConvertFromUTF16
        (const UTF16ConstPointer&       srcBeg,
         const UTF16ConstPointer&       srcEnd,
         UTF16ConstPointer&             srcAt,
         const UTF8Pointer&             destBeg,
         const UTF8ConstPointer&        destEnd,
         UTF8Pointer&                   destAt);

    /*!-------------------------------------------------------------------------
    function:     ConvertToUTF16()
    description:  Converts utf-8 string to utf-16 string
                  
                  - see ConvertFromUTF16()

    arguments:    - see ConvertFromUTF16()
    return value: (ConversionResult)
	              - Success:            conversion successful
	              - SourceExhausted:    partial character in source, but hit end
                  - SourceCorrupted:    source contains invalid chars or char sequences
	              - TargetExhausted:    insuffient space in target for conversion
    --------------------------------------------------------------------------*/
    static ConversionResult ConvertToUTF16
        (const UTF8ConstPointer&        srcBeg,
         const UTF8ConstPointer&        srcEnd,
               UTF8ConstPointer&        srcAt,
         const UTF16Pointer&            destBeg,
         const UTF16ConstPointer&       destEnd,
               UTF16Pointer&            destAt);

    /*!-------------------------------------------------------------------------
    function:     ConvertFromUCS2()
    description:  Converts ucs-2 string to utf-8 string

                  - Source string is located between 'srcBeg' end 'srcEnd'.
                  - Destination is written to location behind 'destBeg'.
                  - On function return, 'srcAt' and 'destAt' point behind the last
                    character processed in source and destination, respectively.

    arguments:    srcBeg  [in]  pointer to begin of source field
                  scrEnd  [in]  pointer behind end of source field
                  srcAt   [out] pointer to position in source after function returned
                  destBeg [in]  pointer to begin of destination buffer
                  destEnd [in]  pointer behind end of destination buffer
                  destAt  [out] pointer to position in dest buffer after function returned
    return value: (ConversionResult)
	              - Success:            conversion successful
	              - SourceExhausted:    partial character in source, but hit end
	              - TargetExhausted:    insuffient space in target for conversion
    --------------------------------------------------------------------------*/
    static ConversionResult ConvertFromUCS2
        (const UCS2ConstPointer&        srcBeg,
         const UCS2ConstPointer&        srcEnd,
         UCS2ConstPointer&              srcAt,
         const UTF8Pointer&             destBeg,
         const UTF8ConstPointer&        destEnd,
         UTF8Pointer&                   destAt)
    {
        return ConvertFromUTF16(srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertToUCS2()
    description:  Converts utf-8 string to ucs-2 string
                  
                  - see ConvertFromUCS2()

    arguments:    - see ConvertFromUCS2()
    return value: (ConversionResult)
	              - Success:            conversion successful
	              - SourceExhausted:    partial character in source, but hit end
                  - SourceCorrupted:    source contains invalid chars or char sequences
	              - TargetExhausted:    insuffient space in target for conversion
    --------------------------------------------------------------------------*/
    static ConversionResult ConvertToUCS2
        (const UTF8ConstPointer&        srcBeg,
         const UTF8ConstPointer&        srcEnd,
         UTF8ConstPointer&              srcAt,
         const UCS2Pointer&             destBeg,
         const UCS2ConstPointer&        destEnd,
         UCS2Pointer&                   destAt)
    {
        return ConvertToUTF16(srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);
    }

    /*!-------------------------------------------------------------------------
    function:     ConvertFromUCS4()
    description:  Converts ucs-4 string to utf-8 string
                  
                  - see ConvertFromUTF16()

    arguments:    - see ConvertFromUTF16()
    return value: (ConversionResult)
	              - Success:            conversion successful
	              - TargetExhausted:    insuffient space in target for conversion
    --------------------------------------------------------------------------*/
    static ConversionResult ConvertFromUCS4
        (const UCS4ConstPointer&        srcBeg,
         const UCS4ConstPointer&        srcEnd,
         UCS4ConstPointer&              srcAt,
         const UTF8Pointer&             destBeg,
         const UTF8ConstPointer&        destEnd,
         UTF8Pointer&                   destAt);

    /*!-------------------------------------------------------------------------
    function:     ConvertToUCS4()
    description:  Converts utf-8 string to ucs-4 string
                  
                  - see ConvertFromUTF16()

    arguments:    - see ConvertFromUTF16()
    return value: (ConversionResult)
	              - Success:            conversion successful
	              - SourceExhausted:    partial character in source, but hit end
                  - SourceCorrupted:    source contains invalid chars or char sequences
	              - TargetExhausted:    insuffient space in target for conversion
    --------------------------------------------------------------------------*/
    static ConversionResult ConvertToUCS4
        (const UTF8ConstPointer&        srcBeg,
         const UTF8ConstPointer&        srcEnd,
         UTF8ConstPointer&              srcAt,
         const UCS4Pointer&             destBeg,
         const UCS4ConstPointer&        destEnd,
         UCS4Pointer&                   destAt);

    //---------------------------------------------------------------------------
    // Joerg Mensings extensions(part2) begins here....
    //---------------------------------------------------------------------------

    /*!-------------------------------------------------------------------------
    function:     KernelConvertFromUTF16()
    description:  Converts utf-16 string to utf-8 string supporting 'SAPDB UCS2'

                  - Allows unaligned UCS2 strings
                  - Needs UCS2 swap type specification 'srcSwapped'
                  - 'false' (Highbyte,Lowbyte) 
                  - 'true' (LowByte,HighByte)
                  - see ConvertFromUTF16() for rest of description

    arguments:    srcBeg  [in]  pointer to begin of source field
                  scrEnd  [in]  pointer behind end of source field
                  srcAt   [out] pointer to position in source after function returned
                  srcSwapped [in] bool defining swap type (see above)
                  destBeg [in]  pointer to begin of destination buffer
                  destEnd [in]  pointer behind end of destination buffer
                  destAt  [out] pointer to position in dest buffer after function returned
    return value: (ConversionResult)
	              - Success:            conversion successful
	              - SourceExhausted:    partial character in source, but hit end
	              - TargetExhausted:    insuffient space in target for conversion
    --------------------------------------------------------------------------*/

    static ConversionResult KernelConvertFromUTF16
        (const UCS2Char               * srcBeg,
         const UCS2Char               * srcEnd,
               UCS2Char const       * & srcAt,
         const SAPDB_Int                srcSwapped,
         const UTF8Pointer&             destBeg,
         const UTF8ConstPointer&        destEnd,
         UTF8Pointer&                   destAt);

    /*!-------------------------------------------------------------------------
    function:     KernelConvertToUTF16()
    description:  Converts utf-8 string to utf-16 string supporting 'SAPDB UCS2'

                  - Allows unaligned UCS2 strings
                  - Needs UCS2 swap type specification 'destSwapped'
                  - 'false' (Highbyte,Lowbyte) 
                  - 'true' (LowByte,HighByte)
                  - see ConvertFromUTF16() for rest of description

    arguments:    srcBeg  [in]  pointer to begin of source field
                  scrEnd  [in]  pointer behind end of source field
                  srcAt   [out] pointer to position in source after function returned
                  destBeg [in]  pointer to begin of destination buffer
                  destEnd [in]  pointer behind end of destination buffer
                  destAt  [out] pointer to position in dest buffer after function returned
                  destSwapped [in] bool defining swap type

    return value: (ConversionResult)
	              - Success:            conversion successful
	              - SourceExhausted:    partial character in source, but hit end
                  - SourceCorrupted:    source contains invalid chars or char sequences
	              - TargetExhausted:    insuffient space in target for conversion
    --------------------------------------------------------------------------*/
    static ConversionResult KernelConvertToUTF16
        (const UTF8ConstPointer&        srcBeg,
         const UTF8ConstPointer&        srcEnd,
               UTF8ConstPointer&        srcAt,
               UCS2Char         *       destBeg,
               UCS2Char         *       destEnd,
               UCS2Char         * &     destAt,
         const SAPDB_Int                destSwapped);

    /*!-------------------------------------------------------------------------
    function:     Bytelen()
    description:  calculates number of characters and bytes needed for a given
                  number of characters

    Name was choosen, to make clear that the call differs from 'strlen'...

    This function replaces string length function, if you specify charCount large
    enough (i.e. your buffer size...) since it stop with first ZERO termination
    character found in src string. You can decide on using byte length or number
    of UTF8 characters from the result, since both are returned.

    arguments:    srcBeg [in] the string to parse
                  srcEnd [in] the last+1 pointer
                  srcAt [out] the last+1 scanned position
                  srcLimit [in] maximum number of UTF8 characters to count
                  byteLength [out] the number of bytes needed for charCountUTF8 Characters
                  charCountUTF8 [out] the number of UTF8 Characters in string
    return value: (ConversionResult)
	            - Success:            conversion successful
	            - SourceExhausted:    partial character in source, but hit end
                    - SourceCorrupted:    source contains invalid chars or char sequences
	            - TargetExhausted:    insuffient space in target for conversion
  --------------------------------------------------------------------------*/
    static ConversionResult Bytelen( const UTF8ConstPointer& srcBeg,
                                     const UTF8ConstPointer& srcEnd,
                                     UTF8ConstPointer&       srcAt,
                                     const SAPDB_UInt        srcLimit,
                                     SAPDB_UInt& byteLength,
                                     SAPDB_UInt& charCountUTF8);

    /*!-------------------------------------------------------------------------
    function:     ConvertToASCII()
    description:  Convert given UTF8 string into ASCII string
    
    ASCII string are interpreted as 8-Bit ASCII according to IS8859-LATIN1.

    If an replaceChar is given other than '\0' it is inserted for all 
    characters, that are not contained in this codepage. This is not an error!

    If '\0' is given as replaceChar non ISO8859-1 UTF8 Characters are handled as
    an error (SourceCorrupted).

    arguments:    srcBeg [in] the string to parse
                  srcEnd [in] the last+1 pointer
                  srcAt [out] the last+1 scanned position
                  destBeg [in] the target string
                  destEnd [in] the last+1 pointer into target
                  destAt [out] the last+1 written target position
                  replaceChar [in] replacement for non ISO8859-1 characters
                  replaceCount [out] Number of replacements done (not errors!!!)
    return value: (ConversionResult)
	            - Success:            conversion successful
	            - SourceExhausted:    partial character in source, but hit end
                    - SourceCorrupted:    source contains invalid chars or char sequences
	            - TargetExhausted:    insuffient space in target for conversion
  --------------------------------------------------------------------------*/
    static ConversionResult ConvertToASCII( const UTF8ConstPointer&  srcBeg,
                                            const UTF8ConstPointer&  srcEnd,
                                            UTF8ConstPointer&        srcAt,
                                            const ASCIIPointer&      destBeg,
                                            const ASCIIConstPointer& destEnd,
                                            ASCIIPointer&            destAt,
                                            const SAPDB_Char         replaceChar,
                                            SAPDB_UInt&              replaceCount );

    /*!-------------------------------------------------------------------------
    function:     ConvertFromASCII()
    description:  Convert given ASCII string into UTF8 characters
    
    ASCII string are interpreted as 8-Bit ASCII according to IS8859-LATIN1.

    arguments:    srcBeg [in] the string to parse
                  srcEnd [in] the last+1 pointer
                  srcAt [out] the last+1 scanned position
                  destBeg [in] the target string
                  destEnd [in] the last+1 pointer into target
                  destAt [out] the last+1 written target position
                  replaceChar [in] replacement for non ISO8859-1 characters
                  replaceCount [out] Number of replacements done (not errors!!!)
    return value: (ConversionResult)
	            - Success:            conversion successful
	            - SourceExhausted:    partial character in source, but hit end
                    - SourceCorrupted:    source contains invalid chars or char sequences
	            - TargetExhausted:    insuffient space in target for conversion
  --------------------------------------------------------------------------*/
    static ConversionResult ConvertFromASCII( const ASCIIConstPointer&  srcBeg,
                                              const ASCIIConstPointer&  srcEnd,
                                              ASCIIConstPointer&        srcAt,
                                              const UTF8Pointer&      destBeg,
                                              const UTF8ConstPointer& destEnd,
                                              UTF8Pointer&            destAt);

    //---------------------------------------------------------------------------
    // Joerg Mensings extensions part(2) ends here....
    //---------------------------------------------------------------------------

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Conversion functions                                      */
    /* -----------------------------------------------------------------------*/


private:
    // contains the numbers of bytes of utf8 characters
    // if 'b' is the leading byte of an utf-8 character, 
    // ElementSize[b] is the corresponding total number of bytes of the character.
    // if ElementSize[x] is 0, 'x' is not a leading utf-8 char byte.
    static SAPDB_UTF8 ElementSize[256];

    static SAPDB_UTF8 LeadingByteMark[7];

    static SAPDB_UCS4 UTF8Offsets[7];

};
/*!*****************************************************************************

    endclass: Tools_UTF8Basis

*******************************************************************************/






#endif // Tools_UTF8BASIS_HPP

