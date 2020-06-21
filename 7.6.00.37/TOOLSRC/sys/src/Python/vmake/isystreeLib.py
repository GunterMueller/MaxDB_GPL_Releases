
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
import re
import os
isdir  = os.path.isdir
pathjoin = os.path.join
dirpart = os.path.dirname
exists = os.path.exists
getmtime = os.path.getmtime
import shutil

import vpathLib

own = os.environ ['OWN']
vmakePath = filter (None, string.split (os.environ ['VMAKE_PATH'], ','))
tool = os.environ ['TOOL']

class FileNotFound:
    def __init__ (self, fname):
        self.fname = fname

    def __str__ (self):
        return 'FileNotFound: ' + self.fname


class SysTree:
    def __init__ (self, options):
        self.options = options
        self.configureFileTranslation ()
        self.buildExcludeList (options.exclude)
        self.filesNotFound = []

    def configureFileTranslation (self):
        nl = self.options.nl
        if nl == 'native':
            self.translateData = self.translateNative
        elif nl == 'unix':
            self.translateData = self.translateUnix
        elif nl == 'dos':
            self.translateData = self.translateDos
        else:
            raise 'Option -nl must be [native, unix, dos]', nl

    def buildExcludeList (self, filesWithExcludes):
        self.excludes = excludes = {}
        normpath = os.path.normpath
        join = os.path.join
        for inputfile in filesWithExcludes:
            fnames = string.split (open (inputfile, 'r').read (), '\n')
            fnames = filter (None, fnames)
            for fname in fnames:
                fname = normpath (join ('sys', fname))
                excludes [fname] = 1


    def findInVPath (self, relative):
        for path in vmakePath:
            fullPath = pathjoin (path, relative)
            if exists (fullPath):
                return fullPath
        raise FileNotFound (relative)


    def copy (self, fname):
        fname = os.path.abspath (os.path.normpath (fname))
        relative = fname [len (own) + 1:]
        try:
            fullPath = self.findInVPath (relative)
            if isdir (fullPath):
                self.copyTree (fname)
            else:
                # split relative
                self.copyFile (relative, fullPath)
        except FileNotFound, err:
            self.filesNotFound.append (err.fname)
            print err

    def copyTree (self, fullPath):
        files = vpathLib.listdir (fullPath)
        for layer, relative in files:
            # print layer, relative
            fullPath = pathjoin (layer, relative)
            if isdir (fullPath):
                self.copyTree (pathjoin (own, relative))
            else:
                self.copyFile (relative, fullPath)

    def copyFile (self, relative, fullPath):
        outfile = pathjoin (self.options.outdir, relative)
        if not self.requiresCopy (outfile, fullPath, relative):
            return
        if self.options.verbose:
            print relative
        if self.options.nocopy:
            return
        self.prepareDir (relative)
        data = open (fullPath, 'rb').read ()
        data = self.translateData (data)
        open (outfile, 'wb').write (data)

    def translateNative (self, data):
        return data

    def translateUnix (self, data):
        if self.isTextFile (data):
            data = string.replace (data, '\r\n', '\n')
        return data

    def translateDos (self, data):
        if self.isTextFile (data):
            data = string.replace (data, '\r\n', '\n')
            data = string.replace (data, '\n', '\r\n')
        return data

    def prepareDir (self, relative):
        dirname = dirpart (relative)
        targetDir = pathjoin (self.options.outdir, dirname)
        if not isdir (targetDir):
            os.makedirs (targetDir)

    def isTextFile (self, data):
        chunk = data [:1024]
        for ch in '\x00\x01\x02\x03\x04\x05':
            if string.find (chunk, ch) != -1:
                return None
        return 1

    def requiresCopy (self, target, source, relative):
        if self.excludes.has_key (relative):
            return 0
        if self.options.unconditional:
            return 1
        if not exists (target):
            return 1
        sourcemtime = getmtime (source)
        targetmtime = getmtime (target)
        if targetmtime < sourcemtime:
            return 1
        return 0


class VmakeModules:
    def __init__ (self, targets, platforms, protFiles):
        self.targets = string.join (targets)
        self.modules = {}
        for platform in platforms:
            self.addRun (platform)
        for protFile in protFiles:
            self.addFile (protFile)

    moduleRe = re.compile ('(src/)|(desc/)')

    def addRun (self, platform):
        if platform == 'win32':
            cmd = 'perl %s/bin/ims.pl -m ' % tool
        else:
            cmd = 'perl %s/bin/remims.pl %s -m ' % (tool, platform)
        lines = os.popen (cmd + self.targets, 'rt').readlines ()
        self.parseLines (lines)

    def addFile (self, protFile):
        lines = open (protFile, 'rt').readlines ()
        self.parseLines (lines)

    def getModules (self):
        modules = self.modules.keys ()
        modules.sort ()
        return modules

    def parseLines (self, lines):
        moduleRe = self.moduleRe
        for line in lines:
            if moduleRe.match (line):
                self.modules [line [:-1]] = 1


def fatalError (source,  msg):
    sys.stderr.write (source + ': ' + msg + '\n')
    sys.exit (2)

def getEnvvar (source, varname, isInt = None):
    value = os.environ.get (varname)
    if not value:
        fatalError (source, '$' + varname + ' not in environment')
    if isInt:
        try:
            value = int (value)
        except ValueError:
            fatalError (source, '$' + varname + ' must be numeric')
    return value

def getBuildReleaseVars ():
    source = 'isystreeLib.buildTop;LevelName'
    relver = getEnvvar (source, 'RELVER')
    if re.match ('R[0-9]+$', relver):
        relver = 'V' + relver [1:]
    corrLevel = getEnvvar (source, 'CORRECTION_LEVEL', 1)
    patchlevel = getEnvvar (source, 'BUILDPRAEFIX', 1)
    return (relver, corrLevel, patchlevel)

def createDir (path):
    if not exists (path):
        os.makedirs (path)

def createTreeRoot (outdir, nl, readme):
    release = '%s_%02d_%02d' % getBuildReleaseVars ()
    if release [1:3] >= '75':
        prodname = 'MaxDB'
        dirprefix = 'MaxDB'
    else:
        prodname = 'SAP DB'
        dirprefix = 'SAPDB'
    topDir = pathjoin (outdir, release)
    substitution = VariableSubstitution (locals ())
    #
    # create dirs
    #
    createDir (topDir)
    createDir (pathjoin (topDir, dirprefix + '_DEV'))
    orgDir = pathjoin (topDir, dirprefix + '_ORG')
    createDir (orgDir)
    #
    # copy install script
    #
    writeDistribData (topDir, 'installsources.pl', nl, substitution)
    #
    # copy readme
    #
    if readme:
        writeDistribData (topDir, readme, nl, substitution)
    #
    #
    #
    return orgDir

def writeDistribData (topDir, fname, nl, substitution = None):
    data = open (pathjoin (os.environ ['TOOL'], 'data', 'distribTools', fname)).read ()
    if nl == 'native':
        pass
    elif nl == 'unix':
        data = string.replace (data, '\r\n', '\n')
    elif nl == 'dos':
        data = string.replace (data, '\r\n', '\n')
        data = string.replace (data, '\n', '\r\n')
    else:
        raise 'Option -nl must be [native, unix, dos]', nl
    if substitution:
        data = substitution.substitute (data)
    open (pathjoin (topDir, fname), 'wb').write (data)

class VariableSubstitution:
    rex = re.compile ('%[(]([^)]+)[)]s')
    def __init__ (self, variables):
        self.variables = variables

    def substituteOne (self, match):
        return self.variables [match.group (1)]

    def substitute (self, text):
        return self.rex.sub (self.substituteOne, text)


def createTGZ (datadir, outdir, verbose = None, tag = None):
    pardir, dummy = os.path.split (datadir)
    pardir, topLevel = os.path.split (pardir)
    os.chdir (pardir)
    relver, corrLevel, patchlevel = getBuildReleaseVars ()
    relMajor = int (relver [1])
    relMinor = int (relver [2])
    if (relMajor, relMinor) >= (7, 5):
        prodname = 'maxdb'
        fmt = '%s%s-source-%s_%s_%02d_%02d.tgz'
    else:
        prodname = 'sapdb'
        fmt = '%s%s-source-%s.%s.%02d.%02d.tgz'
    if tag:
        tag = '-' + tag
    else:
        tag = ''
    outfile = (fmt %
        (prodname, tag, relMajor, relMinor, corrLevel, patchlevel))
    outfile = os.path.join (outdir, outfile)
    cmd = 'tar cof - ' + topLevel + ' | gzip -c -9 > ' + outfile
    if verbose:
        print cmd
    os.system (cmd)

def createZIP (datadir, outdir, verbose = None, tag = None):
    pardir, dummy = os.path.split (datadir)
    pardir, topLevel = os.path.split (pardir)
    os.chdir (pardir)
    relver, corrLevel, patchlevel = getBuildReleaseVars ()
    relMajor = int (relver [1])
    relMinor = int (relver [2])
    if (relMajor, relMinor) >= (7, 5):
        prodname = 'maxdb'
        fmt = '%s%s-source-%s_%s_%02d_%02d.zip'
    else:
        prodname = 'sapdb'
        fmt = '%s%s-source-%s.%s.%02d.%02d.zip'
    if tag:
        tag = '-' + tag
    else:
        tag = ''
    outfile = (fmt %
        (prodname, tag, relMajor, relMinor, corrLevel, patchlevel))
    outfile = os.path.join (outdir, outfile)
    cmd = 'zip -r -q %s %s' % (outfile, topLevel)
    if verbose:
        print cmd
    os.system (cmd)


