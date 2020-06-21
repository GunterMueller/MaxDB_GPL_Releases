
import sys
import string
import os
import time

if sys.platform == 'win32':
    import win32pipe
    def getShellOutput (cmd):
        (stdin, stdout) = win32pipe.popen4(cmd, 't')
        data = stdout.read ()
        rc = stdout.close ()
        return rc, data
else:
    def getShellOutput (cmd):
        pipe =  os.popen (cmd + ' 2>&1', 'r')
        data = pipe.read ()
        rc = pipe.close ()
        return rc, data


import fileutil

class CVS:
    def __init__ (self, verbose = None, noAction = None):
        self.verbose = verbose
        self.noAction = noAction
        self.openLog ()
        self.dirCache = {}

    def openLog (self):
        self.log = open ('cvs.log', 'a')
        self.log.write ('=== '
            + time.strftime ("%Y-%m-%d %H:%M:%S", time.localtime (time.time ()))
            + '===\n')

    def logCmd (self, cmd, result):
        self.log.write ('cvs %s\n' % cmd)
        if result:
            lines = filter (None, string.split (result, '\n'))
            for line in lines:
                self.log.write ('    ' + line + '\n')

    if sys.platform == 'win32':
        def parseForError (self, rc, data):
            if rc:
                return rc
            for pattern in [
                    'added independently by second party',
                    ]:
                if string.find (data, pattern) != -1:
                    return 1
            return 0
    else:
        def parseForError (self, rc, data):
            return rc

    def execute (self, cmd):
        cmd = string.replace (cmd, '\\', '/')
        if self.verbose:
            print ('cvs ' + cmd) [:72]
        if self.noAction:
            return
        rc, data = getShellOutput ('cvs ' + cmd)
        rc = self.parseForError (rc, data)
        self.logCmd (cmd, data)
        if rc:
            raise CVSError (rc, data)

    def executeWithFiles (self, cmd, files):
        tmpCmd = cmd
        for file in files:
            if ' ' in file:
                file = quote (file)
            tmpCmd = tmpCmd + ' ' + file
            if len (tmpCmd) > 400:
                self.execute (tmpCmd)
                tmpCmd = cmd
        if tmpCmd != cmd:
            self.execute (tmpCmd)

    def getCvsEntries (self, dirname):
        entriesFile = os.path.join (dirname, 'CVS', 'Entries')
        try:
            touchTime = os.path.getmtime (entriesFile)
        except os.error:
            return None
        try:
            cacheTime, entries = self.dirCache [dirname]
            if cacheTime == touchTime:
                return entries
        except KeyError:
            pass
        entries = {}
        for line in open (entriesFile, 'r').readlines ():
            # D/sys////
            # /configure/1.1/Mon Sep 30 14:50:25 2002/-ko/
            fields = string.split (line, '/')
            if fields [0] == '':
                entries [fields[1]] = 1
        self.dirCache [dirname] = (touchTime, entries)
        return entries
            
    def exists (self, fname):
        dirname, basename = os.path.split (fname)
        entries = self.getCvsEntries (dirname)
        if entries is None:
            return None
        return entries.has_key (basename)
            
    def checkout (self, fname, filetype = 'o'):
        isAdded = None
        if self.exists (fname):
            cmd = 'co '
        else:
            self.createFile (fname)
            isAdded = 1
            cmd = 'add '
        if filetype != 't':
            cmd = cmd + '-k ' + filetype + ' '
        self.execute (cmd + quote (fname))
        if isAdded:
            os.remove (fname)

    def edit (self, fname, filetype = 'o'):
        if self.exists (fname):
            cmd = 'edit '
        else:
            self.createFile (fname)
            cmd = 'add '
            if filetype != 't':
                cmd = cmd + '-k ' + filetype + ' '
        self.execute (cmd + quote (fname))

    def delete (self, fname):
        if os.path.exists (fname):
            if not self.noAction:
                try:
                    os.remove (fname)
                except OSError:
                    os.chmod (fname, 0666)
                    os.remove (fname)
            self.execute ('remove ' + quote (fname))

    def commit (self, message = None, files = None):
        cmd = 'commit '
        if message:
            tmpfile = fileutil.TempFile ().setContent (message)
            cmd = cmd + '-F ' + tmpfile.fname
        else:
            cmd = cmd + '-m ""'
        if files:
            self.executeWithFiles (cmd, files)
        else:
            self.execute (cmd)

    def commitWithQuota (self, message = None):
        cmd = 'commit -l '
        if message:
            tmpfile = fileutil.TempFile ().setContent (message)
            cmd = cmd + '-F ' + tmpfile.fname
        else:
            cmd = cmd + '-m ""'
        cwd = os.path.abspath (os.getcwd ())
        alldirs = fileutil.RecursiveDirIterator ([os.path.abspath ('.')], ['.*/CVS$'])
        try:
            for dirname in alldirs:
                os.chdir (dirname)
                if self.verbose:
                    print os.getcwd (),
                self.execute (cmd)
        except:
            os.chdir (cwd)
    
    def commitSingleDirectory (self, args, dirname, files):
        if os.path.basename (dirname) == 'CVS':
            return
        print args, dirname    
        
    def commitSingleDirectory (self, *args):
        print args
    
    def tagForBranch (self, branchName):
        self.execute ('tag -b ' + branchName)

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

class CVSError:
    def __init__ (self, rc, output):
        self.rc = rc
        self.output = output
        lines = string.split (output, '\n')
        lines = map (string.strip, lines)
        self.outputLines = filter (None, lines)
        if not self.outputLines:
            self.outputLines = ['']

    def __str__ (self):
        return 'CVSError [%s]: %s' % (self.rc, self.outputLines [-1])

def quote (filename):
    return '"' + filename + '"'

if __name__ == "__main__":
    cvs = CVS (1, 1)
    cvs.commitWithQuota (string.join (sys.argv [1:]))

