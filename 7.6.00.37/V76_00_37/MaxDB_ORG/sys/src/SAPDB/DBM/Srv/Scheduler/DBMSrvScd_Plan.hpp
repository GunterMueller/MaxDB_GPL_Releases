/*!
  \file    DBMSrvScd_Plan.hpp
  \author  MarcW
  \brief   plan declaration

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include <time.h>
#include <stdio.h>

#if !defined(DBMSrvScd_Plan_hpp)
#define DBMSrvScd_Plan_hpp

/*!
    @brief plan for job execution

    Currently, only the time of day (hour, minutes, seconds) is taken as a plan. 24 hour
    format.
*/
class DBMSrvScd_Plan {
public:
    /*!
        @brief constructor

        Construct a plan object from a tm structure. Only its components tm_hour, tm_min
        and tm_sec are used.
        @param aPlannedTimeOfDay [IN] time structure
    */
    DBMSrvScd_Plan( const struct tm aPlannedTimeOfDay );

    /*!
        @brief constructor

        Construct a plan object from serialized plan.
        @param aPlanDescription [IN] serialized plan
        @see serialize()
    */
    DBMSrvScd_Plan( const char* aPlanDescription );

    /*! @brief copy constructor */
    DBMSrvScd_Plan( const DBMSrvScd_Plan& otherPlan );

    /*! @brief desctructor */
    virtual ~DBMSrvScd_Plan() {};

    /*! 
        @brief returns a formatted time string im the format HH:mm:ss. Memory must
        be managed by called.
    */
    char* getTimeString() const;

    /*!
        @brief checks, whether its time to run (the owning job should be started)

        @return true, if it's less than 10 seconds after the last execution and the last
                 execution was yesterday of even ealier; false otherwise
    */
    bool itsTimeToRun();

    /*! 
        @brief sets last execution time to now, sets next planned execution time

        This information is not serialized.
    */
    void updateExecutionTimes();

    struct tm getLastPlannedExecutionTime() const;

    /*!
        @brief set a new planned execution time

        @param aPlannedTimeOfDay[IN] new planned execution time, (hour, minutes and
               seconds are used)
    */
    void updatePlannedExecutionTime( const struct tm aPlannedTimeOfDay );

    /*!
        @brief serialize this plan

        New memory is allocated here caller is responsible
        @return reference to serialized plan
    */
    char* serialize();

private:
    struct tm m_PlannedExecutionTime;
    struct tm m_LastExecutionTime;

    size_t getLen( struct tm* atmStruct );
    const char* deserialize( const char* aStart, struct tm* atmStruct );
    void resetStruct( struct tm* atmStruct ) const;
    void initPlannedExecutionTime();
    void setStartTime();

    static time_t m_StartTime;
    static bool m_StartTimeSet;
    static const char SEPCHAR;
};

#endif //DBMSrvScd_Plan_hpp
