/*!**********************************************************************

  module: sapdbwa.hpp	(origin: hwd02waapi.h)

  -----------------------------------------------------------------------

  responsible:	Holger Bischoff
				Markus Özgen

  special area: SAPDB WebAgent C++ Interface

  description:  Export description of WebAgent C++ interface

  see also:     

  -----------------------------------------------------------------------

						Copyright (c) 2000-2005 SAP AG



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




************************************************************************/

#ifndef SAPDBWA_CPP_H
#define SAPDBWA_CPP_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <sapdbwa_define.h>		// Defintions
#include <sapdbwa.h>			// C - API functions
#include <sapdbwa_session.hpp>	// C++ - API sessionpool
#include <sapdbwa_error.h>		// Errors

/*!**********************************************************************

  Chapter: Overview

  description:
  This header-file describes the C++-API for the WebAgent. This API
  can be used to write services (WebAgent User DLL's) that handle
  incoming HTTP requests. The WebAgent API is web server independent.
  Services using this API work with the web servers from Microsoft, 
  Netscape and the Apache. For other servers the standard CGI protocol
  can be used.

  This API includes classes for HTTP request handling (WAHttpRequest),
  writing HTTP responses (WAHttpResponse), session management
  (WAWebSession) and other stuff (WAWebAgent). The class WAErr is used
  to retrieve error information if an operation failed. */
/*!
  EndChapter: Overview

************************************************************************/

/*!**********************************************************************

  Chapter: Prototypes for UserDll functions

  description:
  Every UserDll contains an init- exit- and service function. These are
  the functions called by the WebAgent. The names of these functions
  are configured in the WenAgent.ini file. Thy must have the same
  arguments, return values and calling convention  as the prototypes
  below.
************************************************************************/

// from sapdbwa_session.hpp
class sapdbwa_SessionPool;

class sapdbwa_Err;
class sapdbwa_WebAgent;
class sapdbwa_WebSession;
class sapdbwa_StringSeq;
class sapdbwa_HttpRequest;
class sapdbwa_HttpReply;

/*!**********************************************************************
  Function: UserInit

  description: Prototype for UserDll initialization function.

  The function may have have an individual name for every UserDll.
  The actual name is specified in the WebAgent.ini file. This function is
  called once at the start up of the webserver.

  arguments:
    wa  [in]  WebAgent handle, that can be used to write log messages,
              access a session pool, etc.
    
  return value: One of the WebAgent following return codes specified in
  hwd01wadef.h:
  <UL>
  <LI> sapdbwa_RTC_Error if the initialization failed. In this case the UserDll
       is unloaded.
  <LI> sapdbwa_RTC_Ok if initialization is ok.
  </UL>
  */

extern "C" sapdbwa_Int2 UserInit( sapdbwa_WebAgent &wa );


/*!**********************************************************************
  Function: UserExit

  description: Prototype for UserDll exit function.

  The function may have have an individual name for every UserDll.
  The actual name is specified in the WebAgent.ini file. This function is
  called when the webserver is stopped.

  arguments:
    wa  [in]  WebAgent handle, that can be used to write log messages,
              access a session pool, etc.
    
  return value: One of the WebAgent return codes following specified in
  hwd01wadef.h:
  <UL>
  <LI> sapdbwa_RTC_Error if the exit failed.
  <LI> sapdbwa_RTC_Ok if exit is ok.
  </UL>
  */
extern "C" sapdbwa_Int2 UserExit( sapdbwa_WebAgent &wa );

/*!**********************************************************************
  Function: UserService

  description: Prototype for UserDll service function.

  The function may have have an individual name for every UserDll.
  The actual name is specified in the WebAgent.ini file. This function is
  called for every HTTP-Reqest with the configured prefix.

  arguments:
    wa      [in]  WebAgent handle, that can be used to write log messages,
                  access a session pool, etc.
    request [in]  Request object that holds all information about the
                  incoming HTTP request.
    reply   [in]  The reply object is used to send the HTTP reply back to
                  the client.
                  
  return value: One of the WebAgent return codes specified in hwd01wadef.h:
  <UL>
  <LI> sapdbwa_RTC_Error if a fatal error occured that prevents the handling
       of the request. The WebAgent generates a log file entry. The setting
       of an appropriate staus code (e.g. 500 internal server error) for the
       reply must be done in the service function.
  <LI> sapdbwa_RTC_Accepted if the service function succeeds properly.
  <LI> sapdbwa_RTC_Ignored if the request should be handled by the WebServer.
  </UL>
  */
extern "C" sapdbwa_Int2 UserService( sapdbwa_WebAgent    &wa,
                                   sapdbwa_HttpRequest &request,
                                   sapdbwa_HttpReply   &reply );

typedef sapdbwa_Bool (*sapdbwa_CppDestructor)( sapdbwa_WebAgent &wa, void *userData );

/*!**********************************************************************

  EndChapter: Prototypes for UserDll

************************************************************************/
/*!**********************************************************************

  Class: sapdbwa_WebAgent

  description: A reference to a sapdbwa_WebAgent object is passed to all three
               user-dll-functions. It can be used to initiate WebSessions,
               to access the configured SessionPool or to write log
               messages to the configured log file.
  
************************************************************************/

class sapdbwa_WebAgent
{
public:
    sapdbwa_WebAgent( sapdbwa_Handle wa );
    
    /*!**********************************************************************
      Function: GetParam
      
      description: Get UserDll parameter from the WebAgent.ini file.

      arguments:
      key       [in]  The name of the parameter to retrieve.
      valueBuf  [out] Buffer for the parameter value.
      bufLen    [in]  Length of valueBuf.
    
      return value:
      true if the param exists and is not empty, else false.
    */
    sapdbwa_Bool GetParam( const char *key, char *valueBuf, sapdbwa_Int4 bufLen );
    
    /*!**********************************************************************
      Function: GetDocumentRoot
      
      description: Returns the WebAgent document root folder.

      arguments:
      valueBuf  [out] Buffer for the folder.
      bufLen    [in]  Length of buffer.
    
      return value:
	  true if folder exists, otherwise false
    */
	sapdbwa_Bool GetDocumentRoot( char		    *valueBuf,
                                  sapdbwa_Int4	 bufLen   );
    
    /*!**********************************************************************
      Function: GetSessionPool
      
      description: Get the configured sessionpool.

      The parameters for the sessionpool are specified in the WebAgent.ini
      file.

      arguments:
      pool  [out]  The configured sessionpool.
    
      return value:
      true if a sessionpool exists for the user dll, else false.
    */
    sapdbwa_Bool GetSessionPool( sapdbwa_SessionPool*& pool );

    /*!**********************************************************************
      Function: GetErr

      description: Get error information from sapdbwa_WebAgent.

      arguments:

      return value:
      Reference to error object. This can be used to retrieve
      error information if a previous operation on the sapdbwa_WebAgent failed.
      See <A HREF="hwd06waerr.html">hwd06waerr.html</A> for a description
      of the error class.
    */
    const sapdbwa_Err& GetErr() const;
    
    /*!**********************************************************************
      Function: WriteLogMsg

      description: Write a message in the user dll's log file.

      The name and path of the log file are specified in WebAgent.ini.
      Together with the log-message the actual date and time are written to
      the log file.
      
      arguments:
      msg  [out]  The log message.
    
      return value:
      true if a log file was configured for the user dll and the output of
      the message was ok. false if no log file was configured or the output
      to the log file was not successful.
    */
    sapdbwa_Bool WriteLogMsg( const char *msg ) const;


    /*!**********************************************************************
      Function: SetUseWebSessions

      description: Signal the WebAgent whether WebSessions are used by a
      user dll.

      If WebSessions should be used this function is typically called in
      the initialization function of the user dll withe the value true to
      enable WebSessions. The default value is false.

      If WebSessions are enabled the WebAgent sents a cookie with every reply
      to identify a WebSession.

      arguments:
      useWebSessions  [out]  true to enable, false to disable WebSessions.
    
      return value: true
    */
    sapdbwa_Bool SetUseWebSessions( sapdbwa_Bool useWebSessions );


    /*!**********************************************************************
      Function: StartWebSession

      description: Starts a WebSession with a given timeout value.

      Before the call of StartWebSession a call of SetUseWebSessions( true )
      has to be made to enable WebSessions.

      arguments:
        webSession [out]  The started WebSession.
        timeOut    [out]  The time in sec. after which the WebSession is
                          implicitely closed.
    
      return value: true if the session was successfully started. false if
      startup failed. The following reasons my cause a failure:
      <UL>
      <LI> A memory allocation error (sapdbwa_Error_Memory).
      <LI> WebSessions are disabled for the user dll
           (sapdbwa_Error_WebSessionsDisabled).
      </UL>
    */
    sapdbwa_Bool StartWebSession( sapdbwa_WebSession     &webSession,
                                sapdbwa_UInt4        timeOut = 1200 );


    /*!**********************************************************************
      Function: StartNewWebSession

      description: Starts a new WebSession with a given timeout value.

                   The function creates a new session id. If there is an existing 
                   WebSession in this WebAgent object the function ignores (forgets!)
                   this (existing) session doesn't touch it.

                   Before the call of StartNewWebSession a call of SetUseWebSessions
                   ( true ) has to be made to enable WebSessions.

      arguments:
        webSession [out]  The started WebSession.
        timeOut    [out]  The time in sec. after which the WebSession is
                          implicitely closed.
    
      return value: true if the session was successfully started. false if
      startup failed. The following reasons my cause a failure:
      <UL>
      <LI> A memory allocation error (sapdbwa_Error_Memory).
      <LI> WebSessions are disabled for the user dll
           (sapdbwa_Error_WebSessionsDisabled).
      </UL>
    */
    sapdbwa_Bool StartNewWebSession( sapdbwa_WebSession    &webSession,
                                     sapdbwa_HttpReply     &reply,
                                     sapdbwa_UInt4          timeOut = 1200 );

    /*!**********************************************************************
      Function: WAStartDefaultWebSession

      description:			Starts a WebSession with the default timeout value.

							Before WAStartDefaultWebSession call
							SetUseWebSessions( true ) to enable WebSessions.

      arguments:
        webSession [out]	The started WebSession.
    
      return value:			true if the session was successfully started.
							false if startup failed.
							The following reasons my cause a failure:
							<UL>
							<LI> A memory allocation error (sapdbwa_Error_Memory).
							<LI> WebSessions are disabled for the user dll
								(sapdbwa_Error_WebSessionsDisabled).
							</UL>
    */
	sapdbwa_Bool StartDefaultWebSession( sapdbwa_WebSession  &webSession );


    /*!**********************************************************************
      Function: GetWebSession

      description: Get an active WebSession that was opened in the actual
                   or a previous request.

      arguments:
        webSession [out]  The started WebSession.
    
      return value: true if an active session was found. false if no active
      session exists. The following reasons my cause a failure:
      <UL>
      <LI> A memory allocation error (sapdbwa_Error_Memory).
      <LI> The WebSession is locked by another thread
           (sapdbwa_ERR_WEBSESSION_LOCKED). If multiple requests for one WebSession
           are handled simultaneously this error may occur because only one
           thread gets access to a WebSession per time.
      <LI> The WebSession timed out (sapdbwa_Error_WebSessionTimeout). If the time
           elapsed since the last access to the WebSession is greater than
           the timeout specified in StartWebSession the WebSession is
           implicitly closed. In this case the error id
           sapdbwa_Error_WebSessionTimeout <em> or </em> sapdbwa_Error_NoWebSession may
           be set.
      <LI> No WebSession was started or the WebSession timed out
           (sapdbwa_Error_NoWebSession).
      </UL>
    */
    sapdbwa_Bool GetWebSession( sapdbwa_WebSession  &webSession ) const;

    /*!**********************************************************************
      Function: ReleaseWebSession

      description: Releases an active WebSession that was locked by GetWebSesssion,
                   StartWebSession or StartNewWebSession.

                   After the ReleaseWebSession other threads of the webserver
                   can get an manipulate the session object. You should use the
                   usual mechanisms to protect your data in durning parallel 
                   working.

    */
    void ReleaseWebSession( ) const;

    /*!**********************************************************************
      Function: CloseWebSession

      description: Close an active WebSession that was opened in the actual
                   or a previous request.

      arguments:
        webSession		[in]	The WebSession to close.
		freeUserData	[in]	If true user data will be destroyed by calling
								user's destructor funtion.
    
      return value: true
    */
    sapdbwa_Bool CloseWebSession( sapdbwa_WebSession &webSession,
								  sapdbwa_Bool		  freeUserData = sapdbwa_True );


    /*!**********************************************************************
      Function: GetServiceName

      description:  Retrieves the name of the service. This is the prefix which
                    is to be used with an URI.

      arguments:
    
      return value: Service name.
    */
    const char* GetServiceName();

private:
    sapdbwa_Handle       wa;
    sapdbwa_Err          err;
    sapdbwa_SessionPool  pool;
}; // class sapdbwa_WebAgent

/*!**********************************************************************

  EndClass: sapdbwa_WebAgent

************************************************************************/
/*!**********************************************************************

  Class: sapdbwa_WebSession

  description:
  Because HTTP is a stateless protocol each request is isolated.
  To store session info across mutliple requests cookies can
  be used to identify the session.

  The class sapdbwa_WebSession uses cookies to store a session id at the client.
  Each sapdbwa_WebSession can hold user data that stored at the server side.

  The class sapdbwa_WebAgent provides methods to start, access and close
  WebSessions.

  Important note: Use http://myuserdll/ in your URLs instead of
  http://myuserdll !!! Else IE 4.0 does not send cookies back.
  
************************************************************************/

class sapdbwa_WebSession
{
public:
    sapdbwa_WebSession();
    sapdbwa_WebSession( sapdbwa_WebSessionP webSession );
    ~sapdbwa_WebSession();

    /*!**********************************************************************
      Function: GetSessionData

      description: Return the user data for the WebSession.

      arguments:
    
      return value: The pointer stored with SetSessionData or NULL if no
      pointer was set.
    */
    void* GetSessionData() const;

    /*!**********************************************************************
      Function: SetSessionData

      description: Store data in a WebSession.

      arguments:
        data       [in] A pointer to user data that is associated with the
                        WebSession.
        destructor [in] A pointer to the destructor function for data. This
                        destructor is called with data as argument when the
                        session is explicitely or implicitely closed.

      return value: true
    */
    sapdbwa_Bool SetSessionData( void             *data,
                               sapdbwa_CppDestructor  destructor );


    /*!**********************************************************************
      Function: NewSessionId

      description: Set a new session id for a WebSession to improve security.

      Since the session id may be transferred as plaintext over non secure
      connections anybody can use a network sniffer to read the id to get
      unauthorized access to the webserver. To make this more difficult the
      methos NewSessionId is used to change the id from time to time.
      
      This function should not be used, if pending request for this session
      may be open (e.g. multiple HTML frames are filled).

      arguments:
    
      return value: true
    */
    sapdbwa_Bool NewSessionId();
    
    sapdbwa_WebSessionP GetWebSession() const;
private:
    sapdbwa_WebSessionP webSession;
}; // class sapdbwa_WebSession
    
/*!**********************************************************************

  EndClass: sapdbwa_WebSession

************************************************************************/
/*!**********************************************************************

  Chapter: sapdbwa_StringSeq

  description:
  String sequences are used in sapdbwa_HttpRequest to return cookie names,
  parameter names and parameter values.

************************************************************************/


class sapdbwa_StringSeq
{
public:
    sapdbwa_StringSeq();
    ~sapdbwa_StringSeq();

    /*!**********************************************************************
      Function: GetNumElem

      description: Return the number of elements in the sequence.

      arguments:
    
      return value: The number of elements in the sequence.
    */
    sapdbwa_Int4 GetNumElem() const;

    /*!**********************************************************************
      Function: operator[]

      description: Return the n'th element of the sequence.

      arguments:
        index [in] The index of the requested entry.

      return value: Pointer to the element with given index or NULL if
                    the sequence contains less than index+1 elements.
    */
    const char* operator[]( sapdbwa_Int4 index ) const;

    sapdbwa_StringSeqP GetStringSeq() const;
private:
    sapdbwa_StringSeqP stringSeq;
}; // class sapdbwa_StringSeq


/*!**********************************************************************

  EndChapter: sapdbwa_StringSeq
  
************************************************************************/
/*!**********************************************************************

  Class: sapdbwa_HttpRequest

  description: A sapdbwa_HttpRequest object provides access to all information
  of a incoming HTTP request.

************************************************************************/

class sapdbwa_HttpRequest
{
public:
    sapdbwa_HttpRequest( sapdbwa_HttpRequestP req )
        : req(req), err(sapdbwa_ReqGetErr(req)) {};

    /*!**********************************************************************
      Function: GetAuthorization

      description: Get information from Authorization request header.

      arguments:

      return value: true if userName, password have been set, false
                    if Authorization header doas not exist or has
                    wrong format.

      Note: This function cannot be used with CGI to avoid security holes!
    */
    sapdbwa_Bool GetAuthorization( const char **userName,
                                 const char **password ) const;
    
    /*!**********************************************************************
      Function: GetErr

      description: Get error information from request.

      arguments:

      return value:
      Reference to error object of the request. This can be used to retrieve
      error information if a previous operation on the request failed.
      See <A HREF="hwd06waerr.html">hwd06waerr.html</A> for a description
      of the error class.
    */
    const sapdbwa_Err& GetErr() const;

    /*!**********************************************************************
      Function: GetCookieNames

      description: Return all cookie names from the request.

      arguments:
        names  [out]  The names are stored in this string sequence.

      return value: true for success else false. The functions may fail
      for the following reasons:
      <UL>
      <LI> Memory allocation error (sapdbwa_Error_Memory).
      </LI>
    */
    sapdbwa_Bool GetCookieNames( sapdbwa_StringSeq &names ) const;


    /*!**********************************************************************
      Function: GetCookieValue

      description: Return the value for a cookie with given name.

      arguments:
        name  [in]  The name of the cookie.

      return value: A pointer to the value string or null if a cookie with
      the given name does not exist.
    */
    const char* GetCookieValue( const char *name ) const;


    /*!**********************************************************************
      Function: GetHeader

      description: Return the value for a HTTP header field with given name.

      The method only accepts the CGI style names (e.g. CONTENT_LENGTH,
      EXPIRES, etc.).

      arguments:
        name  [in]  The name of the header field.

      return value: A pointer to the value string or null if a header field
      with the given name does not exist.
    */
    const char* GetHeader( const char *fieldName ) const;


    /*!**********************************************************************
      Function: GetRequestURI

      description: Return the URI of the incoming request.

      arguments:

      return value: A pointer to the request URI string.
    */
    const char* GetRequestURI() const;


    /*!**********************************************************************
      Function: GetIfModifiedSince

      description: Return the if-modified-since header of the incoming request.

      arguments:

      return value: A pointer to the value of the if-modified-since header or
      NULL if the request does not contain that header.
    */
    const char* GetIfModifiedSince() const;


    /*!**********************************************************************
      Function: GetQueryString

      description: Return the query string of the incoming request.

      The query string is the part of the URI that is following after a '?'.

      arguments:

      return value: A pointer to the query string or NULL if the request
      does not contain a query string.
    */
    const char* GetQueryString() const;


    /*!**********************************************************************
      Function: GetPathInfo

      description: Return additional path info of the incoming request.
      
      arguments:

      return value: A pointer to the path info or NULL if the request does 
      not contain additional path info.
    */
    const char* GetPathInfo() const;


    /*!**********************************************************************
      Function: GetMethod

      description: Return the method (GET, POST, PUT, ...) of the request.

      arguments:

      return value: A pointer to the method string.
    */
    const char* GetMethod() const;


    /*!**********************************************************************
      Function: GetContentType

      description: Return the content-type header of the incoming request.

      arguments:

      return value: A pointer to the value of the content-type header or
      NULL if the request does not contain that header.
    */
    const char* GetContentType() const;


    /*!**********************************************************************
      Function: GetContentLength

      description: Return the content-length header of the incoming request.

      arguments:

      return value: A pointer to the value of the content-length header or
      NULL if the request does not contain that header.
    */
    const char* GetContentLength() const;


    /*!**********************************************************************
      Function: GetContentChunked

      description: Returns true if transfer encoding of content is chunked

      arguments:

      return value:
    */
    const sapdbwa_Bool GetContentChunked() const;


    /*!**********************************************************************
      Function: GetPathTranslated

      description: Return the translated path of the incoming request.

      arguments:

      return value: A pointer to the translated path.
    */
    const char* GetPathTranslated() const;


    /*!**********************************************************************
      Function: GetServerName

      description: Return the host name of the webserver.

      arguments:

      return value: A pointer to the webserver name.
    */
    const char* GetServerName() const;


    /*!**********************************************************************
      Function: GetParameterNames

      description: Return a string sequence with all paramter names from the
      query string and from HTML form data.

      arguments:
        names  [out]  The string sequence for the parameter names.

      return value: true for success or false for memory allocation error.
    */
    sapdbwa_Bool GetParameterNames( sapdbwa_StringSeq &names ) const;


    /*!**********************************************************************
      Function: GetParameterValues

      description: Return the value(s) for a given parameter name.

      arguments:
        names  [out]  The string sequence for the parameter values.

      return value: true for success or false for memory allocation error.
    */
    sapdbwa_Bool GetParameterValues( const char *name, sapdbwa_StringSeq &values ) const;


    /*!**********************************************************************
      Function: IsSecure

      description: Check if the request is sent over a secure connection
                   (https with SSL).

      arguments:

      return value: true if connection is secure else false.
    */    
    sapdbwa_Bool IsSecure();

    /*!**********************************************************************
      Function: GetNumericalContentLength

      description: returns the content length of the request as an unsigned integer.

      arguments:

      return value: the content length.
    */    
    sapdbwa_ULong GetNumericalContentLength() const;

    /*!**********************************************************************
      Function: GetReadBodyBytes

      description: returns the number of bytes already read from the webserver's socket.

      arguments:

      return value: the number of bytes already read from the webserver's socket.
    */    
    sapdbwa_ULong GetReadBodyBytes() const;

    
    /*!**********************************************************************
      Function: ExtractValue

      description: Utility Function to extract the value out of a name value pair in a buffer.
                   'headerField' is assumed to be character sequence with length 'headerFieldLen'.
                   If it countains the substring 'UpperCaseName', 'LowerCaseName', 
                   or a mixed case variant of the two (which must be null terminated and have the same length), 
                   the index '*valueStart' is set to the position after the last character of '~CaseName'.
                   '*valueLen' is set to the maximum length of the following characters 
                   being a boundary in the sense of RFC 1521 (which is sufficient for most other kinds 
                   of header values as well). 
                   (The optional enclosing double quotes are trimmed (i.e. not counted).
                   The function returns true if a value was found, false otherwise.

      arguments:
        headerField    [in]    buffer containing a HTTP header field.
        headerFieldLen [in]    length of buffer containing the HTTP header field.
        upperCaseName  [in]    null terminated string containing the upper case Variant of the name.
        lowerCaseName  [in]    null terminated string containing the lower case Variant of the name.
        valueStart     [out]   Index within the header field where the value starts.
        valueLen       [out]   Length of value.

      return value: 'true' if a value was found, false otherwise.
    */    
    static sapdbwa_Bool ExtractValue(const char          * headerField,
                                     sapdbwa_ULong         headerFieldLen,
                                     const char          * upperCaseName,
                                     const char          * lowerCaseName,
                                     sapdbwa_ULong       & valueStart, 
                                     sapdbwa_ULong       & valueLen);


    /*!**********************************************************************
      Function: ReadBody

      description: Read the body data of a POST or PUT request.

      arguments:
        buff  [out]  Memory buffer the data is written to.
        len   [in]   Length of buffer.

      return value: The number of bytes read. If this is equal to len more
      data may be available in a subsequent call.
    */    
    sapdbwa_ULong ReadBody( char *buff, sapdbwa_ULong len );

    
    /*!**********************************************************************
      Function: MimeMultipartBody_NextPart

      description: (Terms in square brackets denote productions from the BNF for mime-multipart messages
                    in RFC 1521.)
                   The input parameter 'boundary' is a null-terminated ascii string and contains the [boundary].
                   This function reads up to (and including) next [delimiter] or [close-delimiter].
                   In the latter case the output parameter '*moreParts' is set to false (is set to true otherwise).

                   The return value of this function is true, or false if an error occurred.

                   Note that this function is mainly useful to set the read position initially after 
                   the first [delimiter]/[close-delimiter] (i.e. skipping the optional [preamble])
                   or to skip the remainder of a partially read mime part.
                   The function 'sapdbwa_MimeMultipartBody_PartBody' (see below) also sets the read position 
                   after the [delimiter]/[close-delimiter].


      arguments:
        boundary    [in]    The current mime multipart [boundary] as extracted from the request header
                            or or the header of a nested mime part.
        moreParts   [out]   Is true if more data of the current [encapsulation] is available 
                            for a subsequent call.

      return value: True, or false if an error occurred.
    */    
    sapdbwa_Bool MimeMultipartBody_NextPart(const char   * boundary,
                                            sapdbwa_Bool & moreParts);


    /*!**********************************************************************
      Function: MimeMultipartBody_PartHeaderField

      description: This function treats the next bytes from the request body as a (logical) line 
                   from the header of the current mime part.
                   Continuation lines (i.e. beginning with whitespace) are included without unfolding 
                   them (see RFC 822, "folding/unfolding").
                   It reads up to (and including) the terminal CRLF resp. CRLFCRLF (see RFC 1521) 
                   and returns the data in 'buff', which must be provided with (at least) size 'len'.
                   The number of bytes actually returned in 'buff' is returned in '*readBytes'.
                   If the output parameter '*moreData' is set to true, more data of this header line 
                   is available in a subsequent call to this function.
                   The output buffer 'buff' does not include these CRLF or CRLFCRLF , nor is it null terminated.
                   If the line was terminated with CRLFCRLF, the output parameter '*moreHeaderFields' 
                   is set to false, otherwise it is true.


      arguments:
        buff               [out]  Memory buffer the data is written to.
        len                [in]   Length of buffer.
        readBytes          [out]  Number of bytes read into the memory buffer.
        moreData           [out]  Is true if more data of the current part header is available
                                  in the request body for a subsequent call.
        moreHeaderFields   [out]  Is true if more data of another part header is available
                                  in the request body for a subsequent call.

      return value: True, or false if an error occurred.
    */    
    sapdbwa_Bool MimeMultipartBody_PartHeaderField(char           * buff,
                                                   sapdbwa_ULong    len,
                                                   sapdbwa_ULong  & readBytes,
                                                   sapdbwa_Bool   & moreData,
                                                   sapdbwa_Bool   & moreHeaderFields);



    /*!**********************************************************************
      Function: MimeMultipartBody_PartBody

      description: (Terms in square brackets denote productions from the BNF for mime-multipart messages
                    in RFC 1521.)
                   Input parameter 'boundary' is a null-terminated ascii string and contains the [boundary].
                   This function reads up to (and including) the next [delimiter] or [close-delimiter]
                   and returns the data in 'buff', which must be provided with (at least) size 'len'.
                   The number of bytes actually returned in 'buff' is returned in '*readBytes'.
                   If the output parameter '*moreData' is set to true, more data of this mime part 
                   is available in a subsequent call to this function.
                   The output buffer 'buff' does not include the [delimiter] or [close-delimiter] , 
                   nor is it null terminated.
                   If a [close-delimiter] was found, the output parameter '*moreParts' is set to false,
                   in case of a [delimiter] it is set to true.
                   The function 'sapdbwa_ReadBody' could subsequently be used to read the optional [epilogue].

      arguments:
        boundary   [in]   The current mime multipart [boundary] as extracted from the request header
                          or or the header of a nested mime part.
        buff       [out]  Memory buffer the data is written to.
        len        [in]   Length of buffer.
        readBytes  [out]  Number of bytes read into the memory buffer.
        moreData   [out]  Is true if more data of the current [body-part] is available
                          in the request body for a subsequent call.
        moreParts  [out]  Is false if the [close-delimiter] has been encountered

      return value: True, or false if an error occurred.
    */    
    sapdbwa_Bool MimeMultipartBody_PartBody(const char            * boundary,
                                            char                  * buff,
                                            sapdbwa_ULong           len,
                                            sapdbwa_ULong         & readBytes,
                                            sapdbwa_Bool          & moreData,
                                            sapdbwa_Bool          & moreParts);
    
private:
    sapdbwa_HttpRequestP req;
    sapdbwa_Err          err;
}; // class sapdbwa_HttpRequest

/*!**********************************************************************

  EndClass: sapdbwa_HttpRequest

************************************************************************/

/*!**********************************************************************

  Class: sapdbwa_HttpReply

  description:
  A sapdbwa_HttpReply object is used in the UserDll service function to send
  a HTTP reply back to the client. A HTTP reply consists of a status code
  several header fields and optional body data.

************************************************************************/

class sapdbwa_HttpReply
{
public:
    sapdbwa_HttpReply( sapdbwa_HttpReplyP rep ) : rep(rep), err(sapdbwa_RepGetErr(rep)) { };

    /*!**********************************************************************
      Function: GetErr

      description: Get error information from reply.

      arguments:

      return value:
      Reference to error object of the reply. This can be used to retrieve
      error information if a previous operation on the reply failed.
      See <A HREF="hwd06waerr.html">hwd06waerr.html</A> for a description
      of the error class.
    */
    const sapdbwa_Err& GetErr() const;

    /*!**********************************************************************
      Function: InitHeader

      description: Initialize the status code and some common HTTP header
      fields for the reply.

      arguments:
        status        [in]  The HTTP status code. These are defined in
                            hwd01wadef.h (STATUS_OK, STATUS_SERVER_ERROR, ...).
        contentType   [in]  The mime type of the body data.
        contentLength [in]  The length of the body data in bytes.
        lastModified  [in]  The last-modified header. Date time header fields
                            are specified in the WebAgent date-time format
                            (yyyymmddhhmmss).
        expires       [in]  The expires header. Date time header fields
                            are specified in the WebAgent date-time format
                            (yyyymmddhhmmss).
        location      [in]  The location header. This can be used to redirect
                            a request to another URL. The status has to be
                            STATUS_REDIRECT if this is set.

      return value: void.
    */
    sapdbwa_Bool InitHeader( sapdbwa_Int2  status,
                           const char *contentType   = NULL,
                           const char *contentLength = NULL,
                           const char *lastModified  = NULL,
                           const char *expires       = NULL,
                           const char *location      = NULL );


    /*!**********************************************************************
      Function: SetHeader

      description: Return the value for a HTTP header field.

      This function accepts only the HTTP-header names (e.g. content-length,
      content-type, ...).

      arguments:
        name  [in]  The name of the header field (e.g. content-length).
        value [in]  The value for the header field (e.g. text/html).

      return value: true if the setting of the header field was successful
      else false.
    */    
    sapdbwa_Bool SetHeader( const char *name, const char *value );

    
    /*!**********************************************************************
      Function: ReplaceHeader

      description: Replace the value of a given header field _and_ its old
                   value.

      This function accepts only the HTTP-header names (e.g. content-length,
      content-type, ...).

      arguments:
        name     [in]  The name of the header field (e.g. content-length).
        oldvalue [in]  The old value of the header field (e.g. text/html).
        value    [in]  The value for the header field (e.g. text/html).

      return value: true if the setting of the header field was successful
      else false.
    */    

    sapdbwa_Bool ReplaceHeader( const char *name, const char *oldvalue, const char *value );

    /*!**********************************************************************
      Function: SetCookie

      description: Set a cookie that is send to the client.

      Cookies have to be set before SentHeader is called.

      arguments:
        name    [IN] The name for the cookie. This must not be NULL.
                     This string is a sequence of characters excluding
                     semi-colon, comma and white space. If there is a need
                     to place such data in the name or value, some encoding
                     method such as URL style %XX encoding is recommended,
                     though no encoding is defined or required.
        value   [IN] The value of the cookie. This must not be NULL. For
                     the value the same restrictions apply than for name
                     above.
        expires [IN] The expires attribute specifies a date string that
                     defines the valid life time of that cookie. Once the
                     expiration date has been reached, the cookie will no
                     longer be stored or given out.
                    
                     The date string is formatted in WebAgent date-format:
                     YYYYMMDDHHMMSS. It is always spefified in
                     timezone GMT.

                     expires is an optional attribute. If it is NULL
                     or empty, the cookie will expire when the user's
                     session ends.
        path    [IN] The path attribute is used to specify the subset
                     of URLs in a domain for which the cookie is
                     valid. If a cookie has already passed domain
                     matching, then the pathname component of the URL
                     is compared with the path attribute, and if there
                     is a match, the cookie is considered valid and is
                     sent along with the URL request. The path "/foo"
                     would match "/foobar" and "/foo/bar.html". The
                     path "/" is the most general path.

                     If the path is NULL, it as assumed to be the same
                     path as the document being described by the header
                     which contains the cookie.
        domain  [IN] When searching the cookie list for valid cookies,
                     a comparison of the domain attributes of the
                     cookie is made with the Internet domain name of
                     the host
                     from which the URL will be fetched. If there is a
                     tail match, then the cookie will go through path
                     matching to see if it should be sent. "Tail
                     matching" means that domain attribute is matched
                     against the tail of the fully qualified domain
                     name of the host. A domain attribute of
                     "acme.com" would match host names
                     "anvil.acme.com" as well as
                     "shipping.crate.acme.com".
                     Only hosts within the specified domain can set a
                     cookie for a domain and domains must have at
                     least two (2) or three (3) periods in them to
                     prevent domains of the form: ".com", ".edu", and
                     "va.us". Any domain that fails within one of the
                     seven special top level domains listed below only
                     require two periods. Any other domain requires at
                     least three. The seven special top level domains
                     are: "COM", "EDU", "NET", "ORG", "GOV", "MIL",
                     and "INT".

                     If domain is NULL the host name of
                     the server which generated the cookie response is
                     used.
        secure  [IN] If a cookie is marked secure, it will only be
                     transmitted if the communications channel with
                     the host is a secure one. Currently this means
                     that secure cookies will only be sent to HTTPS
                     (HTTP over SSL) servers.

                     If secure is not specified, a cookie is
                     considered safe to be sent in the clear over
                     unsecured channels.
        
      return value: true for success, false for failure. */
    sapdbwa_Bool WINAPI SetCookie( const char   *name,
                                 const char   *value,
                                 const char   *expires = NULL,
                                 const char   *path = NULL,
                                 const char   *domain = NULL,
                                 sapdbwa_Bool    secure = false );

    /*!**********************************************************************
      Function: ReplaceCookieValue

      description: Replace the value of a former set cookie.
      
      arguments:
        name    [IN] The name for the cookie. This must not be NULL.
                     This string is a sequence of characters excluding
                     semi-colon, comma and white space. If there is a need
                     to place such data in the name or value, some encoding
                     method such as URL style %XX encoding is recommended,
                     though no encoding is defined or required.
        value   [IN] The value of the cookie. This must not be NULL. For
                     the value the same restrictions apply than for name
                     above.
        
      return value: true for success, false for failure. */
    sapdbwa_Bool WINAPI ReplaceCookieValue( const char   *name,
                                            const char   *value );

    /*!**********************************************************************
      Function: SendHeader

      description: Send the whole HTTP header to the client.

      Before this function is called at least InitHeader has to called.
      After this function was called further calls to InitHeader, SetHeader
      and SetCookie have no effect.

      arguments:

      return value: true if the HTTP header was succesfully sent else false.
    */    
    sapdbwa_Bool SendHeader();


    /*!**********************************************************************
      Function: SendBody

      description: Send body data to the client.

      SendBody may be called multiple times. Before SendBody is called
      SendHeader has to be called.

      arguments:
        buff [in] Buffer with body data.
        len  [in] Length of data in buff. If this is 0 all data until the
                  first null termination is copied. This is useful for
                  sending short strings.

      return value: The number of bytes sent.
    */
    sapdbwa_UInt4 SendBody( const void *buff, sapdbwa_UInt4  len = 0 );
    sapdbwa_UInt4 SendBodyChunk( const void *buff, sapdbwa_UInt4  len = 0 );
    sapdbwa_UInt4 SendBodyChunkEnd();

    /*!**********************************************************************
      Function: SendFile, SendTemplate

      description: Send file data to the client.

      SendFile sends a whole file via SendBody. If the file is not found
	  the reply header will be set with STATUS_NOT_FOUND. If the file could
	  be sent the reply header will be set to STATUS_OK with according
	  mime type.

      In opposit to SendFile, SendTemplate sends the file via SendBody
	  with no setting of reply header in any case.

      arguments:
        fileName [in] Name of the file.

      return value: True, if the file could be sent, else false.
    */
	sapdbwa_Bool SendFile( const char	*fileName   );
	sapdbwa_Bool SendTemplate( const char	*fileName );

private:
    sapdbwa_HttpReplyP rep;
    sapdbwa_Err		     err;

    friend class sapdbwa_WebAgent;
}; // class sapdbwa_HttpReply


/*!**********************************************************************

  EndClass: sapdbwa_HttpReply

************************************************************************/

/************************************************************************

  Chapter: Inline definitions of methods

************************************************************************/

/************************************************************************

  Chapter: methods for sapdbwa_WebAgent

************************************************************************/

inline sapdbwa_WebAgent::sapdbwa_WebAgent( sapdbwa_Handle wa )
    : wa(wa), err(sapdbwa_HandleGetErr(wa)), pool(sapdbwa_GetSessionPool( wa ))
{
}; // sapdbwa_WebAgent


inline sapdbwa_Bool sapdbwa_WebAgent::GetParam( const char  *key,
                                         char        *valueBuf,
                                         sapdbwa_Int4   bufLen )
{
    return sapdbwa_GetParam( this->wa, key, valueBuf, bufLen );
}; // GetParam


inline sapdbwa_Bool sapdbwa_WebAgent::GetDocumentRoot( char	        *valueBuf,
													   sapdbwa_Int4	 bufLen   )
{
    return sapdbwa_GetDocumentRoot( this->wa, valueBuf, bufLen );
};


inline sapdbwa_Bool sapdbwa_WebAgent::GetSessionPool( sapdbwa_SessionPool* &myPool )
{
    if (this->pool.GetPool() != NULL) {
        myPool = &this->pool;
    } else {
        myPool = NULL;
    }; // else
    
    return (myPool != NULL);
}; // GetSessionPool


inline const sapdbwa_Err& sapdbwa_WebAgent::GetErr() const
{
    return this->err;
}; // GetErr


inline sapdbwa_Bool sapdbwa_WebAgent::WriteLogMsg( const char *msg ) const
{
    return sapdbwa_WriteLogMsg( this->wa, msg );
}; // WriteLogMsg


inline sapdbwa_Bool sapdbwa_WebAgent::GetWebSession( sapdbwa_WebSession  &webSession ) const
{
    sapdbwa_WebSessionP ses = sapdbwa_GetWebSession( this->wa );
    webSession = sapdbwa_WebSession( ses );

    return (ses != NULL);
}; // GetWebSession

inline void sapdbwa_WebAgent::ReleaseWebSession( ) const
{
    sapdbwa_ReleaseWebSession( this->wa );

}; // ReleaseWebSession

inline sapdbwa_Bool sapdbwa_WebAgent::StartWebSession( sapdbwa_WebSession  &webSession,
                                           sapdbwa_UInt4        timeOut )
{
    sapdbwa_WebSessionP ses = sapdbwa_StartWebSession( this->wa, timeOut );
    webSession = sapdbwa_WebSession( ses );

    return (ses != NULL);
}; // StartWebSession

inline sapdbwa_Bool sapdbwa_WebAgent::StartNewWebSession( sapdbwa_WebSession  &webSession,
                                                          sapdbwa_HttpReply   &reply,
                                                          sapdbwa_UInt4        timeOut )
{
    sapdbwa_WebSessionP ses = sapdbwa_StartNewWebSession( this->wa, reply.rep, timeOut );
    webSession = sapdbwa_WebSession( ses );

    return (ses != NULL);
}; // StartNewWebSession

inline sapdbwa_Bool sapdbwa_WebAgent::StartDefaultWebSession( sapdbwa_WebSession  &webSession )
{
    sapdbwa_WebSessionP ses = sapdbwa_StartDefaultWebSession( this->wa );
    webSession = sapdbwa_WebSession( ses );

    return (ses != NULL);
}; // StartDefaultWebSession


inline sapdbwa_Bool sapdbwa_WebAgent::CloseWebSession( sapdbwa_WebSession &webSession,
													   sapdbwa_Bool		   freeUserData )
{
    return sapdbwa_CloseWebSession( this->wa, webSession.GetWebSession(), freeUserData );
}; // CloseWebSession


inline sapdbwa_Bool sapdbwa_WebAgent::SetUseWebSessions( sapdbwa_Bool useWebSessions )
{
    return sapdbwa_SetUseWebSessions( this->wa, useWebSessions );
}; // SetUseWebSessions


inline const char* sapdbwa_WebAgent::GetServiceName()
{
    return sapdbwa_GetServiceName(this->wa);
};

/************************************************************************

  Chapter: methods for sapdbwa_WebSession

************************************************************************/

inline sapdbwa_WebSession::sapdbwa_WebSession( sapdbwa_WebSessionP webSession )
    : webSession(webSession)
{
}; // sapdbwa_WebSession

inline sapdbwa_WebSession::sapdbwa_WebSession()
    : webSession(NULL)
{
}; // sapdbwa_WebSession


inline sapdbwa_WebSession::~sapdbwa_WebSession()
{    
}; // ~sapdbwa_WebSession


inline void* sapdbwa_WebSession::GetSessionData() const
{
    return sapdbwa_GetSessionData( this->webSession );
}; // GetSessionData


inline sapdbwa_Bool sapdbwa_WebSession::SetSessionData( void            *data,
                                                 sapdbwa_CppDestructor destructor )
{
    return sapdbwa_SetSessionData( this->webSession,
                              data,
                              (sapdbwa_CDestructor) destructor );
}; // SetSessionData


inline sapdbwa_Bool sapdbwa_WebSession::NewSessionId()
{
    return sapdbwa_NewSessionId( this->webSession );
}; // NewSessionId


inline sapdbwa_WebSessionP sapdbwa_WebSession::GetWebSession() const
{
    return this->webSession;
}; // GetWebSession

/************************************************************************

  Chapter: methods for sapdbwa_StringSeq

************************************************************************/

inline sapdbwa_StringSeq::sapdbwa_StringSeq()
{
    this->stringSeq = sapdbwa_CreateStringSeq();
}; /* sapdbwa_StringSeq */

    
inline sapdbwa_StringSeq::~sapdbwa_StringSeq()
{
    sapdbwa_DestroyStringSeq( this->stringSeq );
}; /* ~sapdbwa_StringSeq */


inline sapdbwa_Int4 sapdbwa_StringSeq::GetNumElem() const
{
    return sapdbwa_GetNumElem( this->stringSeq );
}; /* GetNumElem */


inline const char* sapdbwa_StringSeq::operator[]( sapdbwa_Int4 index ) const
{
    return sapdbwa_GetStringByIndex( this->stringSeq, index );
}; /* operator[] */


inline sapdbwa_StringSeqP sapdbwa_StringSeq::GetStringSeq() const
{
    return this->stringSeq;
}; /* sapdbwa_StringSeqP */

/************************************************************************

  Chapter: methods for sapdbwa_HttpRequest

************************************************************************/

inline const sapdbwa_Err& sapdbwa_HttpRequest::GetErr() const
{
    return this->err;
}; // GetErr


inline sapdbwa_Bool
sapdbwa_HttpRequest::GetAuthorization( const char **userName,
                                 const char **password ) const
{
    return sapdbwa_GetAuthorization( this->req, userName, password );
}; // GetErr


inline sapdbwa_Bool sapdbwa_HttpRequest::GetCookieNames( sapdbwa_StringSeq &names ) const
{
    return sapdbwa_GetCookieNames( this->req, names.GetStringSeq() );
}; /* GetCookieNames */


inline const char* sapdbwa_HttpRequest::GetCookieValue( const char *name ) const
{
    return sapdbwa_GetCookieValue( this->req, name );
}; /* GetCookieValue */


inline const char* sapdbwa_HttpRequest::GetHeader( const char *fieldName ) const
{
    return sapdbwa_GetHeader( this->req, fieldName );
}; /* GetHeader */


inline const char* sapdbwa_HttpRequest::GetRequestURI() const
{
    return sapdbwa_GetRequestURI( this->req );
}; /* GetRequestURI */


inline const char* sapdbwa_HttpRequest::GetIfModifiedSince() const
{
    return sapdbwa_GetIfModifiedSince( this->req );
}; /* GetIfModifiedSince */


inline const char* sapdbwa_HttpRequest::GetQueryString() const
{
    return sapdbwa_GetQueryString( this->req );
}; /* GetQueryString */


inline const char* sapdbwa_HttpRequest::GetPathInfo() const
{
    return sapdbwa_GetPathInfo( this->req );
}; /* GetPathInfo */


inline const char* sapdbwa_HttpRequest::GetMethod() const
{
    return sapdbwa_GetMethod( this->req );
}; /* GetMethod */


inline const char* sapdbwa_HttpRequest::GetContentType() const
{
    return sapdbwa_GetContentType( this->req );
}; /* GetContentType */


inline const char* sapdbwa_HttpRequest::GetContentLength() const
{
    return sapdbwa_GetContentLength( this->req );
}; /* GetContentLength */

inline const sapdbwa_Bool sapdbwa_HttpRequest::GetContentChunked() const
{
    return sapdbwa_GetContentChunked( this->req );
};


inline const char* sapdbwa_HttpRequest::GetPathTranslated() const
{
    return sapdbwa_GetPathTranslated( this->req );
}; /* GetPathTranslated */


inline const char* sapdbwa_HttpRequest::GetServerName() const
{
    return sapdbwa_GetServerName( this->req );
}; /* GetServerName */


inline sapdbwa_Bool sapdbwa_HttpRequest::GetParameterNames( sapdbwa_StringSeq &names ) const
{
    return sapdbwa_GetParameterNames( this->req, names.GetStringSeq() );
}; /* GetParameterNames */

inline sapdbwa_Bool sapdbwa_HttpRequest::GetParameterValues( const char   *name,
                                                     sapdbwa_StringSeq &values ) const
{
    return sapdbwa_GetParameterValues( this->req, name, values.GetStringSeq() );
}; /* GetParameterValues */


inline sapdbwa_Bool sapdbwa_HttpRequest::IsSecure()
{
    return sapdbwa_IsSecure( this->req );
}; // IsSecure

inline sapdbwa_ULong sapdbwa_HttpRequest::GetNumericalContentLength() const
{
    return sapdbwa_GetNumericalContentLength( this->req );
}

inline sapdbwa_ULong sapdbwa_HttpRequest::GetReadBodyBytes() const
{
    return sapdbwa_GetReadBodyBytes( this->req );
}

inline sapdbwa_Bool sapdbwa_HttpRequest::ExtractValue(const char          * headerField,
                                                      sapdbwa_ULong         headerFieldLen,
                                                      const char          * upperCaseName,
                                                      const char          * lowerCaseName,
                                                      sapdbwa_ULong       & valueStart, 
                                                      sapdbwa_ULong       & valueLen)
{
    return sapdbwa_ExtractValue(headerField, headerFieldLen, upperCaseName, lowerCaseName, &valueStart, &valueLen);
}

inline sapdbwa_ULong sapdbwa_HttpRequest::ReadBody( char *buff, sapdbwa_ULong len )
{
    return sapdbwa_ReadBody( this->req, buff, len );
}; /* ReadBody */

inline sapdbwa_Bool sapdbwa_HttpRequest::MimeMultipartBody_NextPart(const char   * boundary,
                                                                    sapdbwa_Bool & moreParts)
{
    return sapdbwa_MimeMultipartBody_NextPart(this->req, boundary, &moreParts);
} /* MimeMultipartBody_NextPart */

inline sapdbwa_Bool sapdbwa_HttpRequest::MimeMultipartBody_PartHeaderField(char           * buff,
                                                                           sapdbwa_ULong    len,
                                                                           sapdbwa_ULong  & readBytes,
                                                                           sapdbwa_Bool   & moreData,
                                                                           sapdbwa_Bool   & moreHeaderFields)
{
    return sapdbwa_MimeMultipartBody_PartHeaderField(this->req, buff, len, &readBytes, &moreData, &moreHeaderFields);
} /* MimeMultipartBody_PartHeaderField */

inline sapdbwa_Bool sapdbwa_HttpRequest::MimeMultipartBody_PartBody(const char            * boundary,
                                                                    char                  * buff,
                                                                    sapdbwa_ULong           len,
                                                                    sapdbwa_ULong         & readBytes,
                                                                    sapdbwa_Bool          & moreData,
                                                                    sapdbwa_Bool          & moreParts)
{
    return sapdbwa_MimeMultipartBody_PartBody(this->req, boundary, buff, len, &readBytes, &moreData, &moreParts);
} /* MimeMultipartBody_PartBody */



/************************************************************************

  Chapter: methods for sapdbwa_HttpReply

************************************************************************/

inline const sapdbwa_Err& sapdbwa_HttpReply::GetErr() const
{
    return this->err;
}; // GetErr


inline sapdbwa_Bool sapdbwa_HttpReply::InitHeader( sapdbwa_Int2  status,
                                            const char *contentType, 
                                            const char *contentLength, 
                                            const char *lastModified, 
                                            const char *expires,
                                            const char *location )
{
    return sapdbwa_InitHeader( this->rep,
                          status,
                          contentType,
                          contentLength,
                          lastModified,
                          expires,
                          location );
}; /* InitHeader */


inline sapdbwa_Bool sapdbwa_HttpReply::SetHeader( const char *name,
                                          const char *value )
{
    return sapdbwa_SetHeader( this->rep, name, value );
}; /* SetHeader */

inline sapdbwa_Bool sapdbwa_HttpReply::ReplaceHeader( const char *name,
                                                      const char *oldvalue,
                                                      const char *value )
{
    return sapdbwa_ReplaceHeader( this->rep, name, oldvalue, value );
}; /* SetHeader */

inline sapdbwa_Bool sapdbwa_HttpReply::SetCookie( const char   *name,
                                           const char   *value,
                                           const char   *expires,
                                           const char   *path,
                                           const char   *domain,
                                           sapdbwa_Bool        secure )
{
    return sapdbwa_SetCookie( this->rep,
                         name, value,
                         expires, path, domain, secure );
}; /* SetCookie */

inline sapdbwa_Bool sapdbwa_HttpReply::ReplaceCookieValue ( const char   *name,
                                                            const char   *value )
{
    return sapdbwa_ReplaceCookieValue( this->rep, name, value );

}; /* ReplaceCookieValue */

inline sapdbwa_Bool sapdbwa_HttpReply::SendHeader()
{
    return sapdbwa_SendHeader( this->rep );
}; /* SendHeader */


inline sapdbwa_UInt4 sapdbwa_HttpReply::SendBody( const void *buff,
                                          sapdbwa_UInt4     len )
{
    return sapdbwa_SendBody( this->rep, (void*) buff, len );
}; /* SendBody */

inline sapdbwa_UInt4 sapdbwa_HttpReply::SendBodyChunk( const void       *buff,
                                                       sapdbwa_UInt4     len )
{
    return sapdbwa_SendBodyChunk( this->rep, (void*) buff, len );
}; /* SendBodyChunk */

inline sapdbwa_UInt4 sapdbwa_HttpReply::SendBodyChunkEnd()
{
    return sapdbwa_SendBodyChunkEnd( this->rep );
}; /* SendBodyChunkEnd */


inline sapdbwa_Bool sapdbwa_HttpReply::SendFile( const char *fileName )
{
    return sapdbwa_SendFile( this->rep, fileName );
}; /* SendFile */

inline sapdbwa_Bool sapdbwa_HttpReply::SendTemplate( const char *fileName )
{
    return sapdbwa_SendTemplate( this->rep, fileName );
}; /* SendTemplate */
/************************************************************************

  EndChapter: Inline definitions of methods

************************************************************************/

#endif
