
# baseModules.py
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
import os

platforms = [
    'linux',
    'win32',
]

def backtick (cmd):
    "read output of shell command"
    lines = os.popen (cmd, 'rt').readlines ()
    return lines

def isModule (line):
    if line [:4] == 'src/':
        return 1
    if line [:5] == 'desc/':
        return 1
    return 0

def remims (platform, targets):
    "call remims -m on platform"
    return backtick ('remims ' + platform + ' -m ' + targets)

def localims (targets):
    "call ims locally"
    return backtick ('perl -S ims.pl -m ' + targets)

def baseModules (options, targets):
    "create a list of distinct modules required to compile targets"
    moduleSet = {}
    targetString = string.join (targets)
    for platform in platforms:
        if platform == 'win32':
            lines = localims (targetString)
        else:
            lines = remims (platform, targetString)
        lines = filter (isModule, lines)
        for line in lines:
            moduleSet [line] = 1
    modules = moduleSet.keys ()
    modules.sort ()
    open (options.outfile, 'wt').writelines (modules)


def main (options, args):
    "create a list of distinct modules required to compile targets"
    if len (args) == 0:
        args = ['all']
    baseModules (options, args)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outfile', ':', 'basefiles.dta', 'name of output file'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
