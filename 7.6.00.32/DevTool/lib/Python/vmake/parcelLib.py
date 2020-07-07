
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

import string
import re
import os

import yutil

_version = (0, 0, 1)
_versionString = string.join (map (str, _version), '.')

_desc = os.environ ['DESC']
_instroot = os.environ ['INSTROOT']

class Parcel:
    defaultVmakeCommands = {'rebuild': '-U', 'make': ''}

    def __init__ (self, parcelName):
        self.parcelName = parcelName
        self.loadDescription ()

    def loadDescription (self):
        # get iview name
        fullName = os.path.join (_desc, 'parcels', self.parcelName + '.parcel')
        localName = yutil.iview (fullName)
        if localName is None:
            raise ParcelException ('not found: ' + fullName)
        # get attributes
        attributes = {}
        execfile (localName, attributes)
        # set fileName
        self.fileName = attributes.get ('fileName',
            'sapdb-' + self.parcelName + '-%(platform)s-%(release)s')
        # set vmakeTargets
        self.vmakeTargets = attributes.get ('vmakeTargets', [])
        # set vmakeCommands
        self.vmakeCommands = attributes.get ('vmakeCommands', {})
        self.vmakeCommands.update (self.defaultVmakeCommands)
        # set rootDir
        self.rootDir = attributes.get ('rootDir', _instroot)
        # set excludes
        self.excludes = map (re.compile, attributes.get ('excludes', []))
        # set README
        self.README = attributes.get ('README')
        # set INSTALL
        self.INSTALL = attributes.get ('INSTALL')

    def dump (self):
        print self
        for attr in ['fileName', 'vmakeTargets', 'vmakeCommands',
                'rootDir', 'excludes']:
            print "%s: %s" % (attr, getattr (self, attr))
        for attr in ['README', 'INSTALL']:
            print "%s: %s" % (attr, `getattr (self, attr)`[:20])

    def __repr__ (self):
        return 'Parcel ("' + self.parcelName + '")'

    def packParcel (self, options):
        # run vmake
        if options.vmake:
            self.vmake (options)
        # get distributables
        distributables = self.getDistributables (options)
        distributables = self.filterExcludes (distributables)
        # get filename
        filename = self.getFilename (options)
        # change to root dir
        distributables = self.stripCommonRoot (distributables)
        os.chdir (os.path.join (_instroot, self.rootDir))
        # write README INSTALL
        if self.INSTALL:
            open ('INSTALL', 'wt').write (self.INSTALL)
            distributables.insert (0, 'INSTALL')
        if self.README:
            open ('README', 'wt').write (self.README)
            distributables.insert (0, 'README')
        # call pack command
        packer = getattr (self, options.packer + 'Pack')
        packer (filename, distributables)
        print filename

    def getDistributables (self, options):
        distributables = []
        for target in self.vmakeTargets:
            fullPath = os.path.join (_instroot, 'etc', target + '.lst')
            if options.nomake and os.path.exists (fullPath):
                pass
            else:
                os.system ('imf.pl -F ' + target)
            list = open (fullPath, 'rt').readlines ()
            list = map (string.strip, list)
            distributables.extend (list)
        return distributables

    def filterExcludes (self, distributables):
        return filter (self.isIncluded, distributables)

    def isIncluded (self, fname):
        for exclude in self.excludes:
            if exclude.search (fname):
                return None
        return 1

    def getFilename (self, options):
        if options.o and not os.path.isdir (options.o):
            return os.path.abspath (options.o)
        platform = options.platform
        release = options.release
        fileName = self.fileName
        if options.parcelName:
            fileName = string.replace (fileName, '-' + self.parcelName + '-',
                '-' + options.parcelName + '-')
        filename = fileName % locals ()
        return os.path.abspath (filename) + '.' + options.packer

    def stripCommonRoot (self, distributables):
        rootDir = normjoin (_instroot, self.rootDir) + os.sep
        rootlen = len (rootDir)
        result = []
        for distfile in distributables:
            fullPath = normjoin (_instroot, distfile)
            if fullPath [:rootlen] == rootDir:
                remainingLen = len (fullPath) - len (rootDir)
                stripped = distfile [-remainingLen:]
                result.append (stripped)
            else:
                raise ParcelError (distfile + ' not in dir ' + self.rootDir)
        return result


    def zipPack (self, outfile, filelist):
        os.system ('zip ' + outfile + ' ' + string.join (filelist))

    def tgzPack (self, outfile, filelist):
        os.system ('tar cf - ' + string.join (filelist) + ' | gzip -c > ' + outfile)

    def vmake (self, options):
        if options.env:
            self.addEnvironmentVariables (options.env)
        # check for an alias
        alias = options.vmake
        # use argument itself if not found
        plainCommand = options.vmake
        vmakeCommand = self.vmakeCommands.get (alias, plainCommand)
        os.system ("imf.pl " + vmakeCommand
            + " " + string.join (self.vmakeTargets))

    def addEnvironmentVariables (self, envList):
        for entry in envList:
            name, value = string.split (entry, '=')
            os.environ [name] = value


class ParcelException:
    def __init__ (self, msg):
        self.msg = msg

    def __str__ (self):
        return self.msg

def defaultPlatformString ():
    osname = os.name
    if os.name == 'nt':
        osname = 'win32'
    return osname

def defaultReleaseString ():
    releaseFile = os.path.join (os.environ ['OWN'], 'Release.dat')
    return open (releaseFile, 'rt').read ()

def defaultPacker ():
    if os.name == 'nt':
        result = 'zip'
    else:
        result = 'tgz'
    return result

def normjoin (*parts):
    full = apply (os.path.join, tuple (parts))
    full =os.path.normpath (full)
    full = os.path.normcase (full)
    return full

