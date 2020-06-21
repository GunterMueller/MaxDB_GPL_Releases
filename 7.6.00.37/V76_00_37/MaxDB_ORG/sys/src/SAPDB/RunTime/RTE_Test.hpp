/*!
  @file           RTE_Test.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Runtime Test call interface

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

#ifndef RTE_TEST_HPP
#define RTE_TEST_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"

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
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL FUNCTIONS                                                       *
 *===========================================================================*/

/*!
  @brief RTE Common test call entrypoint
  @param Trans [inout] the transaction context
  @param utf8StringArgument [in] Optional addition argument (may be 0)
  @param utf8StringArgumentByteLength [in] Optional length of argument
  @return none (possible error code returned via transaction context
 */
void RTE_Test ( tgg00_TransContext& Trans
              , SAPDB_UTF8 const *utf8StringArgument=0
              , SAPDB_Int4 utf8StringArgumentByteLength=0);

#endif /* RTE_TEST_HPP */
