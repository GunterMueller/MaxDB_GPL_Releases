package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: FileGetlist.java
  -----------------------------------------------------------------------------


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



*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
import java.sql.SQLException;
import java.util.StringTokenizer;
import java.util.Vector;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        FileGetlist
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer file_getlist command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class FileGetlist extends BackupState
{
  public Vector vHeader;
  public Vector vFilelist;

  public FileGetlist() throws SQLException, TestDatabaseException
  {
    super();

    vHeader   = new Vector();
    vFilelist = new Vector();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    int m_iHeader   = 0;

    //Header of Filelist
    vHeader.add("keyname");
    vHeader.add("mode");
    vHeader.add("size");
    vHeader.add("date");
    vHeader.add("time");
    vHeader.add("comment");
    vHeader.add("filename");

    //List of files
    vFilelist.add("KNLDIAG");
    vFilelist.add("KNLDIAGERR");
    vFilelist.add("KNLTRC");
    vFilelist.add("UTLPRT");
    vFilelist.add("BACKHIST");
    vFilelist.add("DBMPRT");
    vFilelist.add("DBMPAHI");
    vFilelist.add("LCINITCMD");
    vFilelist.add("INSTPRT");


    //  create one global try-catch block to handle exceptions
    try
    {
      //Too few parameters
      oDBM.execCmd("file_getlist", DBMUtility.OK_OK, false);

      /**
       *  Something like this is expected
       *
       *  keyname,    mode,     size,   date,     time,   comment,                        filename
       *
       *  KNLDIAG     ASCII     819200  20020930  135919  Database Messages               knldiag
       *  KNLDIAGERR  ASCII     19883   20020930  135932  Database Errors                 knldiag.err
       *  KNLTRC      BINARY    4915200 20020930  135920  Database Trace                  knltrace
       *  UTLPRT      ASCII     102400  20020930  135918  Utility Statements              dbm.utl
       *  BACKHIST    ASCII     281     20020917  161819  Backup History                  dbm.knl
       *  DBMPRT      ASCII     16636   20020930  161620  Database Manager Protocol       dbm.prt
       *  DBMPAHI     ASCII     55620   20020930  135732  Database Parameter History      DB74.pah
       *  LCINITCMD   ASCII     14751   20020716  120725  LiveCache Initialisation Script lcinit.bat
       *  INSTPRT     ASCII     721196  20020917  161832  Installation Protocol           dbm.ins
       */

      //Searching the string for some keywords
      StringTokenizer m_stLine = new StringTokenizer (oDBM.getAnswer(), "\n");

      //Seraching for header strings
      while (m_stLine.hasMoreTokens()) {

        String m_sOneLine = m_stLine.nextToken().toString().toUpperCase();

        for (int i = 0;i < vHeader.size();i++) {

          if (m_sOneLine.indexOf(vHeader.get(i).toString().toUpperCase()) >= 0) {

            m_iHeader++;
          }
        }
        break;
      }

      m_stLine = new StringTokenizer (oDBM.getAnswer(), "\n");

      //Searching for filenames
      while (m_stLine.hasMoreTokens()) {

        String m_sOneLine = m_stLine.nextToken().toString().toUpperCase();

        for (int i = 0;i < vFilelist.size();i++) {

          if (m_sOneLine.indexOf(vFilelist.get(i).toString().toUpperCase()) >= 0) {

            vFilelist.remove(i);
            break;
          }
        }
      }

      if (vFilelist.size() == 0 && m_iHeader == vHeader.size()) {

        addMessage("'file_getlist'", TestProtocol.INFO, oDBM.getAnswer());
      } else {
        addMessage("'file_getlist'", TestProtocol.ERROR, oDBM.getAnswer());
      }
    }
    // catch any kind of exception and write it to the protocol
    catch (Exception e) {
      // standard error handler

      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally {
      try {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }
      catch (Exception e) {
        // do nothing
      } // end try catch
    } // end finally
  } // end run
  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId() {
    return "FileGetlist";
  } // end getTestClassId
} //end of class MediumGet