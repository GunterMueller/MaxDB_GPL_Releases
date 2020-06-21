#!/usr/bin/env x_python
# ireport.py
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

# new features 0.2:
#   command abbrevations
#   command table
#   command edit
#   option -pager
#   command xmlexport
#   command grid, compressed
# new features 0.3:
#   readline history
# 0.3.1 by Elrond [elrond@samba-tng.org]
#   bugfix: read_history for Python 1.5.2
#   new feature: commands commit, rollback
#   new feature: display of LONG columns
# 0.3.2 by Elrond [elrond@samba-tng.org]
#   new feature: command table displays full catalog info
# 0.3.3
#   bugfix: xmlexport produced XML that was not well formed
#   bugfix: command table accepts names in the same format as SELECT
#   new feature: command format for printf like formatting
#   new feature: option -batch

import sys
import string
from string import find, index, join, ljust, replace, split, strip, upper
import re
import os
from cmd import Cmd
try:
    import readline
except:
    readline = None
import UserList

import sdb.sql

_version = (0, 3, 3)
_versionString = join (map (str, _version), '.')

_credits = [
    ('SAP Labs Berlin', ('Maintainer', )),
    ('Elrond', ('bug fixes', 'commit + rollback', 'LONG display', 'table info')),
]

#
# shell utility functions
#
def _buildShortcuts (clazz):
    locals = clazz.__dict__
    commands = []
    for name, member in locals.items ():
        if (name [:3] == 'do_') and (name != 'do_EOF'):
            commands.append ((name [3:], member))
    name2len = []
    shortcut2name = {}
    for command, member in commands:
        shortcut = _findShortest (command, commands)
        name2len.append ((command, len (shortcut)))
        if len (shortcut) < len (command):
            for i in xrange (len (shortcut), len (command)):
                locals ['do_' + command [:i]] = member
    name2len.sort ()
    clazz.name2len = name2len

def _findShortest(name, commands):
    for i in xrange (len (name)):
        shortcut = name [:i]
        matchCount = 0
        for command, member in commands:
            if command [:i] == shortcut:
                matchCount = matchCount + 1
                if matchCount == 2:
                    break
        if matchCount == 1:
            return shortcut
    return name

class ShowTableInfo:
    def __init__(self, session, out = None):
        self.session = session
        if out is None:
            out = sys.stdout
        self.out = out
        self.writtenchars = 0
        self.maybreak = 0
        self.prefix = ["  "]

    def write(self, st):
        """A quick hack to do some linebreaking... not realy great"""
        if not st:
            return
        if self.writtenchars == 0:
            s=join(self.prefix, '')
            self.out.write(s)
            self.writtenchars = len(s)
        if self.writtenchars + len(st) > 60:
            i = string.rfind(st, " ", 0, -1) + 1
            if i and i < len(st):
                self.write(st[:i])
                self.write(st[i:])
                return
            # Okay, we now have a unbreakable string, break now if we can
            if self.maybreak:
                self.out.write("\n")
                self.maybreak = 0
                self.writtenchars = 0
        if self.writtenchars == 0:
            s=join(self.prefix, '')
            self.out.write(s)
            self.writtenchars = len(s)
        self.out.write(st)
        self.writtenchars = self.writtenchars + len(st)
        if st[-1] in [" ", "\t"]:
            self.maybreak = 1
        else:
            self.maybreak = 0
        if st[-1] == "\n":
            self.writtenchars = 0

    def write_addpref(self, st, pref):
        """This one writes one string, but adds the prefix for
        any broken lines, it will keep the prefix, so you have
        to pop it."""
        if not st:
            self.prefix.append(pref)
            return
        self.write(st[:1])
        self.prefix.append(pref)
        self.write(st[1:])

    def write_withpref(self, st, pref):
        if not st: return
        self.write_addpref(st, pref)
        self.prefix.pop(-1)

    def show_resultset(self, resultset):
        """Show a complete result set in python format"""
        if not resultset: return
        for row in resultset:
            self.write_withpref(repr(row)+"\n", " ")

    def show_title(self, st):
        self.prefix.pop(-1)
        self.write(st+"\n")
        self.prefix.append("  ")

    def show_cols(self, resultset):
        if not resultset: return
        self.show_title("Columns:")
        for colname, mode, datatype, encoding, length, dec, default in resultset:
            self.write_addpref(colname + " " + datatype, "  ")
            if datatype in ["CHAR", "VARCHAR", "FIXED", "FLOAT"]:
                self.write("("+str(length))
                if dec:
                    self.write(", "+str(dec))
                self.write(")")
            if encoding:
                self.write(" " + encoding)
            if mode == 'MAN':
                self.write(" not null")
            if default:
                self.write(" " + "default " + default)
            self.write("\n")
            self.prefix.pop(-1)

    def show_index(self, resultset, tablename):
        if not resultset: return
        self.show_title("Indexes:")
        oldindexname = None
        firstcol = 1
        for indexname, type, colno, colname, sort in resultset:
            if type != '':
                type = type + " "
            if oldindexname != indexname:
                if oldindexname:
                    self.prefix.pop(-1)
                    self.write(")\n")
                self.write_addpref(type + "index " + indexname +
                    " on " + tablename + " (", "  ")
                oldindexname = indexname
                firstcol = 1
            if not firstcol:
                self.write(", ")
            self.write(colname + " " + sort)
            firstcol = 0
        if oldindexname:
            self.prefix.pop(-1)
            self.write(")\n")

    def show_primarykey(self, resultset):
        if not resultset: return
        self.show_title("Primary key:")
        firstkey = 1
        self.write("(")
        self.prefix.append(" ")
        for keyname, in resultset:
            if not firstkey:
                self.write(", ")
            self.write(keyname)
            firstkey = 0
        self.prefix.pop(-1)
        self.write(")\n")

    def show_foreignkeys(self, resultset):
        if not resultset: return
        self.show_title("Foreign keys:")
        for fkeyname, columnname, reftablename, refcolumnname, rule in resultset:
            self.write_withpref("foreign key " + fkeyname +
                " (" + columnname + ") " +
                "references " + reftablename + "(" + refcolumnname + ") " +
                "on " + rule + "\n", "  ")

    def show_constraints(self, resultset):
        if not resultset: return
        self.show_title("Constraints:")
        for consname, defin in resultset:
            self.write_addpref("constraint " + consname + " check ", "  ")
            l = defin.read(10000)
            while l:
                self.write(l)
                l = defin.read(10000)
            self.prefix.pop(-1)
            self.write("\n")

    def show_synonyms(self, resultset):
        if not resultset: return
        self.show_title("Synonyms for this table:")
        for owner, synname, public in resultset:
            self.write("%s.%s" % (owner, synname))
            if public != "NO":
                self.write(" PUBLIC")
            self.write("\n")

    def show_views(self, resultset):
        if not resultset: return
        self.show_title("Views, which reference this table:")
        oldviewname = None
        for owner, viewname, defin, viewcol, tab, tabcol in resultset:
            if viewname != oldviewname:
                if oldviewname: self.prefix.pop(-1)
                self.write(owner+"."+viewname+":\n")
                self.prefix.append("  ")
                self.write_addpref(defin.read(1), "  ")
                l = defin.read(10000)
                while l:
                    self.write(l)
                    l = defin.read(10000)
                self.prefix.pop(-1)
                self.write("\n")
                oldviewname = viewname
            self.write("%s -> %s.%s\n" % (viewcol, tab, tabcol))
        if oldviewname: self.prefix.pop(-1)

    def sql_call_with_resultset(self, sqlcommand, fn, extrainfo = None):
        """Execute some SQL command and call fn with the result,
        if there is a result"""
        try:
            result = self.session.sql(sqlcommand)
        except sdb.sql.SQLError, err:
            if err.errorCode == 100:
                # Row not found, empty resultset
                return 1
            print err
            return 0
        if extrainfo:
            return fn(result, extrainfo)
        else:
            return fn(result)

    def tableinfo(self, owner, tablename):
        """Shows some useful information about a table (columns, ...)
        """
        # Columns
        self.sql_call_with_resultset(
            """select columnname, mode, datatype, codetype, len, dec, "DEFAULT"
            from domain.columns
            where tablename = '%s'
            and   owner = '%s'
            """ % (tablename, owner),
            self.show_cols)
        # Primary key
        self.sql_call_with_resultset(
            """select columnname
            from domain.columns
            where tablename = '%s'
            and   owner = '%s'
            and keypos > 0
            order by keypos
            """ % (tablename, owner),
            self.show_primarykey)
        # Foreign keys
        self.sql_call_with_resultset(
            """select FKEYNAME, COLUMNNAME, REFTABLENAME, REFCOLUMNNAME, RULE
            from domain.foreignkeycolumns
            where tablename = '%s'
            and   owner = '%s'
            """ % (tablename, owner),
            self.show_foreignkeys)
        # Indexes
        self.sql_call_with_resultset(
            """select indexname, type, columnno, columnname, sort
            from domain.indexcolumns
            where tablename = '%s'
            and   owner = '%s'
            order by indexname, columnno
            """ % (tablename, owner),
            self.show_index, tablename)
        # Synonyms
        self.sql_call_with_resultset(
            """select owner, synonymname, public
            from domain.synonyms
            where tablename = '%s'
            and   owner = '%s'
            """ % (tablename, owner),
            self.show_synonyms)
        # Constraints
        self.sql_call_with_resultset(
            """select constraintname, definition
            from domain.constraints
            where tablename = '%s'
            and   owner = '%s'
            """ % (tablename, owner),
            self.show_constraints)
        # Views
        self.sql_call_with_resultset(
            """select viewcolumns.owner, viewcolumns.viewname,
            viewdefs.definition,
            viewcolumns.viewcolumnname, viewcolumns.tablename, viewcolumns.columnname
            from viewcolumns, viewdefs
            where viewcolumns.viewname = viewdefs.viewname
            and tablename = '%s'
            and viewcolumns.owner = '%s'
            """ % (tablename, owner),
            self.show_views)

class StringResultSet(UserList.UserList):
    def __init__(self, list = None, header = 'STRING'):
        self.currentind = 0
        self.laststr = ""
        UserList.UserList.__init__(self, list)
        self.header = header

    def write(self, st):
        i = string.find(st, "\n")
        if i >= 0:
            laststr = self.laststr + st[:i]
            self.append(laststr)
            self.laststr = ""
            self.write(st[i+1:])
            return
        self.laststr = self.laststr + st

    def ret_cur(self):
        if self.currentind < 0 or self.currentind >= len(self):
            self.currentind = -1
            return None
        # Return the string as a tuple, like a normal row.
        return (self[self.currentind], )

    current = ret_cur

    def absolute(self, ind):
        self.currentind = ind
        return self.ret_cur()
    def next(self):
        self.currentind = self.currentind + 1
        return self.ret_cur()
    def prev(self):
        self.currentind = self.currentind - 1
        return self.ret_cur()
    def relative(self, ofs):
        self.currentind = self.currentind + ofs
        return self.ret_cur()
    def first(self):
        return self.absolute(0)
    def columnCount(self):
        return 1
    def columnInfo(self, colnum):
        if colnum != 0:
            raise IndexError
        return (self.header, "Char", 70, 0)

class InteractiveReport (Cmd):
    prompt = '==> '
    intro = """MaxDB Interactive Report %s
Type 'help' for help.
An empty line repeats the last command""" % _versionString

    cursor = None
    cards = None
    lastSQL = ''
    sqlFile = ''
    reportKind = 'compressed'

    def __init__ (self, session, options):
        Cmd.__init__ (self)
        self.session = session
        for name, value in options.__dict__.items ():
            setattr (self, name, value)
        if self.cards:
            self.do_card ()
        else:
            self.do_report ()

    def do_help (self, cmdName):
        """show command help

        without argument: lists all commands
        with argument: shows help for the specified command
        """
        if cmdName:
            try:
                method = getattr (self, 'do_' + cmdName)
                print "Help for command '%s': %s" % (cmdName, method.__doc__)
            except (AttributeError, KeyError):
                print "No help for '" + cmdName + "'"
        else:
            print '(star indicates abbrevation)'
            for name, shortlen in self.name2len:
                self.methodHelp (name, shortlen)

    def methodHelp (self, name, shortlen):
        member = getattr (self, 'do_' + name)
        doc = member.__doc__
        if doc == None:
            doc = ''
        try:
            summary = doc [:index (doc, '\n')]
            more = ' ...'
        except ValueError:
            summary = doc
            more = ''
        if shortlen < len (name):
            displayname = name [:shortlen] + '*' + name [shortlen:]
        else:
            displayname = name
        print '%-15s %s%s' % (displayname, summary, more)

    def do_quit (self, strarg = None):
        """quit program"""
        return 1

    def do_EOF (self, strarg = None):
        print ""
        return self.do_quit()

    def do_option (self, strarg):
        """change display options
        """
        blank = find (strarg, ' ')
        if blank == -1:
            blank = len (strarg)
        name = strarg [:blank]
        value = strip (strarg[blank:])
        if name in ['lines', 'scroll']:
            try:
                value = int (value)
            except ValueError:
                print 'option', name, 'must be numeric'
        elif name in []:
            pass
        else:
            print 'unknown option', name
        setattr (self, name, value)

    def do_sql (self, strarg, show = 1):
        """execute a SQL command

        Displays the result if possible.
        """
        try:
            result = self.session.sql (strarg)
            self.lastSQL = strarg
            if isinstance (result, sdb.sql.SapDB_ResultSet):
                self.setCursor (result)
                if show:
                    self.do_next ()
            else:
                if result:
                    print repr(result)
        except sdb.sql.SQLError, err:
            print err
            if err.errorPos > 0:
                print 'at', err.errorPos

    def do_commit (self, strarg):
        """commits the current transaction"""
        self.session.commit()

    def do_rollback (self, strarg):
        """aborts the current transaction"""
        self.session.rollback()

    def do_again (self, strarg):
        """reexecute last SQL command"""
        self.do_sql (self.lastSQL)

    def do_edit (self, strarg):
        """edit and execute SQL"""
        if not self.sqlFile:
            import tempfile
            self.sqlFile = tempfile.mktemp ('.sql')
            stream = open (self.sqlFile, 'wt')
            stream.close ()
        os.system (self.editor + ' ' + self.sqlFile)
        sql = open (self.sqlFile, 'rt').read ()
        self.do_sql (sql)

    def default (self, strarg):
        if len (strarg) > 6:
            self.do_sql (strarg)
        else:
            Cmd.default (self, strarg)

    def setCursor (self, cursor):
        self.cursor = cursor
        self.cardFormat = CardFormat (cursor)
        self.compressedFormat = CompressedFormat (cursor)
        self.gridFormat = GridFormat (cursor)
        if self.reportKind == 'compressed':
            self.rowFormat = self.compressedFormat
        else:
            self.rowFormat = self.gridFormat

    def do_next (self, strarg = None):
        """display next page
        """
        if self.hasCursor ():
            self.displayPage (self.cursor.next ())

    def do_prev (self, strarg = None):
        """display previous page
        """
        if self.hasCursor ():
            row = self.cursor.relative ((- self.scroll * 2) + 1)
            self.displayPage (row)

    def do_top (self, strarg = None):
        """display first page
        """
        if self.hasCursor ():
            row = self.cursor.first ()
            self.displayPage (row)

    def do_bottom (self, strarg = None):
        """display last page
        """
        if self.hasCursor ():
            row = self.cursor.absolute (- self.scroll)
            self.displayPage (row)

    def do_card (self, strarg = None):
        """displays one row per screen
        """
        self.scroll = 1
        self.displayPage = self.displayCard
        if self.cursor:
            self.onecmd ('next')

    def setupReport (self, reportKind = None):
        if reportKind != None:
            self.reportKind = reportKind
        self.scroll = self.lines
        if self.pager:
            self.displayPage = self.displayPagedExternal
        else:
            self.displayPage = self.displayRows
        if self.cursor:
            if self.reportKind == 'compressed':
                self.rowFormat = self.compressedFormat
            elif self.reportKind == 'printf':
                self.rowFormat = self.printfFormat
            else:
                self.rowFormat = self.gridFormat

    def do_report (self, strarg = None):
        """displays one row per line
        """
        self.setupReport ()
        if self.cursor:
            self.onecmd ('next')

    def do_compressed (self, strarg = None):
        """set compressed row format

        |value1|value2|value3|...|value n|
        """
        self.reportKind = 'compressed'
        self.do_report ()

    def do_grid (self, strarg = None):
        """displays one row per line

        |value1   |value2    |......|value n|
        """
        self.reportKind = 'grid'
        self.do_report ()

    def do_format (self, strarg = None):
        """displays one row per line using prinf specification

        |value1   |value2    |......|value n|
        """
        try:
            if strarg:
                self.printfFormat = PrintfFormat (self.cursor, strarg)
            self.reportKind = 'printf'
            self.do_report ()
        except:
            kind, exc, traceback = sys.exc_info ()
            print '***' + str (exc)

    #def do_tableinfo(self, strarg = None):
    #    """Shows some useful information about a table (columns, ...)
    #    """
    #    if not strarg:
    #        print "*** specify tablename"
    #        return
    #    tableinfoshower = ShowTableInfo(self.session, sys.stdout)
    #    tableinfoshower.tableinfo(strarg)

    def do_table (self, strarg = None):
        """lists tables and table definitions

        table [ownerpattern.]tablepattern

        If there is only one table matching the pattern,
        the structure of this table is displayed.
        """
        #
        # build select based on parameter
        #
        select = 'select owner, tablename, type, tabletype from domain.tables where true '
        if not strarg:
            owner = ''
            tablepattern = ''
        else:
            list = split (strarg, '.')
            if len (list) == 1:
                owner = ''
                tablepattern = list [0]
            else:
                owner = list [0]
                tablepattern = list [1]
        if owner == '':
            select = select + ' and owner = usergroup'
        elif owner == '*':
            pass
        else:
            select = select + " and owner like '" + owner + "'"
        if tablepattern in ['', '*']:
            pass
        else:
            select = select + " and tablename like '" + tablepattern + "'"
        select = select + " and type <> 'RESULT'"
        select = select + ' order by owner, tablename'
        #
        # execute select
        #
        try:
            cursor = self.session.sql (select)
            row1 = cursor.next ()
            if not row1:
                cursor = self.session.sql ('show table ' + strarg)
                row1 = cursor.next ()
            if not row1:
                print '*** no matching tables ***'
                return
            row2 = cursor.next ()
            if row2:
                #
                # show table select as regular select
                #
                self.setCursor (cursor)
                self.displayPage (self.cursor.first ())
            else:
                #
                # print tablename and select columns
                #
                owner = row1 [0]
                tablename = row1 [1]
                str_res_set = StringResultSet(
                    header = 'Table: %s.%s' % (owner, tablename))
                tableinfoshower = ShowTableInfo(self.session, str_res_set)
                tableinfoshower.tableinfo(owner, tablename)
                self.setCursor (str_res_set)
                self.printfFormat = PrintfFormat (self.cursor, '%s')
                self.setupReport ('printf')
                self.displayPage (self.cursor.first ())
        except sdb.sql.SQLError, err:
            print err

    def do_xmlexport (self, strarg = None):
        '''exports result set to xml file

        xmlexport [filename]

        <ResultSet>
            <Description>
                <Column name="..." typ="..." length="..." scale="..."\>
            <Description>
            <Rows>
                <Value>...</Value>
                <Value isnull="yes"/>
            </Rows>
        </ResultSet>
        '''
        if not strarg:
            fname = 'resultset.xml'
        else:
            fname = strarg
        stream = open (fname, 'wt')
        cursor = self.cursor
        # write header
        stream.write ('<?xml version="1.0"?>\n')
        stream.write ('<ResultSet>\n')
        # write schema
        stream.write ('    <Description>\n')
        for i in xrange (cursor.columnCount ()):
            name, type, length, scale = cursor.columnInfo (i)
            if scale >= 0:
                stream.write ('        <Column name="%s" type="%s" length="%d" scale="%d"/>\n'
                    % (name, type, length, scale))
            else:
                stream.write ('        <Column name="%s" type="%s" length="%d"/>\n'
                    % (name, type, length))
        stream.write ('    </Description>\n')
        # write data
        stream.write ('    <Rows>\n')
        for row in self.cursor:
            stream.write ('        <Row>\n')
            for entry in row:
                if entry is None:
                    stream.write ('            <Value isnull="true"/>\n')
                else:
                    strval = str (entry)
                    strval = replace (strval, '&', '&amp;')
                    strval = replace (strval, '<', '&lt;')
                    stream.write ('            <Value>' + str (strval) + '</Value>\n')
            stream.write ('        </Row>\n')
        stream.write ('    </Rows>\n')
        # close tags
        stream.write ('</ResultSet>\n')
        stream.close ()

    def do_credits (self, strarg = None):
        """display contributors page"""
        for name, topics in _credits:
            print name + ':', join (topics, ', ')


    def hasCursor (self):
        if self.cursor:
            result = 1
        else:
            result = None
            print "no result set available"
        return result

    def linesOfText (self, textlen):
        lines = (textlen + self.columns) / self.columns
        return lines

    def displayRows (self, row):
        write = self.rowFormat.write
        linesRemaining = self.scroll
        for header in [self.rowFormat.header0, self.rowFormat.header1]:
            charsWritten = len (header)
            sys.stdout.write (header)
            linesRemaining = linesRemaining - self.linesOfText (charsWritten)
        charsWritten = self.writeRow (row, write)
        linesRemaining = linesRemaining - self.linesOfText (charsWritten)
        next = self.cursor.next
        while linesRemaining > 0:
            row = next ()
            if not row:
                return
            charsWritten = self.writeRow (row, write)
            linesRemaining = linesRemaining - self.linesOfText (charsWritten)

    def displayCard (self, row):
        self.writeRow (row, self.cardFormat.write)

    def writeRow (self, row, write, stream = sys.stdout):
        if not row:
            line = '<<<End of Cursor>>>'
            result = len (line)
            stream.write ('<<<End of Cursor>>>\n')
        else:
            result = write (row, stream)
        return result

    def displayPagedExternal (self, row):
        write = self.rowFormat.write
        pipe = os.popen (self.pager, 'wt')
        pipe.write (self.rowFormat.header0)
        pipe.write (self.rowFormat.header1)
        self.writeRow (row, write, pipe)
        next = self.cursor.next
        while 1:
            row = next ()
            if not row:
                break
            self.writeRow (row, write, pipe)
        pipe.close ()

    def displayAll (self, showHeader= 1, stream = sys.stdout):
        write = self.rowFormat.write
        if showHeader:
            stream.write (self.rowFormat.header0)
            stream.write (self.rowFormat.header1)
        next = self.cursor.next
        while 1:
            row = next ()
            if not row:
                break
            self.writeRow (row, write, stream)


_buildShortcuts (InteractiveReport)

#
# row formats
#
class Format:
    def getDescription (self, cursor):
        result = []
        for i in xrange (cursor.columnCount ()):
            result.append (list (cursor.columnInfo (i)))
        return result

    def write (self, row, stream = sys.stdout):
        list = []
        for format, value in map (None, self.formats, row):
            if isinstance(value, sdb.sql.SapDB_LongReader):
                value = value.read(10000)
            list.append (format.format (value))
        line = join (list, '')
        stream.write (line)
        return len (line)

class CardFormat (Format):
    def __init__ (self, cursor):
        formats = []
        description = self.getDescription (cursor)
        maxName = 0
        for name, kind, length, frac in description:
            maxName = max (maxName, len (name))
        for name, kind, length, frac in description:
            prefix = ljust (name, maxName)
            fmt = prefix + ': %s\n'
            nullfmt = prefix + ': ?\n'
            formats.append (ValueFormat (fmt, nullfmt))
        self.formats = formats

class CompressedFormat (Format):
    def __init__ (self, cursor):
        formats = []
        description = self.getDescription (cursor)
        header0 = ''
        header1 = ''
        for name, kind, length, frac in description:
            fmt = '|%s'
            nullfmt = '|?'
            formats.append (ValueFormat (fmt, nullfmt))
            header0 = header0 + '|' + name
            header1 = header1 + '+' + ('-' * len (name))
        fmt = formats [-1].stdFormat
        nullfmt = formats [-1].nullFormat
        formats [-1] = ValueFormat (fmt + '|\n', nullfmt + '|\n')
        self.formats = formats
        self.header0 = header0 + '|\n'
        self.header1 = header1 + '+\n'

class GridFormat (Format):
    def __init__ (self, cursor):
        formats = []
        description = self.getDescription (cursor)
        header0 = ''
        header1 = ''
        for name, kind, length, frac in description:
            displaysize = len(name)
            if kind in ['Fixed', 'Smallint', 'Integer']:
                if frac == 0:
                    displaysize = max(displaysize, length)
                    fmt = '|%' + str(displaysize) + 'd'
                else:
                    displaysize = max(displaysize, length + 1)
                    fmt = '|%' + str(displaysize - 1) + '.' + `frac` + 'f'
                nullfmt = '|' + (' ' * (displaysize - 1)) + '?'
            elif kind == 'Float':
                fmt = '|%' + str(displaysize) + 'g'
                nullfmt = '|' + (' ' * (displaysize-1)) + '?'
            else:
                displaysize = max(displaysize, length)
                fmt = '|%-' + str(displaysize) + 's'
                nullfmt = '|' + (' ' * (displaysize - 1)) + '?'
            formats.append (ValueFormat (fmt, nullfmt))
            header0 = header0 + ('|%-*.*s' %  (displaysize, displaysize, name))
            header1 = header1 + '+' + ('-' * displaysize)
        fmt = formats [-1].stdFormat
        nullfmt = formats [-1].nullFormat
        formats [-1] = ValueFormat (fmt + '|\n', nullfmt + '|\n')
        self.formats = formats
        self.header0 = header0 + '|\n'
        self.header1 = header1 + '+\n'

class PrintfFormat (Format):
    printfRE = re.compile ('%(?!%)'
        + '(?P<flags>-?[+]?0?)'
        + '(?P<width>[0-9]+)?'
        + '([.](?P<precision>[0-9]+))?'
        + '(?P<conversion>[diouxXfeEgGcs])')

    def __init__ (self, cursor, format, nullRepr = '#NULL'):
        pos = 0
        formats = []
        descriptions = self.getDescription (cursor)
        header0 = ''
        header1 = ''
        match = self.printfRE.search (format, pos)
        descriptionIndex = -1
        while match:
            fmt = format [pos:match.end ()]
            prefix = format [pos:match.start()]
            flags, width, prec, conv = match.group (
                'flags', 'width', 'precision', 'conversion')
            if flags is None:
                flags = ''
            descriptionIndex = descriptionIndex + 1
            name, kind, length, frac = descriptions [descriptionIndex]
            # format header and null
            xfmt = prefix + '%' + flags
            if conv == 's':
                # use same format
                if width != None:
                    xfmt = xfmt + width
                if prec != None:
                    xfmt = xfmt + '.' + prec
            else:
                # format one number and use this length as a string format
                if width is None:
                    widthHelper = ''
                else:
                    widthHelper = width
                if prec is None:
                    precHelper = ''
                else:
                    precHelper = '.' + prec
                helpFormat = '%' + flags + widthHelper + precHelper + conv
                formattedNumber = helpFormat % 1.1
                xfmt= xfmt + `len (formattedNumber)`
            xfmt = xfmt + 's'
            # header format
            header0 = header0 + (xfmt % name)
            line = '-' * len (name)
            header1 = header1 + (xfmt % line)
            # null format
            nullfmt = xfmt % nullRepr
            #
            formats.append (ValueFormat (fmt, nullfmt))
            pos = match.end ()
            remainder = format [pos:]
            match = self.printfRE.search (format, pos)
        if len (formats) != len (descriptions):
            raise 'PrintfError', 'invalid number of format expressions'
        formats [-1].concat (remainder + '\n')
        self.formats = formats
        self.header0 = header0 + remainder + '\n'
        self.header1 = header1 + remainder + '\n'

#
# value format
#
class ValueFormat:
    def __init__ (self, stdFormat, nullFormat):
        self.stdFormat = stdFormat
        self.nullFormat = nullFormat

    def format (self, value):
        if value is None:
            return self.nullFormat
        else:
            return self.stdFormat % value

    def concat (self, fmtConcat, nullConcat = None):
        if nullConcat is None:
            nullConcat = fmtConcat
        self.stdFormat = self.stdFormat + fmtConcat
        self.nullFormat = self.nullFormat + nullConcat

#
# tutorial
#

class TutorialShell (InteractiveReport):
    prompt = 'Tutorial> '
    intro = None
    introText = """*Welcome to the tutorial.
*You can end the tutorial by typing Ctrl-Z on NT or Ctrl-D on Unix."""
    __tutorial = [
        ('sql', "enter a SQL command like 'sql SELECT * from messages'"),
        ('next', "advance to the next page by entering 'next'"),
        ('next', "you can repeat the last command by pressing <ENTER>"),
        ('bottom', "go to the end of the result set by entering 'bottom'"),
        ('prev', "return to the previous page by entering 'prev'"),
        ('top', "return to the beginning by entering 'top'"),
        ('card', "you can switch to a different display by entering 'card'"),
        ('next', "now, 'next' will display the next record"),
        ('report', "'report' switches back to a paged display"),
        ('next', "'next' will actually display the next page"),
        ('grid', "'grid' will display fixed sized columns"),
        ('compressed', "'compressed' switches back to, well 'compressed' format"),
        ('table', "'table' displays information about SQL tables"),
    ]

    def __init__ (self, session, options):
        InteractiveReport.__init__ (self, session, options)
        self.ignoreUnmatch = None
        self.tutorialIndex = -1
        print self.introText
        self.advanceTutorial ()

    def getCmdPart (self, line):
        i, n = 0, len(line)
        while i < n and line[i] in self.identchars: i = i+1
        cmd, arg = line[:i], strip(line[i:])
        return cmd

    def onecmd (self, line):
        cmd = self.getCmdPart (line)
        result = InteractiveReport.onecmd (self, line)
        if cmd == self.allowedCommand:
            atEnd = self.advanceTutorial ()
            if atEnd:
                result = 1
        else:
            if self.ignoreUnmatch:
                pass   # no need to rub it in
            else:
                print ("*Nice try, but the tutorial continues with '"
                    + self.allowedCommand + "'")
        self.ignoreUnmatch = None
        return result

    def advanceTutorial (self, plus = 1):
        self.tutorialIndex = self.tutorialIndex + plus
        try:
            if not self.ignoreUnmatch:
                nextLesson = self.__tutorial [self.tutorialIndex]
                self.allowedCommand, text = nextLesson
                print '*Lesson', self.tutorialIndex + 1, ':',
                print text
            atEnd = None
        except IndexError:
            print ("*This concludes the tutorial. "
                + "You're on your own now.")
            atEnd = 1
        return atEnd

    def emptyline (self):
        self.ignoreUnmatch = 1
        InteractiveReport.emptyline (self)
        print '*That was easy, wasn\'t it? Now try a different command'
        self.advanceTutorial (0)
        self.ignoreUnmatch = 1

    def do_exit (self, line):
        result = InteractiveReport.do_exit (self, line)
        print '*Giving up already?'
        self.ignoreUnmatch = 1
        return result

    do_EOF = do_exit

def where ():
    import traceback
    traceback.print_stack ()

#
# main routine and other
#
def fatalError (msg):
    sys.stderr.write (msg + '\n')
    sys.exit (2)

def createSession (options):
    if not options.userinfo:
        fatalError ('user name must be specified (option -u)')
    if not options.dbname:
        fatalError ('dabase name must be specified (option -d)')
    name, pwd = split (options.userinfo, ',')
    try:
        connectoptions = []
        if not options.transactions:
            connectoptions.append ('autocommit=on')
        if options.oracle:
            connectoptions.append ('sqlmode=oracle')
        if options.odbc:
            connectoptions.append ('component=ODB')
        optstring = string.join (connectoptions, '&')
        result = sdb.sql.connect (name, pwd, options.dbname, options.node, optstring)
    except sdb.sql.SQLError, err:
        if err.errorCode == -4008:
            name = upper (name)
            result = sdb.sql.connect (name, pwd, options.dbname, options.node, optstring)
    return result

def manageReadline (options):
    if readline and hasattr(readline, "read_history_file"):
        histfile = os.path.expanduser (options.history)
        try:
            readline.read_history_file (histfile)
        except IOError:
            pass
        import atexit
        atexit.register(readline.write_history_file, histfile)

def interactive (report, options, args):
    if args:
        report.do_sql (join (args))
        if report.hasCursor ():
            report.onecmd ('next')
            report.cmdloop ('')
    else:
        report.cmdloop ()

def batch (report, options, args):
    if not args:
        sys.stderr.write ('ireport: no sql given')
        sys.exit (2)
    report.do_sql (join (args), show = None)
    if report.hasCursor ():
        if options.batch in ['grid', 'compressed']:
            format = options.batch
        else:
            # build formatter
            format = 'printf'
            report.printfFormat = PrintfFormat (report.cursor, options.batch)
            # iterate over cursor
        report.setupReport (format)
        report.displayAll (showHeader = None)

def main (options, args):
    """Displays results of SQL queries

    Any additional arguments are taken as an SQL command
    """
    manageReadline (options)
    if options.batch:
        constructor = InteractiveReport
    elif options.tutorial:
        constructor = TutorialShell
    else:
        constructor = InteractiveReport
    report = constructor (createSession (options), options)
    if options.batch:
        batch (report, options, args)
    else:
        interactive (report, options, args)

def defaultEditor ():
    try:
        editor = os.environ ['EDITOR']
    except:
        if os.name == 'nt':
            editor = 'edit'
        else:
            editor = 'vi'
    return editor

def defaultLines ():
    try:
        lines = int (os.environ ['LINES'])
    except:
        lines = 24
    return lines

def defaultCols ():
    try:
        lines = int (os.environ ['COLUMNS'])
    except:
        lines = 80
    return lines

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('u', 'userinfo', ':', None, '<user>,<pwd>'),
        ('d', 'dbname', ':', None, 'database name'),
        ('n', 'node', ':', '', 'server name'),
        ('lines', None, ':', defaultLines (), 'lines per screen'),
        ('columns', None, ':', defaultCols (), 'columns per line'),
        ('editor', None, ':', defaultEditor (), 'editor to edit SQL'),
        ('pager', None, ':', None, 'program to scroll result sets'),
        ('batch', None, ':', None, 'to stdout [grid, compressed or printf format]'),
        ('version', None, '', _versionString, 'version information'),
        ('oracle', None, '', None, 'enable Oracle (tm) SQL syntax'),
        ('odbc', None, '', None, 'enable ODBC/JDBC SQL syntax'),
        ('transactions', None, '', None, 'use transactions instead of autocommit'),
        ('history', None, ':', '~/.ireport', 'name of history file'),
        ('tutorial', None, '', None, 'start tutorial'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())


