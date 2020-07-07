/*!***************************************************************************

  module      : Log_Exceptions.hpp
  responsible : UweH
  special area: Logging
  created     : 2000-11-21
  last changed: 2000-11-21  12:00
  Copyright (c) 2000-2005 SAP AG
  description : defines exceptions in this component



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

#ifndef LOG_EXCEPTION_HPP
#define LOG_EXCEPTION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

// Constants for error messages, generated from "Log_Messages.genmsg"
#include "Logging/Log_Messages.hpp"    /* nocheck but_imkdep */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! ---------------------------------------------------------------------------
declaration: SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( Data )
description: defines common exception class of component "Data"
             (class Log_Exceptions)
*/
SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( Log );
/*! enddeclaration */  

#endif // DATA_EXCEPTION_HPP
