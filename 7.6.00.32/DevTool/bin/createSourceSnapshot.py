#!/usr/bin/env python
# createSourceSnapshot.py
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


import sys
import string
import os
import tempfile

import pack

import perforce.SourceTree
import perforce.perforceLib

import distribTools.releaseInfo
import distribTools.common

class Snapshot:
    def __init__ (self, options, root):
        self.options = options
        p4 = perforce.perforceLib.PerforceSession (options.perforceClient)
        if root == '//sapdb/VTools':
            referenceFile = 'Contents.txt'
        else:
            referenceFile = 'sys/src/sp/vsp001'
        self.sourceTree = perforce.SourceTree.SourceTree (root,
            referenceFile, perforceClient = p4)
        self.releaseInfo = distribTools.releaseInfo.path2release (root, '/')
        self.excludeFiles = distribTools.common.loadExcludesFromPerforce (
            p4, root + '/sys/config/noOpenSource')
        if root == '//sapdb/VTools':
            self.excludeFiles = map (lambda s: s [1:], self.excludeFiles)

    def create (self):
        self.sync ()
        self.getFileList ()
        self.writeXMLFileList ()
        stdfiles = distribTools.common.copyStandardFiles (self.sourceTree.getLocalPath (), [
            ('distribTools/snapshot.readme', 'README')])
        self.pack (stdfiles)
        self.transfer ()

    def sync (self):
        if self.options.changelist:
            self.changeList = self.options.changelist
            revision = '@%d' % self.changeList
        else:
            self.changeList = self.sourceTree.lastChange ()
            revision = None
        self.sourceTree.sync (revision)

    def getFileList (self):
        self.sourceTree.getLocalPath ()
        fileList = self.sourceTree.allFiles ()
        self.fileList = filter (self.isValidFileEntry, fileList)

    def isValidFileEntry (self, fileDescription):
        filename, kind = fileDescription
        for whatever, _excl in self.excludeFiles:
            if string.find (filename, _excl) != -1:
                return None
        localFile = os.path.join (self.sourceTree.localPath, filename)
        if not os.path.exists (localFile):
            return None
        return 1

    def writeXMLFileList (self):
        fname = os.path.join (self.sourceTree.getLocalPath (), 'filelist.xml')
        stream = open (fname, 'wt')
        stream.write ('<?xml version="1.0"?>\n')
        releaseString = distribTools.releaseInfo.release2string (self.releaseInfo)
        stream.write ('<Snapshot release="%s" changelist="%d">\n'
            % (releaseString, self.changeList))
        stream.write ('<Files>\n')
        for filename, (kind, options) in self.fileList:
            tag = '    <File kind="%s"' % kind
            if options:
                tag = tag + ' options="' + string.join (options, ',') + '"'
            tag = tag + '>'
            stream.write (tag + filename + '</File>\n')
        stream.write ('</Files>\n')
        stream.write ('</Snapshot>\n')
        stream.close ()

    def pack (self, stdfiles):
        relid = distribTools.releaseInfo.release2filename (self.releaseInfo)
        filename = '%s_%05d' % (relid, self.changeList)
        fname = os.path.join (self.options.outdir, filename)
        filelist = ['filelist.xml'] + stdfiles
        for filename, kind in self.fileList:
            filelist.append (filename)
        self.snapshotFile = pack.pack (fname, filelist, self.sourceTree.getLocalPath ())

    def transfer (self):
        if self.options.ftp:
            distribTools.common.transferByFTP (self.options,
                self.snapshotFile, self.options.subdir)

def main (options, args):
    distribTools.common.checkOptions (options)
    for root in options.root:
        Snapshot (options, root).create ()

def _options ():
    return ([
        # (optstring, varname, typechar, default, help)
        ('changelist', None, ':', 0, 'sync to change list first'),
        ('subdir', None, ':', '/7.3/updates', 'subdir of staging'),
        ]
        + distribTools.common._cmdlineOptions)

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())


