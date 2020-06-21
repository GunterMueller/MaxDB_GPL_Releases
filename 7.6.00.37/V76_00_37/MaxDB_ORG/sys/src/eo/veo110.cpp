/*!
  @file           veo110.cpp
  @author         RaymondR
  @special area   parent/child pipe communication.
  @brief          This Classes are used to build up parent/child
              proccess or thread communication via pipes.
  @see            


                There are two main classes, one for the
                parent side (teo110_ParentPipe) and one for the
                child (teo110_ChildPipe).

\if EMIT_LICENCE

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



\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo60.h"
#include "geo110.h"
#include "gsp00.h"
#include "geo007_1.h"

#include <ctype.h>

#if defined(_WIN32)

# include <crtdbg.h>

#else /* UNIX */

# include <fcntl.h>
# include <assert.h>

# if defined(_DEBUG)
#  define _ASSERT      assert
# else
#  define _ASSERT(_assertion)
# endif

#endif

#if 0
// keep around for debugging
static char hexDigitsC [] = "0123456789abcdef";

static void
dumpHex (const void * bufParam, int len)
{
    const unsigned char * buf = (unsigned char *) bufParam;
    int offset = 0;
    char line [81];
    char * cursor;
    int i;

    while (offset < len) {
        /*
         * init
         */
        int charsInChunk = 16;
        if (len - offset < 16) {
            charsInChunk = len - offset;
        }
        memset (line, ' ', sizeof (line));
        line [80] = '\0';
        cursor = line;
        /*
         * offset
         */
        sprintf (cursor, "%04d  ", offset);
        cursor += 6;
        /*
         * hex
         */
        for (i = 0; i < charsInChunk; ++i) {
            int high = buf [offset + i] / 16;
            int low  = buf [offset + i] % 16;
            cursor [0] = hexDigitsC [high];
            cursor [1] = hexDigitsC [low];
            cursor += 3;
            switch (i) {
                case 3:
                case 7:
                case 11:
                    cursor [0] = '-';
                    cursor += 2;
                    break;
                default:
                    break;
            }
        }
        /*
         * chars
         */
        cursor = line + 65;
        cursor [0] = '|';
        ++cursor;
        for (i = 0; i < charsInChunk; ++i) {
            if (buf [offset + i] < 32) {
                cursor [i] = '.';
            }
            else {
                cursor [i] = buf [offset + i];
            }
        }
        cursor += charsInChunk;
        cursor [0] = '|';
        ++cursor;
        /*
         * output
         */
        cursor [0] = '\0';
        fprintf (serverTrace, "%s\n", line);
        offset += charsInChunk;
    }
}
#endif
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
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/




/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           class teo110_PipeDesc                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

void teo110_PipeDesc::eo110_PipeDescToASCII ( teo110_ASCIIPipeDesc         &ASCIIPipeDesc ) const
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeDesc::eo110_PipeDescToASCII"));

  tsp00_Uint4    SpacePerHandle = (sizeof(teo110_PipeHdl) * 2);
  char*          pASCIIPipeDesc = &ASCIIPipeDesc[0];

  // --- convert 'm_ReadPipe'
  eo200_PipeHdlToASCII ( m_ReadPipe, pASCIIPipeDesc );

  pASCIIPipeDesc += SpacePerHandle;

  // --- convert 'm_WritePipe'
  eo200_PipeHdlToASCII ( m_WritePipe, pASCIIPipeDesc );

  pASCIIPipeDesc += SpacePerHandle;

  // --- convert 'm_ShortMessagePipe'
  eo200_PipeHdlToASCII ( m_ReadAsyncReqRespPipe, pASCIIPipeDesc );

  pASCIIPipeDesc += SpacePerHandle;

  // --- convert 'm_ShortMessagePipe'
  eo200_PipeHdlToASCII ( m_WriteAsyncReqRespPipe, pASCIIPipeDesc );

  // add zero-terminator
  ASCIIPipeDesc[sizeof(teo110_ASCIIPipeDesc) - 1] = '\0';
}

/*---------------------------------------------------------------------------*/

void teo110_PipeDesc::eo110_ASCIIToPipeDesc ( teo110_ASCIIPipeDesc const   ASCIIPipeDesc )
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeDesc::eo110_ASCIIToPipeDesc"));

  tsp00_Uint4    SpacePerHandle = (sizeof(teo110_PipeHdl) * 2);
  char const *   pASCIIPipeDesc = &ASCIIPipeDesc[0];

  // --- convert ASCII string to pipe handles
  m_ReadPipe             = eo200_ASCIIToPipeHdl ( pASCIIPipeDesc );

  pASCIIPipeDesc        += SpacePerHandle;
  m_WritePipe            = eo200_ASCIIToPipeHdl ( pASCIIPipeDesc );

  pASCIIPipeDesc        += SpacePerHandle;
  m_ReadAsyncReqRespPipe = eo200_ASCIIToPipeHdl ( pASCIIPipeDesc );

  pASCIIPipeDesc         += SpacePerHandle;
  m_WriteAsyncReqRespPipe = eo200_ASCIIToPipeHdl ( pASCIIPipeDesc );
}




/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           class teo110_PipeBase                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


bool teo110_PipeBase::eo110_WritePipe  ( teo110_PipeHdl const          PipeHandle,
                                         void const * const            SendBuffer,
                                         size_t const                  SendLen,
                                         size_t                        &BytesSend,
                                         teo200_EventList * const      pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeBase::eo110_WritePipe"));
  // dumpHex (SendBuffer, SendLen);

  teo001_SystemRc  rc         = NO_ERROR_EO001;

#   if defined(_WIN32)

     DWORD dwBytesSend;

     if ( 0 == WriteFile( PipeHandle, SendBuffer, (DWORD const)SendLen, &dwBytesSend, NULL ) )
     {
       rc        = GetLastError();
       BytesSend = 0;
     }
     else
       BytesSend = dwBytesSend;

#   else  /* UNIX */

    int   Result;

     do
     {
       Result = write ( PipeHandle, (char*) SendBuffer, SendLen );
     }
     while ( 0 > Result && EINTR == errno ); // PTS 1105497

     if ( 0 > Result )
       rc        = errno;
     else
       BytesSend = Result;

#   endif /* _WIN32/UNIX */

  if ( rc != NO_ERROR_EO001 )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_WRITE_PIPE, FUNCTION_NAME_MEO60, rc );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return ( NO_ERROR_EO001 == rc );
  }

  return ( rc == NO_ERROR_EO001 );
}

/*---------------------------------------------------------------------------*/

extern "C" int system (const char *);

bool teo110_PipeBase::eo110_ReadPipe  ( teo110_PipeHdl const          PipeHandle,
                                        void * const                  RecvBuffer,
                                        size_t const                  RecvBufferSize,
                                        size_t                        &BytesReceived,
                                        teo200_EventList * const      pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeBase::eo110_ReadPipe"));

  teo001_SystemRc  rc = NO_ERROR_EO001;

# if defined(_WIN32)

  DWORD dwBytesReceived;

  if ( 0 == ReadFile( PipeHandle, RecvBuffer, (DWORD const)RecvBufferSize, &dwBytesReceived, NULL ) )
  {
    rc            = GetLastError();
    BytesReceived = 0;
  }
  else
    BytesReceived = dwBytesReceived;

# else  /* UNIX */

   int  Result;

   do
   {
     Result = read ( PipeHandle, (char *) RecvBuffer, RecvBufferSize);
   }
   while ( 0 > Result && EINTR == errno ); // PTS 1105497

   if ( 0 > Result )
     rc             = errno;
   else
     BytesReceived = Result;

# endif /* _WIN32/UNIX */

  if ( rc != NO_ERROR_EO001 )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_READ_PIPE, FUNCTION_NAME_MEO60, rc );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
  }

  //if (rc == NO_ERROR_EO001) {
  //    dumpHex (RecvBuffer, BytesReceived);
  //}
  return ( rc == NO_ERROR_EO001 );
}

/*---------------------------------------------------------------------------*/

bool  teo110_PipeBase::eo110_CreatePipe  ( teo110_PipeHdl * const    ReadPipe,
                                           bool const                InheritReadPipe,
                                           teo110_PipeHdl * const    WritePipe,
                                           bool const                InheritWritePipe,
                                           teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeBase::teo110_CreatePipe"));

  teo001_SystemRc  rc;

# if defined(_WIN32)

   HANDLE              NewHdl;

   if ( ::CreatePipe (ReadPipe, WritePipe, NULL, 0) == 0 )
     rc = GetLastError();
   else
   {
     rc = NO_ERROR_EO001;

     if ( InheritReadPipe )
     { // --- make the read pipe inheritable
       if ( ::DuplicateHandle( GetCurrentProcess(), *ReadPipe, GetCurrentProcess(),
                               &NewHdl, 0, TRUE, DUPLICATE_SAME_ACCESS ) )
       {
         CloseHandle( *ReadPipe );
         _ASSERT(GetLastError() == NO_ERROR);
         *ReadPipe = NewHdl;
       }
       else
         rc = GetLastError();
     }
     else if ( InheritWritePipe )
     { // --- make the write pipe inheritable
       if ( ::DuplicateHandle( GetCurrentProcess(), *WritePipe, GetCurrentProcess(),
                               &NewHdl, 0, TRUE, DUPLICATE_SAME_ACCESS ) )
       {
         CloseHandle( *WritePipe );
         _ASSERT(GetLastError() == NO_ERROR);
         *WritePipe = NewHdl;
       }
       else
         rc = GetLastError();
     }
   }
# else  /* UNIX */

   teo110_PipeHdl Hdl[2];

   if ( -1 == pipe( Hdl ) )
     rc = errno;
   else
   {
     rc = NO_ERROR_EO001;

     *ReadPipe  = Hdl[0];
     *WritePipe = Hdl[1];

     if ( InheritReadPipe == false )
        eo110_CloseOnExec ( *ReadPipe );
     else if ( InheritWritePipe == false )
        eo110_CloseOnExec ( *WritePipe );
   }

# endif /* _WIN32/UNIX */

  if ( rc != NO_ERROR_EO001 )
   {
     *ReadPipe  = INVALID_PIPE_HANDLE_EO110;
     *WritePipe = INVALID_PIPE_HANDLE_EO110;

     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CREATE_PIPE, FUNCTION_NAME_MEO60, rc );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
   }

  return ( rc == NO_ERROR_EO001 );
}

/*---------------------------------------------------------------------------*/

void teo110_PipeBase::eo110_ClosePipeHdl ( teo110_PipeHdl            &Pipe )
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeBase::eo110_ClosePipeHdl"));

  if ( Pipe != INVALID_PIPE_HANDLE_EO110 )
  {
#   if defined(_WIN32)
     CloseHandle ( Pipe );
     _ASSERT(GetLastError() == NO_ERROR);
#   else  /* UNIX */
     close ( Pipe );
#   endif /* _WIN32/UNIX */

    Pipe = INVALID_PIPE_HANDLE_EO110;
  }
}

/*---------------------------------------------------------------------------*/

int teo110_PipeBase::eo110_CloseOnExec ( teo110_PipeHdl Pipe )
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeBase::eo110_CloseOnExec"));
	int rc = 0;

# if defined(_WIN32)
#else
 	 int fd_flag;

	 rc = fcntl(Pipe, F_GETFD, &fd_flag);

	 if ( rc == 0 )
	 	 rc = fcntl(Pipe, F_SETFD, fd_flag|FD_CLOEXEC);
# endif

  return rc;
}






/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class teo110_ChildPipe                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


teo110_ChildPipe::teo110_ChildPipe ()
{
  ROUTINE_DBG_MEO60 (_T("teo110_ChildPipe::teo110_ChildPipe"));
}

/*---------------------------------------------------------------------------*/

teo110_ChildPipe::~teo110_ChildPipe ()
{
  ROUTINE_DBG_MEO60 (_T("teo110_ChildPipe::~teo110_ChildPipe"));

  eo110_CloseData  ();
  eo110_CloseAsync ();
}

/*---------------------------------------------------------------------------*/

void teo110_ChildPipe::eo110_Initialize ( teo110_PipeDesc const       PipeDesc )
{
  ROUTINE_DBG_MEO60 (_T("teo110_ChildPipe::eo110_Initialize"));

  m_PipeDesc = PipeDesc;
}

/*---------------------------------------------------------------------------*/

void teo110_ChildPipe::eo110_CloseData ()
{
  ROUTINE_DBG_MEO60 (_T("teo110_ChildPipe::eo110_CloseDataPipe"));

  eo110_ClosePipeHdl( m_PipeDesc.m_ReadPipe );
  eo110_ClosePipeHdl( m_PipeDesc.m_WritePipe );
}

/*---------------------------------------------------------------------------*/

void teo110_ChildPipe::eo110_CloseAsync ()
{
  ROUTINE_DBG_MEO60 (_T("teo110_ChildPipe::eo110_CloseAsyncPipe"));

  eo110_ClosePipeHdl( m_PipeDesc.m_ReadAsyncReqRespPipe );
  eo110_ClosePipeHdl( m_PipeDesc.m_WriteAsyncReqRespPipe );
}



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class teo110_ParentPipe                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


bool teo110_ParentPipe::eo110_Create  ( teo110_PipeDesc           &ChildPipeDesc,
                                        bool const                Inheritable,
                                        teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo110_ParentPipe::eo110_Create"));

  // --- try close old!
  eo110_CloseAllPipes();

  if (( eo110_CreatePipe( &m_ChildPipeDesc.m_ReadPipe,             Inheritable,
                          &m_PipeDesc.m_WritePipe,                 false, pEventList )       == false ) ||
      ( eo110_CreatePipe( &m_PipeDesc.m_ReadPipe,                  false,
                          &m_ChildPipeDesc.m_WritePipe,            Inheritable, pEventList ) == false ) ||
      ( eo110_CreatePipe( &m_ChildPipeDesc.m_ReadAsyncReqRespPipe, Inheritable,
                          &m_PipeDesc.m_WriteAsyncReqRespPipe,     false, pEventList )       == false ) ||
      ( eo110_CreatePipe( &m_PipeDesc.m_ReadAsyncReqRespPipe,      false,
                          &m_ChildPipeDesc.m_WriteAsyncReqRespPipe,Inheritable, pEventList ) == false ))
  {
    eo110_CloseAllPipes();
    return false;
  }

  ChildPipeDesc = m_ChildPipeDesc;

  return true;
}

/*---------------------------------------------------------------------------*/

void teo110_ParentPipe::eo110_ReleaseChildPipeHandles  ()
{
  ROUTINE_DBG_MEO60 (_T("teo110_ParentPipe::eo110_ReleaseChildPipeHandles"));

  eo110_ClosePipeHdl ( m_ChildPipeDesc.m_ReadPipe );
  eo110_ClosePipeHdl ( m_ChildPipeDesc.m_WritePipe );
  eo110_ClosePipeHdl ( m_ChildPipeDesc.m_ReadAsyncReqRespPipe );
  eo110_ClosePipeHdl ( m_ChildPipeDesc.m_WriteAsyncReqRespPipe );

  return;
}

/*---------------------------------------------------------------------------*/

void teo110_ParentPipe::eo110_CloseAllPipes  ()
{
  ROUTINE_DBG_MEO60 (_T("teo110_ParentPipe::eo110_CloseAllPipes"));

  // --- close child pipe handles
  eo110_ReleaseChildPipeHandles();

  eo110_CloseData  ();
  eo110_CloseAsync ();

  return;
}


/*---------------------------------------------------------------------------*/

void teo110_ParentPipe::eo110_CloseData ()
{
  ROUTINE_DBG_MEO60 (_T("teo110_ParentPipe::eo110_CloseDataPipe"));

  eo110_ClosePipeHdl ( m_PipeDesc.m_WritePipe );
  eo110_ClosePipeHdl ( m_PipeDesc.m_ReadPipe );
}

/*---------------------------------------------------------------------------*/

void teo110_ParentPipe::eo110_CloseAsync ()
{
  ROUTINE_DBG_MEO60 (_T("teo110_ParentPipe::eo110_CloseAsyncPipe"));

  eo110_ClosePipeHdl ( m_PipeDesc.m_WriteAsyncReqRespPipe );
  eo110_ClosePipeHdl ( m_PipeDesc.m_ReadAsyncReqRespPipe );
}

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           class teo110_PipeDesc                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


void  teo110_PipeDesc::eo200_PipeHdlToASCII ( teo110_PipeHdl const    PipeHdl,
                                              char * const            ASCIIPipeHdl ) const
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeDesc::eo200_PipeHdlToASCII"));

  char const  * const HexASCIIMap = "0123456789ABCDEF";

  int             i;
  tsp00_Longuint  HdlVal = (tsp00_Longuint)PipeHdl;

  for ( i = (sizeof(teo110_PipeHdl) * 2); i > 0; i-- )
  {
    ASCIIPipeHdl[i-1] = HexASCIIMap[HdlVal % 0x10];
    HdlVal /= 0x10;
  }

}

/*---------------------------------------------------------------------------*/

teo110_PipeHdl  teo110_PipeDesc::eo200_ASCIIToPipeHdl ( char const * const ASCIIPipeHdl )
{
  ROUTINE_DBG_MEO60 (_T("teo110_PipeDesc::eo200_ASCIIToPipeHdl"));

  int             Pos;
  char            Ch;
  tsp00_Longuint  HdlVal = 0;


  for ( Pos = 0; Pos < (sizeof(teo110_PipeHdl) * 2); Pos++ )
  {
    HdlVal *= 0x10;
    Ch      = ASCIIPipeHdl[Pos];

    if ( ('0' <= Ch) && ('9' >= Ch) )
      HdlVal += Ch - '0';
    else if (( ('A' <= Ch) && ('F' >= Ch) ) || ( ('a' <= Ch) && ('f' >= Ch) ))
      HdlVal += toupper(Ch) - 'A' + 10;
    else
    {
      HdlVal = 0;
      break;
    }
  }

  return (teo110_PipeHdl)HdlVal;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/