
# updSniffProjects.py
#
#    ========== licence begin LGPL
#    Copyright (C) 2001 SAP AG
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

import os

import sniff.ProjectTree

def updateSniffProjects (options, args):
    #
    # run update
    #
    projectTree = sniff.ProjectTree.SniffProjectTree ()
    projectTree.updateTree (args, options)

def main (options, args):
    """updates Sniff projects in directory tree

    Projects are updated by adding all files in a directory
    to the projects FileSet

    """
    if len (args) == 0:
        doc  = os.environ ['OWN'] + '/sys/doc'
        src  = os.environ ['SRC']
        desc = os.environ ['DESC']
        for dir in [desc, doc, src]:
            if os.path.isdir (dir):
                args.append (dir)
    updateSniffProjects (options, args)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('forceNew', None, '', 1, 'overwrite projects with current template'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
