/*===========================================================================*
 *  INCLUDES                                                                 *
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
 *===========================================================================*/

#include "Oms/OMS_DateTime.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_DbpError.hpp"
#include "ggg00.h"
#include "ggg01.h" // OMS errors
#include "Oms/OMS_Globals.hpp"
#include "RunTime/RTE_IInterface.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif

#define SEC_PER_DAY 86400L  // 24*60*60

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

static int omsDateTimeMTbl[13]={ 0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337, 367 };

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

_INLINE int omsDay2Year( unsigned long v );
_INLINE unsigned long omsYMD2Date( int y, int m, int d );
_INLINE unsigned long omsYear2Day( int v );
_INLINE int omsGetDay  ( unsigned long date ); 
_INLINE int omsGetMonth( unsigned long date );
 
/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

_INLINE int omsDay2Year( unsigned long v )
{
  return int(1970L + (v+0.5)/365.25);
}

_INLINE unsigned long omsYear2Day( int v )
{
  return int((v-1970L)*365.25+0.25);
}

_INLINE unsigned long omsYMD2Date( int y, int m, int d )
{
  m--; d--;
  if( y<100 ) y+=1900;
  if( y<1970 || y>2099 ) 
    OMS_Globals::Throw(e_OMS_invalid_timestamp, "omsYMD2Date", __MY_FILE__, __LINE__);
  if( m<0 || m>=12 ) 
    OMS_Globals::Throw(e_OMS_invalid_timestamp, "omsYMD2Date", __MY_FILE__, __LINE__);
  unsigned long x=omsYear2Day(y) + d;

  const int off = ((y%4)==0) ? 31+29 : 31+28;
  if( m>=2 ) x+=off; else x-=306;

  if( m<2 ) m+=10; else m-=2;
  x+=omsDateTimeMTbl[m];

  return x;
}

_INLINE int omsGetDay( unsigned long date ) 
{
  long x=date;             // Tag seit BASE_YEAR
  int j=omsDay2Year(x);  // Jahr dazu
  x-=omsYear2Day(j);     // Tag seit Anfang des Jahres j

  int off = ((j%4)==0) ? 31+29 : 31+28;
  if( x>=off ) x-=off; else x+=306;
  int m=0;
  while( x >= omsDateTimeMTbl[m] ) m++;
  m--;
  int t=(int)(x-omsDateTimeMTbl[m]);
  t++;
  return t;
}

_INLINE int omsGetMonth( unsigned long date )
{
  long x=date;             // Tag seit BASE_YEAR
  int j=omsDay2Year(x);  // Jahr dazu
  x-=omsYear2Day(j);     // Tag seit Anfang des Jahres j

  int off = ((j%4)==0) ? 31+29 : 31+28;
  if( x>=off ) x-=off; else x+=306;
  int m=0;
  while( x >= omsDateTimeMTbl[m] ) m++;
  m--;
  if( m>=10 ) m-=10; else m+=2;
  m++;
  return m;
}

/*===========================================================================*
*  GLOBAL FUNCTIONS (CODE)                                                  *
*===========================================================================*/

OMS_DateTime::OMS_DateTime( long year, long month, long day, long hour, long min, long sec )
{
  dtValue = omsYMD2Date (year, month, day) * SEC_PER_DAY + (sec+60*(min+60*hour));
}

int OMS_DateTime::day ( void ) const 
{
  return omsGetDay (dtValue/SEC_PER_DAY);
}

int OMS_DateTime::month ( void ) const
{
  return omsGetMonth (dtValue/SEC_PER_DAY);
}

int OMS_DateTime::year ( void ) const
{
  return omsDay2Year (dtValue/SEC_PER_DAY);
}

/*----------------------------------------------------------------------*/

extern
void omsGetLocalDateTimeString(char* pBuf, int bufSize)
{
    if (bufSize < sizeof(RTE_IInterface::Timestamp))
  {
    OMS_Globals::Throw(e_buffer_too_small, "omsGetLocalDateTime", __MY_FILE__, __LINE__);
  }
  RTE_IInterface::Instance().FillProtocolTimeStamp(*REINTERPRET_CAST(RTE_IInterface::Timestamp*, pBuf));
  pBuf[sizeof(RTE_IInterface::Timestamp)-1] = 0;
}

/*----------------------------------------------------------------------*/

extern
void omsGetLocalDateTime(int& year,
                         int& month,
                         int& day,
                         int& hours,
                         int& minutes,
                         int& seconds)
{
  char ts[sizeof(RTE_IInterface::Timestamp)];
  omsGetLocalDateTimeString(&ts[0], sizeof(ts));
  sscanf(ts, RTE_IF_TIMESTAMP_FORMAT, &year, &month, &day, &hours, &minutes, &seconds);
}

/*----------------------------------------------------------------------*/

extern 
void omsGetUTCDateTimeString (char* pBuf, int bufSize)
{
  if (bufSize < sizeof(RTE_IInterface::Timestamp))
  {
    OMS_Globals::Throw(e_buffer_too_small, "omsGetUTCDateTimeString", __MY_FILE__, __LINE__);
  }
  RTE_IInterface::Instance().FillProtocolGMTTimeStamp(*REINTERPRET_CAST(RTE_IInterface::Timestamp*, pBuf));
  pBuf[sizeof(RTE_IInterface::Timestamp)-1] = 0;
}

/*----------------------------------------------------------------------*/

extern
void omsGetUTCDateTime(int& year,
                       int& month,
                       int& day,
                       int& hours,
                       int& minutes,
                       int& seconds)
{
  char ts[sizeof(RTE_IInterface::Timestamp)];
  omsGetUTCDateTimeString(&ts[0], sizeof(ts));
  sscanf(ts, RTE_IF_TIMESTAMP_FORMAT, &year, &month, &day, &hours, &minutes, &seconds);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/