#!/usr/bin/env python
# copy32to64.py

import sys
import os
import string
import shutil

_programName = 'copy32to64'
_toolCommand = os.environ['TOOLSHELL'] + ' %s' + os.environ['TOOLEXT'] + ' '
_toolCommand = '%(TOOLSHELL)s %(TOOL)s/bin/%%s%(TOOLEXT)s ' % os.environ

class Copy32to64:
    def __init__ (self, runMake, verbose, own32):
        self.status = 0
        self.runMake = runMake
        self.verbose = verbose
        self.own32 = own32
        if own32 != os.environ.get ('OWN'):
            self.cmdPattern = '(unset BIT64; cd %s; OWN=%s; . .iprofile; set; %%s)' % (own32, own32)
        else:
            self.cmdPattern = '%s'
        self.targets = []
        self.files = []

    def getStatus (self):
        return self.status

    def log (self, text, forContinue = None):
        if self.verbose:
            if forContinue:
                print text,
            else:
                print text

    def system (self, cmd):
        fullCmd = self.cmdPattern % cmd
        self.log (fullCmd)
        return os.system (fullCmd)

    def addTarget (self, target):
        self.targets.append (target)

    def makeCommand (self, target, forDistrib = None):
        letter = self.runMake
        if letter == None:
            letter = 'f'
        cmd = _toolCommand % ('im' + letter)
        if forDistrib:
            cmd = cmd + ' -F '
        cmd = cmd + target
        self.log (cmd)
        rc = self.system (cmd)
        if rc != 0:
            self.status = rc
            result = None
            self.log ('vmake Error: ' + `rc`)
        else:
            result = 1
        return result

    def makeTarget (self, target):
        result = 1
        if self.runMake:
            result = self.makeCommand (target)
        return result

    def getDistribFiles (self, targets):
        outputDir = os.path.join (self.own32, 'usr/etc/')
        if not os.path.exists (outputDir):
            os.mkdir (outputDir)
        ok = self.makeCommand (string.join (targets), 1)
        if ok:
            for target in targets:
                listfile = outputDir + target + '.lst'
                try:
                    lines = open (listfile, 'r').readlines ()
                except IOError:
                    fatalError ('Cannot open distrib list ' + listfile)
                files = map (string.strip, lines)
                self.files = self.files + files
        return ok

    def make (self):
        targetString = string.join (self.targets)
        ok = self.makeTarget (targetString)
        if ok:
            ok = self.getDistribFiles (self.targets)
        return ok

    def copyFiles (self, targetDir):
        pathjoin = os.path.join
        localInst = pathjoin (self.own32, 'usr')
        for file in self.files:
            src = pathjoin (localInst, file)
            target = pathjoin (targetDir, file)
            self.log ("%-30s ==> %s" % (src, target))
            shutil.copy2 (src, target)

def fatalError (text):
    sys.stderr.write ("%s: Error: %s\n" % (_programName, text))
    sys.exit (5)

def warning (text):
    sys.stderr.write ("%s: Warning: %s\n" % (_programName, text))

def chooseMakeOption (options, letter):
    makeOption = getattr (options, letter)
    # no option given
    if makeOption == None:
        return
    # option not set yet, use new option
    elif options.make == None:
        options.make = makeOption
        return
    # option already set, ignore new value
    else:
        warning ('make option set to %s, -%s ignored' % (options.make, makeOption))

def find64InstRoot (options):
    if options.own64:
        return
    if os.environ.get ('BIT64'):
        options.own64 = os.environ ['OWN']
    else:
        fatalError ('64bit OWN must be set [-64 <dir>]')

def checkOwn32 (own32):
    if not os.path.isdir (own32):
        fatalError ('32bit directory %s doesn\'t exist' % own32)
    if not os.path.exists (os.path.join (own32, '.iprofile')):
        fatalError ('No .iprofile in ' + own32)

_patterns32 = [
    '%(prefix)s32%(suffix)s',
    '%(prefix)s%(suffix)s',
]

def find32Own (options):
    if options.own32:
        checkOwn32 (options.own32)
        return
    if not os.environ.get ('BIT64'):
        options.own32 = os.environ ['OWN']
    else:
        own64 = options.own64
        try:
            pos64 = string.index (own64, '64')
        except ValueError:
            fatalError ('Cannot convert %s to 32bit equivalent' % own64)
        prefix = own64 [:pos64]
        suffix = own64 [pos64 + 2:]
        candidates = []
        for pattern in _patterns32:
            own32 = pattern % locals ()
            if os.path.isdir (own32):
                options.own32 = own32
                checkOwn32 (own32)
                return
            candidates.append (own32)
        fatalError ('No suitable 32bit $OWN found ' + `candidates`)

def checkOptions (options):
    find64InstRoot (options)
    find32Own (options)
    options.make = None
    for letter in ['f', 'q', 's']:
        chooseMakeOption (options, letter)

def main (options, args):
    """Makes and copies 32bit target into 64bit tree
    """
    checkOptions (options)
    copy = Copy32to64 (options.make, options.verbose, options.own32)
    if len (args) == 0:
        args = ['all32']
    for arg in args:
        copy.addTarget (arg)
    copy.make ();
    copy.copyFiles (os.path.join (options.own64, 'usr'))
    sys.exit (copy.getStatus ())

def _options ():
    if os.environ.get ('BIT64'):
        own64 = os.environ ['OWN']
        own32 = None
    else:
        own64 = None
        own32 = os.environ ['OWN']
    return [
        # (optstring, varname, typechar, default, help)
        ('64', 'own64', ':', own64, '$OWN of 64bit tree'),
        ('32', 'own32', ':', own32, '$OWN of 32bit tree'),
        ('s', None, '', None, 'make slow targets'),
        ('q', None, '', None, 'make quick targets'),
        ('f', None, '', None, 'make fast targets'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('version', None, '', 0.2, 'version information'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
