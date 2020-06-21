
import string
import time
import os

class MappedType:
    def __init__ (self, decl):
        self.decl = decl

    def __repr__ (self):
        return '<%s %s>' % (self.__class__.__name__, self.decl)

    def varDcls (self, name, typeString, default):
        result = "%s %s" % (typeString, name)
        if default:
            if string.strip (default) [0] == '"':
                default = 'LIT(' + default + ')'
            result = result + " = " + default
        return [result]

    #     def varDcls (self, name):
    #         return [(self.decl, name)]
    #
    #     def asParseTupleParameter (self, name):
    #         return '&' + name

    def asCallParameter (self, name):
        return name
    #
    #     def asMakeTupleParameter (self, name):
    #         return name
    #
    #     def py2c (self, name):
    #         return None
    #
    #     def c2py (self, name):
    #         return None

    def writeCleanup (self, method):
        pass

class GluedType:
    def __init__ (self, gluename):
        self.decl = gluename
        # self.came = cname
        if gluename [-2:] == ' *':
            gluename = gluename [:-2]
        self.gluename = gluename

    def varDcls (self, name, typeString, default):
        result = "%sObjectT * %s" % (self.gluename, name)
        if default:
            if string.strip (default) [0] == '"':
                default = 'LIT(' + default + ')'
            result = result + " = " + default
        return [result]

    def asParseTupleParameter (self, name):
          return '&' + name

    def py2c (self, name):
        # check right type
        return None

    def asCallParameter (self, name):
        return name

    def getArg (self, name, index): # for Perl
        gluename = self.gluename
        pattern = ("%(name)s = (%(gluename)sObjectT*) getGluePointer (ST(%(index)s), %(gluename)sClassID, croakBuf);\n"
            "    if ((%(name)s == NULL) || !is%(gluename)sObject (%(name)s)) {\n"
            "        croak (croakBuf);\n"
            "    }")
        return pattern % locals ()

class AbstractGlue:

    def write (self, str):
        self.cStream.write (str)

    def writeBlock (self, list, indent = ''):
        cStream = self.cStream
        for line in list:
            cStream.write (indent + line + '\n')

    def writeCompound (self, entry, lines, indent = ''):
        write = self.cStream.write
        write (indent + entry + ' {\n')
        for line in lines:
            write ('%s    %s\n' % (indent, line))
        write (indent + '}\n')

    def writeComment (self, comment, indent = ''):
        self.cStream.write (indent + '/*' + comment + '*/')

    def writeBlockComment (self, commentlist, indent = ''):
        cStream = self.cStream
        cStream.write (indent + '/*\n')
        for line in commentlist:
            cStream.write (indent + ' * ' + line + '\n')
        cStream.write (indent + ' */\n')

    def snippetLongname (self, snippets, kind):
        try:
            result = snippets.longname (kind)
        except AttributeError:
            result = kind
        return result

    def __snippetOutput (self, snippets, code, fullName, indent = ''):
        longname = snippets.longname (fullName)
        write = self.cStream.write
        write (indent + '/* snippet ' + longname + ' */\n')
        write (code)
        write (indent + '/* endsnippet ' + longname + ' */\n')

    def writeSnippet (self, snippets, kind, language = '', indent = ''):
        if snippets == None:
            self.cStream.write ('/* no code for key %s */\n'
                    % (language + '/Generic_' + kind))
            return None
        snippetsFound = None
        for region in ['Generic_', language]:
            try:
                fullName = region + kind
                code = snippets [fullName]
                self.__snippetOutput (snippets, code, fullName, indent)
                snippetsFound = 1
            except KeyError:
                pass
        if not snippetsFound:
            keyTerm = snippets.longname (language + '/Generic_')
            self.cStream.write ('/* no code for key %s in %s*/\n'
                % (keyTerm, `snippets.keys ()`))
        return snippetsFound

    def writeMethodTable (self, mtableName, items):
        cStream = self.cStream
        cStream.write ('static PyMethodDef %s [] = {\n' % mtableName)
        for item in items:
            if item.isMethod:
                mname = item.pyname
                cStream.write ('    {"%s", (PyCFunction)%s_%s, 1},\n'
                    % (mname, mname, item.context.pyname))
        cStream.write ('    {NULL, NULL}\n')
        cStream.write ('};\n\n')


    def supports (self, item):
        return item.supportFor ('language', self.language)

class AbstractModuleGlue (AbstractGlue):
    def __init__ (self):
        self.typeMapping = {}
        self.addStandardTypes ()

    def addType (self, newType):
        self.typeMapping [newType.decl] = newType

    def addTargetSpecificTypes (self, typeDict):
        try:
            map = typeDict ['Generic']
            for typeMap in map:
                self.addType (typeMap)
        except KeyError:
            pass
        try:
            map = typeDict [self.language]
            for typeMap in map:
                self.addType (typeMap)
        except KeyError:
            pass

    def findType (self, decl):
        try:
            result = self.typeMapping [decl]
        except KeyError:
            result = self.defaultType (decl)
        return result

    def addTypeAlias (self, newType, baseType):
        typeMapping = self.typeMapping
        typeMapping [newType] = typeMapping [baseType]


    def writeModuleHeader (self, stream, module, target):
        if module.info:
            write = stream.write
            format = '   %-15s %s\n'
            write ('/*\n===========================================\n')
            for category, content in module.info:
                write (format % (category + ':', content))
            write (format % ('target:', target))
            now = time.localtime (time.time ())
            write (format % ('last changed:', time.strftime ('%d.%m.%Y', now)))
            relver = os.environ.get ('RELVER', 'R75')
            if (relver [0]) == 'R' and (relver >= 'R75'):
                licence = GPL
            else:
                licence = LGPL
            write (licence % now [0])
            write ('===========================================*/\n\n')

GPL = """
    ========== licence begin  GPL
    Copyright (c) 2000-%d SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
    ========== licence end
"""

LGPL = """
    ========== licence begin LGPL
    Copyright (c) 2000-%d SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
    ========== licence end
"""
