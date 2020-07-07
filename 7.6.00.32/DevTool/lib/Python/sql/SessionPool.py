
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

