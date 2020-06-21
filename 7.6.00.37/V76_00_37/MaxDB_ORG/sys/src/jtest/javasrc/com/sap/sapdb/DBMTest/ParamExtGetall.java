package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ParamExtGetall.java
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
 * Class:        ParamExtGetall
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer param_extgetall command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ParamExtGetall extends BackupState
{
  private Vector ParamLabel_Name;
  private Vector ParamLabel_CodeN;
  private Vector ParamLabel_CodeO;

  public ParamExtGetall() throws SQLException, TestDatabaseException
  {
    super();

    ParamLabel_Name = new Vector();
    ParamLabel_CodeN = new Vector();
    ParamLabel_CodeO = new Vector();
    //Expected label (some keywords only) of param_extgetall
    ParamLabel_Name.add("KERNELVERSION");
    ParamLabel_Name.add("INSTANCE_TYPE");
    ParamLabel_Name.add("DEFAULT_CODE");
    ParamLabel_Name.add("DATE_TIME_FORMAT");
    ParamLabel_Name.add("CONTROLUSERID");
    ParamLabel_Name.add("CONTROLPASSWORD");
    ParamLabel_Name.add("MAXLOGVOLUMES");
    ParamLabel_Name.add("MAXDATAVOLUMES");
    ParamLabel_Name.add("LOG_MIRRORED");
    ParamLabel_Name.add("LOG_SEGMENT_SIZE");
    ParamLabel_Name.add("LOG_IO_QUEUE");
    ParamLabel_Name.add("MAXCPU");
    ParamLabel_Name.add("MAXUSERTASKS");
    ParamLabel_Name.add("MAXSERVERTASKS");
    ParamLabel_Name.add("MAXLOCKS");
    ParamLabel_Name.add("SESSION_TIMEOUT");
    ParamLabel_Name.add("REQUEST_TIMEOUT");
    ParamLabel_Name.add("RUNDIRECTORY");
    //Expected label (some code only) of param_extgetall on new Releases
    ParamLabel_CodeN.add("string");         //KERNELVERSION
    ParamLabel_CodeN.add("string");         //INSTANCE_TYPE
    ParamLabel_CodeN.add("string");         //DEFAULT_CODE
    ParamLabel_CodeN.add("string");         //DATE_TIME_FORMAT
    ParamLabel_CodeN.add("string");         //CONTROLUSERID
    ParamLabel_CodeN.add("crypt");          //CONTROLPASSWORD
    ParamLabel_CodeN.add("int");            //MAXARCHIVELOGS
    ParamLabel_CodeN.add("int");            //MAXDATADEVSPACES
    ParamLabel_CodeN.add("string");         //LOG_MIRRORED
    ParamLabel_CodeN.add("int");            //LOG_SEGMENT_SIZE
    ParamLabel_CodeN.add("int");            //LOG_IO_QUEUE
    ParamLabel_CodeN.add("int");            //MAXCPU
    ParamLabel_CodeN.add("int");            //MAXUSERTASKS
    ParamLabel_CodeN.add("int");            //MAXSERVERTASKS
    ParamLabel_CodeN.add("int");            //MAXLOCKS
    ParamLabel_CodeN.add("int");            //SESSION_TIMEOUT
    ParamLabel_CodeN.add("int");            //REQUEST_TIMEOUT
    ParamLabel_CodeN.add("string");         //RUNDIRECTORY
    //Expected label (some code only) of param_extgetall on old Releases
    ParamLabel_CodeO.add("c40");            //KERNELVERSION
    ParamLabel_CodeO.add("c8");             //INSTANCE_TYPE
    ParamLabel_CodeO.add("c18");            //DEFAULT_CODE
    ParamLabel_CodeO.add("c18");            //DATE_TIME_FORMAT
    ParamLabel_CodeO.add("c64");            //CONTROLUSERID
    ParamLabel_CodeO.add("crypt");          //CONTROLPASSWORD
    ParamLabel_CodeO.add("int2");           //MAXARCHIVELOGS
    ParamLabel_CodeO.add("int2");           //MAXDATADEVSPACES
    ParamLabel_CodeO.add("c8");             //LOG_MIRRORED
    ParamLabel_CodeO.add("int4");           //LOG_SEGMENT_SIZE
    ParamLabel_CodeO.add("int2");           //LOG_IO_QUEUE
    ParamLabel_CodeO.add("int2");           //MAXCPU
    ParamLabel_CodeO.add("int2");           //MAXUSERTASKS
    ParamLabel_CodeO.add("int2");           //MAXSERVERTASKS
    ParamLabel_CodeO.add("int4");           //MAXLOCKS
    ParamLabel_CodeO.add("int4");           //SESSION_TIMEOUT
    ParamLabel_CodeO.add("int4");           //REQUEST_TIMEOUT
    ParamLabel_CodeO.add("c64");            //RUNDIRECTORY
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);

    //  create one global try-catch block to handle exceptions
    try
    {
      //Getting info of MAXDATADEVSPACES
      oDBM.execCmd("param_extgetall",DBMUtility.OK_OK);
      String param = oDBM.getAnswer();

      //Creating vector for label
      Vector ParamArray = new Vector();

      StringTokenizer st_param = new StringTokenizer(param,"\n");
      while (st_param.hasMoreTokens())
      {
        StringTokenizer st_st = new StringTokenizer(st_param.nextToken(),"\t");
        while (st_st.hasMoreTokens())
        {
          ParamArray.add(st_st.nextToken());
        }
      }

      int m_param_name_ok_count = 0;
      int m_param_code_ok_count = 0;
      int k = 0;
      //Checking the gotten label string - some keywords only
      while (k < ParamLabel_Name.size()) {
        for (int i = 0;i < ParamArray.size();i++) {

          //System.out.println(ParamArray.get(i).toString().trim());
          //System.out.println(ParamLabel_Name.get(k).toString().trim());

          //Checking the parameter name
          if (ParamArray.get(i).toString().trim().equals(ParamLabel_Name.get(k).toString().trim())) {

            m_param_name_ok_count++;

            //System.out.println(ParamArray.get(i).toString().trim());
            //System.out.println(ParamLabel_Name.get(k).toString().trim());

            //Checking the parameter code and dimension
            if (ParamArray.get(i+1).toString().trim().equals(ParamLabel_CodeN.get(k).toString().trim()) ||
                ParamArray.get(i+1).toString().trim().equals(ParamLabel_CodeO.get(k).toString().trim())) {

               m_param_code_ok_count++;

               //System.out.println(ParamArray.get(i+1).toString().trim());
               //System.out.println(ParamLabel_CodeN.get(k).toString().trim());
               //System.out.println(ParamLabel_CodeO.get(k).toString().trim());
            }
            break;
          }
        }
        k++;
        if (m_param_name_ok_count == ParamLabel_Name.size()) {
          break;
        }
      }

      //System.out.println("m_param_name_ok_count: " + m_param_name_ok_count + "  :  " + ParamLabel_Name.size());
      //System.out.println("m_param_code_ok_count: " + m_param_code_ok_count + "  :  " + ParamLabel_CodeN.size());

      if (m_param_name_ok_count == ParamLabel_Name.size() &&
          m_param_code_ok_count == ParamLabel_CodeN.size()) {

        addMessage(this.toString() + ".run",
                   TestProtocol.INFO,
                   "Both labels are equal.\n"+param);
      } else {
        addMessage(this.toString() + ".run",
                   TestProtocol.ERROR,
                   "Both labels are not equal.\n"+param);
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
    return "ParamExtGetall";
  } // end getTestClassId
} //end of class MediumGet