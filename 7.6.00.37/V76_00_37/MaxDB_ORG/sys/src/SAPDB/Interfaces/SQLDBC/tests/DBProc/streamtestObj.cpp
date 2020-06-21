/***************************************************************************\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
/* streamtestObj.cpp : Implementation of Cstreamtest                            */
/****************************************************************************/

#include <lcbasetypes.h>
#include <livecachetypes.h>
#include "streamtest.h" /* nocheck */
#include "SAPDB/Interfaces/SQLDBC/tests/DBProc/streamtestObj.h"

/****************************************************************************/
/* Register is the very first Method called after the creation of the       */
/* Component-Object.                                                        */
/* (within the "constructor" of the Component-Object no communication with  */
/*  the liveCache has been activated yet!)                                  */

STDMETHODIMP Cstreamtest::Register()
{
  HRESULT hr	= S_OK;
  try {
    
      co90CheckVersion ((IUnknown**)&this->m_pIUnknown, 
                        ILIVECACHESINK_VERSION, 
                        LIVECACHETYPES_VERSION, LIVECACHEOMS_VERSION, 
                        LIVECACHESQL_VERSION );

/* *** TODO ***                                                            */
/* Register all persistent objects used by DB-Procedures here              */
  }
  catch (DbpError e) {
    omsExceptionHandler (e);
  }
	return hr;
}

template <class T> 
SqlCol charArray(T t) {
    return SqlCol ((void *)t, SqlCol::VCHAR, sizeof(T));
};


/****************************************************************************/
/* Istreamtest method implementation  (to be inserted here)                   */

//----------------------------------------------------------------------
STDMETHODIMP Cstreamtest::TEST_CHARSTREAM_IN(CharStreamHandle* instream)
{
    log(LOG_SEVERITY_INFO, "TEST_CHARSTREAM_IN called.");
    OmsIStream<CharStreamHandle, OmsTypeChar> iStream(this, *instream);
    int count = 0;
    OmsTypeChar *ch;
    OmsTypeChar *cmp=(OmsTypeChar *)"Homer Maggie Bart and Knecht Ruprecht";
    int cmplen = strlen((char *)cmp);
    while(ch = iStream.omsRead()) {
        if(*ch != cmp[count % cmplen]) {
            log(LOG_SEVERITY_ERROR, "DID FIND CHAR %c INSTEAD OF %c.", *ch, cmp[count % cmplen]);
            return S_FALSE;
        }
        ++count;
        if((count == 1)  || (count % 100 == 0)) {
            log(LOG_SEVERITY_INFO, "READ %d CHARS", (count));
        }
    }
    log(LOG_SEVERITY_INFO, "READ %d CHARS", (count));
    return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP Cstreamtest::TEST_CHARSTREAM_2(CharStreamHandle* in1,
                                            CharStreamHandle *in2)
{
    log(LOG_SEVERITY_INFO, "TEST_CHARSTREAM_2 called.");
    OmsIStream<CharStreamHandle, OmsTypeChar> iStream1(this, *in1);
    OmsIStream<CharStreamHandle, OmsTypeChar> iStream2(this, *in2);
    {
        int count = 0;
        OmsTypeChar *ch;
        OmsTypeChar *cmp=(OmsTypeChar *)"Homer Maggie Bart and Knecht Ruprecht";
        int cmplen = strlen((char *)cmp);
        while(ch = iStream1.omsRead()) {
            if(*ch != cmp[count % cmplen]) {
                log(LOG_SEVERITY_ERROR, "STREAM 1 DID FIND CHAR %c INSTEAD OF %c.", *ch, cmp[count % cmplen]);
                return S_FALSE;
            }
            ++count;
            if((count == 1)  || (count % 100 == 0)) {
                log(LOG_SEVERITY_INFO, "READ %d CHARS FROM 1st STREAM", (count));
            }
        }
    }
    {
        int count = 0;
        OmsTypeChar *ch;
        OmsTypeChar *cmp=(OmsTypeChar *)"Homer Maggie Bart and Knecht Ruprecht";
        int cmplen = strlen((char *)cmp);
        while(ch = iStream2.omsRead()) {
            if(*ch != cmp[count % cmplen]) {
                log(LOG_SEVERITY_ERROR, "STREAM 2 DID FIND CHAR %c INSTEAD OF %c.", *ch, cmp[count % cmplen]);
                return S_FALSE;
            }
            ++count;
            if((count == 1)  || (count % 100 == 0)) {
                log(LOG_SEVERITY_INFO, "READ %d CHARS FROM 1nd STREAM", (count));
            }
        }
    }
    return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP Cstreamtest::TEST_CHARSTREAM_OUT(CharStreamHandle* out, 
                                              StringName*name)
{
    strcpy(*name, "Homer Maggie Bart");
    OmsOStream<CharStreamHandle, OmsTypeChar> ostream(this, *out);
    for(int k=0; k<4000; ++k) {
        if(k % 100 == 0) {
            log(LOG_SEVERITY_INFO, "TEST_CHARSTREAM_OUT: WRITTEN %d CHARS.", k);
        }
        ostream.omsWrite('A');
    }
    ostream.omsClose();
    return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP Cstreamtest::TEST_CHARSTREAM_ECHO(CharStreamHandle* instream, CharStreamHandle *outstream,
                                               StringName *inputname, StringName* outputname)
{
    return S_OK;
}
    

//----------------------------------------------------------------------
void
Cstreamtest::log(int severity, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buffer[512];
    vsprintf(buffer, fmt, args);
    if(strlen(buffer) > 80) {
        buffer[80] = '\0';
    }
    char statement[512];
    sprintf(statement, "INSERT INTO STREAMTEST_LOG VALUES (DEFAULT, %d, '%s')", severity, buffer);
    sql(statement)();
    va_end(args);

}

class ResultSet {
    bool       do_prepare;
    SqlHandle* sqlhandle;
    char       cursorName[67];
    long       fetchparamcnt;
    SqlCol     fetchparms[20];
    SQL select;
    SQL fetch;
    
public :
    ResultSet(const char* cname, SqlHandle* h) { 
        do_prepare    = true;
        sqlhandle     = h;
        fetchparamcnt = 0;
        strcpy (cursorName, "\"");
        strcat (cursorName, cname);
        strcat (cursorName, "\"");
    }

    void operator= (const char* stmt) {
        char* cursor_stmt = new char[100+strlen(stmt)];
        if (cursor_stmt) {
            strcpy (cursor_stmt, "DECLARE ");
            strcat (cursor_stmt, cursorName);
            strcat (cursor_stmt, " CURSOR FOR ");
            strcat (cursor_stmt, stmt);
            select = sqlhandle->sql(cursor_stmt);
            delete[] cursor_stmt;
        }
    }

    ResultSet &operator<< (const SqlCol& c) {
        select << c;
        return *this;
    }

    ResultSet &operator>> (const SqlCol& c) {
        if (do_prepare)
            fetchparms[fetchparamcnt++] = c;
        else
            fetch << c;
        return *this;
    }

    void sqlClose () {
        char close_stmt[100];
        strcpy (close_stmt, "CLOSE ");
        strcat (close_stmt, cursorName);
        sqlhandle->sql(close_stmt).sqlExecute();
    }

    bool next() {
        if (do_prepare) {
            int params;
            char stmt[1000];
            select.sqlExecute();
            strcpy (stmt, "Fetch ");
            strcat (stmt, cursorName);
            strcat (stmt, " Using Descriptor");
            SQL prep_fetch  = sqlhandle->sql(stmt);
            prep_fetch.sqlPrepare();
            params = prep_fetch.sqlNumOutCols();
            strcpy (stmt, "FETCH ");
            strcat (stmt, cursorName);
            strcat (stmt, " INTO ?");
            long ix;
            for (ix = 1; ix < params; ix++)
                strcat (stmt, ",?");
            fetch = sqlhandle->sql(stmt);
            for (ix = 0; ix < params; ix++)
                fetch << fetchparms[ix];
            do_prepare = false;
        }
        return (1 == fetch.sqlExecute());
    }

};


/****************************************************************************/

STDMETHODIMP Cstreamtest::OUTCOPY_MESSAGES(StreamHandle * ostream)
{
    try {
        TMessageRow messageRow;
        OmsOStream<StreamHandle, TMessageRow> outStream(this, *ostream);
        ResultSet rs("RES", this);
        rs = "SELECT * FROM SYSDBA.MESSAGES";
        rs >> messageRow.msgNo >> charArray(messageRow.language) 
           >> charArray(messageRow.msg);
        memset(messageRow.language, ' ', sizeof(messageRow.language));
        memset(messageRow.msg, ' ', sizeof(messageRow.msg));
        while (rs.next()) {
          outStream.omsWrite(messageRow);
          memset(messageRow.language, ' ', sizeof(messageRow.language));
          memset(messageRow.msg, ' ', sizeof(messageRow.msg));
        }
        rs.sqlClose();
        outStream.omsClose();
    }
    catch (DbpError sqle)
	{
      sqlExceptionHandler(sqle);
	}
	

	return S_OK;
}

/****************************************************************************/

STDMETHODIMP Cstreamtest::INCOPY_MESSAGES(StreamHandle * istream)
{
    try {
        TMessageRow *pMessageRow;
        SQL insert = sql("INSERT INTO NEWMESSAGES VALUES (?,?,?)");
       
        OmsIStream<StreamHandle, TMessageRow> inStream(this, *istream);
        while (pMessageRow = inStream.omsRead()) {
            insert << pMessageRow->msgNo << charArray(pMessageRow->language) 
                   << charArray(pMessageRow->msg);
            insert();
        }
    }
    catch (DbpError sqle)
	{
      sqlExceptionHandler(sqle);
	}
	return S_OK;
}

/****************************************************************************/

STDMETHODIMP Cstreamtest::OUTCOPY_MESSAGES2(StreamHandle * ostream, StreamHandle * ostream2)
{
    try {
        TMessageRow messageRow;
        OmsOStream<StreamHandle, TMessageRow> outStream(this, *ostream);
        OmsOStream<StreamHandle, TMessageRow> outStream2(this, *ostream2);
        ResultSet rs("RES", this);
        rs = "SELECT * FROM SYSDBA.MESSAGES";
        rs >> messageRow.msgNo >> charArray(messageRow.language) >> charArray(messageRow.msg);
        memset(messageRow.language, ' ', sizeof(messageRow.language));
        memset(messageRow.msg, ' ', sizeof(messageRow.msg));
        while (rs.next()) {
          outStream.omsWrite(messageRow);
          outStream2.omsWrite(messageRow);
          memset(messageRow.language, ' ', sizeof(messageRow.language));
          memset(messageRow.msg, ' ', sizeof(messageRow.msg));
        }
        rs.sqlClose();
        outStream.omsClose();
        outStream2.omsClose();
    }
    catch (DbpError sqle)
	{
      sqlExceptionHandler(sqle);
	}
	

	return S_OK;
}

/****************************************************************************/

STDMETHODIMP Cstreamtest::INCOPY_MESSAGES2(StreamHandle * istream, StreamHandle * istream2)
{
    try {
        TMessageRow *pMessageRow=NULL;
        TMessageRow *pMessageRow2=NULL;
        boolean count=0; // prevents endless loop;
        SQL insert = sql("INSERT INTO NEWMESSAGES VALUES (?,?,?)");
       
        OmsIStream<StreamHandle, TMessageRow> inStream(this, *istream);
        OmsIStream<StreamHandle, TMessageRow> inStream2(this, *istream2);
        do {
          if (!count || pMessageRow)
            pMessageRow = inStream.omsRead();
          if (!count || pMessageRow2) 
            pMessageRow2 = inStream2.omsRead();
          count++;
          if (pMessageRow) {
            insert << pMessageRow->msgNo << charArray(pMessageRow->language) << charArray(pMessageRow->msg);
            insert();
          }
          if (pMessageRow2) {
            insert << pMessageRow2->msgNo << charArray(pMessageRow2->language) << charArray(pMessageRow2->msg);
            insert();
          }          
        } while (count > 10000 || pMessageRow || pMessageRow2);
    }
    catch (DbpError sqle)
	{
      sqlExceptionHandler(sqle);
	}
	return S_OK;
}
