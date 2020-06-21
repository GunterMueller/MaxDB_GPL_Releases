/*!
  @file           DBMSrvKnl_Session.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to Kernel Communication - Specification

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
*/

#ifndef _DBMSrvKnl_Session_HPP_
#define _DBMSrvKnl_Session_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_SQLRecord.hpp"
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_SQLField.hpp"

/*
  -----------------------------------------------------------------------------
  forward declarations
  -----------------------------------------------------------------------------
 */
class  PIn_ReplySegment;

/*!
  @brief     Kernel session object

  One object of this class represents a kernel session

 */
class DBMSrvKnl_Session {

public:
  /*! @brief   user type */
  typedef enum {
    userUnknown,
    userDBM,
    userSYSDBA,
    userSQL
  } UserType;

  /*! @brief   task type */
  typedef enum {
    taskUnknown,
    taskNormal,
    taskEvent
  } TaskType;

  /*! @brief   result type */
  typedef enum {
    resultUnknown,
    resultNothing,
    resultError,
    resultMessage,
    resultUtility,
    resultSQL
  } ResultType;

  /*! \brief type of implicit reconnects */
  enum SubsessionType
  {
      dbExecute,
      backupRestoreCheck,
      noSubsession        //!< must remain last in enum
  };

  /*! @brief   Constructor */
  DBMSrvKnl_Session
    (  );

  /*! @brief   Destructor */
  ~DBMSrvKnl_Session
    (  );

  /*! @brief connect to user task */
  bool NormalConnect
    ( const char      * szDB,
      const char      * szUser,
      const char      * szPassword,
      DBMSrvMsg_Error & oMsgList,
      bool              bWithNoTempFiles = false );

  /*! @brief   connect to  event task */
  bool EventConnect
    ( const char      * szDB,
      DBMSrvMsg_Error & oMsgList );

  /*! @brief   execute a statement */
  bool Execute
    ( const char      * szCommand,
      ResultType      & oResultType,
      DBMSrvMsg_Error & oMsgList );

  /*! @brief   execute a statement */
  bool Execute
    ( const char      * szCommand,
      DBMSrvMsg_Error & oMsgList );

  /*! @brief   execute a statement */
  bool ExecuteWithoutCommit
    ( const char      * szCommand, 
      DBMSrvMsg_Error & oMsgList );

  /*! @brief   execute a statement */
  static bool Execute
    ( const char      * szDB,
      const char      * szUser,
      const char      * szPassword,
      const char      * szCommand,
      DBMSrvMsg_Error & oMsgList );

  /*! @brief   execute a statement */
  static bool ExecuteWithNoTempFiles
    ( const char      * szDB,
      const char      * szUser,
      const char      * szPassword,
      const char      * szCommand,
      DBMSrvMsg_Error & oMsgList );

  /*! @brief  execute a request with statement
      @param  szCommand [IN]  - null terminated statement
      @param  oMsgList  [OUT] - filled in case of errors
      @return true if successfull, in case of false see oMsgList
   */
  bool Request
    ( const char      * szCommand,
      DBMSrvMsg_Error & oMsgList,
      bool              useImplicitReconnectFlag=true);

  /*! @brief   execute a request to the event task
      @param  oMsgList  [OUT] - filled in case of errors
      @return true if successfull, in case of false see oMsgList
   */
  bool EventRequest
    ( DBMSrvMsg_Error & oMsgList );

  /*! @brief  receive a answer after request
      @param  oResultType [OUT ] - the type of the kernel answer
      @param  oMsgList    [OUT] - filled in case of errors
      @return true if successfull, in case of false see oMsgList
   */
  bool Receive
    ( ResultType & oResultType, 
      DBMSrvMsg_Error & oMsgList );
  
  /*! @brief  receive a answer after request from the eventtask
      @param  pEventBuffer [IN] - pointer to buffer for the eventdescription
      qparam  nBufferSize  [IN] - size of buffer
      @param  oMsgList    [OUT] - filled in case of errors
      @return true if successfull, in case of false see oMsgList
   */
  bool EventReceive
    ( void * const      pEventBuffer, 
      size_t            nBufferSize,
      DBMSrvMsg_Error & oMsgList );

  /*! @brief  check whether a answer is available or not
      @return true if answer is available
   */
  bool ReplyAvailable
    (  );

  /*! @brief   set cancel flag for kernel task */
  void Cancel
    (  );

  /*! @brief   release a connection 
      @param  oMsgList  [OUT] - filled in case of errors
      \param  keepConnectInfo [IN] true if connect info should not be reseted
      @return true if successfull, in case of false see oMsgList
  */
  bool Release
    ( DBMSrvMsg_Error & oMsgList,
      bool              keepConnectInfo=false,
      bool              sendReleaseCmdOverConnection=true);

  /*! @brief  Returns the kernel message
      @return A Msg_List object with the message from the database kernel.
   */
  const Msg_List KernelMessage
    ( ) const;

  /*! @brief get connect flag
      @return true if connected
   */
  bool isConnected( ) const
  {
      return m_Connected;//(m_TaskType != taskUnknown);
  }

  /*! @brief get record object */
  DBMSrvKnl_SQLRecord & GetRecordObject
    ( );

  /*! @brief  get the database name of the connection
      @return A const pointer to a null terminated string. The
              string will be empty if there is no cionnection to
              a database.
   */
  const char * GetDatabase() const { return this->szDatabase.asCharp();}

  /*! @brief  get the request pending state of the session
      @return true if there is a reuest pending otherwise false.
   */
  bool IsRequestPending() const { return this->bRequestPending;}

  /*! @brief checks for sysdba property 
      @param szUser [IN] check this user or currrent user if NULL
      @return true if user is the SYSDBA
   */
  bool isSYSDBA(const char * szUser = NULL);

  /*! @brief gets the sysdba 
      @return true if user is the SYSDBA
   */
  const char * getSYSDBA() const {return this->szSYSDBA.asCharp();}

  /*! \brief set the flag allowing one implicit reconnection */
  void allowReconnectAtNextExecute(SubsessionType subsessionType);

  /*! \brief check the flag for implicit reconnections
      \param info [IN] info message explaining the reason for the reconnect */
  bool hasInterruptedSubsession(
      SubsessionType   subsessionType,
      DBMSrvMsg_Info & info) const;

  /*! \brief unset the flag allowing one implicit reconnection, in other words
      disallow implicit reconnections */
  void disallowReconnectAtNextExecute();

  /*! \brief create a copy of the current session and connect
      \param duplicate [OUT] session object that holds the duplicated connection
      \param error     [IN/OUT] error object (if needed, new messages are appended)
      \return true if successful, false otherwise */
  bool duplicateSessionAndConnect(
            DBMSrvKnl_Session & duplicate,
            DBMSrvMsg_Error   & error) const;

private:

  /*! @brief   non public version of the execute */
  bool InternExecute
    ( const char      * szCommand,
      ResultType      & oResultType,
      DBMSrvMsg_Error & oMsgList,
      bool              useImplicitReconnectFlag=true);

  /*! \brief executed a close command not issued explicitly by the user */
  bool closeResultSet(
      ResultType & resultType);

  /*! @brief parameter count of answer */
  SAPDB_Int4 ParamCount
    ();

  /*! @brief parameter info of answer */
  tsp1_param_info * ParamInfo
    ( );

  /*! @brief size of one record */
  SAPDB_Int4 RecordSize
    ();

  /*! @brief count of records in result */
  SAPDB_Int4 RecordCount
    ();

  /*! @brief name of a field */
  Tools_DynamicUTF8String FieldName
    ( const SAPDB_Int4     nField );

  /*! @brief get a field value */
  bool GetValue
    ( const SAPDB_Int4     nField,
      DBMSrvKnl_SQLField    & oField );

  /*! @brief   connect to  a task */
  bool Connect
    (TaskType          oTaskType,
     const char      * szDB,
     DBMSrvMsg_Error & oMsgList);

  /*! @brief   connect connect user to session */
  bool ConnectUser
    ( const char *      szUser,
      const char *      szPassword,
      bool              bNoTempFile,
      DBMSrvMsg_Error & oMsgList);

  /*! @brief   reset all members of the object
      \param keepConnectInfo [IN] true if connect info should not be reseted*/
  void Reset(bool keepConnectInfo=false);

  /*! @brief   fetch record*/
  bool FetchRecordBlock
    ( bool            & bEnd,
      bool              bFirst,
      DBMSrvMsg_Error & oMsgList);

  /*! @brief   execute a statement */
  static bool Execute
    ( const char      * szDB,
      const char      * szUser,
      const char      * szPassword,
      const char      * szCommand,
      DBMSrvMsg_Error & oError,
      bool              bWithNoTempFiles);

  /*! @brief   send request to kernel */
  bool Request
    ( SAPDB_Int4        nRequestLength,
      bool            & connectionBroken,
      DBMSrvMsg_Error & oMsgList );

  /*! @brief   receive answer from kernel */
  tsp1_packet * Receive
    ( DBMSrvMsg_Error & oMsgList );

  /*! @brief   cancel a request */
  bool BreakSession
    (  );

  void ReadErrorText
    ( PIn_ReplySegment & oSegment,
      DBMSrvMsg_Error  & oMsgList );

  bool AllowedInDbFull
    ( const char * szCommand );

  bool tryImplicitReconnect(
    DBMSrvMsg_Error & error );

  /*! @brief   kernel session handle */
  SAPDB_Int4             nSessionHandle;
  bool                   m_SessionHandleIsValid; //!< true if nSessionHandle represents a RTE session handle created with sqlaconnect() and not yet closed with sqlarelease()
  /*! @brief   size of paket */
  SAPDB_Int4             nPacketSize;
  /*! @brief   request packet */
  tsp1_packet         *  pPacket;
  /*! @brief   receive packet */
  tsp1_packet         *  pReceivePacket;
  /*! @brief   unicode flag */
  SAPDB_Int4             nUnicode;
  /*! @brief   username     */
  tsp00_KnlIdentifierc   szUser;
  /*! @brief   password     */
  tsp00_Namec            szPwd;
  /*! @brief   SYSDBA of database */
  tsp00_KnlIdentifierc   szSYSDBA;
  /*! @brief   database name */
  tsp00_DbNamec          szDatabase;
  /*! @brief   connect flag */
  bool                   m_Connected;
  /*! \brief type of task */
  TaskType               m_TaskType;
  /*! @brief   auto commit flag */
  bool                   bAutoCommit;
  /*! @brief user type information */
  UserType               oUserType;
  /*! @brief   no temporary files flag */
  bool                    bNoTempFiles;
  /*! @brief   flag if connected in admin */
  bool                    bAdminConnect;
  /*! @brief   command allowed in db full */
  bool                    bAllowedInDbFull;
  /*! @brief data base state object */
  DBMSrvKnl_State        oState;
  /*! @brief record object */
  DBMSrvKnl_SQLRecord       oRecord;
  /*! @brief   ist there a pending reqeust */
  bool                    bRequestPending;
  /*! @brief   is there a open result */
  bool                    bIsResultOpen;

  SubsessionType m_ImplicitReconnectType;
  bool           m_ImplicitReconnectIsAllowed[DBMSrvKnl_Session::noSubsession]; //!< if action is no fetch or backup_replace or etc. an implicit reconnect can be allowed for some functions (db_execute... + backup/recover_start)
  bool           m_ImplicitReconnectEndedSubsession[DBMSrvKnl_Session::noSubsession]; //!< allows tracking of implict reconnects
  DBMSrvMsg_Info m_ReconnectMessage[DBMSrvKnl_Session::noSubsession];

  bool implicitReconnectFlagIsSet();
 
  /*! @brief returns a text for a task type (used in error message) */
  const char* getTextForTasktype(TaskType taskType);
  
  static const char* const m_TextTasktypeUnknown;
  static const char* const m_TextTasktypeUser;
  static const char* const m_TextTasktypeEvent;

  friend class DBMSrvKnl_SQLRecord;
}; // end class DBMSrvKnl_Session

#endif /* _DBMSrvKnl_Session_HPP_ */
