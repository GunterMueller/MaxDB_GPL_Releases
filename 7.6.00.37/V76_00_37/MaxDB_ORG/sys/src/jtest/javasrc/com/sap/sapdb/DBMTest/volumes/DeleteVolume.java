/*
 * Created on 30.03.2005
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
package com.sap.sapdb.DBMTest.volumes;

import java.io.File;
import java.io.IOException;
import java.sql.SQLException;
import java.util.StringTokenizer;
import java.util.Vector;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d025561
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class DeleteVolume extends DBMTestCase {

	class FileInRundirectory extends File
	{
		public FileInRundirectory(String fileName) throws TestDatabaseException
		{
			super(((TestDatabaseSap) getDatabase()).getRunDir()
			      +m_DBM.getSrvProperty("file.separator")
			      +fileName);
			TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();
		}
	};
	
	boolean doesNotExist(String fileName) throws IOException, TestDatabaseException
	{
		FileInRundirectory file=new FileInRundirectory(fileName);

		if(file.exists())
		{
            // report error
            this.addMessage(
                "volumes...",
                TestProtocol.ERROR,
                "volume "
                + file.getCanonicalPath()
                + " does already exist, but should not exist.");
            
            return false;
		}
		else
			return true;
	}

	boolean exists(String fileName) throws IOException, TestDatabaseException
	{
		FileInRundirectory file=new FileInRundirectory(fileName);

		if(!file.exists())
		{
            // report error
            this.addMessage(
                "volumes...",
                TestProtocol.ERROR,
                "volume "
                + file.getCanonicalPath()
                + " does not exist, but should exist.");
            
            return false;
		}
		else
			return true;
	}
	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public DeleteVolume() throws SQLException, TestDatabaseException {
		super();
		// TODO Auto-generated constructor stub
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest()
	{	
		TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();
        String name = getTestClassId();

        //create one global try-catch block to handle exceptions
        try
		{	
            //get number of current data volumes
            int    numberDataVolumes=0;
            Vector labelArray = new Vector();
  
            m_DBM.execCmd("param_getvolsall", DBMUtility.OK_OK);

            StringTokenizer lines = new StringTokenizer(m_DBM.getAnswer(),"\n");
            while (lines.hasMoreTokens())
            {
            	if(lines.nextToken().startsWith("DATA_VOLUME_NAME_"))
            		numberDataVolumes++;
            }

            //start testing
            
            //1. test correct parameter handling
            
            //1.1 delete last data volume
            String fileName1=getTestClassId()+"_DATA"+(numberDataVolumes+1);
            
            String[] variableArguments_1_1={"", " DATA"}; // test "db_deletevolume" and "db_deletevolume DATA" 
            
            for(int i=0; i<variableArguments_1_1.length; i++)
	            if(doesNotExist(fileName1))
	            {
	                m_DBM.execCmd("db_addvolume DATA "+fileName1+" F 625",
	                		DBMUtility.OK_OK, true);
	                
	                if(exists(fileName1))
	                {
	                	m_DBM.execCmd("db_deletevolume"+variableArguments_1_1[i], DBMUtility.OK_OK, true);    	
	                	doesNotExist(fileName1);
	                }
	            }
            
            //1.2 delete data volume by index
            String fileName2=getTestClassId()+"_DATA"+(numberDataVolumes+2);
            String[] variableArguments_1_2={"", "ID ", "DATA ", "DATA ID "};
            
            for(int i=0; i<variableArguments_1_2.length; i++)
	            if(doesNotExist(fileName1) && doesNotExist(fileName2))
	            {
	                m_DBM.execCmd("db_addvolume DATA "+fileName1+" F 625",
	                		DBMUtility.OK_OK, true);
	                m_DBM.execCmd("db_addvolume DATA "+fileName2+" F 625",
	            		DBMUtility.OK_OK, true);
	                
	                if(exists(fileName1) && exists(fileName2))
	                {
	                	m_DBM.execCmd("db_deletevolume "
	                			+variableArguments_1_2[i]
								+(numberDataVolumes+1), DBMUtility.OK_OK, true);
	
	                	doesNotExist(fileName1);
	                	exists(fileName2);
	                	
	                	m_DBM.execCmd("db_deletevolume "
	                			+variableArguments_1_2[i]
	                			+(numberDataVolumes+2), DBMUtility.OK_OK, true); 
	                }
	            }
            
            //1.3 delete data volume by name
            String[] variableArguments_1_3={"", "DATA "};
            
            for(int i=0; i<variableArguments_1_3.length; i++)
	            if(doesNotExist(fileName1) && doesNotExist(fileName2))
	            {
	                m_DBM.execCmd("db_addvolume DATA "+fileName1+" F 625",
	                		DBMUtility.OK_OK, true);
	                m_DBM.execCmd("db_addvolume DATA "+fileName2+" F 625",
	            		DBMUtility.OK_OK, true);
	                
	                if(exists(fileName1) && exists(fileName2))
	                {
	                	m_DBM.execCmd("db_deletevolume "
	                			+variableArguments_1_3[i]
								+"NAME "
								+fileName1, DBMUtility.OK_OK, true);
	
	                	doesNotExist(fileName1);
	                	exists(fileName2);
	                	
	                	m_DBM.execCmd("db_deletevolume "
	                			+variableArguments_1_3[i]
								+"NAME "
								+fileName2, DBMUtility.OK_OK, true); 
	                }
	            }
            
	        //2. try to delete a volume, that has no device type parameter
	        if(doesNotExist(fileName1))
	        {
	        	m_DBM.execCmd("db_addvolume DATA "+fileName1+" F 625",
                		DBMUtility.OK_OK, true);
                
                if(exists(fileName1))
                {
                	String s="";
                	
                	if(10>numberDataVolumes+1)
                		s+="000";
                	else if(100>numberDataVolumes+1)
                		s+="00";
                	else if(1000>numberDataVolumes+1)
                		s+="0";
                	
                	s+=(numberDataVolumes+1);
                	
                	m_DBM.execCmd("param_directdel DATA_VOLUME_TYPE_"+s, DBMUtility.OK_OK, true);
                	m_DBM.execCmd("db_deletevolume DATA ID "+(numberDataVolumes+1), DBMUtility.ERR_VOLTYPE, true);    	
                	exists(fileName1);
                	
                	m_DBM.execCmd("db_deletevolume", DBMUtility.ERR_VOLTYPE, true);    	
                	exists(fileName1);
                	
                	m_DBM.execCmd("param_directput DATA_VOLUME_TYPE_"+s+" F", DBMUtility.OK_OK, true);
                	m_DBM.execCmd("db_deletevolume DATA ID "+(numberDataVolumes+1), DBMUtility.OK_OK, true);    	
                }
	        }
	        
	        //3. try to delete a volume, that has no volume mode parameter
	        if(doesNotExist(fileName1))
	        {
	        	m_DBM.execCmd("db_addvolume DATA "+fileName1+" F 625",
                		DBMUtility.OK_OK, true);
                
                if(exists(fileName1))
                {
                	String s="";
                	
                	if(10>numberDataVolumes+1)
                		s+="000";
                	else if(100>numberDataVolumes+1)
                		s+="00";
                	else if(1000>numberDataVolumes+1)
                		s+="0";
                	
                	s+=(numberDataVolumes+1);
                	
                	m_DBM.execCmd("param_directdel DATA_VOLUME_MODE_"+s, DBMUtility.OK_OK, true);
                	m_DBM.execCmd("db_deletevolume DATA ID "+(numberDataVolumes+1), DBMUtility.OK_OK, true);    	
                	doesNotExist(fileName1);
                }
	        }
	        
            //clean up the volumes the conventional way
            /*
            m_DBM.execCmd("db_offline", DBMUtility.OK_OK);

            //clean up Run Directory
            m_DBM.execCmd("param_delvolume 2 DATA", DBMUtility.OK_OK);
            // this is the automatically added data volume
            if (exists(fileName1))
            	new FileInRundirectory(fileName1).delete();

            m_DBM.execCmd("param_delvolume 3 DATA", DBMUtility.OK_OK);
            // this is the manually added data volume
            if (exists(fileName2))
            	new FileInRundirectory(fileName2).delete();

            m_DBM.execCmd("db_reinstall", DBMUtility.OK_OK);*/
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
            } catch (Exception e) {
                // do nothing
            } // end try catch
        } // end finally
		
	}

	/**
     * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
     * will set the name to the name of the java class. To make the test name independend
     * from the java implementation, this method should return a unique string.
     * Attention: The uniqueness is not automatically checked by anybody.
     * @return The UNIQUE name of the test class
     */
    public static String getTestClassId() {
        return "DeleteVolume";
    } // end getTestClassId
}
