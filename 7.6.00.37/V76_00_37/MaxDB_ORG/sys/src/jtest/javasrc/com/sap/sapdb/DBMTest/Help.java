package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: Help.java
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
 * Class:        Help
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer help command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class Help extends BackupState
{
  private Vector vFile;
  public Help() throws SQLException, TestDatabaseException
  {
    super();

    vFile = new Vector();

    vFile.add("archive_stage");
    vFile.add("archive_stage_repeat");
    vFile.add("autolog_cancel");
    vFile.add("autolog_off");
    vFile.add("autolog_on");
    vFile.add("autolog_show");
    vFile.add("backup_cancel");
    vFile.add("backup_ext_ids_forget");
    vFile.add("backup_ext_ids_get");
    vFile.add("backup_ext_ids_list");
    vFile.add("backup_ext_ids_listnext");
    vFile.add("backup_history_close");
    vFile.add("backup_history_date");
    vFile.add("backup_history_list");
    vFile.add("backup_history_listnext");
    vFile.add("backup_history_open");
    vFile.add("backup_ignore");
    vFile.add("backup_replace");
    vFile.add("backup_start");
    vFile.add("backup_state");
    vFile.add("db_addvolume");
    vFile.add("db_clear");
    vFile.add("db_admin");
    vFile.add("db_cons");
    vFile.add("db_create");
    vFile.add("db_drop");
    vFile.add("db_enum");
    vFile.add("db_migratecatalog");
    vFile.add("db_offline");
    vFile.add("db_online");
    vFile.add("db_reg");
    vFile.add("db_reinstall");
    vFile.add("db_restart");
    vFile.add("db_restartinfo");
    vFile.add("db_speed");
    vFile.add("db_state");
    vFile.add("db_start");
    vFile.add("db_stop");
    vFile.add("db_connect");
    vFile.add("db_execute");
    vFile.add("db_release");
    vFile.add("dban_start");
    vFile.add("dban_stop");
    vFile.add("dban_state");
    vFile.add("dbm_configset");
    vFile.add("dbm_configget");
    vFile.add("dbm_getpath");
    vFile.add("dbm_setpath");
    vFile.add("dbm_version");
    vFile.add("diag_histlist");
    vFile.add("diag_pack");
    vFile.add("event_list");
    vFile.add("event_set");
    vFile.add("event_delete");
    vFile.add("event_release");
    vFile.add("event_wait");
    vFile.add("exec_lcinit");
    vFile.add("exec_nipingsrv");
    vFile.add("exec_xpu");
    vFile.add("exec_sdbinfo");
    vFile.add("file_backup");
    vFile.add("file_getfirst");
    vFile.add("file_getlist");
    vFile.add("file_getnext");
    vFile.add("file_operation");
    vFile.add("file_restore");
    vFile.add("help");
    vFile.add("info");
    vFile.add("info_next");
    vFile.add("inst_enum");
    vFile.add("inst_reg");
    vFile.add("inst_unreg");
    vFile.add("load_systab");
    vFile.add("load_lcapps");
    vFile.add("load_r3tab");
    vFile.add("load_tutorial");
    vFile.add("medium_date");
    vFile.add("medium_delete");
    vFile.add("medium_get");
    vFile.add("medium_getall");
    vFile.add("medium_label");
    vFile.add("medium_labeloffline");
    vFile.add("medium_put");
    vFile.add("package_info");
    vFile.add("param_abortsession");
    vFile.add("param_addvolume");
    vFile.add("param_checkall");
    vFile.add("param_commitsession");
    vFile.add("param_copy");
    vFile.add("param_directdel");
    vFile.add("param_directget");
    vFile.add("param_directgetall");
    vFile.add("param_directgetallnext");
    vFile.add("param_directput");
    vFile.add("param_extget");
    vFile.add("param_extgetall");
    vFile.add("param_extgetallnext");
    vFile.add("param_getdefault");
    vFile.add("param_getexplain");
    vFile.add("param_getfull");
    vFile.add("param_gethelp");
    vFile.add("param_gethistory");
    vFile.add("param_gethistorynext");
    vFile.add("param_getproperties");
    vFile.add("param_gettype");
    vFile.add("param_getvalue");
    vFile.add("param_getvolsall");
    vFile.add("param_getvolume");
    vFile.add("param_init");
    vFile.add("param_put");
    vFile.add("param_restore");
    vFile.add("param_rmfile");
    vFile.add("param_startsession");
    vFile.add("param_versions");
    vFile.add("recover_cancel");
    vFile.add("recover_check");
    vFile.add("recover_config");
    vFile.add("recover_ignore");
    vFile.add("recover_replace");
    vFile.add("recover_start");
    vFile.add("recover_state");
    vFile.add("recover_state_check");
    vFile.add("recover_volume");
    vFile.add("release");
    vFile.add("show");
    vFile.add("show_list");
    vFile.add("show_next");
    vFile.add("service_connect");
    vFile.add("service_release");
    vFile.add("sql_connect");
    vFile.add("sql_execute");
    vFile.add("sql_fetch");
    vFile.add("sql_info");
    vFile.add("sql_recreateindex");
    vFile.add("sql_release");
    vFile.add("sql_updatestat");
    vFile.add("user_create");
    vFile.add("user_delete");
    vFile.add("user_get");
    vFile.add("user_getall");
    vFile.add("user_getrights");
    vFile.add("user_logon");
    vFile.add("user_put");
    vFile.add("trace_show");
    vFile.add("trace_on");
    vFile.add("trace_off");
    vFile.add("trace_flush");
    vFile.add("trace_clear");
    vFile.add("trace_prot");
    vFile.add("version");
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
      //Too few parameters
      oDBM.execCmd("help", DBMUtility.OK_OK);

      StringTokenizer m_stLine = new StringTokenizer (oDBM.getAnswer(), "\n");

      while (m_stLine.hasMoreTokens()) {

        String m_sCurrentToken = m_stLine.nextToken();

        for (int i = 0;i < vFile.size();i++) {

          if (m_sCurrentToken.indexOf(vFile.get(i).toString()) >= 0) {

            vFile.remove(i);
            break;
          }
        }
      }

      if (vFile.size() == 0) {

        addMessage("'help'", TestProtocol.INFO, "All commands found.");
      } else {

        for (int i = 0;i < vFile.size();i++) {

          addMessage("'help'", TestProtocol.ERROR, "Could not find: " + vFile.get(i));
        }
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
    return "Help";
  } // end getTestClassId
} //end of class MediumGet