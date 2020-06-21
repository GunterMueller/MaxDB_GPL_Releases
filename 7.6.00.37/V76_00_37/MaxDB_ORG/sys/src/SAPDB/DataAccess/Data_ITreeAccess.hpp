/*****************************************************************************/
/*!
  @file         Data_ITreeAccess.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class .

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#ifndef DATA_ITREEACCESS_HPP
#define DATA_ITREEACCESS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_PrimPage.hpp"
#include "DataAccess/Data_Types.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class Data_ITreeAccess
 *
 * @brief Interface class for all iterators over a b*-tree.
 */
class Data_ITreeAccess
{
public:
    /*! @brief Destructor. */
    virtual ~Data_ITreeAccess() {};

    /*!
     * @brief Returns the next leaf page. If there are no more pages,
     * the page returned is invalid.
     */
    virtual Data_PrimPage& GetNext() = 0;

    /*!
     * @brief Returns the estimated number of leaf nodes. If this
     * value is not available, 0 can be returned.
     */
    virtual Data_PageCount GetNumberOfEstimatedLeafNodes() const = 0;

    /*!
     * @brief Returns the file no of the file the tree belongs to.
     */
    virtual FileDir_FileNo GetFileNo() const = 0;
};

#endif // DATA_ITREEACCESS_HPP
