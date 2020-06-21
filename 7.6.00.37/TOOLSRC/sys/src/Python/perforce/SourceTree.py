
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

import string
import re
import time
from UserList import UserList

import perforceLib

class PerforceObject:
    def __init__ (self, perforceClient = None):
        if perforceClient is None:
            perforceClient = perforceLib.PerforceSession ()
        self.perforceClient = perforceClient

    def getLines (self, *args):
        return self.perforceClient.getLines (string.join (args))


class SourceTree (PerforceObject):
    """Handles operations relative to a release tree
    """
    def __init__ (self, perforcePath, anchorFile, perforceClient = None):
        PerforceObject.__init__ (self, perforceClient)
        self.perforcePath = perforcePath
        self.localPath = None
        self._allFiles = None
        self._typeMapping = None
        self.anchorFile = anchorFile

    def getLocalPath (self):
        """Returns the local path of the source tree
        """
        if not self.localPath:
            localFile = self.perforceClient.fstat (
                self.perforcePath + '/' + self.anchorFile).clientFile
            self.localPath = localFile [:- len (self.anchorFile) - 1]
        return self.localPath

    def allFiles (self, suppressedDirs = [], forceRead = None,
            revision = None, skipDeleted = None):
        """returns a list of (filename, filetype) tuples

        filenames are relative to perforcePath.
        """
        if not forceRead and self._allFiles is not None:
            return self._allFiles
        result = self._allFiles = []
        self._typeMapping = {}
        cmd = 'files %s/...' % self.perforcePath
        if revision:
            cmd = cmd + revision
        rex = re.compile (r'([^#]+)#\d+\s*-\s*(\w+)\s+change\s+\d+\s+[(](\w+([+]\w+)?)[)]')
        rootLen = len (self.perforcePath) + 1
        for line in self.perforceClient.getResultIterator (cmd):
            match = rex.match (line)
            if not match:
                print line
            assert match
            if match:
                filename, action, kind = match.group (1, 2, 3)
                if  skipDeleted and (action == 'delete'):
                    continue
                filename = filename [rootLen:]
                isSuppressed = None
                for dirname in suppressedDirs:
                    if filename [:len (dirname)] == dirname:
                        isSuppressed = 1
                        break
                if not isSuppressed:
                    kindTuple = perforceLib.parseFiletype (kind)
                    result.append ((filename, kindTuple))
                    self._typeMapping [filename] = kindTuple
        return result

    def getFileType (self, filename):
        if self._typeMapping is None:
            self.allFiles ()
        try:
            result = self._typeMapping [filename]
        except KeyError:
            fstat = self.perforceClient.fstat (self.perforcePath + '/' + filename)
            if fstat is None:
                raise KeyError, filename
            kindTuple = perforceLib.parseFiletype (fstat.headType)
            self._allFiles.append ((filename, kindTuple))
            self._typeMapping [filename] = kindTuple
            result = kindTuple
        return result

    def getChanges (self, lowerBound = 0):
        return ChangeHistory (self.perforceClient, self.perforcePath, lowerBound)

    def getBranches (self, anchorFile = None, lowerBound = 0):
        if anchorFile is None:
            anchorFile = self.anchorFile
        lines = self.getLines ('filelog ' + self.perforcePath + '/' + anchorFile)
        result = []
        rex = re.compile ('[.]{3} [.]{3} branch into (.*)#[0-9]+')
        for line in lines:
            match = rex.match (line)
            if match:
                newBranch = match.group (1)
                perforceRoot = newBranch [: -len (anchorFile) - 1]
                fstat = self.perforceClient.fstat (
                    perforceRoot + '/' + anchorFile + '#1')
                if fstat is None:
                    continue
                startChange = fstat.headChange
                creationTime = time.gmtime (int (fstat.headTime))
                changeid = int (startChange)
                if changeid > lowerBound:
                    result.append ((perforceRoot, int (startChange),
                        creationTime [:3]))
        return self.sortedBranches (result)

    def sortedBranches (self, branchInfos):
        sorter = map (lambda t: (t [1], t), branchInfos)
        sorter.sort ()
        return map (lambda t: t [1], sorter)

    def lastChange (self):
        searchExpr = self.perforcePath + '/...'
        lines = self.getLines ('changes -m 1 ' + searchExpr)
        match = ChangeHistory._changeRex.match (lines [0])
        return int (match.group (1))

    def sync (self, revision = None):
        searchExpr = self.perforcePath + '/...'
        if revision:
            searchExpr = searchExpr + revision
        self.getLines ('sync ' + searchExpr)

class ChangeList (PerforceObject):
    isBranch = None
    def __init__ (self, perforceClient, changelistId):
        PerforceObject.__init__ (self, perforceClient)
        self.id = changelistId
        self.descriptionAvailable = None

    def __str__ (self):
        return 'ChangeList (' +  `self.id` + ')'

    __repr__ = __str__

    def setDate (self, date):
        self.date = date

    baseRex = re.compile (r'Change \d+ by [^ ]+ on (\d\d\d\d)/(\d\d)/(\d\d)')
    actionRex = re.compile ('.*(//.*)#[0-9]+ ([a-z]+)')
    def describe (self):
        self.descriptionText = self.getLines ('describe -s ' + `self.id`)
        lines = self.descriptionText [:]
        baseMatch = self.baseRex.match (lines [0])
        assert baseMatch
        self.date = baseMatch.group (1, 2, 3)
        del lines [:2]
        commentEnd = 0
        while lines [commentEnd] != 'Affected files ...\n':
            commentEnd = commentEnd+ 1
        self.comment = string.join (lines [:commentEnd])
        self.files = files = []
        for line in lines [commentEnd + 2:]:
            line = string.strip (line)
            if not line:
                continue
            match = self.actionRex.match (line)
            if not match:
                # this file doesn't belong to this branch
                continue
            assert match
            fname, action = match.group (1, 2)
            files.append ((fname, action))
        self.descriptionAvailable = 1
        return self.files

    def getRelativeFiles (self, root, addAction = None):
        """returns list of changed files relative to root

            files not under root are filtered
        """
        result = []
        if root [-1] != '/':
            root = root + '/'
        rootLen = len (root)
        for filename, action in self.files:
            if filename [:rootLen] == root:
                filename = filename [rootLen:]
                if addAction:
                    entry = (filename, action)
                else:
                    entry = filename
                result.append (entry)
        return result

class BranchChangeList (ChangeList):
    isBranch = 1
    def __init__ (self, perforceClient, changelistId, branchTo):
        ChangeList.__init__ (self, perforceClient, changelistId)
        self.branchTo = branchTo

    def __str__ (self):
        return 'Branch (' +  `self.id` + ') -> ' + self.branchTo

    __repr__ = __str__

class ChangeHistory (PerforceObject, UserList):
    def __init__ (self, perforceClient, rootPath, lowerBound = 0):
        PerforceObject.__init__ (self, perforceClient)
        UserList.__init__ (self)
        self.rootPath = rootPath
        self.getChanges (rootPath, lowerBound)

    _changeRex = re.compile (r'Change\s+(\d+)\s+on\s+(\d+)/(\d+)/(\d+)')
    def getChanges (self, rootPath, lowerBound = 0):
        lines = self.getLines ('changes ' + rootPath + '/...')
        for line in lines:
            match = self._changeRex.match (line)
            assert match
            id, year, month, day = map (int, match.group (1, 2, 3,4))
            if id <= lowerBound:
                break
            changeList = ChangeList (self.perforceClient, id)
            changeList.setDate ((year, month, day))
            self.append (changeList)
        self.reverse ()
        return self

    def mergeBranchChanges (self, branchInfos):
        if len (branchInfos) == 0:
            return
        newList = []
        infoIndex = 0
        branchRoot, branchChangeList, branchDate = branchInfos [infoIndex]
        for changeList in self:
            if branchChangeList and (branchChangeList < changeList.id):
                newChange = BranchChangeList (self.perforceClient,
                    branchChangeList, branchRoot)
                newChange.date = branchDate
                newList.append (newChange)
                try:
                    infoIndex = infoIndex + 1
                    branchRoot, branchChangeList, branchDate = branchInfos [infoIndex]
                except IndexError:
                    branchChangeList = None
            newList.append (changeList)
        self[:] = newList

    def getAttributedList (self, callback):
        result = []
        for changeList in self:
            callResult = callback (changeList)
            result.append ((callResult, changeList))
        return result

def testFileStatistics (sourceTree):
    fileCount = 0
    typeStats = {}
    for name, (kind, options) in sourceTree.allFiles (
            ['sys/src/sap/', 'sys/src/sap45A/', 'sys/src/sap50A/']):
        fileCount = fileCount + 1
        typeStats [kind] = typeStats.get (kind, 0) + 1
    items = typeStats.items ()
    items.sort ()
    return fileCount, items

def testFileTypes (sourceTree):
    for filename in ['sys/src/sp/gsp00',
            'sys/src/sap50A/dptrace.h',
            'sys/src/noexists']:
        try:
            result = sourceTree.getFileType (filename)
        except KeyError, exc:
            result = (KeyError, exc)
        print 'getFileType (%s) ->' % filename, result

def testGetChanges (sourceTree):
    changes = sourceTree.getChanges ()
    callback = Attributer ().callback
    for attribute, changeList in changes.getAttributedList (callback):
        print repr (attribute), changeList

class Attributer:
    def __init__ (self):
        self.lastDate = None

    def callback (self, changeList):
        result = []
        if self.lastDate and (self.lastDate != changeList.date):
            result.append ('day change')
        self.lastDate = changeList.date
        if changeList.isBranch:
            result.append ('branch')
        return result

def testBranchChanges (sourceTree):
    branches = sourceTree.getBranches ()
    for row in branches:
        print row
    changes = sourceTree.getChanges ()
    changes.mergeBranchChanges (branches)
    callback = Attributer ().callback
    for attribute, changeList in changes.getAttributedList (callback):
        print repr (attribute), changeList

if __name__ == "__main__":
    sourceTree = SourceTree ('//sapdb/V73/c_00/develop', 'sys/src/sp/gsp00')
    #print 'localPath () -> ', sourceTree.getLocalPath ()
    #print 'testFileStatistics () ->', testFileStatistics (sourceTree)
    #testFileTypes (sourceTree)
    #testGetChanges (sourceTree)
    #testBranchChanges (sourceTree)
    print 'lastChange () ->', sourceTree.lastChange ()

