
import string

class CommandCollection:
    def __init__ (self, session, stringDict):
        self.session = session
        self.stringDict = stringDict

    def getStringForCommand (self, commandName):
        return self.stringDict [commandName]

    def __getattr__ (self, name):
        try:
            cmd = self.getStringForCommand (name)
        except KeyError:
            raise AttributeError (name)
        prepared = self.session.prepare (cmd)
        callable = prepared.execute
        setattr (self, name, callable)
        return callable

def connect (options, userInfoName = 'userInfo',
        dbnameName = 'dbname', nodeName = 'node',
        propName = 'connectProperties'):
    userInfo = getattr (options, userInfoName)
    dbname = getattr (options, dbnameName)
    node = getattr (options, nodeName)
    try:
        properties = getattr (options, propName)
    except AttributeError:
        properties = ''
    pos = string.find (userInfo, ',')
    if pos != -1:
        username = userInfo [:pos]
        pwd = userInfo [pos + 1:]
    else:
        username = userInfo
        pwd = ''
    if not node:
        pos = string.find (dbname, ':')
        if pos != -1:
            node = dbname [:pos]
            dbname = dbname [pos + 1:]
        else:
            node = ''
    import sapdb
    result = sapdb.connect (username, pwd, dbname, node, properties)
    return result

