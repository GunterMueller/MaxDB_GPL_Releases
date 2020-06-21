/****************************************************************************

  module      : FrameCtrl_Exception.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: 
  description : 


  last changed: 2001-04-19  20:01
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


*****************************************************************************/


#ifndef FRAMECTRL_EXCEPTION_HPP
#define FRAMECTRL_EXCEPTION_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"


/*!-----------------------------------------------------------------------------
declaration: SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( IOMan )
description: defines common exception class of this component
             
             - class IOMan_Exception
------------------------------------------------------------------------------*/
SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( FrameCtrl );
  

/*! enddeclaration */

#endif  /* FRAMECTRL_EXCEPTION_HPP */
