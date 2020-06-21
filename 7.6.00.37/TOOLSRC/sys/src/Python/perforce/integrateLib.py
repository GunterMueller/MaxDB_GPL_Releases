
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
import tempfile

import perforceLib
import ChangeList

class SubmittedChangeList:
    def __init__ (self, id):
        self.id = int (id)
        self.readInfo ()

    def dump (self):
        print self.description
        for file in self.files:
            print '   ', file

    def readInfo (self):
        lines = os.popen ('p4 -s describe -s ' + `self.id`).readlines ()
        description = []
        files = self.files = []
        for line in lines:
            kind, text = splitLine (line)
            if kind == 'error':
                raise perforceLib.PerforceError (text)
            elif kind == 'text':
                description.append (text)
            elif kind == 'info1':
                files.append (actionFromText (text))
        self.description = self.cleanDescription (description)

    def cleanDescription (self, lines):
        del lines [0]
        while string.strip (lines [0]) == '':
            del lines [0]
        result = string.join (lines, '')
        pos = string.find (result, 'Affected files')
        if pos != -1:
            result = result [:pos - 1]
        return result

    #
    #
    #
    def integrateToBranch (self, branch, options):
        if options.changeList:
            newid = str (options.changeList)
        else:
            newChange = ChangeList.newChangeList (self.description)
            newid = str (newChange.id)
        command = ' integrate -c ' + newid + ' -b ' + branch + ' -s -d '
        if options.reverse:
            command = command + ' -r '
        integrationMap = self.getIntegrationMap (branch)
        for action in self.files:
            source = action.fname
            srev = action.revision
            #try:
            #    target = integrationMap [source]
            #except KeyError:
            #    sys.stderr.write ('not in branch view: ' + source + '\n')
            #    continue
            target = ''
            if options.single and (srev > 1):
                revString = '%d,%d' % (srev - 1, srev)
            else:
                revString = `srev`
            cmd = ' integrate -c %s -d -s %s#%s %s' % (newid, source, revString, target)
            print 'p4', cmd
            os.system ('p4 ' + cmd)
        return newid

    def getIntegrationMap (self, branch):
        p4cmd = 'p4 -s integrate -n -b %s %s/...@%s' % (
            branch, os.environ ['OWN'], self.id)
        lines = os.popen (p4cmd, 'rt').readlines ()
        namePattern = '([^#]+)#([0-9]+)'
        rex = re.compile ('info: ' + namePattern + ' - [^ ]+ from '
            + namePattern)
        result = {}
        for line in lines:
            match = rex.match (line)
            if match:
                target, trev, source, srev = match.group (1, 2, 3, 4)
                result [source] = target
        return result

class Action:
    def __init__ (self, fname, revision, action):
        self.fname = fname
        self.revision = int (revision)
        self.action = action

    def __str__ (self):
        return self.fname + '#' + `self.revision` + ' ' + self.action

_changeRE = re.compile ('([^#]+)#([0-9]+) (.*)')
def actionFromText (line):
    match = _changeRE.match (line)
    if match:
        fname, revision, action = match.group (1, 2, 3)
        result = Action (fname, revision, action)
    else:
        result = None
    return result

def splitLine (line):
    pos = string.find (line, ':')
    kind = line [:pos]
    text = line [pos + 2:]
    return kind, text

def simpleIntegrate (changeid, branch, options):
    changeid = int (changeid)
    if options.changeList:
        newid = options.changeList
    else:
        submitted = SubmittedChangeList (changeid)
        newChange = ChangeList.newChangeList (submitted.description)
        newid = newChange.id
    if options.reverse:
        reverse = ' -r '
    else:
        reverse = ''
    command = 'p4 integrate -c %s -b %s %s %s/...@%d,%d' % (
        newid, branch, reverse, os.environ ['OWN'], changeid - 1, changeid)
    print command
    os.system (command)
    return newid

