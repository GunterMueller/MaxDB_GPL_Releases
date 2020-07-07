
from sql import sapdb
from sql import sqlHelper
import qaccConfig

username, password, dbname, host, options = qaccConfig.connectArgs

def connect (username = username, password = password,
        dbname = dbname, host = host, options = options):
    return sapdb.connect (username, password, dbname, host, options)

CommunicationError = sapdb.CommunicationError
SQLError = sapdb.SQLError

def extendedConnect (username = username, password = password,
        dbname = dbname, host = host, options = options, vardict = None):
    if vardict is None:
        vardict = sqlHelper.SelfDict ()
    innerConnection = connect (username, password, dbname, host, options)
    return sqlHelper.ExtendedConnection (innerConnection, vardict)

