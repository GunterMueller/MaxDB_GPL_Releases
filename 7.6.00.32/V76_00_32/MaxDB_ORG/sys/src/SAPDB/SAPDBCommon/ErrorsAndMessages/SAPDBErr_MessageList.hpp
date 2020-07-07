/*!***************************************************************************

  module      : SAPDBErr_MessageList.hpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: message list handling
  description : Creating, queueing and managing message lists

                C++ exception handling required!


  last changed: 2000-06-16  11:20
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


#ifndef SAPDBERR_MESSAGELIST_HPP
#define SAPDBERR_MESSAGELIST_HPP

#include "Messages/Msg_List.hpp"

// This define is found in Msg_List.hpp and enforces to replace SAPDBErr_MessageList implementation by Msg_List
#if 0 /* #ifndef REPLACE_SAPDBERR_MESSAGE_LIST */

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class Msg_List;

/*!---------------------------------------------------------------------------
  Class:        SAPDBErr_MessageList
  Description:  Creating, queueing and managing an messages.

                The SAPDBErr_MessageList class provides a functionality to store 
                and queue messagesd like success, warning and error messages. 
                In most situations SAPDBErr_MessageList is used to store the error message
                for the last operation that failed. The implementation allows you to build up
                a list of messages which is as long as you like.

                SAPDBErr_MessageList assists you in conserving memory space
                by allowing two objects sharing the same values buffer spaces.  
*/
class SAPDBErr_MessageList 
{
public:
  /*!---------------------------------------------------------------------------
    Declaration:  Public types
    Description:  Public types used by public member functions.
  */
  typedef SAPDB_UInt4  MessageID;

  typedef enum   { Error = 1,
                   Info,
                   Warning } MessageType;

  typedef RTE_ISystem::DateTime      MessageDateTime;
    
  /*! EndDeclaration:  Public types */

  
  /*!---------------------------------------------------------------------------
     Function:     SAPDBErr_MessageList
     Description:  Create a new message list object.


     Arguments:    Component             [in]            - The name of the calling component e.g. (RTE, LOGGING ...)
                   FileName              [in]            - The name of the calling source file.
                   LineNumber            [in]            - The line number of the calling source file.
                   Type                  [in]            - message information type like error, warning and info
                   ID                    [in]            - An unequivocal identifier corresponding to the current 
                                                           message.
                   Message               [in]            - The message string. Only '%s' argument specifications are 
                                                           allowed!
                                                             Example: "My message looks like this %s, %s rc = %s".
                   NumOfArgs             [in]            - Number of following arguments. Used for format string checks.
                   Arg0                  [in (optional)] - Argument 0. <br>
                   ...                                              
                   Arg9                  [in (optional)] - Argument 9.
     Return value: none
  */

  SAPDBErr_MessageList( SAPDB_Char  const * const  Component,
                        SAPDB_Char  const * const  FileName,
                        SAPDB_UInt4 const          LineNumber,
                        MessageType const          Type,
                        MessageID   const          ID,
                        SAPDB_Char  const * const  Message,
                        SAPDB_UInt4 const          NumOfArgs,
                        SAPDB_Char  const * const  Arg0 = NULL,
                        SAPDB_Char  const * const  Arg1 = NULL,
                        SAPDB_Char  const * const  Arg2 = NULL,
                        SAPDB_Char  const * const  Arg3 = NULL,
                        SAPDB_Char  const * const  Arg4 = NULL,
                        SAPDB_Char  const * const  Arg5 = NULL,
                        SAPDB_Char  const * const  Arg6 = NULL,
                        SAPDB_Char  const * const  Arg7 = NULL,
                        SAPDB_Char  const * const  Arg8 = NULL,
                        SAPDB_Char  const * const  Arg9 = NULL )
                      {
                        FillMessageList( Component, FileName, LineNumber, 
                                         Type, ID, 0, Message, NumOfArgs, 
                                         Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, 
                                         Arg6, Arg7, Arg8, Arg9 );
                      };

  SAPDBErr_MessageList( SAPDB_Char  const * const  Component,
                        SAPDB_Char  const * const  FileName,
                        SAPDB_UInt4 const          LineNumber,
                        SAPDB_UInt4 const          SystemRC,
                        MessageType const          Type,
                        MessageID   const          ID,
                        SAPDB_Char  const * const  Message,
                        SAPDB_UInt4 const          NumOfArgs,
                        SAPDB_Char  const * const  Arg0 = NULL,
                        SAPDB_Char  const * const  Arg1 = NULL,
                        SAPDB_Char  const * const  Arg2 = NULL,
                        SAPDB_Char  const * const  Arg3 = NULL,
                        SAPDB_Char  const * const  Arg4 = NULL,
                        SAPDB_Char  const * const  Arg5 = NULL,
                        SAPDB_Char  const * const  Arg6 = NULL,
                        SAPDB_Char  const * const  Arg7 = NULL,
                        SAPDB_Char  const * const  Arg8 = NULL,
                        SAPDB_Char  const * const  Arg9 = NULL )
                      {
                        FillMessageList( Component, FileName, LineNumber, 
                                         Type, ID, SystemRC, Message, NumOfArgs, 
                                         Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, 
                                         Arg6, Arg7, Arg8, Arg9 );
                      };

  inline   SAPDBErr_MessageList( const SAPDBErr_MessageList &MessageList );
           SAPDBErr_MessageList( const Msg_List &msgList);
  inline   SAPDBErr_MessageList();

  virtual ~SAPDBErr_MessageList();

  /*!---------------------------------------------------------------------------
     Function:     ClearMessageList
     Description:  Empty an message list object.
     Arguments:    none
     Return value: none
  */
  void                ClearMessageList();


  /*!---------------------------------------------------------------------------
     Function:     IsEmpty
     Description:  Returns true if the message list is empty.
     Arguments:    none
     Return value: none
  */
  inline bool const   IsEmpty() const;


  /*!---------------------------------------------------------------------------
     Function:     operator =
     Description:  This operator assigns a value to the SAPDBErr_MessageList object.
  */
  SAPDBErr_MessageList const& operator = ( const SAPDBErr_MessageList& MessageList );
  SAPDBErr_MessageList const& operator = ( const Msg_List& MessageList );


  /*!---------------------------------------------------------------------------
     Function:     operator +
     Description:  This operator appends a message list to another.
       
                   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                   You should always append an newer message to an older one to
                   get a correct message ordering.

                   Examples:
                      MsgList  = OldMsgLst + NewMsgLst;
                      MsgList  = MsgList   + SAPDBErr_MessageList ("MyComponent", ....);
                      MsgList += SAPDBErr_MessageList ("MyComponent", ....);

                   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  */
  SAPDBErr_MessageList operator+ ( const SAPDBErr_MessageList& MessageList ) const;
  SAPDBErr_MessageList operator+ ( const Msg_List& MessageList ) const;


  /*!---------------------------------------------------------------------------
     Function:     AppendNewMessage
     Description:  This methode appends a new message list to the current message list.
       
                   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                   You should always append an newer message to an older one to
                   get a correct message ordering.

                   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  */
  void   AppendNewMessage ( SAPDBErr_MessageList const &MessageList );
  void   AppendNewMessage ( Msg_List const &MessageList );

  /*!---------------------------------------------------------------------------
     Function:     NumOfMessages
     Description:  Returns the number of messages.
     Arguments:    none
     Return value: Number of messages.
                   
  */
  SAPDB_UInt4  NumOfMessages() const { return m_NumOfMessages; }

  /*!---------------------------------------------------------------------------
     Function:     NextMessage
     Description:  Returns a pointer to next message object of the message list.
     Arguments:    none
     Return value: Pointer to the next message list object. 
                   Returns NULL if no object exists.
  */
  inline SAPDBErr_MessageList * const   NextMessage() const;



  /*!---------------------------------------------------------------------------
     Function:     ID
     Description:  Returns the message identifier.
     Arguments:    none
     Return value: Message identifier.
  */
  inline const SAPDB_UInt4    ID() const;

  /*!---------------------------------------------------------------------------
     Function:     SystemRC
     Description:  Returns the system return code.
     Arguments:    none
     Return value: System return code.
  */
  inline SAPDB_UInt4 const SystemRC() const;


  /*!---------------------------------------------------------------------------
     Function:     Type
     Description:  Returns the message type (warning, error, info).
     Arguments:    Long [in (optional)] - if 'true' it returns the 
                               message type string in long form.
     Return value: Message type (MessageType).                        <br>
                   Message type in string form:                       <br>
                   - Long  =  true  -> "WARNING", "ERROR", "INFO"     <br>
                   - Long  =  false -> "WRN", "ERR", ""               <br>
     prototypes: 2
  */
  inline MessageType const  Type() const;
  SAPDB_Char const * const  Type( bool const  Long ) const;

  /*!---------------------------------------------------------------------------
     Function:     Component
     Description:  Returns the component name.
     Arguments:    none
     Return value: component.
  */
  inline SAPDB_Char const * const  Component() const;

  /*!---------------------------------------------------------------------------
     Function:     FileName
     Description:  Returns the file name of module where the message was created.
     Arguments:    none
     Return value: Module file name.
  */
  inline SAPDB_Char const * const  FileName() const;

  /*!---------------------------------------------------------------------------
     Function:     LineNumber
     Description:  Returns the line number of module where the message was created.
     Arguments:    none
     Return value: Line number.
  */
  inline SAPDB_UInt4 const LineNumber() const;

  /*!---------------------------------------------------------------------------
     Function:     Message
     Description:  Returns the message string.
     Arguments:    none
     Return value: Message string.
  */
  inline SAPDB_Char const * const  Message() const;

  /*!---------------------------------------------------------------------------
     Function:     DateTime
     Description:  Returns the date and time the message was created.
     Arguments:    Long [in (optional)] - if 'true' it returns the 
                               date and time string in long form.
     Return value: Message date (MessageDateTime).                    <br>
                   Message date  in string form:                      <br> 
                   - Long  =  true  -> "YYYY-MM-DD HH:MM:SS.sss"      <br>
                   - Long  =  false -> "YYYY-MM-DD HH:MM:SS"          <br>
     prototypes: 2
  */
  inline SAPDB_Char const * const  DateTime( bool const  Long ) const;
  inline MessageDateTime const&    DateTime() const;

private:
  SAPDB_UInt4           BuildMessageString ( SAPDB_Char  const * const  Message,
                                             SAPDB_Char  const * const  Args[],
                                             SAPDB_UInt4 const          NumOfArgs,
                                             SAPDB_Char  * const        MessageString );
                       
  void                  CopyMessageList    ( SAPDBErr_MessageList const &MessageList );
  void                  CopyMessageList    ( Msg_List const &MessageList );
  static void ConvertSingleMsg_ListToSAPDBErr_MessageList(SAPDBErr_MessageList &target, Msg_List const &source);
  SAPDBErr_MessageList* SplitObjectList    ();


  void                  FillMessageList    ( SAPDB_Char  const * const  Component,
                                             SAPDB_Char  const * const  FileName,
                                             SAPDB_UInt4 const          LineNumber,
                                             MessageType const          Type,
                                             MessageID   const          ID,
                                             SAPDB_UInt4 const          SystemRC,
                                             SAPDB_Char  const * const  Message,
                                             SAPDB_UInt4 const          NumOfArgs,
                                             SAPDB_Char  const * const  Arg0,
                                             SAPDB_Char  const * const  Arg1,
                                             SAPDB_Char  const * const  Arg2,
                                             SAPDB_Char  const * const  Arg3,
                                             SAPDB_Char  const * const  Arg4,
                                             SAPDB_Char  const * const  Arg5,
                                             SAPDB_Char  const * const  Arg6,
                                             SAPDB_Char  const * const  Arg7,
                                             SAPDB_Char  const * const  Arg8,
                                             SAPDB_Char  const * const  Arg9 );
                        
  // as above but with explicit message time given...
  void                  FillMessageList    ( SAPDB_Char  const * const  Component,
                                             SAPDB_Char  const * const  FileName,
                                             SAPDB_UInt4 const          LineNumber,
                                             MessageType const          Type,
                                             MessageID   const          ID,
                                             MessageDateTime const     &DateTime,
                                             SAPDB_UInt4 const          SystemRC,
                                             SAPDB_Char  const * const  Message,
                                             SAPDB_UInt4 const          NumOfArgs,
                                             SAPDB_Char  const * const  Arg0,
                                             SAPDB_Char  const * const  Arg1,
                                             SAPDB_Char  const * const  Arg2,
                                             SAPDB_Char  const * const  Arg3,
                                             SAPDB_Char  const * const  Arg4,
                                             SAPDB_Char  const * const  Arg5,
                                             SAPDB_Char  const * const  Arg6,
                                             SAPDB_Char  const * const  Arg7,
                                             SAPDB_Char  const * const  Arg8,
                                             SAPDB_Char  const * const  Arg9 );
                        
  // --- counter handling
  //
  inline void         AddDataRef();
  inline void         SubtractDataRef();
  inline SAPDB_UInt4  GetDataRef();
  inline void         AddObjectRef();
  inline void         SubtractObjectRef();
  inline SAPDB_UInt4  GetObjectRef();

private:

  struct MessageData
  {
    SAPDB_UInt4                  Version;
    SAPDB_UInt4                  DataLen;
    SAPDB_UInt4                  DataRefCnt;
                                 
    MessageDateTime              DateTime;
                                 
    SAPDB_UInt4                  SystemRC;
    SAPDB_UInt4                  ID;
    SAPDB_UInt4                  Type;
    SAPDB_UInt4                  LineNumber;

    // ---- string buffer offsets
    SAPDB_UInt2                  DateTimeStringOffset;
    SAPDB_UInt2                  BigDateTimeStringOffset;
    SAPDB_UInt2                  ComponentStringOffset;
    SAPDB_UInt2                  FileNameStringOffset;
    SAPDB_UInt2                  MessageStringOffset;
    SAPDB_UInt2                  Filler;
                                 
    SAPDB_Char                   Strings[1];    // - The string buffer goes here!
  };

private:
  SAPDB_UInt4                    m_NumOfMessages;
  SAPDB_UInt4                    m_ObjectRefCnt;
  MessageData*                   m_pMessageData;  // - Message data ( NULL means 'no message' )
  SAPDBErr_MessageList*          m_pNextMessage;

  static const MessageDateTime   EmptyDateTimeValue;

};
/*! EndClass: SAPDBErr_MessageList */
    

/*---------------------------------------------------------------------------*/

inline SAPDBErr_MessageList::MessageType const SAPDBErr_MessageList::Type() const                  
{ 

  return ( m_pMessageData != NULL ) ? (MessageType)m_pMessageData->Type : Info; 
}       

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt4 const SAPDBErr_MessageList::SystemRC() const
{ 
  return ( m_pMessageData != NULL ) ? m_pMessageData->SystemRC : 0;
}

/*---------------------------------------------------------------------------*/

inline const SAPDB_UInt4  SAPDBErr_MessageList::ID() const
{ 
  return ( m_pMessageData != NULL ) ? m_pMessageData->ID : 0;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Char const * const SAPDBErr_MessageList::Component() const   
{ 
  return ( m_pMessageData != NULL ) ? (m_pMessageData->Strings + m_pMessageData->ComponentStringOffset) : "";
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Char const * const SAPDBErr_MessageList::FileName() const   
{ 
  return ( m_pMessageData != NULL ) ? (m_pMessageData->Strings + m_pMessageData->FileNameStringOffset) : "";
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt4 const SAPDBErr_MessageList::LineNumber() const   
{ 
  return ( m_pMessageData != NULL ) ? m_pMessageData->LineNumber : 0;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Char const * const SAPDBErr_MessageList::Message() const   
{ 
  return ( m_pMessageData != NULL ) ? (m_pMessageData->Strings + m_pMessageData->MessageStringOffset) : "";
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Char const * const SAPDBErr_MessageList::DateTime( bool const  Long ) const
{ 
  const SAPDB_Char* DateTime;

  if ( m_pMessageData != NULL )
  {
    if ( Long )
      DateTime = m_pMessageData->Strings + m_pMessageData->BigDateTimeStringOffset;
    else
      DateTime = m_pMessageData->Strings + m_pMessageData->DateTimeStringOffset;
  }
  else
    DateTime = "";

  return DateTime;
}

/*---------------------------------------------------------------------------*/

inline SAPDBErr_MessageList::MessageDateTime const& SAPDBErr_MessageList::DateTime() const                  
{ 
  return ( m_pMessageData != NULL ) ? m_pMessageData->DateTime : EmptyDateTimeValue;
}

/*---------------------------------------------------------------------------*/

inline SAPDBErr_MessageList * const SAPDBErr_MessageList::NextMessage() const
{
  return m_pNextMessage;
}

/*---------------------------------------------------------------------------*/

inline bool const SAPDBErr_MessageList::IsEmpty() const
{ 
  return m_pMessageData == NULL; 
}

/*---------------------------------------------------------------------------*/

inline void SAPDBErr_MessageList::AddDataRef()
{
  if ( m_pMessageData != NULL )
    m_pMessageData->DataRefCnt++;

  return;
}

/*---------------------------------------------------------------------------*/

inline void SAPDBErr_MessageList::SubtractDataRef()
{
  if (( m_pMessageData != NULL ) && ( m_pMessageData->DataRefCnt > 0 ))
    m_pMessageData->DataRefCnt--;

  return;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt4 SAPDBErr_MessageList::GetDataRef()
{
  return ( m_pMessageData != NULL ) ? m_pMessageData->DataRefCnt : 0;
}

/*---------------------------------------------------------------------------*/

inline void SAPDBErr_MessageList::AddObjectRef()
{
  m_ObjectRefCnt++;

  return;
}

/*---------------------------------------------------------------------------*/

inline void SAPDBErr_MessageList::SubtractObjectRef()
{
  if ( m_ObjectRefCnt > 0 )
    m_ObjectRefCnt--;

  return;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt4 SAPDBErr_MessageList::GetObjectRef()
{
  return m_ObjectRefCnt;
}

/*---------------------------------------------------------------------------*/

inline SAPDBErr_MessageList::SAPDBErr_MessageList()
                            : m_ObjectRefCnt(0),
                              m_NumOfMessages(0),
                              m_pMessageData(NULL),
                              m_pNextMessage(NULL) {;}

/*---------------------------------------------------------------------------*/

inline SAPDBErr_MessageList::SAPDBErr_MessageList( SAPDBErr_MessageList const &MessageList )
                            : m_ObjectRefCnt(0),
                              m_NumOfMessages(0),
                              m_pMessageData(NULL),
                              m_pNextMessage(NULL)
{ 
  CopyMessageList( MessageList );
}



/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


/*===========================================================================*
 *  EXAMPES                                                                  *
 *===========================================================================*/

//
// New SAPDBErr_MessageList definition discussion
//
#endif /* REPLACE_SAPDBERR_MESSAGE_LIST */

#endif  /* SAPDBERR_MESSAGELIST_HPP */
