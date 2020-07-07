/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_DateTime.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Date/Time handling
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

#ifndef _DBMCLI_DATETIME_HPP_
#define _DBMCLI_DATETIME_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define DT_FMT_CHAR_YEAR   'Y'
#define DT_FMT_CHAR_MONTH  'M'
#define DT_FMT_CHAR_DAY    'D'

#define DT_FMT_CHAR_HOUR   'H'
#define DT_FMT_CHAR_MINUTE 'N'
#define DT_FMT_CHAR_SECOND 'S'

#define DT_INT_DATE_FMT     "YYYY-MM-DD"
#define DT_INT_TIME_FMT     "HH:NN:SS"
#define DT_INT_DATETIME_FMT "YYYY-MM-DD HH:NN:SS"

#define DT_NUM_DATE_FMT     "YYYYMMDD"
#define DT_NUM_TIME_FMT     "HHNNSS"
#define DT_NUM_DATETIME_FMT "YYYYMMDDHHNNSS"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
/*! Declaration: Formats */
typedef enum
{
  DBMCLI_DT_INT_DATE_FMT = 0,
  DBMCLI_DT_NUM_DATE_FMT = 1
} DBMCli_DTDateFormat;

typedef enum
{
  DBMCLI_DT_INT_TIME_FMT = 0,
  DBMCLI_DT_NUM_TIME_FMT = 1
} DBMCli_DTTimeFormat;

typedef enum
{
  DBMCLI_DT_INT_DATETIME_FMT = 0,
  DBMCLI_DT_NUM_DATETIME_FMT = 1
} DBMCli_DTDateTimeFormat;
/*! EndDeclaration: Formats */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Date
  -----------------------------------------------------------------------------
  description: Class to handle a date 
  -----------------------------------------------------------------------------
*/
class DBMCli_Date 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Date
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   5
      -------------------------------------------------------------------------
    */
    DBMCli_Date ( );

    DBMCli_Date ( const SAPDB_Int nYear, 
                  const SAPDB_Int nMonth, 
                  const SAPDB_Int nDay );

    DBMCli_Date ( const DBMCli_String & sDate,
                  const DBMCli_DTDateFormat nFormat ); 

    DBMCli_Date ( const DBMCli_String & sDate,
                  const DBMCli_String & sFormat ); 

    DBMCli_Date ( const DBMCli_String & sDate );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Date
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Date ( );

    /*!
      -------------------------------------------------------------------------
      function:     Set
      prototyoes:   4
      -------------------------------------------------------------------------
    */
    void Set ( const SAPDB_Int nYear, 
               const SAPDB_Int nMonth, 
               const SAPDB_Int nDay );

    void Set ( const DBMCli_String & sDate,
               const DBMCli_DTDateFormat nFormat ); 

    void Set ( const DBMCli_String & sDate,
               const DBMCli_String & sFormat ); 

    void Set ( const DBMCli_String & sDate );

    /*!
      -------------------------------------------------------------------------
      function:     Get
      -------------------------------------------------------------------------
    */
    DBMCli_String Get ( const DBMCli_DTDateFormat nFormat ) const; 

    /*!
      -------------------------------------------------------------------------
      function:     Year
      -------------------------------------------------------------------------
    */
    SAPDB_Int Year ( ) const { return m_nYear; };

    /*!
      -------------------------------------------------------------------------
      function:     Month
      -------------------------------------------------------------------------
    */
    SAPDB_Int Month ( ) const { return m_nMonth; };

    /*!
      -------------------------------------------------------------------------
      function:     Day
      -------------------------------------------------------------------------
    */
    SAPDB_Int Day ( ) const { return m_nDay; };

    /*!
      -------------------------------------------------------------------------
      function:     operator =
      -------------------------------------------------------------------------
    */
    DBMCli_Date & operator = (const DBMCli_Date & Date);

    /*!
      -------------------------------------------------------------------------
      function:     IsNull
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsNull ( ) const { return (m_nDay == 0 && m_nMonth == 0 && m_nYear == 0); }

    /*!
      -------------------------------------------------------------------------
      function:     Now
      -------------------------------------------------------------------------
    */
    static DBMCli_Date Now ( );

  private:
    SAPDB_Int m_nYear;
    SAPDB_Int m_nMonth;
    SAPDB_Int m_nDay;

}; 

/*! EndClass: DBMCli_Date */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Time
  -----------------------------------------------------------------------------
  description: Class to handle a time 
  -----------------------------------------------------------------------------
*/
class DBMCli_Time 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Time
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   5
      -------------------------------------------------------------------------
    */
    DBMCli_Time ( );

    DBMCli_Time ( const SAPDB_Int nHour, 
                  const SAPDB_Int nMinute, 
                  const SAPDB_Int nSecond );

    DBMCli_Time ( const DBMCli_String & sTime,
                  const DBMCli_DTTimeFormat nFormat );

    DBMCli_Time ( const DBMCli_String & sTime,
                  const DBMCli_String & sFormat ); 

    DBMCli_Time ( const DBMCli_String & sTime );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Time
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Time ( );

    /*!
      -------------------------------------------------------------------------
      function:     Set
      prototypes:   4
      -------------------------------------------------------------------------
    */
    void Set ( const SAPDB_Int nHour, 
               const SAPDB_Int nMinute, 
               const SAPDB_Int nSecond );

    void Set ( const DBMCli_String & sTime,
               const DBMCli_DTTimeFormat nFormat );

    void Set ( const DBMCli_String & sTime,
               const DBMCli_String & sFormat ); 

    void Set ( const DBMCli_String & sTime );

    /*!
      -------------------------------------------------------------------------
      function:     Get
      -------------------------------------------------------------------------
    */
    DBMCli_String Get ( const DBMCli_DTTimeFormat nFormat ) const;

    /*!
      -------------------------------------------------------------------------
      function:     Hour
      -------------------------------------------------------------------------
    */
    SAPDB_Int Hour ( ) const { return m_nHour; };

    /*!
      -------------------------------------------------------------------------
      function:     Minute
      -------------------------------------------------------------------------
    */
    SAPDB_Int Minute ( ) const { return m_nMinute; };

    /*!
      -------------------------------------------------------------------------
      function:     Second
      -------------------------------------------------------------------------
    */
    SAPDB_Int Second ( ) const { return m_nSecond; };

    /*!
      -------------------------------------------------------------------------
      function:     operator =
      -------------------------------------------------------------------------
    */
    DBMCli_Time & operator = (const DBMCli_Time & Time);

    /*!
      -------------------------------------------------------------------------
      function:     IsNull
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsNull ( ) const { return (m_nHour == 0 && m_nMinute == 0 && m_nSecond == 0); }

    /*!
      -------------------------------------------------------------------------
      function:     Now
      -------------------------------------------------------------------------
    */
    static DBMCli_Time Now ( );

  private:
    SAPDB_Int m_nHour;
    SAPDB_Int m_nMinute;
    SAPDB_Int m_nSecond;
}; 

/*! EndClass: DBMCli_Time */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_DateTime
  -----------------------------------------------------------------------------
  description: Class to handle DateTime commands to the dbm server.
  -----------------------------------------------------------------------------
*/
class DBMCli_DateTime 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_DateTime
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   5
      -------------------------------------------------------------------------
    */
    DBMCli_DateTime ( );

    DBMCli_DateTime ( const DBMCli_Date & oDate, 
                      const DBMCli_Time & oTime  );

    DBMCli_DateTime ( const DBMCli_String & sDateTime,
                      const DBMCli_DTDateTimeFormat nFormat );

    DBMCli_DateTime ( const DBMCli_String & sDateTime );

    DBMCli_DateTime ( const DBMCli_String & sDate,
                      const DBMCli_String & sTime  );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_DateTime
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_DateTime ( );

    /*!
      -------------------------------------------------------------------------
      function:     Set
      prototypes:   4
      -------------------------------------------------------------------------
    */
    void Set ( const DBMCli_String & sDateTime,
               const DBMCli_DTDateTimeFormat nFormat );

    void Set ( const DBMCli_String & sDateTime );

    void Set ( const DBMCli_String & sDate,
               const DBMCli_String & sTime  );

    void Set ( const DBMCli_String & sDate, const DBMCli_DTDateFormat nDateFormat,
               const DBMCli_String & sTime, const DBMCli_DTTimeFormat nTimeFormat );
    /*!
      -------------------------------------------------------------------------
      function:     Get
      -------------------------------------------------------------------------
    */
    DBMCli_String Get ( const DBMCli_DTDateTimeFormat nFormat ) const;

    /*!
      -------------------------------------------------------------------------
      function:     Date
      -------------------------------------------------------------------------
    */
    const DBMCli_Date & Date ( ) const { return m_oDate; };

    /*!
      -------------------------------------------------------------------------
      function:     Time
      -------------------------------------------------------------------------
    */
    const DBMCli_Time & Time ( ) const { return m_oTime; };

    /*!
      -------------------------------------------------------------------------
      function:     operator =
      -------------------------------------------------------------------------
    */
    DBMCli_DateTime & operator = (const DBMCli_DateTime & DateTime);

    /*!
      -------------------------------------------------------------------------
      function:     operator -
      -------------------------------------------------------------------------
    */
    friend SAPDB_Int operator - (const DBMCli_DateTime & DateTime1, const DBMCli_DateTime & DateTime2);

    /*!
      -------------------------------------------------------------------------
      function:     IsNull
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsNull ( ) const { return (m_oDate.IsNull() && m_oTime.IsNull()); }

    /*!
      -------------------------------------------------------------------------
      function:     Now
      -------------------------------------------------------------------------
    */
    static DBMCli_DateTime Now ( );


  private:
    DBMCli_Date m_oDate;
    DBMCli_Time m_oTime;
}; 

/*! EndClass: DBMCli_DateTime */



#endif // _DBMCLI_DATETIME_HPP_
