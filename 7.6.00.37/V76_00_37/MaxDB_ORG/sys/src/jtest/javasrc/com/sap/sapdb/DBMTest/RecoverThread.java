package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: RecoverThread.java
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

import java.util.Properties;

import com.sap.dbtech.powertoys.DBM;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabase;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;

/**
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        RecoverThread
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer recover_start command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class RecoverThread extends Thread
{
  private DBMUtility m_oDBM;
  private DBM m_DBM;
  private Properties properties;

  private String dbrootKeyC = "dbroot";
  private String dbnameKeyC = "dbname";
  private String hostKeyC = "host";

  private TestCase m_TC;
  private TestDatabase m_DB;

  public RecoverThread(TestDatabase oDB,
                       TestCase oTC,
                       ThreadGroup aThreadGroup,
                       String mHost,
                       String mName,
                       String mRoot)
  {
    //call Thread Constructor with ThreadGroup
    super(aThreadGroup,"RecoverThread");
    m_DB = oDB;
    m_TC = oTC;
    properties = new Properties();

    properties.setProperty (hostKeyC, mHost);
    properties.setProperty (dbnameKeyC, mName);
    properties.setProperty (dbrootKeyC, mRoot);

    try
    {
      m_DBM = new DBM(properties);
    }
    catch (com.sap.dbtech.rte.comm.RTEException e)
    {
      m_TC.addMessage(this.toString() + ".run",
                      TestProtocol.ERROR,
                      e.toString());
    }
    m_oDBM = new DBMUtility(oDB, m_TC, m_DBM);
  } // end RecoverCheckThread

  public void run()
  {
    TestDatabaseSap sapdb = (TestDatabaseSap) m_DB;
    try
    {
      m_oDBM.execCmd("user_logon " + TestCase.getDatabase().getDBMUser() + "," + TestCase.getDatabase().getDBMPassword(),DBMUtility.OK_OK);
      //Setting database to modus admin
      m_oDBM.execCmd("db_cold -" + sapdb.getKernelMode(),DBMUtility.OK_OK);

      //Service connect
      m_oDBM.execCmd("util_connect",DBMUtility.OK_OK);
      //
      synchronized(this) {
      this.notify();
      }
      //Checking recover medium
      for (int i=0;i<5;i++)
      {
        m_oDBM.execCmd("recover_start datamedium data",DBMUtility.OK_OK,false);
        Thread.sleep(1000);
      }
      //Service release
      m_oDBM.execCmd("util_release",DBMUtility.OK_OK);
      //Setting database to modus online
      m_oDBM.execCmd("db_online -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
    }
    // catch any kind of exception and write it to the protocol
    catch (Exception e)
    {
      // standard error handler
      m_TC.addMessage(this.toString() + ".run",
                      TestProtocol.ERROR,
                      e.toString());
    }// end try catch
  }
}