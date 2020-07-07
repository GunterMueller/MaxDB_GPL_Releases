
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

import cStringIO

class RE_Filter:
    def __init__ (self, reCallbackPairs):
        self.pairs = reCallbackPairs

    def filterFile (self, fname, outfile = None):
        data = open (fname, 'rt').read ()
        newdata = self.filter (data)
        hasChanged = (data != newdata)
        if outfile:
            open (outfile, 'wt').write (newdata)
        elif hasChanged:
            open (fname, 'wt').write (newdata)
        return hasChanged

    def filter (self, data):
        pos = 0
        dataLen = len (data)
        outstream = cStringIO.StringIO ()
        reStates = self.buildInitialState (data)
        self.buildNextState (reStates, data, pos)
        matchStart, match, expr, callback = reStates [0]
        while matchStart < dataLen:
            outstream.write (data [pos:matchStart])
            pos, replacement = callback (match, data)
            outstream.write (replacement)
            self.buildNextState (reStates, data, pos)
            matchStart, match, expr, callback = reStates [0]
        outstream.write (data [pos:])
        return outstream.getvalue ()

    def buildInitialState (self, data):
        reStates = []
        for expr, callback in self.pairs:
            state = (-1, None, expr, callback)
            reStates.append (state)
        return reStates

    def buildNextState (self, reStates, data, pos):
        for i in xrange (len (reStates)):
            matchStart, match, expr, callback = reStates [i]
            if matchStart >= pos:
                # assumimg a sorted list
                # the remaining matches are still valid
                break
            match = expr.search (data, pos)
            if match:
                state = (match.start (), match, expr, callback)
            else:
                state = (len (data), None, expr, callback)
            reStates [i] = state
        reStates.sort ()

