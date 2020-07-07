
# pascalScanner.py
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
from string import letters, digits, hexdigits, whitespace, lower, split, index, find, strip
import re

from SyntaxNode import SyntaxNode

InvalidCharacter = 'pascalScanner.InvalidCharacter'

_wsRE = re.compile ('[' + whitespace + ']+')
_wsCommentRE = re.compile ('[' + whitespace + ']*[(][*]')
_identRE = re.compile ('[a-zA-Z_][a-zA-Z_0-9]*')
_numberRE = re.compile ('[0-9]+')
_floatRE = re.compile ('[0-9]+[.][0-9]+([eE][+-]?[0-9]+)?')
_hexnumberRE = re.compile ('[0-9a-fA-F]+')

identifierID = 0
literalID = 1
operatorID = 2
keywordID = 3

def buildOperatorList ():
    operators = split ('<> <= >= := .. < > = : , . ; [ ] ( ) + - * - / ^ @')
    result = map (lambda str: (str, len (str)), operators)
    return result

def buildKeywordList ():
    keywords = split ("""
            array   begin   case    const   do      else    end
            for     function goto   if      in      of      otherwise
            packed  procedure record repeat set     then    type
            var     while   with    string  div     mod""")
    dict = {}
    for word in keywords:
        dict [word] = 1
    return dict

_operators = buildOperatorList ()
_keywords = buildKeywordList ()

class Scanner:
    def __init__ (self, stream):
        self.stream = stream
        self.line = stream.readline ()
        self.colno = 0
        self.filename = ''
        self.currentToken = None
        self.keepComments = None
        self.concomLine = []

    def currentPos (self):
        return (self.stream.lineno, self.colno)

    def next (self):
        if self.currentToken:
            result = self.currentToken
            self.currentToken = None
            return result
        self.skipSpace ()
        line = self.line
        colno = self.colno
        comment = None
        if line [colno:colno + 2] == '(*':
            if self.keepComments:
                comment = self.readComments ()
            else:
                self.skipComments ()
            line = self.line
            colno = self.colno
        pos = (self.filename, self.stream.lineno, colno + 1)
        c = line [colno]
        if c == "'":
            token, colno = self.readString (line, colno + 1)
        elif c == '$':
            token, colno = self.readHexNumber (line, colno + 1)
        elif c in digits:
            token, colno = self.readNumber (line, colno)
        elif c in letters or c == '_':
            match = _identRE.match (line, colno)
            value = match.group (0)
            lowValue = lower (value)
            if _keywords.has_key (lowValue):
                token = Keyword (lowValue)
            else:
                token = Identifier (value)
            colno = colno + len (value)
        else:
            for opstring, oplen in _operators:
                if opstring == line [colno:colno + oplen]:
                    token = Operator (opstring)
                    colno = colno + oplen
                    break
            else:
                raise InvalidCharacter, c
        token.pos = pos
        if self.keepComments:
            if comment:
                token.preComment = comment
            comment, colno = self.tryPostComment (line, colno)
            if comment:
                token.postComment = comment
        if self.concomLine:
            #print 'attaching  concom to %s (%x) at %s' % (token, hash(token), pos)
            token.concomLine = self.concomLine
            self.concomLine = []
        self.colno = colno
        #print 'nextToken:', token, token.value, pos
        return token

    def nextFew (self, count):
        result = []
        for i in xrange (count):
            result.append (self.next ())
        return result

    def pushback (self, token):
        self.currentToken = token

    def skipSpace (self):
        ws = _wsRE
        line = self.line
        colno = self.colno
        match = ws.match (line, colno)
        while match:
            start, end = match.span ()
            if end == len (line):
                line = self.stream.readline ()
                colno = 0
            else:
                colno = end
            match = ws.match (line, colno)
        self.line = line
        self.colno = colno

    def skipComments (self):
        self.readComments ()

    def readComments (self):
        comments = []
        while self.line [self.colno:self.colno + 2] == '(*':
            comments.append (self.readComment ())
            self.skipSpace ()
        return comments

    def readComment (self):
        comment = ''
        colno = self.colno + 2
        commentEnd = find (self.line, '*)', colno)
        while commentEnd == -1:
            comment = comment + self.line [colno:]
            self.line = self.stream.readline ()
            colno = 0
            commentEnd = find (self.line, '*)', colno)
        comment = comment + self.line [colno:commentEnd]
        self.colno = commentEnd + 2
        return comment

    def tryPostComment (self, line, colno):
        match = _wsCommentRE.match (line, colno)
        if match:
            self.colno = match.end () - 2
            comment = self.readComment ()
            colno = self.colno
        else:
            comment = None
        return comment, colno

    def readString (self, line, colno):
        apos = "'"
        nextFound = index (line, apos, colno)
        value = ''
        while line [nextFound + 1] == apos:
            value = value + line [colno:nextFound] + apos
            nextFound = index (line, "'", nextFound + 2)
            colno = nextFound + 2
        value = value + line [colno:nextFound]
        colno = nextFound + 1
        if line [colno:colno + 2] == 'xc':
            value = translateHexstring (value)
            escapeAll = 1
            colno = colno + 2
        else:
            value, count = translateEscapes (value)
            escapeAll = (count == len (value))
        if len (value) == 1:
            litval = CharacterLiteral (value)
        else:
            litval = StringLiteral (value, escapeAll)
        return litval, colno

    def readNumber (self, line, colno):
        if line [colno:colno + 2] == '0x':
            token, colno = self.readHexNumber (line, colno + 2)
        else:
            match = _floatRE.match (line, colno)
            if match:
                value = match.group (0)
                colno = colno + len (value)
                token = RealLiteral (value)
                return token, colno
            match = _numberRE.match (line, colno)
            if not match:
                raise 'Invalid Number', line [colno:-1]
            value = match.group (0)
            colno = colno + len (value)
            if line [colno] == '.':
                match = _numberRE.match (line, colno + 1)
                if match:
                    postPoint = match.group (0)
                    colno = colno + 1 + len (postPoint)
                    token = RealLiteral (value + '.' + postPoint)
                else:
                    token = IntegerLiteral (value)
            else:
                token = IntegerLiteral (value)
        return token, colno

    def readHexNumber (self, line, colno):
        match = _hexnumberRE.match (line, colno)
        if not match:
            raise 'Invalid Hexnumber', line [colno:-1]
        value = match.group (0)
        colno = colno + len (value)
        token = HexLiteral (value)
        return token, colno


class Token (SyntaxNode):
    def __init__ (self, id, value):
        self.id = id
        self.value = value

    def asTuple (self):
        return (self.id, self.value)

    def __repr__ (self):
        return '<%s %s>' % (self.__class__.__name__, self.value)

    specialRE = re.compile (r'(\s*[^=]+)\s*=(.*)')
    def getSpecialComment (self, kind, keyWord):
        comment = self.getAttribute (kind)
        if not comment:
            return None
        #
        # try (* ident=value *) syntax
        #
        commentText = strip (comment)
        match = self.specialRE.match (commentText)
        if match:
            ident, value = match.group (1, 2)
            if ident == keyWord:
                return strip (value)
            else:
                return None
        #
        # try (* ident value *) syntax
        #
        words = split (commentText)
        if words [0] != keyWord:
            result = None
        else:
            result = string.join (words [1:])
        return result
    
class Identifier (Token):
    def __init__ (self, value):
        if lower (value) == 'boolean':
            value = 'pasbool'
        Token.__init__ (self, identifierID, value)

class Keyword (Token):
    def __init__ (self, value):
        Token.__init__ (self, keywordID, value)

class Operator (Token):
    def __init__ (self, value):
        Token.__init__ (self, operatorID, value)

class Literal (Token):
    def __init__ (self, value):
        Token.__init__ (self, literalID, value)

class StringLiteral (Literal):
    def __init__ (self, value, escapeAll = None):
        Literal.__init__ (self, value)
        self.escapeAll = escapeAll

class CharacterLiteral (Literal):
    pass

class IntegerLiteral (Literal):
    pass

class RealLiteral (Literal):
    pass

class HexLiteral (Literal):
    pass

hexValues = {
        '0': 0, '1': 1, '2': 2, '3': 3, '4': 4,
        '5': 5, '6': 6, '7': 7, '8': 8, '9': 9,
        'A': 10, 'B': 11, 'C': 12, 'D': 13, 'E': 14, 'F': 15,
        'a': 10, 'b': 11, 'c': 12, 'd': 13, 'e': 14, 'f': 15}

def translateHexstring (value):
    result = ''
    for i in xrange (0, len(value), 2):
        highbyte = hexValues [value [i]]
        lowbyte = hexValues [value [i + 1]]
        ascii = chr (highbyte * 16 + lowbyte)
        result = result + ascii
    return result

def translateEscapes (str):
    backslash = '\\'
    count = 0
    pos = find (str, backslash)
    if pos == -1:
        return str, count
    result = ''
    lastpos = 0
    while pos != -1:
        count = count + 1
        result = result + str [lastpos:pos]
        if len (str) == (pos + 1):
            result = result + backslash
            lastpos = pos + 1
        elif str [pos + 1] == backslash:
            result = result + backslash
            lastpos = pos + 2
        else:
            highbyte = hexValues [str [pos + 1]]
            lowbyte = hexValues [str [pos + 2]]
            ascii = chr (highbyte * 16 + lowbyte)
            result = result + ascii
            lastpos = pos + 3
        pos = find (str, backslash, lastpos)
    return result, count
