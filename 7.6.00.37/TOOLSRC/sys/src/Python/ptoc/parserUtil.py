
# parserUtil.py
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


def readUntil (scanner, tokenid, value, startToken = None):
    """Return a list of tokens up to and including a match"""
    # print "readUntil (..., tokenid = %d, value = '%s', ...)" % (tokenid, value)
    result = []
    if startToken:
        result.append (startToken)
    searchval = (tokenid, value)
    token = scanner.next ()
    while searchval != token.asTuple ():
        result.append (token)
        token = scanner.next ()
    result.append (token)
    return result

def readNested (scanner, startToken, tokenPatterns):
    result = [startToken]
    dict = {}
    for id, startval, endval in tokenPatterns:
        dict [(id, startval)] = (id, endval)
    top = dict [(startToken.id, startToken.value)]
    stack = [top]
    while len (stack) > 0:
        token = scanner.next ()
        result.append (token)
        tokenTuple = token.asTuple ()
        if tokenTuple == top:
            del stack [-1]
            try:
                top = stack [-1]
                # print 'popping to', top
            except IndexError:
                top = None
        else:
            try:
                top = dict [tokenTuple]
                # print 'pushing', tokenTuple
                stack.append (top)
            except KeyError:
                pass
    return result

def tokenUntil (tokens, startPos, tokenid, value):
    pos = startPos
    stopToken = (tokenid, value)
    while tokens [pos].asTuple () != stopToken:
        pos = pos + 1
    return tokens [startPos:pos + 1]

def tokenUntilAny (tokens, startPos, stopTokens):
    pos = startPos
    while not tokens [pos].asTuple () in stopTokens:
        pos = pos + 1
    return tokens [startPos:pos + 1]

def splitTokens (tokens, tokenid, value):
    result = []
    pos = 0
    while pos < len (tokens):
        try:
            part = tokenUntil (tokens, pos, tokenid, value)
        except IndexError:
            part = tokens [pos:]
        result.append (part)
        pos = pos + len (part)
    return result

def stringUntil (tokens, startPos, stopValue):
    pos = startPos
    try:
        while tokens [pos]!= stopValue:
            pos = pos + 1
    except IndexError:
        pos = len (tokens) - 1
    return tokens [startPos:pos + 1]

def stringUntilAny (tokens, startPos, stopValues):
    pos = startPos
    try:
        while not (tokens [pos] in stopValues):
            pos = pos + 1
    except IndexError:
        pos = len (tokens) - 1
    return tokens [startPos:pos + 1]

def tokenNested (tokens, pos, startToken, tokenPatterns):
    result = [startToken]
    dict = {}
    for id, startval, endval in tokenPatterns:
        dict [(id, startval)] = (id, endval)
    top = dict [startToken]
    stack = [top]
    while len (stack) > 0:
        pos = pos + 1
        token = tokens [pos]
        result.append (token)
        tokenTuple = token.asTuple ()
        if tokenTuple == top:
            del stack [-1]
            try:
                top = stack [-1]
                # print 'popping to', top
            except IndexError:
                top = None
        else:
            try:
                top = dict [tokenTuple]
                # print 'pushing', tokenTuple
                stack.append (top)
            except KeyError:
                pass
    return result

    