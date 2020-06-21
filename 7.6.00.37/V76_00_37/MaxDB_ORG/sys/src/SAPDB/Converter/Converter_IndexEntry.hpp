/*!
  @file           Converter_IndexEntry.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Entry of converter index
*/

/*
    ========== licence begin  GPL
    Copyright (c) 2000-2004 SAP AG

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


#ifndef CONVERTER_INDEXENTRY_HPP
#define CONVERTER_INDEXENTRY_HPP


#include "Converter/Converter_Entry.hpp"
#include "Converter/Converter_IndexPage.hpp"


/*!
   @interface      Converter_IndexEntry
   @brief          Entry of converter index
 */
class Converter_IndexEntry :
public Converter_ChainNode <Converter_IndexEntryNo>,
public Converter_Entry     <Converter_IndexPage>
{};


#endif // CONVERTER_INDEXENTRY_HPP
