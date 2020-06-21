/*!
  @file           geo110.h
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



#ifndef GEO110_H
#define GEO110_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(_WIN32)
# include <windows.h>
#else  /* UNIX */
# include <unistd.h>
#endif /* _WIN32/UNIX */

#include "geo42.h"
#include "geo200.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*-----------------------------------------------*/
/*------------- forward declartions -------------*/
/*-----------------------------------------------*/

class teo110_PipeBase;
class teo110_ChildPipe;
class teo110_ParentPipe;

/*---------------------------------------------------------------------------*/
/*------------------------- typedef teo110_PipeHdl --------------------------*/
/*---------------------------------------------------------------------------*/

# if defined(_WIN32)
   typedef  HANDLE     teo110_PipeHdl;
# else  /* UNIX */
   typedef  int        teo110_PipeHdl;
# endif /* _WIN32/UNIX */

# if defined(_WIN32)
#  define INVALID_PIPE_HANDLE_EO110     (teo110_PipeHdl)INVALID_HANDLE_VALUE
# else  /* UNIX */
#  define INVALID_PIPE_HANDLE_EO110     (teo110_PipeHdl)-1
# endif 



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           class teo110_PipeDesc                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo110_PipeDesc
   @Description    Pipe descriptor.


                A pipe descriptor is an opaque class that contains the pipe handles 
                associated with a object.

 */

class teo110_PipeDesc
{
public:
  typedef char      teo110_ASCIIPipeDesc[ (sizeof(teo110_PipeHdl) * 2) +      // m_ReadPipe
                                          (sizeof(teo110_PipeHdl) * 2) +      // m_WritePipe
                                          (sizeof(teo110_PipeHdl) * 2) +      // m_ReadAsyncReqRespPipe
                                          (sizeof(teo110_PipeHdl) * 2) + 1];  // m_WriteAsyncReqRespPipe

  /*!
     @Description    Convert pipe descriptor to ASCII pipe descriptor string.


                The function 'eo110_PipeDescToASCII' converts a pipe descriptor 
                to an ASCII string. This is useful to pass a pipe descriptor 
                to the client process via programm arguments.
     @param          ASCIIPipeDesc [out] - ASCII pipe descriptor.
     @Return value   none

   */

  void              eo110_PipeDescToASCII  ( teo110_ASCIIPipeDesc         &ASCIIPipeDesc ) const;

  /*!
     @Description    Convert ASCII pipe descriptor string to pipe descriptor.


                This function creates pipe descriptor based on the specified 
                ASCII pipe descriptor string. 
     @param          ASCIIPipeDesc [in] - ASCII pipe descriptor.
     @Return value   none

   */

  void              eo110_ASCIIToPipeDesc  ( teo110_ASCIIPipeDesc const   ASCIIPipeDesc );

  teo110_PipeHdl&   eo110_ReadPipe                   () { return m_ReadPipe; };
  teo110_PipeHdl&   eo110_WritePipe                  () { return m_WritePipe; };
  teo110_PipeHdl&   eo110_ReadAsyncReqRespPipe       () { return m_ReadAsyncReqRespPipe; };
  teo110_PipeHdl&   eo110_WriteAsyncReqRespPipe      () { return m_WriteAsyncReqRespPipe; };

public:
  teo110_PipeDesc        () : m_WriteAsyncReqRespPipe(INVALID_PIPE_HANDLE_EO110), 
                              m_ReadAsyncReqRespPipe (INVALID_PIPE_HANDLE_EO110),  
                              m_ReadPipe             (INVALID_PIPE_HANDLE_EO110),          
                              m_WritePipe            (INVALID_PIPE_HANDLE_EO110)  {;};

private:
  void              eo200_PipeHdlToASCII   ( teo110_PipeHdl const         PipeHdl,
                                             char * const                 ASCIIPipeHdl ) const;
  teo110_PipeHdl    eo200_ASCIIToPipeHdl   ( char const * const           ASCIIPipeHdl );

private:
  friend class teo110_PipeBase;       // - necessary for direct access to m_ReadPipe,
  friend class teo110_ChildPipe;      //   m_WritePipe and m_ShortMessagePipe
  friend class teo110_ParentPipe;     //

  teo110_PipeHdl    m_ReadPipe;
  teo110_PipeHdl    m_WritePipe;
  teo110_PipeHdl    m_ReadAsyncReqRespPipe;
  teo110_PipeHdl    m_WriteAsyncReqRespPipe;
};





/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                           class teo110_PipeBase                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo110_PipeBase
   @Description    Pipe base class.


                This pipe class provides the send and receive functionality.
  The class is derived from teo42_TransferUnitBase.

 */

class teo110_PipeBase : public teo42_TransferUnitBase
{
public:
                    teo110_PipeBase()  {;};
  virtual           ~teo110_PipeBase() {;};

  /*!
     @Description    Send data.


                This function sends data on a pipe.
     @param          SendBuffer [in] - A buffer containing the data to be transmitted.
     @param          SendLen [in] - The length of the data in SendBuffer.
     @param          BytesSend [out] - Number of bytes send.
     @param          pEventList [inout] - The event list to be used,                                       if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  virtual bool      eo42_RawSend          ( void const * const            SendBuffer,
                                            size_t const                  SendLen,
                                            size_t                        &BytesSend,
                                            teo200_EventList * const      pEventList = NULL )
                                          {
                                            return  eo110_WritePipe ( m_PipeDesc.m_WritePipe, SendBuffer, 
                                                                      SendLen, BytesSend, pEventList );
                                          };

  /*!
     @Description    Receive data.


                This function receives data from a pipe.
     @param          RecvBuffer [inout] - A buffer for the incoming data.              RecvBufferSize[in]      - The size of RecvBuffer.
     @param          BytesReceived [out] - Number of bytes received.
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  virtual bool      eo42_RawReceive       ( void * const                  RecvBuffer,
                                            size_t const                  RecvBufferSize,
                                            size_t                        &BytesReceived,
                                            teo200_EventList * const      pEventList = NULL )
                                          {
                                            return eo110_ReadPipe ( m_PipeDesc.m_ReadPipe, RecvBuffer, 
                                                                    RecvBufferSize, BytesReceived, pEventList );
                                          };
 
  /*!
     @Description    Sends a request/response to a specific destination asyncronously to
              'eo42_RawSend'. The request/response can only be received with 
              'eo42_RecvAsyncReqResp'. The methode is usefull for cancel, 
              kill and dump resquest implementations.
     @param          SendBuffer [in] - A buffer containing the data to be transmitted.
     @param          SendLen [in] - The length of the data in SendBuffer.
     @param          BytesSend [out] - Number of bytes send.
     @param          pEventList [inout] - The event list to be used,                                       if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  virtual bool      eo42_SendAsyncReqResp ( void const * const            SendBuffer,
                                            size_t const                  SendLen,
                                            size_t                        &BytesSend,
                                            teo200_EventList * const      pEventList = NULL )
                                          {
                                            return  eo110_WritePipe ( m_PipeDesc.m_WriteAsyncReqRespPipe, 
                                                                      SendBuffer, SendLen, BytesSend, 
                                                                      pEventList );
                                          };

  /*!
     @Description    Receives a request/response from a specific destination, which
              was send with 'eo42_SendAsyncReqResp'.
     @param          RecvBuffer [inout] - A buffer for the incoming data.              RecvBufferSize[in]      - The size of RecvBuffer.
     @param          BytesReceived [out] - Number of bytes received.
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  virtual bool      eo42_RecvAsyncReqResp ( void * const                  RecvBuffer,
                                            size_t const                  RecvBufferSize,
                                            size_t                        &BytesReceived,
                                            teo200_EventList * const      pEventList = NULL )
                                          {
                                            return eo110_ReadPipe ( m_PipeDesc.m_ReadAsyncReqRespPipe, RecvBuffer,
                                                                    RecvBufferSize, BytesReceived, pEventList );
                                          };

  teo110_PipeDesc&  eo110_PipeDesc       () { return m_PipeDesc; }

protected:
  teo110_PipeDesc   m_PipeDesc;

protected:
  bool              eo110_CreatePipe      ( teo110_PipeHdl * const        ReadPipe,
                                            bool const                    InheritReadPipe,
                                            teo110_PipeHdl * const        WritePipe,
                                            bool const                    InheritWritePipe,
                                            teo200_EventList * const      pEventList );

  bool              eo110_WritePipe       ( teo110_PipeHdl const          PipeHandle,
                                            void const * const            SendBuffer,
                                            size_t const                  SendLen,
                                            size_t                        &BytesSend,
                                            teo200_EventList * const      pEventList = NULL );
  bool              eo110_ReadPipe        ( teo110_PipeHdl const          PipeHandle,
                                            void * const                  RecvBuffer,
                                            size_t const                  RecvBufferSize,
                                            size_t                        &BytesReceived,
                                            teo200_EventList * const      pEventList = NULL );
  void              eo110_ClosePipeHdl    ( teo110_PipeHdl                &Pipe );

  int               eo110_CloseOnExec     ( teo110_PipeHdl                Pipe );

};



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class teo110_ParentPipe                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo110_ParentPipe
   @Description    Parent pipe class.


                This pipe class creates the communication pipes for bidirectional 
                data transmission. teo110_ParentPipe is derived from teo110_PipeBase,
                so it provides send and receive functionality via teo110_Send and 
                teo110_Receive.
                The ChildPipeDesc variable return by the constructor is used as input 
                of the teo110_ChildPipe::eo110_Initialize function.

 */

class teo110_ParentPipe : public teo110_PipeBase
{
public:
  /*!
     @Description    Create the parent pipes object.


                This constructor creates the parent pipe object. After
                this call there are no communication pipes open. Use 
                teo110_Create to create/open the communication pipes.
     @Return value   none

   */

                    teo110_ParentPipe     () {;};
  virtual           ~teo110_ParentPipe    () { eo110_CloseAllPipes(); };

  /*!
     @Description    Create the communication pipes.


                This function creates the communication pipes for bidirectional 
                data transmission.
     @param          ChildPipeDesc [out] - The child pipe descriptor, is needed to create                                         the child pipe object. Use it as input                                         of the teo110_ChildPipe::eo110_Initialize                                         function.                                        If you are using the communication pipes for IPC,                                         the parent process has to close the child pipe                                         ( eo110_ReleaseChildPipeHandles ) immediately                                         after the child was started.                                         See also: eo110_PipeDescToASCII and                                         eo110_ASCIIToPipeDesc.
     @param          pEventList [inout] - The event list to be used,                                        if NULL throw EventList on error.
     @Return value   true if ok
              false else (setting *pEventList)

   */

  bool              eo110_Create          ( teo110_PipeDesc               &ChildPipeDesc,
                                            bool const                    Inheritable,
                                            teo200_EventList * const      pEventList  = NULL );

  /*!
     @Description    Close the child communication pipes descriptor.


                If you are using the communication pipes for IPC, the
                parent process has to close the child pipe immediately 
                after the child has started.
     @Return value   none

   */

  void              eo110_ReleaseChildPipeHandles  ();

  /*!
     @Description    Close the data/async pipe.
     @Return value   none

   */

  void              eo110_CloseData  ();
  void              eo110_CloseAsync ();

private:
  void              eo110_CloseAllPipes   ();

private: 
  // --- make copy operator private, so external copy is not allowed!
  const teo110_ParentPipe& operator =      ( teo110_ParentPipe const      &Pipe ) { return *this; }
                    teo110_ParentPipe      ( teo110_ParentPipe            &Pipe ) {;}

private:
    teo110_PipeDesc   m_ChildPipeDesc;
};




/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class teo110_ChildPipe                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          teo110_ChildPipe
   @Description    Child pipe class.


                This pipe class is used for bidirectional data transmission. 
                To create a child pipe object, the pipe descriptor return by 
                the function teo110_ParentPipe::teo110_Create is needed.
                teo110_ChildPipe is derived from teo110_PipeBase, so it provides send 
                and receive functionality via teo110_Send and teo110_Receive.

 */

class teo110_ChildPipe : public teo110_PipeBase
{
public:
  /*!
     @Description    Create the client communication object.


                This constructor creates the client object for 
                bidirectional data transmission.
     @Return value   none

   */

                    teo110_ChildPipe      ();
  virtual           ~teo110_ChildPipe     ();

  /*!
     @Description    Initialize the client pipe.
     @param          PipeDesc [in] - The child pipe descriptor, return by                                         the constructor of teo110_ParentPipe.
     @Return value   none

   */

  void              eo110_Initialize      ( teo110_PipeDesc const         PipeDesc );

  /*!
     @Description    Close the data/async pipe.
     @Return value   none

   */

  void              eo110_CloseData  ();
  void              eo110_CloseAsync ();

private: 
  // --- make copy operator private, so external copy is not allowed!
  const teo110_ChildPipe& operator =      ( teo110_ChildPipe const        &Pipe ) { return *this; }
                    teo110_ChildPipe      ( teo110_ChildPipe              &Pipe ) {;}
};



/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO110_H */
