
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
import re
import cStringIO

true = 'true'

class FileType:
    """models a Sniff+ FileTypes struct
    """
    fieldOrder = ["BuildTool", "DerivedFrom", "DerivedFromInfo",
        "Language", "Signatures", "SubType", "Type"]
    stringAttributes = ["BuildTool",
        "Language", "Signatures", "SubType", "Type"]
    requiredAttributes = ["Signatures", "Type", "SubType"]
    BuildTool = ""
    DerivedFrom = None
    DerivedFromInfo = None
    Language = ""
    Signatures = None
    SubType = None
    Type = None
    def __init__ (self, name, **attributes):
        """creates a new FileType
        """
        self.checkConstructorArgs (name, attributes)
        self.name = name
        for attrName, attrValue in attributes.items ():
            setattr (self, attrName, attrValue)
        self.buildRex ()

    def checkConstructorArgs (self, name, dict):
        for attrName in self.requiredAttributes:
            if not dict.has_key (attrName):
                raise AttributeMissing (name, attrName)

    def buildRex (self):
        entries = string.split (self.Signatures, ':')
        rexList = self.rexList = []
        for entry in entries:
            if entry [:2] == '*.':
                entry = '.*[.]' + entry [2:]
            else:
                string.replace (entry, '*', '.*')
            rexList.append (re.compile (entry + '$'))


    def writeToProjectFile (self, stream, indent = ''):
        """writes a FileType description t o a Sniff+ project file
        """
        stream.write (indent + '"' + self.name + '" : struct (\n')
        for attrName in self.fieldOrder:
            value = getattr (self, attrName)
            if value is not None:
                stream.write (indent + '	"%s" : ' % attrName)
                if type (value) == type (''):
                    stream.write ('"' + value + '"')
                else:
                    value.writeToProjectFile (stream, indent + '	')
                if attrName == self.fieldOrder [-1]:
                    delimiter = ''
                else:
                    delimiter = ','
                stream.write (delimiter + '\n')
        stream.write (indent + '),\n')

    def matches (self, fname):
        for rex in self.rexList:
            if rex.match (fname):
                return true
        return None

class FileTypesOrder:
    """Contains all the known filetypes.

    This is also used to infer the file type of a file name.
    """
    fileTypesStringCache = None
    fileTypesOrderStringCache = None
    def __init__ (self, *fileTypes):
        """creates a new FileTypeOrder
        """
        self.fileTypes = fileTypes

    def writeToProjectFile (self, stream, indent = ''):
        """writes the struct FileTypes and the array FileTypesOrder
        to a Sniff+ project file.
        """
        #
        #
        #
        stream.write (indent + '"FileTypes" : struct (\n')
        for fileType in self.fileTypes:
            fileType.writeToProjectFile (stream, indent + '    ')
        stream.write (indent + '),\n')
        #
        #
        #
        stream.write (indent + '"FileTypesOrder" : array (\n')
        for fileType in self.fileTypes:
            stream.write (indent + '    "%s",\n' % fileType.name)
        stream.write (indent + '),\n')

    def getFileTypesString (self):
        if not self.fileTypesStringCache:
            stream = cStringIO.StringIO ()
            stream.write ('struct (\n')
            for fileType in self.fileTypes:
                fileType.writeToProjectFile (stream, '		')
            stream.write ('	)')
            self.fileTypesStringCache = stream.getvalue ()
        return self.fileTypesStringCache

    def getFileTypesOrderString (self):
        if not self.fileTypesOrderStringCache:
            stream = cStringIO.StringIO ()
            stream.write ('array (\n')
            for fileType in self.fileTypes:
                stream.write ('		"%s",\n' % fileType.name)
            stream.write ('	)')
            self.fileTypesOrderStringCache = stream.getvalue ()
        return self.fileTypesOrderStringCache

    def inferFileType (self, fname):
        for fileType in self.fileTypes:
            if fileType.matches (fname):
                return fileType.name
        return None


class DerivedFrom:
    def writeToProjectFile (self, stream, indent = ''):
        stream.write ('array (\n' + indent + ')')

class DerivedFromInfo:
    def writeToProjectFile (self, stream, indent = ''):
        stream.write ('struct (\n')
        stream.write (indent + '    "DerivedLoose" : 0,\n')
        stream.write (indent + '    "DerivedType" : 0,\n')
        stream.write (indent + '    "GenerateDir" : "",\n')
        stream.write (indent + '    "ViewSpecific" : 0\n')
        stream.write (indent + ')')

#
# exceptions
#
class AttributeMissing:
    def __init__ (self, objectname, attributeName):
        self.objectname = objectname
        self.attributeName = attributeName

    def __str__ (self):
        return self.attributeName + ' for ' + self.objectname

#
# Data
#

asciiDocumentation = FileType ("ASCII-Documentation",
	BuildTool = "",
	Language = "",
	Signatures = "*.txt:README*:readme*:Readme*:ReadMe*",
	SubType = "ASCII",
	Type = "Documentation"
)
assemblerImplementation = FileType ("Assembler Implementation",
	BuildTool = "Assembler",
	Language = "GNUAssembler",
	Signatures = "*.asm:*.s:*.Asm",
	SubType = "Assembler-Type",
	Type = "Implementation"
)
cppImplementation = FileType ("C++ Implementation",
	BuildTool = "C++ Compiler",
	Language = "ANSI C/C++",
	Signatures = "*.c:*.C:*.cc:*.CC:*.cpp:*.CPP:*.cxx:*.CXX",
	SubType = "C++-Type",
	Type = "Implementation"
)
debugOutput = FileType ("Debug Output",
	BuildTool = "",
	Language = "",
	Signatures = "*.log:*.pfidbg:*.dbg",
	SubType = "Debug Output",
	Type = "Other"
)
diabLinker = FileType ("Diab Linker Command File",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "*.dld",
	SubType = "Diab Files",
	Type = "Other"
)
htmlDocumentation = FileType ("HTML-Documentation",
	BuildTool = "",
	Language = "HTML",
	Signatures = "*.htm:*.html",
	SubType = "HTML",
	Type = "Documentation"
)
cHeader = FileType ("Header",
	BuildTool = "",
	Language = "ANSI C/C++",
	Signatures = "*.h:*.H:*.hh:*.HH:*.hpp:*.HPP:*.hxx:*.HXX:*.ci",
	SubType = "Interface",
	Type = "Interface"
)
idlInterface = FileType ("IDL-Interface",
	BuildTool = "IDL Compiler",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "*.idl",
	SubType = "IDL-Type",
	Type = "Implementation"
)
image = FileType ("Image",
	BuildTool = "",
	Language = "",
	Signatures = "*.png:*.gif:*.image:*.im:*.jpg:*.jpeg:*.bmp:*.ico",
	SubType = "Image",
	Type = "Other"
)
javaSource = FileType ("Java",
	BuildTool = "SniffJMake",
	Language = "Java",
	Signatures = "*.java",
	SubType = "Java-Type",
	Type = "Implementation"
)
lexSource = FileType ("Lex Source",
	BuildTool = "Lexer",
	Language = "",
	Signatures = "*.l",
	SubType = "CodeGenerating-Type",
	Type = "Implementation"
)
makeFile = FileType ("Make",
	BuildTool = "",
	Language = "Make",
	Signatures = "Makefile:makefile:*.mk:*.mak:*.MAK:MAKEFILE:Makefile*:makefile*",
	SubType = "Make",
	Type = "Make"
)
perlSource = FileType ("Perl",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "Perl",
	Signatures = "*.pl:*.Pl:*.pm",
	SubType = "Perl-Type",
	Type = "Implementation"
)
projectFile = FileType ("Project",
	BuildTool = "",
	Language = "",
	Signatures = "*.prj:*.proj",
	SubType = "Project",
	Type = "Project"
)
pythonSource = FileType ("Python",
	BuildTool = "",
	Language = "Python",
	Signatures = "*.py:*.pyw",
	SubType = "Python-Type",
	Type = "Script"
)
sapdbConfig = FileType ("SAPDB Configuration",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "*.pcf",
	SubType = "CodeGenerating-Type",
	Type = "Implementation"
)
sapdbDescription = FileType ("SAPDB Description",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "*.mac:*.shm:*.lnk:*.com:*.lib:*.dld:*.rel:*.jpr",
	SubType = "Make",
	Type = "Make"
)
sapdbMessage = FileType ("SAPDB Message",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "*.msg:*.genmsg:*.de:*.en",
	SubType = "",
	Type = "Implementation"
)
pascalSource = FileType ("SAPDB Pascal",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "[gv][a-z][a-z][0-9][0-9]*",
	SubType = "",
	Type = "Implementation"
)
sapdbPrecompiler = FileType ("SAPDB Precompiler",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "ANSI C/C++",
	Signatures = "*.cpc:*.cpo:*.oci:*.OC",
	SubType = "C-Type",
	Type = "Implementation"
)
sapdbResource = FileType ("SAPDB Resource",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "*.rc:*.def",
	SubType = "CodeGenerating-Type",
	Type = "Implementation"
)
sutProtocol = FileType ("SAPDB SUT Protocol",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "*.punix:*.tpunx:*.upnix:*.uunix:*.cunix:*.cpnix",
	SubType = "",
	Type = "Other"
)
sutSource = FileType ("SAPDB SUT Source",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "",
	Signatures = "*.vdnts:*.cmd:*.diffscr",
	SubType = "",
	Type = "Other"
)
sqlSource = FileType ("SQL",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	DerivedFromInfo = DerivedFromInfo (),
	Language = "SQL",
	Signatures = "*.sql:*.SQL:*.trg:*.Trg",
	SubType = "SQL-Type",
	Type = "Implementation"
)
shellSource = FileType ("ShellScript",
	BuildTool = "",
	Language = "ShellScript",
	Signatures = "*.sh:*.csh",
	SubType = "ShellScript-Type",
	Type = "Script"
)
xmlFile = FileType ("XML",
	BuildTool = "",
	DerivedFrom = DerivedFrom (),
	    DerivedFromInfo = DerivedFromInfo (),
	Language = "XML",
	Signatures = "*.xml:*.xsd",
	SubType = "XML-Type",
	Type = "Script"
)
yaccSource = FileType ("Yacc Source",
	BuildTool = "Parser Generator",
	Language = "",
	Signatures = "*.y",
	SubType = "CodeGenerating-Type",
	Type = "Implementation"
)


allFileTypes = FileTypesOrder (
    asciiDocumentation,
    assemblerImplementation,
    cppImplementation,
    debugOutput,
    diabLinker,
    htmlDocumentation,
    cHeader,
    idlInterface,
    image,
    javaSource,
    lexSource,
    makeFile,
    perlSource,
    projectFile,
    sapdbConfig,
    sapdbDescription,
    sapdbMessage,
    sapdbPrecompiler,
    sapdbResource,
    sutProtocol,
    sutSource,
    sqlSource,
    shellSource,
    xmlFile,
    yaccSource,
    pascalSource)

if __name__ == "__main__":
    import sys
    allFileTypes.writeToProjectFile (sys.stdout)

