/*!
  @file           DBMCliEvtCm_Event.cpp
  @author         MarcW
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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/RunTime/RTE_ISystem.hpp"
#include "SAPDB/DBM/Cli/DBMCli_DateTime.hpp"
#include "SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp"

const char* const DBMCliEvtCm_Event::AttribName("Name");
const char* const DBMCliEvtCm_Event::AttribValue1("Value1");
const char* const DBMCliEvtCm_Event::AttribValue2("Value2");
const char* const DBMCliEvtCm_Event::AttribPriority("Priority");

/*! @brief Constructor  */
DBMCliEvtCm_Event::DBMCliEvtCm_Event
    (DBMCli_Result & oResult )
    : m_sName(""),
      m_oPriority(prio_unknown),
      m_oDate(),
      m_oTime(),
      m_nValue1(0),
      m_nValue2(0),
      m_nCount(0),
      m_sText(""),
      m_sDescription("")
{
  DBMCli_String sLine;
  
  oResult.SetPos(0);

  while (oResult.GetLine(sLine)) {
    SetProperty(sLine);
  } // end while
} // end DBMCli_Event::DBMCli_Event

DBMCliEvtCm_Event::DBMCliEvtCm_Event(
    const DBMCli_String& name,
    const SAPDB_Int4 id,
    const Priority prio,
    const DBMCli_String& text,
    const SAPDB_Int4 count,
    const RTE_ISystem::DateTime rteDatetime
    ): m_sName(name),
       m_oPriority(prio),
       m_oDate(DBMCli_Date::Now()),
       m_oTime(DBMCli_Time::Now()),
       m_nValue1(id),
       m_nValue2(0),
       m_nCount(count),
       m_sText(text),
       m_sDescription("")
{
    DBMCli_Date eventDate(rteDatetime.Year, rteDatetime.Month, rteDatetime.Day);
    DBMCli_Time eventTime(rteDatetime.Hour, rteDatetime.Minute, rteDatetime.Second);
    DBMCli_DateTime dbmDateTime(eventDate, eventTime);
    m_sText += "; ";
    m_sText += dbmDateTime.Get(DBMCLI_DT_INT_DATETIME_FMT);
}

DBMCliEvtCm_Event::DBMCliEvtCm_Event (
    const DBMCli_String& name,
    const Priority prio,
    const DBMCli_String& text,
    const SAPDB_Int4 count,
    const SAPDB_Int4 value1,
    const SAPDB_Int4 value2)
         : m_sName(name),
           m_oPriority(prio),
           m_oDate(DBMCli_Date::Now()),
           m_oTime(DBMCli_Time::Now()),
           m_nValue1(value1),
           m_nValue2(value2),
           m_nCount(count),
           m_sText(text),
           m_sDescription(""){}


DBMCliEvtCm_Event::DBMCliEvtCm_Event (const DBMCli_String& oEvtString )
    : m_sName(""),
      m_oPriority(prio_unknown),
      m_oDate(),
      m_oTime(),
      m_nValue1(0),
      m_nValue2(0),
      m_nCount(0),
      m_sText(""),
      m_sDescription("")
{
    int start(0), len(oEvtString.Find(SEPCHAR));
    if( len < 0 )
        return;
    m_sName = oEvtString.Mid(start, len);

    start += len+1; len = oEvtString.Find(SEPCHAR, start) - start;
    if( len < 0 )
        return;
    m_oPriority = (Priority) atoi((const char *)oEvtString.Mid(start, len));

    start += len+1; len = oEvtString.Find(SEPCHAR, start) - start;
    if( len < 0 )
        return;
    DBMCli_DateTime dateTime(oEvtString.Mid(start, len), DBMCLI_DT_NUM_DATETIME_FMT);
    m_oDate = dateTime.Date();
    m_oTime = dateTime.Time();

    start += len+1; len = oEvtString.Find(SEPCHAR, start) - start;
    if( len < 0 )
        return;
    m_nValue1 = atoi((const char *)oEvtString.Mid(start, len));

    start += len+1; len = oEvtString.Find(SEPCHAR, start) - start;
    if( len < 0 )
        return;
    m_nValue2 = atoi((const char *)oEvtString.Mid(start, len));

    start += len+1; len = oEvtString.Find(SEPCHAR, start) - start;
    if( len < 0 )
        return;
    m_nCount = atoi((const char *)oEvtString.Mid(start, len));

    start += len+1; len = oEvtString.GetLength() - start;
    if( len <= 0 ) {
        // no text in event
        return;
    }
    m_sText = oEvtString.Mid(start, len);
}

DBMCli_String DBMCliEvtCm_Event::serialize() const {
    DBMCli_String oEvt;
    
    oEvt += GetName();
    oEvt += SEPCHAR;

    switch(GetPriority()) {
        case prio_low:
            oEvt += "1";
            break;
        case prio_medium:
            oEvt += "2";
            break;
        case prio_high:
            oEvt += "3";
            break;
        default:
            oEvt += "0";
    }
    
    oEvt += SEPCHAR;
    DBMCli_DateTime dt(GetDate(),GetTime());
    oEvt += dt.Get(DBMCLI_DT_NUM_DATETIME_FMT);

    char buffer[256];
    oEvt += SEPCHAR;
    sprintf(buffer,"%d",GetValue1());
    oEvt += buffer;
    
    oEvt += SEPCHAR;
    sprintf(buffer,"%d",GetValue2());
    oEvt += buffer;

    oEvt += SEPCHAR;
    sprintf(buffer,"%d",GetCount());
    oEvt += buffer;

    oEvt += SEPCHAR;
    sprintf(buffer,"%s", (const char*)GetText());
    oEvt += buffer;

    return DBMCli_String(oEvt);
}

/*! @brief private member  */
void DBMCliEvtCm_Event::SetProperty
    ( const DBMCli_String & sProperty )
{
  int nAssign = sProperty.Find("=");
  if (nAssign > 0) {
    DBMCli_String sName(sProperty.Left(nAssign));
    DBMCli_String sValue((nAssign+1>=sProperty.GetLength())?DBMCli_String(""):sProperty.Mid(nAssign+1));
    
    sName.Trim();
    sValue.Trim();
    
    if (sName.Compare("NAME")        == 0) {
      m_sName = sValue;
    } else if (sName.Compare("PRIORITY")    == 0) {
      if (sValue.Compare("LOW") == 0) {
        m_oPriority = prio_low;
      } else if (sValue.Compare("MEDIUM") == 0) {
        m_oPriority = prio_medium;
      } else if (sValue.Compare("HIGH") == 0) {
        m_oPriority = prio_high;
      } else  {
        m_oPriority = prio_unknown;
      } // end if
    } else if (sName.Compare("DATE")        == 0) {
      m_oDate.Set(sValue, DT_NUM_DATE_FMT);
    } else if (sName.Compare("TIME")        == 0) {
      m_oTime.Set(sValue, DT_NUM_TIME_FMT);
    } else if (sName.Compare("VALUE1")      == 0) {
      m_nValue1      = atoi(sValue);
    } else if (sName.Compare("VALUE2")      == 0) {
      m_nValue2      = atoi(sValue);
    } else if (sName.Compare("COUNT")       == 0) {
      m_nCount       = atoi(sValue);
    } else if (sName.Compare("TEXT")        == 0) {
      m_sText        = sValue;
    } else if (sName.Compare("DESCRIPTION") == 0) {
      m_sDescription = sValue;
    } // end if
  } // end if

} // end DBMCliEvtCm_Event::SetProperty

const char DBMCliEvtCm_Event::SEPCHAR = '/';