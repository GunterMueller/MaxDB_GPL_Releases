
# vndStream.py
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

"""Read parts from a module frame

Allows navigation through part names and transparent
handling of conditional compiling
"""

from string import lower
import re
import yutil

_partSequence = [None, 'define', 'use', 'synonym', 'specification',
        'description', 'code']
_partStart = '%s *:.*\n$'
_partEnd = '\.CM \*-END-\* *%s'
InvalidPartName = 'vdnStream.InvalidPartName'

def _partREs (partName):
    startRE = re.compile (_partStart % partName, re.I)
    endRE = re.compile (_partEnd % partName, re.I)
    return startRE, endRE

class VDNStream:
    def __init__ (self, fname, concomHandler = None):
        self.fname = fname
        self._restart ()
        self.hideText = None
        self.concomHandler = concomHandler

    def openPart (self, partName):
        startRE, self.endpartRE = _partREs (partName)
        lowerPart = lower (partName)
        for pname in _partSequence:
            if lowerPart == pname:
                self._restart ()
                break;
            if lowerPart == self.currentPart:
                # no need to search further
                break;
        self._searchFor (startRE)
        self.currentPart = lowerPart

    def readline (self):
        line = self.__basicReadLine ()
        if line [0] == '&':
            self.concomHandler (line, self)
            return '\n'
        if self.hideText:
            return '\n'
        return line
        
    def __basicReadLine (self):
        result = self.stream.readline ()
        if not result or self.endpartRE.match (result):
            raise EOFError, self.currentPart
        self.lineno = self.lineno + 1
        return result

    def _restart (self):
        self.stream = yutil.openFile (self.fname)
        self.lineno = 0
        self.currentPart = None
        self.hideLines = None

    def _searchFor (self, rex):
        lineno = self.lineno
        try:
            line = self.__basicReadLine ()
            while line and not rex.match (line):
                lineno = lineno + 1
                line = self.readline ()
        except KeyError: # EOFError:
            raise InvalidPartName

