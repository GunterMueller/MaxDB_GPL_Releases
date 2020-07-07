#!/usr/bin/env python
# updCVS.py.py

import sys
import string
import os
import time
import traceback

import internalMail

import distribTools.perforce2cvs
p2c = distribTools.perforce2cvs

def timestamp ():
    return time.strftime ("%Y-%m-%d %H:%M:%S", time.localtime (time.time ()))

def sendSuccess (success):
    internalMail.sendMail ('perforce2cvs@sap.corp',
            'daniel.dittmar@sap.com', 'Success (%s)' % timestamp () , success)

def sendFailure (subject):
    internalMail.sendFailure ('perforce2cvs@sap.corp',
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
    options.p4 = p2c.perforceSession (options.perforceClient)
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
            updater = p2c.UpdCVS (arg, options.p4, options.verbose,
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
        ('upto', None, '', None, 'limit upper changelist'),
        ('a', 'argFile', ':', None, 'file containing arguments'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

