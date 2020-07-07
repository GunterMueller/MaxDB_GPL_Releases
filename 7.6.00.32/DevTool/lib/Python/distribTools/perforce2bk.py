
#
#    ========== licence begin LGPL
#    Copyright (C) 2003 SAP AG
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
import sys
import os
import string
import re
import tempfile

import perforce.SourceTree
import perforce.perforceLib
import distribTools.releaseInfo
import distribTools.common
from distribTools.BitKeeper import BitKeeper

true, false = 'true', None

#if not os.environ.has_key ('HOME') and os.environ.has_key ('USERPROFILE'):
#    os.environ ['HOME'] = os.environ ['USERPROFILE']

def fatalError (tool, msg):
    sys.stderr.write (tool + ': ' + msg + '\n')
    sys.exit (2)

def ptype2bktype (kind, options):
    if kind == 'binary':
        result = '-b'
    else:
        result = ''
    return result

_buildRE = re.compile ('//sapdb/V([0-9]+)/c_([0-9]+)/b_([0-9]+)')
    
def pbranch2bktag (pbranch):
    match = _buildRE.match (pbranch)
    if match:
        majorMinor, corr, build = match.group (1, 2, 3)
        major = majorMinor [:1]
        minor = majorMinor [1:]
        result = '%s_%s_%s_%s' % (major, minor, corr, build)
    else:
        path = string.split (pbranch, '/')
        path = filter (None, path)  # remove empty parts
        path = path [1:]            # remove depot name
        result = string.join (path, '-')
    return 'Release-' + result

class FullBitKeeper:
    def __init__ (self, p4, depot, verbose = None, noAction = None,
            subdirs = None):
        self.p4 = p4
        self.depot = depot
        self.verbose = verbose
        self.noAction = noAction
        self.subdirs = subdirs
        self.bk = BitKeeper (verbose, noAction)
        self.excludeFiles = distribTools.common.loadExcludesFromPerforce (
            p4, depot + '/sys/config/noOpenSource')

    def syncTo (self, changelist, cont = None):
        revision = '@' + `changelist`
        fileList = self.getFilelist (revision)
        #
        # copy files to BitKeeper tree
        #
        listlen = float (len (fileList))
        counter = 0.0
        for fname, (kind, options) in fileList:
            if self.verbose:
                counter = counter + 1.0
                percent = int (counter * 100.0 / listlen)
                print "%02d %% %s" % (percent, fname)
            if cont and os.path.exists (fname):
                continue
            newFile = not self.bk.exists (fname)
            if not newFile:
                self.bk.checkout (fname)
            if not self.noAction:
                self.p4.copyFile (self.depot + '/' + fname + revision, fname)
            self.bk.checkin (fname, newFile, ptype2bktype (kind, options))
        self.bk.commit ('synced to perforce changelist %d' % changelist)
        self.bk.push ()

    def getFilelist (self, revision):
        sourceTree = perforce.SourceTree.SourceTree (self.depot,
            'sys/src/sp/vsp001', perforceClient = self.p4)
        fileList = sourceTree.allFiles (skipDeleted = true,
            revision = revision)
        fileList = filter (self.isValidFileEntry, fileList)
        return fileList
    
    def isValidFileEntry (self, fileDescription):        
        filename, kind = fileDescription
        filename = '/' + filename
        for _excl in self.excludeFiles:
            if string.find (filename, _excl) != -1:
                return None
        if not self.subdirs:            
            return 1
        #
        # restrict files to those in subdirs
        #
        for subdir in self.subdirs:
            if string.find (filename, subdir) != -1:
                return 1
        return None

class UpdBitKeeper:
    def __init__ (self, bkdir, p4, verbose = None, noAction = None):
        self.bkdir = os.path.abspath (bkdir)
        self.p4 = p4
        self.verbose = verbose
        self.noAction = noAction
        self.port, self.depot, self.lastChange, self.noOpenSource = loadPerforceLink (bkdir)
        self.p4.setPort (self.port)
        assert (self.depot)
        self.sourceTree = perforce.SourceTree.SourceTree (self.depot,
            'sys/src/sp/vsp001', perforceClient = p4)

    def update (self, upto = None):
        bk = BitKeeper (self.verbose, self.noAction)
        os.chdir (self.bkdir)
        self.excludeFiles = distribTools.common.loadExcludesFromPerforce (
            self.p4, self.noOpenSource)

        #
        # get changes
        #
        branches = self.sourceTree.getBranches (lowerBound = self.lastChange)
        changes = self.sourceTree.getChanges (self.lastChange)
        changes.mergeBranchChanges (branches)

        #
        # apply changes
        #
        listlen = float (len (changes))
        counter = 0.0
        lastChange = None
        for change in changes:
            if upto and change.id > upto:
                break
            if self.verbose:
                counter = counter + 1.0
                percent = int (counter * 100.0 / listlen)
                print "%02d %% %s" % (percent, change)
            if change.isBranch:
                # bkTag = pbranch2bkbranch (change.branchTo)
                # bk.tagForBranch (bkTag)
                lastChange = change.id
            else:
                changedFiles = self.applyChange (bk, change)
                if changedFiles:
                    bk.commit (self.getChangeComment (change), changedFiles)
                    if not self.noAction:
                        writePerforceLink (self.bkdir, self.depot, change.id, 
                            self.noOpenSource, self.port)
                        lastChange = change.id
        bk.push ()
        return lastChange

    def applyChange (self, bk, change):
        change.describe ()
        revision = '@' + `change.id`
        changedFiles = []
        validFiles = filter (self.isValidFile, change.files)
        for file, action in self.deleteToFront (validFiles):
            relative = file [len (self.depot) + 1:]
            changedFiles.append (relative)
            if action == 'delete':
                bk.delete (relative)
            else:
                newFile = not bk.exists (relative)
                if not newFile:
                    bk.checkout (relative)
                if not self.noAction:
                    self.p4.copyFile (self.depot + '/' + relative + revision, relative)
                bk.checkin (relative, newFile, self.getBKFileType (relative))
        return changedFiles

    def isValidFile (self, fileinfo):
        file, action = fileinfo
        if file [:len (self.depot)] != self.depot:
            return None
        for _excl in self.excludeFiles:
            if string.find (file, _excl) != -1:
                return None
        return 1

    def deleteToFront (self, changes):
        deletes = []
        other = []
        for entry in changes:
            if entry [1] == 'delete':
                deletes.append (entry)
            else:
                other.append (entry)
        return deletes + other


    def getChangeComment (self, change):
        return string.strip (change.comment) + ('\n\nPerforce changelist %d\n' % change.id)

    def getBKFileType (self, relative):
        kind, options = self.sourceTree.getFileType (relative)
        return ptype2bktype (kind, options)

def loadPerforceLink (bkDir):
    dict = {'depot': None, 'changelist': -1, 'port': None}
    linkFile = os.path.join (bkDir, 'BitKeeper/etc', 'perforceLink')
    if os.path.exists (linkFile):
        execfile (linkFile, dict)
    depot = dict ['depot']
    try:
        noOpenSource = dict ['noOpenSource']
    except KeyError:
        noOpenSource = depot + '/sys/config/noOpenSource'
    return dict ['port'], depot, dict ['changelist'], noOpenSource

def writePerforceLink (bkDir, depot, changelist, noOpenSource = None,
        port = None):
    if noOpenSource is None:
        noOpenSource = depot + '/sys/config/noOpenSource'
    stream = open (os.path.join (bkDir, 'BitKeeper/etc', 'perforceLink'), 'w')
    if port:
        stream.write ('port = %s\n' % repr (port))
    stream.write ('depot = %s\n' % repr (depot))
    stream.write ('changelist = %d\n' % changelist)
    stream.write ('noOpenSource = %s\n' % repr (noOpenSource))    
    stream.close ()

def getStartingChangelist (p4, depot):
    lines = p4.getLines ('changes ' + depot + '/...')
    words = string.split (lines [-1])
    return int (words [1])

def getCurrentChangelist (p4, depot):
    lines = p4.getLines ('changes -m 1 ' + depot + '/...')
    words = string.split (lines [0])
    return int (words [1])

def perforceSession (clientName):
    return perforce.perforceLib.PerforceSession (clientName)

