/*!
  @file           IFRUtil_DateTime.h
  @author         D039759
  @ingroup        
  @brief          Date and Time verifying utility functions.
  @see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (C) 2001-2005 SAP AG

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
#ifndef IFRUTIL_DATETIME_H
#define IFRUTIL_DATETIME_H

#include "Interfaces/Runtime/IFR_Types.h"

IFR_BEGIN_NAMESPACE

class IFRUtil_DateTime
{
public:
    static inline IFR_Bool isDateValid(SQL_DATE_STRUCT& date)
    {
        if(date.year < 1 ||
           date.month < 1 || date.month > 12 
           || date.day < 1) {
            return false;
        } else {
            switch(date.month) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                return date.day <= 31;
            case 2:
                return date.day <= (((date.year%400==0) || (date.year%4==0 && date.year%100!=0)) ? 29 : 28);
            case 4:
            case 6:
            case 9:
            case 11:
                return date.day <= 30;
            default: 
                return false;
            }
        }
    }
    
    static inline IFR_Bool isTimeValid(SQL_TIME_STRUCT& time)
    {
        return time.hour >= 0 &&
            time.hour <= 23 && 
            time.minute >= 0 &&
            time.minute <= 59 &&
            time.second >=0 &&
            time.second <= 59;
    }

    static inline IFR_Bool isTimestampValid(SQL_TIMESTAMP_STRUCT& ts)
    {
        if(ts.year < 1 ||
           ts.month < 1 || ts.month > 12 
           || ts.day < 1) {
            return false;
        } else {
            switch(ts.month) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                return ts.day <= 31;
            case 2:
                return ts.day <= (((ts.year%400==0) || (ts.year%4==0 && ts.year%100!=0)) ? 29 : 28);
            case 4:
            case 6:
            case 9:
            case 11:
                return ts.day <= 30;
            default: 
                return false;
            }
        }
        
        return ts.hour >= 0 &&
            ts.hour <= 23 && 
            ts.minute >= 0 &&
            ts.minute <= 59 &&
            ts.second >=0 &&
            ts.second <= 59 &&
            ts.fraction <= 999999999ul;
    }
};

IFR_END_NAMESPACE

#endif
