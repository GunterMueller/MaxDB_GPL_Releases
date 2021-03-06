/*!
    @file           IOMan_ConverterPages.hpp
    @author         TorstenS
    @ingroup        IOManagement
    @brief          This module is used to define a variant of the template 
                    class IOMan_KernelPage suitable for converter pages.

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


#ifndef IOMAN_CONVERTERPAGES_HPP
#define IOMAN_CONVERTERPAGES_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Converter/Converter_Page.hpp"
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
    @brief  Defines abbreviation for converter page variant of a common
            page collection template
 */
/* -----------------------------------------------------------------------*/

typedef IOMan_Pages<Converter_Page>         IOMan_ConverterPages;

/* -----------------------------------------------------------------------*/
/*!
    @brief  Defines abbreviation for converter page iterator variant of a 
            common page collection template iterator.
 */
/* -----------------------------------------------------------------------*/

typedef IOMan_PagesIterator<Converter_Page> IOMan_ConverterPagesIterator;

#endif  /* IOMAN_CONVERTERPAGES_HPP */
