
# vndStream.py
"""Read parts from a module frame

Allows navigation through part names and transparent
handling of conditional compiling
"""

from string import lower
import regex
import yutil

_partSequence = [None, 'define', 'use', 'synonym', 'specification',
        'description', 'code']
_partStart = '%s *:.*\n$'
_partEnd = '\.CM \*-END-\* *%s'
InvalidPartName = 'vdnStream.InvalidPartName'

def _partREs (partName):
    startRE = regex.compile (_partStart % partName, regex.casefold)
    endRE = regex.compile (_partEnd % partName, regex.casefold)
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
        if not result or (self.endpartRE.match (result) != -1):
            raise EOFError, self.currentPart
        self.lineno = self.lineno + 1
        return result

    def _restart (self):
        self.stream = yutil.openFile (self.fname)
        self.lineno = 0
        self.currentPart = None
        self.hideLines = None

    def _searchFor (self, re):
        lineno = self.lineno
        try:
            line = self.__basicReadLine ()
            while line and (re.match (line) == -1):
                lineno = lineno + 1
                line = self.readline ()
        except KeyError: # EOFError:
            raise InvalidPartName

