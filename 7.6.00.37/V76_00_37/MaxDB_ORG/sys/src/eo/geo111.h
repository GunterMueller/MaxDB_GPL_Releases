/*!
  @file           geo111.h
  @author         RaymondR
  @special area   
  @brief          
  @see            

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



#ifndef GEO111_H
#define GEO111_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo42.h"
#include "geo110.h"
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

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class teo111_ChildComm                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

class teo111_ChildComm : public teo110_ChildPipe
{
public:
                    teo111_ChildComm   ( int                                            argc, 
                                         TCHAR**                                        argv,
                                         bool                                           &Ok );
  virtual           ~teo111_ChildComm  () {;}


# if !defined (AIX)
protected:
   // access declarations ( hide some base class methodes )
   using teo110_ChildPipe::eo42_SendAsyncReqResp;
   using teo110_ChildPipe::eo110_Initialize;
   using teo110_ChildPipe::eo42_TransferUnitBufferSize;
# endif

private: 
  // --- make copy operator private, so external copy is not allowed!
  const teo111_ChildComm& operator =      ( teo111_ChildComm const        &Comm ) { return *this; }
};

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class teo111_ParentComm                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

class teo111_ParentComm : public teo110_ParentPipe
{
public:
                    teo111_ParentComm  () : m_ClientStarted(false) {;};
  virtual           ~teo111_ParentComm ();

  bool              teo111_Connect     ( _TCHAR const * const                           Executable,
                                         teo200_EventList * const                       pEventList = NULL );

private:
  bool              eo111_StartClient  ( _TCHAR const * const                           Executable,
                                         teo110_PipeDesc::teo110_ASCIIPipeDesc const *  ASCIIPipeDesc,
                                         teo200_EventList * const                       pEventList = NULL );

# if !defined (AIX)
protected:
   // access declarations  ( hide some base class methodes )
   using teo110_ParentPipe::eo42_RecvAsyncReqResp;
   using teo110_ParentPipe::eo110_Create;
   using teo110_ParentPipe::eo110_ReleaseChildPipeHandles;
   using teo110_ParentPipe::eo42_TransferUnitBufferSize;
# endif

private:
  bool                    m_ClientStarted;

#if defined(_WIN32)
  PROCESS_INFORMATION     m_ProcInfo;
#endif /* _WIN32/UNIX */

private:
  // --- make copy operator private, so external copy is not allowed!
  const teo111_ParentComm& operator =      ( teo111_ParentComm const        &Comm ) { return *this; }
};

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

#endif  /* GEO111_H */
