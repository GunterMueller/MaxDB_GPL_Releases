
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

class NullAcquest:
    def acquire (self, name, default = None):
        #print 'acquire from top'
        return default

    def getAcquisitionList (self, name, all = None):
        return []

    def setAcquiredAttribute (self, name, value):
        pass  # alternative: throw an exception

_defaultTop = NullAcquest ()
_listType = type ([])

class Acquest:
    def __init__ (self, context = None):
        if context is None:
            context = _defaultTop
        self.acquisitionContext = context

    def setContext (self, context):
        assert not (self is context)
        self.acquisitionContext = context

    def acquire (self, name, default = None):
        #print '<acquire name="%s" self="%s">' % (name, self)
        try:
            result = getattr (self, name)
            #print 'acquire', name, 'in', self
        except AttributeError:
            try:
                context = self.acquisitionContext
            except AttributeError:
                print 'no context in', self
            #try:
            result = self.acquisitionContext.acquire (name, default)
            #except AttributeError:
            #    result = default
        #print '</acquire>'
        return result

    def lockAcquisitions (self, *names):
        for name in names:
            value = self.acquire (name)
            setattr (self, name, value)

    def promoteContext (self, *acquisitions):
        for acquisition in acquisitions:
            # print 'promote acquisition from', self, 'to', acquisition
            assert not (self is acquisition)
            acquisition.acquisitionContext = self


    def getAcquisitionList (self, name, all = None):
        definesIt = hasattr (self, name)
        if all or not definesIt:
            upstream = self.acquisitionContext.getAcquisitionList (name, all)
        else:
            upstream = []
        if definesIt:
            value = getattr (self, name)
        else:
            value = None
        upstream.append ((value, self))
        return upstream

    def printAcquisitionList (self, name, all = None):
        list = self.getAcquisitionList (name, all)
        print 'occurrences of', name
        for value, obj in list:
            print '   ', value, obj

    def setAcquiredAttribute (self, name, value):
        if hasattr (self, name):
            setattr (self, name, value)
        else:
            self.acquisitionContext.setAcquiredAttribute (name, value)

    def superAcquire (self, name, default = None):
        result = self.acquisitionContext.acquire (name, default)
        return result

    def uncoupleDependents (self):
        self.acquisitionContext = _defaultTop
        self.uncoupleSubDependents ()

    def uncoupleSubDependents (self):
        self.uncoupleDependentsInList (self.__dict__.values ())

    def uncoupleDependentsInList (self, list):
        for object in list:
            if isinstance (object, Acquest):
                object.uncoupleDependents ()
            elif type (object) == _listType:
                self.uncoupleDependentsInList (object)

