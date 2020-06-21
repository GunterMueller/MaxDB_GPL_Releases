/*!
  @file           Kernel_DateTime.hpp
  @author         Till Luhmann
  @author         MartinB
  @author         TorstenS
  @ingroup        KernelCommon
  @brief          Kernel date time support
  @see            

\if EMIT_LICENCE
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

\endif
*/



#ifndef KERNEL_DATETIME_HPP
#define KERNEL_DATETIME_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "RunTime/System/RTESys_Time.h"

/// the invalid value is ZERO
#define KERNEL_INVALID_DATE 0

///	date value (e.g. 20010213 == 13.02.2001)
typedef SAPDB_Invalidatable<SAPDB_UInt4, KERNEL_INVALID_DATE> Kernel_Date;

/// the invalid value is ZERO
#define KERNEL_INVALID_TIME 0

/// time value (e.g. 00154857 == 15:48:57)
typedef SAPDB_Invalidatable<SAPDB_UInt4, KERNEL_INVALID_TIME> Kernel_Time;


/*!
   @brief   kernel time handling
*/
class Kernel_DateTime
{
public:

    /*!
        @brief  Default constructor. Creates an invalid internal date/time 
        @return none
     */

    Kernel_DateTime()
    {}

    /*!
        @brief  Constructor, creates an object with the given system time
        @param  date [in]  internal date
        @param  time [in]  internal time
        @return none
    */

    Kernel_DateTime(
        const Kernel_Date   date,
        const Kernel_Time   time )
    :
    m_Date( date ),
    m_Time( time )
    {}

    /*!
        @brief  Constructor, creates an object with the given system time in UTC format
        @param  utcTimeStamp [in]  timestamp in UTC format
        @return none
    */

    Kernel_DateTime(  const SAPDB_UInt8 utcTimeStamp )
    {
        SAPDB_UInt4 myDate;
        SAPDB_UInt4 myTime;

        RTESys_FillKernelTimeStampFromUTC( utcTimeStamp, myDate, myTime );

        if( 19700101 == myDate ){ // initial time value startet 1970-01-01
            Invalidate();
        }
        else
        {
            m_Date = myDate;
            m_Time = myTime;
        }
    }

    /*!
        @brief  Copy Constructor.
        @param  dateTime [in]  internal date/time 
        @return none
    */

    Kernel_DateTime( const Kernel_DateTime &dateTime )
    :
    m_Date( dateTime.m_Date ),
    m_Time( dateTime.m_Time )
    {}

    /*!
         @brief  Checks whether the current internal date/time is valid.
         @return bool
     */

    bool IsValid() const{
        return( m_Date.IsValid() && m_Time.IsValid());
    }

    /*!
         @brief  Sets the sytem time stored in the object to invalid. 
                 This will not effect the real system time!
         @return none
     */

    void Invalidate()
    {
        m_Date.Invalidate();
        m_Time.Invalidate();
    }

    /*!
         @brief  Determines the current sytem time.
         @return none
     */

    void DetermineTime()
    {
        SAPDB_UInt4 myDate;
        SAPDB_UInt4 myTime;

        RTESys_FillKernelTimeStamp( myDate, myTime );

        m_Date = myDate;
        m_Time = myTime;
    }

    /*!
        @brief  Determines the current system time minus a specified delay.
        @param  delayInSeconds [in] specifies the delay the current
                                    system time is reduced.
        @return none
    */

    void DetermineDelayedTime( const SAPDB_Int4   delayInSeconds )
    {
        SAPDB_UInt4 myDate;
        SAPDB_UInt4 myTime;

        if( 0 == delayInSeconds )
            RTESys_FillKernelTimeStamp( myDate, myTime );
        else
            RTESys_FillDelayedKernelTimeStamp( delayInSeconds, myDate, myTime );

        m_Date = myDate;
        m_Time = myTime;
    }

    /*!
        @brief  Assignment operator.
        @param  dateTime [in]  internal date/time 
        @return none
    */

    Kernel_DateTime& operator=( const Kernel_DateTime &dateTime )
    {
        m_Date = dateTime.m_Date;
        m_Time = dateTime.m_Time;

        return( *this );
    }

    /*!
        @brief  Returns the current date in internal format YYYYMMDD
               (e.g. 20010213 == 13.02.2001)
        @return Kernel_Date
    */

    Kernel_Date GetDate() const{
        return( m_Date );
    }

    /*!
        @brief  Returns the current time in internal format 00HHMMSS
               (e.g. 00154857 == 15:48:57).
        @return Kernel_Time
    */

    Kernel_Time GetTime() const{
        return( m_Time );
    }

    /*!
        @brief  Returns the current date/time value into a sql timestamp
                which looks like YYYYMMDDHHMMSS000000. Note that this 
                timestamp is not null terminated!
        @param  timeBuffer [out]  not null terminated time stamp, build 
                                  from date and time values.
        @return none
    */

    void GetDateTimeAsSQLTimeStamp( RTESys_SQLTimestamp VAR_ARRAY_REF timeBuffer )
    {
        RTESys_FillSQLTimeStampFromKernelTimeStamp( m_Date, m_Time, timeBuffer );
    }

    /*!
       @brief   compares the current time with the given date/time in numeric format
       @param   dateRightOp [in] date of right operand
       @param   timeRightOp [in] time of right operand
       @return  bool (this olderThan RightOp)
     */

    bool OlderThan(
        const Kernel_Date   dateRightOp,
        const Kernel_Time   timeRightOp )
    {
        return( Kernel_DateTime::OlderThan( m_Date, m_Time, dateRightOp, timeRightOp ));
    }

    /*!
       @brief   compares the current Date/Time with the given date/time in numeric format
                (PTS 1125481 mb 2003-11-18)
       @param   rightOp [in] Date/Time of right operand
       @return  bool (this olderThan RightOp)
     */

    bool OlderThan( const Kernel_DateTime   rightOp )
    {
        return( Kernel_DateTime::OlderThan( m_Date, m_Time, rightOp.GetDate(), rightOp.GetTime() ));
    }

    /*!
       @brief   compares two tuples of (date/time) in numeric format (PTS 1111525 mb 2001-01-15)
       @param   dateLeftOp  [in] date of left operand
       @param   timeLeftOp  [in] time of left operand
       @param   dateRightOp [in] date of right operand
       @param   timeRightOp [in] time of right operand
       @return  bool (LeftOp olderThan RightOp)
     */

    static bool OlderThan(
        const Kernel_Date   dateLeftOp,
        const Kernel_Time   timeLeftOp,
        const Kernel_Date   dateRightOp,
        const Kernel_Time   timeRightOp)
    {
        if( dateLeftOp < dateRightOp )
        {
            return true;
        }
        else if( dateLeftOp == dateRightOp )
        {
            return timeLeftOp < timeRightOp;
        }
        else
        {
            return false;
        }
    }

private:

    // internal kernel date
    Kernel_Date m_Date;

    // internal kernel time
    Kernel_Time m_Time;
};

#endif // KERNEL_DATETIME_HPP
