
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

import string

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

def connect (options, userInfoName = 'userInfo',
        dbnameName = 'dbname', nodeName = 'node',
        propName = 'connectProperties'):
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
    import sapdb
    result = sapdb.connect (username, pwd, dbname, node, properties)
    return result

