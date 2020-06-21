# changed file
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

class Context:
    isMethod = None
    def __init__ (self, name, pyname = None, doc = '',
            precondition = None, snippets = None, **attributes):
        self.name = name
        if pyname:
            self.pyname = pyname
        else:
            self.pyname = name
        self.items = []
        self.doc = doc
        self.precondition = precondition
        self.snippets = snippets
        self.constantParts = 0
        self.attributes = attributes

    def add (self, item):
        item.context = self
        self.items.append (item)

    def addAll (self, itemlist):
        for item in itemlist:
            item.context = self
        self.items = self.items + itemlist

    def insert (self, item):
        item.context = self
        self.items.insert (0, item)

    def insertAll (self, itemlist):
        for item in itemlist:
            item.context = self
        self.items = itemlist + self.items

    isClass = None
    isMethod = None

    char2Type = {'s': 'char *', 'i': 'int', 'l': 'long', 'o': 'PyObject *'}

    def argType (self, argstr):
        try:
            return self.char2Type [argstr]
        except KeyError:
            return 'void*'

    type2Char = {'char *': 's', 'int': 'i', 'long': 'l'}

    def pyType (self, typestr):
        try:
            return self.type2Char [typestr]
        except KeyError:
            return 'o'

    def typeSynonyms (self, typeChar, typeNames):
        type2Char = self.type2Char
        for typeName in typeNames:
            type2Char [typeName] = typeChar


    def methods (self):
        result = []
        for item in self.items:
            if (item.isMethod):
                result.append (item)
        return result

    def isRealConstructor (self):
        return None


    def supportFor (self, key, value):
        try:
            ownValue = self.attributes [key]
        except KeyError:
            return 1   # not set: supported for all
        return value == ownValue

    def isDocumented (self):
        return self.doc != None

class Module (Context):

    def __init__ (self, name, pyname = None, doc = '',
            snippets = None, exceptions = None, exceptionClasses = None,
            info = None,
            modpath = None, **attributes):
        apply (Context.__init__, (self, name, pyname, doc, None, snippets), attributes)
        if exceptions == None:
            exceptions = []
        self.exceptions = exceptions
        self.exceptionClasses = exceptionClasses
        if exceptionClasses:
            for exception in exceptionClasses:
                exception.context = self
        self.classes = []
        self.info = info
        self.modpath = modpath

    def writeGlue (self, generator):
        generator.writeModuleGlue (self)


class Class (Context):

    isClass = 1

    def __init__ (self, name, pyname = None, doc = '',
            precondition = None, destructor = "0", isEmbedded = None,
            isCpp = None,
            snippets = None, **attributes):
        apply (Context.__init__, (self, name, pyname, doc, precondition, snippets), attributes)
        self.destructor = destructor
        self.constructor = None
        self.isEmbedded = isEmbedded
        self.isCpp = isCpp

    def writeGlue (self, generator):
        self.context.classes.append (self)
        self.listSpecialMethods ()
        generator.writeClassGlue (self)

    def listSpecialMethods (self):
        specials = {}
        for item in self.items:
            specialID = item.attributes.get ('specialMethod')
            if specialID != None:
                specials [specialID] = item
        self.specialMethods = specials

class Method (Context):
    isMethod = None
    isConstructor = None

    def __init__ (self, description, pyname = None, doc = '', snippets = None,
            exception = None, **attributes):
        if pyname == None:
            pyname = description.pyname
        apply (Context.__init__, (self, description.name, pyname, doc, None, snippets),
            attributes)
        self.parms = description.parms
        self.returns = description.returns
        self.exception = exception

    isMethod = 1

    def writeGlue (self, generator):
        generator.methodWriter.writeGlue (self)

class Constructor (Method):
    isConstructor = 1

    def __init__ (self, clazz, description, pyname = None, doc = '',
            snippets = None, exception = None, **attributes):
        apply (Method.__init__,  (self, description, pyname, doc, snippets, exception),
            attributes)
        self.clazz = clazz
        if self.isRealConstructor ():
            clazz.constructor = self

    def writeGlue (self, generator):
        generator.constructorWriter.writeGlue (self)

    def isRealConstructor (self):
        return self.pyname == self.clazz.pyname

class ExportedConstants (Context):
    def __init__ (self, typechar, names):
        Context.__init__ (self, '')
        self.typechar = typechar
        self.names = names

    def writeGlue (self, generator):
        generator.writeConstantPart (self)

class Exception:
    def __init__ (self, name, doc = '', pycode = ''):
        self.name = name
        self.doc = doc
        self.pycode = pycode

