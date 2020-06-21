#    coding: utf-8
#
#    module:       import.py
#
#    responsible : d033893
#
#    special area: sdb.loader
#
#    description : 
#
#    prerequisite: 
#
#    remarks:  
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

#system
import sys
import os
import time
import optlib
import socket
#string
import string
import re
import tempfile
#dav
import httplib
import urllib
#sapdb
from   sdb import dbm,loader,sql

#usage  : davimport <source dir> <host:port> <target dir>
#default: davimport .webdav  p66796:85 /webdav

def _options ():
     return [
     ('mode', 'mode'      , ':', 'PARAM'                  , 'specifies demo  mode'),                 # PARAM,INTER,DEBUG
     #source
     ('s', 'sourceDir'    , ':', ''                       , 'specifies source directory'), 
     #target
     ('n', 'node'         , ':', socket.gethostname()     , 'specifies servernode'),
     ('r', 'instroot'     , ':', ''                       , 'specifies INSTROOT path'),
     ('d', 'dbname'       , ':', ''                       , 'specifies dav database'), 
     ('a', 'dbaUser'      , ':', 'DBA'                    , 'specifies sapdb sysdba user'),
     ('i', 'init'         , ':', 'data'                   , 'init sapdb'),                   
     ('h', 'host'         , ':', ''                       , 'specifies dav host        '), 
     ('t', 'targetDir'    , ':',  '/webdav'               , 'specifies target directory'),      
]

INDEPPROGRAMS   = ''
TESTPATH        = ''

def getEnvironment(options,environment):
    global INDEPPROGRAMS
    global TESTPATH
    if sys.platform[:3] == 'win': 
       import _winreg
       try:
           indepprog     = _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\SAP\\SAP DBTech"),"IndepPrograms") 
           INDEPPROGRAMS = os.path.join(indepprog[0] )
           TESTPATH      = os.path.join(indepprog[0],'lib','test')
       except EnvironmentError,err:
           INDEPPROGRAMS = os.environ['INSTROOT']
           TESTPATH      = os.path.join(os.environ['TEST'])
           os.putenv('DEVENV','TRUE')
    if sys.platform[:3] != 'win': 
       INDEPPROGRAMS = os.environ['INSTROOT']
       TESTPATH      = os.path.split(os.environ['JTEST_ROOT'])[0]
    if options.sourceDir == '':
       options.sourceDir  =  os.path.join (TESTPATH   ,'sdb','loader','demo')   
    environment.append(INDEPPROGRAMS)
    environment.append(TESTPATH)

def main (options, args):
    
    options.mode = string.upper(options.mode)

    sourceDir = options.sourceDir
    targetDir = options.targetDir
    
    environment = []
    getEnvironment(options,environment)
    
    init_dav_env      (options, environment)
    init_dav_service  (options, environment)
    init_dav_resources(options, environment)
    
def init_dav_service  (options, environment):
    input = None
    INDEPPROGRAMS = environment[0]
    TESTPATH      = environment[1]
    
    while input not in ["y","n"] and string.upper (options.init) == 'DATA':   
          if options.mode == 'INTER':
             input = raw_input('Init      DAV Service ?     [y]/[n] : ')
          if options.mode == 'PARAM':
             print             'Init      DAV Service '
             input  = "y"
          if input == "n": 
              pass          
        
          if input == "y": 
              cmd = 'net stop  sapdbwww'
              try:
                  os.system (cmd)
              except OSError, err:
                  print 'command failed:', err 
        
              try:
                  os.chdir (os.path.join (os.path.join (INDEPPROGRAMS,'pgm')))
              except OSError, err:
                  print 'command failed:', err  
    
              cmd = 'wahttp -u'
              try:
                 os.system (cmd)
              except OSError, err:
                 print 'command failed:', err  
        
              cmd = 'wahttp -i'
              try:
                 os.system (cmd)
              except OSError, err:
                  print 'command failed:', err  
            
              cmd = 'regedit /s %s' % os.path.join (TESTPATH,'sdb','dav','demo','dav_env.reg')
              try:
                  os.system (cmd)
              except OSError, err:
                  print 'command failed:', err         
    
              cmd = 'net start sapdbwww'
              try:
                 os.system (cmd)
              except OSError, err:
                 print 'command failed:', err 

def init_dav_env       (options, environment):
    indep_progpath = None
    TESTPATH = environment[1]
    try:
        session = dbm.DBM('',options.dbname,options.instroot,'DBM,DBM')
        sourceindepprogpath = session.cmd('dbm_getpath INDEPPROGPATH')
        indep_progpath       = re.split('\n',sourceindepprogpath )[0]
    except dbm.CommunicationError,err:
        pass

    wahttp_conf_file = os.path.join(indep_progpath,'pgm','wahttp.conf')
    wahttp_conf_     = open( wahttp_conf_file , 'r')
    wahttp_conf      = string.replace(wahttp_conf_.readline(),'\n','')
    wahttp_conf_.close()
    
    if  sys.platform == 'win32':
        dav_env_file  = os.path.join ( TESTPATH ,'sdb','dav','demo','dav_env.reg')
        dav_env       = open( dav_env_file, 'w')
        
        dav_env.write ('Windows Registry Editor Version 5.00\n')
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s]\n'                  % wahttp_conf )
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\COMServices]\n'      % wahttp_conf )
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\GeneralSettings]\n'  % wahttp_conf )
        dav_env.write ('"developerMode"="1"\n')
        dav_env.write ('"AdminUserName"="admin"\n')
        dav_env.write ('"AdminUserPassword"="init"\n')
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\Global]\n'           % wahttp_conf )
        dav_env.write ('"LogFile"    ="%s"\n'                       % string.replace(os.path.join(indep_progpath,'Log','wa.log')    ,'\\','\\\\'))
        dav_env.write ('"ConfLogFile"="%s"\n'                       % string.replace(os.path.join(indep_progpath,'Log','waconf.log'),'\\','\\\\'))
        dav_env.write ('"logWithInfo"="0"\n')
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\HTTP]\n'             % wahttp_conf )
        dav_env.write ('"Port"="85"\n')
        dav_env.write ('"Threading"="1"\n')
        dav_env.write ('"Browsable"="0"\n')
        dav_env.write ('"LogRequests"="1"\n')
        dav_env.write ('"DocumentRoot"="%s"\n'                      %  string.replace(os.path.join(indep_progpath,'Documents'),'\\','\\\\'))
        dav_env.write ('"DefaultDocument"="index.html"\n')
        dav_env.write ('"MimeTypeFile"="%s"\n'                      %  string.replace(os.path.join(indep_progpath,'config','mime.types'),'\\','\\\\'))
        dav_env.write ('"LogDirectory"="%s"\n'                      %  string.replace(os.path.join(indep_progpath,'Log'),'\\','\\\\'))
        dav_env.write ('"MaxThreads"="10"\n')
        dav_env.write ('"enableWebDAV"="1"\n')
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\Resources]\n'         % wahttp_conf )
        dav_env.write ('"documentRoot"="%s"\n'                       % string.replace(os.path.join(indep_progpath,'Documents','WARoot'),'\\','\\\\'))
        dav_env.write ('"MIMETypes"   ="%s"\n'                       % string.replace(os.path.join(indep_progpath,'config','mime.types'),'\\','\\\\'))
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\Services]\n'          % wahttp_conf )
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\Services\waecho]\n'   % wahttp_conf )
        dav_env.write ('"ExitFunction"="wd83Exit"\n')
        dav_env.write ('"InitFunction"="wd83Init"\n')
        dav_env.write ('"Library"="%s"\n'                            % string.replace(os.path.join(indep_progpath,'pgm','waecho.dll'),'\\','\\\\'))
        dav_env.write ('"LibraryType"="C"\n')
        dav_env.write ('"LogFile"="%s"\n'                            % string.replace(os.path.join(indep_progpath,'Log','waecho.log'),'\\','\\\\'))
        dav_env.write ('"ServiceFunction"="wd83Service"\n')
        dav_env.write ('"ServiceName"="waecho"\n')
        dav_env.write ('"SessionPool"=""\n')
        dav_env.write ('"serviceStart"="1"\n')
        dav_env.write ('"useFastCGIForCookiePath"="1"\n')
        dav_env.write ('"withSSL"="0"\n')
        dav_env.write ('"sslURL"=""\n')
        dav_env.write ('"webSessionTimeout"="0"\n')
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\Services\webdav]\n'   % wahttp_conf )
        dav_env.write ('"ExitFunction"="WDVHandlerExit"\n')
        dav_env.write ('"InitFunction"="WDVHandlerInit"\n')
        dav_env.write ('"Library"="%s"\n'                            % string.replace(os.path.join(indep_progpath,'pgm','wdvhandler.dll'),'\\','\\\\'))
        dav_env.write ('"LibraryType"="C"\n')
        dav_env.write ('"LogFile"="%s"\n'                            % string.replace(os.path.join(indep_progpath,'Log','webdav.log'),'\\','\\\\'))
        dav_env.write ('"ServiceFunction"="WDVHandlerService"\n')
        dav_env.write ('"ServiceName"="webdav"\n')
        dav_env.write ('"SessionPool"="WebDAV"\n')
        dav_env.write ('"serviceStart"="1"\n')
        dav_env.write ('"useFastCGIForCookiePath"="0"\n')
        dav_env.write ('"withSSL"="0"\n')
        dav_env.write ('"sslURL"=""\n')
        dav_env.write ('"webSessionTimeout"="0"\n')
        dav_env.write ('"IDXSearchLib"="%s"\n'                       % string.replace(os.path.join(indep_progpath,'pgm','sapdbxis'),'\\','\\\\'))
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\Services\webdbm]\n'   % wahttp_conf )
        dav_env.write ('"ExitFunction"="DBMWeb_Service_Exit"\n')
        dav_env.write ('"InitFunction"="DBMWeb_Service_Init"\n')
        dav_env.write ('"Library"="%s"\n'                            % string.replace(os.path.join(indep_progpath,'pgm','webdbm.dll'),'\\','\\\\'))
        dav_env.write ('"LibraryType"="CPP"\n')
        dav_env.write ('"LogFile"="%s"\n'                            % string.replace(os.path.join(indep_progpath,'Log','webdbm.log'),'\\','\\\\'))
        dav_env.write ('"ServiceFunction"="DBMWeb_Service_Service"\n')
        dav_env.write ('"ServiceName"="webdbm"\n')
        dav_env.write ('"SessionPool"=""\n')
        dav_env.write ('"serviceStart"="1"\n')
        dav_env.write ('"useFastCGIForCookiePath"="0"\n')
        dav_env.write ('"withSSL"="0"\n')
        dav_env.write ('"sslURL"=""\n')
        dav_env.write ('"webSessionTimeout"="0"\n')
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\Services\websql]\n'    % wahttp_conf )
        dav_env.write ('"ExitFunction"="WebSQLExit"\n')
        dav_env.write ('"InitFunction"="WebSQLInit"\n')
        dav_env.write ('"Library"="%s"\n'                             % string.replace(os.path.join(indep_progpath,'pgm','websql.dll'),'\\','\\\\'))
        dav_env.write ('"LibraryType"="CPP"\n')
        dav_env.write ('"LogFile"="%s"\n'                             % string.replace(os.path.join(indep_progpath,'Log','websql.log'),'\\','\\\\'))
        dav_env.write ('"ServiceFunction"="WebSQLService"\n')
        dav_env.write ('"ServiceName"="websql"\n')
        dav_env.write ('"SessionPool"=""\n')
        dav_env.write ('"withSSL"="0"\n')
        dav_env.write ('"sslURL"="https://your-server/websql"\n')
        dav_env.write ('"webSessionTimeout"="3200"\n')
        dav_env.write ('"serviceStart"="1"\n')
        dav_env.write ('"useFastCGIForCookiePath"="0"\n')
        dav_env.write ('"connectionPoolTimeout"="3200"\n')
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\SessionPools]\n'        % wahttp_conf )
        dav_env.write ('\n')
        dav_env.write ('[HKEY_LOCAL_MACHINE\%s\SessionPools\WebDAV]\n' % wahttp_conf )
        dav_env.write ('"serverDB"="%s"\n'                             % options.dbname )
        dav_env.write ('"serverNode"=""\n')
        dav_env.write ('"userId"="DBA"\n')
        dav_env.write ('"password"="DBA"\n')
        dav_env.write ('"datasource"=""\n')
        dav_env.write ('"driver"="MAXDB"\n')
        dav_env.write ('"autocommit"="Off"\n')
        dav_env.write ('"poolType"="ODBC"\n')
        dav_env.write ('"poolSize"="10"\n')
        dav_env.write ('"sqlTraceFilename"=""\n')
        dav_env.close()        

def init_dav_resources (options, environment ): 

    init_dav_index(options) 
    
    if string.upper (options.init) == 'DATA':
       try:
          if options.mode == 'INTER':
             input('\nInit      DAV Data')
          if options.mode == 'PARAM':
             print '\nInit      DAV Data'
             i = 1/0
       except:
          pass  
     
       init_dav_data(options.host,options.sourceDir,options.targetDir)   
    
def init_dav_data(host,sourceDir,targetDir):
    for file in os.listdir(sourceDir):
        sourceFile = os.path.join(sourceDir, file)
        if os.path.isdir(sourceFile) == 1:
           MkCol(host, urllib.quote(file), targetDir )
           init_dav_data(host, sourceFile, targetDir + '/' + urllib.quote(file))
        else:
           Put  (host, sourceFile, targetDir )
   
def init_dav_index(options):
    #delete
    if  string.upper(options.init) == 'SYSTEM':
        try:
            session = sql.connect ( options.dbaUser, options.dbaUser, options.dbname, options.node ) 
        except sql.CommunicationError , err :
            print 'command failed:' , err

        cmd = 'DELETE FROM XML_ASSIGN_DC_IDX'
        try:    
            session.sql(cmd)
        except sql.SQLError, err :
            print 'command failed:' , err
        cmd = 'DELETE FROM XML_DOCUMENTCLASS'    
        try:        
            session.sql(cmd)
        except sql.SQLError, err :
            print 'command failed:' , err
        cmd = 'DELETE FROM XML_INDEXENGINE'    
        try:                   
            session.sql(cmd)
        except sql.SQLError, err :
            print 'command failed:' , err    
        cmd = 'DELETE FROM XML_XMLINDEX'    
        try:                   
            session.sql(cmd)
        except sql.SQLError, err :
            print 'command failed:' , err
        cmd = 'DELETE FROM XML_INDEXINGSERVICEDESC'
        try:    
           session.sql(cmd)
        except sql.SQLError, err :
           print 'command failed:' , err
        cmd = 'DELETE FROM XML_SESSIONPOOL'    
        try:        
           session.sql(cmd)
        except sql.SQLError, err :
           print 'command failed:' , err
        cmd = 'DELETE FROM XML_INDEXINGLIB'    
        try:                   
           session.sql(cmd)
        except sql.SQLError, err :
           print 'command failed:' , err
        cmd = 'DELETE FROM XML_SEARCHLIB'    
        try:                   
           session.sql(cmd)
        except sql.SQLError, err :
           print 'command failed:' , err
        #dav
        cmd = 'DELETE FROM WEBDAV_CONTAINER'
        try:    
            session.sql(cmd)
        except sql.SQLError, err :
            print 'command failed:' , err
        cmd ='DELETE FROM WEBDAV_INODE'
        try:    
            session.sql('DELETE FROM WEBDAV_INODE')
        except sql.SQLError, err :
            print 'command failed:' , err    
        cmd = 'DELETE FROM WEBDAV_NAME_SPACE'    
        try:                   
            session.sql(cmd)
        except sql.SQLError, err :
            print 'command failed:' , err
        cmd = 'DELETE FROM WEBDAV_PROPERTY'    
        try:                   
            session.sql(cmd)
        except sql.SQLError, err :
            print 'command failed:' , err
        cmd = 'DELETE FROM WEBDAV_PROPERTY_MANAGEMENT'    
        try:                   
            session.sql(cmd)
        except sql.SQLError, err :
            print 'command failed:' , err
       
        session.commit () 
        session.release()
    
    #index   
    if  string.upper(options.init) == 'DATA':        
        try:
            session = loader.Loader ( options.node, '' ) 
        except loader.CommunicationError , err :
            print 'command failed:' , err
        
        try:
            session.cmd( ' USE USER %s %s SERVERDB %s ON %s '  % ( options.dbaUser, options.dbaUser, options.dbname, options.node ))
        except loader.LoaderError, err: 
            print 'command failed:' , err   
        
        dataLoad (session, """
                 LOAD DATA TABLE "XML_ASSIGN_DC_IDX"
                                 "DCID"       1 CHAR
                                 "IDXID"      2 CHAR
                 INSTREAM FILE *
                 COMPRESSED ASCII
                 DECIMAL '//./' 
                 DATE ISO 
                 TIME ISO 
                 TIMESTAMP ISO 
                 NULL '?                   ' 
                 BOOLEAN 'TRUE/FALSE'
                 ""","""
"060000000F0EC0F46406000054420000AB7FBF2ABEFFF2AC","01000000170DC0F46406000054420000AB7FBF2ABEFFF2AC"
"060000000F0EC0F46406000054420000AB7FBF2ABEFFF2AC","020000005D0DC0F46406000054420000AB7FBF2ABEFFF2AC"
"060000000F0EC0F46406000054420000AB7FBF2ABEFFF2AC","03000000880DC0F46406000054420000AB7FBF2ABEFFF2AC"
"060000000F0EC0F46406000054420000AB7FBF2ABEFFF2AC","04000000C40DC0F46406000054420000AB7FBF2ABEFFF2AC"
"060000000F0EC0F46406000054420000AB7FBF2ABEFFF2AC","05000000F70DC0F46406000054420000AB7FBF2ABEFFF2AC"
"060000000F0EC0F46406000054420000AB7FBF2ABEFFF2AC","06000000C40DC0F46406000054420000AB7FBF2ABEFFF2AC"
"060000000F0EC0F46406000054420000AB7FBF2ABEFFF2AC","07000000F70DC0F46406000054420000AB7FBF2ABEFFF2AC"             
""")

        dataLoad (session, """
                  LOAD DATA TABLE "XML_DOCUMENTCLASS" 
                  "DCID"        1 CHAR 
                  "NAME"        2 CHAR 
                  "DESCRIPTION" 3 CHAR DEFAULT NULL 
                  INSTREAM FILE *
                  COMPRESSED ASCII 
                  DECIMAL '//./' 
                  DATE ISO 
                  TIME ISO 
                  TIMESTAMP ISO 
                  NULL '?                   ' 
                  BOOLEAN 'TRUE/FALSE'   
              ""","""
"060000000F0EC0F46406000054420000AB7FBF2ABEFFF2AC","OIM:Catalog",""
""")
        
        dataLoad (session, """
        LOAD DATA TABLE "XML_XMLINDEX" 
        "IDXID"       1 CHAR 
        "NAME"        2 CHAR DEFAULT NULL 
        "DESCRIPTION" 3 CHAR DEFAULT NULL 
        "XPATHBASE"   4 CHAR DEFAULT NULL 
        "XPATHVALUE"  5 CHAR DEFAULT NULL 
        INSTREAM FILE *
        COMPRESSED ASCII 
        DECIMAL '//./' 
        DATE ISO 
        TIME ISO 
        TIMESTAMP ISO 
        NULL '?                   ' 
        BOOLEAN 'TRUE/FALSE'    
        ""","""
"01000000110DC0F46406000054420000AB7FBF2ABEFFF2AC","Connection","","/oim:Transfer/uml:System/uml:SystemModels/uml:Model/uml:ModelPackages/dbm:ConnectionSet/dbm:ConnectionSetConnections","/sdbm:Connection/@dbm:ConnectString"
"020000005D0DC0F46406000054420000AB7FBF2ABEFFF2AC","Catalog","","/oim:Transfer/uml:System/uml:SystemModels/uml:Model/uml:ModelPackages/dbm:DataSource/dbm:DataSourceDeployedCatalogs","/dbm:DeployedCatalog/@name"
"03000000880DC0F46406000054420000AB7FBF2ABEFFF2AC","Schema","","/oim:Transfer/uml:System/uml:SystemModels/uml:Model/uml:ModelPackages/dbm:DataSource/dbm:DataSourceDeployedCatalogs/dbm:DeployedCatalog/dbm:CatalogSchemas/uml:NamespaceOwnedElement","/sdbm:DeployedSchema/@oim:id"
"04000000C40DC0F46406000054420000AB7FBF2ABEFFF2AC","Table","","/oim:Transfer/uml:System/uml:SystemModels/uml:Model/uml:ModelPackages/dbm:DataSource/dbm:DataSourceDeployedCatalogs/dbm:DeployedCatalog/dbm:CatalogSchemas/uml:NamespaceOwnedElement/sdbm:DeployedSchema/uml:NamespaceOwnedElement","/sdbm:DeployedTable/@oim:id"
"05000000F70DC0F46406000054420000AB7FBF2ABEFFF2AC","View","","/oim:Transfer/uml:System/uml:SystemModels/uml:Model/uml:ModelPackages/dbm:DataSource/dbm:DataSourceDeployedCatalogs/dbm:DeployedCatalog/dbm:CatalogSchemas/uml:NamespaceOwnedElement/sdbm:DeployedSchema/uml:NamespaceOwnedElement","/dbm:DeployedView/@oim:id"
"06000000C40DC0F46406000054420000AB7FBF2ABEFFF2AC","Synonym","","/oim:Transfer/uml:System/uml:SystemModels/uml:Model/uml:ModelPackages/dbm:DataSource/dbm:DataSourceDeployedCatalogs/dbm:DeployedCatalog/dbm:CatalogSchemas/uml:NamespaceOwnedElement/sdbm:DeployedSchema/uml:NamespaceOwnedElement","/dbm:TableSynonym/@oim:id"
"07000000F70DC0F46406000054420000AB7FBF2ABEFFF2AC","Column","","/oim:Transfer/uml:System/uml:SystemModels/uml:Model/uml:ModelPackages/dbm:DataSource/dbm:DataSourceDeployedCatalogs/dbm:DeployedCatalog/dbm:CatalogSchemas/uml:NamespaceOwnedElement/sdbm:DeployedSchema/uml:NamespaceOwnedElement/sdbm:DeployedTable/dbm:ColumnSetColumns/uml:ClassifierFeature","/dbm:DeployedColumn/@oim:id"
""") 
    
def MkCol(host, sourceDir,targetDir):
    print('MKCOL with http://' + host + targetDir + '/' + sourceDir)
    h = httplib.HTTP(host)
    h.putrequest('MKCOL', targetDir + '/' + sourceDir)
    h.putheader ('Host' , host)
    h.endheaders()
    errcode, errmsg, headers = h.getreply()
    if errcode != 201:
       print(errmsg)
       print('<host:port>.......:  '   + host)
       print('<Target Dir>.:  '        + targetDir)
       print('<Source Dir>..........:' + sourceDir)
       print('\n')

def Put(host, path, targetDir ):
    myFile = urllib.quote(os.path.basename(path))
    input = open(path, 'rb')
    data  = input.read()
    input.close()

    h = httplib.HTTP(host)
    h.putrequest('PUT', targetDir + '/' + myFile)
    h.putheader ('Host', host)
    h.putheader ('Content-Type','text/plain')
    h.putheader ('Content-Length',str(len(data)))
    xml = re.split  ('\.',myFile)[1]
    if xml == 'xml':
       h.putheader('SAPDB_DocumentClass','OIM:Catalog')
    h.endheaders()
    h.send(data)
    errcode, errmsg, headers = h.getreply()
    if errcode != 201 and errcode != 204:
       print(errmsg)
       print('<host:port>.......:  ' + host)
       print('<Target Dir>.:  ' + targetDir)
       print('<File>............:  ' + path)
       xml = ''

def loaderCmd (session, cmdStr):
    try:
        result = session.cmd (cmdStr)
    except loader.LoaderError, err:
        raise loader.LoaderError, err, sys.exc_traceback
    except:
        kind, val, traceback = sys.exc_info()
        raise kind, val, traceback       
               
def dataLoad (session, loadcmd, data = None):
    if data != None:
        # write data to tmpfile
        tmpname = tempfile.mktemp ()
        stream = open (tmpname, 'w')
        stream.write (data)
        stream.close ()
        # patch load cmd
        match = re.search ('INSTREAM FILE +[*]', loadcmd)
        if match != None:
            start, end = match.span ()
            loadcmd = loadcmd [:start] + ("INSTREAM FILE '%s'" % tmpname) + loadcmd [end:]
    # execute command
    loaderCmd (session, loadcmd)
    # clean up
    if data != None:
       os.unlink (tmpname)                      
    
if __name__ == '__main__':
   optlib.optMain2 (main, _options () )
