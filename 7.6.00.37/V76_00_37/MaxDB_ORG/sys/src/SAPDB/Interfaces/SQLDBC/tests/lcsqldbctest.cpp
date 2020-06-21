/*!
  @file           lcsqldbctest.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Test for compressed OMS streams.
                  Structure of test.

                  @note This test needs an installed lcsqldbc shared object 
                  on server side.

                  1. Creates the livecachetestpackage and procedures.

                  2. Reads data from the livecache by calling the procedure
                  OUTCOPY_MESSAGES and stores the returned streamdata in a 
                  temporary file 'data'.

                  3. Reads the content of the file 'data' and send it as stream
                  to the livecache by calling the procedure INCOPY_MESSAGES

                  @note This is an SQLDBC test program and is not suitable for use
                  in production environments.  

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/Interfaces/tests/PacketSpy.h"
#include "livecachetypes.h" /* nocheck */
#include <string.h>

/**
   Batchsize of the input data
*/  
int  array_size = 1;

/**
   Reference of the packet spy and modify runtime
*/
PacketSpyRuntime *packetSpy;

/**
   The stream data struct
*/
typedef struct _TStreamRow {
  OmsTypeInt4  msgNo; /*!< contains the error number of the messages table */
  char  language[3]; /*!< contains the language of the messages table */
  char  msg[160];    /*!< contains the error message of the messages table */
} TStreamRow;


FILE  *ostream;  /*!< File handle for storing stream data */
FILE  *istream;  /*!< File handle for storing stream data */
FILE  *ostream2; /*!< File handle for reading stream data from a file */
FILE  *istream2; /*!< File handle for reading stream data from a file */

long written = 0;  /*!< Number of byte written to the ostream file */
long written2 = 0; /*!< Number of byte written to the second ostream file */
/**
   callbackfunction sending data from livecache to the client.
 */
int WriteStream(SQL_LC_StreamParm *StreamParm, void *rgbInStream, int noOfRows, int cbBufSize, void* pMask);
/**
   callbackfunction reading data from client and sending to the livecache.
 */
int ReadStream(SQL_LC_StreamParm *StreamParm, void *rgbOutStream, int cbStreamMax, int *pcbStreamLen, int *pcbNoOfRowsRead, void* pMask);

/**
 * @brief Testclass for livecache dbprocedure tests.
 */
class lcsqldbctest
    : public SQLDBC_TestCase
{
public:
    lcsqldbctest()
    : SQLDBC_TestCase("lcsqldbctest")
    {
    }

    
  /**
   * @brief Prepares the testcase.
   */
  SQLDBC_Bool prepare()
  {
    execSQL("DROP TABLE  newmessages");
    execSQL("CREATE TABLE newmessages as SELECT * FROM SYSDBA.MESSAGES");
    execSQL("DROP DBPROC INCOPY_MESSAGES2");
    execSQL("DROP DBPROC OUTCOPY_MESSAGES2");
    execSQL("DROP package STREAMTEST");
    if (connection()->getKernelVersion() > 70403)
        execSQL("create OR REPLACE package STREAMTEST file 'lcsqldbc'");
    else
      execSQL("CREATE OR REPLACE PACKAGE STREAMTEST AS 'livecache.lcsqldbc.v001'");
    execSQL("CREATE DBPROC INCOPY_MESSAGES2 IN STREAMTEST EXECUTE INPROC");
    execSQL("CREATE DBPROC OUTCOPY_MESSAGES2 IN STREAMTEST EXECUTE INPROC");
    connection()->commit();
    return m_success;
  }


  /**
   * @brief Runs the testcase.
   */
  SQLDBC_Bool run()
  {
    SQLDBC_ConnectProperties *cprop = new SQLDBC_ConnectProperties();
    cprop->setProperty("OPTIMIZEDSTREAMS", "TRUE");

    SQLDBC_Connection *conn = environment()->createConnection();
    
    tc_errorcheck(conn)->connect(hostname(), database(), username(), password(), *cprop);
    
    COStreamDescriptor<TStreamRow, 3> streamDesc(1, &WriteStream);
    TStreamRow streamRow;memset(&streamRow, 0, sizeof(streamRow));
    streamDesc.Describe(streamRow.msgNo);
    streamDesc.Describe(streamRow.language, sizeof (streamRow.language));
    streamDesc.Describe(streamRow.msg,      sizeof (streamRow.msg));
    
    COStreamDescriptor<TStreamRow, 3> streamDesc2(2, &WriteStream);
    TStreamRow streamRow2;memset(&streamRow2, 0, sizeof(streamRow2));
    streamDesc2.Describe(streamRow2.msgNo);
    streamDesc2.Describe(streamRow2.language, sizeof (streamRow2.language));
    streamDesc2.Describe(streamRow2.msg,      sizeof (streamRow2.msg));

    if( NULL == (ostream  = fopen( "data", "wb" )) ) {
      logerror("could not open output file");
      return m_success;
    }
    if( NULL == (ostream2  = fopen( "data2", "wb" )) ) {
      logerror("could not open output file" );
      return m_success;
    }
    SQLDBC_PreparedStatement *pstmt = conn->createPreparedStatement();
    
    tc_errorcheck(pstmt)->prepare("CALL OUTCOPY_MESSAGES2(?, ?)");
    tc_errorcheck(pstmt)->setBatchSize(array_size);
    tc_errorcheck(pstmt)->bindParameter(1,  SQLDBC_HOSTTYPE_STREAM, &streamDesc, 
                                            (SQLDBC_Length*)calloc(array_size, sizeof(SQLDBC_Length)), 
                                            sizeof(streamDesc));
    tc_errorcheck(pstmt)->bindParameter(2,  SQLDBC_HOSTTYPE_STREAM, &streamDesc2, 
                                            (SQLDBC_Length*)calloc(array_size, sizeof(SQLDBC_Length)), 
                                            sizeof(streamDesc2));
    char tmppacket[1000];
    ReplyPacket reply( (tsp1_packet*)tmppacket, sizeof(tmppacket), false);
    ReplySegment segment = reply.AddSegment();
    PIn_Part part = segment.AddPart(sp1pk_abap_ostream);
    part = segment.AddPart(sp1pk_data);
    part = segment.AddPart(sp1pk_nil);
    segment.ClosePart(part);
    
    packetSpy->setReplyData(tmppacket, sizeof(tmppacket));

    tc_expectederrorcheck(pstmt, -10330)->execute();
    
    fclose( ostream );
    fclose( ostream2 );
    
    readFile(conn);
    
    tc_errorcheck(conn)->commitWorkRelease();
    
    return m_success;
  }

  /**
     Reads the content of a file and send it to the livecache procedure
  */

  SQLDBC_Bool  readFile(SQLDBC_Connection *conn) {
    CIStreamDescriptor<TStreamRow, 3> streamDesc(1, &ReadStream);
    TStreamRow streamRow;memset(&streamRow, 0, sizeof(streamRow));
    streamDesc.Describe(streamRow.msgNo);
    streamDesc.Describe(streamRow.language, sizeof (streamRow.language));
    streamDesc.Describe(streamRow.msg,      sizeof (streamRow.msg));
    
    CIStreamDescriptor<TStreamRow, 3> streamDesc2(2, &ReadStream);
    TStreamRow streamRow2;memset(&streamRow2, 0, sizeof(streamRow2));
    streamDesc2.Describe(streamRow2.msgNo);
    streamDesc2.Describe(streamRow2.language, sizeof (streamRow2.language));
    streamDesc2.Describe(streamRow2.msg,      sizeof (streamRow2.msg));
    if( NULL == (istream  = fopen( "data", "rb" )) ) {
      logerror("could not open input file\n" );
      return m_success;
    }
    if( NULL == (istream2  = fopen( "data2", "rb" )) ) {
      logerror("could not open input file\n" );
      return m_success;
    }
    SQLDBC_Statement *stmt = conn->createStatement();
    SQLDBC_PreparedStatement *pstmt = conn->createPreparedStatement();
    
    tc_errorcheck(stmt)->execute("delete from newmessages");

    tc_errorcheck(pstmt)->prepare("CALL INCOPY_MESSAGES2(?, ?)");

    tc_errorcheck(pstmt)->setBatchSize(array_size);
    tc_errorcheck(pstmt)->bindParameter(1,  SQLDBC_HOSTTYPE_STREAM, &streamDesc, 
                                        (SQLDBC_Length*)calloc(array_size, sizeof(SQLDBC_Length)), 
                                        sizeof(streamDesc));
    tc_errorcheck(pstmt)->bindParameter(2,  SQLDBC_HOSTTYPE_STREAM, &streamDesc2, 
                                        (SQLDBC_Length*)calloc(array_size, sizeof(SQLDBC_Length)), 
                                        sizeof(streamDesc2));

    tc_errorcheck(pstmt)->execute();

    tc_errorcheck(conn)->commit();

    execSQL("select TRUE from dual where (select count(*) from newmessages) = (select count(*) from messages)");

    fclose( istream );
    fclose( istream2 );
    return m_success;
  }
};

/** 
    Main program for running SQLDBC tests
*/

int main (int argc, char **argv)
{
    // Initalize from command line params
  packetSpy = new PacketSpyRuntime();
  
  SQLDBC_TestFrame testframe(argc, argv, packetSpy);
    testframe.addTestCase (new lcsqldbctest());
    if (testframe.run ()) 
        return 0;
    return 1;
}


int WriteStream(SQL_LC_StreamParm *StreamParm, void *rgbInStream, int noOfRows, int cbBufSize, void* pMask)
{
  long numwritten;
  FILE *stream = (StreamParm->ABAPTab.hABAPTab.ABAPTabId == 1) ? 
    ostream : ostream2;
  long *writtenp = (StreamParm->ABAPTab.hABAPTab.ABAPTabId == 1) ? 
    &written : &written2;
  numwritten = fwrite( rgbInStream, sizeof(TStreamRow), noOfRows, stream );
  *writtenp+= numwritten;
  printf ("write stream : TabID %d %d rows, written %d\n", StreamParm->ABAPTab.hABAPTab.ABAPTabId, noOfRows, *writtenp);
  if (numwritten != noOfRows)
    return SQL_STREAM_ERROR;
  return(SQL_STREAM_OK);
}

int ReadStream(SQL_LC_StreamParm *StreamParm, void *rgbOutStream, int cbStreamMax, int *pcbStreamLen, int *pcbNoOfRowsRead, void* pMask) 
{ 
     size_t numread;
     size_t requiredBytes = (cbStreamMax / sizeof (TStreamRow)) * sizeof(TStreamRow);;
     FILE *stream = (StreamParm->ABAPTab.hABAPTab.ABAPTabId == 1) ? 
    istream : istream2;
     printf ("read stream : TabID %d required %d ", StreamParm->ABAPTab.hABAPTab.ABAPTabId, requiredBytes);
     numread = fread(rgbOutStream, 1, requiredBytes, stream );
     *pcbStreamLen    = numread;
     *pcbNoOfRowsRead = numread / sizeof(TStreamRow);
     printf ("read bytes: %d\n", numread); 
     if (numread < requiredBytes)
         return SQL_STREAM_NO_MORE_DATA;
     else
         return (SQL_STREAM_OK); 
}

