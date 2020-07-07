/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_DateTime.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Data / Time handling
  version:      
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



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




*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_DateTime.hpp"
#include <time.h>

/*    
  =============================================================================
  class: DBMCli_Date
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Date :: DBMCli_Date
  -----------------------------------------------------------------------------
*/
DBMCli_Date :: DBMCli_Date ( )
{
  m_nYear  = 0;
  m_nMonth = 0;
  m_nDay   = 0;

} // end DBMCli_Date :: DBMCli_Date

DBMCli_Date :: DBMCli_Date ( const SAPDB_Int nYear, 
                             const SAPDB_Int nMonth, 
                             const SAPDB_Int nDay )
             : m_nYear ( nYear ),
               m_nMonth ( nMonth ),
               m_nDay ( nDay )
{
} // end DBMCli_Date :: DBMCli_Date
/*
  -----------------------------------------------------------------------------
*/
DBMCli_Date :: DBMCli_Date ( const DBMCli_String & sDate,
                             const DBMCli_DTDateFormat nFormat )
{
  Set( sDate, nFormat );
} // end DBMCli_Date :: DBMCli_Date
/*
  -----------------------------------------------------------------------------
*/
DBMCli_Date ::  DBMCli_Date ( const DBMCli_String & sDate, 
                              const DBMCli_String & sFormat )
{
  Set( sDate, sFormat );
} // end DBMCli_Date :: DBMCli_Date
/*
  -----------------------------------------------------------------------------
*/
DBMCli_Date ::  DBMCli_Date ( const DBMCli_String & sDate )
{
  Set( sDate );
} // end DBMCli_Date :: DBMCli_Date

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Date :: ~DBMCli_Date
  -----------------------------------------------------------------------------
*/
DBMCli_Date :: ~DBMCli_Date ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Date :: Set
  -----------------------------------------------------------------------------
*/
void DBMCli_Date :: Set ( const SAPDB_Int nYear, 
                          const SAPDB_Int nMonth, 
                          const SAPDB_Int nDay )
{
  m_nYear  = nYear;
  m_nMonth = nMonth;
  m_nDay   = nDay;
}

void DBMCli_Date :: Set ( const DBMCli_String & sDate,
                          const DBMCli_DTDateFormat nFormat )
{
  m_nYear = 0;
  m_nMonth = 0;
  m_nDay = 0;

  if( sDate.GetLength() > 0 ) {
    switch( nFormat ) {
      case DBMCLI_DT_INT_DATE_FMT:
        m_nYear = atoi( sDate.Left( 4 ) );
        m_nMonth = atoi( sDate.Mid( 5, 2 ) );
        m_nDay = atoi( sDate.Mid( 8, 2 ) );
        break;

      case DBMCLI_DT_NUM_DATE_FMT:
        m_nYear = atoi( sDate.Left( 4 ) );
        m_nMonth = atoi( sDate.Mid( 4, 2 ) );
        m_nDay = atoi( sDate.Mid( 6, 2 ) );
        break;
    }
  }
}

void DBMCli_Date :: Set( const DBMCli_String & sDate, 
                         const DBMCli_String & sFormat )
{
  m_nYear = 0;
  m_nMonth = 0;
  m_nDay = 0;

  if( sDate.GetLength() > 0 ) {

    if( sDate.GetLength() >= sFormat.GetLength() ) {

      DBMCli_String sYear;
      DBMCli_String sMonth;
      DBMCli_String sDay;

      for( SAPDB_Int i = 0; i < sFormat.GetLength(); i++ ) {
        switch( sFormat[i] ) {

          case DT_FMT_CHAR_YEAR:
            sYear += sDate[i];
            break;

          case DT_FMT_CHAR_MONTH:
            sMonth += sDate[i];
            break;

          case DT_FMT_CHAR_DAY:
            sDay += sDate[i];
            break;

          default:
            break;
        }
      }

      m_nYear = atoi( sYear );
      m_nMonth = atoi( sMonth );
      m_nDay = atoi( sDay );
    }
  }
}

void DBMCli_Date :: Set ( const DBMCli_String & sDate )
{
  m_nYear = 0;
  m_nMonth = 0;
  m_nDay = 0;

  if( sDate.GetLength() > 0 ) {

    DBMCli_String sFormat( DT_INT_DATE_FMT );

    if( sDate.GetLength() >= sFormat.GetLength() ) {
      m_nYear = atoi( sDate.Left( 4 ) );
      m_nMonth = atoi( sDate.Mid( 5, 2 ) );
      m_nDay = atoi( sDate.Mid( 8, 2 ) );
    }
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Date :: Get
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Date :: Get ( const DBMCli_DTDateFormat nFormat ) const
{
  DBMCli_String sDate;

  switch( nFormat ) {
    case DBMCLI_DT_INT_DATE_FMT:
      sDate.Format( "%04d-%02d-%02d", m_nYear, m_nMonth, m_nDay ); 
      break;

    case DBMCLI_DT_NUM_DATE_FMT:
      sDate.Format( "%04d%02d%02d", m_nYear, m_nMonth, m_nDay ); 
      break;
  }
  return sDate;
}

/*!
  -------------------------------------------------------------------------
  public operator DBMCli_Date :: operator =
  -------------------------------------------------------------------------
*/
DBMCli_Date & DBMCli_Date :: operator = (const DBMCli_Date & Date) 
{

  this->m_nDay    = Date.m_nDay;
  this->m_nMonth  = Date.m_nMonth;
  this->m_nYear   = Date.m_nYear;

  return *this;
} // end DBMCli_Date :: operator =

/*!
  -------------------------------------------------------------------------
  public function DBMCli_Date :: Now
  -------------------------------------------------------------------------
*/
DBMCli_Date DBMCli_Date :: Now ( ) 
{
  DBMCli_Date oD;

  time_t      aTime;
  struct tm * pTime;

  time(&aTime);

  pTime = localtime(&aTime);

  oD.m_nDay    = pTime->tm_mday;
  oD.m_nMonth  = pTime->tm_mon + 1;
  oD.m_nYear   = pTime->tm_year + 1900;

  return oD;
} // end DBMCli_Date :: Now

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/


/*    
  =============================================================================
  class: DBMCli_Time
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Time :: DBMCli_Time
  -----------------------------------------------------------------------------
*/
DBMCli_Time :: DBMCli_Time ( )
{
  m_nHour    = 0;
  m_nMinute  = 0;
  m_nSecond  = 0;
} // end DBMCli_Time :: DBMCli_Time
/*
  -----------------------------------------------------------------------------
*/
DBMCli_Time :: DBMCli_Time ( const SAPDB_Int nHour, 
                             const SAPDB_Int nMinute, 
                             const SAPDB_Int nSecond )
             : m_nHour ( nHour ),
               m_nMinute ( nMinute ),
               m_nSecond ( nSecond )
{
} // end DBMCli_Time :: DBMCli_Time
/*
  -----------------------------------------------------------------------------
*/
DBMCli_Time :: DBMCli_Time ( const DBMCli_String & sTime,
                             const DBMCli_DTTimeFormat nFormat )
{
  Set( sTime, nFormat );
} // end DBMCli_Time :: DBMCli_Time
/*
  -----------------------------------------------------------------------------
*/
DBMCli_Time ::  DBMCli_Time ( const DBMCli_String & sTime, 
                              const DBMCli_String & sFormat )
{
  Set( sTime, sFormat );
} // end DBMCli_Time :: DBMCli_Time
/*
  -----------------------------------------------------------------------------
*/
DBMCli_Time ::  DBMCli_Time ( const DBMCli_String & sTime )
{
  Set( sTime );
} // end DBMCli_Time :: DBMCli_Time

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Time :: ~DBMCli_Time 
  -----------------------------------------------------------------------------
*/
DBMCli_Time :: ~DBMCli_Time ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Time :: Set
  -----------------------------------------------------------------------------
*/
void DBMCli_Time :: Set ( const SAPDB_Int nHour, 
                          const SAPDB_Int nMinute, 
                          const SAPDB_Int nSecond )
{
  m_nHour = nHour;
  m_nMinute = nMinute;
  m_nSecond = nSecond;
}

void DBMCli_Time :: Set ( const DBMCli_String & sTime,
                          const DBMCli_DTTimeFormat nFormat )
{
  m_nHour = 0;
  m_nMinute = 0;
  m_nSecond = 0;

  if( sTime.GetLength() > 0 ) {
    switch( nFormat ) {
      case DBMCLI_DT_INT_TIME_FMT:
        m_nHour = atoi( sTime.Left( 2 ) );
        m_nMinute = atoi( sTime.Mid( 3, 2 ) );
        m_nSecond = atoi( sTime.Mid( 6, 2 ) );
        break;

      case DBMCLI_DT_NUM_TIME_FMT:
        if( sTime.GetLength() >= 8 ) {
          m_nHour = atoi( sTime.Left( 4 ) );
          m_nMinute = atoi( sTime.Mid( 4, 2 ) );
          m_nSecond = atoi( sTime.Mid( 6, 2 ) );
        } else {
          m_nHour = atoi( sTime.Left( 2 ) );
          m_nMinute = atoi( sTime.Mid( 2, 2 ) );
          m_nSecond = atoi( sTime.Mid( 4, 2 ) );
        }
        break;
    }
  }
}

void DBMCli_Time ::  Set ( const DBMCli_String & sTime, 
                           const DBMCli_String & sFormat )
{
  m_nHour = 0;
  m_nMinute = 0;
  m_nSecond = 0;

  if( sTime.GetLength() > 0 ) {

    if( sTime.GetLength() >= sFormat.GetLength() ) {

      DBMCli_String sHour;
      DBMCli_String sMinute;
      DBMCli_String sSecond;

      for( SAPDB_Int i = 0; i < sFormat.GetLength(); i++ ) {
        switch( sFormat[i] ) {

          case DT_FMT_CHAR_HOUR:
            sHour += sTime[i];
            break;

          case DT_FMT_CHAR_MINUTE:
            sMinute += sTime[i];
            break;

          case DT_FMT_CHAR_SECOND:
            sSecond += sTime[i];
            break;

          default:
            break;
        }
      }

      m_nHour = atoi( sHour );
      m_nMinute = atoi( sMinute );
      m_nSecond = atoi( sSecond );
    }
  }
}

void DBMCli_Time ::  Set ( const DBMCli_String & sTime )
{
  m_nHour = 0;
  m_nMinute = 0;
  m_nSecond = 0;

  if( sTime.GetLength() > 0 ) {

    DBMCli_String sFormat( DT_INT_TIME_FMT );

    if( sTime.GetLength() >= sFormat.GetLength() ) {

      m_nHour = atoi( sTime.Left( 2 ) );
      m_nMinute = atoi( sTime.Mid( 3, 2 ) );
      m_nSecond = atoi( sTime.Mid( 6, 2 ) );
    }
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Time :: Get
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Time :: Get ( const DBMCli_DTTimeFormat nFormat ) const
{
  DBMCli_String sTime;

  switch( nFormat ) {
    case DBMCLI_DT_INT_TIME_FMT:
      sTime.Format( "%02d:%02d:%02d", m_nHour, m_nMinute, m_nSecond ); 
      break;

    case DBMCLI_DT_NUM_TIME_FMT:
      sTime.Format( "%02d%02d%02d", m_nHour, m_nMinute, m_nSecond ); 
      break;
  }
  return sTime;
} // end DBMCli_Time :: Get

/*!
  -------------------------------------------------------------------------
  public DBMCli_Time :: operator =
  -------------------------------------------------------------------------
*/
DBMCli_Time & DBMCli_Time :: operator = (const DBMCli_Time & Time) 
{

  this->m_nHour    = Time.m_nHour;
  this->m_nMinute  = Time.m_nMinute;
  this->m_nSecond  = Time.m_nSecond;

  return *this;
} // end DBMCli_Time :: operator =

/*!
  -------------------------------------------------------------------------
  public function DBMCli_Time :: Now
  -------------------------------------------------------------------------
*/
DBMCli_Time DBMCli_Time :: Now ( ) 
{
  DBMCli_Time oT;

  time_t      aTime;
  struct tm * pTime;

  time(&aTime);

  pTime = localtime(&aTime);

  oT.m_nHour    = pTime->tm_hour;
  oT.m_nMinute  = pTime->tm_min;
  oT.m_nSecond  = pTime->tm_sec;

  return oT;
} // end DBMCli_Time :: Now

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*    
  =============================================================================
  class: DBMCli_DateTime
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_DateTime :: DBMCli_DateTime
  -----------------------------------------------------------------------------
*/
DBMCli_DateTime :: DBMCli_DateTime ( )
{
//  m_oDate = DBMCli_Date :: Now();
//  m_oTime = DBMCli_Time :: Now();
} // end DBMCli_DateTime :: DBMCli_DateTime
/*
  -----------------------------------------------------------------------------
*/
DBMCli_DateTime ::  DBMCli_DateTime ( const DBMCli_Date & oDate, 
                                      const DBMCli_Time & oTime )
                 : m_oDate ( oDate ),
                   m_oTime ( oTime )
{
} // end DBMCli_DateTime :: DBMCli_DateTime
/*
  -----------------------------------------------------------------------------
*/
DBMCli_DateTime :: DBMCli_DateTime ( const DBMCli_String & sDateTime,
                                     const DBMCli_DTDateTimeFormat nFormat )
{
  Set( sDateTime, nFormat );
} // end DBMCli_DateTime :: DBMCli_DateTime
/*
  -----------------------------------------------------------------------------
*/
DBMCli_DateTime :: DBMCli_DateTime ( const DBMCli_String & sDateTime ) 
{
  Set( sDateTime );
} // end DBMCli_DateTime :: DBMCli_DateTime
/*
  -----------------------------------------------------------------------------
*/
DBMCli_DateTime :: DBMCli_DateTime ( const DBMCli_String & sDate,
                                     const DBMCli_String & sTime  )
{
  Set( sDate, sTime );
} // end DBMCli_DateTime :: DBMCli_DateTime

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_DateTime :: ~DBMCli_DateTime
  -----------------------------------------------------------------------------
*/
DBMCli_DateTime :: ~DBMCli_DateTime ( )
{
} // end DBMCli_DateTime :: ~DBMCli_DateTime

/*
  -----------------------------------------------------------------------------
  public function DBMCli_DateTime :: Set
  -----------------------------------------------------------------------------
*/
void DBMCli_DateTime :: Set ( const DBMCli_String & sDateTime,
                              const DBMCli_DTDateTimeFormat nFormat )
{
  if( sDateTime.GetLength() > 0 ) {
    switch( nFormat ) {
      case DBMCLI_DT_INT_DATETIME_FMT:
        m_oDate.Set( sDateTime.Left( 10 ), DBMCLI_DT_INT_DATE_FMT );
        m_oTime.Set( sDateTime.Mid( 11, 8 ), DBMCLI_DT_INT_TIME_FMT ); 
        break;
      case DBMCLI_DT_NUM_DATETIME_FMT:
        m_oDate.Set( sDateTime.Left( 8 ), DBMCLI_DT_NUM_DATE_FMT );
        m_oTime.Set( sDateTime.Mid( 8, 6 ), DBMCLI_DT_NUM_TIME_FMT ); 
        break;
    }
  }
}

void DBMCli_DateTime :: Set ( const DBMCli_String & sDateTime ) 
{
  if( sDateTime.GetLength() > 0 ) {

    DBMCli_String sFormat( DT_INT_DATETIME_FMT );

    if( sDateTime.GetLength() >= sFormat.GetLength() ) {
      m_oDate.Set( sDateTime.Left( 10 ) );
      m_oTime.Set( sDateTime.Mid( 11, 8 ) ); 
    }
  }
}

void DBMCli_DateTime :: Set ( const DBMCli_String & sDate,
                              const DBMCli_String & sTime  )
{
  m_oDate.Set(sDate);
  m_oTime.Set(sTime);
}

void DBMCli_DateTime :: Set ( const DBMCli_String & sDate, const DBMCli_DTDateFormat nDateFormat,
                              const DBMCli_String & sTime, const DBMCli_DTTimeFormat nTimeFormat )
{
  m_oDate.Set(sDate, nDateFormat);
  m_oTime.Set(sTime, nTimeFormat);
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_DateTime :: Get
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_DateTime :: Get ( const DBMCli_DTDateTimeFormat nFormat ) const
{
  DBMCli_String sDateTime;

  switch( nFormat ) {
    case DBMCLI_DT_INT_DATETIME_FMT:
      sDateTime.Format( "%04d-%02d-%02d %02d:%02d:%02d", 
                         m_oDate.Year(), m_oDate.Month(), m_oDate.Day(),
                         m_oTime.Hour(), m_oTime.Minute(), m_oTime.Second() ); 
      break;

    case DBMCLI_DT_NUM_DATETIME_FMT:
      sDateTime.Format( "%04d%02d%02d%02d%02d%02d", 
                         m_oDate.Year(), m_oDate.Month(), m_oDate.Day(),
                         m_oTime.Hour(), m_oTime.Minute(), m_oTime.Second() ); 
      break;
  }
  return sDateTime;
} // end DBMCli_DateTime :: Get

/*!
  -------------------------------------------------------------------------
  public DBMCli_DateTime :: operator =
  -------------------------------------------------------------------------
*/
DBMCli_DateTime & DBMCli_DateTime :: operator = (const DBMCli_DateTime & DateTime) 
{

  this->m_oDate  = DateTime.m_oDate;
  this->m_oTime  = DateTime.m_oTime;

  return *this;
} // end DBMCli_DateTime :: operator =

/*!
  -------------------------------------------------------------------------
  public function DBMCli_DateTime :: Now
  -------------------------------------------------------------------------
*/
DBMCli_DateTime DBMCli_DateTime :: Now ( ) 
{
  DBMCli_DateTime oDT(DBMCli_Date::Now(), DBMCli_Time::Now());

  return oDT;
} // end DBMCli_DateTime :: Now

/*!
  -------------------------------------------------------------------------
  public DBMCli_DateTime :: operator =
  -------------------------------------------------------------------------
*/
SAPDB_Int operator - (const DBMCli_DateTime & DateTime1, 
                      const DBMCli_DateTime & DateTime2) 
{

  struct tm sTm_1;
  struct tm sTm_2;

  // filling structure 1
  memset(&sTm_1, 0, sizeof(sTm_1));
  sTm_1.tm_year = DateTime1.m_oDate.Year   () - 1900;
  sTm_1.tm_mon  = DateTime1.m_oDate.Month  () - 1;
  sTm_1.tm_mday = DateTime1.m_oDate.Day    ();
  sTm_1.tm_hour = DateTime1.m_oTime.Hour   ();
  sTm_1.tm_min  = DateTime1.m_oTime.Minute ();
  sTm_1.tm_sec  = DateTime1.m_oTime.Second ();

  // filling structure 2
  memset(&sTm_2, 0, sizeof(sTm_2));
  sTm_2.tm_year = DateTime2.m_oDate.Year   () - 1900;
  sTm_2.tm_mon  = DateTime2.m_oDate.Month  () - 1;
  sTm_2.tm_mday = DateTime2.m_oDate.Day    ();
  sTm_2.tm_hour = DateTime2.m_oTime.Hour   ();
  sTm_2.tm_min  = DateTime2.m_oTime.Minute ();
  sTm_2.tm_sec  = DateTime2.m_oTime.Second ();

  return (SAPDB_Int) difftime(mktime(&sTm_1), mktime(&sTm_2));
} // end DBMCli_DateTime :: operator =

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/


