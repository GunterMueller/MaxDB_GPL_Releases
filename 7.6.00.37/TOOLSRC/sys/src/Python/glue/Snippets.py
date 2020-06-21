
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
import re

class Snippets:
    ws = '[' + string.whitespace + ']*'
    def __init__ (self, data, startPattern, endPattern):
        self.data = data
        self.startPattern = startPattern
        self.endPattern = endPattern
        self.endPatternRE = re.compile (endPattern)

    def getSnippet (self, key):
        data = self.data
        match = re.search (self.startPattern + self.ws + key, data)
        if not match:
            raise KeyError, key
        try:
            start = match.end ()
            start = string.index (data, '\n', start) + 1
        except:
            raise KeyError, key
        match = self.endPatternRE.search (data, start)
        if not match:
            raise KeyError, key
        return data [start:match.start ()]

    __getitem__ = getSnippet
    
    def keys (self):
        result = []
        data = self.data
        rex = re.compile (self.startPattern)
        match = rex.search (data)
        find = string.find
        while match:            
            start = match.start ()
            end = find (data, '\n', start)
            keyString = data [start + len (match.group (0)):end]
            keyString = string.strip (keyString)
            if keyString [-2:] == '*/':
                keyString = string.strip (keyString [:-2])
            if keyString:
                result.append (keyString)
            match = rex.search (data, end)
        return result

    def values (self):
        keys = self.keys ()
        result = []
        for key in keys:
            result.append (self.getSnippet (key))
        return result

    def items (self):
        keys = self.keys ()
        result = []
        for key in keys:
            result.append ((key, self.getSnippet (key)))
        return result
        

    def subSnippets (self, subkey):
        return SubSnippets (self, subkey)

    def longname (self, key):
        return key

defaultStartPattern = '/\*snippet '
defaultEndPattern = '/\*endsnippet '

class SnippetsFile (Snippets):
    def __init__ (self, fname, startPattern = defaultStartPattern, endPattern = defaultEndPattern):
        data = open (fname, "r").read ()
        Snippets.__init__ (self, data, startPattern, endPattern)

class SnippetsString (Snippets):
    def __init__ (self, data, startPattern = defaultStartPattern, endPattern = defaultEndPattern):
        Snippets.__init__ (self, data, startPattern, endPattern)

class SubSnippets (Snippets):
    def __init__ (self, snippetObject, subkey):
        Snippets.__init__ (self, snippetObject.data,
            snippetObject.startPattern + self.ws + subkey,
            snippetObject.endPattern)
        self.subkey = subkey

    def longname (self, key):
        return self.subkey + ' ' + key

