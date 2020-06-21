
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

from tkinterLib import tkinterLib
tkinterLib.assertTkinterInstalled ()

import string
import os

import diffLib
from   Tkinter import *

from tkinterLib import tkHelp

_version = "0.3"

true = 'true'

class AbstractWDiff:
    helpText = """
compares two files side by side

    Allows to merge changes into one file.

    Colors:
        red     lines deleted
        green   lines added
        yellow  lines changed
        grey    diff copied
        gold    current diff

    Keys:
        n       next difference
        p       previous difference
        <space> copy/undo current diff
        s       save file
        x       exit

    Cursors left panel:
        right arrow     merge changes into right panel

    Cursors right panel:
        X               delete difference
        exchange        revert change
    """
    def __init__ (self, options):
        self.options = options
        self.diffs = []
        self.changeCount = 0
        self.helpWindow = None
        self.currentDiff = 0

    def addDiff (self, file1, file2):
        self.diffs.append (diffLib.Diff2 (file1, file2))

    def mainLoop (self, title = 'wdiff', line = -1, files = None):
        root = Tk ()
        root.title (title)
        self.bindNavigation (root)
        if files:
            self.labels = files
        try:
            root.winfo_toplevel().wm_state ('zoomed')
        except TypeError:
            pass # does not work in 1.5.2
        self.show (root, line)
        root.mainloop ()

    #
    # events
    #

    def ignore (self, event):
        print event, event.type, dir (event)
        return 1

    def moveToTop (self, event = None):
        self.moveTo (0.0)
        self.noteNewDiffPosition (0)

    def moveToEnd (self, event = None):
        self.moveTo (1.0)
        self.noteNewDiffPosition (len (self.diffObjects) - 1)

    def moveTo (self, where):
        for widget in self.synchronizedWidgets:
            widget.yview ('moveto', where)

    def moveToLine (self, lineno):
        index = str (lineno - 1) + ".0"
        for widget in self.synchronizedWidgets:
            widget.see (index)

    def scrollDown (self, event = None):
        self.scrollPage (1)

    def scrollUp (self, event = None):
        self.scrollPage (-1)

    def scrollPage (self, pageCount):
        for widget in self.synchronizedWidgets:
            widget.yview ('scroll', pageCount, 'pages')
        self.updateCurrentDiff ()

    def scrollbarCommand (self, *args):
        for widget in self.synchronizedWidgets:
            apply (widget.yview, args)
        self.updateCurrentDiff ()

    def scrollbarHCommand (self, *args):
        for widget in self.synchronizedWidgets:
            apply (widget.xview, args)

    def nextDiff (self, event = None):
        self.moveDiff (+1)

    def prevDiff (self, event = None):
        self.moveDiff (-1)

    def moveDiff (self, count):
        self.moveToDiff (self.currentDiff + count)

    def moveToDiff (self, diffIndex):
        try:
            newPos = diffIndex
            if newPos < 0:
                self.outOfBoundsWarning ()
                return
            nextDiff = self.diffObjects [newPos]
            widget = self.textWidgets [0]
            height = int (widget ['height'])
            diffStart = nextDiff.linePos
            newLine = diffStart - (height / 2)
            for widget in self.synchronizedWidgets:
                widget.yview (newLine)
            self.noteNewDiffPosition (newPos)
        except IndexError:
            self.outOfBoundsWarning ()

    def updateCurrentDiff (self, oldPos = None):
        textWidget = self.textWidgets [0]
        diffObjects = self.diffObjects
        posOfCurrentDiff = diffObjects [self.currentDiff].linePos
        if textWidget.bbox (str (posOfCurrentDiff) + '.0'):
            # diff is still visible, no need to move focus
            return
        topIndex = textWidget.index ('@0,0')
        topLine = int (string.split (topIndex, '.') [0])
        bottomLine = topLine + int (textWidget ['height']) - 1
        diffIndex = self.currentDiff
        if topLine > posOfCurrentDiff:
            while 1:
                nextIndex = diffIndex + 1
                if nextIndex >= len (diffObjects):
                    #print 'breaking at end'
                    break
                nextDiff = diffObjects [nextIndex]
                if nextDiff.linePos > bottomLine:
                    #print 'next diff is out of reach'
                    break
                if (nextDiff.linePos <= bottomLine) \
                        and (nextDiff.linePos > topLine):
                    diffIndex = nextIndex
                    #print 'next diff is visible'
                    break
                diffIndex = nextIndex
        else:
            while 1:
                nextIndex = diffIndex - 1
                if nextIndex < 0:
                    #print 'breaking at end'
                    break
                nextDiff = diffObjects [nextIndex]
                if nextDiff.linePos > topLine:
                    #print 'next diff is out of reach'
                    break
                if (nextDiff.linePos <= topLine) \
                        and (nextDiff.linePos > bottomLine):
                    diffIndex = nextIndex
                    #print 'next diff is visible'
                    break
                diffIndex = nextIndex
        self.noteNewDiffPosition (diffIndex)

    def noteNewDiffPosition (self, newDiffPos):
        self.diffObjects [self.currentDiff].unmarkCurrent ()
        self.currentDiff = newDiffPos
        self.diffObjects [self.currentDiff].markCurrent ()
        self.statusbar.noteDiffPos (newDiffPos + 1)

    def outOfBoundsWarning (self):
        self.statusbar.message ('   *** no more diffs ***   ', bg = 'red')
        self.statusbar.bell ()

    def toggleCurrent (self, event = None):
        self.diffObjects [self.currentDiff].toggleMine ()

    def bindNavigation (self, window):
        window.bind ('<KeyPress Next>', self.scrollDown)
        window.bind ('<KeyPress Prior>', self.scrollUp)
        window.bind ('<KeyPress Home>', self.moveToTop)
        window.bind ('<KeyPress End>', self.moveToEnd)
        window.bind ('<KeyPress F1>', self.help)
        window.bind ('<KeyPress space>', self.toggleCurrent)
        window.bind ('<Control KeyPress l>', self.toggleLineWidgets);
        window.bind ('<Button-3>', self.popup)
        for key, command in [
                ('s', self.save),
                ('x', self.exit),
                ('n', self.nextDiff),
                ('p', self.prevDiff)]:
            window.bind ('<KeyPress ' + key + '>', command)
            window.bind ('<Control KeyPress ' + key + '>', command)

    def popup (self, event):
        self.diffPopup.post (event.x_root, event.y_root)


    #
    # misc
    #

    def save (self, event = None):
        data = self.mine.get ("1.0", END)
        if data [-2:] == '\n\n':
            data = data [:-1]
        data = string.replace (data, diffLib.imaginaryLine, '')
        fname = self.getMineFile ()
        try:
            open (fname, 'wt').write (data)
            message = fname + ' saved'
        except IOError, exc:
            message = 'Save error: ' + str (exc)
        self.statusbar.message (message)

    def exit (self, event = None):
        self.parent.quit ()

    def createWidgetsAndLayout (self, parent, textLabels):
        self.parent = parent
        #
        # create Widgets
        #
        menu = self.createMenu (parent)
        textWidgets = self.textWidgets = []
        lineWidgets = self.lineWidgets = []
        for i in xrange (len (textLabels)):
            widget = Text (parent, font = ('Courier', 9),
                height = 50, wrap = NONE,
                cursor = 'arrow')
            textWidgets.append (widget)
            widget = Text (parent, font = ('Courier', 9),
                height = 50, width = 6, wrap = NONE)
            lineWidgets.append (widget)
        self.synchronizedWidgets = textWidgets + lineWidgets
        self.statusbar = WDiffStatusBar (parent, len (self.diffs [0]))
        #toolbar = self.createToolbar (parent)
        #infobar = self.createInfobar (parent)
        self.bindNavigation (parent)
        self.lineWidgetsVisible = 'true'

        #
        # packing
        #
        buttonRow, labelRow, editorRow, hscrollRow, statusRow = range (5)
        fillAll = N + S + W + E
        fillVertical = N + S
        lineOffset, editorOffset, scrollbarOffset = range (3)
        #toolbar.grid (row = buttonRow, sticky = fillAll)
        #infobar.grid (row = buttonRow, col = 2, sticky = fillAll)
        for i in xrange (len (textLabels)):
            col = i * 3
            # add label
            label = Label (parent, text = textLabels [i],
                anchor = W, relief = SUNKEN)
            label.grid (row = labelRow, column = col + lineOffset,
                columnspan = 3, sticky = fillAll)
            # add line widget
            lineWidget = lineWidgets [i]
            lineWidget.grid (row = editorRow, column = col + lineOffset,
                sticky = fillAll)
            # add text widget
            textWidget = textWidgets [i]
            textWidget.grid (row = editorRow, column = col + editorOffset,
                sticky = fillAll)
            # add vertival scrollbar
            vscrollbar = Scrollbar (parent, orient = VERTICAL,
                command = self.scrollbarCommand)
            textWidget.config (yscrollcommand = str (vscrollbar) + ' set')
            vscrollbar.grid (row = editorRow, column = col + scrollbarOffset,
                sticky = fillAll)
            # add horizontal scrollbar
            hscrollbar = Scrollbar (parent, orient = HORIZONTAL,
                command = self.scrollbarHCommand)
            textWidget.config (xscrollcommand = str (hscrollbar) + ' set')
            hscrollbar.grid (row = hscrollRow, column = col + lineOffset,
                columnspan = 2, sticky = fillAll)
            # configure column
            parent.grid_columnconfigure (col + editorOffset, weight = 1)
        self.statusbar.grid (row = statusRow, column = 0,
            columnspan = 2, sticky = W)
        parent.grid_rowconfigure (editorRow, weight = 1)
        #
        # apply options
        #
        self.applyOptions ()
        #
        #
        #
        return textWidgets

    def applyOptions (self):
        if self.options.nonum:
            self.removeLineWidgets ()

    def addLineWidgets (self):
        fillAll = N + S + W + E
        for i in xrange (len (self.lineWidgets)):
            col = i* 3
            lineOffset = 0
            editorRow = 2
            lineWidget = self.lineWidgets [i]
            lineWidget.grid (row = editorRow, col = col + lineOffset,
                sticky = fillAll)
        self.lineWidgetsVisible = 'true'

    def removeLineWidgets (self):
        for lineWidget in self.lineWidgets:
            lineWidget.grid_forget ()
        self.lineWidgetsVisible = None

    def toggleLineWidgets (self, event = None):
        if self.lineWidgetsVisible:
            self.removeLineWidgets ()
        else:
            self.addLineWidgets ()

    def createMenu (self, parent):
        menu = Menu (parent)
        parent ['menu'] = menu
        #
        # File Menu
        #
        file = Menu (menu, tearoff = 0)
        menu.add_cascade (label = 'File', underline = 0, menu = file)
        file.add_command (label = 'Save', command = self.save,
            underline = 0, accelerator = 'Ctrl-S')
        file.add_separator ()
        file.add_command (label = 'eXit', command = self.exit,
            underline = 1, accelerator = 'Ctrl-X')
        #
        # Diff menu
        #
        diff = self.diffPopup = Menu (menu)
        menu.add_cascade (label = 'Diff', underline = 0, menu = diff)
        diff.add_command (label = 'Top', command = self.moveToTop,
            accelerator = 'Home')
        diff.add_command (label = 'Bottom', command = self.moveToEnd,
            accelerator = 'End')
        diff.add_command (label = 'Prev', underline = 0, command = self.prevDiff,
            accelerator = 'p')
        diff.add_command (label = 'Next', underline = 0, command = self.nextDiff,
            accelerator = 'n')
        diff.add_separator ()
        diff.add_command (label = ' Change', underline = 0,
            command = self.toggleCurrent, accelerator = 'space')
        #
        # color menu
        #
        color = Menu (menu, relief = SUNKEN)
        menu.add_cascade (label = 'Colors', underline = 0, menu = color)
        for label, colkey in [('added', 'a'),
                              ('deleted', 'd'), ('changed', 'c'),
                              ('current', 'current'), ('updated', 'updated')]:
            xcolor = _diffColors [colkey]
            color.add_command (label = label, background = xcolor)
        #
        # layout menu
        #
        layout = self.createLayoutMenu (menu)
        #
        # Help menu
        #
        help = Menu (menu, tearoff = 0)
        menu.add_cascade (label = 'Help', underline = 0, menu = help)
        help.add_command (label = 'Help', underline = 0,
            command = self.help, accelerator = 'F1')
        #
        # return
        #
        return menu

    def createLayoutMenu (self, menu):
        layout = Menu (menu, relief = SUNKEN, tearoff = 0)
        menu.add_cascade (label = 'Layout', underline = 0, menu = layout)
        layout.add_command (label = 'Line Numbers',
            command = self.toggleLineWidgets,
            accelerator = 'Ctrl-L')
        return layout


    def createToolbar (self, parent):
        toolbar = Frame (parent)
        i = 0
        for label, command, underline, iconFile in [
                ('Save', self.save, 0, 'Save'),
                ('Exit', self.exit, 1, 'Stop'),
                ('Prev', self.prevDiff, 0, 'Up'),
                ('Next', self.nextDiff, 0, 'Down'),
                (' Change' ,self.toggleCurrent, 0, 'DocumentIn'),
                ]:
            #fullname = ('D:/SAPDevelop/Devtool/lib/Python/tkinterLib/icons/'
            #    + iconFile + '.gif')
            #icon = PhotoImage (file = fullname, format = 'GIF',
            #    width = 20, height = 20)
            #button = Button (toolbar, image = icon, command = command)
            #button.grid (row = 0, column = i, sticky = E)
            #i = i + 1
            button = Button (toolbar, text = label,
                command = command, underline = underline)
            button.grid (row = 0, column = i, sticky = W + E, padx = 20)
            toolbar.grid_columnconfigure (i, weight = 1)
            i = i + 1
        return toolbar

    def createInfobar (self, parent):
        infobar = Frame (parent)
        i = 0
        for label, colkey in [('added', 'a'),
                              ('deleted', 'd'), ('changed', 'c'),
                              ('current', 'current'), ('updated', 'updated')]:
            color = _diffColors [colkey]
            button = Label (infobar, text = label, background = color,
                relief = SUNKEN)
                #relief = SOLID)
            button.grid (row = 0, column = i, sticky = W + E, padx = 20)
            infobar.grid_columnconfigure (i, weight = 1)
            i = i + 1
        return infobar

    def help (self, event = None):
        if self.helpWindow is None:
            self.helpWindow = tkHelp.TkHelp (self.destroyHelp, self.helpText)
            self.helpWindow.show ()
        else:
            self.helpWindow.takeFocus ()

    def destroyHelp (self):
        self.helpWindow = None


class WDiff (AbstractWDiff):

    labels = ['Changed', 'Local']
    def show (self, parent, line = -1):
        baseW, mineW = self.createWidgetsAndLayout (
            parent, self.labels)
        self.mine = mineW
        self.fillSplitText (mineW, baseW)
        for widget in [mineW, baseW]:
            widget.config (state = DISABLED)
        if line > 0:
            self.currentDiff = 0
            newPos = "%d.0" % line
            for widget in self.synchronizedWidgets:
                widget.see (newPos)
            self.updateCurrentDiff ()
        else:
            self.currentDiff = -1
            self.nextDiff ()

    def createLayout (self, parent):
        diffFrame = Frame (parent)
        mainFrame = Frame (diffFrame)
        mainFrame.pack (fill = BOTH, expand = 1)
        return diffFrame, mainFrame

    def createTextWidget (self, parent, file = None):
        widget = Text (parent, height = 50, cursor = 'arrow')
        widget.pack (fill = BOTH, expand = 1)
        if file != None:
            self.fillText (widget, file)
        return widget

    def fillSplitText (self, mine, yours):
        diffCount = 0
        mainDiff = self.diffs [0]
        diffs = mainDiff.getSplitFile ()
        diffObjects = self.diffObjects = []
        for kind, lines, alternate, diff in diffs:
            text = string.join (lines, '')
            if kind == 's':
                mine.insert (END, text)
                yours.insert (END, text)
            else:
                diffCount = diffCount + 1
                newEntry = WDiffEntry (self, diffCount, kind,
                    mine, yours, text, string.join (alternate, ''))
                diffObjects.append (newEntry)
        myLineWidget, yourLineWidget = self.lineWidgets
        myLines, yourLines = mainDiff.getLineNumbers ()
        myLineWidget.insert (END, myLines)
        yourLineWidget.insert (END, yourLines)

    def getMineFile (self):
        return self.diffs [0].file1


class WDiffStatusBar (Label):
    def __init__ (self, frame, diffCount):
        Label.__init__ (self, frame, anchor = 'w', relief = SUNKEN)
        self.config (text = 'status')
        # self.pack (side = RIGHT, fill = X, expand = 1)
        self.diffCount = diffCount
        self.diffPos = 0
        self.changeCount = 0
        self.stdbg = self ['bg']

    def isChanged (self):
        return self.changeCount > 0

    def noteDiffPos (self, newPos):
        self.diffPos = newPos
        self.displayStatus ()

    def addChange (self):
        self.changeCount = self.changeCount + 1
        self.displayStatus ()

    def revertChange (self):
        self.changeCount = self.changeCount - 1
        self.displayStatus ()

    def displayStatus (self, event = None):
        if self.changeCount > 0:
            changedFlag = '*'
        else:
            changedFlag = ' '
        text = '%s at %d of %d differences' % (changedFlag,
            self.diffPos, self.diffCount)
        self.config (text = text, bg = self.stdbg)

    def message (self, msg, bg = None):
        self.config (text = msg)
        if bg != None:
            self.config (bg = bg)
        self.after (5000, self.displayStatus)


class WDiffEntry:
    deletedFont = None

    def __init__ (self, main, diffIndex, kind, mine, yours, text, altText):
        self.main = main
        self.diffIndex = diffIndex - 1
        tag = self.tag = 'tag%010d' % diffIndex
        self.kind = kind
        self.mine = mine
        self.yours = yours
        self.text = text
        self.altText = altText
        self.isUpdated = None
        self.fillMineWidget (mine, text, tag)
        self.fillYourWidget (yours, altText, tag)

    def asTKIndex (self):
       return str (self.linePos) + '.0'

    def fillMineWidget (self, widget, text, tag):
        widget.insert (END, text, tag)
        widget.tag_config (tag, background = _diffColors [self.kind])
        if self.kind == 'd':
            crossCursorHandler.bindTag (widget, tag)
            widget.tag_bind (tag, '<1>', self.changeMine)

    def fillYourWidget (self, widget, text, tag):
        posString = widget.index (END)
        self.linePos = int (string.split (posString, '.') [0])
        widget.insert (END, text, tag)
        self.unchangedColor = _diffColors [self.kind]
        widget.tag_config (tag, background = self.unchangedColor)
        yourCursorHandler.bindTag (widget, tag)
        widget.tag_bind (tag, '<1>', self.changeMine)

    def changeMine (self, event = None):
        tag = self.tag
        mine = self.mine
        yours = self.yours
        self.replaceTagSelection (self.altText, tag)
        self.isUpdated = true
        mineCursorHandler.bindTag (mine, tag)
        mine.tag_bind (tag, '<1>', self.restoreMine)
        mine.tag_config (tag, background = _diffColors ['updated'])
        if (event != None) and (event.widget == mine):
            mine.config (cursor = 'exchange')
        noCursorHandler.bindTag (yours, tag)
        yours.config (cursor = 'arrow')
        yours.tag_bind (tag, '<1>', self.ignore)
        self.main.moveToDiff (self.diffIndex)
        self.main.statusbar.addChange ()


    def restoreMine (self, event = None):
        tag = self.tag
        mine = self.mine
        yours = self.yours
        self.replaceTagSelection (self.text, tag)
        self.isUpdated = None
        if self.kind == 'd':
            crossCursorHandler.bindTag (mine, tag)
            if self.kind == 'd':
                mine.config (cursor = 'X_cursor')
            else:
                mine.config (cursor = 'arrow')
            mine.tag_bind (tag, '<1>', self.changeMine)
        else:
            noCursorHandler.bindTag (mine, tag)
            mine.config (cursor = 'arrow')
            mine.tag_bind (tag, '<1>', self.ignore)
        mine.tag_config (tag, background = self.unchangedColor)
        yourCursorHandler.bindTag (yours, tag)
        yours.tag_bind (tag, '<1>', self.changeMine)
        self.main.moveToDiff (self.diffIndex)
        self.main.statusbar.revertChange ()

    def toggleMine (self):
        if self.isUpdated:
            self.restoreMine ()
        else:
            self.changeMine ()

    def ignore (self, event):
        pass

    def replaceTagSelection (self, newText, tag):
        mine = self.mine
        mine.config (state = NORMAL)
        fromVal, toVal = mine.tag_nextrange (tag, "1.0")
        mine.delete (fromVal, toVal)
        mine.insert (fromVal, newText, tag)
        mine.config (state = DISABLED)

    def markCurrent (self):
        self.yours.tag_config (self.tag, background = _diffColors ['current'])

    def unmarkCurrent (self):
        self.yours.tag_config (self.tag, background = self.unchangedColor)

_diffColors = {
 'a': 'green',
 'd': 'red',
 'c': 'yellow',
 'current': 'gold',
 'updated': 'grey',
 'default': 'grey',
}

_diffColors = {
 'a': 'mediumseagreen',
 'd': 'palevioletred',
 'c': 'khaki',
 'current': 'gold',
 'updated': 'grey',
 'default': 'grey',
}

#
# cursorHandling
#

class CursorHandler:
    def __init__ (self, entryCursor, exitCursor = 'arrow'):
        self.entryCursor = entryCursor
        self.exitCursor = exitCursor

    def entry (self, event):
        event.widget.config (cursor = self.entryCursor)

    def exit (self, event):
        event.widget.config (cursor = self.exitCursor)

    def bindTag (self, widget, tag):
        widget.tag_bind (tag, '<Enter>', self.entry)
        widget.tag_bind (tag, '<Leave>', self.exit)

noCursorHandler = CursorHandler ('arrow')
mineCursorHandler = CursorHandler ('exchange')
yourCursorHandler = CursorHandler ('sb_right_arrow')
crossCursorHandler = CursorHandler ('X_cursor')

baseCursorHandler = CursorHandler ('sb_right_arrow')
otherCursorHandler = CursorHandler ('sb_left_arrow')


def showChangeCursor (event):
    event.widget.config (cursor = 'exchange')

def showStdCursor (event):
    event.widget.config (cursor = 'arrow')

#
# diff3
#

class WDiff3 (AbstractWDiff):
    helpText = """
    compares three files side by side

    Allows to merge changes into one file.

    Arguments: base mine other
        base    the common ancestor of mine and other
        mine    the local file
        other   the matching file in a different branch

    Colors:
        yellow  lines changed

    Keys:
        n       next difference
        p       previous difference
        Ctrl-s  save file

    Cursors left panel:
        right arrow     merge changes into middle panel

    Cursors middle panel:
        exchange        revert change

    Cursors right panel:
        left arrow      merge changes into middle panel
    """
    labels = ['Base', 'Mine', 'Other']

    def __init__ (self, options, base, mine, other, labels = None):
        self.baseWidgetHidden = None
        AbstractWDiff.__init__ (self, options)
        if labels != None:
            self.labels = labels
        self.addDiff (base, mine, other)

    def dump (self):
        self.diffInfo.dump ()

    def addDiff (self, base, mine, other):
        self.diffs.append (diffLib.Diff3 (base, mine, other))

    def bindNavigation (self, window):
        AbstractWDiff.bindNavigation (self, window)
        window.bind ('<Control KeyPress b>', self.toggleBaseWidgets);

    def createLayoutMenu (self, menu):
        layout = AbstractWDiff.createLayoutMenu (self, menu)
        layout.add_command (label = 'Base Source',
            command = self.toggleBaseWidgets,
            accelerator = 'Ctrl-B')
        return layout

    def applyOptions (self):
        AbstractWDiff.applyOptions (self)
        if self.options.nobase:
            self.hideBaseWidgets ()

    def hideBaseWidgets (self):
        baseWidget = self.textWidgets [0]
        self.baseEditorSize = baseWidget ['width']
        baseWidget.config (width = 2)
        self.baseWidgetHidden = 'true'

    def showBaseWidgets (self):
        baseWidget = self.textWidgets [0]
        baseWidget.config (width = self.baseEditorSize)
        self.baseWidgetHidden = None

    def toggleBaseWidgets (self, event = None):
        if self.baseWidgetHidden:
            self.showBaseWidgets ()
        else:
            self.hideBaseWidgets ()


    def show (self, parent, line = -1):
        baseW, mineW, otherW = self.createWidgetsAndLayout (
            parent, self.labels)
        self.mine = mineW
        self.fillSplitText (baseW, mineW, otherW)
        if line > 0:
            self.currentDiff = 0
            newPos = "%d.0" % line
            for widget in self.synchronizedWidgets:
                widget.see (newPos)
            self.updateCurrentDiff ()
        else:
            self.currentDiff = -1
            self.nextDiff ()

    def fillSplitText (self, base, mine, other):
        textWidgets = [base, mine, other]
        diffCount = 0
        mainDiff = self.diffs [0]
        diffs = mainDiff.getSplitFile ()
        diffObjects = self.diffObjects = []
        for kind, baseChunk, mineChunk, otherChunk, diff in diffs:
            if kind == 's':
                for widget in textWidgets:
                    widget.insert (END, baseChunk)
            else:
                #if mineChunk != otherChunk:
                if self.isRealDiff (mineChunk, otherChunk):
                    diffCount = diffCount + 1
                    diffEntry = WDiff3Entry (self, diffCount,
                        base, baseChunk, mine, mineChunk,
                        other, otherChunk)
                    diffObjects.append (diffEntry)
                else:
                    # the change has been incorporated into both releases
                    baseWidget, mineWidget, otherWidget = textWidgets
                    baseWidget.insert (END, baseChunk)
                    mineWidget.insert (END, mineChunk)
                    otherWidget.insert (END, otherChunk)
        for widget in textWidgets:
            widget.config (state = DISABLED)
        self.statusbar.diffCount = len (diffObjects)
        baseLineWidget, myLineWidget, otherLineWidget = self.lineWidgets
        baseLines, myLines, otherLines = mainDiff.getLineNumbers ()
        baseLineWidget.insert (END, baseLines)
        myLineWidget.insert (END, myLines)
        otherLineWidget.insert (END, otherLines)

    def isRealDiff (self, mineChunk, otherChunk):
        if string.strip (mineChunk) == string.strip (otherChunk):
            return None
        else:
            return 1


    def getMineFile (self):
        return self.diffs [0].mine


class WDiff3Entry:
    def __init__ (self, main, diffCount, baseWidget, baseText,
            mineWidget, mineText, otherWidget, otherText):
        self.main = main
        tag = self.tag = 'tag%010d' % diffCount
        self.baseWidget = baseWidget
        self.baseText = baseText
        self.mineWidget = mineWidget
        self.mineText = mineText
        self.otherWidget = otherWidget
        self.otherText = otherText
        self.fillBaseWidget (baseWidget, baseText, tag)
        self.fillMineWidget (mineWidget, mineText, tag)
        self.fillOtherWidget (otherWidget, otherText, tag)
        self.isChanged = None

    def fillWidget (self, widget, text, tag, mouseHandler, cursor):
        widget.insert (END, text, tag)
        widget.tag_config (tag, background = _diffColors ['c'])
        self.unchangedColor = _diffColors ['c']
        cursor.bindTag (widget, tag)
        widget.tag_bind (tag, '<1>', mouseHandler)

    def fillBaseWidget (self, widget, text, tag):
        self.fillWidget (widget, text, tag, self.useBase, baseCursorHandler)

    def fillMineWidget (self, widget, text, tag):
        posString = widget.index (END)
        self.linePos = int (string.split (posString, '.') [0])
        self.fillWidget (widget, text, tag, self.ignore, noCursorHandler)

    def fillOtherWidget (self, widget, text, tag):
        self.fillWidget (widget, text, tag, self.useOther, otherCursorHandler)

    def changeMine (self, changeText):
        tag = self.tag
        mine = self.mineWidget
        self.replaceTagSelection (changeText, tag)
        mineCursorHandler.bindTag (mine, tag)
        mine.tag_bind (tag, '<1>', self.restoreMine)
        if not self.isChanged:
            self.main.statusbar.addChange ()
            self.isChanged = 1
        else:
            mine.tag_config (tag, background = _diffColors ['updated'])

    def useBase (self, event):
        self.changeMine (self.baseText)

    def useOther (self, event):
        self.changeMine (self.otherText)

    def restoreMine (self, event):
        tag = self.tag
        mine = self.mineWidget
        self.replaceTagSelection (self.mineText, tag)
        noCursorHandler.bindTag (mine, tag)
        mine.config (cursor = 'arrow')
        mine.tag_bind (tag, '<1>', self.ignore)
        mine.tag_config (tag, background = self.unchangedColor)
        self.main.statusbar.revertChange ()
        self.isChanged = None

    def replaceTagSelection (self, newText, tag):
        mine = self.mineWidget
        mine.config (state = NORMAL)
        fromVal, toVal = mine.tag_nextrange (tag, "1.0")
        mine.delete (fromVal, toVal)
        mine.insert (fromVal, newText, tag)
        mine.config (state = DISABLED)

    def ignore (self, event):
        pass

    def markCurrent (self):
        for widget in [self.baseWidget, self.otherWidget]:
            widget.tag_config (self.tag, background = _diffColors ['current'])

    def unmarkCurrent (self):
        for widget in [self.baseWidget, self.otherWidget]:
            widget.tag_config (self.tag, background = self.unchangedColor)

def readOptions (**defaults):
    dir = os.getcwd ()
    while 1:
        fullName = os.path.join (dir, 'wdiff.conf')
        if os.path.exists (fullName):
            globals = {}
            # read content
            execfile (fullName, globals, defaults)
            break
        olddir = dir
        dir = os.path.dirname (dir)
        if dir == olddir:
            # nothing found
            break
    return defaults

