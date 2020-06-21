  /*!***************************************************************************

  module:       Tools_UTF8Basis.cpp

  ------------------------------------------------------------------------------

  responsible:  TillL

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

#include "ToolsCommon/Tools_UTF8Basis.hpp"

// constants in ucs-4 domain:
#define SAPDB_UCS4_REPLACEMENTCHAR      0x0000FFFDUL
#define SAPDB_MAX_UCS2                  0x0000FFFFUL
#define SAPDB_MAX_UTF16                 0x0010FFFFUL
#define SAPDB_MAX_UCS4                  0x7FFFFFFFUL

#define SAPDB_UCS4_HALFSHIFT            10
#define SAPDB_UCS4_HALFBASE             0x00010000UL

// constants in utf-16 domain:
#define SAPDB_UCS4_HALFMASK             0x03FFUL

#define SAPDB_UTF16_SRGT_HIGH_START     0xD800UL
#define SAPDB_UTF16_SRGT_HIGH_END       0xDBFFUL
#define SAPDB_UTF16_SRGT_LOW_START      0xDC00UL
#define SAPDB_UTF16_SRGT_LOW_END	    0xDFFFUL



/*******************************************************************************

    class: Tools_UTF8Basis

*******************************************************************************/
SAPDB_UTF8 Tools_UTF8Basis::ElementSize[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6};

//------------------------------------------------------------------------------
SAPDB_UTF8 Tools_UTF8Basis::LeadingByteMark[7] = 
    {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

//------------------------------------------------------------------------------
SAPDB_UCS4 Tools_UTF8Basis::UTF8Offsets[7] =
    {0UL, 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
          0x03C82080UL, 0xFA082080UL, 0x82082080UL};


//------------------------------------------------------------------------------
Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::ConvertFromUTF16
    (const UTF16ConstPointer&       srcBeg,
     const UTF16ConstPointer&       srcEnd,
     UTF16ConstPointer&             srcAt,
     const UTF8Pointer&             destBeg,
     const UTF8ConstPointer&        destEnd,
     UTF8Pointer&                   destAt)
{
    ConversionResult            result   = Success;
    register UTF16ConstIterator srcIter  = srcBeg;
    register UTF8Iterator       destIter = destBeg;
    register SAPDB_UCS4         ucs4char;
    register SAPDB_UCS4         utf16elem2;     // must be ucs4!
    register SAPDB_UInt         bytesToWrite;

    // loop whole utf-16 string:
    while (srcIter < srcEnd) {

        // -----------------------------------------------------
        // convert next utf-16 character to ucs-4:
        // -----------------------------------------------------
        ucs4char = *srcIter++;
        if (ucs4char >= SAPDB_UTF16_SRGT_HIGH_START && 
            ucs4char <= SAPDB_UTF16_SRGT_HIGH_END)
        {
            if (srcIter == srcEnd)
            {
                // error: 2nd utf-16 character missing in source
                result = SourceExhausted;
                --srcIter;
                break;
            }
            
            utf16elem2 = *srcIter;
            if (utf16elem2 >= SAPDB_UTF16_SRGT_LOW_START && 
                utf16elem2 <= SAPDB_UTF16_SRGT_LOW_END) 
            {
			    ucs4char = ((ucs4char - SAPDB_UTF16_SRGT_HIGH_START) << SAPDB_UCS4_HALFSHIFT) 
                         +
    	                   utf16elem2 - SAPDB_UTF16_SRGT_HIGH_END 
                         +
                           SAPDB_UCS4_HALFBASE;
                ++srcIter;
            }
        }

        // -----------------------------------------------------
        // determine byte size of next utf-8 character:
        // -----------------------------------------------------
             if (ucs4char <  0x80)           { bytesToWrite = 1; } 
        else if (ucs4char <  0x800)          { bytesToWrite = 2; } 
        else if (ucs4char <  0x10000)        { bytesToWrite = 3; } 
        else if (ucs4char <  0x200000)       { bytesToWrite = 4; } 
        else if (ucs4char <  0x4000000)      { bytesToWrite = 5; } 
        else if (ucs4char <= SAPDB_MAX_UCS4) { bytesToWrite = 6; } 
        else
            { bytesToWrite = 2; 
              ucs4char     = SAPDB_UCS4_REPLACEMENTCHAR; }

        // -----------------------------------------------------
        // check if dest buffer is exhausted:
        // -----------------------------------------------------
        if (destIter + bytesToWrite > destEnd) 
        {
            result = TargetExhausted;
            break;
        }

        destIter += bytesToWrite;
        // -----------------------------------------------------
        // write utf-8 bytes to dest:
        // -----------------------------------------------------
        switch (bytesToWrite) 
        {	// note: code falls through cases!
            case 6:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 5:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 4:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 3:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 2:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 1:	*--destIter =  ucs4char | LeadingByteMark[bytesToWrite];
        }
        destIter += bytesToWrite;
    }

    srcAt  = srcIter;
    destAt = destIter;

    return result;
}

//------------------------------------------------------------------------------
Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::KernelConvertFromUTF16
    (const       UCS2Char         * srcBeg,
     const       UCS2Char         * srcEnd,
                 UCS2Char const   * & srcAt,
     const SAPDB_Int                srcSwapped,
     const UTF8Pointer&             destBeg,
     const UTF8ConstPointer&        destEnd,
     UTF8Pointer&                   destAt)
{
    ConversionResult            result = Success;
    register       UCS2Char   * srcIter  = const_cast<UCS2Char *>(srcBeg);
    register UTF8Iterator       destIter = destBeg;
    register SAPDB_UCS4         ucs4char;
    register SAPDB_UCS4         utf16elem2;     // must be ucs4!
    register SAPDB_UInt         bytesToWrite;
    int firstIndex = ( srcSwapped ? 0 : 1 );

    // loop whole utf-16 string:
    while (srcIter < srcEnd) {

        // -----------------------------------------------------
        // convert next utf-16 character to ucs-4:
        // -----------------------------------------------------
        ucs4char = srcIter->c[firstIndex] + (srcIter->c[1-firstIndex]<<8);
        ++srcIter;

        if (ucs4char >= SAPDB_UTF16_SRGT_HIGH_START && 
            ucs4char <= SAPDB_UTF16_SRGT_HIGH_END)
        {
            if (srcIter == srcEnd)
            {
                // error: 2nd utf-16 character missing in source
                result = SourceExhausted;
                --srcIter;
                break;
            }

            utf16elem2 = srcIter->c[firstIndex] + (srcIter->c[1-firstIndex]<<8);
            if (utf16elem2 >= SAPDB_UTF16_SRGT_LOW_START && 
                utf16elem2 <= SAPDB_UTF16_SRGT_LOW_END) 
            {
                ucs4char = ((ucs4char - SAPDB_UTF16_SRGT_HIGH_START) << SAPDB_UCS4_HALFSHIFT) 
                         +
    	                   utf16elem2 - SAPDB_UTF16_SRGT_HIGH_END 
                         +
                           SAPDB_UCS4_HALFBASE;
                ++srcIter;
            }
        }

        // -----------------------------------------------------
        // determine byte size of next utf-8 character:
        // -----------------------------------------------------
             if (ucs4char <  0x80)           { bytesToWrite = 1; } 
        else if (ucs4char <  0x800)          { bytesToWrite = 2; } 
        else if (ucs4char <  0x10000)        { bytesToWrite = 3; } 
        else if (ucs4char <  0x200000)       { bytesToWrite = 4; } 
        else if (ucs4char <  0x4000000)      { bytesToWrite = 5; } 
        else if (ucs4char <= SAPDB_MAX_UCS4) { bytesToWrite = 6; } 
        else
            { bytesToWrite = 2; 
              ucs4char     = SAPDB_UCS4_REPLACEMENTCHAR; }

        // -----------------------------------------------------
        // check if dest buffer is exhausted:
        // -----------------------------------------------------
        if (destIter + bytesToWrite > destEnd) 
        {
            result = TargetExhausted;
            break;
        }

        destIter += bytesToWrite;
        // -----------------------------------------------------
        // write utf-8 bytes to dest:
        // -----------------------------------------------------
        switch (bytesToWrite) 
        {	// note: code falls through cases!
            case 6:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 5:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 4:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 3:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 2:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 1:	*--destIter =  ucs4char | LeadingByteMark[bytesToWrite];
        }
        destIter += bytesToWrite;
    }

    srcAt  = srcIter;
    destAt = destIter;

    return result;
}

//------------------------------------------------------------------------------
Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::ConvertToUTF16
    (const UTF8ConstPointer&        srcBeg,
     const UTF8ConstPointer&        srcEnd,
     UTF8ConstPointer&              srcAt,
     const UTF16Pointer&            destBeg,
     const UTF16ConstPointer&       destEnd,
     UTF16Pointer&                  destAt)
{
    ConversionResult            result    = Success;
    register UTF8ConstIterator  srcIter   = srcBeg;
    register UTF16Iterator      destIter  = destBeg;
    register SAPDB_UCS4         ucs4char;
    register SAPDB_UInt         bytesToWrite;

    // loop whole utf-8 string:
    while (srcIter < srcEnd)
    {
        ucs4char    = 0;
        bytesToWrite = ElementSize[*srcIter];

        if (bytesToWrite == 0)
        {
            // error: utf-8 string corrupted
            result = SourceCorrupted;
            break;
        }

        if (srcIter + bytesToWrite > srcEnd) 
        {
            // error: utf-8 bytes missing in source
            result = SourceExhausted; 
            break;
        }

        switch(bytesToWrite) 
        {	// note: code falls through cases!
        case 6:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 5:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 4:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 3:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 2:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 1:	ucs4char += *srcIter++; break;
        }

        ucs4char -= UTF8Offsets[bytesToWrite];

        if (ucs4char <= SAPDB_MAX_UCS2) 
        {
            if (destIter >= destEnd) 
            {
                // dest buffer exhausted:
                result = TargetExhausted; 
                break;
            }
            *destIter++ = static_cast<SAPDB_UTF16>(ucs4char);
        } 
        else if (ucs4char > SAPDB_MAX_UTF16) 
        {
            if (destIter >= destEnd) 
            {
                // dest buffer exhausted:
                result = TargetExhausted; 
                break;
            }
            *destIter++ = static_cast<SAPDB_UTF16>(SAPDB_UCS4_REPLACEMENTCHAR);
        } 
        else 
        {
            if (destIter + 1 >= destEnd) 
            {
                // dest buffer exhausted:
                result = TargetExhausted; 
                break;
            }
            ucs4char  -= SAPDB_UCS4_HALFBASE;

            *destIter++ = static_cast<SAPDB_UTF16>
                          ((ucs4char >> SAPDB_UCS4_HALFSHIFT) 
                           +
                           SAPDB_UTF16_SRGT_HIGH_START);

            *destIter++ = static_cast<SAPDB_UTF16>
                          ((ucs4char & SAPDB_UCS4_HALFMASK) 
                           + 
                           SAPDB_UTF16_SRGT_LOW_START);
        }
    }

    srcAt  = srcIter;
    destAt = destIter;

    return result;
}

//------------------------------------------------------------------------------
Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::KernelConvertToUTF16
    (const UTF8ConstPointer&        srcBeg,
     const UTF8ConstPointer&        srcEnd,
     UTF8ConstPointer&              srcAt,
           UCS2Char         *       destBeg,
           UCS2Char         *       destEnd,
           UCS2Char         * &     destAt,
     const SAPDB_Int                destSwapped)
{
    ConversionResult            result    = Success;
    register UTF8ConstIterator  srcIter   = srcBeg;
    register       UCS2Char *   destIter  = destBeg;
    register SAPDB_UCS4         ucs4char;
    register SAPDB_UInt         bytesToWrite;
    int                firstIndex = ( destSwapped ? 0 : 1 );

    // loop whole utf-8 string:
    while (srcIter < srcEnd)
    {
        bytesToWrite = ElementSize[*srcIter];

        if (bytesToWrite == 0)
        {
            // error: utf-8 string corrupted
            result = SourceCorrupted;
            break;
        }

        if (srcIter + bytesToWrite > srcEnd) 
        {
            // error: utf-8 bytes missing in source
            result = SourceExhausted; 
            break;
        }

        ucs4char    = 0;

        switch(bytesToWrite) 
        {	// note: code falls through cases!
        case 6:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 5:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 4:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 3:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 2:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 1:	ucs4char += *srcIter++; break;
        }

        ucs4char -= UTF8Offsets[bytesToWrite];

        if (ucs4char > SAPDB_MAX_UTF16)
        {
            ucs4char = SAPDB_UCS4_REPLACEMENTCHAR;
        }

        if (ucs4char <= SAPDB_MAX_UCS2) 
        {
            if (destIter >= destEnd) 
            {
                // dest buffer exhausted:
                result = TargetExhausted; 
                break;
            }
            destIter->c[firstIndex] = static_cast<SAPDB_UInt1>(ucs4char & 0xFF);
            destIter->c[1-firstIndex] = static_cast<SAPDB_UInt1>((ucs4char>>8) & 0xFF);
            ++destIter;
        } 
        else 
        {
            if (destIter + 1 >= destEnd) 
            {
                // dest buffer exhausted:
                result = TargetExhausted; 
                break;
            }
            ucs4char  -= SAPDB_UCS4_HALFBASE;

            SAPDB_UTF16 copyUTF16;

            copyUTF16   = static_cast<SAPDB_UTF16>
                          ((ucs4char >> SAPDB_UCS4_HALFSHIFT) 
                           +
                           SAPDB_UTF16_SRGT_HIGH_START);

            destIter->c[firstIndex] = static_cast<SAPDB_UInt1>(copyUTF16 & 0xFF);
            destIter->c[1-firstIndex] = static_cast<SAPDB_UInt1>((copyUTF16>>8) & 0xFF);
            ++destIter;

            copyUTF16   = static_cast<SAPDB_UTF16>
                          ((ucs4char & SAPDB_UCS4_HALFMASK) 
                           + 
                           SAPDB_UTF16_SRGT_LOW_START);

            destIter->c[firstIndex] = static_cast<SAPDB_UInt1>(copyUTF16 & 0xFF);
            destIter->c[1-firstIndex] = static_cast<SAPDB_UInt1>((copyUTF16>>8) & 0xFF);
            ++destIter;
        }
    }

    srcAt  = srcIter;
    destAt = destIter;

    return result;
}

//------------------------------------------------------------------------------
Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::ConvertFromUCS4
    (const UCS4ConstPointer&        srcBeg,
     const UCS4ConstPointer&        srcEnd,
     UCS4ConstPointer&              srcAt,
     const UTF8Pointer&             destBeg,
     const UTF8ConstPointer&        destEnd,
     UTF8Pointer&                   destAt)
{
    ConversionResult            result   = Success;
    register UCS4ConstIterator  srcIter  = srcBeg;
    register UTF8Iterator       destIter = destBeg;
    register SAPDB_UCS4         ucs4char;
    register SAPDB_UInt         bytesToWrite;

    // loop whole ucs-4 string:
    while (srcIter < srcEnd) {

        // -----------------------------------------------------
        // get next ucs-4 char:
        // -----------------------------------------------------
        ucs4char = *srcIter++;

        // -----------------------------------------------------
        // determine byte size of next utf-8 character:
        // -----------------------------------------------------
             if (ucs4char <  0x80)           { bytesToWrite = 1; } 
        else if (ucs4char <  0x800)          { bytesToWrite = 2; } 
        else if (ucs4char <  0x10000)        { bytesToWrite = 3; } 
        else if (ucs4char <  0x200000)       { bytesToWrite = 4; } 
        else if (ucs4char <  0x4000000)      { bytesToWrite = 5; } 
        else if (ucs4char <= SAPDB_MAX_UCS4) { bytesToWrite = 6; } 
        else
            { bytesToWrite = 2; 
              ucs4char     = SAPDB_UCS4_REPLACEMENTCHAR; }

        // -----------------------------------------------------
        // check if dest buffer is exhausted:
        // -----------------------------------------------------
        if (destIter + bytesToWrite > destEnd) 
        {
            result = TargetExhausted;
            break;
        }

        destIter += bytesToWrite;
        // -----------------------------------------------------
        // write utf-8 bytes to dest:
        // -----------------------------------------------------
        switch (bytesToWrite) 
        {	// note: code falls through cases!
            case 6:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 5:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 4:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 3:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 2:	*--destIter = (ucs4char | 0x80) & 0xBF; ucs4char >>= 6;
            case 1:	*--destIter =  ucs4char | LeadingByteMark[bytesToWrite];
        }
        destIter += bytesToWrite;
    }

    srcAt  = srcIter;
    destAt = destIter;

    return result;
}

//------------------------------------------------------------------------------
Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::ConvertToUCS4
    (const UTF8ConstPointer&        srcBeg,
     const UTF8ConstPointer&        srcEnd,
     UTF8ConstPointer&              srcAt,
     const UCS4Pointer&             destBeg,
     const UCS4ConstPointer&        destEnd,
     UCS4Pointer&                   destAt)
{
    ConversionResult            result    = Success;
    register UTF8ConstIterator  srcIter   = srcBeg;
    register UCS4Iterator       destIter  = destBeg;
    register SAPDB_UCS4         ucs4char;
    register SAPDB_UInt         bytesToWrite;

    // loop whole utf-8 string:
    while (srcIter < srcEnd)
    {
        ucs4char    = 0;
        bytesToWrite = ElementSize[*srcIter];

        if (bytesToWrite == 0)
        {
            // error: utf-8 string corrupted
            result = SourceCorrupted;
            break;
        }

        if (srcIter + bytesToWrite > srcEnd) 
        {
            // error: utf-8 bytes missing in source
            result = SourceExhausted; 
            break;
        }

        switch(bytesToWrite) 
        {	// note: code falls through cases!
        case 6:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 5:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 4:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 3:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 2:	ucs4char += *srcIter++; ucs4char <<= 6;
        case 1:	ucs4char += *srcIter++; break;
        }
        
        ucs4char -= UTF8Offsets[bytesToWrite];

        if (destIter >= destEnd) 
        {
            // dest buffer exhausted:
            result = TargetExhausted; 
            break;
        }

        if (ucs4char > SAPDB_MAX_UCS4) 
        {
            *destIter++ = SAPDB_UCS4_REPLACEMENTCHAR;
        } 
        else 
        {
            *destIter++ = ucs4char;
        }
    }

    srcAt  = srcIter;
    destAt = destIter;

    return result;
}

//------------------------------------------------------------------------------
Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::Bytelen(const UTF8ConstPointer& srcBeg,
                                                           const UTF8ConstPointer& srcEnd,
                                                           UTF8ConstPointer&       srcAt,
                                                           const SAPDB_UInt        srcLimit,
                                                           SAPDB_UInt&  charCountUTF8,
                                                           SAPDB_UInt&  byteCountUTF8)
{
  register SAPDB_UInt         bytesToWrite;
  ConversionResult            result    = Success;
  register SAPDB_UInt         byteCount = 0;
  register SAPDB_UInt         charCount = 0;
  register UTF8ConstIterator  srcIter   = srcBeg;
  register SAPDB_UInt         maxParse  = srcLimit;

  // loop whole utf-8 string:
  while (srcIter < srcEnd && maxParse-- > 0 )
  {
    if ( *srcIter == 0 )
    {
      /* zero termination detected */
      break;
    }

    bytesToWrite = ElementSize[*srcIter];

    if (bytesToWrite == 0)
    {
        // error: utf-8 string corrupted
        result = SourceCorrupted;
        break;
    }

    if (srcIter + bytesToWrite > srcEnd) 
    {
        // error: utf-8 bytes missing in source
        result = SourceExhausted; 
        break;
    }

    ++charCount;
      byteCount += bytesToWrite;

      srcIter += bytesToWrite;
  }

  srcAt         = srcIter;
  charCountUTF8 = charCount;
  byteCountUTF8 = byteCount;

  return result;
}

/*---------------------------------------------------------------------------*/

Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::ConvertFromASCII(
    const ASCIIConstPointer&  srcBeg,
    const ASCIIConstPointer&  srcEnd,
    ASCIIConstPointer&        srcAt,
    const UTF8Pointer&      destBeg,
    const UTF8ConstPointer& destEnd,
    UTF8Pointer&            destAt)
{
  ConversionResult            result   = Success;
  register ASCIIConstIterator srcIter  = srcBeg;
  register UTF8Iterator       destIter = destBeg;
  register SAPDB_Char         asciiChar;

  // loop whole ASCII string:
  while (srcIter < srcEnd) 
  {

    // -----------------------------------------------------
    // check if dest buffer is exhausted:
    // -----------------------------------------------------
    if (destIter + 1 > destEnd) 
    {
      result = TargetExhausted;
      break;
    }

    // -----------------------------------------------------
    // get next ASCII char:
    // -----------------------------------------------------
    asciiChar = *srcIter++;

    if ( (asciiChar & 0x80) == 0 )
    {
      *destIter++ = asciiChar;
    }
    else
    {
      // -----------------------------------------------------
      // check if dest buffer is exhausted:
      // -----------------------------------------------------
      if (destIter + 2 > destEnd) 
      {
        result = TargetExhausted;
        break;
      }

      // -----------------------------------------------------
      // split ascii character into two UTF8 Characters
      // -----------------------------------------------------
      destIter[1] = 0x80 | ( asciiChar & 0x3F );
      destIter[0] = LeadingByteMark[2] | ( ( asciiChar&0xFF ) >> 6 ) ;
      destIter += 2;
    }
  }

  srcAt  = srcIter;
  destAt = destIter;

  return result;
}

/*---------------------------------------------------------------------------*/
Tools_UTF8Basis::ConversionResult Tools_UTF8Basis::ConvertToASCII(
    const UTF8ConstPointer&  srcBeg,
    const UTF8ConstPointer&  srcEnd,
    UTF8ConstPointer&        srcAt,
    const ASCIIPointer&      destBeg,
    const ASCIIConstPointer& destEnd,
    ASCIIPointer&            destAt,
    const SAPDB_Char         replaceChar,
    SAPDB_UInt&              replaceCount )
{
  register SAPDB_UInt         bytesToWrite;
  ConversionResult            result   = Success;
  register UTF8ConstIterator  srcIter  = srcBeg;
  register ASCIIIterator      destIter = destBeg;
  register SAPDB_UTF8         utf8char;

  replaceCount = 0;

  // loop whole utf-8 string:
  while (srcIter < srcEnd) 
  {
    // -----------------------------------------------------
    // check if dest buffer is exhausted:
    // -----------------------------------------------------
    if (destIter + 1 > destEnd) 
    {
      result = TargetExhausted;
      break;
    }

    bytesToWrite = ElementSize[*srcIter];

    if ( bytesToWrite == 0 
      || (srcIter + bytesToWrite) > srcEnd )
    {
      // error: utf-8 string corrupted
      result = SourceCorrupted;
      break;
    }

    if (bytesToWrite == 1)
    {
      *destIter++ = *srcIter;
    }
    else
    {
      // -----------------------------------------------------
      // get next UTF8 char:
      // -----------------------------------------------------
      utf8char = *srcIter;

      if ( utf8char > 0xC3 ) /* Ascii limitation !!! */
      {
        if ( replaceChar != 0 )
        {
          *destIter++ = replaceChar;
          replaceCount++;
        }
        else
        {
          result = SourceCorrupted;
          break;
        }
      }
      else
      {
        if ( utf8char == LeadingByteMark[bytesToWrite] )
        {
          /* 2 byte sequence for single sequence character ... */
          result = SourceCorrupted;
          break;
        }

        if ( ( srcIter[1] & LeadingByteMark[bytesToWrite] ) != 0x80 )
        {
          /* indicate correct error position */
          ++srcIter;
          result = SourceCorrupted;
          break;
        }

        *destIter++ = ((utf8char & 0x3) << 6) | (srcIter[1] & 0x3F) ;
      }
    }

    srcIter += bytesToWrite;
  }

  srcAt  = srcIter;
  destAt = destIter;

  return result;
}
