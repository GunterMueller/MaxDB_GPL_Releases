
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

import re
from xml.parsers.xmlproc.dtdparser import DTDParser

from Schemical import *

true = 'true'

class DTDReader (DTDParser):
    def __init__ (self):
        DTDParser.__init__ (self)
        self.set_dtd_consumer (self)

    def parseFile (self, fname):
        self.parse_resource (fname)

    def dtd_start (self):
        self.elementDict = {}
        self.elements = []
        self.schema = Schema (self.elements)
        self.properties = []
        self.elementDefaults = []
        self.attributeDefaults = []
        #print 'dtd_start'

    def new_parameter_entity (self, name, value):
        pass

    def new_external_pe (self, entName, pubId, sysId):
        pass

    def new_element_type (self, name, content):
        #print 'new_element_type', name, content
        translated = self.translateContent (content)
        element = Element (name, translated)
        self.elementDict [name] = element
        self.elements.append (element)
        for name, value in self.elementDefaults:
            element.addProperty (name ,value)

    def new_attribute (self, elementName, attrName, a_type, a_decl, a_def):
        #print 'new_attribute', elementName, attrName, a_type, a_decl, a_def
        if a_def is not None:
            a_decl = a_def
        if type (a_type) == type ([]):
            domain = a_type
            a_type = ''
        else:
            domain = None
        attribute = Attribute (attrName, a_type, a_decl, domain)
        self.elementDict [elementName].addAttribute (attribute)
        for name, value in self.attributeDefaults:
            attribute.addProperty (name ,value)

    def dtd_end (self):
        #print 'dtd_end'
        self.schema.closeBuild ()
        for name, value in self.properties:
            self.schema.addProperty (name, value)
        self.resolveGrouping ()

    def convertPropertyValue (self, stringValue):
        if stringValue in ['false', 'no', 'off']:
            return None
        try:
            return int (stringValue)
        except ValueError:
            pass
        try:
            return float (stringValue)
        except ValueError:
            pass
        return stringValue

    propertyRE = re.compile (r'\s*[pP]:\s*([^=]+)(=.*)?')
    def handle_comment (self, comment):
        match = self.propertyRE.match (comment)
        if match:
            name, value = match.group (1, 2)
            name = string.strip (name)
            if value:
                value = string.strip (value [1:])
                value = self.convertPropertyValue (value)
            else:
                value = 'yes'
            words = string.split (name, '.')
            if words [0] == 'defaultproperty':
                try:
                    defaults = getattr (self, words [1] + 'Defaults')
                    shortName = string.join (words [2:], '.')
                    defaults.append (shortName, value)
                except AttributeError:
                    pass
            self.properties.append ((name, value))

    def translateContent (self, content):
        if len (content) == 3:
            result = self.translateContent3 (content)
        elif len (content) == 2:
            result = self.translateContent2 (content)
        else:
            result = None
        return result

    def translateContent3 (self, content):
        combinator, subContent, repetition = content
        if subContent == [('#PCDATA', '')]:
            result =  PCDATA ()
        elif repetition in listRepetitions:
            element = self.translateContent3 ((combinator, subContent, ''))
            result = List  (element, repetition2bool (repetition))
        elif combinator == '':
            assert len (subContent) == 1
            if len (subContent [0]) == 3:
                result = self.translateContent3 (subContent [0])
            else:
                result = self.translateContent2 (subContent [0])
        elif combinator == ',':
            translated = map (self.translateContent, subContent)
            result = Sequence (translated)
        elif combinator == '|':
            translated = map (self.translateContent, subContent)
            result = Alternatives (translated)
        else:
            result = None
        return result

    def translateContent2 (self, content):
        elementName, repetition = content
        if repetition == '?':
            result = Optional (elementName)
        elif repetition == '':
            result = Once (elementName)
        elif repetition in listRepetitions:
            result = List (elementName, repetition2bool (repetition))
        else:
            result = None
        return result

    def resolveGrouping (self):
        for element in self.elements:
            groupName = element.group
            if groupName:
                group = self.elementDict [groupName]
                groupElements = group.elements
                if groupElements is None:
                    groupElements = []
                    group.properties ['elements'] = groupElements
                groupElements.append (element)

listRepetitions = ['*', '+']

def repetition2bool (repetition):
    assert repetition in listRepetitions
    if repetition == '*':
        return true
    else:
        return None

