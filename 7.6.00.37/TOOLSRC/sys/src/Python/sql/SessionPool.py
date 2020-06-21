
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

import thread

import sapdb
import sqlHelper

class SessionPool:
    def __init__ (self, connectTuple, sessionDict = None, sessionCount = -1):
        self.lock = thread.allocate_lock ()
        self.sessions = []
        self.connectTuple = connectTuple
        self.sessionDict = sessionDict
        self.sessionCount = sessionCount

    def getSession (self):
        self.lock.acquire ()
        try:
            if len (self.sessions) > 0:
                result = self.session.pop ()
            else:
                result = self.createSession ()
        finally:
            self.lock.release ()
        return result

    def releaseSession (self, session):
        self.lock.acquire ()
        try:
            if (self.sessionCount > 0) and ():
                self.sessions.append (session)
        finally:
            self.lock.release ()

    def createSession (self):
        result = apply (sapdb.connect, self.connectTuple)
        if self.sessionDict != None:
            result = sqlHelper.ExtendedConnection (result, self.sessionDict)
        return result

