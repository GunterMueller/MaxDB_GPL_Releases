
import sys
import string
from parserUtil import readUntil, splitTokens, tokenUntil, tokenUntilAny, tokenNested
from pasScanner import *

class SyntaxError:
    def __init__ (self, token, msg = ''):
        self.token = token
        self.msg = msg
        self.writeError ()

    def writeError (self):
        pos = self.token.pos
        if pos == None:
            prefix = ''
        else:
            prefix = '%s (%s:%d): ' % pos
        errmsg = prefix + self.formatError ()
        sys.stderr.write (errmsg + '\n')

    def formatError (self):
        return 'SyntaxError near %: %s' % (`self.token`, self.msg)

class TokenExpected (SyntaxError):
    def __init__ (self, token, expectedList):
        self.expectedList = expectedList
        SyntaxError.__init__ (self, token)

    def formatError (self):
        expectedList = self.expectedList
        token = self.token
        if (type (expectedList) != type ([])) or (len (expectedList) == 1):
            result = 'Found %s, expected %s' % (`token`, expectedList [0])
        else:
            result = 'Found %s, expected one of %s' % (
                    `token`, string.joinfields (expectedList, ', '))
        return result

def readConstPart (scanner, handler):
    inConsts = 1
    while inConsts:
        startToken = scanner.next ()
        if startToken.id != identifierID:
            inConsts = None
            scanner.pushback (startToken)
            break
        tokens = readUntil (scanner, operatorID, ';', startToken)
        handler (tokens)

_equalToken = (operatorID, '=')
_commaToken = (operatorID, ',')
_colonToken = (operatorID, ':')
_ppToken = (operatorID, '..')
_caseToken = (keywordID, 'case')
_endToken = (keywordID, 'end')
_packedToken = (keywordID, 'packed')
_recordToken = (keywordID, 'record')

def readTypePart (scanner, handler):
        inTypes = 1
        while inTypes:
            startToken = scanner.next ()
            if startToken.id != identifierID:
                inTypes = None
                scanner.pushback (startToken)
                break
            nextFew = scanner.nextFew (2)
            if nextFew [0].asTuple () != _equalToken:
                raise TokenExpected (nextFew [0], ['='])
                raise 'SyntaxError', 'found %s instead of =' % nextFew [0].value
            kindToken = nextFew [1].asTuple ()
            if kindToken == _packedToken:
                nextOne = scanner.next ()
                kindToken = nextOne.asTuple ()
                nextFew.append (nextOne)
            if kindToken == _recordToken:
                #print 'reading record', startToken.value
                rest = readUntil (scanner, keywordID, 'end')
                rest.append (scanner.next ())
            else:
                #print 'reading other type', kindToken, startToken.value
                rest = readUntil (scanner, operatorID, ';')
            tokens = [startToken] + nextFew + rest
            type = parseTypes (nextFew [-1:] + rest)
            handler (startToken, type)

class RoutDcl:
    def __init__ (self, name):
        self.name = name
        self.returnType = None
        self.parms = []

def parseRoutDcl (scanner, startToken):
    nameToken = scanner.next ()
    routdcl = RoutDcl (nameToken.value)
    nextOne = scanner.next ()
    tokens = [startToken, nameToken, nextOne]
    if nextOne.value == '(':
        parmTokens = readUntil (scanner, operatorID, ')', nextOne)
        tokens = tokens + parmTokens
        nextOne = scanner.next ()
        routdcl.parms = breakParmTokens (parmTokens)
    tokens.append (nextOne)
    if nextOne.value == ':':
        typetoken = scanner.next ()
        routdcl.returnType = typetoken.value
        tokens.append (nextOne)     # read ;
        tokens.append (scanner.next ())
    routdcl.dcltokens = tokens
    return routdcl

def breakParmTokens (tokens):
    result = []
    pos = 1
    tokenCount = len (tokens) - 1       # ignore brace
    while pos < tokenCount:
        if tokens [pos].value == 'var':
            pos = pos + 1
            var = 'var'
        else:
            var = ''
        parmname = tokens [pos].value
        if tokens [pos + 1].value != ':':
            raise TokenExpected (tokens [pos + 1], [':'])
            raise 'SyntaxError', (': expected instead of ' + tokens [pos + 1].value,
                                    tokens [pos + 1].pos)
        type = tokens [pos + 2].value
        result.append ((var, parmname, type))
        pos = pos + 4
    return result

#
# chapter parsing of type declarations
#

def parseTypes (tokens):
    selector = tokens [0].value
    if selector == 'packed':
        del tokens [0]
        selector = tokens [0].value
    try:
        constructor = selectorMap [selector]
        result = constructor (tokens)
    except KeyError:
        if tokens [-1].value == ';':
            addOn = 1
        else:
            addOn = 0
        if len (tokens) == 1 + addOn:
            result = DerivedDcl (tokens)
        elif tokens [1].value == '..':
            result = RangeDcl (tokens)
        else:
            raise 'Cannot parse', tokens
    return result

class DerivedDcl:
    def __init__ (self, tokens):
        self.tokens = tokens
        self.sourceToken = tokens [0]

    def typename (self):
        return self.sourceToken.value

    def __repr__ (self):
        return '<DerivedDcl %s>' % self.typename ()

class RangeDcl:
    def __init__ (self, tokens):
        self.tokens = tokens

    def range (self):
        lowerbound = self.tokens [0].value
        upperbound = self.tokens [2].value
        return (lowerbound, upperbound)

    def extent (self):
        upperbound = self.tokens [2].value
        return (upperbound, upperbound)

    def typename (self):
        return 'int'

class PointerDcl:
    def __init__ (self, tokens):
        self.tokens = tokens
        self.type = parseTypes (tokens [1:])

class EnumerationDcl:
    def __init__ (self, tokens):
        self.tokens = tokens
        elements = []
        for token in tokens [1:-2]:   # skip braces and ';'
            if token.value != ',':
                elements.append (token.value)
        self.elements = elements

    def extent (self):
        return (self.elements [-1], len (self.elements) - 1)

    def declareValues (self, dict):
        elements = self.elements
        for i in xrange (len (elements)):
            dict [elements [i]] = i

    def typename (self):
        return self.__typename

    def setTypename (self, name):
        self.__typename = name

class SetDcl:
    def __init__ (self, tokens):
        self.tokens = tokens
        self.elementType = parseTypes (tokens [2:])

class ArrayDcl:
    def __init__ (self, tokens):
        self.tokens = tokens
        if tokens [0].value == 'string':
            self.parseStringType (tokens)
        else:
            if tokens [-1].value == ';':
                self.elementType = tokens [-2].value
            else:
                self.elementType = tokens [-1].value
            boundsTokenLists = splitTokens (tokens [2:], operatorID, ',')
            bounds = []
            for tokenList in boundsTokenLists:
                if tokenList [1].value == '..':
                    lowerBound = tokenList [0].value
                    upperBound = tokenList [2].value
                else:
                    lowerBound = "1"
                    upperBound = tokenList [0].value
                bounds.insert (0, (lowerBound, upperBound))
            self.bounds = bounds

    def parseStringType (self, tokens):
        lowerBound = "1"
        if tokens [1].value == '[':
            upperBound = tokens [2].value
        else:
            upperBound = "1"
        self.bounds = [(lowerBound, upperBound)]
        self.elementType = 'char'

class RecordDcl:
    def __init__ (self, tokens):
        self.tokens = tokens
        self.parsed = None

    def parse (self):
        pos = 1
        tokens = self.tokens
        stopTokens = [_endToken, _caseToken]
        fieldTokens = tokenUntilAny (tokens, pos, stopTokens)
        self.__fields= self.parseFields (fieldTokens [:-1])
        if fieldTokens [-1].value == 'case':
            self.parseVariant (tokens, pos + len (fieldTokens), fieldTokens [-1])
        else:
            self.__variantDescription = None
        self.parsed = 1

    def parseFields (self, tokens):
        result = []
        if len (tokens) == 0:
            return result
        parts = splitTokens (tokens, operatorID, ';')
        if len (parts [-1]) == 0:
            del parts [-1]
        for part in parts:
            name = part [0].value
            type = parseTypes (part [2:])
            result.append ((name, part [0], type))
        return result

    def parseVariant (self, tokens, pos, caseToken):
        if tokens [1] == _colonToken:
            selectorName = tokens [0].value
            selectorType = tokens [2].value
            pos = pos + 4
        else:
            selectorName = None
            selectorType = tokens [0].value
            pos = pos + 2
        variants = []
        while tokens [pos].value != 'end':
            vname = tokens [pos]
            otherSelectors = tokenUntil (tokens, pos + 1, operatorID, ':')
            pos = pos + len (otherSelectors) + 1
            fieldTokens = tokenNested (tokens, pos, (operatorID, '('),
                [(operatorID, '(', ')')])
            pos = pos + len (fieldTokens)
            if tokens [pos].value == ';':
                pos = pos + 1
            fields = self.parseFields (fieldTokens [1:-1])
            variants.append ((vname, fields, fieldTokens [-1]))
        self.__variantDescription = (caseToken, selectorName, selectorType, variants)

    def fields (self):
        if not self.parsed:
            self.parse ()
        return self.__fields

    def variants (self):
        if not self.parsed:
            self.parse ()
        return self.__variantDescription

    def enumerations (self):
        if not self.parsed:
            self.parse ()
        enums = []
        for name, token, type in self.__fields:
            if type.__class__ == SetDcl:
                type = type.elementType
            if type.__class__ == EnumerationDcl:
                enums.append ((type, name))
        if self.__variantDescription:
            caseToken, variantName, variantType, variants = self.__variantDescription
            for vname, fields, endtoken in variants:
                for name, token, type in fields:
                    if type.__class__ == SetDcl:
                        type = type.elementType
                    if type.__class__ == EnumerationDcl:
                        enums.append ((type, name))
        return enums

    def compactInfo (self):
        fields = self.fields ()
        if self.__variantDescription:
            caseToken, selName, selType, variants = self.__variantDescription
            selector = (selName, selType)
        else:
            selector = None
            variants = []
        enumerations = self.enumerations ()
        return (fields, selector, variants, enumerations)

selectorMap = {
        '^': PointerDcl,
        '(': EnumerationDcl,
        'set': SetDcl,
        'array': ArrayDcl,
        'string': ArrayDcl,
        'record': RecordDcl,
        }


nestedStatements = [
        (keywordID, 'begin', 'end'),
        (keywordID, 'case', 'end'),
        (keywordID, 'record', 'end'),
        ]
