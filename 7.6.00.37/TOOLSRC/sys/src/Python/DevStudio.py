# DevStudio
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

"""
NAME

DevStudio - access to path hierarchy of SAP DB development environment 
            'SAP DB Developer Studio'

SYNOPSIS

import DevStudio

try:

    module = DevStudio.Module( ':Kern/Optimizer/join.c' )
    path_to_mod = DevStudio.VmakePath.dir( module )
    module.Incopy( 'D:\\V72\\tmp\\' + module.GetBasename() )
    
except DevStudio.ModuleTypeError, name :
    suite
except DevStudio.MissingModule, name :
    suite
except DevStudio.VmakeServerError, message :
    suite
except OSError, OSErrorClass :
    suite
"""

import VmakeServer
import os, string, re, posixpath, errno
VERSION = '1.02';

# module types 
Description = "/sys/desc/"
Source      = "/sys/src/"

# exceptions
ModuleTypeError = 'ModuleTypeError'
MissingModule   = 'MissingModule'
VmakeServerError= 'VmakeServerError'

###################################
class DevPath:
    "class for one 'vmake path' hierarchy"
    
    __remote_regex = re.compile('(\w{2,}):(([a-zA-Z]:)?([\\\\/]\w+)+)');
    __os_regex     = re.compile('[a-zA-Z]:\\.*')
    
    def __init__(self, Basepath):
        try:
            self.Status = {}
            if Basepath[0]:
                regmatch = self.__remote_regex.match( Basepath[0] )
                if regmatch:
                    self.Status['Basepath'] = regmatch.group(2)
                    self.Status['Basehost'] = regmatch.group(1)
                else:
                    self.Status['Basepath'] = Basepath[0]
                    self.Status['Basehost'] = 'localhost'
                if self.__os_regex.match( self.Status['Basepath'] ):
                    self.Status['OS'] = 'NT'
                elif self.Status['Basepath'][0:2] == '\\\\':
                    self.Status['OS'] = 'NT'
                else:
                    self.Status['OS'] = 'UNIX'
                if Basepath[1]:
                    self.Nextpath = DevPath(Basepath[1:])
        except IndexError:
            pass

    def explain(self):
        """
        returns a list of dictionaries concerning the path hierarchy,
        each dictionary contain 'Basehost', 'Basepath' and 'OS'"""
        try:
            return [ self.Status ] + self.Nextpath.explain()
        except NameError:
            return [ self.Status ]
    
    def dir(self, module, startlevel=0, stoplevel=-1):
        """
        returns path to module within 'VMAKE_PATH'
        usage: dir( module [[, startlevel], stoplevel]) """
        if startlevel > 0:
            try:
                return self.Nextpath.dir(module, startlevel-1, stoplevel-1)
            except AttributeError:
                raise MissingModule, module.GetName()
        else:
            try:
                self.__check_module(module)
                # create module instance member 'Path' and 'Host'
                # module member 'Stat' created in __check_module()
                module.module.Path = self.__compose_path( module )
                module.module.Host = self.Status[ 'Basehost']
                if self.Status['OS'] == 'NT':
                    module.module.Path = os.path.normpath(module.module.Path)
                return module.module.Path
            except OSError, val:
                try:
                    if val.errno == errno.ENOENT :
                        if stoplevel <> 0:
                            return self.Nextpath.dir(module, startlevel-1, 
                                stoplevel-1)
                        else:
                            raise MissingModule, module.GetName()
                    else:
                        raise MissingModule, module.GetName()
                except AttributeError:
                    raise MissingModule, module.GetName()
            except VmakeServer.NetError, val:
                raise VmakeServerError, val
            
    def __compose_path(self, module):
        return self.Status['Basepath'] + module.GetType() + \
               module.GetLayer() + module.GetBasename()

    def __check_module(self, module): # could throw VmakeServer.Error or OSError
        module.module.Stat = VmakeServer.stat( self.Status['Basehost'], 
            self.__compose_path(module))

    def getExtraInfo(self):
        if not hasattr(DevPath, 'ExtraInfo'):
            extra = os.path.join(os.environ['TMP'], 'Extra')
            if posixpath.isfile( extra ): os.unlink( extra )
            mod = Module('Extra')
            mod.Incopy( extra )
            fd = open(extra, 'r')
            extra_regex = re.compile('\.\w+\s+\w+')
            # create static member
            DevPath.ExtraInfo = filter( extra_regex.match, fd.readlines())
            fd.close()

        
###################################
class BaseModule:
    "superclass for all possible kinds of modules"
    
    def __init__(self, name):
        self.Name = name
        self.getType()
    
    stdext_regex = re.compile('\.(c|cpp|h|hpp|p|s|rc|dlg|mc|def|idl|y|l)$')

    def is_std_ext(self):
        return self.stdext_regex.search( self.Name )

    def getType(self):
        if not hasattr(self, 'Type'):
            if self.is_descname():
                self.Type = Description
            elif self.is_srcname():
                self.Type = Source
            else:
                raise ModuleTypeError, self.Name
        return self.Type

    def getBaseName(self):
        return os.path.basename( self.Name )

    def is_srcname(self):
        ext = os.path.splitext( self.Name )[ 1 ]
        if  ( ext and self.src_regex.match(os.path.splitext(self.Name)[0])):
            return self.is_std_ext()
        return self.src_regex.match( self.Name )
        
    def is_descname(self):
        return self.desc_regex.match( self.Name )


###################################
class OldstyledModule( BaseModule ):
    "subclass for old styled module names"

    desc_regex = re.compile('^\w+(-\w+)?\.(mac|shm|lnk|dld|shr|rel|lib|com)$' +  '|'
        '(Extra|Langinfo|Langextinfo)$')
    src_regex  = re.compile('^[vhgi]([a-zA-Z]{2,})\d{2,}([a-zA-Z]+)?(-\w+)?(\.\w+)?$')
    extra_regex= re.compile('[\w-]+(\.\w+)+$')

    def getLayer(self):
        if not hasattr(self, 'Layer'):
            if self.getType() == Description:
                self.Layer = ""
            else:
                # it's a source file
                # extra files already have a layer
                self.Layer = self.src_regex.match(self.Name).group(1) + '/'
        return self.Layer
    
    def getType(self):
        try:
            return BaseModule.getType(self)
        except ModuleTypeError, name:
            # it my be a source file of type 'Extra'
            ext = self.extra_regex.match(self.Name)
            if ext and ext.group(1):
                VmakePath.getExtraInfo()
                reg = re.compile(ext.group(1) + '\s+(\w+)')
                for i in VmakePath.ExtraInfo:
                    if reg.match(i):
                        break
                else:
                    raise ModuleTypeError, self.Name
                self.Layer = reg.match(i).group(1) + '/'
                self.Type = Source
                return self.Type
            raise ModuleTypeError, self.Name


###################################
class NewstyledModule( BaseModule ):
    "subclass for fully qualified module names"

    desc_regex = re.compile('^::((((\w+/)*)?\w+(-\w+)?\.(mac|shm|lnk|dld|shr|rel|lib|com))|(Extra|Langinfo|Langextinfo))$')
    src_regex  = re.compile('^:([^:](\w+/)+)\w+(-\w+)?(\.\w+)?$')
    
    def getLayer(self):
        if not hasattr(self, 'Layer'):
            if self.getType() == Description:
                self.Layer = self.desc_regex.match( self.Name ).group(3)
                if not self.Layer: self.Layer = ''
            else:
                self.Layer = self.src_regex.match( self.Name ).group(1)
        return self.Layer

    
###################################
class Module:
    "proxy class for hiding module implementation"

    def __init__(self, name):
        try:
            # try old styled module name
            self.module = OldstyledModule( name )
        except ModuleTypeError:
            try:
                # try new styled module name
                self.module = NewstyledModule( name )
            except ModuleTypeError:
                try:
                    # try absolute path name
                    name = os.path.abspath( name )
                    if ( os.path.commonprefix( [ VmakePath.Status['Basepath'], name ] )
                        == VmakePath.Status['Basepath']):
                        # absolute path must match with $OWN
                        # compose qualified module name
                        regex = re.compile('[\\\\/]sys[\\\\/]desc[\\\\/](.+)')
                        if regex.search( name ):
                            newname = '::' + re.sub('\\\\', '/', regex.search( name ).group(1))
                            self.module = NewstyledModule( newname )
                        else:
                            regex = re.compile('[\\\\/]sys[\\\\/]src[\\\\/](.+)')
                            if regex.search( name ):
                                newname = ':' + re.sub('\\\\', '/', regex.search( name ).group(1))
                                self.module = NewstyledModule( newname )
                        if not hasattr(self, 'module') :
                            raise ModuleTypeError, name
                    else:
                        raise ModuleTypeError, name
                except ModuleTypeError:
                    raise ModuleTypeError, name

    def GetName(self):
        'return the name of the module'
        return self.module.Name

    def GetBasename(self):
        'return the base name of the module'
        return self.module.getBaseName()

    def GetLayer(self):
        'returns the layer of the module'
        return self.module.getLayer()

    def GetType(self):
        """
        return the type of the module, possible types 
        are 'Description' or 'Module'"""
        return self.module.getType()

    def GetPath(self):
        'return the path to the module'
        return self.module.Path
    
    def GetStat(self):
        'return the Stat struct for the module'
        return self.module.Stat
    
    def Incopy(self, path, startlevel=0, stoplevel=-1):
        "copies the module to 'path', 'path' contains the filename"
        try:
            if not hasattr(self, 'Path'): VmakePath.dir( self )
            VmakeServer.incopy( self.module.Host, self.module.Path, path )
        except VmakeServer.NetError, val:
            raise VmakeServerError, val

# initialize "vmake path" hierarchy
VmakePath = DevPath(string.splitfields(os.environ['VMAKE_PATH'], ','))


