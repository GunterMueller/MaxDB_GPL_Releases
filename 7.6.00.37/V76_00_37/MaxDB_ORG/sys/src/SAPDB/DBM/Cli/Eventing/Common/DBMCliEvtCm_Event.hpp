/*!
@file           DBMCli_EventListener.hpp
@author         Bernd Vorsprach - bernd.vorsprach@sap.com
@brief          event class

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
*/

#ifndef DBMCliEvtCm_Event_HPP
#define DBMCliEvtCm_Event_HPP

/*
-----------------------------------------------------------------------------
includes
-----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_DateTime.hpp"
#include "DBM/Cli/DBMCli_Result.hpp"
#include "RunTime/RTE_ISystem.hpp"

/*!
    @brief     Event
*/
class DBMCliEvtCm_Event
{
public:  
    /*! @brief priority type */
    typedef enum {
        prio_unknown,
        prio_low,
        prio_medium,
        prio_high
    } Priority;


    static const char* const AttribName;
    static const char* const AttribValue1;
    static const char* const AttribValue2;
    static const char* const AttribPriority;

    /*! @brief create an event from a DBMCli_Result object */
    DBMCliEvtCm_Event (DBMCli_Result& oResult);

    /*! @brief create an event from string containing a serialized event */
    DBMCliEvtCm_Event (const DBMCli_String& oEvtString);

    /*!
    @brief create an event "for another event"

    Creation date and time are set to local system time. This should be
    used to document errors.

    @param name event name
    @param value1 value of value1 field
    @param prio priority of the event
    @param text event text
    @param count value of the count field
    @param rteDatetime date and time appended to the event text
    @see Priority
    */
    DBMCliEvtCm_Event (
        const DBMCli_String& name,
        const SAPDB_Int4 value1,
        const Priority prio,
        const DBMCli_String& text,
        const SAPDB_Int4 count,
        const RTE_ISystem::DateTime rteDatetime);

    /*!
    @brief create an event

    Creation date and time are set to local system time. This should be used
    to document real own happenings...

    @param name event name
    @param prio priority of the event
    @param text event text
    @param count value of the count field
    @param value1 value of value1 field
    @param value2 value of value2 field
    */
    DBMCliEvtCm_Event (
        const DBMCli_String& name,
        const Priority prio,
        const DBMCli_String& text,
        const SAPDB_Int4 count,
        const SAPDB_Int4 value1 = 0,
        const SAPDB_Int4 value2 = 0);

    /*! @brief get a event property */
    const DBMCli_String & GetName        () const { return m_sName;       }
    /*! @brief get a event property */
    Priority        GetPriority    () const  { return m_oPriority;   }
    /*! @brief get a event property */
    const DBMCli_Date   & GetDate        () const  { return m_oDate;       }
    /*! @brief get a event property */
    const DBMCli_Time   & GetTime        () const  { return m_oTime;       }
    /*! @brief get a event property */
    SAPDB_Int4      GetValue1      () const  { return m_nValue1;     }
    /*! @brief get a event property */
    SAPDB_Int4      GetValue2      () const  { return m_nValue2;     }
    /*! @brief get a event property */
    SAPDB_Int4      GetCount       () const  { return m_nCount;      }
    /*! @brief get a event property */
    const DBMCli_String & GetText        () const  { return m_sText;       }
    /*! @brief get a event property */
    const DBMCli_String & GetDescription () const  { return m_sDescription;}
    /*! @brief get the event as a string */
    DBMCli_String serialize() const;

private:
    /*! @breif set a property */
    void SetProperty (const DBMCli_String & sProperty);
    /*! @brief property member */
    DBMCli_String m_sName;
    /*! @brief property member */
    Priority      m_oPriority;
    /*! @brief property member */
    DBMCli_Date   m_oDate;
    /*! @brief property member */
    DBMCli_Time   m_oTime;
    /*! @brief property member */
    SAPDB_Int4    m_nValue1;
    /*! @brief property member */
    SAPDB_Int4    m_nValue2;
    /*! @brief property member */
    SAPDB_Int4    m_nCount;
    /*! @brief property member */
    DBMCli_String m_sText;
    /*! @brief property member */
    DBMCli_String m_sDescription;

    static const char SEPCHAR;
};

#endif //DBMCliEvtCm_Event_HPP
