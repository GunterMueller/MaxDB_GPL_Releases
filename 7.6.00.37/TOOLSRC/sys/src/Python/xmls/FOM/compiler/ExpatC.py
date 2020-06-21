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

import sys
import string
import os

import fileutil

from SchemaCompiler import SchemaCompiler
from Schemical import Element

class ExpatC (SchemaCompiler):
    schemaName = '<unknown>'
    def defaultName (self, base):
        self.schemaName = base
        return base + 'Parser.cpp'

    def parserClass (self):
        return self.schemaName + 'Parser'

    def writeIntro (self, stream, fname):
        # self.writeHeaderFile (fname)
        stream.write (cformat ('', (fname, self.schemaName), [
            '/*',
            '    module:     %s',
            '    generated from: %s',
            '*/',
            '',
            '#include "ZExternal/expat/expat.h"',
            '#include "XML/FOMParser/FOM_IElementHandler.hpp"',
            ]))
        hppIndex = 0
        while 1:
            header = self.schema.getProperty ('parser.hpp.%d' % hppIndex)
            if not header:
                break
            stream.write ('#include "%s"\n' % header)
            hppIndex = hppIndex + 1
        for element in self.schema.elements:
            if element.isGroup or (self.schema.getProperty ('tag.' + element.name + '.ignore') == 'true'):
                continue
            includeFile = element.getProperty ('includefile')
            if not includeFile:
                includePattern = element.getProperty ('includepattern')
                if includePattern:
                    cppName = self.getElementClassName (element)
                    includeFile = includePattern % cppName
            if includeFile:
                stream.write ('#include "%s"\n' % includeFile)

    def writeHeaderFile (self, cppName):
        schemaName = self.schemaName
        fname = schemaName + 'Parser.hpp'
        stream = open (fname, 'w')
        includeGuard = string.upper (schemaName) + 'PARSER_HPP'
        parserClass = self.parserClass ()
        stream.write (cformat ('', locals (), [
            '/*',
            '    module:     %(fname)s',
            '    generated from: %(schemaName)s',
            '*/',
            '',
            '#ifndef %(includeGuard)s',
            '#define %(includeGuard)s',
            '',
            '#include "XML/FOMParser/FOM_AbstractParser.hpp"',
            '',
            'class %(parserClass)s : public FOM_AbstractParser',
            '{',
            '  public:',
            '    virtual const FOM_IElementHandler * findTagHandler (const char * name);',
            '};',
            ]))
        stream.write ('\n#endif\n')
        stream.close ()


    def compileElement (self, element, stream):
        if element.isGroup or (self.schema.getProperty ('tag.' + element.name + '.ignore') == 'true'):
            return
        handlerClass = self.getHandlerClassName (element)
        self.writeMethodFluff (stream)
        parserClass = self.parserClass ()
        stream.write (cformat ('', locals (), [
            'class %(handlerClass)s : public FOM_IElementHandler',
            '{',
            'public:',
            '    virtual void * create (FOM_AbstractParser * aparser, ',
            '                       const char ** atts) const;',
            #'    virtual void   setAttribute (void * element,',
            #'                       const char * attName,',
            #'                       const char * attValue) const;',
            '    virtual void   addElement (FOM_AbstractParser * aparser,',
            '                       void * element,',
            '                       const char * name,',
            '                       void * subElement) const;',
            '    virtual void   closeElement (FOM_AbstractParser * aparser,',
            '                       void * element) const;',
            '    virtual void   setCharacterData (FOM_AbstractParser * aparser,',
            '                       void * element,',
            '                       const string & data) const;',
            '    static const %(handlerClass)s instance;',
            '};',
            '',
            'const %(handlerClass)s %(handlerClass)s::instance;',
            ]))
        for name in ['create', 'addElement', 'closeElement', 'setCharacterData']:
            try:
                method = getattr (self, 'write_' + name)
                self.writeMethodFluff (stream)
                method (element, stream)
            except AttributeError:
                pass

    def writeFactory (self, stream):
        stream.write (cformat ('', None, [
            '/*----------------------------------------------------------------------',
            '',
            '  chapter: handler factory',
            '',
            '----------------------------------------------------------------------*/',
            '',
            ]))
        #
        # write lookup table
        #
        stream.write (cformat ('', locals (), [
            'static const struct {',
            '    const char * tagName;',
            '    const FOM_IElementHandler * handler;',
            '} tagMapping [] = {',
            ]))
        lookup = []
        for element in self.schema.elements:
            if element.isGroup:
                continue
            if element.getProperty ('isString'):
                handlerClass = 'FOM_PurePCDataHandler'
            elif self.schema.getProperty ('tag.' + element.name + '.ignore') == 'true':
                handlerClass = 'IgnoreTagHandler'
            else:
                handlerClass = self.getHandlerClassName (element)
            lookup.append ((element.name, handlerClass))
        lookup.sort ()
        for name, handlerClass in lookup:
            stream.write ('    {"%s", &%s::instance},\n' % (name, handlerClass))
        stream.write (cformat ('', None, [
            '    {NULL, NULL}',
            '};',
            ]))
        #
        # write lookup method
        #
        #self.writeMethodFluff (stream)
        #parserClass = self.parserClass ()
        #stream.write (cformat ('', locals (), [
        #    'class %(parserClass)s : public FOM_AbstractParser',
        #    '{',
        #    '  public:',
        #    '    virtual const FOM_IElementHandler * findTagHandler (const char * name);',
        #    '};',
        #    ]))
        self.writeMethodFluff (stream)
        schemaClass = self.schemaName + 'Parser'
        high = len (lookup) - 1
        stream.write (cformat ('', locals (), [
            'const FOM_IElementHandler * %(schemaClass)s::findTagHandler (',
            '    const char * name)',
            '{',
            '    int low = 0;',
            '    int current = -1;',
            '    int high = %(high)s;',
            '    int compareResult;',
            '    while ((current != low) || (current != high)) {',
            '        current = (high - low) / 2 + low;',
            '        /*fprintf (stdout, "    low: %%d \'%%s\' current: %%d \'%%s\' high: %%d \'%%s\'\\n",',
            '            low, tagMapping [low].tagName,',
            '            current, tagMapping [current].tagName,',
            '            high, tagMapping [high].tagName);',
            '        */',
            '        compareResult = strcmp (name, tagMapping [current].tagName);',
            '        if (compareResult == 0) {',
            '            return tagMapping [current].handler;',
            '        }',
            '        if (compareResult < 0) {',
            '            if (high > current) {',
            '                high = current;',
            '            }',
            '            else {',
            '                high = low;',
            '            }',
            '        }',
            '        else {',
            '            if (low < current) {',
            '                low = current;',
            '            }',
            '            else {',
            '                low = high;',
            '            }',
            '        }',
            '    }',
            '    return NULL;',
            '}',
            ]))

    def getHandlerClassName (self, element):
        prefix = element.getProperty ('handlerclassprefix', '')
        return prefix + element.name + 'Handler'

    def getElementClassName (self, element):
        prefix = element.getProperty ('realclassprefix', '')
        return prefix + element.name

    def getInterfaceClassName (self, element):
        groupName = element.getProperty ('group')
        if not groupName:
            return self.getElementClassName (element)
        superElement = self.schema.get (groupName)
        return self.getElementClassName (superElement)

    def expandSubclasses (self, namelist, isListEntry):
        result = []
        for name in namelist:
            element = self.schema.get (name)
            elements = element.getProperty ('elements')
            if elements:
                for subClass in elements:
                    result.append ((name, subClass.name, isListEntry))
            else:
                result.append ((name, name, isListEntry))
        return result

    def writeMethodFluff (self, stream):
        stream.write ('\n/*----------------------------------------*/\n\n')

    def write_create (self, element, stream):
        self.writeMethodIntro (element, stream, [
            'void  * %(handlerClass)s::create (',
            '    FOM_AbstractParser * aparser, ',
            '    const char ** atts) const',
            '{',
            '    %(parserClass)s * parser = static_cast <%(parserClass)s *> (aparser);',
            ])
        #
        # declare attributes
        #
        if element.attributes is None:
            attributes = []
        else:
            attributes = element.attributes
        def varName (attribute):
            return attribute.name + 'Variable'

        for attribute in attributes:
            if self.schema.getProperty ('attributealias.' + attribute.name):
                continue
            variableName = varName (attribute)
            dataType = self.attributeType (attribute)
            if dataType:
                typeString = dataType
            else:
                typeString = 'const char *'
            stream.write (cformat ('    ', locals (), [
                '%(typeString)s %(variableName)s;',
                ]))
            if attribute.default() != None:
                stream.write (cformat ('    ', locals (), [
                    'bool %(variableName)sSet = false;',
                    ]))
        #
        # parse attributes
        #
        if attributes:
            stream.write (cformat('    ', None, [
                'for (int attsIndex = 0; atts [attsIndex] != NULL; attsIndex += 2) {',
                '    const char * attName = atts [attsIndex];',
                '    const char * attValue = atts [attsIndex + 1];'
                ]))
            delimiter = ''
            hasValidAttributes = None
            for attribute in attributes:
                attName = attribute.name
                if self.schema.getProperty ('attributealias.' + attName):
                    continue
                hasValidAttributes = 1
                variableName = varName (attribute)
                convExpression = 'attValue'
                dataType = self.attributeType (attribute)
                if dataType:
                    conversion = self.schema.getProperty ('parseconversion.' + dataType)
                    if conversion is None:
                        conversion = 'dont_know_how_to_convert_chars_to_' + dataType
                    if string.find (conversion, '%s') != -1:
                        convExpression = conversion % 'attValue'
                    else:
                        convExpression = conversion + ' (attValue)'
                stream.write (cformat ('        ', locals (), [
                    delimiter + 'if (strcmp (attName, "%(attName)s") == 0) {',
                    '    %(variableName)s = %(convExpression)s;'
                    ]))
                if attribute.default() != None:
                    stream.write (cformat ('        ', locals (), [
                        '    %(variableName)sSet = true;',
                        ]))
                stream.write (cformat ('        ', locals (), [
                    '}'
                    ]))
                delimiter = 'else '
            if hasValidAttributes:
                stream.write (cformat ('        ', None, [
                    'else {',
                    '    parser->unknownAttribute (attName);',
                    '}',
                    ]))
            else:
                stream.write ('        parser->unknownAttribute (attName);\n')
            stream.write ('    }\n')
            #
            # set defaults
            #
            for attribute in attributes:
                attName = attribute.name
                default = attribute.default()
                if default == None:
                    continue
                default = '"%s"' % default
                convExpression = default
                dataType = self.attributeType (attribute)
                if dataType:
                    conversion = self.schema.getProperty ('parseconversion.' + dataType)
                    if conversion is None:
                        conversion = 'dont_know_how_to_convert_chars_to_' + dataType
                    if string.find (conversion, '%s') != -1:
                        convExpression = conversion % default
                    else:
                        convExpression = conversion + ' (' + default + ') '
                variableName = varName (attribute)
                stream.write (cformat ('    ', locals (), [
                    'if (%(variableName)sSet == false) {',
                    '    %(variableName)s = %(convExpression)s;',
                    '}'
                    ]))
        if not attributes:
            stream.write (cformat  ('    ', None, [
                'if (atts [0] != NULL) {',
                '    parser->unknownAttribute (atts [0]);',
                '}',
                ]))
        #
        # create new object
        #
        constructorArgs = []
        defaultArguments = self.schema.getProperty ('default.constructor.arguments')
        if defaultArguments:
            names = string.split (defaultArguments, ',')
            names = map (string.strip, names)
            for name in names:
                alias = self.schema.getProperty ('attributealias.' + name,
                    'no attributealias.' + name)
                constructorArgs.append (alias)
        for attribute in attributes:
            if attribute.getProperty ('constructorparameter'):
                alias = self.schema.getProperty ('attributealias.' + attribute.name)
                if alias:
                    constructorArgs.append (alias)
                else:
                    constructorArgs.append (varName (attribute))
        argumentList = string.join (constructorArgs, ', ')
        allocator = self.schema.getProperty ('allocator', '')
        if allocator:
            allocator = '(' + allocator + ')'
        cppClass = self.getElementClassName (element)
        stream.write (cformat ('    ', locals (), [
            '%(cppClass)s * result = new %(allocator)s %(cppClass)s (%(argumentList)s);',
            ]))
        # postnew
        snippetId = element.getProperty ('postnew')
        if snippetId:
            snippet = self.schema.getProperty ('customcode.' + snippetId)
            if snippet:
                code = snippet
            else:
                code = snippetId
            stream.write ('    ' + code + '\n')
        #
        # set non constructor arguments
        #
        for attribute in attributes:
            if not attribute.getProperty ('constructorparameter'):
                methodName = attribute.getProperty ('setter', 'set' + attribute.name)
                variableName = attribute.name + 'Variable'
                stream.write (cformat ('    ', locals (), [
                    'result->%(methodName)s (%(variableName)s);',
                    ]))
        #
        # finish method
        #
        stream.write (cformat ('', None, [
            '    return result;',
            '}',
            ]))

    def write_setAttribute (self, element, stream):
        self.writeMethodIntro (element, stream, [
            'void %(handlerClass)s::setAttribute (',
            '    %(parserClass)s * parser, ',
            '    void         * element,',
            '    const char   * attName,',
            '    const char   * attValue) const',
            '{',
            '    %(parserClass)s * parser = static_cast <%(parserClass)s *> (aparser);',
            '    %(cppClass)s * self = static_cast<%(cppClass)s *> (element);',
            ])
        if element.attributes:
            delimiter = ''
            for attr in element.attributes:
                attrName = attr.name
                variableName, conversionLine = self.generateParseConversion (
                    attr, 'attValue')
                methodName = attr.getProperty ('setter', 'set' + attrName)
                stream.write (cformat ('    ', locals (), [
                    delimiter + 'if (strcmp (attName, "%(attrName)s") == 0) {',
                    conversionLine,
                    '    self->%(methodName)s (%(variableName)s);',
                    '}',
                    ]))
                delimiter = 'else '
            stream.write (cformat ('    ', None, [
                'else {',
                '    /* TODO: throw exception because of unknown attribute */',
                '}'
                ]))
        else:
            stream.write ("    /* TODO: throw exception because of unknown attribute */\n")
        stream.write ("\n}\n")

    def write_addElement (self, element, stream):
        self.writeMethodIntro (element, stream, [
            'void %(handlerClass)s::addElement (',
            '    FOM_AbstractParser * aparser, ',
            '    void         * element,',
            '    const char   * name,',
            '    void         * subElement) const',
            '{',
            '    %(parserClass)s * parser = static_cast <%(parserClass)s *> (aparser);',
            '    %(cppClass)s * self = reinterpret_cast<%(cppClass)s *> (element);',
            ])
        try:
            varnames = self.expandSubclasses (element.varnames (), None)
            elementClass = self.getElementClassName (element)
            listnames = self.expandSubclasses (element.listnames (), 'true')
            allnames = varnames + listnames
            if allnames:
                delimiter = ''
                for varname, subClassName, isListEntry in allnames:
                    if isListEntry:
                        prefix = 'add'
                    else:
                        prefix = 'set'
                    methodName = element.getProperty (varname + '.setter', prefix + varname)
                    subElement = self.schema.elementDict [subClassName]
                    if subElement.getProperty ('isString'):
                        subElementClass = 'FOM_PurePCData'
                        expr = 'typedElement->getData ()'
                        delete = '    delete typedElement;'
                    else:
                        subElementClass = self.getElementClassName (subElement)
                        expr = 'typedElement'
                        delete = ''
                    stream.write (cformat ('    ', locals (), [
                        delimiter + 'if (strcmp (name, "%(subClassName)s") == 0) {',
                        '    %(subElementClass)s * typedElement = reinterpret_cast<%(subElementClass)s *> (subElement);',
                        '    self->%(methodName)s (%(expr)s);',
                        delete,
                        '}',
                        ]))
                    delimiter = 'else '
                stream.write (cformat ('    ', None, [
                'else {',
                '    parser->unknownElement (name);',
                '}',
                ]))
            else:
                stream.write ("    parser->unknownElement (name);\n")
        except:
            exception_class, exception_instance, traceback = sys.exc_info()
            print '\nerror:', self.getElementClassName (element), exception_class, exception_instance,'\n'
        stream.write ("\n}\n")

    def write_closeElement (self, element, stream):
        self.writeMethodIntro (element, stream, [
            'void %(handlerClass)s::closeElement (',
            '    FOM_AbstractParser * aparser, ',
            '    void         * element) const',
            '{',
            '    %(parserClass)s * parser = static_cast <%(parserClass)s *> (aparser);',
            '    %(cppClass)s * self = static_cast<%(cppClass)s *> (element);',
            ])
        postClose = element.getProperty ('postclose')
        if postClose:
            stream.write ('    ' + postClose + '\n')
        stream.write ("\n}\n")


    def write_setCharacterData (self, element, stream):
        self.writeMethodIntro (element, stream, [
            'void %(handlerClass)s::setCharacterData (',
            '    FOM_AbstractParser * aparser, ',
            '    void         * element,',
            '    const string & data) const',
            '{',
            '    %(parserClass)s * parser = static_cast <%(parserClass)s *> (aparser);',
            '    %(cppClass)s * self = static_cast<%(cppClass)s *> (element);',
            ])
        stream.write ("""    /* TODO: convert string to other datatypes */\n""")
        if element.getKind () == Element.pcdataKind:
            stream.write ('    self->setData (data);\n')
        stream.write ("\n}\n")

    def generateParseConversion (self, attribute, varname):
        variableName = varname
        dataType = self.attributeType (attribute)
        if dataType:
            variableName = 'typedValue'
            conversion = self.schema.getProperty ('parseconversion.' + dataType)
            if conversion is None:
                conversion = 'dont_know_how_to_convert_chars_to_' + dataType
            conversionLine = ('    %(dataType)s %(variableName)s '
                + '= %(conversion)s (%(varname)s);') % locals ()
        else:
            conversionLine = '    /* no conversion */'
        return variableName, conversionLine

    def attributeType (self, attribute):
        result = attribute.getProperty ('datatype')
        if result is None:
            result = self.schema.getProperty ('attributes.'
                + attribute.name + '.datatype')
        return result

    def writeMethodIntro (self, element, stream, intro):
        handlerClass = self.getHandlerClassName (element)
        cppClass = self.getElementClassName (element)
        parserClass = self.parserClass ()
        stream.write (cformat ('', locals (), intro))

def cformat (indent, formatArg, lines):
    text = indent + string.join (lines, '\n' + indent) + '\n'
    if formatArg:
        text = text % formatArg
    return text


