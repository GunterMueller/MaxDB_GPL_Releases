
import sys
import os
envget = os.environ.get
import time

from sql import sqlHelper
import qaccDB

class QACC_Session:
    def __init__ (self, session = None):
        if session == None:
            session = qaccDB.connect ()
        self.session = sqlHelper.ExtendedConnection (session, sqlHelper.SelfDict ())

    def getConfigurationId (self, configuration):
        configSet = ConfigurationSet (self.session)
        return configSet.useConfig (configuration)

    def createRun (self, dir, when, configID):
        (runID, ) = self.session.sqlX ('select %(runid)s.nextval into ? from dual')
        self.session.sqlX ('insert into %(Runs)s (%(run-id)s, %(dir)s, %(when)s, %(config-id)s) values (?, ?, ?, ?)',
            [runID, dir, when, configID])
        return runID

    def createRunFromConfiguration (self, configuration, when):
        configID = self.getConfigurationId (configuration)
        return self.createRun (configID, when)

    def commit (self):
        self.session.commit ()

    def findRunID (self, dir, when, configID):
        when = padTimestamp (when)
        cursor = self.session.sqlX ('''select %(run-id)s from %(Runs)s
            where %(dir)s = ? and %(when)s = ? and %(config-id)s = ?''',
                [dir, when, configID])
        row = cursor.next ()
        if row:
            result = row [0]
        else:
            result = None
        return result

    def invalidateCache (self, dir, when, configID):
        self.session.sqlX ('''delete from %(ReportCache)s
            where %(title)s in
            (select %(title)s from %(ReportConfigs)s where %(config-id)s = ?)
            ''', [configID])
        runID = self.findRunID (dir, when, configID)
        if runID != None:
            self.session.sqlX ('''delete from %(Data)s
                where %(run-id)s = ?''', [runID])
            self.session.sqlX ('''delete from %(Runs)s
                where %(run-id)s = ?''', [runID])

    def insertData (self, dir, target, runID, success, quality = None):
        # print 'insertData', dir, target, runID, ok, quality
        self.session.sqlX ('''insert into %(Data)s
            (%(dir)s, %(target)s, %(run-id)s, %(success)s, %(quality)s)
            values (?, ?, ?, ?, ?)
            ''', [dir + '/.', target, runID, success, quality], rcToIgnore = [-1, 200])

InvalidConfiguration = 'configuration not unique'

class ConfigurationSet:
    def __init__ (self, session):
        self.session = session
        self.dict = {}

    def createConfig (self, config):
        id = self.nextId ()
        for propname, propvalue in config.items ():
            self.insertProperty (id, propname, propvalue)
        return id

    def useConfig (self, configuration):
        ids = self.findConfigs (configuration)
        if len (ids) == 0:
            result = self.createConfig (configuration)
        elif len (ids) == 1:
            result = ids [0]
        else:
            raise InvalidConfiguration, configuration
        return result

    def findConfigs (self, propertyDict = None, **properties):
        propertyDict = self.mergeDicts (propertyDict, properties)
        items = propertyDict.items ()
        from cStringIO import StringIO
        stream = StringIO ()
        stream.write ('select c0.%(config-id)s from %(Configurations)s c0')
        for i in xrange (1, len (items)):
            stream.write (', %(Configurations)s c' + `i`)
        stream.write ('\nwhere 1 = 1 \n')
        for i in xrange (len (items)):
            stream.write ('and c%d.%%(propname)s = ? and c%d.%%(propvalue)s = ? \n' % (i, i))
        for i in xrange (1, len (items)):
            stream.write ('and c%d.%%(config-id)s = c%d.%%(config-id)s ' % (i - 1, i))
        sql = stream.getvalue ()
        # print sql
        arguments = []
        for name, value in items:
            arguments.append (name)
            arguments.append (value)
        cursor = self.session.sqlX (sql, arguments)
        result = []
        for (idVal,) in cursor:
            result.append (idVal)
        return result

    def mergeDicts (self, *dicts):
        result = {}
        for dict in dicts:
            if dict != None:
                result.update (dict)
        return result


    def expandConfigIds (self, ids):
        return map (self.getConfigById, ids)


    def getConfigById (self, id):
        if self.dict.has_key (id):
            return self.dict [id]
        self.dict [id] = result = self.selectConfig (id)

    #
    # database related stuff
    #
    def commit (self):
        self.session.commit ()

    def nextId (self):
        (result, ) = self.session.sqlX ('select %(confid)s.nextval into ? from dual')
        return result

    def insertProperty (self, id, name, value):
        self.session.sqlX (
            'insert into %(Configurations)s (%(config-id)s, %(propname)s, %(propvalue)s) values (?, ?, ?)',
            [id, name, value])

    def selectConfig (self, id):
        cursor = self.session.sqlX (
            'select %(propname)s, %(propvalue)s from %(Configurations)s where %(config-id)s = ?', [id])
        dict = {}
        for name, value in cursor:
            dict [name] = value
        result = apply (Configuration, (id,), dict)
        return result

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

    def __getitem__ (self, name):
        if self.properties.has_key (name):
            return self.properties [name]
        else:
            raise KeyError, name

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


def padTimestamp (when):
    diff = 14 - len (when)
    if diff > 0:
        when = when + ('0' * diff)
    return when

_stdPlatforms = {
    'linux2': 'linux',
    'sunos5': 'sunos',
    'aix4': 'aix',
    'osf1V4': 'dec',
    'reliantunix-n5': 'sni',
    'hp-uxB': 'hp',
    'win32': 'mswin',
}

stdOptions = [
        ('platform', None, ':', _stdPlatforms.get (sys.platform, sys.platform), 'name of platform'),
        ('release', None, ':', envget ('RELVER'), 'release id'),
        ('bit64', None, '', envget ('BIT64'), 'assume 64 bit environment'),
        ('when', None, ':', None, 'date of test run'),
        ('P', 'property', '=', None, 'additional configuration properties'),
        ('OWN', None, ':', envget ('OWN'), 'root of development tree'),

]

