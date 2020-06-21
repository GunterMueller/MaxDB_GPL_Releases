/*

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

*/

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "hsp77.h"
#include "heo52.h"

#define MAX_DATE_DIGITS 8
#define MAX_TIME_DIGITS 6

Catalog_Timestamp::Catalog_Timestamp()
{
    vtimestamp(*reinterpret_cast<tsp00_Timestamp*>(&m_timestamp));
    m_timestamp[sizeof(tsp00_Timestamp)] = 0;
}

//-----------------------------------------------------------------

Catalog_Timestamp::Catalog_Timestamp(SAPDB_Int4 date, SAPDB_Int4 time)
{
    if ((0 == date) && (0 == time))
    {
        vtimestamp(*reinterpret_cast<tsp00_Timestamp*>(&m_timestamp));
        m_timestamp[sizeof(tsp00_Timestamp)] = 0;
    }
    else
    {
        // 8 == MAX_DATE_DIGITS
        // 6 == MAX_TIME_DIGITS
        sp77sprintf(&m_timestamp[0], sizeof(m_timestamp), "%08d%06d", date, time);
    }
}

//-----------------------------------------------------------------

void Catalog_Timestamp::ToInt (SAPDB_Int4& date, SAPDB_Int4& time)
{
    char* p = &m_timestamp[MAX_DATE_DIGITS + MAX_TIME_DIGITS - 1];
    time    = 0;
    int exp = 1;
    for (int ix = 0; ix < MAX_TIME_DIGITS; ++ix)
    {
        time += (*p - '0') * exp;
        --p;
        exp *= 10;
    }
    date  = 0;
    exp  = 1;
    for (int jx = 0; jx < MAX_DATE_DIGITS; ++jx)
    {
        date += (*p - '0') * exp;
        --p;
        exp *= 10;
    }
}
