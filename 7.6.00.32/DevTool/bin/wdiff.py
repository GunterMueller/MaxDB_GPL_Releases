
# iwdiff.py
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

import sys
import os
import string
import time
sys.path.append (os.path.abspath (os.path.join (sys.path [0], '..', 'lib', 'python')))

def fillDir (dirname, filename):
    basename = os.path.basename (filename)
    return os.path.join (dirname, basename)

def fillDirArgs (file1, file2):
    if os.path.isdir (file1) and os.path.isfile (file2):
        file1 = fillDir (file1, file2)
    elif os.path.isdir (file2) and os.path.isfile (file1):
        file2 = fillDir (file2, file1)
    return file1, file2

def p4fstat (filename):
    cmd = 'p4 fstat ' + filename
    lines = os.popen (cmd, 'r').readlines ()
    if not lines:
        os.system (cmd)
        sys.exit (2)
    values = {}
    for row in filter (None, map (string.split, lines)):
        values [row [1]] = row [2]
    return values

def getOtherBranch (localfile, branch):
    #
    # get prefix of other file
    #
    branchTuple = tuple (map (int, string.split (branch, '.')))
    if len (branchTuple) == 2:
        depot = '//sapdb/V%s%s/develop' % branchTuple
    elif len (branchTuple) == 3:
        depot = '//sapdb/V%s%s/c_%02d/develop' % branchTuple
    elif len (branchTuple) == 4:
        depot = '//sapdb/V%s%s/c_%02d/b_%02d' % branchTuple
    #
    # get relative of localfile
    #
    relative = os.path.abspath (localfile) [len (os.environ ['OWN']):]
    #
    # find loca file in other release
    #
    perforceOther = depot + string.replace (relative, '\\', '/')
    values = p4fstat (perforceOther)
    otherFile = values.get ('clientFile')
    if otherFile and os.path.exists (otherFile):
        return otherFile, otherFile
    #
    # copy file from Perforce if it is not on disk
    #
    revisionSuffix = '@' + values ['headChange']
    otherFile = os.path.join (os.environ ['TMP'], os.path.basename (localfile) + revisionSuffix)
    perforceId = values ['depotFile']
    cmd = 'p4 print -q -o %s %s' % (otherFile, perforceId)
    os.system (cmd)
    return otherFile, perforceId
    sys.exit (1)


def getCurrentPerforce (localfile, revisionSuffix = None):
    #
    # get fstat
    #
    values = p4fstat (localfile)
    #
    # copy file to disk
    #
    if not revisionSuffix:
        revisionSuffix = '@' + values ['headChange']
    perforceId = values ['depotFile'] + revisionSuffix
    localname = os.path.join (os.environ ['TMP'], os.path.basename (localfile) + revisionSuffix)
    cmd = 'p4 print -q -o %s %s' % (localname, perforceId)
    os.system (cmd)
    #
    # return fname
    #
    return localname, perforceId

def main (options, args):
    """compares two files side by side

    Allows to merge changes into one file.
    If only one file is given, then the latest Perforce revision is used.

    Colors:
        red     lines deleted
        green   lines added
        yellow  lines changed

    Keys:
        n       next difference
        p       previous difference
        Ctrl-s  save file

    Cursors left panel:
        right arrow     merge changes into right panel

    Cursors right panel:
        X               delete difference
        exchange        revert change

    Options:
    """
    if len (args) == 0:
        sys.stderr.write ('at least one filename must be given\n')
        sys.exit (2)
    mine = args [0]
    if len (args) > 2:
        sys.stderr.write ('only two files can be compared\n')
        sys.exit (2)
    if len (args) == 2:
        mine, other = fillDirArgs (mine, args [1])
        otherFile, otherId = other, other
    elif options.branch:
        otherFile, otherId = getOtherBranch (mine, options.branch)
    else:
        otherFile, otherId = getCurrentPerforce (mine, options.revision)
    import diffTools.wdiffLib
    from fileutil import basename
    wdiff = diffTools.wdiffLib.WDiff (options)
    wdiff.addDiff (mine, otherFile)
    title = 'wp4diff: %s' % (basename (mine),)
    wdiff.mainLoop (title, line = options.line, files = [otherId, mine])

def _options ():
    from diffTools.wdiffLib import _version, readOptions
    defaults = readOptions (nonum = None)
    return [
        # (optstring, varname, typechar, default, help)
        ('version', None, '', _version, 'version information'),
        ('line', None, ':', -1, 'move to this line'),
        ('nonum', None, '', defaults ['nonum'], 'don\'t display line numbers'),
        ('revision', None, ':', None, 'Revision in Perforce notation'),
        ('branch', None, ':', None, 'diff local file from other branch')
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

