
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

class ProjectFileError:
    def __init__ (self, msg):
        self.msg = msg

    def __str__ (self):
        return 'ProjectFile.error: ' + self.msg

error = ProjectFileError

def loadTemplate ():
    fname = 'SAPDB.ptmpl.txt'
    for part in sys.path:
        fullPath = os.path.join (part, 'sniff', fname)
        if os.path.exists (fullPath):
            result = open (fullPath, 'rt').read ()
            return result
    raise ProjectFileError (fname + ' not found')

class ProjectFile:

    template = loadTemplate ()
    propertyRE = re.compile ('\n\t(\w+)\s*:', re.I)
    fileFilter = None

    def __init__ (self, fname, forceNew = None):
        self.fname = fname
        self.loadData (forceNew)

    def loadData (self, forceNew):
        if not forceNew and os.path.exists (self.fname):
            text = open (self.fname, 'rt').read ()
        else:
            text = self.template
        list = self.propertyRE.split (text)
        self.entryText = list [0]
        properties = self.properties = []
        for i in xrange (1, len (list), 2):
            properties.append ([list [i], list [i + 1]])

    def filterValidFiles (self, fileList):
        filterFun = self.getFileFilter ().isValid
        return filter (filterFun, fileList)

    def getFileFilter (self):
        if self.fileFilter == None:
            fileFilter = self.fileFilter = FileFilter ()
            fileTypes = self.getProperty ('FileTypes')
            signatureRex = re.compile (r'Signatures\s*:\s*"([^"]*)"')
            pos = 0
            match = signatureRex.search (fileTypes, pos)
            while match:
                signatures = match.group (1)
                if signatures != "\\":
                    entries = string.split (signatures, ':')
                    for entry in entries:
                        fileFilter.addGlobRE (entry)
                pos = match.end ()
                match = signatureRex.search (fileTypes, pos)
        return self.fileFilter


    def getProperty (self, requestedKey, default = None):
        for key, value in self.properties:
            if key == requestedKey:
                return value
        return default

    def writeData (self, variables):
        if os.path.exists (self.fname):
            currentData = open (self.fname, 'rt').read ()
        else:
            currentData = ''
        newData = self.template % variables
        if newData != currentData:
            stream = open (self.fname, 'wt')
            stream.write (newData)
            stream.close ()
            result = 1
        else:
            result = None
        return result


    def replaceEntry (self, key, data):
        if data [0] != ' ':
            data = ' ' + data
        if data [-1] != ',':
            data = data + ','
        for entry in self.properties:
            if entry [0] == key:
                entry [1] = data
                break

extRE = re.compile ('[*]([.][a-zA-Z0-9]+)')

class FileFilter:
    def __init__ (self):
        self.validExtensions = {}
        self.rex = []

    def addExtension (self, ext):
        self.validExtensions [ext] = 1

    def addRE (self, rex):
        self.rex.append (re.compile (rex))

    def addGlobRE (self, rex):
        match = extRE.match (rex)
        if match:
            # simple extension match
            self.addExtension (match.group (1))
        else:
            # complex glob pattern
            rex = string.replace (rex, '*', '.*')
            self.addRE (rex)

    def isValid (self, fname):
        ext = os.path.splitext (fname) [1]
        if self.validExtensions.has_key (ext):
            return 1
        for rex in self.rex:
            if rex.match (fname):
                return 1
        # print 'ignored', fname
        return None


if __name__ == "__main__":
    #projectFile = ProjectFile ()
    #projectFile.write (sys.stdout)
    projectFile = ProjectFile ('dummy')
    projectFile.replaceEntry ('VCSystem', '"CVS",')
    projectFile.writeData ()

