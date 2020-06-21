/*!
  \file    Loader_SpecialConstants.hpp
  \author  SteffenS
  \brief   Conversion of special constants - defaults, assignments, bool

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

#ifndef LOADER_SPECIALCONSTANTS_HPP
#define LOADER_SPECIALCONSTANTS_HPP

#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_Common.hpp"


class Loader_SpecialConstants
{
public:
    static LOADER_Int4 InsertDefaultValue(//LOADER_SQLColumnInfo*   pColumnInfo,
                                          Loader_ColumnData*  pFieldData,
                                          LOADER_Byte*        pszBuffer,
                                          LOADER_Int4         lBufferLength);

    static LOADER_ErrorCode increaseSysKey(LOADER_Byte* pValue);

    static LOADER_ErrorCode convertToBoolean(Loader_ColumnData*  pColumnData);

    static LOADER_Int4      convertDateTimeValues(Loader_ColumnData* pColumnData, LOADER_DateTimeMask* pDateTimeMask, LOADER_DateTimeFormat DateTimeFormat);
};

//*
//*     All following prototypes (i41...) are for compatibility reasons with the respective
//*     pascal source vin41 where no prototypes exist.
//*
externPascal void i41dget(tin_date_mask&    DateStruct,
                          tsp00_Addr        pszSourceDate,
                          int               lDateLength,
                          tsp00_Addr        pszDestDate,
                          int&              lError);

externPascal void i41tget(tin_date_mask&    TimeStruct,
                          tsp00_Addr        pszSourceTime,
                          int               lTimeLength,
                          tsp00_Addr        pszDestTime,
                          int&              lError);

externPascal void i41tsget(tin_date_mask&   TimeStampStruct,
                           tsp00_Addr       pszSourceTimeStamp,
                           int              lTimeStampLength,
                           tsp00_Addr       pszDestTimeStamp,
                           int&             lError);


#endif // LOADER_SPECIALCONSTANTS_HPP