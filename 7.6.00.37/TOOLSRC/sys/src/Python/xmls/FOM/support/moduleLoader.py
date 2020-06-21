# moduleLoader.py
#
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
from types import ModuleType

def splitModulePath (pathString, seperator = '.'):
    objectPath = string.split (pathString, seperator)
    moduleName = objectPath [0]
    try:
        module = __import__ (moduleName)
    except ImportError:
        raise ModuleLoadError (moduleName)
    del objectPath [0]
    # print module
    for current in objectPath:
        nextName = moduleName + '.' + current
        try:
            __import__ (nextName)
        except ImportError, exc:
            break
        module = getattr (module, current)
        moduleName = nextName
    return module, moduleName

def loadModule (pathString, seperator = '.'):
    objectPath = string.split (pathString, seperator)
    moduleName = objectPath [0]
    try:
        module = __import__ (moduleName)
    except ImportError:
        raise ModuleLoadError (moduleName)
    del objectPath [0]
    # print module
    while len (objectPath) > 0:
        current = objectPath [0]
        if hasattr (module, current):
            break
        moduleName = moduleName + '.' + current
        try:
            __import__ (moduleName)
        except ImportError:
            break
        module = getattr (module, current)
        if type (module) != ModuleType:
            print 'break'
            break
        # print module
        del objectPath [0]
    object = module
    for name in objectPath:
        try:
            object = getattr (object, name)
        except AttributeError:
            raise AttributeLoadError (object, name)
        # print object
    return object

class LoadError:
    pass

class ModuleLoadError (LoadError):
    def __init__ (self, modulePath):
        self.modulePath = modulePath

    def __repr__ (self):
        return 'Module ' + self.modulePath + ' not found'

class AttributeLoadError (LoadError):
    def __init__ (self, object, searchedName):
        self.object = object
        self.searchedName = searchedName

    def __repr__ (self):
        return 'no attribute ' + self.searchedName + ' in ' + repr (self.object)

if __name__ == "__main__":
    print splitModulePath ('xmls.FOM.support.moduleLoader.loadModule')
    print loadModule ('xmls.FOM.support.moduleLoader.loadModule')

