
# projectInfo.py
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
import os

import config

class ProjectParser:
    def __init__ (self):
        self.configurations = {}
        self.targetTypes = {}

    def readInput (self, dirname):
        files = os.listdir (dirname)
        for fname in files:
            fullname = os.path.join (dirname, fname)
            self.parseProject (fullname)
        return self

    configRE = re.compile ('!MESSAGE "[^ ]+ - ([^"]+)" [(]based on ')
    targTypeRE = re.compile ('# TARGTYPE "Win32 [(]x86[)] ([^"]+)" 0x([0-9a-f]+)')
    def parseProject (self, fname):
        partRE = re.compile ('!MESSAGE "[^ ]+ - ([^"]+)"')
        for line in open (fname, 'rt').readlines ():
            match = self.configRE.match (line)
            if match:
                configName = match.group (1)
                self.configurations [configName] = 1
            match = self.targTypeRE.match (line)
            if match:
                kindName, kindCode = match.group (1, 2)
                self.targetTypes [(kindName, kindCode)] = 1

    def dump (self):
        configurations = self.configurations.keys ()
        configurations.sort ()
        print 'Configurations'
        for config in configurations:
            print '    ' + config
        print 'Target Types'
        targetTypes = self.targetTypes.keys ()
        targetTypes.sort ()
        for kindName, kindCode in targetTypes:
            print '    ' + kindName, kindCode

def projectInfo ():
    parser = ProjectParser ()
    parser.readInput (config.inputDsp)
    parser.dump ()

def main (options, args):
    projectInfo ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
