
import sys
import os
path = os.path
from string import join, joinfields, splitfields, lower, upper, center, \
        strip, atoi, atol
import regex
import vdnStream
from pasScanner import *
from pasParser import *
from parserUtil import stringUntil, stringUntilAny

if os.environ ['RELVER'] > 'R71':
    includeFile = 'gsp00_2'
    enumMacro = 'ENUM_%s_MSP00'
    enumTemplate = 'tsp00_Enum<%s_Enum, %s, %s>'
    smallSetMacro = 'SMALLSET_%s_MSP00 (      %%s, %s, %s)'
    setMacro = 'SET_%s_MSP00 (           %%s, %s, %s, %d)'
    smallSetTemplate = 'tsp00_SmallSet<%s, %s>'
    setTemplate = 'tsp00_Set<%s, %s, %d>'
    arrayMacro = 'ARRAY_%s_MSP00'
    array2Macro = 'ARRAY2_%s_MSP00'
    arrayTemplate = 'ARRAY_TYPESTRING_MSP00(%s, %s%s)'
    byteArrayMacro = 'BYTEARRAY_%s_MSP00'
    byteArray2Macro = 'BYTEARRAY2_%s_MSP00'
    byteArrayTemplate = 'tsp00_ByteArray<%s%s>'
    pascalStringMacro = 'PASCALSTRING_%s_MSP00'
    pascalString2Macro = 'PASCALSTRING2_%s_MSP00'
    pascalStringTemplate = 'tsp00_PascalString<%s%s>'
    oldPascalStringMacro = 'OLDPASCALSTRING_%s_MSP00'
    oldPascalString2Macro = 'OLDPASCALSTRING2_%s_MSP00'
    oldPascalStringTemplate = 'tsp00_OldPascalString<%s%s>'
    cstringMacro = 'CSTRING_TYPE_MSP00'
else:
    includeFile = 'geo001x'
    enumMacro = 'ENUM_%s_MSP00'
    enumTemplate = 'tsp00_Enum<%s_Enum, %s, %s>'
    smallSetMacro = 'SMALLSET_%s_MSP00 (      %%s, %s, %s)'
    setMacro = 'SET_%s_MSP00 (           %%s, %s, %s, %d)'
    smallSetTemplate = 'tsp00_SmallSet<%s, %s>'
    setTemplate = 'tsp00_Set<%s, %s, %d>'
    arrayMacro = 'ARRAY_%s_MSP00'
    array2Macro = 'ARRAY2_%s_MSP00'
    arrayTemplate = 'tsp00_Array<%s, %s%s>'
    byteArrayMacro = 'PASCALSTRING_%s_MSP00'
    byteArray2Macro = 'PASCALSTRING2_%s_MSP00'
    byteArrayTemplate = 'tsp00_PascalString<%s%s>'
    pascalStringMacro = 'PASCALSTRING_%s_MSP00'
    pascalString2Macro = 'PASCALSTRING2_%s_MSP00'
    pascalStringTemplate = 'tsp00_PascalString<%s%s>'
    oldPascalStringMacro = 'PASCALSTRING_%s_MSP00'
    oldPascalString2Macro = 'PASCALSTRING2_%s_MSP00'
    oldPascalStringTemplate = 'tsp00_PascalString<%s%s>'
    cstringMacro = 'CSTRING_TYPE_MSP00'

if os.environ ['RELVER'] >= 'R73':
    int1Type = 'char'           #'SAPDB_Int1'
    uint1Type = 'unsigned char' #'SAPDB_UInt1'
    int2Type = 'short'          #'SAPDB_Int2'
    uint2Type = 'unsigned short'#'SAPDB_UInt2'
    int4Type = 'int'            #'SAPDB_Int4'
else:
    int1Type = 'tsp_int1'
    uint1Type = 'tsp_uint1'
    int2Type = 'tsp_int2'
    uint2Type = 'tsp_uint2'
    int4Type = 'tsp_int4'

ConstNotDeclared = 'Constant not declared'

class PtoC00:

    def __init__ (self, scanner, outstream, constants, types, enumMaxima, options):
        self.verbose = options.verbose
        self.keepComments = not options.stripComments
        self.warnings = options.warnings
        self.outstream = outstream
        self.scanner = scanner
        self.scanner.keepComments = self.keepComments
        self.constants = constants
        self.types = types
        self.typeNames = []
        self.enumMaxima = enumMaxima
        self.modid = ''
        self.stringTypes = {}
        self.enumTypes = {}
        self.setTypes = {}
        self.forwardTypes = {}
        self.declarators = {
                DerivedDcl: DerivedDeclarator (self),
                RangeDcl: RangeDeclarator (self),
                EnumerationDcl: EnumerationDeclarator (self),
                SetDcl: SetDeclarator (self),
                PointerDcl: PointerDeclarator (self),
                ArrayDcl: ArrayDeclarator (self),
                RecordDcl: RecordDeclarator (self),
                }

    def error (self, text):
        sys.stderr.write ('ptoc00: Error: %s\n' % text)
        self.outstream.flush ()
        raise 'Compile Error', text

    def warning (self, text):
        sys.stderr.write ('ptoc00: Warning: %s\n' % text)

    def translate (self):
        self.errors = 0
        try:
            scanner = self.scanner
            while 1:
                token = scanner.next ()
                if token.id != keywordID:
                    raise TokenExpected (token, ['CONST', 'TYPE'])
                if scanner.concomLine:
                    self.handleConComLines (scanner.concomLine)
                self.processPreComments ([token])
                if token.value == 'const':
                    self.writeHeading (self.outstream, 'constants')
                    readConstPart (scanner, self.handleConstantDecl)
                elif token.value == 'type':
                    self.handleConcom (token)
                    self.writeHeading (self.outstream, 'types')
                    readTypePart (scanner, self.handleTypeDecl)
                else:
                    raise TokenExpected (token, ['CONST', 'TYPE'])
                    raise SyntaxError, (token, token.value, 'CONST or TYPE expected')
        except EOFError:
            if scanner.concomLine:
                self.handleConComLines (scanner.concomLine)
            self.instantiateTemplates ()
            self.closeOutstream ()
        except SyntaxError:
            self.closeOutstream ()
            self.errors = self.errors + 1
        return self.errors

    def handleConstantDecl (self, tokens):
        constname = tokens [0].value
        if self.verbose:
            print constname
        self.handleConcom (tokens [0])
        self.processPreComments (tokens)
        constdef = map (tokenAsString, tokens [2:-1])
        defAsString = join (constdef)
        if len (constdef) > 1:
            defAsString = '(' + defAsString + ')'
        comment = ''
        if self.keepComments:
            comment = self.getPostComment (tokens)
        try:
            value = int (defAsString)
            str = '#define %-30s %10d%s\n' % (constname, value, comment)
            self.constants [constname] = value
        except ValueError:
            try:
                self.constants [constname] = self.constants [defAsString]
            except KeyError:
                pass
            str = '#define %-30s %s%s\n' % (constname, defAsString, comment)
        self.outstream.write (str)

    def handleTypeDecl (self, startToken, type):
        typename = tokenAsString (startToken)
        if self.verbose:
            print typename
        self.types [typename] = type
        self.typeNames.append (typename)
        self.handleConcom (startToken)
        self.processPreComments ([startToken])
        lastToken = type.tokens [-2]
        attribute = lastToken.getAttribute ('postComment')
        if attribute != None:
            if attribute == 'ptoc00Ignore':
                self.outstream.write ('/* %s ignored */\n' % typename)
                return
            if attribute [:15] == 'ptoc00Redefine=':
                self.outstream.write ('typedef %-26s %s;\n' % (attribute [15:], typename))
                return
        declarator = self.declarators [type.__class__]
        try:
            declarator.handleType (typename, type)
        except ConstNotDeclared, value:
            file, lineno, colno = startToken.pos
            sys.stderr.write ("Error (line %d): %s: '%s'\n"
                    % (lineno, ConstNotDeclared, value))
            self.errors = self.errors + 1

    def constval (self, val):
        try:
            numval = int (val)
        except ValueError:
            try:
                numval = self.constants [val]
                numval = int (numval)
            except KeyError:
                raise ConstNotDeclared, val
                #if self.warnings:
                #    self.warning ('constant %s not defined' % val)
        return numval

    def writeProlog (self, stream, fname, includes, options):
        self.logicalIncludes = includes
        upfname = upper (fname)
        stream.write ("""
/* some common prolog for every module
 */

#ifndef %s_H
#define %s_H

""" % (upfname, upfname))
        self.writeHeading (stream, 'includes')
        self.writeIncludeDirective (stream, includeFile)
        for include in includes:
            self.writeIncludeDirective (stream, include)
        if None:
            stream.write ("""
#ifdef  __opencxx
#pragma -C InvGlbName-
#endif
""")

    def writeIncludeDirective (self, stream, fname):
        prepToken = upper (fname)
        stream.write ("""
#ifndef %s_H
#include "%s.h"
#endif
""" % (prepToken, fname))

    def closeOutstream (self):
        if None:
            self.outstream.write ("""
#ifdef  __opencxx
#pragma -C InvGlbName+
#endif
""")
        self.outstream.write ("\n\n#endif\n")
        self.outstream.close ()


    def instantiateTemplates (self):
        allNames = self.enumTypes.keys () + self.setTypes.keys ()
        if len (allNames) > 0:
            write = self.outstream.write;
            write ('\nstruct t%s_TemplateDummy {\n' % self.prefix)
            for name in allNames:
                if name [-1] != '_':
                    write ('    %-20s dummy_%s;\n' % (name, name))
            write ('};\n')

    def writeHeading (self, stream, text):
        heading = ''
        for c in text:
            heading = heading + c + ' '
        heading = center (upper (heading), 40)
        hlen = len (heading)
        stars = '*' * (hlen + 2)
        comment = '\n/%s\n *%s*\n %s/\n\n' % (stars, heading, stars)
        stream.write (comment)

    def writeBlockComment (self, commentlist, indent):
        write = self.writeComment
        for comment in commentlist:
            write (comment, indent, '\n')

    def writeComment (self, comment, space = '', nl = ''):
        if comment in prettyGenerated:
            return
        matchSize = inlineC.match (comment)
        if matchSize > -1:
            self.outstream.write ('%s%s%s' % (space, comment[matchSize:], nl))
        else:
            self.outstream.write ('%s/*%s*/%s' % (space, comment, nl))

    def processPreComments (self, tokens):
        if not self.keepComments:
            return
        preComment = tokens [0].getAttribute ('preComment')
        if preComment:
            self.writeBlockComment (preComment, '    ')

    def getPostComment (self, tokens, ):
        if not self.keepComments:
            return ''
        try:
            postComment = tokens [-2].getAttribute ('postComment')
        except IndexError:
            postComment = None
        if not postComment:
            postComment = tokens [-1].getAttribute ('postComment')
        if postComment:
            comment = '    /*%s*/' % postComment
        else:
            comment = ''
        return comment


    def openOutstream (self, fname, includes, options):
        outfile = options.outfile
        if outfile:
            if path.isdir (outfile):
                cname = path.join (outfile, fname + '.h')
            else:
                cname = outfile
        else:
            cname = fname + '.h'
        result = open (cname, "w")
        self.writeProlog (result, fname, includes, options)
        return result

    # concom specifics

    def handleConcom (self, token):
        #print 'trying %s (%x)' % (token, hash(token))
        concomLines = token.getAttribute ('concomLine')
        if concomLines:
            #print '    found', concomLine
            self.handleConComLines (concomLines)

    concomRE = regex.compile (' *& *\([a-zA-Z]+\) *\(.*\)')

    def handleConComLines (self, lines):
        re = self.concomRE
        for line in lines:
            if re.match (line) == -1:
                raise 'Concom error', line
            keyword, parmstring = re.group (1, 2)
            keyword = lower (keyword)
            if keyword == 'ifdef':
                self.outstream.write ('\n#if defined (%s)\n\n' % strip (parmstring))
            elif keyword == 'ifndef':
                self.outstream.write ('\n#if !defined (%s)\n\n' % strip (parmstring))
            elif keyword == 'else':
                self.outstream.write ('\n#else\n\n')
            elif keyword == 'endif':
                self.outstream.write ('\n#endif\n\n')
            elif keyword == 'if':
                self.outstream.write ('\n#if %s\n\n' % self.expression (parmstring))
            else:
                raise 'Invalid Concom keyword', ('%s in %s' % (keyword, line))

    inRE = regex.compile (' *\([$a-z]+\) *in *[[]\(.*\)]', regex.casefold)
    eqRE = regex.compile (' *\([$a-z]+\) *= *\([a-z0-9]+\)', regex.casefold)

    def expression (self, parmstring):
        inRE = self.inRE
        eqRE = self.eqRE
        if eqRE.match (parmstring) != -1:
            return 'defined (%s)' % eqRE.group (2)
        elif inRE.match (parmstring) != -1:
            words = splitfields (inRE.group (2), ',')
            words = map (lambda word: 'defined (%s)' % strip (word), words)
            return joinfields (words, ' || ')
        else:
            raise 'Invalid Concom expression', parmstring

    def genSizeof (self, sourceFile):
        from excludeCpp import exclude
        if sourceFile in exclude:
            return
        typeNames = self.typeNames
        typeNames.sort ()
        outstream = open (sourceFile + '.cpp', "w")
        write = outstream.write
        write ('\n#include <stdio.h>\n')
        write ('#include <stdlib.h>\n')
        # gen includes
        self.writeIncludeDirective (outstream, includeFile)
        for include in self.logicalIncludes:
            self.writeIncludeDirective (outstream, include)
        self.writeIncludeDirective (outstream, sourceFile)
        write ('\nint main (int argc, char *argv[])\n{\n')
        write ('    fprintf (stdout, "--- %s ---\\n", argv [1]);\n')
        lastType = ''
        for typeName in typeNames:
            if typeName == lastType:
                continue
            lastType = typeName
            formatted = '%s :' % typeName
            formatString = '%-32.32s%8d'
            write ('    fprintf (stdout, "%s\\n", "%s", sizeof (%s));\n' % (formatString, formatted, typeName))
        write ('    exit (0);\n');
        write ('    return 0;\n}\n')
        outstream.close ()

    def isStringType (self, typename):
        result = self.stringTypes.get (typename);
        return result

    def declareAsStringType (self, typename):
        self.stringTypes [typename] = 1

    def isEnumType (self, typename):
        result = self.enumTypes.get (typename);
        return result

    def declareAsEnumType (self, typename):
        self.enumTypes [typename] = 1

    def declareAsSetType (self, typename):
        self.setTypes [typename] = 1

    # end class PtoC00

class CppDeclarator:
    def __init__ (self, ptoc00):
        self.ptoc00 = ptoc00
        self.write = ptoc00.outstream.write
        self.constval = ptoc00.constval
        self.handleConcom = ptoc00.handleConcom

    def handleType (self, typename, type):
        typedef = self.typeString (type)
        self.write ('typedef %-26s %s;\n' % (typedef, typename))

    def declareVar (self, varname, type):
        typedef = self.typeString (type)
        result = '%-19s %s' % (typedef, varname)
        return result

    def declareVarByMacro (self, varname, type):
        str = self.typeString (type, 'VAR')
        result = str % varname
        return result

    def getDeclarator (self, type):
        declarator = self.ptoc00.declarators [type.__class__]
        return declarator

    def cppTypeString (self, type, modifier):
        return modifier + self.typeString (type)

class DerivedDeclarator (CppDeclarator):
    def handleType (self, typename, type):
        typedef = self.typeString (type)
        self.write ('typedef %-26s %s;\n' % (typedef, typename))
        if self.ptoc00.isStringType (typedef):
            self.write ('typedef %-26s %s;\n' % (typedef + 'c', typename + 'c'))
            self.ptoc00.declareAsStringType (typename)
        elif self.ptoc00.isEnumType (typedef):
            self.write ('typedef %-26s %s;\n'
                    % (typedef + '_Enum', typename + '_Enum'))
            self.write ('typedef %s_Enum %s_Param;\n' % (typename, typename))
            self.ptoc00.declareAsEnumType (typename)

    def typeString (self, type):
       return type.typename ()

class RangeDeclarator (CppDeclarator):
    def typeString (self, type):
        lowerBound, upperBound = type.range ()
        lowerBound = self.ptoc00.constval (lowerBound)
        upperBound = self.ptoc00.constval (upperBound)
        if (-127 <= lowerBound) and (upperBound <= 128):
            reprType = int1Type
        elif (0 <= lowerBound) and (upperBound <= 255):
            reprType = uint1Type
        elif (-32000 <= lowerBound) and (upperBound <= 32000):
            reprType = int2Type
        elif (0 <= lowerBound) and (upperBound <= 64000):
            reprType = uint2Type
        else:
            reprType = int4Type
        return reprType

    def handleType (self, typename, type):
        CppDeclarator.handleType (self, typename, type)
        lowerBound, upperBound = type.range ()
        maxSymbol = upperBound
        maxValue = self.constval (upperBound) - self.constval (lowerBound) + 1
        self.ptoc00.enumMaxima [typename] = (maxSymbol, maxValue)

class EnumerationDeclarator (CppDeclarator):
    def handleType (self, typename, type, fullBlown = 1):
        type.setTypename (typename)
        write = self.write
        write ('\ntypedef enum %s_Enum\n{\n' % typename)
        for element in type.elements [:-1]:
            write ('    %s,\n' % element)
        write ('    %s\n' % type.elements [-1])
        write ('} %s_Enum;\n' % typename)
        write ('typedef %s_Enum %s_Param;\n' % (typename, typename))
        maxVal, count = type.extent ()
        type.declareValues (self.ptoc00.constants)
        if fullBlown:
            tstring = self.typeString (type, 'TYPE')
            tstring = tstring % typename
            write ('typedef %s;\n' % tstring)
        self.ptoc00.enumMaxima [typename] = type.extent ()
        self.ptoc00.declareAsEnumType (typename)

    def declareVar (self, varname, type):
        return self.declareVarByMacro (varname, type)

    def basicTypeString (self, type, fmt):
        maxVal, count = type.extent ()
        if count <= 256:
            reprType= uint1Type
        else:
            reprType = uint2Type
        typename = type.typename ()
        result = fmt % (typename, maxVal, reprType)
        return result

    def typeString (self, type, declkind):
        localEnumMacro = enumMacro % declkind
        fmt = localEnumMacro + ' (          %%s, %s_Enum, %s, %s)'
        return self.basicTypeString (type, fmt)

    def cppTypeString (self, type, modifier):
        fmt = enumTemplate
        return modifier + self.basicTypeString (type, fmt)

class SetDeclarator (CppDeclarator):
    def handleType (self, typename, type):
        elementType = type.elementType
        if elementType.__class__ == EnumerationDcl:
            declarator = self.getDeclarator (elementType)
            enumTypename = typename + '_'   # that's how gnu p2c does it
            declarator.handleType (enumTypename, elementType, None)
        str = self.typeString (type, 'TYPE')
        str = str % typename
        self.write ('typedef %s;\n' % str)
        self.ptoc00.declareAsSetType (typename)

    def declareVar (self, varname, type):
        return self.declareVarByMacro (varname, type)

    def setInfo (self, type):
        elementType = type.elementType
        minSymbol = None
        isEnum = 1
        isChar = None
        if elementType.__class__ == DerivedDcl:
            if elementType.typename () == 'char':
                maxSymbol = "255"
                maxValue = 255
                isEnum = None
                isChar = 1
            else:
                try:
                    elemtypeName = elementType.typename ()
                    maxSymbol, maxValue = self.ptoc00.enumMaxima [elemtypeName]
                    if self.ptoc00.types [elemtypeName].__class__ == RangeDcl:
                        isEnum = None
                except KeyError:
                    self.ptoc00.error ('no upper bound defined for ' + `elementType`)
        elif elementType.__class__ == RangeDcl:
            lowerBound, upperBound = elementType.range ()
            maxSymbol = upperBound
            minSymbol = lowerBound
            maxValue = self.constval (upperBound) - self.constval (lowerBound)
            isEnum = None
        else:
            maxSymbol, elementCount = elementType.extent ()
            maxValue = elementCount - 1
        if maxValue < 8:
            bytesRequired = 1
        else:
            bytesRequired = (maxValue + 1 + 15) / 16 * 2  # align to 2 bytes
        elemTypeName = elementType.typename ()
        if isEnum:
            elemTypeName = elemTypeName + '_Enum'
        elif isChar:
            elemTypeName = 'unsigned char'
        return elemTypeName, maxSymbol, bytesRequired

    def basicTypeString (self, type, fmt, fmt2):
        elementType = type.elementType
        if elementType.__class__ == DerivedDcl:
            if elementType.typename () == 'char':
                maxSymbol = "255"
                maxValue = 255
                fmt = fmt2
            else:
                try:
                    elemtypeName = elementType.typename ()
                    maxSymbol, maxValue = self.ptoc00.enumMaxima [elemtypeName]
                    if self.ptoc00.types [elemtypeName].__class__ == RangeDcl:
                        fmt = fmt2
                except KeyError:
                    self.ptoc00.error ('no upper bound defined for ' + `elementType`)
        elif elementType.__class__ == RangeDcl:
            lowerBound, upperBound = elementType.range ()
            maxSymbol = upperBound
            maxValue = self.constval (upperBound) - self.constval (lowerBound)
            fmt = fmt2
        else:
            maxSymbol, elementCount = elementType.extent ()
            maxValue = elementCount - 1
        if maxValue < 8:
            bytes = 1
        else:
            bytes = (maxValue + 1 + 15) / 16 * 2  # align to 2 bytes
        setClass = (fmt % (elementType.typename (), maxSymbol, bytes))
        return setClass

    def typeString (self, type, declkind):
        elemtype, maxSymbol, bytesRequired = self.setInfo (type)
        if bytesRequired == 1:
            fmt = smallSetMacro
            result = fmt % (declkind, elemtype, maxSymbol)
        else:
            fmt = setMacro
            result = fmt % (declkind, elemtype, maxSymbol, bytesRequired)
        return result

    def cppTypeString (self, type, modifier):
        elemtype, maxSymbol, bytesRequired = self.setInfo (type)
        if bytesRequired == 1:
            result = smallSetTemplate % (elemtype, maxSymbol)
        else:
            result = setTemplate %  (elemtype, maxSymbol, bytesRequred)
        return modifier + result

class PointerDeclarator (CppDeclarator):
    def handleType (self, typename, type):
        pointedToType = type.type
        if (pointedToType.__class__ == DerivedDcl) \
                and (not self.ptoc00.types.has_key (pointedToType.typename ())):
            tname = pointedToType.typename ()
            if not self.ptoc00.forwardTypes.get (tname):
                self.write ('typedef struct %s %s;\n' % (tname, tname))
                self.ptoc00.forwardTypes [tname] = 1
        typedef = self.baseTypeString (type)
        self.write ('typedef %-25s *%s;\n' % (typedef, typename))

    def declareVar (self, varname, type):
        typedef = self.baseTypeString (type)
        result = '%-18s *%s' % (typedef, varname)
        return result

    def baseTypeString (self, type):
        declarator = self.getDeclarator (type.type)
        return declarator.typeString (type.type)

    def cppTypeString (self, type, modifier):
        return modifier + self.baseTypeString (type) + ' * ' + modifier

class ArrayDeclarator (CppDeclarator):
    def handleType (self, typename, type):
        str = self.typeString (type, 'TYPE')
        str = str % typename
        self.write ('typedef %s;\n' % str)
        # try to declare C string
        cvariant = self.cstringVariant (type)
        if cvariant:
            str = cvariant % (typename + 'c')
            self.write ('typedef %s;\n' % str)
            self.ptoc00.declareAsStringType (typename)

    def declareVar (self, varname, type):
        return self.declareVarByMacro (varname, type)

    def mapBounds (self, bounds):
        lowerBound, upperBound = bounds
        try:
            indexType = self.ptoc00.types [upperBound]
            if type (indexType) == type(1):
                # builtin type, no explicit Type object
                maxSymbol, maxValue= self.ptoc00.enumMaxima [upperBound]
                lowerBound = '0'
                upperBound = maxSymbol
            elif indexType.__class__ == RangeDcl:
                lowerBound, upperBound = indexType.range ()
                lowerBound = `self.constval (lowerBound)`
                upperBound = `self.constval (upperBound)`
            else:
                maxSymbol, maxValue= self.ptoc00.enumMaxima [upperBound]
                lowerBound = '0'
                upperBound = maxSymbol
        except KeyError:
            pass
        if lowerBound == '1':
            lowerBound = ''
        else:
            lowerBound = ' - ' + lowerBound + ' + 1'
        return lowerBound, upperBound

    def basicTypeString (self, type, fmt):
        elementType = type.elementType
        bounds = type.bounds
        if len (bounds) == 1:
            lower0, upper0 = self.mapBounds (bounds [0])
            bounds = (upper0, lower0)
        elif len (bounds) == 2:
            lower0, upper0 = self.mapBounds (bounds [0])
            lower1, upper1 = self.mapBounds (bounds [1])
            bounds = (upper0, lower0, upper1, lower1)
        else:
            print 'Arrays with %d dimensions not implemented' % len (bounds)
        if elementType in ['char', 'tsp00_Byte', 'tsp00_Char']:
            result = fmt % bounds
        else:
            result = fmt % ((elementType,) + bounds)
        return result

    def typeString (self, type, declkind):
        elementType = type.elementType
        if len (type.bounds) == 1:
            if elementType == 'char':
                localArrayMacro = oldPascalStringMacro % declkind
                fmt = localArrayMacro + ' (  %%s, %s%s)'
            elif elementType == 'tsp00_Byte':
                localArrayMacro = byteArrayMacro % declkind
                fmt = localArrayMacro + ' (  %%s, %s%s)'
            elif elementType == 'tsp00_Char':
                localArrayMacro = pascalStringMacro % declkind
                fmt = localArrayMacro + ' (  %%s, %s%s)'
            else:
                localArrayMacro = arrayMacro % declkind
                fmt = localArrayMacro + ' (         %%s, %s, %s%s)'
        else:
            if elementType == 'char':
                localArrayMacro = oldPascalString2Macro % declkind
                fmt = localArrayMacro + ' ( %%s, %s%s, %s%s)'
            elif elementType == 'tsp00_Byte':
                localArrayMacro = byteArray2Macro % declkind
                fmt = localArrayMacro + ' ( %%s, %s%s, %s%s)'
            elif elementType == 'tsp00_Char':
                localArrayMacro = pascalString2Macro % declkind
                fmt = localArrayMacro + ' ( %%s, %s%s, %s%s)'
            else:
                localArrayMacro = array2Macro % declkind
                fmt = localArrayMacro + ' (        %%s, %s, %s%s, %s%s)'
        return self.basicTypeString (type, fmt)

    def cstringVariant (self, type):
        elementType = type.elementType
        if len (type.bounds) == 1:
            if elementType == 'char':
                localArrayMacro = cstringMacro
                fmt = localArrayMacro + '      (  %%s, %s%s)'
            else:
                return None
        else:
            return None
        return self.basicTypeString (type, fmt)

    def cppTypeString (self, type, modifier):
        elementType = type.elementType
        if len (type.bounds) == 1:
            if elementType == 'char':
                fmt = oldPascalStringTemplate
            elif elementType == 'tsp00_Byte':
                fmt = byteArrayTemplate
            elif elementType == 'tsp00_Char':
                fmt = pascalStringTemplate
            else:
                fmt = arrayTemplate
        else:
            print ('Arrays with %d dimensions not implemented'
                    % len (type.bounds))
        return modifier + self.basicTypeString (type, fmt)

class RecordDeclarator (CppDeclarator):
    def handleType (self, typename, type):
        for enum, name in type.enumerations ():
            enumTypeName = 't%s_%s' % (self.ptoc00.prefix, name)
            declarator = self.getDeclarator (enum)
            declarator.handleType (enumTypeName + '_', enum, None)
        if self.ptoc00.forwardTypes.has_key (typename):
            typedef, endName = '', ''
        else:
            typedef, endName = 'typedef', typename
        write = self.write
        write ('\n%s struct                     %s\n{\n' % (typedef, typename))
        self.writeFields (type.fields (), '    ')
        keywordEndToken = type.tokens [-2]
        variantDesc = type.variants ()
        if variantDesc:
            caseToken, selectorName, selectorType, variants = variantDesc
            self.handleConcom (caseToken)
            if selectorName:
                self.writeFields ([(selectorName, selectorType)], '    ')
            write ('    union\n    {\n')
            for vname, fields, endtoken in variants:
                self.handleConcom (vname)
                vname = vname.value
                write ('    /* %s */\n' % vname)
                write ('        struct %s_%s\n        {\n'
                        % (typename, vname))
                self.writeFields (fields, '            ', '_F')
                self.handleConcom (endtoken)
                write ('        } C_%s;\n' % vname)
            self.handleConcom (keywordEndToken)
            unionName = 'variant'
            write ('    } %s;\n' % unionName)
            write ('#if defined (__cplusplus)\n')
            for kind in ['', 'const ']:
                write ('    /* %saccessor functions */\n' % kind)
                for vname, fields, endtoken in variants:
                    self.handleConcom (vname)
                    vname = vname.value
                    structname = '%s.C_%s' % (unionName, vname)
                    self.writeFieldAccessors (fields, structname, kind, '    ')
                    self.handleConcom (endtoken)
                self.handleConcom (keywordEndToken)
            write ('#endif    /* defined (__cplusplus) */\n')
        else:
            self.handleConcom (keywordEndToken)
        write ('} %s;\n' % endName)

    def writeFields (self, fields, indent, suffix = ''):
        write = self.write
        for fieldname, token, type in fields:
            self.handleConcom (token)
            declarator = self.getDeclarator (type)
            str = declarator.declareVar (fieldname + suffix, type)
            write ('%s%s;\n' % (indent, str))

    def writeFieldAccessors (self, fields, structname, kind, indent):
        write = self.write
        for fieldname, token, type in fields:
            self.handleConcom (token)
            declarator = self.getDeclarator (type)
            typedef = declarator.cppTypeString (type, kind)
            write ('%s%-18s &%-22s () %s { return this->%s.%s_F; };\n'
                    % (indent, typedef, fieldname, kind,
                       structname, fieldname))


inlineC = regex.compile (' *inlineC')
prettyGenerated = ['ENDCASE', 'ENDIF', 'ENDWHILE', 'ENDREPEAT']

class ConcomCopy:
    concomRE = regex.compile (' *& *\([a-zA-Z]+\) *\(.*\)')

    def __init__ (self, outstream):
        self.outstream = outstream
        self.doCopy = 1

    def handleLine (self, line, stream):
        if self.doCopy:
            re = self.concomRE
            if re.match (line) == -1:
                raise 'Concom error', line
            keyword, parmstring = re.group (1, 2)
            keyword = lower (keyword)
            if keyword == 'ifdef':
                self.outstream.write ('\n#if defined (%s)\n\n' % strip (parmstring))
            elif keyword == 'ifndef':
                self.outstream.write ('\n#if !defined (%s)\n\n' % strip (parmstring))
            elif keyword == 'else':
                self.outstream.write ('\n#else\n\n')
            elif keyword == 'endif':
                self.outstream.write ('\n#endif\n\n')
            elif keyword == 'if':
                self.outstream.write ('\n#if %s\n\n' % self.concomcExpression (parmstring))
            else:
                raise 'Invalid Concom keyword', ('%s in %s' % (keyword, line))

    inRE = regex.compile (' *\([$a-z]+\) *in *[[]\(.*\)]', regex.casefold)
    eqRE = regex.compile (' *\([$a-z]+\) *= *\([a-z0-9]+\)', regex.casefold)

    def concomExpression (self, parmstring):
        inRE = self.inRE
        eqRE = self.eqRE
        if eqRE.match (parmstring) != -1:
            return 'defined (%s)' % eqRE.group (2)
        elif inRE.match (parmstring) != -1:
            words = splitfields (inRE.group (2), ',')
            words = map (lambda word: 'defined (%s)' % strip (word), words)
            return joinfields (words, ' || ')
        else:
            raise 'Invalid Concom expression', parmstring


class TokenBoundConcom:
    def attachScanner (self, scanner):
        self.scanner = scanner

    def handleLine (self, line, stream):
        self.scanner.concomLine.append (line)

# chapter convert tokens to string representations for C

def stringAsString (token):
    if token.escapeAll:
        prefix = '' # "(unsigned char *) "
        list = map (fullEscapeChar, token.value)
    else:
        prefix = ""
        list = map (escapeChar, token.value)
    return '%s "%s"' % (prefix, joinfields (list, ''))

def charAsString (token):
    ch = ord (token.value)
    if ch < 127:
        result = "'%s'" % escapeChar (token.value)
    else:
        result = '0x%x' % ch
    return result

def integerAsString (token):
    return token.value

def hexintAsString (token):
    return '0x%s' % token.value

def keywordAsString (token):
    result = token.value
    if result == 'div':
        result = '/'
    elif result == 'mod':
        result = '%'
    return result

def anyAsString (token):
    return token.value

_converterMap = {
        StringLiteral: stringAsString,
        CharacterLiteral: charAsString,
        IntegerLiteral: integerAsString,
        RealLiteral: anyAsString,
        HexLiteral: hexintAsString,
        Identifier: anyAsString,
        Operator: anyAsString,
        Keyword: keywordAsString,
        }

def tokenAsString (token):
    func = _converterMap [token.__class__]
    return func (token)

def buildEscapedMap ():
    map = "0123456789ABCDEF"
    result = [''] * 256
    for c in xrange (0, 32):
        highbyte = map [c / 16]
        lowbyte = map [c % 16]
        result [c] = '\\x%c%c' % (highbyte, lowbyte)
    result [ord ('\n')] = '\\n'
    result [ord ('\r')] = '\\r'
    for c in xrange (32, 127):
        ch = chr (c)
        result [c] = ch
    result [ord ('\\')] = '\\\\'
    result [ord ("'")] = "\\'"
    result [ord ('"')] = '\\"'
    for c in xrange (127, 256):
        highbyte = map [c / 16]
        lowbyte = map [c % 16]
        result [c] = '\\x%c%c' % (highbyte, lowbyte)
    return result

def buildFullEscapeMap ():
    map = "0123456789ABCDEF"
    result = [''] * 256
    for c in xrange (0, 256):
        highbyte = map [c / 16]
        lowbyte = map [c % 16]
        result [c] = '\\x%c%c' % (highbyte, lowbyte)
    return result

_charmap = buildEscapedMap ()
_fullEscapeMap = buildFullEscapeMap ()

def escapeChar (c):
    return _charmap [ord(c)]

def fullEscapeChar (c):
    return _fullEscapeMap [ord(c)]

def basename (fname):
    (result, ext) = path.splitext (path.basename (fname))
    return result

def ignoreConcom (line, stream):
    pass

class ConstantExtractor:
    def __init__ (self, scanner, constants, types, enumMaxima):
        self.constants = constants
        self.types = types
        self.enumMaxima = enumMaxima
        self.scanner = scanner

    def translate (self):
        try:
            scanner = self.scanner
            while 1:
                token = scanner.next ()
                if token.id != keywordID:
                    raise TokenExpected (token, ['CONST', 'TYPE'])
                    raise SyntaxError, (token, token.value, 'CONST or TYPE expected')
                if token.value == 'const':
                    readConstPart (scanner, self.handleConstantDecl)
                elif token.value == 'type':
                    readTypePart (scanner, self.handleTypeDecl)
                else:
                    raise TokenExpected (token, ['CONST', 'TYPE'])
                    raise SyntaxError, (token, token.value, 'CONST or TYPE expected')
        except EOFError:
            pass

    def constval (self, val):
        try:
            numval = int (val)
        except ValueError:
            try:
                numval = self.constants [val]
            except KeyError:
                numval = val
                #sys.stderr.write ('ptoc00: constant %s not defined\n' % val)
        return numval

    def handleConstantDecl (self, tokens):
        constname = tokens [0].value
        constdef = map (tokenAsString, tokens [2:-1])
        defAsString = join (constdef)
        if len (constdef) > 1:
            defAsString = '(' + defAsString + ')'
        try:
            value = self.constval (defAsString)
            self.constants [constname] = value
        except ValueError:
            pass

    def handleTypeDecl (self, startToken, type):
        typename = tokenAsString (startToken)
        self.types [typename] = type
        lastToken = type.tokens [-2]
        if lastToken.getAttribute ('postComment') == 'ptoc00Ignore':
            return
        if type.__class__ == SetDcl:
            type = type.elementType
        if type.__class__ == EnumerationDcl:
            self.handleEnumeration (typename, type)
        elif type.__class__ == RangeDcl:
            self.handleRange (typename, type)
        elif type.__class__ == RecordDcl:
            self.handleRecord (typename, type)

    def handleEnumeration (self, typename, enumeration):
        enumeration.declareValues (self.constants)
        elements = enumeration.elements
        self.enumMaxima [typename] = enumeration.extent ()

    def handleRange (self, typename, range):
        lowerBound, upperBound = range.range ()
        maxSymbol = upperBound
        maxValue = self.constval (upperBound) - self.constval (lowerBound) + 1
        self.enumMaxima [typename] = (maxSymbol, maxValue)

    def handleRecord (self, typename, type):
        enums = type.enumerations ()
        for enumeration, fieldname in enums:
            enumeration.declareValues (self.constants)

class ConstantExtractorVPath (ConstantExtractor):
    def __init__ (self, fname, constants, types, enumMaxima):
        instream = vdnStream.VDNStream (fname, ignoreConcom)
        instream.openPart ('code')
        scanner = Scanner (instream)
        ConstantExtractor.__init__ (self, scanner, constants, types, enumMaxima)

class ConstantExtractorVmake (ConstantExtractor):
    def __init__ (self, fnames, constants, types, enumMaxima):
        instream = MultiFileStream (fnames, ignoreConcom)
        scanner = Scanner (instream)
        ConstantExtractor.__init__ (self, scanner, constants, types, enumMaxima)

def initSymbolTables ():
    constants = {}
    types = {}
    for t in ['char', 'short', 'int', 'long', 'float', 'double',
            'boolean', 'bool', 'pasbool']:
        types [t] = 1
    enumMaxima = {'char': (255, 255), 'boolean': ('true', 1),
            'bool': ('true', 1), 'pasbool': ('true', 1)}
    return constants, types, enumMaxima

class PtoC00VPath (PtoC00):
    def __init__ (self, fname, includes, options):
        constants, types, enumMaxima = self.parseIncludes (includes)
        outstream = self.openOutstream (fname, includes, options)
        concom = TokenBoundConcom ()
        instream = vdnStream.VDNStream (fname, concom.handleLine)
        instream.openPart ('code')
        scanner = Scanner (instream)
        scanner.filename = fname
        concom.attachScanner (scanner)
        PtoC00.__init__ (self, scanner, outstream, constants, types, enumMaxima, options)
        self.prefix = fname [1:]


    def parseIncludes (self, includes):
        constants, types, enumMaxima = initSymbolTables ()
        for include in includes:
            ConstantExtractorVPath (include, constants, types, enumMaxima).translate ()
        return constants, types, enumMaxima

class PtoC00Vmake (PtoC00):
    def __init__ (self, sources, includes, options):
        logicalName = self.logicalName (sources)
        logicalIncludes = self.logicalIncludes (includes)
        constants, types, enumMaxima = self.parseIncludes (includes)
        outstream = self.openOutstream (logicalName, logicalIncludes, options)
        concom = TokenBoundConcom ()
        instream = MultiFileStream (sources, concom.handleLine)
        scanner = Scanner (instream)
        concom.attachScanner (scanner)
        PtoC00.__init__ (self, scanner, outstream, constants, types, enumMaxima, options)
        self.prefix = logicalName [1:]

    def logicalName (self, sources):
        result = basename (sources [0])
        return result

    def logicalIncludes (self, includes):
        result = []
        for include in includes:
            fname = basename (include)
            if not fname in result:
                result.append (fname)
        return result

    def parseIncludes (self, includes):
        constants, types, enumMaxima = initSymbolTables ()
        if len (includes) > 0:
            ConstantExtractorVmake (includes, constants, types, enumMaxima).translate ()
        return constants, types, enumMaxima

class MultiFileStream:
    def __init__ (self, fnames, concomHandler):
        self.fnames = fnames
        self.index = -1
        self.concomHandler = concomHandler
        self.hideText = None
        self.openNext ()

    def openNext (self):
        self.index = self.index + 1
        try:
            self.fname = self.fnames [self.index]
            self.lines = open (self.fname, "r").readlines ()
            if self.fname [-4:] == '.con':
                firstline = 'CONST\n'
            elif self.fname [-4:] == '.typ':
                firstline = 'TYPE\n'
            else:
                firstline = '\n'
            self.lines.insert (0, firstline)
            self.lineno = -1
        except IndexError:
            raise EOFError, self.fname

    def readline (self):
        line = self.__basicReadLine ()
        if line [0] == '&':
            self.concomHandler (line, self)
            return '\n'
        if self.hideText:
            return '\n'
        return line

    def __basicReadLine (self):
        self.lineno = self.lineno + 1
        try:
            result = self.lines [self.lineno]
        except IndexError:
            self.openNext ()
            result = self.__basicReadLine ()
        return result

