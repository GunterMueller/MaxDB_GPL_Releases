package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: MediumLabelOffline.java
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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

import java.io.File;
import java.sql.SQLException;

import com.sap.sapdb.DBMTest.base.BackgroundProcess;
import com.sap.sapdb.DBMTest.base.DBFiller;
import com.sap.sapdb.DBMTest.base.LinebreakConverter;
import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.utilities.Execute;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        MediumLabelOffline
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer medium_labeloffline command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class MediumLabelOffline extends MediumPut
{
  public MediumLabelOffline() throws SQLException, TestDatabaseException
  {
          super();
  }
  
  String getLocation(String answer)
  {
  	DBMAnswerTable oAnswer    = new DBMAnswerTable();
    int []         nColumnPos = new int[2];

    nColumnPos[0] = 0;
    nColumnPos[1] = 21;

    oAnswer.analyzeColsByFixColumnWidth(answer, nColumnPos);
    
    return oAnswer.getField("Location", 0, 1);
  }
  
  void analyzeLabelAnswer(String answer)
  {
  	DBMAnswerTable oAnswer    = new DBMAnswerTable();
    int []         nColumnPos = new int[2];

    nColumnPos[0] = 0;
    nColumnPos[1] = 21;

    oAnswer.analyzeColsByFixColumnWidth(answer, nColumnPos);

    //checking whether result is correct
    if (oAnswer.getField("Returncode", 0, 1).equals("0") &&                                                                             //Returncode
        oAnswer.getField("Kernel Version", 0, 1).length() == 40 &&                                                                      //Length of Kernel Version
        oAnswer.getField("Label", 0, 1).indexOf("DAT_") == 0 &&                                                                         //Label
        oAnswer.getField("Database ID", 0, 1).indexOf(oAnswer.getField("Server", 0, 1)+":"+oAnswer.getField("Database", 0, 1)) == 0 &&  //Database ID
		oAnswer.getField("Is Consistent", 0, 1).equals("true")) {
      addMessage(this.toString() + ".run",
             TestProtocol.INFO,
             "Label string was checked and is ok!");
    } else {
      addMessage(this.toString() + ".run",
             TestProtocol.ERROR,
             "Label string was checked and is not ok!");
    } // end if
  }
  
  String getLabelWithout(String label, String withoutTag)
  {
  	String rc="";
  	int indexLocation=label.indexOf(withoutTag);

  	if(-1==indexLocation)
  	{
  		rc=label;
  	}
  	else
  	{
  		int indexEndLocation=label.indexOf("\n", indexLocation);
  		rc=label.substring(0, indexLocation)+label.substring(indexEndLocation+1);
  	}

  	return rc;
  }
  
  boolean compareLabelAnswersWithout(String answer1, String answer2, String comment1, String comment2, String withoutTag)
  {
  	String answer1Without=answer1;
  	String answer2Without=answer2;

  	if(!withoutTag.equals(""))
  	{
  		answer1Without=getLabelWithout(answer1, withoutTag);
  	  	answer2Without=getLabelWithout(answer2, withoutTag);
  	}

  	boolean rc=answer1Without.equals(answer2Without);

  	if(!rc)
  	{
  		String errorStart="\nUnexpected mismatch between two backup labels";
  		
  		if(!withoutTag.equals(""))
  			errorStart+=" (lines starting with '"+withoutTag+"' were ignored)";
  		
  		errorStart+=":\n\n";
  		
  		addMessage(this.toString(),
  	             TestProtocol.ERROR,
  	             LinebreakConverter.convertToOSSpecificLinebreaks(
  	                 errorStart+
				     "Label produced by a "+comment1+":"+"\n"+
				     answer1+"\n"+
				     "Label produced by a "+comment2+":"+"\n"+
					 answer2+"\n"));
  	}

  	return rc;
  }

  boolean compareLabelAnswers(String answer1, String answer2, String comment1, String comment2)
  {
  	return compareLabelAnswersWithout(answer1, answer2, comment1, comment2, "");
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
    String outPath = getTestClassId();
    TestDatabaseSap db= (TestDatabaseSap) super.getDatabase();

    //  create one global try-catch block to handle exceptions
    try
    {
    	String pathSep=System.getProperty("file.separator");
    	String pipeDir = null;
    	boolean createPipe=false;
		
		if(System.getProperty("os.name").toUpperCase().startsWith("WINDOWS"))
		{
			pipeDir="\\\\.\\pipe\\";
		}
		else
		{
			pipeDir=db.getRunDir()+pathSep;
			createPipe=true;
		}
    	
		//util connect
		oDBM.execCmd("util_connect",DBMUtility.OK_OK);

		//create 1st backup medium and backup
		oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);
		oDBM.execCmd("backup_start datamedium recovery data autoignore",DBMUtility.OK_OK);

		//create 2nd backup medium
		oDBM.execCmd("medium_put DataPipe "+pipeDir+outPath+"2 PIPE data",DBMUtility.OK_OK);
      
		//create 3rd unusable backup media group
		oDBM.execCmd("medium_put FileGroup\\data1 "+outPath+"3a file data 0 8 yes", DBMUtility.OK_OK);
		oDBM.execCmd("medium_put FileGroup\\data2 "+outPath+"3b file data 0 8 yes", DBMUtility.OK_OK);
      
		//create 4th and 5th media and use them to create a backup with replace
		oDBM.execCmd("medium_put DataFileS1 "+outPath+"4 FILE DATA 32 8 yes", DBMUtility.OK_OK);
		oDBM.execCmd("medium_put DataFileS2 "+outPath+"5 FILE DATA 0 0 yes", DBMUtility.OK_OK);
		oDBM.execCmd("backup_start DataFileS1", DBMUtility.OK_OK);
		oDBM.execCmd("backup_start DataFileS2", DBMUtility.OK_OK);
		
		//create 6th medium and use it to create a log backup
		oDBM.execCmd("autolog_off", DBMUtility.OK_OK);
		oDBM.execCmd("medium_put LogFile "+outPath+"6 FILE LOG", DBMUtility.OK_OK);
		DBFiller.fill(getDatabase());
		oDBM.execCmd("backup_start LogFile", DBMUtility.OK_OK);
		String logBackupLocation=getLocation(oDBM.getAnswer());
		String logBackupNumber=logBackupLocation.substring(1+logBackupLocation.indexOf("."));
		
		//create 7th medium that has a space in the name
		oDBM.execCmd("medium_put DataSpace \""+db.getRunDir()+pathSep+outPath+" 7\" FILE DATA", DBMUtility.OK_OK);
		oDBM.execCmd("backup_start DataSpace", DBMUtility.OK_OK);

		//util release
		oDBM.execCmd("util_release",DBMUtility.OK_OK);

		//setting database to offline
		oDBM.execCmd("db_offline",DBMUtility.OK_OK);

		//getting label of file medium
		oDBM.execCmd("medium_labeloffline datamedium", DBMUtility.OK_OK);

		String answerFile=oDBM.getAnswer();
		analyzeLabelAnswer(answerFile);

		if(createPipe)
		{
			String    mkfifoCall="mkfifo "+pipeDir+outPath+"2";
			String [] mkfifoEnvironment={""};
			Execute mkfifo=new Execute(mkfifoCall, mkfifoEnvironment);
			mkfifo.exec(new File(db.getInstRoot()));
			
			if(0!=mkfifo.getExitValue())
				addMessage(this.toString(),
	      	  			TestProtocol.ERROR,
						"Creation of pipe using mkfifo failed with exit code "+mkfifo.getExitValue()+".\n"+
						"Called '"+mkfifoCall+"'.");
		}
		
		String pipe2fileCall=db.getInstRoot()+pathSep+"bin"+pathSep+"pipe2file "+
      							"-d file2pipe "+
								"-f "+db.getRunDir()+pathSep+outPath+"1 "+
								"-p "+pipeDir+outPath+"2";
		String [] environment={""};
      
		BackgroundProcess pipe2file=new BackgroundProcess(pipe2fileCall, environment, db.getInstRoot());

		oDBM.execCmd("medium_labeloffline DataPipe", DBMUtility.OK_OK);
		pipe2file.join(100000);
      
		if(pipe2file.isAlive())
		{
      	  	addMessage(this.toString(),
      	  			TestProtocol.ERROR,
					"Could not stop pipe2file called as:\n"+
				    	pipe2fileCall);
		}
		
		String answerPipe=oDBM.getAnswer();
		analyzeLabelAnswer(answerPipe);
      
		compareLabelAnswersWithout(answerFile, answerPipe, "medium_labeloffline with FILE", "medium_labeloffline with PIPE", "Location");

		//setting database to online
		oDBM.execCmd("db_online -f",DBMUtility.OK_OK);
      
		//getting label of file medium from the database
		oDBM.execCmd("medium_label datamedium", DBMUtility.OK_OK);
      
		String answerFileKernel=oDBM.getAnswer();
      
		compareLabelAnswers(answerFile, answerFileKernel, "medium_labeloffline with FILE", "medium_label with FILE");
      
		//negative test, media group should not be usable
		oDBM.execCmd("medium_labeloffline FileGroup", DBMUtility.ERR_BAWRONGMEDIUM);

		//test labels of file medium created during backup with replace
		oDBM.execCmd("medium_labeloffline DataFileS1", DBMUtility.OK_OK);
		String answerOfflineFileReplace1=oDBM.getAnswer();
		oDBM.execCmd("medium_labeloffline DataFileS2", DBMUtility.OK_OK);
		String answerOfflineFileReplace2=oDBM.getAnswer();
		oDBM.execCmd("medium_label DataFileS1", DBMUtility.OK_OK);
		String answerFileReplace1=oDBM.getAnswer();
		oDBM.execCmd("medium_label DataFileS2", DBMUtility.OK_OK);
		String answerFileReplace2=oDBM.getAnswer();
      
		compareLabelAnswers(answerFileReplace1, answerOfflineFileReplace1, "medium_label with FILE", "medium_labeloffline with FILE");
		compareLabelAnswers(answerFileReplace2, answerOfflineFileReplace2, "medium_label with FILE created by backup_replace", "medium_labeloffline with FILE created by backup_replace");
		
		//test label of log backup
		oDBM.execCmd("medium_labeloffline LogFile "+logBackupNumber, DBMUtility.OK_OK);
		String answerOfflineFileLog=oDBM.getAnswer();
		oDBM.execCmd("medium_label LogFile "+logBackupNumber, DBMUtility.OK_OK);
		String answerFileLog=oDBM.getAnswer();
		
		compareLabelAnswers(answerFileLog, answerOfflineFileLog, "medium_label with LOG FILE", "medium_labeloffline with LOG FILE");
		
		//test obsolete version
		oDBM.execCmd("backup_labeloffline datamedium", DBMUtility.OK_OK);
		String answerFileObsolete=oDBM.getAnswer();

		compareLabelAnswers(answerFile, answerFileObsolete, "medium_labeloffline with FILE", "backup_labeloffline with FILE");
		
		//test variant without medium
		oDBM.execCmd("medium_labeloffline "+db.getRunDir()+pathSep+outPath+"1 DEVICETYPE FILE BLOCKSIZE 1",  DBMUtility.OK_OK);
		String answerFileDirect=oDBM.getAnswer();

		compareLabelAnswersWithout(answerFile, answerFileDirect, "medium_labeloffline with FILE medium", "medium_labeloffline from FILE but without a backup medium", "Location");
	
		//test spaces in file names
		oDBM.execCmd("medium_labeloffline \""+db.getRunDir()+pathSep+outPath+" 7\" FILE",  DBMUtility.OK_OK);
		String answerFileSpaceDirect=oDBM.getAnswer();
		
		analyzeLabelAnswer(answerFileSpaceDirect);
		
		oDBM.execCmd("medium_label DataSpace",  DBMUtility.OK_OK);
		String answerFileSpaceMedium=oDBM.getAnswer();
		
		compareLabelAnswers(answerFileSpaceDirect, answerFileSpaceMedium, "medium_labeloffline <location with space> FILE", "medium_label <medium>");
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
      	//cleanup rundirectory
      	File runDirectory=new File(db.getRunDir());
      	
      	String [] fileNames=runDirectory.list();
      	
      	for(int i=0; i< fileNames.length; i++)
      	{
      		File file=new File(db.getRunDir()+File.separator+fileNames[i]);
      		
      		if(!file.isDirectory() && 0==file.getName().indexOf(outPath))
      		{
      			file.delete();
      			addMessage(this.toString() + ".run",
      		             TestProtocol.INFO,
      		             "deleting "+file.getName()+" ("+file.getAbsoluteFile()+")");
      		}
      	}

        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }
      catch (Exception e)
      {
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
  public static String getTestClassId()
  {
    return "MediumLabelOffline";
  } // end getTestClassId

} //end of class MediumGet