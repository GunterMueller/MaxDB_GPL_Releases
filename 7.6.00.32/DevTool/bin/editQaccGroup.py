
# editQaccGroup.py

import mylib

from testTools.QACC import qaccDB

def executePrepared (prepared, parameters, allowedError):
    try:
        prepared.execute (parameters)
    except qaccDB.SQLError, sqlErr:
        if errorCode == allowedError:
            pass
        else:
            raise

def add (session, groupName, args):
    insert = session.prepare ('insert into "TargetGroup" ("name", "target") values (?, ?)')
    for target in args:
        executePrepared (insert, [groupName, target], 200)

def replace (session, groupName, args):
    clear (session, groupName)
    add (session, groupName, args)

def remove (session, groupName, args):
    delete = session.prepare ('delete from "TargetGroup" where "name" = ? and "target" = ?')
    for target in args:
        executePrepared (delete, [groupName, target], 100)

def clear (session, groupName, args = None):
    session.sqlX ('delete from "TargetGroup" where "name" = ?', [groupName])

def show (session, groupName, args = None):
    if groupName:
        print "group " + groupName + ":"
        for (target,) in session.sqlX ('select distinct "target" from "TargetGroup" '
                'where "name" = ? order by "target"', [groupName]):
            print '   ', target
    else:
        print "all groups:"
        for (groupName,) in session.sql ('select distinct "name" from "TargetGroup" order by "name"'):
            print '   ', groupName


_defaultGroups = [
    ('Main', ['all.mac', 'kernel.shm', 'quickknl.shm', 'slowknl.shm',
        'dbm.mac', 'allpre.mac', 'odbc.mac', 'ls.mac']),
    ('Scripting', ['sapdbpy.mac', 'dbmpy.mac', 'repmanpy.mac',
        'sapdbperl.mac', 'dbmperl.mac', 'repmanperl.mac']),
]

def defaults (session):
    for name, targets in _defaultGroups:
        replace (session, name, targets)

def checkOptions (options):
    editOptions = [options.add, options.replace, options.remove, options.clear]
    actionCount = len (filter (None, editOptions))
    if actionCount > 1:
        mylib.fatalError ('only one of -add, -replace, -remove or -clear allowed',
            'editQaccGroup')
    if (actionCount == 1) and not options.groupName:
        mylib.fatalError ('no -groupName specified', 'editQaccGroup')


def main (options, args):
    checkOptions (options)
    session = qaccDB.connect ()
    if options.defaults:
        defaults (session)
    for optionName in ['add', 'replace', 'remove', 'clear', 'show']:
        if getattr (options, optionName):
            routine = globals () [optionName]
            routine (session, options.groupName, args)
    session.commit ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('show', None, '', None, 'display group targets'),
        ('groupName', None, ':', None, 'name of target group to edit'),
        ('add', None, '', None, 'add command line arguments to group'),
        ('replace', None, '', None, 'replace group targets with command line arguments'),
        ('remove', None, '', None, 'remove command line arguments from group'),
        ('clear', None, '', None, 'remove the whole target group'),
        ('defaults', None, '', None, 'reset the default groups'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

