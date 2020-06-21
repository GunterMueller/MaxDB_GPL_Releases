/*!========================================================================

  @file         RTEComm_URIUtils.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        URI utility class.

                The URI utility class is used by the URI parser and
                URI builder and contains methodes needed by one of
                these classes.

  @since        2004-04-16  12:06
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


#ifndef RTECOMM_URIUTILS_HPP
#define RTECOMM_URIUTILS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <ctype.h>

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/Communication/RTEComm_ParseURIEscapeRFC2396.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define URI_NULL_CHAR   '\0'

#define SCHEME_LIVECACHE      "liveCache"
#define SCHEME_MAXDB          "MAXDB"

#define PROTOCOL_LOCAL        "local"
#define PROTOCOL_REMOTE       "remote"
#define PROTOCOL_REMOTESSL    "remotes"
#define PROTOCOL_SAPNI        "sapni"
#define PROTOCOL_SAPNISSL     "sapnis"
#define PROTOCOL_SAPNI_END    ":inpas"

#define PATH_DATABASE         "database"
#define PATH_DBMSERVER        "dbmserver"
#define PATH_PROCSERVER       "procserver"
#define PATH_LISTENER         "listener"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define STRCHR (char*)strchr // we dont need constnes on some plattforms

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! 
  @class        RTEComm_URIUtils
  @brief        URI utility class.

                The URI utility class is used by the URI parser and
                URI builder and contains methodes needed by one ore both 
                of these classes.

*/
class RTEComm_URIUtils
{
public:
    //
    // DON'T FORGET SAPDB_ToStringClass BELOW!!!!
    //
    enum URIRet { NoError = 0,        // No error.
                  CommonError,        // error occured.
                  ProtocolParseError, // Error parsing protocol.
                  LocationParseError, // Location part is not well formed.
                  PathParseError,     // Path part is not well formed.
                  QueryParseError,    // Query part is not well formed.
                  OutOfMemory };      // No more memory to complete operation.

    //
    // DON'T FORGET SAPDB_ToStringClass BELOW!!!!
    //
    enum PartID { URIPart          = ESC_URI,           // The whole URI string
                  SchemePart       = ESC_SCHEME,        // scheme part   ( liveCache: | maxdb: )
                  ProtnamePart     = ESC_PROTNAME,      // protocol part ( local: | http: .. )
                  LocationPart     = ESC_LOCATION,      // Location part ( e.g. //<hostname:<port>/DATABASE/... )
                  HostPart         = ESC_HOST,          // host part     ( <hostname part>:<port> )
                  HostnamePart     = ESC_HOSTNAME,      // hostname part ( <hostname> | >ip_addr> )
                  PathPart         = ESC_PATH,          // path part     ( /database/" <serverdb> .. )
                  PathContentPart  = ESC_PATH_CONTENT,  // content of path between slash separators 
                  QueryPart        = ESC_QUERY,         // query part    ( ? <option> "=" <value> )
                  QueryContentPart = ESC_QUERY_CONTENT, // content of query between separators like '?', '&'
                  FragmentPart     = ESC_FRAGMENT};     // fragment    ( # <any>> )

protected:
    /*!
      @brief    contructor / destructor of URI Utility class
     */
    RTEComm_URIUtils()  {;}
    virtual  ~RTEComm_URIUtils();

    /*!
      @brief    Check if URI part needes to be escaped.

      Check if an unescaped URI part string 
      needs to be escaped described in RFC2396.

      @param    partID             [in]   - the URI part specifier.
      @param    unescaped          [in]   - unescaped URI part string.
      @return   true if escaping needed
     */
    SAPDB_Bool
        EscapingNeeded ( PartID const       partID,
                         SAPDB_UTF8 * const unescaped ) const;

    /*!
      @brief    Skip URI part allowed characters.

      Skip all allowed characters of an URI part string. Stop on
      the firt character which needs to be escaped.

      @param    partID             [in]   - the URI part specifier.
      @param    unescaped          [in]   - unescaped URI part string.
      @return   Pointer to the first character which needs to escaped,
                zero if there is no.
     */
    SAPDB_UTF8*
        SkipAllowedCharacters ( PartID const       partID,
                                SAPDB_UTF8 * const unescaped ) const;

    /*!
      @brief    Skip URI part allowed characters.

      Skip all allowed characters of an URI part string. Stop on
      the firt character which needs to be escaped.

      @param    partID             [in]   - the URI part specifier.
      @param    unescaped          [in]   - unescaped URI part string.
      @return   Pointer to the first character which needs to escaped,
                zero if there is no.
     */
    SAPDB_Char*
        SkipAllowedCharacters ( PartID const       partID,
                                SAPDB_Char * const unescaped ) const
    {
        return reinterpret_cast<SAPDB_Char*>( SkipAllowedCharacters(partID, (SAPDB_UTF8*)unescaped) );
    }

    /*!
      @brief    Escape a URI part string conform to RFC2396.

      Calling this methode with an unescaped URI part string leads
      to an escaped character string which is conform to RFC2396.
      If no output buffer is given (NULL) which is normaly used to
      hold the escaped character string, the methode only returns
      the output buffer size needed.

      @param    partID             [in]   - the URI part specifier.
      @param    unescaped          [in]   - unescaped URI part string.
      @param    escapedBufferSize  [in]   - optional argument specifying the ouput buffersize.
      @param    escaped            [out]  - optional argument, output buffer holding the 
                                            escaped URI part string. If the value is null
                                            this methode only returns the minimum size needed 
                                            for the escaped output buffer.
      @return   on error = 0 else
                the minimum size needed for the escaped output buffer
     */
    SAPDB_UInt4
        EscapeURIPart ( PartID const             partID,
                        SAPDB_UTF8 const * const unescaped,
                        SAPDB_UInt2 const        escapedBufferSize = 0,
                        SAPDB_Char * const       escaped           = 0 ) const;

    /*!
      @brief    Unescape a URI string.

      Calling this methode with an escaped URI string leads
      to an unescaped character string.
      If no output buffer is given (NULL) which is normaly used to
      hold the unescaped character string, the methode only returns
      the output buffer size needed.

      @param    escaped              [in]   - escaped URI string.
      @param    unescapedBufferSize  [in]   - optional argument specifying the ouput buffersize.
      @param    unescaped            [out]  - optional argument, output buffer holding the 
                                              unescaped URI string. If the value is null
                                              this methode only returns the minimum size needed 
                                              for the unescaped output buffer.
      @return   on error = 0 else
                the minimum size needed for the unescaped output buffer
     */
    SAPDB_UInt4
        UnescapeURI               ( SAPDB_Char const * const  escaped,
                                    SAPDB_UInt2  const        unescapedBufferSize = 0,
                                    SAPDB_UTF8 * const        unescaped           = 0 ) const;
    /*!
      @brief    URI string length.

      Returns the URI string length.

      @param    uri              [in]   - URI string.

      @return   length of string
     */
    SAPDB_UInt4 URILen( SAPDB_UTF8 const * uri ) const
    {
        return static_cast<SAPDB_UInt4>( strlen((SAPDB_Char const *)uri) );
    }

    /*!
      @brief    URI string length.

      Returns the URI string length.

      @param    uri              [in]   - URI string.

      @return   length of string
     */
    SAPDB_UInt4 URILen( SAPDB_Char const * const uri ) const
    {
        return static_cast<SAPDB_UInt4>( strlen(uri) );
    }

    /*!
      @brief    Copy URI string.

      Copy a URI string from source to destination buffer.

      @param    uriDst           [out]  - URI string destination buffer.
      @param    uriSrc           [in]   - URI string source buffer.

      @return   pointer to the destination string
     */
    SAPDB_UTF8* URICpy( SAPDB_UTF8* uriDst, SAPDB_UTF8 const * const uriSrc ) const
    {
        return reinterpret_cast<SAPDB_UTF8*>( strcpy(reinterpret_cast<SAPDB_Char*>(uriDst),
                                                     (SAPDB_Char const *)uriSrc) );
    }

    /*!
      @brief    Copy URI string.

      Copy a URI string from source to destination buffer.

      @param    uriDst           [out]  - URI string destination buffer.
      @param    uriSrc           [in]   - URI string source buffer.

      @return   pointer to the destination string
     */
    SAPDB_Char* URICpy( SAPDB_Char* uriDst, SAPDB_UTF8 const *  const uriSrc ) const
    {
        return strcpy(uriDst, (SAPDB_Char const *)uriSrc);
    }

    /*!
      @brief    Copy URI string.

      Copy a URI string from source to destination buffer.

      @param    uriDst           [out]  - URI string destination buffer.
      @param    uriSrc           [in]   - URI string source buffer.

      @return   pointer to the destination string
     */
    SAPDB_UTF8* URICpy( SAPDB_UTF8* uriDst, SAPDB_Char const * const uriSrc ) const
    {
        return reinterpret_cast<SAPDB_UTF8*>( strcpy(reinterpret_cast<SAPDB_Char*>(uriDst), uriSrc) );
    }

    /*!
      @brief    Copy URI string.

      Copy a URI string from source to destination buffer.

      @param    uriDst           [out]  - URI string destination buffer.
      @param    uriSrc           [in]   - URI string source buffer.

      @return   pointer to the destination string
     */
    SAPDB_Char* URICpy( SAPDB_Char* uriDst, SAPDB_Char const * const uriSrc ) const
    {
        return strcpy(uriDst, uriSrc);
    }

    /*!
      @brief    Trim a URI string.

      Calling this methode does a inplace URI string trimming.

      @param    uri              [in]   - URI string.

      @return   pointer to trimmed URI string
     */
    inline SAPDB_UTF8* URITrim( SAPDB_UTF8 * const uri ) const;

    /*!
      @brief    Trim a URI string.

      Calling this methode does a inplace URI string trimming.

      @param    uri              [in]   - URI string.

      @return   pointer to trimmed URI string
     */
    SAPDB_Char* URITrim( SAPDB_Char * const uri ) const
    { 
        return reinterpret_cast<SAPDB_Char*>(URITrim((SAPDB_UTF8*)uri)); 
    };

    /*!
      @brief    Find a substring in a URI string.

      Find a substring in a URI string from left to right.

      @param    uri              [in]   - URI string.
      @param    substr           [in]   - Substring to be located. 

      @return   A pointer to the first occurrence of 'substr' 
                in string, or NULL if 'substr' is not found.
     */
    SAPDB_UTF8  * const URIFindStr( SAPDB_UTF8 * const uri, SAPDB_Char const * const substr ) const
    { 
        return (SAPDB_UTF8 *)URIFindStr((SAPDB_Char *)uri, substr); 
    };

    /*!
      @brief    Find a substring in a URI string.

      Find a substring in a URI string from left to right.

      @param    uri              [in]   - URI string.
      @param    substr           [in]   - Substring to be located. 

      @return   A pointer to the first occurrence of 'substr' 
                in string, or NULL if 'substr' is not found.
     */
    inline SAPDB_Char * const URIFindStr( SAPDB_Char * const uri, SAPDB_Char const * const substr ) const;

    /*!
      @brief    Find a character in a URI string.

      Find a character in a URI string from left to right.

      @param    uri              [in]   - URI string.
      @param    character        [in]   - Character to be located. 

      @return   A pointer to the first occurrence of 'character' 
                in string, or NULL if 'character' is not found.
     */
    SAPDB_Char * const URIFindChar( SAPDB_Char const * const uri, SAPDB_Char character ) const
    { 
        return STRCHR(uri, tolower(character) ); 
    };
    
    /*!
      @brief    Find a character in a URI string.

      Find a character in a URI string from left to right.

      @param    uri              [in]   - URI string.
      @param    character        [in]   - Character to be located. 

      @return   A pointer to the first occurrence of 'character' 
                in string, or NULL if 'character' is not found.
     */
    SAPDB_UTF8 * const URIFindChar( SAPDB_UTF8 const * const uri, SAPDB_Char const character ) const
    { 
        return (SAPDB_UTF8 *)STRCHR((SAPDB_Char const *)uri, character); 
    };

    /*!
      @brief    Compare URI strings.

      Compare two URI strings, ignore case.

      @param    uri1           [in]  - First URI string.
      @param    uri2           [in]  - Second URI string.

      @return   true indicates that uri1 is identical to uri2 else false.
     */
    inline SAPDB_Bool URIsIdentical( SAPDB_Char const * uri1, SAPDB_Char const * uri2, SAPDB_UInt4 len = 0 ) const;

    /*!
      @brief    Compare URI strings.

      Compare two URI strings, ignore case.

      @param    uri1           [in]  - First URI string.
      @param    uri2           [in]  - Second URI string.

      @return   true indicates that uri1 is identical to uri2 else false.
     */
    SAPDB_Bool URIsIdentical( SAPDB_UTF8 const * uri1, SAPDB_UTF8 const * uri2, SAPDB_UInt4 len = 0 ) const
    {
        return URIsIdentical((SAPDB_Char const *)uri1,
                             (SAPDB_Char const *)uri2, len );
    }


    /*!
      @brief    Compare URI strings.

      Compare two URI strings, ignore case.

      @param    uri1           [in]  - First URI string.
      @param    uri2           [in]  - Second URI string.

      @return   true indicates that uri1 is identical to uri2 else false.
     */
    SAPDB_Bool URIsIdentical( SAPDB_UTF8 const * uri1, SAPDB_Char const * uri2, SAPDB_UInt4 len = 0 ) const
    {
        return URIsIdentical((SAPDB_Char const *)uri1, uri2, len);
    }

    /*!
      @brief    Build an unescaped UTF8 character string.

      @param    unescaped   [out]  - On ouput an buffer has been
                                     allocated holding an unescaped
                                     UTF8 character string. The
                                     caller has to free this buffer
                                     via 'FreeUnescapedString' if 
                                     needed!
                esacped       [in] - The URI string which might contain 
                                     escaped characters in the form of %XX

      @return   NoError, OutOfMemory.
      */
    URIRet     CreateUnescapedString( SAPDB_UTF8 *              &unescaped,
                                      SAPDB_Char const * const  esacped,
                                      SAPDBErr_MessageList      &messageList ) const;

    void FreeUnescapedString( SAPDB_UTF8 *  &unescaped ) const;

private:
    // don't allow a copy
	inline   RTEComm_URIUtils( const RTEComm_URIUtils &c ) {;}; 

    // Convert a character to it's escaped equivalent. We escape UTF8 characters
    // byte wise, so 'unescaped' has not a UTF8-Type. The calling function must
    // assure that there are at least three characters of escaped buffer space
    // available.
    inline SAPDB_Char* 
        EscapeChar( SAPDB_Char const   unescaped, 
                    SAPDB_Char *       escaped ) const;

    //
    // Convert a escaped character sequence ( %<hexdigit><hexdigit> )
    // to its corresponding unescaped character. The calling function
    // must assure that the escaped character string is well formed!
    //
    inline SAPDB_Char 
        UnescapeChars ( SAPDB_Char const *  escaped ) const;
};


/*!
  @brief convert RTEComm_URIUtils::URIRet into readable string
  @param state [in] the return value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString( RTEComm_URIUtils::URIRet retVal )
{
    switch (retVal)
    {
    case RTEComm_URIUtils::NoError              : return SAPDB_ToStringClass("No error");
    case RTEComm_URIUtils::CommonError          : return SAPDB_ToStringClass("Common error");
    case RTEComm_URIUtils::ProtocolParseError   : return SAPDB_ToStringClass("Protocol parse error");
    case RTEComm_URIUtils::LocationParseError   : return SAPDB_ToStringClass("Location parse error");
    case RTEComm_URIUtils::PathParseError       : return SAPDB_ToStringClass("Path parse error");
    case RTEComm_URIUtils::QueryParseError      : return SAPDB_ToStringClass("Query parse error");
    case RTEComm_URIUtils::OutOfMemory          : return SAPDB_ToStringClass("Out of memory");
    }

    return SAPDB_ToStringClass((SAPDB_Int4)retVal);
}

/*!
  @brief convert RTEComm_URIUtils::URIRet into readable string
  @param state [in] the return value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString( RTEComm_URIUtils::PartID partID )
{
    switch (partID)
    {
    case RTEComm_URIUtils::URIPart          : return SAPDB_ToStringClass("URI");
    case RTEComm_URIUtils::SchemePart       : return SAPDB_ToStringClass("Scheme");
    case RTEComm_URIUtils::ProtnamePart     : return SAPDB_ToStringClass("Protocol name");
    case RTEComm_URIUtils::LocationPart     : return SAPDB_ToStringClass("Location ");
    case RTEComm_URIUtils::HostPart         : return SAPDB_ToStringClass("Host");
    case RTEComm_URIUtils::HostnamePart     : return SAPDB_ToStringClass("Host name");
    case RTEComm_URIUtils::PathPart         : return SAPDB_ToStringClass("Path");
    case RTEComm_URIUtils::PathContentPart  : return SAPDB_ToStringClass("Path content");
    case RTEComm_URIUtils::QueryPart        : return SAPDB_ToStringClass("Query");
    case RTEComm_URIUtils::QueryContentPart : return SAPDB_ToStringClass("Query content");
    case RTEComm_URIUtils::FragmentPart     : return SAPDB_ToStringClass("Fragment");
    }
    return SAPDB_ToStringClass((SAPDB_Int4)partID);
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UTF8* RTEComm_URIUtils::URITrim( SAPDB_UTF8 * const uri ) const
{
    SAPDB_Int4  lenURI = URILen( uri ) - 1;

    while ((lenURI >= 0) && ( uri[lenURI] == ' ') ) --lenURI;

    uri[++lenURI] = 0;

    if ( *uri == ' ' )
    {
        SAPDB_UTF8 *src = uri;
        SAPDB_UTF8 *dst = uri;

        while ( *src == ' ' ) ++src;

        if ( src != dst )
        {
          while ( *src != '\0' ) *(dst++) = *(src++);
          *dst = '\0';
        }
    }
    return uri;
}   

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool RTEComm_URIUtils::URIsIdentical( SAPDB_Char const * uri1, SAPDB_Char const  * uri2, SAPDB_UInt4 len ) const
{
    int c1, c2;

    do 
    { 
        c1 = tolower(*(uri1++) );
        c2 = tolower(*(uri2++) );
        if ( 0 == --len ) break;
    }  
    while ( c1 && c1 == c2 );

    return c1 == c2;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Char * const RTEComm_URIUtils::URIFindStr( SAPDB_Char * const uri1, SAPDB_Char const * const uri2 ) const
{
    SAPDB_Char *       currStr = reinterpret_cast<SAPDB_Char*>(uri1);
    SAPDB_Char *       tmpURI1;
    SAPDB_Char const * tmpURI2;

    if ( URI_NULL_CHAR == uri2[0] )
        return  uri1;

    while ( currStr[0] )
    {
        tmpURI1 = currStr;
        tmpURI2 = static_cast<SAPDB_Char const * >(uri2);

        while ( tmpURI1[0] && tmpURI2[0] && !(tolower(tmpURI1[0]) - tolower(tmpURI2[0])) )
            tmpURI1++, tmpURI2++;

        if (URI_NULL_CHAR == tmpURI2[0])
            return currStr;

        currStr++;
    }

    return 0;
}

/*!---------------------------------------------------------------------------
  @class   RTEComm_URIPathSegmentList
  @brief   Path Segement List.

           This class creates a list of separated unescaped path segements 
           from an escaped URI path part. There methodes to navigate through
           list.
 ----------------------------------------------------------------------------*/

class RTEComm_URIPathSegmentList : protected RTEComm_URIUtils
{
public:
    /*!
      @brief    contructor / destructor of RTEComm_URIPathSegmentList
     */
    RTEComm_URIPathSegmentList() : m_FirstElement(0) {;}
    virtual ~RTEComm_URIPathSegmentList();

public:
    /*!
      @brief    Build a path segment list containing unescaped UTF8
                path segments.

      @param    esacpedURIPath  [in] - The escaped URI path string.

      @return   NoError, OutOfMemory.
      */
    URIRet Create( SAPDB_Char *  const       esacpedURIPath,
                   SAPDBErr_MessageList      &messageList );

    /*!
      @brief    Get the first path segment.
      @return   pointer to the first segment if any else null.
      */
    SAPDB_UTF8 const * const  First() const
    {
        return m_FirstElement ? m_FirstElement->unescapedSeg : 0;
    }

    /*!
      @brief    Get the next path segment.

      @param    currSegment   [in] - pointer to current segment
                                     string
      @return   pointer to the next segment if any else null.
      */
    SAPDB_UTF8 const * Next( SAPDB_UTF8 const * const currSegment ) const;

private:
    struct segmentListElement // path segment list element
    {
        struct segmentListElement*  nextSegElem;    // pointer to the next segment element if any else null.
        SAPDB_UTF8                  unescapedSeg[1];// The corresponding string goes here.
                                                    // Because we can not declare a zero sized 
                                                    // buffer, we use 1.
    };

private:
    segmentListElement*    m_FirstElement;     // pointer to the first list element
};

/*!---------------------------------------------------------------------------
  @class   RTEComm_URIQueryOptList
  @brief   Query string option list.

           If the given URI path contains a query string 
           starting with a question mark all values 
           separated by an ampersand are handled as single
           options.

           e.g. ...?myopt&myopt=value

           An option can have the form [<option>[[=]<optValue>]].
           If the option contains an equal sign the following string
           is handled as option value.

           Example:  opt1=abc&opt2=2222
 ----------------------------------------------------------------------------*/

class RTEComm_URIQueryOptList : protected RTEComm_URIUtils
{
public:
    /*!
      @brief    contructor / destructor of RTEComm_URIQueryOptList
     */
    RTEComm_URIQueryOptList() : m_FirstElement(0) {;}
    virtual ~RTEComm_URIQueryOptList();

public:
    /*!
      @brief    Build a query option list containing unescaped UTF8
                options and their contents.

      @param    esacpedQuery  [in] - escaped query string starting after '?'
      @return   NoError, OutOfMemory.
      */
    URIRet Create( SAPDB_Char *  const       esacpedQuery,
                   SAPDBErr_MessageList      &messageList );

    /*!
      @brief    Get the first option and it's option value.

                optValue     [out] - pointer to option value of the first
                                     option.
      @return   pointer to the first option if any else null.
      */
    SAPDB_UTF8 const * const  First( SAPDB_UTF8 const * &optValue ) const
    {
        optValue =  m_FirstElement ? m_FirstElement->unescapedValue : 0;
        return m_FirstElement ? m_FirstElement->unescapedOptName : 0;
    }

    /*!
      @brief    Get the next option and it's option value.

      @param    currOption    [in] - pointer to current option.
                optValue     [out] - pointer to option value of the next
                                     option.

      @return   pointer to the next option if any else null.
      */
    SAPDB_UTF8 const * Next( SAPDB_UTF8 const * const currOption,
                             SAPDB_UTF8 const *       &optValue ) const;

private:
    struct queryOptListElement // query option list element
    {
        struct queryOptListElement* nextOptElem;        // pointer to the next option element if any else null.
        SAPDB_UTF8 *                unescapedValue;     // pointer to option value
        SAPDB_UTF8                  unescapedOptName[1];// The option name goes here.
                                                        // Because we can not declare a zero sized 
                                                        // buffer, we use 1.
    };

private:
    queryOptListElement*    m_FirstElement;     // pointer to the first list element
};

#endif  /* RTECOMM_URIUTILS_HPP */
