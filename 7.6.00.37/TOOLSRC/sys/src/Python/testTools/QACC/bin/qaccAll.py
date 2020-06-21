
# qaccAll.py
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


import os
import time

class Now:
    def __str__ (self):
        return time.strftime ("%Y-%m-%d %H:%M:%S", time.localtime (time.time ()))

__builtins__.now = Now ()


hasBIT32 = hasBIT64 = 'true'

_platforms = [
    ('aix',     'is8802', hasBIT32, hasBIT64),
    ('sun',     'us8803', hasBIT32, hasBIT64),
    ('hp',      'hs8802', hasBIT32, hasBIT64),
    ('dec',     'ds8800', None,     hasBIT64),
    ('sni',     'ss8802', hasBIT32, hasBIT64),
    ('linux',   'ls8800', hasBIT32, None),
]

mainCommand = r'd:\SAPDevelop\DevTool\python\python.exe D:\TOOLSRC\develop\sys\src\Python\testTools\QACC\bin\qaccMake.py '

def getRelDir (relString, requiresBitness, bitness):
    if not requiresBitness:
        return relString
    else:
        return relString + '_' + bitness


def parseSinglePlatform (platform, host, hasBIT64, relString, relDir):
    # main arg
    platform = ' -platform ' + platform
    remoteOwn = ' -OWN ftp://' + host + '/rel/' + relDir
    # bit 64
    if hasBIT64:
        bit64Option = ' -bit64 '
    else:
        bit64Option = ''
    relOption = ' -release ' + relString
    speedOption = ' -f '
    # build command
    options = platform + remoteOwn + bit64Option + relOption
    print options
    # execute command
    os.system (mainCommand + ' -f ' + options)

def parseSingleRelease (relString):
    for platform, host, hastBIT32, hasBIT64 in _platforms:
        if hasBIT32:
            parseSinglePlatform (platform, host, None, relString,
                getRelDir (relString, hastBIT32 and hasBIT64, '32'))
        if hasBIT64:
            parseSinglePlatform (platform, host, hasBIT64, relString,
                getRelDir (relString, hastBIT32 and hasBIT64, '64'))


def main (options, args):
    for relString in options.rel:
        print '======', relString, '========================'
        parseSingleRelease (relString)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('rel', None, '::', None, 'release string'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
