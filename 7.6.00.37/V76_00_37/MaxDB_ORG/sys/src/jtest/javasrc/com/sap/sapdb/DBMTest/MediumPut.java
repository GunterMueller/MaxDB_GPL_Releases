package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: MediumPut.java
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

import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;
import com.sap.sapdb.testframe.testcase.TestCaseException;

/**
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        MediumPut
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer medium_put command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class MediumPut extends TestCase
{
   public final static String MEDIUM_PUT_CMD = "medium_put";
   public final static String MEDIUM_GET_CMD = "medium_get";
   public final static String NAME = "name";
   public final static String LONGNAME = "name_with_more_than_onehundred_thirty_two_characters_that_are_more_than_any_time_before_and_I_hope_enough_for_the_next_five_years_or_forever";
   public final static String LOCATION = "/usr/tst";
   public final static String FILE = "FILE";
   public final static String TAPE = "TAPE";
   public final static String UNKNOWN = "UNKNOWN";
   public final static String NO_REWIND = "NOREWIND";
   public final static String PIPE = "PIPE";
   //public final static String AUTOLOADER = "AUTOLOADER";
   public final static String DATA = "DATA";
   public final static String PAGES = "PAGES";
   public final static String AUTO = "AUTO";
   public final static String LOG = "LOG";
   public final static String DEF_BLK_SIZE = "8";
   public final static String YES = "YES";
   public final static String NO = "NO";
   public final static String OS_CMD = "zip.exe";
   public final static String GET_CMD = MEDIUM_GET_CMD+" "+NAME;
  /**
   * A minimal constructor.Constructor.
   */
  public MediumPut() throws SQLException, TestDatabaseException
  {
          super();
  } // end MediumPut

  /**
   * Clean up test, i.e. delete data created in prepare, ...
   * @exception TestCaseException Should be thrown, when cleanUp fails
   */
  public static void cleanUp() throws TestCaseException
  {
    // do nothing
  } // end cleanUp

  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId()
  {
    return "MediumPut";
  } // end getTestClassId

  /**
   * Sets the timeout value in milliseconds. After the timeout is reached, the test will be aborted
   * by the test driver.
   * @return The default return value is 0, which means there is no timeout value set. Test will never
   * be aborted by the test driver.
   */
  public static long getTimeout()
  {
    return 0;
  } // end getTimeout

  /**
   * Sets weather test can create and work with verification data or not.
   * @return The default return value is false, which means that no verification data
   * is used to verify the test results. In this case the test has to implement the verification
   * itself.
   */
  public static boolean isReferenceEnabled()
  {
    return false;
  } // end isReferenceEnabled

  /**
   * Sets weather test can be executed only on SAPDB or not.
   * @return The default return value is true. This means, the test can run on SAPDB only.
   */
  public static boolean isSAPOnly()
  {
          return true;
  } // end isSAPOnly

  /**
   * Prepare test, i.e. create users, tables and data
   * @exception TestCaseException Should be thrown, when preparation fails
   */
  public static void prepare() throws TestCaseException
  {
    // do nothing
  } // end prepare
  private String compVals(String val,String comp,String ifEqual,String ifElse)
  {
      if (val.equals(comp))
      {
         return ifEqual;
      }
      return ifElse;
  }
  /**
   * Changes db instance paramer and restart db
   */
   public void changeDBParam(DBMUtility dbm, String paramName, String paramValue)
   {
       //check current value of the parameter
       dbm.execCmd("param_getvalue "+paramName,DBMUtility.OK_OK);

       //if param wrong then change it
       if (dbm.getAnswer().indexOf(paramValue+"\n")<0)
       {
           dbm.execCmd("param_startsession",DBMUtility.OK_OK);
           dbm.execCmd("param_put "+paramName+" "+paramValue,DBMUtility.OK_OK);
           dbm.execCmd("param_checkall",DBMUtility.OK_OK);
           dbm.execCmd("param_commitsession",DBMUtility.OK_OK);
           dbm.execCmd("db_offline",DBMUtility.OK_OK);
           dbm.execCmd("db_online -f",DBMUtility.OK_OK);
       }
   }
  /**
   * Get medium_put command
   */
   private String getMediumPutCommand(String name,String loc,String aType,
                                      String aBackupType,String aSize,String aBlkSize,
                                      String aOverWrite,String aAutoLdr,String osCmd)
   {
       return (MEDIUM_PUT_CMD
                     + compVals(name,"",""," " + name)
                     + compVals(loc,"",""," " + loc)
                     + compVals(aType,"",""," " + aType)
                     + compVals(aBackupType,"",""," " + aBackupType)
                     + compVals(aSize,"",""," " + aSize)
                     + compVals(aBlkSize,"",""," " + aBlkSize)
                     + compVals(aOverWrite,"",""," " + aOverWrite)
                     + compVals(aAutoLdr,"",""," " + aAutoLdr)
                     + compVals(osCmd,"",""," " + osCmd));
   }
  /**
   * Exec cmd
   */
   public boolean execCmd(DBMUtility dbm, MessageTable msgTab, String name,String loc,String aType,
                          String aBackupType,String aSize,String aBlkSize,String aOverWrite,
                          String aAutoLdr,String osCmd) throws Exception
   {
       String sCmd = getMediumPutCommand(name,loc,aType,aBackupType,aSize,
                                         aBlkSize,aOverWrite,aAutoLdr,osCmd);

       String reply =  name + "\t" + loc
                     + "\t" + compVals(aType,"",UNKNOWN,aType)
                     + "\t" + compVals(aBackupType,"",UNKNOWN,aBackupType)
                     + "\t" + compVals(aSize,"","0",aSize)
                     + "\t" + compVals(aBlkSize,"",DEF_BLK_SIZE,aBlkSize.equals("0")?DEF_BLK_SIZE:aBlkSize)
                     + "\t" + compVals(aOverWrite,"",NO,aOverWrite)
                     + "\t" + compVals(aAutoLdr,"",NO,aAutoLdr);

       if (dbm.execCmd(sCmd, DBMUtility.OK_OK))
       {
          //execute a get reply command
          dbm.execCmd(MEDIUM_GET_CMD + " " + name, DBMUtility.OK_OK);
          //parse the reply

          msgTab.setMessage(dbm.getAnswer());

          if (!msgTab.getRow(0).startsWith(reply))
          {
              this.addMessage("command "+sCmd+" didn't set right values", TestProtocol.ERROR , dbm.getAnswer());
              return false;
          }
       }
       else
       {
          return false;
       }
       return true;
   }
   /**
   * Exec cmd and compare it with reply
   */
   public boolean execCmd(DBMUtility dbm, MessageTable msgTab, String sCmd, String reply) throws Exception
   {
       if (dbm.execCmd(sCmd, DBMUtility.OK_OK))
       {
          //parse the reply

          msgTab.setMessage(dbm.getAnswer());

          if (!msgTab.getRow(0).startsWith(reply))
          {
              this.addMessage("command "+sCmd+" didn't return right reply", TestProtocol.ERROR , dbm.getAnswer());
              return false;
          }
       }
       else
       {
          return false;
       }
       return true;
   }

  /**
   * Exec cmd
   */
   public void execCmd(DBMUtility dbm, String name,String loc,String aType,
                          String aBackupType,String aSize,String aBlkSize,String aOverWrite,
                          String aAutoLdr,String osCmd, int Error) throws Exception
   {
       String sCmd = getMediumPutCommand(name,loc,aType,aBackupType,aSize,
                                         aBlkSize,aOverWrite,aAutoLdr,osCmd);

       if(!dbm.execCmd(sCmd, Error))
          this.addMessage("Command was \""+sCmd+"\".", TestProtocol.ERROR , dbm.getAnswer());
   }

  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    //create message table for checking DBM replies
    MessageTable msgTab = new MessageTable();
    String rep = null;

    //  create one global try-catch block to handle exceptions
    try
    {
      //Too few parameters
      execCmd(oDBM,"","","","","","","","","",DBMUtility.ERR_TOFEWPARAM);

      //Too few parameters
      execCmd(oDBM,NAME,"","","","","","","","",DBMUtility.ERR_TOFEWPARAM);

      //Only name and location
      execCmd(oDBM,msgTab,NAME,LOCATION,"","","","","","","");

      //Only name, location, type
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,"","","","","","");

      //Only name, location, type, backup type
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,DATA,"","","","","");

      //Only name, location, type, backup type, size
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,DATA,"1","","","","");

      //Only name, location, type, backup type, size, block size
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,DATA,"1","10","","","");

      //Only name, location, type, backup type, size, block size, overwrite
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,DATA,"1","10",YES,"","");

      //Only name, location, type, backup type, size, block size, overwrite, autoload
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,DATA,"1","10",YES,YES,"");

      //Correct command
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,DATA,"1","10",YES,YES,OS_CMD);

      //Correct command with long name
      //execCmd(oDBM,msgTab,LONGNAME,LOCATION,FILE,DATA,"1","10",YES,YES,OS_CMD);

      //Omitted medium name
      execCmd(oDBM,"",LOCATION,FILE,DATA,"1","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Omitted medium location
      execCmd(oDBM,NAME,"",FILE,DATA,"1","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Omitted medium type
      execCmd(oDBM,NAME,LOCATION,"",DATA,"1","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Incorrect medium type
      execCmd(oDBM,NAME,LOCATION,"f",DATA,"1","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Medium type NO-REWIND
      //execCmd(oDBM,msgTab,NAME,LOCATION,NO_REWIND,DATA,"1","10",YES,YES,OS_CMD);

      //Medium type PIPE
      execCmd(oDBM,msgTab,NAME,LOCATION,PIPE,DATA,"1","10",YES,YES,OS_CMD);

      //Medium type AUTOLOADER
      //execCmd(oDBM,msgTab,NAME,LOCATION,AUTOLOADER,DATA,"1","10",YES,YES,OS_CMD);

      //Medium type UNKNOWN
      execCmd(oDBM,msgTab,NAME,LOCATION,UNKNOWN,DATA,"1","10",YES,YES,OS_CMD);

      //Medium type TAPE
      execCmd(oDBM,msgTab,NAME,LOCATION,TAPE,DATA,"1","10",YES,YES,OS_CMD);

      //Omitted medium backup type
      execCmd(oDBM,NAME,LOCATION,FILE,"","1","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Incorrect medium backup type
      execCmd(oDBM,NAME,LOCATION,FILE,"d","1","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Medium backup type PAGES
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,PAGES,"1","10",YES,YES,OS_CMD);

      //Medium backup type LOG
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,LOG,"1","10",YES,YES,OS_CMD);

         //   oDBM.execCmd("medium_getall",DBMUtility.OK_OK);
         //   String my_getall = oDBM.getAnswer();
         //   System.out.println("--------------------------------------------------------");
         //   System.out.println("               Medium backup type LOG                   ");
         //   System.out.println("--------------------------------------------------------");
         //   System.out.println(my_getall);
         //   System.out.println("--------------------------------------------------------");

      //Medium backup type AUTO
      execCmd(oDBM,msgTab,NAME,LOCATION,TAPE,AUTO,"1","10",YES,YES,OS_CMD);

      //Omitted medium size
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Incorrect medium size
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"-1","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Incorrect medium size
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"s","10",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Omitted medium block size
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"1","",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Incorrect medium block size
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"1","bs",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Incorrect medium block size
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"1","-1",YES,YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Omitted medium overwrite/autoloader
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"1","1","",YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Incorrect medium overwrite
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"1","1","y",YES,OS_CMD,DBMUtility.ERR_MENOVALID);

      //Incorrect medium autoloader
      execCmd(oDBM,NAME,LOCATION,TAPE,AUTO,"1","1",YES,"y",OS_CMD,DBMUtility.ERR_MENOVALID);

      // correct autosave medium for next test
      execCmd(oDBM,msgTab,"auto","autosave",FILE,AUTO,"0","0",NO,NO,"");
    }
    // catch any kind of exception and write it to the protocol
    catch (Exception e)
    {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally
    {
      try
      {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }
      catch (Exception e)
      {
        // do nothing
      } // end try catch
    } // end finally
  } // end run

} // end class MediumPut
