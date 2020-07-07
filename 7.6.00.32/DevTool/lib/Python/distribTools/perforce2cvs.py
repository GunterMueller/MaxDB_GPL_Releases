import sys
import os
import string
import re
import tempfile

import perforce.SourceTree
import perforce.perforceLib
import distribTools.releaseInfo
import distribTools.common
from distribTools.CVS import CVS

true, false = 'true', None

temporaryFiles = ['sys/changes/commit.txt', 'cvs.log']

if not os.environ.has_key ('HOME') and os.environ.has_key ('USERPROFILE'):
    os.environ ['HOME'] = os.environ ['USERPROFILE']

def fatalError (tool, msg):
    sys.stderr.write (tool + ': ' + msg + '\n')
    sys.exit (2)

def ptype2cvstype (kind, options):
    if kind == 'binary':
        result = 'b'
    else:
        result = 'o'
    return result

_buildRE = re.compile ('//sapdb/V([0-9]+)/c_([0-9]+)/b_([0-9]+)')

def pbranch2cvsbranch (pbranch):
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

class FullCVS:
    def __init__ (self, p4, depot, verbose = None, noAction = None,
            subdirs = None, noOpenSource = None):
        self.p4 = p4
        self.depot = depot
        self.verbose = verbose
        self.noAction = noAction
        self.subdirs = subdirs
        self.cvs = CVS (verbose, noAction)
        if noOpenSource:
            self.excludeFiles = distribTools.common.loadExcludesFromVar (p4, noOpenSource)
        else:
            self.excludeFiles = distribTools.common.loadExcludesFromPerforce (
                p4, depot + '/sys/config/noOpenSource')

    def syncTo (self, changelist, cont = None):
        revision = '@' + `changelist`
        fileList = self.getFilelist (revision)
        #
        # copy files to cvs tree
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
            self.cvs.checkout (fname, ptype2cvstype (kind, options))
            if not self.noAction:
                self.p4.copyFile (self.depot + '/' + fname + revision, fname)
        self.cvs.commit ('synced to perforce changelist %d' % changelist)

    def resyncTo (self, changelist, quick = None):
        revision = '@' + `changelist`
        fileList = self.getFilelist (revision)
        self.deleteSuperfluousLocalFiles (fileList)
        #
        # copy files to cvs tree
        #
        listlen = float (len (fileList))
        counter = 0.0
        for fname, (kind, options) in fileList:
            if self.verbose:
                counter = counter + 1.0
                percent = int (counter * 100.0 / listlen)
                print "%02d %% %s" % (percent, fname)
            if os.path.exists (fname):
                if not quick:
                    # copy to local file
                    tmpfile = 'tmpfile.cache'
                    self.p4.copyFile (self.depot + '/' + fname + revision, tmpfile)
                    # compare checksum
                    p4cksum, cvscksum = getChecksums ([tmpfile, fname])
                    if p4cksum [:2] != cvscksum [:2]:
                        print repr (p4cksum)
                        print repr (cvscksum)
                        self.cvs.checkout (fname, ptype2cvstype (kind, options))
                        if not self.noAction:
                            self.p4.copyFile (self.depot + '/' + fname + revision, fname)
            else:
                self.cvs.checkout (fname, ptype2cvstype (kind, options))
                if not self.noAction:
                    self.p4.copyFile (self.depot + '/' + fname + revision, fname)
        self.cvs.commit ('resynced to perforce changelist %d' % changelist)
    
    def getFilelist (self, revision):
        filelistCache = 'filelist.cache'
        temporaryFiles.append (filelistCache)
        if distribTools.common.testMode and os.path.exists (filelistCache):
            fileList = eval (open (filelistCache, 'r').read ())
        else:
            sourceTree = perforce.SourceTree.SourceTree (self.depot,
                'sys/src/sp/vsp001', perforceClient = self.p4)
            fileList = sourceTree.allFiles (skipDeleted = true,
                revision = revision)
            if distribTools.common.testMode:
                open (filelistCache, 'w').write (repr (fileList))
        fileList = filter (self.isValidFileEntry, fileList)
        return fileList

    def isValidFileEntry (self, fileDescription):
        filename, kind = fileDescription
        filename = '/' + filename
        for decider in self.excludeFiles:
            result = decider.decide (filename)
            if result > 0:
                return 1
            elif result < 0:
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

    def deleteSuperfluousLocalFiles (self, p4files):
        filelistCache = 'localfiles.cache'
        temporaryFiles.append (filelistCache)
        if not distribTools.common.testMode or not  os.path.exists (filelistCache):
            os.system ('find . -type f | grep -v /CVS/ > ' + filelistCache)
        data = open (filelistCache, 'r').read ()
        fileList = string.split (data, '\n')
        if  not fileList [-1]:
            del fileList [-1]
        p4dict = {}
        for fname, attr in p4files:
            p4dict [fname] = 1
        for fname in fileList:
            fname = fname [2:]
            if not p4dict.has_key (fname) and not (fname in temporaryFiles):
                self.cvs.delete (fname)
    
class UpdCVS:
    def __init__ (self, cvsdir, p4, verbose = None, noAction = None):
        self.cvsdir = os.path.abspath (cvsdir)
        self.p4 = p4
        self.verbose = verbose
        self.noAction = noAction
        self.port, self.depot, self.lastChange, self.noOpenSource = loadPerforceLink (cvsdir)
        self.p4.setPort (self.port)
        assert (self.depot)
        self.sourceTree = perforce.SourceTree.SourceTree (self.depot,
            'sys/src/sp/vsp001', perforceClient = p4)

    def update (self, upto = None):
        cvs = CVS (self.verbose, self.noAction)
        os.chdir (self.cvsdir)
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
                #cvsTag = pbranch2cvsbranch (change.branchTo)
                #cvs.tagForBranch (cvsTag)
                lastChange = change.id
            else:
                changedFiles = self.applyChange (cvs, change)
                if changedFiles:
                    cvs.commit (self.getChangeComment (change), changedFiles)
                    if not self.noAction:
                        writePerforceLink (self.cvsdir, self.depot, change.id,
                            self.noOpenSource, self.port)
                        lastChange = change.id
        return lastChange

    def applyChange (self, cvs, change):
        change.describe ()
        revision = '@' + `change.id`
        changedFiles = []
        validFiles = filter (self.isValidFile, change.files)
        for file, action in self.deleteToFront (validFiles):
            relative = file [len (self.depot) + 1:]
            changedFiles.append (relative)
            if action == 'delete':
                cvs.delete (relative)
            else:
                cvs.edit (relative, self.getCvsFileType (relative))
                if not self.noAction:
                    self.p4.copyFile (self.depot + '/' + relative + revision, relative)
        if not changedFiles:
            return changedFiles
        #
        # describe change
        #
        changeDescriptionFile = 'sys/changes/commit.txt'
        cvs.edit (changeDescriptionFile, 'o')
        firstLine = string.split (change.descriptionText [0])
        firstLine = string.join (firstLine [:3] + ['sapdb@sap.com'] + firstLine [4:]) + '\n'
        open (changeDescriptionFile, 'w').writelines ([firstLine] + change.descriptionText [1:])
        changedFiles.append (changeDescriptionFile)
        #
        # return
        #
        return changedFiles

    def isValidFile (self, fileinfo):
        file, action = fileinfo
        if file [:len (self.depot)] != self.depot:
            return None
        for decider in self.excludeFiles:
            result = decider.decide (file)
            if result > 0:
                return 1
            elif result < 0:
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

    def getCvsFileType (self, relative):
        kind, options = self.sourceTree.getFileType (relative)
        return ptype2cvstype (kind, options)

def loadPerforceLink (cvsDir):
    dict = {'depot': None, 'changelist': -1, 'port': None}
    linkFile = os.path.join (cvsDir, 'CVS', 'perforceLink')
    if os.path.exists (linkFile):
        execfile (linkFile, dict)
    depot = dict ['depot']
    try:
        noOpenSource = dict ['noOpenSource']
    except KeyError:
        noOpenSource = depot + '/sys/config/noOpenSource'
    return dict ['port'], depot, dict ['changelist'], noOpenSource

def writePerforceLink (cvsDir, depot, changelist, noOpenSource = None,
        port = None):
    if noOpenSource is None:
        noOpenSource = depot + '/sys/config/noOpenSource'
    stream = open (os.path.join (cvsDir, 'CVS', 'perforceLink'), 'w')
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

def perforceSession (clientName, port = None):
    return perforce.perforceLib.PerforceSession (clientName, port = port)

def getChecksums (files):
    for i in xrange (len (files)):
        files [i] = '"' + files [i] + '"'
    lines = os.popen ('cksum ' + string.join (files), 'r').readlines ()
    result = []
    for line in lines:
        line = line.strip ()
        if line:
            row = string.split (line)
            result.append (tuple (row))
    return result

