
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
import os
import re
import cStringIO

imaginaryLine = '---imaginary line---\n'

class AbstractDiff:
    def __init__ (self, fileArgs):
        self.fileArgs = fileArgs
        self.diffList = None

    def __len__ (self):
        return len (self.getDiffList ())

    def getDiffList (self):
        if self.diffList == None:
            self.createDiffList ()
        return self.diffList

    def createDiffList (self):
        quotedArgs = map (quote, self.fileArgs)
        cmd = self.diffcmd + string.join (quotedArgs)
        data = os.popen (cmd, 'rt').read ()
        data = '\n' + data
        diffList = self.diffList = []
        matcher = self.diffRE
        pos = 0
        match = matcher.search (data, pos)
        while match:
            diffList.append (self.createDiffChunk (data, match))
            pos = match.end ()
            match = matcher.search (data, pos)

    def dump (self):
        for diff in self.getDiffList ():
            diff.dump ()

def quote (str):
    return '"' + str + '"'

class Diff2 (AbstractDiff):
    def __init__ (self, file1, file2):
        self.file1 = file1
        self.file2 = file2
        AbstractDiff.__init__ (self, [file1, file2])

    diffcmd = 'diff -b '
    diffRE = re.compile (r'\n(\d+)(,\d+)?([adc])(\d+)(,\d+)?')

    def createDiffChunk (self, data, match):
        mineFrom, mineTo, kind, yoursFrom, yoursTo = match.group (1, 2, 3, 4, 5)
        myRange = translateStrings (mineFrom, mineTo)
        yourRange = translateStrings (yoursFrom, yoursTo)
        result = _FactoryDict [kind] (myRange, yourRange, match.group (0))
        return result

    def getPatchedFiles (self):
        mine = []
        yours = []
        myLines = open (self.file1, 'rt').readlines ()
        yourLines = open (self.file2, 'rt').readlines ()
        myPos = 0
        yourPos = 0
        for diff in self.getDiffList ():
            diffKind = diff.kind
            myFrom, myTo = diff.myRange
            yourFrom, yourTo = diff.yourRange
            #
            # add common data
            #
            commonChunk = myLines [myPos:myFrom]
            mine.append (('s', commonChunk, commonChunk, None))
            yours.append (('s', commonChunk, commonChunk, None))
            #
            # add diff data
            #
            if diffKind == 'a':
                myChunk = [imaginaryLine] * (yourTo - yourFrom)
                myKind = 'd'
                yourChunk = yourLines [yourFrom:yourTo]
                yourKind = 'a'
            elif diffKind == 'd':
                myChunk = myLines [myFrom:myTo]
                myKind = 'a'
                yourChunk = [imaginaryLine] * (myTo - myFrom)
                yourKind = 'd'
            elif diffKind == 'c':
                myChunk = myLines [myFrom:myTo]
                yourChunk = yourLines [yourFrom:yourTo]
                sizeDiff = (myTo - myFrom) - (yourTo - yourFrom)
                if sizeDiff >= 0:
                    yourChunk = yourChunk + ([imaginaryLine] * sizeDiff)
                else:
                    myChunk = myChunk + ([imaginaryLine] * -sizeDiff)
                myKind = yourKind = 'c'
            mine.append ((myKind, myChunk, yourChunk, diff))
            yours.append ((yourKind, yourChunk, myChunk, diff))
            myPos = myTo
            yourPos = yourTo
        #
        # add remaining common data
        #
        commonChunk = myLines [myPos:]
        mine.append (('s', commonChunk, commonChunk, None))
        yours.append (('s', commonChunk, commonChunk, None))
        #
        #
        #
        return mine, yours

    def getSplitFile (self):
        result = []
        myLines = open (self.file1, 'rt').readlines ()
        yourLines = open (self.file2, 'rt').readlines ()
        myPos = 0
        yourPos = 0
        for diff in self.getDiffList ():
            diffKind = diff.kind
            myFrom, myTo = diff.myRange
            yourFrom, yourTo = diff.yourRange
            #
            # add common data
            #
            commonChunk = myLines [myPos:myFrom]
            result.append (('s', commonChunk, commonChunk, None))
            #
            # add diff data
            #
            if diffKind == 'a':
                myChunk = [imaginaryLine] * (yourTo - yourFrom)
                yourChunk = yourLines [yourFrom:yourTo]
            elif diffKind == 'd':
                myChunk = myLines [myFrom:myTo]
                yourChunk = [imaginaryLine] * (myTo - myFrom)
            elif diffKind == 'c':
                myChunk = myLines [myFrom:myTo]
                yourChunk = yourLines [yourFrom:yourTo]
                sizeDiff = (myTo - myFrom) - (yourTo - yourFrom)
                if sizeDiff >= 0:
                    yourChunk = yourChunk + ([imaginaryLine] * sizeDiff)
                else:
                    myChunk = myChunk + ([imaginaryLine] * -sizeDiff)
            result.append ((diffKind, myChunk, yourChunk, diff))
            myPos = myTo
            yourPos = yourTo
        #
        # add remaining common data
        #
        commonChunk = myLines [myPos:]
        result.append (('s', commonChunk, commonChunk, None))
        #
        #
        #
        return result

    def getLineNumbers (self):
        myNumbers = cStringIO.StringIO ()
        yourNumbers = cStringIO.StringIO ()
        myPos = 0
        yourPos = 0
        for diff in self.getDiffList ():
            diffKind = diff.kind
            myFrom, myTo = diff.myRange
            yourFrom, yourTo = diff.yourRange
            #
            # add common data
            #
            fillLineNumbers (myNumbers, myPos, myFrom, '=')
            fillLineNumbers (yourNumbers, yourPos, yourFrom, '=')
            #
            # add diff data
            #
            if diffKind == 'a':
                myNumbers.write ('\n' * (yourTo - yourFrom))
                fillLineNumbers (yourNumbers, yourFrom, yourTo, 'a')
            elif diffKind == 'd':
                fillLineNumbers (myNumbers, myFrom, myTo, 'd')
                yourNumbers.write ('\n' * (myTo - myFrom))
            elif diffKind == 'c':
                fillLineNumbers (myNumbers, myFrom, myTo, 'c')
                fillLineNumbers (yourNumbers, yourFrom, yourTo, 'c')
                sizeDiff = (myTo - myFrom) - (yourTo - yourFrom)
                if sizeDiff >= 0:
                    yourNumbers.write ('\n' * sizeDiff)
                else:
                    myNumbers.write ('\n' *  -sizeDiff)
            myPos = myTo
            yourPos = yourTo
        #
        # add remaining common data
        #
        # fillLineNumbers (myNumbers, myPos, myFrom)
        # fillLineNumbers (yourNumbers, yourPos, yourFrom)
        return yourNumbers.getvalue (), myNumbers.getvalue ()


class DiffChunk:
    def __init__ (self, myRange, yourRange, diffStr = ''):
        self.myRange = myRange
        self.yourRange = yourRange
        self.diffString = diffStr

    def dump (self):
        sys.stdout.write (str (self) + '\n')

    def __str__ (self):
        return '<%s %s:%s %s>' % (self.__class__.__name__,
            self.myRange, self.yourRange, self.diffString)

def fixDiffRange (range):
    fromVal, toVal = range
    fromVal = fromVal - 1 # then lines [fromVal:toVal] gives data
    return fromVal, toVal

class AddDiff (DiffChunk):
    kind = 'a'
    def __init__ (self, myRange, yourRange, diffStr = ''):
        DiffChunk.__init__ (self, myRange, fixDiffRange (yourRange), diffStr)

class ChangeDiff  (DiffChunk):
    kind = 'c'
    def __init__ (self, myRange, yourRange, diffStr = ''):
        DiffChunk.__init__ (self, fixDiffRange (myRange),
            fixDiffRange (yourRange), diffStr)

class DeleteDiff (DiffChunk):
    kind = 'd'
    def __init__ (self, myRange, yourRange, diffStr = ''):
        DiffChunk.__init__ (self, fixDiffRange (myRange), yourRange, diffStr)

_FactoryDict = {
    'a': AddDiff,
    'c': ChangeDiff,
    'd': DeleteDiff,
}

def translateStrings (fromString, toString):
    fromInt = int (fromString)
    if toString:
        toInt = int (toString [1:])
    else:
        toInt = fromInt
    return fromInt, toInt

class Diff3 (AbstractDiff):
    def __init__ (self, base, mine, other):
        self.base = base
        self.mine = mine
        self.other = other
        AbstractDiff.__init__ (self, [mine, base, other])

    diffcmd = 'diff3 '
    diffRE = re.compile (r'(\d+)(,\d+)?([adc])(\d+)(,\d+)?')

    def createDiffList (self):
        quotedArgs = map (quote, self.fileArgs)
        cmd = self.diffcmd + string.join (quotedArgs)
        data = '\n' + os.popen (cmd, 'rt').read ()
        # open ('diff.out', 'wt').write (data)
        diffList = self.diffList = []
        sepRE = re.compile ('\n====([0-9]+)?')
        commandRE = re.compile ('([123]):([0-9]+)(,[0-9]+)?([ac])\n?')
        chunks = sepRE.split (data)
        for number, rest in makeTuple (chunks [1:], 2):
            if number:
                number = int (number)
            #print rest
            splittet = commandRE.split (rest)
            #print splittet
            list = makeTuple (splittet [1:], 5)
            #print list
            variations = [None] * 4
            for index, fromVal, toVal, command, lines in list:
                variations [int (index)] = Diff3Info (command, fromVal, toVal, lines)
            diffList.append (Diff3Chunk (number, variations [2],
                variations [1], variations [3]))

    def getSplitFile (self):
        result = []
        baseLines = open (self.base, 'rt').readlines ()
        myLines = open (self.mine, 'rt').readlines ()
        otherLines = open (self.other, 'rt').readlines ()
        basePos = 0
        myPos = 0
        otherPos = 0
        join = string.join
        for diff in self.getDiffList ():
            #
            # add shared data
            #
            baseStart, baseEnd = diff.baseInfo.range
            mineStart, mineEnd = diff.mineInfo.range
            otherStart, otherEnd = diff.otherInfo.range
            commonChunk = join (myLines [myPos:mineStart], '')
            #diff.dump ()
            #print commonChunk [:-1]
            result.append (('s', commonChunk, commonChunk, commonChunk, diff))
            #
            # add differences
            #
            baseChunk = baseLines [baseStart:baseEnd]
            myChunk = myLines [mineStart:mineEnd]
            otherChunk = otherLines [otherStart:otherEnd]
            baseChunk, myChunk, otherChunk = alignChunks (baseChunk, myChunk, otherChunk)
            #diff.dump ()
            #print '#ifdef BASE'
            #print baseChunk [:-1]
            #print '#elif MINE'
            #print myChunk [:-1]
            #print '#elif OTHER'
            #print otherChunk [:-1]
            #print '#endif'
            result.append (('c', baseChunk, myChunk, otherChunk, diff))
            basePos = baseEnd
            myPos = mineEnd
            otherPos = otherEnd
        #
        # add remaining common data
        #
        commonChunk = join (myLines [myPos:], '')
        #print commonChunk [:-1]
        result.append (('s', commonChunk, commonChunk, commonChunk, None))
        #
        #
        #
        return result

    def getLineNumbers (self):
        baseNumbers = cStringIO.StringIO ()
        myNumbers = cStringIO.StringIO ()
        otherNumbers = cStringIO.StringIO ()
        basePos = 0
        myPos = 0
        otherPos = 0
        for diff in self.getDiffList ():
            #
            # add numbers
            #
            baseStart, baseEnd = diff.baseInfo.range
            myStart, myEnd = diff.mineInfo.range
            otherStart, otherEnd = diff.otherInfo.range
            fillLineNumbers (baseNumbers, basePos, baseStart, '=')
            fillLineNumbers (myNumbers, myPos, myStart, '=')
            fillLineNumbers (otherNumbers, otherPos, otherStart, '=')
            #
            # fill changes
            #
            fillLineNumbers (baseNumbers, baseStart, baseEnd, 'c')
            fillLineNumbers (myNumbers, myStart, myEnd, 'c')
            fillLineNumbers (otherNumbers, otherStart, otherEnd, 'c')
            #
            # fill missing lines
            #
            baseLen = baseEnd - baseStart
            myLen = myEnd - myStart
            otherLen = otherEnd - otherStart
            maxLen = max (baseLen, myLen, otherLen)
            baseNumbers.write ('\n' * (maxLen - baseLen))
            myNumbers.write ('\n' * (maxLen - myLen))
            otherNumbers.write ('\n' * (maxLen - otherLen))
            #
            # move forward
            #
            basePos = baseEnd
            myPos = myEnd
            otherPos = otherEnd
        #
        # add remaining common data
        #
        # fillLineNumbers (myNumbers, myPos, myFrom)
        # fillLineNumbers (yourNumbers, yourPos, yourFrom)
        return (
            baseNumbers.getvalue (),
            myNumbers.getvalue (),
            otherNumbers.getvalue ())

def alignChunks (*chunks):
    lineCounts = map (len, chunks)
    maxCount = max (lineCounts)
    result = [None] * len (chunks)
    for i in xrange (len (chunks)):
        diffCount = maxCount - lineCounts [i]
        chunk = chunks [i]
        if diffCount > 0:
            chunk = chunk + ([imaginaryLine] * diffCount)
        result [i] = string.join (chunk, '')
    return tuple (result)

def makeTuple (list, tupleSize):
    result = []
    for i in xrange (0, len (list), tupleSize):
        result.append (tuple (list [i:i + tupleSize]))
    return result


class Diff3Chunk:
    def __init__ (self, number, baseInfo, mineInfo, otherInfo):
        self.number = number
        self.baseInfo = baseInfo
        self.mineInfo = mineInfo
        self.otherInfo = otherInfo

    def dump (self):
        print '====' + `self.number`
        print '    base\t', self.baseInfo
        print '    mine\t', self.mineInfo
        print '    other\t', self.otherInfo

    def getRangeBegins (self):
        return (
            self.baseInfo.range [0],
            self.mineInfo.range [0],
            self.yourInfo.range [0])

    def getRangeEnds (self):
        return (
            self.baseInfo.range [1],
            self.mineInfo.range [1],
            self.yourInfo.range [1])


class Diff3Info:
    def __init__ (self, command, fromVal, toVal, lines):
        self.command = command
        if toVal is None:
            fromVal = int (fromVal)
            toVal = fromVal
        else:
            fromVal = int (fromVal) - 1
            toVal = int (toVal[1:])
        if (command == 'c') and (fromVal == toVal):
            fromVal = fromVal - 1
        self.range = (fromVal, toVal)

    def __repr__ (self):
        return '<%s %s>' % (self.command, self.range)

def fillLineNumbers (stream, fromLine, toLine, prefix = ' '):
    for i in xrange (fromLine + 1, toLine + 1):
        stream.write ('%6d\n' % i)


