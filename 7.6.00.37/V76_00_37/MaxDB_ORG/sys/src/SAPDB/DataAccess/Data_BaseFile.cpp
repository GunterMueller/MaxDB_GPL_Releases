/***************************************************************************/
/*!

  @file        Data_BaseFile.cpp
  @author      UweH
  @ingroup     File
  @brief       This is the implementation of Data_BaseFile
  @since       2001-05-12
  @sa          see also ....
*/
/*!
\if EMIT_LICENCE
  copyright:    Copyright (c) 2001-2005 SAP AG


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
#include "DataAccess/Data_BaseFile.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

inline void FileTypeToString (Data_FileType  filetype,
                              SAPDB_Char    *string,
                              SAPDB_UInt     stringlength)
{
    /*
    SAPDB_UInt copylength 
    switch ( filetype )
    {
        case Data_UnknownFile:
            SAPDB_Memcpy (string,"UnknownFile",stringlength);
            break;
        case Data_UndoFile:
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
    Data_FixSizeTempFile
    }
    */
}
//-----------------------------------------------------------------------------------------------------------
void Data_BaseFile::WriteToTrace (const char * title) const
{
    Kernel_VTrace trace;
    if ( title != 0 )
        trace << title << FlushLine;
	trace << "filetype: "   << m_Type << ", rootpno: " << m_RootId.PageNo()
          << ", addrMode: " << Data_AddressingModeStrings [m_RootId.PageRecoveryMode().GetAddressingMode()]
          << ", recov: "    << Data_RecoverabilityStrings [m_RootId.PageRecoveryMode().GetRecoverability()];
}

//-----------------------------------------------------------------------------------------------------------

void Data_BaseFile::AppendContextInfo (SAPDB_Char  const * const  filename,
                                       SAPDB_UInt4 const          linenumber,
                                       SAPDBErr_MessageList      &errlist,
                                       SAPDB_Char  const * const  title) const
{
    errlist = errlist +
              Data_Exception(filename,linenumber,DATA_BASE_FILE_INFO,
                             (title!=0?title:"Data_BaseFile"),
                             SAPDB_ToString(m_Type),
                             SAPDB_ToString(m_RootId.PageNo()),
                             Data_AddressingModeStrings [m_RootId.PageRecoveryMode().GetAddressingMode()],
                             Data_RecoverabilityStrings [m_RootId.PageRecoveryMode().GetRecoverability()]);
}