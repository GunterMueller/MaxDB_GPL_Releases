
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#

import sys
import string
from UserDict import UserDict

caseSensitive = 'true'

class CommandCollection:
    def __init__ (self, session, stringDict):
        self.session = session
        self.stringDict = stringDict

    def getStringForCommand (self, commandName):
        return self.stringDict [commandName]

    def __getattr__ (self, name):
        try:
            cmd = self.getStringForCommand (name)
        except KeyError:
            raise AttributeError (name)
        prepared = self.session.prepare (cmd)
        callable = prepared.execute
        setattr (self, name, callable)
        return callable

class ExtendedConnection:
    def __init__ (self, connection, macroDict = None):
        self.connection = connection
        self.stmtCache = {}
        self.macroDict = macroDict

    def sqlX (self, cmd, args = [], rcToIgnore = []):
        try:
            prepared = self.stmtCache [cmd]
        except KeyError:
            expandedCommand = self.expandCommand (cmd)
            try:
                prepared = self.connection.prepare (expandedCommand)
            except:
                clazz, exc, backtrace = sys.exc_info ()
                showErrorPos (expandedCommand, exc.errorPos)
                raise
            self.stmtCache [cmd] = prepared
        try:
            result = prepared.execute (args)
        except:
            clazz, exc, backtrace = sys.exc_info ()
            try:
                rc = getattr (exc, 'errorCode')
            except AttributeError:
                rc = None
            if rc in rcToIgnore:
                result = None
            else:
                raise exc
        return result

    def explainSQL (self, cmd, args = []):
        expandedCmd = self.expandCommand (cmd)
        print '======================================'
        print expandedCmd
        print '--------------------------------------'
        cursor = self.connection.sqlX ('explain ' + expandedCmd, args)
        for row in cursor:
            print row


    def __getattr__ (self, key):
        return getattr (self.connection, key)

    def expandCommand (self, cmd):
        if self.macroDict is not None:
            #print cmd
            expandedCommand = cmd % self.macroDict
            #print expandedCommand
        else:
            expandedCommand = cmd
        return expandedCommand

class SelfDict (UserDict):
    def __getitem__ (self, key):
        result = self.get (key)
        if result is None:
            result = '"' + key + '"'
        return result

class SchemaManager:
    def __init__ (self, schema, session, caseSensitive = None):
        if caseSensitive:
            self.schema = map (self.quoteName, schema)
        else:
            self.schema = schema
        self.session = session
        self.caseSensitive = caseSensitive

    def create (self):
        for kind, name, definition in self.schema:
            print kind, name
            cmd = "create %s %s %s" % (kind, name, definition)
            self.session.sql (cmd)
        self.session.commit ()

    def drop (self):
        for kind, name, definition in self.schema:
            try:
                self.session.sql ("drop %s %s" % (kind, name))
            except:
                pass
        self.session.commit ()

    def recreate (self):
        self.drop ()
        self.create ()

    def quoteName (self, schemaTuple):
        kind, name, definition = schemaTuple
        name = '"' + name + '"'
        return kind, name, definition

    def callByName (self, command):
        getattr (self, command) ()


def showErrorPos (sql, errorPos):
    lines = string.split (sql, '\n')
    lineStart = 1
    for line in lines:
        print line
        lineEnd = lineStart + len (line)
        if lineStart <= errorPos <= lineEnd:
            errorCol = errorPos - lineStart - 1
            print ('*' * errorCol) + '^'
        lineStart = lineEnd + 1

def connect (options, userInfoName = 'userInfo',
        dbnameName = 'dbname', nodeName = 'node',
        propName = 'connectProperties', module = None):
    userInfo = getattr (options, userInfoName)
    dbname = getattr (options, dbnameName)
    node = getattr (options, nodeName)
    try:
        properties = getattr (options, propName)
    except AttributeError:
        properties = ''
    pos = string.find (userInfo, ',')
    if pos != -1:
        username = userInfo [:pos]
        pwd = userInfo [pos + 1:]
    else:
        username = userInfo
        pwd = ''
    if not node:
        pos = string.find (dbname, ':')
        if pos != -1:
            node = dbname [:pos]
            dbname = dbname [pos + 1:]
        else:
            node = ''
    if module == None:
        import sapdb
        module = sapdb
    result = module.connect (username, pwd, dbname, node, properties)
    return result

stddbOptions = [
    ('u', 'userInfo', ':', None, 'user <name>,<pwd>'),
    ('d', 'dbname', ':', None, 'db name'),
    ('n', 'node', ':', '', 'host name'),
    ('P', 'connectProperties', '::', None, 'connect properties'),
    ]


