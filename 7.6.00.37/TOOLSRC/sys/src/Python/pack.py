
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
"""interface to various packing ad compressing programs
"""

import sys
import os
import tempfile

import fileutil

def tar (outfile, filelist, compress = 1, fileroot = None):
    """create a [compressed] tar file
    """
    assert len (filelist) > 0
    outfile = fileutil.defaultExtension (outfile, 'tar')
    outfile = os.path.abspath (outfile)
    if fileroot:
        currentDir = os.getcwd ()
        os.chdir (fileroot)
    try:
        #
        # write files to tmpfile
        #
        fname = tempfile.mktemp ()
        stream = open (fname, 'wt')
        for filename in filelist:
            stream.write (filename + '\n')
        stream.close ()
        #
        # build tar command
        #
        if compress:
            compressflag = 'z'
        else:
            compressflag = ''
        cmd = 'tar c%sf %s --files-from=%s' % (compressflag, outfile, fname)
        #
        # execute
        #
        #print cmd
        os.system (cmd)
        #
        # clear tmpfile
        #
        os.remove (fname)
    finally:
        if fileroot:
            os.chdir (currentDir)
    return outfile

def tgz (outfile, filelist, fileroot = None):
    outfile = fileutil.defaultExtension (outfile, 'tgz')
    return tar (outfile, filelist, 1, fileroot)

def zip (outfile, filelist, fileroot = None):
    """create a zip file
    """
    assert len (filelist) > 0
    outfile = fileutil.defaultExtension (outfile, 'zip')
    outfile = os.path.abspath (outfile)
    print 'zipping to', outfile
    if fileroot:
        currentDir = os.getcwd ()
        os.chdir (fileroot)
    try:
        #
        # build zip command
        #
        cmd = 'zip -@ %s' % outfile
        #
        # execute
        #
        if os.path.exists (outfile):
            os.remove (outfile)
        #print cmd
        pipe = os.popen (cmd, 'wt')
        for filename in filelist:
            pipe.write (filename + '\n')
        pipe.close ()
    finally:
        if fileroot:
            os.chdir (currentDir)
    return outfile

if sys.platform == 'win32':
    pack = zip
    tar = zip
else:
    pack = tgz

