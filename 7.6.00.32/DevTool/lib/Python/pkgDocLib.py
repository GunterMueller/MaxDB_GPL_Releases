
import re
import os
import shutil

import fileutil
import html

from acquisition import Acquest
from FileSystem import FileSystem
from docLib import genDate
from cppDoc import CppDoc
# from VdnWeaver import VdnWeaver

ospath = os.path

class PackageDoc (Acquest):
    def __init__ (self, name, home = '.', members = None, **options):
        Acquest.__init__ (self)
        self.name = name
        self.home = home
        self.members = members
        # options
        self.wantIndex = None
        options2attributes (self, options)

    def loadInfo (self, sourceFS):
        try:
            pkgFile = fileutil.defaultExtension (self.name, 'docpkg')
            data = sourceFS.subdir (self.home).open (pkgFile, 'r').read ()
        except IOError:
            pass
        else:
            self.PackageDoc = PackageDoc
            self.SourceDoc = SourceDoc
            self.DirectoryDoc = DirectoryDoc
            exec data in self.__dict__

    def make (self, sourceFS, targetFS, unconditional = None):
        self.loadInfo (sourceFS)
        ok = 1
        sourceFS = sourceFS.subdir (self.home)
        targetFS = targetFS.subdir (self.home)
        self.wantIndex = None    # index mechanism a bit unclear to date
                                 # but something like pushIndex/popIndex is required
        if self.wantIndex:
            self.weaver = VdnWeaver (self.name, targetFS.base,
                self.acquire ('root'))
        if self.acquire ('addDefaults'):
            self.addDefaultMember (sourceFS)
        self.members = map (normalizeMember, self.members,
            [sourceFS] * len (self.members))
        apply (self.promoteContext, tuple (self.members))
        memberdocs = []
        for member in self.members:
            thisOK, info = member.make (sourceFS, targetFS, unconditional)
            if thisOK:
                memberdocs.append (info)
            ok = ok and thisOK
            if ok and self.wantIndex:
                self.weaver.save ()
        if self.wantIndex:
            self.weaver.genIndex ()
        # copy optional html
        htmlfile = sourceFS.search (self.name + '.html', self.name + '.htm')
        if htmlfile:
            if unconditional or requiresUpdate (sourceFS, htmlfile, targetFS, htmlfile):
                shutil.copy2 (sourceFS.absolute (htmlfile), targetFS.absolute (htmlfile))
            info = (self.name, htmlfile, '')
        else:
            info = self.genOverview (sourceFS, targetFS, memberdocs)
        return ok, info

    def addDefaultMember (self, sourceFS):
        files = sourceFS.listdir ()
        for file in files:
            addIt = None
            if (file == 'doc') and (sourceFS.isdir (file)):
                addIt = 1
            else:
                ext = fileutil.extension (self.name)
                if ext in ['.h', '.hpp']:
                    addIt = 1
            if addIt and (file not in self.members):
                self.members.append (file)

    def genOverview (self, sourceFS, targetFS, memberdocs):
        # get package description
        contentsFile = sourceFS.search ('Contents.txt')
        if contentsFile:
            description = self.parseContentsFile (sourceFS.absolute (contentsFile))
        else:
            description = ''
        # todo: get old descriptions
        #     merge and save
        #     set new and updated
        # open html stream
        htmlfile = self.name + '.html'
        title = 'Package ' + self.name
        htmlstream = html.HTMLFile (targetFS.absolute (htmlfile), title)
        # write header
        htmlstream.titleHeader (title)
        htmlstream.pushTag ('Blockquote')
        # write description
        if description:
            htmlstream.write (description)
        # write links to members
        htmlstream.pushTag ('DL')
        htmlstream.pseudoBreak ()
        for name, file, memberDesc in memberdocs:
            # write link
            htmlstream.tag ('DT')
            htmlstream.write (html.href (file, name) + '\n')
            # write description
            if memberDesc:
                htmlstream.tag ('DD')
                htmlstream.write (memberDesc)
        htmlstream.popTag ()
        # close file
        htmlstream.line ()
        genDate (htmlstream)
        htmlstream.close ()
        return (self.name, htmlfile, description)

    contentsRE = re.compile ('\n([a-zA-Z][a-zA-Z ]*):')

    def parseContentsFile (self, contentsFile):
        text = open (contentsFile, 'rt').read ()
        # parse into entries
        entries = self.contentsRE.split ('\n' + text)
        # get description entry
        for i in xrange (len (entries)):
            if entries [i] == 'contents':
                return entries [i + 1]
        return ''


def requiresUpdate (sourceFS, sourcename, targetFS, targetname):
    try:
        targetTime = targetFS.getmtime (targetname)
    except os.error, err:
        return 1
    sourceTime = sourceFS.getmtime (sourcename)
    return sourceTime > targetTime

stringType = type ('')

def normalizeMember (member, sourceFS = None):
    if type (member) == stringType:
        if sourceFS.isdir (member):
            result = DirectoryDoc (member)
        else:
            ext = fileutil.extension (member)
            if ext in ['.pkg', 'docpkg']:
                result = PackageDoc (member)
            else:
                result = SourceDoc (member)
    else:
        result = member
    return result

def packageFromFile (fname, **options):
    dir, file = ospath.split (fname)
    packageName, ext = ospath.splitext (file)
    if not dir:
        dir = '.'
    package = PackageDoc (packageName, dir)
    options2attributes (package, options)
    return package

class SourceDoc (Acquest):
    def __init__ (self, name, **options):
        self.name = name
        options2attributes (self, options)

    def make (self, sourceFS, targetFS, unconditional = None):
        targetfile = fileutil.setExtension (self.name, 'html')
        ok = 1
        if unconditional or requiresUpdate (sourceFS, self.name, targetFS, targetfile):
            ext = fileutil.extension (self.name)
            documenter = _documenters [ext] ()
            self.promoteContext (documenter)
            ok = documenter.readSource (sourceFS.absolute (self.name))
            if ok:
                outfile = targetFS.require (targetfile)
                documenter.genHTML (outfile)
                result = (self.name, outfile, documenter.getShortdesc ())
            else:
                self.acquire ('weaver').removeCurrentEntries ()
                result = None
        return ok, result

    def __repr__ (self):
        return '<SourceDoc ' + self.name + '>'

class DirectoryDoc (Acquest):
    def __init__ (self, name):
        self.name = name

    def make (self, sourceFS, targetFS, unconditional = None):
        files = targetFS.listdir (self.name)
        for file in files:
            if unconditional or requiresUpdate (sourceFS, file, targetFS, file):
                shutil.copy2 (sourceFS.absolute (file), targetFS.absolute (file))
        info = (self.name, self.name, None)
        return ok, info


def options2attributes (obj, options):
    if options:
        for name, value in options.items ():
            setattr (obj, name, value)

_documenters = {
    '.h': CppDoc,
    '.hpp': CppDoc,
    }

