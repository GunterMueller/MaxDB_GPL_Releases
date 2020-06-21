# distribTools/rpmLib.py
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
import os
pathjoin = os.path.join
isdir = os.path.isdir
fileExists = os.path.exists
getmtime = os.path.getmtime
import shutil

import common

def notImplemented (procname):
    print procname, 'not implemented'

#
# handle build tool sources
#

toolDepot = '//sapdb/TOOLSRC/develop/'

def copyToolSources (options):
    return
    if options.toolSync:
        options.p4.cmd ('sync ' + toolDepot + '...' + options.toolSync, 1)
    localRoot = options.p4.getLocalRoot (toolDepot)
    if options.clear:
        shutil.rmtree (localRoot)
    excludes = map (os.path.normpath, common.loadExcludes ())
    for sub in ['desc', 'src']:
        source = pathjoin (localRoot, 'sys', sub)
        target = pathjoin (options.packDir, 'buildtools', 'sys', sub)
        copyTree (source, target, excludes, options)

#
# handle main sources
#

def copyMainSources (options):
    sourceDepot = getSourceDepot (options.sapdbRel)
    copySources (options, sourceDepot, options.mainSync,
        common._excludeFiles, 'main')

def getSourceDepot (relstring):
    items = string.split (relstring, '.')
    result = '//sapdb/V' + items [0] + items [1] + '/'
    if len (items) == 2:
        result = result + 'develop/'
        return result
    result = result + ('c_%02d/' % int (items [2]))
    if len (items) == 3:
        result = result + 'develop/'
        return result
    result = result + ('b_%02d/' % int (items [3]))
    return result

#
# create actual rpm
#

def packRPM (options):
    notImplemented ('packRPM')


#
# common routines
#

def copySources (options, depot, syncOption, excludes, subdir):
    if syncOption:
        options.p4.cmd ('sync ' + depot + '...' + syncOption, 1)
    localRoot = options.p4.getLocalRoot (depot)
    if options.clear:
        shutil.rmtree (localRoot)
    excludes = map (os.path.normpath, excludes)
    for sub in ['desc', 'src']:
        source = pathjoin (localRoot, 'sys', sub)
        target = pathjoin (options.packDir, subdir, 'sys', sub)
        copyTree (source, target, excludes, options)


def copyTree (source, target, excludes, options):
    if options.verbose:
        print source
    targetdirExists = isdir (target)
    entries = os.listdir (source)
    if source [-1] != os.sep:
        source = source + os.sep
    for entry in entries:
        fullSource = pathjoin (source, entry)
        if isExcluded (fullSource, excludes):
            if options.veryVerbose:
                print 'X', fullSource, 'skipped'
            continue
        fullTarget = pathjoin (target, entry)
        if isdir (fullSource):
            copyTree (fullSource, fullTarget, excludes, options)
        else:
            if options.unconditional or hasChanged (fullSource, fullTarget):
                if options.veryVerbose:
                    print '=', fullTarget, 'copied'
                if not targetdirExists:
                    if not isdir (target):
                        os.makedirs (target)
                    targetdirExists = 1
                shutil.copyfile (fullSource, fullTarget)

def isExcluded (fname, patterns):
    for pattern in patterns:
        # print fname [-len (pattern):] == pattern, `fname [-len (pattern):]`, `pattern`
        if fname [-len (pattern):] == pattern:
            return 1
    return None

def hasChanged (fullSource, fullTarget):
    if not fileExists (fullTarget):
        return 1
    sourceTime = getmtime (fullSource)
    targetTime = getmtime (fullTarget)
    return targetTime < sourceTime

def getDefaultRel ():
    relver = os.environ.get ('RELVER')
    if relver is None:
        return None
    if (relver [0] != 'R') or (len (relver) != 3):
        return None
    result = relver [1] + '.' + relver [2]
    cl = os.environ.get ('CORRECTION_LEVEL')
    if cl:
        result = result + '.' + cl
    build = os.environ.get ('BUILDPRAEFIX')
    if build:
        result = result + '.' + build
    return result

#
# Perforce handling
#
class Perforce:
    def __init__ (self, client = None):
        if client:
            self.prefix = 'p4 -c ' + client + ' '
        else:
            self.prefix = 'p4 '

    def cmd (self, cmd, quiet = None):
        if quiet:
            stream = os.popen (self.prefix + cmd)
            stream.read ()
            return stream.close ()
        else:
            return os.system (self.prefix + cmd)

    def readlines (self, cmd):
        return os.popen (self.prefix + cmd).readlines ()

    def getLocalRoot (self, depot, filename = 'sys/src/sp/vsp001'):
        lines = self.readlines ('fstat ' + depot + filename)
        for line in lines:
            words = string.split (line)
            if words [1] == 'clientFile':
                clientFile = words [2]
                localRoot = clientFile [:-len (filename)]
                return localRoot
        raise 'Missing information: no clientFile entry for ' + filename

