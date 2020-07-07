
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#

import string
import re
import os
isdir  = os.path.isdir
pathjoin = os.path.join
dirpart = os.path.dirname
exists = os.path.exists
getmtime = os.path.getmtime
import vpathLib

own = os.environ ['OWN']
vmakePath = filter (None, string.split (os.environ ['VMAKE_PATH'], ','))
tool = os.environ ['TOOL']

class FileNotFound:
    def __init__ (self, fname):
        self.fname = fname

    def __str__ (self):
        return 'FileNotFound: ' + self.fname


class SysTree:
    def __init__ (self, options):
        self.options = options
        self.configureFileTranslation ()

    def configureFileTranslation (self):
        nl = self.options.nl
        if nl == 'native':
            self.translateData = self.translateNative
        elif nl == 'unix':
            self.translateData = self.translateUnix
        elif nl == 'dos':
            self.translateData = self.translateDos
        else:
            raise 'Option -nl must be [native, unix, dos]', nl

    def findInVPath (self, relative):
        for path in vmakePath:
            fullPath = pathjoin (path, relative)
            if exists (fullPath):
                return fullPath
        raise FileNotFound (relative)


    def copy (self, fname):
        fname = os.path.abspath (os.path.normpath (fname))
        relative = fname [len (own) + 1:]
        try:
            fullPath = self.findInVPath (relative)
            if isdir (fullPath):
                self.copyTree (fname)
            else:
                # split relative
                self.copyFile (relative, fullPath)
        except FileNotFound, err:
            print err

    def copyTree (self, fullPath):
        files = vpathLib.listdir (fullPath)
        for layer, relative in files:
            # print layer, relative
            fullPath = pathjoin (layer, relative)
            if isdir (fullPath):
                self.copyTree (pathjoin (own, relative))
            else:
                self.copyFile (relative, fullPath)

    def copyFile (self, relative, fullPath):
        self.prepareDir (relative)
        outfile = pathjoin (self.options.outdir, relative)
        if not self.requiresCopy (outfile, fullPath):
            return
        if self.options.verbose:
            print relative
        data = open (fullPath, 'rb').read ()
        data = self.translateData (data)
        open (outfile, 'wb').write (data)

    def translateNative (self, data):
        return data

    def translateUnix (self, data):
        if self.isTextFile (data):
            data = string.replace (data, '\r\n', '\n')
        return data

    def translateDos (self, relative, fullPath):
        if self.isTextFile (data):
            data = string.replace (data, '\r\n', '\n')
            data = string.replace (data, '\n', '\r\n')
        return data

    def prepareDir (self, relative):
        dirname = dirpart (relative)
        targetDir = pathjoin (self.options.outdir, dirname)
        if not isdir (targetDir):
            os.makedirs (targetDir)

    def isTextFile (self, data):
        chunk = data [:1024]
        for ch in '\x00\x01\x02\x03\x04\x05':
            if string.find (chunk, ch) != -1:
                return None
        return 1

    def requiresCopy (self, target, source):
        if self.options.unconditional:
            return 1
        if not exists (target):
            return 1
        sourcemtime = getmtime (source)
        targetmtime = getmtime (target)
        if targetmtime < sourcemtime:
            return 1
        return 0


class VmakeModules:
    def __init__ (self, targets, platforms):
        self.targets = string.join (targets)
        self.modules = {}
        for platform in platforms:
            self.addRun (platform)

    moduleRe = re.compile ('(src/)|(desc/)')
    def addRun (self, platform):
        if platform == 'win32':
            cmd = 'perl %s/bin/ims.pl -m ' % tool
        else:
            cmd = 'perl %s/bin/remims.pl %s -m ' % (tool, platform)
        lines = os.popen tool/bin/tr_cpc.pl
tool/bin/ixterm.pl
tool/bin/ixterm.pm
tool/bin/PCTest.pm
tool/bin/pctest.pl
tool/bin/pccmpct.pl
tool/bin/pcmkcpo.pl
tool/bin/pcclear.pl
tool/bin/pcmapdb.pl
tool/bin/pcxuser.pl
tool/bin/CPCTest.pm
tool/bin/pcstartdb.pl
tool/bin/pcvt.pl
tool/bin/cpctest.pl
tool/bin/cpct.pl
tool/bin