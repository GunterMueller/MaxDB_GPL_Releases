package com.sap.sapdb.DBMTest.util;

/**
  -----------------------------------------------------------------------------
  module: DBMUtility.java
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

import java.util.HashSet;

import com.sap.dbtech.powertoys.DBM;
import com.sap.dbtech.powertoys.DBMException;
import com.sap.sapdb.DBMTest.base.LinebreakConverter;
import com.sap.sapdb.testframe.driver.TestDatabase;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;
/**
 * Usefull functions for the DBMServer test.
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMUtility
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  Usefull functions for the DBMServer test.
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMUtility {

  private TestDatabase   m_oDB;
  private TestCase       m_oTC;
  private String         m_szLastPayload;
  private String         m_LastAnswer;
  private int            m_nLastError;
  private String         m_szLastError;
  private DBM            m_DBM;
  private DBMAnswerTable m_oVersion;


  /**
   * Constructor.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     DBMUtility
   * -------------------------------------------------------------------------
   * description:  constructor
   * arguments:    oDB  [IN] - the current database
   *               oTC  [IN] - the current test case
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public DBMUtility(TestDatabase oDB, TestCase oTC)
  {
    m_oDB = oDB;
    m_oTC = oTC;
    
    m_oVersion = new DBMAnswerTable();
    execCmd("dbm_version", DBMUtility.OK_OK);
    m_oVersion.analyzeColsByFieldSeparator(getAnswer(), '=');
  } // end DBMUtility

  public DBMUtility(TestDatabase oDB, TestCase oTC, DBM oDBM)
  {
    m_oDB = oDB;
    m_oTC = oTC;
    m_DBM = oDBM;

    m_oVersion = new DBMAnswerTable();
    execCmd("dbm_version", DBMUtility.OK_OK);
    m_oVersion.analyzeColsByFieldSeparator(getAnswer(), '=');
  } // end DBMUtility

  /**
   * Read database server properties.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getSrvProperty
   * -------------------------------------------------------------------------
   * description:  read a property
   * return value: the property
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String getSrvProperty(String sProperty)
  {
    String sResult = "";

    if (sProperty == "file.separator") {
      if (m_oVersion.getField("OS", 0, 1).compareTo("UNIX") == 0) {
        sResult = "/";
      } else {
        sResult = "\\";
      } // end if
    } else if (sProperty == "dbmversion.os") {
      sResult = m_oVersion.getField("OS", 0, 1);
    } else if (sProperty == "dbmversion.version") {
      sResult = m_oVersion.getField("VERSION", 0, 1);
    } else if (sProperty == "dbmversion.instroot") {
      sResult = m_oVersion.getField("INSTROOT", 0, 1);
    } else if (sProperty == "dbmversion.sysname") {
      sResult = m_oVersion.getField("SYSNAME", 0, 1);
    } // end if

    return sResult;
  } // end getSrvProperty

  /**
   * Read the last answer.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getAnswer
   * -------------------------------------------------------------------------
   * description:  read the last answer
   * return value: the last answer
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String getAnswer()
  {
    return m_szLastPayload;
  } // end getAnswer
  
  public String getCompleteAnswer()
  {
  	return m_LastAnswer;
  }
  
  public int getLastError()
  {
  	return m_nLastError;
  }

  /**
   * Execute DBMServer command.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     execCmd
   * -------------------------------------------------------------------------
   * description:  execute DBMServer command
   * arguments:    sCmd      [IN] - the DBMServer command
   *               nExpected [IN] - the expected return value
   * return value: true if the expected return value comes from the DBMServer
   *               otherwise false
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public boolean execCmd(String sCmd, int nExpected)
  {
    return execCmd( sCmd, nExpected, false);
  } // end execCmd

  /**
   * Execute DBMServer command.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     execCmd
   * -------------------------------------------------------------------------
   * description:  execute DBMServer command
   * arguments:    sCmd      [IN] - the DBMServer command
   *               nExpected [IN] - the expected return value
   *               bWriteInfo[IN] - TestProtocol.INFO write or not
   * return value: true if the expected return value comes from the DBMServer
   *               otherwise false
   * -------------------------------------------------------------------------
   * </xmp>
   */
  
  public boolean execCmd(String sCmd, int nExpected, boolean bWriteInfo)
  {
  	int [] tempArray =new int [] {nExpected};

  	boolean rc=execCmd(sCmd, tempArray, bWriteInfo);
  	
  	return rc;
  }
  
  public boolean execCmd(String sCmd, int [] expectedRCs)
  {
  	return execCmd(sCmd, expectedRCs, false);
  }
  
  public boolean execCmd(final String sCmd, final int [] expectedRCs, final boolean bWriteInfo) {

	  class RawCommandExecutor extends Thread {
		  boolean m_Result = false;
		  boolean getResult() {
			  return m_Result;
		  }
		  public void run() {
			  m_Result = execCmdInThread(sCmd, expectedRCs, bWriteInfo);
		  }
	  };
	  
	  RawCommandExecutor rawCommandExecutor = new RawCommandExecutor();
	  rawCommandExecutor.start();
	  try {
		  // let extra thread execute the command
		  rawCommandExecutor.join();
	  }
	  catch( InterruptedException ire ) {
		  rawCommandExecutor.interrupt();
		  m_oTC.addMessage("DBMServer command execution interrupted", TestProtocol.ERROR , "command: "+ sCmd, ire);
	  }
	  return rawCommandExecutor.getResult();
  }
  
  private boolean execCmdInThread(String sCmd, int [] expectedRCs, boolean bWriteInfo)
  {
  	String linebreak=System.getProperty("line.separator");
    boolean bRc = true;
    
    HashSet x=new HashSet();
    
    for(int i=0; i<expectedRCs.length; i++)
    	x.add(new Integer(expectedRCs[i]));
    	

    if (bWriteInfo) {
      m_oTC.addMessage("DBMServer command", TestProtocol.INFO , sCmd);
    }

    try
    {
      m_szLastPayload = "";
      m_LastAnswer = "";
      m_szLastError   = "";
      m_nLastError    = 0;

	  if (m_DBM == null)
	  {
    	  m_szLastPayload = m_oDB.executeDBMcmd(sCmd);
          m_LastAnswer = m_szLastPayload;
	  }
	  else
	  {
    	  m_szLastPayload = m_DBM.cmd(sCmd);
          m_LastAnswer = m_szLastPayload;
	  }
      
      if ( !x.contains(new Integer(OK_OK)) && !x.contains(new Integer(OK_ERR_IGNORE)))
      {
        m_oTC.addMessage(
        		"unexpected success for a DBM command at",
        		TestProtocol.ERROR,
        		linebreak +
			     getDBMCLILikeAnswer(sCmd));
        bRc = false;
      } // end if
    }
    catch (Exception e)
    {
      if (e instanceof TestDatabaseException || e instanceof DBMException)
      {
      	Exception eTemp = null;
        if( e instanceof DBMException )
        	eTemp = e;
        else if( e instanceof TestDatabaseException ) {
        	eTemp = ((TestDatabaseException) e).getOriginalException();
        }

        if (eTemp instanceof DBMException)
        {
          DBMException eDBM = (DBMException) eTemp;

          m_szLastPayload = eDBM.getMessage();
          m_LastAnswer = eDBM.getMessage();
          
          if (m_szLastPayload.indexOf('\n') >= 0)
          {
            m_szLastPayload = m_szLastPayload.substring(m_szLastPayload.indexOf('\n') + 1);
          }
          else
          {
            m_szLastPayload = "";
          } // end if
          m_szLastError   = eDBM.getErrorID();
          m_nLastError    = eDBM.getErrorCode();

          if(!x.contains(new Integer(m_nLastError)) && !x.contains(new Integer(OK_ERR_IGNORE)))
          {
            m_oTC.addMessage("encountered an unexpected DBM answer at",
            		         TestProtocol.ERROR ,
            		         linebreak +
							     getDBMCLILikeAnswer(sCmd));
            bRc = false;
          } // end if
        }
        else
        {
          m_oTC.addMessage("unexpected contained exception class " + eTemp.getClass().getName(), TestProtocol.ERROR , "DBMServer", eTemp);
          bRc = false;
        } // end if
      }
      else
      {
        m_oTC.addMessage("unexpected exception class " + e.getClass().getName(), TestProtocol.ERROR , "DBMServer", e);
        bRc = false;
      } // end if
    } // end try catch

    if (m_szLastPayload.length() > 0 && bWriteInfo == true)
    {
      m_oTC.addMessage(sCmd, TestProtocol.INFO , m_szLastPayload);
    } // end if
    return bRc;
  } // end execCmd
  
  public boolean execCmd(String command, int primaryRC, int sndRC) {
  	
  	if(execCmd(command, primaryRC)) {
  		String sndRCAsString=new String(String.valueOf(sndRC)+",");
  	
  		if(0==getAnswer().indexOf(sndRCAsString))
  			return true;
  		else
  		{
  			String linebreak=System.getProperty("line.separator");
  			
  			m_oTC.addMessage("",
   		         TestProtocol.ERROR ,
				 linebreak+
				     "Encountered an unexpected secondary returncode in DBM answer. Expected return"+linebreak+
				     "code "+String.valueOf(sndRC)+" to follow return code "+String.valueOf(primaryRC)+" but got the following answer instead:"+linebreak+
    		         linebreak +
    		         getDBMCLILikeAnswer(command));
  		}
  	}
  	
  	return false;
  }

  /**
   * Execute DBMServer command.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     execCmd
   * -------------------------------------------------------------------------
   * description:  execute DBMServer command
   * arguments:    sCmd      [IN] - the DBMServer command
   *               nExpected [IN] - the expected return value
   * return value: true if the expected return value comes from the DBMServer
   *               otherwise false
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public boolean execCmd(String sCmd, String gCmd, String reply) {

    boolean bRc = true;

    MessageTable msgTab = new MessageTable();

    try {
      //execute command
      execCmd(sCmd, OK_OK);
      //execute a get reply command
      execCmd(gCmd, OK_OK);
      //parse the reply
      msgTab.setMessage(getAnswer());
//      System.out.println("\n!"+msgTab.getRow(0)+"!\n!"+reply+"!");
      if (!msgTab.getRow(0).startsWith(reply)) {
          m_oTC.addMessage("command "+sCmd+" didn't set right values", TestProtocol.ERROR , m_szLastPayload);
          bRc = false;
      }

    } catch (Exception e) {
        m_oTC.addMessage("unexpected exception", TestProtocol.ERROR , "DBMServer", e);
        bRc = false;

    } // end try catch

    return bRc;
  } // end execCmd
  
  private String getDBMCLILikeAnswer(String command) {
  	
  	String linebreak=System.getProperty("line.separator");
  	String message=LinebreakConverter.convertToOSSpecificLinebreaks(getCompleteAnswer());
    String errorHeader=new String();

  	if(0!=m_nLastError)
  		errorHeader=new Integer(m_nLastError).toString()+","+m_szLastError+": ";
  	
  	return "DBMServer>"+command+linebreak+
    			(m_nLastError==0?"OK":"ERR")+linebreak+
				errorHeader+message+linebreak+
				linebreak+
				"---"+linebreak;
  }

  /**
   * Check database state and database speed
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     checkStateAndSpeed
   * -------------------------------------------------------------------------
   * description:  Check database state and database speed
   * arguments:    sState    [IN] - expected database state
   *               sSpeed    [IN] - expected database speed
   *               sError    [IN] - additional errortext
   * return value: true for the expected state and speed
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public boolean checkStateAndKernel(String sState, String sKernel, String sErrortext)
  {
    boolean bRc = true;

    if (sState.length() > 0) {
      if (execCmd("db_state", DBMUtility.OK_OK)) {
        if (getAnswer().indexOf(sState) < 0) {
          m_oTC.addMessage(TestCase.getTestClassId(), TestProtocol.ERROR, "Database state is "+getAnswer()+" and not "+sState+" \nInfo: "+sErrortext+"!");
          bRc = false;
        } // end if
      } else {
        m_oTC.addMessage(TestCase.getTestClassId(), TestProtocol.ERROR, "Cannot determine database state ("+getAnswer()+") \nInfo: "+sErrortext+"!");
        bRc = false;
      } // end if
    } // end if

    if (sKernel.length() > 0) {
      if (execCmd("show version", DBMUtility.OK_OK)) {
        if (getAnswer().indexOf(sKernel) < 0) {
          m_oTC.addMessage(TestCase.getTestClassId(), TestProtocol.ERROR, "Database kernel is "+getAnswer()+" and not "+sKernel+" \nInfo: "+sErrortext+"!");
          bRc = false;
        } // end if
      } else {
        m_oTC.addMessage(TestCase.getTestClassId(), TestProtocol.ERROR, "Cannot determine database kernel ("+getAnswer()+") \nInfo: "+sErrortext+"!");
        bRc = false;
      } // end if
    } // end if

    return bRc;
  } // end checkStateAndSpeed

  /**
   * DBMServer error code.
   * There is a set of similar functions for all DBMServer error codes
   */
  public static final int OK_OK                   =      0;

  public static final int OK_ERR_IGNORE           =     -1;

  public static final int ERR_COMMON              = -24999;
  public static final int ERR_NOTIMPL             = -24998;
  public static final int ERR_FILE                = -24997;
  public static final int ERR_PARAM               = -24996;
  public static final int ERR_MEM                 = -24995;
  public static final int ERR_RTE                 = -24994;
  public static final int ERR_RTEEXT              = -24993;
  public static final int ERR_NOUTILSESSION       = -24992;
  public static final int ERR_NODBSESSION         = -24991;
  public static final int ERR_NOBACKUPSESSION     = -24990;
  public static final int ERR_NOHISTORYSESSION    = -24989;
  public static final int ERR_SQL                 = -24988;
  public static final int ERR_TOSMALL             = -24987;
  public static final int ERR_NOMEDIA             = -24986;
  public static final int ERR_MENOTFOUND          = -24985;
  public static final int ERR_MENOVALID           = -24984;
  public static final int ERR_BANOVALIDTYPE       = -24983;
  public static final int ERR_ERR_NOHISTORY       = -24982;
  public static final int ERR_HIENTRYNOTFOUND     = -24981;
  public static final int ERR_NOHELP              = -24980;
  public static final int ERR_XPNOTFOUND          = -24979;
  public static final int ERR_XPSYNTAX            = -24978;
  public static final int ERR_COMMAND             = -24977;
  public static final int ERR_PARAMEXIST          = -24976;
  public static final int ERR_NOXPSESSION         = -24975;
  public static final int ERR_XPSESSION           = -24974;
  public static final int ERR_XPCHECK             = -24973;
  public static final int ERR_XPINVCONF           = -24972;
  public static final int ERR_XPNOCHECK           = -24971;
  public static final int ERR_XPNOMODIFY          = -24970;
  public static final int ERR_XPDIRTY             = -24969;
  public static final int ERR_XPREADONLY          = -24968;
  public static final int ERR_SYSTEM              = -24967;
  public static final int ERR_LOGON               = -24966;
  public static final int ERR_SYSLOGON            = -24965;
  public static final int ERR_EXECUTE             = -24964;
  public static final int ERR_DEVCHECK            = -24963;
  public static final int ERR_VOLTYPE             = -24962;
  public static final int ERR_STATE               = -24961;
  public static final int ERR_DBRUN               = -24960;
  public static final int ERR_XPOVERFLOW          = -24959;
  public static final int ERR_BAOVERWRITE         = -24958;
  public static final int ERR_NOUSER              = -24957;
  public static final int ERR_BASAVETYPECON       = -24956;
  public static final int ERR_BANOSAVETYPE        = -24955;
  public static final int ERR_BANOAUTOMED         = -24954;
  public static final int ERR_DBEXIST             = -24953;
  public static final int ERR_USRSAVE             = -24952;
  public static final int ERR_USRREAD             = -24951;
  public static final int ERR_USRFAIL             = -24950;
  public static final int ERR_USREXISTS           = -24949;
  public static final int ERR_NODBNAME            = -24948;
  public static final int ERR_DBNAME              = -24947;
  public static final int ERR_TOMANYPARAM         = -24946;
  public static final int ERR_TOFEWPARAM          = -24945;
  public static final int ERR_USRDELSELF          = -24944;
  public static final int ERR_USRDELCOLD          = -24943;
  public static final int ERR_USRDELSDBA          = -24942;
  public static final int ERR_NEEDLOGON           = -24941;
  public static final int ERR_NEEDSYSLOGON        = -24940;
  public static final int ERR_LASTREG             = -24939;
  public static final int ERR_NOENTRY             = -24938;
  public static final int ERR_MISSRIGHT           = -24937;
  public static final int ERR_DBNORUN             = -24936;
  public static final int ERR_NOUSERS             = -24935;
  public static final int ERR_VERSION             = -24934;
  public static final int ERR_NOSERVICESESSION    = -24933;
  public static final int ERR_XPMIN               = -24932;
  public static final int ERR_XPMAX               = -24931;
  public static final int ERR_XPVALUESET          = -24930;
  public static final int ERR_USRFRM              = -24929;
  public static final int ERR_NOREPLY             = -24928;
  public static final int ERR_TOOLCHK             = -24927;
  public static final int ERR_MEDIUMCHK           = -24926;
  public static final int ERR_PREPARE             = -24925;
  public static final int ERR_DBREQ               = -24924;
  public static final int ERR_TOOLREQ             = -24923;
  public static final int ERR_OPCHK               = -24922;
  public static final int ERR_POSTOP              = -24921;
  public static final int ERR_BACKUPOP            = -24920;
  public static final int ERR_CLEANUP             = -24919;
  public static final int ERR_MISSMIRR            = -24918;
  public static final int ERR_MAXDATA             = -24917;
  public static final int ERR_MAXDEVCNT           = -24916;
  public static final int ERR_NOADDD              = -24915;
  public static final int ERR_KEYEXISTS           = -24914;
  public static final int ERR_SERVICEDB           = -24913;
  public static final int ERR_WRONGINST           = -24912;
  public static final int ERR_INVDBNAME           = -24911;
  public static final int ERR_PARAMVER            = -24910;
  public static final int ERR_DBAUNKNOWN          = -24909;
  public static final int ERR_DOMPUNKNOWN         = -24908;
  public static final int ERR_DBAWRONG            = -24907;
  public static final int ERR_EBFDIRTY            = -24906;
  public static final int ERR_NOEBIDLIST          = -24905;
  public static final int ERR_DOMPWRONG           = -24904;
  public static final int ERR_IMPERSONATE         = -24903;
  public static final int ERR_ASCII               = -24902;
  public static final int ERR_NOTCOLD             = -24901;
  public static final int ERR_BAWRONGMEDIUM       = -24890;
  public static final int ERR_EVENTNONEAVAIL      = -24866;
  public static final int ERR_VOLMODE             = -24865;
  public static final int ERR_FILECHK             = -24837;
  public static final int ERR_CMDOPTION           = -24799;
  public static final int ERR_NOT_EXCLUSIVE       = -24709;
  public static final int ERR_RESULTLOST          = -24706;
}

/** EndClass: DBMUtility */
