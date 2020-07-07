
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
import cPickle
import os
import urllib

from html import *

_cacheVersion = 0.1

def normpath (path):
    path = os.path.normpath (path)
    if os.sep != '/':
        path = string.replace (path, os.sep, '/')
    return path

class VdnWeaver:
    def __init__ (self, packageName = 'cppDoc', homedir = '.', rootdir = None):
        self.packageName = packageName
        homedir = normpath (homedir)
        self.homedir = homedir
        if not rootdir:
            rootdir = homedir
        else:
            rootdir = os.path.normpath (rootdir)
        self.relativePackage = relativePathList (rootdir, homedir)
        cacheFile = self.namePackageFile ('wdx')
        keywords = None
        if os.path.exists (cacheFile):
            version, rest = cPickle.load (open (cacheFile, 'rb'))
            if version == _cacheVersion:
                keywords = rest
        if keywords == None:
            keywords = []
        self.keywords = keywords

    def namePackageFile (self, extension):
        if self.homedir  == '.':
            filename = self.packageName + '.' + extension
        else:
            filename = os.path.join (self.homedir, self.packageName + '.' + extension)
        return filename

    def nameExtendedPackageFile (self, namepart, extension):
        if self.homedir  == '.':
            filename = self.packageName + namepart + '.' + extension
        else:
            filename = os.path.join (self.homedir, self.packageName + namepart + '.' + extension)
        return filename

    def save (self):
        cacheFile = self.namePackageFile ('wdx')
        dumpStream = open (cacheFile, 'wb')
        cPickle.dump ((_cacheVersion, self.keywords), dumpStream, 1)

    layerRE = re.compile ('[hg]([a-z][a-z])[0-9]+')

    def detectFileDir (self, filename):
        match = self.layerRE.match (filename)
        if match:
            result = match.group (1)
        else:
            result = '.'
        return result

    def notCurrentFile (self, entry):
        result = (entry [2].file != self.fileForLinks)
        return result

    def setCurrentFile (self, filename):
        if os.sep != '/':
            filename = string.replace (filename, os.sep, '/')
        self.currentFile = filename
        fileDir = self.detectFileDir (filename)
        self.pathList = string.split (fileDir, '/')
        self.fileForLinks = fileDir + '/' + filename
        self.keywords = filter (self.notCurrentFile, self.keywords)

    def removeCurrentEntries (self):
        self.keywords = filter (self.notCurrentFile, self.keywords)

    def sourceRelative (self, filename):
        path, base = os.path.split (filename)
        filePathList = string.split (path, '/')
        currentPathList = self.pathList [:]
        while filePathList and currentPathList and (filePathList [0] == currentPathList [0]):
            del filePathList [0]
            del currentPathList [0]
        relativeList = (['..'] * len (currentPathList)) + filePathList
        result = string.join (relativeList + [base], '/')
        return result

    def indexRelative (self, filename):
        filedir, basename = os.path.split (filename)
        relative = relativePathList (self.relativePackage [:], filedir)
        result = string.join (relative + [basename], '/')
        return result

    def fixLink (self, targetFile, targetAnchor = None, allowLocal = 1):
        targetDir = self.detectFileDir (targetFile)
        if allowLocal:
            if targetFile == self.currentFile:
                result = ''
            else:
                result = self.sourceRelative (targetFile)
        else:
            result = self.sourceRelative (targetFile)
        if targetAnchor:
            result = result + '#' + targetAnchor
        return result

    def getLink (self, file, anchor, text, defaultText = None):
        if not text:
            text = defaultText
        href = self.fixLink (file, anchor)
        return '<A HREF="%s">%s</A>' % (href, text)

    def getClassLink (self, classname, text = None):
        classSource = splitVdnName (classname) [1]
        if classSource:
            result = self.getLink (classSource, classname, text, classname)
        else:
            if text == None:
                result = classname
            else:
                result = text
        return result

    def getMethodLink (self, methodname, classname, text = None):
        anchor = classname + '::' + methodname
        result = self.getLink (splitVdnName (classname) [1],
            urllib.quote (anchor, ''), text, anchor)
        return result

    def getRoutineLink (self, routinename, text = None):
        result = self.getLink (splitVdnName (routinename) [1],
            routinename, text, routinename)
        return result

    def addEntry (self, rawname, entry):
        sortname = string.lower (rawname)
        self.keywords.append ((sortname, rawname, entry))

    def addClass (self, clazz, description = None):
        entry = IndexEntry (clazz, _clazz, None, self.fileForLinks,
            clazz, description)
        rawname = splitVdnName (clazz) [0]
        self.addEntry (rawname, entry)

    def addMethod (self, method, clazz, description = None):
        if method == clazz:
            kind = _constructor
        elif method == '~' + clazz:
            kind = _destructor
        else:
            kind = _method
        entry = IndexEntry (method, kind, clazz, self.fileForLinks,
            clazz + '::' + method, description)
        rawname = splitVdnName (method) [0]
        self.addEntry (rawname, entry)

    def addRoutine (self, routine, description = None):
        entry = IndexEntry (routine, _function, None, self.fileForLinks,
            routine, description)
        rawname = splitVdnName (routine) [0]
        self.addEntry (rawname, entry)

    def addAnything (self, name, description = None):
        entry = IndexEntry (name, _other, None, self.fileForLinks,
            None, description)
        self.addEntry (name, entry)

    def genIndex (self):
        htmlstream = HTMLFile (self.nameExtendedPackageFile('Index', 'html'),
            'Index ' + self.packageName)
        htmlstream.titleHeader (self.packageName + ' Index')
        self.keywords.sort ()
        htmlstream.pushTag ('dl')
        write = htmlstream.write
        for sortname, rawname, entry in self.keywords:
            htmlstream.tag ('dt')
            link = self.indexRelative (entry.file)
            if entry.anchor:
                link = link + '#' + entry.anchor
            write (href (link, entry.name) + ' (' + entry.kind)
            if entry.context:
                write (' in ' + entry.context)
            write (')\n')
            # put description
        htmlstream.popTag ()
        htmlstream.close ()

_clazz = 'class'
_method = 'method'
_constructor = 'constructor'
_destructor = 'destructor'
_function = 'function'
_other = 'topic'

class IndexEntry:
    def __init__ (self, name, kind, context, file, anchor, description):
        self.name = name
        self.kind = kind
        self.context = context
        self.file = file
        self.anchor = anchor
        self.description = description

    def href (self):
        result = self.file
        if self.anchor:
            result = result + '#' + self.anchor
        return result

    def __repr__ (self):
        return "<IndexEntr %s '%s'>" % (self.name, self.href ())

_globalIdentRE = re.compile ('[~]?[a-z]([a-z]+)([0-9]+)_?(.*)$')

def splitVdnName (name):
    match = _globalIdentRE.match (name)
    if match:
        layer, number, rawname = match.group (1, 2, 3)
        filename = getFilename (layer, number)
    else:
        filename = ''
        rawname = name
    return rawname, filename

def getFilename (layer, number):
    return layer + '/h' + layer + number + '.html'

_stringType = type ('')

def relativePathList (sourcePath, targetPath):
    if type (sourcePath) == _stringType:
        sourcePath = string.split (sourcePath, '/')
    if type (targetPath) == _stringType:
        targetPath = string.split (targetPath, '/')
    while sourcePath and targetPath and (sourcePath [0] == targetPath [0]):
        del sourcePath [0]
        del targetPath [0]
    relativePath = (['..'] * len (sourcePath)) + targetPath
    return relativePath

