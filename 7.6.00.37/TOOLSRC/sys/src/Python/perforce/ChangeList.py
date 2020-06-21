
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

import string
import re
import os
import tempfile

import fileutil
import perforceLib

class ChangeList:
    def __init__ (self, id, connectOptions = None):
        self.id = id
        self.session = perforceLib.PerforceSession (options = connectOptions)

    def __str__ (self):
        return '<Perforce:ChangeList %s>' % self.id

    def editFile (self, fname, mode = 't'):
        if not os.path.exists (fname):
            # possible: check file must be synced
            self.addFile (fname, mode)
        else:
            # check file is readable
            self.session.execute ('edit ' + fname)

    def addFile (self, fname, mode = 't'):
        if mode == 't':
            filetype = ' -t text '
        elif mode == 'b':
            filetype = ' -t binary '
        else:
            filetype = ''
        self.session.execute ('add ' + filetype + fname)

    def dropIfEmpty (self):
        self.session.execute ('change -d ' + `self.id`)


def newChangeList (description, connectOptions = None):
    if connectOptions == None:
        connectOptions = perforceLib.ConnectOptions ()
    optionString = connectOptions.getOptionsString ()
    text = os.popen ('p4 ' + optionString + ' change -o').read ()
    #
    # set comment
    #
    lastlineLabel    = '^Files:$'
    lastlinePos = string.find (text, lastlineLabel)
    lastlineEnd = lastlinePos + len (lastlineLabel)
    descriptionLabel = '<enter description here>'
    descriptionPos = string.find (text, descriptionLabel)
    descriptionEnd = descriptionPos + len (descriptionLabel)
    newText = text [:descriptionPos] + '\n\t' + description + text [descriptionEnd:lastlineEnd]
    tmpname = fileutil.writeTmpFile (newText)
    #
    # retrieve id
    #
    try:
        cmd = optionString + ' change -i < ' + tmpname
        idList = perforceLib.parseP4 (cmd, 'Change ([0-9]+) created')
        assert len (idList) == 1
        id = idList [0]
    finally:
        try:
            os.remove (tmpname)
        except:
            pass
    #
    # create ChangeList object
    #
    return ChangeList (id, connectOptions)

if __name__ == "__main__":
    changeList = newChangeList ('this tests creating changelists from python')
    print changeList

