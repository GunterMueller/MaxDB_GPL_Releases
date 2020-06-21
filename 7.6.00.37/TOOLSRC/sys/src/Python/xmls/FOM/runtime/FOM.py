
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

from UserList import UserList

class CompiledElement:
    namespace = ''
    prefix = ''
    attributes = []
    elements = []
    isList = None
    preserveSpace = 1
    isInline = None
    pcdata = ''

    def __init__ (self, *args):
        for name, default in self.__class__.attributes:
            setattr (self, name, default)
        for name in self.__class__.elements:
            setattr (self, name, None)

    def toXML (self, stream, indent = ''):
        stream.write (indent + '<' + self.__class__.prefix + self.__class__.name)
        for name, default in self.__class__.attributes:
            value = getattr (self, name)
            if value != default:
                stream.write (' ' + name + '="' + str (value) +'"')
        if self.hasContent ():
            stream.write ('>\n')
            self.content2XML (stream, indent)
            stream.write (indent + '</' + self.__class__.name + '>\n')
        else:
            stream.write ('/>\n')

    def toFile (self, fname):
        stream = open (fname, 'wt')
        stream.write ('<?xml version="1.0"?>\n')
        self.toXML (stream)
        stream.close ()

    def hasContent (self):
        return self.__class__.elements or self.pcdata

    def content2XML (self, stream, indent):
        for elementName in self.__class__.elements:
            element = getattr (self, elementName)
            if element:
                if type (element) == type ([]):
                    for subElement in element:
                        subElement.toXML (stream, indent + '    ')
                else:
                    element.toXML (stream, indent + '    ')
        if self.pcdata:
            stream.write (self.pcdata)

    def addContent (self, text):
        self.pcdata = self.pcdata + text

    def setAttributes (self, dict):
        for key, value in dict.items ():
            setattr (self, key, value)

    def getNestedAttribute (self, nameList, default = None):
        object = self
        try:
            for name in nameList:
                object = getattr (object, name)
            return object
        except AttributeError:
            return default

    def createPath (self, nameList, factory):
        object = self
        for name in nameList:
            try:
                newObject = getattr (object, name)
                if newObject is None:
                    raise AttributeError
                else:
                    object = newObject
            except AttributeError:
                newObject = factory [name] ()
                setattr (object, name, newObject)
                object = newObject
        return object


class CompiledListElement (CompiledElement, UserList):
    isList = 'true'

    def __init__ (self, *args):
        CompiledElement.__init__ (self)
        UserList.__init__ (self)

    def hasContent (self):
        return len (self) > 0

    def content2XML (self, stream, indent):
        CompiledElement.content2XML (self, stream, indent)
        for entry in self:
            entry.toXML (stream, indent + '    ')

class CompiledPCData (CompiledElement):
    def __str__ (self):
        return self.pcdata

