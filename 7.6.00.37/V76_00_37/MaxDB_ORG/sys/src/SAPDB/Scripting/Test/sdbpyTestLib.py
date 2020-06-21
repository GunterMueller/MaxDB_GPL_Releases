#!/usr/bin/env python
#
#   @file   sdbpyTestLib.py
#
#   @author DanielD
#
#   @brief  test framework for python modules
#
#    ========== licence begin  GPL
#    Copyright (c) 2004-2005 SAP AG
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
import traceback
import time
import re
import types


PYTHON_SUPPORTS_FUNCTION_PROPERTIES = None
PYTHON_ITERATORS = None

def calculateVersionVariables ():
    global SDB_SUPPORTS_FUNCTION_PROPERTIES
    def dummyfunc ():
        pass
    try:
        dummyfunc.newattribute = 1
        PYTHON_SUPPORTS_FUNCTION_PROPERTIES = True
    except AttributeError:
        PYTHON_SUPPORTS_FUNCTION_PROPERTIES = None

    global PYTHON_ITERATORS
    if hasattr ([], '__iter__'):
        PYTHON_ITERATORS = True

calculateVersionVariables ()

class Test:
    def assertEqual (self, expected, actual, comment = None):
        if expected != actual:
            msg = 'Diff: expected %s, actual %s' % (repr (expected), repr (actual))
            if comment:
                msg = msg + ' - ' + comment
            raise TestFailure (msg)

    def setRunner (self, runner):
        self.runner = runner

    def __str__ (self):
        return self.__class__.__name__

class TestGroup (Test):
    def __iter__ (self):
        result = []
        for methodname in self.testMethods:
            try:
                method = getattr (self, methodname)
                testcase = self.MethodWrapper (method)
            except AttributeError:
                testcase = self.MethodNotImplemented (self, methodname)
            result.append (testcase)
        return iter (result)

    class MethodWrapper:
        def __init__ (self, method):
            self.method = method

        def run (self, session):
            self.method (session)

        def __str__ (self):
            return self.method.im_class.__name__ + '.' + self.method.__name__

        def __getattr__ (self, name):
            return getattr (self.method, name)

    class MethodNotImplemented:
        todo = 1
        def __init__ (self, object, name):
            self.displayname = object.__class__.__name__ + '.' + name

        def run (self, session):
            raise TestFailure ('not implemented: ' + str (self))

        def __str__ (self):
            return self.displayname

class TestFailure:
    def __init__ (self, msg):
        self.msg = msg

    def __str__ (self):
        return '[ERR: %s]' % self.msg

class TestRunner:
    def __init__ (self, options, testCases):
        self.options = options
        self.connectData = options.connectTuple
        self.testCases = testCases
        self.testsRun = 0
        self.testsOK = 0
        self.timeStarted = time.time ()
        self.createConnectPermutations ()

    def createConnectPermutations (self):
        result = []
        if not self.options.sqlmode and not self.options.component:
            result.append ({})
        elif self.options.sqlmode and self.options.component:
            for sqlmode in self.options.sqlmode:
                for component in self.options.component:
                    options = {'sqlmode': sqlmode, 'component': component}
                    result.append (options)
        elif self.options.component:
            for component in self.options.component:
                result.append ({'component': component})
        elif self.options.sqlmode:
            for sqlmode in self.options.sqlmode:
                result.append ({'sqlmode': sqlmode})
        self.connectPermutations = result


    def runAll (self, patterns = None):
        filter = self.createFilter (patterns)
        for testcase in TestIterator (self.testCases):
            if filter.allows (testcase):
                for options in self.connectPermutations:
                    self.runOne (testcase, options)

    def runOne (self, testcase, options):
        session = self.createSession ()
        self.testsRun = self.testsRun + 1
        if options.values ():
            displayname = "%s %s" % (testcase, options.values ())
        else:
            displayname = testcase
        if self.options.verbose:
            log (displayname)
        try:
            testcase.run (session)
            self.testsOK = self.testsOK + 1
        except TestFailure, err:
            log (err, 'in', displayname)
        except:
            clazz, value, tb = sys.exc_info ()
            log ("EXCEPTION '%s' in %s" % (value, displayname))
            log.printException ()

    def createFilter (self, patterns):
        if patterns:
            filter = FilterByName (patterns)
        else:
            filter = AllowEverything ()
        return self.StdFilter (self.options, filter)

    class StdFilter:
        def __init__ (self, options, filter):
            self.options = options
            self.filter = filter

        def allows (self, testcase):
            if self.filter and not self.filter.allows (testcase):
                return None
            if self.isToDo (testcase) and not self.options.complete:
                return None
            return 1

        def isToDo (self, testcase):
            if hasattr (testcase, 'todo'):
                return 1
            return None


    def getDisplayName (self, method):
        classname = method.im_class.__name__
        name = method.__name__
        if name == 'run':
            result = classname
        else:
            result = classname + '.' + name
        return result

    def report (self):
        log ("%d / %d Tests OK" % (self.testsOK, self.testsRun))
        log ("Elapsed %02d:%02d:%02d.%02d" % self.timeDiff ())

    def timeDiff (self, startTime = None, endTime = None):
        if startTime is None:
            startTime = self.timeStarted
        if endTime is None:
            endTime = time.time ()
        timediff = endTime - startTime
        seconds = int (timediff)
        cents = int ((timediff - seconds) * 100)
        if seconds >= 60:
            seconds = seconds % 60
            minutes = seconds / 60
        else:
            minutes = 0
        if minutes >= 60:
            minutes = minutes % 60
            hours = minutes / 60
        else:
            hours = 0
        return hours, minutes, seconds, cents

    def hasErrors (self):
        result = self.testsOK != self.testsRun
        return result

class AllowEverything:
    def allows (self, testcase):
        return 1

class FilterByName:
    def __init__ (self, allowedPatterns):
        self.patterns = map (re.compile, allowedPatterns)

    def allows (self, testcase):
        name = str (testcase)
        for rex in self.patterns:
            if rex.search (name):
                return 1
        return None

class TestIterator:
    def __init__ (self, initialList = []):
        self.stack = initialList [:]
        self.stack.reverse ()

    def __iter__ (self):
        return self

    def __getitem__ (self, index):
        return self.next ()

    def next (self):
        #
        # handle end condition
        #
        if len (self.stack) == 0:
            raise StopIteration, self
        #
        # get next element
        #
        element = self.stack.pop ()
        #
        # if element is class: instantiate and get iterator
        #
        if type (element) == types.ClassType:
            element = element ()
        #
        # if element has run method: return
        #
        if hasattr (element, 'run'):
            return element
        #
        # if element is iterable
        #
        try:
            iterator = iter (element)
        except TypeError:
            pass
        else:
            self.pushIterator (iterator)
            return self.next ()
        #
        # don't know how to handle this kind of element
        #
        raise 'unknown element kind', (element, type (element))


    def materializeIterator (self, iterator):
        result = []
        for element in iterator:
            result.append (element)
        return result

    def pushIterator (self, iterator):
        list = self.materializeIterator (iterator)
        list.reverse ()
        self.stack.extend (list)

class Logger:
    def __init__ (self, stream = sys.stdout):
        self.stream = stream
        self.rawWrite = stream.write

    def printit (self, *text):
        timestamp =  time.strftime ("%H:%M:%S ", time.localtime (time.time ()))
        self.rawWrite (timestamp + string.join (map (str, text)) + '\n')

    __call__ = printit

    def printException (self):
        traceback.print_exc (file = self.stream)

log = Logger ()

if not PYTHON_ITERATORS:
    class Iterator:
        def __init__ (self, input):
            self.input = input
            self.atend = None

        def __getitem__ (self, index):
            return self.input.next ()

    StopIteration = IndexError

    def iter (input):
        if hasattr (input, '__iter__'):
            iterator = input.__iter__ ()
            if hasattr (iterator, 'next'):
                iterator = Iterator (iterator)
            return iterator
        else:
            return input

def fatalError (msg):
    sys.stderr.write (msg + '\n')
    sys.exit (2)

def checkOptions (options):
    if not options.userInfo:
        fatalError ('option -u <username>,<password> must be specified')
    options.username, options.password = string.split (options.userInfo, ',')
    if not options.dbname:
        fatalError ('option -d <dbname> must be specified')
    options.connectTuple = (options.username, options.password,
        options.dbname, options.node)
    if options.allmodes:
        if not options.sqlmode:
            options.sqlmode = ['INTERNAL', 'ORACLE']
        if not options.component:
            options.component = ['DOM', 'ODBC', 'SDB', 'DBM']
    if options.log:
        global log
        log = Logger (open (options.log, 'a'))
    options.verbose = not options.quiet

def stdOptions ():
    return [
        # (optstring, varname, typechar, default, help)
        ('u', 'userInfo', ':', None, 'sql user <username>,<password>'),
        ('dbm', None, ':', None, 'dbm user <username>,<password>'),
        ('d', 'dbname', ':', None, 'database name'),
        ('n', 'node', ':', '', 'network name of server'),
        ('q', 'quiet', '', None, 'write only errors to log'),
        ('log', None, ':', None, 'logfile'),
        ('sqlmode', None, '::', None, 'sqlmode [ORACLE, INTERN]'),
        ('component', None, '::', None, 'client mode'),
        ('allmodes', None, '', None, 'test using all connect options'),
        ('complete', None, '', None, 'execute also disabled tests'),
        ]

