#!/usr/bin/env python
# testptoc.py

import sys
import os
import shutil

def fatalError (msg):
    sys.stderr.write ('testptoc: ' + msg + '\n')
    sys.exit (2)

if sys.platform == 'win32':
    devnull = 'NUL:'
else:
    devnull = '/dev/null'

class PtocTester:
    def __init__ (self, options):
        self.ptocCmd = options.ptocCmd
        self.defaultToChecks = options.checks
        self.keep = options.keep
        if options.show:
            self.diffcmd = 'diff -b %s %s'
        else:
            self.diffcmd = 'diff -b -q %s %s > ' + devnull
        self.input = []
        self.ok = 1

    def addInput (self, arg):
        if os.path.isfile (arg):
            if arg [-2:] == '.p':
                self.input.append ((arg [:-2], self.defaultToChecks))
            elif arg [-6:] == '.org.c':
                self.input.append ((arg [:-6], self.defaultToChecks))
            else:
                print 'not a valid ptoc test case:', arg
        elif os.path.isdir (arg):
            for subdir, addChecks in [('fast', None), ('quick', 1), ('slow', 1)]:
                self.addDir (os.path.join (arg, subdir), addChecks)
            self.addDir (arg, self.defaultToChecks)
        elif os.path.isfile (arg + '.p'):
            self.input.append ((arg, self.defaultToChecks))
        else:
            print 'not a valid ptoc test case:', arg

    def addDir (self, dirname, addChecks):
        files = os.listdir (dirname)
        for file in files:
            fullpath = os.path.join (dirname, file)
            if os.path.isfile (fullpath) and (fullpath [-2:] == '.p'):
                self.input.append ((fullpath [:-2], addChecks))

    def testAll (self):
        current = 0
        errors = 0
        count = len (self.input)
        format = self.getFormat ()
        for fname, addChecks in self.input:
            current = current + 1
            print format % (current, count, errors, fname),
            result = self.testOne (fname, addChecks)
            print '\t' + result
            if result != 'OK':
                errors = errors + 1
        print format % (current, count, errors, 'TOTAL'),

    def testOne (self, fname, addChecks):
        testcase = TestCase (fname)
        try:
            testcase.source2local ()
            if addChecks:
                ptoc = self.ptocCmd + ' -C '
            else:
                ptoc = self.ptocCmd + ' '
            rc = os.system (ptoc + testcase.localPascal ())
            if rc != 0:
                self.ok = None
                return 'ERR'
            diffRC = os.system (self.diffcmd % (testcase.orgresult (), testcase.cfile ()))
            if diffRC == 0:
                return 'OK'
            else:
                self.ok = None
                return 'DIFF'
        finally:
            if not self.keep:
                testcase.clean ()

    def getFormat (self):
        inputlen = str (len (self.input))
        decfmt = '%' + str (len (inputlen)) + 'd'
        return decfmt + ' / ' + decfmt + ' (' + decfmt + ' errors) %s'

class TestCase:
    def __init__ (self, fname):
        self.fname = fname
        self.basename = os.path.basename (fname)
        self.tmps = [self.cfile ()]

    def localPascal (self):
        return self.basename + '.p'

    def source (self):
        return self.fname + '.p'

    def cfile (self):
        return self.basename + '.c'

    def orgresult (self):
        return self.fname + '.org.c'

    def source2local (self):
        local = self.localPascal ()
        if os.path.exists (local):
            return
        shutil.copy (self.source (), local)
        self.tmps.append (local)

    def clean (self):
        for file in self.tmps:
            os.remove (file)

def checkOptions (options):
    if not options.tool:
        fatalError ('option -tool must be set')
    if sys.platform == 'win32':
        exe = '.exe'
    else:
        exe = ''
    ptoc = os.path.join (options.tool, 'pgm', 'ptoc' + exe)
    if not os.path.exists (ptoc):
        ptoc = os.path.join (options.tool, 'bin', 'ptoc' + exe)
        if not os.path.exists (ptoc):
            fatalError ('Cannot find ptoc executable')
    os.environ ['TOOL'] = options.tool
    ptocCmd = ptoc + ' -qn -e '
    options.ptocCmd = ptocCmd

def main (options, args):
    """Runs ptoc on several Pascal files as a regression test.

    """
    checkOptions (options)
    ok = 1
    if not args:
        args = ['.']
    tester = PtocTester (options)
    for arg in args:
        tester.addInput (arg)
    tester.testAll ()
    if not tester.ok:
        sys.exit (3)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('tool', None, ':', os.environ.get ('TOOL'), 'root of ptoc installation'),
        ('C', 'checks', '', None, 'add range checks'),
        ('k', 'keep', '', None, 'keep generated C files'),
        ('S', 'show', '', None, 'show difference'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
