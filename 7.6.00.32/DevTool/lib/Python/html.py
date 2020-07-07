
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
import regsub

verbose = None

class HTMLStream:
    #- used to output HTML
    # targetStream -- all output goes there
    # stack -- holds open tags
    def __init__ (self, outStream, title = None, bodyList = None, frames = None, **bodyAttributes):
        self.targetStream = outStream
        self.stack = []
        if title != None:
            self.pushTag ('HTML')
            outStream.write ('<HEAD><TITLE>' + title + '</TITLE></HEAD>\n')
            if not frames:
                apply (self.pushTag, ('BODY', bodyList), bodyAttributes)
                #outStream.write ('<BODY' + attributeString (bodyList, bodyAttributes) + '>\n')
                self.pseudoBreak ()

    def close (self, frames = None):
        self.popTag (-1)
        self.pseudoBreak ()
        self.targetStream.close ()

    def write (self, string):
        self.targetStream.write (string)

    def tag (self, tagString, attributes = None, **byName):
        self.targetStream.write ('<' + tagString + attributeString (attributes, byName) + '>')

    def pushTag (self, tagString, attributes = None, **byName):
        self.targetStream.write ('<' + tagString + attributeString (attributes, byName) + '>')
        self.stack.append (tagString)

    def pushTags (self, *taglist):
        for tag in taglist:
            self.pushTag (tag)

    def popTag (self, count = 1):
        if count < 0:
            count = len (self.stack)
        pos = len (self.stack)
        while count > 0:
            count = count - 1
            pos = pos - 1
            self.targetStream.write ('</' + self.stack[pos] + '>')
            del self.stack[pos]

    def popTagSafe (self, tag):
        stack = self.stack
        pos = len (stack) - 1
        toptag = stack [pos]
        if string.lower (toptag) == string.lower (tag):
            self.targetStream.write ('</' + toptag + '>')
            del stack [pos]
        else:
            raise "Tags don't match", (toptag, tag)

    def popTagsSafe (self, *tagList):
        for tag in taglist:
            self,popTagSafe (self, tag)

    def popIfPushed (self, tag):
        stack = self.stack
        if (len (stack) > 0) and (string.lower (self.stack [-1]) == string.lower (tag)):
            self.popTag ()

    def dumpStack (self):
        print self.stack

    def lineBreak (self, count = 1):
        self.targetStream.write ('<BR>\n' * count)

    def pseudoBreak (self):
        self.targetStream.write ('\n')

    def titleHeader (self, title, attributes = None, **byName):
        line = header (title, 1, mergeAttributes (attributes, byName),
            align = "center") + '<HR>\n'
        self.write (line)

    def line (self, attributes = None, **byName):
        self.targetStream.write ('\n<HR' + attributeString (attributes, byName) + '>\n')

    def commentLine (self, text):
        self.targetStream.write ('<!--' + text + '-->\n')

    def splitWrite (self, *texts):
        # this is only valid is targetStream is a Tee
        apply (self.targetStream.splitWrite, texts)

def attributeString (attributes, dict = None):
    result = ''
    if dict != None:
        attributes = mergeAttributes (attributes, dict)
    if attributes != None:
        for name, val in attributes:
            if val == None:
                valstr = ''
            else:
                valstr = '="' + str (val) + '"'
            result = result + ' ' + name + valstr
    return result

def tagged (tagString, string, attributes = None, **byName):
    return '<' + tagString + attributeString (attributes, byName) + '>' + string + '</' + tagString + '>'

def taggednl (tagString, string, attributes = None, **byName):
    return tagged (tagString, string, mergeAttributes (attributes, byName)) + '\n'

def paragraph (string, attributes = None, **byName):
    return taggednl ('P', string, mergeAttributes (attributes, byName))

def preformatted (string):
    return taggednl ('PRE', string)

def header (string, level = 1, attributes = None, **byName):
    return taggednl ('H' + `level`, string, mergeAttributes (attributes, byName))

def href (url, alternateText = None, attributes = None, **byName):
    if alternateText == None:
        alternateText = url
    return ('<A HREF="' + url + '" ' + attributeString (attributes, byName) + '>'
        + alternateText + '</A>')

def imgLink (url, imageSrc, alternateText = None, attributes = None, **byName):
    attrlist = mergeAttributes (attributes, byName)
    pic = anyTag ('img', attrlist, src = imageSrc)
    return href (url, pic)

def mailto (email, name = None, attributes = None, **byName):
    if name == None:
        name = email
    return ('<A HREF="mailto:' + email + '" ' + attributeString (attributes, byName) + '>'
        + name + '</A>')

def anchor (name, text = '', attributes = None, **byName):
    return '<A NAME="' + name + '" '+ attributeString (attributes, byName) + '>' + text + '</A>'

def anyTag (tag, attributes = None, **byName):
    return '<' + tag + attributeString (attributes, byName) + '>'

def pythonMacro (name, contents = None, attributes = None, **byName):
    entry = '<--pymacro ' + name + attributeString (attributes, byName) + '-->'
    if contents != None:
        exit = contents + '<--/pymacro ' + name + '-->'
    else:
        exit = ''
    return entry + exit

def attributes (**dict):
    return mergeAttributes ([], dict)

def mergeAttributes (list, dict):
    if dict == None:
        return list
    if list == None:
        list = []
    for key, value in dict.items ():
        list.append ((key, value))
    return list

def toDictionary (attributes):
    result = {}
    lower = string.lower
    for key, value in attributes:
        result [lower (key)] = value
    return result

def getAttribute (attributes, searchKey):
    searchKey = string.lower (searchKey)
    for key, value in attributes:
        if string.lower (key) == searchKey:
            return value
    raise KeyError, searchkey

def HTMLFile (fname, title = None, bodyList = None, frames = None, **bodyAttributes):
    stream = open (fname, "w")
    return apply (HTMLStream, (stream, title, bodyList, frames), bodyAttributes)

def cleanHTML (str):
    gsub = regsub.gsub
    for pat, repl in [  ('&', '&amp'),
                        ('<', '&lt;'),
                        ('>', '&gt;'),
                     ]:
        str = gsub (pat, repl, str)
    return str
