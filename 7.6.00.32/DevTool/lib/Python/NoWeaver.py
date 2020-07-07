
class NoWeaver:
    def __init__ (self):
        pass

    def setCurrentFile (self, fname):
        pass

    def removeCurrentEntries (self):
        pass

    def addClass (self, className, anchor, description = None):
        pass

    def addMethod (self, className, memberName, anchor = None, description = None):
        pass

    def addFunction (self, functionName, anchor = None, description = None):
        pass

    addRoutine = addFunction

    def getClassLink (self, classname, text = None):
        if text == None:
            result = classname
        else:
            result = text
        return result

    def addAnything (self, name, description = None):
        pass

    def endTransaction (self, ok):
        pass

