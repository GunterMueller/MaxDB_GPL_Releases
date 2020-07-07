#!/usr/bin/env python
# minimizeIncludes.py

import sys
import string
import re
import os
import glob

src = os.environ ['SRC']
testdir = os.path.join (src, 'test')

class CompHeader:
    def __init__ (self, verbose):
        self.verbose = verbose
        self.errors = None

    def prerequisites (self):
        if not os.path.isdir (testdir):
            os.mkdir (testdir)

    def compile (self, fname):
        localsource = getLocalFile (fname)
        base, extension = os.path.splitext (os.path.basename (localsource))
        outext = '.cpp'
        source, includes = self.readSource (localsource)
        #
        # write source
        #
        sourcename = 'IncTest_' + base + outext
        targetfile = os.path.join (testdir, sourcename)
        stream = open (targetfile, 'w')
        stream.write (source)
        stream.close ()
        #
        # write description
        #
        self.createDescription ()
        #
        # execute compiler
        #
        rc, output = execPipe (itool ('ims', ['-u', ':test/' + sourcename]))
        if rc:
            self.errors = 1
            print 'RC:', rc, '\n', output
            print '---', 'header', fname, 'is missing some includes'
        return rc

    def readSource (self, localfile):
        source = open (localfile, 'r').read ()
        lines = source.split ('\n')
        includes = []
        includeRE = re.compile (r'\s*#\s*include\s*"([^"]+)"')
        for line in lines:
            match = includeRE.match (line)
            if match:
                includes.append (match.group (1))
        return source, includes

    def createDescription (self):
        stream = open (os.path.join (testdir, 'test.com'), 'w')
        stream.write ('\n#\n# include dependencies (do not edit)\n\#\n')
        stream.close ()
        os.system ('python %s/bin/imkdep.py -force %s/test.com' % (os.environ ['TOOL'], testdir))

    def trim (self, fname):
        localsource = getLocalFile (fname)
        base, extension = os.path.splitext (os.path.basename (localsource))
        outext = '.cpp'
        source, includes = self.readSource (localsource)
        for include in includes:
            #
            # write source
            #
            sourcename = 'IncTest_' + base + outext
            targetfile = os.path.join (testdir, sourcename)
            stream = open (targetfile, 'w')
            source = self.removeOneInclude (source, include)
            stream.write (source)
            stream.close ()
            #
            # write description
            #
            self.createDescription ()
            #
            # execute compiler
            #
            rc, output = execPipe (itool ('ims', ['-u', ':test/' + sourcename]))
            if rc == 0:
                print '---', include, 'not necessary in', fname
            else:
                if self.verbose:
                    print '+++', include, 'necessary in', fname

    def removeOneInclude (self, source, include):
        lines = source.split ('\n')
        result = []
        includeRE = re.compile (r'\s*#\s*include\s*"%s"' % include)
        for line in lines:
            match = includeRE.match (line)
            if match:
                result.append ('/* ' + line + ' */')
            else:
                result.append (line)
        return string.join (result, '\n')

def execPipe (command):
    stream = os.popen (command, 'r')
    output = stream.read ()
    rc = stream.close ()
    return rc, output

def getLocalFile (fname):
    if os.path.exists (fname):
        return fname
    rc, output = execPipe (itool ('iview', ['-e', '0', fname]))
    if rc:
        print output
        raise 'iview failed', (rc, fname)
    result = output.split (': ') [0]
    return result

def itool (name, args):
    cmd = ('%(TOOLSHELL)s %(TOOL)s/bin/' + name + '%(TOOLEXT)s ') % os.environ
    cmd = cmd + string.join (args, ' ')
    return cmd

class TeeStream:
    def __init__ (self, *streams):
        if len (streams) == 1:
            streams = (sys.stdout,) + streams
        self.streams = streams

    def write (self, data):
        for stream in self.streams:
            stream.write (data)

class LineIterator:
    def __init__ (self, stream):
        self.readline = stream.readline

    def __iter__ (self):
        return self

    def next (self):
        result = self.readline ()
        if not result:
            raise StopIteration
        return result.strip ()

def expandArgs (args):
    result = []
    for arg in args:
        if arg [0] == '@':
            entries = open (arg [1:], 'r').read ().split ('\n')
            result.extend (entries)
        else:
            globbed = glob.glob (arg)
            if len (globbed) == 0:
                globbed = [arg]
            result.extend (globbed)
    return result

def checkOptions (options):
    if options.log:
        logfile = open (options.log, 'w')
        sys.stdout = TeeStream (logfile)

def main (options, args):
    """
    check if all #includes are necessary
    
    args are either files in iview notation or wildcards.
    
    All tests are done on a copy located in $SRC/test, the original
    files and descriptions are unchanged.

    """
    checkOptions (options)
    compiler = CompHeader (options.verbose)
    compiler.prerequisites ()
    if options.stdin:
        args = LineIterator (sys.stdin)
    else:
        args = expandArgs (args)
    for arg in args:
        ext = os.path.splitext (arg) [1]
        if ext not in ['.h', '.hpp', '.c', '.cpp']:
            continue
        if options.verbose:
            print '***', arg
        rc = compiler.compile (arg)
        if not rc:
            compiler.trim (arg)
    if compiler.errors:
        sys.exit (3)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('stdin', None, '', None, 'read filenames from stdin'),
        ('log', None, ':', None, 'name of protocol file (default: no log)'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

