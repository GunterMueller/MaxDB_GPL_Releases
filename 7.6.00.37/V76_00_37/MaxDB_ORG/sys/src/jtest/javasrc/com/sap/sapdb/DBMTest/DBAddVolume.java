package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBAddVolume.java
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
import java.io.File;
import java.sql.SQLException;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBAddVolume
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer db_addvolume command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBAddVolume extends BackupState {
    private static final String AUTOADD_DATAVOL_NAME = "DAT02.dat";
    private static final String AUTOADD_LOGVOL_NAME = "LOG02.dat";

    public DBAddVolume() throws SQLException, TestDatabaseException {
        super();
    }

    /**
     * run() contains the test implementation
     */
    public void run() {
        DBMUtility oDBM = new DBMUtility(getDatabase(), this);
        TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();
        String name = getTestClassId();

        //  create one global try-catch block to handle exceptions
        try {
            // file objects to check whether volumes exists
            File manualDataVol =
                new File(
                    sapdb.getRunDir()
                        + oDBM.getSrvProperty("file.separator")
                        + name
                        + "_DATA");
            File manualLogVol =
                new File(
                    sapdb.getRunDir()
                        + oDBM.getSrvProperty("file.separator")
                        + name
                        + "_LOG");
            File autoDataVol =
                new File(
                    sapdb.getRunDir()
                        + oDBM.getSrvProperty("file.separator")
                        + AUTOADD_DATAVOL_NAME);
            File autoLogVol =
                new File(
                    sapdb.getRunDir()
                        + oDBM.getSrvProperty("file.separator")
                        + AUTOADD_LOGVOL_NAME);

            //Too few parameters - command only
            oDBM.execCmd("db_addvolume", DBMUtility.ERR_TOFEWPARAM);
            //illegal mode
            oDBM.execCmd("db_addvolume ILLEGALMODE", DBMUtility.ERR_PARAM);
            //Too few parameters - datamedium type, name of device
            oDBM.execCmd(
                "db_addvolume DATA " + name + "_DATA",
                DBMUtility.ERR_PARAM);
            //Too few parameters - datamedium type, name of masterdevice, type of devspace
            oDBM.execCmd(
                "db_addvolume DATA " + name + "_DATA F",
                DBMUtility.ERR_PARAM);
            //automatic calculation of location, name and size (type is F)
            oDBM.execCmd("db_addvolume DATA", DBMUtility.OK_OK);
            if (!autoDataVol.exists()) {
                // report error
                this.addMessage(
                    "volumes...",
                    TestProtocol.ERROR,
                    "data volume "
                        + autoDataVol.getCanonicalPath()
                        + " does not exist");
            }
            //datamedium type, name of masterdevice, type of devspace, block size [pages]
            oDBM.execCmd(
                "db_addvolume DATA " + name + "_DATA F 625",
                DBMUtility.OK_OK);
            if (!manualDataVol.exists()) {
                // report error
                this.addMessage(
                    "volumes...",
                    TestProtocol.ERROR,
                    "data volume "
                        + manualDataVol.getCanonicalPath()
                        + " does not exist");
            }

            //Too few parameters - datamedium type, name of device
            oDBM.execCmd(
                "db_addvolume LOG " + name + "_LOG",
                DBMUtility.ERR_PARAM);
            //Too few parameters - datamedium type, name of masterdevice, type of devspace
            oDBM.execCmd(
                "db_addvolume LOG " + name + "_LOG F",
                DBMUtility.ERR_VOLMODE);
            //automatic calculation of location, name and size
            oDBM.execCmd("db_addvolume LOG", DBMUtility.OK_OK);
            if (!autoLogVol.exists()) {
                // report error
                this.addMessage(
                    "volumes...",
                    TestProtocol.ERROR,
                    "data volume "
                        + autoLogVol.getCanonicalPath()
                        + " does not exist");
            }

            //datamedium type, name of masterdevice, type of devspace, block size [pages]
            oDBM.execCmd(
                "db_addvolume LOG " + name + "_LOG F 625",
                DBMUtility.OK_OK);
            if (!manualLogVol.exists()) {
                // report error
                this.addMessage(
                    "volumes...",
                    TestProtocol.ERROR,
                    "data volume "
                        + manualLogVol.getCanonicalPath()
                        + " does not exist");
            }

            oDBM.execCmd("db_offline", DBMUtility.OK_OK);

            //Run Directory clean up
            oDBM.execCmd("param_delvolume 2 DATA", DBMUtility.OK_OK);
            // this is the automatically added data volume
            if (autoDataVol.exists())
                autoDataVol.delete();

            oDBM.execCmd("param_delvolume 3 DATA", DBMUtility.OK_OK);
            // this is the manually added data volume
            if (manualDataVol.exists())
                manualDataVol.delete();

            oDBM.execCmd("param_delvolume 2 LOG", DBMUtility.OK_OK);
            // this is the automatically added log volume
            if (autoLogVol.exists())
                autoLogVol.delete();

            oDBM.execCmd("param_delvolume 3 LOG", DBMUtility.OK_OK);
            // this is the manually added log volume
            if (manualLogVol.exists())
                manualLogVol.delete();

            oDBM.execCmd("db_reinstall", DBMUtility.OK_OK);
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
    } // end run

    /**
     * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
     * will set the name to the name of the java class. To make the test name independend
     * from the java implementation, this method should return a unique string.
     * Attention: The uniqueness is not automatically checked by anybody.
     * @return The UNIQUE name of the test class
     */
    public static String getTestClassId() {
        return "DBAddVolume";
    } // end getTestClassId
} //end of class MediumGet