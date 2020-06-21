
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

ParseError = 'Cdcl.ParseError'

class Parameter:
    def __init__ (self, type, mode, name = '', isHelper = None, defaultVal = None):
        self.type = type
        self.mode = mode
        self.name = name
        self.isHelper = isHelper
        self.defaultVal = defaultVal

    def isOutput (self):
        return self.mode in ['OUT', 'INOUT']

    def isScriptingOutput (self):
        return (self.mode in ['OUT', 'INOUT']) and not self.isHelper

    def isInput (self):
        return self.mode in ['IN', 'INOUT']

    def isScriptingInput (self):
        return (self.mode in ['IN', 'INOUT']) and not self.isHelper

class Cdcl:
    def __init__ (self, str, pyname = None): #, inargs = None, outargs = None):
        match = _funcRE.match (str)
        if not match:
            raise ParseError, str
        self.name, returns, parmstring = match.group (2, 1, 3)
        if pyname:
            self.pyname = pyname
        else:
            self.pyname = self.name
        self.parms = parseParms (parmstring)
        self.returns = normalizeType (returns, 'OUT')
        if self.returns.type == 'void':
            self.returns = None
        if self.returns:
            self.returns.name = 'result'
            self.parms.append (self.returns)

_identifier = '([a-zA-Z_][a-zA-Z_0-9]*)'
_whitespace = '[' + string.whitespace + ']*'
_namedcl = _whitespace + '(.+[^a-zA-Z_0-9])' + _whitespace + _identifier
_parms = '[(](.*)[)]'
_helpPrefix = '(' + _whitespace + 'HELP' + ')?'
_funcRE = re.compile (_namedcl + _whitespace + _parms)
_namedclRE = re.compile (_namedcl)

def parseParms (s):
    if not s:
        return []
    list = string.splitfields (s, ',')
    allparms = []
    if (len (list) == 1) and isVoid (list [0]):
        return allparms
    for item in list:
        # parse default value
        try:
            pos = string.index (item, '=')
            defaultVal = item [pos + 1:]
            item =  item [:pos]
        except ValueError:
            defaultVal = None

        # split name and type
        item = string.strip (item)
        match = _namedclRE.match (item)
        if not match:
            raise ParseError, item
        name, parmtype = match.group (2, 1)
        parm = normalizeType (parmtype, 'IN')
        if (parm.isOutput ()) and (parm.type [-1] == '*'):
            parm.type = string.strip (parm.type [:-1])
        parm.name = name
        parm.defaultVal = defaultVal
        allparms.append (parm)
    return allparms

def isVoid (parmdcl):
    return string.strip (parmdcl) == 'void'

def normalizeType (typestring, defaultMode = 'IN'):
    # detect mode
    mode = defaultMode
    isHelper = None
    words = string.split (typestring)
    if words [0] == 'HELP':
        isHelper = 1
        del words [0]
    if words [0] in ['IN', 'OUT', 'INOUT']:
        mode = words [0]
        del words [0]
    typestring = string.join (words)

    # normalize actual type
    identpat = '\([a-zA-Z0-9_]\)'
    oppat = '\([][*()]\)'
    replacement = '\\1 \\2'
    typestring = re.compile (identpat + ' *' + oppat).sub (
                    replacement, string.strip (typestring))
    typestring = re.compile (oppat + ' *' + identpat).sub (
                    replacement, string.strip (typestring))
    typestring = re.compile (' +').sub (' ', typestring)
    result = Parameter (typestring, mode, isHelper = isHelper)
    return result

def buildParmList (parmnames, parms, returnType = None):
    result = []
    for name in parmnames:
        if name:
            parm = findParmNamed (parms, name)
        else:
            parm = (name, returnType)
        result.append (parm)
    return result

def findParmNamed (parms, name):
    for parm in parms:
        if parm [0] == name:
            return parm
    raise KeyError, name
