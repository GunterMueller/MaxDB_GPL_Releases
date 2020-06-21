package com.sap.sapdb.DBMTest.util.user;

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

public class DBMUserManagementException extends Exception {

    /**
     * Use this one if ther is no special userID involved 
     * 
     * @param aAction action at which the error occurred
     */
    public DBMUserManagementException(String aAction, String aMessage) {
        m_Action = aAction;
        m_Message = aMessage;
        m_UserID = null;
    }
    
    public DBMUserManagementException(String aAction, String aUserID, String aMessage) {
        m_Action = aAction;
        m_Message = aMessage;
        m_UserID = aUserID;
    }
    
    /**
     * get the message... 
     */
    public String getMessage() {
        StringBuffer message = new StringBuffer();
        if( m_Action != null )
            message.append("action: " + m_Action);
            
        if( m_UserID != null )
            message.append(" user: " + m_UserID);
            
        if( m_Message != null )
            message.append(" cause: " + m_Message);
            
        if( message.length() == 0)
            return message.toString();
        else
            return '(' + message.toString() + ')';     
     }

    /**
     * new toString method
     */
    public String toString() {
        String message = getMessage();
        return this.getClass().getName() + (message.length()>0?": " + message:"");
    }

    private final String m_Action;
    private final String m_UserID;
    private final String m_Message;
}
