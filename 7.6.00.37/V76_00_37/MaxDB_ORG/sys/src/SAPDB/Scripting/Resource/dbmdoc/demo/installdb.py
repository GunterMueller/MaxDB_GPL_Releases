#    coding: utf-8
#
#    module:       installdb.py
#
#    responsible : d033893 
#
#    special area: sdb.dbm
#
#    description : installation of database via DB Manager
#
#    prerequisite: Please install the sdb Scripting Interface.
#
#  remarks:      For including a private configuration this script 
#                looks for a file named "myparams.dbm" in the current
#                directory. In this file you can provide private
#                definitions of parameters and devspaces.
#
#                Lines starting with "param_put" contains a valid
#                "param_put" DBMServer command.
#                This lines will be used in the "Create param file..."
#                section of this script.
# 
#                Lines starting with "param_adddevspace" contains a
#                a valid "param_adddevspace" DBMServer command.         
#                This lines will be used in the "Add devspaces..."
#                section of this script.
#
#                All other lines will be ignored.
#
#    ========== licence begin  GPL
#    Copyright (c) 2001-2005 SAP AG
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
import time
import os
import optlib
import socket
#strings
import string
import re
#sapdb
from   sdb  import dbm,loader,sql

LOADERAPPLPATH  =  os.getcwd()
LOADERDATAPATH  =  os.getcwd()

def _options ():
    return [
    ('n', 'node'             , ':', socket.gethostname()    , 'specifies servernode'),
    ('r', 'instroot'         , ':', ''                      , 'specifies INSTROOT path'),
    ('d', 'dbname'           , ':', 'sourcedb'              , 'specifies sapdb to create'),
    ('a', 'dbauser'          , ':', 'DBA,DBA'               , 'specifies sapdb SYSDBA user'),
    ('s', 'dbmuser'          , ':', 'DBM,DBM'               , 'specifies sapdb SYSTEM user'),
    ('e', 'charencoding'     , ':', 'ascii'                 , 'specifies sapdb char encoding'),         # ASCII , UNICODE
    ('o', 'osuser'           , ':', ''                      , 'specifies operating system user'),
    ('b', 'restore'          , ':', None                    , 'restore database'),
    ('mode', 'mode'          , ':', 'PARAM'                 , 'specifies demo  mode'),                  # PARAM,INTER,DEBUG
]

def check_options(options):
    rc    = 0 
    
    options.mode          = string.upper(options.mode)
    options.charencoding  = string.upper(options.charencoding)
    options.dbname        = string.upper(options.dbname)
    options.osuser        = string.upper(options.osuser)
    options.dbmuser       = string.upper(options.dbmuser)
    options.dbauser       = string.upper(options.dbauser)
    
    if options.instroot == '':
       options.instroot = os.environ['INSTROOT'] 
    if os.name == 'posix' :
       options.osuser = 'remuser,Remuser'   
    
    #Transformation Resources
    print '-n %s'  % options.node 
    print '-r %s'  % options.instroot
    print '-d %s'  % options.dbname
    print '-a %s'  % options.dbauser     
    print '-o %s'  % options.osuser       
    print '-m %s'  % options.dbmuser      
    print '-e %s'  % options.charencoding
    print ''

def main (options, args):
    input = None
    print "\nLoader Installation Module:\n'%s'" % os.path.abspath(sys.argv[0])
    if sys.platform[:3] == 'win':
       import _winreg
       try: 
          logdir = os.path.join(_winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"APPDATA")[0],'sdb','loader','log') 
          if os.path.isdir(logdir) == False:
             os.makedirs  (logdir)
       except EnvironmentError:
          print "Error reading Registry"
    if sys.platform[:3] != 'win':
       try:
          logdir = os.path.join(os.getenv('HOME'),'sdb','loader','log') 
          if os.path.isdir(logdir) == False:
             os.makedirs  (logdir)
       except:
          pass
    check_options(options)
    
    if exist_db (options) == 'true':
       if options.mode == 'INTER':
          input    = raw_input('Drop      Database ? [y]/[n] : ')
       if options.mode == 'PARAM':
          input  = "y"
       if input == "y":   
          drop_db  (options) 
    
    if options.mode == 'INTER':
       input    = raw_input('Create    Database ? [y]/[n] : ')
    if options.mode == 'PARAM':
       input  = "y"
    if input == "y":   
       create_db (options)
          
    if options.mode == 'INTER':
       input    = raw_input('Backup    Database ? [y]/[n] : ')
    if options.mode == 'PARAM':
       input  = "y"
    if input == "y":   
       backup_db (options) 
               
def exist_db (options):
    dbexist = 'false'  
    try: 
        session = dbm.DBM (options.node, '', options.instroot) 
        dbinstances = re.split('\n',session.cmd('db_enum'))  
              
        for dbinstance in dbinstances :
            parameter = re.split('\t',dbinstance)
            if parameter[0] == options.dbname : #and parameter[1] == string.upper(options.instroot):
               dbexist = 'true' 
        session.release()        
    except dbm.CommunicationError, err :
         raise dbm.CommunicationError, err, sys.exc_traceback
    except dbm.DBMServError, err:
         raise dbm.DBMServError, err , sys.exc_traceback 
    except:
         kind, val, traceback = sys.exc_info()
         raise kind, val, traceback            
           
    return dbexist
    
def drop_db (options):
    try:
        session = dbm.DBM (options.node, options.dbname, options.instroot, options.dbmuser )
        dbmcmd ( options, session, 'db_stop'   , 'Stop      DB')
        dbmcmd ( options, session, 'db_drop'   , 'Drop      DB')
        session.release() 
    except dbm.CommunicationError, err :
        raise dbm.CommunicationError, err, sys.exc_traceback
    except dbm.DBMServError, err:
        raise dbm.DBMServError, err , sys.exc_traceback 
    except:
        kind, val, traceback = sys.exc_info()
        raise kind, val, traceback           
    
def create_db (options):
    try:
        session =  dbm.DBM (options.node, '', options.instroot)   
        version =  checkVersion (session, options)
        
        dbmcmd ( options, session, 'db_create %s %s %s' % (options.dbname, options.dbmuser, options.osuser), 'Create    DB')
        if version < (7, 2, 0):
            session = dbm.DBM (options.node, options.dbname, '', options.dbmuser)

        dbmcmd ( options, session, 'param_startsession',                 'Start     Session')
        dbmcmd ( options, session, 'param_init '                       , 'Init      Param'  )
        dbmcmd ( options, session, 'param_put MAXUSERTASKS  10'        , 'Put       Param'  )
        dbmcmd ( options, session, 'param_put CACHE_SIZE  5000'        , 'Put       Param'  )

        if  (string.upper(options.charencoding[0:5]) == 'ASCII'   ) :
             dbmcmd ( options, session, 'param_put _UNICODE     NO'         , 'Put       Param Unicode No'   )
             dbmcmd ( options, session, 'param_put DEFAULT_CODE ASCII'      , 'Put       Param DefaultCode'  ) 
                         
        if  (string.upper(options.charencoding[0:7]) == 'UNICODE' ) :
             dbmcmd ( options, session, 'param_put _UNICODE     YES'        , 'Put       Param Unicode Yes'  )
             dbmcmd ( options, session, 'param_put DEFAULT_CODE UNICODE'    , 'Put       Param DefaultCode'  )
             
        dbmcmd ( options, session, "param_checkall");
        dbmcmd ( options, session, "param_commitsession");

        if version < (7, 4, 0):
           dbmcmd ( options, session, 'param_adddevspace 1 SYS  sys_001  F'       ,'Add       SYS  Volume')
           dbmcmd ( options, session, 'param_adddevspace 1 LOG  LOG_001  F 3000 ' ,'Add       LOG  Volume');
           dbmcmd ( options, session, 'param_adddevspace 1 DATA DAT_001  F 12000 ','Add       DATA Volume');
        if version > (7, 4, 0):
           dbmcmd ( options, session, 'param_addvolume   1 LOG  LOG_001  F 3000 ' ,'Add       LOG  Volume');
           dbmcmd ( options, session, 'param_addvolume   1 DATA DAT_001  F 12000 ','Add       DATA Volume');

        if version < (7, 4, 4):
           dbmcmd ( options,  session, 'db_reg   -f'   ,                              'Register  Fast')
           dbmcmd ( options,  session, 'db_start -fast',                              'Start     Offline')
           dbmcmd ( options,  session, 'util_connect')
           dbmcmd ( options,  session, 'util_execute INIT CONFIG',                    'Init      Configuration')
           dbmcmd ( options,  session, 'util_activate %s' % (options.dbauser ),       'Init      DATA Volume')
           dbmcmd ( options,  session, 'util_release')
        
        if version >= (7, 4, 4):
           dbmcmd ( options,  session, 'db_start -fast',                              'Start     Offline')
           dbmcmd ( options,  session, 'db_activate %s'   % (options.dbauser ),       'Init      DATA Volume')
        
        dbmcmd   ( options,  session, 'util_execute SET LOG AUTO OVERWRITE ON ',     'Init      LOG Overwrite')
        dbmcmd   ( options,  session, 'util_release')
        dbmcmd   ( options,  session, 'db_online ',                                  'Start     Online')
        dbmcmd   ( options,  session, 'load_systab  -u %s -ud domp'    % (options.dbauser),                             'Load      Systemtables')
        
        if version >= (7, 5, 0) and version < (7, 6, 0) :
           dbmcmd ( options,  session, 'dbm_configset RunEventDispatcher yes'     ,'Start     EventDispatcher')
           dbmcmd ( options,  session, 'db_restart -fast',                         'Start     Online')
        #if version >= (7, 6, 0):
        #  dbmcmd ( options,  session, 'auto_extend ON'                            ,'Start     EventDispatcher')

        if(socket.gethostname() == 'p66796' or
           socket.gethostname() == 'P76209' or
           socket.gethostname() == 'p47579'   ):
           nsr_env_file  = os.path.join(options.instroot, 'nsr.txt')
           nsr_env = open( nsr_env_file, 'w')
           nsr_env.write ('NSR_HOME        "%s"\n' % os.path.join('C:\Program Files','nsr','bin'))
           nsr_env.write ('NSR_HOST        "%s"\n' % socket.gethostname() )
           nsr_env.write ('NSR_IGNOREERROR " using unlocked access"\n')
           nsr_env.write ('NSR_IGNOREERROR " unknown error 109 (0x6d)"\n')
           nsr_env.close()
           dbmcmd    ( options,  session, 'dbm_configset -raw NSR_ENV %s ' % nsr_env_file  , 'Set       Configuration NSR')
        if version < (7, 2, 0):
            dbm2cmd (session, 'Restart serverdb', 'db_restart')
        
        session.release() 
    except dbm.CommunicationError, err :
        raise dbm.CommunicationError, err, sys.exc_traceback
    except dbm.DBMServError, err:
        raise dbm.DBMServError, err , sys.exc_traceback 
    except:
        kind, val, traceback = sys.exc_info()
        raise kind, val, traceback           
            
def backup_db(options):
    try:
        indepdatapath  = None
        loaderdatapath = None
        tapedatapath   = None
        session = dbm.DBM (options.node, options.dbname, options.instroot, options.dbmuser )
        if sys.platform[:3] == 'win': 
           import _winreg
           try:
               #indepdatapath = os.path.join (_winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"HOMEDRIVE")[0], _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"HOMEPATH")[0])
               indepdatapath  = _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders"),"Personal")[0]
               tapedatapath   = os.path.join (indepdatapath, 'sdb','loader','tape', options.node, options.dbname)
           except EnvironmentError:
               pass  
        if sys.platform[:3] != 'win': 
           indepdatapath_ = session.cmd('dbm_getpath IndepDataPath ')
           indepdatapath  = re.split('\n',indepdatapath_)[0]
           tapedatapath   = os.path.join(indepdatapath,'wrk', options.dbname, 'tape', options.dbname)
           
        if  os.path.isdir(tapedatapath) == False:   
            os.makedirs  (tapedatapath)
           
        version = checkVersion (session, options)
        if version >= (7, 4, 3):
           backupdata  = '%s'    % os.path.join(tapedatapath,options.dbname)
           backuplog   = '%slog' % os.path.join(tapedatapath,options.dbname)
        
        if  os.path.isfile(backupdata):
            os.unlink(backupdata)
        if  os.path.isfile(backuplog):
            os.unlink(backuplog)
            
        dbmcmd ( options,  session, 'user_logon %s '                  % (options.dbmuser),                                 'Logon     DBM' ) 
        dbmcmd ( options,  session, 'medium_put %slog "%s" FILE LOG'  % (options.dbname,backuplog),                        'Put       LOG  Medium')
        dbmcmd ( options,  session, 'medium_put %s    "%s" FILE DATA' % (options.dbname,backupdata),                       'Put       DATA Medium ')
        dbmcmd ( options,  session, 'util_connect',                                                                        'Connect   Utility')
        dbmcmd ( options,  session, 'backup_start %s'                 % (options.dbname),                                  'Start     Backup')
        #dbmcmd ( options,  session, 'autolog_on %slog '               % (options.dbname),                                  'AutoLog   On ')
        dbmcmd ( options,  session, 'util_release',                                                                        'Release   Utility')    
        session.release() 
    except dbm.CommunicationError, err :
         raise dbm.CommunicationError, err, sys.exc_traceback
    except dbm.DBMServError, err:
         raise dbm.DBMServError, err , sys.exc_traceback 
    except:
         kind, val, traceback = sys.exc_info()
         raise kind, val, traceback           
        
def restore_db(options):
    try:
        session =  dbm.DBM (options.node, '', options.instroot)   
        indepdatapath_ = session.cmd('dbm_getpath IndepDataPath ')
        indepdatapath  = re.split('\n',indepdatapath_)[0]
        if version > (7, 4, 3):
           backupdata  = '%s'    % os.path.join (indepdatapath, 'wrk', options.dbname, 'LOADER', options.dbname)
           backuplog   = '%slog' % os.path.join (indepdatapath, 'wrk', options.dbname, 'LOADER', options.dbname)
           restoredata = '%s'    % os.path.join (indepdatapath, 'wrk', options.dbname, 'LOADER', options.dbname)
        if version <= (7, 4, 3):
           backupdata  = '%s'    % os.path.join (indepdatapath, 'wrk', options.dbname)
           backuplog   = '%slog' % os.path.join (indepdatapath, 'wrk', options.dbname)
           restoredata = '%s'    % os.path.join (indepdatapath, 'wrk', options.dbname) 
               
        if  os.path.isfile(backupdata):
            os.unlink(backupdata)
        if  os.path.isfile(backuplog):
            os.unlink(backuplog)
            
        dbmcmd ( options,  session, 'db_create %s %s %s' % (options.dbname, options.dbmuser, options.osuser),              'Create    DB')
        dbmcmd ( options,  session, 'db_reg   -f',                                                                         'Register  Fast')
        dbmcmd ( options,  session, 'backup_media_put data %s FILE DATA' % ( restoredata ),                                'Put       DATA Medium ')   
        dbmcmd ( options,  session, 'service_connect')
        dbmcmd ( options,  session, 'backup_restore_config data',                                                          'Restore   Backup')
        dbmcmd ( options,  session, 'param_startsession ')
        dbmcmd ( options,  session, 'param_put rundirectory %s' % os.path.join(options.instroot,  'wrk', options.dbname),  'Put       Param RunDir' ) 
        dbmcmd ( options,  session, 'param_checkall ')
        dbmcmd ( options,  session, 'param_commitsession')
        dbmcmd ( options,  session, 'service_release')
        dbmcmd ( options,  session, 'db_cold  -f',                                                                         'Start     Offline')
        dbmcmd ( options,  session, 'util_connect')
        dbmcmd ( options,  session, 'util_execute INIT CONFIG',                                                            'Init      Configuration')
        dbmcmd ( options,  session, 'util_release')
        dbmcmd ( options,  session, 'util_connect')
        dbmcmd ( options,  session, 'recover_start data DATA',                                                             'Start     Recovery') 
        dbmcmd ( options,  session, 'util_release')
        dbmcmd ( options,  session, 'db_warm',                                                                             'Start     Online') 
        dbmcmd ( options,  session, 'medium_put %slog "%s" FILE LOG'  % (options.dbname,backuplog),                        'Put       LOG  Medium')
        dbmcmd ( options,  session, 'medium_put %s    "%s" FILE DATA' % (options.dbname,backupdata),                       'Put       DATA Medium ')
        dbmcmd ( options,  session, 'util_connect')
        dbmcmd ( options,  session, 'backup_start %s'                 % (options.dbname),                                  'Start     Backup')
        #dbmcmd ( options,  session, 'autolog_on %slog '               % (options.dbname),                                  'AutoLog   On ')
        dbmcmd ( options,  session, 'util_release')
        dbmcmd ( options,  session, 'load_systab -u %s -ud domp'      % (options.dbauser),                                 'Load      Systemtables')
        session.release() 
    except dbm.CommunicationError, err :
         raise dbm.CommunicationError, err, sys.exc_traceback
    except dbm.DBMServError, err:
         raise dbm.DBMServError, err , sys.exc_traceback 
    except:
         kind, val, traceback = sys.exc_info()
         raise kind, val, traceback           
    
def fatalError (msg):
    sys.stderr.write ('\ninstalldb.py: ' + msg + '\n')
    sys.exit (3)

def show (options,comment):
    if  options.mode == 'INTER':    
        if comment:
           print '%-30s' % (comment + '...'),

def getLines (fname):
    result = []
    if os.path.exists (fname):
        data = open (fname, 'r').read ()
        result = string.split (data, '\n')
    return result

def dbmcmd ( options, session, cmd, comment = None):
    global LOADERAPPLPATH
    if sys.platform[:3] == 'win': 
       import _winreg
       LOADERAPPLPATH = _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"APPDATA")[0]
    if sys.platform[:3] != 'win': 
       LOADERAPPLPATH = os.environ['HOME']
    protfile  = os.path.join(LOADERAPPLPATH, 'sdb','loader','log', 'install.prt')
    log = open( protfile, 'a')
    try:
        log.write (cmd + '\n')
        try:
            show (options,comment)
            result = session.cmd (cmd)
            log.write ('//' + result + '\n')
            if comment:
               if options.mode == 'INTER': 
                  print 'OK'
        except dbm.DBMServError, err:
            log.write (str (err.additionalInfo) + '\n')
            fatalError ('ERR at DBMServer command ' + cmd + ': ' + str (err.additionalInfo))
    finally:
        log.close ()
    return result

def dbm2cmd (session, comment, *cmds):
    show (comment)
    for cmd in cmds:
        dbmcmd ( options, session, cmd)
    if  options.mode == 'INTER':     
        print 'OK'

def checkVersion (session, options):
    show (options, 'Checking  version')
    versionString = dbmcmd ( options, session, 'version') #dbm_version
    match = re.search ('"(\d+)[.](\d+)[.](\d+)"', versionString)
    version = tuple (map (int, match.group (1, 2, 3)))
    if version < (7, 1, 3):
        fatalError ('ERR  Sorry, no support for version %d.%d.%d' % version)
    if options.mode == 'INTER':     
       print 'OK'
    return version    
        
if __name__ == '__main__':
    optlib.optMain2 (main, _options ())
