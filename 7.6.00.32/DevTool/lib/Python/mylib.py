
# mylib.py              some common routines
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#


import sys
import string
import os
import getopt
import types
import time

def fatalError (errText, caller = None):
    if caller != None:
        errText = caller + ': ' + errText
    sys.stderr.write (errText + '\n')
    sys.exit (2)

def intArg (valText, optChar):
    try:
        result = string.atoi (valText)
    except:
        fatalError ('Argument to option -' + optChar + ' must be numeric')
    return result

def expandFileArgs (fileArgs):
    result = []
    for f in fileArgs:
        if f[0] == '@':
            lines = open (f[1:], "r").readlines ()
            for l in lines:
                result.append (l [:-1])
        else:
            result.append (f)
    return result

def makeStream (stream, mode = 'r'):
    if type (stream) == type (''):
        stream = open (stream, mode)
    return stream

def loadClass (className, moduleName = None):
    if moduleName == None:
        moduleName = className
    globals = {}
    locals = {}
    exec "from %s import %s; result = %s" % (moduleName, className, className) in globals, locals
    return locals ['result']

def loadFunction (functionName, moduleName):
    globals = {}
    locals = {}
    exec "from %s import %s; result = %s" % (moduleName, functionName, functionName) in globals, locals
    return locals ['result']

def loadDynamic (itemName, moduleName):
    globals = {}
    locals = {}
    exec "from %s import %s; result = %s" % (moduleName, itemName, itemName) in globals, locals
    return locals ['result']

def readConfig (fname, *keys):
    dict = {}
    execfile (fname, dict)
    if not keys:
        return dict
    resultList = []
    tupleType = types.TupleType
    for key in keys:
        if type (key) == tupleType:
            key, default = key
            try:
                resultList.append (dict [key])
            except KeyError:
                resultList.append (default)
        else:
            resultList.append (dict [key])
    if len (resultList) == 1:
        return resultList [0]
    else:
        return tuple (resultList)

def lookup (dict, key, default = None):
    try:
        result = dict [key]
    except KeyError:
        if default != None:
            result = default
        else:
            raise KeyError, key
    return result

def timeString (timeval = None):
    if timeval == None:
        timeval = time.localtime (time.time ())
    elif type (timeval) != type (()):
        timeval = time.localtime (timeval)
    return time.strftime ("%d. %b %Y", timeval)

class IndexIterator:
    def __init__ (self, collection, flatTuple = None):
        self.collection = collection
        if flatTuple:
            self.__getitem__ = self.getFlat
        else:
            self.__getitem__ = self.getStd

    def getStd (self, index):
        return (index, self.collection [index])

    def getFlat (self, index):
        return (index,) + self.collection [index]

