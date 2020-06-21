#!/usr/bin/env python
# createSimplePackage.py

import sys
import string
import re
import os
from distribTools import common

class Packer:
    def __init__ (self, rootdir, excludes = []):
        self.rootdir = rootdir
        self.excludes = map (re.compile, excludes)
        self.infiles = []

    def addTarget (self, vmakeTarget):
        #
        # create distribution list
        #
        output = common.perlOutput ('imf', '-F', vmakeTarget)
        rex = re.compile ("Creating distribution file '(.*)'")
        for line in output:
            match = rex.match (line)
            if match:
                listfile = match.group (1)
        #
        # add files
        #
        normroot = normfile (self.rootdir)
        rootlen = len (normroot)
        instroot = os.environ ['INSTROOT']
        entries = string.split (open (listfile, 'r').read ())
        for entry in entries:
            if self.isExcluded (entry):
                continue
            fullpath = os.path.join (instroot, entry)
            normpath = normfile (fullpath)
            if normpath [:rootlen] == normroot:
                self.infiles.append (fullpath [rootlen + 1:])

    def isExcluded (self, fname):
        for exclude in self.excludes:
            if exclude.search (fname):
                return 1
        return None

    def createArchive (self, name, outdir):
        archiveName = self.archiveName (name)
        extension = common.detectExtension ()
        outfile = os.path.join (outdir, archiveName) + extension
        os.chdir (self.rootdir)
        packFiles (outfile, self.infiles)
        return outfile

    def archiveName (self, name):
        prefix = 'maxdb'
        platform = common.detectPlatform ()
        release = common.detectRelease ()
        return '%(prefix)s-%(name)s-%(platform)s-%(release)s' % locals ()


def normfile (fname):
    return os.path.normcase (os.path.normpath (fname))

def packFiles (outfile, files):
    if sys.platform == 'win32':
        pipe = os.popen ('zip -@ ' + outfile, 'w')
        for file in files:
            pipe.write (file + '\n')
    else:
        os.system ('tar cf - ' + string.join (files) + ' | gzip -c > ' + outfile)

def checkOptions (options, args):
    if options.test:
        print common.detectPlatform (1)
        sys.exit (0)

def main (options, args):
    """create an archive containing all distrib files
    """
    checkOptions (options, args)
    packer = Packer (options.rootdir, options.exclude)
    for arg in args:
        packer.addTarget (arg)
    outfile = packer.createArchive (options.name, options.outdir)
    if options.ftp:
        common.ftpTransfer (options.ftp, outfile)
        os.remove (outfile)
    else:
        print outfile

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('rootdir', None, ':', os.environ ['INSTROOT'], 'files are archived relative to this dir'),
        ('outdir', None, ':', os.environ ['TMP'], 'output directory'),
        ('exclude', None, '::', None, 'exclude files matching these patterns'),
        ('name', None, ':', 'unknown', 'name of package (default: first target)'),
        ('test', None, '', None, 'test environment'),
        ('ftp', None, ':', None, '[<user>:<pwd>@]host[:<path>] for FTP transfer'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

