/*!
    \file    ToolsTime_Time.hpp
    \author  TiloH
    \ingroup time handling for Tools
    \brief   declaration of a class for handling time data

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#if !defined TOOLSTIME_TIME_HPP
#define TOOLSTIME_TIME_HPP

class ToolsTime_TimeFormat; //forward declaration to avoid include of ToolsTime_TimeFormat.hpp

/*! \brief a class for handling date and time values

    Use class ToolsTime_Formatter to transform a time object if this class into a formatted time string
    \see ToolsTime_Formatter */
class ToolsTime_Time
{
  public:
    /*! \bief default constructor
       
       The default constructor initializes the object with the current time */
    ToolsTime_Time(void);

    /*! \brief constructor
        
        \param string [IN] zero terminated string representing a time
        \param format [IN] the time format of string */
    ToolsTime_Time(const char * string, const ToolsTime_TimeFormat & format);

    /*! \brief constructor

        \param string        [IN]  zero terminated string representing a time (time can be followed by something else)
        \param format        [IN]  the time format of string start
        \param endOfDateTime [OUT] pointer to first character after the time within string */
    ToolsTime_Time(const char * string, const ToolsTime_TimeFormat & format, const char * & endOfDateTime);

    /*! \brief set object to current time */
    void setTo();

    /*! \brief set object to a certain time
    
        The time is parsed from a string using a specified time format
        \param time   [IN] the desired time as string
        \param format [IN] the time format of parameter time */
    void setTo(const char * time, const ToolsTime_TimeFormat & format);

    /*! \brief set object to a certain time
    
        The time is parsed from a zero terminated string using a specified time format, the end of the
        time in the string is returned via a parameter
        \param time      [IN]  the desired time as string (zero terminated)
        \param format    [IN]  the time format that must be used
        \param endOfTime [OUT] end of time within time (first character after the time) */
    void setTo(const char * time, const ToolsTime_TimeFormat & format, const char * & endOfTime);

    /*! \brief check if a object represents a valid time
    
        \return true, if object represents a valid time, false otherwise */
    bool isValidTime() const;

    /*! \brief compare operator <=
 
       \param otherDateTime [IN] the object (*this) is compared to
       \return true, if (*this) represents a point in time older or equal to that of otherDateTime
               false, otherwise */
    bool operator<=(const ToolsTime_Time & otherDateTime) const;

    /*! \brief compare operator ==
 
       \param otherDateTime [IN] the object (*this) is compared to
       \return true, if (*this) represents a point equal to that of otherDateTime
               false, otherwise */
    bool operator==(const ToolsTime_Time & otherDateTime) const;

    /*! \brief compare operator !=
 
        \param otherDateTime [IN] the object (*this) is compared to
        \return true, if (*this) represents a point not equal to that of otherDateTime
               false, otherwise */
    bool operator!=(const ToolsTime_Time & otherDateTime) const;

    /*! \brief compare operator >
 
        \param otherDateTime [IN] the object (*this) is compared to
        \return true, if (*this) represents a point newer then that of otherDateTime
               false, otherwise */
    bool operator>(const ToolsTime_Time & otherDateTime) const;

    /*! \brief get year, month, day, hour, minute and second of the time object

        \param year   [OUT] the year as integer
        \param month  [OUT] the month as integer
        \param day    [OUT] the day as integer
        \param hour   [OUT] the hour as integer
        \param minute [OUT] the minute as integer
        \param second [OUT] the second as integer */
    void getParts(int & year, int & month, int & day, int & hour, int & minute, int & second) const;

    /*! \brief get the year of the time object

        \return year as integer */
    int getYear() const;

    /*! \brief get the month of the time object

        \return month as integer */
    int getMonth() const;

    /*! \brief get the day of the time object

        \return day as integer */
    int getDay() const;

    /*! \brief get the hour of the time object

        \return hour as integer */
    int getHour() const;

    /*! \brief get the minute of the time object

        \return minute as integer */
    int getMinute() const;

    /*! \brief get the second of the time object

        \return second as integer */
    int getSecond() const;

  private:
    void setMonthDaySecondsTo(int year, int month, int day, int hour, int minute, int second);

    int m_Year;           //!< storage for the year
    int m_MonthDaySecond; //!< storage for month, day, hour, minute and second (hour, minute and seconds are simply added up in seconds - hence the name)

    static int monthConversionFactor; //!< factor used to split month from days and seconds
    static int dayConversionFactor;   //!< factor used to split day from seconds
    static int secondsPerHour;        //!< factor used to calculate hours out of seconds
    static int secondsPerMinute;      //!< factor used to calculate minutes out of seconds

    static int maxMonthDaySeconds;  //!< for generating compiler warnings if int is too small for maximal possible value of m_MonthDaySecond (e.g. on 16-bit cellular phone)
};

#endif
