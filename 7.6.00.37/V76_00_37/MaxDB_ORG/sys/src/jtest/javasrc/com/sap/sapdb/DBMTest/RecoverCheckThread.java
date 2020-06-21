/*


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



*/
package com.sap.sapdb.DBMTest;

/**
 * Title:
 * Description:
 * Copyright:    Copyright (c) 2002
 * Company:
 * @author
 * @version 1.0
 */

import java.util.Properties;

import com.sap.dbtech.powertoys.DBM;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabase;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;

public class RecoverCheckThread extends Thread
{
  private DBMUtility m_oDBM;
  private DBM m_DBM;
  private Properties properties;

  private String dbrootKeyC = "dbroot";
  private String dbnameKeyC = "dbname";
  private String hostKeyC = "host";

  private TestCase m_TC;

  public RecoverCheckThread(TestDatabase oDB,
                            TestCase oTC,
                            ThreadGroup aThreadGroup,
                            String mHost,
                            String mName,
                            String mRoot)
  {
    //call Thread Constructor with ThreadGroup
    super(aThreadGroup,"RecoverCheckThread");
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
    try
    {
      m_oDBM.execCmd("user_logon " + TestCase.getDatabase().getDBMUser() + "," + TestCase.getDatabase().getDBMPassword(),DBMUtility.OK_OK);
      //Service release
      m_oDBM.execCmd("service_release",DBMUtility.OK_OK);
      //Service connect
      m_oDBM.execCmd("service_connect",DBMUtility.OK_OK);
      //
      synchronized(this) {
      this.notify();
      }
      //Checking recover medium
      for (int i=0;i<10;i++)
      {
        m_oDBM.execCmd("recover_check datamedium data", DBMUtility.OK_OK, false);
        Thread.sleep(1000);
      }
      //Service release
      m_oDBM.execCmd("service_release",DBMUtility.OK_OK);
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