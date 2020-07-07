#!/usr/bin/env python
# fullBitKeeper.py
#
#    ========== licence begin LGPL
#    Copyright (C) 2003 SAP AG
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
import os

import distribTools.perforce2bk
p2bk = distribTools.perforce2bk

def fatalError (msg):
    sys.stderr.write ('fullBitKeeper: ' + msg + '\n')
    sys.exit (2)

def checkOptions (options):
    options.p4 = p2bk.perforceSession (options.perforceClient)
    if not options.bk or not os.path.isdir (options.bk):
        fatalError ('-bk ' + `options.bk` + ' is not a BitKeeper directory')
    options.bk = os.path.abspath (options.bk)
    os.chdir (options.bk)
    if not options.depot:
        options.depot, dummy = p2bk.loadPerforceLink (options.bk)
    if not options.depot:
        p2bk.fatalError ('-depot must be set')
    if options.changelist == -1:
        if options.current:
            options.changelist = p2bk.getCurrentChangelist (options.p4, options.depot)
        else:
            options.changelist = p2bk.getStartingChangelist (options.p4, options.depot)
    if options.noAction:
        options.verbose = 1


def main (options, args):
    checkOptions (options)
    bk = p2bk.FullBitKeeper (options.p4, options.depot, verbose = options.verbose,
        noAction = options.noAction, subdirs = args)
    bk.syncTo (options.changelist, options.cont)
    if options.noAction:
        pass
    elif args:
        # don't overwrite original perforceLink
        pass
    else:
        p2bk.writePerforceLink (options.bk, options.depot, options.changelist)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('bk', None, ':', None, 'BitKeeper dir'),
        ('depot', None, ':', None, 'perforce depot'),
        ('changelist', None, ':', -1, 'changelist to sync to'),
        ('current', None, '', None, 'sync to current state'),
        ('p4', 'perforceClient', ':', None, 'name of perforce client'),
        ('continue', 'cont', '', None, 'ignore existing files'),
        ('n', 'noAction', '', None, 'no action'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
