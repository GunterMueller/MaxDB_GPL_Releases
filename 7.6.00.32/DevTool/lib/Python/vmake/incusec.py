#!/bin/env python
# incusec.py
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
import re
import os
from vmake import yutil

version = "0.8"

class IncludeChecker:
    errors = 0
    displayName = '<unknwon>'

    def checkFile (self, fname, displayName = None):
        source = open (fname, 'r').read ()
        if displayName == None:
            displayName= fname
        self.displayName = displayName
        self.checkSource (source)

    def checkVDN (self, sourceName):
        if os.path.exists (sourceName):
            self.checkFile (sourceName)
        else:
            fullName = yutil.findFile (sourceName)
            self.checkFile (fullName, sourceName)

    def checkSource (self, source):
        rex = re.compile ('#include +"\([gh][a-z][a-z][0-9]+\)[.]h"')
        pos = 0
        match = rex.search (source, pos)
        while match:
            pos = match.end ()
            name = match.group (1)
            found = self.checkFor (name, source, pos)
            if not found:
                print self.displayName + (': header %s.h unnecessary' % name)
                self.errors = 1
            match = rex.search (source, pos)
        del self.displayName

    def checkFor (self, name, source, pos):
        id = name [1:]
        layer = name [1:3]
        number = name [3:]
        patterns = [id, string.upper (id)]
        if number [:2] == '00':    # possibly old pascal header
            patterns.append (layer + number [2:]) # allow abreviated form sp1
        elif name [0] == 'h':     # possibly old pascal functions
            patterns.append (layer [0] + number)
        # print patterns
        for pattern in patterns:
            if string.find (source, pattern, pos) != -1:
                return 1
        return 0

def main (options, args):
    """
    Tries to detect unnecessary includes by searching
    for identifiers conforming to the naming convention.
    """
    checker = IncludeChecker ()
    for arg in args:
        checker.checkVDN (arg)
    if checker.errors:
        sys.exit (1)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('version', None, '', version, 'version information'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
