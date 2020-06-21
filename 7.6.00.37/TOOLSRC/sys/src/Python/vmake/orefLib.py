
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
import os
pathjoin = os.path.join
isdir = os.path.isdir
import re
import tempfile

if os.name == 'nt':
    class nmParser:
        tmpfile = tempfile.mktemp ()
        def __init__ (self, path = None):
            if path:
                self.dumpbin = '"' + path + '\\dumpbin"'
            else:
                self.dumpbin = 'dumpbin'
            self.pascalfiles = {}

        def readPipe (self, cmd):
            fullCmd = cmd % self.tmpfile
            rc = os.system (fullCmd + " > nul:")
            data = open (self.tmpfile, 'rt').read ()
            try:
                os.remove (self.tmpfile)
            except:
                pass
            return data

        def cleanName (self, functionName):
            if functionName [:6] == '__imp_':
                functionName = functionName [6:]
            if functionName [0] == '_':
                functionName = functionName [1:]
            elif functionName [0] == '?':
                functionName = self.unmangle (functionName)
            return functionName


        def parseAll (self, argString):
            imports = []
            exports = []
            currentFile = 'unnown'
            data = self.readPipe (self.dumpbin + ' /OUT:%s /SYMBOLS ' + argString)
            #fileRE = re.compile ('^Dump of file (.*)')
            fileRE = re.compile ('[0-9A-F]{3} [0-9A-F]{8} DEBUG  notype       Filename')
            importRE = re.compile ('[0-9A-F]{3} [0-9A-F]{8} UNDEF.*[|] (.*)')
            dllImportRE = re.compile ('(.*)@[0-9]+$')
            exportRE = re.compile (r'SECT.*External\s+[|] (.*)')
            lines = string.split (data, '\n')
            inMainFile = None
            mainFile = None
            filenamePending = None
            for line in lines:
                # print line
                if filenamePending:
                    fullname = string.strip (line)
                    currentFile = os.path.basename (fullname)
                    filenamePending = None
                    if fullname == currentFile:
                        mainFile = currentFile
                    continue
                importMatch = importRE.match (line)
                if importMatch:
                    functionName = importMatch.group (1)
                    match = dllImportRE.match (functionName)
                    if match:
                        functionName = match.group (1)
                    functionName = self.cleanName (functionName)
                    if functionName [:5] == 'sql__':
                        self.pascalfiles [mainFile] = mainFile [:-2]
                    imports.append ((currentFile, functionName))
                else:
                    fileMatch = fileRE.match (line)
                    if fileMatch:
                        filenamePending = 1
                        #fullname = fileMatch.group (1)
                        #currentFile = os.path.basename (fullname)
                    else:
                        exportMatch = exportRE.search (line)
                        if exportMatch:
                            functionName = exportMatch.group (1)
                            functionName = self.cleanName (functionName)
                            exports.append ((currentFile, functionName))
                            if mainFile != currentFile:
                                imports.append ((mainFile, functionName))
            return imports, exports

        def parse (self, argString):
            imports, exports = self.parseAll (argString)
            return imports


        identifier = '[a-z][a-z0-9_]*'
        className = identifier + '(?:<[^>]+>)?'
        nameRE = re.compile ('(' + className + '::)?' + identifier + '[(]|$', re.I)
        destructorRE = re.compile ('(' + className + ')::`(scalar|vector) deleting destructor\'', re.I)

        def unmangle (self, functionName):
            startPos = string.find (functionName, '(')
            if startPos == -1:
                print 'start not found in', functionName
                return functionName
            if functionName [-1] == ')':
                endPos = len (functionName) -1
            else:
                endPos = string.find (functionName, ')\n')
            if endPos == -1:
                print 'end not found in', functionName [startPos:]
                return functionName
            fullUnmangled = functionName [startPos + 1:endPos]
            # test for mangled name
            match = self.nameRE.search (fullUnmangled)
            if match:
                FROM, TO = match.span (0)
                return fullUnmangled [FROM:TO-1]
            else:
                # test for destructor
                match = self.destructorRE.search (fullUnmangled)
                if match:
                    className, kind = match.group (1, 2)
                    result = className + '::~'
                    if kind == 'vector':
                        result = result + '[]'
                    return result
                else:
                    return fullUnmangled

elif os.name == 'posix':
    class nmParser:
        def __init__ (self, path = None):
            pass

        def readPipe (self, cmd):
            return os.popen (cmd, 'r').read ()

        def parseAll (self, argString):
            imports = []
            exports = []
            currentFile = 'unnown'
            # expect GNU nm
            nmCmd = 'nm --demangle --extern-only -f posix -p --print-armap '
            data = self.readPipe (nmCmd + argString)
            lines = string.split (data, '\n')
            isTemplateInstance = None
            for line in lines:
                line = string.strip (line)
                if not line:
                    continue
                if line [-1] == ':':
                    fullname = line [:-1]
                    isTemplateInstance = '<' in fullname
                    currentFile = os.path.basename (fullname)
                    continue
                if isTemplateInstance:
                    continue
                entries = string.split (line)
                symbol = entries [0]
                if len (entries) == 1:
                    continue
                kind = entries [1]
                if len (entries) > 2:
                    value = entries [2]
                else:
                    value = '0'
                if kind in ['U', 'C']:
                    imports.append ((currentFile, symbol))
                elif kind == 'T':
                    exports.append ((currentFile, symbol))
            return imports, exports

        def parse (self, argString):
            imports, exports = self.parseAll (argString)
            return imports

else:
    raise 'no nm class defined', os.name

class AbstractOref:
    def __init__ (self, path = None):
        self.parser = nmParser (path)

    def add (self, fname):
        if isdir (fname):
            self.addDir (fname)
        else:
            self.addFile (fname)

    def addDir (self, dir):
        if self.verbose:
            print 'adding', dir
        self.add (pathjoin (dir, '*.o'))
        files = os.listdir (dir)
        for file in files:
            fullpath = pathjoin (dir, file)
            if isdir (fullpath):
                self.addDir (fullpath)


class ORef (AbstractOref):
    def __init__ (self, verbose, path):
        AbstractOref.__init__ (self)
        self.verbose = verbose
        self.functions = Bag ()
        self.exports = Bag ()

    def addFile (self, fname):
        imports, exports = self.parser.parseAll (fname)
        add = self.functions.add
        for object, importName in imports:
            if importName:
                add (importName, object)
        add = self.exports.add
        for object, exportName in exports:
            if exportName:
                add (exportName, object)

    def items (self):
        return self.functions.items ()

    def exportsOnly (self):
        return result

    def format (self, stream):
        stream.write ("""
Object XRef
===========

[fname]: module exports this symbol
(I):     Import, symbol is not exported by any object
(?):     Questionable, symbol is not imported by any object

symbol                             object files
========================================================================
""")
        for low, name, objects in self.items ():
            exportingObjects = self.exports.get (name, [])
            self.writeEntry (stream, name, objects, exportingObjects)
        for low, name, exportingObjects in self.exports.items ():
            if not self.functions.has_key (name):
                self.writeEntry (stream, name, [], exportingObjects)

    def writeEntry (self, stream, name, objects, exportingObjects = []):
        maxNameLen = 35
        if not exportingObjects:
            linkTag = ' (I)'
        elif not objects:
            linkTag = ' (?)'
        else:
            linkTag = ''
        line = name + linkTag + ' ' \
            + ('.' * (maxNameLen - 2 - len (name) - len (linkTag))) + ' '
        if len (line) > maxNameLen:
            stream.write (line + '\n')
            line = line = ' ' * maxNameLen
        knownObjects = {}
        pascalfiles = self.parser.pascalfiles
        for object in exportingObjects:
            if object [-6:] == '.rel.o':
                continue
            #object = stripObjectExt (object)
            if knownObjects.has_key (object):
                continue
            knownObjects [object] = 1
            object = pascalfiles.get (object, object)
            object = '[' + object + ']'
            if len (line) + len (object) > 75:
                stream.write (line + '\n')
                line = ' ' * maxNameLen
            line = line + object + ' '
        for object in objects:
            if object [-6:] == '.rel.o':
                continue
            #object = stripObjectExt (object)
            object = pascalfiles.get (object, object)
            if len (object) == 5:
                object = object + ' ' # better alignment
            if len (line) + len (object) > 75:
                stream.write (line + '\n')
                line = ' ' * maxNameLen
            line = line + object + ' '
        stream.write (line + '\n')

def stripObjectExt (object):
    if object [-2:] == '.o':
        object = object [:-2]
    return object

class Bag:
    def __init__ (self):
        self.dict = {}
        self.keys = self.dict.keys
        self.has_key = self.dict.has_key

    def add (self, key, value):
        try:
            list = self.dict [key]
        except KeyError:
            list = []
            self.dict [key] = list
        list.append (value)

    def items (self):
        lower = string.lower
        items = self.dict.items ()
        result = []
        for key, list in items:
            result.append ((lower (key), key, unique (list)))
        result.sort ()
        return result

    def get (self, key, default = None):
        return self.dict.get (key, default)


class OnceQueue:
    def __init__ (self):
        self.pending = []
        self.seen = {}

    def add (self, item):
        if not self.seen.has_key (item):
            self.seen [item] = 1
            self.pending.append (item)

    def addAll (self, list):
        for item in list:
            self.add (item)

    def next (self):
        if len (self.pending) == 0:
            return None
        result = self.pending [0]
        del self.pending [0]
        return result

    def seenItems (self):
        return self.seen.keys ()

class Hull (AbstractOref):
    def __init__ (self, path):
        self.export2exporter = {}
        self.exporter2export = Bag ()
        self.export2importer = {}
        self.importer2imports = Bag ()
        self.addOref (path)

    def addOref (self, path):
        parser = OrefFileParser (path)
        for name, exporters, importers in parser:
            if not exporters:
                continue
            self.export2exporter [name] = exporters
            self.export2importer [name] = importers
            for oname in importers:
                self.importer2imports.add (oname, name)

    def getHull (self, symbols):
        queue = OnceQueue ()
        for symbol in symbols:
            try:
                queue.addAll (self.export2exporter [symbol])
            except KeyError:
                available = self.export2exporter.keys ()
                available.sort ()
                print symbol, 'not in'
                for x in available:
                    print x
        ofile = queue.next ()
        while ofile:
            imports = self.importer2imports.get (ofile, [])
            for symbol in imports:
                queue.addAll (self.export2exporter [symbol])
            ofile = queue.next ()
        result = queue.seenItems ()
        result.sort ()
        return result

def unique (list):
    list.sort ()
    result = []
    last = ''
    for item in list:
        if item != last:
            result.append (item)
            last = item
    return result

class OrefFileParser:
    def __init__ (self, fname):
        self.stream = open (fname, 'r')
        self.currentLine = self.stream.readline ()
        self.skipHeader ()

    def nextLine (self):
        result = self.currentLine
        self.currentLine = self.stream.readline ()
        return result

    def skipHeader (self):
        while self.currentLine [:40] != ('=' * 40):
            self.nextLine ()
        self.nextLine ()

    identifierRE = re.compile ('[^ ]+')
    def next (self):
        line = self.nextLine ()
        if not line:
            raise IndexError
        match = self.identifierRE.match (line)
        name = match.group (0)
        rest = line [35:]
        files = string.split (rest)
        while self.currentLine and self.currentLine [0] == ' ':
            line = self.nextLine ()
            files.extend (string.split (line))
        exporters = []
        importers = []
        for file in files:
            if file [0] == '[':
                assert file [-1] == ']', 'doubious ' + file
                exporters.append (file [1:-1])
            else:
                assert file [-1] != ']', 'doubious ' + file
                importers.append (file)
        return name, exporters, importers

    def __getitem__ (self, index):
        return self.next ()

