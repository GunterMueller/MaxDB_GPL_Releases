/*!
  @file           IOMan_LogPages.hpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This module is used to define a variant of the template 
                  class IOMan_KernelPage suitable for log pages.
  @see            

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



#ifndef IOMAN_LOGPAGES_HPP
#define IOMAN_LOGPAGES_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Logging/Log_Page.hpp"
#include "IOManager/IOMan_KernelPages.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
    @brief  Defines abbreviation for log page variant of a common
            page collection template
 */
/* -----------------------------------------------------------------------*/

typedef IOMan_Pages<Log_Page>           IOMan_LogPages;

/* -----------------------------------------------------------------------*/
/*!
    @brief  Defines abbreviation for log page iterator variant of a 
            common page collection template iterator.
 */
/* -----------------------------------------------------------------------*/

typedef IOMan_PagesIterator<Log_Page>   IOMan_LogPagesIterator;


#endif  /* IOMAN_LOGPAGES_HPP */
