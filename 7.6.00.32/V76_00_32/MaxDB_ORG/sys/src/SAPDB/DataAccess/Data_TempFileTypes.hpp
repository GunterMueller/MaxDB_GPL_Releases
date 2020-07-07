/*****************************************************************************/
/*!
  @file
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

#ifndef DATA_TEMPFILETYPES_HPP
#define DATA_TEMPFILETYPES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Data_TempFileTypes
{
public:
    enum FileType {
        None = 0,
        ComplexSelect,
        ComplexView,
        FromSelect,
        UserResult,
        TempTable,
        Pars,
        ParsTrigger,
        Into,
        KeyUpdate,
        Usage,
        JoinResult,
        Subquery,
        Correlation,
        Link,
        Column,
        Distinct,
        SaveScheme,
        ErrorText,
        Union,
        UnionIndex,
        InternResult,
        TempInv,
        LongSurrogates,
        OuterJoinKeys,
        OuterJoin,
        UnionIntersect,
        Trigger,
        Filler28,
        PendingLink,
        LateUniqueCheck,
        CacheRollback,
        LogPosList,
        TempLog,
        Filler34,
        OnlinePno,
        GatewayResult,
        Recursive,
        OmsVersion,
        OmsVersionIndex
    };

    static SAPDB_Int GetFileTypeCount();

    static SAPDB_Bool IsMonitoringRelevant(
        const tgg00_TfnTemp& fileType);

    static const SAPDB_Char* GetTypeName(
        const FileType fileType);

    static FileType MapTfnTempToFileType(const tgg00_TfnTemp_Param ttfnTemp)
    { // MSVC cannot resolve method if it is defined in *.cpp file...
        SAPDBERR_ASSERT_STATE(
            ttfnTemp < Data_TempFileTypes::GetFileTypeCount() );
        return (Data_TempFileTypes::FileType) (ttfnTemp);
    }


    static tgg00_TfnTemp_Param MapFileTypeToTfnTemp(const FileType fileType)
    { // MSVC cannot resolve method if it is defined in *.cpp file...
        return (tgg00_TfnTemp_Param) (fileType);
    }

private:
    typedef struct {
        FileType      fileType;
        const char*   name;
        bool          monitorRelevant;
    } TempFileInfo;

    static const TempFileInfo c_fileInfo[];
};

inline SAPDB_Bool Data_TempFileTypes::IsMonitoringRelevant(
    const tgg00_TfnTemp& fileType)
{
    SAPDBERR_ASSERT_STATE( fileType < Data_TempFileTypes::GetFileTypeCount() );

    return c_fileInfo[ fileType ].monitorRelevant;
}

inline const SAPDB_Char* Data_TempFileTypes::GetTypeName(
    const Data_TempFileTypes::FileType fileType)
{
    SAPDBERR_ASSERT_STATE( fileType < Data_TempFileTypes::GetFileTypeCount() );
    return  c_fileInfo[ fileType ].name;
}

#endif // DATA_TEMPFILETYPES_HPP
