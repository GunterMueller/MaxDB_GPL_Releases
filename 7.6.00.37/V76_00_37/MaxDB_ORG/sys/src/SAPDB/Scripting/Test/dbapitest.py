#!/usr/bin/env python
#
#   @file   dbapitest.py
#
#   @author DanielD
#
#   @brief  test Python DB API interface to MaxDB
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

from sdbpyTestLib import *

from sdb import dbapi

class DbapiTestRunner (TestRunner):
    def createSession (self, options = {}):
        return apply (dbapi.connect, self.connectData, options)

class SqlCommon:
    def createTable (self, session, name, definition):
        cursor = session.cursor ()
        try:
            cursor.execute ('drop table ' + name)
        except dbapi.ProgrammingError:
            pass
        cmd = 'create table ' + name + ' ' + definition
        # print cmd
        cursor.execute (cmd)

class SimpleSelect (Test):
    """Select values of several data types from DUAL,
        check that values and structure of cursor match expextations
    """
    def run (self, session):
        cursor = session.cursor ()
        cursor.execute ("select 'Hello', 1, 1.0 from dual")
        (strvalue, intvalue, floatvalue) = cursor.fetchone ()
        self.assertEqual ('Hello', strvalue)
        self.assertEqual (1, intvalue)
        self.assertEqual (1.0, floatvalue)
        #self.assertEqual ((('EXPRESSION1', 'Char', 1, 5, 0, None, 'OUT'),
        #                   ('EXPRESSION2', 'Fixed', 3, 1, 0, None, 'OUT'),
        #                   ('EXPRESSION3', 'Fixed', 3, 1, 0, None, 'OUT')
        #    ), cursor.getDescription ())

class LargeResultSet (Test):
    """Select everything from table MESSAGES, which should
        span several communication segments.

        Count both by using next and by using the iteration protocol.
    """
    def run (self, session):
        cursor = session.cursor ()
        cursor.execute ("select count (*) from messages")
        (size,) = cursor.fetchone ()
        #
        # use next
        #
        cursor.execute ("select * from messages")
        count = 0
        while cursor.fetchone ():
            count = count + 1
        self.assertEqual (size, count)
        #
        # use iteration
        #
        #count = 0
        #cursor.execute ("select * from messages")
        #for row in cursor:
        #    count = count + 1
        #self.assertEqual (size, count)

class Inserts (Test, SqlCommon):
    """Insert several records into the database.

        This is a limited test of prepared statements.
    """
    def run (self, session):
        cursor = session.cursor ()
        self.createTable (session, "inserttest", """(
            intval integer,
            strval char (20),
            PRIMARY KEY (intval))""")
        # insert = session.prepare ('insert into inserttest values (?, ?)')
        #
        # check that parameter description is as expected
        #
        #self.assertEqual ((('', 'Integer', 4, 10, 0, None, 'IN'),
        #                   ('', 'Char',    1, 20, 0,    1, 'IN'),
        #    ), insert.getDescription ())
        rowCount = 300
        #
        # insert values
        #
        for i in xrange (rowCount):
            tuple = (i, '%0d5 xxx' % i)
            cursor.execute ('insert into inserttest values (?, ?)', tuple)
            if i % 200 == 0:
                session.commit ()
        session.commit ()
        #
        # fetch and compare values
        #
        cursor.execute ("""select intval, strval from inserttest
            order by intval""")
        for i in xrange (rowCount):
            tuple = (i, '%0d5 xxx' % i)
            self.assertEqual (tuple, cursor.fetchone ())
        self.assertEqual (None, cursor.fetchone (), 'cursor has more values than expected')

class Longs (TestGroup, SqlCommon):
    testMethods = ['simpleWriteAndRead', 'writeVariations', 'readVariations',
        'multipleLongsPerRow', 'mixLongsWithOtherSQL']

    class SimpleLongData:
        """An iterator to create key/LONG pairs with the LONGs increasing in size

            The LONG element can be a string, a stream or an integer representing the length
        """
        def __init__ (self, elements, longformat):
            self.elements = elements
            self.index = 0
            self.longSize = 1
            self.longformat = longformat

        def __iter__ (self):
            return self

        def next (self):
            if self.index >= self.elements:
                raise StopIteration, self
            self.longsize = self.longSize * 10
            if self.longformat == 'string':
                longvalue = 'x' * self.longSize
            elif self.longformat == 'size':
                longvalue = self.longSize
            elif self.longformat == 'stream':
                longvalue = PseudoStream (self.longSize)
            else:
                raise 'unsupported long format', self.longformat
            result = self.index, longvalue
            self.index = self.index + 1
            return result

        def __getitem__ (self, index):
            return self.next ()


    def simpleWriteAndRead (self, session):
        """Insert strings, read whole LONGs into one string
        """
        cursor = session.cursor ()
        self.createTable (session, 'longtest', """(keycol integer, longcol LONG,
            PRIMARY KEY (keycol))""")
        #
        # insert
        #
        for expected in self.SimpleLongData (7, 'string'):
            cursor.execute ('insert into longtest values (?, ?)', expected)
        #
        # fetch
        #
        cursor.execute ('select keycol, longcol from longtest')
        for expected in self.SimpleLongData (7, 'string'):
            keyval, longval = cursor.fetchone ()
            if longval != None:
                longval = longval.read ()
            self.assertEqual (expected, (keyval, longval))

    def writeVariations (self, session):
        """Insert LONGs using streams
        """
        cursor = session.cursor ()
        self.createTable (session, 'longtest', """(keycol integer, longcol LONG,
            PRIMARY KEY (keycol))""")
        #
        # insert
        #
        for keyval, longstream in self.SimpleLongData (7, 'stream'):
            cursor.execute ('insert into longtest values (?, ?)', (keyval, longstream.read))
        #
        # fetch
        #
        cursor.execute ('select keycol, longcol from longtest')
        for expected in self.SimpleLongData (7, 'string'):
            keyval, longval = cursor.fetchone ()
            if longval != None:
                longval = longval.read ()
            self.assertEqual (expected, (keyval, longval))

    def readVariations (self, session):
        """Read LONGs in chunks
        """
        cursor = session.cursor ()
        self.createTable (session, 'longtest', """(keycol integer, longcol LONG,
            PRIMARY KEY (keycol))""")
        #
        # insert
        #
        for row in self.SimpleLongData (7, 'string'):
            cursor.execute ('insert into longtest values (?, ?)', row)
        #
        # fetch
        #
        cursor.execute ('select keycol, longcol from longtest')
        for expected in self.SimpleLongData (7, 'string'):
            keyval, longval = cursor.fetchone ()
            if longval != None:
                longval = longval.read ()
            else:
                longval = ''
                chunk = longval.read (512)
                while chunk:
                    longval = longval + chunk
                    chunk = longval.read (512)
            self.assertEqual (expected, (keyval, longval))

    if PYTHON_SUPPORTS_FUNCTION_PROPERTIES:
        def multipleLongsPerRow (self, session):
            """Use a table where multiple LONG columns are mixed with ordinary data columns
            """
            raise TestFailure ('not implemented')

        multipleLongsPerRow.todo = True

        def mixLongsWithOtherSQL (self, session):
            """Read LONG value in mixed other and execute othe SQL commands between

                There was a bug where executing other SQL would overwrite the
                communication segment.
            """
            raise TestFailure ('not implemented')

        mixLongsWithOtherSQL.todo = True

class PseudoStream:
    def __init__ (self, size):
        self.size = size
        self.charsRead = 0

    def read (self, count):
        chunksize = min (count, self.size - self.charsRead)
        # print '***', 'requested', count, 'deliviering', chunksize, 'pos/size', self.charsRead, '/', self.size
        self.charsRead = self.charsRead + chunksize
        return 'x' * chunksize

class Exceptions (TestGroup, SqlCommon):
    testMethods = ['syntaxError', 'parameterError']
    def syntaxError (self, session):
        cursor = session.cursor ()
        try:
            cursor.execute ('select * from_ messages')
        except dbapi.ProgrammingError, err:
            expected = (-5015, 10, '42000')
            actual = (err.errorCode, err.errorPos, err.sqlState)
            self.assertEqual (expected, actual)

    def parameterError (self, session):
        cursor = session.cursor ()
        self.createTable (session, 'notnulltest', '(a integer key, b integer not null)')
        try:
            cursor.execute ('insert into notnulltest values (?, ?)', [1, None])
            raise TestFailure ('ProgrammingError not raised')
        except dbapi.ProgrammingError, err:
            expected = (-5005, 8, 'I5005')
            actual = (err.errorCode, err.errorPos, err.sqlState)
            self.assertEqual (expected, actual)

class Types (Test):
    """Test input and output of various Python types, especially border cases
    """
    todo = 1
    def run (self, session):
        raise TestFailure ('not implemented: ' + str (self))

class ReferenceCounting (TestGroup):
    """Errors in reference counting is a common error in C extensions.
        - missing increment: crash because object gets deleted too soon
        - missing decrement: memory leak because object never gets deleted
    """
    testMethods = ['fetchValues', 'parameters', 'exceptions']

    if PYTHON_SUPPORTS_FUNCTION_PROPERTIES:
        def fetchValues (self, session):
            """Test that result values from cursors have the right reference cout

                Special cases: None, small integers, True and False, row
            """
            raise TestFailure ('not implemented')

        fetchValues.todo = True

        def parameters (self, session):
            """Test that refcounts of parameter values don't change after
                a PreparedStatement has been executed
            """
            raise TestFailure ('not implemented')

        parameters.todo = True

        def exceptions (self, session):
            """Test that exception objects and their attributes have
                the proper reference count
            """
            raise TestFailure ('not implemented')

        exceptions.todo = True

tests = [
    SimpleSelect,
    LargeResultSet,
    Inserts,
    Longs,
    Exceptions,
    Types,
    ReferenceCounting,
]

def dbapitest (options, args):
    runner = DbapiTestRunner (options, tests)
    runner.runAll (args)
    if not options.quiet:
        runner.report ()
    if runner.hasErrors ():
        return None
    else:
        return 1

def main (options, args):
    checkOptions (options)
    if not dbapitest (options, args):
        sys.exit (3)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('u', 'userInfo', ':', None, 'sql user <username>,<password>'),
        ('dbm', None, ':', None, 'dbm user <username>,<password>'),
        ('d', 'dbname', ':', None, 'database name'),
        ('n', 'node', ':', '', 'network name of server'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('q', 'quiet', '', None, 'write only errors to log'),
        ('log', None, ':', None, 'logfile'),
        ('sqlmode', None, '::', None, 'sqlmode [ORACLE, INTERN]'),
        ('component', None, '::', None, 'client mode'),
        ('allmodes', None, '', None, 'test using all connect options'),
        ('complete', None, '', None, 'execute also disabled tests'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

