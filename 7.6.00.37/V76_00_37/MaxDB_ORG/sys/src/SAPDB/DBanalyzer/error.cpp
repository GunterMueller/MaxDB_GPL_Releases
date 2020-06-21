/*!
  -----------------------------------------------------------------------------
  module: error.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  dbanalyzer error handling
  version:      7.4 and higher 
  -----------------------------------------------------------------------------

  Copyright (c) 2002-2005 SAP AG


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




  -----------------------------------------------------------------------------
*/

#include "SAPDB/DBanalyzer/error.hpp"
#include "SAPDB/DBanalyzer/protocol.hpp"

DBA_error::DBA_error ()
{
    Clear();
}

DBA_error::~DBA_error ()
{
}

DBA_void DBA_error::Clear ()
{
  m_nNumber      = DBA_OK;
  m_nValues      = 0;
  m_sValue1      = "";;
  m_sValue2      = "";;
  m_sValue3      = "";;
  m_sExtText     = "";
  m_bReconnect   = false;
  m_bIgnore      = false;
  m_nNativeError = 0;
}

DBA_void DBA_error::SetError (const DBA_errornum nNumber)
{
  Clear();
  m_nNumber = nNumber;
}

DBA_void DBA_error::SetError (const DBA_errornum nNumber,
                              const DBA_long     nNativeError)
{
  Clear();
  m_nNumber      = nNumber;
  m_nNativeError = nNativeError;
}

DBA_void DBA_error::SetError(const DBA_errornum nNumber, 
                             const DBA_string & sValue1)
{
  Clear();
  m_nNumber = nNumber;
  m_nValues = 1;
  m_sValue1 = sValue1;
}

DBA_void DBA_error::SetError(const DBA_errornum nNumber, 
                             const DBA_string & sValue1,
                             const DBA_string & sValue2)
{
  Clear();
  m_nNumber = nNumber;
  m_nValues = 2;
  m_sValue1 = sValue1;
  m_sValue2 = sValue2;
}

DBA_void DBA_error::SetError(const DBA_errornum nNumber, 
                             const DBA_string & sValue1,
                             const DBA_string & sValue2,
                             const DBA_string & sValue3)
{
  Clear();
  m_nNumber = nNumber;
  m_nValues = 3;
  m_sValue1 = sValue1;
  m_sValue2 = sValue2;
  m_sValue3 = sValue3;
}

DBA_void DBA_error::SetExtText(const DBA_string & sExtText)
{
  m_sExtText = sExtText;
}

DBA_string DBA_error::GetErrorMsg()
{
  DBA_string sError;
  struct {
    DBA_errornum  nValue;
    const char  * szText;
  }               errorArray[] = DBA_ERRTXT;
  int nIndex = 0;
  
  for (nIndex = 0; errorArray[nIndex].nValue != DBA_ERR_UNKNOWN; ++nIndex)
      if (errorArray[nIndex].nValue == m_nNumber)
          break;

  sError = errorArray[nIndex].szText;

  if (m_nValues > 0) {
    DBA_char   sBuffer[1024];

    switch (m_nValues) {
      case 1:
        sprintf(sBuffer, sError.CharPtr(), m_sValue1.CharPtr());
        break;  

      case 2:
        sprintf(sBuffer, sError.CharPtr(), m_sValue1.CharPtr(),
                                           m_sValue2.CharPtr());
        break;  
      case 3:
        sprintf(sBuffer, sError.CharPtr(), m_sValue1.CharPtr(),
                                           m_sValue2.CharPtr(),
                                           m_sValue3.CharPtr());
        break;  
      default:
        sprintf(sBuffer, sError.CharPtr());
        break;  
    }
    sError = DBA_string(sBuffer);
  }

  return sError;
} // end GetErrorMsg

//
// DBA_error::ShowErrorMsg
//
DBA_void DBA_error::ShowErrorMsg(DBA_protocol * pProtocol, DBA_bool bInfo) 
{
  DBA_string sClass;

  if (bInfo) {
    sClass = "INFO";
  } else {
    sClass = (m_nNumber == DBA_WRN_CALC) ? "WARNING" : "ERROR";
  } // end if

  DBA_string sError = GetErrorMsg();
  
  if (pProtocol != NULL) {
    pProtocol->WriteError( sClass, m_nNumber, sError, m_sExtText);
  } else {
    printf("%s %d: %s\n", sClass.CharPtr(), (int) m_nNumber, sError.CharPtr());
    if (!m_sExtText.Empty()) {
      printf("%s\n\n", m_sExtText.CharPtr());
    } else {
      printf("\n");
    } // end if
  } // end if
} // end DBA_error::ShowErrorMsg

DBA_void DBA_error::SetReconnect(const DBA_bool bReconnect)
{
  m_bReconnect = bReconnect;
}

DBA_bool DBA_error::GetReconnect()
{
  return m_bReconnect;
}

DBA_void DBA_error::SetIgnore(const DBA_bool bIgnore)
{
  m_bIgnore = bIgnore;
}

DBA_bool DBA_error::GetIgnore()
{
  return m_bIgnore;
}

