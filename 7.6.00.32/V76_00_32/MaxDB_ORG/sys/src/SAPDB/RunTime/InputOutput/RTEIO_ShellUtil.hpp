/*!*********************************************************************

    @file         RTEIO_ShellUtil.hpp

    @author       DanielD

    @brief        equivalent to fileutil shell commands

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#ifndef RTEIO_SHELLUTIL_HPP
#define RTEIO_SHELLUTIL_HPP

#include "Messages/Msg_List.hpp"

/*!

  @class RTEIO_ShellUtil

  @brief implements equivalent to fileutil shell commands

 */
class RTEIO_ShellUtil
{
public:
    enum {continueOnError};

    /*!
        @brief create a directory

        All the path segments are created.
     */
    static bool CreateDirectory (const char * path, Msg_List & msgList);

    /*!
        @brief remove a file
     */
    static bool RemoveFile (const char * path, Msg_List & msgList);

    /*!
        @brief remove a directory

        The directory must be empty.
     */
    static bool RemoveDir (const char * path, Msg_List & msgList);

    /*!
        @brief remove a directory tree including all files
     */
    static bool RemoveTree (const char * path, int options, Msg_List & msgList);

    /*!
        @brief remove a directory tree including all files
     */
    static bool RemoveTree (const char * path, Msg_List & msgList);

private:
    RTEIO_ShellUtil ();  // static methods only
};

#endif

