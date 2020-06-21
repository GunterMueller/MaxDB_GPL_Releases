#!/usr/bin/env python
# perforceExchange.py
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
import re
import os
import ftplib

import optlib
import pack

import perforce.SourceTree
import distribTools.releaseInfo
import distribTools.common

class PerforceExchange:
    def __init__ (self, options, root):
        self.options = options
        self.sourceTree = perforce.SourceTree.SourceTree (root,
            'sys/src/sp/vsp001', perforceClient = options.p4)
        self.own = self.sourceTree.getLocalPath ()
        self.counterFile = os.path.join (self.own, '.lastChange')
        if options.initCounter:
            self.lastChange = options.initCounter
        else:
            self.lastChange = self.getCounter ()
        self.filebase = distribTools.releaseInfo.releasePath2filename (root, '/')

    def getCounter (self):
        """gets the changelist id of the current sources
        """
        if not os.path.exists (self.counterFile):
            result = 0
        else:
            data = open (self.counterFile, 'rt').read ()
            result = int (data)
        return result

    def setCounter (self, newValue):
        """keep the current changelist id as a bookmark
        """
        open (self.counterFile, 'wt').write (`newValue`)

    def createExchange (self):
        changes = self.getChanges ()
        if not changes:
            return
        chunker = ChangeChunker (self.lastChange)
        attributedList = changes.getAttributedList (chunker.callback)
        chunk = []
        chunkStart = self.lastChange
        for (action, changeRange), changeList in attributedList:
            # print action, changeRange
            if action == 'branch':
                newFiles = self.createBranch (changeList)
            else:
                newFiles = self.packChange (changeList, changeRange)
            chunk.extend (newFiles)
            if action in ['branch', 'day change']:
                chunkStart = self.packChunk (chunk, chunkStart, changeList.id)
                chunk = []
        exchangeEnd = self.packChunk (chunk, chunkStart, changeList.id)
        self.setCounter (exchangeEnd)

    def getChanges (self):
        branches = self.sourceTree.getBranches (lowerBound = self.lastChange)
        changes = self.sourceTree.getChanges (self.lastChange)
        changes.mergeBranchChanges (branches)
        return changes

    def packChange (self, changeList, changeRange):
        changeList.describe ()
        changedFiles = self.getChangedFiles (changeList)
        #
        # sync to change list
        #
        self.sourceTree.sync ('@' + `changeList.id`)
        #
        # write description
        #
        descFile = self.writeChangeDescription (changeList,
            changeRange, changedFiles)
        #
        # pack changed files
        #
        dataFile = self.packChangedFiles (changeList,
            changeRange, changedFiles)
        #
        #
        #
        return [descFile, dataFile]

    def getChangedFiles (self, changeList):
        result = []
        root = self.sourceTree.perforcePath
        if root [-1] != '/':
            root = root + '/'
        rootLen = len (root)
        for fullname, action in changeList.files:
            if fullname [:rootLen] == root:
                relative = fullname [rootLen:]
                ftype = self.sourceTree.getFileType (relative)
                result.append ((relative, action, ftype))
        return result

    def writeChangeDescription (self, changeList, changeRange, changedFiles):
        """generate the description of a change list in XML
        """
        prevChange, thisChange = changeRange
        descName = '%s_%05d.desc' % (self.filebase, changeList.id)
        stream = open (self.outputFile (descName), 'wt')
        stream.write ('<?xml version="1.0"?>\n')
        stream.write ('<PerforceChange id="%d" previous="%d">\n'
            % (changeList.id, prevChange))
        stream.write ('    <Comment>' + entify (changeList.comment) + '</Comment>\n')
        stream.write ('    <Files>\n')
        for name, action, (ftype, foptions) in changedFiles:
            tag = '        <File action="%s" format="full" kind="%s"' \
                % (action, ftype)
            if foptions:
                tag = tag + (' options="%s"' % string.join (foptions, ','))
            tag = tag + '>' + name + '</File>\n'
            stream.write (tag)
        stream.write ('    </Files>\n')
        stream.write ('</PerforceChange>\n')
        stream.close ()
        return descName

    def packChangedFiles (self, changeList, changeRange, changedFiles):
        dataFile = '%s_%05d' % (self.filebase, changeList.id)
        # collect files
        filelist = []
        for name, action, (ftype, foptions) in changedFiles:
            if action == 'delete':
                continue
            filelist.append (name)
        #
        if not filelist:
            return None
        # call tar
        createdFile = pack.tar (self.outputFile (dataFile),
            filelist, fileroot = self.sourceTree.localPath)
        return os.path.basename (createdFile)

    def createBranch (self, changeList):
        targetBranch = distribTools.releaseInfo.releasePath2filename (
            changeList.branchTo, '/')
        #
        # write desc file
        #
        descFile = '%s_%05d.desc' % (self.filebase, changeList.id)
        stream = open (self.outputFile (descFile), 'wt')
        stream.write ('<?xml version="1.0"?>\n')
        stream.write ('<PerforceChange id="%d">\n' % changeList.id)
        stream.write ('    <Comment>Branch from %s to %s</Comment>\n'
            % (self.filebase, targetBranch))
        stream.write ('    <Files>\n')
        stream.write ('        <!--- all files are branched --->\n')
        stream.write ('    </Files>\n')
        stream.write ('</PerforceChange>\n')
        stream.close ()
        #
        # pack and transfer branch
        #
        archive = '%s_%05dbranchto_%s' % (
            self.filebase, changeList.id, targetBranch)
        localFile = pack.pack (self.outputFile (archive), [descFile],
            self.options.outdir)
        distribTools.common.transferByFTP (self.options, localFile)
        return []

    def packChunk (self, chunk, chunkStart, chunkEnd):
        chunk = filter (None, chunk)
        if not chunk:
            return chunkStart
        archivebase = '%s_%05dto%05d' % (self.filebase, chunkStart, chunkEnd)
        archive = self.outputFile (archivebase)
        localFile = pack.pack (archive, chunk, self.options.outdir)
        distribTools.common.transferByFTP (self.options, localFile)
        return chunkEnd

    def outputFile (self, fname):
        return os.path.join (self.options.outdir, fname)

class ChangeChunker:
    """Inserts 'pack' events into a collection of change lists
    """
    def __init__ (self, lastChange):
        self.lastChange = lastChange
        self.lastDate = None

    def callback (self, changeList):
        if changeList.isBranch:
            action = 'branch'
        elif self.lastDate and (self.lastDate != changeList.date):
            action = 'day change'
        else:
            action = None
        result = (action, (self.lastChange, changeList.id))
        self.lastChange = changeList.id
        self.lastDate = changeList.date
        return result

def entify (text):
    text = string.replace (text, '&', '&amp;')
    text = string.replace (text, '>', '&lt;')
    return text

def main (options, args):
    distribTools.common.checkOptions (options)
    for root in options.root:
        PerforceExchange (options, root).createExchange ()

def _options ():
    return ([
        # (optstring, varname, typechar, default, help)
        ('initCounter', None, ':', 0, 'initialize perforce counter'),
        ]
        + distribTools.common._cmdlineOptions)

if __name__ == '__main__':
    import optlib
    args = optlib.expandFileArgs (sys.argv [1:])
    args = map (os.path.expandvars, args)
    optlib.optMain2 (main, _options ())

