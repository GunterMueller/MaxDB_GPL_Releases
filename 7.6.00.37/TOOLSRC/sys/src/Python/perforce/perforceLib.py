
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
import re
import os
import tempfile

def depotList (p4cmd, regex, groupIndex = 1):
    return parseP4 (p4cmd, regex, [groupIndex])

def depot2local (depotList, p4options = ''):
    tmpname = tempfile.mktemp ()
    stream = open (tmpname, 'w')
    stream.write (string.join (depotList, '\n') + '\n')
    stream.close ()
    result = parseP4 (p4options + ' -s -x ' + tmpname + ' fstat', 'clientFile (.*)$', [1])
    os.remove (tmpname)
    return result

def parseP4 (p4cmd, regex, groupIndices = (1,)):
    if type (regex) == type (''):
        regex = re.compile (regex)
    stream = os.popen ('p4 ' + p4cmd, 'r')
    result = []
    line = stream.readline ()
    while line:
        # print line
        match = regex.search (line)
        if match:
            groups = apply (match.group, groupIndices)
            result.append (groups)
        line = stream.readline ()
    stream.close ()
    return result

def execP4 (p4cmd, errorRE = None):
    if errorRE == None:
        null = os.popen ('p4 ' + p4cmd, 'r')
        null.read ()
        rc = null.close ()
        if rc:
            os.system ('pwd')
            raise PerforceError ('p4 failed [%s]: %s' % (rc, p4cmd))
    else:
        _execP4CheckForError (p4cmd, errorRE)

def _execP4CheckForError (p4cmd, errorRE):
    if type (regex) == type (''):
        regex = re.compile (regex)
    stream = os.popen ('p4 ' + p4cmd, 'r')
    result = []
    line = stream.readline ()
    exception = None
    while line:
        # print line
        match = regex.search (line)
        if match:
            exception = PerforceError (match.group (0))
            stream.read ()
            break
        line = stream.readline ()
    stream.close ()
    if exception:
        raise exception

class PerforceError:
    def __init__ (self, msg):
        self.msg = msg

    def __str__ (self):
        return 'Perforce: ' + self.msg

error = PerforceError

class PerforceSession:
    def __init__ (self, client = None, host = None, port = None,
            password = None, user = None, options = None):
        if options != None:
            self.options = options
        else:
            self.options = ConnectOptions (client, host, port, password, user)
        self.optionsString = self.options.getOptionsString ()

    def setPort (self, port):
        self.options.port = port
        self.optionsString = self.options.getOptionsString (1)    
        
    def parseOutput (self, p4cmd, regex, groupIndices = (1,)):
        return parseP4 (self.optionsString + p4cmd, regex, groupIndices)

    def execute (self, p4cmd, errorRE = None):
        execP4 (self.optionsString + p4cmd, errorRE)

    def executeFileList (self, p4cmd, fileList, errorRE = None):
        if not fileList:
            return
        # name tmp file
        tmpname = tempfile.mktemp ()
        # write tmp file
        stream = open (tmpname, 'w')
        for filename in fileList:
            stream.write (filename + '\n')
        stream.close ()
        #execute p4
        try:
            execP4 (self.optionsString + ' -x ' + tmpname + ' ' + p4cmd, errorRE)
        finally:
            os.remove (tmpname)

    def getPipe (self, cmd):
        return os.popen ('p4 ' + self.optionsString + cmd)

    def getLines (self, cmd):
        stream = self.getPipe (cmd)
        return stream.readlines ()
        stream.close ()

    def getResultIterator (self, cmd):
        return LineIterator (self.getPipe (cmd))

    def fstat (self, filename):
        lines = self.getLines ('fstat ' + filename)
        if len (lines) == 0:
            return None
        result = FileStat ()
        for line in lines:
            parts = string.split (line)
            if len (parts) == 3:
                setattr (result, parts [1], parts [2])
        return result

    def listdir (self, dir):
        lines = self.getLines ('fstat ' + dir + '/*')
        result = []
        for line in lines:
            words = string.split (line)
            if (len (words) > 2) and (words [1] == 'depotFile'):
                result.append (words [2])
        return result

    def copyFile (self, perforceFile, localFile):
        cmd = 'print -q -o ./"' + localFile + '" "' + perforceFile + '"'
        self.execute (cmd)
        mode = os.stat (localFile) [0]
        if not (0200 & mode):
            os.chmod (localFile, mode | 0200)

    def readFile (self, perforceFile):
        stream = self.getPipe ('print -q ' + perforceFile)
        result = stream.read ()
        stream.close ()
        return result

class LineIterator:
    def __init__ (self, stream):
        self.stream = stream

    def __getitem__ (self, index):
        result = self.stream.readline ()
        if not result:
            self.stream.close ()
            raise IndexError
        return result

class FileStat:
    pass

_modifier = (
    ('x', 'exec'),
    ('k', 'keyword'),
    ('c', 'compressed'),
    ('l', 'long'),
    ('u', 'uncompressed'),
)

def parseFiletype (filetype):
    options = []
    for modifier, word in _modifier:
        if filetype [0] == modifier:
            options.append (word)
            filetype = filetype [1:]
    return filetype, options

class ConnectOptions:
    def __init__ (self, client = None, host = None, port = None,
            password = None, user = None):
        self.client = client
        self.host = host
        self.port = port
        self.password = password
        self.user = user
        self.optionString = self.buildOptionString ()

    def getOptionsString (self, forceRebuild = None):
        if forceRebuild:
            self.optionString = self.buildOptionString ()
        return self.optionString

    def buildOptionString (self):
        result = ''
        if self.client:
            result = result + ' -c ' + self.client
        if self.host:
            result = result + ' -H ' + self.host
        if self.port:
            result = result + ' -p ' + str (self.port)
        if self.password:
            result = result + ' -P ' + self.password
        if self.user:
            result = result + ' -u ' + self.user
        return result + ' '

if __name__ == "__main__":
    p4 = PerforceSession ()
    data = p4.readFile ('//sapdb/V73/c_00/develop/sys/config/noOpenSource/testing.dta')
    for line in string.split (data, '\n'):
        print repr (line)

