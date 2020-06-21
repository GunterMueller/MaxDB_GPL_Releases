
# optlib.py                 routines used by modules for
#                           command line processing
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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


import sys
import string
from glob import glob
import os

def fatalError (errText):
    sys.stderr.write (errText + '\n')
    sys.exit (2)

def intArg (valText, optChar):
    try:
        result = string.atoi (valText)
    except:
        fatalError ('Argument to option -' + optChar + ' must be numeric')
    return result

if os.name == 'nt':
    def simpleGlob (fname):
        fnames = glob (fname)
        if len (fnames) == 0:
            # no match found, return literally
            return [fname]
        elif len (fnames) == 1:
            return fnames
        else:
            return filter (lambda fname: not os.path.isdir (fname), fnames)
else:
    def simpleGlob (fname):
        # under UNIX, globbing is done by the shell
        return [fname]

def expandFileArgs (fileArgs):
    result = []
    for f in fileArgs:
        if f[0] == '@':
            lines = open (f[1:], "r").readlines ()
            for line in lines:
                result.append (line [:-1])
        else:
            result.append (f)
    return result

class Options:
    def __init__ (self, **defaults):
        for key, value in defaults.items ():
            setattr (self, name, value)

    def setValue (self, name, value, typechar):
        if typechar == '::':
            list = getattr (self, name)
            list.append (value)
        else:
            setattr (self, name, value)

    def __getitem__ (self, key):
        try:
            result = getattr (self, key)
        except AttributeError:
            result = None
        return None

    def fillOptions (self, fname = None, **defaults):
        for key, value in defaults.items ():
            setattr (self, key, value)
        if fname:
            execfile (fname, defaults)

    def buildExtendedList (self, attrname, sepchar = ','):
        args = getattr (self, attrname)
        result = []
        for arg in args:
            result = result + string.splitfields (arg, sepchar)
        setattr (self, attrname, result)
        return result


    #def __getattr__ (self, key):
    #    print '__getattr__', key
    #    try:
    #        result = getattr (self, key)
    #    except AttributeError:
    #        result = None
    #    return result

    def dump (self, stream = sys.stdout, indent = ''):
        keys = self.__dict__.keys ()
        keys.sort ()
        for key in keys:
            stream.write ("%s%s: %s\n" % (indent, key, `getattr (self, key)`))

def _buildDescDict (optionDesc):
    dictResult = {}
    listResult = []
    opts = Options ()
    for descTuple in optionDesc:
        tlen = len (descTuple)
        optstr = descTuple [0]
        if (tlen >= 2) and (descTuple [1] != None):
            varname = descTuple [1]
        else:
            varname = optstr
        if tlen >= 3:
            typechar = descTuple [2]
        else:
            typechar = None
        if tlen >= 4:
            default = descTuple [3]
        else:
            default = None
        if tlen >= 5:
            help = descTuple [4]
        else:
            help = varname
        descTuple = (optstr, varname, typechar, default, help)
        dictResult [optstr] = descTuple
        listResult.append (descTuple)
        if typechar != '::':
            opts.setValue (varname, default, typechar)
        else:
            setattr (opts, varname, [])
    return opts, dictResult, listResult

def usage (optionDesc, key, docString):
    write = sys.stderr.write
    unknownOption = (key != 'h') and (key != '?')
    if unknownOption:
        write ('unknown Option -%s\n' % key)
    write ('Usage: %s [options] args\n' % sys.argv [0])
    if docString:
        write (docString)
    for tag, varname, typechar, default, help in optionDesc:
        if typechar == '::':
            fullTag = "-%s %s" % (tag, varname)
            addOn = ' (multiple)'
        elif typechar == ':':
            fullTag = "-%s %s" % (tag, varname)
            addOn = ' (default %s)' % `default`
        else:
            fullTag = "-%s" % (tag)
            addOn = ''
        write ('\t%-15s %s%s\n' % (fullTag, help, addOn))
    if unknownOption:
        sys.exit (1)
    else:
        sys.exit (0)

def version (verstring):
    print verstring
    sys.exit (0)

def parseArgs (optionDesc, docString, args = None):
    inOptions = 1
    index = -1
    if args == None:
        args = sys.argv [1:]
    opts, descDict, optionDesc = _buildDescDict (optionDesc)
    try:
        while inOptions:
            index = index + 1
            current = args [index]
            if current [0] in "-+":
                option = current [1:]
                optstr, varname, typechar, default, help = descDict [option]
                if optstr == 'version':
                    version (default)
                if typechar:
                    index = index + 1
                    value = args [index]
                    if type (default) == type (1):
                        value = intArg (value, option)
                else:
                    value = 1
                opts.setValue (varname, value, typechar)
            else:
                inOptions = None
    except KeyError, key:
        usage (optionDesc, str (key), docString)
    except IndexError:
        pass
    return opts, args [index:]

trapKeyboardInterrupt = 1

def optMain2 (mainRoutine, optDesc, args = None):
    options, args = parseArgs (optDesc, mainRoutine.__doc__, args)
    try:
        mainRoutine (options, args)
    except KeyboardInterrupt:
        if trapKeyboardInterrupt:
            sys.stderr.write ('Aborted')
        else:
            (type, value, tb) = (sys.exc_type, sys.exc_value, sys.exc_traceback)
            import traceback
            traceback.print_exception(type, value, tb)
        sys.exit (255)

def optMain (mainRoutine, *optDesc):
    optMain2 (mainRoutine, optDesc)

def genFiles (args):
    for fname in args:
        fname = fname + '.py'
        stream = open (fname, "w")
        stream.write ('\n# %s\n' % fname)
        stream.write ("""
def main (options, args):
    for arg in args:
        pass

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
""")
        stream.close ()

# entry when called as script
if __name__ == '__main__':
    genFiles ( sys.argv[1:] )

