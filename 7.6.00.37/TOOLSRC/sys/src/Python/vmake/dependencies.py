
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

import os
import time
import cPickle

import vmakeLib

_dayMillisC = 24 * 60 * 60 * 1000


_speedMapping = {
    's': 'slow',
    'q': 'quick',
    'f': 'fast',
}

class VmakeDependencies:
    def __init__ (self, target = 'all', speed = 's'):
        self.loadVmakeData (target, speed)
        mainDependencies = DependenciesSet ()
        includeDependencies = DependenciesSet ()

    def getCachePath (self, speed):
        return (os.environ ['OWN'] + '/sys/wrk/' + _speedMapping [speed]
                + '/all.vmake.pycache')

    def loadVmakeData (self, target, speed):
        cachePath = self.getCachePath (speed)
        dataLoaded = None
        if os.path.exists (cachePath):
            modified = os.path.getmtime (cachePath)
            now = time.time ()
            if now - modified < _dayMillisC:
                self.all = cPickle.load (open (cachePath, 'rb'))
                dataLoaded = 1
        if not dataLoaded:
            self.callVmake (speed)
            cPickle.dump (self.all, open (cachePath, 'wb'), 1)

    def callVmake (self, speed):
        self.all = {}
        parser = vmakeLib.VmakeParser ()
        parser.registerHandler (None, self.targetEvent)
        parser.parseCollection (['all', 'allknl'], speed)

    def targetEvent (self, target):
        if target.version == '':
            target.version = vmakeLib.independentVersion
        self.all [(target.name, target.version)] = target

    def dependentsOf (self, headerList, targetList = None):
        queue = OnceQueue ()
        for headerName in headerList:
            try:
                target = self.all [(headerName, vmakeLib.independentVersion)]
            except KeyError:
                keys = self.all.keys ()
                keys.sort ()
                for key in keys:
                    name = key [0]
                    if name == headerName:
                        print key
                raise
            queue.add (target, target.asKey ())
        modules = {}
        while not queue.isEmpty ():
            target = queue.next ()
            if not hasattr (target, 'callers'):
                continue
            for module in target.callers:
                subtarget = self.all [module]
                if subtarget.kind == 'module':
                    modules [module] = 1
                queue.add (subtarget, subtarget.asKey ())
        if targetList:
            targetModules = self.modulesOf (targetList)
            result = []
            for module in modules.keys ():
                if targetModules.has_key (module):
                    result.append (module)
        else:
            result = modules.keys ()
        return result

    def modulesOf (self, targetList):
        queue = OnceQueue ()
        targetList = map (macname, targetList)
        for targetName in targetList:
            for speed in "sqf ":
                target = None
                try:
                    target = self.all [(targetName, speed)]
                    queue.add (target, target.asKey ())
                except KeyError:
                    pass
                if target != None:
                    continue
        modules = {}
        while not queue.isEmpty ():
            target = queue.next ()
            if not hasattr (target, 'dependencies'):
                continue
            for module in target.dependencies:
                try:
                    subtarget = self.all [module]
                except KeyError:
                    continue
                if subtarget.kind == 'module':
                    modules [module] = 1
                queue.add (subtarget, subtarget.asKey ())
        return modules

def macname (name):
    if name [-4:] != '.mac':
        name = name + '.mac'
    return name

class DependenciesSet:
    def __init__ (self):
        self.dict = []

    def add (self, base, newDep):
        try:
            dict = self.dict [base]
        except KeyError:
            dict = {}
            self.dict [base] = dict
        dict [newDep] = 1

    def dependenciesOf (self, base):
        pass


class OnceQueue:
    def __init__ (self, seen = None):
        self.seen = {}
        self.queue = []

    def add (self, item, name):
        if not self.seen.has_key (name):
            self.queue.append (item)
            self.seen [name] = 1

    def isEmpty (self):
        return len (self.queue) == 0

    def next (self):
        result = self.queue [0]
        del self.queue [0]
        return result



