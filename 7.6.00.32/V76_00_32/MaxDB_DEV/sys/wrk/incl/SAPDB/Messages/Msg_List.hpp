/*!
  @file           Msg_List.hpp
  @author         JoergM
  @ingroup        RunTime
  @brief          Creating, queueing and managing message lists. Details in Msg_List class description below.

  Derived from SAPDBErr_MessageList implementation.
  Original author of SAPDBErr_MessageList is RaymondR

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef MSG_LIST_HPP
#define MSG_LIST_HPP

//! The definition of MSG_LIST_GENERATOR_SPECIFIES_ARGUMENT_COUNT indicates message monitor too buildmsg generates argument count specification used in ctor and AddDetail */
// #define MSG_LIST_GENERATOR_SPECIFIES_ARGUMENT_COUNT

//! The definition of MSG_LIST_ARGUMENT_COUNT_MUST_MATCH forces necessary exact match of argument count specification in ctor and AddDetail */
#define MSG_LIST_ARGUMENT_COUNT_MUST_MATCH

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_ISystem.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Messages/Msg_IArg.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(Msg_List)
//! Support for old SAPDBErr_MessageList() constructors still supported
#define SAPDBERR_MESSAGELIST_STILL_SUPPORTED
#endif

//! Maximum arguments per message if individual arguments are specified
/*!
 If you thing about to change the maximum argument count from 10...
 
 First decide if you really have to do that ;-)....

 This is what you have to do if you want 11 argument

 \li modify this definition from 10 to 11
 \li modify every MSG_CTOR_*_ARGS macros by adding Arg10
 \li modify every MSG_CTOR_SETUP_*_VALIDARGS macros by adding Arg10
 \li add MSG_CTOR_11_ARGS 
 \li add MSG_CTOR_SETUP_11_VALIDARGS 
 \li add class Msg_Has11Args
 \li modify comment section in MSG_CTOR macro
 \li add MSG_CTOR(Msg_Has11Args,MSG_CTOR_11_ARGS,MSG_CTOR_SETUP_11_VALIDARGS)
 \li modify comment section in MSG_ADD_DETAIL macro
 \li add MSG_ADD_DETAIL(Msg_Has11Args,MSG_CTOR_11_ARGS,MSG_CTOR_SETUP_11_VALIDARGS)

 For even higher values you have to repeats these steps...

 If you go above 20 you have to change 'MSG_LIST_ARG_MAX' too.
 */
#define MSG_LIST_SINGLE_ARGS_MAX                        10

//! Maximum argument list length per message
#define MSG_LIST_ARG_MAX                                20

//! Version of Message list content format
#define MSG_LIST_VERSION 3

//! Short form (excluding milliseconds) of time stamp format
#define MSG_LIST_SHORT_TIME_HEADER_FORMAT_STR           "%04d-%02d-%02d %02d:%02d:%02d"
//! Length of short form time stamp
#define MSG_LIST_SHORT_TIME_HEADER_FORMAT_STR_LEN       19

//! Long form (including milliseconds) of time stamp format
#define MSG_LIST_LONG_TIME_HEADER_FORMAT_STR            "%04d-%02d-%02d %02d:%02d:%02d.%03d"
//! Length of long form time stamp
#define MSG_LIST_LONG_TIME_HEADER_FORMAT_STR_LEN        23

//! Message list tag
#define MSG_LIST_MESSAGE_LIST_TAG                       "MSGL"
//! Message tag
#define MSG_LIST_MESSAGE_TAG                            "MSG"
//! Message list contained in message list tag
#define MSG_LIST_CONTAINED_MESSAGE_LIST_TAG             "MSGC"
//! Message list contained in message list tag
#define MSG_LIST_CONTAINED_DETAILS_LIST_TAG             "MSGD"
//! Message list contained arguments tag
#define MSG_LIST_ARGUMENTS_TAG                          "MSG_ARGS"
//! Message type tag
#define MSG_LIST_MESSAGE_TYPE_TAG                       "_TYPE"
//! Message output sequence number tag
#define MSG_LIST_MESSAGE_NUMBER_TAG                     "_NO"
//! Componant name tag
#define MSG_LIST_COMPONENT_TAG                          "_COMP"
//! Message id tag
#define MSG_LIST_ID_TAG                                 "_ID"
//! Message short text tag
#define MSG_LIST_TEXT_TAG                               "_TEXT"
//! Module file name tag
#define MSG_LIST_FILE_NAME_TAG                          "_FILE"
//! Module line number tag
#define MSG_LIST_LINE_NUMBER_TAG                        "_LINE"
//! Time stamp tag
#define MSG_LIST_TIMESTAMP_TAG                          "_TIME"
//! Calling process id tag
#define MSG_LIST_PROCESS_ID_TAG                         "_PROCESS"
//! Calling thread id tag
#define MSG_LIST_THREAD_ID_TAG                          "_THREAD"
//! Calling task id tag
#define MSG_LIST_TASK_ID_TAG                            "_TASK"
//! Calling ukt id tag
#define MSG_LIST_UKT_ID_TAG                             "_UKT"
//! Message detail count tag
#define MSG_LIST_DETAILS_TAG                            "_DETAILS"
//! Legacy message label tag
#define MSG_LIST_LABEL_TAG                              "_LABEL"
//! Indentation string used
#define MSG_LIST_INDENTATION                            " "

//! Special component that is ignored by 'TraceMessageCopy()'...
#define MSG_MSGOUTPUT_COMPONENT                         "MsgOutput"

//! Macro to Setup Msg_List contructor standard parameters defined by SDBMSG_... macros
#define MSG_CTOR_STANDARD_PARAMETER_SET \
      SAPDB_UInt4 const          ID MSG_COMMA \
      SAPDB_Char  const * const  Component MSG_COMMA \
      SAPDB_Char  const * const  FileName MSG_COMMA \
      SAPDB_UInt4 const          LineNumber MSG_COMMA \
      SAPDB_Char  const * const  Message

//! Macro to Setup Msg_List contructor initalization list
#define MSG_CTOR_INIT_LIST \
    m_pRegistrySlot(0) MSG_COMMA \
    m_NumOfMessages(0) MSG_COMMA \
    m_NumOfSubMessages(0) MSG_COMMA \
    m_NumOfDetails(0) MSG_COMMA \
    m_ObjectRefCnt(0) MSG_COMMA \
    m_pMessageData(0) MSG_COMMA \
    m_pNextMessage(0) MSG_COMMA \
    m_OutputSequenceNumber(0) MSG_COMMA \
    m_OutputDateTime(EmptyDateTimeValue) MSG_COMMA \
    m_OutputProcessId(0) MSG_COMMA \
    m_OutputThreadId(0) MSG_COMMA \
    m_OutputTaskId(0) MSG_COMMA \
    m_OutputSchedulerId(0) MSG_COMMA \
    m_DumpTruncated(0)

//! Macro to Setup any number of optional Msg_IArg parameters
#define MSG_CTOR_ANY_ARGS \
    Msg_IArg const * const Arg0 = 0 MSG_COMMA \
    Msg_IArg const * const Arg1 = 0 MSG_COMMA \
    Msg_IArg const * const Arg2 = 0 MSG_COMMA \
    Msg_IArg const * const Arg3 = 0 MSG_COMMA \
    Msg_IArg const * const Arg4 = 0 MSG_COMMA \
    Msg_IArg const * const Arg5 = 0 MSG_COMMA \
    Msg_IArg const * const Arg6 = 0 MSG_COMMA \
    Msg_IArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IArg const * const Arg9 = 0

//! Macro to copy any number of optional Msg_IArg parameters
#define MSG_CTOR_SETUP_ANY_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3->GetArg(); ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4->GetArg(); ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5->GetArg(); ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6->GetArg(); ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7->GetArg(); ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8->GetArg(); ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9->GetArg(); ValidArgCount++; }

//! Macro to Setup any number of up to 10 Msg_IOptArg parameters
#define MSG_CTOR_0_ARGS \
    Msg_IOptArg const * const Arg0 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg1 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg2 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg3 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg4 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg5 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg6 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 0 used Msg_IArg and 10 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_0_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0; ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1; ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2; ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3; ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4; ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5; ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6; ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7; ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 9 Msg_IOptArg parameters
#define MSG_CTOR_1_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IOptArg const * const Arg1 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg2 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg3 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg4 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg5 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg6 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 1 used Msg_IArg and 9 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_1_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1; ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2; ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3; ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4; ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5; ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6; ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7; ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 8 Msg_IOptArg parameters
#define MSG_CTOR_2_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IOptArg const * const Arg2 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg3 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg4 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg5 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg6 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 2 used Msg_IArg and 8 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_2_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2; ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3; ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4; ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5; ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6; ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7; ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 7 Msg_IOptArg parameters
#define MSG_CTOR_3_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IArg const * const Arg2 MSG_COMMA \
    Msg_IOptArg const * const Arg3 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg4 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg5 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg6 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 3 used Msg_IArg and 7 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_3_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3; ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4; ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5; ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6; ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7; ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 6 Msg_IOptArg parameters
#define MSG_CTOR_4_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IArg const * const Arg2 MSG_COMMA \
    Msg_IArg const * const Arg3 MSG_COMMA \
    Msg_IOptArg const * const Arg4 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg5 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg6 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 4 used Msg_IArg and 6 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_4_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3->GetArg(); ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4; ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5; ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6; ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7; ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 5 Msg_IOptArg parameters
#define MSG_CTOR_5_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IArg const * const Arg2 MSG_COMMA \
    Msg_IArg const * const Arg3 MSG_COMMA \
    Msg_IArg const * const Arg4 MSG_COMMA \
    Msg_IOptArg const * const Arg5 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg6 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 5 used Msg_IArg and 5 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_5_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3->GetArg(); ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4->GetArg(); ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5; ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6; ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7; ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 4 Msg_IOptArg parameters
#define MSG_CTOR_6_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IArg const * const Arg2 MSG_COMMA \
    Msg_IArg const * const Arg3 MSG_COMMA \
    Msg_IArg const * const Arg4 MSG_COMMA \
    Msg_IArg const * const Arg5 MSG_COMMA \
    Msg_IOptArg const * const Arg6 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 6 used Msg_IArg and 4 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_6_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3->GetArg(); ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4->GetArg(); ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5->GetArg(); ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6; ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7; ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 3 Msg_IOptArg parameters
#define MSG_CTOR_7_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IArg const * const Arg2 MSG_COMMA \
    Msg_IArg const * const Arg3 MSG_COMMA \
    Msg_IArg const * const Arg4 MSG_COMMA \
    Msg_IArg const * const Arg5 MSG_COMMA \
    Msg_IArg const * const Arg6 MSG_COMMA \
    Msg_IOptArg const * const Arg7 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 7 used Msg_IArg and 3 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_7_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3->GetArg(); ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4->GetArg(); ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5->GetArg(); ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6->GetArg(); ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7; ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 2 Msg_IOptArg parameters
#define MSG_CTOR_8_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IArg const * const Arg2 MSG_COMMA \
    Msg_IArg const * const Arg3 MSG_COMMA \
    Msg_IArg const * const Arg4 MSG_COMMA \
    Msg_IArg const * const Arg5 MSG_COMMA \
    Msg_IArg const * const Arg6 MSG_COMMA \
    Msg_IArg const * const Arg7 MSG_COMMA \
    Msg_IOptArg const * const Arg8 = 0 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 8 used Msg_IArg and 2 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_8_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3->GetArg(); ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4->GetArg(); ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5->GetArg(); ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6->GetArg(); ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7->GetArg(); ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8; ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up to 1 Msg_IOptArg parameters
#define MSG_CTOR_9_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IArg const * const Arg2 MSG_COMMA \
    Msg_IArg const * const Arg3 MSG_COMMA \
    Msg_IArg const * const Arg4 MSG_COMMA \
    Msg_IArg const * const Arg5 MSG_COMMA \
    Msg_IArg const * const Arg6 MSG_COMMA \
    Msg_IArg const * const Arg7 MSG_COMMA \
    Msg_IArg const * const Arg8 MSG_COMMA \
    Msg_IOptArg const * const Arg9 = 0

//! Macro to copy 9 used Msg_IArg and 1 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_9_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3->GetArg(); ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4->GetArg(); ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5->GetArg(); ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6->GetArg(); ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7->GetArg(); ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8->GetArg(); ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9; ValidArgCount++; }

//! Macro to Setup any number of up 10 Msg_IArg and no Msg_IOptArg parameters
#define MSG_CTOR_10_ARGS \
    Msg_IArg const * const Arg0 MSG_COMMA \
    Msg_IArg const * const Arg1 MSG_COMMA \
    Msg_IArg const * const Arg2 MSG_COMMA \
    Msg_IArg const * const Arg3 MSG_COMMA \
    Msg_IArg const * const Arg4 MSG_COMMA \
    Msg_IArg const * const Arg5 MSG_COMMA \
    Msg_IArg const * const Arg6 MSG_COMMA \
    Msg_IArg const * const Arg7 MSG_COMMA \
    Msg_IArg const * const Arg8 MSG_COMMA \
    Msg_IArg const * const Arg9

//! Macro to copy 10 used Msg_IArg and 0 optional Msg_IOptArg parameters
#define MSG_CTOR_SETUP_10_VALIDARGS \
    if ( Arg0 ) { ValidArgs[ValidArgCount] = Arg0->GetArg(); ValidArgCount++; } \
    if ( Arg1 ) { ValidArgs[ValidArgCount] = Arg1->GetArg(); ValidArgCount++; } \
    if ( Arg2 ) { ValidArgs[ValidArgCount] = Arg2->GetArg(); ValidArgCount++; } \
    if ( Arg3 ) { ValidArgs[ValidArgCount] = Arg3->GetArg(); ValidArgCount++; } \
    if ( Arg4 ) { ValidArgs[ValidArgCount] = Arg4->GetArg(); ValidArgCount++; } \
    if ( Arg5 ) { ValidArgs[ValidArgCount] = Arg5->GetArg(); ValidArgCount++; } \
    if ( Arg6 ) { ValidArgs[ValidArgCount] = Arg6->GetArg(); ValidArgCount++; } \
    if ( Arg7 ) { ValidArgs[ValidArgCount] = Arg7->GetArg(); ValidArgCount++; } \
    if ( Arg8 ) { ValidArgs[ValidArgCount] = Arg8->GetArg(); ValidArgCount++; } \
    if ( Arg9 ) { ValidArgs[ValidArgCount] = Arg9->GetArg(); ValidArgCount++; }

//! Macro to allow passing of macros containing comma as arguments to other macros
/*! 
 Used to sepcify an comma separated lists (see MSG_CTOR_7_ARGS for example) as value of a macro.
 Such a macro could not be used as argument to another macro (i.e. MSG_CTOR) without replacing every
 comma by another macro... If you use a \b ',' in a macro definition directly, you cannot uses this macro
 definition as argument to another macro definition! The reason is the simple text replacement parser
 of C preprocessor which leads to an illegal number of passed arguments...
 
 Using a macro to define a \b ',' is the only way to pass macros as single arguments to macros!
 */
#define MSG_COMMA ,

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Msg_RegistrySlot; 

#if defined(SAPDB_FAST)||defined(SAPDB_QUICK)||defined(SAPDB_SLOW) // just to keep this out of doxygens reach....
/*! @class Msg_HasArgs
    @brief Helper class used to select Msg_List ctor with any number or requiered or optional args */
class Msg_HasArgs { }; /*! @endclass Msg_HasArgs */
/*! @class Msg_HasAnyArgs
    @brief Helper class used to select Msg_List ctor with up to 10 requiered args and no optional args */
class Msg_HasAnyArgs : public Msg_HasArgs { }; /*! @endclass Msg_HasAnyArgs */
/*! @class Msg_Has0Args
    @brief Helper class used to select Msg_List ctor with exactly 0 requiered args and up to 10 optional args */
class Msg_Has0Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has0Args */
/*! @class Msg_Has1Args
    @brief Helper class used to select Msg_List ctor with exactly 1 requiered args and up to 9 optional args */
class Msg_Has1Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has1Args */
/*! @class Msg_Has2Args
    @brief Helper class used to select Msg_List ctor with exactly 2 requiered args and up to 8 optional args */
class Msg_Has2Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has2Args */
/*! @class Msg_Has3Args
    @brief Helper class used to select Msg_List ctor with exactly 3 requiered args and up to 7 optional args */
class Msg_Has3Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has3Args */
/*! @class Msg_Has4Args
    @brief Helper class used to select Msg_List ctor with exactly 4 requiered args and up to 6 optional args */
class Msg_Has4Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has4Args */
/*! @class Msg_Has5Args
    @brief Helper class used to select Msg_List ctor with exactly 5 requiered args and up to 5 optional args */
class Msg_Has5Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has5Args */
/*! @class Msg_Has6Args
    @brief Helper class used to select Msg_List ctor with exactly 6 requiered args and up to 4 optional args */
class Msg_Has6Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has6Args */
/*! @class Msg_Has7Args
    @brief Helper class used to select Msg_List ctor with exactly 7 requiered args and up to 3 optional args */
class Msg_Has7Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has7Args */
/*! @class Msg_Has8Args
    @brief Helper class used to select Msg_List ctor with exactly 8 requiered args and up to 2 optional args */
class Msg_Has8Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has8Args */
/*! @class Msg_Has9Args
    @brief Helper class used to select Msg_List ctor with exactly 9 requiered args and 1 optional args */
class Msg_Has9Args   : public Msg_HasArgs { };  /*! @endclass Msg_Has9Args */
/*! @class Msg_Has10Args
    @brief Helper class used to select Msg_List ctor with exactly 10 requiered args and no optional args */
class Msg_Has10Args  : public Msg_HasArgs { };  /*! @endclass Msg_Has10Args */
#endif


/*!
  @class Msg_List
  @brief Creating, queueing and managing an messages. A long and detailed description is found here:

    The Msg_List class provides a functionality to store
    and queue messages like success, warning and error messages. The type of a message is set during
    construction of the message using the enumeration values Msg_List::Error, Msg_List::Warning or
    Msg_List::Info. The implementation allows you to build up a list of messages which is as long as
    you like by added message with Overrule() or AddDetail() member functions.

    If a message list is not empty, its first message is the 'main message'. This 'main message'
    is what defines the values for 'Type, ID and Component' of the Msg_List. 

    If you think, the 'main message' must be replaced with a new 'main message' you can overrule a
    message. The overrule means you provide a message list content (with a new main message) and all
    previous messages (includind the former main message) are appended as messages.

    Each message in a message list can have details. Details are always informations that have
    been added to the current 'main message'. You cannot easely add details to other message in
    the message list, which would mean to parse the complete message list before anyhow...

    If you add a detail to a message list, the detail is appended to the 'main message' detail list.
    If you message list contains already more than one message, the details are not necessarily 
    appended at the end of the list!

    Msg_List assists you in conserving memory space by allowing two objects sharing the same values.
    This means reference counting is used for all message content and all contains messages.

    Msg_List can also be transferred using memory buffers and the member function Load() and Store().

    Since output thread, task and UKT are cost intensive to determine, they must be set explicitly
    with a call to SetCallerIdentification(). During output the output routine should use the member
    function SetOutputIdentification() to set Task,UKT,Thread and ProcessId as well as a sequence number.
    This information cannot be overwritten before a ClearMessage() is applied or the message was overruled. A copy of the message
    list does not contain that sequence number and the sequence number together with all other output
    identification is transferred by Load() and Store() member functions.

    @code

    ----------------------------------------------------------------------------------------------
    The message list public member functions of interest for the produces of message list contents
    ----------------------------------------------------------------------------------------------
    @endcode

    As a message producer, your standard problem is to decide what to do with messages that were
    produced by others. If you got a filled message list as result from a subroutine call you must
    decide what to do with it. You can pass it as result, you can overrule it with your own 
    'main message' (Overrule) and you can add a detail (AddDetail) without modifying the 'main message'.

    You can also group all message in a message list as a message list. The member MarkAsSubMessages()
    adds structure information that allows to group sub messages in a message list.

    @code
    Msg_List()
    @endcode
    
        This is the default ctors creating an empty message list used i.e. as 'empty' input
    
    @code

    Example using MSG_LIST_SINGLE_ARGS_MAX == 10

    Msg_List( SAPDB_UInt4 const          Type,   --- Msg_List::Error, Msg_List::Warning and Msg_List::Info
              SAPDB_UInt4 const          ID,
              SAPDB_Char  const * const  Component,
              SAPDB_Char  const * const  FileName,
              SAPDB_UInt4 const          LineNumber,
              SAPDB_Char  const * const  Message,
              Msg_HasArgs|Msg_HasAnyArgs|Msg_Has0Args|Msg_Has1Args|...|Msg_Has10Args *,  --- one of these....
              Msg_IArg|Msg_IOptArg const * Arg0 [= 0],
              Msg_IArg|Msg_IOptArg const * Arg1 [= 0],   --- Msg_IArg are only optional for Msg_HasAnyArgs
              Msg_IArg|Msg_IOptArg const * Arg2 [= 0],   --- otherwise they are requiered
              Msg_IArg|Msg_IOptArg const * Arg3 [= 0],
              Msg_IArg|Msg_IOptArg const * Arg4 [= 0],   --- Msg_IOptArg are always optional, but you
              Msg_IArg|Msg_IOptArg const * Arg5 [= 0],   --- can never put an Msg_IArg after specifying
              Msg_IArg|Msg_IOptArg const * Arg6 [= 0],   --- the first Msg_IOptArg...
              Msg_IArg|Msg_IOptArg const * Arg7 [= 0],
              Msg_IArg|Msg_IOptArg const * Arg8 [= 0],
              Msg_IArg|Msg_IOptArg const * Arg9 [= 0] ) support new Msg_List style
    @endcode

        This is the form the message monitor produces. 
        The macros SDBMSG_... defines ID,Component,FileName,LineNumber,Message and a pointer Msg_HasNArgs arguments
        Msg_Arg(TAG, value) are derived from Msg_IArg interface. The TAG should be specified using the macros
        generated by the message monitor database input.

        Using the message monitor (buildmsg generated) defines is mandatory... 
        leading to simple constructors like 
                Msg_List(Msg_List::Error, SDBMSG_SAPDBMEM_OUT_OF_MEMORY)

        The special 'pointer' Msg_Has...Args' is only used to allow at compile time to check, that no requiered 
        arguments were forgotten. The number of requiered arguments is also known by the message monitor and 
        summarizes all arguments contains in short text and long text! See message monitor usage description for 
        further details.

        For every possible (0 - MSG_LIST_SINGLE_ARGS_MAX) requiered arguments a constructor is defined.
        Those ctor's only differ in the 'Msg_Has...Args' pointer and the number of requiered and optional arguments. If you want
        to add any optional arguments you must use Msg_OptArg() instead of Msg_Arg()! You also cannot exceed
        MSG_LIST_SINGLE_ARGS_MAX arguments in total (requiered + optional) and optional arguments always have
        to specified after all requiered arguments are specified.

        NOTE: The Msg_HasArgs ctor is only available if the macro MSG_LIST_ARGUMENT_COUNT_MUST_MATCH is not defined!

        The Msg_HasAnyArgs and Msg_HasArgs ctor allow no optional arguments, but any number of requiered arguments.

        For backward compatibility a additional form without an argument count pointer definition is only available,
        if the macro MSG_LIST_GENERATOR_SPECIFIES_ARGUMENT_COUNT is not defined!

        The compatibiliy with message generated by genmsg instead of message monitor buildmsg tool is also available

    @code
    Msg_List( SAPDB_Char  const * const  Component,
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
    @endcode
              
        This is an OBSOLETE constructur for compatibility with SAPDBErr_MessageList supporting old 
        SAPDBErr_MessageList construction style. This style does not allow argument 
        identification and combines all given arguments in a short text. The Message given
        is not the real message, but a format string containing '%s' for each argument.
              
    @code
    Msg_List& operator = (Msg_List &messageList)
      or
    void Assign(Msg_List &messageList)
    @endcode

        are used to set the content (implicitly clearing old content) of the message list.

    @code
    void ClearMessageList()
    @endcode    
        allows to explicitly clear content of a message list. No output is generated.

    @code
    bool const IsEmpty() const
    @endcode    
        allows to check if a given message list is empty

    @code
    void Overrule(Msg_List const &messageList)
    @endcode
    
        this calls replaces the 'main message' in a message list with the given
        message list, but the existing content is still available as secondary message list.

        The argument of this member function is directly supported by message monitor sample output.
        The macros SDBMSG_... defines ID,Component,FileName,LineNumber and Message arguments
        Msg_Arg("TAG", value) are derived from Msg_IArg interface.

        Using the message monitor (buildmsg generated) defines is mandatory... 
        leading to simple calls like 

        msg.Overrule(Msg_List(Msg_List::Error, SDBMSG_SAPDBMEM_OUT_OF_MEMORY));

    @code
    void AddDetail( SAPDB_UInt4 const          ID,
                    SAPDB_Char  const * const  Component,
                    SAPDB_Char  const * const  FileName,
                    SAPDB_UInt4 const          LineNumber,
                    SAPDB_Char  const * const  Message,
                    Msg_IArg const * Arg0 = 0,
                    Msg_IArg const * Arg1 = 0,
                    Msg_IArg const * Arg2 = 0,
                    Msg_IArg const * Arg3 = 0,
                    Msg_IArg const * Arg4 = 0,
                    Msg_IArg const * Arg5 = 0,
                    Msg_IArg const * Arg6 = 0,
                    Msg_IArg const * Arg7 = 0,
                    Msg_IArg const * Arg8 = 0,
                    Msg_IArg const * Arg9 = 0 )
    @endcode

        This member function is directly supported by message monitor sample output.
        The macros SDBMSG_... defines ID,Component,FileName,LineNumber and Message arguments
        Msg_Arg("TAG", value) are derived from Msg_IArg interface.

        Using the message monitor (buildmsg generated) defines is mandatory... 
        leading to simple calls like msg.AddDetail(SDBMSG_SAPDBMEM_OUT_OF_MEMORY)

    If message list generation runs out of memory, it does not show it externally, but directly
    writes into standard diagnostic files or output channels used. 

    @code
    void MarkAsSubMessages()
    @endcode

        This allows to add grouping information that declares all messages in the list to build a
        list of submessages. It makes sense to use this call just before using 'Overrule' to allow
        output generation to add framing for the overruled messages. See description of this member
        function for usage scenario.

    @code

    --------------------------------------------------------------------------------
    The following section is for those developers, that want to parse a message list
    --------------------------------------------------------------------------------
    @endcode

    The message content output is unified by the output member function

    @code
    SAPDB_UTF8 const *XML_Output( SAPDB_UInt4 bufferSize,
                                  SAPDB_Char *pBuffer,
                                  SAPDB_UInt4 &neededSize,
                                  Msg_List &errList) const
    @endcode

    while also includes tags for Timestamps, sub structuring etc.

    If you want to write your own output generator, you need a complete set of member function
    to access substructuring and internal message details like timestamps etc. .

    If you are only internested in the message text you can choose between

    @code
    SAPDB_Char const * const  Message() const
    @endcode

        to get only the short text and

    @code
    SAPDB_UTF8 const *MessageWithArguments( SAPDB_UInt4 bufferSize,
                                            SAPDB_Char *pBuffer,
                                            SAPDB_UInt4 &neededSize )  const

        or

    SAPDB_UTF8 const *MessageWithInsertedArguments( SAPDB_UInt4 bufferSize,
                                                    SAPDB_Char *pBuffer,
                                                    SAPDB_UInt4 &neededSize ) const
    @endcode

        to get the shorttext with appended or replaced argument tags. 
        This form is used for old style knldiag output and slowci.

    Other output detail member functions are
    
    @code
    ID(), Type() or TypeAsString(), Component(), LineNumber(), FileName(), DateTime()
    @endcode

    to access individual arguments of the first message in a message list use

    @code
    ArgCount(), ArgTag() and ArgValue()
    @endcode

    To parse though all messages and details in a message list use 

    @code
    Msg_List * const NextMessage() const
    @endcode

    Message list can be further structured. 
    
    @code
    SAPDB_UInt4 NumOfMessages() const
    SAPDB_UInt4 NumOfSubMessages() const
    SAPDB_UInt4 NumOfDetails() const
    @endcode
    What a detail is has been described above. If a message has details, its member function NumOfDetails()
    allows the output parser to indicate the next messages are details and not just information messages.
    A message itself cannot be asked if it is a detail...
    
    You can also group all message in a message list using MarkAsSubMessages(). This allows the output parser
    to use the member function NumOfSubMessages() to decide that a message list contains a list of 
    messages. It does not have any further effect on the message.

    For numbering messages contained in a message list use NumOfMessages(), but note that this will
    return 0 for all messages, but the 'main message'! A submessage itself cannot be asked which number
    it has in a list, since it could be referenced by several lists.

    @code
    SAPDB_UInt8 const GetOutputSequenceNumber();
    RTE_ISystem::DateTime const & GetOutputDateTime() const
    SAPDB_UInt4 const GetOutputProcessId() const
    SAPDB_UInt4 const GetOutputThreadId() const
    SAPDB_UInt4 const GetOutputTaskId() const
    SAPDB_UInt4 const GetOutputSchedulerId() const
    @endcode

    these are finally the output and caller identification retrieval member functions.

*/
class Msg_List
{
public:
    /*!---------------------------------------------------------------------------
      Public types used by public member functions.
     */

    //! Message type used for error messages. Alternative for enumeration Msg_List::Error
    static SAPDB_UInt4 const ErrorMsg;
    //! Message type used for warning messages. Alternative for enumeration Msg_List::Warning
    static SAPDB_UInt4 const WarningMsg;
    //! Message type used for informational messages. Alternative for enumeration Msg_List::Info
    static SAPDB_UInt4 const InfoMsg;
    //! value for unspecified data time stamp
    static RTE_ISystem::DateTime const EmptyDateTimeValue;

    //! Message time stamp string pBuffer for short form "YYYY-MM-DD HH:MM:SS"
    typedef SAPDB_Char ShortTimeBuffer[MSG_LIST_SHORT_TIME_HEADER_FORMAT_STR_LEN+1];
    //! Message time stamp string pBuffer for short form "YYYY-MM-DD HH:MM:SS.mmm"
    typedef SAPDB_Char LongTimeBuffer[MSG_LIST_LONG_TIME_HEADER_FORMAT_STR_LEN+1];

#ifdef SAPDBERR_MESSAGELIST_STILL_SUPPORTED
    //! backward compatibility MessageID type
    typedef SAPDB_UInt4  MessageID;

    //! backward compatibility MessageType enumeration
    typedef enum   { Error = 1, //!< error
                     Warning,   //!< warning
                     Info       //!< info
    } MessageType;
#endif

    // --- internal data specification
    //

    /*!
      @struct MessageData
      @brief MessageData internal representation structure (for Msg_List internal use only)

      The message binary representation consists of a fixed part (0x30 bytes) followed by
      an optional offset array (depending on number of arguments added) and the
      strings. The total MessageData length is 64bit aligned. Length is stored in MessageData.
     */
    struct MessageData
    {
        SAPDB_UInt4                  Version;      //!< Offset 0x00: binary data version
        SAPDB_UInt4                  DataLen;      //!< Offset 0x04:binary data length in bytes including variable part + alignment
        SAPDB_UInt4                  DataRefCnt;   //!< Offset 0x08:data reference count
        SAPDB_UInt4                  ID;           //!< Offset 0x0c:message id
        RTE_ISystem::DateTime        DateTime;     //!< Offset 0x10:data + time stamp (16 bytes)
        SAPDB_UInt4                  Type;         //!< Offset 0x20:message type info,warning,error
        SAPDB_UInt4                  LineNumber;   //!< Offset 0x24:source file line number from which message was created
        SAPDB_UInt2                  ComponentStringOffset; //!< Offset 0x28:byte offset in variable part for component string
        SAPDB_UInt2                  FileNameStringOffset;  //!< Offset 0x2a:byte offset in variable part for file name
        SAPDB_UInt2                  MessageStringOffset;   //!< Offset 0x2c:byte offset in variable part for message text
        SAPDB_UInt2                  ArgumentCount;         //!< Offset 0x2e:number of additional arguments
        /*!
            Variable part consists of arguement string offset array (possible empty + strings). Length of variable part is
            DataLen - 0x30. All string offsets are releativ to begin of variable part.

            Offset 0x30:Begin of possible empty variable length ArgumentCount * 2 Argument Tag/Value string offset array.
            All string offsets are releativ to beginning of variable part

            Offset 0x30 + sizeof(SAPDB_UInt2) * 2 * ArgumentCount: Begin of zero terminated strings
         */
        SAPDB_Char                   VariablePart[1];
    };

    /*!
      @brief   empty message list ctor
      @returns none
     */
    Msg_List()
    : m_pMessageData(0)
    {}

#ifdef SAPDBERR_MESSAGELIST_STILL_SUPPORTED
    /*!
    @brief   message list ctor replacing SAPDBErr_MessageList ctor
    @returns none
    */
    Msg_List( SAPDB_Char  const * const  Component,
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
              SAPDB_Char  const * const  Arg9 = NULL );

    /*!
    @brief   message list ctor replacing SAPDBErr_MessageList ctor
    @returns none
    */
    Msg_List( SAPDB_Char  const * const  Component,
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
              SAPDB_Char  const * const  Arg9 = NULL );

    /*!
      @brief return internaqlly stored system rc (faked for compatibility only...)
      @return always 0
     */
    SAPDB_UInt4 const SystemRC() const { return 0; }
#endif

#ifndef MSG_LIST_GENERATOR_SPECIFIES_ARGUMENT_COUNT
//! Macro to Setup Basic Msg_List contructor
#define MSG_CTOR_BASIC(arg_parameter_list_, validarg_copy_list_) \
    /* \
      @param Type                  [in]            - message information type like Error, Warning and Info \
      @param Component             [in]            - The name of the calling component e.g. (RTE, LOGGING ...) \
      @param FileName              [in]            - The name of the calling source file. \
      @param LineNumber            [in]            - The line number of the calling source file. \
      @param ID                    [in]            - An unequivocal identifier corresponding to the current message. \
      @param Message               [in]            - The short message string. \
      @param Arg0                  [in (optional)] - Argument 0. \
      @param Arg1                  [in (optional)] - Argument 1. \
      @param Arg2                  [in (optional)] - Argument 2. \
      @param Arg3                  [in (optional)] - Argument 3. \
      @param Arg4                  [in (optional)] - Argument 4. \
      @param Arg5                  [in (optional)] - Argument 5. \
      @param Arg6                  [in (optional)] - Argument 6. \
      @param Arg7                  [in (optional)] - Argument 7. \
      @param Arg8                  [in (optional)] - Argument 8. \
      @param Arg9                  [in (optional)] - Argument 9. \
    */ \
    Msg_List( SAPDB_UInt4 const          Type \
            , MSG_CTOR_STANDARD_PARAMETER_SET \
            , arg_parameter_list_ ) \
            : MSG_CTOR_INIT_LIST \
{ \
    Msg_IOptArg  const * ValidArgs[MSG_LIST_SINGLE_ARGS_MAX]; \
    SAPDB_UInt4       ValidArgCount = 0; \
    \
    validarg_copy_list_ \
    \
    FillMessageList( Type \
                   , Component \
                   , FileName \
                   , LineNumber \
                   , ID \
                   , Message \
                   , ValidArgCount \
                   , ValidArgs ); \
    TraceMessageCopy(); \
}
    //! @brief   message list object ctor with individual arguments
    MSG_CTOR_BASIC( MSG_CTOR_ANY_ARGS, MSG_CTOR_SETUP_ANY_VALIDARGS )
#undef MSG_CTOR_BASIC
#endif /* MSG_LIST_GENERATOR_SPECIFIES_ARGUMENT_COUNT */

//---

    /*
      Almost identical with MSG_CTOR_BASIC macro, but add argument count type parameter
     */
//! Macro to Setup Msg_List contructor for various allowed number of optional arguments
#define MSG_CTOR(arg_count_type_parameter_, arg_parameter_list_, validarg_copy_list_) \
    /* \
      @param Type                  [in]            - message information type like Error, Warning and Info \
      @param Component             [in]            - The name of the calling component e.g. (RTE, LOGGING ...) \
      @param FileName              [in]            - The name of the calling source file. \
      @param LineNumber            [in]            - The line number of the calling source file. \
      @param ID                    [in]            - An unequivocal identifier corresponding to the current message. \
      @param Message               [in]            - The short message string. \
      followed possibly by Msg_HasNArgs pointer to allow compiler to valid correct ctor argument passing \
      @param Arg0                  [in (optional)] - Argument 0. \
      @param Arg1                  [in (optional)] - Argument 1. \
      @param Arg2                  [in (optional)] - Argument 2. \
      @param Arg3                  [in (optional)] - Argument 3. \
      @param Arg4                  [in (optional)] - Argument 4. \
      @param Arg5                  [in (optional)] - Argument 5. \
      @param Arg6                  [in (optional)] - Argument 6. \
      @param Arg7                  [in (optional)] - Argument 7. \
      @param Arg8                  [in (optional)] - Argument 8. \
      @param Arg9                  [in (optional)] - Argument 9. \
    */ \
    Msg_List( SAPDB_UInt4 const          Type \
            , MSG_CTOR_STANDARD_PARAMETER_SET \
            , arg_count_type_parameter_ * \
            , arg_parameter_list_ ) \
    : MSG_CTOR_INIT_LIST \
{ \
    Msg_IOptArg  const * ValidArgs[MSG_LIST_SINGLE_ARGS_MAX]; \
    SAPDB_UInt4       ValidArgCount = 0; \
    \
    validarg_copy_list_ \
    \
    FillMessageList( Type \
                   , Component \
                   , FileName \
                   , LineNumber \
                   , ID \
                   , Message \
                   , ValidArgCount \
                   , ValidArgs ); \
    TraceMessageCopy(); \
}
#ifndef MSG_LIST_ARGUMENT_COUNT_MUST_MATCH
    //! @brief   This ctor is used with up to 10 any type arguments
    MSG_CTOR(Msg_HasArgs,MSG_CTOR_ANY_ARGS,MSG_CTOR_SETUP_ANY_VALIDARGS)
#endif
    //! @brief   This ctor is used with up to 10 requiered arguments
    MSG_CTOR(Msg_HasAnyArgs,MSG_CTOR_ANY_ARGS,MSG_CTOR_SETUP_ANY_VALIDARGS)
    //! @brief   This ctor is used with no requiered but up to 10 optional arguments
    MSG_CTOR(Msg_Has0Args,MSG_CTOR_0_ARGS,MSG_CTOR_SETUP_0_VALIDARGS)
    //! @brief   This ctor is used with 1 requiered but up to 9 optional arguments
    MSG_CTOR(Msg_Has1Args,MSG_CTOR_1_ARGS,MSG_CTOR_SETUP_1_VALIDARGS)
    //! @brief   This ctor is used with 2 requiered but up to 8 optional arguments
    MSG_CTOR(Msg_Has2Args,MSG_CTOR_2_ARGS,MSG_CTOR_SETUP_2_VALIDARGS)
    //! @brief   This ctor is used with 3 requiered but up to 7 optional arguments
    MSG_CTOR(Msg_Has3Args,MSG_CTOR_3_ARGS,MSG_CTOR_SETUP_3_VALIDARGS)
    //! @brief   This ctor is used with 4 requiered but up to 6 optional arguments
    MSG_CTOR(Msg_Has4Args,MSG_CTOR_4_ARGS,MSG_CTOR_SETUP_4_VALIDARGS)
    //! @brief   This ctor is used with 5 requiered but up to 5 optional arguments
    MSG_CTOR(Msg_Has5Args,MSG_CTOR_5_ARGS,MSG_CTOR_SETUP_5_VALIDARGS)
    //! @brief   This ctor is used with 6 requiered but up to 4 optional arguments
    MSG_CTOR(Msg_Has6Args,MSG_CTOR_6_ARGS,MSG_CTOR_SETUP_6_VALIDARGS)
    //! @brief   This ctor is used with 7 requiered but up to 3 optional arguments
    MSG_CTOR(Msg_Has7Args,MSG_CTOR_7_ARGS,MSG_CTOR_SETUP_7_VALIDARGS)
    //! @brief   This ctor is used with 8 requiered but up to 2 optional arguments
    MSG_CTOR(Msg_Has8Args,MSG_CTOR_8_ARGS,MSG_CTOR_SETUP_8_VALIDARGS)
    //! @brief   This ctor is used with 9 requiered but 1 optional arguments
    MSG_CTOR(Msg_Has9Args,MSG_CTOR_9_ARGS,MSG_CTOR_SETUP_9_VALIDARGS)
    //! @brief   This ctor is used with 10 requiered but no optional arguments
    MSG_CTOR(Msg_Has10Args,MSG_CTOR_10_ARGS,MSG_CTOR_SETUP_10_VALIDARGS)
#undef MSG_CTOR
    /*!
      @brief   message list object ctor with given argument vector

      @param Type                  [in]            - message information type like Error, Warning and Info
      @param Component             [in]            - The name of the calling component e.g. (RTE, LOGGING ...)
      @param FileName              [in]            - The name of the calling source file.
      @param LineNumber            [in]            - The line number of the calling source file.
      @param ID                    [in]            - An unequivocal identifier corresponding to the current message.
      @param Message               [in]            - The short message string.
      @param ArgCount              [in]            - Argument count
      @param ArgArray              [in]            - Argument array
      @returns       none
    */

    Msg_List( SAPDB_UInt4 const          Type,
              MSG_CTOR_STANDARD_PARAMETER_SET ,
              SAPDB_UInt4 const          ArgCount,
              Msg_IOptArg const **       ArgArray )
    : MSG_CTOR_INIT_LIST
    {
        FillMessageList( Type
                       , Component
                       , FileName
                       , LineNumber
                       , ID
                       , Message
                       , ArgCount
                       , ArgArray );
        TraceMessageCopy();
    }

    /*!
      @brief   message list object ctor with given argument vector and time stamp

      @param Type                  [in]            - message information type like Error, Warning and Info
      @param Component             [in]            - The name of the calling component e.g. (RTE, LOGGING ...)
      @param FileName              [in]            - The name of the calling source file.
      @param LineNumber            [in]            - The line number of the calling source file.
      @param ID                    [in]            - An unequivocal identifier corresponding to the current message.
      @param DateTime              [in]            - Date and time when message was created
      @param Message               [in]            - The short message string.
      @param ArgCount              [in]            - Argument count
      @param ArgArray              [in]            - Argument array
      @returns       none
    */
    Msg_List( SAPDB_UInt4 const            Type,
              SAPDB_UInt4 const            ID,
              SAPDB_Char  const * const    Component,
              SAPDB_Char  const * const    FileName,
              SAPDB_UInt4 const            LineNumber,
              RTE_ISystem::DateTime const &DateTime,
              SAPDB_Char  const * const    Message,
              SAPDB_UInt4 const            ArgCount,
              Msg_IOptArg const **         ArgArray )
    : MSG_CTOR_INIT_LIST
    {
        FillMessageList( true
                       , Type
                       , Component
                       , FileName
                       , LineNumber
                       , ID
                       , DateTime
                       , Message
                       , ArgCount
                       , ArgArray );
        TraceMessageCopy();
    }

    /*!
    @brief   copy message list ctor
    @returns none
    */
    Msg_List( Msg_List const &MessageList )
    : MSG_CTOR_INIT_LIST
    {
        CopyMessageList( MessageList );
    }

    /*!
    @brief   virtual dtor
    @returns none
    */
    virtual ~Msg_List();

    /*!
    @brief   Returns true if the message list is empty.
    @returns true if message list is empty, false if message in message list
    */
    inline bool const   IsEmpty() const
    {
        return ( m_pMessageData == 0 );
    }

    /*!
    @brief   Returns the number of messages contained in 'main message'

    This call is only meaningfull for a message list that is used for output. The number
    of messages returned for subsequent messages in a message list may be zero.

    @returns Number of messages in complete Msg_List object
    */
    inline SAPDB_UInt4  NumOfMessages() const { return m_NumOfMessages; }

    /*!
    @brief   Returns the number of sub messages contained in 'first message'

    If the number of submessages is non zero, the XML_Output will frame the message and the
    NumOfSubMessages() messages in a 'MSGC' frame.

    @returns Number of messages
    */
    inline SAPDB_UInt4  NumOfSubMessages() const { return m_NumOfSubMessages; }

    /*!
    @brief   Returns the number of detail messages contained in 'first message'

    If the number of details is non zero, the XML_Output will frame the next message and the
    NumOfDetails() messages in a 'MSGD' frame.

    @returns Number of details
    */
    inline SAPDB_UInt4  NumOfDetails() const { return m_NumOfDetails; }

    /*!
    @brief set the 'main message' sequence number and other identification

    You can set the output sequence number only once. To reset it, you must reset the complete message list.
    The only way to use the same message with a different output sequence number is by copying it into
    a new message and setting the new output sequence number in the new message.

    The outputTimeStamp is generated for that time, when you successfully set the output sequence
    number.

    By loading a message using 'Load' member function, the sequence number is set too.

    @param outputSequenceNumber [in] the new output sequence number (must be > 0)
    @param outputThreadId [in] thread issuing the message (or 0 if not from multithreaded process)
    @param outputTaskId [in] task issuing the message (or 0 if not from inside a task)
    @param outputSchedulerId [in] scheduler (UKT) used by task
    @return true if set new output identification, false if already set
     */
    SAPDB_Bool SetOutputIdentification( SAPDB_UInt8 const outputSequenceNumber
                                      , SAPDB_UInt4 const outputThreadId=0
                                      , SAPDB_UInt4 const outputTaskId=0
                                      , SAPDB_UInt4 const outputSchedulerId=0 );

    /*!
    @brief set the 'main message' caller identification

    You can set the caller identification if you want. This will allow to use the internal tags _PROCESS,_THREAD,_TASK and _UKT.
    If no values are givenm only the process id is set, which is the only information that always exists...
    This call is internally used by SetOutputIdentification() too.

    @param outputThreadId [in] thread issuing the message (or 0 if not from multithreaded process)
    @param outputTaskId [in] task issuing the message (or 0 if not from inside a task)
    @param outputSchedulerId [in] scheduler (UKT) used by task ( or 0 if not within a UKT)
     */
    void SetCallerIdentification( SAPDB_UInt4 const outputThreadId=0
                                , SAPDB_UInt4 const outputTaskId=0
                                , SAPDB_UInt4 const outputSchedulerId=0 );
    /*!
    @brief get the 'main message' output sequence number

    Cannot be inlined, since it needs atomic operation on 32bit platforms, since it is an 8Byte value

    @return the current output sequence number or 0 if not set
     */
    SAPDB_UInt8 const GetOutputSequenceNumber() const;

    /*!
    @brief get the 'main message' output timestamp
    @return the time stamp generated by call to SetOutputIdentification()
     */
    inline RTE_ISystem::DateTime const & GetOutputDateTime() const { return m_OutputDateTime; }

    /*!
    @brief get the 'main message' output process id
    @return the process id generated by call to SetOutputIdentification()
     */
    inline SAPDB_UInt4 const GetOutputProcessId() const { return m_OutputProcessId; }

    /*!
    @brief get the 'main message' output thread id
    @return the thread id passed as parameter to last successfull SetOutputIdentification()
     */
    inline SAPDB_UInt4 const GetOutputThreadId() const { return m_OutputThreadId; }

    /*!
    @brief get the 'main message' output task id
    @return the task id passed as parameter to last successfull SetOutputIdentification()
     */
    inline SAPDB_UInt4 const GetOutputTaskId() const { return m_OutputTaskId; }
    /*!
    @brief get the 'main message' output scheduler id
    @return the scheduler id (AKA UKT_Id) passed as parameter to last successfull SetOutputIdentification()
     */
    inline SAPDB_UInt4 const GetOutputSchedulerId() const { return m_OutputSchedulerId; }

    /*!
      @brief  OBSOLETE use Overrule() instead

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        You should always combine an newer message to an older one
        to get a correct message ordering.

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

      Output order is always stack order : first messages in list is last message in output

      Alternate historical name of 'Overrule'. 'Overrule' makes more sense since an 'AppendNewMessage'
      does not append anymore, but actually prepends....

     @param MessageList [in] the message list containing the messages to append
    */
    inline void AppendNewMessage ( Msg_List const &MessageList ) { (void)PushMessage( MessageList ); }

    /*!
      @brief   This methode overrules the 'main message' of a given message list

      Overruling works as prepending given message list in front of message list content.
      
      Since the first message in a message list always is the 'main message' for output it 
      overrules the meaning of the message list.
      
      This alternate name for 'AppendNewMessage' makes more sense since an 'Overrule'
      actually sets the type of the complete message list to the type of the given messageList
      and keeps the former message content as additional information. The output of the message
      list always uses the message list order. Thanks to Daniel for discussion about changing the
      former name 'AppendNewMessage'.

      Since internally the message list handling is using its own allocator the out of memory
      situation is very unlikely and result always in internal output calls.

     @param MessageList [in] the message list containing the messages to push

     @return none
    */
    inline void Overrule ( Msg_List const &MessageList ) { (void)PushMessage( MessageList ); }

    /*!
      @brief   This methode pushes a given message list

      Output order is always stack order : first messages in list is last message in output

      This alternate name for 'AppendNewMessage' makes more sense since an 'Overrule'
      actually sets the type of the complete message list to the type of the given messageList
      and keeps the former message content as additional information. This is equivalent to
      a push operation on a stack. You may think about the output of the message list to do
      the 'pop' operation. Thanks to Daniel for this more imaginative name.

      If the operation fails, the message list content will be as much of 'MessageList' as was
      still possible to copy and 'false' is returned. The caller may decide to ignore it or not.

      Since internally the message list handling is using its own allokator the out of memory
      situation is very unlikely...

     @param MessageList [in] the message list containing the messages to push

     @return true if succeeded, false if out of memory for operation
    */
    SAPDB_Bool PushMessage ( Msg_List const &MessageList );

    /*!
      @brief   This marks the contained messages in the message list as submessages
      This call allows the output parser to group messages which logically build a group
      of messages into a list. XML_Output frames these submessages using the MSGC tag.

      @code

      A usage scenario is to collect individual messages using 'Overrule' in a message list
      first, then to mark the list and finally use 'Overrule' for the wanted error message
      that contains all the collected submessages as a message list.

      SAPDB_Bool CheckParameters(Msg_List &messageList)
      {
          SAPDB_Bool gotProblem = false;

          for ( i = 0; i < numOfParameter; i++ )
          {
              Msg_List tmpList;
              if ( !CheckSingleParameter(i, tmpList) ) // subroutines always set the message list !
              {
                  messageList.Overrule(tmpList);
                  gotProblem = true;
              }
          }

          if ( gotProblem )
          {
              messageList.MarkAsSubMessages();
              messageList.Overrule(SDBMSG_RTECONF_PARAMETER_CHECK_FAILED);
          }
      }

      Another scenario is to collect two message lists (i.e. from two alternative subroutine
      calls) and combine them as a 

      SAPDB_Bool FindFile(Msg_List &messageList)
      {
          SAPDB_Bool gotProblem = false;

          Msg_List localSearchResult;
          Msg_List globalSearchResult;

          if ( !SearchLocal(localSearchResult) )
          {
              localSearchResult.AddDatail(SDBMSG_RTE_WHEREVER, "local search");
              localSearchResult.MarkAsSubMessages();
          }
          else
          {
              return true;
          }

          if ( !SearchGlobal(globalSearchResult) )
          {
              globalSearchResult.AddDatail(SDBMSG_RTE_WHEREVER, "global search");
              globalSearchResult.MarkAsSubMessages();
          }
          else
          {
              return true;
          }

          messageList.Assign(localSearchResult);
          messageList.Overrule(globalSearchResult);
          messageList.Overrule(SDBMSG_RTE_SEARCH_FAILED_AFTER_RETRIES);
      }

      @endcode
     */
    inline void MarkAsSubMessages() { m_NumOfSubMessages = m_NumOfMessages; }

    /*!
      @brief   This test if the 'first message' should be handed as head of a list of submessages
     */
    inline SAPDB_Bool CheckIfHandledAsList() const { return m_NumOfSubMessages != 0; }

#ifndef MSG_LIST_GENERATOR_SPECIFIES_ARGUMENT_COUNT

//! Macro to Setup Basic Msg_List::AddDetail member function contructor
#define MSG_ADD_DETAIL_BASIC(arg_parameter_list_, validarg_copy_list_) \
    /* \
\
      A detail is an additional information that does not change neither the meaning nor \
      the message. It therefore is not appended as message, which would overrule the type \
      of the messagelist, but inserted as second element behind the current message. \
\
      @param ID                    [in]            - An unequivocal identifier corresponding to the current message. \
      @param Component             [in]            - The name of the calling component e.g. (RTE, LOGGING ...) \
      @param FileName              [in]            - The name of the calling source file. \
      @param LineNumber            [in]            - The line number of the calling source file. \
      @param Message               [in]            - The short message string. \
      @param Arg0                  [in (optional)] - Argument 0. \
      @param Arg1                  [in (optional)] - Argument 1. \
      @param Arg2                  [in (optional)] - Argument 2. \
      @param Arg3                  [in (optional)] - Argument 3. \
      @param Arg4                  [in (optional)] - Argument 4. \
      @param Arg5                  [in (optional)] - Argument 5. \
      @param Arg6                  [in (optional)] - Argument 6. \
      @param Arg7                  [in (optional)] - Argument 7. \
      @param Arg8                  [in (optional)] - Argument 8. \
      @param Arg9                  [in (optional)] - Argument 9. \
    */ \
    inline void AddDetail( \
        MSG_CTOR_STANDARD_PARAMETER_SET \
      , arg_parameter_list_ ) \
{ \
    Msg_IOptArg  const * ValidArgs[MSG_LIST_SINGLE_ARGS_MAX]; \
    SAPDB_UInt4       ValidArgCount = 0; \
    \
    validarg_copy_list_ \
    \
    InsertDetailOfMessageList( ID \
                             , Component \
                             , FileName \
                             , LineNumber \
                             , Message \
                             , ValidArgCount \
                             , ValidArgs ); \
}
    //! @brief   This methode adds a detail to the 'main message' in message list
    MSG_ADD_DETAIL_BASIC( MSG_CTOR_ANY_ARGS, MSG_CTOR_SETUP_ANY_VALIDARGS )
#undef MSG_ADD_DETAIL_BASIC
#endif

//! Macro to Setup Msg_List::AddDetail for various allowed number of optional arguments
#define MSG_ADD_DETAIL(arg_count_type_parameter_, arg_parameter_list_, validarg_copy_list_) \
    /* \
      A detail is an additional information that does not change neither the meaning nor \
      the message. It therefore is not appended as message, which would overrule the type \
      of the messagelist, but inserted as second element behind the current message. \
    \
      @param ID                    [in]            - An unequivocal identifier corresponding to the current message. \
      @param Component             [in]            - The name of the calling component e.g. (RTE, LOGGING ...) \
      @param FileName              [in]            - The name of the calling source file. \
      @param LineNumber            [in]            - The line number of the calling source file. \
      @param Message               [in]            - The short message string. \
      followed possibly by Msg_HasNArgs pointer to allow compiler to valid correct member function argument passing \
      @param Arg0                  [in (optional)] - Argument 0. \
      @param Arg1                  [in (optional)] - Argument 1. \
      @param Arg2                  [in (optional)] - Argument 2. \
      @param Arg3                  [in (optional)] - Argument 3. \
      @param Arg4                  [in (optional)] - Argument 4. \
      @param Arg5                  [in (optional)] - Argument 5. \
      @param Arg6                  [in (optional)] - Argument 6. \
      @param Arg7                  [in (optional)] - Argument 7. \
      @param Arg8                  [in (optional)] - Argument 8. \
      @param Arg9                  [in (optional)] - Argument 9. \
    */ \
    inline void AddDetail( \
        MSG_CTOR_STANDARD_PARAMETER_SET \
      , arg_count_type_parameter_ * \
      , arg_parameter_list_ ) \
{ \
    Msg_IOptArg  const * ValidArgs[MSG_LIST_SINGLE_ARGS_MAX]; \
    SAPDB_UInt4       ValidArgCount = 0; \
    \
    validarg_copy_list_ \
    \
    InsertDetailOfMessageList( ID \
                             , Component \
                             , FileName \
                             , LineNumber \
                             , Message \
                             , ValidArgCount \
                             , ValidArgs ); \
}
#ifndef MSG_LIST_ARGUMENT_COUNT_MUST_MATCH
    //! @brief   This methode adds a detail with up to 10 any type arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_HasArgs,MSG_CTOR_ANY_ARGS,MSG_CTOR_SETUP_ANY_VALIDARGS)
#endif
    //! @brief   This methode adds a detail with up to 10 requiered arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_HasAnyArgs,MSG_CTOR_ANY_ARGS,MSG_CTOR_SETUP_ANY_VALIDARGS)
    //! @brief   This methode adds a detail with no requiered but up to 10 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has0Args,MSG_CTOR_0_ARGS,MSG_CTOR_SETUP_0_VALIDARGS)
    //! @brief   This methode adds a detail with 1 requiered and up to 9 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has1Args,MSG_CTOR_1_ARGS,MSG_CTOR_SETUP_1_VALIDARGS)
    //! @brief   This methode adds a detail with 2 requiered and up to 8 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has2Args,MSG_CTOR_2_ARGS,MSG_CTOR_SETUP_2_VALIDARGS)
    //! @brief   This methode adds a detail with 3 requiered and up to 7 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has3Args,MSG_CTOR_3_ARGS,MSG_CTOR_SETUP_3_VALIDARGS)
    //! @brief   This methode adds a detail with 4 requiered and up to 6 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has4Args,MSG_CTOR_4_ARGS,MSG_CTOR_SETUP_4_VALIDARGS)
    //! @brief   This methode adds a detail with 5 requiered and up to 5 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has5Args,MSG_CTOR_5_ARGS,MSG_CTOR_SETUP_5_VALIDARGS)
    //! @brief   This methode adds a detail with 6 requiered and up to 4 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has6Args,MSG_CTOR_6_ARGS,MSG_CTOR_SETUP_6_VALIDARGS)
    //! @brief   This methode adds a detail with 7 requiered and up to 3 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has7Args,MSG_CTOR_7_ARGS,MSG_CTOR_SETUP_7_VALIDARGS)
    //! @brief   This methode adds a detail with 8 requiered and up to 2 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has8Args,MSG_CTOR_8_ARGS,MSG_CTOR_SETUP_8_VALIDARGS)
    //! @brief   This methode adds a detail with 9 requiered and 1 optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has9Args,MSG_CTOR_9_ARGS,MSG_CTOR_SETUP_9_VALIDARGS)
    //! @brief   This methode adds a detail with 10 requiered but no optional arguments to the 'main message' in message list
    MSG_ADD_DETAIL(Msg_Has10Args,MSG_CTOR_10_ARGS,MSG_CTOR_SETUP_10_VALIDARGS)
#undef MSG_ADD_DETAIL

    /*!
      @brief OBSOLETE Use Overrule() or AddDetail() instead

      Create a combination of message list content and given message list in given order.

      The first message in a message list concatenation is the message, which defines
      the message type!

      a = b + c;   
      
      if 'b' is error,runtime,56 and 'c' is info,logging,8 , 'a' will be error,runtime,56 with
      the NextMessage() pointing to info,logging,8

      This call does not modifies the original content!
      
      @param MessageList [in] the message list containing the messages to combine with
     */
    inline Msg_List operator + ( Msg_List const &MessageList ) const
    {
        Msg_List Result = MessageList;
        
        Result.Overrule(*this);

        return Result;
    }

    /*!
      @brief replace complete content of message list with given message list

      This implicitly does a ClearMessageList() follwed by a CopyMessageList()

      @param MessageList [in] the message list to assign
      @return a reference to the assigned message list
     */
    inline Msg_List& operator = ( Msg_List const &MessageList )
    {
        // --- do nothing on Messagelist = Messagelist
        if ( &MessageList != this )
        {
            if ( 0 != m_pMessageData ) DoClear();   // --- remove old content first!
            CopyMessageList( MessageList );
        }
        return *this;
    }

    /*!
    @brief   Empty an message list object.
    */
    inline void ClearMessageList() { if ( 0 != m_pMessageData ) DoClear(); }

    /*!
    @brief   Empty an message list object.
    */
    inline void Clear() { if ( 0 != m_pMessageData ) DoClear(); }

    /*!
      @brief replace complete content of message list with given message list

      This implicitly does a ClearMessageList() follwed by a CopyMessageList()
      If the same message list is assigned, nothing is done

      @param MessageList [in] the message list to assign
      @return nothing
     */
    inline void Assign( Msg_List const &MessageList )
    {
        // --- do nothing on Messagelist = Messagelist
        if ( &MessageList != this )
        {
            if ( 0 != m_pMessageData ) DoClear();   // --- remove old content first!
            CopyMessageList( MessageList );
        }
    }

    /*!
    @brief   Returns a pointer to next message of the message list.
    @returns Pointer to the next message list object.
    Returns 0 if no object exists.
    */
    inline Msg_List * const   NextMessage() const { return m_pNextMessage; }

    //-----------------------------------------------------------------------------------
    //  Message List element access functions
    //-----------------------------------------------------------------------------------

    /*!
    @brief   Returns the message identifier of 'first message'
    @returns Message identifier.
    */
    inline const SAPDB_UInt4    ID() const
    {
        return ( m_pMessageData ? m_pMessageData->ID : 0 );
    }

#ifdef SAPDBERR_MESSAGELIST_STILL_SUPPORTED
    /*!
    @brief   Returns the message type (Warning, Error, Info) of 'first message'
    @returns Message type in string form
    */
    inline MessageType const Type() const
    {
        return (MessageType)( m_pMessageData ? m_pMessageData->Type : Info );
    }
#endif

    /*!
    @brief   Returns the message type (Warning, Error, Info) of 'first message'
    @returns Message type in string form
    */
    inline SAPDB_Char const *TypeAsString() const
    {
        static SAPDB_Char const * typeNames[4] =
        {
            "Unknown"
           ,"Error"
           ,"Warning"
           ,"Info"
        };

        return typeNames[Type() > 4U ? 0U : Type()];
    }

    /*!
    @brief   Returns the component name of 'first message'
    @returns component string
    */
    inline SAPDB_Char const * const  Component() const
    {
        return ( m_pMessageData ? (m_pMessageData->VariablePart + m_pMessageData->ComponentStringOffset) : "" );
    }

    /*!
    @brief   Returns the file name of module where the message was created of 'first message'
    @returns Module file name.
    */
    inline SAPDB_Char const * const  FileName() const
    {
        return ( m_pMessageData ? (m_pMessageData->VariablePart + m_pMessageData->FileNameStringOffset) : "" );
    }

    /*!
    @brief   Returns the line number of module where the message was created of 'first message'
    @returns Line number.
    */
    inline SAPDB_UInt4 const LineNumber() const
    {
        return ( m_pMessageData ? m_pMessageData->LineNumber : 0 );
    }

    /*!
      @brief get data reference
      @return current data reference count or 0 if no data
     */
    inline SAPDB_UInt4 const GetDataRef() const
    {
        return ( m_pMessageData ? m_pMessageData->DataRefCnt : 0 );
    }

    /*!
      @brief get object reference
      @return current object reference count
     */
    inline SAPDB_UInt4  GetObjectRef() const { return m_ObjectRefCnt; }

    /*!
    @brief   Returns the message string of 'first message'
    @returns Message string.
    */
    inline SAPDB_Char const * const  Message() const
    {
        return ( m_pMessageData ? (m_pMessageData->VariablePart + m_pMessageData->MessageStringOffset) : "" );
    }

    /*!
    @brief   Return the message string with arguments of 'first message'

    This is used in slowci protocol output file. It does not include other message
    contained or file/linenumber. Argument tags are not displayed. Arguments are
    separated by comma.

    @param   bufferSize [in] output pBuffer size
    @param   pBuffer [out] output pBuffer
    @param   neededPascalSize [out] number of bytes used or needed !!! NOT including zero termination !!!
    @return "buffer space exhausted" if neededPascalSize > bufferSize problem, else line (not necessary zero terminated!!!!)
     */
    SAPDB_UTF8 const *MessageWithArguments( SAPDB_UInt4  bufferSize,
                                            SAPDB_Char  *pBuffer,
                                            SAPDB_UInt4 &neededPascalSize ) const;

    /*!
    @brief   Return the message string with arguments of 'first message'

    This is used in dbm protocol console output. It does not include other message
    contained or file/linenumber. Argument tags in message short text are replaced.
    Additional arguments are added as comma separated list of tag and value if wanted.
    
    @param   bufferSize [in] output pBuffer size
    @param   pBuffer [out] output pBuffer
    @param   neededSize [out] number of bytes used or needed including zero termination
    @param   addListOfExtraArguments [in] indicate extra arguments should be added as comma separated list
    @return "buffer space exhausted" if problem, else buffer (always zero terminated)
     */
    SAPDB_UTF8 const *MessageWithInsertedArguments( SAPDB_UInt4 bufferSize,
                                                    SAPDB_Char *pBuffer,
                                                    SAPDB_UInt4 &neededSize,
                                                    SAPDB_Bool  addListOfExtraArguments = true) const;

    /*!
    @brief   Return the text string with replaced argument tags

    This routine replaces tags in form $TAG$ with the values. Each argument which is not referenced
    in textWithTags will be added in a comma seprated list, if wanted.

    @param   textWithTags [in] the text with the tags to replace
    @param   tagCount [in] size of tag arrays
    @param   tagNameArray [in] array of tagCount tag names 
    @param   tagValueArray [in] array of tagCount tag values 
    @param   bufferSize [in] output pBuffer size
    @param   pBuffer [out] output pBuffer
    @param   neededSize [out] number of bytes used or needed including zero termination
    @param   addListOfExtraArguments [in] indicate extra arguments should be added as comma separated list
    @return "buffer space exhausted" if problem, else buffer (not necessary zero terminated)
     */
    static SAPDB_UTF8 const *SubstituteArgumentTags( SAPDB_Char const  *textWithTags
                                                   , SAPDB_UInt4 const  tagCount
                                                   , SAPDB_Char const **tagNameArray
                                                   , SAPDB_Char const **tagValueArray
                                                   , SAPDB_UInt4        bufferSize
                                                   , SAPDB_Char        *pBuffer
                                                   , SAPDB_UInt4       &neededSize
                                                   , SAPDB_Bool         addListOfExtraArguments = true);

    /*!
    @brief   Returns the date and time the 'first message' was created 
    @returns Message date (RTE_ISystem::DateTime)
    */
    inline RTE_ISystem::DateTime const&    DateTime() const
    {
        return ( m_pMessageData ? m_pMessageData->DateTime : EmptyDateTimeValue );
    }

    /*!
    @brief   Returns the 'first message' argument count
    @returns Number of argument contained in message
    */
    inline SAPDB_UInt2 const ArgCount() const
    {
        return ( m_pMessageData ? m_pMessageData->ArgumentCount : 0 );
    }

    /*!
    @brief   Returns the tag of given 'first message' arguemt
    @param   argumentNo [in] the wanted argument (0 = first)
    @returns the tag string or 0 if no such argument
    */
    inline SAPDB_Char const * const  ArgTag(SAPDB_UInt4 argumentNo) const
    {
        return (
              ( m_pMessageData && m_pMessageData->ArgumentCount > argumentNo )
            ? ( m_pMessageData->VariablePart + ((SAPDB_UInt4 *)m_pMessageData->VariablePart)[2*argumentNo] ) : 0
               );
    }

    /*!
    @brief   Returns the value of given 'first message' argument
    @param   argumentNo [in] the wanted argument (0 = first)
    @returns the value string or 0 if no such argument
    */
    inline SAPDB_UTF8 const * const  ArgValue(SAPDB_UInt4 argumentNo) const
    {
        return (SAPDB_UTF8 const *) (
              ( m_pMessageData && m_pMessageData->ArgumentCount > argumentNo )
            ? ( m_pMessageData->VariablePart + ((SAPDB_UInt4 *)m_pMessageData->VariablePart)[2*argumentNo+1] ) : 0
                );
    }

    /*!
    @brief   Returns the value of given 'first message' argument
    @param   argumentName [in] the wanted argument 
    @returns the tag string or 0 if no such argument
    */
    SAPDB_UTF8 const * const  ArgValue(SAPDB_Char const * argumentName) const;

    /*!
    @brief   Returns the size of the binary content of the 'first message' only
    @returns the size of the contained message data or 0 if no data present
    */
    inline SAPDB_UInt4 BinarySize() const
    {
        return  ( m_pMessageData ? m_pMessageData->DataLen : 0 );
    }

    /*!
    @brief   Returns the pointer to the binary content of the 'first message' only
    @returns the pointer to the contained message data or 0 if no data present
    */
    inline MessageData * BinaryContent() const { return m_pMessageData; }

    /*!
    @brief allow to check if loaded content was truncated

    The value is set by 'Load' member function if loaded buffer was result
    of an incomplete dump or was not loaded completely due to insufficient
    buffer size
    */
    inline SAPDB_Bool const WasDumpTruncated() const { return m_DumpTruncated != 0; }

    //-----------------------------------------------------------------------------------
    //  Message List network transfer functions
    //-----------------------------------------------------------------------------------

    /*!
     @brief Calculate minimum and needed size to store a complete message list into a buffer

     If the message list (including all messages ever appended) would be dumped into a
     given buffer for transfer, it varies in size due to compression used. The actual
     size can be calculated in advance using this routine.
     If it has to be truncated, it will get truncated but to be meaningfull, it still
     needs a minimum size. This minimum size if returned too.

     @param neededSizeInBytes [out] actual number of bytes used in buffer
     @param minimumSizeInBytes [out] the minimum size in bytes if truncation is allowed
     @param errList [out] the error list in case of internal problem
     @return true if call succeded, false else
     */
    SAPDB_Bool CalcNeededSizeInBytes ( SAPDB_UInt4 &neededSizeInBytes,
                                       SAPDB_UInt4 &minimumSizeInBytes,
                                       Msg_List    &errList ) const;

    /*!
     @brief Store a complete message list into buffer

     The message list (including all messages ever appended) is dumped into given buffer.
     If it has to be truncated, it will get truncated. To prevent truncation set the
     truncationAllow flag to false. The actual number of bytes written into the buffer
     is returned. The buffer must not be alignment in any way. It is always written in
     network byte order. This seems cost intensive, but since the 'integer' value must be
     written as individual bytes anyhow...

     @param bufferSizeInBytes [in] size of buffer to fill
     @param allowTruncate [in] truncation can be forbidden by using false here...
     @param pBuffer [out] buffer to fill
     @param savedBytes [out] actual number of bytes used in buffer
     @param errList [out] the error list in case of problem (i.e. truncation needed, but forbidden)
     @return true if store succeded, false if not (errList filled)
     */
    SAPDB_Bool Store ( SAPDB_UInt4 const  bufferSizeInBytes,
                       SAPDB_Bool  const  allowTruncate,
                       void * const       pBuffer,
                       SAPDB_UInt4       &savedBytes,
                       Msg_List &         errList ) const;

    /*!
     @brief Load a complete message list from a given buffer

     The message list (including all messages ever appended) is loaded from a given buffer.
     If it had to be truncated during store, it will be returned truncated but still usable.

     This call may also set the message sequence number, which allows to identify the message
     which has been stoed. This is important if the message loaded was found to be truncated.

     @param bufferSizeInBytes [in] size of buffer to load from
     @param pBuffer [in] buffer to load from
     @param wasTruncated [out] truncation can be forbidden by using false here...
     @param errList [out] the error list in case of problem (i.e. truncation needed, but forbidden)
     @return true if load succeded, false if not (errList filled)
     */
    SAPDB_Bool Load( SAPDB_UInt4 const  bufferSizeInBytes,
                     void * const       pBuffer,
                     SAPDB_Bool        &wasTruncated,
                     Msg_List &         errList );

    //-----------------------------------------------------------------------------------
    //  Message List element access convertion functions
    //-----------------------------------------------------------------------------------

    /*!
    @brief   Returns the date and time the 'first message' was created as string
    @param   pBuffer [inout] - filled with date and time string in short form "YYYY-MM-DD HH:MM:SS"
    @returns Message date  in string form
    */
    SAPDB_Char const * const  DateTime( ShortTimeBuffer &pBuffer ) const;

    /*!
    @brief   Returns the date and time the 'first message' was created as string
    @param   pBuffer [inout] - filled with date and time string in long form "YYYY-MM-DD HH:MM:SS.sss"
    @returns Message date in string form
    */
    SAPDB_Char const * const  DateTime( LongTimeBuffer &pBuffer ) const;

    /*!
    @brief   Return the complete message as XML style output line

    @param   bufferSize [in] output pBuffer size
    @param   pBuffer [out] output pBuffer
    @param   neededSize [out] number of bytes used or needed
    @param   errList [out] filled in case of problem
    @return 0 if problem, else line (zero and `\n' terminated) ready for output
     */
    SAPDB_UTF8 const *XML_Output( SAPDB_UInt4 bufferSize,
                                  SAPDB_Char *pBuffer,
                                  SAPDB_UInt4 &neededSize,
                                  Msg_List &errList) const;

#ifndef NO_TEST_CODE
    /*!
    @brief   Return the 'first message' as old style output line

    Old style is i.a. used in knldiag, knldiag.err, xserver_host.prt, appldiag...

    @param   bufferSize [in] output pBuffer size
    @param   pBuffer [out] output pBuffer
    @param   neededSize [out] number of bytes used or needed
    @param   errList [out] filled in case of problem
    @return 0 if problem, else line (zero and `\n' terminated) ready for output
     */
    SAPDB_UTF8 const *Old_Output( SAPDB_UInt4 bufferSize,
                                  SAPDB_Char *pBuffer,
                                  SAPDB_UInt4 &neededSize,
                                  Msg_List &errList) const;

    /*!
    @brief   Return the 'first message' as Hexdump output

    Used for emergency diagnostics only...

    @param   bufferSize [in] output pBuffer size
    @param   pBuffer [out] output pBuffer
    @param   neededSize [out] number of bytes used or needed
    @param   errList [out] filled in case of problem
    @return 0 if problem, else line (zero and `\n' terminated) ready for output
     */
    SAPDB_Char const *HexDump_Output( SAPDB_UInt4 bufferSize,
                                      SAPDB_Char *pBuffer,
                                      SAPDB_UInt4 &neededSize,
                                      Msg_List &errList) const;

    /*!
    @brief   Return the 'first message' as Diagnostic output

    Used for diagnostics only... Shows all contained values with their internal meaning

    @param   bufferSize [in] output pBuffer size
    @param   pBuffer [out] output pBuffer
    @param   neededSize [out] number of bytes used or needed
    @param   errList [out] filled in case of problem
    @return 0 if problem, else line (zero and `\n' terminated) ready for output
     */
    SAPDB_Char const *Diagnostic_Output( SAPDB_UInt4 bufferSize,
                                         SAPDB_Char *pBuffer,
                                         SAPDB_UInt4 &neededSize,
                                         Msg_List &errList) const;
#endif /* !NO_TEST_CODE */

private:
    /*!
      @brief update the 'ignoring message' in case of message limit exceeded

      First inspect the second message in the message list for being an 'IgnoringMessage'.

      If it does not exist, it is the special case of a 'maximum of 1 message'. In this case
      the ignoring message is always added... even if the limit is exceeded... Otherwise
      the 'ignore' information would be lost.

      If it is no 'ignoring message' the second message found is replaced by an ignoring message.
      The number of messages ignored in that case is (1 + additionalMessagesToIgnore). The
      current main messages may no longer contain details or submessages. Therefore the detail
      count and the submessages count is reset if no new main message is given.

      The new main message is replacing the old main message. The details and submessages count
      of the first message is always reset if the second message must indicate ignoring of messages

      If an 'ignoring message' is found, its current number of ignored messages is retrieved and
      the additionalMessagesToIgnore is added. The result is used as parameter value for the 
      details tag. The 'usedLimit' is always used to replace the former
      'usedLimit'.

      The tags 

        SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__LIMIT

      and 

        SDBMSGTAG_MESSAGES_IGNORED_MESSAGES__ENTRY_COUNT

      are defined in SDBMsg_Messages.h. The message number Messages_IGNORED_MESSAGES is also
      found in that header file as enumeration macro.

      @param pNewMainMsg [in] new main message to replace old first message (can be 0 for adding detail)
      @param usedLimit [in] the value for message list length limit found when message list was to be added
      @param additionalMessagesToIgnore [in] number of additional messages to ignore
     */
    void UpdateIgnoringMessage( Msg_List   *pNewMainMsg
                              , SAPDB_UInt4 usedLimit
                              , SAPDB_UInt4 additionalMessagesToIgnore);

    /*!
    @brief internal used routine for message list output

    If a message is parsed, that contains a message list, this call is used recursive
     */
    void XML_ContainedMessages( SAPDB_UInt4 containedMessages
                              , SAPDB_UInt4 firstMessage
                              , SAPDB_UInt4 firstSubmessage
                              , SAPDB_UInt4 listLevel
                              , SAPDB_UInt4 &bufferSizeLeft
                              , SAPDB_Char *&pBuffer
                              , SAPDB_UInt4 &neededSize
                              , Msg_List &errList) const;

    /*!
      @brief Insert a single detail to an 'main message' in a message list

      This creates a additional message data content. The given argument pointer array may
      contain 0 pointers which have to be skipped first...

      A detail is inserted without own type and does not create a timestamp (except it is inserted
      as only content, in which case it is inserted as 'InfoMessage'...

      @param ID [in] the per component unique id
      @param Component [in] the component name string
      @param FileName [in] the source file name
      @param LineNumber [in] the source file line number
      @param Message [in] the short message text
      @param ArgCount [in] the argument count
      @param ArgArray [in] the argument pointer array
     */
    void InsertDetailOfMessageList(
              SAPDB_UInt4 const          ID,
              SAPDB_Char  const * const  Component,
              SAPDB_Char  const * const  FileName,
              SAPDB_UInt4 const          LineNumber,
              SAPDB_Char  const * const  Message,
              SAPDB_UInt4 const          ArgCount,
              Msg_IOptArg const **       ArgArray );

    /*!
      @brief fill a single message list with current creation time stamp

      This creates the message data content. The given argument pointer array may
      contain 0 pointer which have to be skipped first...

      @param Type [in] the message type (info,error,warning)
      @param Component [in] the component name string
      @param FileName [in] the source file name
      @param LineNumber [in] the source file line number
      @param ID [in] the per component unique id
      @param Message [in] the short message text
      @param ArgCount [in] the argument count
      @param ArgArray [in] the argument pointer array
     */
    void FillMessageList(   SAPDB_UInt4 const          Type,
                            SAPDB_Char  const * const  Component,
                            SAPDB_Char  const * const  FileName,
                            SAPDB_UInt4 const          LineNumber,
                            SAPDB_UInt4 const          ID,
                            SAPDB_Char  const * const  Message,
                            SAPDB_UInt4 const          ArgCount,
                            Msg_IOptArg const **       ArgArray );

    /*!
      @brief fill a single message list with known creation time stamp

      This creates the message data content. The given argument pointer array may
      contain 0 pointer which have to be skipped first...

      @param DoRegister [in] flag to indicate message registration wanted
      @param Type [in] the message type (info,error,warning)
      @param Component [in] the component name string
      @param FileName [in] the source file name
      @param LineNumber [in] the source file line number
      @param ID [in] the per component unique id
      @param DateTime [in] the timestamp when message content was created
      @param Message [in] the short message text
      @param ArgCount [in] the argument count
      @param ArgArray [in] the argument pointer array
     */
    void FillMessageList(   SAPDB_Bool  const          DoRegister,
                            SAPDB_UInt4 const          Type,
                            SAPDB_Char  const * const  Component,
                            SAPDB_Char  const * const  FileName,
                            SAPDB_UInt4 const          LineNumber,
                            SAPDB_UInt4 const          ID,
                            RTE_ISystem::DateTime const &DateTime,
                            SAPDB_Char  const * const  Message,
                            SAPDB_UInt4 const          ArgCount,
                            Msg_IOptArg const **       ArgArray );

    /*!
      @brief copy a message list
      @param MessageList [out] target to copy into
     */
    inline void CopyMessageList( Msg_List const &MessageList )
    {
        CopySelf(true, MessageList);
    }


    /*!
      @brief copy a message list
      @param DoRegister [in] register copy if true
      @param MessageList [out] target to copy into
     */
    void CopySelf( SAPDB_Bool const DoRegister,
                   Msg_List const &MessageList );

    /*!
      @brief store message content of given message into buffer
      @param pMsg [in] message to store
      @param ptr [inout] pointer inside message store buffer
      @param bytesLeft [inout] number of bytes left
      @param singleMsgSavedBytes [inout] number of bytes saved
      @param allowTruncate [in] do allow truncation
      @param errList [out] set in case of error
      @return true if store succeeded, false if failed
     */
    SAPDB_Bool StoreSingleMessage ( Msg_List const *   pMsg
                                  , SAPDB_UInt4       &bytesLeft
                                  , SAPDB_Byte *      &ptr
                                  , SAPDB_UInt4       &singleMsgSavedBytes
                                  , SAPDB_Bool  const  allowTruncate
                                  , Msg_List &         errList ) const;

    /*!
      @brief split a message list
      @return splitted messageList
     */
    Msg_List* SplitObjectList ();

    // --- counter handling
    //

    /*!
      @brief increase data reference
      if data data reference count ++
     */
    inline void AddDataRef()
    {
        if ( 0 != m_pMessageData )
		{
            m_pMessageData->DataRefCnt++;
		}
    }

    /*!
      @brief decrease data reference
      if data data reference count --
     */
    inline void         SubtractDataRef()
    {
        if ( ( 0 != m_pMessageData )
          && ( m_pMessageData->DataRefCnt > 0 ) )
		{
            m_pMessageData->DataRefCnt--;
		}
    }

    /*!
      @brief Check TRACE_MESSAGES level and if wanted dump a copy of generated message into knldiag
      If tracing is switched of this call does nothing. Else depending on message type and trace level
      a copy of the message is dumped into knldiag.
      @return none
     */
    void         TraceMessageCopy() const;

    /*!
      @brief increase object reference
      current object reference count ++
     */
    inline void         AddObjectRef()
    {
        ++m_ObjectRefCnt;
    }

    /*!
      @brief decrease object reference
      if no 0 current object reference count --
     */
    inline void         SubtractObjectRef()
    {
        if ( m_ObjectRefCnt > 0 ) --m_ObjectRefCnt;
    }

    /*!
      @brief   Empty an message list object.
     */
    void DoClear();

    Msg_RegistrySlot const        *m_pRegistrySlot;     //!< Slot where message list is registered
    SAPDB_UInt4                    m_NumOfMessages;     //!< number of messages contained in object
    SAPDB_UInt4                    m_NumOfSubMessages;  //!< number of messages contained in as message list
    SAPDB_UInt4                    m_NumOfDetails;      //!< number of messages that are details of the first message
    SAPDB_UInt4                    m_ObjectRefCnt;      //!< object reference count
    MessageData*                   m_pMessageData;      //!< binary message data ( 0 means 'no message data contained' )
    Msg_List*                      m_pNextMessage;      //!< next message

    // the following parameters are needed to idenitify the output
    SAPDB_UInt8                    m_OutputSequenceNumber; //!< the output sequence number
    RTE_ISystem::DateTime          m_OutputDateTime;    //!< the output identification time stamp
    SAPDB_UInt4                    m_OutputProcessId;   //!< the output process id
    SAPDB_UInt4                    m_OutputThreadId;    //!< the output thread id
    SAPDB_UInt4                    m_OutputTaskId;      //!< the output thread id
    SAPDB_UInt4                    m_OutputSchedulerId; //!< the output scheduler id
    SAPDB_UInt4                    m_DumpTruncated;     //!< loaded dump content was truncated
};
/*! EndClass  Msg_List */

#ifdef SAPDBERR_MESSAGELIST_STILL_SUPPORTED

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

//! backward compatibility error type string
# define MESSAGE_TYPE_ERR                        "ERROR"
//! backward compatibility error type short string
# define MESSAGE_TYPE_ERR_SHORT                  "ERR"
//! backward compatibility info type string
# define MESSAGE_TYPE_INFO                       "INFO"
//! backward compatibility info type short string
# define MESSAGE_TYPE_INFO_SHORT                 ""
//! backward compatibility warning type string
# define MESSAGE_TYPE_WRN                        "WARNING"
//! backward compatibility warning type short string
# define MESSAGE_TYPE_WRN_SHORT                  "WRN"

//! backward compatibility message list version
# define MESSAGE_LIST_VERSION                     2

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*! Chapter: Macros and defines */


/*!---------------------------------------------------------------------------
  Declaration:  __CONTEXT__
 */
#define __CONTEXT__ __FILE__,__LINE__

/*!---------------------------------------------------------------------------
  Declaration:  SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS
  Description:  Create a component specific exception class.

                This class will be derived from 'SAPDBErr_MessageList'. The constructors
                parameters are equal to the base class constructors.

  Example:
       SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( RTE );   // creates a class named 'SAPDBErr_RTE_Exception'
       SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( IO );    // creates a class named 'SAPDBErr_IO_Exception'
*/

#define SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS(_Component)                                \
          class _Component##_Exception : public SAPDBErr_MessageList                         \
          {                                                                                  \
          public:                                                                            \
              _Component##_Exception() : SAPDBErr_MessageList () {;}                         \
                                                                                             \
              _Component##_Exception( SAPDB_Char const * const   FileName,                   \
                                      SAPDB_UInt4 const          LineNumber,                 \
                                      MessageType const          Type,                       \
                                      MessageID const            ID,                         \
                                      SAPDB_Char const * const   Message,                    \
                                      SAPDB_UInt4 const          NumOfArgs,                  \
                                      SAPDB_Char const * const   Arg0 = NULL,                \
                                      SAPDB_Char const * const   Arg1 = NULL,                \
                                      SAPDB_Char const * const   Arg2 = NULL,                \
                                      SAPDB_Char const * const   Arg3 = NULL,                \
                                      SAPDB_Char const * const   Arg4 = NULL,                \
                                      SAPDB_Char const * const   Arg5 = NULL,                \
                                      SAPDB_Char const * const   Arg6 = NULL,                \
                                      SAPDB_Char const * const   Arg7 = NULL,                \
                                      SAPDB_Char const * const   Arg8 = NULL,                \
                                      SAPDB_Char const * const   Arg9 = NULL ) :             \
                                        SAPDBErr_MessageList ( #_Component, FileName,        \
                                                               LineNumber, Type,             \
                                                               ID, Message, NumOfArgs,       \
                                                               Arg0, Arg1, Arg2, Arg3, Arg4, \
                                                               Arg5, Arg6, Arg7, Arg8, Arg9 )\
                                                               {;}                           \
                                                                                             \
              _Component##_Exception( _Component##_Exception const &MessageList )            \
                                    : SAPDBErr_MessageList( MessageList )  {;}               \
                                                                                             \
              virtual ~_Component##_Exception() {;};                                         \
                                                                                             \
              _Component##_Exception operator + ( const SAPDBErr_MessageList& List ) const    \
                                       {  _Component##_Exception     Result   = *this;       \
                                          Result.AppendNewMessage ( List );                  \
                                          return Result;                                     \
                                       }                                                     \
          };


/*! EndDeclaration: SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS */

/*!---------------------------------------------------------------------------
  Declaration:  SAPDBERR_DEFINE_COMPONENT_MESSAGELIST_CLASS
  Description:  Create a component specific messagelist class.

                This class will be derived from 'SAPDBErr_MessageList'. The constructors
                parameters are equal to the base class constructors.

  Example:
       SAPDBERR_DEFINE_COMPONENT_MESSAGELIST_CLASS( RTE );   // creates a class named 'SAPDBErr_RTE_MessageList'
       SAPDBERR_DEFINE_COMPONENT_MESSAGELIST_CLASS( IO );    // creates a class named 'SAPDBErr_IO_MessageList'
*/

#define SAPDBERR_DEFINE_COMPONENT_MESSAGELIST_CLASS(_Component)                              \
          class _Component##_MessageList : public SAPDBErr_MessageList                       \
          {                                                                                  \
          public:                                                                            \
              _Component##_MessageList() : SAPDBErr_MessageList () {;}                       \
                                                                                             \
              _Component##_MessageList( SAPDB_Char const * const   FileName,                 \
                                      SAPDB_UInt4 const          LineNumber,                 \
                                      MessageType const          Type,                       \
                                      MessageID const            ID,                         \
                                      SAPDB_Char const * const   Message,                    \
                                      SAPDB_UInt4 const          NumOfArgs,                  \
                                      SAPDB_Char const * const   Arg0 = NULL,                \
                                      SAPDB_Char const * const   Arg1 = NULL,                \
                                      SAPDB_Char const * const   Arg2 = NULL,                \
                                      SAPDB_Char const * const   Arg3 = NULL,                \
                                      SAPDB_Char const * const   Arg4 = NULL,                \
                                      SAPDB_Char const * const   Arg5 = NULL,                \
                                      SAPDB_Char const * const   Arg6 = NULL,                \
                                      SAPDB_Char const * const   Arg7 = NULL,                \
                                      SAPDB_Char const * const   Arg8 = NULL,                \
                                      SAPDB_Char const * const   Arg9 = NULL ) :             \
                                        SAPDBErr_MessageList ( #_Component, FileName,        \
                                                               LineNumber, Type,             \
                                                               ID, Message, NumOfArgs,       \
                                                               Arg0, Arg1, Arg2, Arg3, Arg4, \
                                                               Arg5, Arg6, Arg7, Arg8, Arg9 )\
                                                               {;}                           \
                                                                                             \
              _Component##_MessageList( _Component##_MessageList const &MessageList )        \
                                    : SAPDBErr_MessageList( MessageList )  {;}               \
                                                                                             \
              virtual ~_Component##_MessageList() {;};                                       \
                                                                                             \
              _Component##_MessageList operator + ( const SAPDBErr_MessageList& List ) const  \
                                       {  _Component##_MessageList     Result   = *this;     \
                                          Result.AppendNewMessage ( List );                  \
                                          return Result;                                     \
                                       }                                                     \
          };


/*! EndDeclaration: SAPDBERR_DEFINE_COMPONENT_MESSAGELIST_CLASS */

#endif /* SAPDBERR_MESSAGELIST_STILL_SUPPORTED */

#endif  /* MSG_LIST_HPP */
