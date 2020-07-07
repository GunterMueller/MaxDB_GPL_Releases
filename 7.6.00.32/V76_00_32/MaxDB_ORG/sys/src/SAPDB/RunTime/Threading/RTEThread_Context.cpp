/****************************************************************************

  module      : RTEThread_Context.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Multithreading Thread Context
  description : Common class used to access thread context

  last changed: 2000-07-26  18:14
 first created: 2000-02-28  14:45

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(WIN32)
#include <windows.h>
#include <limits.h>
#else
#if defined(PA11) || defined(PA20W)
#define _POSIX_C_SOURCE 199506L
#endif /* PA11 || PA20W */
#include <pthread.h>
#endif

#include "RunTime/Threading/RTEThread_Context.hpp"
#include "RunTime/Tasking/RTETask_IContext.hpp"
#include "RunTime/RTE_IGlobal.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/RTE_MessageList.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifndef WIN32
#define PTHREAD_API_CALL(_rc,_x) (_rc) = (_x)
#define PTHREAD_MUTEX_INIT(_pmutex) pthread_mutex_init(_pmutex, NULL)
#define PTHREAD_COND_INIT(_pcond) pthread_cond_init (_pcond, NULL)
#endif /* !WIN32 */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTEThread_Context::RTEThread_Context()
{
}

/*---------------------------------------------------------------------------*/

RTEThread_Context::~RTEThread_Context()
{
}

/*---------------------------------------------------------------------------*/

RTETask_IContext *
RTEThread_Context::CurrentTaskContext()
{
  return m_CurrentTaskContext;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEThread_IContext::CreateTLSKey(RTE_TLSKey &Key, void (*Destructor)(void *) )
{
  RTE_SystemRc LastError;

#if defined(WIN32)

  Key = TlsAlloc();
  LastError = ( Key == 0xFFFFFFFF ? GetLastError() : RTE_ISystem::NoError );

#else

  PTHREAD_API_CALL(LastError,pthread_key_create((pthread_key_t*)&Key, NULL));

#endif

  if ( LastError != RTE_ISystem::NoError )
  {
    RTE_IGlobal::Instance().SetLastErrorMessage(
        SAPDBErr_MessageList( RTE_CONTEXT, LastError, RTEERR_TLS_KEY_CREATE ), 
            false);
  }

  return (LastError != RTE_ISystem::NoError);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEThread_IContext::DeleteTLSKey( RTE_TLSKey Key )
{
  RTE_SystemRc LastError;
  SAPDB_UInt4 keyVal;

#if defined(WIN32)

  LastError = ( TlsFree((DWORD)Key) ? RTE_ISystem::NoError : GetLastError() );
  keyVal = (SAPDB_UInt4)Key;
#else

  PTHREAD_API_CALL(LastError,pthread_key_delete((pthread_key_t)Key));
  keyVal = (SAPDB_UInt4)Key;

#endif

  if ( LastError != RTE_ISystem::NoError )
  {
    RTE_IGlobal::Instance().SetLastErrorMessage(
        SAPDBErr_MessageList( RTE_CONTEXT, LastError, RTEERR_TLS_KEY_DELETE, ToStr(keyVal) ),
            false);
  }

  return ( LastError != RTE_ISystem::NoError );
}

/*---------------------------------------------------------------------------*/

void * RTEThread_IContext::GetTLS(       RTE_TLSKey Key )
{
  RTE_SystemRc LastError;
  SAPDB_UInt4 keyVal;
  void *Value;

#if defined(WIN32)

  Value = TlsGetValue((DWORD)Key);

  LastError = ( Value != NULL ? RTE_ISystem::NoError : GetLastError() );
  keyVal = (SAPDB_UInt4)Key;

#else 

  /* pthread_getspecific() does not return errors !! */
  LastError = RTE_ISystem::NoError;
  Value = pthread_getspecific((pthread_key_t)Key); 
  keyVal = (SAPDB_UInt4)Key;

#endif /* WIN32 */

  if ( LastError != RTE_ISystem::NoError )
  {
    RTE_IGlobal::Instance().SetLastErrorMessage(
        SAPDBErr_MessageList( RTE_CONTEXT, LastError, RTEERR_TLS_GET, ToStr(keyVal) ),
            false);
  }
  SAPDBERR_ASSERT_STATE(LastError != RTE_ISystem::NoError);

  return Value;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEThread_IContext::SetTLS( RTE_TLSKey Key, void *Value )
{
  RTE_SystemRc LastError;
  SAPDB_UInt4 keyVal;

#if defined(WIN32)

  LastError = ( TlsSetValue((DWORD)Key, Value) ? RTE_ISystem::NoError : GetLastError() );
  keyVal = (SAPDB_UInt4)Key;
  
#else 

  PTHREAD_API_CALL(LastError, pthread_setspecific((pthread_key_t)Key, Value));
  keyVal = (SAPDB_UInt4)Key;

#endif /* WIN32 */

  if ( LastError != RTE_ISystem::NoError )
  {
    RTE_IGlobal::Instance().SetLastErrorMessage(
        SAPDBErr_MessageList( RTE_CONTEXT, LastError, RTEERR_TLS_SET, ToStr(keyVal), ToStr(Value) ),
            false);
  }

  return ( LastError != RTE_ISystem::NoError );
}

/*---------------------------------------------------------------------------*/

const SAPDBErr_MessageList & RTEThread_Context::GetLastErrorMessage()
{
  return m_LastErrorMessage;
}

/*---------------------------------------------------------------------------*/

void RTEThread_Context::SetLastErrorMessage(const SAPDBErr_MessageList &LastErrorMessage, SAPDB_Bool DoAppend)
{
  if ( DoAppend )
  {
    m_LastErrorMessage.AppendNewMessage(LastErrorMessage);
  }
  else
  {
    m_LastErrorMessage = LastErrorMessage;
  }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
