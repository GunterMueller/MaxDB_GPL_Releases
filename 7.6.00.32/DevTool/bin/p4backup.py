
# p4backup.py
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
import os
import shutil

from perforce import perforceLib

_version = 0.1

def p4backup (options, client, args):
    pathjoin = os.path.join
    dirname = os.path.dirname
    isdir = os.path.isdir
    if client:
        backuproot = pathjoin (options.outdir, client)
        p4options = '-c ' + client
    else:
        backuproot = options.outdir
        p4options = ''
    depotList = perforceLib.depotList (p4options + ' -s opened ' + args, 'info: ([^#]+)')
    localList = perforceLib.depot2local (depotList, p4options)
    for localFile in localList:
        if os.path.exists (localFile):
            drive, dir = os.path.splitdrive (localFile)
            remoteFile = pathjoin (backuproot, dir [1:])
            if options.verbose:
                print remoteFile
            remoteDir = dirname (remoteFile)
            if not isdir (remoteDir):
                makedirs (remoteDir)
            try:
                shutil.copy2 (localFile, remoteFile)
            except IOError, exc:
                print 'ERR', exc

def makedirs(name, mode=0777):
    """makedirs(path [, mode=0777]) -> None

    Super-mkdir; create a leaf directory and all intermediate ones.
    Works like mkdir, except that any intermediate path segment (not
    just the rightmost) will be created if it does not exist.  This is
    recursive.

    """
    head, tail = os.path.split(name)
    if head and tail and not os.path.exists(head) and not os.path.exists(head + os.sep):
        makedirs(head, mode)
    os.mkdir(name, mode)


def main (options, args):
    """copies all files opened in perforce to the backup server

    Any arguments are passed to the 'p4 opened' command
    """
    if not options.p4:
        options.p4 = ['']
    for p4 in options.p4:
        p4backup (options, p4, string.join (args))

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outdir', ':', r'\\dber010.ber.sap.corp\Home' + '\\' + os.environ ['USERNAME'], 'output directory'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('p4', None, '::', None, 'p4 clients to backup'),
        ('version', None, '', _version, 'version information'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
