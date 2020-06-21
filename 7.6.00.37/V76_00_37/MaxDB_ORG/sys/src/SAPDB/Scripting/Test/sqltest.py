#!/usr/bin/env python
#
#   @file   sqltest.py
#
#   @author DanielD
#
#   @brief  test Python interface to MaxDB
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

from sdb import sql

#
# databse variables
#
charType = 'Char'

#
# other variables
#
verbose = None

#
#
#
class SqlTestRunner (TestRunner):
    def createSession (self, options = {}):
        optionStrings = map (lambda t: "%s=%s" % t, options.items ())
        optionString = string.join (optionStrings, '&')
        return apply (sql.connect, self.connectData + (optionString,))

class SqlCommon:
    def createTable (self, session, name, definition):
        try:
            session.sql ('drop table ' + name)
        except sql.SQLError, err:
            pass
        cmd = 'create table ' + name + ' ' + definition
        # print cmd
        session.sql (cmd)

    def selectAndFetch (self, session, sql):
        result = []
        for row in session.sql (sql):
            if len (row) == 1:
                result.append (row [0])
            else:
                result.append (row)
        if len (result) == 1:
            return result [0]
        else:
            return result

    def expectSQLError (self, expectedError, callable, args):
        try:
            apply (callable, args)
            raise TestFailure ('expecting SQLError %s' % repr (expectedError))
        except sql.SQLError, err:
            actual = (err.errorCode, err.errorPos, err.sqlState)
            self.assertEqual (actual, expectedError)

class SimpleSelect (Test):
    """Select values of several data types from DUAL,
        check that values and structure of cursor match expextations
    """
    def run (self, session):
        cursor = session.sql ("select 'Hello', 1, 1.0 from dual")
        (strvalue, intvalue, floatvalue) = cursor.next ()
        self.assertEqual ('Hello', strvalue)
        self.assertEqual (1, intvalue)
        self.assertEqual (1.0, floatvalue)
        self.assertEqual ((('EXPRESSION1', charType, 1, 5, 0, None, 'OUT'),
                           ('EXPRESSION2', 'Fixed', 3, 1, 0, None, 'OUT'),
                           ('EXPRESSION3', 'Fixed', 3, 1, 0, None, 'OUT')
            ), cursor.getDescription ())

class LargeResultSet (Test):
    """Select everything from table MESSAGES, which should
        span several communication segments.

        Count both by using next and by using the iteration protocol.
    """
    def run (self, session):
        (size,) = session.sql ("select count (*) from messages").next ()
        #
        # use next
        #
        cursor = session.sql ("select * from messages")
        count = 0
        while cursor.next ():
            count = count + 1
        self.assertEqual (size, count)
        #
        # use iteration
        #
        count = 0
        cursor = session.sql ("select * from messages")
        for row in cursor:
            count = count + 1
        self.assertEqual (size, count)

class Inserts (Test, SqlCommon):
    """Insert several records into the database.

        This is a limited test of prepared statements.
    """
    def run (self, session):
        self.createTable (session, "inserttest", """(
            intval integer,
            strval char (20) ASCII,
            PRIMARY KEY (intval))""")
        insert = session.prepare ('insert into inserttest values (?, ?)')
        #
        # check that parameter description is as expected
        #
        self.assertEqual ((('', 'Integer', 4, 10, 0, None, 'IN'),
                           ('', 'Char',    1, 20, 0,    1, 'IN'),
            ), insert.getDescription ())
        rowCount = 300
        #
        # insert values
        #
        for i in xrange (rowCount):
            tuple = (i, '%0d5 xxx' % i)
            insert.execute (tuple)
            if i % 200 == 0:
                session.commit ()
        session.commit ()
        #
        # fetch and compare values
        #
        cursor = session.sql ("""select intval, strval from inserttest
            order by intval""")
        for i in xrange (rowCount):
            tuple = (i, '%0d5 xxx' % i)
            self.assertEqual (tuple, cursor.next ())
        self.assertEqual (None, cursor.next (), 'cursor has more values than expected')

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
        self.createTable (session, 'longtest', """(keycol integer, longcol LONG,
            PRIMARY KEY (keycol))""")
        #
        # insert
        #
        insert = session.prepare ('insert into longtest values (?, ?)').execute
        for expected in self.SimpleLongData (7, 'string'):
            insert (expected)
        #
        # fetch
        #
        cursor = session.sql ('select keycol, longcol from longtest')
        for expected in self.SimpleLongData (7, 'string'):
            keyval, longval = cursor.next ()
            if longval != None:
                longval = longval.read ()
            self.assertEqual (expected, (keyval, longval))

    def writeVariations (self, session):
        """Insert LONGs using streams
        """
        self.createTable (session, 'longtest', """(keycol integer, longcol LONG,
            PRIMARY KEY (keycol))""")
        #
        # insert
        #
        insert = session.prepare ('insert into longtest values (?, ?)').execute
        for keyval, longstream in self.SimpleLongData (7, 'stream'):
            insert ((keyval, longstream.read))
        #
        # fetch
        #
        cursor = session.sql ('select keycol, longcol from longtest')
        for expected in self.SimpleLongData (7, 'string'):
            keyval, longval = cursor.next ()
            if longval != None:
                longval = longval.read ()
            self.assertEqual (expected, (keyval, longval))

    def readVariations (self, session):
        """Read LONGs in chunks
        """
        self.createTable (session, 'longtest', """(keycol integer, longcol LONG,
            PRIMARY KEY (keycol))""")
        #
        # insert
        #
        insert = session.prepare ('insert into longtest values (?, ?)').execute
        for row in self.SimpleLongData (7, 'string'):
            insert (row)
        #
        # fetch
        #
        cursor = session.sql ('select keycol, longcol from longtest')
        for expected in self.SimpleLongData (7, 'string'):
            keyval, longval = cursor.next ()
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
        self.expectSQLError ((-5015, 10, '42000'),
            session.sql, ('select * from_ messages',))

    def parameterError (self, session):
        self.createTable (session, 'notnulltest', '(a integer key, b integer not null)')
        insert = session.prepare ('insert into notnulltest values (?, ?)')
        self.expectSQLError ((-5005, 8, 'I5005'),
            insert.execute, ([1, None],))

class Types (TestGroup, SqlCommon):
    """Test input and output of various Python types, especially border cases
    """
    testMethods = ['failedConversion']

    def failedConversion (self, session):
        self.createTable (session, 'dataconversion', '(a integer key)')
        insert = session.prepare ('insert into dataconversion values (?)').execute
        self.expectSQLError ((-817, 0, 'S0817'), insert, (['not a number'],))

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

class Navigation (TestGroup, SqlCommon):
    """Check that moving around in a result set always hits the right record.
    """
    testMethods = ['absolute', 'currentAtBorder']


    tableRows = None

    def createNavigationTable (self, session, rowcount = 200):
        if self.tableRows == rowcount:
            return
        self.createTable (session, 'navigation', "(poscol integer key, fillcol char (1000))")
        insert = session.prepare ('insert into navigation (poscol) values (?)').execute
        for i in xrange (rowcount):
            insert ([i + 1])
        session.commit ()
        self.tablerows = rowcount

    def createCursor (self, session):
        return session.sql ('select poscol, fillcol from navigation order by poscol')

    def createTester (self, session, rowcount = 200):
        self.createNavigationTable (session, rowcount)
        cursor = self.createCursor (session)
        tester = self.NavigationTester (cursor, rowcount, self)
        return tester

    class NavigationTester:
        def __init__ (self, cursor, size, tester):
            self.cursor = cursor
            self.pos = 0
            self.size = size
            self.tester = tester

        def checkRow (self, funcname, parameters, expected):
            if expected < 1 or self.size < expected:
                expected = None
            function = getattr (self.cursor, funcname)
            row = apply (function, parameters)
            if row is None:
                actual = None
            else:
                actual, filler = row
            #
            # tracing output
            #
            if verbose:
                paramStrings = map (repr, parameters)
                paramString = string.join (paramStrings, ', ')
                print "%s (%s) => %s [expected %s]" % (
                    funcname, paramString, actual, expected)
            #
            # comparison
            #
            self.tester.assertEqual (expected, actual)

        def absolute (self, value):
            if value < 0:
                expected = self.size + value + 1
            else:
                expected = value
            self.checkRow ('absolute', [value], expected)
            if 1 <= value or value <= self.size:
                self.pos = expected

        def current (self):
            self.checkRow ('current', [], self.pos)

        def next (self):
            self.checkRow ('next', [], self.pos + 1)
            if self.pos <= self.size:
                self.pos = self.pos + 1

        def previous (self):
            self.checkRow ('previous', [], self.pos - 1)
            if self.pos >= 1:
                self.pos = self.pos - 1

    def absolute (self, session):
        """Check access by absolute row
        """
        tester = self.createTester (session)
        for i in [10, -10, 200, 199, 1, 10, 0, 201, -201, 10, -1, 1]:
            tester.absolute (i)

    def currentAtBorder (self, session):
        tester = self.createTester (session)
        tester.absolute (-3)
        for i in range (3):
            # last iteration goes beyond end of result set
            tester.next ()
            tester.current ()
        #
        # check that stepping back into the result set works
        #
        tester.previous ()
        tester.current ()
        #
        # the same for start of cursor
        #
        tester.absolute (3)
        for i in range (3):
            # last iteration goes beyond start of result set
            tester.previous ()
            tester.current ()
        #
        # check that stepping forward into the result set works
        #
        tester.next ()
        tester.current ()

tests = [
    SimpleSelect,
    LargeResultSet,
    Inserts,
    Longs,
    Exceptions,
    Types,
    ReferenceCounting,
    Navigation,
]

def checkDbVars (runner):
    session = runner.createSession ()
    result = SqlCommon ().selectAndFetch (session,
        "select VALUE from dbparameters where description = '_UNICODE'")
    global charType
    if result == 'YES':
        charType = 'Unicode'

def sqltest (options, args):
    runner = SqlTestRunner (options, tests)
    checkDbVars (runner)
    runner.runAll (args)
    if not options.quiet:
        runner.report ()
    if runner.hasErrors ():
        return None
    else:
        return not None

def myCheckOptions (options):
    checkOptions (options)
    if options.verbose:
        global verbose
        verbose = options.verbose

def main (options, args):
    myCheckOptions (options)
    ok = sqltest (options, args)
    if not ok:
        sys.exit (3)

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, stdOptions ())

