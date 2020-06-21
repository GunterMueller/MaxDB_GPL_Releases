
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

class Configuration:
    stdProperties = ['release', 'platform', 'bits']
    def __init__ (self, id, **properties):
        self.id = id
        self.properties = properties.copy ()
        self.itemCache = None

    def __getattr__ (self, name):
        if self.properties.has_key (name):
            return self.properties [name]
        else:
            raise AttributeError, name

    def items (self):
        if self.itemCache != None:
            return self.itemCache
        result = self.itemCache = []
        for key in self.stdProperties:
            try:
                result.append ((key, self.properties [key]))
            except KeyError:
                pass
        items = self.properties.items ()
        items.sort ()
        for key, value in items:
            if not (key in self.stdProperties):
                result.append ((key, value))
        return result

    def __str__ (self):
        return '[Config ' + `self.id` + ' ' + `self.properties` + ']'


class ConfigurationSet:
    def __init__ (self, session):
        self.session = session
        self.dict = {}

    def createConfig (self, config):
        id = self.nextId ()
        for propname, propvalue in config.items ():
            self.insertProperty (id, propname, propvalue)

    def updateConfig (self, config):
        pass

    def findConfig (self, config):
        pass

    def findConfigs (self, **properties):
        items = properties.items ()
        from cStringIO import StringIO
        stream = StringIO ()
        stream.write ('select c0.id from configurations c0')
        for i in xrange (1, len (items)):
            stream.write (', configurations c' + `i`)
        stream.write ('\nwhere true \n')
        for i in xrange (len (items)):
            stream.write ('and c%d.propname = ? and c%d.propvalue = ? \n' % (i, i))
        for i in xrange (1, len (items)):
            stream.write ('and c%d.id = c%d.id ' % (i - 1, i))
        sql = stream.getvalue ()
        print sql
        select = self.session.prepare (sql)
        arguments = []
        for name, value in items:
            arguments.append (name)
            arguments.append (value)
        cursor = select.execute (arguments)
        result = []
        for (idVal,) in cursor:
            print 'loop start'
            result.append (self.selectConfig (idVal))
            print 'loop end'
        print 'ready to return'
        return result

    #
    # database related stuff
    #
    def commit (self):
        self.session.commit ()

    _nextIdCmd = None
    def nextId (self):
        if self._nextIdCmd is None:
            self._nextIdCmd = self.session.prepare ('select confid.nextval into ? from dual')
        (result, ) = self._nextIdCmd.execute ()
        return result

    _insertPropertyCmd = None
    def insertProperty (self, id, name, value):
        if self._insertPropertyCmd is None:
            self._insertPropertyCmd = self.session.prepare (
                'insert into Configurations (id, propname, propvalue) values (?, ?, ?)')
        self._insertPropertyCmd.execute ([id, name, value])

    _selectConfigCmd = None
    def selectConfig (self, id):
        if self._selectConfigCmd is None:
            self._selectConfigCmd = self.session.prepare (
                'select propname, propvalue from configurations where id = ?')
        cursor = self._selectConfigCmd.execute ([id])
        dict = {}
        for name, value in cursor:
            dict [name] = value
        result = apply (Configuration, (id,), dict)
        return result

#
#
#

class SchemaManager:
    def __init__ (self, schema, session):
        self.schema = schema
        self.session = session

    def create (self):
        for kind, name, definition in _schema:
            self.session.sql ("create %s %s %s" % (kind, name, definition))
        self.session.commit ()

    def drop (self):
        for kind, name, definition in _schema:
            self.session.sql ("drop %s %s" % (kind, name))
        self.session.commit ()

    def recreate (self):
        self.drop ()
        self.create ()
#
# tests
#
def createSampleConfigurations ():
    set = ConfigurationSet (createSession ())
    for release in ['7.2', '7.3', '7.4']:
        for platform in ['win', 'linux', 'sun', 'aix', 'hp', 'dec', 'sni']:
            for bits in ['32', '64']:
                config = Configuration (None, release = release,
                    platform = platform, bits = bits)
                set.createConfig (config)
    set.commit ()

def findConfiguration ():
    set = ConfigurationSet (createSession ())
    for release in ['7.2', '7.3', '7.4']:
        for platform in ['win', 'linux', 'sun', 'aix', 'hp', 'dec', 'sni']:
            configs = set.findConfigs (release = release,
                platform = platform)
            print '================='
            for config in configs:
                print config.id, config.items ()

#
# helper functions
#

_schema = [
    ('sequence', 'confid', ''),
    ('table', 'Configurations', """(
        id integer key,
        propname varchar (20) key,
        propvalue varchar (50)
        )
    """),
]

def createSession (session = None):
    if session != None:
        return session
    from sql import sapdb72
    session = sapdb72.connect ('DANIEL', 'DANIEL', 'db72')
    return session

_cmds = {
    'sample': createSampleConfigurations,
    'find': findConfiguration,
}

def main (args):
    cmd = args [0]
    if cmd in ['create', 'drop', 'recreate']:
        manager = SchemaManager (_schema, createSession ())
        getattr (manager, cmd) ()
    else:
        try:
            handler = _cmds [cmd]
            handler ()
        except KeyError:
            print 'command', cmd, 'not defined'

if __name__ == "__main__":
    import sys
    main (sys.argv [1:])

