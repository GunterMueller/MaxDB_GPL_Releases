
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

from AbstractCompiler import AbstractCompiler
from Schemical import *

class DTDGenerator (AbstractCompiler):
    def __init__ (self):
        self.elementParameters = []

    def defaultName (self, schemaName):
        return schemaName + 'DTD.dtd'

    def genSchemaProperties (self, schemical, stream):
        for name, value in schemical.properties.items ():
            stream.write ('<!--P: %s=%s -->\n' % (name, value))

    def genElement (self, element, stream):
        if element.isGroup:
            self.writeGroupEntity (element, stream)
            return
        eName = element.name
        for name, value in element.properties.items ():
            stream.write ('<!--P: %s.%s=%s -->\n' % (eName, name, value))
        contentString = self.contentDefinition (element.content)
        for parameterName in self.elementParameters:
            pattern = r'\b' + parameterName + r'\b'
            replace = '%' + parameterName + ';'
            contentString = re.sub (pattern, replace, contentString)
        stream.write ('<!ELEMENT ' + eName + ' '
            + contentString + ' >\n')
        # attributes
        if element.attributes:
            self.genElementAttributes (eName, element.attributes, stream)

    def writeGroupEntity (self, element, stream):
        stream.write ('<!ENTITY ' + element.name + ' % "')
        names = map (lambda e: e.name, element.elements)
        stream.write (string.join (names, ' | '))
        stream.write ('">\n')
        self.elementParameters.append (element.name)

    def genElementAttributes (self, elementName, attributes, stream):
        for attribute in attributes:
            prefix = elementName + '.' + attribute.name
            for name, value in attribute.properties.items ():
                stream.write ('<!--P: %s.%s=%s -->\n' % (prefix, name, value))
        stream.write ('<!ATTLIST ' + elementName)
        for attribute in attributes:
            output = '\n    ' + self.attributeDefinition (attribute)
            stream.write (output)
        stream.write ('\n>\n\n')

    def attributeDefinition (self, attribute):
        output = attribute.name + ' ' + attribute.kind + ' '
        if attribute.domain:
            altList = string.join (attribute.domain, ' | ')
            output = output + '( ' + altList + ' )'
        if attribute.modifier:
            output = output + ' ' + attribute.modifier
        return output

    def contentDefinition (self, content):
        if not content:
            return 'EMPTY'
        clazz = content.__class__
        if clazz == Sequence:
            return self.sequenceDefinition (content)
        elif clazz == Alternatives:
            return self.alternativesDefinition (content)
        elif clazz == List:
            return self.listDefinition (content)
        elif clazz == Optional:
            return self.optionalDefinition (content)
        elif clazz == Once:
            return self.onceDefinition (content)
        elif clazz == PCDATA:
            return self.pcdataDefinition (content)
        else:
            raise 'unknown content class', clazz

    def combineDefinitions (self, content, delimiter):
        list = []
        for item in content.elements:
            list.append (self.contentDefinition (item))
        return '(' + string.join (list, delimiter) + ')'

    def sequenceDefinition (self, content):
        return self.combineDefinitions (content, ', ')

    def alternativesDefinition (self, content):
        return self.combineDefinitions (content, ' | ')

    def listDefinition (self, content):
        if content.maybeEmpty:
            listChar = '*'
        else:
            listChar = '+'
        return self.contentDefinition (content.content) + listChar

    def optionalDefinition (self, content):
        return self.contentDefinition (content.content) + '?'

    def onceDefinition (self, content):
        return content.content

    def pcdataDefinition (self, content):
        return '#PCDATA'


