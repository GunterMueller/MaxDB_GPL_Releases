#/usr/bin/env python
# historian.py
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


import os
import re
from UserList import UserList

class Historian:
    def __init__ (self, options):
        if options.url [:4] == 'ftp:':
            self.location = FTPLocation (options.url)
        else:
            self.location = FileLocation (options.url)
        self.getFileInformation ()

    def getFileInformation (self):
        for name, size in self.location.list ():
            self.addFile (name, size)

class SourceHistorian (Historian):
    fileRex = re.compile (r'(V\d\d_\d\d_(?:(?:develop)|(?:\d\d)))_(.*)')
    snapshotRex = re.compile (r'\d+$')
    changeRex = re.compile (r'(\d+)to(\d+)$')
    branchRex = re.compile (r'(\d+)branchto_(.*)$')
    def __init__ (self, options):
        self.branches = {}
        Historian.__init__ (self, options)

    def getFileInformation (self):
        Historian.getFileInformation (self)
        self.sortBranches ()

    def addFile (self, name, size):
        base = os.path.splitext (name) [0]
        match = self.fileRex.match (base)
        if match:
            branchName, info = match.group (1, 2)
            for rex, action in [
                    (self.snapshotRex, self.addSnapshotFile),
                    (self.changeRex, self.addChangeFile),
                    (self.branchRex, self.addBranchFile),
                    ]:
                infoMatch = rex.match (info)
                if infoMatch:
                    action (branchName, name, size, infoMatch)
                    break

    def addSnapshotFile (self, branchName, fileName, size, match):
        id = int (match.group (0))
        self.getBranch (branchName).append ((id, 'snapshot', fileName, size, None))

    def addChangeFile (self, branchName, fileName, size, match):
        lastId, currentId = map (int, match.group (1, 2))
        self.getBranch (branchName).append ((currentId, 'change', fileName, size, lastId))

    def addBranchFile (self, branchName, fileName, size, match):
        id, targetBranch = match.group (1, 2)
        id = int (id)
        self.getBranch (branchName).append ((id, 'label', fileName, size, targetBranch))
        newBranch = self.getBranch (targetBranch)
        newBranch.origin = branchName
        newBranch.splitSecond = id

    def getBranch (self, branchName):
        try:
            result = self.branches [branchName]
        except KeyError:
            result = Branch (branchName)
            self.branches [branchName] = result
        return result

    def sortBranches (self):
        for branch in self.branches.values ():
            branch.sort ()

    def dumpBranch (self, branchName):
        branch = self.branches [branchName]
        for row in branch:
            print row

    def getHistory (self, branchName, startChange = 0):
        result = []
        branch = self.branches [branchName]
        lowest = branch.getHighestChange ()
        while lowest > startChange:
            entries, lowest = branch.getRange (startChange, lowest)
            result = entries + result
            if lowest is None:
                break
            if lowest > startChange:
                try:
                    branch = self.branches [branch.origin]
                except KeyError:
                    break
        return result

    def getMaxInfoRestore (self, branchName):
        result = []
        hasSnapshot = None
        history = self.getHistory (branchName)
        for tuple in history:
            # (id, branchName, action, filename, size, info)
            action = tuple [2]
            if action == 'snapshot':
                if hasSnapshot:
                    # ignore
                    pass
                else:
                    result.append (tuple)
                    hasSnapshot = 1
            else:
                result.append (tuple)
        return result

    def getMinSizeRestore (self, branchName):
        result = []
        history = self.getHistory (branchName)
        history.reverse ()
        for tuple in history:
            # (id, branchName, action, filename, size, info)
            action = tuple [2]
            if action == 'snapshot':
                result.append (tuple)
                break
            else:
                result.append (tuple)
        result.reverse ()
        return result

    def getMaxInfoUpdate (self, branchName, startChange):
        result = []
        history = self.getHistory (branchName)
        for tuple in history:
            # (id, branchName, action, filename, size, info)
            action = tuple [2]
            if action != 'snapshot':
                result.append (tuple)
        return result

    def getMinSizeUpdate (self, branchName, startChange):
        history = self.getHistory (branchName)
        history.reverse ()
        alternatives = []
        currentHistory = []
        currentSize = 0
        for tuple in history:
            (id, branchName, action, filename, size, info) = tuple
            if action == 'snapshot':
                altSize = currentSize + size
                altHist = currentHistory + [tuple]
                alternatives.append ((altSize, altHist))
            else:
                currentHistory.append (tuple)
                currentSize = currentSize + size
        alternatives.append ((currentSize, currentHistory))
        alternatives.sort ()
        minSize, result = alternatives [0]
        result.reverse ()
        return result

class FileLocation:
    def __init__ (self, dirname):
        self.dirname = dirname

    def list (self):
        result = []
        list = os.listdir (self.dirname)
        pathjoin = os.path.join
        getsize = os.path.getsize
        for entry in list:
            fullname = pathjoin (self.dirname, entry)
            size = getsize (fullname)
            result.append ((entry, size))
        return result

    def getText (self, fname):
        return open (os.path.join (self.dirname, fname), 'rt').read ()

class FTPLocation:
    def __init__ (self, url):
        pass

class Branch (UserList):
    """describes actions on a specific branch
    """
    # the branch which originated this branch
    origin = None
    # the change list of the split
    splitSecond = None
    def __init__ (self, branchName):
        UserList.__init__ (self)
        self.branchName = branchName

    def getRange (self, lowerBound = 0, upperBound = None):
        """returns a list of actions limited by  starting changelist
        """
        if upperBound is None:
            upperBound = self.getHighestChange ()
        result = []
        for id, action, filename, size, info in self:
            if lowerBound < id <= upperBound:
                result.append ((id, self.branchName, action, filename, size, info))
        if len (result) > 0:
            lowest = result [0] [0]
            if lowest > lowerBound:
                lowest = self.splitSecond
        else:
            lowest = self.splitSecond
        return result, lowest

    def getHighestChange (self):
        if len (self) > 0:
            result = self [-1] [0]
        else:
            result = self.splitSecond
        return result

def checkOptions (options):
    assert options.url is not None, 'option -url must be specified'

def dumpHistory (history):
    for row in history:
        print row

def main (options, args):
    checkOptions (options)
    historian = SourceHistorian (options)
    # historian.dumpBranch ('V73_00_develop')
    # dumpHistory (historian.getHistory ('V73_00_18'))
    #dumpHistory (historian.getMaxInfoRestore ('V73_00_18'))
    #dumpHistory (historian.getMinSizeRestore ('V73_00_18'))
    #dumpHistory (historian.getMaxInfoUpdate ('V73_00_develop', 1000))
    #dumpHistory (historian.getMinSizeUpdate ('V73_00_develop', 1000))

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('url', None, ':', None, 'location of update files [local or ftp:]'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

