/****************************************************************************/
/*!
  @file     Kernel_FileIO.cpp

  -------------------------------------------------------------------------

  @author TorstenS

  last changed: 2001-05-30  16:21
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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

*/
/*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo51.h"                          // vgetpid
#include "heo55k.h"                         // vbegexcl/vendexcl
#include "heo66.h"                          // RTE File operations
#include "hsp77.h"                          // sp77sprintf
#include "hgg08.h"                          // g08config

#include "IOManager/IOMan_IPage.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "KernelCommon/Kernel_Messages.hpp"
#include "RunTime/RTE_Message.hpp"


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
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

Kernel_FileIO::Kernel_FileIO( const tsp00_VFilename  &FileName )
        :
        m_TaskId( 0 ),
        m_IsOpen( false ),
        m_FileError( tsp00_VfReturn::fromConst( vf_ok )),
        m_FileNo( 0 ),
        m_FileName( FileName ),
        m_doNotOpen( false )
{
    Open();
}

/*---------------------------------------------------------------------------*/

Kernel_FileIO::Kernel_FileIO(
    const tsp00_VFilename  &FileName,
    const SAPDB_Int4        dumpFileNo)
        :
        m_TaskId( 0 ),
        m_IsOpen( true ),
        m_FileError( tsp00_VfReturn::fromConst( vf_ok )),
        m_FileNo( dumpFileNo ),
        m_FileName( FileName ),
        m_doNotOpen( true )
{
}

/*---------------------------------------------------------------------------*/

Kernel_FileIO::Kernel_FileIO(
    const SAPDB_Char    *Label,
    const SAPDB_Char    *Extension,
    const SAPDB_Char    *FirstName,
    const SAPDB_Char    *SecondName)
        :
        m_TaskId( 0 ),
        m_IsOpen( false ),
        m_FileError( tsp00_VfReturn::fromConst( vf_ok )),
        m_FileNo( 0 ),
        m_doNotOpen( false )
{
    memset( m_FileName, 0x20, sizeof( m_FileName ));

    if( NULL == SecondName )
        sp77sprintf (m_FileName, sizeof(m_FileName), "%s%s.%s",
                     Label, FirstName, Extension);
    else
        sp77sprintf (m_FileName, sizeof(m_FileName), "%s-%s-%s.%s",
                     Label, FirstName, SecondName, Extension);

    Open();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Kernel_FileIO::Open()
{
    // Get and hold the region as long as the file io operation takes time
    // This is necessary to synchonize the operations of more than one
    // user on the same file!
    
    Lock();

    if( m_doNotOpen || m_IsOpen )
        return( true );

    tsp00_ErrText   ErrText;

    vfopen( m_FileName, &m_FileNo, m_FileError, ErrText );

    if( vf_ok != m_FileError )
    {
        Kernel_Exception ErrMsg( __CONTEXT__,
                                 KERNEL_ERR_OPEN_FAILED, m_FileName, ErrText );
        RTE_Message( ErrMsg );
        return( false );
    }
    m_IsOpen = true;
    return( m_IsOpen );
}

/*---------------------------------------------------------------------------*/

void
Kernel_FileIO::Close()
{
    if( m_IsOpen && (! m_doNotOpen) )
    {
        if(( vf_ok == m_FileError ) || ( vf_eof == m_FileError ))
        {
            tsp00_ErrText   ErrText;

            vfclose( m_FileNo, m_FileError, ErrText );
        }
        m_IsOpen = false;
    }
    Unlock();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Kernel_FileIO::Write( IOMan_IPage    &Page )
{
    if( !m_IsOpen )
        return( false );

    tsp00_ErrText   ErrText;

    vfwrite (m_FileNo, reinterpret_cast< char* >( Page.GetFramePtr() ),
             m_FileError, ErrText);

    return( vf_ok == m_FileError );
}

/*===========================================================================*
 *  PRIVATE METHODS                                                          *
 *===========================================================================*/

void 
Kernel_FileIO::Lock()
{
    if( m_doNotOpen || IsLocked() )
        return;
    vgetpid( m_TaskId );
    vbegexcl( m_TaskId, g08config );
}

/*---------------------------------------------------------------------------*/

void 
Kernel_FileIO::Unlock()
{
    if( m_doNotOpen || ! IsLocked())
        return;

    vendexcl( m_TaskId, g08config );
    m_TaskId = 0;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
