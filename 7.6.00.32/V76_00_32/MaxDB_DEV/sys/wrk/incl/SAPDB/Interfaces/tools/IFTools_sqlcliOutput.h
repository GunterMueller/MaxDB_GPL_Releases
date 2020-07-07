/*!**************************************************************************

  module      : IFTools_sqlcliOutput.h

  -------------------------------------------------------------------------

  responsible : D031096

  special area:
  description : Interface Runtime 8.0

  description:  Provides a class that handles the output for command line tool sqlcli

  last changed: 2003-01-30
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2003-2005 SAP AG


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




*****************************************************************************/
#ifndef IFTOOLS_SQLCLIOUTPUT_H
#define IFTOOLS_SQLCLIOUTPUT_H

#include "SAPDB/Interfaces/tools/IFTools_sqlcliProperties.h"
#define IFTOOLS_OUT_LONG_BUF 4096
 
/**
 * @ingroup sqlcli
 * @brief Handles the output for command line tool sqlcli. 
 */
class IFTools_sqlcliOutputPrinter;
 
class IFTools_sqlcliOutput {
public:
  IFTools_sqlcliOutput         (IFTools_sqlcliProperties   *m_sqlcliEnv);
  ~IFTools_sqlcliOutput        ();

  virtual void printError(SQLDBC_ErrorHndl error);
  virtual void printError(const char *fmt, ...);
  virtual void printCmdTrace(const char *fmt, ...);
  virtual SQLDBC_Bool printResult(const char *fmt, ...);
  virtual void printComment(const char *fmt, ...);

  SQLDBC_Bool registerResultSet(SQLDBC_ResultSetMetaData* rsmd);
  SQLDBC_Bool registerOutputValues(SQLDBC_ParameterMetaData* rsmd);
  SQLDBC_Bool printRow(SQLDBC_ResultSet *rs);
  SQLDBC_Bool printColumnNames();
  SQLDBC_Bool printOutputValues(SQLDBC_PreparedStatement *rs);

  inline SQLDBC_Bool printNullValue(SQLDBC_Int4 colLen){
    size_t len = strlen(this->m_sqlcliEnv->getnullvalue());
    return this->printResult("%-*.*s", colLen, 
                       ((SQLDBC_Int4)len > colLen)?colLen:(SQLDBC_Int4)len, 
                       this->m_sqlcliEnv->getnullvalue());
                       
  }

private:
  
 void deletePrinter(SQLDBC_Bool delArray); 
 SQLDBC_Bool printRecord(SQLDBC_ResultSet * rs, SQLDBC_Int4 colIndex, IFTools_sqlcliOutputPrinter *outp);
 SQLDBC_Bool printOutputValue(SQLDBC_PreparedStatement * rs, SQLDBC_Int4 colIndex, IFTools_sqlcliOutputPrinter *outp);
 IFTools_sqlcliOutputPrinter **m_printer;  
 SQLDBC_Int4 m_printerLen;
 char* m_buffer;
 SQLDBC_Int4 m_bufLen;
 IFTools_sqlcliProperties  *m_sqlcliEnv;     //!<environment     
};


class IFTools_sqlcliOutputPrinter {
  public:
    inline IFTools_sqlcliOutputPrinter (char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac, SQLDBC_HostType hostType):
    m_colname(colname),
    m_colLen(colLen),
    m_colFrac(colFrac),
    m_hostType(hostType),
    m_streamAllData(SQLDBC_FALSE)
    {}
     
    ~IFTools_sqlcliOutputPrinter   (){
      if (m_colname) delete m_colname;
    }
    
    inline SQLDBC_Int4      getColLength (){ return this->m_colLen;}
    inline const char*      getColName   (){ return this->m_colname;}
    inline SQLDBC_HostType  gethostType  (){ return this->m_hostType;}
    inline SQLDBC_Bool      streamAllData(){ return this->m_streamAllData;}
  
    virtual SQLDBC_Bool printColName(IFTools_sqlcliOutput *out)= 0;
    virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen)=0;
    virtual SQLDBC_Bool printNextChunk(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){return SQLDBC_TRUE;}

  protected:
    char        *m_colname;
    SQLDBC_Length   m_colLen;
    SQLDBC_Int4     m_colFrac;
    SQLDBC_HostType m_hostType;
    SQLDBC_Bool     m_streamAllData;
}; 

#endif
