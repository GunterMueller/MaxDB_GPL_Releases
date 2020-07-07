
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
from ftplib import FTP

import optlib

testMode = os.environ.get ('DANIELD_TESTMODE')

#
# files to exclude
#

def stripComment (line):
    pos = string.find (line, '#')
    if pos == -1:
        result = line
    else:
        result = line [:pos]
    return result

class RE_Match:
    def __init__ (self, result, reString):
        self.result = result
        self.rex = re.compile (reString)

    def decide (self, value):
        if self.rex.search (value):
            return self.result
        else:
            return 0

    def __str__ (self):
        return '[%s %d]' % (self.rex.pattern, self.result)
    
class SubstringMatch:
    def __init__ (self, result, substring):
        self.result = result
        self.substring = substring

    def decide (self, value):
        if string.find (value, self.substring) != -1:
            return self.result
        else:
            return 0    

    def __str__ (self):
        return '[%s %d]' % (self.substring, self.result)
    

def getExcludeLines (lines):
    result = []
    rules = {
        'noDir':        (SubstringMatch, -1),
        'alwaysDir':    (SubstringMatch, 1),
        'noRE':         (RE_Match, -1),
        'alwaysRE':     (RE_Match, 1),
    }
    rex = re.compile (r'-([a-zA-Z_]+)\s*:\s*(.*)')
    for line in lines:
        line = string.strip (stripComment (line))
        if not line:
            continue
        match = rex.match (line)
        assert match, "'%s' must match %s" % (line, rex.pattern)
        try:
            keyword, data = match.group (1, 2)
            clazz, resultVal = rules [keyword]
        except KeyError:
            print '***', line
            raise 'unknown keyword in NoOpenSource rule file', keyword
        decider = clazz (resultVal, data)
        result.append (decider)
    return result

def getExcludeLinesFromFile (fname):
    lines = open (fname, 'r').readlines ()
    return getExcludeLines (lines)

defaultExclude = os.path.join (os.environ.get ('OWN', ''), 'sys', 'config', 'noOpenSource')

def loadExcludes (path = defaultExclude):
    files = os.listdir (path)
    result = [SubstringMatch (-1, '/config/noOpenSource/')]
    for file in files:
        result.extend (getExcludeLinesFromFile (os.path.join (path, file)))
    return result

def isExcluded (fname, patterns):
    for isRE, pattern in patterns:
        if isRE:
            if pattern.match (fname):
                return 1
        else:
            if fname [:len (pattern)] == pattern:
                return 1
    return None

def loadExcludesFromPerforce (p4, path):
    files = p4.listdir (path)
    result = []
    for file in files:
        data = p4.readFile (file)
        lines = string.split (data, '\n')
        result.extend (getExcludeLines (lines))
    return result

def loadExcludesFromVar (p4, noOpenSource):
    if os.path.isdir (noOpenSource):
        result = loadExcludes (noOpenSource)
    else:
        result = loadExcludesFromPerforce (p4, noOpenSource)
    return result

#
# command line options
#

import perforce.perforceLib

_cmdlineOptions = [
        ('ftp', None, ':', 'FileExchange:ExchangeFile@p48047', 'ftp login'),
        ('staging', None, ':', '/staging/sapdb', 'staging dir on FTP server'),
        ('p4', 'perforceClient', ':', None, 'name of perforce client'),
        ('root', None, '::', None, 'root dir in depot notation'),
        ('perforceRoot', None, ':', None, 'use all directories under this root'),
        ('o', 'outdir', ':', '.', 'output directory'),
]

def checkOptions (options):
    if not options.perforceClient:
        optlib.fatalError ('perforce client must be specified (-p4)')
    options.p4 = perforce.perforceLib.PerforceSession (options.perforceClient)
    options.root = getRoots (options)
    assert options.root, '-root must be specified'
    for root in options.root:
        if root [:2] != '//':
            optlib.fatalError ('-root not in perforce depot notation\n')
    options.outdir = os.path.abspath (options.outdir)

def getRoots (options):
    if options.perforceRoot:
        lines = options.p4.getLines (' dirs ' + options.perforceRoot + '/*')
        return map (string.strip, lines)
    else:
        return options.root

#
# copy standard source files
#
_licenceFiles = [
    'licence/gpl.txt', 'licence/lesser.txt', 'licence/fdl.txt',
]

_standardFiles = _licenceFiles [:]

def copyStandardFiles (targetDir, readmes = []):
    candidates = [
        # os.path.join (os.environ ['SRC'], 'data'),
        os.path.join (os.environ ['TOOL'], 'data')]
    result = []
    for file in _standardFiles:
        fullSource = findFile (file, candidates)
        if not fullSource:
            continue
        fullTarget = os.path.join (targetDir, file)
        fullDir = os.path.dirname (fullTarget)
        if not os.path.isdir (fullDir):
            os.makedirs (fullDir)
        shutil.copyfile (fullSource, fullTarget)
        result.append (file)
    candidates = [
        os.path.join (os.environ ['SRC'], 'data'),
        os.path.join (os.environ ['TOOL'], 'data')]
    for inname, outname in readmes:
        readmeIn = findFile (inname, candidates)
        if readmeIn:
            shutil.copyfile (readmeIn, os.path.join (targetDir, outname))
        result.append (outname)
    return result

def findFile (fname, dirs):
    for dir in dirs:
        fullPath = os.path.join (dir, fname)
        print 'trying', fullPath
        if os.path.exists (fullPath):
            return fullPath
    return None

#
# archives naming and packing
#

toolvars = {}
def readToolvars ():
    lines = perlOutput ('toolvarsDump')
    state = 'wkey'
    for line in lines:
        if state == 'wkey':
            if not line:
                continue
            key = line
            state = 'wvalue'
        elif state == 'wvalue':
            toolvars [key] = line
            state = 'wkey'
            del key
        else:
            raise 'invalid state', state
    return
    items = toolvars.items ()
    items.sort ()
    for key, value in items:
        print '%s=%s' % (key, value)

osspecMap = {
    'WIN32': 'win',
    'LINUX': 'linux',
    'SVR4': 'solaris',
    'SOLARIS': 'solaris',
    'AIX': 'aix5',
    'HPUX': 'hpux',
    'OSF1': 'tru64',
}

machMap = {
    'I386': 'i386',
    'SUN': 'sparc',
    'SPARC': 'sparc',
    '_IBMR2': 'ppc',
    'PA20W': 'hppa',
    'PA11': 'hppa',
    'ALPHA': 'alpha',
    'IA64': 'ia64',
    'HP_IA64': 'ia64',
    'X86_64': 'x86_64',
    'AMD64': 'x86_64',
    'SDBonPPC64': 'ppc',
}

def detectPlatform (withBit = 1):
    if not toolvars:
        readToolvars ()
    osspec = osspecMap [toolvars ['OSSPEC']]
    try:
        os.environ ['BIT64']
        bit = '64bit'
    except KeyError:
        bit = '32bit'
    mach = machMap [toolvars ['MACH']]
    if withBit:
        return osspec + '-' + bit + '-' + mach
    else:
        return osspec + '-' + mach

def detectRelease ():
    if not toolvars:
        readToolvars ()
    relver = toolvars ['RELVER']
    major = int (relver [1])
    minor = int (relver [2])
    corr = int (toolvars ['CORRECTION_LEVEL'])
    build = int (toolvars ['BUILDPRAEFIX'])
    if toolvars ['RELSTAT'] == 'TEST':
        # alpha = 'a'
        alpha = '' # too cumbersome, because release packages are built using TEST
    else:
        alpha = ''
    return '%d_%d_%02d_%02d%s' % (major, minor, corr, build, alpha)

def detectExtension ():
    if sys.platform == 'win32':
        return '.zip'
    else:
        return '.tgz'


#
# ftp
#

import ftplib

def transferByFTP (options, fname, subdir = '/7.3/updates'):
    """transfer a file to the FTP staging area
    """
    if not options.ftp:
        return
    match = re.match ('(.*):(.*)@(.*)', options.ftp)
    user, password, host = match.group (1, 2, 3)
    session = ftplib.FTP (host, user, password)
    try:
        remoteDir = options.staging + subdir
        session.cwd (remoteDir)
        remoteName = os.path.basename (fname)
        stream = open (fname, 'rb')
        session.storbinary ('STOR ' + remoteName, stream, 4096)
    finally:
        session.quit ()

def _split2 (text, delimiter, optionalIsSecond = 1):
    pos = string.find (text, delimiter)
    if pos == -1:
        if optionalIsSecond:
            return (text, '')
        else:
            return ('', text)
    else:
        return (text [:pos], text [pos + 1:])

def _parseTarget (target):
    userinfo, location = _split2 (target, '@', None)
    if userinfo:
        username, pwd = _split2 (userinfo, ':', 1)
    else:
        username, pwd = ('anonymous', 'anonymous@invalidserver.net')
    host, dirname = _split2 (location, ':', 1)
    return host, username, pwd, dirname

def ftpTransfer (ftpinfo, outfile):
    if not ftpinfo:
        return
    if ftpinfo == 'default':
        ftpinfo = 'remuser:Remuser@ls8801.ber.sap.corp:ftp-incoming/OpenSource'
    host, username, pwd, targetdir = _parseTarget (ftpinfo)
    ftp = FTP(host, username, pwd)
    if targetdir:
        ftp.cwd (targetdir)
    cmd = 'STOR ' + os.path.basename (outfile)
    stream = open (outfile, 'rb')
    ftp.storbinary (cmd, stream, 8192)
    stream.close ()
    ftp.close ()


#
# misc
#
def perlOutput (cmd, *args):
    perlfile = os.path.join (os.environ ['TOOL'], 'bin', cmd + '.pl')
    cmdline = 'perl ' + perlfile + ' ' + string.join (args)
    lines = os.popen (cmdline, 'r').readlines ()
    return map (string.strip, lines)


#
# test
#
if __name__ == "__main__":
    print stripComment ('real data  # this is a comment')

