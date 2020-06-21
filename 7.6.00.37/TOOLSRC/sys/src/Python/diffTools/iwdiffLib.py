
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

import fileutil
from vmake import yutil
import wdiffLib

class IWDiff (wdiffLib.WDiff):
    def __init__ (self, options, source):
        wdiffLib.WDiff.__init__ (self, options)
        own, upper = yutil.getDiffPair (source)
        if own is None:
            raise 'NotLocal', source
        self.addDiff (own, upper)
        self.labels = upper, own

class IWDiff3 (wdiffLib.WDiff3):
    availableRenamings = [
       (7, 8, 'rename7to8.dta'),
    ]
    def __init__ (self, options, relPath, target):
        self.getReleases (relPath)
        mine, self.relativePath = self.getMine (target)
        base = self.getBase (mine)
        other = self.getOther ()
        wdiffLib.WDiff3.__init__ (self, options, base, mine, other)

    def getReleases (self, relPath):
        mineRelease, mineDat = getRelInfo (os.environ ['OWN'])
        otherRelease, otherDat = getRelInfo (relPath)
        self.labels = ['Base', mineDat, otherDat]
        isUpMerge = mineRelease > otherRelease
        mineVPath = getVmakePath ()
        otherVPath = self.otherVPath = getVmakePath (relPath)
        if isUpMerge:
            self.baseVPath = mineVPath
        else:
            self.baseVPath = otherVPath
        self.createRenaming (mineRelease, otherRelease)

    def createRenaming (self, mineRelease, otherRelease):
        mineMajor = mineRelease [0]
        otherMajor = otherRelease [0]
        renamer = None
        for lowMajor, highMajor, dataFile in self.availableRenamings:
            if (lowMajor == otherMajor) and (highMajor == mineMajor):
                renamer = self.createRenamer (dataFile, None)
            elif (lowMajor == mineMajor) and (highMajor == otherMajor):
                renamer = self.createRenamer (dataFile, 'true')
        self.renamer = renamer

    def createRenamer (self, dataFile, reverse):
        from vmake import renameLib
        fullName = renameLib.findRenameMap (dataFile)
        result = renameLib.ProjectRenamer (fullName, reverse = reverse)
        return result

    def getMine (self, target):
        relative = yutil.relativePath (target)
        mine = yutil.findRelativeFile (relative)
        if mine is None:
            mine = self.createEmptyFile (relative, self.labels [1])
        return mine, relative

    def getBase (self, mine):
        list = string.split (self.relativePath, os.sep)
        assert list [0] == 'sys'
        changedList = ['branchorigin'] + list [1:]
        relPath = string.join (changedList, os.sep)
        base = yutil.findRelativeFile (relPath, vpath = self.baseVPath)
        if base is None:
            base = self.createEmptyFile (self.relativePath,
                    self.labels [0], mine)
        return base

    def getOther (self):
        otherFile = yutil.findRelativeFile (self.relativePath, vpath = self.otherVPath)
        if otherFile is None:
            return self.createEmptyFile (self.relativePath, self.labels [2])
        if self.renamer:
            base = fileutil.basename (otherFile)
            renamedFile = os.path.join (os.environ ['TMP'], 'renamed.' + base)
            self.renamer.filterFile (otherFile, renamedFile)
            otherFile = renamedFile
        return otherFile

    def createEmptyFile (self, fname, kind, fillFile = None):
        baseName = fileutil.basename (fname)
        fname = os.path.join (os.environ ['TMP'], 'empty.'
            + kind + '.' + baseName)
        stream = open (fname, 'wt')
        if fillFile:
            data = open (fillFile, 'rt').read ()
        else:
            data = '%s: %s does not exist\n' % (kind, baseName)
        stream.write (data)
        stream.close ()
        return fname

    def getMineFile (self):
        fullName = os.path.join (os.environ ['OWN'], self.relativePath)
        if os.path.exists (fullName):
            return fullName
        yutil.inew (fullName)
        return fullName

def getRelInfo (dir):
    data = open (os.path.join (dir, 'Release.dat'), 'rt').read ()
    parts = string.split (data, '.')
    list = map (int, parts)
    return tuple (list), data

def getVmakePath (dir = None):
    if dir is None:
        pathvar = os.environ ['VMAKE_PATH']
    else:
        pathvar = parseIprofile (dir)
    result = string.split (pathvar, ',')
    return result

def parseIprofile (dir):
    lines = open (os.path.join (dir, 'iprofile.bat'), 'rt').readlines ()
    env = {'OWN': dir}
    setRE = re.compile (' *set +([^=]+)=(.*)', re.IGNORECASE)
    varRE = re.compile ('%[^%]+%')
    def expandvar (match, env = env):
        varname = match.group (0) [1:-1]
        if env.has_key (varname):
            return env [varname]
        else:
            return ''
    for line in lines:
        match = setRE.match (line)
        if match:
            varname, value = match.group (1, 2)
            value = string.strip (value)
            value = varRE.sub (expandvar, value)
            if varname == 'VMAKE_PATH':
                return value
            else:
                env [varname] = value
    return ''


