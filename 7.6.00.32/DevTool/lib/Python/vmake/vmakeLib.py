
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
import os
import string
import re

independentVersion = ' '

class VmakeItem:
    name = '<unknown>'
    version = independentVersion

    def __init__ (self, kind):
        self.kind = kind

    def __repr__ (self):
        return "<VMAKE %s '%s'>" % (self.kind, self.name)

    def asKey (self):
        return (self.name, self.version)

class VmakeParser:
    def __init__ (self):
        self.handler = {}
        self.unknownHandler = unknownHandler

    def registerHandler (self, kind, handler):
        if kind == None:
            self.unknownHandler = handler
        else:
            self.handler [kind] = handler

    def callHandler (self, kind, item):
        try:
            handler = self.handler [kind]
        except KeyError:
            handler = self.unknownHandler
        handler (item)

    def parseStream (self, instream):
        try:
            while 1:
                self.parseItem (instream)
        except EOFError:
            pass

    def parseCollection (self, target, speed = 's'):
        if type (target) == type ([]):
            target = string.join (target)
        instream = PipeReader ('perl -S im' + speed + '.pl -c ' + target)
        self.parseStream (instream)

    def parseRemoteCollection (self, platform, target, speed = 's'):
        if type (target) == type ([]):
            target = string.join (target)
        instream = PipeReader ('perl -S remim' + speed + '.pl '
            + platform + ' -c ' + target)
        line = instream.readline ()
        while not string.strip (line):
            # skip remim? output by skipping to first empty line
            line = instream.readline ()
        self.parseStream (instream)

    simplePropertyRE = re.compile (' +\([a-z_]+\) +\([^ ]+\)')
    nameRE = re.compile (' +\([a-z]+\)')
    valueRE = re.compile (" +'\([^']*\)'[ \t]+'\([^']*\)'")
    optionsRE = re.compile (" +'\([^']*\)'")

    def parseItem (self, instream):
        line = instream.skipEmptyLines ()
        kind = string.strip (line)
        item = VmakeItem (kind)
        line = instream.readline ()
        simplePropertyRE = self.simplePropertyRE
        nameRE = self.nameRE
        valueRE = self.valueRE
        optionsRE = self.optionsRE
        match = simplePropertyRE.match (line)
        while match:
            propName, propValue = match.group (1, 2)
            if propValue [0] == "'":
                propValue = propValue [1:-1]
            elif propValue [0] in "0123456789":
                propValue = int (propValue)
            setattr (item, propName, propValue)
            line = instream.readline ()
            match = simplePropertyRE.match (line)
        inProperties = 1
        while inProperties:
            nameMatch, valueMatch, optionsMatch = matchFirst (line, 
                nameRE, valueRE, optionsRE)
            if nameMatch:
                currentList = []
                setattr (item, nameMatch.group (1), currentList)
            elif valueMatch:
                currentList.append (valueMatch.group (1, 2))
            elif optionsMatch:
                currentList.append (optionsMatch.group (1))
            else:
                instream.pushbackLine (line)
                inProperties = None
                break
            line = instream.readline ()
        # self.findItemBreak (instream, line)
        self.callHandler (kind, item)
        return item

    def findItemBreak (self, instream, line):
        while 1:
            if not line:
                # empty lines as delimiter
                break
            if line [0] != ' ':
                # new items start at first column
                instream.pushbackLine (line)
                break
            line = instream.readline ()

def matchFirst (text, *rex):
    result = [None] * len (rex)
    for i in xrange (len (rex)):
        match = rex [i].match (line)
        if match:
            result [i] = match
            break
    return result

class StreamReader:
    def __init__ (self, instream):
        self.instream = instream
        instream.readline ()
        self.currentLine = None

    def readline (self):
        if self.currentLine != None:
            result = self.currentLine
            self.currentLine = None
        else:
            result = self.instream.readline ()
            if not result:
                raise EOFError
            result = result [:-1]   # strip \n
        return result

    def pushbackLine (self, line):
        self.currentLine = line

    def skipEmptyLines (self):
        while 1:
            line = string.strip (self.readline ())
            if line:
                if line == 'WARNING: newer file on later VMAKE_PATH':
                    continue
                if line [0] == '(':
                    continue
                if line == 'Warning: There are different ways of writings':
                    self.readline ()
                    continue
                return line

class PipeReader (StreamReader):
    def __init__ (self, command):
        instream = os.popen (command, 'r')
        StreamReader.__init__ (self, instream)

def unknownHandler (item):
    sys.stderr.write ('no handler defined for ' + `item` + '\n')

def ignoreHandler (item):
    pass

