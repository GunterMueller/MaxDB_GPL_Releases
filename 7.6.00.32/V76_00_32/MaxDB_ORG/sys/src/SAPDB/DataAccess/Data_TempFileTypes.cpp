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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_TempFileTypes.hpp"

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

const Data_TempFileTypes::TempFileInfo Data_TempFileTypes::c_fileInfo[] =
{
    { Data_TempFileTypes::None, "None", false },
    { Data_TempFileTypes::ComplexSelect, "ComplexSelect", true },
    { Data_TempFileTypes::ComplexView, "ComplexView", true },
    { Data_TempFileTypes::FromSelect, "FromSelect", true },
    { Data_TempFileTypes::UserResult, "UserResult", true },
    { Data_TempFileTypes::TempTable, "TempTable", true },
    { Data_TempFileTypes::Pars, "Pars", false },
    { Data_TempFileTypes::ParsTrigger, "ParsTrigger", false },
    { Data_TempFileTypes::Into, "Into", false },
    { Data_TempFileTypes::KeyUpdate, "KeyUpdate", false },
    { Data_TempFileTypes::Usage, "Usage", false },
    { Data_TempFileTypes::JoinResult, "JoinResult", false },
    { Data_TempFileTypes::Subquery, "Subquery", false },
    { Data_TempFileTypes::Correlation, "Correlation", false },
    { Data_TempFileTypes::Link, "Link", false },
    { Data_TempFileTypes::Column, "Column", false },
    { Data_TempFileTypes::Distinct, "Distinct", false },
    { Data_TempFileTypes::SaveScheme, "SaveScheme", false },
    { Data_TempFileTypes::ErrorText, "ErrorText", false },
    { Data_TempFileTypes::Union, "Union", false },
    { Data_TempFileTypes::UnionIndex, "UnionIndex", false },
    { Data_TempFileTypes::InternResult, "InternResult", false },
    { Data_TempFileTypes::TempInv, "TempInv", false },
    { Data_TempFileTypes::LongSurrogates, "LongSurrogates", false },
    { Data_TempFileTypes::OuterJoinKeys, "OuterJoinKeys", false },
    { Data_TempFileTypes::OuterJoin, "OuterJoin", false },
    { Data_TempFileTypes::UnionIntersect, "UnionIntersect", false },
    { Data_TempFileTypes::Trigger, "Trigger", false },
    { Data_TempFileTypes::Filler28, "Filler28", false },
    { Data_TempFileTypes::PendingLink, "PendingLink", false },
    { Data_TempFileTypes::LateUniqueCheck, "LateUniqueCheck", false },
    { Data_TempFileTypes::CacheRollback, "CacheRollback", false },
    { Data_TempFileTypes::LogPosList, "LogPosList", false },
    { Data_TempFileTypes::TempLog, "TempLog", false },
    { Data_TempFileTypes::Filler34, "Filler34", false },
    { Data_TempFileTypes::OnlinePno, "OnlinePno", false },
    { Data_TempFileTypes::GatewayResult, "GatewayResult", false },
    { Data_TempFileTypes::Recursive, "Recursive", false },
    { Data_TempFileTypes::OmsVersion, "OmsVersion", false },
    { Data_TempFileTypes::OmsVersionIndex, "OmsVersionIndex", false }
};

/*************************************************************************/

SAPDB_Int Data_TempFileTypes::GetFileTypeCount()
{
    return (sizeof(c_fileInfo)/sizeof(TempFileInfo));
}

/*************************************************************************/
