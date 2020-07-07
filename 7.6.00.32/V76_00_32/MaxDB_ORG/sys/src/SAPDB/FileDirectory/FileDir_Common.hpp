/*****************************************************************************/
/*!

  @file         FileDir_Common.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file defining types local to file directory.

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

#ifndef FILEDIR_COMMON_HPP
#define FILEDIR_COMMON_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "FileDirectory/FileDir_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

class FileDir_BaseEntry;

/// Trace topic for file directory.
extern SAPDBTrace_Topic FileDir_Trace;

/*!
 * @brief Hash node holding a key-value pair.
 */
struct FileDir_Node {
    FileDir_BaseEntry* entry;    ///< Pointer to entry of this node.
    FileDir_Node* next;      ///< Pointer to next entry in this
                             ///  bucket, 0 if there is none.
    FileDir_Node() {}              ///< Empty constructor.
    /// Constructor.
    FileDir_Node( FileDir_BaseEntry& fileEntry,
                  FileDir_Node* nextNode )
        : entry(&fileEntry),
          next(nextNode)
    {}
};


/*!
 * @class FileDir_Common
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief This class contains some common static methods that are used
 *        throughout the file directory.
 */
class FileDir_Common
{
public:
    /*!
     * @brief Shut down kernel as a result of a failed memory allocation.
     *
     * @param requestedSize [in] amount of memory requested (in bytes)
     */
    static void OutOfMemoryCrash(const SAPDB_UInt requestedSize);

    /// Contains the different database page types that make up the
    /// file directory.
    enum FileDir_PageTypes {
        FileDir_PageIndexPage,
        FileDir_Page
    };
};

enum FileDir_EntryLayouts {
    fdelInitial = 0,
    fdelWithLobSize,
    fdelWithFileAttributes
};

#endif // FILEDIR_COMMON_HPP
