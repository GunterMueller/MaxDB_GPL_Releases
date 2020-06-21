/*****************************************************************************/
/*!

  @file           Data_FileTypes.hpp
  @ingroup        File
  @author         UweH

  @brief          general types for component file.

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
/*****************************************************************************/
#ifndef Data_FileTypes_HPP
#define Data_FileTypes_HPP
/// This describes some file types.
enum Data_FileType
{
    Data_UnknownFile,
    Data_UndoFile,
    Data_RedoFile,
    Data_HistoryFile,
    Data_HistoryDirectory,
    Data_OpenTransFile,
    Data_TableFile,
    Data_IndexFile,
    Data_OmsFile,
    Data_OmsFixedFile,
    Data_OmsKeyFile,
    Data_OmsKeyPartitionFile,
    Data_OmsVarFile,
    Data_OmsContFile,
    Data_ShortColumnFile,
    Data_FDirCompatFile,
    Data_FixSizeTempFile,
    Data_SingleIndexFile
};

/// Possible states a file can be in. Stored in file directory.
enum Data_FileState
{
    Data_FileOk,
    Data_FileDeleted,
    Data_FileNotAccessible,
    Data_FileUnloaded,
    Data_FileReadOnly,
    Data_FileBad
};
#endif // Data_FileTypes_HPP
