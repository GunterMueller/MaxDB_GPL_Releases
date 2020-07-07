/*****************************************************************************/
/*!
 @file    FileDir_Common.cpp
 @author  MartinKi
 @ingroup FileDir_Directory_Files

 @brief Implementation of class FileDir_Common.

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

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "RunTime/RTE_Crash.hpp"


void FileDir_Common::OutOfMemoryCrash(const SAPDB_UInt requestedSize)
{
    FileDir_Exception errMsg(
        __CONTEXT__, FILEDIR_OUT_OF_MEMORY,
        SAPDB_ToString(requestedSize) );
    RTE_Crash(errMsg);
}

/**************************************************************************/
