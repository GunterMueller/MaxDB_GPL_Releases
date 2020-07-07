#!/usr/bin/env python
#
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

import os, sys
from stat import *
from regsub import gsub

def isWriteable (file) :
    '''checks if file is writeable'''

    return S_IMODE(os.stat(file)[ST_MODE]) & S_IWRITE

def lsdir(basedir, dir):
    '''recursively descend the directory rooted at dir,
    printing each regular file'''


    for f in os.listdir(dir):
        mode = os.stat('%s/%s' % (dir, f))[ST_MODE]
        if S_ISDIR(mode):
            # recurse into directory
            lsdir('%s/%s' % (basedir, f), '%s/%s' % (dir, f))
        elif S_ISREG(mode):
            if isWriteable(dir + '/' + f) :
                # p4 opened dir + '/' + f
                print '\t%s\%s' % (gsub('/', '\\',basedir), f)

def checkdir(dir):
    '''prints message and calls lsdir to check dir recursively'''

    if os.path.exists (dir):
        print '\n', 'find in', dir ,'\n'
        lsdir('',dir)
    else:
        print dir, "doesn't exists\n"

if __name__ == '__main__':

    DirsToScan = ['SRC', 'DESC']

    for OneDir in DirsToScan:
        try:
            checkdir (os.environ[OneDir])
        except KeyError:
            print OneDir, 'is undefined\n'

