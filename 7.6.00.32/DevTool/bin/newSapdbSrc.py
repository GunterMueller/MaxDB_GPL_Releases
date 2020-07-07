#!/usr/bin/env python
# newSapdbSrc.py

import sys
import os
import string
import re
from UserDict import UserDict

def addPythonLib ():
    toolLib = os.path.normpath (os.path.join (sys.path [0], '..', 'lib/Python'))
    sys.path.insert (1, toolLib)

addPythonLib ()

if sys.platform == 'win32':
    win32 = 1
    exeext = '.exe'
    batext = '.cmd'
    defaultEditor = 'notepad.exe'
    iprofilebase = 'iprofile.cmd'
    def envvar (name):
        return '%' + name + '%'

else:
    win32 = None
    exeext = ''
    batext = ''
    defaultEditor = os.environ.get ('EDITOR', 'vi')
    iprofilebase = '.iprofile'
    def envvar (name):
        return '$' + name

def fatalError (msg):
    sys.stderr.write ('newSapdbSrc ERR: ' + msg + '\n')
    sys.exit (2)

def getOutfile (output, defaultName):
    if os.path.isdir (output):
        output = os.path.join (output, defaultName)
    return output

class RexReplace (UserDict):
    def replaceMatch (self, match):
        return self [match.group (0)]

def findInPath (fname):
    path = os.environ ['PATH']
    for dirname in string.split (path, os.pathsep):
        fullPath = os.path.join (dirname, fname)
        if os.path.exists (fullPath):
            return dirname
    return ''


def isPythonInclude (path):
    if not os.path.exists (os.path.join (path, 'Python.h')):
        return None
    if os.path.exists (os.path.join (path, 'config.h')):
        return 1
    if os.path.exists (os.path.join (path, 'pyconfig.h')):
        return 1
    return None

def possiblePythonIncludes ():
    result = []
    for suffix in ['', '2.1', '2.2', '2.3']:
        result.append ('/usr/include/python' + suffix)
    return result

if win32:
    def findPythonIncludes (pyroot = None):
        if pyroot is None:
            return 'NO'
        else:
            dirname = os.path.join (pyroot, 'include')
            if isPythonInclude (dirname):
                return dirname
            else:
                return 'NO'

else:

    def findPythonIncludes ():
        for dirname in possiblePythonIncludes ():
            if isPythonInclude (dirname):
                return dirname
        return 'NO'

_releaseFileName = 'sys/src/sp/gsp100.h'
_VERSIONSFileName = 'sys/src/config/Buildinfo/VERSIONS'

def findReleaseFile (vmakePath, filename = _releaseFileName):
    for dirname in vmakePath:
        fullPath = os.path.join (dirname, filename)
        if os.path.exists (fullPath):
            return fullPath
    return None

def makeAbsolute (path):
    colonPos = string.find (path, ':')
    if colonPos > 1:
        # no colon or one letter for DOS drive
        return path
    else:
        return os.path.abspath (path)

def createVMakePath (args, releaseNumber = None):
    if len (args) == 0:
        fatalError ('must specify at least one source directory')
    if releaseNumber == None:
        # find release file
        releaseFile = findReleaseFile (args)
        if releaseFile is None:
            fatalError ('no directory contained ' + _releaseFileName)
    # make all absolute
    vmakePath = map (makeAbsolute, args)
    # create non existent directories
    if not os.path.exists (vmakePath [0]):
        os.mkdir (vmakePath [0])
    #
    return vmakePath

def getReleaseFrom_gsp100 (gsp100):
    source = open (gsp100, 'r').read ()
    result = []
    for name in ['MAJOR_VERSION_NO_SP100', 'MINOR_VERSION_NO_SP100',
            'CORRECTION_LEVEL_SP100', 'BUILD_PREFIX_SP100']:
        match = re.search (r'#\s*define\s+' + name + r'\s+(\d+)', source)
        if match is None:
            fatalError ('invalid release info in ' + gsp100)
        result.append (match.group (1))
    return result

def getReleaseFrom_VERSIONS (versions):
    lines = map (string.strip, open (versions, 'r').readlines ())
    state = 'intro'
    for line in lines:
        if line == '[kernel]':
            state = 'kernel'
        elif line [0] == '[':
            stat = 'outro'
        elif (line [:8] == 'version=') and (state == 'kernel'):
            versionString = line [8:]
            return string.split (versionString, '.')
    fatalError ('invalid release info in ' + versions)

def getReleaseInfo (vmakePath):
    versionFile = findReleaseFile (vmakePath, _VERSIONSFileName)
    if versionFile:
        return getReleaseFrom_VERSIONS (versionFile)
    releaseFile = findReleaseFile (vmakePath)
    return getReleaseFrom_gsp100 (releaseFile)

def fillTemplate (options, vmakePath):
    if options.release:
        releaseInfo = string.split (options.release, '.')
    else:
        releaseInfo = getReleaseInfo (vmakePath)
    tool = os.path.abspath (os.path.normpath (os.path.join (sys.path[0], '..')))
    if (win32) and (options.pyinclude == 'NO'):
        options.pyinclude = findPythonIncludes (options.python)
    # get template
    templateFile = os.path.join (tool, 'data', 'distribTools', 'iprofile')
    if not os.path.exists (templateFile):
        fatalError ('Cannot find ' + templateFile)
    source = open (templateFile, 'r').read ()
    # build dict
    replacements = RexReplace ({
        '�DEVTOOL': tool,
        '�MAJ':  releaseInfo [0],
        '�MIN':  releaseInfo [1],
        '�COR':  releaseInfo [2],
        '�BLDPRFX': releaseInfo [3],
        '�DEV':  vmakePath [0],
        '�ORG':  string.join (vmakePath [1:], ','),
        '�PERL': options.perl,
        '�PYTHON': options.python,
        '�PYINCL': options.pyinclude,
        '�EDITOR': options.editor,
    })
    # replace values
    filled = re.sub ('�[A-Za-z+]+', replacements.replaceMatch, source)
    # write
    iprofile = os.path.join (vmakePath [0], iprofilebase)
    open (iprofile, 'w').write (filled)

if win32:
    def writeInitScript (options, vmakePath):
        initScript = getOutfile (options.output, 'devsapdb' + batext)
        stream = open (initScript, 'w')
        stream.write ('@ echo off\n')
        stream.write ('rem script to init the SAP DB developpment environment\n')
        if options.vcvars:
            stream.write ('call "' + options.vcvars + '"\n')
        stream.write ('cd ' + vmakePath [0] + '\n')
        stream.write ('call ' + iprofilebase + '\n')
        stream.close ()
        print "execute " + initScript + ' to start SAP DB development'
else:
    def writeInitScript (options, vmakePath):
        initScript = getOutfile (options.output, 'devsapdb')
        stream = open (initScript, 'w')
        stream.write ('# script to init the SAP DB developpment environment\n')
        stream.write ('cd ' + vmakePath [0] + '\n')
        stream.write ('. ' + iprofilebase + '\n')
        stream.close ()
        print "execute . " + initScript + ' to start SAP DB development'

def newSapdbSrc (options, args):
    vmakePath = createVMakePath (args, options.release)
    fillTemplate (options, vmakePath)
    writeInitScript (options, vmakePath)

def checkOptions (options):
    if not options.perl:
        fatalError ('Cannot find perl' + exeext + ' through ' + envvar ('PATH'))
    if win32 and (os.path.basename (options.perl) == 'bin'):
        options.perl = os.path.dirname (options.perl)
    if not options.python:
        fatalError ('Cannot find python' + exeext + ' through ' + envvar ('PATH'))


def main (options, args):
    """initialize a new SAP DB development source tree

    All command line arguments are directories to be used to search
    for sources, at least one must contain a file sys/src/sp/gsp100.h.

    If the first directory doesn't exist, it will be created

    """
    checkOptions (options)
    newSapdbSrc (options, args)

def _options ():
    options = [
        # (optstring, varname, typechar, default, help)
        ('perl', None, ':', findInPath ('perl' + exeext), 'path to perl executable'),
        ('python', None, ':', findInPath ('python' + exeext), 'path to python executable'),
        ('pyinclude', None, ':', findPythonIncludes (), 'path to Python include files'),
        ('editor', None, ':', defaultEditor, 'default editor'),
        ('o', 'output', ':', 'devsapdb' + batext, 'name of script to init dev environment'),
        ('release', None, ':', None, 'release number d.d.dd.dd'),
        ]
    if win32:
        options = options + [
        ('vcvars', None, ':', None, 'batch file to initialize Visual C++ environment [vcvars32.bat]'),
        ]
    return options

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

