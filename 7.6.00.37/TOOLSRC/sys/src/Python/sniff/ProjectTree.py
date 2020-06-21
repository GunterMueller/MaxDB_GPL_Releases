# ProjectTree.py
#
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

"""Manage a complete Sniff project tree
"""

import string
import re
import os
joinpath = os.path.join
normpath = os.path.normpath
normcase = os.path.normcase
isdir = os.path.isdir
exists = os.path.exists

import fileutil
import ProjectFile
import FileTypes

own = os.environ ['OWN']
sysDir = joinpath (own, 'sys')
src = os.environ ['SRC']
desc = os.environ ['DESC']
docDir = joinpath (own, 'sys', 'doc')

excludeDirs = [r'src\\sap', r'src\\redist', r'src\\vb',
    r'src\\jtest', r'src\\jtdev', r'src\\jt', r'src\\jv',
    r'src\\Interfaces', r'src\\co\\sap',
    r'src\\SAPDB\\SAPDBCommon\\STL',
    r'src\\SAPDB\\SAPDBCommon\\Standard_Template_Library',
    r'desc\\SAPDB\\SAPDBCommon\\STL',
    r'src\\SAPDB\\Interfaces', 'r\\[tT]est',
    r'src\\SAPDB\\SAPDBCommon\\Memory_Management',
    r'\\[.]sniffdir']

_defaultExcludes = ['/[.]sniff', '/.ProjectCache', '/.RetrieverIndex',
    '/sniff_data']

def stripComment (line):
    pos = string.find (line, '#')
    if pos != -1:
        line = line [:pos]
    if line [:len ('-noDirRE')] == '-noDirRE':
        line = line [len ('-noDirRE') + 1:]
    line = string.strip (line)
    return line

def doubleback (line):
    return string.replace (line, '\\', '\\\\')

def loadExcludeDirs ():
    fname = os.path.join (sysDir, 'config', 'noSniff.dta')
    if exists (fname):
        lines = open (fname, 'r').readlines ()
        lines = filter (None, map (stripComment, lines))
    else:
        lines = []
    lines.extend (_defaultExcludes)
    result = map (normpath, lines)
    if os.sep == '\\':
        result = map (doubleback, result)
    return result

def getDirectoryMapping ():
    os.path.join (src, 'DirectoryMapping')
    if exists (os.path.join (src, 'DirectoryMapping')):
        lines = open (os.path.join (src, 'DirectoryMapping')).readlines ()
    else:
        lines = open (os.path.join (desc, 'DirectoryMapping')).readlines ()
    result = {}
    for line in lines:
        commentpos = string.find (line, '#')
        if commentpos != -1:
            line = line [:commentpos]
        line = string.strip (line)
        if not line:
            continue
        short, dirname = string.split (line, ',')
        short = string.strip (short)
        dirname = string.strip (dirname)
        fullpath = normcase (normpath (joinpath (src, 'SAPDB', dirname)))
        result [fullpath] = short
        fullpath = normcase (normpath (joinpath (desc, 'SAPDB', dirname)))
        result [fullpath] = short +'Desc'
        fullpath = normcase (normpath (joinpath (docDir, 'SAPDB', dirname)))
        result [fullpath] = short +'Doc'
    return result

def makeFileString (sourceFileEntry):
    return projectFileTemplate % sourceFileEntry

def sniffNormpath (pathname):
    return string.replace (pathname, '\\', '/')

def sineOwn (dirname):
    return dirname [len (own) + 1:]

def sineSys (dirname):
    return dirname [len (sysDir) + 1:]

class SniffProjectTree:
    def __init__ (self):
        self.filesAdded = []
        self.filesUnchanged = []
        self.fileTypes = FileTypes.allFileTypes

    def getShortName (self, dirname):
        try:
            short = self.mapping [normcase (dirname)]
        except KeyError:
            short = os.path.basename (dirname)
        return short

    def updateTree (self, dirs, options):
        mapping = self.mapping = getDirectoryMapping ()
        for dirname in fileutil.DepthFirstDirs (dirs, loadExcludeDirs ()):
            if options.verbose:
                print dirname
            short = self.getShortName (dirname)
            self.addDir (dirname, short, options)

    createTree = updateTree

    def addDir (self, dirname, short, options):
        localProjectFile = short + '.proj'
        projectFileName = joinpath (dirname, localProjectFile)
        projectFile = ProjectFile.ProjectFile (projectFileName, options.forceNew)
        self.filesAdded.append (projectFileName)
        sniffDir = sniffNormpath (sineSys (dirname))
        subProjectDirs, sourceFiles = self.teeFiles (dirname)
        sourceFiles = self.filterSourceFiles (sourceFiles, sniffDir)
        #if not self.listIncludesLocalProjectFile (localProjectFile, sourceFiles):
        #    sourceFiles.append ((localProjectFile, "Project"))
        self.writeProjectFile (projectFile, projectFileName, sourceFiles, subProjectDirs)

    def teeFiles (self, dirname):
        """split file listing into directories and regular files
        """
        dirs = []
        files = []
        for fname in os.listdir (dirname):
            fullName = joinpath (dirname, fname)
            if isdir (fullName):
                dirs.append (fullName)
            else:
                files.append (fname)
        return dirs, files

    def filterSourceFiles (self, sourceFiles, sniffDir):
        result = []
        for sourceFile in sourceFiles:
            fileType = self.fileTypes.inferFileType (sourceFile)
            if fileType:
                result.append ((sniffDir + '/' + sourceFile, fileType))
        return result

    def listIncludesLocalProjectFile (self, localProjectFile, sourceFiles):
        for sourceFile, fileType in sourceFiles:
            if localProjectFile == sourceFile:
                return 1
        return None


    def writeProjectFile (self, projectFile, projectFileName, sourceFiles, subProjectDirs):
        BuildDir = sniffNormpath (sineSys (os.path.dirname (projectFileName)))
        DisplayName = os.path.splitext (os.path.basename (projectFileName)) [0]
        FileCount = len (sourceFiles)
        #
        # source list
        #
        lines = map (makeFileString, sourceFiles)
        if len (lines) > 0:
            fileSet = "\n" + string.join (lines, ",\n") + "\n        "
        else:
            fileSet = ''
        FileSet = ' array (%s)' % fileSet
        #
        #
        #
        FileTypes = self.fileTypes.getFileTypesString ()
        FileTypesOrder = self.fileTypes.getFileTypesOrderString ()
        #
        #
        #
        GenerateDir = 'sniff' + '/' + BuildDir + '/' + '.sniff'
        #
        # sub project list
        #
        subProjectEntries = []
        for subProjectDir in subProjectDirs:
            subProjectFile = self.getShortName (subProjectDir) + '.proj'
            if exists (joinpath(subProjectDir, subProjectFile)):
                subProjectDir = sniffNormpath (sineSys (subProjectDir))
                # fileCount = fileCount + self.genSubProject (subProjectDir, subProjectFile)
                newEntry = subProjectTemplate % locals ()
                subProjectEntries.append (newEntry)
        if len (subProjectEntries) > 0:
            subProjects = "\n" + string.join (subProjectEntries, ",\n") + "\n        "
        else:
            subProjects = ''
        SubProj = ' array (%s)' % subProjects
        #
        # putting it all together
        #
        isWritten = projectFile.writeData (locals ())
        if not isWritten:
            self.filesUnchanged .append (projectFile.fname)
        return FileCount + 1   # for project file

    def submitChanges (self):
        stream = open ('newfiles.out', 'wt')
        for file in self.filesAdded:
            stream.write (file + '\n')
        stream.close ()

projectFileTemplate = '''		struct (
			"FileName" : "%s",
			"FileType" : "%s"
		)'''

subProjectTemplate = '''		SubProjSchema (
			SubProjDir  : "%(subProjectDir)s",
			SubProjName : "%(subProjectFile)s"
		)'''

subProjectTemplate = '        "%(subProjectDir)s/%(subProjectFile)s"'

