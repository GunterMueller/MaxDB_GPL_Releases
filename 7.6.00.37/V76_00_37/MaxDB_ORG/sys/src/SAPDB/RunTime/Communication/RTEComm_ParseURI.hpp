/*!========================================================================

  @file         RTEComm_ParseURI.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Parse a liveCache Uniform Resource Identifier (URI)

                The liveCache Uniform Resource Identifier (URI) is a compact
                string of characters used to identify liveCache specific 
                resources.
                The following class is used to separate this URI ASCII
                string in particular unescaped UTF8 parts.

  @since        2003-10-04  7:30
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

#ifndef RTECOMM_PARSEURI_HPP
#define RTECOMM_PARSEURI_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Communication/RTEComm_URIUtils.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include    "RunTime/RTE_MessageList.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

       

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class RTEComm_URI_NILocation;
class RTEComm_URI_IPLocation;
class RTEComm_URI_DBPath;
class RTEComm_URI_ProcSrvPath;


/*!---------------------------------------------------------------------------
  @class   RTEComm_ParseURI
  @brief   Parse a liveCache Uniform Resource Identifier (URI).

           The liveCache Uniform Resource Identifier (URI) is a compact
           string of characters used to identify liveCache specific resources.
           This class handles the individual specification of the liveCache
           URI scheme described in 'liveCacheURI.doc' which is part of the
           document tree. The generic definitions about URI and the common 
           syntax elements are found in RFC 2396. 

           Any URI syntax is dependent upon a specific scheme. The liveCache URIs
           are written as follows:

  @code
                [<scheme>:]<protocol-specific-part><path-part>[?<options>]

                scheme = "liveCache" | "maxdb"

                protocol-and-location-part  =  <remote-communication> | 
                                               <local-communication > | 
                                               <sapni-communication > | 
                                               <http-communication >

                local-communication  = "local:" | "local://." | "//." | ""
                remote-communication = ["remote:"] "//" <host> [":" <port> ]
                sapni-communication	 = "sapni:" "/h/" <host> ["/s/" <port>])
                                        n*("/h/" <host> ["/s/" <port>]) "/:inpas"

					               :inpas – end of sapni string

                path-part =  <database-path> | <procserver-path> | <any-path>

   	            database-path = "/database/" [<serverdb>]["/service" |
 				  			                              "/event"   |
                                                        ( "/session/" <sessionid> ) 
                                                         ["/" <additional path segments > ]]

                procserver-path	= "/procserver" ["/" < additional path segments > ]

                options = [<option> "=" <value> n*[ "&" <option> "=" <value> ]]

  @endcode

           The following class can be used to separate an liveCache URI ASCII
           string in particular unescaped UTF8 parts/segments.

 ----------------------------------------------------------------------------*/

class RTEComm_ParseURI : public RTEComm_URIUtils
{
public:
    /*! @brief    contructor of URI Utility class */
    RTEComm_ParseURI() : m_ProtocolName(0),
                         m_IPLocation(0),
                         m_NILocation(0),
                         m_isNISSLLocation(false),
                         m_isSSLLocation(false),
                         m_hasListenerPath (false),
                         m_DBPath(0),
                         m_ProcServerPath(0),
                         m_OpaquePathElements(0),
                         m_QueryOptList(0),
                         m_Fragment(0),
                         m_escapedURI(0),
                         m_escapedPath(0),
                         m_LocalLocation(false) {;}

    /*! @brief destructor of URI Utility class */
    virtual ~RTEComm_ParseURI(); 

    /*!
      @brief    Get a pointer to the escaped URI string.
        
                Returns the pointer to the stored escaped URI string 
                which has been given via Parse().

      @return    pointer to the escaped URI.
     */
    SAPDB_Char const *         URI() const          { return m_escapedURI; }

    /*!
      @brief    Get a pointer to the escaped URI path part.
        
                Returns the pointer to the stored escaped URI 
                path part.

      @return    pointer to the escaped URI path part.
     */
    SAPDB_Char const *         Path() const          { return m_escapedPath; }

    
    /*!
      @brief    Parse a liveCache Uniform Resource Identifier (URI)

                A given liveCache URI string will be separated in particular 
                unescaped UTF8 parts / segments. After parsing a URI via 'Parse' 
                the different parts / segments can be evaluated with the 
                following methodes.

      @param    uri                [in]   - The escaped URI string which has to
                                            be parsed.
      @param    messageList        [out]  - Filled in case of problems

      @return   Any return value defined in 'class RTEComm_URIUtils'.

                  e.g.
                    NoError             - no error.
                    CommonError         - error occured.
                    ProtocolParseError  - error parsing protocol.
                    LocationParseError  - location part is not well formed.
                    PathParseError      - path part is not well formed.
                    QueryParseError     - query part is not well formed.
                    OutOfMemory         - no more memory to complete operation.
     */
    URIRet Parse ( SAPDB_Char const * const        uri,
                   SAPDBErr_MessageList      &messageList );


    /*!
      @brief    Get a pointer to the protocol name.
        
                Specifies the protocol which has to be used to
                reach the location specified by a location part.

                e.g.  local://./...  -> 'local'
                      http://...    -> 'http'
                      sapni:/h/...  -> 'sapni'
                      sapnis:/h/...  -> 'sapnis'
                      remotes://...  -> 'remotes'

      @return    pointer to the protocol name.
     */
    SAPDB_UTF8 const *              Protocol() const          { return m_ProtocolName; }

    /*!
      @brief    Check if the requested location is on the local server.

                No further action concerning the location is needed.

      @return   true if local.
     */
    SAPDB_Bool                      IsLocalLocation() const   { return false != m_LocalLocation; }

    /*!
      @brief    Check if it's a IP location.
        
                'IsIPLocation' is used to check if the URI contains 
                a IP location string. This location string is parsed
                by a IPLocation object. The methode 'IPLocation()' returns
                the pointer to such a IPLocation object which describes the 
                IP specific connect information.

                e.g. //myserver:8033/...

      @return   true if location can be reached via IP communication.
     */
    SAPDB_Bool                      IsIPLocation() const      { return 0 != m_IPLocation; }

    /*!
      @brief    Check if it's a SSL location.
        
               'IsSSLLocation' is used to check if the URI contains 
                a secure IP location string. This location string is parsed
                by a IPLocation object. The methode 'IPLocation()' returns
                the pointer to the IPLocation object which describes the 
                IP specific connect information.

                e.g. remotes://myserver:8033/...

      @return   true if location can be reached via secure IP communication.
     */
    SAPDB_Bool                      IsSSLLocation() const      { return m_isSSLLocation; }

    /*!
      @brief    Check if it's a SAPNI location.
        
               'IsNILocation' is used to check if the URI contains 
                a SAPNI location string. This location string is parsed
                by a NILocation object. The methode 'NILocation()' returns
                the pointer to such a NILocation object which describes the 
                SAPNI specific connect information.

                e.g. sapni:/h/myserver/s/8033/...:inpas

      @return   true if location can be reached via SAPNI communication.
     */
    SAPDB_Bool                      IsNILocation() const      { return 0 != m_NILocation; }

    /*!
      @brief    Check if it's a SAPNI SSL location.
        
               'IsNISSLLocation' is used to check if the URI contains 
                a secure SAPNI location string. This location string is parsed
                by a NILocation object. The methode 'NILocation()' returns
                the pointer to the NILocation object which describes the 
                SAPNI specific connect information.

                e.g. sapnis:/h/myserver/s/8033/...:inpas

      @return   true if location can be reached via secure SAPNI communication.
     */
    SAPDB_Bool                      IsNISSLLocation() const      { return m_isNISSLLocation; }

    /*!
      @brief    Get a pointer to a IPLocation object.
        
                This methode can be used to get a pointer to a 
                IPLocation object describing the IP specific connect 
                information.

      @return   pointer to IPLocation object if any else null.
     */
    RTEComm_URI_IPLocation const *  IPLocation() const        { return m_IPLocation; }

    /*!
      @brief    Get a pointer to a NILocation object.
        
                This methode can be used to get a pointer to a 
                NILocation object describing the  SAPNI specific 
                connect information.

      @return   pointer to NILocation object if any else null.
     */
    RTEComm_URI_NILocation const *  NILocation() const        { return m_NILocation; }
    
    /*!
      @brief    Check if it's a database request.
        
                Checks if the URI path specifies a 
                database location. The methode 'DBPath()' can
                be used to get a pointer to a DBPath object 
                describing the database specific path information

                e.g. .../database/mydatabasename/event/...

      @return   true if path specifies a database.
     */
    SAPDB_Bool                      HasDBPath() const          { return 0 != m_DBPath; }

    /*!
      @brief    Check if it's a procserver request.
        
                Checks if the URI path specifies a 
                database location. The methode 'ProcSrvPath()' can
                be used to get a pointer to a ProcSrvPath object 
                describing the procserver specific path information

                e.g. .../procserver/..

      @return   true if path specifies a procserver.
     */
    SAPDB_Bool                      HasProcSrvPath() const     { return 0 != m_ProcServerPath; }

    /*!
      @brief    Check if it's a listener request.
        
                Checks if the URI path specifies a 
                listener location.

                e.g. .../listener

      @return   true if path specifies a listener.
     */
    SAPDB_Bool                      HasListenerPath() const     { return m_hasListenerPath; }


    /*!
      @brief    Check if the given URI path does not specify a
                known path type like database or procserver.
        
                If the given URI path is opaque to the parser, 
                the path will be separated into single 
                path segments which can be evaluated via 
                'FirstOpaquePathSegment' and 'NextOpaquePathSegment'.

                e.g. .../any1/any2/...

      @return   true if path is opaque to the URI parser.
     */
    SAPDB_Bool                      HasOpaquePath() const      { return 0 != m_OpaquePathElements; }
  
    /*!
      @brief    Get pointer to a DBPath object.
        
                This methode can be used to get a pointer 
                to a DBPath object describing the database 
                specific path information

      @return   pointer to the DBPath object if any else null.
     */
    RTEComm_URI_DBPath const *      DBPath() const            { return m_DBPath; }

    /*!
      @brief    Get pointer to a ProcSrvPath object.
        
                This methode can be used to get a pointer 
                to a ProcSrvPath object describing the 
                procserver specific path information

      @return   pointer to the ProcSrvPath object if any else null.
     */
    RTEComm_URI_ProcSrvPath const * ProcSrvPath() const       { return m_ProcServerPath; }


    /*!
      @brief    Get first opaque path segment.
        
                If the given URI path is opaque to the parser, 
                the path will be separated into single 
                path segments which can be evaluated via 
                'FirstOpaquePathSegment' and 'NextOpaquePathSegment'.
                Use 'IsOpaquePath' to check if the path is opaque.

                e.g. /any1/any2/...

                'FirstOpaquePathSegment' can be used to get the first 
                path segment. Referring to the previous path sample it
                would return 'any1'.

      @return   pointer to the first path segment or NULL if no
				path segments exist.
     */
    inline SAPDB_UTF8 const * const FirstOpaquePathSegment() const;


    /*!
      @brief    Get next opaque path segment.
        
                If the given URI path is opaque to the parser, 
                the path will be separated into single 
                path segments which can be evaluated via 
                'FirstOpaquePathSegment' and 'NextOpaquePathSegment'.
                Use 'IsOpaquePath' to check if the path is opaque.

                e.g. /any1/any2/any3...

                'NextOpaquePathSegment' can be used to get the next 
                path segment. Referring to the previous path sample it
                would return 'any2' if the pointer to first segment
                is given (in this case 'any1').

      @return   pointer to the next path segment or NULL if no further path
				segment exists or no path segments exists at all.
     */
    inline SAPDB_UTF8 const * const NextOpaquePathSegment ( SAPDB_UTF8 const * const currSegment ) const;


    /*!
      @brief    Does the URI string contain options.
        
                If the given URI path contains a query string 
                starting with a question mark all values 
                separated by an ampersand are handled as single
                options.

                e.g. ...?myopt&myopt1=value

                An option can have the form [\<option\>[[=]\<optValue\>]].
                If the option contains an equal sign the following string
                is handled as option value.

      @return   true if URI contains options
     */
    SAPDB_Bool                      HasOption          () const  { return 0 != m_QueryOptList; }

    /*!
      @brief    Get the first option and it's option value.

      @param    optValue     [out] - pointer to option value.

      @return   pointer to the first option if any else null.
      */
    inline SAPDB_UTF8 const * const FirstOption( SAPDB_UTF8 const *         &optValue ) const;


    /*!
      @brief    Get the next option and it's option value.

      @param    currOption  [in] - pointer to current option.
      @param    optValue     [out] - pointer to option value.

      @return   pointer to the next option if any else null.
      */
    inline SAPDB_UTF8 const * const NextOption ( SAPDB_UTF8 const * const   currOption,
                                                 SAPDB_UTF8 const *         &optValue ) const;

    /*!
      @brief    Find an option and and return it's option value.

      @param    option        [in] - pointer to requested option.
      @param    optValue     [out] - pointer to option value.

      @return   pointer to the next option if any else null.
      */
    inline SAPDB_Bool               FindOption ( SAPDB_UTF8 const * const   option,
                                                 SAPDB_UTF8 const *         &optValue ) const;

/*!
  @brief    Get fragment from the tail of the URI.

            If the URI string contains a '#', then the substring
            after the first '#' and up to the end of the URI string
            is the fragment identifier, which is not interpreted 
            by the URI parser

      @return   pointer to the fragment if any else null.
      */

    SAPDB_UTF8 const *              Fragment() const          { return m_Fragment; }

    /*!
      @brief    Clean up all resources

      @return   none
      */

    void                            CleanUp ();

private:
    URIRet StripFragment            ( SAPDB_Char * const        uri,
                                      SAPDBErr_MessageList      &messageList );
    URIRet SeparateScheme           ( SAPDB_Char *              &uri,
                                      SAPDBErr_MessageList      &messageList );
    URIRet ParseProtocol            ( SAPDB_Char *              &uri,
                                     SAPDB_Char *               &protocolName,
                                      SAPDBErr_MessageList      &messageList );
    URIRet ParseLocation            ( SAPDB_Char *              &uri,
                                      SAPDB_Char *              protocolName,
                                      SAPDBErr_MessageList      &messageList );
    URIRet ParsePath                ( SAPDB_Char *              &uri,
                                      SAPDBErr_MessageList      &messageList );
    URIRet ParseQuery               ( SAPDB_Char *              &uri,
                                      SAPDBErr_MessageList      &messageList );
    inline URIRet ParseLocalLocation( SAPDB_Char *              &uri,
                                      SAPDBErr_MessageList      &messageList );

private:
    SAPDB_UTF8*                 m_ProtocolName;
    SAPDB_Bool                  m_LocalLocation;
    RTEComm_URI_IPLocation*     m_IPLocation;
    RTEComm_URI_NILocation*     m_NILocation;
    SAPDB_Bool                  m_isNISSLLocation;
    SAPDB_Bool                  m_isSSLLocation;
    SAPDB_Bool                  m_hasListenerPath;
    RTEComm_URI_DBPath*         m_DBPath;
    RTEComm_URI_ProcSrvPath*    m_ProcServerPath;
    RTEComm_URIQueryOptList*    m_QueryOptList;
    RTEComm_URIPathSegmentList* m_OpaquePathElements;
    SAPDB_UTF8*                 m_Fragment;

    SAPDB_Char*                 m_escapedURI;
    SAPDB_Char*                 m_escapedPath;
};

/*---------------------------------------------------------------------------*/

inline SAPDB_UTF8 const * const 
    RTEComm_ParseURI::FirstOption( SAPDB_UTF8 const *  &optValue ) const
{
    return m_QueryOptList ? m_QueryOptList->First(optValue): 0;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UTF8 const * const 
    RTEComm_ParseURI::NextOption( SAPDB_UTF8 const * const currOption,
                                  SAPDB_UTF8 const *       &optValue ) const
{
    return m_QueryOptList->Next( currOption, optValue );
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool
    RTEComm_ParseURI::FindOption( SAPDB_UTF8 const * const   option,
                                  SAPDB_UTF8 const *         &optValue ) const
{
    optValue = 0;

    SAPDB_UTF8 const*  foundOption = FirstOption( optValue );

    while (( 0 != foundOption ) && !URIsIdentical ( foundOption, option ) )
    {
        foundOption = NextOption ( foundOption, optValue );
    }

    return 0 != foundOption;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UTF8 const * const 
    RTEComm_ParseURI::FirstOpaquePathSegment() const
{
    return m_OpaquePathElements ? m_OpaquePathElements->First(): 0;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UTF8 const * const 
    RTEComm_ParseURI::NextOpaquePathSegment( SAPDB_UTF8 const * const currSegment ) const
{
	return m_OpaquePathElements ? m_OpaquePathElements->Next(currSegment) : 0;
}

/*!---------------------------------------------------------------------------
  @class   RTEComm_URI_IPLocation
  @brief   IP location information.

           This class describes the IP location information which
           might be given by an URI string.

             e.g. ...//myserver:8033/...

 ----------------------------------------------------------------------------*/

class RTEComm_URI_IPLocation : public RTEComm_URIUtils
{
    friend class RTEComm_ParseURI;

public:
    /*!
      @brief    Get the IP hostname.

      @return   pointer to the IP hostname if any else null.
      */
    SAPDB_UTF8 const * const Hostname() const     { return m_Hostname; }

    /*!
      @brief    Get the IP port number.

      @return   pointer to the IP port number if any else null.
      */
    SAPDB_UTF8 const * const PortNo() const       { return m_PortNo; }

public: // we need the destructor public for 'destroy'
    /*! @brief    destructor of URI Utility class */
    virtual ~RTEComm_URI_IPLocation();

private:
    /*! @brief    contructor of URI Utility class */
    RTEComm_URI_IPLocation() : RTEComm_URIUtils(),
                               m_Hostname(0),
                               m_PortNo(0) {;}

    inline URIRet Parse( SAPDB_Char *              &uri,
                         SAPDBErr_MessageList      &messageList );

private:
    SAPDB_UTF8*         m_Hostname;
    SAPDB_UTF8*         m_PortNo;
};


/*!---------------------------------------------------------------------------
  @class   RTEComm_URI_NILocation
  @brief   SAPNI location information.

           This class describes the SAPNI location information which
           might be given by an URI string.

             e.g. sapni:/h/myserver/s/8033/...:inpas

 ----------------------------------------------------------------------------*/

class RTEComm_URI_NILocation : public RTEComm_URIUtils
{
    friend class RTEComm_ParseURI;

public:
    /*!
      @brief    Get the IP hostname of the last host in SAPNI string.

      @return   pointer to the IP hostname.
      */
    SAPDB_UTF8 const * const LastHostname() const { return m_LastHostname; }

    /*!
      @brief    Get the IP port number of the last host in SAPNI string.

      @return   pointer to the IP port number if any else null.
      */
    SAPDB_UTF8 const * const LastPortNo() const   { return m_LastPortNo; }

    /*!
      @brief    Get the whole SAPRouter string.

      @return   pointer to the SAPRouter string.
      */
    SAPDB_UTF8 const * const SAPRouter() const    { return m_SAPRouter; }

public: // we need the destructor public for 'destroy'
    virtual ~RTEComm_URI_NILocation();

private:
    RTEComm_URI_NILocation() : RTEComm_URIUtils(),
                               m_LastHostname(0),
                               m_LastPortNo(0),
                               m_SAPRouter(0) {;}

    inline URIRet Parse( SAPDB_Char *              &uri,
                         SAPDBErr_MessageList      &messageList );

private:
    SAPDB_UTF8*         m_LastHostname;
    SAPDB_UTF8*         m_LastPortNo;
    SAPDB_UTF8*         m_SAPRouter;
};

/*!---------------------------------------------------------------------------
  @class   RTEComm_URI_DBPath
  @brief   Database path object.
        
           If the parsed URI conatains database path information an,
           object of this class is generated. This object holds the
           database path specific informations like database name and
           session information. 
           The URI path must start with the path segment 'database' 
           and is followed by the database name.

            e.g. .../database/mydatabasename/event/...

           The path segments following the database name are
           handled in a path segment list which can be evaluated
           by using the methodes 'FirstOpaquePathSegment' and 
           'NextOpaquePathSegment'.
           
 ----------------------------------------------------------------------------*/

class RTEComm_URI_DBPath : public RTEComm_URIUtils
{
    friend class RTEComm_ParseURI;

public:
    /*!
      @brief    Get the database name.

      @return   pointer to the database name if any else null.
      */
    SAPDB_UTF8 const * const ServerDB()           const { return m_ServerDB; }


    /*!
      @brief    Get first path segment following the 
                database name.

      The path segments following the database name are
      handled in a path segment list which can be evaluated
      by using the methodes 'FirstOpaquePathSegment' and 
      'NextOpaquePathSegment'.

      @return   pointer to the path segment following the 
                database name. Returns null if no path segments
				following the database name exists.
      */
    SAPDB_UTF8 const * const FirstOpaquePathSegment() const
    {
        return m_OpaquePathElements ? m_OpaquePathElements->First(): 0;
    }

    /*!
      @brief    Get next path segment following the given
                segment pointer.

      @param    currSegment    [in] - pointer to current path segment.

      @return   pointer to the path segment following the 
                given segment pointer. Returns null if no following 
                path segment exists or no path segments
				following the database name exists at all.
      */
    SAPDB_UTF8 const * const NextOpaquePathSegment( SAPDB_UTF8 const * const currSegment ) const
    {
		return m_OpaquePathElements ? m_OpaquePathElements->Next( currSegment ): 0;
    }
public: // we need the destructor public for 'destroy'
    virtual ~RTEComm_URI_DBPath();

private:
    RTEComm_URI_DBPath() : RTEComm_URIUtils(),
                           m_ServerDB(0),
                           m_OpaquePathElements(0) {;}

    inline URIRet Parse( SAPDB_Char *              &uri,
                         SAPDBErr_MessageList      &messageList );

private:
    SAPDB_UTF8*                 m_ServerDB;
    RTEComm_URIPathSegmentList* m_OpaquePathElements;
};

/*!---------------------------------------------------------------------------
  @class   RTEComm_URI_ProcSrvPath
  @brief   ProcServer path object.
        
           If the parsed URI conatains procserver path information, an
           object of this class is generated. This object holds the
           procserver path specific informations.
           The URI path must start with the path segment 'procserver'. 

            e.g. .../procserver/...

           The path segments are handled in a path segment list which 
           can be evaluated by using the methodes 'FirstOpaquePathSegment'
           and 'NextOpaquePathSegment'.
           
 ----------------------------------------------------------------------------*/

class RTEComm_URI_ProcSrvPath : public RTEComm_URIUtils
{
    friend class RTEComm_ParseURI;

public:
    /*!
      @brief    Get first path segment.

      The path segments are handled in a path segment list
      which can be evaluated by using the methodes 
      'FirstOpaquePathSegment' and 'NextOpaquePathSegment'.

      @return   pointer to the first path segment. 
                Returns null if no path segments
                exist.
      */
    SAPDB_UTF8 const * const FirstOpaquePathSegment() const
    {
        return m_OpaquePathElements ? m_OpaquePathElements->First(): 0;
    }

    /*!
      @brief    Get next path segment.

      @param    currSegment    [in] - pointer to current path segment.

      @return   pointer to the path segment following the 
                given segment pointer. Returns null if no following 
                path segment exists or no path segments exist at all.
      */
    SAPDB_UTF8 const * const NextOpaquePathSegment( SAPDB_UTF8 const * const currSegment ) const
    {
		return m_OpaquePathElements ? m_OpaquePathElements->Next(currSegment) : 0;
    }

public: // we need the destructor public for 'destroy'
    virtual ~RTEComm_URI_ProcSrvPath();

private:
    RTEComm_URI_ProcSrvPath() : RTEComm_URIUtils(),
                                m_OpaquePathElements(0) {;}

    inline URIRet Parse( SAPDB_Char *              &uri,
                         SAPDBErr_MessageList      &messageList );

private:
    RTEComm_URIPathSegmentList* m_OpaquePathElements;
};

#endif  /* RTECOMM_PARSEURI_HPP */
