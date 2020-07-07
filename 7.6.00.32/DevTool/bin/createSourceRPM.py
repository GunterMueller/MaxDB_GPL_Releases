#!/usr/bin/env python
# createSourceRPM.py
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

from distribTools import rpmLib

def checkOptions (options):
    options.p4 = rpmLib.Perforce (options.p4client)
    if options.sapdbRel is None:
        import optlib
        optlib.fatalError ('must specify -sapdbRel x.x.x.x')

def main (options, args):
    checkOptions (options)
    rpmLib.copyToolSources (options)
    rpmLib.copyMainSources (options)
    rpmLib.packRPM (options)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('c', 'p4client', ':', None, 'perforce client'),
        ('toolSync', None, ':', None, 'sync $TOOL to this revision'),
        ('mainSync', None, ':', None, 'sync main sources to this revision'),
        ('sapdbRel', None, ':', rpmLib.getDefaultRel (), 'SAP DB release'),
        ('packDir', None, ':', '.', 'local source mirror'),
        ('clear', None, '', None, 'clear target directories'),
        ('u', 'unconditional', '', None, 'copy unconditional'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('V', 'veryVerbose', '', None, 'very verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
