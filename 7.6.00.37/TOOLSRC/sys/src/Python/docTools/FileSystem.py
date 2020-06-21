
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

from string import split, replace, rfind, join

import os
ospath = os.path
pathjoin = os.path.join
dirname = os.path.dirname
basename = os.path.basename
dirsplit = os.path.split
splitext = os.path.splitext
normpath = os.path.normpath
normcase = os.path.normcase
exists = os.path.exists
abspath = os.path.abspath
getmtime = os.path.getmtime

class FileSystem:
    def __init__ (self, base = '.'):
        self.base = base

    def __str__ (self):
        return '<FileSystem ' + abspath (self.base) + '>'

    def absolute (self, name):
        result = pathjoin (self.base, name)
        return ospath.abspath (result)

    def exists (self, name):
        name = pathjoin (self.base, name)
        return ospath.exists (name)

    def isdir (self, name):
        name = pathjoin (self.base, name)
        return ospath.isdir (name)

    def getmtime (self, name):
        name = normpath (pathjoin (self.base, name))
        if exists (name):
            return getmtime (name)
        else:
            raise os.error, 'No such file: ' + name

    def join (self, part1, part2 = None):
        result = pathjoin (self.base, part1)
        if part2:
            result = pathjoin (result, part2)
        return result

    def listdir (self, dir = None, options = None):
        if dir:
            dir = pathjoin (self.base, dir)
        else:
            dir = self.base
        return os.listdir (dir)

    def open (self, fname, attributes = 'r'):
        fname = pathjoin (self.base, fname)
        if ('w' in attributes) or ('a' in attributes):
            dir, base = ospath.split (fname)
            if not ospath.exists (dir):
                os.makedirs (dir)
        return open (fname, attributes)

    def require (self, fname = None):
        if fname:
            fullname = pathjoin (self.base, fname)
        else:
            fullname = self.base
        if not ospath.exists (fullname):
            self.open (fname, 'w')
        return fullname

    def mkdir (self, dirname):
        dir = pathjoin (self.base, dirname)
        if not ospath.exists (dir):
            os.makedirs (dir)

    def stat (self, name):
        name = pathjoin (self.base, name)
        result = os.stat (name)
        return result

    def subdir (self, name):
        path = pathjoin (self.base, name)
        return FileSystem (path)

    def search (self, *files):
        for file in files:
            if self.exists (file):
                return file
        return None

    def file (self, filename = None):
        if filename == None:
            return File ('', self)
        filename = abspath (filename)
        relative = makeRelative (filename, self.base)
        if relative != None:
            relative = replace (relative, '\\', '/')
            return File (relative, self)
        raise 'FileError', (filename + ' not in ' + self.base)

    def shellName (self, fname):
        return pathjoin (self.base, fname)


class VmakeFS:
    def __init__ (self, path = None):
        if path == None:
            path = os.environ['VMAKE_PATH']
        if type (path) == type (''):
            path = split (path, ',')
        self.path = path

    def __str__ (self):
        return '<VmakeFS ' + self.path [0] + '>'

    def findFile (self, name, defaultToOwn = None):
        for dir in self.path:
            fullname = pathjoin (dir, name)
            if ospath.exists (fullname):
                return fullname
        if defaultToOwn:
            return pathjoin (self.path [0], name)
        else:
            return None

    def absolute (self, name):
        if ospath.isabs (name):
            return name
        fullname = self.findFile (name, 1)
        return fullname

    def exists (self, name):
        return self.findFile (name) != None

    def isdir (self, name):
        abs = self.absolute (name)
        if abs:
            result = ospath.isdir (abs)
        else:
            result = None
        return result

    def getmtime (self, name):
        name = self.findFile (name, 1)
        return ospath.getmtime (name)

    def join (self, part1, part2 = None):
        result = pathjoin (self.path [0], part1)
        if part2:
            result = pathjoin (result, part2)
        return result

    def listdir (self, dir = None, options = None):
        files = {}
        for path in self.path:
            if dir:
                list = self.rawListdir (pathjoin (path, dir))
            else:
                list = self.rawListdir (path)
            for entry in list:
                if not files.has_key (entry):
                    files [entry] = 1
        result = files.keys ()
        result.sort ()
        return result

    def rawListdir (self, path):
        try:
            result = os.listdir (path)
        except os.error:
            result = []
        return result

    def open (self, fname, attributes = 'r'):
        fname = self.findFile (fname, 1)
        if ('w' in attributes) or ('a' in attributes):
            dir, base = ospath.split (fname)
            if not ospath.exists (dir):
                os.makedirs (dir)
        return open (fname, attributes)

    def require (self, fname = None):
        if fname:
            fullname = self.findFile (fname, 1)
        else:
            fullname = self.findFile ('', 1)
        if not ospath.exists (fullname):
            self.open (fname, 'w')
        return fullname

    def mkdir (self, dirname):
        dir = pathjoin (self.path [0], dirname)
        if not ospath.exists (dir):
            os.makedirs (dir)

    def stat (self, name):
        name = self.findFile (name, 1)
        result = os.stat (name)
        return result

    def subdir (self, name):
        newpath = []
        for dir in self.path:
            newname = pathjoin (dir, name)
            newname = normpath (newname)
            newpath.append (newname)
        return VmakeFS (newpath)

    def search (self, *files):
        for file in files:
            if self.exists (file):
                return file
        return None

    def file (self, filename = None):
        if filename == None:
            return File ('', self)
        filename = abspath (filename)
        for dir in self.path:
            relative = makeRelative (filename, dir)
            if relative != None:
                relative = replace (relative, '\\', '/')
                return File (relative, self)
        raise 'VmakePathError', (filename + ' not in ' + self.path [0])

    def shellName (self, fname):
        return self.findFile (fname)

    class VmakePathError:
        def __init__ (self, msg):
            self.msg = msg

        def __str__ (self):
            return 'VMakePathError: ' + msg

class File:
    #
    # core functionality
    #
    def __init__ (self, name, filesystem = FileSystem ()):
        try:
            if name [0] == '/':
                name = name [1:]
        except IndexError:
            pass
        self.name = name
        self.filesystem = filesystem

    def __str__ (self):
        return '/' + self.name

    def __repr__ (self):
        return ('<File ' + self.filesystem.__class__.__name__
            + ' /' + self.name + '>')

    #
    # analysing the filename
    #
    def directory (self):
        return File (dirname (self.name), self.filesystem)

    def shellName (self):
        return self.filesystem.shellName (self.name)

    def dirname (self):
        return '/' + dirname (self.name)

    def basename (self):
        return basename (self.name)

    def extension (self):
        return splitext (self.name) [1]

    def dirSplit (self):
        dir, base = dirsplit (self.name)
        return dir, base

    def relativeTo (self, dir):
        dirPath = filter (None, split (dir.name, '/'))
        selfPath = filter (None, split (self.name, '/'))
        while (len (dirPath) > 0) and (len (selfPath) > 0) and (dirPath [0] == selfPath [0]):
            del dirPath [0]
            del selfPath [0]
        if len (dirPath) > 0:
            upwards = join (['..'] * len (dirPath), '/') + '/'
        else:
            upwards = ''
        downwards = join (selfPath, '/')
        result = upwards + downwards
        return result

    def relativeToX (self, otherFile):
        selfDir, selfBase = self.dirSplit ()
        otherDir = otherFile.directory ()
        relative = selfDir.relativeTo (otherDir)
        return relative

    #
    # getting information
    #
    def exists (self):
        return self.filesystem.exists (self.name)

    def isdir (self):
        return self.filesystem.isdir (self.name)

    def stat (self):
        return self.filesystem.stat (self.name)

    def getmtime (self):
        return self.filesystem.getmtime (self.name)

    def isNewerThan (self, otherFile):
        try:
            selfChanged = self.getmtime ()
        except os.error:
            return None
        try:
            otherChanged = otherFile.getmtime ()
        except os.error:
            return 1
        return selfChanged > otherChanged

    def listdir (self):
        return self.filesystem.listdir (self.name)

    #
    # creating new files
    #
    def mapTo (self, filesystem):
        return File (self.name, filesystem)

    def join (self, remainder):
        if remainder [0] == '/':
            newname = remainder [1:]
        else:
            oldname = self.name
            if not oldname:
                newname = remainder
            elif oldname [-1] == '/':
                newname = oldname + remainder
            else:
                newname = self.name + '/' + remainder
        return File (newname, self.filesystem)

    def setExtension (self, ext):
        name = self.name
        pointPos = rfind (name, '.')
        if pointPos == -1:
            newName = name + ext
        else:
            newName = name [:pointPos] + ext
        return File (newName, self.filesystem)

    def mkdir (self):
        # TODO: check if directory exists
        try:
            self.filesystem.mkdir (self.name)
        except:
            pass

    #
    # getting file content
    #
    def open (self, mode = 'r'):
        return self.filesystem.open (self.name, mode)

    def data (self):
        return self.open ('rb').read ()

    def text (self):
        return self.open ('rt').read ()

def startsWith (all, prefix):
    preflen = len (prefix)
    if all [:preflen] == prefix:
        result = 1
    else:
        result = None
    return result

def makeRelative (all, prefix):
    preflen = len (prefix)
    firstPart = all [:preflen]
    if normcase (firstPart) == normcase (prefix):
        result = all [preflen:]
    else:
        result = None
    return result

