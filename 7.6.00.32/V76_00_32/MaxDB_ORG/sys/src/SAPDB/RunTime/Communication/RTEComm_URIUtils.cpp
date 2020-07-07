/*!========================================================================

  @file         RTEComm_URIUtils.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        URI utility class.

                The URI utility class is used by the URI parser and
                URI builder and contains methodes needed by one of
                these classes.

  @since        2003-11-06  17:33
  @sa           

  ==========================================================================

  \if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    <string.h> // for strlen(), strchr(), strcpy()
#include    <ctype.h> // isalnum()

#define     DECLARE_ESCAPE_CHAR_ARRAY
#include    "RunTime/Communication/RTEComm_ParseURIEscapeRFC2396.hpp"

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Communication/RTEComm_URIUtils.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Comm_Messages.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


RTEComm_URIUtils::~RTEComm_URIUtils() {;}

//
// Convert a character to it's escaped equivalent. We escape UTF8 characters
// byte wise, so 'unescaped' has not a UTF8-Type. The calling function must
// assure that there are at least three characters of escaped buffer space
// available.
inline SAPDB_Char* RTEComm_URIUtils::EscapeChar( SAPDB_Char const     unescaped,
                                                 SAPDB_Char *         escaped ) const
{
    static const SAPDB_Char EscapeCharTable[] = "0123456789ABCDEF";

    *escaped++ = '%';
    *escaped++ = EscapeCharTable[(unsigned char)unescaped >> 4];
    *escaped++ = EscapeCharTable[(unsigned char)unescaped & 0xF];

	return escaped;
}
/*---------------------------------------------------------------------------*/

//
// Convert a escaped character sequence ( %<hexdigit><hexdigit> )
// to its corresponding unescaped character. The calling function
// must assure that the escaped character string is well formed!
//
inline SAPDB_Char RTEComm_URIUtils::UnescapeChars ( SAPDB_Char const *  escaped ) const
{
    SAPDB_Char unescapedChar;

    // must start with '%' ( escaped[0] == '%' )
    unescapedChar  = (escaped[1] >= 'A' ? ((escaped[1] & 0xdf) - 'A') + 10: (escaped[1] - '0'));
    unescapedChar *= 16;
    unescapedChar += (escaped[2] >= 'A' ? ((escaped[2] & 0xdf) - 'A') + 10: (escaped[2] - '0'));

    return unescapedChar;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
    RTEComm_URIUtils::EscapingNeeded ( PartID const        partID,
                                       SAPDB_UTF8 * const  unescaped ) const
{   
	SAPDB_Char const * pSrc          = reinterpret_cast<SAPDB_Char const *>(unescaped);

	while ( *pSrc != 0 )
	{
        // Look into the escape character table if the character needs to be
        // escaped. The escaping depends on the part type ( partID ).
		if ( escapeCharsRFC2396[(unsigned char)*pSrc] & partID )
		{
			// Existing escapes are a special case
			if ((*pSrc == '%') && (isxdigit(pSrc[1]) && isxdigit(pSrc[2])))
			{
                pSrc += 2; 
			}
			else 
                return true;
		}
		++pSrc;
	}

    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_UTF8 *
    RTEComm_URIUtils::SkipAllowedCharacters  ( PartID const        partID,
                                               SAPDB_UTF8 * const  unescaped ) const
{   
	SAPDB_Char  * pSrc          = reinterpret_cast<SAPDB_Char*>(unescaped);

	while ( *pSrc != 0 )
	{
        // Look into the escape character table if the character needs to be
        // escaped. The escaping depends on the part type ( partID ).
		if ( escapeCharsRFC2396[(unsigned char)*pSrc] & partID )
		{
			// Existing escapes are a special case
			if ((*pSrc == '%') && (isxdigit(pSrc[1]) && isxdigit(pSrc[2])))
                pSrc += 2; 
			else 
                return reinterpret_cast<SAPDB_UTF8* >(pSrc);
		}
		++pSrc;
	}

    return 0;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4
    RTEComm_URIUtils::EscapeURIPart ( PartID const              partID,
                                      SAPDB_UTF8 const * const  unescaped,
                                      SAPDB_UInt2  const        escapedBufferSize,
                                      SAPDB_Char * const        escaped ) const
{   
    // We escape UTF8 characters byte wise, so 'pSrc' has not a UTF8-Type.
	SAPDB_Char const * pSrc          = reinterpret_cast<SAPDB_Char const *>(unescaped);
	SAPDB_Char       * pDst          = escaped;
    SAPDB_UInt4        escapedStrLen = 1;  // set to one for terminating zero character
    SAPDB_Bool         insufficientBufferSize = false;

	while ( *pSrc != 0 )
	{
        // Look into the escape character table if the character needs to be
        // escaped. The escaping depends on the part type ( partID ).
		if ( escapeCharsRFC2396[(unsigned char)*pSrc] & partID )
		{
			// Existing escapes are a special case
			if ((*pSrc == '%') && (isxdigit(pSrc[1]) && isxdigit(pSrc[2])))
			{
                // If pDst is null we only step over the source escape sequence
                if ( pDst == 0 ) 
                    pSrc += 2; 
                else
                {
                    if (( escapedBufferSize - escapedStrLen ) < 3 )
                    {
                        insufficientBufferSize = true;
                        break;
                    }
                    *pDst++ = *pSrc;
                    *pDst++ = *++pSrc;
                    *pDst++ = *++pSrc;
                }
                escapedStrLen += 2;
			}
			else 
			{   // If pDst is null we only count the escape sequence length
                if ( pDst != 0 ) 
                {
                    if (( escapedBufferSize - escapedStrLen ) < 3 )
                    {
                        insufficientBufferSize = true;
                        break;
                    }
                    pDst = EscapeChar( *pSrc, pDst );
                }
                escapedStrLen += 2;
            }
		}
		else if ( pDst != 0 ) // If pDst is null we only count the length
		{
            if ( 0 == ( escapedBufferSize - escapedStrLen ) )
            {
                insufficientBufferSize = true;
                break;
            }
			*pDst++ = *pSrc;
		}
        ++escapedStrLen;
		++pSrc;
	}
    if ( pDst != 0 )
    {
	    *pDst = '\0';
		SAPDBERR_ASSERT_STATE( escapedStrLen == strlen(escaped) + 1 );
    }

    return (insufficientBufferSize) ? 0 : escapedStrLen;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4
    RTEComm_URIUtils::UnescapeURI ( SAPDB_Char const * const  escaped,
                                    SAPDB_UInt2  const        unescapedBufferSize,
                                    SAPDB_UTF8 * const        unescaped ) const
{   
	SAPDB_Char const * pSrc                   = escaped;
	SAPDB_UTF8       * pDst                   = unescaped;
    SAPDB_UInt4        UnescapedStrLen        = 1;  // set to one for terminating zero character
    SAPDB_Bool         insufficientBufferSize = false;

	while ( *pSrc != 0 )
	{
        if ((*pSrc == '%') && (isxdigit(pSrc[1]) && isxdigit(pSrc[2])))
        {
            // If pDst is null we only step over the source escape sequence
            if ( pDst != 0 ) 
            {
                if ( 0 == ( unescapedBufferSize - UnescapedStrLen ))
                {
                    insufficientBufferSize = true;
                    break;
                }

                *pDst++ = UnescapeChars ( pSrc );
            }
            pSrc += 2; 
        }
        else if ( pDst != 0 ) 
        {
            if ( 0 == ( unescapedBufferSize - UnescapedStrLen ))
            {
                insufficientBufferSize = true;
                break;
            }
            *pDst++ = *pSrc;
        }
        ++UnescapedStrLen;
		++pSrc;
	}
    if ( pDst != 0 )
    {
	    *pDst = '\0';
		SAPDBERR_ASSERT_STATE( UnescapedStrLen == strlen((SAPDB_Char*)unescaped) + 1 );
    }

    return (insufficientBufferSize) ? 0 : UnescapedStrLen;
}

/*---------------------------------------------------------------------------*/

RTEComm_URIUtils::URIRet
    RTEComm_URIUtils::CreateUnescapedString( SAPDB_UTF8 *              &unescaped,
                                             SAPDB_Char const *  const esacped,
                                             SAPDBErr_MessageList      &messageList ) const
{
    URIRet retVal = NoError;

    if ( ( 0 == esacped ) || ( URI_NULL_CHAR == esacped[0] ))
        unescaped = 0;
    else
    {
        // check the number of characters needed to hold unescaped character string.
        SAPDB_UInt4  unescapedBufferSize = UnescapeURI ( esacped, 0, 0 );

        unescaped = (SAPDB_UTF8*)RTEMem_RteAllocator::Instance().Allocate(unescapedBufferSize * sizeof(SAPDB_UTF8));

        if ( 0 == unescaped )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
            retVal      = OutOfMemory;
        }
        else
        {
            // unescape string after buffer has been allocated
            unescapedBufferSize = UnescapeURI ( esacped, unescapedBufferSize, unescaped );

            SAPDBERR_ASSERT_STATE( unescapedBufferSize != 0 ); // Oops ...
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

void RTEComm_URIUtils::FreeUnescapedString( SAPDB_UTF8 *  &unescaped ) const
{

    if ( 0 != unescaped )
    {
        RTEMem_RteAllocator::Instance().Deallocate(unescaped);
        unescaped = 0;
    }
    return;
}

/*---------------------------------------------------------------------------*/
/*----------------------- RTEComm_URIPathSegmentList ------------------------*/
/*---------------------------------------------------------------------------*/

RTEComm_URIUtils::URIRet 
    RTEComm_URIPathSegmentList::Create( SAPDB_Char *  const       esacpedURIPath,
                                        SAPDBErr_MessageList      &messageList )
{
    URIRet retVal = NoError;

    // No empty string ...
    if ( ( 0 != esacpedURIPath ) && ( URI_NULL_CHAR != esacpedURIPath[0] ))
    {
        SAPDB_UInt4  bufferSize = 0;
        SAPDB_UInt4  segmentCnt = 0; 

        // Parse() messes up the string, so we make a copy first
        SAPDB_Char* tmpPath = (SAPDB_Char*)RTEMem_RteAllocator::Instance().Allocate((URILen(esacpedURIPath) + 1) * sizeof(SAPDB_Char));

        if ( 0  == tmpPath )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
            retVal      = OutOfMemory;
        }
        else
        {
            URICpy( tmpPath, esacpedURIPath );

            SAPDB_Char*  currSeg = tmpPath;
            SAPDB_Char*  nextSeg = tmpPath;

            // Step though the escaped segements, separate them and calculate the buffer
            // size needed to hold the path segment list.
            do
            {   
                if  ( 0 != ( nextSeg = URIFindChar( nextSeg, '/' )))
                {
                    nextSeg[0] = URI_NULL_CHAR; // separate segments
                    nextSeg++;

                    // a trailing slash only, no segments?
                    if ( URI_NULL_CHAR == nextSeg[0] ) 
                        nextSeg = 0;
                }
                if ( 0 != currSeg[0] ) // ignore empty path segments
                {
                    // calculate the next sements size.
                    bufferSize += SAPDB_ALIGN_8BYTE(UnescapeURI( currSeg, 0, 0 ) +
                                                    offsetof(segmentListElement, unescapedSeg));
                    segmentCnt++; 
                }
                currSeg = nextSeg;
            }
            while ( 0 != currSeg );

            // allocate buffer to hold the path segement list
            SAPDB_Byte* buffer = (SAPDB_Byte*)RTEMem_RteAllocator::Instance().Allocate(bufferSize * sizeof(SAPDB_Byte));

            if ( 0 == buffer )
            {
                messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                retVal      = OutOfMemory;
            }
            else
            {
                segmentListElement* segElement;

                m_FirstElement = segElement = reinterpret_cast<segmentListElement*>(buffer);

                // Step through the escaped separated segments and fill up the segment list
                for ( currSeg = tmpPath; 0 != segmentCnt; segmentCnt-- )
                {   
                    while ( 0 == currSeg[0] )
                        ++currSeg; // skip empty strings

                    // unescape string...
                    SAPDB_UInt4 SizeNeeded = UnescapeURI ( currSeg, bufferSize, segElement->unescapedSeg );

                    SAPDBERR_ASSERT_STATE( SizeNeeded != 0 ); // Oops ...

                    // caclulate offset to the current list element
                    SizeNeeded = SAPDB_ALIGN_8BYTE( SizeNeeded + 
                                                    offsetof(segmentListElement, unescapedSeg));

                    if ( SizeNeeded  < bufferSize )
                    {
                        bufferSize             -= SizeNeeded;  // calculate the remaining buffer size
                        buffer                 += SizeNeeded;
                        currSeg                += URILen(currSeg) + 1;
                        segElement->nextSegElem = reinterpret_cast<segmentListElement*>(buffer);
                        segElement              = segElement->nextSegElem;
                    }
                    else
                    {
                        segElement->nextSegElem = 0;
                        bufferSize              = 0;
                    }
                }
            }
            RTEMem_RteAllocator::Instance().Deallocate(tmpPath);
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

SAPDB_UTF8 const * 
    RTEComm_URIPathSegmentList::Next( SAPDB_UTF8 const * const currSegment ) const
{
    if ( 0 == currSegment ) return 0;

    segmentListElement const *  segElement;

    segElement = reinterpret_cast<segmentListElement const*>
                  ( reinterpret_cast<SAPDB_Byte const*>(currSegment) - 
                    offsetof(segmentListElement, unescapedSeg) );

    return segElement->nextSegElem ? segElement->nextSegElem->unescapedSeg : 0;
}

/*---------------------------------------------------------------------------*/

RTEComm_URIPathSegmentList::~RTEComm_URIPathSegmentList() 
{
    RTEMem_RteAllocator::Instance().Deallocate(m_FirstElement);
}

/*---------------------------------------------------------------------------*/
/*------------------------- RTEComm_URIQueryOptList -------------------------*/
/*---------------------------------------------------------------------------*/

RTEComm_URIUtils::URIRet 
    RTEComm_URIQueryOptList::Create( SAPDB_Char *  const       esacpedQuery,
                                     SAPDBErr_MessageList      &messageList )
{
    URIRet retVal = NoError;

    // No empty string ...
    if ( ( 0 != esacpedQuery ) && ( URI_NULL_CHAR != esacpedQuery[0] ))
    {
        SAPDB_UInt4  bufferSize = 0;
        SAPDB_UInt4  OptionCnt  = 0;

        // Parse() messes up the string, so we make a copy first
        SAPDB_Char* tmpQuery = (SAPDB_Char*)RTEMem_RteAllocator::Instance().Allocate((URILen(esacpedQuery) + 1) * sizeof(SAPDB_Char));

        if ( 0  == tmpQuery )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
            retVal      = OutOfMemory;
        }
        else
        {
            URICpy( tmpQuery, esacpedQuery );

            SAPDB_Char*  nextOpt = tmpQuery;
            SAPDB_Char*  currOpt = tmpQuery;
            SAPDB_Char*  currContent;
            SAPDB_UInt4  len;

            // Step though the escaped query strings, separate the options and calculate 
            // the buffer size needed to hold the option list.
            do
            {   
                if  ( 0 != ( nextOpt = URIFindChar( nextOpt, '&' )))
                {
                    nextOpt[0] = URI_NULL_CHAR; // separate options
                    nextOpt++;

                    // a trailing ampersand only, no options?
                    if ( URI_NULL_CHAR == nextOpt[0] ) 
                        nextOpt = 0;
                }

                if ( 0 != currOpt[0] ) // ignore empty strings
                {
                    if ( 0 != (currContent = URIFindChar( currOpt, '=' )))
                    {
                        currContent[0] = URI_NULL_CHAR; // separate option and content
                        currContent++;
                        len        = UnescapeURI( currOpt, 0, 0 );
                        len       += UnescapeURI( currContent, 0, 0 );
                        currContent[-1] = '=';          // restore option
                    }
                    else
                        len = UnescapeURI( currOpt, 0, 0 );

                    bufferSize += SAPDB_ALIGN_8BYTE( len + offsetof(queryOptListElement, unescapedOptName) ) ;
                    OptionCnt++; 
                }

                currOpt = nextOpt;
            }
            while ( 0 != nextOpt );

            // allocate buffer to hold the option list
            SAPDB_Byte* buffer = (SAPDB_Byte*)RTEMem_RteAllocator::Instance().Allocate(bufferSize * sizeof(SAPDB_Byte));

            if ( 0 == buffer )
            {
                messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                retVal      = OutOfMemory;
            }
            else
            {
                queryOptListElement* optElement;

                m_FirstElement = optElement = reinterpret_cast<queryOptListElement*>(buffer);

                // Step through the escaped separated option and fill up the list
                for ( currOpt = tmpQuery; 0 != OptionCnt; OptionCnt-- )
                {
                    SAPDB_UInt4 SizeNeeded;

                    while ( 0 == currOpt[0] )
                        ++currOpt; // skip empty strings

                    if ( 0 != (currContent = URIFindChar( currOpt, '=' )))
                    {
                        currContent[0] = URI_NULL_CHAR; // separate option and content
                        currContent++;
                        SizeNeeded =  UnescapeURI( currOpt, bufferSize, optElement->unescapedOptName );
                        SAPDBERR_ASSERT_STATE( SizeNeeded != 0 ); // Oops ...

                        optElement->unescapedValue = 
                                       optElement->unescapedOptName + 
                                       URILen(optElement->unescapedOptName) + 1;

                        SizeNeeded += UnescapeURI( currContent, bufferSize - SizeNeeded, 
                                                   optElement->unescapedValue );
                        SAPDBERR_ASSERT_STATE( SizeNeeded != 0 ); // Oops ...

                        currOpt  = currContent + URILen(currContent) + 1;
                    }
                    else
                    {
                        optElement->unescapedValue = 0;
                        SizeNeeded = UnescapeURI( currOpt, bufferSize, optElement->unescapedOptName );
                        SAPDBERR_ASSERT_STATE( SizeNeeded != 0 ); // Oops ...
                        currOpt  += URILen(currOpt) + 1;
                    }

                    // caclulate offset to the next option list element
                    SizeNeeded = SAPDB_ALIGN_8BYTE( SizeNeeded + 
                                                    offsetof(queryOptListElement, unescapedOptName ));

                    if ( SizeNeeded  < bufferSize )
                    {
                        bufferSize             -= SizeNeeded;  // calculate the remaining buffer size
                        buffer                 += SizeNeeded;
                        optElement->nextOptElem = reinterpret_cast<queryOptListElement*>(buffer);
                        optElement              = optElement->nextOptElem;
                    }
                    else
                    {
                        optElement->nextOptElem = 0;
                        bufferSize              = 0;
                    }
                }
            }
            RTEMem_RteAllocator::Instance().Deallocate(tmpQuery);
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

SAPDB_UTF8 const * 
    RTEComm_URIQueryOptList::Next( SAPDB_UTF8 const * const currOption,
                                   SAPDB_UTF8 const *       &optValue ) const
{
    if ( 0 == currOption ) return 0;

    queryOptListElement const *  optElement;

    optElement = reinterpret_cast<queryOptListElement const*>
                  ( reinterpret_cast<SAPDB_Byte const*>(currOption) - 
                    offsetof(queryOptListElement, unescapedOptName) );

    optValue =  optElement->nextOptElem ? optElement->nextOptElem->unescapedValue : 0;

    return optElement->nextOptElem ? optElement->nextOptElem->unescapedOptName : 0;
}

/*---------------------------------------------------------------------------*/

RTEComm_URIQueryOptList::~RTEComm_URIQueryOptList() 
{
    RTEMem_RteAllocator::Instance().Deallocate(m_FirstElement);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/