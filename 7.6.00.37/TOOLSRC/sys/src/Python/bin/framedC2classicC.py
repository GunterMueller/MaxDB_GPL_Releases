
# framedC2classicC.py
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
import yutil

src = os.environ ['SRC']

def getSourceDir (module):
    if os.path.isabs (module):
        result = os.path.dirname (module)
    else:
        layer = module [1:3]
        result = os.path.join (src, layer) + os.sep
    return result

def newModuleName (options, module):
    if options.outname:
        return options.outname
    if module [-1] == 'c':
        modifier = ''
    elif module [-1] == 'x':
        modifier = 'pp'
    else:
        raise 'Unknwn module type', module
    if module [3:5] == '00':
        result = 'g' + module [1:-1] + '.h' + modifier
    else:
        result = module [:-1] + '.c' + modifier
    return result

def createModule (fullname):
    if not os.path.exists (fullname):
        os.system ('inew -e 0 ' + fullname)
    if not os.path.exists (fullname):
        raise 'Inew failed', fullname

def iget (fullname):
    if not os.path.exists (fullname):
        os.system ('iget -e 0 ' + fullname)
    if not os.path.exists (fullname):
        raise 'Inew failed', fullname


class FramedModule:
    infoRE = re.compile ('[.]tt 3 [$]([^$]*)[$]([^$]*)', re.I)
    def __init__ (self, name):
        self.name = name
        stream = yutil.openFile (name)
        self.data = stream.read ()
        match = self.infoRE.search (self.data)
        if match:
            self.author, self.topic = match.group (1, 2)
        else:
            self.author, self.topic = ('unknown', 'unknown')

    codeRE = re.compile ('\n *Code *:', re.I)
    def getCodeRange (self):
        match = self.codeRE.search (self.data)
        assert (match != None)
        codeStart = string.index (self.data, '\n', match.end (0)) + 1
        codeEnd = string.index (self.data, '\n.CM', codeStart) + 1
        return codeStart, codeEnd

    def getCode (self):
        start, end = self.getCodeRange ()
        return self.data [start:end]

    def clearCode (self, newName):
        start, end = self.getCodeRange ()
        self.data = (self.data [:start] + '\n/* code moved to '
            + newName + ' */\n\n\n' + self.data [end:])

    def save (self, fname = None):
        if fname == None:
            fname = self.name
        srcDir = getSourceDir (fname)
        fullname = os.path.join (srcDir, fname)
        iget (fullname)
        open (fullname, 'wt').write (self.data)

def writeNewModule (fname, framed, isInclude):
    stream = open (fname, 'wt')
    #
    # header
    #
    basename = os.path.basename (fname)
    author, topic = framed.author, framed.topic
    stream.write ("""/*!================================================================

 module:        %s

 -------------------------------------------------------------------

 responsible:   %s

 description:   %s

 -------------------------------------------------------------------

 copyright:     SAP AG 2000

 ===================================================================*/
 """
 % (basename, author, topic))
    #
    # include guard
    #
    if isInclude:
        defid = string.upper (basename)
        defid = string.replace (defid, '.', '_')
        stream.write ('\n#ifndef %s' % defid)
        stream.write ('\n#define %s\n' % defid)
        stream.write ('\n\nadd includes here\n')
    #
    # code
    #
    code = framed.getCode ()
    stream.write (code)
    #
    # end include guard
    #
    if isInclude:
        stream.write ('\n#endif  /* %s */\n' % defid)
    stream.close ()


def framedC2classicC (options, module):
    # get directory
    srcDir = getSourceDir (module)
    # detect include and language
    newName = newModuleName (options, module)
    extension = os.path.splitext (newName) [1]
    isInclude = extension [1] == 'h'
    # create new module
    fullNew = os.path.join (srcDir, newName)
    createModule (fullNew)
    # get old module
    framed = FramedModule (os.path.join (srcDir, module))
    # build new module from code chunk
    writeNewModule (fullNew, framed, isInclude)
    # delete code chunk from old module
    framed.clearCode (newName)
    framed.save (os.path.join (srcDir, module))


def main (options, args):
    """Converts old style framed C to class style C

    This program creates a new module by extracting
    the code part from the old module.

    In addition, it removes the code part from the old module.

    This program does not:
        - convert concom directives with # directives
          (use concom2cpp.py for this)
        - insert the proper include directives
          (use incldump.py for this)

    """
    for arg in args:
        framedC2classicC (options, arg)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outname', ':', None, 'name of output file'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
