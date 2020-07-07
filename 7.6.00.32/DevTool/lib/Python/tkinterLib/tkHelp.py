
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

from Tkinter import *

class TkHelp:
    def __init__ (self, closeCallback, text):
        self.closeCallback = closeCallback
        self.text = text

    def show (self):
        #
        # create widgets
        #
        toplevel = self.toplevel = Toplevel ()
        toplevel.title ('Help')
        text = Text (toplevel)
        closeButton = Button (toplevel, text = 'Close',  command = self.close)
        #
        # fill data
        #
        text.insert (END, self.text)
        text.config (state = DISABLED)
        #
        # layout
        #
        text.grid (row = 0, col = 0, sticky = N + S + W + E)
        closeButton.grid (row = 1, col = 0)
        toplevel.grid_rowconfigure (0, weight = 1)
        self.takeFocus ()

    def close (self, event = None):
        self.toplevel.destroy ()
        self.closeCallback ()

    def takeFocus (self):
        self.toplevel.focus_set ()


