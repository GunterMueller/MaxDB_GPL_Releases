
# updateSources.py
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
import os

from distribTools.Historian import SourceHistorian

class DownLoader:
    def __init__ (self, options):
        self.options = options
        self.historian = SourceHistorian (options)
        self.detectBranch (options)

    def detectBranch (self, options):
        if options.branch in ['DEVELOP', 'STABLE']:
            branch = self.historian.location.getText(options.branch)
        else:
            branch = options.branch
        print branch
        self.branch = branch
        if not self.historian.branches.has_key (branch):
            fatalError ('no branch named: ' + branch)

    def initSources (self):
        print ('get action history')
        print ('work through history')

def fatalError (msg):
    sys.stderr.write ('updateSources: ' + msg + '\n')
    sys.exit (2)

def checkOptions (options):
    if not options.url:
        fatalError ('options -url must be specified')
    if not os.path.isdir (options.localdir):
        fatalError ('-localdir \'%s\' must be a directory' % options.localDir)
    if options.cache and not os.path.isdir (options.cache):
        fatalError ('-cache \'%s\' must be a directory' % options.cache)

def main (options, args):
    downloader = DownLoader (options)
    if options.initial:
        downloader.initSources ()
    else:
        downloader.updateSources ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('url', None, ':', None, 'location of update files [local or ftp:]'),
        ('branch', None, ':', 'STABLE', 'DEVELOP, STABLE or V73_00_...'),
        ('change', None, ':', 0, 'id of last downloaded change'),
        ('initial', None, '', None, 'create from scratch'),
        ('localdir', None, ':', '.', 'target directory'),
        ('cache', None, ':', None, 'dir to cache downloaded files'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
