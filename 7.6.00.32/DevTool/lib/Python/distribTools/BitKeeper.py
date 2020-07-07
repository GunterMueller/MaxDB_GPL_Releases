#
#    ========== licence begin LGPL
#    Copyright (C) 2003 SAP AG
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
import time

import fileutil

if sys.platform == 'win32':
    import win32pipe
    def getShellOutput (cmd):
        #(stdin, stdout) = win32pipe.popen4(cmd, 't')
        #data = stdout.read ()
        #rc = stdout.close ()
        rc = os.system (cmd)
        data = ''
        return rc, data
else:
    def getShellOutput (cmd):
        pipe =  os.popen (cmd + ' 2>&1', 'r')
        data = pipe.read ()
        rc = pipe.close ()
        return rc, data

class BitKeeper:
    def __init__ (self, verbose = None, noAction = None):
        self.verbose = verbose
        self.noAction = noAction
        self.openLog ()
        self.dirCache = {}

    def openLog (self):
        self.log = open ('bk.log', 'a')
        self.log.write ('=== '
            + time.strftime ("%Y-%m-%d %H:%M:%S", time.localtime (time.time ()))
            + '===\n')

    def logCmd (self, cmd, result):
        self.log.write ('bk %s\n' % cmd)
        if result:
            lines = filter (None, string.split (result, '\n'))
            for line in lines:
                self.log.write ('    ' + line + '\n')


    def execute (self, cmd):
        cmd = string.replace (cmd, '\\', '/')
        if self.verbose:
            print ('bk ' + cmd) [:72]
        if self.noAction:
            return
        rc, data = getShellOutput ('bk ' + cmd)
        # rc = self.parseForError (rc, data)
        self.logCmd (cmd, data)
        if rc:
            raise BitKeeperError (rc, data)

    def getBKEntries (self, dirname):
        pass

    def exists (self, fname):
        dirname, basename = os.path.split (fname)
        sccsName = os.path.join (dirname, 'SCCS', 's.' + basename)
        return os.path.exists (sccsName)

    def checkout (self, fname):
        self.execute ('edit ' + quote (fname))

    def checkin (self, fname, isAdded, ftype):
        if isAdded:
            self.execute ('new ' + ftype + ' ' + quote (fname))
        else:
            self.execute ('ci -y ' + quote (fname))

    def delete (self, fname):
        if os.path.exists (fname):
            self.execute ('rm ' + quote (fname))

    def commit (self, message = None, files = None):
        cmd = 'commit -a -d '
        if message:
            tmpfile = fileutil.TempFile ().setContent (message)
            cmd = cmd + '-Y' + tmpfile.fname
        self.execute (cmd)

    def push (self):
        cmd = 'push'
        self.execute (cmd)

    def mkdir (self, dirname):
        parts = string.split (os.path.normpath (dirname), os.sep)
        dir = ''
        for part in parts:
            dir = os.path.join (dir, part)
            if dir:
                if not os.path.isdir (dir):
                    os.mkdir (dir)
            if not os.path.isdir (os.path.join (dir, 'CVS')):
                try:
                    self.execute ('add ' + dir)
                except CVSError, err:
                    print '   ', str (err)

    def createFile (self, fname):
        self.mkdir (os.path.dirname (fname))
        try:
            stream = open (fname, 'w')
        except IOError:
            os.chmod (fname, 0666)
            stream = open (fname, 'w')
        stream.close ()

class BitKeeperError:
    def __init__ (self, rc, output):
        self.rc = rc
        self.output = output
        lines = string.split (output, '\n')
        lines = map (string.strip, lines)
        self.outputLines = filter (None, lines)
        if not self.outputLines:
            self.outputLines = ['']

    def __str__ (self):
        return 'BitKeeperError [%s]: %s' % (self.rc, self.outputLines [-1])

def quote (filename):
    return '"' + filename + '"'


