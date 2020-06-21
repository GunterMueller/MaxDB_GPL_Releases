
#
# Schema
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


import string

class Schema:
    def __init__ (self, elements, nameSpace = None, importSchemas = None):
        self.elements = elements
        self.nameSpace = nameSpace
        if nameSpace:
            for element in self.elements:
                if not element.prefix:
                    element.prefix = nameSpace + ':'
        if importSchemas:
            for importSchema in importSchemas:
                self.elements = self.elements + importSchema.elements
        self.properties = {}

    def addProperty (self, name, value):
        splitted = string.split (name, '.')
        if self.elements and (len (splitted) > 1):
            matchName = splitted [0]
            for element in self.elements:
                if element.name == matchName:
                    shortenedName = name [len (matchName) + 1:]
                    element.addProperty (shortenedName, value)
                    return
        self.properties [name] = value

    def closeBuild (self):
        elementDict = self.elementDict = {}
        for element in self.elements:
            elementDict [element.name] = element

    def __getitem__ (self, name):
        return self.elementDict [name]

    def getProperty (self, name, default = None):
        return self.properties.get (name, default)

    def get (self, name, default = None):
        return self.elementDict.get (name, default)

class Element:
    normalKind = 0
    listKind = 1
    pcdataKind = 2
    emptyKind = 3
    anyKind = 4
    prefix = ''
    def __init__ (self, name, content = None, attributes = None, comment = None):
        self.name = name
        self.comment = comment
        self.content = _shortCut (content)
        if self.content and not isinstance (self.content, Sequence):
            self.content = Sequence ([self.content])
        self.attributes = attributes
        self.properties = {}

    def addAttribute (self, attribute):
        if self.attributes is None:
            self.attributes = []
        self.attributes.append (attribute)

    def displayName (self):
        return self.prefix + self.name

    def identifier (self):
        name = self.displayName ()
        result = string.replace (name, ':', '_')
        return result

    def getKind (self):
        if not self.content:
            return self.emptyKind
        if len (self.content) > 1:
            return self.normalKind
        singleElement = self.content [0]
        if isinstance (singleElement, List):
            return self.listKind
        elif isinstance (singleElement, PCDATA):
            return self.pcdataKind
        else:
            return self.normalKind

    def isList (self):
        return self.getKind () == self.listKind

    def varnames (self):
        if self.isList ():
            return []
        elif self.content:
            return self.content.varnames ()
        else:
            return []

    def listnames (self):
        if self.content:
            return self.content.listnames ()
        else:
            return []

    def addProperty (self, name, value):
        splitted = string.split (name, '.')
        if self.attributes and (len (splitted) > 1):
            matchName = splitted [0]
            for attribute in self.attributes:
                if attribute.name == matchName:
                    shortenedName = name [len (matchName) + 1:]
                    attribute.addProperty (shortenedName, value)
                    return
        self.properties [name] = value

    def __getattr__ (self, name):
        return self.properties.get (name)

    def getProperty (self, name, default = None):
        return self.properties.get (name, default)

    def __str__ (self):
        return "<%s '%s'>" % (self.__class__.__name__, self.name)

    __repr__ = __str__

#
# Content
#

class ElementContent:
    def __init__ (self):
        pass

    def varnames (self):
        return ['<not implemented>']

    def listnames (self):
        return ['<not implemented>']

class Sequence (ElementContent):
    def __init__ (self, elements):
        self.elements = map (_shortCut, elements)

    def varnames (self):
        result = []
        for entry in self:
            result.extend (entry.varnames ())
        return result

    def listnames (self):
        result = []
        for entry in self:
            result.extend (entry.listnames ())
        return result

    def __len__ (self):
        return len (self.elements)

    def __getitem__ (self, index):
        return self.elements [index]

class Alternatives (ElementContent):
    def __init__ (self, elements):
        self.elements = map (_shortCut, elements)

    def varnames (self):
        result = []
        for entry in self.elements:
            result.extend (entry.varnames ())
        return result

    def listnames (self):
        return []

class List (ElementContent):
    def __init__ (self, content, maybeEmpty):
        self.content = _shortCut (content)
        self.maybeEmpty = maybeEmpty

    def varnames (self):
        if isinstance (self.content, Once):
            result = self.content.varnames ()
        else:
            result = []
        return result

    def listnames (self):
        if isinstance (self.content, Once):
            result = self.content.varnames ()
        elif isinstance (self.content, Alternatives):
            result = self.content.varnames()
        else:
            result = []
        return result

class Optional (ElementContent):
    def __init__ (self, content):
        self.content = _shortCut (content)

    def varnames (self):
        return self.content.varnames ()


    def listnames (self):
        return self.content.listnames ()

class Once (ElementContent):
    def __init__ (self, content):
        self.content = content

    def varnames (self):
        return [self.content]

    def listnames (self):
        return []

class PCDATA (ElementContent):
    def __init__ (self):
        pass

    def varnames (self):
        return []

    def listnames (self):
        return []
#
# Attribute
#

class Attribute:
    def __init__ (self, name, kind, modifier = '#IMPLIED', domain = None):
        self.name = name
        self.kind = kind
        self.modifier = modifier
        self.domain = domain
        self.properties = {}

    def default (self):
        if (self.modifier != None) and (self.modifier [:1] != '#'):
            return self.modifier
        else:
            return None

    def addProperty (self, name, value):
        self.properties [name] = value

    def __repr__ (self):
        return '<Attribute ' + self.name + '>'

    __str__ = __repr__

    def __getattr__ (self, name):
        return self.properties.get (name)

    def getProperty (self, name, default = None):
        return self.properties.get (name, default)

#
# helper
#

_stringType = type ('')
_listType = type ([])
_tupleType = type (())

def _shortCut (content):
    if type (content) is _stringType:
        result = Once (content)
    elif type (content) is _listType:
        result = Sequence (content)
    elif (type (content) == _tupleType) and (len (content) == 2):
        main, modifier = content
        if modifier == '?':
            result = Optional (main)
        elif modifier == '*':
            result = List (main, 1)
        elif modifier == '+':
            result = List (main, None)
        else:
            result = content
    else:
        result = content
    return result

