/****************************************************************************

  module      : Converter_PageFrameAllocator.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Converter_PageFrameAllocator
  description : This module offers the facility to request and release
                memory capable for converter page I/O operations.
                
                The Converter_PageFrameAllocator class is a convenient 
                capsule of the component frame control, which is the only
                one manager for I/O capable memory.


  last changed: 2001-06-22  17:39
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2004 SAP AG

  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end

*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Converter/Converter_Converter.hpp"
#include "Converter/Converter_PageFrameAllocator.hpp"
#include "FrameControl/FrameCtrl_IConverter.hpp"

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

Kernel_Page::PageFrame
Converter_PageFrameAllocator::New( const SAPDB_Bool bIsRecoverable )
{
    if( bIsRecoverable )
        Converter_Converter::Instance().IncrementPermConvPageCount();
    else
        Converter_Converter::Instance().IncrementTempConvPageCount();

    return FrameCtrl_IConverter::GetInstance().NewConvFrame( m_Task.ID());
}

/*---------------------------------------------------------------------------*/

void
Converter_PageFrameAllocator::Free( Data_PageFrame      &frame,
                                    const SAPDB_Bool     bIsRecoverable )
{
    if( bIsRecoverable )
        Converter_Converter::Instance().DecrementPermConvPageCount();
    else
        Converter_Converter::Instance().DecrementTempConvPageCount();

    FrameCtrl_IConverter::GetInstance().FreeConvFrame( m_Task.ID(), frame );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
