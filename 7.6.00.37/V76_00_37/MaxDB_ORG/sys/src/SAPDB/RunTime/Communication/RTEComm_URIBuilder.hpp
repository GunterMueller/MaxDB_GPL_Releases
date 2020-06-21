/*!========================================================================

  @file         RTEComm_URIBuilder.hpp
  @ingroup      Runtime
  @author       StefanP

  @brief        Build a Uniform Resource Identifier (URI)

                The liveCache Uniform Resource Identifier (URI) is a compact
                string of characters used to identify liveCache specific 
                resources.
                The following class is used to build up such an URI from given
                URI parts.

  @since        2005-01-14  18:40
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
  ----------------------------------------------------------------------------*/

#ifndef RTECOMM_URIBUILDER_HPP
#define RTECOMM_URIBUILDER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Communication/RTEComm_URIUtils.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic Communication_Trace;	///<Communication trace

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define URI_LOCALHOST_NAME              "localhost"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

       

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class   RTEComm_URIBuilder
  @brief   From this class a URI may be build from given URI parts such as host name, database etc which complies with the 
           liveCache URI specification. 
           Furthermore additional options can be appended later on.
           The user of this class has two opportunities to do so. If no buffer is given at construction time the 
           memory allocation is done by class itself. If on the other hand a buffer is given it has to be assured that
           the buffer is large enough to hold the entiry URI. Otherwise an error is replied.
           An instance of this class can be used frequently by calling the 'BuildDatabaseURI ()' method without explizit 
           call to the 'CleanUp ()' method before. 

           Currently not all URI's from the liveCache URI specification are supported.

            1. Scheme (liveCache" | "maxdb") is not supported yet
            2. Only URI's for database, listener and DBM server paths can be build by now. Some restrictions concerning
               additionally path elements also exist (cf. liveCache URI specification).
  ----------------------------------------------------------------------------*/

class RTEComm_URIBuilder : public RTEComm_URIUtils
{
public:

    //
    // DON'T FORGET SAPDB_ToStringClass BELOW!!!!
    //
    enum URIBuildRC    {NoError = 0,          ///<No error
                        Error,                ///<An unspecified Error occured
                        URIBufferToSmall,     ///<The given buffer at construction of the instance is to small to hold the URI to build
                        NeedlessPort,         ///<A needless port number was supplied
                        NeedlessEncryption,   ///<A needless encryption flag was supplied
                        WrongProtocol         ///<Wrong protocol   
                        };
    /*! 
        @brief    Contructor of URI Utility class 
        @param      uriBuffer [in]      - Buffer to be used to store the URI. The buffer has to be well initialized
                                          with an URI string or an leading null byte. (default: NULL)
                                          If no buffer is given memory for the URI is allocated by the object itself
        @param      maxLengthURI [in]   - This parameter specifies the maximum length of an URI which
                                          can be hold by the given buffer (default: 0).
    
    */
    RTEComm_URIBuilder    (SAPDB_Char * const     uriBuffer = NULL,
                           SAPDB_UInt4            maxLengthURI = 0)
        :   m_URI (uriBuffer),
            m_MaxLengthURI (maxLengthURI),   //excluding terminating NULL byte
            m_UseOwnMemory (false),
            m_FirstOption (true)
    {
        SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::RTEComm_URIBuilder", Communication_Trace, 9);
        m_LengthURI = NULL != m_URI ? static_cast <SAPDB_UInt4>(strlen (m_URI)) : 0;
    }

    /*! @brief destructor of URI Builder class */
    virtual ~RTEComm_URIBuilder()
    { 
        CleanUp();
    }

    /*! 
        @brief    Build up an escaped URI string from given fragments describing a database path. 
                  If 'localhost' is given as hostname a URI for remote communication is build.

        @param      serverDB [in]           - Name of the Server DB
        @param      host [in]               - Host or server name respectively. If NULL is given a
                                              URI for local communication is build.
        @param      port [in]               - Port number as string or NULL (use default port)
        @param      msgList [out]           - Message list
        @param      encryptionWanted [in]   - true (false): encryption is (not) wanted (default: false)
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall, NeedlessPort, NeedlessEncryption: URI was not build
     */
    URIBuildRC  BuildDatabaseURI                   (SAPDB_UTF8 const * const  serverDB,
                                                    SAPDB_Char const * const  host,
                                                    SAPDB_Char const * const  port,
                                                    Msg_List &                msgList,
                                                    SAPDB_Bool                encryptionWanted = false);
    /*! 
        @brief    Build up an escaped URI string from given fragments describing a database path.
                  If 'localhost' is given as hostname a URI for remote communication is build.

        @param      serverDB [in]           - Name of the Server DB
        @param      host [in]               - Host or server name respectively. A port may also be specified by
                                              append ":<port>" to the hostname.
                                              If NULL is given a URI for local communication is build.
        @param      msgList [out]           - Message list
        @param      encryptionWanted [in]   - true (false): encryption is (not) wanted (default: false)
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall, NeedlessEncryption: URI was not build
     */
    URIBuildRC  BuildDatabaseURI                   (SAPDB_UTF8 const * const  serverDB,
                                                    SAPDB_Char const * const  host,
                                                    Msg_List &                msgList,
                                                    SAPDB_Bool                encryptionWanted = false)
    {
        return (BuildDatabaseURI (serverDB, host, NULL, msgList, encryptionWanted));
    }

    /*! 
        @brief    Build up an escaped URI string for the local communication protocol from 
                  given fragments describing a database path.

        @param      serverDB [in]           - Name of the Server DB
        @param      msgList [out]           - Message list
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall: URI was not build
     */
    URIBuildRC  BuildDatabaseURI                   (SAPDB_UTF8 const * const  serverDB,
                                                    Msg_List &                msgList)
    {
        return (BuildDatabaseURI (serverDB, (SAPDB_Char *)0, (SAPDB_Char *)0, msgList));
    }


    /*! 
        @brief    Build up an escaped URI string from given fragments describing a listener path. 
                  If 'localhost' is given as hostname a URI for remote communication is build.

        @param      host [in]               - Host or server name respectively.
                                              If NULL is given a URI for local communication is build.
        @param      port [in]               - Port number as string or NULL (use default port)
        @param      msgList [out]           - Message list
        @param      encryptionWanted [in]   - true (false): encryption is (not) wanted (default: false)
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall, NeedlessPort, NeedlessEncryption, WrongProtocol: URI was not build
     */
    URIBuildRC  BuildListenerURI                   (SAPDB_Char const * const  host,
                                                    SAPDB_Char const * const  port,
                                                    Msg_List &                msgList,
                                                    SAPDB_Bool                encryptionWanted = false);

    /*! 
        @brief    Build up an escaped URI string from given fragments describing a listener path.
                  If 'localhost' is given as hostname a URI for remote communication is build.

        @param      host [in]               - Host or server name respectively. A port may also be specified by
                                              append ":<port>" to the hostname.
                                              If NULL is given a URI for local communication is build.
        @param      msgList [out]           - Message list
        @param      encryptionWanted [in]   - true (false): encryption is (not) wanted (default: false)
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall, NeedlessEncryption, WrongProtocol: URI was not build
     */
    URIBuildRC  BuildListenerURI                   (SAPDB_Char const * const  host,
                                                    Msg_List &                msgList,
                                                    SAPDB_Bool                encryptionWanted = false)
    {
        return (BuildListenerURI (host, NULL, msgList, encryptionWanted));
    }

    /*! 
        @brief    Build up an escaped URI string from given fragments describing a database path over the 
                  DBM server.
                  If 'localhost' is given as hostname a URI for remote communication is build.

        @param      serverDB [in]           - Name of the Server DB or NULL (destination is the DBMServer)
        @param      host [in]               - Host or server name respectively.
                                              If NULL is given a URI for local communication is build.
        @param      port [in]               - Port number as string or NULL (use default port)
        @param      msgList [out]           - Message list
        @param      encryptionWanted [in]   - true (false): encryption is (not) wanted (default: false)
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall, NeedlessPort, NeedlessEncryption: URI was not build
     */
    URIBuildRC  BuildDBMURI                        (SAPDB_UTF8 const * const  serverDB,
                                                    SAPDB_Char const * const  host,
                                                    SAPDB_Char const * const  port,
                                                    Msg_List &                msgList,
                                                    SAPDB_Bool                encryptionWanted = false);

    /*! 
        @brief    Build up an escaped URI string from given fragments describing a database path over the 
                  DBM server. 
                  If 'localhost' is given as hostname a URI for remote communication is build.

        @param      serverDB [in]           - Name of the Server DB or NULL (destination is the DBMServer)
        @param      host [in]               - Host or server name respectively. A port may also be specified by
                                              append ":<port>" to the hostname.
                                              If NULL is given a URI for local communication is build.
        @param      msgList [out]           - Message list
        @param      encryptionWanted [in]   - true (false): encryption is (not) wanted (default: false)
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall, NeedlessEncryption: URI was not build
     */
    URIBuildRC  BuildDBMURI                        (SAPDB_UTF8 const * const  serverDB,
                                                    SAPDB_Char const * const  host,
                                                    Msg_List &                msgList,
                                                    SAPDB_Bool                encryptionWanted = false)
    {
        return (BuildDBMURI (serverDB, host, NULL, msgList, encryptionWanted));
    }

    /*! 
        @brief    Build up an escaped URI string for the local communication protocol from 
                  given fragments describing a database path over the DBM server.
        @param      serverDB [in]           - Name of the Server DB or NULL (destination is the DBMServer)
        @param      msgList [out]           - Message list
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall: URI was not build
     */
    URIBuildRC  BuildDBMURI                        (SAPDB_UTF8 const * const  serverDB,
                                                    Msg_List &                msgList)
    {
        return (BuildDBMURI (serverDB, NULL, NULL, msgList));
    }

    /*! 
        @brief    Build up an escaped URI string from given fragments describing a DBM server path. 
                  If 'localhost' is given as hostname a URI for remote communication is build.

        @param      host [in]               - Host or server name respectively.
                                              If NULL is given a URI for local communication is build.
        @param      port [in]               - Port number as string or NULL (use default port)
        @param      msgList [out]           - Message list
        @param      encryptionWanted [in]   - true (false): encryption is (not) wanted (default: false)
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall, NeedlessPort, NeedlessEncryption: URI was not build
     */
    URIBuildRC  BuildDBMURI                        (SAPDB_Char const * const  host,
                                                    SAPDB_Char const * const  port,
                                                    Msg_List &                msgList,
                                                    SAPDB_Bool                encryptionWanted = false)
    {
        return (BuildDBMURI (NULL, host, port, msgList, encryptionWanted));
    }

    /*! 
        @brief    Build up an escaped URI string from given fragments describing a DBM server path. 
                  If 'localhost' is given as hostname a URI for remote communication is build.

        @param      host [in]               - Host or server name respectively. A port may also be specified by
                                              append ":<port>" to the hostname.
                                              If NULL is given a URI for local communication is build.
        @param      msgList [out]           - Message list
        @param      encryptionWanted [in]   - true (false): encryption is (not) wanted (default: false)
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall, NeedlessEncryption: URI was not build
     */
    URIBuildRC  BuildDBMURI                        (SAPDB_Char const * const  host,
                                                    Msg_List &                msgList,
                                                    SAPDB_Bool                encryptionWanted = false)
    {
        return (BuildDBMURI (NULL, host, NULL, msgList, encryptionWanted));
    }

    /*! 
        @brief    Build up an escaped URI string for the local communication protocol from 
                  given fragments describing a DBM Server path.

        @param      msgList [out]           - Message list
        
        @return   NoError: URI was build.
                  Error, URIBufferToSmall: URI was not build
     */
    URIBuildRC  BuildDBMURI                        (Msg_List &                msgList)
    {
        return (BuildDBMURI (NULL, NULL, NULL, msgList));
    }

  /*!
        @brief    Append option to an URI string: '?optionName=optionValue' for the first option
                  or '&optionName=optionValue" for further options respectively. 

        @param      optionName [in]         - Name of the option
        @param      optionValue [in]        - Value of the option [SAPDB_Int]
        @param      msgList [out]           - Message list

        @return   NoError: Option was appended.
                  Error, URIBufferToSmall: Option was not appended.
  */
    URIBuildRC  Append                             (SAPDB_UTF8 const * const       optionName,
                                                    SAPDB_Int  const               optionValue,
                                                    Msg_List &                     msgList)
        {
            return (Append (optionName, (SAPDB_UTF8 *)(SAPDB_Char *)(SAPDB_ToString (optionValue)),msgList));
        }

  /*!
        @brief    Append option to an URI string: '?optionName=optionValue' for the first option
                  or '&optionName=optionValue" for further options respectively. 

        @param      optionName [in]         - Name of the option
        @param      optionValue [in]        - Value of the option [SAPDB_UInt4] 
        @param      msgList [out]           - Message list

        @return   NoError: Option was appended.
                  Error, URIBufferToSmall: Option was not appended.

  */

    URIBuildRC  Append                             (SAPDB_UTF8 const * const       optionName,
                                                    SAPDB_UInt4 const              optionValue,
                                                    Msg_List &                     msgList)
        {
            return (Append (optionName, (SAPDB_UTF8 *)(SAPDB_Char *)(SAPDB_ToString (optionValue)), msgList));
        }

  /*!
        @brief    Append option to an URI string: '?optionName=optionValue' for the first option
                  or '&optionName=optionValue" for further options respectively. 

        @param      optionName [in]         - Name of the option
        @param      optionValue [in]        - Value of the option [SAPDB_Char *] 
        @param      msgList [out]           - Message list

        @return   NoError: Option was appended.
                  Error, URIBufferToSmall: Option was not appended.
  */

    URIBuildRC  Append                              (SAPDB_UTF8 const * const       optionName,
                                                     SAPDB_Char const * const       optionValue,
                                                     Msg_List &                     msgList)
        {
            return (Append (optionName, (SAPDB_UTF8 *)(optionValue), msgList));
        }

  /*!
        @brief    Append option to an URI string: '?optionName=optionValue' for the first option
                  or '&optionName=optionValue" for further options respectively. 

        @param      optionName [in]         - Name of the option
        @param      optionValue [in]        - Value of the option [SAPDB_UTF8 *]
        @param      msgList [out]           - Message list

        @return   NoError: Option was appended.
                  Error, URIBufferToSmall: Option was not appended.
  */
  
    URIBuildRC  Append                             (SAPDB_UTF8 const * const       optionName,
                                                    SAPDB_UTF8 const * const       optionValue,
                                                    Msg_List &                     msgList);            


    /*!
        @brief    Get current URI string (escaped).
        
                  Returns the pointer to the stored escaped URI string
                  which has been build from the given fragments.

        @return   Pointer to the escaped URI or NULL if no URI was build yet.
     */
    SAPDB_Char const * const    GetURI    () const
    {
        SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::GetURI", Communication_Trace, 9);
        return m_URI;
    }

    /*!
        @brief    Get the length of the current URI string (escaped).
        
                  Returns the the length of the stored escaped URI string
                  which has been build from the given fragments.

        @return   Length of the URI.
     */
    SAPDB_UInt4                 GetURILength    () const
    {
        SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::GetURILength", Communication_Trace, 9);
        return m_LengthURI;
    }


    /*!
        @brief    Clean up all resources

        @return   none
     */

    void                        CleanUp ();

private:
    // Do not allow to use copy constructor outside
	RTEComm_URIBuilder     (const RTEComm_URIBuilder & uriBuilder)
        :   m_URI (uriBuilder.m_URI),
            m_MaxLengthURI (uriBuilder.m_MaxLengthURI),
            m_UseOwnMemory (uriBuilder.m_UseOwnMemory),
            m_FirstOption (uriBuilder.m_FirstOption),
            m_LengthURI (uriBuilder.m_LengthURI)
        {;}

    /*!
      @brief    Build up URI
     */

    URIBuildRC  BuildURI                           (SAPDB_Char const * const  path,
                                                    SAPDB_Char const * const  protocol,
                                                    SAPDB_UTF8 const * const  serverDB,
                                                    SAPDB_UInt4               lenServerDB,
                                                    SAPDB_Char const * const  host,
                                                    SAPDB_Char const * const  port,
                                                    SAPDB_Bool                encryptionWanted,
                                                    Msg_List &                msgList);

    /*!
      @brief    Get protocol name from host name

      @return   PROTOCOL_LOCAL, PROTOCOL_REMOTE, PROTOCOL_REMOTESSL, PROTOCOL_SAPNI, PROTOCOL_SAPNISSL or 'NULL' if an error occured
     */
    SAPDB_Char const * const    GetProtocol    (SAPDB_Char const * const    host, 
                                                SAPDB_Bool                  encryptionWanted,
                                                Msg_List &                  msgList);

  /*!
     @brief   AddSeparator

              Add  the separator to the URI string. It depends which separator is chosen:
              OPTION_PART_SEPARATOR if it is the first separator else OPTION_SEPARATOR. 

     @return  true:		Separator was appended
              false:	Buffer is to small to append the separator
  */

    void    AddSeparator    ();

private:
//    static RTE_Nodename     m_LocalNodeName;

    SAPDB_Char *            m_URI;
    SAPDB_Bool              m_UseOwnMemory;
    SAPDB_UInt4             m_LengthURI;
    SAPDB_Bool              m_FirstOption;
    SAPDB_UInt4             m_MaxLengthURI;
};

/*!
  @brief convert RTEComm_URIBuilder::URIBuildRC into readable string
  @param state [in] the return value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString( RTEComm_URIBuilder::URIBuildRC retVal )
{
    switch (retVal)
    {
    case RTEComm_URIBuilder::NoError                : return SAPDB_ToStringClass("No error");
    case RTEComm_URIBuilder::Error                  : return SAPDB_ToStringClass("Error");
    case RTEComm_URIBuilder::URIBufferToSmall       : return SAPDB_ToStringClass("URI Buffer to small");
    case RTEComm_URIBuilder::NeedlessPort           : return SAPDB_ToStringClass("Needless port");
    case RTEComm_URIBuilder::NeedlessEncryption     : return SAPDB_ToStringClass("Needless encryption");
    case RTEComm_URIBuilder::WrongProtocol          : return SAPDB_ToStringClass("Wrong protocol");
    }

    return SAPDB_ToStringClass((SAPDB_Int4)retVal);
}

#endif  /* RTECOMM_URIBUILDER_HPP */
