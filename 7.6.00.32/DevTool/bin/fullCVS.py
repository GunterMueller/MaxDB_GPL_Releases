#!/usr/bin/env python
# fullCVS.py

import os

import distribTools.perforce2cvs
p2c = distribTools.perforce2cvs

def checkOptions (options):
    p4port, depot, lastChange, noOpenSource = p2c.loadPerforceLink (options.cvs)
    options.p4 = p2c.perforceSession (options.perforceClient, port = p4port)
    options.noOpenSource = noOpenSource
    if not options.cvs or not os.path.isdir (options.cvs):
        fatalError ('-cvs ' + `options.cvs` + ' is not a cvs directory')
    options.cvs = os.path.abspath (options.cvs)
    os.chdir (options.cvs)
    if not options.depot:
        options.depot= depot
    if not options.depot:
        p2c.fatalError ('-depot must be set')
    if options.changelist == -1:
        if options.current:
            options.changelist = p2c.getCurrentChangelist (options.p4, options.depot)
        elif options.resync:
            options.changelist = lastChange
        else:
            options.changelist = p2c.getStartingChangelist (options.p4, options.depot)
    if options.noAction:
        options.verbose = 1


def main (options, args):
    checkOptions (options)
    cvs = p2c.FullCVS (options.p4, options.depot, verbose = options.verbose,
        noAction = options.noAction, subdirs = args, noOpenSource = options.noOpenSource)
    if options.resync:
        cvs.resyncTo (options.changelist, options.quick)
    else:
        cvs.syncTo (options.changelist, options.cont)
    if options.noAction:
        pass
    elif args:
        # don't overwrite original perforceLink
        pass
    else:
        p2c.writePerforceLink (options.cvs, options.depot, options.changelist)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('cvs', None, ':', None, 'cvs dir'),
        ('depot', None, ':', None, 'perforce depot'),
        ('changelist', None, ':', -1, 'changelist to sync to'),
        ('current', None, '', None, 'sync to current state'),
        ('p4', 'perforceClient', ':', None, 'name of perforce client'),
        ('continue', 'cont', '', None, 'ignore existing files'),
        ('resync', None, '', None, 'make sure that cvs matches current perforce'),
        ('quick', None, '', None, 'with -resync: don\'t compare file contents'),
        ('n', 'noAction', '', None, 'no action'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
