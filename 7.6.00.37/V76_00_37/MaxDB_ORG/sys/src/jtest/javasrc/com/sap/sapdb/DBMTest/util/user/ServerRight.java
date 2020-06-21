package com.sap.sapdb.DBMTest.util.user;

import java.util.HashMap;

/*
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

public class ServerRight {

    public static final int DBInfoRead = 0;
    public static final int SystemCmd = 1;
    public static final int ExecLoad = 2;
    public static final int UserMgm = 3;
    public static final int DBFileRead = 4;
    public static final int Backup = 5;
    public static final int InstallMgm = 6;
    public static final int LoadSysTab = 7;
    public static final int ParamCheckWrite = 8;
    public static final int ParamFull = 9;
    public static final int ParamRead = 10;
    public static final int DBStart = 11;
    public static final int DBStop = 12;
    public static final int Recovery = 13;
    public static final int AccessSQL = 14;
    public static final int AccessUtility = 15;
    public static final int SharedMemoryAdmin = 16;
    public static final int SchedulerAdmin = 17;
    public static final int SchedulerUse = 18;


    public static ServerRight get(int aRight) {
        return (ServerRight) m_ServerRights.get(new Integer(aRight));
    }

    public static HashMap getAllRights() {
        HashMap outMap = new HashMap(m_ServerRights);
        return outMap;
    }

    private ServerRight(int rightIndex, String rightText) {
        m_ID = rightIndex;
        m_Text = rightText;
    }

    public final int m_ID;
    public final String m_Text;
    
    /**
     * filled in static initializer
     */
    private static HashMap m_ServerRights = null;

    static {
        m_ServerRights = new HashMap();

        m_ServerRights.put(
            new Integer(DBInfoRead), 
            new ServerRight(DBInfoRead, "DBInfoRead"));        

        m_ServerRights.put(
            new Integer(SystemCmd), 
            new ServerRight(SystemCmd, "SystemCmd"));        

        m_ServerRights.put(
            new Integer(ExecLoad), 
            new ServerRight(ExecLoad, "ExecLoad"));        

        m_ServerRights.put(
            new Integer(UserMgm), 
            new ServerRight(UserMgm, "UserMgm"));        

        m_ServerRights.put(
            new Integer(DBFileRead), 
            new ServerRight(DBFileRead, "DBFileRead"));        

        m_ServerRights.put(
            new Integer(Backup), 
            new ServerRight(Backup, "Backup"));        

        m_ServerRights.put(
            new Integer(InstallMgm), 
            new ServerRight(InstallMgm, "InstallMgm"));        

        m_ServerRights.put(
            new Integer(LoadSysTab), 
            new ServerRight(LoadSysTab, "LoadSysTab"));        

        m_ServerRights.put(
            new Integer(ParamCheckWrite), 
            new ServerRight(ParamCheckWrite, "ParamCheckWrite"));        

        m_ServerRights.put(
            new Integer(ParamFull), 
            new ServerRight(ParamFull, "ParamFull"));        

        m_ServerRights.put(
            new Integer(ParamRead), 
            new ServerRight(ParamRead, "ParamRead"));        

        m_ServerRights.put(
            new Integer(DBStart), 
            new ServerRight(DBStart, "DBStart"));        

        m_ServerRights.put(
            new Integer(DBStop), 
            new ServerRight(DBStop, "DBStop"));        

        m_ServerRights.put(
            new Integer(Recovery), 
            new ServerRight(Recovery, "Recovery"));        

        m_ServerRights.put(
            new Integer(AccessSQL), 
            new ServerRight(AccessSQL, "AccessSQL"));        

        m_ServerRights.put(
            new Integer(AccessUtility), 
            new ServerRight(AccessUtility, "AccessUtility"));        

        m_ServerRights.put(
            new Integer(SharedMemoryAdmin), 
            new ServerRight(SharedMemoryAdmin, "SharedMemoryAdmin"));        

        m_ServerRights.put(
            new Integer(SchedulerAdmin), 
            new ServerRight(SchedulerAdmin, "SchedulerMgm"));        

        m_ServerRights.put(
            new Integer(SchedulerUse), 
            new ServerRight(SchedulerUse, "Scheduling"));        
    }
}
