#!/usr/bin/env python
# toolSources.py

import string
import os
import perforce.perforceLib
import perforce.SourceTree
import distribTools.common

basedir = 'TOOLSRC'

class ToolSources:
    def __init__ (self):
        pass

    def archive (self, changelist):
        self.depot = '//prodprojects/vmake/dev/src/_tools/maketools'
        if not os.path.isdir (basedir):
            os.mkdir (basedir)
        self.p4 = perforce.perforceLib.PerforceSession ()
        self.excludeFiles = distribTools.common.loadExcludesFromPerforce (
            self.p4, self.depot + '/sys/config/noOpenSource')
        filelist = self.getFilelist (changelist)
        for relative, (kind, options) in filelist:
            self.copyFile (relative, kind)

    def copyFile (self, relative, kind):
        print  relative
        dirname = os.path.dirname (relative)
        localdir = os.path.join (basedir, dirname)
        if not os.path.isdir (localdir):
            os.makedirs (localdir)
        localfile = os.path.join (basedir, relative)
        depotfile = self.depot + '/' + relative
        cmd = 'p4 print -q %s > %s' % (depotfile, localfile)
        #print cmd
        rc = os.system (cmd)
        #print '=>', rc
        if kind.endswith ('text'):
            self.win2unix (localfile)

    def win2unix (self, fname):
        instream = open (fname, 'r')
        data = instream.read ()
        instream.close ()
        outstream = open (fname, 'wb')
        outstream.write (data)
        outstream.close ()

    def getFilelist (self, changelist):
        sourceTree = perforce.SourceTree.SourceTree (self.depot,
            'sys/src/sp/vsp001', perforceClient = self.p4)
        if changelist:
            revision = '@%d' % changelist
        else:
            revision = None
        fileList = sourceTree.allFiles (skipDeleted = True,
            revision = revision)
        fileList = filter (self.isValidFileEntry, fileList)
        return fileList

    def isValidFileEntry (self, fileDescription):
        filename, kind = fileDescription
        filename = '/' + filename
        for isRE, _excl in self.excludeFiles:
            if isRE:
                if _excl.search (filename):
                    #print "ignored", filename, "because of", _excl.pattern
                    return None
            else:
                if string.find (filename, _excl) != -1:
                    #print "ignored", filename, "because of", _excl
                    return None
        return 1

def main (options, args):
    archiver = ToolSources ()
    archiver.archive (options.changelist)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('cl', 'changelist', ':', 0, 'Perforce changelist'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
