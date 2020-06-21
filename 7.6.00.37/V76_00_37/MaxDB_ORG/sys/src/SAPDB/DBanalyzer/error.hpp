/*!
  -----------------------------------------------------------------------------
  module: error.hpp
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

#ifndef _ERROR_HPP_
#define _ERROR_HPP_

#include "SAPDB/DBanalyzer/types.hpp"

typedef enum {
  DBA_OK            = 0,
  DBA_ERR,
  DBA_ERR_ARGS,
  DBA_ERR_OPENFILE,
  DBA_ERR_CONNECT,
  DBA_ERR_EXECUTE,
  DBA_ERR_INVCONFIGKEY,
  DBA_ERR_NOMEM,
  DBA_ERR_PARSE,
  DBA_ERR_CALC,
  DBA_ERR_TYPE,
  DBA_WRN_CALC,
  DBA_ERR_CREATEDIR,
  DBA_ERR_ACTIVE,
  DBA_ERR_OTHERDB,
  DBA_ERR_UNKNOWNOBJ,
  DBA_ERR_DUPLICATE,
  DBA_ERR_FORMTAB,
  DBA_ERR_KILL,
  DBA_ERR_UNKNOWNVAL,
  DBA_ERR_NOTACTIVE,
  DBA_ERR_PREPARE,
  DBA_ERR_RECONNECT,
  DBA_ERR_DISCONNECT,
  DBA_ERR_UNKNOWN
} DBA_errornum;

#define DBA_ERRTXT { \
  {DBA_OK,                "OK"                                                                 }, \
  {DBA_ERR,               "Common error."                                                      }, \
  {DBA_ERR_ARGS,          "Invalid/incomplete program arguments."                              }, \
  {DBA_ERR_OPENFILE,      "Cannot open file '%s'."                                             }, \
  {DBA_ERR_CONNECT,       "Cannot connect to database."                                        }, \
  {DBA_ERR_EXECUTE,       "Cannot execute SQL statement."                                      }, \
  {DBA_ERR_INVCONFIGKEY,  "Invalid key in configuration (Line %s)."                            }, \
  {DBA_ERR_NOMEM,         "Cannot allocate memory."                                            }, \
  {DBA_ERR_PARSE,         "Cannot parse expression for %s."                                    }, \
  {DBA_ERR_CALC,          "Cannot calculate expression for %s."                                }, \
  {DBA_ERR_TYPE,          "Expression result type do not match for %s."                        }, \
  {DBA_WRN_CALC,          "Cannot calculate expression for %s."                                }, \
  {DBA_ERR_CREATEDIR,     "Cannot create directory %s."                                        }, \
  {DBA_ERR_ACTIVE,        "Database Analyzer active in directory \"%s\"."                      }, \
  {DBA_ERR_OTHERDB,       "Data for database %s exist in directory \"%s\"."                    }, \
  {DBA_ERR_UNKNOWNOBJ,    "Unknown object %s."                                                 }, \
  {DBA_ERR_DUPLICATE,     "Duplicate object identifier %s (Line %s)."                          }, \
  {DBA_ERR_FORMTAB,       "Formula and Table property not allowed for parameter %s (Line %s)." }, \
  {DBA_ERR_KILL,          "Cannot stop process."                                               }, \
  {DBA_ERR_UNKNOWNVAL,    "Cannot find value %s in object %s."                                 }, \
  {DBA_ERR_NOTACTIVE,     "Database Analyzer not active in directory \"%s\"."                  }, \
  {DBA_ERR_PREPARE,       "Cannot prepare SQL statement."                                      }, \
  {DBA_ERR_RECONNECT,     "Option '-reconnect' has not been implemented yet."                  }, \
  {DBA_ERR_DISCONNECT,    "Session was disconnected."                                          }, \
  {DBA_ERR_UNKNOWN,       "Unknown error."                                                     }  \
} // end DBA_ERRTXT

// forward declaration
class DBA_protocol;

class DBA_error {
  public:
  DBA_error ();
  ~DBA_error ();

  DBA_void Clear ();

  DBA_errornum GetError() { return m_nNumber;}

  DBA_void SetError(const DBA_errornum nNumber);
  DBA_void SetError(const DBA_errornum nNumber,
                    const DBA_long     nNativeError);
  DBA_void SetError(const DBA_errornum nNumber, 
                    const DBA_string & sValue1);
  DBA_void SetError(const DBA_errornum nNumber, 
                    const DBA_string & sValue1,
                    const DBA_string & sValue2);
  DBA_void SetError(const DBA_errornum nNumber, 
                    const DBA_string & sValue1,
                    const DBA_string & sValue2,
                    const DBA_string & sValue3);

  DBA_void SetExtText(const DBA_string & sExtText); 

  DBA_string GetErrorMsg();
  DBA_long   GetNativeError() {return m_nNativeError;}
  DBA_void ShowErrorMsg( DBA_protocol * pProtocol, DBA_bool bInfo = false );

  DBA_void SetReconnect(const DBA_bool bReconnect);
  DBA_bool GetReconnect();

  DBA_void SetIgnore(const DBA_bool bIgnore);
  DBA_bool GetIgnore();

  private:
  DBA_errornum m_nNumber;
  DBA_int      m_nValues;
  DBA_string   m_sValue1;
  DBA_string   m_sValue2;
  DBA_string   m_sValue3;
  DBA_string   m_sExtText;
  DBA_bool     m_bReconnect;
  DBA_bool     m_bIgnore;
  DBA_long     m_nNativeError;
};


#endif