/*


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



*/
#ifndef __OMS_DATETIME_HPP
#define __OMS_DATETIME_HPP

#include "Oms/OMS_AFX.h"

#define OMS_DT_SEC_PER_HOUR 3600L  // 60*60

class AFX_EXT_CLASS OMS_DateTime
{
public:
  OMS_DateTime() { dtValue = 0; }
  OMS_DateTime( long year, long month, long day, long hour, long min, long sec );

  unsigned long getDTValue ( void ) const { return dtValue; }
  int seconds( void ) const { return dtValue%60; }
  int minutes( void ) const { return (dtValue/60)%60; }
  int hours  ( void ) const { return (dtValue/OMS_DT_SEC_PER_HOUR)%24; }
  int day    ( void ) const;
  int month  ( void ) const;
  int year   ( void ) const;
private:
  unsigned long dtValue;
};

/*----------------------------------------------------------------------*/

extern 
void omsGetLocalDateTimeString(char*, int);

/*----------------------------------------------------------------------*/

extern
void omsGetLocalDateTime(int& year,
                         int& month,
                         int& day,
                         int& hours,
                         int& minutes,
                         int& seconds);

/*----------------------------------------------------------------------*/

extern
void omsGetUTCDateTimeString(char*, int);

/*----------------------------------------------------------------------*/

extern
void omsGetUTCDateTime(int& year,
                       int& month,
                       int& day,
                       int& hours,
                       int& minutes,
                       int& seconds);

#endif // __OMS_DATETIME_HPP