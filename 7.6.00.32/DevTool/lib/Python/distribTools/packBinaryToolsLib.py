#!/usr/bin/env python
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

import sys
import string
import re
import os
import shutil
import ftplib
import time

import fileutil
from distribTools import common

pathjoin = os.path.join

def todo (msg):
    print 'TODO:', msg

_platformMap = {
    'win32': ('win32', 'i386'),
    'linux2': ('linux', 'i386'),
}

def getPlatform ():
    pyplatform = sys.platform
    return _platformMap.get (pyplatform, (pyplatform, 'unknown'))

def getOldMaxChange (refdir):
    filename = pathjoin (refdir, 'syncinfo.dat')
    data = open (filename, 'rb').read ()
    match = re.search ('<maxchangenr>(.*)</maxchangenr>', data)
    if not match:
        raise 'wrong format', filename
    try:
        result = int (string.strip (match.group (1)))
    except ValueError:
        raise 'wrong integer format', filename
    return result

def getCurrentMaxChange ():
    p4cmd = 'p4 changes -m 1 //sapdb/TOOLSRC/...'
    data = os.popen (p4cmd, 'r').read ()
    match = re.match ('Change ([0-9]+)', data)
    if not match:
        raise 'wrong format', data
    return int (match.group (1))

def getYesterdaysChangelist ():
    today = time.localtime (time.time ())
    todayString = '%04d/%02d/%02d' % (today [0], today [1], today [2])
    p4cmd = ('p4 -p perforce5001.wdf.sap.corp:5001 changes -m 1 '
        + '//prodprojects/vmake/dev/src/_tools/maketools/...@' + todayString)
    p4result = os.popen (p4cmd, 'r').read ()
    data = string.split (p4result)
    return int (data [1])

def reCompile (pattern):
    return re.compile (pattern, re.I)

def extensionExclude (extension):
    return re.compile ('[.]' + extension + '$', re.I)

def subdirExclude (subpath):
    subpath = string.split (subpath, '/')
    return re.compile (string.join ([''] + subpath + [''], _sepPattern), re.I)

if os.sep == '\\':
    _sepPattern = '\\' + os.sep
else:
    _sepPattern = os.sep

_ignorePatterns = (map (reCompile, [
    '[.][0-9]+$',
    'Masm386[.]exe$',
    '[/\\\\]Ml[.]exe$',
    'lint-nt[.]exe$',
    'python[0-9][0-9](_64)?[.]lib$',
    'tel[.]pl$',
    'windiff[.].*',
    'handleex[.].*',
    '[/\\\\]_.*[.]exe$',
    'vmak.*-',
    'vmak.*[0-9]+[.][0-9]+[.][0-9]+[.][A-Z]$',
    'core',
    'pgm.tools.tar.gz',
    'fhvmake',
    'fastcap.h',     # RaymondR: MS confidential
    'flex',
    'instdbmsrv',
    'instlserver',
    'alias.txt',
    'file2developer.txt',
    'sutmail',
    'tccmail',
    'imember',
    'duser.pm',
    ])
    + map (extensionExclude, ['f', 'q', 's', 'pyc', 'old', 'old2', 'old3',
        'save', 'prt', 'axel', 'new', 'bak', 'holger', 'org', 'pyd'])
    + map (subdirExclude, ['incl/perforce', 'lib/java/Sibermerge',
        'bin/bin', 'bin/demodb', 'lib/lint', # 'lib/Python/glue',
        'Posix/backup'])
    )

toolDirs = ['bin', 'data', 'pgm', 'incl', 'lib']
if sys.platform == 'win32':
    toolDirs = toolDirs + ['Posix']
else:
    toolDirs = toolDirs + ['gnu']

class TreeCopy:
    def __init__ (self, ignorePatterns, verbose = None, nocopy = None):
        self.ignorePatterns = ignorePatterns
        self.verbose = verbose
        self.nocopy = nocopy

    def copy (self, sourceDir, targetDir, rootPrefix = ''):
        sourceDir = self.normdir (sourceDir)
        targetDir = self.normdir (targetDir)
        rootPrefix = self.normdir (rootPrefix, 1)
        for fullname in fileutil.RecursiveFileIterator ([sourceDir]):
            self.copyFile (fullname, targetDir, rootPrefix)

    def copyFile (self, fullname, targetDir, rootPrefix):
        relative = fullname [len (rootPrefix):]
        if self.exclude (fullname, relative):
            if self.verbose:
                print 'XX', fullname
            return
        target = pathjoin (targetDir, relative)
        if self.verbose:
            print '=>', target
        if self.nocopy:
            return
        try:
            shutil.copy2 (fullname, target)
        except IOError:
            self.createDirs (target)
            try:
                shutil.copy2 (fullname, target)
            except IOError, exc:
                print 'ERR:', relative, str (exc)

    def exclude (self, fullname, relative):
        for rex in self.ignorePatterns:
            if rex.search (fullname):
                return 1
        return None

    def excludeFile (self, fullname, relative):
        return None

    def normdir (self, dirname, addSep = None):
        dirname = os.path.normpath (dirname)
        if addSep and (dirname [-1] != os.sep):
            dirname = dirname + os.sep
        return dirname

    def createDirs (self, fullname):
        dirname = os.path.dirname (fullname)
        if not os.path.isdir (dirname):
            os.makedirs (dirname)


class TreeDiffCopy (TreeCopy):
    def __init__ (self, refdir, ignorePatterns, verbose = None, nocopy = None):
        TreeCopy.__init__ (self, ignorePatterns, verbose, nocopy)
        self.refdir = refdir

    def exclude (self, fullname, relative):
        if TreeCopy.exclude (self, fullname, relative):
            return 1
        originalFile = pathjoin (self.refdir, relative)
        if not os.path.exists (originalFile):
            return None
        try:
            orgData = open (originalFile, 'rb').read ()
            newData = open (fullname, 'rb').read ()
        except IOError:
            return 1
        if orgData == newData:
            return 1
        else:
            return None

def copyDummyScripts (outdir):
    if os.name != 'posix':
        return
    binDir = pathjoin (outdir, 'bin')
    for script in ['instlserver', 'instdbmsrv']:
        fullname = os.path.join (binDir, script)
        stream = open (fullname, 'w')
        stream.write ("""#!/bin/sh
# dummy script, no longer necessary
""")
        mode = os.stat (fullname) [0]
        mode = mode + 73
        os.chmod (fullname, mode)
        stream.close ()

def copyInstallScripts (outdir):
    dataDir = pathjoin (os.environ ['TOOL'], 'data', 'distribTools')
    shutil.copy2 (pathjoin (dataDir, 'installtools.pl'), outdir)
    shutil.copy2 (pathjoin (dataDir, 'iprofile'), outdir)

if sys.platform == 'win32':
    _zipCmd = 'zip -r %(outfile)s %(dirname)s'
    _zipExt = '.zip'
else:
    _zipCmd = 'tar cf - %(dirname)s | gzip -c -9 > %(outfile)s'
    _zipExt = '.tgz'

def packDir (dirname, outfile = 'DevTool', verbose = None):
    result = pathjoin (dirname, outfile)
    os.chdir (dirname)
    dirname = 'DevTool'
    cmd = _zipCmd % locals ()
    if verbose:
        os.system (cmd)
    else:
        os.popen (cmd, 'r').read ()
    return result

def archiveName (changelist = 0):
    if changelist == 0:
        changelist = getYesterdaysChangelist ()
    return 'maxdb-buildtools-%s-%d%s' % (common.detectPlatform (None), changelist, _zipExt)

def packBinaryTools (options):
    outdir = pathjoin (options.outdir, 'DevTool')   # add main subdir
    copyClerk = TreeCopy (_ignorePatterns, options.verbose, options.nocopy)
    tool = os.environ ['TOOL']
    for dirname in toolDirs:
        source = pathjoin (tool, dirname)
        target = outdir
        copyClerk.copy (source, target, tool)
    if not options.nocopy:
        copyDummyScripts (outdir)
        copyInstallScripts (outdir)
        outfile = archiveName (options.changelist)
        packDir (options.outdir, outfile, options.verbose)
        if options.ftp:
            common.ftpTransfer (options.ftp, outfile)
            os.remove (outfile)
        shutil.rmtree (outdir)

def packToolsUpdate (options):
    outdir = pathjoin (options.outdir, 'DevTool')   # add main subdir
    if not options.nocopy and os.path.exists (outdir):
        shutil.rmtree (outdir)
    refdir = pathjoin (options.refdir, 'DevTool')
    platform, processor = getPlatform ()
    oldChange = getOldMaxChange (refdir)
    newChange= getCurrentMaxChange ()
    outfile = 'sapdb-devtools-%s-%s-%05dto%05d' % (
        platform, processor, oldChange, newChange)
    copyClerk = TreeDiffCopy (refdir, _ignorePatterns,
        options.verbose, options.nocopy)
    tool = os.environ ['TOOL']
    filesCopied = None
    for dirname in toolDirs:
        source = pathjoin (tool, dirname)
        target = outdir
        copyClerk.copy (source, target, tool)
        filesCopied = 1
    if filesCopied and not options.nocopy:
        # write changelist number
        syncinfoFile = pathjoin (outdir, 'syncinfo.dat')
        open (syncinfoFile, 'wt').write (
            '<maxchangenr>%s</maxchangenr>\n' % newChange)
        # zip directory
        localfile = packDir (options.outdir, outfile, verbose = options.verbose)
        # transfer to staging area
        if options.ftp:
            putFTP (options.ftp, options.staging + '/develop/tools/updates-' + platform,
                outfile, localfile)
        # update reference directory
        copyClerk = TreeCopy ([], options.verbose, None)
        copyClerk.copy (outdir, options.refdir, options.outdir)
        shutil.rmtree (outdir)

def putFTP (connectString, remoteDir, filename, localfile):
    match = re.match ('(.*):(.*)@(.*)', connectString)
    user, password, host = match.group (1, 2, 3)
    session = ftplib.FTP (host, user, password)
    try:
        session.cwd (remoteDir)
        session.storbinary ('STOR ' + filename + _zipExt,
            open (localfile, 'rb'), 4096)
    finally:
        session.quit ()

