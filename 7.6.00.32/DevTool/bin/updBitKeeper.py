#!/usr/bin/env python
# updBitKeeper.py.py
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
import string
import os
import time
import traceback

import internalMail

import distribTools.perforce2bk
p2bk = distribTools.perforce2bk

def timestamp ():
    return time.strftime ("%Y-%m-%d %H:%M:%S", time.localtime (time.time ()))

def sendSuccess (success):
    internalMail.sendMail ('perforce2bk@sap.corp',
            'daniel.dittmar@sap.com', 'Success (%s)' % timestamp () , success)

def sendFailure (subject):
    internalMail.sendFailure ('perforce2bk@sap.corp',
            'daniel.dittmar@sap.com', '%s (%s)' % (subject, timestamp ()))

def expandArgFile (argFile, args):
    result = []
    if argFile:
        base = os.path.dirname (argFile)
        lines = open (argFile, 'r').readlines ()
        filenames = filter (None, map (string.strip, lines))
        for filename in filenames:
            result.append (os.path.join (base, filename))
    return result + args

def checkOptions (options):
    options.p4 = p2bk.perforceSession (options.perforceClient)
    if options.noAction:
        options.verbose = 1
        options.noMail = 1
    if options.upto:
        options.upto = int (options.upto)

def main (options, args):
    checkOptions (options)
    success = ''
    ok = 1
    args = expandArgFile (options.argFile, args)
    for arg in map (os.path.abspath, args):
        try:
            updater = p2bk.UpdBitKeeper (arg, options.p4, options.verbose,
                options.noAction)
            lastChange = updater.update (options.upto)
            if lastChange is None:
                msg = "%s => no changes\n" % arg
            else:
                msg = "%s => updated to %s\n" % (arg, lastChange)
            success = success + msg
        except:
            if not options.noMail:
                sendFailure (arg)
            traceback.print_exc ()
            ok = 0
    if success:
        if not options.noMail:
            sendSuccess (success)
    if not ok:
        sys.exit (3)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('p4', 'perforceClient', ':', None, 'name of perforce client'),
        ('n', 'noAction', '', None, 'no action'),
        ('noMail', None, '', None, 'don\'t send error or sucess mails'),
        ('upto', None, ':', None, 'limit upper changelist'),
        ('a', 'argFile', ':', None, 'file containing arguments'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

