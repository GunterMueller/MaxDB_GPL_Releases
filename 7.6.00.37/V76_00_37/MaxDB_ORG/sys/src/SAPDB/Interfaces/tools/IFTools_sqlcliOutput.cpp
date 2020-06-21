/*!**************************************************************************

module      : IFTools_sqlcliOutput.cpp

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

#include "SAPDB/Interfaces/tools/IFTools_sqlcliOutput.h"
#include <stdarg.h>

class IFTools_sqlcliOutputDefPrinter : public IFTools_sqlcliOutputPrinter {
public:
  IFTools_sqlcliOutputDefPrinter (char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac, SQLDBC_HostType hostType):
      IFTools_sqlcliOutputPrinter(colname, colLen, colFrac, hostType) {}

      ~IFTools_sqlcliOutputDefPrinter   (){}

      virtual SQLDBC_Bool printColName(IFTools_sqlcliOutput *out){
        if (m_colLen){
          return out->printResult("%-*.*s",m_colLen,(m_colLen>17)?17:m_colLen, m_colname);
        }
        return SQLDBC_OK;
      }	
      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return out->printResult("%-*.*s", m_colLen, (bufLen > m_colLen)?m_colLen:bufLen, buffer);        
      }
}; 
//----------------------------------------------------------------------------------

class IFTools_sqlcliOutputFixPointPrinter: public IFTools_sqlcliOutputDefPrinter {
public:
  IFTools_sqlcliOutputFixPointPrinter (char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputDefPrinter(colname, colLen, colFrac, SQLDBC_HOSTTYPE_ASCII)
      {
        this->computeNumColLen();
      }

      ~IFTools_sqlcliOutputFixPointPrinter   (){}

      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return out->printResult("%*.*s", m_colLen, (bufLen > m_colLen)?m_colLen:bufLen, buffer);        
      }

private:
  void computeNumColLen()
  {
    SQLDBC_Int korr = ((m_colLen % 3))?0:1;
    m_colLen += (m_colLen/3-korr)+1; //+1 for minus
    if (m_colFrac) m_colLen++; // decimal point
  }
};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputFloatPrinter: public IFTools_sqlcliOutputDefPrinter {
public:
  IFTools_sqlcliOutputFloatPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputDefPrinter(colname, colLen+6, colFrac, SQLDBC_HOSTTYPE_ASCII)
      {}
      ~IFTools_sqlcliOutputFloatPrinter   (){}

      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return out->printResult("%*.*s", m_colLen, (bufLen > m_colLen)?m_colLen:bufLen, buffer);        
      }

};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputBinaryPrinter: public IFTools_sqlcliOutputDefPrinter {
public:
  IFTools_sqlcliOutputBinaryPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputDefPrinter(colname, colLen*2+2, colFrac, SQLDBC_HOSTTYPE_BINARY)
      {}
      ~IFTools_sqlcliOutputBinaryPrinter   (){}

      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        SQLDBC_Int4  maxOutLen = ((m_colLen-2) < bufLen*2)?(m_colLen/2-1):bufLen; 
        if (out->printResult("0x")== SQLDBC_FALSE) return SQLDBC_FALSE;
        return printBinaryData(out, buffer, maxOutLen);
      }

protected:
      SQLDBC_Bool printBinaryData(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        unsigned char *ptr;
        SQLDBC_Int4 i;
        for (ptr=(unsigned char *)buffer, i=0; i<bufLen; ++i,ptr++)
          if (out->printResult("%02X",*ptr)==SQLDBC_FALSE) return SQLDBC_FALSE;
        return out->printResult("%*s", m_colLen - (i+1)*2, "");        
      };
};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputUnicodePrinter: public IFTools_sqlcliOutputDefPrinter {
public:
  IFTools_sqlcliOutputUnicodePrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputDefPrinter(colname, colLen, colFrac, SQLDBC_HOSTTYPE_UTF8)
      {}
      ~IFTools_sqlcliOutputUnicodePrinter   (){}
      
      virtual SQLDBC_Bool printColName(IFTools_sqlcliOutput *out){
        if (m_colLen){
          return out->printResult("%-*.*s",m_colLen,(m_colLen>17)?17:m_colLen, m_colname);
        }
        return SQLDBC_TRUE;
      }	
      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return out->printResult("%-*.*s", m_colLen, (bufLen > m_colLen)?m_colLen:bufLen, buffer);        
      }

};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputBooleanPrinter: public IFTools_sqlcliOutputDefPrinter {
public:
  IFTools_sqlcliOutputBooleanPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputDefPrinter(colname, 5, colFrac, SQLDBC_HOSTTYPE_INT1)
      {}
      ~IFTools_sqlcliOutputBooleanPrinter   (){}

      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        SQLDBC_Int1* erg = (SQLDBC_Int1*)buffer;
        return out->printResult("% 5s", (*erg)?"TRUE":"FALSE");
      };
};

//----------------------------------------------------------------------
class IFTools_sqlcliOutputLongBinaryPrinter: public IFTools_sqlcliOutputBinaryPrinter {
public:
  IFTools_sqlcliOutputLongBinaryPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputBinaryPrinter(colname, colLen, colFrac)
      {}
      ~IFTools_sqlcliOutputLongBinaryPrinter   (){}

};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputLongUniPrinter: public IFTools_sqlcliOutputUnicodePrinter {
public:
  IFTools_sqlcliOutputLongUniPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputUnicodePrinter(colname, 2*colLen, colFrac)
      {}
      ~IFTools_sqlcliOutputLongUniPrinter   (){}
};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputLongAsciiPrinter: public IFTools_sqlcliOutputDefPrinter {
public:
  IFTools_sqlcliOutputLongAsciiPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputDefPrinter(colname, colLen, colFrac, SQLDBC_HOSTTYPE_ASCII)
      {}
      ~IFTools_sqlcliOutputLongAsciiPrinter   (){}
};

//----------------------------------------------------------------------
class IFTools_sqlcliOutputWholeLongBinaryPrinter: public IFTools_sqlcliOutputBinaryPrinter {
public:
      IFTools_sqlcliOutputWholeLongBinaryPrinter(char *colname, SQLDBC_Int4 colFrac):
        IFTools_sqlcliOutputBinaryPrinter(colname, strlen(colname), colFrac)
        { m_colLen = IFTOOLS_OUT_LONG_BUF;
          m_streamAllData = SQLDBC_TRUE;}
          
      ~IFTools_sqlcliOutputWholeLongBinaryPrinter   (){}

      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        if (out->printResult("0x")==SQLDBC_FALSE) return SQLDBC_FALSE;
        return printBinaryData(out, buffer, bufLen);
      }       
      
      virtual SQLDBC_Bool printNextChunk(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return printBinaryData(out, buffer, bufLen);
      }   
};
//----------------------------------------------------------------------
class IFTools_sqlcliOutputWholeLongAsciiPrinter: public IFTools_sqlcliOutputLongAsciiPrinter {
public:
  IFTools_sqlcliOutputWholeLongAsciiPrinter(char *colname, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputLongAsciiPrinter(colname, strlen(colname), colFrac)
      {m_streamAllData = SQLDBC_TRUE;}
      ~IFTools_sqlcliOutputWholeLongAsciiPrinter   (){}

      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return out->printResult("%.*s", bufLen, buffer);
      }       

      virtual SQLDBC_Bool printNextChunk(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return printValue(out, buffer, bufLen);
      }   

};//----------------------------------------------------------------------
class IFTools_sqlcliOutputWholeLongUniPrinter: public IFTools_sqlcliOutputLongUniPrinter {
public:
  IFTools_sqlcliOutputWholeLongUniPrinter(char *colname, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputLongUniPrinter(colname, strlen(colname), colFrac)
      {m_streamAllData = SQLDBC_TRUE;}
      ~IFTools_sqlcliOutputWholeLongUniPrinter   (){}
      
      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return out->printResult("%.*s", bufLen, buffer);
      }       

      virtual SQLDBC_Bool printNextChunk(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return printValue(out, buffer, bufLen);
      }   
};

//----------------------------------------------------------------------
class IFTools_sqlcliOutputUnAlignedDefPrinter : public IFTools_sqlcliOutputPrinter {
public:
  IFTools_sqlcliOutputUnAlignedDefPrinter (char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac, SQLDBC_HostType hostType):
      IFTools_sqlcliOutputPrinter(colname, colLen, colFrac, hostType) {}

      ~IFTools_sqlcliOutputUnAlignedDefPrinter   (){}

      virtual SQLDBC_Bool printColName(IFTools_sqlcliOutput *out){
        if (m_colLen){
          return out->printResult("%s", m_colname);
        }
        return SQLDBC_FALSE;
      }	
      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        SQLDBC_Length i, curLen=0, curPos=0;
        if (out->printResult("\"")==SQLDBC_FALSE) return SQLDBC_FALSE;
        for (i=0; i< bufLen; i++ ){
          switch(buffer[i]) {
            case('\"'):{
              if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
              if (out->printResult("\\\"")==SQLDBC_FALSE) return SQLDBC_FALSE; 
              curPos+=1;
              break;
            }
            case('\n'):{
              if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
              if (out->printResult("\\n")==SQLDBC_FALSE) return SQLDBC_FALSE; curPos+=1;
              break;
            }
            case('\b'):{
              if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
              if (out->printResult("\\b")==SQLDBC_FALSE) return SQLDBC_FALSE; curPos+=1;
              break;
            }
            case('\r'):{
              if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
              if (out->printResult("\\r")==SQLDBC_FALSE) return SQLDBC_FALSE; curPos+=1;
              break;
            }
            case('\a'):{
              if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
              if (out->printResult("\\a")==SQLDBC_FALSE) return SQLDBC_FALSE; curPos+=1;
              break;
            }
            case('\f'):{
              if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
              if (out->printResult("\\f")==SQLDBC_FALSE) return SQLDBC_FALSE; curPos+=1;
              break;
            }
            case('\t'):{
              if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
              if (out->printResult("\\t")==SQLDBC_FALSE) return SQLDBC_FALSE; curPos+=2;
              break;
            }
            case('\\'):{
              if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
              if (out->printResult("\\\\")==SQLDBC_FALSE) return SQLDBC_FALSE; curPos+=1;
              break;
            }
            default:{
              curLen++;
              break;
            }
          }
        }        
        if (this->printRest(out, buffer, &curPos, &curLen)==SQLDBC_FALSE) return SQLDBC_FALSE;
        if (out->printResult("\"")==SQLDBC_FALSE) return SQLDBC_FALSE;
        return SQLDBC_TRUE;
      }
   private:
     SQLDBC_Bool printRest(IFTools_sqlcliOutput *out, char* buf, SQLDBC_Length* pos, SQLDBC_Length* len ){
       if (*len) {
        if (out->printResult("%.*s", *len, &buf[(*pos)])==SQLDBC_FALSE) return SQLDBC_FALSE;
        *pos += *len;
        *len=0;
       }
       return SQLDBC_TRUE;  
     }    
}; 
//----------------------------------------------------------------------------------

class IFTools_sqlcliOutputUnalignedFixPointPrinter: public IFTools_sqlcliOutputFixPointPrinter {
public:
  IFTools_sqlcliOutputUnalignedFixPointPrinter (char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputFixPointPrinter(colname, colLen, colFrac)
      {}

      ~IFTools_sqlcliOutputUnalignedFixPointPrinter   (){}

      virtual SQLDBC_Bool printColName(IFTools_sqlcliOutput *out){
        return out->printResult("%s", m_colname);
      }	
      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return out->printResult("%s",buffer);        
      }
};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputUnalignedFloatPrinter: public IFTools_sqlcliOutputFloatPrinter {
public:
  IFTools_sqlcliOutputUnalignedFloatPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputFloatPrinter(colname, colLen+6, colFrac)
      {}
      ~IFTools_sqlcliOutputUnalignedFloatPrinter   (){}

      virtual SQLDBC_Bool printColName(IFTools_sqlcliOutput *out){
        return out->printResult("%s", m_colname);
      }	
      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        return out->printResult("%s", buffer);        
      }

};
//----------------------------------------------------------------------

class IFTools_sqlcliOutputUnalignedBinaryPrinter: public IFTools_sqlcliOutputUnAlignedDefPrinter {
public:
  IFTools_sqlcliOutputUnalignedBinaryPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputUnAlignedDefPrinter(colname, colLen*2+2, colFrac, SQLDBC_HOSTTYPE_BINARY)
      {}
      ~IFTools_sqlcliOutputUnalignedBinaryPrinter   (){}

      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        unsigned char *ptr;
        SQLDBC_Int4 i, maxOutLen = ((m_colLen-2) < bufLen*2)?(m_colLen/2-1):bufLen; 
        if (out->printResult("0x")==SQLDBC_FALSE) return SQLDBC_FALSE;
        for (ptr=(unsigned char *)buffer, i=0; i<maxOutLen; i++,ptr++)
          if (out->printResult("%02X",*ptr)==SQLDBC_FALSE) return SQLDBC_FALSE;
        return SQLDBC_TRUE;
      };
};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputUnalignedBooleanPrinter: public IFTools_sqlcliOutputUnAlignedDefPrinter {
public:
  IFTools_sqlcliOutputUnalignedBooleanPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputUnAlignedDefPrinter(colname, 5, colFrac, SQLDBC_HOSTTYPE_INT1)
      {}
      ~IFTools_sqlcliOutputUnalignedBooleanPrinter   (){}

      virtual SQLDBC_Bool printValue(IFTools_sqlcliOutput *out, char* buffer, SQLDBC_Int4 bufLen){
        SQLDBC_Int1* erg = (SQLDBC_Int1*)buffer;
        return out->printResult("%s", (*erg)?"TRUE":"FALSE");
      };
};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputUnalignedUniPrinter: public IFTools_sqlcliOutputUnAlignedDefPrinter {
public:
  IFTools_sqlcliOutputUnalignedUniPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputUnAlignedDefPrinter(colname, colLen*2, colFrac, SQLDBC_HOSTTYPE_UTF8)
      {}
      ~IFTools_sqlcliOutputUnalignedUniPrinter   (){}

};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputUnalignedLongBinaryPrinter: public IFTools_sqlcliOutputUnalignedBinaryPrinter {
public:
  IFTools_sqlcliOutputUnalignedLongBinaryPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputUnalignedBinaryPrinter(colname, colLen, colFrac)
      {}
      ~IFTools_sqlcliOutputUnalignedLongBinaryPrinter   (){}

};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputUnalignedLongUniPrinter: public IFTools_sqlcliOutputUnalignedUniPrinter {
public:
  IFTools_sqlcliOutputUnalignedLongUniPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputUnalignedUniPrinter(colname, 2*colLen, colFrac)
      {}
      ~IFTools_sqlcliOutputUnalignedLongUniPrinter   (){}
};

//----------------------------------------------------------------------

class IFTools_sqlcliOutputUnalignedLongAsciiPrinter: public IFTools_sqlcliOutputUnAlignedDefPrinter {
public:
  IFTools_sqlcliOutputUnalignedLongAsciiPrinter(char *colname, SQLDBC_Int4 colLen, SQLDBC_Int4 colFrac):
      IFTools_sqlcliOutputUnAlignedDefPrinter(colname, colLen, colFrac, SQLDBC_HOSTTYPE_ASCII)
      {}
      ~IFTools_sqlcliOutputUnalignedLongAsciiPrinter   (){}
};


//----------------------------------------------------------------------
IFTools_sqlcliOutput::IFTools_sqlcliOutput(IFTools_sqlcliProperties *sqlcliProp):
m_sqlcliEnv(sqlcliProp),
m_printer(0),
m_printerLen(0),
m_buffer(0),
m_bufLen(0)
{}

//----------------------------------------------------------------------
IFTools_sqlcliOutput::~IFTools_sqlcliOutput(){
  this->deletePrinter(SQLDBC_TRUE);
  delete m_buffer;
  m_bufLen=0;
}

//----------------------------------------------------------------------
void IFTools_sqlcliOutput::deletePrinter(SQLDBC_Bool delArray){
  if (m_printer){
    for (int i=0; i<m_printerLen; ++i){
      delete m_printer[i];
    }   
    if (delArray) delete []m_printer;
    m_printerLen = 0;
  }
} 
//----------------------------------------------------------------------

void IFTools_sqlcliOutput::printError(SQLDBC_ErrorHndl error)
{
  if ( strlen(error.getSQLState()))
    fprintf (m_sqlcliEnv->geterrfile(),"* %d: %s SQLSTATE: %s\n", error.getErrorCode(),
    error.getErrorText(),
    error.getSQLState());
  else
    fprintf (m_sqlcliEnv->geterrfile(),"* %d: %s\n", error.getErrorCode(),
    error.getErrorText());
    this->m_sqlcliEnv->forceProgramExit();
}

//----------------------------------------------------------------------

void IFTools_sqlcliOutput::printError(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(m_sqlcliEnv->geterrfile(), fmt, ap);
  va_end(ap); 
  this->m_sqlcliEnv->forceProgramExit();
}

//----------------------------------------------------------------------
void IFTools_sqlcliOutput::printCmdTrace(const char *fmt, ...){
  if (m_sqlcliEnv->getcmdtrace()){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap); 
  }
}
//----------------------------------------------------------------------
void IFTools_sqlcliOutput::printComment(const char *fmt, ...){
  if (!m_sqlcliEnv->getnoComment()){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap); 
  }
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliOutput::printResult(const char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  
  errno=0;
  
  vfprintf(m_sqlcliEnv->getoutfile(), fmt, ap);
  
  if(errno){
    return SQLDBC_FALSE;
  }
  
  va_end(ap);
  return SQLDBC_TRUE; 
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliOutput::registerResultSet(SQLDBC_ResultSetMetaData* rsmd){

  SQLDBC_Int4 length,frac,maxColLength=0; 
  SQLDBC_Length bufferlength;
  SQLDBC_Bool aligned = m_sqlcliEnv->getalignedOutput();
  m_printerLen = rsmd->getColumnCount();
  m_printer = new IFTools_sqlcliOutputPrinter* [m_printerLen];

  for(SQLDBC_Int2 i = 1; i <= m_printerLen; i++)
  {
    char *colName = new char[m_sqlcliEnv->maxKnlIdentifierUTF8];

    if (//rsmd->getColumnName (i, colName, SQLDBC_StringEncodingUTF8, m_sqlcliEnv->maxKnlIdentifierUTF8)
      rsmd->getColumnName(i, colName,
      SQLDBC_StringEncodingUTF8,
      m_sqlcliEnv->maxKnlIdentifierUTF8,
      &bufferlength)

      ) {
        this->printError (ERR_GET_COLUMNNAME, i);
        this->deletePrinter(SQLDBC_TRUE);                  
        return SQLDBC_FALSE;                  
      }
      frac    = rsmd->getScale(i); 
      length  = strlen (colName); 
      if (rsmd->getColumnLength(i)>length)length = rsmd->getColumnLength(i);

      switch(rsmd->getColumnType (i))
      {
      case SQLDBC_SQLTYPE_NUMBER        :
      case SQLDBC_SQLTYPE_FIXED         :{
        length += 2;
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputFixPointPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedFixPointPrinter(colName, length, frac);
        break;
      } 
      case SQLDBC_SQLTYPE_SMALLINT      :
      case SQLDBC_SQLTYPE_INTEGER       : {
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputFixPointPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedFixPointPrinter(colName, length, frac);
        break;
                                          }

      case SQLDBC_SQLTYPE_FLOAT         :
      case SQLDBC_SQLTYPE_VFLOAT        :{
        length += 2;
        m_printer[i-1] = (aligned)
        ?
        new   IFTools_sqlcliOutputFloatPrinter(colName, length, frac)
        :
        new   IFTools_sqlcliOutputUnalignedFloatPrinter(colName, length, frac); 
        break;                         
                                         }
      case SQLDBC_SQLTYPE_CHB           :
      case SQLDBC_SQLTYPE_ROWID         :
      case SQLDBC_SQLTYPE_VARCHARB      :{
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputBinaryPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedBinaryPrinter(colName, length, frac);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_CHA           :               
      case SQLDBC_SQLTYPE_CHE           :
      case SQLDBC_SQLTYPE_VARCHARA      :
      case SQLDBC_SQLTYPE_VARCHARE      :

      case SQLDBC_SQLTYPE_DATE          :
      case SQLDBC_SQLTYPE_TIME          :
      case SQLDBC_SQLTYPE_TIMESTAMP     :{
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputDefPrinter(colName, length, frac, SQLDBC_HOSTTYPE_ASCII);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnAlignedDefPrinter(colName, length, frac, SQLDBC_HOSTTYPE_ASCII);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_UNICODE       :
      case SQLDBC_SQLTYPE_VARCHARUNI    :{
        if (aligned) 
          m_printer[i-1] = new   IFTools_sqlcliOutputUnicodePrinter(colName,length,frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedUniPrinter(colName,length,frac);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_BOOLEAN       :{
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputBooleanPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedBooleanPrinter(colName, length, frac);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_ABAPTABHANDLE :{
        break;
                                         }

      case SQLDBC_SQLTYPE_STRB          :
      case SQLDBC_SQLTYPE_LONGB         :{
        length = m_sqlcliEnv->getmaxLongLenth();   
        if (length == IFTOOLS_PROP_WHOLE_LONG){
          length = IFTOOLS_OUT_LONG_BUF;
          m_printer[i-1] = new   IFTools_sqlcliOutputWholeLongBinaryPrinter(colName, frac);
        } else if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputLongBinaryPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedLongBinaryPrinter(colName, length, frac);
        break;                         
                                         }

      case SQLDBC_SQLTYPE_STRA          :
      case SQLDBC_SQLTYPE_STRE          :
      case SQLDBC_SQLTYPE_LONGA         :
      case SQLDBC_SQLTYPE_LONGE         :{
        length = m_sqlcliEnv->getmaxLongLenth();   
        if (length == IFTOOLS_PROP_WHOLE_LONG){
          length = IFTOOLS_OUT_LONG_BUF;
          m_printer[i-1] = new   IFTools_sqlcliOutputWholeLongAsciiPrinter(colName, frac);
        } else if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputLongAsciiPrinter(colName, length, frac);
        else  
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedLongAsciiPrinter(colName, length, frac);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_STRUNI        :
      case SQLDBC_SQLTYPE_LONGUNI       :{
        length = m_sqlcliEnv->getmaxLongLenth();   
        if (length == IFTOOLS_PROP_WHOLE_LONG){
          length = IFTOOLS_OUT_LONG_BUF;
          m_printer[i-1] = new   IFTools_sqlcliOutputWholeLongUniPrinter(colName, frac);
        } else if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputLongUniPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedLongUniPrinter(colName, length, frac);
        break;                         
                                         }  
      default: {// unknown type - should not occur 
        this->printError (ERR_UNKNOWN_COLTYPE, rsmd->getColumnType (i), i);
        this->deletePrinter(SQLDBC_TRUE);                  
        return SQLDBC_FALSE;                  
               }
      } /*end switch*/ 
      if (maxColLength<length) maxColLength = length;
  } /*end for*/
  if ((++maxColLength) > m_bufLen){
    delete m_buffer;        
    m_buffer = new char[maxColLength];
    m_bufLen = maxColLength; 
  }
  return SQLDBC_TRUE;
}        

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliOutput::registerOutputValues(SQLDBC_ParameterMetaData* rsmd){

  SQLDBC_Int4 length,frac,maxColLength=0; 
  SQLDBC_Length bufferlength;
  SQLDBC_Bool aligned = m_sqlcliEnv->getalignedOutput();
  m_printerLen = rsmd->getParameterCount();
  m_printer = new IFTools_sqlcliOutputPrinter* [m_printerLen];

  for(SQLDBC_Int2 i = 1; i <= m_printerLen; i++)
  {
	if (rsmd->getParameterMode(i) == SQLDBC_ParameterMetaData::parameterModeIn) {
	  continue;
	}
    char *colName = new char[m_sqlcliEnv->maxKnlIdentifierUTF8];

    if (//rsmd->getColumnName (i, colName, SQLDBC_StringEncodingUTF8, m_sqlcliEnv->maxKnlIdentifierUTF8)
	  rsmd->getParameterName(i, colName,
      SQLDBC_StringEncodingUTF8,
      m_sqlcliEnv->maxKnlIdentifierUTF8,
      &bufferlength)

      ) {
        this->printError (ERR_GET_COLUMNNAME, i);
        this->deletePrinter(SQLDBC_TRUE);                  
        return SQLDBC_FALSE;                  
      }
      frac    = rsmd->getScale(i); 
      length  = strlen (colName); 
	  if (rsmd->getParameterLength(i)>length)length = rsmd->getParameterLength(i);

	  switch(rsmd->getParameterType (i))
      {
      case SQLDBC_SQLTYPE_FIXED         :
      case SQLDBC_SQLTYPE_NUMBER        :
      case SQLDBC_SQLTYPE_SMALLINT      :
      case SQLDBC_SQLTYPE_INTEGER       : {
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputFixPointPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedFixPointPrinter(colName, length, frac);
        break;
                                          }

      case SQLDBC_SQLTYPE_FLOAT         :
      case SQLDBC_SQLTYPE_VFLOAT        :{
        m_printer[i-1] = (aligned)
        ?
        new   IFTools_sqlcliOutputFloatPrinter(colName, length, frac)
        :
        new   IFTools_sqlcliOutputUnalignedFloatPrinter(colName, length, frac); 
        break;                         
                                         }
      case SQLDBC_SQLTYPE_CHB           :
      case SQLDBC_SQLTYPE_ROWID         :
      case SQLDBC_SQLTYPE_VARCHARB      :{
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputBinaryPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedBinaryPrinter(colName, length, frac);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_CHA           :               
      case SQLDBC_SQLTYPE_CHE           :
      case SQLDBC_SQLTYPE_VARCHARA      :
      case SQLDBC_SQLTYPE_VARCHARE      :

      case SQLDBC_SQLTYPE_DATE          :
      case SQLDBC_SQLTYPE_TIME          :
      case SQLDBC_SQLTYPE_TIMESTAMP     :{
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputDefPrinter(colName, length, frac, SQLDBC_HOSTTYPE_ASCII);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnAlignedDefPrinter(colName, length, frac, SQLDBC_HOSTTYPE_ASCII);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_UNICODE       :
      case SQLDBC_SQLTYPE_VARCHARUNI    :{
        if (aligned) 
          m_printer[i-1] = new   IFTools_sqlcliOutputUnicodePrinter(colName,length,frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedUniPrinter(colName,length,frac);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_BOOLEAN       :{
        if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputBooleanPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedBooleanPrinter(colName, length, frac);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_ABAPTABHANDLE :{
        break;
                                         }

      case SQLDBC_SQLTYPE_STRB          :
      case SQLDBC_SQLTYPE_LONGB         :{
        length = m_sqlcliEnv->getmaxLongLenth();   
        if (length == IFTOOLS_PROP_WHOLE_LONG){
          length = IFTOOLS_OUT_LONG_BUF;
          m_printer[i-1] = new   IFTools_sqlcliOutputWholeLongBinaryPrinter(colName, frac);
        } else if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputLongBinaryPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedLongBinaryPrinter(colName, length, frac);
        break;                         
                                         }

      case SQLDBC_SQLTYPE_STRA          :
      case SQLDBC_SQLTYPE_STRE          :
      case SQLDBC_SQLTYPE_LONGA         :
      case SQLDBC_SQLTYPE_LONGE         :{
        length = m_sqlcliEnv->getmaxLongLenth();   
        if (length == IFTOOLS_PROP_WHOLE_LONG){
          length = IFTOOLS_OUT_LONG_BUF;
          m_printer[i-1] = new   IFTools_sqlcliOutputWholeLongAsciiPrinter(colName, frac);
        } else if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputLongAsciiPrinter(colName, length, frac);
        else  
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedLongAsciiPrinter(colName, length, frac);
        break;                         
                                         }
      case SQLDBC_SQLTYPE_STRUNI        :
      case SQLDBC_SQLTYPE_LONGUNI       :{
        length = m_sqlcliEnv->getmaxLongLenth();   
        if (length == IFTOOLS_PROP_WHOLE_LONG){
          length = IFTOOLS_OUT_LONG_BUF;
          m_printer[i-1] = new   IFTools_sqlcliOutputWholeLongUniPrinter(colName, frac);
        } else if (aligned)
          m_printer[i-1] = new   IFTools_sqlcliOutputLongUniPrinter(colName, length, frac);
        else
          m_printer[i-1] = new   IFTools_sqlcliOutputUnalignedLongUniPrinter(colName, length, frac);
        break;                         
                                         }  
      default: {// unknown type - should not occur 
		  this->printError (ERR_UNKNOWN_COLTYPE, rsmd->getParameterType (i), i);
        this->deletePrinter(SQLDBC_TRUE);                  
        return SQLDBC_FALSE;                  
               }
      } /*end switch*/ 
      if (maxColLength<length) maxColLength = length;
  } /*end for*/
  if ((++maxColLength) > m_bufLen){
    delete m_buffer;        
    m_buffer = new char[maxColLength];
    m_bufLen = maxColLength; 
  }
  return SQLDBC_TRUE;
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliOutput::printColumnNames(){
  if (this->m_sqlcliEnv->getnoHeader()) return SQLDBC_TRUE;
  SQLDBC_Int4 length, rowlength=0;
  if (this->printResult("%s",this->m_sqlcliEnv->getrowprefix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
  for(SQLDBC_Int2 i = 0; i < m_printerLen; i++)
  {
    if (0 != (length = m_printer[i]->getColLength()))
    {
      if (this->printResult("%s",this->m_sqlcliEnv->getcolumnprefix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
      if (this->printResult("%-*.*s",length,(length>18)?18:length, m_printer[i]->getColName())==SQLDBC_FALSE) return SQLDBC_FALSE;
      if (this->printResult("%s",this->m_sqlcliEnv->getcolumnsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    }
  }
  if (this->printResult("%s",this->m_sqlcliEnv->getrowsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 

  if (this->m_sqlcliEnv->getalignedOutput()){
    if (this->printResult("%s",this->m_sqlcliEnv->getrowprefix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    for(SQLDBC_Int2 m = 0; m < m_printerLen; m++)
    {
      if (this->printResult("%s",this->m_sqlcliEnv->getcolumnprefix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
      for (SQLDBC_Int2 l=0;l<m_printer[m]->getColLength();l++) this->printResult("-"); /* underline */
      if (this->printResult("%s",this->m_sqlcliEnv->getcolumnsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    }
    if (this->printResult("%s",this->m_sqlcliEnv->getrowsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
  }
  return SQLDBC_TRUE;
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliOutput::printOutputValues(SQLDBC_PreparedStatement *rs){
  if (this->printResult("%s",this->m_sqlcliEnv->getrowprefix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
  for(SQLDBC_Int2 i = 0; i < m_printerLen; i++){
    if (this->printResult("%s",this->m_sqlcliEnv->getcolumnprefix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    if( !  this->printOutputValue(rs, i+1, m_printer[i])) return SQLDBC_FALSE;     
    if (this->printResult("%s",this->m_sqlcliEnv->getcolumnsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE;
  }
  if (this->printResult("%s",this->m_sqlcliEnv->getrowsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
  return SQLDBC_TRUE;
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliOutput::printRow(SQLDBC_ResultSet *rs){
  if (this->printResult("%s",this->m_sqlcliEnv->getrowprefix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
  for(SQLDBC_Int2 i = 0; i < m_printerLen; i++){
    if (this->printResult("%s",this->m_sqlcliEnv->getcolumnprefix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    if( !  this->printRecord(rs, i+1, m_printer[i])) return SQLDBC_FALSE;     
    if (this->printResult("%s",this->m_sqlcliEnv->getcolumnsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE;
  }
  if (this->printResult("%s",this->m_sqlcliEnv->getrowsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
  return SQLDBC_TRUE;
}
//----------------------------------------------------------------------
SQLDBC_Bool  IFTools_sqlcliOutput::printRecord(SQLDBC_ResultSet * rs, SQLDBC_Int4 colIndex, IFTools_sqlcliOutputPrinter *outp){
  SQLDBC_Length indicator=0;
  SQLDBC_Retcode erg = rs->getRowSet()->getObject(colIndex, outp->gethostType(), m_buffer,  &indicator, m_bufLen);

  //     SQLDBC_Retcode getObject(SQLDBC_Int4 columnIndex, 
  //                            SQLDBC_HostType columnType, 
  //                            void* columnAddr, 
  //                            SQLDBC_Length * columnLengthIndicator,
  //                            const SQLDBC_Length columnSize);

  if (erg == SQLDBC_DATA_TRUNC){
    outp->printValue(this, m_buffer, indicator);
    if (outp->streamAllData()){
      while(erg == SQLDBC_DATA_TRUNC){
        indicator=0;
        erg = rs->getRowSet()->getObject(colIndex, outp->gethostType(), m_buffer,  &indicator, m_bufLen);  
        if (erg != SQLDBC_OK && erg != SQLDBC_DATA_TRUNC && erg != SQLDBC_NO_DATA_FOUND){
          if (this->printResult("%s",this->m_sqlcliEnv->getcolumnsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
          if (this->printResult("%s",this->m_sqlcliEnv->getrowsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
          this->printError (rs->error());
          return SQLDBC_FALSE;                  
        } else {
          outp->printValue(this, m_buffer, indicator);
        }
      }
      return SQLDBC_TRUE; 
    } else {
      //this->printResult("%s",this->m_sqlcliEnv->getrecordsep()); /* linebreak */
      //this->printError (ERR_INTERNAL_TRUNC, colIndex,outp->getColLength(),m_bufLen);
      //return SQLDBC_FALSE;                    
      return SQLDBC_TRUE;                 
    }  
  } else if (erg != SQLDBC_OK){
    if (this->printResult("%s",this->m_sqlcliEnv->getcolumnsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    if (this->printResult("%s",this->m_sqlcliEnv->getrowsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    this->printError (rs->error());
    return SQLDBC_FALSE;                  
  } else { 
    if(indicator == SQLDBC_NULL_DATA){ 
      this->printNullValue(outp->getColLength());     
      return SQLDBC_TRUE;
    }
    outp->printValue(this, m_buffer, indicator);
    return SQLDBC_TRUE;
  }
}

//----------------------------------------------------------------------
SQLDBC_Bool  IFTools_sqlcliOutput::printOutputValue(SQLDBC_PreparedStatement * rs, SQLDBC_Int4 colIndex, IFTools_sqlcliOutputPrinter *outp){
  SQLDBC_Length indicator=0;
  SQLDBC_Retcode erg = rs->getObject(colIndex, outp->gethostType(), m_buffer,  &indicator, m_bufLen);

  //     SQLDBC_Retcode getObject(SQLDBC_Int4 columnIndex, 
  //                            SQLDBC_HostType columnType, 
  //                            void* columnAddr, 
  //                            SQLDBC_Length * columnLengthIndicator,
  //                            const SQLDBC_Length columnSize);

  if (erg == SQLDBC_DATA_TRUNC){
    outp->printValue(this, m_buffer, indicator);
    if (outp->streamAllData()){
      while(erg == SQLDBC_DATA_TRUNC){
        indicator=0;
        erg = rs->getObject(colIndex, outp->gethostType(), m_buffer,  &indicator, m_bufLen);  
        if (erg != SQLDBC_OK && erg != SQLDBC_DATA_TRUNC && erg != SQLDBC_NO_DATA_FOUND){
          if (this->printResult("%s",this->m_sqlcliEnv->getcolumnsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
          if (this->printResult("%s",this->m_sqlcliEnv->getrowsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
          this->printError (rs->error());
          return SQLDBC_FALSE;                  
        } else {
          outp->printValue(this, m_buffer, indicator);
        }
      }
      return SQLDBC_TRUE; 
    } else {
      //this->printResult("%s",this->m_sqlcliEnv->getrecordsep()); /* linebreak */
      //this->printError (ERR_INTERNAL_TRUNC, colIndex,outp->getColLength(),m_bufLen);
      //return SQLDBC_FALSE;                    
      return SQLDBC_TRUE;                 
    }  
  } else if (erg != SQLDBC_OK){
    if (this->printResult("%s",this->m_sqlcliEnv->getcolumnsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    if (this->printResult("%s",this->m_sqlcliEnv->getrowsuffix())==SQLDBC_FALSE) return SQLDBC_FALSE; 
    this->printError (rs->error());
    return SQLDBC_FALSE;                  
  } else { 
    if(indicator == SQLDBC_NULL_DATA){ 
      this->printNullValue(outp->getColLength());     
      return SQLDBC_TRUE;
    }
    outp->printValue(this, m_buffer, indicator);
    return SQLDBC_TRUE;
  }
}