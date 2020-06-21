
# fileutil.py           search for a file
#                       create all directories of a path
#
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
import tempfile
import stat

def find (fname, searchPath = None, defaultExtension = None):
    return findMulti (fname, searchPath, [defaultExtension])

def findExt (fname, defaultExtensions):
    if type (defaultExtensions) == type (''):
        defaultExtension = [defaultExtensions]
    return findMulti (fname, None, defaultExtensions)

def findMulti (fname, searchPath = None, defaultExtensions = None):
    # absolute -> replace searchpath
    if os.path.isabs (fname):
        searchPath = ['']
    else:
        # searchPath is string -> break into list
        if type ('') == type (searchPath):
            searchPath = string.splitfields (searchPath, os.pathsep)
        elif searchPath == None:
            searchPath = [os.curdir]
    # has extension -> ignore defaults
    (root, ext) = os.path.splitext (fname)
    if ext == '':
        hasExtension = 0
    else:
        hasExtension = 1
    for s in searchPath:
        if hasExtension:
            fullname = os.path.join (s, fname)
            if os.path.exists (fullname):
                return fullname
        else:
            for ext in defaultExtensions:
                fullname = os.path.join (s, fname + '.' + ext)
                if os.path.exists (fullname):
                    return fullname
        return None

class FileFinder:
    # searchPath
    # defaultExtensions
    def __init__ (searchPath, defaultExtensions):
        self.searchPath = searchPath;
        self.defaultExtensions = defaultExtensions

    def find (self, fname):
        return findMulti (fname, self.searchPath, self.defaultExtensions)

def defaultExtension (fname, defExt):
    (root, ext) = os.path.splitext (fname)
    if ext == '':
        fname = fname + '.' + defExt
    return fname

def setExtension (fname, extension):
    (root, ext) = os.path.splitext (fname)
    if extension != None:
        return root + '.' + extension
    else:
        return root

def missExtension (fname):
    (root, ext) = os.path.splitext (fname)
    return root

def extension (fname):
    (root, ext) = os.path.splitext (fname)
    return ext

def basename (fname):
    (result, ext) = os.path.splitext (os.path.basename (fname))
    return result

def backupName (fname):
    head, tail = os.path.split (fname)
    return os.path.join (head, '~' + tail)

def makeBackup (fname):
    bname = backupName (fname)
    if os.path.exists (bname):
        os.remove (bname)
    os.rename (fname, bname)
    return bname

def multijoin (*pathParts):
    result = ''
    join = os.path.join
    for part in pathParts:
        result = join (result, part)
    return result

def createDirs (path, includesFile = None):
    ospath = os.path
    ossep = os.sep
    if includesFile:
        path, base = ospath.split (path)
    if len (path) == 0:
        return
    if ospath.exists (path):
        return
    dirlist = string.splitfields (path, ossep)
    if ospath.isabs (path):
        partpath = dirlist [0]
        dirlist = dirlist [1:]
    else:
        partpath = "."
    for d in dirlist:
        partpath = partpath + ossep + d
        print 'createDirs: ', partpath
        if not ospath.isdir (partpath):
            os.mkdir (partpath, 0)

def copyFile (source, target):
    blocksize = 1024 * 1024
    try:
        instream = open (source, "rb")
    except IOError:
        sys.stderr.write ('Cannot open ' + source + '\n')
        return
    outstream = open (target, "wb")
    while 1:
        block = instream.read (blocksize)
        outstream.write (block)
        if len (block) < blocksize:
            break
    instream.close ()
    outstream.close ()

def readlines (source):
    if type (source) == type (''):
        source = open (source, 'r')
    result = []
    while 1:
        line = source.readline ()
        if len (line) == 0:
            break
        if line [-1] == '\n':
            line = line [:-1]
        result.append (line)
    return result

class TextStream:
    def __init__ (self, source, strip = None):
        if type (source) == type (''):
            # a string is supposed to be a filename
            self.fileObject = open (source, "r")
        else:
            self.fileObject = source
        self.strip = strip
#        self.lineno = -1

    def readline (self):
        result = self.fileObject.readline ()
        if not result:
            raise EOFError
#        self.lineno = self.lineno + 1
        if self.strip and (result [-1] == '\n'):
            return result [:-1]
        else:
            return result

    def readlines (self, count = -1):
        if not count and not self.strip:
            return self.fileObject.readlines ()
        else:
            result = []
            try:
                while count != 0:
                    result.append (self.readline (1))
                    count = count - 1
            except 'EOF':
                pass
            return result

    def __getitem__ (self, index):
        try:
#            if index > self.lineno + 1:
#                self.readlines (index - self.lineno)
            return self.readline ()
        except EOFError:
            raise IndexError

class BinaryStream:
    def __init__ (self, source, blockSize = 4096):
        if type (source) == type (''):
            # a string is supposed to be a filename
            self.fileObject = open (source, "rb")
        else:
            self.fileObject = source
        self.blockSize = blockSize

    def readblock (self):
        result = self.fileObject.read (self.blockSize)
        if not result:
            self.fileObject.close ()
            raise EOFError
        return result

    def __getitem__ (self, index):
        try:
            return self.readblock ()
        except EOFError:
            raise IndexError

class RecursiveFileIterator:
    def __init__ (self, roots, dirsToIgnore = []):
        self.dirQueue = []
        self.includeDirs = None
        self.fileQueue = []
        self.dirsToIgnore = dirsToIgnore [:]
        for root in roots:
            self.addFile (root)

    def addFile (self, fname):
        if os.path.isdir (fname):
            for ignore in self.dirsToIgnore:
                ignoreLen = len (ignore)
                if fname [-ignoreLen:] == ignore:
                    return
            self.dirQueue.insert (0, fname)
            if self.includeDirs:
                self.fileQueue.insert (0, fname)
        else:
            self.fileQueue.insert (0, fname)

    def __getitem__ (self, index):
        while len (self.fileQueue) == 0:
            self.nextDir ()
        result = self.fileQueue [0]
        del self.fileQueue [0]
        return result

    def nextDir (self):
        dir = self.dirQueue [0]   # fails with IndexError, which is fine
                                  # for iterator interface
        del self.dirQueue [0]
        list = os.listdir (dir)
        list.reverse ()           # reverse in combination
                                  # with insert (0, ...) in addFile
                                  # inplements depth first
        join = os.path.join
        for basename in list:
            fullPath = join (dir, basename)
            self.addFile (fullPath)


class RecursiveDirIterator:
    def __init__ (self, roots, excludeREs):
        self.excludeREs = []
        for excludeRE in excludeREs:
            if type (excludeRE) == type (''):
                excludeRE = re.compile (excludeRE)
            self.excludeREs.append (excludeRE)
        self.dirQueue = []
        for root in roots:
            self.addDir (root)

    def addDir (self, dirname):
        for excludeRE in self.excludeREs:
            if excludeRE.search (dirname):
                return
        self.dirQueue.insert (0, dirname)

    def nextDir (self):
        dir = self.dirQueue [0]   # fails with IndexError, which is fine
                                  # for iterator interface
        del self.dirQueue [0]
        list = os.listdir (dir)
        list.reverse ()           # reverse in combination
                                  # with insert (0, ...) in addFile
                                  # inplements depth first
        join = os.path.join
        isdir = os.path.isdir
        for basename in list:
            fullPath = join (dir, basename)
            if isdir (fullPath):
                self.addDir (fullPath)
        return dir

    def __getitem__ (self, index):
        return self.nextDir ()

class DepthFirstDirs:
    def __init__ (self, roots, excludeREs):
        self.collectDirs (roots, excludeREs)

    def collectDirs (self, roots, excludeREs):
        data = []
        for dirname in RecursiveDirIterator (roots, excludeREs):
            data.append (dirname)
        data.sort ()
        data.reverse ()
        self.data = data

    def __getitem__ (self, index):
        return self.data [index]



class TeeStream:
    def __init__ (self, stream1, stream2 = sys.stdout):
        self.streams = [stream1, stream2]

    def write (self, text):
        for stream in self.streams:
            stream.write (text)

    def writelines (self, lines):
        for stream in self.streams:
            stream.writelines (lines)

def writeTmpFile (text, kind = 't'):
    tmpname = tempfile.mktemp ()
    stream = open (tmpname, 'w' + kind)
    stream.write (text)
    stream.close ()
    return tmpname

if sys.platform == 'win32':
    def isWritable (fname):
        mode = os.stat (fname)
        return 0200 & mode [0]
elif os.name == 'posix':
    def isWriteble (fname):
        # check for user, group, world
        raise 'isWritable () not implemented'
else:
    def isWriteble (fname):
        raise 'isWritable () not implemented'

class TempFile:
    def __init__ (self):
        self.fname = tmpname = tempfile.mktemp ()

    def setContent (self, data):
        stream = open (self.fname, 'w')
        stream.write (data)
        stream.close ()
        return self

    def __del__ (self):
        try:
            os.remove (self.fname)
        except:
            pass


