
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

_splitfields = string.splitfields
_strip = string.strip

TypeDict = {}

class CType:
    def __init__ (self, name, typeChar):
        self.name = normalizeType (name)
        self.typeChar = typeChar
        TypeDict [self.name] = self

    def convertedVar (self, varname):
        return self.name
        
    def asNativeParameter (self, varname):
        return self.convertedVar (varname)

    def asPythonParameter (self, varname, parseTuple = None):
        str = varname
        if parseTuple:
            str = '&' + str
        return str

    def declaration (self, varname, stream, indent = ''):
        str = '%s    %s;' % (self.name, varname)
        self.genCode ([str], stream, indent)
   
    def genToPy (self, varname, stream, indent = ''):
        pass

    def genFromPy (self, varname, stream, indent = ''):
        pass

    def genFree (self, varname, stream, indent = ''):
        pass

    def genCode (self, lines, stream, indent = ''):
        for line in lines:
            stream.write (indent + line + '\n')

            
class CBuiltin (CType):
    def __init__ (self, name, typeChar):
        CType.__init__ (self, name, typeChar)

        
class CString (CBuiltin):
    def __init__ (self, name):
        CBuiltin.__init__ (self, name, 's')
    
    def genFree (self, varname, stream, indent = ''):
        stream.write (indent + 'free (%s);\n' % varname)

        
class CPointerBuiltin (CBuiltin):
    def __init__ (self, name, typeChar):
        CBuiltin.__init__ (self, name, typeChar)

    def asNativeParameter (self, varname):
        return '&' + varname
        
        
class CObject (CType):
    def __init__ (self, name, fromPy, toPy, free):
        CType.__init__ (self, name, 'o')
        self.fromPy = _makeBlock (fromPy)
        self.toPy = _makeBlock (toPy)
        self.free = _makeBlock (free)

    def declaration (self, varname, stream, indent = ''):
        pyVar = 'PyObject*    %s;' % varname
        nativeVar = '%s    %s;' % (self.name, self.convertedVar (varname))
        self.genCode ([pyVar, nativeVar], stream, indent)
   
    def convertedVar (self, varname):
        return self.name + 'Conv'
        
    def genToPy (self, stream, indent = ''):
        self.genCode (self.toPy)

    def genFromPy (self, stream, indent = ''):
        self.genCode (self.fromPy)

    def genFree (self, stream, indent = ''):
        self.genCode (self.free)

        
def _makeBlock (str):
    lines = _splitfields (str, '\n')
    #lines = map (_strip, lines)
    return lines

def fillStandardTypes (dict):
    builtinTypes = [ ('int', 'i'), ('long', 'l'), ('char', 'c'),
                            ('float', 'f'), ('double', 'd')]
    for name, typeChar in builtinTypes:
        newType = CBuiltin (name, typeChar)
    for name, typeChar in builtinTypes:
        newType = CPointerBuiltin (name, typeChar)
    newType = CString ('char*')
    
def makeSynonym (typename, synonymFor, dict = TypeDict):
    dict [normalizeType (typename)] = dict [normalizeType (synonymFor)]

def getType (typename, dict = TypeDict):
    return dict [normalizeType (typename)]
    
_identPat = '\([a-zA-Z_][a-zA-Z_0-1]*\)'
_wsPat = '[' + string.whitespace + ']+'
_identSPidentRE = re.compile (_identPat + _wsPat + _identPat)
_wsRE = re.compile (_wsPat)

def normalizeType (typeString):
    typeString = _strip (typeString)
    typeString = _identSPidentRE.sub ('\\1\0\\2', typeString)
    # repeat so overlapping patterns are handled OK
    typeString = _identSPidentRE.sub ('\\1\0\\2', typeString)
    typeString = _wsRE.sub ('', typeString)
    typeString = typeString.replace ('\0', ' ')
    return typeString


# initialization

fillStandardTypes (TypeDict)
