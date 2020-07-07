
# 72diff73.py
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
import string
import re
import os

def fatalError (msg):
    sys.stderr.write ("72to73: " + msg + '\n')
    sys.exit (1)

diff3cmd = 'diff3 %(mine)s %(older)s %(yours)s'
mergecmd = 'diff3 -e --merge %(mine)s %(older)s %(yours)s'
#silvermergecmd = 'cd /d "%(dir)s" & "%(guipgm)s" %(older)s %(yours)s %(mine)s %(outfile)s'
sibermergeDir = os.environ ['TOOL'] + r'\lib\java\SiberMerge'
sibermergecmd = 'cd /d ' + sibermergeDir + ' & javaw -cp merge.jar merge.Merge %(older)s %(yours)s %(mine)s %(outfile)s'

def system (cmd):
    # print cmd
    return os.system (cmd)


class Differ:
    def __init__ (self, options):
        self.loadOptions (options)
        self.calculate72 ()
        self.path73 = self.splitVmakePath (os.environ ['VMAKE_PATH'])
        self.createRenamer ()

    def loadOptions (self, options):
        configFile = os.path.join (os.environ ['OWN'], 'diff.conf')
        importDict = {}
        if os.path.exists (configFile):
            execfile (configFile, importDict)
        for key, value in options.__dict__.items ():
            if importDict.has_key (key) and (value is None):
                setattr (options, key, importDict [key])
        self.options = options

    def calculate72 (self):
        iprofile = os.path.join (self.options.rel72, 'iprofile.bat')
        if not os.path.exists (iprofile):
            fatalError ("Can't find " + iprofile)
        lines = open (iprofile).readlines ()
        vmakePath = self.parseIprofile (lines)
        self.path72 = self.splitVmakePath (vmakePath)

    def splitVmakePath (self, pathvar):
        path = string.split (pathvar, ',')
        result = filter (None, path)
        result = map (os.path.normcase, result)
        return result

    def parseIprofile (self, lines):
        env = {'OWN': self.options.rel72}
        setRE = re.compile (' *set +([^=]+)=(.*)', re.IGNORECASE)
        varRE = re.compile ('%[^%]+%')
        def expandvar (match, env = env):
            varname = match.group (0) [1:-1]
            if env.has_key (varname):
                return env [varname]
            else:
                return ''
        for line in lines:
            match = setRE.match (line)
            if match:
                varname, value = match.group (1, 2)
                value = string.strip (value)
                value = varRE.sub (expandvar, value)
                if varname == 'VMAKE_PATH':
                    return value
                else:
                    env [varname] = value

    def createRenamer (self):
        from renameLib import ProjectRenamer, findRenameMap
        mapFile = findRenameMap ('rename72to73.dta')
        self.renamer = ProjectRenamer (mapFile)


    def diff (self, arg):
        yours, older, mine = self.getFilenames (arg)
        yours = self.renameVariables (yours)
        outfile = os.path.join (os.getcwd (), os.path.basename (arg))
        if self.options.diff:
            system (diff3cmd % locals ())
        if self.options.merge:
            system (mergecmd % locals ())
        if self.options.gui:
            oldtime = self.getmtime (outfile)
            system (sibermergecmd % locals ())
            newtime = self.getmtime (outfile)
            if newtime > oldtime:
                print 'Output written to', outfile

    def getmtime (self, fname):
        if os.path.exists (fname):
            mtime = os.path.getmtime (fname)
        else:
            mtime = 0
        return mtime

    def getFilenames (self, arg):
        v73, relative = self.iview (arg)
        for path in self.path72:
            v72 = path + '/sys' + relative
            if os.path.exists (v72):
                break
        for path in self.path73:
            branch = path + '/branchorigin' + relative
            if os.path.exists (branch):
                break
        return v72, branch, v73

    def iview (self, arg):
        cmd = ('%(TOOLSHELL)s %(TOOL)s/bin/iview%(TOOLEXT)s -e 0 ' % os.environ)\
            + arg
        data = string.strip (os.popen (cmd).read ())
        copiedPos = string.find (data, ' copied to')
        if copiedPos != -1:
            v73 = data [:copiedPos]
        elif string.find (data, 'local source') != -1:
            pos = string.find (data, ':', 3)
            v73 = data [:pos]
        else:
            fatalError ("Couldn't iview " + arg)
        v73 = os.path.normcase (v73)
        for path in self.path73:
            path = path + '\\sys'
            if v73 [:len (path)] == path:
                relative = v73 [len (path):]
                break
        return v73, relative

    def renameVariables (self, yours):
        outfile = os.path.basename (yours) + '.v72'
        outfile = os.path.join (os.environ ['TMP'], outfile)
        self.renamer.filterFile (yours, outfile)
        return outfile

def fixOptions (options):
    if not options.merge and not options.gui:
        options.diff = 'true'

def main (options, args):
    """Does a three way compare between the 7.2 source, the current 7.3
        source and the branching 7.3 source.
    """
    fixOptions (options)
    differ = Differ (options)
    for arg in args:
        differ.diff (arg)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('rel72', None, ':', None, '%OWN% of 7.2 tree'),
        ('diff', None, '', None, 'diff using diif3'),
        ('merge', None, '', None, 'merge using diff3'),
        ('gui', None, '', None, 'merge interactively using SiberMerge'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

