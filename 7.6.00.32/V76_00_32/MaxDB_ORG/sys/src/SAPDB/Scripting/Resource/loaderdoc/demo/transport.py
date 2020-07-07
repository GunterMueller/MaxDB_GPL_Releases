#    coding: utf-8
#
#    module:       transport.py
#
#    version:      7.6.00
#
#    responsible : d033893
#
#    special area: loader
#
#    description : transport, export or import sdb data/metadata using the 
#                  transformation service provided by sdb loader. 
#                  imports, executes and validates default or userdefined
#                  transformation packages via generation and execution of  
#                  sdb loader statements from the relational model of 
#                  sdb database instances.
#
#    prerequisite: install sdb and std python libraries
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your options) any later version.
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

try:
    #system
    import  os
    import  sys
    import  stat
    import  time
    import  socket
    import  signal
    import  shutil
    import  thread
    mutex = thread.allocate_lock()
    import  getpass
    #environment
    import  ConfigParser
    #string
    import  string
    import  codecs
    import  re
    #csv parser
    import  csv
    #xml parser
    from    xml             import sax
    #dom
    from    xml.dom.minidom import parse
    #archive
    import  zlib
    import  zipfile
    import  tarfile
    #sdb
    import  optlib
    from    sdb             import dbm,loader,sql
    if sys.platform[:3]  == 'win':
       import _winreg
except ImportError, err :
    print err
    print sys.path
'''
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     |
|                                             Loader transport parameter                                                              |
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     |
| ExportID ImportID    Transport Type      LoggingPosition       Package-Part    Medium         Format Export-Import  CharEncoding Lob|
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     |
|                                            Loader Session Parameter                                                                 |
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     |
|  Session Attribute       Parameter Value                                 Default Value                                              |
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     | 
|  SET CONFIGURATION       [EXCLUDE|INCLUDE]                               EXCLUDE                                                    | 
|  SET COMPRESSED          \'/<s>/<d>/\'                                   \'/,/"/\'                                                  |
|  SET SQLMODE             [INTERNAL|ANSI|DB2|ORACLE]                      INTERNAL                                                   | 
|  SET LANGUAGE            [ENG|DEU]                                       \'ENG\'                                                    | 
|  SET CODETYPE            [ASCII|UCS2|UTF8]                               ASCII                                                      |
|  SET CODEPAGE            [http://www.unicode.org/Public/MAPPINGS/]       ISO-8859-1                                                 |
|  SET DATE                [INTERNAL|ISO|USA|EUR|JIS]                      ISO                                                        |
|  SET TIME                [INTERNAL|ISO|USA|EUR|JIS]                      ISO                                                        |
|  SET TIMESTAMP           [INTERNAL|ISO|USA|EUR|JIS]                      ISO                                                        |
|  SET DECIMAL             \'/[<t>]/<d>/\'                                 \'//./\'                                                   |
|  SET BOOLEAN                                                             \'TRUE/FALSE\'                                             |
|  SET NULL                                                                \'?                   \'                                   |
|  SET TRANSACTION SIZE                                                    1000                                                       |
|  SET ISOLATION LEVEL                                                     3                                                          |
|  SET MAXERRORCOUNT                                                       100                                                        |
|  SET PACKAGETIMEOUT                                                      1000                                                       |
|  SET BACKUPTOOL          [TSM|NETWORKER|BACKINT]                         NETWORKER                                                  | 
|  SET ODBCTRACE           [ON|OFF]                                        OFF                                                        |
|  SET CHECKCONSTRAINT     [ON|OFF]                                        OFF                                                        |
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     |
|                                             Loader transformation parameter values                                                  |
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
|                    |                    |                 |                |               |                       |                |
| What should be     | Transformation of  |  Logging of     | Package of     | Medium of     | Format of             | Mapping of     |
| exported/imported ?| Models und Data    |  Transformation | Transformation | Staging Area  | Models and Data       | Char Encodings |
|                    |                    |                 |                |               |                       |                |
|-------------------------------------------------------------------------------------------------------------------------------------|
|Catalog, Data and   |EXPORT|IMPORT       |[RESTORE]        | DB             |               |                       |                |
|Package of a        |CONFIGURATION       |                 |                | FILE          | DTL, XMI              | UCS2           |
|Database            |CATALOG             |                 |                | PIPE          | DDL, XMI              | UTF8           |
|                    |DATA                |                 |                | TAPE          | PAGES, RECORDS        | ASCII          |
|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              | DYNAMIC        |
|-------------------------------------------------------------------------------------------------------------------------------------|
|Catalog, Data and   |EXPORT|IMPORT       |[RESTORE]        | USER           |               |                       |                |
|Package of a        |CONFIGURATION       |                 |                | FILE          | DTL, XMI              | UCS2           |
|Database User       |CATALOG             |                 |                | PIPE          | DDL, XMI              | UTF8           |
|                    |DATA                |                 |                | TAPE          | PAGES, RECORDS        | ASCII          |
|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              | DYNAMIC        |
|-------------------------------------------------------------------------------------------------------------------------------------|
|Catalog, Data and   |EXPORT|IMPORT       |[RESTORE]        | SCHEMA         |               |                       |                |
|Package of a        |CONFIGURATION       |                 |                | FILE          | DTL, XMI              | UCS2           |
|Database Schema     |CATALOG             |                 |                | PIPE          | DDL, XMI              | UTF8           |
|                    |DATA                |                 |                | TAPE          | PAGES, RECORDS        | ASCII          |
|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              | DYNAMIC        |
|-------------------------------------------------------------------------------------------------------------------------------------|
|Catalog, Data and   |EXPORT|IMPORT       |[RESTORE]        | TABLE          |               |                       |                |
|Package of a        |CONFIGURATION       |                 |                | FILE          | DTL, XMI              | UCS2           |
|Database Table      |CATALOG             |                 |                | PIPE          | DDL, XMI              | UTF8           |
|                    |DATA                |                 |                | TAPE          | PAGES, RECORDS,       | ASCII          |
|                    |                    |                 |                |               | CSV, FWV              | DYNAMIC        |
|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              |                |
|-------------------------------------------------------------------------------------------------------------------------------------|
|Data and Package    |EXPORT|IMPORT       |                 | COLUMNS FIELDS |               |                       |                |
|of a qualified      |                    |                 |                | FILE          |                       | UCS2           |
|Database Columnset  |                    |                 |                | PIPE          |                       | UTF8           |
|                    |DATA                |                 |                | TAPE          | CSV, FWV              | ASCII          |
|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              | DYNAMIC        |
|                    |                    |                 |                |               |                       |                |
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     |
|                                            Loader transport options                                                                 |
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
'''		
options =[
# 0 ###### 1 #################### 2 ## 3 ###################### 4 ########################
('mode'  ,'mode'               , ':', ''       , 'specifies mode                     '),# 0# PARAM,INTER,DEBUG
('',     'packagesource'       , ':', 'HISTORY', 'packagesource                      '),# 1# HISTORY, DIRECTORY, REPOSITORY
('',     'packagerecord'       , ':', ''       , 'packagerecord                      '),# 2# 
('',     'packagenumber'       , ':', ''       , 'packagenumber                      '),# 3# 
('',     'sourcepackagepath'   , ':', ''       , 'specifies source packagepath       '),# 4# 
('',     'targetpackagepath'   , ':', ''       , 'specifies target packagepath       '),# 5#   
('',     'packagename'         , ':', ''       , 'packagename                        '),# 6# 
('',     'packageguid'         , ':', ''       , 'packageguid                        '),# 7# 
('',     'packagepath'         , ':', ''       , 'packagepath                        '),# 8# 
('',     'packageexecution'    , ':', ''       , 'packageexecution                   '),# 9#
('',     'packagecreation'     , ':', ''       , 'packagecreation                    '),#10#
('',     'packagecomment'      , ':', ''       , 'packagecomment                     '),#11# 
('T',    'transporttype'       , ':', ''       , 'type           of transport        '),#12# EXECUTE, GENERATE
('t',    'transportdirection'  , ':', ''       , 'type           of transformation   '),#13# IMPORT , EXPORT, TRANSPORT
('p',    'packageref'          , ':', ''       , 'packageref     of transformation   '),#14# DB     , USER,  SCHEMA,  TABLE
('P',    'packagepart'         , ':', ''       , 'packagepart    of transformation   '),#15# DB     , USER,  SCHEMA,  TABLE
('s',    'streamtypes'         , ':', ''       , 'streamtypes    of transformation   '),#16# CATALOG, DATA,  PACKAGE
('m',    'streammedium'        , ':', ''       , 'streammedium   of transformation   '),#17# PIPE   , FILE,  TAPE
('f',    'streamformat'        , ':', ''       , 'streamformat   of transformation   '),#18# RECORDS, PAGES, COMPRESSED, FORMATTED, DDL, XML
('F',    'streamformat_target' , ':', ''       , 'streamformat   of transformation   '),#19# PAGE   , ROW  , DDL, DTL
('c',    'streamcharencoding'  , ':', ''       , 'streamcoding   of transformation   '),#20# ASCII  , UCS2 , UTF8, DYNAMIC
('l',    'streamlob'           , ':', ''       , 'lobmask        of transformation   '),#21# TABLE  , COLUMN,COLUMNVALUE
('i',    'initposition'        , ':', 'START'  , 'initposition   of transformation   '),#22# START  , RESTART
('nSRC', 'sourcedbnode'        , ':', ''       , 'specifies source  database computer'),#23#
('dSRC', 'sourcedbname'        , ':', ''       , 'specifies source  database name    '),#24#
('uSRC', 'sourcedbuser'        , ':', ''       , 'specifies source  database user    '),#25#
('sSRC', 'sourcedbschema'      , ':', ''       , 'specifies source  database schema  '),#26#
('nTRG', 'targetdbnode'        , ':', ''       , 'specifies target  database computer'),#27#
('dTRG', 'targetdbname'        , ':', ''       , 'specifies target  database name    '),#28#
('uTRG', 'targetdbuser'        , ':', ''       , 'specifies target  database user    '),#29#
('sTRG', 'targetdbschema'      , ':', ''       , 'specifies target  database schema  '),#30#
('R',    'loaderinstroot'      , ':', ''       , 'specifies loader  instroot         '),#31# 
('X',    'migration'           , ':', 'sdb'    , 'specifies source  database system  '),#32# SDB, ADABAS, ORACLE
]
'''
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     |
|                                            Loader transport parameter                                                               |
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
|PACKAGEEXECUTION    |PACKAGENAME         |TRANPORTTYPE -T  |TRANSPORTDIR -t    |PACKAGEREF -p    |PART -P           |STREAMS -s      |      
|0                   |1                   |2                |3                  |4                |5                 |6               |  
|INITPOS -i          |MEDIUM  -m          |FORMAT       -f  |-F                 |CODING -c        |LOB -l            |ARCHIVE         |  
|7                   |8                   |9                |10                 |11               |12                |13              |  
|PACKAGEGUID         |EXPORTER_VERSION    |IMPORTER_VERSION |SOURCE_USERKEY     |TARGET_USERKEY   |SOURCE_NODE -nSRC |SOURCE_DB -dSRC | 
|14                  |15                  |16               |17                 |18               |19                |20              | 
|SOURCE_USER -uSRC   |SOURCE_SCHEMA-sSRC  |TARGET_NODE -nTRG|TARGET_DB -dTRG    |TARGET_USER-uTRG |TARGET_SCHEMA-sTRG|SOURCE_VERSION  |
|21                  |22                  |23               |24                 |25               |26                |27              |
|TARGET_VERSION      |PACKAGECOMMENT      |PACKAGECREATION  |PACKAGEPATH                                                              |
|28                  |29                  |30               |40                                                                       |
|-------------------------------------------------------------------------------------------------------------------------------------|
|                                                                                                                                     |
|                                            Loader transport configuration                                                           |
|                                                                                                                                     |
|-------------------------------------------------------------------------------------------------------------------------------------|
'''																																																																																																																			  
TESTLOADER              = 'FALSE'
#TRANFORMATION LOADER RESOURCES
INDEPPROGPATH           = ''
EXPORTER_VERSION        = ''
IMPORTER_VERSION        = ''
#TRANFORMATION DB RESOURCES
DB_INSTROOT_SOURCE      = ''
DB_INSTROOT_TARGET      = ''
DB_VERSION_SOURCE        = ''
DB_VERSION_TARGET       = ''
DB_INDEPDATAPATH_SOURCE = ''
DB_INDEPDATAPATH_TARGET = ''
DB_DEFAULTCODE_SOURCE   = 'ASCII'
DB_CODE_SOURCE          = 'ASCII'
DB_DEFAULTCODE_TARGET   = 'ASCII'
DB_CODE_TARGET          = 'ASCII'
DBA_SOURCE              = ''
DBA_TARGET              = ''
DB_SCHEMAS_SOURCE       = ''
DB_SCHEMAS_TARGET       = ''
#TRANSFORMATION CONFIGURATION
LOADERPROGPATH          = None
LOADERDATAPATH          = None
LOADERCONFIGFILE        = None
LOADERCONFIGURATION     = None
LOADERLOGHISTORY        = None
#TRANSFORMATION RUNTIME
PACKAGETIMEOUT          = 0
TRANSACTIONSIZE         = 1000
ISOLATIONLEVEL          = 3
MAXERRORCOUNT           = 150
AUTOCOMMIT              = 'ON'
ODBCTRACE               = 'OFF'
CHECKCONSTRAINT         = 'OFF'
#TRANSFORMATION PARAMETERS
PACKAGESOURCE           = 'HISTORY'
PACKAGEEXECUTION        = 'EXECUTE'
PACKAGETIMESTAMP        = ''
PACKAGEARCHIVE          = ''
PACKAGEPATH             = 'HOME'
PACKAGEPROTOCOL         = 'HOME'
PACKAGEFORMAT           = 'COMPRESSED'
PACKAGENAME             = 'TIMESTAMP'
PACKAGECOMMENT          = ''
VERBOSE                 = True
MONITOR                 = False
TRANSFORMATIONSYNTAX    = 'UNIFIED' 
CASESENSITIVE           = False
SYSTEMATTRIBUTE         = 'OFF'
STREAMCATALOG           = True
STREAMDATA              = True
STREAMDATA_DUPLICATES   = 'UPDATE DUPLICATES'
STREAMSTATISTICS        = False
STREAMPACKAGE           = True
FIELDCOMPRESSED         = '/,/\"/'
FIELDBOOLVALUE          = 'TRUE/FALSE'
FIELDDECIMALVALUE       = '///'
FIELDNULLVALUE          = '?                   '
FIELDATEVALUE           = 'ISO'
FIELDTIMEVALUE          = 'ISO' 
FIELDTIMESTAMPVALUE     = 'ISO' 
FIELDLOBMASK            = '####'
#TRANSFORMATION PACKAGE
EXISTSDBA               = "SELECT DISTINCT OWNER     FROM DOMAIN.USERS   WHERE USERMODE   LIKE 'SYSDBA'" 
EXISTSSCHEMA            = "SELECT SCHEMANAME         FROM DOMAIN.SCHEMAS WHERE SCHEMANAME    = '%s'"
MAXUSERTASKS            = ''
USEDUSERTASKS           = ''
DEFAULTCODE             = ''
CODE                    = ''
DISTINCTCODE            = "SELECT DISTINCT CODETYPE  FROM DOMAIN.COLUMNS WHERE OWNER = '%s' AND TABLENAME = '%s' AND CODETYPE <> ''"
PACKAGEIMPORT           = ''
PACKAGEMAXSTEP_SOURCE   = ''
PACKAGEMAXSTEP_TARGET   = ''
PACKAGEROWCOUNT_SOURCE  = ''
PACKAGEROWCOUNT_TARGET  = ''
STEPIDROWCOUNT_SOURCE   = ''
STEPIDROWCOUNT_TARGET   = ''
STEPIDSROWCOUNT_SOURCE  = ''
STEPIDSROWCOUNT_TARGET  = ''
PACKAGEVALIDATE         = ''
PACKAGETABLES           = ''
#TRANSFORMATION CONSTANTS
DATE                    = 10
DATEINTERNAL            =  8
TIME                    =  8
TIMEINTERNAL            =  8
TIMESTAMP               = 26
TIMESTAMPINTERNAL       = 20
SMALLINT                =  5
INTEGER                 = 10
HEXLENGTH               =  2
SIGN                    =  2
FLOAT                   = 16
FLOATMASK               =  6  
LONGPOSITION            = 21
STEPID                  =  0

def _options ():
     return options

def signal_handler(sig):
    if( sig == signal.SIGINT   or
        sig == signal.SIGTER   or
        sig == signal.SIGABR   or
        sig == signal.SIGBREAK   ):
        try:
           os._exit(0)
        except OSError, err:
           print err
           
def main (options, args):
    global       STEPID
    input        = None
    package      = None
    rc           = True
    repository   = TransformationPackageRepository(options)
    package      = repository.select()
    if package not in [None]:
       try:
          package.output()
          package  = TransformationPackage(options)
         #raw_input             ('STOP:REPOSITORY:SELECT  : %s ' % options.packageguid)
          if package.transformation not in [None]:
            #raw_input          ('STOP:PACKAGE:EXECUTE    : %s ' % package.packageguid)
             package.execute()
             package.release()
             package.archive()
            #raw_input          ('STOP:REPOSITORY:INSERT  : %s ' % options.packageguid)
             repository.insert()
       except EOFError:
          thread.interrupt_main()
       except EnvironmentError:
          thread.interrupt_main()
       except KeyboardInterrupt:
          thread.interrupt_main()
       except:
          sys.exc_traceback
    else:
      print 'Package %s does not exist in history' % options.packagename

class TransformationPackageRepository:
      def __init__(self, options = None ) :
          self.options           = options
          self.transportlog      = None
          self.init()
               
      def init(self): 
          global PACKAGEGUID 
          global PACKAGETIMESTAMP 
          PACKAGEGUID         = loader._createGUID()
          PACKAGETIMESTAMP    = time.strftime("%Y%m%d%H%M%S",time.localtime())
          self.options.packageexecution = PACKAGETIMESTAMP
          self.environ              ()
          self.package_record = dict()
          self.package_name   = dict()
          self.package_number = dict()
          
      def select(self):
          packagearchive = ''             
          packageref     = self.options.packageref
          package        = None
          
          if packageref not in ['']:
             if len(re.split(':',packageref)) == 1:
                 self.options.packageref     = string.strip(re.split(':',packageref)[0])
                 if string.upper(self.options.packageref) not in ['DB','USER','SCHEMA','TABLE']:
                    self.options.packagename = self.options.packageref
             if len(re.split(':',packageref))== 2:
                self.options.packageref    = string.strip(re.split(':',packageref)[0])                  
                self.options.packagename   = string.strip(re.split(':',packageref)[1])
                
          if self.options.packagesource in ['COMMANDLINE'] :
             self.config             (package)
             self.header             ()
             self.input_packagepath  ()
             self.input_packagename  ()
          
          if self.options.packagesource in ['DIRECTORY','']:
             self.config             (package)
             self.header             ()
             self.input_packagesource()
             self.input_packagepath  ()
             self.input_packagename  ()
             self.directory          ()
             if self.options.packagerecord not in [''] and self.package_record.has_key(self.options.packagerecord):
                package = self.package_record[self.options.packagerecord]
             if self.options.packagename   not in [''] and self.package_name  .has_key(self.options.packagename):
                package = self.package_name[self.options.packagename]
             if self.options.packagenumber not in [''] and self.package_number.has_key(self.options.packagenumber):
                package = self.package_number[self.options.packagenumber]
                
          if self.options.packagesource in ['HISTORY']  :
             self.history            ()
             if self.options.packagerecord not in [''] and self.package_record.has_key(self.options.packagerecord):
                package = self.package_record[self.options.packagerecord]
             if self.options.packagename   not in [''] and self.package_name  .has_key(self.options.packagename):
                package = self.package_name[self.options.packagename]
             if self.options.packagenumber not in [''] and self.package_number.has_key(self.options.packagenumber):
                package = self.package_number[self.options.packagenumber]
             if package in [None] and self.options.packagename not in ['']:
                return package
             self.config             (package)
             self.header             ()
             self.input_packagepath  ()
             self.input_packagename  ()
          
          if package in [None] or self.options.packagename not in [''] : 
             self.input  (package)
             self.default() 
             package  = TransportPackage(self.options.packagerecord,self.options.packageguid  ,self.options.packagecreation   ,self.options.packageexecution,self.options.packagepath ,          
                                         self.options.packagename  ,self.options.transporttype,self.options.transportdirection,self.options.packageref  ,
                                         self.options.packagepart  ,self.options.streamtypes  ,self.options.streammedium      ,self.options.streamformat,          
                                         self.options.streamformat_target,self.options.streamcharencoding,self.options.streamlob,packagearchive ,
                                         self.options.sourcedbnode ,self.options.sourcedbname ,self.options.sourcedbuser,self.options.sourcedbschema,
                                         self.options.targetdbnode ,self.options.targetdbname ,self.options.targetdbuser,self.options.targetdbschema,self.options.packagecomment )    

          return package  
          
      def input(self  ,package = None):
          self.input_package            (package)
          self.input_packagecomment     ()
          self.input_transportdirection ()
          self.input_match_packages     ()
          self.input_sourcedatabase     () #EXPORTMONITOR
          self.input_sourcedatapackage  ()
          self.input_targetdatabase     () #IMPORTMONITOR
          self.input_packagepart        () #MATCHING  
          self.input_streamtypes        ()
          self.input_streamformat       ()
          self.input_transporttype      () 
          
      def environ(self,verbose = False): 
          global LOADERPROGPATH
          global LOADERDATAPATH
          global LOADERCONFIGFILE
          global LOADERLOGHISTORY
          global INDEPPROGPATH    
          global TESTLOADER  
          rc = True 
          if TESTLOADER in ['FALSE',None]:
             TESTLOADER  = os.getenv('TESTLOADER')
             if TESTLOADER in ['FALSE',None]:
                TESTLOADER = 'FALSE' 
             else:
                TESTLOADER = 'TRUE'
                self.options.packagesource = 'COMMANDLINE' 
          else :
             TESTLOADER = 'TRUE'
             self.options.packagesource    = 'COMMANDLINE' 
          
          INSTROOT = os.getenv('INSTROOT')   
          if INSTROOT == None:
             INSTROOT  = ''
          try:
              INDEPPROGPATH = loader.__file__
              for i in range(4):
                  INDEPPROGPATH = os.path.split(INDEPPROGPATH)[0]
              if self.options.loaderinstroot == '':
                 self.options.loaderinstroot  = INDEPPROGPATH
              if sys.platform[:3] == 'win':
                 LOADERPROGPATH  = _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"APPDATA")[0] 
                 LOADERDATAPATH  = _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders"),"Personal")[0]
                 if(os.path.isfile (os.path.join (LOADERPROGPATH, 'sdb'   ,'loader','config','loader.cfg')) == False and
                    os.path.isfile (os.path.join (INDEPPROGPATH , 'config','loader.cfg')) == True ):
                    shutil.copy    (os.path.join (INDEPPROGPATH , 'config','loader.cfg'),
                                    os.path.join (LOADERPROGPATH, 'sdb'   ,'loader','config','loader.cfg'))
                    os.chmod       (os.path.join (LOADERPROGPATH, 'sdb'   ,'loader','config','loader.cfg') ,stat.S_IRWXG|stat.S_IRWXU )
                 LOADERCONFIGFILE = os.path.join (LOADERPROGPATH, 'sdb'   ,'loader','config','loader.cfg')
                 LOADERLOGHISTORY = os.path.join (LOADERPROGPATH, 'sdb'   ,'loader','log','transport'+os.extsep+'log')
              if sys.platform[:3] != 'win':
                 LOADERPROGPATH = os.getenv('HOME')
                 LOADERDATAPATH = os.getenv('HOME')
                 if(os.path.isfile (os.path.join (LOADERPROGPATH, '.sdb'  ,'loader','config','loader.cfg')) == False and
                    os.path.isfile (os.path.join (INDEPPROGPATH , 'config','loader.cfg')) == True ):
                    shutil.copy    (os.path.join (INDEPPROGPATH , 'config','loader.cfg'),
                                    os.path.join (LOADERPROGPATH, '.sdb'  ,'loader','config','loader.cfg'))
                    os.chmod       (os.path.join (LOADERPROGPATH, '.sdb'  ,'loader','config','loader.cfg') ,stat.S_IRWXG|stat.S_IRWXU )
                 LOADERCONFIGFILE = os.path.join (LOADERPROGPATH, '.sdb'  ,'loader','config','loader.cfg')
                 LOADERLOGHISTORY = os.path.join (LOADERPROGPATH, '.sdb'  ,'loader','log','transport'+os.extsep+'log')
              if verbose == True:
                 print 'ENVIRONMENT'
                 print 'INDEPPROGPATH          : %s' % INDEPPROGPATH   
                 print 'LOADERPROGPATH         : %s' % LOADERPROGPATH  
                 print 'LOADERCONFIGFILE       : %s' % LOADERCONFIGFILE
                 print 'LOADERLOGHISTORY       : %s' % LOADERLOGHISTORY
                 print 'LOADERDATAPATH         : %s' % LOADERDATAPATH  
          except dbm.CommunicationError, err :
              print err
          except dbm.DBMServError, err:
              print err             
          except EnvironmentError:
                print "Error reading home environment variable" 
          return rc
          
      def history(self): 
          rc = True
          if self.options.packagesource in ['HISTORY']:
             global LOADERLOGHISTORY
             self.package_record.clear()
             self.package_name  .clear() 
             self.package_number.clear()  
             packagepath         = ''
             packagecomment      = ''
             try:
                if os.path.isfile          (LOADERLOGHISTORY ) in [True]:
                   self.transportlog = file(LOADERLOGHISTORY,'r') 
                   records = csv.reader(self.transportlog, delimiter  = '|'  )
                   number=0
                   for record in records :
                       number=number+1
                       if len(record)   <= 30:
                          packagepath    = PACKAGEPATH
                          packagecomment = ''
                       else:
                          packagepath    = record[40]
                          packagecomment = record[29]
                       package                        = TransportPackage(number,record[14],record[30],record[0] ,packagepath,record[1] ,record[2] ,record[3] ,record[4] ,record[5] ,record[6] ,record[8] ,record[9] ,record[10],
                                                                                record[11],record[12],record[13],record[19],record[20],record[21],record[22],record[23],record[24],record[25],record[26] ,packagecomment )
                       if number > 2:  
                          self.package_record[number] = package 
                   number = 0
                   for key in self.package_record.keys():
                       self.package_name  [self.package_record[key].packagename] = self.package_record[key]
                   number = 0
                   for key in self.package_name.keys():
                       number = number + 1
                       self.package_number[number] = self.package_name[key]  
             except:
                 pass  #print ' Package history may be corrupt'
          return rc
      
      def directory(self):
          if self.options.packagesource in ['DIRECTORY']: 
             self.package_record.clear()
             self.package_name  .clear() 
             self.package_number.clear()  
             self.config       ()
             self.input_package()
             self.default() 
             packagearchive = ''
             packagename    = ''
             packageref     = self.options.packageref
             if packageref not in ['']:
                if len(re.split(':',packageref)) == 1:
                   packageref    = string.strip(re.split(':',packageref)[0])
                   packagename   = packageref
                if len(re.split(':',packageref))== 2:
                   packageref    = string.strip(re.split(':',packageref)[0])                  
                   packagename   = string.strip(re.split(':',packageref)[1])
                if len(re.split(':',packageref))== 3:
                   packageref    = string.strip(re.split(':',packageref)[0])                  
                   packagename   = string.strip(re.split(':',packageref)[1])
             number=0
             for root,dirs,files in os.walk(os.path.join(self.options.packagepath,'sdb','loader','packages')):
                 if len(files)> 0 :
                    for file in files:
                        if(string.find(file,'.par'))>=0:
                           number=number+1
                           package  = open(os.path.join(root,file))
                           options  = package.readline()
                           package  = TransportPackage()
                           package.packagerecord          = number
                           package.packagepath            = self.options.packagepath
                           package.packagearchive         = ''
                           #check consistence between history and directory
                           transporttype                  = string.replace(string.replace(re.split('-',options) [1] ,'T ',''),' ','')
                           if transporttype not in ['']:
                              if len(re.split(':',transporttype)) == 1:
                                 package.transporttype    = string.strip(re.split(':',transporttype)[0])
                                 package.packageguid      = ''
                              if len(re.split(':',transporttype))== 2:
                                 package.transporttype    = string.strip(re.split(':',transporttype)[0])
                                 package.packagecreation  = string.strip(re.split(':',transporttype)[1])
                              if len(re.split(':',transporttype))== 3:
                                 package.transporttype    = string.strip(re.split(':',transporttype)[0])
                                 package.packagecreation  = string.strip(re.split(':',transporttype)[1])
                                 package.packageguid      = string.strip(re.split(':',transporttype)[2])
                           package.transportdirection     = string.replace(string.replace(re.split('-',options) [2] ,'t ',''),' ','')
                           packageref                          = string.replace(string.replace(re.split('-',options) [3] ,'p ',''),' ','')
                           if packageref not in ['']:
                              if len(re.split(':',packageref)) == 1:
                                 package.packageref    = string.strip(re.split(':',packageref)[0])
                                 package.packagename   = packageref
                              if len(re.split(':',packageref))== 2:
                                 package.packageref    = string.strip(re.split(':',packageref)[0])                  
                                 package.packagename   = string.strip(re.split(':',packageref)[1])
                              if len(re.split(':',packageref))== 3:
                                 package.packageref    = string.strip(re.split(':',packageref)[0])                  
                                 package.packagename   = string.strip(re.split(':',packageref)[1])
                                 package.packagecomment= string.strip(re.split(':',packageref)[2])
                           package.packagepart            = string.replace(string.replace(re.split('-',options) [4] ,'P ',''),' ','')
                           package.streamtypes            = string.replace(string.replace(re.split('-',options) [5] ,'s ',''),' ','')
                           package.initposition           = string.replace(string.replace(re.split('-',options) [6] ,'i ',''),' ','')
                           package.streammedium           = string.replace(string.replace(re.split('-',options) [7] ,'m ',''),' ','')
                           package.streamformat           = string.replace(string.replace(re.split('-',options) [8] ,'f ',''),' ','')
                           package.streamformat_target    = string.replace(string.replace(re.split('-',options) [9] ,'F ',''),' ','')
                           package.streamcharencoding     = string.replace(string.replace(re.split('-',options) [10],'c ',''),' ','')
                           package.streamlob              = string.replace(string.replace(re.split('-',options) [11],'l ',''),' ','')
                           try:
                               package.sourcedbnode       = string.replace(string.replace(re.split('-',options) [12],'nSRC ',''),' ','')
                               package.sourcedbname       = string.replace(string.replace(re.split('-',options) [13],'dSRC ',''),' ','')
                               package.sourcedbuser       = string.replace(string.replace(re.split('-',options) [14],'uSRC ',''),' ','')
                               package.sourcedbschema     = string.replace(string.replace(re.split('-',options) [15],'sSRC ',''),' ','')
                               if package.sourcedbschema ==['']:
                                  package.sourcedbschema  = re.split(',',package.sourcedbuser)[0] 
                               package.targetdbnode       = string.replace(string.replace(re.split('-',options) [16],'nTRG ',''),' ','')
                               package.targetdbname       = string.replace(string.replace(re.split('-',options) [17],'dTRG ',''),' ','')
                               package.targetdbuser       = string.replace(string.replace(re.split('-',options) [18],'uTRG ',''),' ','')
                               package.targetdbschema     = string.replace(string.replace(re.split('-',options) [19],'sTRG ',''),' ','') 
                               if package.targetdbschema ==['']:
                                  package.targetdbschema  = re.split(',',spackage.targetdbuser)[0] 
                           except:
                               pass
                           self.package_record[number] = package
             number = 0
             for key in self.package_record.keys():
                 self.package_name  [self.package_record[key].packagename] = self.package_record[key]
             number = 0
             for key in self.package_name.keys():
                 number = number + 1
                 self.package_number[number] = self.package_name[key] 

      def config(self,package=None,verbose=False):
          rc = True
          global PACKAGERESOURCE
          global PACKAGEPATH
          global PACKAGENAME
          global PACKAGEPROTOCOL
          global PACKAGEFORMAT
          global PACKAGETIMESTAMP
          global PACKAGEEXECUTION
          global PACKAGEARCHIVE
          global LOADERCONFIGFILE
          global LOADERCONFIGURATION
          global LOADERPROGPATH
          global LOADERDATAPATH
          global INDEPPROGPATH
          global PACKAGETIMEOUT       
          global MAXERRORCOUNT        
          global TRANSACTIONSIZE 
          global ISOLATIONLEVEL 
          global ODBCTRACE    
          global AUTOCOMMIT  
          global STREAMCATALOG   
          global STREAMDATA      
          global STREAMDATA_DUPLICATES
          global STREAMSTATISTICS
          global STREAMPACKAGE   
          global VERBOSE              
          global MONITOR              
          global TRANSFORMATIONSYNTAX 
          global CASESENSITIVE
          global SYSTEMATTRIBUTE
          global FIELDLOBMASK
          global FIELDBOOLVALUE
          try:
              if package     in [None]:
                 if self.options.packagename not in [''] and self.options.packageref not in ['','DB','USER','SCHEMA','TABLE'] :
                    if sys.platform[:3] == 'win':
                       LOADERCONFIGFILE = os.path.join(self.options.packagepath, 'sdb'  ,'loader','packages','export',string.replace(self.options.sourcedbnode,':','_'),self.options.sourcedbname,self.options.packageref,'%s.cfg' %self.options.packageref)
                    if sys.platform[:3] != 'win': 
                       LOADERCONFIGFILE = os.path.join(self.options.packagepath, 'sdb'  ,'loader','packages','export',string.replace(self.options.sourcedbnode,':','_'),self.options.sourcedbname,self.options.packageref,'%s.cfg' %self.options.packageref)
              if package not in [None]:
                 if package.packagename not in [''] and package.packageref not in ['','DB','USER','SCHEMA','TABLE'] :
                    if sys.platform[:3] == 'win':
                       LOADERCONFIGFILE = os.path.join(package.packagepath,      'sdb'  ,'loader','packages','export',string.replace(package.sourcedbnode,':','_'),package.sourcedbname,package.packageref,'%s.cfg' %package.packageref)
                    if sys.platform[:3] != 'win': 
                       LOADERCONFIGFILE = os.path.join(package.packagepath,      'sdb'  ,'loader','packages','export',string.replace(package.sourcedbnode,':','_'),package.sourcedbname,package.packageref,'%s.cfg' %package.packageref)
              if  os.path.isfile(LOADERCONFIGFILE ) == True :
                  os.chmod      (LOADERCONFIGFILE ,stat.S_IRWXG|stat.S_IRWXU )
                  LOADERCONFIGURATION = ConfigParser.SafeConfigParser()
                  LOADERCONFIGURATION.read(LOADERCONFIGFILE)
                  try:
                      PACKAGESOURCE    = LOADERCONFIGURATION.get          ('PACKAGE','SOURCE')
                  except ConfigParser.NoOptionError, err:
                      configfile = open(LOADERCONFIGFILE,'w')
                      LOADERCONFIGURATION.set                             ('PACKAGE','SOURCE','HISTORY')
                      LOADERCONFIGURATION.write(configfile)
                      configfile.close()
                  try:
                      PACKAGEPATH         = LOADERCONFIGURATION.get       ('PACKAGE','PATH')
                      if PACKAGEPATH in ['HOME','HOMEDIR'] and os.path.isdir(LOADERDATAPATH) :
                         PACKAGEPATH      = LOADERDATAPATH
                  except ConfigParser.NoOptionError, err:
                      if os.path.isdir    (LOADERDATAPATH):
                         PACKAGEPATH     = LOADERDATAPATH
                         configfile = open(LOADERCONFIGFILE,'w')
                         LOADERCONFIGURATION.set                          ('PACKAGE','PATH',PACKAGEPATH)
                         LOADERCONFIGURATION.write(configfile)
                         configfile.close()
                  try:
                      PACKAGEPROTOCOL     = LOADERCONFIGURATION.get       ('PACKAGE','PROTOCOL')
                  except ConfigParser.NoOptionError, err:
                      PACKAGEPROTOCOL     = 'PACKAGEPATH'
                      configfile = open(LOADERCONFIGFILE,'w')
                      LOADERCONFIGURATION.set                             ('PACKAGE','PROTOCOL','PACKAGEPATH')
                      LOADERCONFIGURATION.write(configfile)
                      configfile.close()
                  try:
                      CASESENSITIVE       = LOADERCONFIGURATION.getboolean('STREAM','CASESENSITIVE')
                  except ConfigParser.NoOptionError, err:
                      configfile = open(LOADERCONFIGFILE,'w')
                      LOADERCONFIGURATION.set                             ('STREAM','CASESENSITIVE','FALSE')
                      LOADERCONFIGURATION.write(configfile)
                      configfile.close()
                  PACKAGENAME             = LOADERCONFIGURATION.get       ('PACKAGE','NAME')
                 #PACKAGEARCHIVE          = LOADERCONFIGURATION.get       ('PACKAGE','ARCHIVE')
                  PACKAGEEXECUTION        = LOADERCONFIGURATION.get       ('PACKAGE','EXECUTION')
                  PACKAGETIMEOUT          = LOADERCONFIGURATION.getint    ('PACKAGE','TIMEOUT')
                  VERBOSE                 = LOADERCONFIGURATION.getboolean('PACKAGE','VERBOSE')
                  MONITOR                 = LOADERCONFIGURATION.getboolean('PACKAGE','MONITOR')
                  ODBCTRACE               = LOADERCONFIGURATION.get       ('DATABASE_CONNECTION','ODBCTRACE')
                  SYSTEMATTRIBUTE         = LOADERCONFIGURATION.get       ('DATABASE_DESIGNTIME','SYSTEMATTRIBUTE')
                  AUTOCOMMIT              = LOADERCONFIGURATION.get       ('DATABASE_CONNECTION','AUTOCOMMIT')
                  ISOLATIONLEVEL          = LOADERCONFIGURATION.getint    ('DATABASE_CONNECTION','ISOLATIONLEVEL')
                  TRANSACTIONSIZE         = LOADERCONFIGURATION.getint    ('DATABASE_RUNTIME'   ,'TRANSACTIONSIZE')
                  MAXERRORCOUNT           = LOADERCONFIGURATION.getint    ('LOADER','MAXERRORCOUNT')
                  TRANSFORMATIONSYNTAX    = LOADERCONFIGURATION.get       ('STREAM','TRANSFORMATIONSYNTAX')
                  STREAMCATALOG           = LOADERCONFIGURATION.getboolean('STREAM_CATALOG','STREAM')    
                  STREAMDATA              = LOADERCONFIGURATION.getboolean('STREAM_DATA'   ,'STREAM')
                  STREAMDATA_DUPLICATES   = LOADERCONFIGURATION.get       ('STREAM_DATA'   ,'DUPLICATE_DATA')
                  FIELDLOBMASK            = LOADERCONFIGURATION.get       ('STREAM_DATA'   ,'LOBFILE_MASK')
                  FIELDBOOLVALUE          = LOADERCONFIGURATION.get       ('STREAM_DATA'   ,'BOOLEAN')
                  if string.upper(STREAMDATA_DUPLICATES) == 'IGNORE':
                     STREAMDATA_DUPLICATES                = 'IGNORE DUPLICATES'
                  if string.upper(STREAMDATA_DUPLICATES) == 'REJECT':
                     STREAMDATA_DUPLICATES                = 'REJECT DUPLICATES'
                  if string.upper(STREAMDATA_DUPLICATES) == 'UPDATE':
                     STREAMDATA_DUPLICATES                = 'UPDATE DUPLICATES'
                  try:
                      STREAMSTATISTICS  = LOADERCONFIGURATION.getboolean('STREAM_STATISTICS','STREAM')    
                  except:
                      pass
                  STREAMPACKAGE         = LOADERCONFIGURATION.getboolean('STREAM_PACKAGE','STREAM')
                  try:
                      PACKAGEFORMAT     = LOADERCONFIGURATION.get       ('STREAM_PACKAGE','FORMAT')
                  except ConfigParser.NoOptionError, err:
                      configfile = open(LOADERCONFIGFILE,'w')
                      LOADERCONFIGURATION.set                           ('STREAM_PACKAGE','FORMAT','COMPRESSED')
                      LOADERCONFIGURATION.write(configfile)
                      configfile.close()
                  if self.options.streamlob in ['TABLE']:
                     FIELDLOBMASK       = ''
                  if self.options.streamlob == 'COLUMN':
                     FIELDLOBMASK       = ''
                  if self.options.streamlob == 'COLUMNVALUE':
                     FIELDLOBMASK       = FIELDLOBMASK
                  if verbose in [True]:
                     print 'CONFIGURATION'
                     print 'CONFIGURATION FILE     : %s ' %LOADERCONFIGFILE
                     print 'PACKAGEPATH            : %s ' %PACKAGEPATH          
                     print 'PACKAGENAME            : %s ' %PACKAGENAME            
                     print 'PACKAGEPROTOCOL        : %s ' %PACKAGEPROTOCOL        
                     print 'PACKAGEFORMAT          : %s ' %PACKAGEFORMAT 
                     print 'PACKAGEEXECUTION       : %s ' %PACKAGEEXECUTION   
                     print 'PACKAGEARCHIVE         : %s ' %PACKAGEARCHIVE         
                     print 'PACKAGETIMEOUT         : %s ' %PACKAGETIMEOUT         
                     print 'MAXERRORCOUNT          : %s ' %MAXERRORCOUNT      
                     print 'TRANSACTIONSIZE        : %s ' %TRANSACTIONSIZE        
                     print 'ISOLATIONLEVEL         : %s ' %ISOLATIONLEVEL         
                     print 'ODBCTRACE              : %s ' %ODBCTRACE          
                     print 'AUTOCOMMIT             : %s ' %AUTOCOMMIT             
                     print 'STREAMCATALOG          : %s ' %STREAMCATALOG      
                     print 'STREAMDATA             : %s ' %STREAMDATA             
                     print 'STREAMDATA_DUPLICATES  : %s ' %STREAMDATA_DUPLICATES
                     print 'STREAMSTATISTICS       : %s ' %STREAMSTATISTICS     
                     print 'STREAMPACKAGE          : %s ' %STREAMPACKAGE      
                     print 'VERBOSE                : %s ' %VERBOSE                
                     print 'MONITOR                : %s ' %MONITOR                
                     print 'TRANSFORMATIONSYNTAX   : %s ' %TRANSFORMATIONSYNTAX 
                     print 'CASESENSITIVE          : %s ' %CASESENSITIVE      
                     print 'SYSTEMATTRIBUTE        : %s ' %SYSTEMATTRIBUTE        
                     print 'FIELDLOBMASK           : %s ' %FIELDLOBMASK  	
              else:
                 #print 'Package configuration file \n %s\n does not exist' % LOADERCONFIGFILE
                  rc = False
          except EnvironmentError, err:
             print 'Error using the package archive: \n %s ' % err
             rc = False
          except csv.Error , err :
             print 'Error using the package archive: \n %s ' % err
          except IOError , (errno,err):
             print 'Error using the package archive: \n %s ' % err
          except ConfigParser.NoOptionError, err:
             print 'Error using the package archive: \n %s ' % err
          except ConfigParser.Error , err:
             print 'Error using the package archive: \n %s ' % err
          return rc
          
      def update(self,select = ''):
          self.package_name  .clear() 
          self.package_number.clear()  
          if select not in ['']:
             transportdirection = select
          else:
             transportdirection = ''
          if transportdirection in [''] :
             for key in self.package_record.keys():
                 if(self.package_record.has_key(key) and 
                    self.package_record[key].packagepath             in [self.options.packagepath       ] and 
                    self.package_record[key].transportdirection      in [self.options.transportdirection]   ):
                    self.package_name  [self.package_record[key].packagename] = self.package_record[key]
             number = 0
             for key in self.package_name.keys():
                 number = number + 1
                 self.package_number[number] = self.package_name[key] 
             for (key,package) in self.package_number.items(): 
                  if  package.streamcharencoding == 'DYNAMIC':
                      package.streamcharencoding  = ''
                  if VERBOSE == False:
                    #print ' [%i]:  %-15s, created at %s' %( key, package.packagename, time.strftime("%A, %d %B %Y %H:%M:%S ", time.strptime(package.packageexecution,"%Y%m%d%H%M%S") ))
                     print ' [%i]:  %-15s, created at %s' %( key, package.packagename, package.packageexecution )
                  if VERBOSE == True :
                     print ''
                     print ' [%i]:' % key
                     self.output(package)
          if transportdirection in ['IMPORT'] :
             for key in self.package_record.keys():
                 if(self.package_record.has_key(key) and 
                    self.package_record[key].packagepath             in [self.options.packagepath       ] and
                    self.package_record[key].transportdirection      in ['EXPORT'                       ] and 
                    self.package_record[key].transporttype           in ['EXECUTE'                      ] and
                    self.package_record[key].streammedium            in ['FILE'                         ] and
                    re.search(self.options.sourcedbnode,self.package_record[key].sourcedbnode ) != None   and
                    re.search(self.options.sourcedbname,self.package_record[key].sourcedbname ) != None   ):
                    self.package_name  [self.package_record[key].packagename] = self.package_record[key]
             number = 0
             for key in self.package_name.keys():
                 number = number + 1
                 self.package_number[number] = self.package_name[key] 
             for (key,package) in self.package_number.items(): 
                  if  package.streamcharencoding == 'DYNAMIC':
                      package.streamcharencoding  = ''
                  if VERBOSE == False:
                    #print ' [%i]:  %-15s, created at %s' %( key, package.packagename, time.strftime("%A, %d %B %Y %H:%M:%S ", time.strptime(package.packageexecution,"%Y%m%d%H%M%S") ))
                     print ' [%i]:  %-15s, created at %s' %( key, package.packagename, package.packageexecution )
                  if VERBOSE == True :
                     print ''
                     print ' [%i]:' % key
                     self.output(package)
          
      def insert(self):
          global LOADERPROGPATH
          sourcepackagepath        = self.options.sourcepackagepath
          sourcehistorypath        = ''
          sourcehistorypath_tgz    = ''
          targetpackagepath        = self.options.targetpackagepath
          targethistorypath        = ''
          targethistorypath_tgz    = ''
          PACKAGEREPOSITORY = (
          """ IMPORT TABLE   SYSLOADER.TRANSFORMATIONPACKAGEHISTORY  UPDATE DUPLICATES"""
          """        DATA    INSTREAM  FILE  '%s'                    CSV    ASCII     """
          """        SEPARATOR '|'                                                    """
          """        DELIMITER ''                                                     """
          """        NULL      '    '                                                 """
          """        DECIMAL   '//./'                                                 """
          """        BOOLEAN   'TRUE/FALSE'                                           """
          """        TIMESTAMP INTERNAL                                               """
          """        DATE      ISO                                                    """
          """        TIME      ISO                                                    """
          """ START                     3 MAXRECORDS                                  """
          """       PACKAGEEXECUTION    1 CHAR DEFAULT NULL                           """
          """       PACKAGENAME         2 CHAR DEFAULT NULL                           """
          """       ACTION              3 CHAR DEFAULT NULL                           """
          """       TRANSFORMATION      4 CHAR DEFAULT NULL                           """
          """       PACKAGEREF          5 CHAR DEFAULT NULL                           """
          """       PACKAGEPART         6 CHAR DEFAULT NULL                           """
          """       STREAMS             7 CHAR DEFAULT NULL                           """
          """       STREAMPOSITION      8 CHAR DEFAULT NULL                           """
          """       STREAMMEDIUM        9 CHAR DEFAULT NULL                           """
          """       STREAMFORMAT       10 CHAR DEFAULT NULL                           """
          """       STREAMINTERN       11 CHAR DEFAULT NULL                           """
          """       STREAMENCODING     12 CHAR DEFAULT NULL                           """
          """       STREAMLARGEOBJECTS 13 CHAR DEFAULT NULL                           """
          """       STREAMARCHIVE      14 CHAR DEFAULT NULL                           """
          """       PACKAGEGUID        15 CHAR DEFAULT NULL                           """
          """       EXPORTERVERSION    16 CHAR DEFAULT NULL                           """
          """       IMPORTERVERSION    17 CHAR DEFAULT NULL                           """
          """       SOURCEKEY          18 CHAR DEFAULT NULL                           """
          """       TARGETKEY          19 CHAR DEFAULT NULL                           """
          """       SOURCECOMPUTER     20 CHAR DEFAULT NULL                           """
          """       SOURCEDATABASE     21 CHAR DEFAULT NULL                           """
          """       SOURCEUSER         22 CHAR DEFAULT NULL                           """
          """       SOURCESCHEMA       23 CHAR DEFAULT NULL                           """
          """       TARGETCOMPUTER     24 CHAR DEFAULT NULL                           """
          """       TARGETDATABASE     25 CHAR DEFAULT NULL                           """
          """       TARGETUSER         26 CHAR DEFAULT NULL                           """
          """       TARGETSCHEMA       27 CHAR DEFAULT NULL                           """
          """       SOURCEVERSION      28 CHAR DEFAULT NULL                           """
          """       TARGETVERSION      29 CHAR DEFAULT NULL                           """
          """       PACKAGECOMMENT     30 CHAR DEFAULT NULL                           """
          """       PACKAGECREATION    31 CHAR DEFAULT NULL                           """
          """       PACKAGEPATH        41 CHAR DEFAULT NULL                           """
          """       PACKAGEEXPORT      42      DEFAULT NULL                           """
          """       PACKAGEIMPORT      43      DEFAULT NULL                           """ ) % LOADERLOGHISTORY
          # PACKAGEHISTORY
          if self.options.transporttype == 'EXECUTE' or self.options.transporttype == 'GENERATE':
             if  self.options.transportdirection   in ['TRANSPORT']:
                 transformationpackage_dir  = os.path.split(self.options.sourcepackagepath)[0]
                 transformationpackage_name = os.path.split(self.options.sourcepackagepath)[1]
                 if sys.platform[:3] == 'win':
                    sourcehistorypath = os.path.join(LOADERPROGPATH,'sdb' ,'loader','packages','export',string.replace(self.options.sourcedbnode,':','_'),self.options.sourcedbname,self.options.packagename)
                    targethistorypath = os.path.join(LOADERPROGPATH,'sdb' ,'loader','packages','import',string.replace(self.options.targetdbnode,':','_'),self.options.targetdbname,self.options.packagename)
                 if sys.platform[:3] != 'win':
                    sourcehistorypath = os.path.join(LOADERPROGPATH,'.sdb','loader','packages','export',string.replace(self.options.sourcedbnode,':','_'),self.options.sourcedbname,self.options.packagename)
                    targethistorypath = os.path.join(LOADERPROGPATH,'.sdb','loader','packages','import',string.replace(self.options.targetdbnode,':','_'),self.options.targetdbname,self.options.packagename)
             elif  self.options.transportdirection  in ['EXPORT']:
                 transformationpackage_dir  = os.path.split(self.options.sourcepackagepath)[0]
                 transformationpackage_name = os.path.split(self.options.sourcepackagepath)[1]
                 if sys.platform[:3] == 'win':
                    sourcehistorypath = os.path.join(LOADERPROGPATH,'sdb' ,'loader','packages','export',string.replace(self.options.sourcedbnode,':','_'),self.options.sourcedbname,self.options.packagename)
                 if sys.platform[:3] != 'win':
                    sourcehistorypath = os.path.join(LOADERPROGPATH,'.sdb','loader','packages','export',string.replace(self.options.sourcedbnode,':','_'),self.options.sourcedbname,self.options.packagename)
             elif  self.options.transportdirection  in  ['IMPORT']:
                 transformationpackage_dir  = os.path.split(self.options.targetpackagepath)[0]
                 transformationpackage_name = os.path.split(self.options.targetpackagepath)[1]
                 if sys.platform[:3] == 'win':
                    sourcehistorypath = os.path.join(LOADERPROGPATH,'sdb' ,'loader','packages','export',string.replace(self.options.sourcedbnode,':','_'),self.options.sourcedbname,self.options.packageref)
                    targethistorypath = os.path.join(LOADERPROGPATH,'sdb' ,'loader','packages','import',string.replace(self.options.targetdbnode,':','_'),self.options.targetdbname,self.options.packagename)
                 if sys.platform[:3] != 'win':
                    sourcehistorypath = os.path.join(LOADERPROGPATH,'.sdb','loader','packages','export',string.replace(self.options.sourcedbnode,':','_'),self.options.sourcedbname,self.options.packageref)
                    targethistorypath = os.path.join(LOADERPROGPATH,'.sdb','loader','packages','import',string.replace(self.options.targetdbnode,':','_'),self.options.targetdbname,self.options.packagename)
             
             #PACKAGECONTENT
             if(self.options.packageref in ['','DB','USER','SCHEMA','TABLE']):
                if sys.platform[:3] == 'win':
                   shutil.copy(os.path.join(LOADERPROGPATH, 'sdb','loader','config','loader.cfg'), os.path.join(transformationpackage_dir,transformationpackage_name,transformationpackage_name + os.extsep + 'cfg'))
                if sys.platform[:3] != 'win':
                   shutil.copy(os.path.join(LOADERPROGPATH,'.sdb','loader','config','loader.cfg'), os.path.join(transformationpackage_dir,transformationpackage_name,transformationpackage_name + os.extsep + 'cfg'))
             if(self.options.packageref in ['TABLE'] and self.options.packagepart in ['TABLE']):
                self.options.packageref    =  transformationpackage_name
             transformation_source_connect = ''
             transformation_target_connect = ''
             if self.options.streamcharencoding == '':
                streamcharencoding = 'DYNAMIC'
             if self.options.streamcharencoding != '':
                streamcharencoding = self.options.streamcharencoding
             if self.options.streamcharencoding == '':
                self.options.streamcharencoding  = 'DYNAMIC'
             if self.options.transportdirection == 'IMPORT':
                self.options.packageref =  os.path.split(self.options.sourcepackagepath)[1]
             #PACKAGEBODY
             if self.options.transportdirection in ['TRANSPORT','EXPORT','IMPORT'] :
                if self.options.sourcedbschema  in ['']:
                   self.options.sourcedbschema   = re.split(',',self.options.sourcedbuser)[0]
                transformation_source_connect    = " -nSRC %s -dSRC %s -uSRC %s -sSRC %s " % ( self.options.sourcedbnode, self.options.sourcedbname, re.split(',',self.options.sourcedbuser)[0]+',*', self.options.sourcedbschema)
             if self.options.transportdirection in ['TRANSPORT','IMPORT']:
                if self.options.targetdbschema  in ['']:
                   self.options.targetdbschema   = re.split(',',self.options.targetdbuser)[0]
                transformation_target_connect    = " -nTRG %s -dTRG %s -uTRG %s -sTRG %s " % ( self.options.targetdbnode, self.options.targetdbname, re.split(',',self.options.targetdbuser)[0]+',*', self.options.targetdbschema)   
             if self.options.transportdirection == 'EXPORT' and self.options.streamtypes == 'PACKAGE' and self.options.streamformat_target == 'DTL' :
                self.options.streamformat_target = 'ROW'
             transformationparameter             = " -T %s:%s:%s -t %s -p %s:%s:%s -P %s -s %s -i %s -m %s -f %s -F %s -c %s -l %s "  % ( self.options.transporttype, self.options.packagecreation, self.options.packageguid, self.options.transportdirection, self.options.packageref, self.options.packagename,self.options.packagecomment, self.options.packagepart, self.options.streamtypes, self.options.initposition, self.options.streammedium,self.options.streamformat,self.options.streamformat_target,streamcharencoding, self.options.streamlob)
             transformationparameter             = transformationparameter + transformation_source_connect + transformation_target_connect 
             transformationparameters            = file(os.path.join(transformationpackage_dir,transformationpackage_name,transformationpackage_name+ '.par'),'w+')
             transformationparameters.write( transformationparameter )                   
             transformationparameters.close()
             #PACKAGEHEADER
             transformation_source_connect       = "|%-20s |%-20s |%-20s |%-20s" % ( '', '', '', '' )
             transformation_target_connect       = "|%-20s |%-20s |%-20s |%-20s|"% ( '', '', '', '' )
             transportlog = ''
             if sys.platform[:3] == 'win':
                transportlog = os.path.join(LOADERPROGPATH, 'sdb' ,'loader','log','transport'+os.extsep+'log')
             if sys.platform[:3] != 'win':
                transportlog = os.path.join(LOADERPROGPATH, '.sdb','loader','log','transport'+os.extsep+'log')
             if os.path.isfile(transportlog) == False :
                transformation_id                = "|%-48s|%-16s |%-16s |%-15s |%-15s "   % ( 'PACKAGEID'        ,   'EXPORTER_VERSION' , 'IMPORTER_VERSION' , 'SOURCE_USERKEY', 'TARGET_USERKEY' )
                transformation_source_connect    = "|%-20s |%-20s |%-20s |%-20s"          % ( 'SOURCE_NODE -nSRC',   'SOURCE_DB -dSRC'  , 'SOURCE_USER -uSRC', 'SOURCE_SCHEMA -sSRC')
                transformation_target_connect    = "|%-20s |%-20s |%-20s |%-20s"          % ( 'TARGET_NODE -nTRG',   'TARGET_DB -dTRG'  , 'TARGET_USER -uTRG', 'TARGET_SCHEMA -sTRG')
                transformation_archive           = "|%-15s |%-15s |%-32s |%-20s |%s|%s|%s|%s|%s|%s|%s|%s|%s|%-128s|%-160s|%-160s" % ('SOURCE_VERSION'   , 'TARGET_VERSION'   , 'PACKAGECOMMENT' , 'PACKAGECREATION','','','','','','','','','','PACKAGEPATH','EXPORTER_ARCHIVE','IMPORTER_ARCHIVE' )
                transformationparameter_1        =  "%-20s |%-15s |%-10s |%-10s |%-15s |%-10s |%-20s |%-10s |%-10s |%-10s |%-10s |%-10s |%-11s  |%-10s "     % ( 'PACKAGEEXECUTION','PACKAGENAME' , 'TYPE -T','TYPE -t' , 'PACKAGEREF -p', 'PART -P', 'STREAMS -s', 'INITPOS -i', 'MEDIUM -m','FORMAT -f','-F ','CODING -c', 'LOB -l','ARCHIVE')
                transformationparameter_1        =  transformationparameter_1 + transformation_id +  transformation_source_connect + transformation_target_connect + transformation_archive + '\n'
                
                transformation_id                = "|%-48s|%-16s |%-16s |%-15s |%-15s "   % ( '14','15','16','17','18')
                transformation_source_connect    = "|%-20s |%-20s |%-20s |%-20s"          % ( '19','20','21','22' )
                transformation_target_connect    = "|%-20s |%-20s |%-20s |%-20s"          % ( '23','24','25','26' )
                transformation_archive           = "|%-15s |%-15s |%-32s |%-20s |%s|%s|%s|%s|%s|%s|%s|%s|%s|%-128s|%-160s|%-160s"        % ( '27','28','29','','','','','','','','','','','40','41','42' )
                transformationparameter_2        =  "%-20s |%-15s |%-10s |%-10s |%-15s |%-10s |%-20s |%-10s |%-10s |%-10s |%-10s |%-10s |%-11s  |%-10s "  % ( '0','1','2','3','4','5','6','7','8','9','10','11','12','13')
                transformationparameter_2        = transformationparameter_2 + transformation_id + transformation_source_connect + transformation_target_connect + transformation_archive + '\n'
                
                transformationparameter          = transformationparameter_1 + transformationparameter_2 
                transformationparameters         = file(transportlog,'a+')
                transformationparameters.write( transformationparameter )
                transformationparameters.close()
             #PACKAGEARCHIVE
             if self.options.packagesource in ['COMMANDLINE','DIRECTORY','HISTORY','REPOSITORY']:
                try:
                    if self.options.transportdirection in ['TRANSPORT']:
                       if os.path.isdir(sourcehistorypath) in [False]:
                          os.makedirs  (sourcehistorypath)
                       if os.path.isdir(targethistorypath) in [False]: 
                          os.makedirs  (targethistorypath) 
                    if self.options.transportdirection in ['EXPORT']   :
                       if os.path.isdir(sourcehistorypath) in [False]:
                          os.makedirs  (sourcehistorypath)
                    if self.options.transportdirection in ['IMPORT']   :
                       if os.path.isdir(targethistorypath) in [False]: 
                          os.makedirs  (targethistorypath)
                    if self.options.transportdirection in ['TRANSPORT','EXPORT']:
                       os.chdir(sourcepackagepath)
                       if os.path.isfile             (os.path.join (sourcehistorypath,self.options.packagename + '.tgz')) in [False]:
                          tar =  tarfile.TarFile.open(os.path.join (sourcehistorypath,self.options.packagename + '.tgz'), 'w:' ) 
                          #design time data
                          if os.path.isfile(self.options.packagename         ):   
                             tar.add       (self.options.packagename         )
                          if os.path.isfile(self.options.packagename + '.par'):   
                             tar.add       (self.options.packagename + '.par')
                          if os.path.isfile(self.options.packagename + '.cfg'):
                             tar.add       (self.options.packagename + '.cfg')
                          if os.path.isfile(self.options.packagename + '.xml'):   
                             tar.add       (self.options.packagename + '.xml')
                          if os.path.isfile(self.options.packagename + '.config'):   
                             tar.add       (self.options.packagename + '.config')
                          if os.path.isfile(self.options.packagename + '.pkg'):
                             tar.add       (self.options.packagename + '.pkg') 
                          if os.path.isfile(self.options.packagename + '.IMPORT'):
                             tar.add       (self.options.packagename + '.IMPORT') 
                          if os.path.isfile(self.options.packagename + '.EXPORT'):
                             tar.add       (self.options.packagename + '.EXPORT') 
                          #runtime time data 
                          if os.path.isfile(self.options.packageexecution + '.PACKAGE'):
                             tar.add       (self.options.packageexecution + '.PACKAGE')                      
                          if os.path.isfile('loader_' + self.options.packageexecution + '.log'):
                             tar.add       ('loader_' + self.options.packageexecution + '.log') 
                       else :
                          tar =  tarfile.TarFile.open(os.path.join (sourcehistorypath,self.options.packagename + '.tgz'), 'a:' ) 
                          #runtime time data 
                          if os.path.isfile(self.options.packageexecution + '.PACKAGE'):
                             tar.add       (self.options.packageexecution + '.PACKAGE')                      
                          if os.path.isfile('loader_' + self.options.packageexecution + '.log'):
                             tar.add       ('loader_' + self.options.packageexecution + '.log') 
                       tar.close()
                    if self.options.transportdirection in ['TRANSPORT','IMPORT']:
                       os.chdir(targetpackagepath)
                       if os.path.isfile             (os.path.join (targethistorypath,self.options.packagename + '.tgz')) in [False]:
                          tar =  tarfile.TarFile.open(os.path.join (targethistorypath,self.options.packagename + '.tgz'), 'w:' ) 
                          #design time data
                          if os.path.isfile(self.options.packagename         ):   
                             tar.add       (self.options.packagename         )
                          if os.path.isfile(self.options.packagename + '.par'):   
                             tar.add       (self.options.packagename + '.par')
                          if os.path.isfile(self.options.packagename + '.cfg'):
                             tar.add       (self.options.packagename + '.cfg')
                          if os.path.isfile(self.options.packagename + '.xml'):   
                             tar.add       (self.options.packagename + '.xml')
                          if os.path.isfile(self.options.packagename + '.config'):   
                             tar.add       (self.options.packagename + '.config')
                          if os.path.isfile(self.options.packagename + '.pkg'):
                             tar.add       (self.options.packagename + '.pkg') 
                          if os.path.isfile(self.options.packagename + '.IMPORT'):
                             tar.add       (self.options.packagename + '.IMPORT') 
                          if os.path.isfile(self.options.packagename + '.EXPORT'):
                             tar.add       (self.options.packagename + '.EXPORT') 
                          #runtime time data 
                          if os.path.isfile(self.options.packageexecution + '.PACKAGE'):
                             tar.add       (self.options.packageexecution + '.PACKAGE')
                          if os.path.isfile('loader_' + self.options.packageexecution + '.log'):
                             tar.add       ('loader_' + self.options.packageexecution + '.log')    
                       else :
                          tar =  tarfile.TarFile.open(os.path.join (targethistorypath,self.options.packagename + '.tgz'), 'a:' ) 
                          #runtime time data 
                          if os.path.isfile(self.options.packageexecution + '.PACKAGE'):
                             tar.add       (self.options.packageexecution + '.PACKAGE')
                          if os.path.isfile('loader_' + self.options.packageexecution + '.log'):
                             tar.add       ('loader_' + self.options.packageexecution + '.log')                                                                      
                       tar.close()
                except TarError, err:
                    print err
                except RuntimeError, err:
                    print err
                except OSError, (errno, err ):
                    print err
                except IOError, (errno, err ):
                    print err
                except:
                   pass 
             
             #PACKAGEBODY
             transformation_id                    = "|%-48s|%-16s |%-16s |%-15s |%-15s " % ( self.options.packageguid , EXPORTER_VERSION, IMPORTER_VERSION, '', '' )
             if self.options.transportdirection    in ['TRANSPORT','EXPORT','IMPORT']:
                if self.options.transportdirection in ['TRANSPORT','EXPORT','IMPORT']:
                   if self.options.sourcedbschema  in ['']:
                      self.options.sourcedbschema  =  re.split(',',self.options.sourcedbuser)[0]
                   transformation_source_connect   = "|%-20s |%-20s |%-20s |%-20s" % ( self.options.sourcedbnode, self.options.sourcedbname, re.split(',',self.options.sourcedbuser)[0]+',*', self.options.sourcedbschema)
                   transformation_target_connect   = "|%-20s |%-20s |%-20s |%-20s" % ('','','', '')
                   if self.options.transportdirection in ['TRANSPORT','EXPORT']:
                      if os.path.isfile               (os.path.join(sourcehistorypath,self.options.packagename + '.tgz')):
                         sourcehistorypath_tgz       = os.path.join(sourcehistorypath,self.options.packagename + '.tgz')
                   if self.options.transportdirection in ['IMPORT']:
                      if os.path.isfile               (os.path.join(sourcehistorypath,self.options.packageref  + '.tgz')):
                         sourcehistorypath_tgz       = os.path.join(sourcehistorypath,self.options.packageref  + '.tgz')
                if self.options.transportdirection in ['TRANSPORT','IMPORT'] :
                   if self.options.targetdbschema  in ['']:
                      self.options.targetdbschema =  re.split(',',self.options.targetdbuser)[0]
                   transformation_target_connect  = "|%-20s |%-20s |%-20s |%-20s" % ( self.options.targetdbnode, self.options.targetdbname, re.split(',',self.options.targetdbuser)[0]+',*' , self.options.targetdbschema)
                   if os.path.isfile               (os.path.join(targethistorypath,self.options.packagename + '.tgz')):
                      targethistorypath_tgz       = os.path.join(targethistorypath,self.options.packagename + '.tgz')
             transformation_archive               = "|%-15s |%-15s |%-32s |%-20s |%s|%s|%s|%s|%s|%s|%s|%s|%s|%-128s|%-160s|%-160s" % ( DB_VERSION_SOURCE, DB_VERSION_TARGET,self.options.packagecomment,self.options.packagecreation,'','','','','','','','','', PACKAGEPATH,sourcehistorypath_tgz, targethistorypath_tgz)
             transformationparameter              =  "%-20s |%-15s |%-10s |%-10s |%-15s |%-10s |%-20s |%-10s |%-10s |%-10s |%-10s |%-10s |%-11s  |%-10s "  % ( self.options.packageexecution, transformationpackage_name, self.options.transporttype, self.options.transportdirection, self.options.packageref, self.options.packagepart, self.options.streamtypes, self.options.initposition, self.options.streammedium,self.options.streamformat,self.options.streamformat_target,self.options.streamcharencoding, self.options.streamlob, PACKAGEARCHIVE)
             transformationparameter              = transformationparameter + transformation_id + transformation_source_connect + transformation_target_connect + transformation_archive + '\n'
             transformationparameters             = file(transportlog,'a+')
             transformationparameters.write( transformationparameter )                   
             transformationparameters.close()
          self.trailer()
             
      def output(self,package=None):
          rc = True
          transformation     = ''
          whitespaces        = ''
          streamformat       = ''
          streamcharencoding = ''
          sourcedbnode       = ''
          sourcedbname       = ''
          sourceusername     = ''
          targetdbnode       = ''
          targetdbname       = ''
          targetusername     = ''
          if package not in  [None]:
             streamcharencoding = package.streamcharencoding
             if package.streamcharencoding in ['DYNAMIC']:
                streamcharencoding = ''
             if package.transportdirection in ['TRANSPORT']:
                transformation = 'Transport'
             if package.transportdirection in ['EXPORT']:
                transformation = 'Export'
                whitespaces    =        '   '
             if package.transportdirection in ['IMPORT']:
                transformation = 'Import'
                whitespaces    =        '   '
             if package.streamformat       in ['COMPRESSED']:
                streamformat = 'CSV'
             if package.streamformat       in ['FORMATTED']:
                streamformat = 'FWV'
             if package.streamformat not   in ['COMPRESSED','FORMATTED']:
                streamformat = package.streamformat
                
            #print       ' %s history record     %s  : %s ' % (transformation, whitespaces, package.packagerecord)
            #print       ' %s package guid  %s       : %s ' % (transformation, whitespaces, package.packageguid)
             print       ' %s package name  %s       : %s ' % (transformation, whitespaces, package.packagename)
             if package.packagecomment not in ['']:
                print    ' %s package comment    %s  : %s ' % (transformation, whitespaces, package.packagecomment )
            #print       ' %s package creation   %s  : %s ' % (transformation, whitespaces, package.packagecreation)
            #print       ' %s package execution  %s  : %s ' % (transformation, whitespaces, package.packageexecution)
            #print       ' %s package path       %s  : %s ' % (transformation, whitespaces, os.path.join(package.packagepath))
            #print       ' %s package status%s   [-T]: %s ' % (transformation, whitespaces, package.transporttype)
            #print       ' %s direction     %s   [-t]: %s ' % (transformation, whitespaces, package.transportdirection)
            #print       ' %s package       %s   [-p]: %s ' % (transformation, whitespaces, package.packageref)
            #print       ' %s part          %s   [-P]: %s ' % (transformation, whitespaces, package.packagepart)
            #print       ' %s streams       %s   [-s]: %s ' % (transformation, whitespaces, package.streamtypes)
            #print       ' %s format        %s   [-f]: %s ' % (transformation, whitespaces, streamformat)
            #print       ' %s format        %s   [-F]: %s ' % (transformation, whitespaces, package.streamformat_target)
            #print       ' %s encoding      %s   [-c]: %s ' % (transformation, whitespaces, streamcharencoding)
            #print       ' %s lobcolumn     %s   [-l]: %s ' % (transformation, whitespaces, package.streamlob)

             if package.sourcedbuser != '':
                sourceusername = re.split(',', package.sourcedbuser)[0]
             if (CASESENSITIVE == True  and   sourceusername != '') or ( sourceusername != '' and sourceusername [0] != '"' and sourceusername[len(sourceusername)-1] != '"'):
                sourceusername = string.upper(sourceusername)
             elif sourceusername != '':
                sourceusername = string.replace(sourceusername,'"','')    
             if package.targetdbuser != '':
                targetusername = re.split(',', package.targetdbuser)[0]
                targetpassword = re.split(',', package.targetdbuser)[1]
             if (CASESENSITIVE == True  and  targetusername != '') or ( targetusername != '' and targetusername [0] != '"' and targetusername[len(targetusername)-1] != '"'):
                targetusername = string.upper(targetusername)
             elif targetusername != '':
                targetusername = string.replace(targetusername,'"','') 
             if package.transportdirection in ['EXPORT']:
                if package.packagecomment     in [''] and package.sourcedbschema not in   [sourceusername, '']:
                   print  ' %s package comment    %s  : %s.%s.%s.%s ' % (transformation, whitespaces, package.sourcedbnode, package.sourcedbname, sourceusername, package.sourcedbschema   )
                if package.packagecomment     in [''] and package.sourcedbschema     in   [sourceusername, '']:
                   print  ' %s package comment    %s  : %s.%s.%s    ' % (transformation, whitespaces, package.sourcedbnode, package.sourcedbname, sourceusername  )
             if package.transportdirection in ['TRANSPORT','IMPORT']:
                if package.packagecomment     in [''] and package.sourcedbschema not in   [sourceusername, '']:
                   print  ' %s package comment    %s  : %s.%s.%s.%s -> %s.%s.%s.%s ' % (transformation, whitespaces, package.sourcedbnode, package.sourcedbname, sourceusername, package.sourcedbschema, package.targetdbnode, package.targetdbname, targetusername, package.targetdbschema )
                if package.packagecomment     in [''] and package.sourcedbschema     in   [sourceusername, '']:
                   print  ' %s package comment    %s  : %s.%s.%s -> %s.%s.%s       ' % (transformation, whitespaces, package.sourcedbnode, package.sourcedbname, sourceusername, package.targetdbnode, package.targetdbname, targetusername  )
             '''
             if package.transportdirection in ['EXPORT']:
                print    ' Source database computer[-nSRC]: %s ' % package.sourcedbnode     
                print    ' Source database name    [-dSRC]: %s ' % package.sourcedbname
                print    ' Source database user    [-uSRC]: %s ' % sourceusername
                if package.sourcedbschema not in   [sourceusername, '']:
                   print ' Source database schema  [-sSRC]: %s ' % package.sourcedbschema
             if package.transportdirection in ['TRANSPORT','IMPORT']:
                print    ' Source database computer[-nSRC]: %s ' % package.sourcedbnode    
                print    ' Source database name    [-dSRC]: %s ' % package.sourcedbname  
                print    ' Source database user    [-uSRC]: %s ' % sourceusername
                if package.sourcedbschema not in   [sourceusername, '']:
                   print ' Source database schema  [-sSRC]: %s ' % package.sourcedbschema
                print    ' Target database computer[-nTRG]: %s ' % package.targetdbnode     
                print    ' Target database name    [-dTRG]: %s ' % package.targetdbname
                print    ' Target database user    [-uTRG]: %s ' % targetusername
                if package.targetdbschema not in   [targetusername,'']:
                   print ' Target database schema  [-sTRG]: %s ' % package.targetdbschema
             '''
             
          if package in [None]:
             streamcharencoding = self.options.streamcharencoding
             if self.options.streamcharencoding in ['DYNAMIC']:
                streamcharencoding = ''
             if self.options.transportdirection in ['TRANSPORT']:
                transformation = 'Transport'
             if self.options.transportdirection in ['EXPORT']:
                transformation = 'Export'
                whitespaces    =        '   '
             if self.options.transportdirection in ['IMPORT']:
                transformation = 'Import'
                whitespaces    =        '   '
             if self.options.streamformat       in ['COMPRESSED']:
                streamformat = 'CSV'
             if self.options.streamformat       in ['FORMATTED']:
                streamformat = 'FWV'
             if self.options.streamformat not   in ['COMPRESSED','FORMATTED']:
                streamformat = self.options.streamformat
            #print       ' %s package source %s      : %s ' % (transformation, whitespaces, self.options.packagesource)                
            #print       ' %s history record     %s  : %s ' % (transformation, whitespaces, self.options.packagerecord)
            #print       ' %s package guid  %s       : %s ' % (transformation, whitespaces, self.options.packageguid)
             print       ' %s package name  %s       : %s ' % (transformation, whitespaces, self.options.packagename)
             print       ' %s package comment    %s  : %s ' % (transformation, whitespaces, self.options.packagecomment )
            #print       ' %s package creation   %s  : %s ' % (transformation, whitespaces, self.options.packagecreation)
            #print       ' %s package execution  %s  : %s ' % (transformation, whitespaces, self.options.packageexecution)
            #print       ' %s package path       %s  : %s ' % (transformation, whitespaces, self.options.packagepath)
            #print       ' %s package status%s   [-T]: %s ' % (transformation, whitespaces, self.options.transporttype)
            #print       ' %s direction     %s   [-t]: %s ' % (transformation, whitespaces, self.options.transportdirection)
            #print       ' %s package       %s   [-p]: %s ' % (transformation, whitespaces, self.options.packageref)
            #print       ' %s part          %s   [-P]: %s ' % (transformation, whitespaces, self.options.packagepart)
            #print       ' %s streams       %s   [-s]: %s ' % (transformation, whitespaces, self.options.streamtypes)
            #print       ' %s format        %s   [-f]: %s ' % (transformation, whitespaces, streamformat)
            #print       ' %s format        %s   [-F]: %s ' % (transformation, whitespaces, self.options.streamformat_target)
            #print       ' %s encoding      %s   [-c]: %s ' % (transformation, whitespaces, streamcharencoding)
            #print       ' %s lobcolumn     %s   [-l]: %s ' % (transformation, whitespaces, self.options.streamlob)
             '''
             if self.options.sourcedbnode not in ['']:
                sourcedbnode = string.replace(self.options.sourcedbnode,'.*','%')
                sourcedbnode = string.replace(self.options.sourcedbnode,'^' ,'' )
             if self.options.sourcedbname not in ['']:
                sourcedbname = string.replace(self.options.sourcedbname,'.*','%')
                sourcedbname = string.replace(self.options.sourcedbname,'^' ,'' )
             if self.options.sourcedbuser not in ['']:
                sourceusername = re.split(',', self.options.sourcedbuser)[0]
             if (CASESENSITIVE == True  and   sourceusername != '') or ( sourceusername != '' and sourceusername [0] != '"' and sourceusername[len(sourceusername)-1] != '"'):
                sourceusername = string.upper(sourceusername)
             elif sourceusername not in ['']:
                sourceusername = string.replace(sourceusername,'"','')    
             if self.options.targetdbnode not in ['']:
                targetdbnode = string.replace(self.options.targetdbnode,'.*','%')
                targetdbnode = string.replace(self.options.targetdbnode,'^' ,'' )
             if self.options.targetdbname not in ['']:
                targetdbname = string.replace(self.options.targetdbname,'.*','%')
                targetdbname = string.replace(self.options.targetdbname,'^' ,'' )
             if self.options.targetdbuser not in ['']:
                targetusername = re.split(',', self.options.targetdbuser)[0]
                targetpassword = re.split(',', self.options.targetdbuser)[1]
             if (CASESENSITIVE == True and targetusername != '') or (targetusername != '' and targetusername [0] != '"' and targetusername[len(targetusername)-1] != '"'):
                targetusername = string.upper(targetusername)
             elif targetusername not in ['']:
                targetusername = string.replace(targetusername,'"','') 
             if self.options.transportdirection in ['TRANSPORT','EXPORT']:
                print    ' Source database computer[-nSRC]: %s ' % sourcedbnode     
                print    ' Source database name    [-dSRC]: %s ' % sourcedbname
                print    ' Source database user    [-uSRC]: %s ' % sourceusername
                if self.options.sourcedbschema not in [sourceusername, '']:
                   print ' Source database schema  [-sSRC]: %s ' % self.options.sourcedbschema
             if self.options.transportdirection in ['TRANSPORT','IMPORT']:
                print    ' Source database computer[-nSRC]: %s ' % sourcedbnode    
                print    ' Source database name    [-dSRC]: %s ' % sourcedbname  
                print    ' Source database user    [-uSRC]: %s ' % sourceusername
                if self.options.sourcedbschema not in [sourceusername, '']:
                   print ' Source database schema  [-sSRC]: %s ' % self.options.sourcedbschema
                print    ' Target database computer[-nTRG]: %s ' % targetdbnode     
                print    ' Target database name    [-dTRG]: %s ' % targetdbname
                print    ' Target database user    [-uTRG]: %s ' % targetusername
                if self.options.targetdbschema not in [targetusername, '']:
                   print ' Target database schema  [-sTRG]: %s ' % self.options.targetdbschema
             '''
          return rc          
          
      def compress  (self):
          pass
      
      def decompress(self):
          if PACKAGEARCHIVE == 'ZIP' :
             try:
                 zipdirname  = os.path.split(self.options.sourcepackagepath)[0]
                 zipfilename = os.path.split(self.options.sourcepackagepath)[1]+ os.extsep + 'zip'
                 zip =  zipfile.ZipFile(os.path.join (zipdirname,zipfilename), 'r', zipfile.ZIP_DEFLATED )
                 if zip.testzip() == None:
                    os.chdir(self.options.sourcepackagepath)
                    for filename in zip.namelist():
                        unzip        = file (filename,'wb')
                        unzip.write(zip.read(filename))
                        unzip.close()
                    zip.close()
                 else:
                    print 'Export Package: %s not Ok' % os.path.split(self.options.sourcepackagepath)[1]
             except IOError, err:
                print 'Error decompessing the package zip archive : \n %s, %s ' % ( err, zipfilename )
             except:
                kind, val, traceback = sys.exc_info()
                raise kind, val, traceback
          elif PACKAGEARCHIVE == 'TAR' :
             try:
                 zipdirname  = os.path.split(self.options.sourcepackagepath)[0]
                 zipfilename = os.path.split(self.options.sourcepackagepath)[1]+ os.extsep + 'tgz'
                 zip =  tarfile.TarFile.open(os.path.join (zipdirname,zipfilename), 'r:gz' )
                 if tarfile.is_tarfile(os.path.join (zipdirname,zipfilename)) == True:
                    os.chdir(self.options.sourcepackagepath)
                    for filename in zip.getnames():
                        zip.extract(filename,self.options.sourcepackagepath)
                    zip.close()
                 else:
                    print 'Export Package: %s not Ok' % os.path.split(self.options.sourcepackagepath)[1]
             except IOError, err:
                print 'Error decompressing the package tar archive : \n %s, %s ' % ( err, zipfilename )
             except:
                kind, val, traceback = sys.exc_info()
                raise kind, val, traceback
                
      def input_match_packages(self):
          input   = '' 
          exist   = False
          global STEPID  
          for key in self.package_number.keys() :
              if self.package_number[key].transportdirection in [self.options.transportdirection]:
                 exist = True
          if exist in [True] and self.options.transporttype  in ['']:
             number  = '' 
             restore = ''
             STEPID  = STEPID+1
             if self.options.transportdirection in ['EXPORT']:
                restore = '(updates data)'
             print "\nStep %i: What do you want to do" % STEPID
             CASE_1 =(""" [1] Choose an %s package %s \n"""
                      """ [2] Create an %s package    \n"""
                      """ \nStep %i: """ ) % (string.lower(self.options.transportdirection),restore,string.lower(self.options.transportdirection),STEPID)
             try:    
                 while input not in [ '1','2' ] :
                       input = raw_input(CASE_1)
                       if input in ['1']:
                          self.update()
                          while number in [''] or self.package_number.has_key(string.atoi(number,10)) == False :
                                number = raw_input("\nStep %i: "  % STEPID )
                          package      = self.package_number[string.atoi(number,10)]
                          self.options.packagename     =  package.packagename
                          self.options.packagecreation =  package.packagecreation
                          #self.options.mode = 'PARAM'       
                          self.input_package (package)
                       if input in ['2']:
                          self.options.packagecreation = self.options.packageexecution
                 input  = '' 
             except EOFError:
                    os._exit(1)
                    
      def input_init(self):
          self.options.packagerecord      = ''      
          self.options.packagenumber      = ''      
          self.options.targetpackagepath  = ''  
          self.options.packageguid        = ''        
         #self.options.packagepath        = ''        
          self.options.packagename        = ''   
         #self.options.packageexecution   = ''   
          self.options.packagecreation    = ''    
          self.options.packagecomment     = ''     
          self.options.transporttype      = ''      
          self.options.transportdirection = '' 
          self.options.packageref         = ''         
          self.options.packagepart        = ''        
          self.options.streamtypes        = ''        
          self.options.streammedium       = ''       
          self.options.streamformat       = ''       
          self.options.streamformat_target= ''
          self.options.streamcharencoding = '' 
          self.options.streamlob          = ''          
          self.options.initposition       = ''       
          self.options.sourcedbnode       = ''       
          self.options.sourcedbname       = ''       
          self.options.sourcedbuser       = ''       
          self.options.sourcedbschema     = ''     
          self.options.targetdbnode       = ''       
          self.options.targetdbname       = ''       
          self.options.targetdbuser       = ''       
          self.options.targetdbschema     = ''  
          
      def input_package (self,package = None):
          if package not in [None]:
             if self.options.transporttype      in ['']:
                self.options.transporttype      = 'EXECUTE' 
             self.options.packagerecord         = package.packagerecord
             self.options.packagepath           = package.packagepath
             if package.packageguid             in ['']:
                self.options.packageguid        = PACKAGEGUID
             else:
                self.options.packageguid        = package.packageguid
             if package.packagecreation         in ['']:
                self.options.packagecreation    = self.options.packageexecution
             else:
                self.options.packagecreation    = package.packagecreation
             self.options.packagecomment        = package.packagecomment
             self.options.packagepart           = package.packagepart
             self.options.streamtypes           = package.streamtypes
             self.options.streammedium          = package.streammedium
             self.options.streamformat          = package.streamformat
             self.options.streamformat_target   = package.streamformat_target 
             self.options.streamcharencoding    = package.streamcharencoding
             self.options.streamlob             = package.streamlob
             if self.options.sourcedbnode       in [''] or re.search('[\^\.\*]',self.options.sourcedbnode) :
                self.options.sourcedbnode       = package.sourcedbnode
             if self.options.sourcedbname       in [''] or re.search('[\^\.\*]',self.options.sourcedbname):
                self.options.sourcedbname       = package.sourcedbname
             if self.options.sourcedbuser       in [''] or re.search('[\^\.\*]',self.options.sourcedbuser):
                self.options.sourcedbuser       = package.sourcedbuser
             if self.options.sourcedbschema     in [''] or re.search('[\^\.\*]',self.options.sourcedbschema):   
                if package.sourcedbschema       in ['']:
                   self.options.sourcedbschema  = re.split(',',self.options.sourcedbuser)[0]
                else:
                   self.options.sourcedbschema  = package.sourcedbschema
             if self.options.targetdbnode       in ['']:
                self.options.targetdbnode       = package.targetdbnode
             if self.options.targetdbname       in ['']:
                self.options.targetdbname       = package.targetdbname
             if self.options.targetdbuser       in ['']:
                self.options.targetdbuser       = package.targetdbuser
             if self.options.targetdbschema     in ['']:
                if package.targetdbschema       in ['']:
                   self.options.targetdbschema   = re.split(',',self.options.targetdbuser)[0]
                else:
                   self.options.targetdbschema   = package.targetdbschema
             if self.options.transportdirection  in ['']:
                self.options.transportdirection  = package.transportdirection
             if self.options.transportdirection  in ['TRANSPORT']:
                self.options.packageref          = package.packageref
                self.options.sourcepackagepath = os.path.join(package.packagepath, 'sdb','loader', 'packages', 'export', string.replace(package.sourcedbnode,':','_'), package.sourcedbname,package.packageref )
                self.options.targetpackagepath = os.path.join(package.packagepath, 'sdb','loader', 'packages', 'import', string.replace(package.targetdbnode,':','_'), package.targetdbname,package.packagename)
             if self.options.transportdirection  in ['EXPORT']:
                self.options.packageref          = package.packageref
                self.options.sourcepackagepath = os.path.join(package.packagepath, 'sdb','loader', 'packages', 'export', string.replace(package.sourcedbnode,':','_'), package.sourcedbname,package.packageref)
             if self.options.transportdirection  in ['IMPORT'] and package.transportdirection in ['IMPORT']:
                self.options.packageref          = package.packageref
                self.options.sourcepackagepath = os.path.join(package.packagepath, 'sdb','loader', 'packages', 'export', string.replace(package.sourcedbnode,':','_'), package.sourcedbname,package.packageref )
                self.options.targetpackagepath = os.path.join(package.packagepath, 'sdb','loader', 'packages', 'import', string.replace(package.targetdbnode,':','_'), package.targetdbname,package.packagename)
             if self.options.transportdirection  in ['IMPORT'] and package.transportdirection in ['EXPORT']:
                self.options.packageref          = package.packagename
                self.options.sourcepackagepath = os.path.join(package.packagepath, 'sdb','loader', 'packages', 'export', string.replace(package.sourcedbnode,':','_'), package.sourcedbname,package.packagename)
             if self.options.packagename         in ['']: 
                self.options.packagename         = package.packagename 
             
             #self.output()   
             #print 'sourcepackagepath %s ' % self.options.sourcepackagepath
             #print 'targetpackagepath %s ' % self.options.targetpackagepath
             #raw_input  ('STOP:OPTION PACKAGE')
          
          if package in [None]:
             transporttype                    = string.upper(self.options.transporttype)
             if transporttype not in ['']:
                if len(re.split(':',transporttype)) == 1:
                   self.options.transporttype = string.strip(re.split(':',transporttype)[0])
                   self.options.packageguid   = ''
                if len(re.split(':',transporttype))== 2:
                   self.options.transporttype = string.strip(re.split(':',transporttype)[0])
                   self.options.packageguid   = string.strip(re.split(':',transporttype)[1])
             self.options.packagepath         = string.strip(self.options.packagepath)
             self.options.packagecreation     = string.strip(self.options.packageexecution)
             self.options.packagepart         = string.upper(self.options.packagepart)
             self.options.streamtypes         = string.upper(self.options.streamtypes)
             self.options.streammedium        = string.upper(self.options.streammedium)
             self.options.streamformat        = string.upper(self.options.streamformat)
             self.options.streamformat_target = string.upper(self.options.streamformat_target)
             self.options.streamcharencoding  = string.upper(self.options.streamcharencoding)
             self.options.streamlob           = string.upper(self.options.streamlob)
             if self.options.sourcedbnode not in ['']:
                if string.upper(self.options.sourcedbnode)[:9] == 'LOCALHOST':
                   self.options.sourcedbnode   = string.replace(self.options.sourcedbnode,self.options.sourcedbnode[:9],socket.gethostname())
             if self.options.sourcedbname not in ['']:
                self.options.sourcedbname      = self.options.sourcedbname
             if self.options.sourcedbuser not in ['']:
                self.options.sourcedbuser      = self.options.sourcedbuser
             if self.options.sourcedbschema not in ['']:
                if CASESENSITIVE == True or (self.options.sourcedbschema [0] == '"' and self.options.sourcedbschema[len(self.options.sourcedbschema)-1] == '"'):
                   self.options.sourcedbschema = string.replace(self.options.sourcedbschema,'"','')
                else:
                   self.options.sourcedbschema = string.upper(self.options.sourcedbschema)
             if self.options.targetdbnode not in ['']:
                if string.upper(self.options.targetdbnode)[:9] == 'LOCALHOST':
                   self.options.targetdbnode   = string.replace(self.options.targetdbnode,self.options.targetdbnode[:9],socket.gethostname())
             if self.options.targetdbname not in ['']:
                self.options.targetdbname      = self.options.targetdbname
             if self.options.targetdbuser not in ['']:
                self.options.targetdbuser      = self.options.targetdbuser 
             if self.options.targetdbschema not in ['']:
                if CASESENSITIVE == True or (self.options.targetdbschema [0] == '"' and self.options.targetdbschema[len(self.options.targetdbschema)-1] == '"'):
                   self.options.targetdbschema = string.replace(self.options.targetdbschema,'"','')
                else:
                   self.options.targetdbschema = string.upper(self.options.targetdbschema) 
             if self.options.transportdirection not in ['']:
                self.options.transportdirection = string.upper(self.options.transportdirection)    
             packageref   = self.options.packageref
             if packageref not in ['']:
                if len(re.split(':',packageref)) == 1:
                   self.options.packageref    = string.strip(re.split(':',packageref)[0])
                   if string.upper(self.options.packageref) not in ['DB','USER','SCHEMA','TABLE']:
                      self.options.packagename   = self.options.packageref
                if len(re.split(':',packageref))== 2:
                   self.options.packageref    = string.strip(re.split(':',packageref)[0])                  
                   self.options.packagename   = string.strip(re.split(':',packageref)[1])
                if len(re.split(':',packageref))== 3:
                   self.options.packageref    = string.strip(re.split(':',packageref)[0])                  
                   self.options.packagename   = string.strip(re.split(':',packageref)[1])
                   self.options.packagecomment= string.strip(re.split(':',packageref)[2])
                   
             #self.output()
             #print 'sourcepackagepath %s ' % self.options.sourcepackagepath
             #print 'targetpackagepath %s ' % self.options.targetpackagepath
             #raw_input  ('STOP:OPTION COMMANDLINE') 
          self.default()    
             
      def default(self):
          rc = True
          if self.options.transportdirection in ['TRANSPORT']:   
             if self.options.transporttype   in ['']:
                self.options.transporttype    = PACKAGEEXECUTION
             if self.options.sourcedbnode	 in ['']   :
                self.options.sourcedbnode     = socket.gethostname()
             if self.options.targetdbnode	 in ['']   :
                self.options.targetdbnode     = socket.gethostname()
             if self.options.packageref      in ['DB'] :
                if TESTLOADER in ['FALSE']:
                   self.options.packagepart   = 'DB'
             if(self.options.packageref      in ['USER'] and 
                self.options.packagepart     in [''] ):
                self.options.packagepart      = 'USER'
             if(self.options.packageref      in ['SCHEMA'] and 
                self.options.packagepart     in [''] ):
                self.options.packagepart      = 'SCHEMA'
             if self.options.streamtypes     in ['']:
                self.options.streamtypes      = 'CATALOG,DATA,PACKAGE'
             if self.options.streammedium    in ['']:
                self.options.streammedium     = 'PIPE'
             if self.options.streamformat    in ['']:
                self.options.streamformat     = 'RECORDS'
             if self.options.streamformat    == 'PAGES':
                self.options.streamformat_target  = 'PAGE'
                self.options.streamlob        = 'TABLE'
             if self.options.streamformat    == 'RECORDS':
                self.options.streamformat_target  = 'ROW'
                self.options.streamlob        = 'TABLE'
             if self.options.initposition    in [''] :
                self.options.initposition     = 'START' 
          if self.options.transportdirection == 'EXPORT' :
             if self.options.transporttype   in ['']:
                self.options.transporttype    = PACKAGEEXECUTION
             if self.options.sourcedbnode    in ['']   :
                self.options.sourcedbnode     = '' #socket.gethostname()
             if self.options.packageref      in ['DB'] :
                if TESTLOADER in ['FALSE']:
                   self.options.packagepart   = 'DB'
             if(self.options.packageref      in ['USER'] and 
                self.options.packagepart     in [''] ):
                self.options.packagepart      = 'USER'
             if(self.options.packageref      in ['USER'] and 
                self.options.packagepart     in ['TABLE'] ):
                self.options.sourcedbschema   = ''
             if(self.options.packageref      in ['SCHEMA'] and 
                self.options.packagepart     in [''] ):
                self.options.packagepart      = 'SCHEMA'
             if self.options.streamtypes     in ['']:
                self.options.streamtypes      = 'CATALOG,DATA,PACKAGE'
             if self.options.streammedium    in ['']:
                self.options.streammedium     = 'FILE'
             if self.options.streamformat    in ['']:
                self.options.streamformat     = 'RECORDS'
             if self.options.streamformat    == 'PAGES':
                self.options.streamformat_target  = 'PAGE'
                self.options.streamlob        = 'TABLE'
             if self.options.streamformat    == 'RECORDS':
                self.options.streamformat_target  = 'ROW'
                self.options.streamlob        = 'TABLE'
             if self.options.initposition    in [''] :
                self.options.initposition     = 'START'
          if self.options.transportdirection == 'IMPORT':
             if self.options.sourcedbnode	 in ['']   :
                self.options.sourcedbnode     = '' #socket.gethostname()
             if self.options.targetdbnode	 in ['']   :
                self.options.targetdbnode     = '' #socket.gethostname()
          #INTER
          if self.options.transportdirection in [''] :
             self.options.mode = 'INTER'
          #PARAM
          if(self.options.transportdirection  not in [''] and
             self.options.sourcedbnode	      not in [''] and
             self.options.sourcedbname	      not in [''] and
             self.options.sourcedbuser	      not in [''] and
             self.options.packageref          not in [''] and
             self.options.packagepart         not in [''] and
             self.options.streammedium        not in [''] and
             self.options.streamformat        not in [''] and
             self.options.streamformat_target not in [''] and
             self.options.streamcharencoding  not in [''] and
             self.options.streamlob           not in [''] and
             self.options.initposition        not in [''] and
             self.options.mode                not in ['INTER']):
             self.options.mode = 'PARAM'
          #INTER OR PARAM
          if(self.options.transportdirection    == 'TRANSPORT' and (
             self.options.sourcedbname	        in [''] or
             self.options.sourcedbuser	        in [''] or
             self.options.targetdbname	        in [''] or
             self.options.targetdbuser          in [''] or
             self.options.mode                  == 'INTER')):
             self.options.mode = 'INTER'
          elif (self.options.transportdirection == 'TRANSPORT'):
             self.options.mode = 'PARAM'
          if(self.options.transportdirection    == 'EXPORT' and (
             self.options.sourcedbname	        in [''] or
             self.options.sourcedbuser	        in [''] or
             self.options.mode                  == 'INTER')):
             self.options.mode = 'INTER'
          elif (self.options.transportdirection == 'EXPORT'):
             self.options.mode = 'PARAM'
          if(self.options.transportdirection    == 'IMPORT' and (
             self.options.packageref    in [''] or
             self.options.targetdbname	        == '' or
             self.options.targetdbuser          == '' or
             self.options.mode                  == 'INTER')) :
             self.options.mode = 'INTER'
          elif (self.options.transportdirection == 'IMPORT'):
             self.options.mode = 'PARAM'
          if( self.options.packageref     in ['DB'] and 
              self.options.packagepart    in ['DB'] and
              self.options.streamformat   in ['COMPRESSED','FORMATTED'] and
              TESTLOADER in ['FALSE']    ):
              print 'Format %s not supported for package %s and packagepart %s ' %(self.options.streamformat, self.options.packageref, self.options.packagepart )
          if( self.options.packageref     in ['USER','SCHEMA'] and 
              self.options.packagepart    in ['USER','SCHEMA'] and
              self.options.streamformat   in ['COMPRESSED','FORMATTED']): 
              self.options.packagepart    = 'TABLE'
          return rc                       
                    
      def header(self):
          if self.options.packagepath in [''] and self.options.packagename in ['']:
             if  os.path.isfile   (LOADERLOGHISTORY ) in [False]: 
                 print   ' Package history: %s not found' % LOADERLOGHISTORY
             if  os.path.isfile   (LOADERLOGHISTORY ) in [True]: 
                 print   ' Package history: %s'           % LOADERLOGHISTORY
             print       ' Package path   : %s'           % PACKAGEPATH
             print       ' Package name   : %s'           % self.options.packageexecution 
             
      def trailer(self): 
          if self.options.packagename not in [''] and self.options.packageexecution not in ['']:
             print' Package execution log  : loader_%s.log ' % self.options.packageexecution
             print' Package name           : %s '            % self.options.packagename
    
      def input_packagesource(self):
          input = ''
          global STEPID
          if os.path.isfile (LOADERLOGHISTORY ) in [False]:
             self.options.packagesource         = 'DIRECTORY'   
          if os.path.isfile (LOADERLOGHISTORY ) in [True]:
             self.options.packagesource         = ''
             try:
                 STEPID = STEPID+1
                 print "\nStep %i: Choose a package source:  " % STEPID 
                 CASE_1 =(""" [1] Import packages from package history \n"""
                          """ [2] Import packages from package path    \n"""
                          """ \nStep %i: """ ) % STEPID 
                 while input not in ['1','2'] and self.options.packagesource in [''] :                           
                       input = raw_input(CASE_1)
                       if input == '1':
                          self.options.packagesource = 'HISTORY'
                       if input == '2':
                          self.options.packagesource = 'DIRECTORY'  
             except EOFError:
                os._exit(1)
                   
      def input_packagepath(self):
          input = ''
          global STEPID
          global PACKAGEPATH
          global LOADERPROGPATH
          global LOADERDATAPATH
          global LOADERCONFIGFILE
          try:
              if self.options.packagepath in [''] and self.options.packagename in ['']:
                 if self.options.packagename in [''] :                  
                    STEPID = STEPID+1
                    print "\nStep %i: Enter a package path: " % ( STEPID )
                 while self.options.packagepath in ['']:
                       input = raw_input(' Package path: ' )
                       if(string.upper(input) in ['HOME','HOMEDIR'] or PACKAGEPATH in ['HOME','HOMEDIR']) and os.access(LOADERDATAPATH,os.F_OK|os.R_OK|os.W_OK):
                          self.options.packagepath  = LOADERDATAPATH
                       if self.options.packagepath in [''] and input     in [''] and os.access(PACKAGEPATH,os.F_OK|os.R_OK|os.W_OK):
                          self.options.packagepath  = PACKAGEPATH
                       if self.options.packagepath in [''] and input not in [''] :
                          self.options.packagepath  = os.path.abspath(input)
                       if not os.access(self.options.packagepath,os.F_OK|os.R_OK|os.W_OK):
                          print " The path %s does not exist or has no read/write permissions" % self.options.packagepath
                          self.options.packagepath = ''
                       if os.name in ['nt'] and os.path.splitunc(self.options.packagepath)[0] != '':
                          print " UNC path is not supported, please map %s as network drive" % os.path.splitunc(self.options.packagepath)[0]
                          self.options.packagepath = ''
                 PACKAGEPATH = self.options.packagepath
              else:
                 self.options.packagepath = PACKAGEPATH
              try:
                  configfile = open(LOADERCONFIGFILE,'w')
                  LOADERCONFIGURATION.set('PACKAGE','PATH',PACKAGEPATH)
                  LOADERCONFIGURATION.write(configfile)
                  configfile.close()
              except ConfigParser.Error ,err:
                  print 'Error setting the package path in configuration: %s   %s '    % (PACKAGEPATH,err)
              except IOError , (errno,err):
                  print 'Error writing the package path in configuration: %s   %i,%s ' % (PACKAGEPATH,errno,err)
              except OSError, err:
                  print 'Error setting the package path in configuration: %s   %i,%s ' % (PACKAGEPATH,errno,err)
              except:
                  print 'Error setting the package path: %s         ' % (PACKAGEPATH)
          except EOFError:
                  os._exit(1)
              
      def input_packagename(self):
          input = ''
          global STEPID
          try:
              if PACKAGENAME != 'TIMESTAMP' and self.options.mode in ['INTER'] :
                 if self.options.packagename in ['']:                  
                    STEPID = STEPID+1
                    print "\nStep %i: Enter a package name:  " % STEPID
                 while self.options.packagename in ['']:
                       input = raw_input(' Package name: ' )
                       if input in ['']:
                          self.options.packagename = self.options.packageexecution
                       elif string.upper(input) in ['DB','USER','SCHEMA','TABLE','COLUMNS']:
                          print ' %s are standard packagenames,\n please choose a different packagename ' % "'DB','USER','SCHEMA','TABLE','COLUMNS'"
                          self.options.packagename = ''
                       elif input not in ['']:
                          self.options.packagename = input
              elif self.options.packagename != ''        and self.options.mode in ['PARAM'] :
                   self.options.packagename = self.options.packagename
              elif self.options.packagename == '':
                   self.options.packagename = self.options.packageexecution
          except EOFError:
                 os._exit(1)
                 
      def input_packagecomment(self):
          input = ''
          global STEPID
          global PACKAGECOMMENT
          if self.options.packagecomment in [''] and self.options.mode in ['INTER','']:                  
             STEPID = STEPID+1
             print "\nStep %i: Enter a package comment:  " % STEPID
             while input in ['']:
                   input = raw_input(' Package comment: ' )
                   if input in ['']:
                      self.options.packagecomment = ''
                      input = 'input'
                   else:
                      try:
                         input  = unicode(input,"ascii")
                         self.options.packagecomment = input  
                      except UnicodeError, err:
                         print err
                         input = ''
      
      def input_transportdirection(self):
          input = ''  
          global STEPID  
          try:              
              if self.options.transportdirection in ['']:
                 STEPID = STEPID+1
                 print "\nStep %i: What do you want to do" % STEPID
              while self.options.transportdirection in [ '' ]:
                    input = raw_input(""" [1] Transport from a source database into a target database\n"""
                                      """ [2] Export    from a source database\n"""
                                      """ [3] Import    into a target database\n """
                                      """ \nStep %i: """ %STEPID )
                    if input == '1':
                       self.options.transportdirection = 'TRANSPORT'
                    if input == '2':
                       self.options.transportdirection = 'EXPORT'
                    if input == '3':
                       self.options.transportdirection = 'IMPORT'
          except EOFError:
                 os._exit(1)
                 
      def input_sourcedatabase(self):
          input = ''
          global STEPID
          global EXPORTMONITOR 
          global IMPORTMONITOR 
          EXPORTMONITOR = None
          IMPORTMONITOR = None
          try:
              username      = ''
              userpassword  = ''
              schemaname    = ''
              if self.options.sourcedbuser not in [''] :
                 try:
                    username     = re.split(',',self.options.sourcedbuser)[0]
                    userpassword = re.split(',',self.options.sourcedbuser)[1]
                    if userpassword == '*':
                       userpassword  = ''
                 except:
                    pass
              if  self.options.transportdirection in ['TRANSPORT','EXPORT']:
                  if(self.options.sourcedbnode    in [''] or 
                     self.options.sourcedbname    in [''] or 
                     username                     in [''] or
                     userpassword                 in ['']    ):
                     STEPID = STEPID+1
                     print "\nStep %i: Enter the source database connection " % STEPID 
                     if self.options.sourcedbnode   not in [''] :
                        if string.upper(input)[:9] == 'LOCALHOST':
                           self.options.sourcedbnode  = string.replace(input,input[:9],socket.gethostname())
                        print ' Database computer: %s' % self.options.sourcedbnode
                     if self.options.sourcedbname   not in [''] :
                        print ' Database name    : %s' % self.options.sourcedbname
                     if username               not in [''] :
                        print ' Database user    : %s' % username
                  while EXPORTMONITOR == None:
                        while self.options.sourcedbnode in ['']:   
                              input    = raw_input(' Database computer: ')
                              if input == '' or string.upper(input)[:9] == 'LOCALHOST':
                                 if input == '' :
                                    self.options.sourcedbnode  = socket.gethostname()
                                 else :
                                    self.options.sourcedbnode  = string.replace(input,input[:9],socket.gethostname())
                              else:
                                 self.options.sourcedbnode = input
                        while self.options.sourcedbname in ['']:   
                              input    = raw_input(' Database name    : ') 
                              self.options.sourcedbname = string.upper(input)
                        while username in ['']:  
                              username = raw_input(' Database user    : ')   
                        while userpassword in ['']:   
                              userpassword   = getpass.getpass()
                        self.options.sourcedbuser = '%s,%s' %( username, userpassword )
                        if self.options.sourcedbuser not in ['']:
                           username     = re.split(',',self.options.sourcedbuser)[0]
                           userpassword = re.split(',',self.options.sourcedbuser)[1]
                        if (CASESENSITIVE == True and username != '') or (username != '' and username [0] != '"' and username[len(username)-1] != '"'):
                           username  = string.upper(username)
                        elif username != '' :
                           username = string.replace(username,'"','')  
                        self.options.sourcedbuser = '%s,%s' %( username, userpassword )  
                        try:
                            EXPORTMONITOR = sql.connect( username, userpassword, self.options.sourcedbname, self.options.sourcedbnode )
                        except (sql.CommunicationError ) , err:
                            print 'Connect failed, try again:' , err
                            EXPORTMONITOR    = None 
                        except (sql.SQLError ) , err:  
                            print 'Connect failed, try again:' , err    
                            EXPORTMONITOR    = None 
                        if EXPORTMONITOR    == None:
                           self.options.sourcedbnode = ''
                           self.options.sourcedbname = ''
                           self.options.sourcedbuser = ''
                           username     = ''
                           userpassword = ''
                  exist_db (self.options, EXPORTMONITOR, IMPORTMONITOR)
                  try:
                      self.options.sourcepackagepath = os.path.join(PACKAGEPATH, 'sdb','loader', 'packages', 'export', string.replace(self.options.sourcedbnode,':','_'), self.options.sourcedbname, self.options.packagename )
                      #create package
                      if   os.path.isdir(self.options.sourcepackagepath) in [False] and self.options.packageref     in ['']:
                           os.makedirs  (self.options.sourcepackagepath)
                      #choose package
                      elif os.path.isdir(self.options.sourcepackagepath) in [False] and self.options.packageref not in [''] and self.options.packagesource     in ['COMMANDLINE']:
                           os.makedirs  (self.options.sourcepackagepath)
                      elif os.path.isdir(self.options.sourcepackagepath) in [False] and self.options.packageref not in [''] and self.options.packagesource not in ['COMMANDLINE']:
                           print 'Package %s does not exist in filesystem' % self.options.packagename
                  except EnvironmentError, err:
                      print 'Indpendent sourcedatapath %s' %err
          except EOFError:
                 os._exit(1)
                 
      def input_sourcedataschema(self):
          input = ''
          global STEPID
          try:
              sourcedbuser     = ''
              userpassword     = ''
              sourcedbschema   = ''
              if self.options.sourcedbuser not in [''] :
                 try:
                    sourcedbuser = re.split(',',self.options.sourcedbuser)[0]
                    userpassword = re.split(',',self.options.sourcedbuser)[1]
                    if userpassword == '*':
                       userpassword  = ''
                 except:
                    pass
              if self.options.sourcedbschema in [''] and self.options.packageref in ['SCHEMA'] :
                 STEPID = STEPID+1
                 print "\nStep %i: Enter the source database schema " % STEPID 
                 while sourcedbschema in [''] :
                       sourcedbschema = raw_input(' Database schema  : ')
                       if sourcedbschema == '':
                          sourcedbschema  = sourcedbuser
                       if exist_schema(self.options,EXPORTMONITOR,IMPORTMONITOR, sourcedbschema ) == True:
                          if CASESENSITIVE == True or       (sourcedbschema [0] == '"' and sourcedbschema[len(sourcedbschema)-1] == '"'):
                             sourcedbschema = string.replace(sourcedbschema,'"','')
                          else:
                             sourcedbschema = string.upper  (sourcedbschema)
                          self.options.sourcedbschema =  sourcedbschema
                       else:
                          print ' Database schema %s does not exist' % sourcedbschema
                          sourcedbschema = ''
          except EOFError:
                 os._exit(1) 
                 
      def input_targetdataschema(self):
          input      = ''
          global    STEPID
          try:
              sourcedbuser   = ''
              sourcedbschema = ''
              targetdbuser   = ''
              targetdbschema = ''
              if self.options.sourcedbuser not in [''] :   
                 sourcedbuser   = re.split(',',self.options.sourcedbuser)[0] 
                 sourcedbschema = self.options.sourcedbschema
              if self.options.targetdbuser not in [''] :
                 targetdbuser  = re.split(',',self.options.targetdbuser)[0]
              if self.options.targetdbschema in [''] and IMPORTMONITOR   != None:
                 STEPID = STEPID+1
                 print "\nStep %i: Enter the target database schema " % STEPID 
                 while targetdbschema == '' :
                       targetdbschema = raw_input(' Database schema  : ')
                       if targetdbschema == '':
                          targetdbschema  = targetdbuser
                       if targetdbschema != '': 
                          if CASESENSITIVE == True or       (targetdbschema [0] == '"' and targetdbschema[len(targetdbschema)-1] == '"'):
                             targetdbschema = string.replace(targetdbschema,'"','')
                          else:
                             targetdbschema = string.upper  (targetdbschema)
                          if targetdbschema !=  self.options.sourcedbschema:
                             print ' Database schema of source %s and target %s do not match' % (self.options.sourcedbschema, targetdbschema )
                             targetdbschema  = ''
                          elif exist_schema(self.options,EXPORTMONITOR,IMPORTMONITOR, targetdbschema ) == True:
                             if CASESENSITIVE == True or       (targetdbschema [0] == '"' and targetdbschema[len(targetdbschema)-1] == '"'):
                                targetdbschema = string.replace(targetdbschema,'"','')
                             else:
                                targetdbschema = string.upper  (targetdbschema)
                             self.options.targetdbschema = targetdbschema
                             self.input_match_schemas()
                          else:
                             print ' Database schema %s does not exist' % targetdbschema
                             targetdbschema = ''
          except EOFError:
                 os._exit(1) 
                 
      def input_match_schemas(self):
          if((self.options.transportdirection    in ['TRANSPORT'] and
              self.options.sourcedbnode       == self.options.targetdbnode and 
              self.options.sourcedbname       == self.options.targetdbname and re.split(',',self.options.sourcedbuser)[0] == re.split(',',self.options.targetdbuser)[0] and
              self.options.sourcedbschema     == self.options.targetdbschema )):
              print '\n Database source and target connect parameters must not match ' 
              IMPORTMONITOR                    = None 
              self.options.targetdbnode        = ''
              self.options.targetdbname        = ''
              self.options.targetdbuser        = ''
              self.options.targetdbschema      = ''
              self.input_targetdatabase  ()
              self.input_targetdataschema()
          if(self.options.transportdirection not in ['TRANSPORT'] and
             self.options.sourcedbschema != self.options.targetdbschema ) :
             print ' Database source %s and target %s schema do not match' % (self.options.sourcedbschema, self.options.targetdbschema )
             self.options.targetdbschema  = ''                   
                  
                 
      def input_sourcedatapackage(self):
          input = ''
          global STEPID
          try:
              if  self.options.transportdirection in ['IMPORT']:
                  if(self.options.sourcedbnode    == '' or  
                     self.options.sourcedbname    == '' ):
                     STEPID = STEPID+1
                     print "\nStep %i: Enter the source database of the export package" % STEPID 
                  count   = 0
                  package = None
                  while package in [None]:
                        sourcedbnode = ''
                        sourcedbname = ''
                        while self.options.sourcedbnode in [''] or self.options.sourcedbname in ['']:   
                              sourcedbnode = raw_input     (' Database computer: ')
                              if sourcedbnode not in [''] and sourcedbnode[:1] not in ['%'] :
                                 sourcedbnode = '^' + sourcedbnode
                              sourcedbnode = string.replace(sourcedbnode,'%' ,'.*')
                              if sourcedbnode == ''  or string.upper(sourcedbnode)[:9] == 'LOCALHOST':
                                 if sourcedbnode == '':
                                    sourcedbnode  = socket.gethostname()
                                 else:
                                    sourcedbnode  = string.replace(input,input[:9],socket.gethostname())
                                 self.options.sourcedbnode = sourcedbnode
                              sourcedbname = raw_input     (' Database name    : ')
                              if sourcedbname[:1] not in ['%'] :
                                 sourcedbname = '^' + sourcedbname
                              sourcedbname = string.replace(sourcedbname,'%','.*') 
                              sourcedbname = string.upper  (sourcedbname) 
                              for (key,package) in self.package_name.items(): 
                                   #self.output(package)
                                   if(re.search(sourcedbnode,package.sourcedbnode) not in [None] and
                                      re.search(sourcedbname,package.sourcedbname) not in [None]    ) :
                                      self.options.sourcedbnode = sourcedbnode
                                      self.options.sourcedbname = sourcedbname
                              sourcedbnode = string.replace(sourcedbnode,'.*','%')
                              sourcedbname = string.replace(sourcedbname,'.*','%')
                              sourcedbnode = string.replace(sourcedbnode,'^' ,'' )
                              sourcedbname = string.replace(sourcedbname,'^' ,'' )
                              if self.options.sourcedbnode in [''] or self.options.sourcedbname in ['']:
                                 self.options.sourcedbnode = ''
                                 self.options.sourcedbname = ''
                                 print ' No export package exists matching source computer %s and database %s' % (sourcedbnode, sourcedbname)
                        input  = ''
                        if self.options.packageref     in ['']:
                           STEPID = STEPID+1
                           print "\nStep %i: Choose an export package \n"  % (STEPID)
                           print  " Export package  path            : %s"   % (self.options.packagepath)
                           print  " Source database computer [-nSRC]: %s"   % (sourcedbnode)
                           print  " Source database name     [-dSRC]: %s\n" % (sourcedbname)
                           self.update('IMPORT')
                           while input  in [''] or self.package_number.has_key(string.atoi(input,10)) == False :
                                 input  = raw_input("\nStep %i: "  % STEPID )
                           package      = self.package_number[string.atoi(input,10)]
                           package.packageguid     = ''
                           package.packagecreation = ''
                           self.input(package)
                           self.decompress()
                        if self.options.packageref not in ['']:
                           package = self.options.packageref
                           self.decompress()
          except EOFError:
                 os._exit(1)
      
      def input_targetdatabase(self):
          input = ''
          global STEPID
          global EXPORTMONITOR 
          global IMPORTMONITOR 
          IMPORTMONITOR = None
          try:
              username     = ''
              userpassword = ''
              if self.options.targetdbuser not in [''] :
                 try:
                    username     = re.split(',',self.options.targetdbuser)[0]
                    userpassword = re.split(',',self.options.targetdbuser)[1]
                    if userpassword == '*':
                       userpassword  = ''
                 except:
                    pass
              if  self.options.transportdirection in ['TRANSPORT','IMPORT']: 
                  if(self.options.targetdbnode    in [''] or 
                     self.options.targetdbname    in [''] or 
                     username                     in [''] or
                     userpassword                 in ['']    ):
                     STEPID = STEPID+1
                     print "\nStep %i: Enter the target database connection " % STEPID 
                     if self.options.targetdbnode not in [''] :
                        if string.upper(input)[:9] == 'LOCALHOST':
                           self.options.targetdbnode     = string.replace(input,input[:9],socket.gethostname())
                        print ' Database computer: %s' % self.options.targetdbnode
                     if self.options.targetdbname not in [''] :
                        print ' Database name    : %s' % self.options.targetdbname
                     if username             not in [''] :
                        print ' Database user    : %s' % username
                  while IMPORTMONITOR == None:
                        while self.options.targetdbnode in ['']:   
                              input = raw_input(' Database computer: ')
                              if input == '' or string.upper(input)[:9] == 'LOCALHOST':
                                 if input == '': 
                                    self.options.targetdbnode = socket.gethostname()
                                 else :
                                    self.options.targetdbnode = string.replace(input,input[:9],socket.gethostname())
                              else:
                                 self.options.targetdbnode = input
                        while self.options.targetdbname in ['']:   
                              input = raw_input(' Database name    : ') 
                              self.options.targetdbname = string.upper(input)
                        while username in ['']: 
                              username = raw_input(' Database user    : ') 
                              if (CASESENSITIVE == True and username != '') or (username != '' and username [0] != '"' and username[len(username)-1] != '"'):
                                 username  = string.upper(username)
                              elif username != '' :
                                 username = string.replace(username,'"','')
                              if username !=  re.split(',',self.options.sourcedbuser)[0]:
                                 print ' Database user of source %s and target %s do not match' %( re.split(',',self.options.sourcedbuser)[0],username)
                                 username = '' 
                        while userpassword in ['']:   
                              userpassword   = getpass.getpass()
                        self.options.targetdbuser = '%s,%s' %( username, userpassword )
                        if self.options.targetdbuser not in ['']:
                           username     = re.split(',',self.options.targetdbuser)[0]
                           userpassword = re.split(',',self.options.targetdbuser)[1]
                        self.options.targetdbuser = '%s,%s' %( username, userpassword )  
                        try:
                            IMPORTMONITOR = sql.connect( username, userpassword, self.options.targetdbname, self.options.targetdbnode )
                        except (sql.CommunicationError ) , err:
                            print 'Connect failed, try again:' , err
                            IMPORTMONITOR    = None 
                        except (sql.SQLError ) , err:  
                            print 'Connect failed, try again:' , err    
                            IMPORTMONITOR    = None 
                        if  IMPORTMONITOR    == None:
                            self.options.targetdbnode = ''
                            self.options.targetdbname = ''
                            self.options.targetdbuser = ''
                            username     = ''
                            userpassword = ''
                        exist_db (self.options, EXPORTMONITOR, IMPORTMONITOR)
                  try:
                     self.options.targetpackagepath = os.path.join(PACKAGEPATH, 'sdb','loader', 'packages', 'import', string.replace(self.options.targetdbnode,':','_'), self.options.targetdbname, self.options.packagename)
                     if os.path.isdir(self.options.targetpackagepath) == False:
                        os.makedirs  (self.options.targetpackagepath)
                  except EnvironmentError, err:
                      print 'Independent target datapath %s ' %err
          except EOFError:
                 os._exit(1)
                  
      def input_packagepart(self):
          input = ''
          global STEPID
          try:
              if( self.options.transportdirection in ['IMPORT'] and self.options.packagepart in ['SCHEMA','TABLE']) :
                  self.input_targetdataschema()
              if( self.options.transportdirection in ['TRANSPORT','EXPORT']  and
                  exist_db   (self.options, EXPORTMONITOR, IMPORTMONITOR) == True):
                 #DB
                 if(exist_dba(self.options, EXPORTMONITOR, IMPORTMONITOR) == True):
                    if self.options.packageref in ['']:
                       STEPID = STEPID+1 
                       print "\nStep %i: Choose what you want include into the package" % STEPID
                       CASE_1 =(""" [1] Complete database; one file per database\n"""
                                """ [2] Complete database; one file per database, excluding some tables \n\nStep %i: """) % STEPID 
                    while self.options.packageref in [''] :
                          input = raw_input( CASE_1 )
                          if input == '1':
                             self.options.packageref     = 'DB'
                             if TESTLOADER in ['FALSE']:
                                self.options.packagepart = 'DB'
                             MONITOR = False
                          if input == '2':
                             self.options.packageref     = 'DB'
                             if TESTLOADER in ['FALSE']:
                                self.options.packagepart = 'DB'
                             MONITOR = False
                             STEPID = STEPID+1 
                             print '\nStep %i: Enter the table names you want exclude from the package ' % STEPID
                             export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                             shutil.copy( os.path.join(self.options.sourcepackagepath,  '%s.config' % os.path.split(self.options.sourcepackagepath)[1]),
                                          os.path.join(self.options.sourcepackagepath,  '%s.config' % self.options.sourcedbname ))
                          if input == '3':
                             self.options.packageref     = 'USER'
                             self.options.packagepart    = 'USER'
                          if input == '4':
                             self.options.packageref     = 'USER'
                             self.options.packagepart    = 'USER'
                             STEPID = STEPID+1 
                             print '\nStep %i: Enter the table names you want exclude from the package ' % STEPID
                             export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                          if input == '5':
                             self.options.packageref     = 'USER'
                             self.options.packagepart    = 'TABLE'
                          if input == '6':
                             self.options.packageref     = 'USER'
                             self.options.packagepart    = 'TABLE'
                             STEPID = STEPID+1 
                             print '\nStep %i: Enter the table names you want exclude from the package ' % STEPID
                             export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                          if input == '7':
                             self.options.packageref     = 'TABLE'    
                             self.options.packagepart    = 'TABLE' 
                             STEPID = STEPID+1 
                             print '\nStep %i: Enter the table names you want include into the package ' % STEPID
                             export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                 #if exist_dba(self.options, EXPORTMONITOR, IMPORTMONITOR ) == False:
                 #USER,SCHEMA,TABLE
                 elif self.options.packageref in [''] or self.options.packagepart in [''] :
                    STEPID = STEPID+1 
                    input = ''
                    print "\nStep %i: Choose what you want include into the package" % STEPID
                    while self.options.packageref in [''] or self.options.packagepart in [''] :
                          if self.options.transportdirection == 'EXPORT':
                             if self.options.sourcedbschema in [re.split(',',self.options.sourcedbuser)[0],'']:
                                CASE_1 = (""" [1] Complete user  ; one file per user                         \n"""
                                          """ [2] Complete user  ; one file per user,  excluding some tables \n"""
                                          """ [3] Complete user  ; one file per table                        \n"""
                                          """ [4] Complete user  ; one file per table, excluding some tables \n"""
                                          """ [5] Complete schema; one file per schema                       \n"""
                                          """ [6] Complete schema; one file per schema,excluding some tables \n"""
                                          """ [7] Complete schema; one file per table                        \n"""
                                          """ [8] Complete schema; one file per table, excluding some tables \n"""
                                          """ [9] Schema   tables; one file per table   \n\nStep %i: """) % STEPID
                                input = raw_input(CASE_1)
                             if self.options.sourcedbschema not in [re.split(',',self.options.sourcedbuser)[0],'']:
                                CASE_2 = (""" [1] Complete schema; one file per schema                       \n"""
                                          """ [2] Complete schema; one file per schema,excluding some tables \n"""
                                          """ [3] Complete schema; one file per table                        \n"""
                                          """ [4] Complete schema; one file per table, excluding some tables \n"""
                                          """ [5] Schema   tables; one file per table   \n\nStep %i: """) % STEPID 
                                input = raw_input(CASE_2)
                                if input == '5':
                                   input  = '9'
                                if input == '4':
                                   input  = '8'
                                if input == '3':
                                   input  = '7'
                                if input == '2':
                                   input  = '6'
                                if input == '1':
                                   input  = '5'
                          if self.options.transportdirection == 'TRANSPORT':
                            #if self.options.sourcedbschema == re.split(',',self.options.sourcedbuser)[0] :
                             CASE_1 = (""" [1] Complete user                          \n"""
                                       """ [2] Complete user  , excluding some tables \n"""
                                       """ [3] Complete schema                        \n"""
                                       """ [4] Complete schema, excluding some tables \n"""
                                       """ [5] Schema   tables  \n\nStep %i: """) % STEPID 
                             input = raw_input(CASE_1)
                             if input == '5':
                                input  = '9'
                             if input == '4':
                                input  = '6'
                             if input == '3':
                                input  = '5'
                             '''
                             if self.options.sourcedbschema != re.split(',',self.options.sourcedbuser)[0]:
                                CASE_2 = (""" [1] Complete schema                        \n"""
                                          """ [2] Complete schema, excluding some tables \n"""
                                          """ [3] Some     tables  \n\nStep %i: """) % STEPID 
                                input = raw_input(CASE_2)
                                if input == '3':
                                   input  = '9'
                                if input == '2':
                                   input  = '6'
                                if input == '1':
                                   input  = '5'
                             '''
                          if input == '1':
                             self.options.packageref     = 'USER'
                             self.options.packagepart    = 'USER'
                          if input == '2':
                             self.options.packageref     = 'USER'
                             self.options.packagepart    = 'USER'
                             STEPID = STEPID+1 
                             print '\nStep %i: Enter the table names you want exclude from the package ' % STEPID
                             export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                             shutil.copy( os.path.join(self.options.sourcepackagepath,  '%s.config' % os.path.split(self.options.sourcepackagepath)[1]),
                                          os.path.join(self.options.sourcepackagepath,  '%s.config' % re.split (',',self.options.sourcedbuser)[0] ))
                          if input == '3':
                             self.options.packageref     = 'USER'
                             self.options.packagepart    = 'TABLE'
                             self.options.sourcedbschema = ''
                             self.options.targtedbschema = ''
                          if input == '4':
                             self.options.packageref     = 'USER'
                             self.options.packagepart    = 'TABLE'
                             self.options.sourcedbschema = ''
                             self.options.targtedbschema = ''
                             STEPID = STEPID+1 
                             print '\nStep %i: Enter the table names you want exclude from the package ' % STEPID
                             export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                          if input == '5' or input == '6':
                             self.options.packageref     = 'SCHEMA'
                             self.options.packagepart    = 'SCHEMA'
                             self.input_sourcedataschema()
                             self.input_targetdataschema()
                             if input == '6' :
                                STEPID = STEPID+1 
                                print '\nStep %i: Enter the table names you want exclude from the package ' % STEPID
                                export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                                shutil.copy( os.path.join(self.options.sourcepackagepath,  '%s.config' % os.path.split(self.options.sourcepackagepath)[1]),
                                             os.path.join(self.options.sourcepackagepath,  '%s.config' % self.options.sourcedbschema))
                          if input == '7' or input == '8' or input == '9':
                             self.options.packageref     = 'SCHEMA'
                             self.options.packagepart    = 'TABLE'
                             self.input_sourcedataschema()
                             self.input_targetdataschema()
                             if input == '8':
                                STEPID = STEPID+1 
                                print '\nStep %i: Enter the table names you want exclude from the package ' % STEPID
                                export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                                shutil.copy( os.path.join(self.options.sourcepackagepath,  '%s.config' % os.path.split(self.options.sourcepackagepath)[1]),
                                             os.path.join(self.options.sourcepackagepath,  '%s.config' % self.options.sourcedbschema ))
                          if input == '9':
                             self.options.packageref     = 'TABLE'    
                             self.options.packagepart    = 'TABLE' 
                             self.input_sourcedataschema()
                             self.input_targetdataschema()
                             STEPID = STEPID+1 
                             print '\nStep %i: Enter the table names you want include into the package ' % STEPID
                             export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                             self.options.sourcedbschema = ''
                 # command line parameters
                 elif(self.options.packageref  in ['USER']  and ( 
                      self.options.packagepart in ['USER']  or 
                      self.options.packagepart in ['TABLE'])and
                      self.options.mode        in ['INTER']     ):
                      STEPID = STEPID+1 
                      print '\nStep %i: Enter the table names you want exclude from the package ' % STEPID
                      export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
                 elif(self.options.packageref  in ['TABLE'] and 
                      self.options.packagepart in ['TABLE'] ):
                      STEPID = STEPID+1 
                      print "\nStep %i: Enter the table names you want include into the package" % STEPID
                      export_configuration(self.options, EXPORTMONITOR, IMPORTMONITOR)
          except EOFError:
                 os._exit(1)
                  
      def input_streamtypes(self):
          input =  ''
          global STEPID
          try:
              if self.options.streamformat_target == '':
                 STEPID = STEPID+1 
                 print "\nStep %i: Choose the transformation streams " %STEPID                   
                 while input not in ['1','2','3','4'] :  
                       input = raw_input(' [1] Catalog and data \n [2] Catalog\n [3] Data \n \nStep %i: ' %STEPID)  
                       if input == '1':
                          self.options.streamtypes = 'CATALOG,DATA,PACKAGE'
                       if input == '2':
                          self.options.streamtypes = 'CATALOG'
                          self.options.streamformat          = 'DDL'
                          self.options.streamformat_target   = 'DDL'
                       if input == '3':
                          self.options.streamtypes = 'DATA,PACKAGE'  
                       if input == '4':
                          self.options.streamtypes = 'PACKAGE'
                          if self.options.streamformat       == 'DDL':
                             self.options.streamformat_target = 'DDL'
                          elif self.options.streamformat     == 'XML':
                             self.options.streamformat_target = 'XML'
                          else:
                             self.options.streamformat_target = ''  
          except EOFError:
                 os._exit(1)
                         
      def input_streamformat(self):
          input = ''
          global STEPID
          try:
              streammedium = ''
              streamformat = ''
              streamformat_target = ''
              streamcharencoding  = ''
              streamlob           = ''
              if self.options.streammedium  in ['']:
                 if self.options.packageref in ['TABLE'] :    
                    self.options.packagepart = 'TABLE'                      
                 if self.options.transportdirection  in ['TRANSPORT'] :
                    streammedium                 = 'PIPE'
                 else:
                    streammedium                 = 'FILE'
              else:
                 streammedium = self.options.streammedium
              
              if self.options.streamformat in ['']:      
                 if  (streammedium               == 'PIPE' ) :
                      streamformat                = 'RECORDS'
                 elif(self.options.packageref    != 'TABLE' or
                      self.options.packagepart   != 'TABLE') :
                      streamformat                = 'RECORDS'
                 elif(self.options.sourcedbnode  == self.options.targetdbnode and
                      self.options.sourcedbname  == self.options.targetdbname and
                      self.options.packagepart   in ['TABLE']):
                      streamformat                = 'SQL'
                 else:
                      streamformat                = 'COMPRESSED'
              else:
                 streamformat = self.options.streamformat
                 
              if self.options.streamformat_target in ['']:
                 streamformat_target              = 'ROW'
              else:
                 streamformat_target = self.options.streamformat_target
              if self.options.streamcharencoding  in ['']:
                 streamcharencoding               = 'DYNAMIC'
              else:
                 streamcharencoding = self.options.streamcharencoding
              if self.options.streamlob in ['']:
                 if(self.options.packageref       != 'TABLE' and
                    self.options.packagepart      != 'TABLE') :               
                    streamlob                      = 'TABLE'
                 else:
                    streamlob                      = 'COLUMNVALUE'
              else:
                  streamlob = self.options.streamlob
              input = ''
              if( self.options.transportdirection not in ['TRANSPORT']    and 
                ( string.find(self.options.streamtypes   ,'DATA'   ) >= 0 or
                  string.find(self.options.streamtypes   ,'PACKAGE') >= 0    )):
                  STEPID = STEPID+1 
                  while input not in ['1','2'] and self.options.streamformat in [''] :  
                        print "\nStep %i: Do you want to specify the data streamformat" %STEPID   
                        input = raw_input(""" [1] Yes \n"""
                                          """ [2] No  \n\nStep %i: """ % STEPID)
                        if input == '1':
                           self.options.streammedium            = streammedium
                           input  = ''
                           STEPID = STEPID+1    
                           if(self.options.transportdirection == 'TRANSPORT'  and DB_DEFAULTCODE_SOURCE == DB_DEFAULTCODE_TARGET) or self.options.transportdirection == 'EXPORT':
                              CASE_1 =(""" [1] Platform independent loader records format (with    logging)\n"""       
                                       """ [2] Platform independent database page  format (without logging, backup necessary)\n \nStep %i: """) %STEPID
                              CASE_2 =(""" [1] Comma separated values; one file per lob column           \n"""
                                       """ [2] Comma separated values; one file per lob value            \n"""
                                       """ [3] Fixed width values;     one file per lob column           \n"""
                                       """ [4] Fixed width values;     one file per lob value            \n"""
                                       """ [5] Platform independent loader records format (with    logging)\n"""
                                       """ [6] Platform independent database page  format (without logging, backup necessary)\n"""
                                       """ [7] No format\nStep %i: """) %STEPID
                              CASE_3 =(""" [1] Comma separated values; one file per lob column           \n"""
                                       """ [2] Comma separated values; one file per lob value            \n"""
                                       """ [3] Fixed width values;     one file per lob column           \n"""
                                       """ [4] Fixed width values;     one file per lob value            \n"""
                                       """ [5] Platform independent loader records format (with    logging)\n"""
                                       """ [6] Platform independent database page  format (without logging, backup necessary)\n              \nStep %i: """) %STEPID
                           if self.options.transportdirection == 'TRANSPORT'  and DB_DEFAULTCODE_SOURCE != DB_DEFAULTCODE_TARGET:
                              CASE_1   = ' [1] Platform independent loader records format       \nStep %i: ' %STEPID
                              CASE_2 =(""" [1] Comma separated values; one file per lob column           \n"""
                                       """ [2] Comma separated values; one file per lob value            \n"""
                                       """ [3] Fixed width values;     one file per lob column           \n"""
                                       """ [4] Fixed width values;     one file per lob value            \n"""
                                       """ [5] Platform independent loader records format (with    logging)\n"""
                                       """ [6] No format\n   \nStep %i: """) %STEPID
                              CASE_3 =(""" [1] Comma separated values; one file per lob column           \n"""
                                       """ [2] Comma separated values; one file per lob value            \n"""
                                       """ [3] Fixed width values;     one file per lob column           \n"""
                                       """ [4] Fixed width values;     one file per lob value            \n"""
                                       """ [5] Platform independent loader records format (with    logging)\n     \nStep %i: """) %STEPID
                              
                           print "\nStep %i: How do you want to export the datastream" %STEPID       
                           if self.options.packagepart in ['DB'] or  self.options.packagepart in ['USER'] or self.options.packagepart in ['SCHEMA'] :
                              while self.options.streamformat in [''] :
                                    input = raw_input(CASE_1)
                                    if input == '1':
                                       self.options.streamformat        = 'RECORDS'
                                       self.options.streamformat_target = 'ROW'
                                    if input == '2':
                                       self.options.streamformat = 'PAGES'
                                       self.options.streamformat_target = 'PAGE'
                                       if self.options.transportdirection == 'TRANSPORT' and DB_DEFAULTCODE_SOURCE != DB_DEFAULTCODE_TARGET:
                                          self.options.streamformat = ''
                           input = ''
                           if self.options.packagepart in ['TABLE']:
                              if( self.options.sourcedbnode   == self.options.targetdbnode   and
                                  self.options.sourcedbname   == self.options.targetdbname ):
                                  while self.options.streamformat in [''] :
                                        input = raw_input(CASE_2)
                                        if input == '1':
                                           self.options.streamformat             = 'COMPRESSED'
                                           self.options.streamlob                = 'COLUMN'
                                           self.options.streamcharencoding       = 'DYNAMIC'
                                        if input == '2':				  
                                           self.options.streamformat             = 'COMPRESSED'
                                           self.options.streamlob                = 'COLUMNVALUE'
                                           self.options.streamcharencoding       = 'DYNAMIC'
                                        if input == '3':
                                           self.options.streamformat             = 'FORMATTED'
                                           self.options.streamlob                = 'COLUMN'
                                           if self.options.transportdirection   == 'TRANSPORT':
                                              if DB_DEFAULTCODE_TARGET          == 'UNICODE':
                                                 self.options.streamcharencoding = 'UTF8'
                                              if DB_DEFAULTCODE_TARGET          == 'ASCII':
                                                 self.options.streamcharencoding = 'ASCII'
                                        if input == '4':
                                           self.options.streamformat             = 'FORMATTED'
                                           self.options.streamlob                = 'COLUMNVALUE'
                                           if self.options.transportdirection   == 'TRANSPORT':
                                              if DB_DEFAULTCODE_TARGET          == 'UNICODE':
                                                 self.options.streamcharencoding = 'UTF8'
                                              if DB_DEFAULTCODE_TARGET          == 'ASCII':
                                                 self.options.streamcharencoding = 'ASCII'
                                        if input == '5':				   
                                           self.options.streamformat             = 'RECORDS'
                                           self.options.streamcharencoding       = 'DYNAMIC'
                                           self.options.streamformat_target      = 'ROW'
                                        if input == '6':				     
                                           self.options.streamformat             = 'PAGES'
                                           self.options.streamformat_target      = 'PAGE'
                                           if self.options.transportdirection   == 'TRANSPORT' and DB_DEFAULTCODE_SOURCE != DB_DEFAULTCODE_TARGET:
                                              self.options.streamformat = ''
                                        if input == '7':
                                           self.options.streamformat             = 'SQL'
                              else:
                                   while self.options.streamformat in [''] :
                                         input = raw_input(CASE_3)
                                         if input == '1':
                                            self.options.streamformat             = 'COMPRESSED'
                                            self.options.streamlob                = 'COLUMN'
                                            self.options.streamcharencoding       = 'DYNAMIC'
                                         if input == '2':
                                            self.options.streamformat             = 'COMPRESSED'
                                            self.options.streamlob                = 'COLUMNVALUE'
                                            self.options.streamcharencoding       = 'DYNAMIC'
                                         if input == '3':
                                            self.options.streamformat             = 'FORMATTED'
                                            self.options.streamlob                = 'COLUMN'
                                            if self.options.transportdirection   == 'TRANSPORT':
                                               if DB_DEFAULTCODE_TARGET          == 'UNICODE':
                                                  self.options.streamcharencoding = 'UTF8'
                                               if DB_DEFAULTCODE_TARGET          == 'ASCII':
                                                  self.options.streamcharencoding = 'ASCII'
                                            if self.options.transportdirection   == 'EXPORT':
                                               if DB_DEFAULTCODE_SOURCE          == 'UNICODE':
                                                  self.options.streamcharencoding = 'UTF8'
                                               if DB_DEFAULTCODE_SOURCE          == 'ASCII':
                                                  self.options.streamcharencoding = 'ASCII'
                                         if input == '4':
                                            self.options.streamformat             = 'FORMATTED'
                                            self.options.streamlob                = 'COLUMNVALUE'
                                            if self.options.transportdirection   == 'TRANSPORT':
                                               if DB_DEFAULTCODE_TARGET          == 'UNICODE':
                                                  self.options.streamcharencoding = 'UTF8'
                                               if DB_DEFAULTCODE_TARGET          == 'ASCII':
                                                  self.options.streamcharencoding = 'ASCII'
                                            if self.options.transportdirection   == 'EXPORT':
                                               if DB_DEFAULTCODE_SOURCE          == 'UNICODE':
                                                  self.options.streamcharencoding = 'UTF8'
                                               if DB_DEFAULTCODE_SOURCE          == 'ASCII':
                                                  self.options.streamcharencoding = 'ASCII'
                                         if input == '5':
                                            self.options.streamformat             = 'RECORDS'
                                            self.options.streamcharencoding       = 'DYNAMIC'
                                         if input == '6':
                                            self.options.streamformat             = 'PAGES'
                                            if self.options.transportdirection   == 'TRANSPORT' and DB_DEFAULTCODE_SOURCE != DB_DEFAULTCODE_TARGET:
                                               self.options.streamformat = ''
                           input = ''
                           STEPID = STEPID+1                            
                           while self.options.streamformat_target in [''] and self.options.streamformat != 'PAGES' and self.options.streamformat != 'RECORDS' :
                                 print "\nStep %i: How do you want to import the data" %STEPID   
                                 input = raw_input(""" [1] Row by row  (with    logging)\n"""
                                                   """ [2] Row by page (without logging, backup necessary) \n\nStep %i: """ % STEPID)
                                 if input == '1':
                                    self.options.streamformat_target = 'ROW'
                                 if input == '2':
                                    self.options.streamformat_target = 'PAGE'
                           if(string.find(self.options.streamtypes,'DATA')    <  0 and
                              string.find(self.options.streamtypes,'PACKAGE') >= 0  ):
                              self.options.streamformat_target = 'DTL'
                        if input == '2':
                           if self.options.streammedium        == '' :
                              self.options.streammedium         = streammedium
                           if self.options.streamformat        == '':
                              self.options.streamformat         = streamformat
                           if self.options.streamformat_target == '':
                              self.options.streamformat_target  = streamformat_target
                           if self.options.streamcharencoding  == '':
                              self.options.streamcharencoding   = streamcharencoding
                           if self.options.streamlob           == '':
                              self.options.streamlob            = streamlob
          except EOFError:
                 os._exit(1)
                         
      def input_transporttype(self):
          input = ''
          global STEPID
          try:
              if self.options.transporttype in ['']:
                 STEPID=STEPID+1
                 while self.options.transporttype in [''] :
                       print '\nStep %i: The package can be executed or generated now ' % STEPID
                       input = ''
                       while input not in ['1','2']:
                             input = raw_input(""" [1] Execute  package to transport data  now   \n"""
                                               """ [2] Generate package to transport data  later \n\nStep %i: """  % STEPID )
                             if input == '1':
                                self.options.transporttype           = 'EXECUTE' 
                             if input == '2':
                                self.options.transporttype           = 'GENERATE' 
          except EOFError:
                 os._exit(1) 
                 
      def input_release(self):
          global  EXPORTMONITOR
          global  IMPORTMONITOR
          try:
              if  EXPORTMONITOR != None:
                  EXPORTMONITOR.release()
              if  IMPORTMONITOR != None:   
                  IMPORTMONITOR.release()
          except  loader.CommunicationError, err :
            raise loader.CommunicationError, err, sys.exc_traceback     
     
                
class TransportPackage:
      def __init__(self, packagerecord = '', packageguid        = '', packagecreation = '', packageexecution = '', packagepath    = '', packagename = '',
                         transporttype = '', transportdirection = '', package         = '', part             = '', streams        = '', medium      = '',
                         format        = '', format_import      = '', coding          = '', lob              = '', archive        = '',
                         source_node   = '', source_db          = '', source_user     = '', source_schema    = '',
                         target_node   = '', target_db          = '', target_user     = '', target_schema    = '', packagecomment ='' ) :  

          self.packagerecord         = packagerecord
          self.packageguid           = string.strip(packageguid)
          self.packagecreation       = string.strip(packagecreation)
          self.packageexecution      = string.strip(packageexecution)
          self.packagepath           = string.strip(packagepath)
          self.packagename           = string.strip(packagename)
          self.transporttype         = string.strip(transporttype)
          self.transportdirection    = string.strip(transportdirection)
          self.packageref            = string.strip(package)
          self.packagepart           = string.strip(part)
          self.streamtypes           = string.strip(streams)
          self.streammedium          = string.strip(medium)
          self.streamformat          = string.strip(format)
          self.streamformat_target   = string.strip(format_import)
          self.streamcharencoding    = string.strip(coding)
          self.streamlob             = string.strip(lob)
          self.archive               = string.strip(archive)
          self.sourcedbnode          = string.strip(source_node)
          self.sourcedbname          = string.strip(source_db)   
          self.sourcedbuser          = string.strip(source_user)
          self.sourcedbschema        = string.strip(source_schema)
          self.targetdbnode          = string.strip(target_node)
          self.targetdbname          = string.strip(target_db)   
          self.targetdbuser          = string.strip(target_user)
          self.targetdbschema        = string.strip(target_schema)  
          self.packagecomment        = string.strip(packagecomment)
          
      def input(self,options):
          self.packageguid           = string.strip(options.packageguid)
          self.packagecreation       = string.strip(options.packagecreation)
          self.packageexecution      = string.strip(options.packageexecution)
          self.packagepath           = string.strip(options.packagepath)
          self.packagename           = string.strip(options.packagename)
          self.transporttype         = string.strip(options.transporttype)
          self.transportdirection    = string.strip(options.transportdirection)
          self.packageref            = string.strip(options.packageref)
          self.packagepart           = string.strip(options.packagepart)
          self.streamtypes           = string.strip(options.streamtypes)
          self.streammedium          = string.strip(options.streammedium)
          self.streamformat          = string.strip(options.streamformat)
          self.streamformat_target   = string.strip(options.streamformat_target)
          self.streamcharencoding    = string.strip(options.streamcharencoding)
          self.streamlob             = string.strip(options.streamlob)
          self.archive               = ''
          self.sourcedbnode          = string.strip(options.sourcedbnode)
          self.sourcedbname          = string.strip(options.sourcedbname)   
          self.sourcedbuser          = string.strip(options.sourcedbuser)
          self.sourcedbschema        = string.strip(options.sourcedbschema)
          self.targetdbnode          = string.strip(options.targetdbnode)
          self.targetdbname          = string.strip(options.targetdbname)   
          self.targetdbuser          = string.strip(options.targetdbuser)
          self.targetdbschema        = string.strip(options.targetdbschema)  
          self.packagecomment        = string.strip(options.packagecomment)
         
      def output(self):
          streamcharencoding        = self.streamcharencoding
          sourceusername            = ''
          targetusername            = ''
          packagecomment            = ''
          if self.streamcharencoding in ['DYNAMIC']:
             streamcharencoding = ''
          if self.transportdirection in ['TRANSPORT']:
             transformation = 'Transport'
             whitespaces    =        ''
          if self.transportdirection in ['EXPORT']:
             transformation = 'Export'
             whitespaces    =        '   '
          if self.transportdirection in ['IMPORT']:
             transformation = 'Import'
             whitespaces    =        '   '
          if self.streamformat       in ['COMPRESSED']:
             streamformat = 'CSV'
          if self.streamformat       in ['FORMATTED']:
             streamformat = 'FWV'
          if self.streamformat not   in ['COMPRESSED','FORMATTED']:
             streamformat = self.streamformat
          if self.sourcedbuser != '':
             sourceusername = re.split(',', self.sourcedbuser)[0]
          if (CASESENSITIVE == True  and   sourceusername != '') or ( sourceusername != '' and sourceusername [0] != '"' and sourceusername[len(sourceusername)-1] != '"'):
             sourceusername = string.upper(sourceusername)
          elif sourceusername != '':
             sourceusername = string.replace(sourceusername,'"','')    
          if self.targetdbuser != '':
             targetusername = re.split(',', self.targetdbuser)[0]
             targetpassword = re.split(',', self.targetdbuser)[1]
          if (CASESENSITIVE == True  and  targetusername != '') or ( targetusername != '' and targetusername [0] != '"' and targetusername[len(targetusername)-1] != '"'):
             targetusername = string.upper(targetusername)
          elif targetusername != '':
             targetusername = string.replace(targetusername,'"','') 
          if self.transportdirection in ['EXPORT']:
             if self.packagecomment     in [''] and self.sourcedbschema not in   [sourceusername, '']:
                packagecomment =  '%s.%s.%s.%s ' % (self.sourcedbnode, self.sourcedbname, sourceusername, self.sourcedbschema   )
             if self.packagecomment     in [''] and self.sourcedbschema     in   [sourceusername, '']:
                packagecomment =  '%s.%s.%s    ' % (self.sourcedbnode, self.sourcedbname, sourceusername  )
          if self.transportdirection in ['TRANSPORT','IMPORT']:
             if self.packagecomment     in [''] and self.sourcedbschema not in   [sourceusername, '']:
                packagecomment =  '%s.%s.%s.%s -> %s.%s.%s.%s ' % (self.sourcedbnode, self.sourcedbname, sourceusername, self.sourcedbschema, self.targetdbnode, self.targetdbname, targetusername, self.targetdbschema )
             if self.packagecomment     in [''] and self.sourcedbschema     in   [sourceusername, '']:
                packagecomment =  '%s.%s.%s -> %s.%s.%s       ' % (self.sourcedbnode, self.sourcedbname, sourceusername, self.targetdbnode, self.targetdbname, targetusername  )
         #print       ' %s history record     %s  : %s ' % (transformation, whitespaces, self.packagerecord)
         #print       ' %s package guid  %s       : %s ' % (transformation, whitespaces, self.packageguid)
          print       ' %s package name  %s       : %s ' % (transformation, whitespaces, self.packagename)
          if self.packagecomment not in ['']:
             print    ' %s package comment    %s  : %s ' % (transformation, whitespaces, self.packagecomment )
          if self.packagecomment     in ['']:
              print    ' %s package comment    %s  : %s ' %(transformation, whitespaces, packagecomment )
          print       ' %s package creation   %s  : %s ' % (transformation, whitespaces, self.packagecreation)
          print       ' %s package execution  %s  : %s ' % (transformation, whitespaces, self.packageexecution)
          print       ' %s package path       %s  : %s ' % (transformation, whitespaces, os.path.join(self.packagepath))
          print       ' %s package status%s   [-T]: %s ' % (transformation, whitespaces, self.transporttype)
          print       ' %s direction     %s   [-t]: %s ' % (transformation, whitespaces, self.transportdirection)
          print       ' %s package       %s   [-p]: %s ' % (transformation, whitespaces, self.packageref)
          print       ' %s part          %s   [-P]: %s ' % (transformation, whitespaces, self.packagepart)
          print       ' %s streams       %s   [-s]: %s ' % (transformation, whitespaces, self.streamtypes)
          print       ' %s format        %s   [-f]: %s ' % (transformation, whitespaces, streamformat)
          print       ' %s format        %s   [-F]: %s ' % (transformation, whitespaces, self.streamformat_target)
          print       ' %s encoding      %s   [-c]: %s ' % (transformation, whitespaces, streamcharencoding)
          print       ' %s lobcolumn     %s   [-l]: %s ' % (transformation, whitespaces, self.streamlob)
                    
          if self.sourcedbuser not in ['']:
             sourceusername = re.split(',', self.sourcedbuser)[0]
          if (CASESENSITIVE == True  and   sourceusername != '') or ( sourceusername != '' and sourceusername [0] != '"' and sourceusername[len(sourceusername)-1] != '"'):
             sourceusername = string.upper(sourceusername)
          elif sourceusername not in ['']:
             sourceusername = string.replace(sourceusername,'"','') 
          if self.targetdbuser not in ['']:
             targetusername = re.split(',', self.targetdbuser)[0]
          if (CASESENSITIVE == True and    targetusername != '') or (targetusername != '' and targetusername [0] != '"'  and targetusername[len(targetusername)-1] != '"'):
             targetusername = string.upper(targetusername)
          elif targetusername not in ['']:
             targetusername = string.replace(targetusername,'"','') 
          if self.transportdirection in ['EXPORT']:
             print    ' Source database computer[-nSRC]: %s ' % self.sourcedbnode     
             print    ' Source database name    [-dSRC]: %s ' % self.sourcedbname
             print    ' Source database user    [-uSRC]: %s ' % sourceusername
             if self.sourcedbschema not in [sourceusername, '']:
                print ' Source database schema  [-sSRC]: %s ' % self.sourcedbschema
          if self.transportdirection in ['TRANSPORT','IMPORT']:
             print    ' Source database computer[-nSRC]: %s ' % self.sourcedbnode    
             print    ' Source database name    [-dSRC]: %s ' % self.sourcedbname  
             print    ' Source database user    [-uSRC]: %s ' % sourceusername
             if self.sourcedbschema not in [sourceusername, '']:
                print ' Source database schema  [-sSRC]: %s ' % self.sourcedbschema
             print    ' Target database computer[-nTRG]: %s ' % self.targetdbnode     
             print    ' Target database name    [-dTRG]: %s ' % self.targetdbname
             print    ' Target database user    [-uTRG]: %s ' % targetusername
             if self.targetdbschema not in [targetusername, '']:
                print ' Target database schema  [-sTRG]: %s ' % self.targetdbschema
       
class TransformationPackage (sax.ContentHandler):
      # transformationpackage model2xml
      def __init__(self, options = None ):
          self.options                 = options
          self.packageguid             = self.options.packageguid
          self.sourcepackagename       = os.path.split(self.options.sourcepackagepath)[1]
          self.targetpackagename       = os.path.split(self.options.targetpackagepath)[1]
          self.sourceconfiguration     = os.path.join (self.options.sourcepackagepath,  '%s.config' % os.path.split(options.sourcepackagepath)[1])
          self.targetconfiguration     = os.path.join (self.options.targetpackagepath,  '%s.config' % os.path.split(options.targetpackagepath)[1])
          self.starttime               = None
          self.stoptime                = None
          #package   
          self.transformation          = None
          self.packageref              = None
          self.packagereader           = None
          self.packagedom              = None
          self.transformationparameter = None
          self.transformation_step     = []
          self.maxerrorcount           = 0
          #db                        
          self.sapdbnode               = None
          self.sapdb                   = None
          #user  
          self.maxstepid               = 0
          self.stepid                  = None                   
          self.user                    = None
          self.pwd                     = None
          self.usercount               = 0
          self.schema                  = ''
          #table                     
          self.table                   = None
          self.tablecount              = 0
          self.index                   = None
          self.indexStatement          = ''
          self.indexStatements         = []
          self.trigger                 = None
          self.triggerstatement        = None
          #column                    
          self.column                  = ''
          self.columnIsNullable        = ''
          self.columnInitialValue      = ''
          self.columnEncoding          = ''
          self.columnEncodingDynamic   = 'ASCII'
          self.columnLength            = 0
          self.columncount             = 0
          self.keycolumn               = None
          self.column_long             = []
          #field 
          self.fieldCompressed         = FIELDCOMPRESSED
          self.fieldDecimalValue       = FIELDDECIMALVALUE
          self.fieldNullValue          = FIELDNULLVALUE
          self.fieldBoolValue          = FIELDBOOLVALUE
          self.fieldDateValue          = FIELDATEVALUE
          self.fieldTimeValue          = FIELDTIMEVALUE
          self.fieldTimeStampValue     = FIELDTIMESTAMPVALUE                    
          self.field_startpos          = 1
          self.field_endpos            = 1
          #transformation            
          self.column_field            = ''
          self.column_field_long       = ''
          self.statement               = ''
          if self.options.streamcharencoding == 'DYNAMIC':
             self.options.streamcharencoding  = ''
          
          self.init()

      def init(self):
          input   = ''
          global STEPID
          self.transformation = Transformation( self.options, self )
          if self.transformation not in [None] and self.cmd_export():
             self.starttime   = time.time()
          else:
             self.transformation = None
          if self.transformation  not in [None] and self.cmd_import() :
             if self.options.mode not in ['PARAM'] :
                STEPID = STEPID+1
                while input  not in ['1','2'] :
                      print '\nStep %i: Continue '   % STEPID
                      input = raw_input(' [1] OK\n [2] CANCEL\n\nStep %i: ' %STEPID )
                if input in ['2']:
                   self.transformation = None
                   print 'Execution of transport canceled by user'          
          else:
             self.transformation = None
             print 'Execution of transport canceled'
         
          return self.transformation
             
      # model transport
      def cmd_export(self):
          rc = True
          if self.options.transportdirection in ['IMPORT'] :
             if os.path.isfile             ( os.path.join(self.options.sourcepackagepath, '%s.xml'% os.path.split(self.options.sourcepackagepath)[1] )) == False:
                print 'Package %s.xml does not exist ' % os.path.split(self.options.sourcepackagepath)[1]
                rc = False
             else:
                self.packageref = os.path.join(self.options.sourcepackagepath, '%s.xml' % os.path.split(self.options.sourcepackagepath)[1])  
          if self.options.transportdirection in ['TRANSPORT','EXPORT'] and self.options.packageguid not in ['']:   
             if self.options.packageref      in ['DB','USER','SCHEMA','TABLE']:
                if os.path.isfile( os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1] )) == False:
                   pass
                if os.path.isfile( os.path.join(self.options.sourcepackagepath, '%s.xml'           % os.path.split(self.options.sourcepackagepath)[1] )) == False:
                   print 'Package %s.xml does not exist ' % os.path.split(self.options.sourcepackagepath)[1]
                   rc = False
                else:
                   self.packageref = os.path.join(self.options.sourcepackagepath, '%s.xml' % os.path.split(self.options.sourcepackagepath)[1])                       
             else:
                self.packageref    = os.path.join( os.path.split(self.options.sourcepackagepath)[0], self.options.packageref,  '%s.xml' % self.options.packageref )                 
          if self.options.transportdirection in ['TRANSPORT','EXPORT'] and self.options.packageguid     in ['']:
             if self.options.packageref      in ['DB','USER','SCHEMA','TABLE']:
                protocol =  self.transformation.exporter.protocol
                self.fieldCompressed     = self.transformation.exporter.fieldCompressed     
                self.fieldDecimalValue   = self.transformation.exporter.fieldDecimalValue     
                self.fieldNullValue      = self.transformation.exporter.fieldNullValue
                self.fieldBoolValue      = self.transformation.exporter.fieldBoolValue
                self.fieldDateValue      = self.transformation.exporter.fieldDateValue     
                self.fieldTimeValue      = self.transformation.exporter.fieldTimeValue     
                self.fieldTimeStampValue = self.transformation.exporter.fieldTimeStampValue 
                self.transformation.exporter.verbose  = False 
                self.transformation.exporter.execute  = True
                
                if self.options.sourcedbschema not in ['']:
                   packageuser = 'USE USER %s %s SCHEMA %s SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1], self.options.sourcedbschema, self.options.sourcedbname, self.options.sourcedbnode )
                else:
                   packageuser = 'USE USER %s %s           SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1],                              self.options.sourcedbname, self.options.sourcedbnode )                
                self.transformation.exporter.cmd  = packageuser
                self.transformation.exporter.cmd_execute()
                  
                # Transport Package and Transport Part
                configuration = "SET CONFIGURATION EXCLUDE \n"
                notable       = ''
                if self.options.packageref in ['DB'] :
                   configuration = "SET CONFIGURATION EXCLUDE \n"      
                   if self.options.packagepart in ['TABLE'] or TESTLOADER == 'TRUE':
                      notable = ''
                   else:
                      notable = ' NO TABLE '
                   package       = "EXPORT DB \n"
                   if os.path.isfile(os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1])) == True:
                      package    = package + " CONFIGURATION INSTREAM FILE '%s'            %s\n"    % (os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1]) , 'ASCII' ) 
                   package       = package + " CATALOG      OUTSTREAM FILE '%s' XML        %s\n"    % (os.path.join(self.options.sourcepackagepath, '%s.xml'           % os.path.split(self.options.sourcepackagepath)[1]) , notable )
                   if notable   == '':
                      package    = package + " PACKAGE      OUTSTREAM FILE '%s'              \n"    % (os.path.join(self.options.sourcepackagepath, '%s.pkg'       % os.path.split(self.options.sourcepackagepath)[1]))
                if self.options.packageref in ['USER'] :   
                   configuration = "SET CONFIGURATION EXCLUDE \n"      
                   if self.options.packagepart in ['TABLE'] or TESTLOADER == 'TRUE':
                      notable = ''
                   else:
                      notable = ' NO TABLE '  
                   package       = "EXPORT USER \n"
                   if os.path.isfile(os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1])) == True:
                      package    = package + " CONFIGURATION INSTREAM FILE '%s'            %s \n"   % (os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1]) , 'ASCII' )
                   package       = package + " CATALOG      OUTSTREAM FILE '%s' XML        %s \n"   % (os.path.join(self.options.sourcepackagepath, '%s.xml'           % os.path.split(self.options.sourcepackagepath)[1]) , notable )
                   if notable   == '':
                      package    = package + " PACKAGE      OUTSTREAM FILE '%s'               \n"   % (os.path.join(self.options.sourcepackagepath, '%s.pkg'       % os.path.split(self.options.sourcepackagepath)[1]))
                if self.options.packageref in ['SCHEMA'] : 
                   if self.options.packagepart in ['TABLE'] or TESTLOADER == 'TRUE':
                      notable = ''
                   else:
                      notable = ' NO TABLE '  
                   schemaname = ''
                   if self.options.sourcedbuser not in [''] and self.options.sourcedbschema not in ['']:
                      username     = re.split(',',self.options.sourcedbuser)[0]
                      userpassword = re.split(',',self.options.sourcedbuser)[1]
                      if (CASESENSITIVE == True and username != '') or (username != '' and username [0] != '"' and username[len(username)-1] != '"'):
                         username  = string.upper(username)
                      elif username != '' :
                         username = string.replace(username,'"','')  
                      schemaname = username 
                   if self.options.sourcedbschema not in ['']:
                      schemaname = self.options.sourcedbschema
                   configuration = "SET CONFIGURATION EXCLUDE \n"      
                   package       = "EXPORT SCHEMA %s  \n"                                           % (schemaname)
                   if os.path.isfile(os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1])) == True:
                      package    = package + " CONFIGURATION INSTREAM FILE '%s'            %s \n"   % (os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1]) , 'ASCII' )
                   package       = package + " CATALOG      OUTSTREAM FILE '%s' XML        %s \n"   % (os.path.join(self.options.sourcepackagepath, '%s.xml'           % os.path.split(self.options.sourcepackagepath)[1]) , notable )
                   if notable   == '':
                      package    = package + " PACKAGE      OUTSTREAM FILE '%s'               \n"   % (os.path.join(self.options.sourcepackagepath, '%s.pkg'       % os.path.split(self.options.sourcepackagepath)[1]))
                if self.options.packageref in ['TABLE'] and self.options.packagepart in ['TABLE']:
                   packages = []
                   packagetablefile = os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1])
                   if os.path.isfile( packagetablefile ) :
                      packagetablefilehandle = file(packagetablefile,'r')  
                      packagecollection = csv.reader(packagetablefilehandle)
                      for package in packagecollection :
                          packages.append(package)
                      if len(packages) == 1:
                         schemaname = packages[0][0]
                         tablename =  packages[0][1]
                         package   = 'EXPORT TABLE "%s"."%s"  \n' % (schemaname,tablename)
                         package   = package + " CATALOG      OUTSTREAM FILE '%s' XML           \n" % (os.path.join(self.options.sourcepackagepath, '%s.xml'           % os.path.split(self.options.sourcepackagepath)[1]))
                         package   = package + " PACKAGE      OUTSTREAM FILE '%s'               \n" % (os.path.join(self.options.sourcepackagepath, '%s.pkg'       % os.path.split(self.options.sourcepackagepath)[1]))
                      else: 
                         configuration = "SET CONFIGURATION INCLUDE \n" 
                         package   = "EXPORT USER \n" 
                         if os.path.isfile(os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1])) == True:
                           package = package + " CONFIGURATION INSTREAM FILE '%s'            %s \n" % (os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1]) , 'ASCII' )
                         package   = package + " CATALOG      OUTSTREAM FILE '%s' XML           \n" % (os.path.join(self.options.sourcepackagepath, '%s.xml'           % os.path.split(self.options.sourcepackagepath)[1]))
                         package   = package + " PACKAGE      OUTSTREAM FILE '%s'               \n" % (os.path.join(self.options.sourcepackagepath, '%s.pkg'       % os.path.split(self.options.sourcepackagepath)[1]))
                
                self.transformation.exporter.cmd      = configuration
                self.transformation.exporter.cmd_execute()
                self.transformation.exporter.cmd      = package
                self.transformation.exporter.cmd_execute()
                self.transformation.exporter.verbose  = True
                self.transformation.exporter.execute  = False
                if os.path.isfile( os.path.join(self.options.sourcepackagepath, '%s.config' % os.path.split(self.options.sourcepackagepath)[1] )) == False:
                   pass
                if os.path.isfile( os.path.join(self.options.sourcepackagepath, '%s.xml'           % os.path.split(self.options.sourcepackagepath)[1] )) == False:
                   print 'Package %s.xml does not exist ' % os.path.split(self.options.sourcepackagepath)[1]
                   rc = False
                else:
                   self.packageref = os.path.join(self.options.sourcepackagepath, '%s.xml' % os.path.split(self.options.sourcepackagepath)[1])                       
             else:
                self.packageref    = os.path.join( os.path.split(self.options.sourcepackagepath)[0], self.options.packageref,  '%s.xml' % self.options.packageref )
          return rc

      # model import
      def cmd_import (self): 
          rc = True
          if VERBOSE == True:
             droptables = []
             if  os.path.isfile(self.packageref) and  os.stat(self.packageref)[stat.ST_SIZE] <= 2000000 :
                 exportmonitor = None
                 importmonitor = None
                 if self.transformation.exporter != None:
                    exportmonitor = self.transformation.exporter.monitor
                 if self.transformation.importer != None:
                    importmonitor = self.transformation.importer.monitor
                 try:
                    packagedom   = parse( self.packageref )
                    packageguids = packagedom.getElementsByTagName('oim:TransferHeader')
                    for packageguid in packageguids:
                        self.packageguid = packageguid.getAttribute ('TransferPackageGuid') 
                    schemas   = packagedom.getElementsByTagName('sdbm:DeployedSchema')
                    for schema in schemas:
                        if( self.options.packagepart in ['TABLE']  ):
                            schemaname =  '"' + schema.getAttribute ('oim:name') + '"'
                            tables     =  schema.getElementsByTagName('sdbm:DeployedTable')
                            for table in tables:
                                tablename = '"' + table.getAttribute('oim:name') + '"'
                                rc = exist_table(self.options,exportmonitor,importmonitor,schemaname,tablename)
                                if rc == True :
                                   if self.options.transportdirection in ['EXPORT'] :
                                      print ' Source database table          : %s.%s' % (schemaname,tablename)
                                   if self.options.transportdirection in ['TRANSPORT','IMPORT'] :
                                      varchars = table.getElementsByTagName('sdbm:VARCHAR')
                                      for varchar in varchars : 
                                          if rc == True:
                                             columnEncoding = varchar.getAttribute('sdbm:Encoding')
                                             if columnEncoding== 'UNICODE' and  DB_CODE_TARGET == 'ASCII':
                                                print ' Source database table          : %s.%s contains UNICODE encoded character columns that can not be imported in an ASCII database' %  (schemaname,tablename)
                                                rc = False
                                      chars = table.getElementsByTagName('sdbm:CHAR')
                                      for char in chars : 
                                          if rc == True:
                                             columnEncoding = char.getAttribute('sdbm:Encoding')
                                             if columnEncoding== 'UNICODE' and  DB_CODE_TARGET == 'ASCII':
                                                print ' Source database table          : %s.%s contains UNICODE encoded character columns that can not be imported in an ASCII database' %  (schemaname,tablename)
                                                rc = False
                                      if rc == True :
                                         print ' Source database table          : %s.%s' % (schemaname,tablename)
                                else:
                                   droptables.append(tablename)
                                   rc = False
                    packagedom.unlink()
                 except:
                    pass
             else:
                 print 'Filesize of %s to large to display ' % self.packageref
             
          return rc  
          
      def output (self):
          streamcharencoding        = self.options.streamcharencoding
          if self.options.streamcharencoding in ['DYNAMIC']:
             streamcharencoding = ''
          if self.options.transportdirection in ['TRANSPORT']:
             transformation = 'Transport'
          if self.options.transportdirection in ['EXPORT']:
             transformation = 'Export'
             whitespaces    =        '   '
          if self.options.transportdirection in ['IMPORT']:
             transformation = 'Import'
             whitespaces    =        '   '
          if self.options.streamformat       in ['COMPRESSED']:
             streamformat = 'CSV'
          if self.options.streamformat       in ['FORMATTED']:
             streamformat = 'FWV'
          if self.options.streamformat not   in ['COMPRESSED','FORMATTED']:
             streamformat = self.options.streamformat
          
         #print       ' %s package source %s      : %s ' % (transformation, whitespaces, self.options.packagesource)                
         #print       ' %s history record     %s  : %s ' % (transformation, whitespaces, self.options.packagerecord)
         #print       ' %s package guid  %s       : %s ' % (transformation, whitespaces, self.options.packageguid)
          print       ' %s package name  %s       : %s ' % (transformation, whitespaces, self.options.packagename)
          print       ' %s package comment    %s  : %s ' % (transformation, whitespaces, self.options.packagecomment )
          print       ' %s package creation   %s  : %s ' % (transformation, whitespaces, self.options.packagecreation)
          print       ' %s package execution  %s  : %s ' % (transformation, whitespaces, self.options.packageexecution)
          print       ' %s package path       %s  : %s ' % (transformation, whitespaces, self.options.packagepath)
          print       ' %s package status%s   [-T]: %s ' % (transformation, whitespaces, self.options.transporttype)
          print       ' %s direction     %s   [-t]: %s ' % (transformation, whitespaces, self.options.transportdirection)
          print       ' %s package       %s   [-p]: %s ' % (transformation, whitespaces, self.options.packageref)
          print       ' %s part          %s   [-P]: %s ' % (transformation, whitespaces, self.options.packagepart)
          print       ' %s streams       %s   [-s]: %s ' % (transformation, whitespaces, self.options.streamtypes)
          print       ' %s format        %s   [-f]: %s ' % (transformation, whitespaces, streamformat)
          print       ' %s format        %s   [-F]: %s ' % (transformation, whitespaces, self.options.streamformat_target)
          print       ' %s encoding      %s   [-c]: %s ' % (transformation, whitespaces, streamcharencoding)
          print       ' %s lobcolumn     %s   [-l]: %s ' % (transformation, whitespaces, self.options.streamlob)
          
          if self.options.sourcedbuser != '':
             sourceusername = re.split(',', self.options.sourcedbuser)[0]
          if (CASESENSITIVE == True  and   sourceusername != '') or ( sourceusername != '' and sourceusername [0] != '"' and sourceusername[len(sourceusername)-1] != '"'):
             sourceusername = string.upper(sourceusername)
          elif sourceusername != '':
             sourceusername = string.replace(sourceusername,'"','')    
          if self.options.targetdbuser != '':
             targetusername = re.split(',', self.options.targetdbuser)[0]
             targetpassword = re.split(',', self.options.targetdbuser)[1]
          if (CASESENSITIVE == True and targetusername != '') or (targetusername != '' and targetusername [0] != '"' and targetusername[len(targetusername)-1] != '"'):
             targetusername = string.upper(targetusername)
          elif targetusername != '':
             targetusername = string.replace(targetusername,'"','') 
          if self.options.transportdirection in ['TRANSPORT','EXPORT']:
             print    ' Source database computer[-nSRC]: %s ' % self.options.sourcedbnode     
             print    ' Source database name    [-dSRC]: %s ' % self.options.sourcedbname
             print    ' Source database user    [-uSRC]: %s ' % sourceusername
             if self.options.sourcedbschema not in [sourceusername,'']:
                print ' Source database schema  [-sSRC]: %s ' % self.options.sourcedbschema
          if self.options.transportdirection in ['TRANSPORT','IMPORT']:
             print    ' Source database computer[-nSRC]: %s ' % self.options.sourcedbnode    
             print    ' Source database name    [-dSRC]: %s ' % self.options.sourcedbname  
             print    ' Source database user    [-uSRC]: %s ' % sourceusername
             print    ' Source database schema  [-sSRC]: %s ' % self.options.sourcedbschema
             print    ' Target database computer[-nTRG]: %s ' % self.options.targetdbnode     
             print    ' Target database name    [-dTRG]: %s ' % self.options.targetdbname
             print    ' Target database user    [-uTRG]: %s ' % targetusername
             if self.options.targetdbschema not in [targetusername,'']:
                print ' Target database schema  [-sTRG]: %s ' % self.options.targetdbschema
      
      def release(self):
          global  EXPORTMONITOR
          global  IMPORTMONITOR
          try:
              if  EXPORTMONITOR != None:
                  EXPORTMONITOR.release()
              if  IMPORTMONITOR != None:   
                  IMPORTMONITOR.release()
          except sql.CommunicationError, err :
              pass
          except:
              pass
          self.transformation.release()
      
      # model parser
      def startElement (self, name, attrs):
          #raw_input('stop %s' %name)
          #oim:TransferHeader
          if name == 'oim:TransferHeader':
             self.packageguid            = attrs.get('TransferPackageGuid', None)  
             if self.options.packageguid in [''] :
                self.options.packageguid = self.packageguid
             if VERBOSE == False:
                print       'Transformation  package guid      : %s '  % self.options.packageguid
                print       'Transformation  package execution : %s\n' % self.options.packageexecution
          #Datasource
          if name == 'dbm:DataSource':
             self.startDataSource()
          #Connection
          if name == 'sdbm:Connection':
             connectString =  map(string.strip, string.split(attrs.get('dbm:ConnectString', None),";"))
             pos           = 1
             for i in connectString:
                 if pos == 1 :
                    pass
                 if pos == 2 :
                    self.sapdbnode  =  ''
                 if pos == 3 :
                    self.sapdb      =  self.options.sourcedbname
                 if pos == 4 :
                    self.user       =  re.split(',',self.options.sourcedbuser)[0]
                 if pos == 5 :
                    self.pwd        = 'initial'
                 pos = pos + 1
          #Catalog
          if name == 'dbm:DeployedCatalog':
             if(self.options.packagepart in ['DB']):
                self.startCatalog()
          #CatalogSchemas
          if name == 'dbm:CatalogSchemas' :
             if(self.options.packagepart in ['USER'] ):
                self.startCatalogSchemas()
          #Schema
          if name == 'sdbm:DeployedSchema' :
             if self.options.packagepart in ['USER','SCHEMA','TABLE'] and TESTLOADER == 'TRUE':
                self.user      = attrs.get('sdbm:Username',None )
                self.schema    = attrs.get('oim:name',None )
                self.options.sourcedbuser     = '%s,%s' % ( self.user, self.pwd )
                self.options.sourcedbschema   = '%s'    % ( self.schema )
                self.options.targetdbuser     = '%s,%s' % ( self.user, self.pwd )
                self.options.targetdbschema   = '%s'    % ( self.schema )
             if self.options.packagepart in ['SCHEMA','TABLE']:
                if  self.options.transportdirection    in ['TRANSPORT','EXPORT']:
                    if self.options.sourcedbschema not in [''] :
                       if(self.options.packageref      in ['SCHEMA'] and
                          self.options.packagepart     in ['SCHEMA','TABLE' ]):
                          self.schema = self.options.sourcedbschema                          
                       else:
                          self.schema = attrs.get('oim:name',None )
                          self.options.sourcedbschema = ''
                          self.options.targetdbschema = ''
                    else:
                       self.schema    = attrs.get('oim:name',None )
                if  self.options.transportdirection    in ['IMPORT']:
                    if self.options.targetdbschema not in ['']:
                       if(self.options.packagepart     in ['SCHEMA']):
                          self.schema = self.options.targetdbschema
                       else:
                          self.schema = attrs.get('oim:name',None )
                          self.options.sourcedbschema = ''
                          self.options.targetdbschema = ''
                    else:
                       self.schema    = attrs.get('oim:name',None )
                self.user             = attrs.get('sdbm:Username',None )
             self.startSchema()
          #Table
          if(self.options.packagepart in ['TABLE'] and (
             name == 'sdbm:DeployedTable' )) :
             self.table            = attrs.get('oim:name', None)
             self.startTable()
          #Index   
          if(self.options.packagepart in ['TABLE'] and (
             name == 'dbm:Index')):
             self.index            = attrs.get('dbm:name', None)
             self.indexStatement   = 'CREATE '
             if attrs.get('dbm:IsUnique',None) == '1':
                self.indexStatement= self.indexStatement + ' UNIQUE '
             self.indexStatement = self.indexStatement   + ' INDEX '   
             self.indexStatement = self.indexStatement   + ' "%s" ON "%s" ' % (self.index, self.table )
             self.indexStatement = self.indexStatement   + ' ( '
          if(self.options.packagepart in ['TABLE'] and (
             name == 'dbm:IndexColumn' )) :
             self.indexStatement   = self.indexStatement + ' "%s" ' % attrs.get('dbm:name', None)
             if attrs.get('dbm:IsAscending',None) == '0':
                self.indexStatement= self.indexStatement + ' DESC,'
             if attrs.get('dbm:IsAscending',None) == '1':
                self.indexStatement= self.indexStatement + ' ASC,'
          if(self.options.packagepart in ['TABLE'] and 
             name == 'dbm:DeployedTrigger'):
             trigger               = attrs.get('oim:id', None)
             self.triggerstatement = attrs.get('dbm:Statements', None )
             string.replace( trigger, '_%s.' % self.user  , '' )
             string.replace( trigger, '.%s'  % self.table , '' )
             self.trigger = trigger
          #Columns
          if(self.options.packagepart in ['TABLE'] and (
             name == 'dbm:DeployedColumn' )): 
             self.column            = attrs.get('dbm:name', None)
             self.columnIsNullable  = attrs.get('dbm:IsNullable'  ,None)
             self.columnInitialValue= attrs.get('dbm:initialValue',None)
             self.columnLength      = attrs.get('dbm:Length',None)
             self.startColumn()
          if self.options.packagepart in ['TABLE'] and name == 'dbm:KeyColumns' :
             self.keycolumn = 'true'
          #Column Types   
          if(self.options.packagepart in ['TABLE'] and (
             name == 'sdbm:BOOLEAN')):
             if self.keycolumn != 'true':
                self.columnTypeBool(name)
          if(self.options.packagepart in ['TABLE'] and (
             name == 'sdbm:CHAR'   or 
             name == 'sdbm:VARCHAR'or
             name == 'sdbm:BINARY' )):
             if self.keycolumn != 'true':
                if(name == 'sdbm:CHAR'    or 
                   name == 'sdbm:VARCHAR'   ):
                   self.columnEncoding = attrs.get('sdbm:Encoding', None) 
                self.columnTypeCharacter(name)
          if(self.options.packagepart in ['TABLE'] and (
             name == 'sdbm:SMALLINT'or
             name == 'sdbm:INTEGER' or
             name == 'sdbm:DECIMAL' or
             name == 'sdbm:FIXED'   or
             name == 'sdbm:FLOAT'   or
             name == 'sdbm:REAL'    or
             name == 'sdbm:DOUBLE_PRECISION' or
             name == 'sdbm:NUMERIC' )):
             if self.keycolumn != 'true':
                self.columnTypeNumber(name)
          if(self.options.packagepart in ['TABLE'] and (
             name == 'sdbm:DATE' or
             name == 'sdbm:TIME' or
             name == 'sdbm:TIMESTAMP' )):
             if self.keycolumn != 'true':
                self.columnTypeDateTime(name)
          if(self.options.packagepart in ['TABLE'] and (                
             name in ['sdbm:LONG'     ,'sdbm:BLOB'] or
             name in ['sdbm:LONG_BYTE','sdbm:CLOB']  )):
             if self.keycolumn != 'true':
                self.columnTypeLong(name)

      def endElement(self, name ):
          #DataSource
          if name == 'dbm:DataSource':
             self.endDataSource()
          #Catalog
          if name == 'dbm:DeployedCatalog' :
             self.endCatalog()
          #CatalogSchemas
          if name == 'dbm:CatalogSchemas' :
             self.endCatalogSchemas()
          #Schema
          if name == 'sdbm:DeployedSchema':
             self.endSchema()
          #Table
          if(self.options.packagepart in ['TABLE'] and 
             name == 'sdbm:DeployedTable') :
             self.endTable ()
          #Index 
          if(self.options.packagepart in ['TABLE'] and 
             name == 'dbm:Index'):
             self.endIndex()
          #Trigger
          if(self.options.packagepart in ['TABLE'] and 
             name == 'dbm:DeployedTrigger'):
             self.endTrigger()
          #Columns
          if(self.options.packagepart in ['TABLE'] and 
             name == 'dbm:DeployedColumn'):
             self.endColumn()
          if(self.options.packagepart in ['TABLE'] and 
             name == 'dbm:KeyColumns') :
             self.endKeyColumns()
      
      # model execution
      def execute  (self):
          global   STEPID
          global   DB_DEFAULTCODE_SOURCE
          global   DB_DEFAULTCODE_TARGET
          try:
             input   = None
             self.options.mode  = 'PARAM' 
             if os.path.isfile( self.packageref ) :
                try:
                   packageparser = sax.make_parser()
                   packageparser.setContentHandler( self ) 
                   # Transformation Package Execution
                   package = file(self.packageref,'r')
                   packageparser.parse (package)
                   package.close()
                   self.stoptime  = time.time()  
                   print   ' Package execution time : %.3fs' % float( self.stoptime - self.starttime )
                except:
                   pass
             else:
                 print '\nPackage %s not found \n ' % self.packageref
          except IOError , (errno,err):
            rc = False
            print '\nError parsing the package information \n  %s : %s ' % ( err, self.packageref )
          except:
            rc = False
            kind, val, traceback = sys.exc_info()
            raise kind, val, traceback
      
      # model package validation
      def cmd_verify(self):
          self.transformation.cmd_verify_package()
      
      # model package statistic
      def cmd_statistic(self):
          print ' Package execution time : %.3fs ' % float( self.stoptime - self.starttime )
          print ' Package execution error: %i '    % self.maxerrorcount
      
      # model package archive    
      def archive(self):
          if PACKAGEARCHIVE == 'ZIP':
             if self.options.transportdirection  in ['TRANSPORT','EXPORT']:
                 try:
                     zipdirname  = os.path.split(self.options.sourcepackagepath)[0]
                     zipfilename = os.path.split(self.options.sourcepackagepath)[1]+os.extsep+'zip'
                     zip =  zipfile.ZipFile(os.path.join (zipdirname,zipfilename), 'w', zipfile.ZIP_DEFLATED )  
                     print ' Package archive        : %s ' % zipfilename
                     os.chdir(self.options.sourcepackagepath)
                     for filename in os.listdir(os.getcwd()):
                         zip.write(filename)
                     zip.close()
                     #VALIDATE PACKAGE
                     zip =  zipfile.ZipFile(os.path.join (zipdirname,zipfilename), 'r', zipfile.ZIP_DEFLATED )  
                     if zip.testzip() == None:
                        zip.close()
                        os.chdir(self.options.sourcepackagepath)
                        for filename in os.listdir(os.getcwd()):
                            os.unlink(filename)
                        os.chdir (os.path.split(self.options.sourcepackagepath)[0])
                        time.sleep(1)
                        os.removedirs(self.options.sourcepackagepath)
                     else:
                        print 'Not ok'
                 except RuntimeError, err:
                     print err
                 except OSError, (errno, err ):
                     if errno != 13:
                        print err
                 except IOError, (errno, err ):
                     print err
                 except:
                     kind, val, traceback = sys.exc_info()
                     raise kind, val, traceback
             if  self.options.transportdirection in ['TRANSPORT','IMPORT']:
                 try:
                     zipdirname  = os.path.split(self.options.targetpackagepath)[0]
                     zipfilename = os.path.split(self.options.targetpackagepath)[1]+os.extsep+'zip'
                     zip =  zipfile.ZipFile(os.path.join (zipdirname,zipfilename), 'w', zipfile.ZIP_DEFLATED )  
                     if  self.options.transportdirection  == 'IMPORT':
                         print ' Package archive        : %s ' % zipfilename
                     os.chdir(self.options.targetpackagepath)
                     for filename in os.listdir(os.getcwd()):
                         zip.write(filename)
                     zip.close()
                     #VALIDATE PACKAGE
                     zip =  zipfile.ZipFile(os.path.join (zipdirname,zipfilename), 'r', zipfile.ZIP_DEFLATED )  
                     if zip.testzip() == None:
                        zip.close()
                        os.chdir(self.options.targetpackagepath)
                        for filename in os.listdir(os.getcwd()):
                            os.unlink(filename)
                        os.chdir (os.path.split(self.options.targetpackagepath)[0])
                        time.sleep(1)
                        os.removedirs(self.options.targetpackagepath)
                     else:
                        print 'Not ok'
                 except RuntimeError, err:
                     print err
                 except OSError, (errno, err ):
                     if errno != 13:
                        print err
                 except IOError, (errno, err ):
                     print err
                 except:
                     kind, val, traceback = sys.exc_info()
                     raise kind, val, traceback
          if PACKAGEARCHIVE == 'TAR':
             if  self.options.transportdirection in ['TRANSPORT','EXPORT']:
                 try:
                     zipdirname  = os.path.split(self.options.sourcepackagepath)[0]
                     zipfilename = os.path.split(self.options.sourcepackagepath)[1]+os.extsep+'tgz'
                     zip =  tarfile.TarFile.open(os.path.join (zipdirname,zipfilename), 'w:gz' )  
                     print ' Package archive        : %s ' % zipfilename
                     os.chdir(self.options.sourcepackagepath)
                     for filename in os.listdir(os.getcwd()):
                         zip.add(filename)
                     zip.close()
                 except RuntimeError, err:
                     print err
                 except OSError, (errno, err ):
                     if errno != 13:
                        print err
                 except IOError, (errno, err ):
                     print err
                 except:
                     kind, val, traceback = sys.exc_info()
                     raise kind, val, traceback      
             if  self.options.transportdirection in ['TRANSPORT','IMPORT']:
                 try:
                     zipdirname  = os.path.split(self.options.targetpackagepath)[0]
                     zipfilename = os.path.split(self.options.targetpackagepath)[1]+os.extsep+'tgz'
                     zip =  tarfile.TarFile.open(os.path.join (zipdirname,zipfilename), 'w:gz' ) 
                     if  self.options.transportdirection  == 'IMPORT':
                         print ' Package archive        : %s ' % zipfilename
                     os.chdir(self.options.targetpackagepath)
                     for filename in os.listdir(os.getcwd()):
                         zip.add(filename)
                     zip.close()
                     #VERIFY PACKAGE
                     if tarfile.is_tarfile(os.path.join (zipdirname,zipfilename)) == True:
                        zip.close()
                        os.chdir(self.options.targetpackagepath)
                        for filename in os.listdir(os.getcwd()):
                            os.unlink(filename)
                        os.chdir (os.path.split(self.options.targetpackagepath)[0])
                        time.sleep(1)
                        os.removedirs(self.options.targetpackagepath)
                     else:
                        print 'Not ok'
                 except RuntimeError, err:
                     print err
                 except OSError, (errno, err ):
                     if errno != 13:
                        print err
                 except IOError, (errno, err ):
                     print err
                 except:
                     kind, val, traceback = sys.exc_info()
                     raise kind, val, traceback
                
      def cmd_dearchive(self):
          if PACKAGEARCHIVE == 'ZIP':
             if((self.options.transportdirection   == 'TRANSPORT' or
                 self.options.transportdirection   == 'IMPORT')   and
                 self.options.streammedium         == 'FILE'):         
                 try:
                     zipdirname  = os.path.split(self.options.sourcepackagepath)[0]
                     zipfilename = os.path.split(self.options.sourcepackagepath)[1]+os.extsep+'zip'
                     zip =  zipfile.ZipFile(os.path.join (zipdirname,zipfilename), 'r', zipfile.ZIP_DEFLATED )
                     if zip.testzip() == None:
                        print ' Package archive        : %s ' % zipfilename
                        os.chdir(self.options.sourcepackagepath)
                        for filename in zip.namelist():
                            unzip        = file (filename,'wb')
                            unzip.write(zip.read(filename))
                            unzip.close()
                        zip.close()
                        os.chdir(os.path.split(self.options.sourcepackagepath)[0])
                     else:
                        print 'Not ok'
                 except IOError, err:
                     print err
                 except:
                     kind, val, traceback = sys.exc_info()
                     raise kind, val, traceback

      # model element callback functions      
      def len (self, name):
          length = 0
          if self.options.streamcharencoding == 'ASCII' :
             length = len(name)
          if self.options.streamcharencoding == 'UTF8' :
             length = len(name)
          if self.options.streamcharencoding == 'UCS2' :
             length = 2*len(name)
          return length   
          
      def int (self, name): 
          length = 0
          if self.options.streamcharencoding == 'ASCII' :
             length = int(name,10)
          if self.options.streamcharencoding == 'UTF8' :
             length = int(name,10)   
          if self.options.streamcharencoding == 'UCS2' :
             length = 2*int(name,10)
          return length
          
      def transformation_statement (self ):
          return self.transformation.cmd_statement()    
          
      def startDataSource(self):
          global EXPORTER_VERSION
          global IMPORTER_VERSION
          self.transformation.cmd_stamp_protocol_start() 
          if  self.options.transportdirection in ['TRANSPORT','EXPORT']:
              #version
              self.transformation.exporter.cmd = 'VERSION\n'
              self.transformation.exporter.execute = True
              version = self.transformation.exporter.cmd_execute()
              self.transformation.exporter.execute = False
              try:
                 EXPORTER_VERSION = string.replace(re.split(',',re.split('\n',version)[1])[0],'"','')
              except:
                 pass
              #protocol loader
              self.transformation.exporter.cmd = 'SET\n'
              configuration = self.transformation.exporter.cmd_execute()
              if configuration != 0:
                 loaderpackages = re.split('Loader packages:', configuration)[0]
                 loaderprotocol = string.replace(loaderpackages,'Loader protocol:','')
                 loaderprotocol = string.strip(loaderprotocol)
                 loaderprotocol = string.replace(loaderprotocol,'\'','')
                 self.transformation.exporter.protocol =  loaderprotocol
              self.transformation.exporter.cmd  = "SET CONFIGURATION   EXCLUDE \n" 
              self.transformation.exporter.cmd_execute()   
              #sqlmode
              self.transformation.exporter.cmd  = "SET CHECKCONSTRAINT OFF \n" 
              self.transformation.exporter.cmd_execute()   
              self.transformation.exporter.cmd = 'SQLMODE INTERNAL\n'
              self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = 'SET SYSTEMATTRIBUTE  %s\n' % SYSTEMATTRIBUTE
              self.transformation.exporter.cmd_execute()
              #transaction
              self.transformation.exporter.cmd = 'AUTOCOMMIT           %s\n' % AUTOCOMMIT
              self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = 'SET TRANSACTION SIZE %s\n' % TRANSACTIONSIZE
              self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = 'SET ISOLATION LEVEL  %s\n' % ISOLATIONLEVEL
              self.transformation.exporter.cmd_execute()              
              self.transformation.exporter.cmd = 'SET MAXERRORCOUNT    %s\n' % MAXERRORCOUNT
              self.transformation.exporter.cmd_execute()
              #print      'SET PACKAGETIMEOUT  %s\n' % PACKAGETIMEOUT
              #format
              self.transformation.exporter.cmd = "SET COMPRESSED       '%s'\n" % FIELDCOMPRESSED
              self.fieldCompressed            = FIELDCOMPRESSED
              self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = "SET LANGUAGE         'ENG'\n"
              self.transformation.exporter.cmd_execute()
              #datatypes
              #self.transformation.exporter.cmd = 'SET CODETYPE         ASCII\n'
              #self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = 'SET CODEPAGE         ISO-8859-1\n'
              self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = "SET BOOLEAN           %s\n" % FIELDBOOLVALUE
              self.fieldBoolValue            =  FIELDBOOLVALUE
              self.transformation.exporter.cmd_execute()
              #self.transformation.exporter.cmd = "SET DECIMAL         '%s'\n" % FIELDDECIMALVALUE
              #self.fieldDecimalValue        =  FIELDDECIMALVALUE
              #self.transformation.exporter.cmd_execute()
              self.fieldDecimalValue         =  FIELDDECIMALVALUE
              self.transformation.exporter.cmd = "SET NULL              '%s'\n" % FIELDNULLVALUE
              self.fieldNullValue            =  FIELDNULLVALUE
              self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = 'SET DATE             %s\n' % FIELDATEVALUE
              self.fieldDateValue            = FIELDATEVALUE
              self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = 'SET TIME             %s\n' % FIELDTIMEVALUE
              self.fieldTimeValue            = FIELDTIMEVALUE
              self.transformation.exporter.cmd_execute()
              self.transformation.exporter.cmd = 'SET TIMESTAMP        %s\n' % FIELDTIMESTAMPVALUE
              self.fieldTimeStampValue       = FIELDTIMESTAMPVALUE
              self.transformation.exporter.cmd_execute()
              #medium
              self.transformation.exporter.cmd = 'SET BACKUPTOOL       NETWORKER\n'
              self.transformation.exporter.cmd_execute()
          if  self.options.transportdirection  in ['TRANSPORT','IMPORT']:
              #version
              self.transformation.importer.cmd = 'VERSION\n'
              self.transformation.importer.execute = True
              version = self.transformation.importer.cmd_execute()
              self.transformation.importer.execute = False
              try:
                 IMPORTER_VERSION = string.replace(re.split(',',re.split('\n',version)[1])[0],'"','')
              except:
                 pass
              #protocol loader
              self.transformation.importer.cmd = 'SET\n'
              configuration = self.transformation.importer.cmd_execute()
              if configuration != 0:
                 loaderpackages = re.split('Loader packages:', configuration)[0]
                 loaderprotocol = string.replace(loaderpackages,'Loader protocol:','')
                 loaderprotocol = string.strip(loaderprotocol)
                 loaderprotocol = string.replace(loaderprotocol,'\'','')
                 self.transformation.importer.protocol =  loaderprotocol
              self.transformation.importer.cmd  = "SET CONFIGURATION   EXCLUDE \n" 
              self.transformation.importer.cmd_execute()   
              #sqlmode
              self.transformation.importer.cmd  = "SET CHECKCONSTRAINT OFF \n" 
              self.transformation.importer.cmd_execute()   
              self.transformation.importer.cmd = 'SQLMODE INTERNAL\n'
              self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = 'SET SYSTEMATTRIBUTE  %s\n' % SYSTEMATTRIBUTE
              self.transformation.importer.cmd_execute()
              #transaction
              self.transformation.importer.cmd = 'AUTOCOMMIT           %s\n' % AUTOCOMMIT
              self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = 'SET TRANSACTION SIZE %s\n' % TRANSACTIONSIZE
              self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = 'SET ISOLATION LEVEL  %s\n' % ISOLATIONLEVEL
              self.transformation.importer.cmd_execute()              
              self.transformation.importer.cmd = 'SET MAXERRORCOUNT    %s\n' % MAXERRORCOUNT
              self.transformation.importer.cmd_execute()
              #print      'SET PACKAGETIMEOUT  %s\n' % PACKAGETIMEOUT
              #format
              self.transformation.importer.cmd = "SET COMPRESSED       '%s'\n" % FIELDCOMPRESSED
              self.fieldCompressed            = FIELDCOMPRESSED
              self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = "SET LANGUAGE         'ENG'\n"
              self.transformation.importer.cmd_execute()
              #datatypes
              #self.transformation.importer.cmd = 'SET CODETYPE         ASCII\n'
              #self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = 'SET CODEPAGE         ISO-8859-1\n'
              self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = "SET BOOLEAN           %s\n" % FIELDBOOLVALUE
              self.fieldBoolValue            =  FIELDBOOLVALUE
              self.transformation.importer.cmd_execute()
              #self.transformation.importer.cmd = "SET DECIMAL         '%s'\n" % FIELDDECIMALVALUE
              #self.fieldDecimalValue        =  FIELDDECIMALVALUE
              #self.transformation.importer.cmd_execute()
              self.fieldDecimalValue         =  FIELDDECIMALVALUE
              self.transformation.importer.cmd = "SET NULL              '%s'\n" % FIELDNULLVALUE
              self.fieldNullValue            =  FIELDNULLVALUE
              self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = 'SET DATE             %s\n' % FIELDATEVALUE
              self.fieldDateValue            = FIELDATEVALUE
              self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = 'SET TIME             %s\n' % FIELDTIMEVALUE
              self.fieldTimeValue            = FIELDTIMEVALUE
              self.transformation.importer.cmd_execute()
              self.transformation.importer.cmd = 'SET TIMESTAMP        %s\n' % FIELDTIMESTAMPVALUE
              self.fieldTimeStampValue       = FIELDTIMESTAMPVALUE
              self.transformation.importer.cmd_execute()
              #medium
              self.transformation.importer.cmd = 'SET BACKUPTOOL       NETWORKER\n'
              self.transformation.importer.cmd_execute()
          
      def startCatalog(self):
          if  self.options.transportdirection      in ['TRANSPORT','EXPORT']:
              if self.options.sourcedbschema   not in ['']:
                 sourcedbuser = 'USE USER %s %s SCHEMA %s SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1], self.options.sourcedbschema, self.options.sourcedbname, self.options.sourcedbnode )
              if self.options.sourcedbschema       in ['']:
                 sourcedbuser = 'USE USER %s %s           SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1],                              self.options.sourcedbname, self.options.sourcedbnode )                 
              try:
                  self.transformation.exporter.cmd  = sourcedbuser
                  self.transformation.exporter.cmd_execute ()
                  self.transformation.exporter.user = re.split(',',self.options.sourcedbuser)[0]
                  self.transformation.exporter.schema = self.options.sourcedbschema
              except loader.LoaderError, err:
                  print 'command failed:' , err

          if  self.options.transportdirection      in ['TRANSPORT','IMPORT']:
              if self.options.targetdbschema   not in ['']:
                 targetdbuser = 'USE USER %s %s SCHEMA %s SERVERDB %s ON %s \n' % ( re.split(',',self.options.targetdbuser)[0],re.split(',',self.options.targetdbuser)[1], self.options.targetdbschema, self.options.targetdbname, self.options.targetdbnode )
              if self.options.targetdbschema       in ['']:
                 targetdbuser = 'USE USER %s %s           SERVERDB %s ON %s \n' % ( re.split(',',self.options.targetdbuser)[0],re.split(',',self.options.targetdbuser)[1],                              self.options.targetdbname, self.options.targetdbnode )                 
              try :
                  self.transformation.importer.cmd  = targetdbuser
                  self.transformation.importer.cmd_execute ()
                  self.transformation.importer.user = re.split(',',self.options.targetdbuser)[0]
                  self.transformation.importer.schema = self.options.targetdbschema
              except loader.LoaderError, err:
                  print 'command failed:' , err  
      
      def startCatalogSchemas(self):
          #Source
          if  self.options.transportdirection        in ['TRANSPORT','EXPORT']:
              self.transformation.exporter.cmd =  ''
              if  self.options.packagepart           in ['USER','TABLE']:
                  #Source
                  if self.options.sourcedbschema not in ['']:
                     cmd = 'USE USER %s %s SCHEMA %s  SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1] , self.options.sourcedbschema , self.options.sourcedbname, self.options.sourcedbnode)
                  if self.options.sourcedbschema     in ['']:
                     cmd = 'USE USER %s %s            SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1] ,                               self.options.sourcedbname, self.options.sourcedbnode)
                  try :
                      self.transformation.exporter.cmd  = cmd
                      self.transformation.exporter.cmd_execute ()
                      self.transformation.exporter.user = re.split(',',self.options.sourcedbuser)[0]
                  except loader.LoaderError, err:
                      print 'command failed:' , err
          #Target
          if  self.options.transportdirection          in ['TRANSPORT','IMPORT']:
              self.transformation.importer.cmd =  ''
              if  self.options.packagepart             in ['USER','TABLE']:
                  if self.options.targetdbschema   not in ['']:
                     cmd = 'USE USER %s %s SCHEMA %s SERVERDB %s ON %s \n' % ( re.split(',',self.options.targetdbuser)[0], re.split(',',self.options.targetdbuser)[1], self.options.targetdbschema , self.options.targetdbname, self.options.targetdbnode )
                  if self.options.targetdbschema       in ['']:
                     cmd = 'USE USER %s %s           SERVERDB %s ON %s \n' % ( re.split(',',self.options.targetdbuser)[0], re.split(',',self.options.targetdbuser)[1],                               self.options.targetdbname, self.options.targetdbnode )                                            
                  try :
                      self.transformation.importer.cmd  = cmd
                      self.transformation.importer.cmd_execute ()
                      self.transformation.importer.user = re.split(',',self.options.targetdbuser)[0]
                  except loader.LoaderError, err:
                      print 'command failed:' , err
          time.sleep(1)         
      def startSchema(self):
          #Source
          if  self.options.transportdirection          in ['TRANSPORT','EXPORT']:
              self.transformation.exporter.cmd     = ''
              #Source
              if  self.options.packagepart             in ['USER','SCHEMA','TABLE']:
                  if   self.options.sourcedbschema not in ['']:
                       cmd = 'USE USER %s %s SCHEMA %s  SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1] , self.options.sourcedbschema , self.options.sourcedbname, self.options.sourcedbnode)
                  elif self.schema != '':
                       cmd = 'USE USER %s %s SCHEMA %s  SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1] , self.schema                 , self.options.sourcedbname, self.options.sourcedbnode)
                  elif self.options.sourcedbschema     in ['']:
                       cmd = 'USE USER %s %s            SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1] ,                               self.options.sourcedbname, self.options.sourcedbnode)
                  elif self.schema == '':
                       cmd = 'USE USER %s %s            SERVERDB %s ON %s \n' % ( re.split(',',self.options.sourcedbuser)[0], re.split(',',self.options.sourcedbuser)[1] ,                               self.options.sourcedbname, self.options.sourcedbnode)                       
                  try :
                      self.transformation.exporter.cmd  = cmd
                      self.transformation.exporter.cmd_execute ()
                      self.transformation.exporter.user = re.split(',',self.options.sourcedbuser)[0]
                  except loader.LoaderError, err:
                      print 'command failed:' , err
          #Target
          if  self.options.transportdirection           in ['TRANSPORT','IMPORT']:
              self.transformation.importer.cmd =  ''
              if  self.options.packagepart              in ['USER','SCHEMA','TABLE']:
                  if   self.options.targetdbschema  not in ['']:
                       cmd = 'USE USER %s %s SCHEMA %s SERVERDB %s ON %s \n' % ( re.split(',',self.options.targetdbuser)[0], re.split(',',self.options.targetdbuser)[1], self.options.targetdbschema , self.options.targetdbname, self.options.targetdbnode )
                  elif self.schema != '':
                       cmd = 'USE USER %s %s SCHEMA %s SERVERDB %s ON %s \n' % ( re.split(',',self.options.targetdbuser)[0], re.split(',',self.options.targetdbuser)[1], self.schema                 , self.options.targetdbname, self.options.targetdbnode )                       
                  elif self.options.targetdbschema      in ['']:
                       cmd = 'USE USER %s %s           SERVERDB %s ON %s \n' % ( re.split(',',self.options.targetdbuser)[0], re.split(',',self.options.targetdbuser)[1],                               self.options.targetdbname, self.options.targetdbnode )
                  elif self.schema == '':
                       cmd = 'USE USER %s %s           SERVERDB %s ON %s \n' % ( re.split(',',self.options.targetdbuser)[0], re.split(',',self.options.targetdbuser)[1],                               self.options.targetdbname, self.options.targetdbnode )                       
                  try :
                      self.transformation.importer.cmd  = cmd
                      self.transformation.importer.cmd_execute ()
                      self.transformation.importer.user = re.split(',',self.options.targetdbuser)[0]
                  except loader.LoaderError, err:
                      print 'command failed:' , err
          time.sleep(1) 
      
      def startTable(self):
          self.statement         = ''
          self.indexStatement    = ''
          self.indexStatements   = []
          self.tablecount        = self.tablecount +1
          self.columncount       = 0
          self.statement_column  = ''
          self.field_startpos    = 1
          self.field_endpos      = 1
          self.column            = ''
          self.column_field      = ''
          self.column_field_long = ''
          self.column_long       = []
          if self.transformation.exporter != None:
             self.transformation.exporter.cmd = ''
          if self.transformation.importer != None:
             self.transformation.importer.cmd = ''
          
      def startColumn(self): 
          self.columncount       = self.columncount + 1   
          self.columnEncoding    = ''            
          
      def endDataSource(self):
          self.transformation.cmd_stamp_protocol_end()
          if self.options.packagepart  in ['DB']:
             mutex.acquire()                                           #acquire
             self.cmd_verify()                                         #release
          self.transformation.release()
           
      def endCatalog(self):
          if self.options.packagepart  in ['DB']   :
             mutex.acquire()                                           #acquire
             self.transformation_statement()
             input  = None
             #Format
             if(self.options.streamformat        == 'XML'     or
                self.options.streamformat        == 'DDL'     or
                self.options.streamformat        == 'PAGES'   or
                self.options.streamformat        == 'RECORDS'  ):
                #Type
                if self.options.transportdirection   == 'TRANSPORT':
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Transport DB %s ? [y]/[n] : ' % ( self.sapdb ) )
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print          'Transport DB %s\n' % ( self.sapdb )
                             input  = "y"
                          if input == "y":
                             self.transformation.cmd_transport_catalog_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()
                if(self.options.transportdirection   == 'EXPORT'  and
                    self.options.streammedium        == 'FILE'     ):
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Export    DB %s ? [y]/[n] : ' % ( self.sapdb ))
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print          'Export    DB %s\n' % ( self.sapdb )
                             input = "y"
                          if input == "y":
                             self.transformation.cmd_export_catalog_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()
                if(self.options.transportdirection   == 'IMPORT' and
                    self.options.streammedium        == 'FILE'    ):
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Import    DB %s ? [y]/[n] : ' % (self.sapdb ))
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print          'Import    DB %s\n' % (self.sapdb )
                             input = "y"
                          if input == "y":
                             self.transformation.cmd_import_catalog_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()                        #release
             self.user = '' 
      
      def endCatalogSchemas(self):
          if(self.options.packagepart  in ['USER']    and
            self.user              not in ['DBA']     and
            TESTLOADER             not in ['TRUE']  ) :
            mutex.acquire()                                           #acquire
            self.transformation_statement()
            input  = None
            #Format
            if( self.options.streamformat        == 'XML'     or
                self.options.streamformat        == 'DDL'     or
                self.options.streamformat        == 'PAGES'   or
                self.options.streamformat        == 'RECORDS'  ):
                #Type
                if self.options.transportdirection    == 'TRANSPORT':
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Transport User %s ? [y]/[n] : ' % ( self.user ) )
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print        '\nTransport User %s' % ( self.user )
                             input = "y"
                          if input == "y":
                             self.transformation.cmd_transport_schema_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()
                if( self.options.transportdirection    == 'EXPORT'  and
                    self.options.streammedium          == 'FILE'     ):
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Export    User   %s ? [y]/[n] :  ' % ( self.user ))
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print        '\nExport    User   %s' % ( self.user )
                             input = "y"
                          if input == "y":
                             self.transformation.cmd_export_schema_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()
                if( self.options.transportdirection     == 'IMPORT' and
                    self.options.streammedium           == 'FILE'    ):
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Import    User   %s ? [y]/[n] :  ' % (self.user ))
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print        '\nImport    User   %s' % (self.user )
                             input = "y"
                          if input == "y":
                             self.transformation.cmd_import_schema_package()
                          if input == "n":
                             if mutex.locked() == True :               #release
                                mutex.release()
            self.user = ''        
      def endSchema(self):
          if(self.options.packagepart in ['SCHEMA'] or 
            (self.options.packagepart in ['USER'  ] and 
             TESTLOADER in ['TRUE'])):
            mutex.acquire()                                           #acquire
            self.transformation_statement()
            input  = None
            #Format
            if( self.options.streamformat        == 'XML'     or
                self.options.streamformat        == 'DDL'     or
                self.options.streamformat        == 'PAGES'   or
                self.options.streamformat        == 'RECORDS'  ):
                #Type
                if self.options.transportdirection    == 'TRANSPORT':
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Transport schema %s ? [y]/[n] : ' % ( self.schema ) )
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print          'Transport schema %s ' % ( self.schema )
                             input = "y"
                          if input == "y":
                             self.transformation.cmd_transport_schema_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()
                if( self.options.transportdirection    == 'EXPORT'  and
                    self.options.streammedium          == 'FILE'     ):
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Export    schema   %s ? [y]/[n] :  ' % ( self.schema ))
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print          'Export    schema   %s ' % ( self.schema )
                             input = "y"
                          if input == "y":
                             self.transformation.cmd_export_schema_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()
                if( self.options.transportdirection     == 'IMPORT' and
                    self.options.streammedium           == 'FILE'    ):
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Import    schema   %s ? [y]/[n] :  ' % (self.schema ))
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print          'Import    schema   %s' % (self.schema )
                             input = "y"
                          if input == "y":
                             self.transformation.cmd_import_schema_package()
                          if input == "n":
                             if mutex.locked() == True :               #release
                                mutex.release()
            self.user = '' 
            
      def endTable(self):
          if self.options.packagepart   in ['TABLE'] :
             mutex.acquire()                                          #acquire
             if self.options.streamformat  not in ['SQL']:
                self.transformation_statement()
             input  = None
             #Format
             if(self.options.streamformat      in ['XML','SQL','DDL','PAGES','RECORDS','COMPRESSED']  or
                self.options.streamformat[0:9] in ['FORMATTED'] ) :
                #Type
                if self.options.transportdirection    == 'TRANSPORT':
                   while input not in ["y","n"]:
                         if self.options.mode == 'INTER':
                            input = raw_input('Transport Table %s ? [y]/[n] : ' % ( self.table ) )
                         if self.options.mode == 'PARAM':
                            if VERBOSE == False:
                               print          'Transport Table %s' % ( self.table )
                            input  = "y"
                         if input == "y":
                            self.transformation.cmd_transport_table_package()
                         if input == "n":
                            if mutex.locked() == True:
                               mutex.release()
                if(self.options.transportdirection    == 'EXPORT'  and
                    self.options.streammedium         == 'FILE'     ):
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Export    Table %s ? [y]/[n] : ' % ( self.table ))
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print          'Export    Table "%s"."%s" ' % ( self.schema,self.table )
                             input  = "y"
                          if input == "y":
                             self.transformation.cmd_export_table_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()
                if(self.options.transportdirection     == 'IMPORT' and
                    self.options.streammedium          == 'FILE'    ):
                    while input not in ["y","n"]:
                          if self.options.mode == 'INTER':
                             input = raw_input('Import    Table %s ? [y]/[n] : ' % (self.table ))
                          if self.options.mode == 'PARAM':
                             if VERBOSE == False:
                                print          'Import    Table %s' % (self.table )
                             input  = "y"
                          if input == "y":
                             self.transformation.cmd_import_table_package()
                          if input == "n":
                             if mutex.locked() == True:
                                mutex.release()                       #release
                              
      def endIndex(self):
          self.indexStatement = self.indexStatement   + ') '
          self.indexStatement = string.replace(self.indexStatement,',)',')\n')
          self.indexStatements.append(self.indexStatement)
          '''
          if(self.options.packagepart   in ['TABLE'] and self.options.streamformat not in ['PAGES'] and self.options.streamformat_target in ['PAGE','DTL'] and  self.column_field_long not in [''] and self.transformation.importer not in [None]) :
             self.transformation.importer.cmd  = 'DROP INDEX   \"%s\" ON \"%s\".\"%s\" \n' % (self.index, self.schema, self.table)
             self.transformation.importer.cmd_execute()
             self.transformation.importer.cmd  =  ''  
          '''
             
      def endTrigger(self):     
          if self.options.packagepart   in ['TABLE'] and self.options.streamformat_target == 'PAGE' and self.transformation.importer != None:
             self.transformation.importer.cmd  = 'DROP TRIGGER \"%s\" OF \"%s\".\"%s\" \n' % (self.trigger, self.schema, self.table)
             self.transformation.importer.cmd_execute()
             self.transformation.importer.cmd  =  ''          

      def endColumn(self):
          column_field = ''
          if self.options.streamformat[0:9]   == 'FORMATTED' :
             if self.keycolumn == None :
                column_field      = "%+16s%-15s  %3i - %3i CHAR " % (' ','\"%s\"' % self.column,self.field_startpos, self.field_endpos)
                self.column_field = self.column_field  + column_field 
                if self.columnIsNullable == '0':
                   self.column_field = self.column_field + "\n"
                if self.columnIsNullable != '0':
                   self.column_field = self.column_field + "DEFAULT NULL \n"
             self.field_startpos = self.field_endpos + 1
          if self.options.streamformat        == 'COMPRESSED':   
             if self.keycolumn == None :
                column_field    = "%+16s%-15s  %3i  CHAR "      % (' ','\"%s\"' % self.column, self.columncount )
                self.column_field = self.column_field  + column_field 
                if self.columnIsNullable == '0':
                   self.column_field = self.column_field + "\n"
                if self.columnIsNullable != '0':
                   self.column_field = self.column_field + "DEFAULT NULL \n"

      def endKeyColumns(self):
          self.keycolumn = None                   
          
      def columnTypeBool (self, name):
          if name == 'sdbm:BOOLEAN':
             fieldBoolLength = '1'
             if self.columnIsNullable == '0':
                self.field_endpos      = self.field_startpos + self.int('%s' %self.columnInitialValue) -1
             if self.columnIsNullable == '1':   
                if len(self.fieldNullValue) >  len('%s' %self.columnInitialValue) :
                   self.field_endpos   = self.field_startpos + len(self.fieldNullValue) -1
                if len(self.fieldNullValue) <= len('%s' %self.columnInitialValue) :
                   self.field_endpos   = self.field_startpos + self.int('%s' %self.columnInitialValue) -1
      
      def columnTypeCharacter(self, name):
          if(name == 'sdbm:CHAR'    or 
             name == 'sdbm:VARCHAR'   ):
             if self.columnIsNullable == '0': 
                self.field_endpos      = self.field_startpos + self.int('%s' %self.columnLength) -1
             if self.columnIsNullable == '1':   
                if len(self.fieldNullValue)  > self.int('%s' % self.columnLength):
                   self.field_endpos   = self.field_startpos + len(self.fieldNullValue)-1
                if len(self.fieldNullValue) <= self.int('%s' %self.columnLength):   
                   self.field_endpos   = self.field_startpos + self.int('%s' %self.columnLength) -1
          if name == 'sdbm:BINARY':  
             self.columnEncoding = 'HEX'
             if self.columnIsNullable == '0': 
                self.field_endpos      = self.field_startpos + HEXLENGTH*self.int('%s' %self.columnLength) -1
             if self.columnIsNullable == '1':   
                if len(self.fieldNullValue)  > self.int('%s' % self.columnLength):
                   self.field_endpos   = self.field_startpos + HEXLENGTH*len(self.fieldNullValue)-1
                if len(self.fieldNullValue) <= self.int('%s' %self.columnLength):   
                   self.field_endpos   = self.field_startpos + HEXLENGTH*self.int('%s' %self.columnLength) -1       
      
      def columnTypeDateTime(self, name ):
          if name == 'sdbm:DATE': 
             fieldDateLength    = 0
             if self.fieldDateValue == 'INTERNAL':
                fieldDateLength = DATEINTERNAL
             if self.fieldDateValue != 'INTERNAL':
                fieldDateLength = DATE
             if self.columnIsNullable == '0': 
                self.field_endpos      = self.field_startpos + self.int( '%i' % fieldDateLength) -1 
             if self.columnIsNullable == '1':   
                if len(self.fieldNullValue) >  self.int( '%i' % fieldDateLength): 
                   self.field_endpos   = self.field_startpos + len(self.fieldNullValue) -1
                if len(self.fieldNullValue) <= self.int( '%i' % fieldDateLength):
                   self.field_endpos   = self.field_startpos + self.int( '%i' % fieldDateLength) -1
          if name == 'sdbm:TIME': 
             fieldTimeLength    = 0
             if self.fieldTimeValue == 'INTERNAL':
                fieldTimeLength = TIMEINTERNAL
             if self.fieldTimeValue != 'INTERNAL':
                fieldTimeLength = TIME
             if self.columnIsNullable == '0': 
                self.field_endpos      = self.field_startpos + self.int( '%i' % fieldTimeLength) -1
             if self.columnIsNullable == '1':   
                if len(self.fieldNullValue) >  len(self.fieldTimeValue) : 
                   self.field_endpos   = self.field_startpos + len(self.fieldNullValue) -1
                if len(self.fieldNullValue) <= len(self.fieldTimeValue) :
                   self.field_endpos   = self.field_startpos + self.int(fieldTimeLength) -1
          if name == 'sdbm:TIMESTAMP':
             fieldTimeStampLength    = 0
             if self.fieldTimeStampValue == 'INTERNAL':
                fieldTimeStampLength = TIMESTAMPINTERNAL
             if self.fieldTimeStampValue != 'INTERNAL':
                fieldTimeStampLength = TIMESTAMP
             if self.columnIsNullable == '0': 
                self.field_endpos      = self.field_startpos + self.int( '%i' % fieldTimeStampLength) -1
             if self.columnIsNullable == '1':   
                if len(self.fieldNullValue) >  self.int( '%i' % fieldTimeStampLength) :
                   self.field_endpos   = self.field_startpos + len(self.fieldNullValue) -1 
                if len(self.fieldNullValue) <= self.int( '%i' % fieldTimeStampLength) :
                   self.field_endpos   = self.field_startpos + self.int( '%i' % fieldTimeStampLength) -1
      
      def columnTypeNumber(self, name ):    
          fieldIntegerLength = 0
          signSymbolLength  = SIGN
          ThsdCharNum       = 0
          FracPart          = 0
          IntPart           = 0
          if name == 'sdbm:SMALLINT':
             fieldIntegerLength = SMALLINT
          if(name == 'sdbm:INTEGER' or
             name == 'sdbm:DECIMAL'):
             fieldIntegerLength = INTEGER
          if name == 'sdbm:FIXED'  :
             fieldIntegerLength = int(string.split(self.columnLength,',')[0])
             FracPart           = int(string.split(self.columnLength,',')[1])
          if name == 'sdbm:FLOAT' :
             fieldIntegerLength = int(self.columnLength)
             field_length       = fieldIntegerLength + FLOATMASK
             if self.columnIsNullable == '0':
                self.field_endpos      = self.field_startpos + self.int('%s' % field_length) -1
             if self.columnIsNullable == '1': 
                if len(self.fieldNullValue) >  self.int('%s' % field_length): 
                   self.field_endpos   = self.field_startpos + len(self.fieldNullValue) - 1
                if len(self.fieldNullValue) <= self.int('%s' % field_length):
                   self.field_endpos   = self.field_startpos + self.int('%s' % field_length) -1
          if name != 'sdbm:FLOAT':
             IntPart          = fieldIntegerLength - FracPart
             field_length     = 0  
             if string.split(self.fieldDecimalValue,'/')[1] <> '':
                ThsdCharNum = ( IntPart - 1) / 3
             field_length = IntPart + FracPart+ signSymbolLength + ThsdCharNum
             if   FracPart == 0:
                  field_length = field_length - 1
             elif IntPart  == 0: 
                  field_length = field_length - ( ThsdCharNum - 1 )
             if self.columnIsNullable == '0':
                self.field_endpos      = self.field_startpos + self.int('%s' % field_length) -1
             if self.columnIsNullable == '1': 
                if len(self.fieldNullValue) >  self.int('%s' % field_length): 
                   self.field_endpos   = self.field_startpos + len(self.fieldNullValue) -1
                if len(self.fieldNullValue) <= self.int('%s' % field_length):
                   self.field_endpos   = self.field_startpos + self.int('%s' % field_length) -1
      def columnTypeLong (self, name ):#
          LOBSTREAM = ' '
          if name in ['sdbm:LONG','sdbm:CLOB'] and self.options.streamformat   != 'RECORDS' :
             if self.column != ''     and self.options.streammedium   == 'FILE'    :
                if  self.options.transportdirection    == 'TRANSPORT' and TESTLOADER == 'TRUE':
                    LOBSTREAM = '  LONGFILE'
                if  self.options.transportdirection    == 'EXPORT':
                    LOBSTREAM = '  LOB           OUTSTREAM FILE'
                if  self.options.transportdirection    == 'IMPORT':
                    LOBSTREAM = '  LOB           INSTREAM  FILE'
                if  self.options.streamlob == 'COLUMN':
                    self.column_field_long = self.column_field_long + "%s \"%s\" '%s' %s \n" % ( LOBSTREAM, self.column, os.path.join(self.options.sourcepackagepath, '%s_%s.DATA%s' % (self.table,self.column,'')),self.options.streamcharencoding)
                    self.field_endpos      = self.field_startpos + LONGPOSITION -1
                if  self.options.streamlob == 'COLUMNVALUE':
                    self.column_field_long = self.column_field_long + " %s \"%s\" '%s' %s \n" % ( LOBSTREAM, self.column, os.path.join(self.options.sourcepackagepath, '%s_%s.DATA%s' % (self.table,self.column,FIELDLOBMASK)),self.options.streamcharencoding)
                    self.field_endpos      = self.field_startpos + self.len(os.path.join(self.options.sourcepackagepath,  '%s_%s.DATA%s' % (self.table,self.column, FIELDLOBMASK))) -1 
             if self.column != ''       and ( self.options.streammedium == 'PIPE' or  self.options.streammedium  == 'PIPE2FILE' ) :
                self.column_field_long = self.column_field_long + "%s  %s '%s' %s \n" % (LOBSTREAM, self.column, '\\\\.\\PIPE\\%s_%s.DATA' % (self.table,self.column),self.options.streamcharencoding)
                self.column_long.append(self.column)
          if name in ['sdbm:LONG_BYTE','sdbm:BLOB'] and self.options.streamformat   != 'RECORDS' :
             if self.column != ''     and self.options.streammedium   == 'FILE'    :
                if  self.options.transportdirection    == 'TRANSPORT' and TESTLOADER == 'TRUE':
                    LOBSTREAM = '  LONGFILE'
                if  self.options.transportdirection    == 'EXPORT':
                    LOBSTREAM = '  LOB           OUTSTREAM FILE'
                if  self.options.transportdirection    == 'IMPORT':
                    LOBSTREAM = '  LOB           INSTREAM  FILE'
                if  self.options.streamlob == 'COLUMN':
                    self.column_field_long  = self.column_field_long + "%s  \"%s\" '%s' \n" % ( LOBSTREAM,self.column, os.path.join(self.options.sourcepackagepath,   '%s_%s.DATA%s' % (self.table,self.column, '')))
                    self.field_endpos       = self.field_startpos + LONGPOSITION -1 
                if  self.options.streamlob == 'COLUMNVALUE':
                    self.column_field_long  = self.column_field_long + "%s  \"%s\" '%s' \n" % (LOBSTREAM,self.column, os.path.join(self.options.sourcepackagepath,   '%s_%s.DATA%s' % (self.table,self.column, FIELDLOBMASK)))
                    self.field_endpos       = self.field_startpos + self.len(os.path.join(self.options.sourcepackagepath,   '%s_%s.DATA%s' % (self.table,self.column,FIELDLOBMASK))) -1
             if self.column != ''       and ( self.options.streammedium == 'PIPE' or  self.options.streammedium  == 'PIPE2FILE' ) :
                if  self.options.transportdirection    == 'EXPORT':
                    LOBSTREAM = '  LOB           OUTSTREAM PIPE'
                if  self.options.transportdirection    == 'IMPORT':
                    LOBSTREAM = '  LOB           INSTREAM  PIPE'
                self.column_field_long = self.column_field_long + "%s \"%s\" '%s' \n" % ( LOBSTREAM, self.column, '\\\\.\\PIPE\\%s_%s.DATA' % (self.table,self.column))
                self.column_long.append(self.column)
                   
class Transformation :
      def __init__( self, options = None, package = None ) :
          self.options            = options
          self.package            = package
          self.transformation     = None
          self.catalogstream      = True
          self.datastream         = True
          self.statisticstream    = True
          self.packagestream      = True
          self.exportstatement    = None
          self.importstatement    = None
          self.exporter           = None
          self.importer           = None
          self.init()
      
      def init(self):
          # streamtypes
          if string.find(self.options.streamtypes,'CATALOG') >= 0:
             self.catalogstream   = True
          else:
             self.catalogstream   = False
          if string.find(self.options.streamtypes,'DATA')    >= 0:
             self.datastream      = True
          else:
             self.datastream      = False
          if string.find(self.options.streamtypes,'PACKAGE') >= 0:
             self.packagestream   = True
          else:
             self.packagestream   = False  
          if STREAMSTATISTICS == True:
             self.statisticstream = True
          else:
             self.statisticstream = False
          # stream loader source
          if( self.options.transportdirection == 'TRANSPORT' or
              self.options.transportdirection == 'EXPORT'      ):
              self.exporter = Transformator( self.options, self.package, 'EXPORT' )
              try:
                 if self.options.mode == 'DEBUG':
                    input('\nStarted   Exporter')
              except:
                 pass                                              
          # stream loader target
          if( self.options.transportdirection == 'TRANSPORT' or
              self.options.transportdirection == 'IMPORT'      ):
              self.importer = Transformator( self.options, self.package, 'IMPORT' )
              try:
                 if self.options.mode == 'DEBUG':
                    input('\nStarted   Importer')
              except:
                 pass 
                 
      def output(self): 
          try:
             export_stmt = self.exportstatement 
             import_stmt = self.importstatement
             export_stmt = string.replace(export_stmt,'\\\\.\\PIPE\\','')
             export_stmt = string.replace(export_stmt,self.options.sourcepackagepath+os.sep, '')
             if TESTLOADER in ['FALSE']:
                export_stmt = re.split('PACKAGE',export_stmt)[0]
             import_stmt = string.replace(import_stmt,'\\\\.\\PIPE\\','')
             import_stmt = string.replace(import_stmt,self.options.sourcepackagepath+os.sep, '')
             import_stmt = string.replace(import_stmt,self.options.targetpackagepath+os.sep, '')
             if TESTLOADER in ['FALSE']:
                import_stmt = re.split('PACKAGE',import_stmt)[0]
                       
             if self.options.transportdirection == 'TRANSPORT':
                print '\n%s;'  % export_stmt
                print '\n%s;'  % import_stmt
             if self.options.transportdirection == 'EXPORT':
                print '\n%s;'  % export_stmt
             if self.options.transportdirection == 'IMPORT':   
                print '\n%s;'  % import_stmt
          except:
             pass 
      
      def release(self):
          if mutex.locked() == True:
             mutex.release()
          if self.exporter != None:
             self.exporter.release()
          if self.importer != None:
             self.importer.release()
      
      def cmd_release_execute_package(self):
          # trigger package and release Transformator
          release        = 'FALSE'
          while release == 'TRUE':
                try:
                    cmd  = "SELECT COUNT(*) FROM SYSDBA.TRANSFORMATIONMODEL WHERE RESTART = FALSE "
                    self.sourcesession.cmd = cmd
                    self.sourcesession.cmd_execute()
                    if self.sourcesession.result[0] == 1 :
                       print '\nStop   Package Execution'
                       self.exporter.loadersession.release()  # python->cancelbyte
                       self.importer.loadersession.release()  # python->cancelbyte
                       release = 'TRUE'
                except sql.SQLError,  err :
                    print 'sql command failed:', err
                except loader.LoaderError, err :
                    print 'loader command failed:', err
                except loader.CommunicationError, err :
                    print 'loader communication failed:', err
                    
      def cmd_statement(self):
          #transformationstreamencoding
          if self.options.streamcharencoding == 'DYNAMIC':
             self.options.streamcharencoding  = ''
          #streamtypes
          datastream_format = self.options.streamformat
          datastream_field  = ''
          if(datastream_format                  in ['COMPRESSED','FORMATTED']  and 
             self.options.streamformat_target   in ['PAGE'] and 
             self.package.column_field_long not in ['']  ):
             datastream_format                  = 'RECORDS'
          if(datastream_format                  in ['COMPRESSED','FORMATTED']  and 
             self.options.streammedium          in ['PIPE'] and
             self.options.streamformat_target   in ['PAGE','ROW'] and 
             self.package.column_field_long not in ['']  ):
             datastream_format                  = 'RECORDS'
          if datastream_format                  in ['COMPRESSED','FORMATTED'] :
             configurationstream_name_mask      = "'%s'"
             configurationstream_name_demask    = "%s.config" 
             catalogstream_name_mask            = "'%s.CATALOG'"       + "%-1s"
             catalogstream_encoding_mask        = "%-6s %s"
             datastream_name_mask               = "'%s.DATA'"          + "%-4s"
             datastream_encoding_mask           = "%-0s %s"
             statisticstream_name_mask          = "'%s.STATISTICS'"    + "%-1s"
             statisticstream_format_mask        = "%-0s" + "CSV"
             statisticstream_encoding_mask      = "%-0s %s"
             if PACKAGEFORMAT                   in ['DTL'] and self.options.packagecreation == self.options.packageexecution:
                if((self.options.transportdirection in ['EXPORT'] or TESTLOADER in ['TRUE']) and
                   datastream_format            not in ['PAGES','RECORDS'] ):
                   packagestream_name_mask      = "'%s.IMPORT'"               + "%-1s"
                   packagestream_format_mask    = "%-0s" + "DTL " 
                if((self.options.transportdirection in ['IMPORT'] or TESTLOADER in ['TRUE']) and
                   datastream_format            not in ['PAGES','RECORDS'] ):
                   packagestream_name_mask      = "'%s.PACKAGE'"               + "%-1s"
                   packagestream_format_mask    = "%-0s" + "DTL "
             if PACKAGEFORMAT               not in ['DTL'] or  self.options.packagecreation != self.options.packageexecution:
                packagestream_name_mask         = "'%s.PACKAGE'"       + "%-1s"
                packagestream_format_mask       = "%-0s" + "CSV APPEND" 
             packagestream_encoding_mask        = "%-0s %s"
          if datastream_format                  in ['PAGES','RECORDS','DDL','XML']:
             configurationstream_name_mask      = "'%s'" 
             configurationstream_name_demask    = "%s.config" 
             catalogstream_name_mask            = "'%s.CATALOG'"       + "%-6s"
             catalogstream_encoding_mask        = "%-6s %s"
             datastream_name_mask               = "'%s.DATA'"          + "%-9s"
             datastream_encoding_mask           = "%-2s %s"
             statisticstream_name_mask          = "'%s.STATISTICS'"    + "%-3s"
             statisticstream_format_mask        = "%-0s" + "CSV"
             statisticstream_encoding_mask      = "%-0s %s"
             packagestream_name_mask            = "'%s.PACKAGE'"       + "%-1s"
             packagestream_format_mask          = "%-0s" + "CSV APPEND"
             packagestream_encoding_mask        = "%-0s %s"
          #transformationstream_names		 
          export_configurationstream_name       = self.options.sourcepackagepath
          import_configurationstream_name       = self.options.targetpackagepath
          export_packagestream_name             = os.path.join(self.options.sourcepackagepath)
          import_packagestream_name             = os.path.join(self.options.targetpackagepath)
          if sys.platform[:3] != 'win':		 
             datastream_name                    = os.path.join(self.options.sourcepackagepath)
             catalogstream_name                 = os.path.join(self.options.sourcepackagepath)
             statisticstream_name               = os.path.join(self.options.sourcepackagepath)
          if sys.platform[:3] == 'win':		 
             if self.options.streammedium       =='FILE':
                datastream_name                 = os.path.join(self.options.sourcepackagepath)
                catalogstream_name              = os.path.join(self.options.sourcepackagepath)
                statisticstream_name            = os.path.join(self.options.sourcepackagepath)
             if self.options.streammedium       =='PIPE' or self.options.streammedium== 'TAPE':  
                datastream_name                 = os.path.join('\\\\','.','PIPE')
                catalogstream_name              = os.path.join('\\\\','.','PIPE')
                statisticstream_name            = os.path.join('\\\\','.','PIPE')
          #DB
          if self.options.packagepart in ['DB']   :
             export_configurationstream_name = os.path.join(export_configurationstream_name,self.options.sourcedbname)
             import_configurationstream_name = os.path.join(import_configurationstream_name,self.options.targetdbname)
             if self.options.streammedium     in ['PIPE']:
                export_catalogstream_name    = os.path.join(catalogstream_name       ,self.options.sourcedbname)
                import_catalogstream_name    = os.path.join(catalogstream_name       ,self.options.sourcedbname)
             if self.options.streammedium not in ['PIPE']: 
                export_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.sapdb)
                import_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.sapdb)        
             datastream_name                 = os.path.join(datastream_name          ,self.package.sapdb)
             statisticstream_name            = os.path.join(statisticstream_name     ,self.package.sapdb)
             export_packagestream_name       = os.path.join(export_packagestream_name,self.options.packageexecution)
             import_packagestream_name       = os.path.join(import_packagestream_name,self.options.packageexecution)
          #USER
          if self.options.packagepart in ['USER'] :
             export_configurationstream_name = os.path.join(export_configurationstream_name,self.package.user)
             import_configurationstream_name = os.path.join(import_configurationstream_name,self.package.user)
             if self.options.streammedium     in ['PIPE']:
                export_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.user)
                import_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.user)
             if self.options.streammedium not in ['PIPE']: 
                export_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.user)
                import_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.user)     
             datastream_name                 = os.path.join(datastream_name          ,self.package.user)
             statisticstream_name            = os.path.join(statisticstream_name     ,self.package.user)
             export_packagestream_name       = os.path.join(export_packagestream_name,self.options.packageexecution)
             import_packagestream_name       = os.path.join(import_packagestream_name,self.options.packageexecution)
          #SCHEMA
          if self.options.packagepart in ['SCHEMA'] :
             export_configurationstream_name = os.path.join(export_configurationstream_name,self.options.sourcedbschema)
             import_configurationstream_name = os.path.join(import_configurationstream_name,self.options.targetdbschema)
             if self.options.streammedium     in ['PIPE']:
                export_catalogstream_name    = os.path.join(catalogstream_name       ,self.options.sourcedbschema)
                import_catalogstream_name    = os.path.join(catalogstream_name       ,self.options.sourcedbschema)
             if self.options.streammedium not in ['PIPE']: 
                export_catalogstream_name    = os.path.join(catalogstream_name       ,self.options.sourcedbschema)
                import_catalogstream_name    = os.path.join(catalogstream_name       ,self.options.targetdbschema)     
             datastream_name                 = os.path.join(datastream_name          ,self.options.sourcedbschema)
             statisticstream_name            = os.path.join(statisticstream_name     ,self.options.sourcedbschema)
             export_packagestream_name       = os.path.join(export_packagestream_name,self.options.packageexecution)
             import_packagestream_name       = os.path.join(import_packagestream_name,self.options.packageexecution)
          #TABLE
          if self.options.packagepart in ['TABLE']:
             export_configurationstream_name = os.path.join(export_configurationstream_name,self.package.table)
             import_configurationstream_name = os.path.join(import_configurationstream_name,self.package.table)
             if self.options.streammedium     in ['PIPE']:
                export_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.table)
                import_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.table)
             if self.options.streammedium not in ['PIPE']: 
                export_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.table)
                import_catalogstream_name    = os.path.join(catalogstream_name       ,self.package.table)  
             datastream_name                 = os.path.join(datastream_name          ,self.package.table)
             statisticstream_name            = os.path.join(statisticstream_name     ,self.package.table)
             export_packagestream_name       = os.path.join(export_packagestream_name,self.options.packageexecution)
             import_packagestream_name       = os.path.join(import_packagestream_name,self.options.packageexecution)
          #configurationstream
          export_configurationstream_name = configurationstream_name_demask %( export_configurationstream_name )
          import_configurationstream_name = configurationstream_name_demask %( import_configurationstream_name )
          configurationstream_medium      = 'FILE'
          configurationstream_format      = 'COMPRESSED'
          configurationstream_encoding    = ''
          #catalogstream
          export_catalogstream_name      = catalogstream_name_mask %( export_catalogstream_name, '' )
          import_catalogstream_name      = catalogstream_name_mask %( import_catalogstream_name, '' )
          catalogstream_medium           = self.options.streammedium     
          if catalogstream_medium       in ['TAPE'] and catalogstream_direction    in ['INSTREAM']:
             catalogstream_medium        = catalogstream_medium + ' EBID ' + self.importer.ddlpipe_ebid
          if self.options.streamformat  in ['DDL']  or  self.options.streamformat  in ['XML']:
             catalogstream_format        = self.options.streamformat
          else:
             catalogstream_format        = 'DDL'
          catalogstream_encoding         = catalogstream_encoding_mask %( '', '' )
          #datastream
          datastream_name                = datastream_name_mask % (datastream_name, '')
          datastream_medium              = self.options.streammedium
          if datastream_medium          in ['TAPE'] and datastream_direction in ['INSTREAM']:
             datastream_medium           = datastream_medium + 'EBID' + self.importer.mappipe_ebid
          datastream_format              = self.options.streamformat
          if(datastream_format          in ['COMPRESSED','FORMATTED'] and self.package.column_field_long not in [''] and 
            ( self.options.streammedium in ['PIPE']                    or self.options.streamformat_target   in ['PAGE'] )):
             datastream_format           = 'RECORDS'
          if datastream_format          in ['PAGES']:
             datastream_encoding         = ''
          if datastream_format          in ['RECORDS'] and self.options.streamcharencoding     in ['UTF8']:
             datastream_encoding         = ''
          if datastream_format          in ['RECORDS'] and self.options.streamcharencoding not in ['UTF8']:
             datastream_encoding         = datastream_encoding_mask  % ('', self.options.streamcharencoding )   
          if datastream_format          in ['COMPRESSED','FORMATTED','DDL','XML'] :   
             datastream_encoding         = datastream_encoding_mask  % ('', self.options.streamcharencoding )
          if datastream_format          in ['COMPRESSED''FORMATTED']:
             if datastream_format       in ['COMPRESSED']:
                datastream_field         = datastream_field + "%-15s SEPARATOR  '%s' \n" % ( '', re.split('/',self.package.fieldCompressed)[1] )
                datastream_field         = datastream_field + "%-15s DELIMITER  '%s' \n" % ( '', re.split('/',self.package.fieldCompressed)[2] )
             datastream_field            = datastream_field + "%-15s NULL       '%s' \n" % ( '', self.package.fieldNullValue  )
             datastream_field            = datastream_field + "%-15s DECIMAL    '%s' \n" % ( '', self.package.fieldDecimalValue) 
             datastream_field            = datastream_field + "%-15s BOOLEAN    '%s' \n" % ( '', self.package.fieldBoolValue) 
             datastream_field            = datastream_field + "%-15s TIMESTAMP   %s  \n" % ( '', self.package.fieldTimeStampValue)
             datastream_field            = datastream_field + "%-15s DATE        %s  \n" % ( '', self.package.fieldDateValue)
             datastream_field            = datastream_field + "%-15s TIME        %s  \n" % ( '', self.package.fieldTimeValue)     
          #statisticstream
          statisticstream_name           = statisticstream_name_mask     %( statisticstream_name, '') 
          statisticstream_medium         = self.options.streammedium
          statisticstream_format         = statisticstream_format_mask   %('') 
          statisticstream_encoding       = statisticstream_encoding_mask %('','') 
          #packagestream
          export_packagestream_name      = packagestream_name_mask       %( export_packagestream_name, '') 
          import_packagestream_name      = packagestream_name_mask       %( import_packagestream_name, '') 
          packagestream_medium           = 'FILE' 
          packagestream_format           = packagestream_format_mask     %('') 
          packagestream_encoding         = packagestream_encoding_mask   %('','') 
          if configurationstream_format  in ['COMPRESSED']:
             configurationstream_format  = 'CSV'
          if configurationstream_format  in ['FORMATTED']:
             configurationstream_format  = 'FWV'
          if datastream_format           in ['COMPRESSED']:
             datastream_format           = 'CSV'
          if datastream_format           in ['FORMATTED']:
             datastream_format           = 'FWV'
          if statisticstream_format      in ['COMPRESSED']:
             statisticstream_format      = 'CSV'
          if statisticstream_format      in ['FORMATTED']:
             statisticstream_format      = 'FWV'
          #streamtypes
          if os.path.isfile(export_configurationstream_name) == True:
             export_configurationstream_name = configurationstream_name_mask     %( export_configurationstream_name )
             export_configurationstream  = "  CONFIGURATION %s %s %s %s %s \n"   % ( 'INSTREAM ', configurationstream_medium, export_configurationstream_name, configurationstream_format, configurationstream_encoding)        
          else:
             export_configurationstream  = ""
          export_catalogstream           = "  CATALOG       %s %s %s %s %s \n"   % ( 'OUTSTREAM', catalogstream_medium      , export_catalogstream_name      , catalogstream_format      , catalogstream_encoding)
          export_datastream              = "  DATA          %s %s %s %s %s \n"   % ( 'OUTSTREAM', datastream_medium         , datastream_name                , datastream_format         , datastream_encoding )
          export_statisticstream         = "  STATISTICS    %s %s %s %s %s  \n"   % ('OUTSTREAM', statisticstream_medium    , statisticstream_name           , statisticstream_format    , statisticstream_encoding )      
          if os.path.isfile(import_configurationstream_name) == True:
             import_configurationstream_name = configurationstream_name_mask     % ( import_configurationstream_name )
             import_configurationstream  = "  CONFIGURATION %s %s %s %s %s \n"   % ( 'INSTREAM ', configurationstream_medium, import_configurationstream_name, configurationstream_format, configurationstream_encoding)        
          else:
             import_configurationstream  = ""
          import_catalogstream           = "  CATALOG       %s %s %s %s %s \n"   % ( 'INSTREAM ', catalogstream_medium      , import_catalogstream_name      , catalogstream_format      , catalogstream_encoding)
          import_datastream              = "  DATA          %s %s %s %s %s \n"   % ( 'INSTREAM ', datastream_medium         , datastream_name                , datastream_format         , datastream_encoding )
          import_statisticstream         = "  STATISTICS    %s %s %s %s %s  \n"   % ('INSTREAM ', statisticstream_medium    , statisticstream_name           , statisticstream_format    , statisticstream_encoding )                   
          if configurationstream_format  in ['CSV']:
             configurationstream_format  = 'COMPRESSED' 
          if configurationstream_format  in ['FWV']:
             configurationstream_format  = 'FORMATTED'
          if datastream_format           in ['CSV']:
             datastream_format           = 'COMPRESSED'
          if datastream_format           in ['FWV']:
             datastream_format           = 'FORMATTED'
          if statisticstream_format      in ['CSV']:
             statisticstream_format      = 'COMPRESSED'
          if statisticstream_format      in ['FWV']:
             statisticstream_format      = 'FORMATTED'
          export_datastream              = export_datastream + datastream_field
          import_datastream              = import_datastream + datastream_field
          if datastream_format          in ['COMPRESSED','FORMATTED']:
             export_datastream           = export_datastream + '%-15s START  %-4s 1   MAXROWS    \n'  %('','')
             import_datastream           = import_datastream + '%-15s START  %-4s 1   MAXRECORDS \n'  %('','') 
             export_datastream           = export_datastream + ''
             import_datastream           = import_datastream + self.package.column_field
             if self.options.streamlob  == 'COLUMN':
                export_datastream        = export_datastream + self.package.column_field_long
                import_datastream        = import_datastream + self.package.column_field_long
             if self.options.streamlob  == 'COLUMNVALUE': 
                export_datastream        = export_datastream + self.package.column_field_long  
                import_datastream        = import_datastream + ''
          if datastream_format          in ['RECORDS'] and self.options.packagepart in ['TABLE'] : 
             export_datastream           = export_datastream + '%-15s START  %-4s 1   MAXROWS    \n' %('','')
             import_datastream           = import_datastream + '%-15s START  %-4s 1   MAXRECORDS \n' %('','') 
          
          export_packagestream           = "  PACKAGE       %s %s %s %s %s  \n"   % ('OUTSTREAM', packagestream_medium      , export_packagestream_name      , packagestream_format      , packagestream_encoding )      
          import_packagestream           = "  PACKAGE       %s %s %s %s %s  \n"   % ('OUTSTREAM', packagestream_medium      , import_packagestream_name      , packagestream_format      , packagestream_encoding )      
          
          append = 'APPEND'
          if( self.options.packagepart         in ['DB','USER''SCHEMA'] and  
              self.options.streamformat_target in ['DTL'] ):
              export_datastream = ''
              import_datastream = ''
          if(self.options.packagepart          in ['TABLE']           and
             datastream_format            not  in ['PAGES','RECORDS'] and 
             self.options.streamformat_target  in ['ROW']   ):
             import_datastream                  = import_datastream    + '%-15s RECORDS USAGE \n' %(' ')
             if PACKAGEFORMAT in ['DTL'] and self.options.packagecreation == self.options.packageexecution:
                export_packagestream            = export_packagestream + '%-15s RECORDS USAGE %s\n'   %(' ',append)
                import_packagestream            = import_packagestream + '%-15s RECORDS USAGE %s\n'   %(' ',append)
          if(self.options.packagepart          in ['TABLE']           and
             datastream_format                 in ['COMPRESSED','FORMATTED'] and  
             self.options.streamformat_target  in ['PAGE']            and 
             self.package.column_field_long    in ['']     ):
             import_datastream                  = import_datastream    + '%-15s PAGE WITH 80 ' %(' ') + '% USAGE \n' 
             if PACKAGEFORMAT in ['DTL'] and self.options.packagecreation == self.options.packageexecution:
                export_packagestream            = export_packagestream + '%-15s PAGE WITH 80 ' %(' ') + '% USAGE ' + '%s \n' %append
                import_packagestream            = import_packagestream + '%-15s PAGE WITH 80 ' %(' ') + '% USAGE ' + '%s \n' %append
          if(self.options.packagepart          in ['TABLE']                  and  
             datastream_format                 in ['COMPRESSED','FORMATTED'] and 
             self.options.streamformat_target  in ['DTL'] ):
             if self.package.column_field_long not in  ['']:
                import_datastream               = import_datastream    + '%-15s RECORDS USAGE \n'   %(' ')
                if PACKAGEFORMAT in ['DTL'] and self.options.packagecreation == self.options.packageexecution:
                   export_packagestream        = export_packagestream + '%-15s RECORDS USAGE %s\n'   %(' ',append)
                   import_packagestream        = import_packagestream + '%-15s RECORDS USAGE %s\n'   %(' ',append)
             if self.package.column_field_long in ['']:
                import_datastream              = import_datastream    + '%-15s PAGE WITH 80 ' %(' ') + '% USAGE \n'
                if PACKAGEFORMAT in ['DTL'] and self.options.packagecreation == self.options.packageexecution:
                   export_packagestream        = export_packagestream + '%-15s PAGE WITH 80 ' %(' ') + '% USAGE ' + '%s \n' %append 
                   import_packagestream        = import_packagestream + '%-15s PAGE WITH 80 ' %(' ') + '% USAGE ' + '%s \n' %append 
          if(self.options.packagepart  in ['TABLE'] ):
             packagestream_format              = "CSV"
             import_packagestream              = "  PACKAGE       %s %s %s %s %s APPEND \n"   % ('OUTSTREAM', packagestream_medium      , import_packagestream_name      , packagestream_format      , packagestream_encoding )      
          
          #transformation
          
          #transportdirection export 
          export_stmt  = 'EXPORT %s ' % self.options.packagepart
          #packageref
          if(self.options.packagepart  in ['DB']   or 
             self.options.packagepart  in ['USER'] ):
             export_stmt = export_stmt  + '\n'
          if(self.options.packagepart  in ['SCHEMA']):
             export_stmt = export_stmt + '"%s" ' % self.options.sourcedbschema
             export_stmt = export_stmt + '\n' 
          if(self.options.packagepart  in ['TABLE']):
             export_stmt = export_stmt + '"%s" ' % self.package.table
             if( self.options.streamformat_target== 'DTL' ):
                 export_stmt = export_stmt + " FOR IMPORT TABLE "
             export_stmt = export_stmt + '\n'    
          #configurationstream   
          if(datastream_format     not in ['DDL']  and
            (self.options.packagepart  in ['DB']   or 
             self.options.packagepart  in ['USER'] or
             self.options.packagepart  in ['SCHEMA']  )): 
             export_stmt = export_stmt + export_configurationstream 
          #catalogstream 
          if self.catalogstream == True :
             export_stmt    = export_stmt + export_catalogstream
          #datastream 
          if(self.datastream   == True  or
             datastream_format in ['PAGES','RECORDS','COMPRESSED','FORMATTED']):
             export_stmt = export_stmt + export_datastream  
          #statisticstream
          if(self.options.packagepart  in ['DB'] ): 
             if self.statisticstream == True:
                export_stmt = export_stmt + export_statisticstream
          #packagestream
          if self.packagestream   == True:
             export_stmt    = export_stmt + export_packagestream
          #transportdirection import
          import_stmt  = 'IMPORT %s ' % self.options.packagepart
          #packageref
          if self.options.packagepart  in ['DB','USER']:
             if self.datastream   == True  and datastream_format == 'RECORDS':
                import_stmt = import_stmt + '%s \n'  % STREAMDATA_DUPLICATES
             else:
                import_stmt = import_stmt  + '\n'
          if(self.options.packagepart  in ['SCHEMA']):
             import_stmt = import_stmt + '"%s" ' % self.options.targetdbschema
             if self.datastream   == True  and datastream_format == 'RECORDS':
                import_stmt = import_stmt + '%s \n'  % STREAMDATA_DUPLICATES
             else:
                import_stmt = import_stmt + '\n' 
          if(self.options.packagepart  in ['TABLE']):
             import_stmt = import_stmt + '"%s" '  % self.package.table
             if self.datastream   == True  and self.options.streamformat_target == 'ROW':
                import_stmt = import_stmt + '%s \n'  % STREAMDATA_DUPLICATES
             else:
                import_stmt = import_stmt + '\n' 
          #configurationstream   
          if(datastream_format     not in ['DDL'] and
             self.options.packagepart  in ['DB','USER','SCHEMA'] ):    
             import_stmt = import_stmt + import_configurationstream 
          #catalogstream 
          if self.catalogstream == True :
             import_stmt    = import_stmt + import_catalogstream
          #datastream
          if(self.datastream   == True  or
             datastream_format in ['PAGES','RECORDS','COMPRESSED','FORMATTED'] ):
             import_stmt = import_stmt + import_datastream  
          #statisticstream
          #statisticstream
          if(self.options.packagepart  in ['DB'] ): 
             if self.statisticstream == True:
                import_stmt = import_stmt + import_statisticstream             
          #packagestream
          if self.packagestream   == True:
             import_stmt = import_stmt + import_packagestream    
          if self.options.transportdirection == 'EXPORT':
             self.exporter.catalogstream_name = export_catalogstream_name
             self.exporter.datastream_name    = datastream_name
             self.exporter.packagestream_name = export_packagestream_name
             self.exporter.cmd = export_stmt
          if self.options.transportdirection == 'IMPORT':
             self.importer.catalogstream_name = import_catalogstream_name
             self.importer.datastream_name    = datastream_name
             self.importer.packagestream_name = import_packagestream_name
             self.importer.cmd = import_stmt
          if self.options.transportdirection == 'TRANSPORT': 
             self.exporter.catalogstream_name = export_catalogstream_name
             self.exporter.datastream_name    = datastream_name
             self.exporter.packagestream_name = export_packagestream_name
             self.exporter.cmd = export_stmt
             self.importer.catalogstream_name = import_catalogstream_name
             self.importer.datastream_name    = datastream_name
             self.importer.packagestream_name = import_packagestream_name
             self.importer.cmd = import_stmt

          self.exportstatement  = export_stmt
          self.importstatement  = import_stmt
          
          if VERBOSE == True:
             self.output()
          
      def cmd_stamp_protocol_start(self):
          if self.exporter != None:
             self.exporter.cmd = 'REMARK "START     PACKAGE x\'%s\' WITH TIMESTAMP %s " \n' % ( self.package.packageguid, self.options.packageexecution  )
             self.exporter.cmd_execute()
          if self.importer != None:
             self.importer.cmd = 'REMARK "START     PACKAGE x\'%s\' WITH TIMESTAMP %s " \n' % ( self.package.packageguid, self.options.packageexecution  )
             self.importer.cmd_execute()
      
      def cmd_stamp_protocol_end(self):
          if self.exporter != None:
             self.exporter.cmd = 'REMARK "END       PACKAGE x\'%s\' WITH TIMESTAMP %s " \n' % ( self.package.packageguid, self.options.packageexecution  )
             self.exporter.cmd_execute()
          if self.importer != None:
             self.importer.cmd = 'REMARK "END       PACKAGE x\'%s\' WITH TIMESTAMP %s " \n' % ( self.package.packageguid, self.options.packageexecution  )
             self.importer.cmd_execute()
             
      def cmd_transport_catalog_package(self):
          input = None
          # stream connector and stream filter
          self.streamconnector =  StreamConnector(self.exporter, self.importer )
          self.streamfilter    =  StreamFilter   (self.options , self )

          #stream package
          if self.options.initposition == 'RESTART':
             try:
                 if options.mode == 'INTER':
                    input('Start init Transformationpackages')
                 if options.mode == 'PARAM':
                    print 'Start init Transformationpackages'
                    i = 1/0
             except:
                 restartRecord = self.importer.cmd_restart_targetdb_package()
                 self.exporter.cmd_restart_sourcedb_package ( restartRecord )

          #stream export
          while input not in ["y"]:
                if self.options.mode == 'INTER':
                   input = raw_input('Export    DB %s ? [y]     : ' % ( self.package.sapdb ))
                if self.options.mode == 'PARAM':
                   input   = "y"
                if input  == "y":
                   #stream loader
                   thread.start_new_thread( self.exporter.cmd_use_serverdb_user      ,())
                   time.sleep(1)
                   thread.start_new_thread( self.exporter.cmd_export_catalog_package ,()) #export
                   time.sleep(1)
                   #stream package
                   if self.options.streammedium == 'FILE' and self.options.streamformat <> 'XML':
                      while self.exporter.packageload  == 'FALSE' :
                            thread.start_new_thread( self.exporter.cmd_import_package   ,())    
                            time.sleep(1) 
                      while self.exporter.packageunload == 'FALSE' :
                            thread.start_new_thread( self.exporter.cmd_monitor_package,())
                            thread.start_new_thread( self.exporter.cmd_export_package ,())#release
                            time.sleep(1)
                      mutex.acquire()                                                         #acquire      
                   if self.options.streammedium == 'TAPE' :
                      while self.exporter.packageunload == 'FALSE' :
                            thread.start_new_thread( self.exporter.cmd_export_package,()) #release
                            time.sleep(1)
                      mutex.acquire()                                                         #acquire
                      self.importer.ddlpipe_ebid = self.exporter.ddlpipe_ebid
                      self.importer.datpipe_ebid = self.exporter.datpipe_ebid
                      self.importer.mappipe_ebid = self.exporter.mappipe_ebid

          input = None

          #stream import
          while input not in ["y"] :
                if self.options.mode == 'INTER':
                    input = raw_input('Import    DB %s ? [y]     : ' % ( self.package.sapdb ))
                if self.options.mode == 'PARAM':
                    input  = "y"
                if input  == "y":
                    #stream loader
                    thread.start_new_thread( self.importer.cmd_use_serverdb_user      ,())
                    time.sleep(1)
                    thread.start_new_thread( self.importer.cmd_import_catalog_package ,())  #import
                    time.sleep(1)
                    #stream package
                    while self.importer.packageload  == 'FALSE' :
                          thread.start_new_thread( self.importer.cmd_import_package   ,())    
                          time.sleep(1) 
                    while self.importer.packageunload == 'FALSE' :
                          thread.start_new_thread( self.importer.cmd_monitor_package,())
                          thread.start_new_thread( self.importer.cmd_export_package ,())   #release
                          time.sleep(1)
          self.exporter.packageload   = 'FALSE'
          self.exporter.packageunload = 'FALSE'
          self.importer.packageload   = 'FALSE'
          self.importer.packageunload = 'FALSE'
			    
      def cmd_export_catalog_package(self):
          if MONITOR == False:
             #stream packages
             self.exporter.cmd_export_catalog_package()
             if  mutex.locked()  == True :
                 mutex.release()           #release
          if MONITOR == True:
             # stream loader
             thread.start_new_thread( self.exporter.cmd_export_catalog_package,())    #export
             # stream package
             while self.exporter.packageload  == 'FALSE' :
                   thread.start_new_thread( self.exporter.cmd_import_package   ,())    
                   time.sleep(1) 
             while self.exporter.packageunload == 'FALSE' :
                   thread.start_new_thread( self.exporter.cmd_monitor_package,())
                   thread.start_new_thread( self.exporter.cmd_export_package ,())    #release
                   time.sleep(1)
             self.exporter.packageload    = 'FALSE'
             self.exporter.packageunload  = 'FALSE'

      def cmd_import_catalog_package(self):
          if MONITOR == False:
             #stream packages
             self.importer.cmd_import_catalog_package()
             if  mutex.locked()  == True :
                 mutex.release()           #release
          if MONITOR == True:
             # stream loader
             thread.start_new_thread( self.importer.cmd_import_catalog_package,())    #export
             # stream package
             while self.importer.packageload  == 'FALSE' :
                   thread.start_new_thread( self.importer.cmd_import_package   ,())    
                   time.sleep(1) 
             while self.importer.packageunload == 'FALSE' :
                   thread.start_new_thread( self.importer.cmd_monitor_package,())
                   thread.start_new_thread( self.importer.cmd_export_package ,())    #release
                   time.sleep(1)
             self.importer.packageload    = 'FALSE'
             self.importer.packageunload  = 'FALSE'

      def cmd_transport_schema_package(self):
          input = None
          # stream connector and stream filter
          self.streamconnector =  StreamConnector(self.exporter, self.importer )
          self.streamfilter    =  StreamFilter   (self.options , self )
          #stream packages
          if self.options.initposition == 'RESTART':
             try:
                 if options.mode == 'INTER':
                    input('Start init Transformationpackages')
             except:
                 restartRecord = self.importer.cmd_restart_targetdb_package()
                 self.exporter.cmd_restart_sourcedb_package( restartRecord )
          #stream export
          while input not in ["y"]:
                if self.options.mode == 'INTER':
                   input = raw_input('Export    Schema %s ? [y] : ' % ( self.package.user ))
                if self.options.mode == 'PARAM':
                   input   = "y"
                if input  == "y":
                   #stream loader
                   thread.start_new_thread( self.exporter.cmd_export_schema_package  ,()) #export
                   #stream package
                   if self.options.streammedium == 'FILE' and self.options.streamformat <> 'XML':
                      while self.exporter.packageload  == 'FALSE' :
                            thread.start_new_thread( self.exporter.cmd_import_package   ,())    
                            time.sleep(1) 
                      while self.exporter.packageunload == 'FALSE' :
                            thread.start_new_thread( self.exporter.cmd_monitor_package,())
                            thread.start_new_thread( self.exporter.cmd_export_package ,())#release
                            time.sleep(1)
                      mutex.acquire()                                                         #acquire      
                   if self.options.streammedium == 'TAPE' :
                      mutex.acquire()
                      self.importer.ddlpipe_ebid = self.exporter.ddlpipe_ebid
                      self.importer.datpipe_ebid = self.exporter.datpipe_ebid
                      self.importer.mappipe_ebid = self.exporter.mappipe_ebid

          input = None
          # stream import
          while input not in ["y"]:
                if self.options.mode == 'INTER':
                   input = raw_input('Import    Schema %s ? [y] : ' % ( self.package.user ))
                if self.options.mode == 'PARAM':
                   input   = "y"
                if input  == "y":
                    #stream loader
                    thread.start_new_thread( self.importer.cmd_import_schema_package ,())  #import
                    time.sleep(1)
                    #stream package
                    while self.importer.packageload  == 'FALSE' :
                          thread.start_new_thread( self.importer.cmd_import_package   ,())    
                          time.sleep(1) 
                    while self.importer.packageunload == 'FALSE' :
                          thread.start_new_thread( self.importer.cmd_monitor_package,())
                          thread.start_new_thread( self.importer.cmd_export_package ,())   #release
                          time.sleep(1)
          self.exporter.packageload   = 'FALSE'
          self.exporter.packageunload = 'FALSE'
          self.importer.packageload   = 'FALSE'
          self.importer.packageunload = 'FALSE'

      def cmd_export_schema_package(self):
          if MONITOR == False:
             #stream package
             self.exporter.cmd_export_schema_package()
             if  mutex.locked()  == True :
                 mutex.release()           #release
          if MONITOR == True:
             # stream loader
             thread.start_new_thread( self.exporter.cmd_export_schema_package,())    #export
             # stream package
             while self.exporter.packageload  == 'FALSE' :
                   thread.start_new_thread( self.exporter.cmd_import_package   ,())    
                   time.sleep(1) 
             while self.exporter.packageunload == 'FALSE' :
                   thread.start_new_thread( self.exporter.cmd_monitor_package,())
                   thread.start_new_thread( self.exporter.cmd_export_package ,())    #release
                   time.sleep(1)
             self.exporter.packageload    = 'FALSE'
             self.exporter.packageunload  = 'FALSE'

      def cmd_import_schema_package(self):
          if MONITOR == False:
             #stream package
             self.importer.cmd_import_schema_package()
             if  mutex.locked()  == True :
                 mutex.release()           #release
          if MONITOR == True:
             # stream loader
             thread.start_new_thread( self.importer.cmd_export_schema_package,())    #export
             # stream package
             while self.importer.packageload  == 'FALSE' :
                   thread.start_new_thread( self.importer.cmd_import_package   ,())    
                   time.sleep(1) 
             while self.importer.packageunload == 'FALSE' :
                   thread.start_new_thread( self.importer.cmd_monitor_package,())
                   thread.start_new_thread( self.importer.cmd_export_package ,())    #release
                   time.sleep(1)
             self.importer.packageload    = 'FALSE'
             self.importer.packageunload  = 'FALSE'

      def cmd_transport_table_package(self):
          input = None
          if( (self.options.sourcedbnode == self.options.targetdbnode)   and
              (self.options.sourcedbname == self.options.targetdbname) and 
               self.options.streamformat == 'SQL' ):
               granted = True
               self.exporter.cmd     = "SELECT PRIVILEGE FROM DOMAIN.TABLEPRIVILEGES WHERE GRANTEE = '%s' AND TABLENAME = '%s' AND PRIVILEGE LIKE 'SELECT' " %(re.split(',',self.options.targetdbuser)[0],self.package.table)    
               self.exporter.cmd_execute()
               if self.exporter.sqlCode == 100: 
                  self.exporter.cmd  = "GRANT SELECT ON %s.%s TO     %s"     %(re.split(',',self.options.sourcedbuser)[0],self.package.table, re.split(',',self.options.targetdbuser)[0])
                  self.exporter.cmd_execute()
                  self.exporter.cmd  = "COMMIT WORK"
                  self.exporter.cmd_execute()
                  granted = False
               self.importer.cmd = "CREATE TABLE %s.%s LIKE       %s.%s"     %(re.split(',',self.options.targetdbuser)[0],self.package.table,re.split(',',self.options.sourcedbuser)[0],self.package.table)   
               self.importer.cmd_execute()
               self.importer.cmd = "COMMIT WORK"
               self.importer.cmd_execute()
               self.importer.cmd = "INSERT INTO  %s.%s SELECT * FROM %s.%s"  %(re.split(',',self.options.targetdbuser)[0],self.package.table,re.split(',',self.options.sourcedbuser)[0],self.package.table)
               self.importer.cmd_execute()
               self.importer.cmd = "COMMIT WORK"
               self.importer.cmd_execute()
               if  granted == False:
                   self.exporter.cmd = "REVOKE SELECT ON %s.%s  FROM %s"     %(re.split(',',self.options.sourcedbuser)[0],self.package.table, re.split(',',self.options.targetdbuser)[0])
                   self.exporter.cmd_execute()
                   self.exporter.cmd = "COMMIT WORK"
                   self.exporter.cmd_execute()
               if mutex.locked() == True:
                  mutex.release()

          if self.options.streamformat != 'SQL' : 
             #stream filter
             self.streamfilter   = StreamFilter   (self.options , self )
             #Medium
             streamformat        = self.options.streamformat
             streamcharencoding  = self.options.streamcharencoding
             #Format
             streamformat_target = self.options.streamformat_target
             
             if self.package.column_field_long <> '' :
                if((self.options.streamformat     == 'FORMATTED' or
                    self.options.streamformat     == 'COMPRESSED'  )):
                    if self.options.streammedium  == 'PIPE' :
                       self.options.streamformat   = 'RECORDS'
                    if self.options.streamformat_target == 'PAGE' :
                       self.options.streamformat_target  = 'ROW'
                if self.options.streamformat      == 'RECORDS':
                   if self.options.streamcharencoding   == 'UTF8':
                      self.options.streamcharencoding    = 'UCS2'
             #stream export
             while input not in ["y"]:
                   if self.options.mode == 'INTER':
                      input = raw_input('Export    Table %s ? [y] : ' % ( self.package.table ))
                   if self.options.mode == 'PARAM':
                      input   = "y"
                   if input  == "y":
                      #stream loader
                      thread.start_new_thread( self.exporter.cmd_export_table_package,   ())    #export
                      if  self.options.streammedium == 'FILE'  :
                          #stream package
                          while  self.exporter.packageload  == 'FALSE' :
                                 thread.start_new_thread( self.exporter.cmd_import_package,())   
                                 time.sleep(1) 
                          while  self.exporter.packageunload == 'FALSE' :
                                 thread.start_new_thread( self.exporter.cmd_monitor_package,())
                                 thread.start_new_thread( self.exporter.cmd_export_package ,()) #release
                                 time.sleep(1)
             input = None
             #stream import
             while input not in ["y"]:
                   if self.options.mode == 'INTER':
                      input = raw_input('Import    Table %s ? [y] : ' % ( self.package.table ))
                   if self.options.mode == 'PARAM':
                      input   = "y"
                   if input  == "y":
                      #stream loader
                      thread.start_new_thread( self.importer.cmd_import_table_package, ())    #import
                      #stream package
                      while self.importer.packageload  == 'FALSE' :
                             thread.start_new_thread( self.importer.cmd_import_package  ,())    
                             time.sleep(1) 
                      while  self.importer.packageunload == 'FALSE':
                             thread.start_new_thread( self.importer.cmd_monitor_package,())
                             thread.start_new_thread( self.importer.cmd_export_package ,())   #release
                             time.sleep(1)
             #Medium
             if streamformat != self.options.streamformat :
                self.options.streamformat = streamformat
             if streamcharencoding != self.options.streamcharencoding :
                self.options.streamcharencoding = streamcharencoding
             #Format
             if streamformat_target != self.options.streamformat_target :
                self.options.streamformat_target = streamformat_target
             self.exporter.packageload   = 'FALSE'
             self.exporter.packageunload = 'FALSE'
             self.importer.packageload   = 'FALSE'
             self.importer.packageunload = 'FALSE'

      def cmd_export_table_package(self):
          if MONITOR == False:
             #stream package
             self.exporter.cmd_export_table_package()
             if  mutex.locked()  == True :
                 mutex.release()           #release
          if MONITOR == True:
             # stream loader
             thread.start_new_thread( self.exporter.cmd_export_table_package, ())    #export
             # stream package
             while self.exporter.packageload  == 'FALSE' :
                   thread.start_new_thread( self.exporter.cmd_import_package   ,())    
                   time.sleep(1) 
             while self.exporter.packageunload == 'FALSE' :
                   thread.start_new_thread( self.exporter.cmd_monitor_package,())
                   thread.start_new_thread( self.exporter.cmd_export_package ,())    #release
                   time.sleep(1)
             self.exporter.packageload    = 'FALSE'
             self.exporter.packageunload  = 'FALSE' 
		  
      def cmd_import_table_package(self):
          if MONITOR == False:
             #stream package
             self.importer.cmd_import_table_package()
             if  mutex.locked()  == True :
                 mutex.release()           #release
          if MONITOR == True:
             # stream connector
             self.streamconnector =  StreamConnector(self.exporter, self.importer )
             self.streamfilter    =  StreamFilter   (self.options , self )
             #stream filter
             self.streamfilter.cmd_dtlsource2dtltarget()
             #stream loader
             thread.start_new_thread(self.importer.cmd_import_table_package, ())    #import
             # stream package
             while self.importer.packageload  == 'FALSE' :
                   thread.start_new_thread( self.importer.cmd_import_package   ,())    
                   time.sleep(1) 
             while self.importer.packageunload == 'FALSE' :
                   thread.start_new_thread( self.importer.cmd_monitor_package,())
                   thread.start_new_thread( self.importer.cmd_export_package ,())   #release
                   time.sleep(1)
             self.importer.packageload    = 'FALSE'
             self.importer.packageunload  = 'FALSE'
                         
      def cmd_import_table_package_index(self):
          for cmd in self.package.indexStatements :
              self.importer.cmd = cmd
              self.importer.cmd_execute()

      def cmd_verify_package(self):
          input         = None
          sourcepackage = None
          targetpackage = None
          while input not in ["y"]:
                if self.options.mode == 'INTER':
                   input = raw_input('\nPackage verification   [y] : ')
                if self.options.mode == 'PARAM':
                   print '\n Package verification   :'
                   input   = "y"
                if input  == "y":
                   if  self.options.packagepart  in ['DB']   :
                       if  self.options.transportdirection == 'TRANSPORT'  :
                           if os.path.isfile                   (os.path.join(self.options.sourcepackagepath,'%s.PACKAGE' % self.options.packageexecution )):
                              sourcepackage =  PACKAGEIMPORT % (os.path.join(self.options.sourcepackagepath,'%s.PACKAGE' % self.options.packageexecution ),'')
                              if self.exporter.packageguid != None:
                                 pass#print 'Import    transformation package of EXPORT '   
                              if self.exporter.packageguid == None:   
                                 pass#print 'Import    transformation package of EXPORT ' 
                              try :
                                     self.importer.loadersession.cmd ("%s" %  sourcepackage  )
                              except loader.CommunicationError, err :
                                     raise loader.CommunicationError, err, sys.exc_traceback
                              except loader.LoaderError, err:
                                     raise loader.LoaderError, err, sys.exc_traceback

                       time.sleep(1)               
                       if  self.options.transportdirection == 'TRANSPORT'  :
                           if os.path.isfile                    (os.path.join(self.options.targetpackagepath,'%s.PACKAGE' % self.options.packageexecution )) :
                               if self.importer.catalog == 'UNICODE' :
                                  self.importer.package  = 'UCS2'
                               if self.importer.catalog == 'ASCII' :
                                  self.importer.package  = 'ASCII'
                               targetpackage =  PACKAGEIMPORT % (os.path.join(self.options.targetpackagepath,'%s.PACKAGE' % self.options.packageexecution ), '') 
                               if self.importer.packageguid != None:
                                  pass#print 'Import    transformation package of IMPORT ' 
                               if self.importer.packageguid == None:
                                  pass#print 'Import    transformation package of IMPORT ' 
                               try :
                                      self.importer.loadersession.cmd ("%s" %  targetpackage )
                               except loader.CommunicationError, err :
                                      raise loader.CommunicationError, err, sys.exc_traceback
                               except loader.LoaderError, err:
                                      raise loader.LoaderError, err, sys.exc_traceback

                       if  self.options.transportdirection == 'TRANSPORT'  :
                           time.sleep(1)
                           cmd = PACKAGEVALIDATE
                           cmd = cmd + '   WHERE  ( T1.start  <> T2.start  or T1.datastreamheaderpos <>  T2.datastreamheaderpos or  T1.datastreamdatapos <> T2.datastreamdatapos  ) '
                           cmd = cmd + '   AND T1.owner = T2.owner and T1.tablename = T2.tablename '
                           self.cmd = cmd
                           try :
                               self.importer.loadersession.cmd ("%s" % self.cmd  )
                           except loader.LoaderError, err:
                               if err.sqlCode == 100:
                                  print 'OK'
                               else:
                                  raise loader.LoaderError, err, sys.exc_traceback
                              
          if mutex.locked () == 1:
             mutex.release()  #release                           
             
      def cmd_purify_package(self):
          if  socket.gethostname() == 'p30882' :  #p30882
              self.exporter.loadersession.release()
              self.importer.loadersession.release()
              try :
                   os.chdir (os.path.join (os.environ ['INSTROOT'], 'wrk'))
              except OSError, (errno, err ):
                   print 'command failed: ', err

              #source
              if  os.path.isfile      ( self.exporter.protocol ) :
                  transformationstep = []
                  protocol      = file( self.exporter.protocol ,'r')
                  protocolline  = protocol.readline()
                  while protocolline :
                        match_1 = re.search ('// R', protocolline )
                        if match_1 != None :
                           cmd  = string.replace( protocolline , "// R\t" , '//\n')
                           transformationstep.append(cmd)
                        protocolline  = protocol.readline()
                  protocol.close()
                  time.sleep(1)
                  protocol      = file( self.exporter.protocol ,'a+')
                  protocol.writelines(transformationstep)
                  protocol.close()

                  sourceloader_batch  = string.replace(self.exporter.protocol, 'prt', 'bat' )
                  sourceloader_purify = string.replace(self.exporter.protocol, 'prt', 'pur' )
                  try:
                     os.rename(self.exporter.protocol, sourceloader_batch )
                  except OSError, err:
                     print 'command failed:', err
                  purify_loadercli  = '\n%s -b %s -E 10\n'   % (os.path.join(self.options.sourcepackagepath,'bin','loadercli'), sourceloader_batch)
                  #purify_loadercli  = 'purify /SaveTextData=%s /ExcludeSystemDir %s.exe -b %s'   % (sourceloader_purify,os.path.join( self.options.sourcepackagepath,'bin','loadercli'), sourceloader_batch)
                  print purify_loadercli
                  try:
                      os.system (purify_loadercli)
                  except OSError, err:
                      print 'command failed:', err

              #target
              if  os.path.isfile      ( self.importer.protocol ) :
                  transformationstep = []
                  protocol      = file( self.importer.protocol ,'r')
                  protocolline  = protocol.readline()
                  while protocolline :
                        match_1 = re.search ('// R', protocolline )
                        if match_1 != None :
                           cmd  = string.replace( protocolline , "// R\t" , '//\n')
                           transformationstep.append(cmd)
                        protocolline  = protocol.readline()
                  protocol.close()
                  time.sleep(1)
                  protocol      = file( self.importer.protocol ,'a+')
                  protocol.writelines(transformationstep)
                  protocol.close()

                  targetloader_batch  = string.replace(self.importer.protocol, 'prt', 'bat' )
                  targetloader_purify = string.replace(self.importer.protocol, 'prt', 'pur' )
                  try:
                     os.rename(self.importer.protocol, targetloader_batch )
                  except OSError, err:
                     print 'command failed:', err
                  purify_loadercli  = '\n%s -b %s -E 10\n'   % (os.path.join(self.options.targetpackagepath,'bin','loadercli'), targetloader_batch)
                  #purify_loadercli  = 'purify /SaveTextData=%s /ExcludeSystemDir %s.exe -b %s'   % (targetloader_purify,os.path.join(self.options.targetpackagepath,'bin','loadercli'),targetloader_batch)
                  print purify_loadercli
                  try:
                      os.system (purify_loadercli)
                  except OSError, err:
                      print 'command failed:', err

class Transformator:
      def __init__(self, options = None, package = None, transportdirection = None):
          self.options                = options
          self.packagepath            = None
          self.package                = package
          self.transportdirection     = transportdirection
          if self.transportdirection in ['EXPORT']:
             self.sapdbnode = options.sourcedbnode
             self.sapdb     = options.sourcedbname
             username = re.split(',',options.sourcedbuser)[0]
             password = re.split(',',options.sourcedbuser)[1]
             if (CASESENSITIVE == True and username != '') or (username != '' and username [0] != '"' and username[len(username)-1] != '"'):
                username = string.upper(username)
             elif username != '':
                username = string.replace(username,'"','') 
             self.user      = username
             self.pwd       = password
             self.schema    = self.options.sourcedbschema
             self.catalog   = DB_DEFAULTCODE_SOURCE
          if self.transportdirection in ['IMPORT']:
             self.sapdbnode = options.targetdbnode
             self.sapdb     = options.targetdbname
             username       = re.split(',',options.targetdbuser)[0]
             password       = re.split(',',options.targetdbuser)[1]
             if (CASESENSITIVE == True and username != '') or ( username   != '' and username [0] != '"' and username[len(username)-1] != '"'):
                username    = string.upper(username)
             elif username != '':
                username    = string.replace(username,'"','') 
             self.user      = username
             self.pwd       = password
             self.schema    = self.options.targetdbschema
             self.catalog   = DB_DEFAULTCODE_TARGET
          self.version                = ''
          self.streammedium           = options.streammedium
          self.streamformat           = options.streamformat
          self.streamcharencoding     = options.streamcharencoding
          self.initposition           = options.initposition
          self.owner                  = None
          self.table                  = None
          self.packagecode            = 'ASCII'
          self.packageguid            = None
          self.packagestart           = None
          self.packageend             = PACKAGETIMEOUT
          self.packageload            = 'FALSE'
          self.packageunload          = 'FALSE'
          self.protocol               = None
          self.cmd                    = None
          self.loaderpackage          = None
          #stream_name
          self.catalogstream_name     = None
          self.datastream_name        = None
          self.packagestream_name     = None 
          # medium pipe
          self.ddlpipe                = "\\\\.\PIPE\\catalog"
          self.datpipe                = "\\\\.\PIPE\\data"
          self.datpipe_source         = "\\\\.\PIPE\\source.data"
          self.datpipe_target         = "\\\\.\PIPE\\target.data"
          self.ddlpipe_source         = os.path.join('\\\\','.','PIPE','%s.CATALOG' % self.options.sourcedbname)
          self.ddlpipe_target         = os.path.join('\\\\','.','PIPE','%s.CATALOG' % self.options.targetdbname)
          # medium tape
          self.ddlpipe_ebid           = None
          self.datpipe_ebid           = None
          self.mappipe_ebid           = None
          # statement
          self.cmd
          self.monitor                = None
          self.sqlCode                = 0
          self.loadersession          = None
          self.errorCode              = 0
          self.execute                = False
          self.verbose                = True
          # protocol session
          if(self.transportdirection in ['EXPORT'] and os.path.isdir(self.options.sourcepackagepath) or 
             self.transportdirection in ['IMPORT'] and os.path.isdir(self.options.targetpackagepath)    ):
             try:
                 if self.package != None:
                    if self.transportdirection in ['TRANSPORT','EXPORT'] :
                       self.loaderpackage  = file(os.path.join(self.options.sourcepackagepath,os.path.split(self.options.sourcepackagepath)[1]) + '.EXPORT' ,'w+')
                    if self.transportdirection in ['TRANSPORT','IMPORT'] :
                       self.loaderpackage  = file(os.path.join(self.options.targetpackagepath,os.path.split(self.options.targetpackagepath)[1]) + '.IMPORT' ,'w+')
             except IOError, err:
                 if self.transportdirection in ['TRANSPORT','EXPORT'] :
                    print 'Package file %s could not be opened: %s ' %(os.path.join(self.options.sourcepackagepath,os.path.split(self.options.sourcepackagepath)[1]) + '.EXPORT',err)
                 if self.transportdirection in ['TRANSPORT','IMPORT'] :
                    print 'Package file %s could not be opened: %s ' %(os.path.join(self.options.targetpackagepath,os.path.split(self.options.targetpackagepath)[1]) + '.IMPORT',err)
             # sql session
             try:
                 if self.transportdirection in ['EXPORT'] and EXPORTMONITOR not in [None]:
                    self.monitor = EXPORTMONITOR
                 if self.transportdirection in ['EXPORT'] and EXPORTMONITOR     in [None]:
                    self.monitor = sql.connect ( self.user, self.pwd, self.sapdb, self.sapdbnode )
                 if self.transportdirection in ['IMPORT'] and IMPORTMONITOR not in [None]:
                    self.monitor = IMPORTMONITOR
                 if self.transportdirection in ['IMPORT'] and IMPORTMONITOR     in [None]:
                    self.monitor = sql.connect ( self.user, self.pwd, self.sapdb, self.sapdbnode )
             except sql.CommunicationError, err :
                 print err
                 raise sql.CommunicationError, err, sys.exc_traceback
             except sql.SQLError, err :
                 print 'Monitor session could not be established: %s' %err
             # loader session
             try:
                 global PACKAGEPROTOCOL
                 if self.transportdirection == 'EXPORT':
                    self.packagepath = self.options.sourcepackagepath
                 if self.transportdirection == 'IMPORT':
                    self.packagepath = self.options.targetpackagepath
                 if PACKAGEPROTOCOL != 'HOME':
                    os.putenv('maxdbloaderpackagepath'     , self.packagepath)
                    os.putenv('maxdbloaderpackageprotocol' , 'loader_%s.log' %                        self.options.packageexecution)
                 os.putenv('maxdbloaderpackageexecution', os.path.join(self.options.sourcepackagepath,self.options.packageexecution))
                 if self.options.packageexecution == self.options.packagecreation:
                    os.putenv('maxdbloaderpackagename'  , os.path.join(self.options.sourcepackagepath,self.options.packagename)     )
                 self.loadersession =  loader.Loader()
                 self.cmd = 'SET ODBCTRACE        %s\n' % ODBCTRACE
                 self.cmd_execute()
                 #input = raw_input('Loader started  ' )
                 #Module Name
                 self.cmd = 'REMARK "Loader Extension  Module: %s"'% os.path.abspath(sys.argv[0])
                 remark = self.cmd_execute()
                 #Configuration Directory
                 self.cmd = 'REMARK "Loader Config  Directory: %s"'% os.path.join (LOADERPROGPATH,'sdb','loader','config')
                 remark = self.cmd_execute()
                 #Package Directory
                 self.cmd = 'REMARK "Loader Package Directory: %s"'% os.path.join (PACKAGEPATH   ,'sdb','loader','packages')
                 remark = self.cmd_execute()
                 #protocol loader
                 self.cmd = 'SET\n'
                 configuration = self.cmd_execute()
                 if configuration != 0:
                    loaderpackages = re.split('Loader packages:', configuration)[0]
                    if TESTLOADER in ['TRUE'] and self.transportdirection in ['EXPORT'] :
                        print '\nLoader packages:%s' % string.replace(re.split('Loader packages:', configuration)[1],'\n','')
                    loaderprotocol = string.replace(loaderpackages,'Loader protocol:','')
                    loaderprotocol = string.strip(loaderprotocol)
                    loaderprotocol = string.replace(loaderprotocol,'\'','')
                    self.protocol =  string.strip(loaderprotocol)
                    if TESTLOADER in ['TRUE'] and self.transportdirection in ['EXPORT'] :
                       print "Loader protocol: '%s'" % string.replace(self.protocol,'\n','')
                 #format
                 self.fieldCompressed           = FIELDCOMPRESSED
                 #datatypes
                 self.fieldBoolValue            = FIELDBOOLVALUE
                 self.fieldDecimalValue         = FIELDDECIMALVALUE
                 self.fieldNullValue            = FIELDNULLVALUE
                 self.fieldDateValue            = FIELDATEVALUE
                 self.fieldTimeValue            = FIELDTIMEVALUE
                 self.fieldTimeStampValue       = FIELDTIMESTAMPVALUE
		  
             except loader.CommunicationError, err :
                    raise loader.CommunicationError, err, sys.exc_traceback
             except loader.LoaderError, err:
                    raise loader.LoaderError, err, sys.exc_traceback
          else:
             if self.transportdirection in ['EXPORT'] and os.path.isdir(self.options.sourcepackagepath) in [False]:
                print 'Package path % for export does not exist' % self.options.sourcepackagepath
                return None
             if self.transportdirection in ['IMPORT'] and os.path.isdir(self.options.targetpackagepath) in [False]:
                print 'Package path % for import does not exist' % self.options.targetpackagepath
                return None
		  
      def release(self):
          # sql session
          if self.monitor != None:
             try:
                self.monitor.release()
             except sql.CommunicationError, err :
                pass
             except:
                pass
          # loader session
          if self.loadersession != None:
             try:
                 self.loadersession.release()
             except loader.CommunicationError, err :
                    raise loader.CommunicationError, err, sys.exc_traceback
             except loader.LoaderError, err:
                    raise loader.LoaderError, err, sys.exc_traceback
          # protocol session
          if self.loaderpackage != None:
             try:
                 time.sleep(0.5)
                 self.loaderpackage.close()
             except IOError, err:
                 print err
          
      def cmd_execute(self):
          rc  = 0
          self.packagestart = time.time()
          cmd = ''
          cmd = self.cmd
          cmd = string.replace(cmd,self.options.sourcepackagepath + os.sep , '')
          cmd = string.replace(cmd,self.options.targetpackagepath + os.sep , '')
          cmd = '\n%s//' % cmd
          #generate
          if self.options.transporttype == 'GENERATE' : 
             self.packageload   = 'TRUE'
             self.packageunload = 'TRUE'
             try :
                  if self.loaderpackage != None and self.cmd[0:8] == 'USE USER':
                     if self.package.schema != '':
                        cmd  = '\nUSE USER %s %s SCHEMA %s SERVERDB %s ON %s \n//'% ( self.user,'*', self.package.schema, self.sapdb, self.sapdbnode)
                     else:
                        cmd  = '\nUSE USER %s %s           SERVERDB %s ON %s \n//'% ( self.user,'*',                      self.sapdb, self.sapdbnode)
                  if self.loaderpackage != None and self.verbose == True and self.cmd[0:6] != 'REMARK' :
                     self.loaderpackage.write(cmd)
             except IOError, err:
                   print err
             if mutex.locked() == True:
                mutex.release()    #release
          #execute
          if self.options.transporttype == 'EXECUTE' or self.execute == True:    
             try :
                 rc = self.loadersession.cmd ("%s" % self.cmd )
             except loader.LoaderError, err:
                 if self.package != None and self.package.packageref != None:
                    self.cmd_error_handler(err)
                 if self.package.packageref == None:
                    print err  
                    self.package.transformation.release()
                    os._exit(1) 
             except loader.CommunicationError, err :
                 print err
                 print 'Received cancel request; stopping execution'
                 self.package.transformation.release()
                 os._exit(1)
             try :
                 if self.loaderpackage != None and self.cmd[0:8] == 'USE USER':
                    if self.schema != '':
                       cmd  = '\nUSE USER %s %s SCHEMA %s SERVERDB %s ON %s \n//'% ( self.user,'*', self.schema, self.sapdb, self.sapdbnode)
                    else:
                       cmd  = '\nUSE USER %s %s           SERVERDB %s ON %s \n//'% ( self.user,'*',              self.sapdb, self.sapdbnode)
                 if self.loaderpackage != None and self.verbose == True and self.cmd[0:6] != 'REMARK':
                    self.loaderpackage.write(cmd)
             except IOError, err:
                   print err
          self.cmd = ''
          return rc
      
      def cmd_error_handler(self,err):
          self.sqlCode          = err.sqlCode
          self.errorCode        = err.errorCode
          if( err.sqlCode       ==  100   or
              err.sqlCode       ==  200   or
              err.sqlCode       == -4004  or
              err.sqlCode       == -4011  or
              err.sqlCode       == -5001  or
              err.sqlCode       == -6000  or
              err.sqlCode       == -6008  or
              err.sqlCode       == -7017  or
              err.errorCode     == -1     or
              err.errorCode     == -25010 or
              err.errorCode     == -25362 or
              err.errorCode     == -25364 or
              err.errorCode     == -25392 or
              err.errorCode     == -25498 or
              err.errorCode     == -25500 or
              err.errorCode     == -25501  ):
              if( err.sqlCode   ==  200   or
                  err.errorCode == -1     or
                  err.errorCode == -25010 or
                  err.errorCode == -25362 or
                  err.errorCode == -25364 or
                  err.errorCode == -25392 or
                  err.errorCode == -25498 or
                  err.errorCode == -25500 or
                  err.errorCode == -25501 ):
                  if err.sqlCode   ==  200  and self.options.streamformat != 'SQL' :
                     print '\nErrors occured executing the package: \n---> See loader server logfile for more information.' 
                     self.package.transformation.release()
                     os._exit(1)
                  if(err.errorCode == -1 or 
                     err.errorCode == -25364 or
                     err.errorCode == -25392 or
                     err.errorCode == -25500  ):
                     print err
                     self.package.transformation.release()
                     os._exit(1)
                  if err.errorCode == -25010 :
                     pass
                  if err.errorCode == -25362 :
                     pass
                  if err.errorCode == -25498 :
                     pass
                  if err.errorCode == -25501 :
                     print err
                     print '\nErrors occured executing the package: \n---> See loader server logfile for more information.' 
                     self.package.transformation.release()
                     os._exit(1)
              elif self.package.maxerrorcount == 1 :
                 print '\nErrors occured executing the package: \n---> See loader server logfile for more information.' 
              elif self.package.maxerrorcount == MAXERRORCOUNT :
                 self.package.transformation.release()
                 os._exit(1)
              else:
                 self.package.maxerrorcount = self.package.maxerrorcount + 1
          else:
             raise loader.LoaderError, err, sys.exc_traceback

      def cmd_use_serverdb_user(self):
          if self.schema != '':
              cmd     = 'USE USER %s %s SCHEMA %s SERVERDB %s' % ( self.user, self.pwd, self.schema, self.sapdb )
          else:
              cmd     = 'USE USER %s %s           SERVERDB %s' % ( self.user, self.pwd,              self.sapdb )
          if self.sapdbnode != '':
             cmd  = cmd + ' ON %s\n'             % ( self.sapdbnode )
          self.cmd     = cmd
          self.cmd_execute()

      def cmd_export_catalog_package(self):
          #Medium
          if   self.streammedium[0:4] == 'PIPE' :
               streammedium = 'PIPE'
          else :
               streammedium = self.streammedium

          #Format
          if( self.streamformat == 'DDL' or
              self.streamformat == 'XML'    ):
              #Medium
              if self.options.streammedium == 'FILE' :
                 cmd = "CATALOGEXTRACT ALL"
                 cmd = cmd +   " OUTSTREAM FILE '%s' %s          "   % (os.path.join(self.options.sourcepackagepath,  '%s.catalog' % (self.options.sourcedbname )),self.options.streamformat,)
              #Medium
              if(self.options.streammedium == 'TAPE' or
                 self.options.streammedium == 'PIPE'   ):
                 cmd = 'CATALOGEXTRACT ALL \n'
                 cmd = cmd +   " OUTSTREAM  %s '%s'  %s    %s    "   % (streammedium, self.ddlpipe_source, self.options.streamformat, self.streamcharencoding )

              if self.streamformat  == 'XML':
                 self.packageload    = 'TRUE'
                 self.packageunload  = 'TRUE'
                 mutex.release()

          #Format
          if( self.streamformat == 'PAGES'   or
              self.streamformat == 'RECORDS'    ):
              restartRecord = self.cmd_restart_package()
              #Medium
              if self.options.streammedium == 'FILE' :
                 cmd = 'DBEXTRACT \n'
                 cmd = cmd +   " CONFIGURATION  INSTREAM  FILE '%s' COMPRESSED  %s     \n"  % (os.path.join(self.options.sourcepackagepath,  '%s.configuration' % self.sapdb ), self.options.streamcharencoding )
                 cmd = cmd +   " CATALOG        OUTSTREAM FILE '%s' DDL         %s     \n"  % (os.path.join(self.options.sourcepackagepath,  '%s.catalog'       % self.sapdb ), self.streamcharencoding )
                 cmd = cmd +   " DATA           OUTSTREAM FILE '%s' %s          %s     \n"  % (os.path.join(self.options.sourcepackagepath,  '%s.data'          % self.sapdb ), self.options.streamformat, self.streamcharencoding )
                 cmd = cmd +   " PACKAGE        OUTSTREAM FILE '%s' COMPRESSED  %s       "  % (os.path.join(self.options.sourcepackagepath,  '%s.package'       % self.sapdb ), self.streamcharencoding )

              #Medium
              if(self.options.streammedium == 'TAPE' or
                 self.options.streammedium == 'PIPE'   ):
                 cmd = 'DBEXTRACT \n'
                 cmd = cmd +   " CONFIGURATION  INSTREAM  FILE '%s'  COMPRESSED %s     \n"  % (os.path.join(self.options.sourcepackagepath,  '%s.configuration' % self.sapdb ), self.streamcharencoding )
                 cmd = cmd +   " CATALOG        OUTSTREAM %s   '%s'  DDL        %s     \n"  % (streammedium, self.ddlpipe_source                     , self.streamcharencoding )
                 cmd = cmd +   " DATA           OUTSTREAM %s   '%s'  %s         %s     \n"  % (streammedium, self.datpipe_source , self.streamformat , self.streamcharencoding )
                 cmd = cmd +   " PACKAGE        OUTSTREAM FILE '%s'  COMPRESSED %s       "  % (os.path.join(self.options.sourcepackagepath,  '%s.package'       % self.sapdb ), self.streamcharencoding )
              #Start
              if restartRecord <> [] :
                 cmd = cmd + " RESTART '%s' " % (restartRecord[0])

          if TRANSFORMATIONSYNTAX == 'STRATIFIED':
             self.cmd = cmd
          if TRANSFORMATIONSYNTAX == 'UNIFIED':
             self.cmd = self.package.transformation.exportstatement
          self.cmd_execute()

      def cmd_import_catalog_package(self):
          #Medium
          if  self.streammedium      == 'TAPE' :
              self.ddlpipe_target =  self.ddlpipe_source
              self.datpipe_source =  self.datpipe_source
          if  self.streammedium[0:4] == 'PIPE' :
              streammedium = 'PIPE'
          else:
              streammedium = self.streammedium

          #Encoding
          if self.catalog == 'UNICODE' :
             self.packagecode  = 'UCS2'
          if self.catalog == 'ASCII' :
             self.packagecode  = 'ASCII'

          #Format
          if( self.streamformat == 'DDL' or
              self.streamformat == 'XML'   ):
              cmd = 'CATALOGLOAD ALL '
              #Medium
              if self.streammedium      == 'FILE':
                 cmd = cmd + "                   INSTREAM FILE '%s'             %s          " % (os.path.join(self.options.sourcepackagepath,   '%s.catalog' % (self.options.sourcedbname )),self.streamformat)
              #Medium
              if self.streammedium[0:4] == 'PIPE' :
                 cmd = cmd + "                   INSTREAM  %s  '%s'             %s  %s      " % (streammedium, self.ddlpipe_target                       , self.streamformat, self.streamcharencoding   )
              #Medium
              if self.streammedium      == 'TAPE':
                 cmd = cmd + "                   INSTREAM  %s  EBID %s  '%s'    %s  %s      " % (streammedium, self.ddlpipe_ebid , self.ddlpipe_target   , self.streamformat, self.streamcharencoding   )

              if self.streamformat  == 'XML':
                 self.packageload    = 'TRUE'
                 self.packageunload  = 'TRUE'

          #Format
          if( self.streamformat == 'PAGES'   or
              self.streamformat == 'RECORDS'    ):
              restartRecord = self.cmd_restart_package()

              #Medium
              if self.streammedium[0:4] == 'FILE' :
                 cmd = 'DBLOAD \n'
                 cmd = cmd +    " CONFIGURATION  INSTREAM FILE  '%s' COMPRESSED  %s   \n" % (os.path.join(self.options.sourcepackagepath,  '%s.configuration' % self.sapdb), self.streamcharencoding )
                 cmd = cmd +    " CATALOG        INSTREAM FILE  '%s' DDL         %s   \n" % (os.path.join(self.options.sourcepackagepath,  '%s.catalog'       % self.options.sourcedbname), self.streamcharencoding)
                 cmd = cmd +    " DATA           INSTREAM FILE  '%s' %s          %s   \n" % (os.path.join(self.options.sourcepackagepath,  '%s.data'          % self.options.sourcedbname), self.options.streamformat, self.streamcharencoding)
              #Medium
              if self.streammedium[0:4] == 'PIPE' :
                 cmd = 'DBLOAD \n'
                 cmd = cmd +    " CONFIGURATION  INSTREAM FILE  '%s' COMPRESSED  %s   \n" % (os.path.join(self.options.sourcepackagepath,  '%s.configuration' % self.sapdb), self.streamcharencoding )
                 cmd = cmd +    " CATALOG        INSTREAM  %s   '%s' DDL         %s   \n" % (streammedium, self.ddlpipe_target                     , self.streamcharencoding   )
                 cmd = cmd +    " DATA           INSTREAM  %s   '%s' %s          %s   \n" % (streammedium, self.datpipe_source , self.streamformat , self.streamcharencoding   )
              #Medium
              if self.streammedium      == 'TAPE':
                 cmd = 'DBLOAD \n'
                 cmd = cmd +    " CONFIGURATION  INSTREAM FILE  '%s' COMPRESSED  %s        \n" % (os.path.join(self.options.sourcepackagepath,  '%s.configuration' % self.sapdb), self.streamcharencoding )
                 cmd = cmd +    " CATALOG        INSTREAM  %s   EBID %s  '%s'   DDL   %s   \n" % (streammedium, self.ddlpipe_ebid , self.ddlpipe_target ,                       self.streamcharencoding   )
                 cmd = cmd +    " DATA           INSTREAM  %s   EBID %s  '%s'    %s   %s   \n" % (streammedium, self.mappipe_ebid , self.datpipe_source , self.streamformat   , self.streamcharencoding   )
              cmd = cmd +       " PACKAGE        OUTSTREAM FILE '%s' COMPRESSED  %s          " % (os.path.join(self.options.targetpackagepath,  '%s.package' % (self.sapdb)), self.packagecode )
              #Start
              if restartRecord <> [] :
                 cmd = cmd +    " RESTART '%s' " % (restartRecord[0])

          if self.streamformat  <> 'XML':
             if TRANSFORMATIONSYNTAX == 'STRATIFIED':
                self.cmd = cmd
             if TRANSFORMATIONSYNTAX == 'UNIFIED':
                self.cmd = self.package.transformation.importstatement
             self.cmd_execute()

      def cmd_export_schema_package(self):
          #Medium
          if   self.streammedium[0:4] == 'PIPE' :
               streammedium = 'PIPE'
          else :
               streammedium = self.streammedium
          #Format
          if( self.streamformat == 'PAGES'   or
              self.streamformat == 'RECORDS'    ):
              cmd = 'TABLEEXTRACT USER \n'
              cmd = cmd +      " CONFIGURATION  INSTREAM  FILE '%s'    COMPRESSED %s \n" % (os.path.join(self.options.sourcepackagepath,   '%s.configuration' % (self.sapdb )), self.streamcharencoding )
              #Medium
              if(self.options.streammedium == 'TAPE' or
                 self.options.streammedium == 'PIPE'   ):
                 cmd = cmd +   " DATA           OUTSTREAM %s   '%s'    %s         %s \n" % (streammedium, self.datpipe_source , self.streamformat  , self.streamcharencoding )
                 cmd = cmd +   " PACKAGE        OUTSTREAM FILE '%s'    COMPRESSED %s   " % (os.path.join (self.options.sourcepackagepath,   '%s.package'       % (self.sapdb )), self.streamcharencoding )
              #Medium
              if(self.options.streammedium == 'FILE'     ):
                 cmd = cmd +   " DATA           OUTSTREAM FILE '%s'    %s         %s \n" % (os.path.join(self.options.sourcepackagepath,   '%s_%s.data'       % (self.options.sourcedbname,self.user )),self.options.streamformat,self.streamcharencoding )
                 cmd = cmd +   " PACKAGE        OUTSTREAM FILE '%s'    COMPRESSED %s   " % (os.path.join(self.options.sourcepackagepath,   '%s_%s.package'    % (self.options.sourcedbname,self.user)), self.streamcharencoding )
          #Format
          if( self.streamformat == 'DDL' or
              self.streamformat == 'XML'      ):
              cmd = 'CATALOGEXTRACT USER \n'
              #Medium
              if(self.options.streammedium == 'TAPE' or
                 self.options.streammedium == 'PIPE'   ):
                 cmd = cmd +   " OUTSTREAM %s   '%s'  %s   %s  "  % ( streammedium, self.ddlpipe_source, self.options.streamformat, self.streamcharencoding )
              #Medium
              if self.options.streammedium == 'FILE'  :
                 cmd = cmd +   " OUTSTREAM FILE '%s'  %s   %s  "    % (os.path.join(self.options.sourcepackagepath,   '%s_%s.dtl'  % (self.sapdb,self.user )),self.options.streamformat, self.streamcharencoding)

          if TRANSFORMATIONSYNTAX == 'STRATIFIED':
             self.cmd = cmd
          if TRANSFORMATIONSYNTAX == 'UNIFIED':
             self.cmd = self.package.transformation.exportstatement
          self.cmd_execute()

      def cmd_import_schema_package(self):
          #Medium
          if  self.streammedium      == 'TAPE' :
              self.ddlpipe_target =  self.ddlpipe_source
              self.datpipe_source =  self.datpipe_source
          if  self.streammedium[0:4] == 'PIPE' :
              streammedium = 'PIPE'
          else:
              streammedium = self.streammedium

          if self.catalog == 'UNICODE' :
             self.packagecode  = 'UCS2'
          if self.catalog == 'ASCII' :
             self.packagecode  = 'ASCII'

          #Format
          if( self.streamformat == 'PAGES'   or
              self.streamformat == 'RECORDS'    ):
              cmd = 'TABLELOAD    USER \n'
              cmd =       cmd + " CONFIGURATION INSTREAM  FILE '%s' COMPRESSED %s \n" % (os.path.join(self.options.sourcepackagepath,  '%s.configuration' % (self.sapdb )), self.streamcharencoding )
              #Medium
              if self.streammedium[0:4] == 'PIPE':
                 cmd =    cmd + " DATA          INSTREAM  %s   '%s' %s         %s \n" % (streammedium, self.datpipe_source , self.streamformat , self.streamcharencoding   )
                 cmd =    cmd + " PACKAGE       OUTSTREAM FILE '%s' COMPRESSED %s   " % (os.path.join(self.options.targetpackagepath,  '%s.package' % (self.sapdb)), self.packagecode )
              #Medium
              if self.streammedium      == 'TAPE':
                 cmd =    cmd + " DATA          INSTREAM  %s   '%s' EBID %s %s %s \n" % (streammedium, self.datpipe_source , self.mappipe_ebid , self.streamformat , self.streamcharencoding   )
                 cmd =    cmd + " PACKAGE       OUTSTREAM FILE '%s' COMPRESSED %s   " % (os.path.join(self.options.targetpackagepath,  '%s.package' % (self.sapdb)), self.packagecode )
              #Medium
              if self.streammedium      == 'FILE':
                 if self.options.streamformat   == 'PAGES'   :
                    cmd = cmd + " DATA          INSTREAM  FILE '%s' %s            \n" % (os.path.join(self.options.sourcepackagepath,  '%s_%s.data'       % (self.options.sourcedbname,self.user )),self.options.streamformat)
                    cmd = cmd + " PACKAGE       OUTSTREAM FILE '%s' COMPRESSED %s   " % (os.path.join(self.options.targetpackagepath,  '%s_%s.package'    % (self.options.targetdbname,self.user)), self.packagecode)
                 if self.options.streamformat   == 'RECORDS' :
                    cmd = cmd + " DATA          INSTREAM  FILE '%s' %s         %s \n" % (os.path.join(self.options.sourcepackagepath,  '%s_%s.data'       % (self.options.sourcedbname,self.user )),self.options.streamformat,self.streamcharencoding)
                    cmd = cmd + " PACKAGE       OUTSTREAM FILE '%s' COMPRESSED %s   " % (os.path.join(self.options.targetpackagepath,  '%s_%s.package'    % (self.options.targetdbname,self.user)), self.packagecode)
          #Format
          if( self.streamformat == 'DDL' or
              self.streamformat == 'XML'      ):
              cmd = 'CATALOGLOAD USER\n'
              #Medium
              if self.streammedium[0:4] == 'PIPE' :
                 cmd = cmd + "                INSTREAM  %s  '%s'          %s  %s  \n" % (streammedium, self.ddlpipe_target                     , self.options.streamformat  , self.streamcharencoding  )
              #Medium
              if self.streammedium      == 'TAPE':
                 cmd = cmd + "                INSTREAM  %s  '%s' EBID %s  %s  %s  \n" % (streammedium, self.ddlpipe_target , self.ddlpipe_ebid , self.options.streamformat , self.streamcharencoding   )
              #Medium
              if self.streammedium      == 'FILE':
                 cmd = cmd + "                INSTREAM FILE '%s'          %s   %s   " % (os.path.join(self.options.sourcepackagepath,  '%s_%s.dtl'  % (self.options.sourcedbname,self.user )), self.options.streamformat, self.streamcharencoding )

              if self.streamformat  == 'XML':
                 self.packageload    = 'TRUE'
                 self.packageunload  = 'TRUE'

          if self.streamformat  <> 'XML':
             if TRANSFORMATIONSYNTAX == 'STRATIFIED':
                self.cmd = cmd
             if TRANSFORMATIONSYNTAX == 'UNIFIED':
                self.cmd = self.package.transformation.importstatement
             self.cmd_execute()

      def cmd_export_table_package(self):
          sapdb        = self.package.transformation.exporter.sapdb
          user         = self.package.user
          table        = self.package.table

          if TRANSFORMATIONSYNTAX == 'STRATIFIED':
             #Medium
             if self.options.streammedium    == 'FILE'  :
                #Format
               if(self.options.streamformat == 'XML' or
                  self.options.streamformat == 'DDL' ) :
                  cmd = 'CATALOGEXTRACT TABLE '
                  cmd = cmd + " \"%s\".\"%s\" \n"                 % (user, table)
                  cmd = cmd + "      OUTSTREAM FILE '%s' %s   \n" % (os.path.join(self.options.sourcepackagepath, '%s_%s_%s.dtl'  % (sapdb,user,table )),self.options.streamformat)
               #Format
               if(self.options.streamformat == 'PAGES' or
                  self.options.streamformat == 'RECORDS'  ):
                  cmd = 'TABLEEXTRACT TABLE '
                  cmd = cmd + " \"%s\".\"%s\" \n"                 % ( user, table)
                  cmd = cmd + " DATA OUTSTREAM FILE '%s' %s %s\n" % (os.path.join(self.options.sourcepackagepath, '%s_%s_%s.data' % (sapdb,user,table )),self.options.streamformat,self.options.streamcharencoding)
               #Format
               if(self.options.streamformat[0:9] == 'FORMATTED'  or
                  self.options.streamformat      == 'COMPRESSED'    ):
                  cmd = "DATAEXTRACT "
                  if self.options.streamformat_target == 'ROW':
                     cmd  = cmd + " FOR DATALOAD TABLE "
                  if self.options.streamformat_target == 'PAGE':
                     cmd  = cmd + " FOR FASTLOAD TABLE "
                  cmd = cmd + " \"%s\".\"%s\" \n"                 % (user, table)
                  cmd = cmd + "      OUTSTREAM FILE '%s'      \n" % (os.path.join(self.options.sourcepackagepath, '%s_%s_%s.dtl'  % (sapdb,user,table )))
                  cmd = cmd + "      OUTSTREAM FILE '%s' %s %s\n" % (os.path.join(self.options.sourcepackagepath, '%s_%s_%s.data' % (sapdb,user,table )),self.options.streamformat,self.options.streamcharencoding)
                  cmd = cmd + self.package.column_field_long
             #Medium
             if(self.options.streammedium[0:4] == 'PIPE' ):
                #Format
                if( self.options.streamformat == 'PAGES' or
                    self.options.streamformat == 'RECORDS'  ) :
                    cmd  = "TABLEEXTRACT TABLE "
                    cmd  = cmd + " \"%s\".\"%s\" \n"                 % ( user, table )
                    cmd  = cmd + " DATA OUTSTREAM PIPE '%s' %s %s\n" % ('\\\\.\\PIPE\\%s_%s_%s.dat' % ( sapdb, user, table ), self.options.streamformat, self.options.streamcharencoding )
                #Format
                if( self.options.streamformat == 'FORMATTED'  or
                    self.options.streamformat == 'COMPRESSED'      ):
                    cmd  = "DATAEXTRACT "
                    if self.options.streamformat_target == 'ROW':
                       cmd  = cmd + " FOR DATALOAD TABLE "
                    if self.options.streamformat_target == 'PAGE':
                       cmd  = cmd + " FOR FASTLOAD TABLE "
                    cmd  = cmd + " \"%s\".\"%s\" \n"               % (user, table)
                    cmd  = cmd + " OUTSTREAM PIPE '%s'         \n" % ('\\\\.\\PIPE\\%s_%s_%s.dtl' % ( sapdb, user, table ) )
                    cmd  = cmd + " OUTSTREAM PIPE '%s' %s   %s   " % ('\\\\.\\PIPE\\%s_%s_%s.dat' % ( sapdb, user, table ), self.options.streamformat, self.options.streamcharencoding )
                    cmd  = cmd + self.package.column_field_long
             self.cmd = cmd
             self.cmd_execute()       
          
          if TRANSFORMATIONSYNTAX == 'UNIFIED':
             if exist_table( self.options, EXPORTMONITOR, IMPORTMONITOR, self.package.schema, self.package.table) == True:
                self.cmd = self.package.transformation.exportstatement
                self.cmd_execute()       

      def cmd_import_table_package(self):
          sapdb  = self.options.sourcedbname
          user   = self.package.user
          table  = self.package.table

          if TRANSFORMATIONSYNTAX == 'STRATIFIED':
             #Medium
             if self.options.streammedium  == 'FILE':
                #Format
                if( self.options.streamformat == 'PAGES'   or
                    self.options.streamformat == 'RECORDS'    ) :
                    cmd = 'TABLELOAD TABLE '
                    cmd = cmd + " \"%s\".\"%s\" \n"                % (user, table)
                    cmd = cmd + " DATA INSTREAM FILE '%s' %s %s\n" % (os.path.join(self.options.sourcepackagepath,   '%s_%s_%s.data' % (sapdb,user,table )),self.options.streamformat,self.options.streamcharencoding)
                    self.cmd = cmd
                    self.cmd_execute()
                #Format
                if( self.options.streamformat == 'DDL'        or
                    self.options.streamformat == 'FORMATTED'  or
                    self.options.streamformat == 'COMPRESSED'   ):
                    for cmd in self.package.transformation_step :
                        self.cmd = cmd
                        self.cmd_execute()
                        #grant and execute
                        if string.find(cmd,'Create') == 0 and string.find(cmd,'Table')  == 0 :
                           self.cmd= 'GRANT ALL ON %s TO PUBLIC' % table
             #Medium
             if(self.options.streammedium[0:4] == 'PIPE' ):
                #Format
                if( self.options.streamformat == 'PAGES'   or
                    self.options.streamformat == 'RECORDS') :
                    cmd      = 'TABLELOAD TABLE '
                    cmd      = cmd + " \"%s\".\"%s\" \n"               % ( user, table )
                    cmd      = cmd + " DATA INSTREAM PIPE '%s' %s %s " % ('\\\\.\\PIPE\\%s_%s_%s.dat' % ( sapdb, user, table ), self.options.streamformat, self.options.streamcharencoding)
                    self.cmd = cmd
                    self.cmd_execute()
                #Format
                if( self.options.streamformat == 'DDL'       or
                    self.options.streamformat == 'FORMATTED' or
                    self.options.streamformat == 'COMPRESSED'   ) :
                    for cmd in self.package.transformation_step :
                        self.cmd = cmd
                        self.cmd_execute()
                        #grant and execute
                        if string.find(cmd,'Create') == 0 and string.find(cmd,'Table')  == 0 :
                           self.cmd= 'GRANT ALL ON %s TO PUBLIC' % table
             self.package.transformation_step = []

          if TRANSFORMATIONSYNTAX == 'UNIFIED':
             if self.options.streamformat not in ['PAGES'] and self.options.streamformat_target in ['PAGE'] and len(self.package.indexStatements)>0:
                self.package.transformation.importstatement = string.replace(self.package.transformation.importstatement,"PAGE WITH 80 % USAGE","RECORDS USAGE")
             self.cmd = self.package.transformation.importstatement
             self.cmd_execute()    
          '''   
          if self.options.streamformat not in ['PAGES'] and self.options.streamformat_target in ['PAGE']: 
             self.package.transformation.cmd_import_table_package_index()  
          '''       

      def cmd_import_package(self):
          if self.options.transporttype == 'EXECUTE':
             try:
                 self.packagestart = time.time()
                 match_0     = None
                 match_1     = None
                 match_2     = None
                 match_3     = None
                 user        = None
                 packageguid = None
                 if  os.path.isfile      ( self.protocol ) :
                     protocol      = file( self.protocol ,'r')
                     protocolline  = protocol.readline()
                     while protocolline and self.packageload  == 'FALSE' :
                           #timestamp user
                           if match_0 == None:
                              match_0  = re.search ("START     PACKAGE x'%s' WITH TIMESTAMP %s" % (self.package.packageguid, self.options.packageexecution ), protocolline )
                           #user
                           if match_0 != None and match_1 == None:
                              if  'TRANSPORT' == self.transportdirection or 'EXPORT' == self.transportdirection:
                                   if TESTLOADER == 'TRUE' and self.options.packageref != self.options.packagepart:
                                      user = self.package.user
                                   else:
                                      user = re.split(',',self.options.sourcedbuser)[0]
                              if  'TRANSPORT' == self.transportdirection or 'IMPORT' == self.transportdirection :
                                   if TESTLOADER == 'TRUE' and self.options.packageref != self.options.packagepart:
                                      user = self.package.user
                                   else:
                                      user = re.split(',',self.options.targetdbuser)[0]
                              match_1  = re.search ('USE USER %s' % user, protocolline )
                           #table
                           if match_1 != None and match_2 == None:
                              if self.options.packagepart in ['DB'] :
                                 match_2  = re.search ('PORT DB'  ,protocolline)
                              if self.options.packagepart in ['USER'] :
                                 match_2  = re.search ('PORT USER',protocolline)
                              if self.options.packagepart in ['SCHEMA'] :
                                 match_2  = re.search ('PORT SCHEMA',protocolline)
                              if self.options.packagepart in ['TABLE'] :   
                                 match_2  = re.search ('PORT TABLE "%s"' % self.package.table, protocolline) 
                           if match_2 != None and match_3 == None:
                              match_3  = re.search ('Import    PACKAGE x', protocolline)
                           #package
                           if match_3 != None :
                              packageguid       = re.split ( '\'', protocolline)
                              packageguid       = string.replace( packageguid[1] , "\n" , '')
                              self.packageguid  = packageguid
                              self.packageload  = 'TRUE'
                              if self.transportdirection== 'EXPORT' or self.transportdirection== 'TRANSPORT':
                                 resultset = self.monitor.sql(PACKAGEMAXSTEP_SOURCE % self.packageguid )
                              if self.transportdirection== 'IMPORT':
                                 resultset = self.monitor.sql(PACKAGEMAXSTEP_TARGET % self.packageguid )
                              if resultset != None:
                                 row = resultset.first()
                                 if row != None:
                                    self.package.maxstepid = row[0] 
                              if self.options.mode == 'INTER':
                                 print "Load      Transformation Package x'%s'" % self.packageguid
                           protocolline = protocol.readline()
                     protocol.close()
             except sql.CommunicationError, err:
                 pass
             except sql.SQLError,  err :
                 pass
             except IndexError,err:
                 pass
           
      def cmd_monitor_package(self):
          if MONITOR == True:
             try:
                 if self.options.streamformat == 'PAGES' or  self.options.streamformat_target == 'PAGE':
                    pass
                 if self.options.streamformat != 'PAGES' and self.options.streamformat_target != 'PAGE' :
                    if self.options.packagepart in ['TABLE']:
                       self.table = self.package.table
                    if self.package.stepid == None:
                       if self.transportdirection== 'EXPORT' or self.transportdirection== 'TRANSPORT':
                          resultset  = self.monitor.sql(PACKAGEROWCOUNT_SOURCE  % self.packageguid )
                       if self.transportdirection== 'IMPORT':
                          resultset  = self.monitor.sql(PACKAGEROWCOUNT_TARGET  % self.packageguid )
                       if resultset != None:
                          row = resultset.first()
                          if row != None:
                             self.package.stepid = row[0] 
                             self.owner          = row[1] 
                             if self.options.packagepart != 'TABLE':
                                self.table       = row[2]
                    if self.package.stepid != None:
                       if self.transportdirection== 'EXPORT' or self.transportdirection== 'TRANSPORT':
                          resultset  = self.monitor.sql(STEPIDROWCOUNT_SOURCE  % (self.packageguid, self.package.stepid ) )
                       if self.transportdirection== 'IMPORT':
                          resultset  = self.monitor.sql(STEPIDROWCOUNT_TARGET  % (self.packageguid, self.package.stepid ) )
                       row = resultset.first()
                       if row != None:
                          if self.package.stepid == row[0]:
                             print '\rTable     %-10s : %s  ' %( self.table, row[3]),
                       if row == None and self.package.maxstepid != self.package.stepid:
                          if self.transportdirection== 'EXPORT' or self.transportdirection== 'TRANSPORT':
                             resultset  = self.monitor.sql(STEPIDSROWCOUNT_SOURCE  % (self.packageguid, self.package.stepid ) )
                          if self.transportdirection== 'IMPORT':
                             resultset  = self.monitor.sql(STEPIDSROWCOUNT_TARGET  % (self.packageguid, self.package.stepid ) )
                          if resultset != None:
                             for row in resultset:
                                 if row != None and self.package.stepid == row[0]:
                                    print '\rTable     %-10s : %s ' %( row[2], row[3])  
                                 if row != None and self.package.stepid != row[0]:
                                    print 'Table     %-10s : %s '   %( row[2], row[3])
                          if self.package.stepid    < self.package.maxstepid:
                             self.package.maxstepid = self.package.stepid
                             self.package.stepid    = None
             except sql.CommunicationError, err:
                 self.package.tranformation.release()
                 os._exit(1)
             except sql.SQLError,  err :
                 pass
             except :
                 pass
          if MONITOR == False:
             pass
     
      def cmd_export_package(self):
          TRANSFORMATIONSYNTAX = 'STRATIFIED' 
          
          if self.packageend > 0 and ( time.time() - self.packagestart > self.packageend ):
             print ' Processing time exceeded; stopping execution'
             self.package.tranformation.release()
             os._exit(1)
          if TRANSFORMATIONSYNTAX == 'UNIFIED' and self.options.transporttype == 'EXECUTE' :
             catalogstream_name = None
             datastream_name    = None
             packagestream_name = None
             catalogstream_lock = None
             datastream_lock    = None
             packagestream_lock = None
             if  'EXPORT' == self.transportdirection :
                 catalogstream_name    = string.replace(self.catalogstream_name,"'","")
                 datastream_name       = string.replace(self.datastream_name   ,"'","")
                 packagestream_name    = string.replace(self.packagestream_name,"'","")
             if  'IMPORT' == self.transportdirection :
                 catalogstream_name    = string.replace(self.catalogstream_name,"'","")
                 datastream_name       = string.replace(self.datastream_name   ,"'","")
                 packagestream_name    = string.replace(self.packagestream_name,"'","")
                 catalogstream_name    = string.replace(catalogstream_name, self.options.sourcedbname, self.options.targetdbname)
                 datastream_name       = string.replace(datastream_name   , self.options.sourcedbname, self.options.targetdbname)
                 packagestream_name    = string.replace(packagestream_name, self.options.sourcedbname, self.options.targetdbname)
             try:
                if self.options.streamformat == 'DDL' or self.options.streamformat == 'XML':
                   if os.path.isfile           (catalogstream_name) :
                      catalogstream = os.rename(catalogstream_name, catalogstream_name)
                      catalogstream_lock = 'release'
                      datastream_lock    = 'release' 
                      packagestream_lock = 'release'
                if(self.options.streamformat == 'COMPRESSED' or self.options.streamformat == 'FORMATTED' or
                   self.options.streamformat == 'RECORDS'    or self.options.streamformat == 'PAGES'    ):
                   if os.path.isfile           (catalogstream_name) :
                      catalogstream = os.rename(catalogstream_name, catalogstream_name) 
                      catalogstream_lock = 'release'
                   if os.path.isfile        (datastream_name) : 
                      datastream = os.rename(datastream_name, datastream_name)
                      catalogstream_lock = 'release' 
                      datastream_lock    = 'release' 
                   if os.path.isfile        ( string.replace(datastream_name,'.DATA','.DATA0000')) : 
                      datastream = os.rename( string.replace(datastream_name,'.DATA','.DATA0000') ,string.replace(datastream_name,'.DATA','.DATA0000'))
                      catalogstream_lock = 'release' 
                      datastream_lock    = 'release'
                   if os.path.isfile           (packagestream_name) :      
                      packagestream = os.rename(packagestream_name, packagestream_name)
                      catalogstream_lock = 'release' 
                      datastream_lock    = 'release' 
                      packagestream_lock = 'release'
                if( catalogstream_lock == 'release' and 
                    datastream_lock    == 'release' and 
                    packagestream_lock == 'release'    ):
                    self.packageunload  = 'TRUE'
                    if  DB_DEFAULTCODE_SOURCE == 'ASCII' and DB_DEFAULTCODE_TARGET == 'ASCII' and MONITOR == True:
                        try:
                           packagefilehandle = file (packagestream_name )
                           resultset   = csv.reader(packagefilehandle, delimiter  = '|')
                           if resultset != None:
                              print 'Transformation package rows   :'
                              for row in resultset:
                                  if MONITOR == True:
                                     if self.package.maxstepid < string.atoi( row[12], 10 ) :
                                        print 'Table     %-10s : %s ' % (row[1], row[33])
                                  if MONITOR == False:
                                     print    'Table     %-10s : %s ' % (row[1], row[33])
                        except:
                          pass
                    if not ( 'EXPORT'  == self.transportdirection and 'PIPE'  == self.streammedium ):
                       if  mutex.locked()  == True :
                           mutex.release()           #release
             except OSError, (errno, err ):
                catalogstream_lock = None
                datastream_lock    = None
                packagestream_lock = None
                pass
                
          if TRANSFORMATIONSYNTAX == 'STRATIFIED' and self.options.transporttype == 'EXECUTE':
             match_0      = None
             match_1      = None
             match_2      = None
             match_3      = None
             match_4      = None
             packageguid  = None
             user         = None
             try:
                 protocolfile = ''
                 if 'EXPORT' == self.transportdirection  or 'IMPORT' == self.transportdirection :
                     if sys.platform[:3] == 'win':
                        protocolfile = os.path.join(LOADERDATAPATH, 'sdb' ,'loader','log',self.protocol)
                     if sys.platform[:3] != 'win':
                        protocolfile = os.path.join(LOADERDATAPATH, '.sdb','loader','log',self.protocol)
                 if  os.path.isfile  ( protocolfile ) :
                     protocol  = file( protocolfile ,'r')
                     protocolline  = protocol.readline()
                     counter       = 0
                     while protocolline and self.packageunload == 'FALSE':
                           #timestamp
                           if match_0 == None:
                              if  'TRANSPORT' == self.transportdirection or 'EXPORT' == self.transportdirection:
                                  if TESTLOADER == 'TRUE' and self.options.packageref != self.options.packagepart:
                                     user = self.package.user
                                  else:
                                     user = re.split(',',self.options.sourcedbuser)[0]
                                  match_0  = re.search ("START     PACKAGE x'%s' WITH TIMESTAMP %s" % (self.package.packageguid, self.options.packageexecution ), protocolline )
                              if  'TRANSPORT' == self.transportdirection or 'IMPORT' == self.transportdirection :
                                  if TESTLOADER == 'TRUE' and self.options.packageref != self.options.packagepart:
                                     user = self.package.user
                                  else:
                                     user = re.split(',',self.options.targetdbuser)[0]
                                  match_0  = re.search ("START     PACKAGE x'%s' WITH TIMESTAMP %s" % (self.package.packageguid, self.options.packageexecution ), protocolline )
                           #user                                  
                           if match_0 != None and match_1 == None:
                              match_1  = re.search ('USE USER %s' % user, protocolline )
                           #part
                           if match_1 != None and match_2 == None :
                              match_2  = re.search ("Import    PACKAGE x'%s'" % self.packageguid , protocolline)
                           if match_2 != None and match_3 == None :
                              #monitor
                              if self.options.packagepart != 'TABLE' :
                                 match_3 = 'ok'
                              if self.options.packagepart in ['TABLE'] :
                                 if self.options.streamformat    == 'PAGES':
                                    match_3 = 'ok'
                                 if self.options.streamformat    == 'RECORDS' or self.options.streamformat == 'COMPRESSED' or self.options.streamformat == 'FORMATTED' :
                                    match_3  = re.search ("Number of R" , protocolline)
                                    if match_3 != None:
                                       packagerows        = re.split (":",protocolline)[1]
                                       packagerows        = re.split (",",packagerows )[0]
                                       if VERBOSE == True:
                                          print '\rTable     %-10s : %s  \n' %( self.package.table, packagerows),
                                 if self.options.streamformat    == 'DDL' or self.options.streamformat  == 'XML' or self.options.streamformat_target == 'DTL':
                                    match_3 = 'ok'
                           if match_3 != None and match_4 == None :
                              #package
                              if 'EXPORT'  == self.transportdirection and 'TAPE'  == self.streammedium :
                                  #format
                                  if self.streamformat    == 'DDL' :
                                     if protocolline[0:8] == '// M  NST':
                                          if counter == 0:
                                             ddlpipe_ebid = re.split  ('NST',protocolline)
                                             ddlpipe_ebid = string.replace(ddlpipe_ebid[1], "\n" , '')
                                             self.ddlpipe_ebid = "\'NST%s\'" % ddlpipe_ebid
                                  if self.streamformat    ==  'RECORDS' or self.streamformat ==  'PAGES':
                                     if protocolline[0:8] == '// M  NST':
                                        if counter == 0:
                                           ddlpipe_ebid = re.split  ('NST',protocolline)
                                           ddlpipe_ebid = string.replace(ddlpipe_ebid[1], "\n" , '')
                                           self.ddlpipe_ebid = "\'NST%s\'" % ddlpipe_ebid
                                        if counter == 1:
                                           datpipe_ebid = re.split  ('NST',protocolline)
                                           datpipe_ebid = string.replace(datpipe_ebid[1], "\n" , '')
                                           self.datpipe_ebid = "\'NST%s\'" % datpipe_ebid
                                        if counter == 2:
                                           mappipe_ebid = re.split  ('NST',protocolline)
                                           mappipe_ebid = string.replace(mappipe_ebid[1], "\n" , '')
                                           self.mappipe_ebid = "\'NST%s\'" % mappipe_ebid
                                        counter  = counter + 1
                              match_4  = re.search ("Export    PACKAGE x'%s'" % self.packageguid , protocolline)
                           #rows
                           if match_4 != None :
                              self.packageunload = 'TRUE'
                              #monitor
                              package = None
                              if self.options.transportdirection == 'EXPORT' or self.options.transportdirection == 'TRANSPORT':
                                 if self.options.packagepart in ['DB'] :
                                    package = os.path.join(self.options.sourcepackagepath,'%s.package' % self.options.sourcedbname)
                                 if self.options.packagepart in ['USER'] :
                                    package = os.path.join(self.options.sourcepackagepath,'%s.package' % re.split(',', self.options.sourcedbuser)[0])
                                 if self.options.packagepart in ['SCHEMA'] :
                                    package = os.path.join(self.options.sourcepackagepath,'%s.package' % self.options.sourcedbschema)
                              if self.options.transportdirection == 'IMPORT' or self.options.transportdirection == 'TRANSPORT':
                                 if self.options.packagepart in ['DB'] :
                                    package = os.path.join(self.options.targetpackagepath,'%s.package' % self.options.targetdbname)
                                 if self.options.packagepart in ['USER'] :
                                    package = os.path.join(self.options.targetpackagepath,'%s.package' % re.split(',', self.options.targetdbuser)[0])
                                 if self.options.packagepart in ['SCHEMA'] :
                                    package = os.path.join(self.options.targetpackagepath,'%s.package' % self.options.targetdbschema)
                              if((self.options.packagepart in ['DB']    or  self.options.packagepart in ['USER'] or  self.options.packagepart in ['SCHEMA']) and self.options.streamformat != 'PAGES' and
                                  DB_DEFAULTCODE_SOURCE == 'ASCII' and DB_DEFAULTCODE_TARGET == 'ASCII' and MONITOR == True ) :
                                 if os.path.isfile(package) :
                                    packagefilehandle = file (package )
                                    resultset   = csv.reader(packagefilehandle, delimiter  = '|')
                                    if resultset != None:
                                       print 'Transformation package rows   :'
                                       for row in resultset:
                                           if MONITOR == True:
                                              if self.package.maxstepid < string.atoi( row[12], 10 ) :
                                                 print 'Table     %-10s : %s ' % (row[1], row[33])
                                           if VERBOSE == True:
                                              print    'Table     %-10s : %s ' % (row[1], row[33])
                              #mutex
                              if not ( 'EXPORT'  == self.transportdirection and 'PIPE'  == self.streammedium ):
                                 if  mutex.locked()  == True :
                                     mutex.release()           #release
          
                           protocolline = protocol.readline()
                     protocol.close()
             except IOError,    (errno, err ):
                pass
                
             TRANSFORMATIONSYNTAX = 'UNIFIED'

      def cmd_restart_package(self):
          restartRecord = []
          if self.initposition == 'RESTART' :
             statement     =  " SELECT DISTINCT PACKAGEGUID, OWNER, TABLENAME, \n DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, \n DATASTREAMDATABLOCK, DATASTREAMDATAPOS, ROWCOUNT \n"
             statement     =  statement + "FROM SYSDBA.TRANSFORMATIONMODEL "
             statement     =  statement + "WHERE RESTART = TRUE "

             result = self.monitor.sql (statement )
             row    = result.next ()
             if row :
                packageguid = ('%02X' * 24) % tuple (map (ord, row[0] ))
                restartRecord.append (packageguid)
                restartRecord.append (row[1])
                restartRecord.append (row[2])
                restartRecord.append (row[3])
                restartRecord.append (row[4])
                restartRecord.append (row[5])
                restartRecord.append (row[6])
                restartRecord.append (row[7])

          return restartRecord

      def cmd_restart_sourcedb_package(self, targetRecord ):
          #Medium Format Start
          if  self.initposition == 'RESTART' and self.streammedium == 'PIPE' and self.streamformat == 'RECORDS' and targetRecord <> [] :
              sourceRecord  = self.cmd_restart_package()
              statement     = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET \n"
              statement     = statement + " DATASTREAMHEADERBLOCK = %s, DATASTREAMHEADERPOS=%s, \n DATASTREAMDATABLOCK=%s,DATASTREAMDATAPOS=%s,\n ROWCOUNT=%s " % ( targetRecord[3], targetRecord[4], targetRecord[5], targetRecord[6], targetRecord[7] )
              statement     = statement + " WHERE PACKAGEGUID= x'%s' \n AND OWNER = '%s' AND TABLENAME = '%s' AND RESTART = TRUE \n"                            % ( sourceRecord[0], targetRecord[1], targetRecord[2])
              print  statement
              self.monitor.sql (statement )
          #Medium Format Start
          if  self.initposition == 'RESTART' and self.streammedium == 'PIPE' and self.streamformat == 'PAGES'   and targetRecord <> [] :
              pass
          #Medium Format Start
          if  self.initposition == 'RESTART' and self.streammedium == 'FILE' and self.streamformat == 'RECORDS' and targetRecord <> [] :
              pass
          #Medium Format Start
          if  self.initposition == 'RESTART' and self.streammedium == 'FILE' and self.streamformat == 'PAGES'   and targetRecord <> [] :
              pass

      def cmd_restart_targetdb_package(self):
          #Medium Format Start
          if  self.initposition == 'RESTART' and self.streammedium == 'PIPE' and self.streamformat == 'RECORDS':
              restartRecord = self.cmd_restart_package()
          #Medium Format Start
          if  self.initposition == 'RESTART' and self.streammedium == 'PIPE' and self.streamformat == 'PAGES'  :
              restartRecord = self.cmd_restart_package()

              statement     = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET \n"
              statement     = statement + " DATASTREAMHEADERBLOCK = 0, DATASTREAMHEADERPOS=0, \n DATASTREAMDATABLOCK=0,DATASTREAMDATAPOS=0,\n ROWCOUNT=0, RESTART = TRUE \n"
              statement     = statement + " WHERE PACKAGEGUID= x'%s' \n AND OWNER = '%s' AND TABLENAME = '%s' \n"  % (restartRecord[0], restartRecord[1], restartRecord[2])
              print statement
              result        = self.monitor.sql (statement)
              statement     = " DELETE %s.%s \n"  % (restartRecord[1], restartRecord[2] )
              print statement
              result        = self.monitor.sql (statement)
          #Medium Format Start
          if  self.initposition == 'RESTART' and self.streammedium == 'FILE' and self.streamformat == 'RECORDS':
              restartRecord = self.cmd_restart_package()
          #Medium Format Start
          if  self.initposition == 'RESTART' and self.streammedium == 'FILE' and self.streamformat == 'PAGES'  :
              restartRecord = self.cmd_restart_package()

          return restartRecord
          
#class Transformator 

class StreamConnector :
      def __init__(self, source_loader = None, target_loader = None ) :
          self.exporter   = source_loader
          self.importer   = target_loader
          if self.exporter != None:
             self.user           = self.exporter.package.user
             self.table          = self.exporter.package.table
          if self.importer != None:
             self.user           = self.importer.package.user
             self.table          = self.importer.package.table
          self.longcolumn     = ''
          self.sourceddltime  = None
          self.sourcedattime  = None
          self.ddlpipe_exist  = 1
          self.datpipe_exist  = 1

      def cmd_datpipe2datfile(self):
          datfile = os.path.join (self.exporter.sapdbroot, '%s_%s.dat'         % ( self.exporter.package.user, self.exporter.package.table))
          datpipe2file    = "pipe2file -d pipe2file -p %s           -f %s"         % ( self.exporter.datpipe, datfile )
          try:
             print 'Start     StreamConnector to Connect \n FILE %s to\n PIPE %s ' % ( self.exporter.datpipe, datfile )
             os.system( datpipe2file )
          except OSError, err:
             print 'command failed:', err

      def cmd_longpipe2longfiles(self):
          for longcolumn in self.exporter.package.column_long:
              self.longcolumn = longcolumn
              thread.start_new_thread( self.cmd_longpipe2longfile,())
              time.sleep(1)

      def cmd_longpipe2longfile(self):
          longpipe  = '\\\\.\\PIPE\\%s_%s_%s_%s.long'                                                                                            % (self.exporter.package.sapdb, self.user ,self.table, self.longcolumn )
          longfile  = os.path.join (self.exporter.sapdbroot, '%s_%s_%s.long' % (self.user, self.table, self.longcolumn ))
          longpipe2file = "pipe2file -d pipe2file -p %s -f %s" % ( longpipe , longfile )
          try:
              print 'Start     StreamConnector to Connect \n FILE %s to\n PIPE %s ' % ( longfile, longpipe )
              os.system( longpipe2file )
          except OSError, err:
              print 'command failed:', err

      def cmd_datfile2datpipe(self):
          print '\nStart     StreamConnector to Connect\n FILE %s to\n PIPE %s '    % ( self.exporter.datfile, self.exporter.datpipe )
          datfile2pipe    = "pipe2file -d file2pipe -p %s           -f %s"          % ( self.exporter.datpipe, self.exporter.datfile )
          try:
             os.system( datfile2pipe )
          except OSError, err:
             print 'command failed:', err

      def cmd_pipe2tape(self):
          counter = 0
          protocolfile = os.path.join(self.exporter.sapdbroot, 'wrk',self.exporter.protocol)
          if  os.path.isfile     ( protocolfile ) :
              protocol     = file( protocolfile ,'r')

              if self.exporter.streamformat <> 'DDL' :
                 protocolline = protocol.readline()
                 while protocolline :
                     if ( protocolline[0:8] == '// M    NST'):
                          if counter == 0:
                             ddlpipe_ebid = re.split  ('NST',protocolline)
                             ddlpipe_ebid = string.replace(ddlpipe_ebid[1] , "\n" , '')
                             self.importer.ddlpipe_ebid = "\'NST%s\'" % ddlpipe_ebid
                          if counter == 1:
                             datpipe_ebid = re.split  ('NST',protocolline)
                             datpipe_ebid = string.replace(datpipe_ebid[1] , "\n" , '')
                             self.importer.datpipe_ebid = "\'NST%s\'" % datpipe_ebid
                          counter  = counter + 1
                     protocolline = protocol.readline()
                 if counter == 2:
                    if mutex.locked() == True:
                       mutex.release() #release

              if self.exporter.streamformat == 'DDL' :
                 protocolline = protocol.readline()
                 while protocolline :
                     if ( protocolline[0:8] == '// M    NST'):
                          if counter == 0:
                             ddlpipe_ebid = re.split  ('NST',protocolline)
                             ddlpipe_ebid = string.replace(ddlpipe_ebid[1] , "\n" , '')
                             self.importer.ddlpipe_ebid = "\'NST%s\'" % ddlpipe_ebid
                          counter  = counter + 1
                     protocolline = protocol.readline()
                 if counter == 1:
                    if mutex.locked() == True:
                       mutex.release() #release

      def cmd_ddlpipe2ddltape(self):
          print '\nStart     StreamConnector to Connect \n TAPE to\n PIPE %s ' % ( self.exporter.ddlpipe_source )
          ddltapesave  = self.exporter.sapdbroot + "\\wrk\\" + self.exporter.sapdb + ".ddltape_save"
          ddlpipe2tape = "\nsave    -s %s -N %s -l full -LL %s 2> %s"          % ( self.exporter.sapdbnode, self.exporter.sapdb,
                                                                                   self.exporter.ddlpipe_source, ddltapesave )
          try:
             os.system( ddlpipe2tape )
          except OSError, err:
             print 'command failed:', err
          if  os.path.isfile  ( ddltapesave ) :
              tapefile  = file( ddltapesave ,'r')
              tapeline  = tapefile.readline()
              while tapeline  :
                  if ( tapeline[0:19] == 'completed savetime='):
                       savetime = re.split ('=',tapeline)
                       savetime = string.replace( savetime[1] , "\n" , '')
                       self.sourceddltime = savetime
                  tapeline = tapefile.readline()
              tapefile.close()

      def cmd_ddltape2ddlpipe(self):
          print '\nStart     StreamConnector to Connect \n TAPE %s to\n PIPE %s' % ( self.sourceddltime, self.exporter.ddlpipe_source )
          ddltape2pipe = "\nrecover -q -s %s -a -t %s -c %s -iY %s "             % ( self.exporter.sapdbnode, self.sourceddltime,
                                                                                     self.exporter.sapdbnode, self.exporter.ddlpipe_source )
          try:
              os.system( ddltape2pipe )
          except OSError, err:
              print 'command failed:', err

      def cmd_datpipe2dattape(self):
          print 'Start     StreamConnector to Connect \n TAPE to\n PIPE %s ' %  self.exporter.datpipe_source
          dattapesave  = self.exporter.sapdbroot + "\\wrk\\" + self.exporter.sapdb + ".dattape_save"
          datpipe2tape = "save    -s %s -N %s -l full -LL %s           2> %s"% ( self.exporter.sapdbnode, self.exporter.sapdb,
                                                                                 self.exporter.datpipe_source , dattapesave  )
          try:
             os.system( datpipe2tape )
          except OSError, err:
             print 'command failed:', err
          if  os.path.isfile  ( dattapesave ) :
              tapefile  = file( dattapesave ,'r')
              tapeline  = tapefile.readline()
              while tapeline :
                  if ( tapeline[0:19] == 'completed savetime='):
                       savetime = re.split ('=',tapeline)
                       savetime = string.replace( savetime[1] , "\n" , '')
                       self.sourcedattime = savetime
                  tapeline = tapefile.readline()
              tapefile.close()

      def cmd_dattape2datpipe(self):
          print '\nStart     StreamConnector to Connect\n TAPE %s to\n PIPE %s ' % ( self.sourcedattime, self.exporter.datpipe_source )
          dattape2pipe = "\nrecover -q -s %s -a -t %s -c %s -iY %s       "       % ( self.exporter.sapdbnode, self.sourcedattime,
                                                                                     self.exporter.sapdbnode, self.exporter.datpipe_source )
          try:
              os.system( dattape2pipe )
          except OSError, err:
              print 'command failed:', err

class StreamFilter :
      def __init__(self, options = None, transformation = None ) :
          self.options        = options
          self.transformation = transformation
          self.ddlerror       = 1
          self.dtlerror       = 1
          self.daterror       = 1

      def cmd_dtlsource2dtltarget(self):
          dtlsource    = None
          if self.options.streammedium == 'FILE':
             dtlsource = os.path.join(self.options.sourcepackagepath,  '%s_%s_%s.dtl'  % (self.options.sourcedbname,re.split(',',self.options.sourcedbuser)[0],self.transformation.package.table ))
          if self.options.streammedium == 'PIPE':
             dtlsource = '\\\\.\\PIPE\\%s_%s_%s.dtl'  % (self.exporter.sapdb,self.exporter.user,self.exporter.package.table )
          try :
             self.dtlerror   = 0
             step            = ''
             dtlsourcestream = file(dtlsource, 'r' )
             sourcedtl       = dtlsourcestream.readline()
             self.importer.package.transformation_step = []
             while sourcedtl :
                   targetdtl = string.strip(sourcedtl)
                   if ( targetdtl[0:2] <> "//" ) and targetdtl <> '' :
                        string.replace(targetdtl, 'ASCII', self.options.streamcharencoding)
                        step = step + targetdtl + "\n"
                   if ( targetdtl[0:2] == "//" or targetdtl == '' )  and step <> '' and step <> ' ':
                        self.importer.package.transformation_step.append(step)
                        step= ''
                   sourcedtl = dtlsourcestream.readline()
             dtlsourcestream.close()
          except IOError, (errno, err ):
             self.dtlerror  = 1

      def cmd_ddlsource2ddltarget(self):
          try :
              self.ddlerror   = 0
              ddlpipesource   = file(self.transformation.exporter.ddlpipe_source, 'rt' )
              ddlpipetarget   = file(self.transformation.importer.ddlpipe_target, 'wt' )
              sourceddl       = ddlpipesource.readline()
              while sourceddl :
                    targetddl = string.replace(sourceddl, '%s' % string.upper(self.transformation.exporter.catalog),
                                                          '%s' % string.upper(self.transformation.importer.catalog) )
                    ddlpipetarget.write(targetddl)
                    sourceddl = ddlpipesource.readline()
              ddlpipesource.close()
              ddlpipetarget.close()
          except IOError, (errno, err ):
              self.ddlerror  = 1
              print 'command failed:', errno, err

      def cmd_datsource2dattarget_bin(self):
          try :
             self.daterror   = 0
             datpipesource   = file(self.exporter.datpipe_source, 'rb')
             datpipetarget   = file(self.exporter.datpipe_target, 'wb')
             sourcedat       = datpipesource.read(400)
             while sourcedat :
                   targetdat = sourcedat
                   datpipetarget.write(targetdat)
             sourcedat = datpipesource.read(400)
             datpipesource.close()
             datpipetarget.close()
          except IOError, (errno, err ):
             print 'command failed:', errno, err
             self.daterror  = 1

      def cmd_datsource2dattarget_txt(self):
          try :
             self.daterror   = 0
             datpipesource   = file(self.exporter.datpipe_source, 'rt')
             datpipetarget   = file(self.exporter.datpipe_target, 'wt')
             sourcedat       = datpipesource.readline()
             while sourcedat :
                   targetdat = sourcedat
                   datpipetarget.write(targetdat)
             sourcedat = datpipesource.read()
             datpipesource.close()
             datpipetarget.close()
          except IOError, (errno, err ):
             print 'command failed: %s, %s \n' %( errno, err )
             self.daterror  = 1
             
def  exist_packageloader (options): 
    bVersion = True
    try:
       session       = loader.Loader()
       versionstring = session.cmd('VERSION')
       match         = re.search ('"(\d+)[.](\d+)[.](\d+)[.](\d+)"', versionstring)
       version       = tuple (map (int, match.group (1, 2, 3, 4)))
       if version < (7,5,0,16):
          print versionstring
          print 'Loader Version 7.5.00.16 or higher required'
          bVersion =  False
          if sys.platform[:3] == 'win':
             print 'Try to start Loader from %s ' % INDEPPROGPATH
       return bVersion
    except loader.LoaderError, err:
           raise loader.LoaderError, err, sys.exc_traceback
    except loader.CommunicationError, err :
           raise loader.CommunicationError, err, sys.exc_traceback
         
def  exist_db (options, exportmonitor, importmonitor):
    rc = True
    maxusertasks  = 0
    usedusertasks = 0
    global DB_INSTROOT_SOURCE
    global DB_INSTROOT_TARGET
    global DB_VERSION_SOURCE
    global DB_VERSION_TARGET
    global DB_INDEPDATAPATH_SOURCE
    global DB_INDEPDATAPATH_TARGET
    global DB_CODE_SOURCE
    global DB_CODE_TARGET
    global DB_DEFAULTCODE_SOURCE
    global DB_DEFAULTCODE_TARGET
    global DB_SCHEMAS_SOURCE
    global DB_SCHEMAS_TARGET
    global PACKAGEIMPORT
    global PACKAGEMAXSTEP_SOURCE
    global PACKAGEMAXSTEP_TARGET  
    global PACKAGEROWCOUNT_SOURCE
    global PACKAGEROWCOUNT_TARGET 
    global STEPIDROWCOUNT_SOURCE
    global STEPIDROWCOUNT_TARGET  
    global STEPIDSROWCOUNT_SOURCE
    global STEPIDSROWCOUNT_TARGET 
    global PACKAGEVALIDATE
    global PACKAGETABLES
    #Source DBM
    try:
        if exportmonitor != None: 
           dbmsource     = dbm.DBM (options.sourcedbnode, options.sourcedbname, '','')
           indepdatapath = dbmsource.cmd('dbm_getpath     INDEPDATAPATH')
           if DB_INDEPDATAPATH_SOURCE == '':
              DB_INDEPDATAPATH_SOURCE = re.split('\n',indepdatapath)[0]
           instroot = dbmsource.cmd('dbm_version  INSTROOT')
           DB_INSTROOT_SOURCE = re.split('\n',instroot)[0]
           version  = dbmsource.cmd('dbm_version BUILD')
           version  = string.replace(version,'DBMServer ','')
           DB_VERSION_SOURCE = '%s.%s' %( re.split('Build',version)[0][0:6],re.split('Build',version)[1][2:4])
           match = re.search ('(\d+)[.](\d+)[.](\d+)', DB_VERSION_SOURCE)
           version = tuple (map (int, match.group (1, 2, 3)))
           if version      >=  (7, 6, 0 ) :
              MAXUSERTASKS    = "SELECT VALUE    FROM ACTIVECONFIGURATION  WHERE PARAMETERNAME LIKE 'MAXUSERTASKS'"
              CODE            = "SELECT VALUE    FROM ACTIVECONFIGURATION  WHERE PARAMETERNAME LIKE '_UNICODE'"
              DEFAULTCODE     = "SELECT VALUE    FROM ACTIVECONFIGURATION  WHERE PARAMETERNAME LIKE 'DEFAULT_CODE'"
              USEDUSERTASKS   = "SELECT COUNT(*) FROM SESSIONS"
           if version      <   (7, 6, 0 ):
              MAXUSERTASKS    = "SELECT VALUE    FROM DBPARAMETERS         WHERE DESCRIPTION   LIKE 'MAXUSERTASKS'"
              CODE            = "SELECT VALUE    FROM DBPARAMETERS         WHERE DESCRIPTION   LIKE '_UNICODE'"
              DEFAULTCODE     = "SELECT VALUE    FROM DBPARAMETERS         WHERE DESCRIPTION   LIKE 'DEFAULT_CODE'"
              USEDUSERTASKS   = "SELECT COUNT(*) FROM CONNECTEDUSERS"
           if version      >= ( 7, 6, 0 ) :
              PACKAGEMAXSTEP_SOURCE  = "SELECT MAX(STEPID)                              FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE "
              PACKAGEROWCOUNT_SOURCE = "SELECT STEPID, SCHEMANAME, TABLENAME, ROWCOUNT  FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = TRUE "
              STEPIDROWCOUNT_SOURCE  = "SELECT STEPID, SCHEMANAME, TABLENAME, ROWCOUNT  FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = TRUE  AND STEPID  = %s "
              STEPIDSROWCOUNT_SOURCE = "SELECT STEPID, SCHEMANAME, TABLENAME, ROWCOUNT  FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = FALSE AND STEPID >= %s "
              DB_SCHEMAS_SOURCE    =("""SELECT  SCHEMANAME                              FROM DOMAIN.SCHEMAS WHERE OWNER LIKE '%s' """
                                     """AND SCHEMANAME NOT LIKE 'OMS' """
                                     """AND SCHEMANAME NOT LIKE 'DOMAIN' """
                                     """AND SCHEMANAME NOT LIKE 'SYSINFO'""")
              PACKAGETABLES          = "SELECT         SCHEMANAME, TABLENAME            FROM DOMAIN.TABLES         WHERE SCHEMANAME LIKE '%s' AND TABLENAME LIKE '%s' AND TABLETYPE = 'TABLE'"                       
           if version      <  (7, 6, 0 ):
              PACKAGEMAXSTEP_SOURCE  = "SELECT MAX(STEPID)                         FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE "
              PACKAGEROWCOUNT_SOURCE = "SELECT STEPID, OWNER, TABLENAME, ROWCOUNT  FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = TRUE "
              STEPIDROWCOUNT_SOURCE  = "SELECT STEPID, OWNER, TABLENAME, ROWCOUNT  FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = TRUE  AND STEPID  = %s "
              STEPIDSROWCOUNT_SOURCE = "SELECT STEPID, OWNER, TABLENAME, ROWCOUNT  FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = FALSE AND STEPID >= %s "
              PACKAGETABLES          = "SELECT         OWNER, TABLENAME            FROM DOMAIN.TABLES              WHERE OWNER      LIKE '%s' AND TABLENAME LIKE '%s' AND TABLETYPE = 'TABLE'"
        if exportmonitor != None: 
           resultset    = exportmonitor.sql(MAXUSERTASKS)
           for row in resultset:
               maxusertasks = row[0]
           resultset    = exportmonitor.sql(USEDUSERTASKS)
           for row in resultset:
               sessions = row[0]    
           if int(maxusertasks) - int(usedusertasks) + 1  < 2:
              print 'At least 2 free USERTASKS for Data Source Needed'
              rc = False
           resultset    = exportmonitor.sql(CODE)
           for row in resultset:
               code = row[0]
               if code[0:2]            == 'NO':
                  DB_CODE_SOURCE        = 'ASCII'
               else:
                  DB_CODE_SOURCE        = 'UNICODE'
           resultset    = exportmonitor.sql(DEFAULTCODE)
           for row in resultset:
               defaultcode = row[0]
               if defaultcode[0:5]     == 'ASCII':
                  DB_DEFAULTCODE_SOURCE = 'ASCII'
               if defaultcode[0:7]     == 'UNICODE':
                  DB_DEFAULTCODE_SOURCE = 'UNICODE'
    except dbm.CommunicationError, err :
           pass
    except dbm.DBMServError, err:
           pass
    except sql.CommunicationError , err:   
           print 'Connect failed, try again:' , err
    except sql.SQLError , err:
           raise sql.SQLError, err, sys.exc_traceback
           print err
        
    #Target DBM
    try:
        if importmonitor != None: 
           dbmtarget     = dbm.DBM (options.targetdbnode, options.targetdbname, '','')
           indepdatapath = dbmtarget.cmd('dbm_getpath     INDEPDATAPATH')
           if DB_INDEPDATAPATH_TARGET == '':
              DB_INDEPDATAPATH_TARGET = re.split('\n',indepdatapath)[0]
           instroot = dbmtarget.cmd('dbm_version  INSTROOT')
           DB_INSTROOT_TARGET = re.split('\n',instroot)[0]
           version = dbmtarget.cmd('dbm_version BUILD')
           version = string.replace(version,'DBMServer ','')
           DB_VERSION_TARGET = '%s.%s' %( re.split('Build',version)[0][0:6],re.split('Build',version)[1][2:4])
           match   = re.search ('(\d+)[.](\d+)[.](\d+)', DB_VERSION_TARGET)
           version = tuple (map (int, match.group (1, 2, 3)))
           if version      >=  (7, 5, 0) :
              MAXUSERTASKS    = "SELECT VALUE    FROM ACTIVECONFIGURATION  WHERE PARAMETERNAME LIKE 'MAXUSERTASKS'"
              CODE            = "SELECT VALUE    FROM ACTIVECONFIGURATION  WHERE PARAMETERNAME LIKE '_UNICODE'"
              DEFAULTCODE     = "SELECT VALUE    FROM ACTIVECONFIGURATION  WHERE PARAMETERNAME LIKE 'DEFAULT_CODE'"
              USEDUSERTASKS   = "SELECT COUNT(*) FROM SESSIONS"
           if version      <   (7, 5, 0):
              MAXUSERTASKS    = "SELECT VALUE    FROM DBPARAMETERS         WHERE DESCRIPTION   LIKE 'MAXUSERTASKS'"
              CODE            = "SELECT VALUE    FROM DBPARAMETERS         WHERE PARAMETERNAME LIKE '_UNICODE'"
              DEFAULTCODE     = "SELECT VALUE    FROM DBPARAMETERS         WHERE DESCRIPTION   LIKE 'DEFAULT_CODE'"
              USEDUSERTASKS   = "SELECT COUNT(*) FROM CONNECTEDUSERS"
           if version      >= (7,6,0) :
              PACKAGEMAXSTEP_TARGET  = "SELECT MAX(STEPID)                              FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE "
              PACKAGEROWCOUNT_TARGET = "SELECT STEPID, SCHEMANAME, TABLENAME, ROWCOUNT  FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = TRUE "
              STEPIDROWCOUNT_TARGET  = "SELECT STEPID, SCHEMANAME, TABLENAME, ROWCOUNT  FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = TRUE  AND STEPID  = %s "
              STEPIDSROWCOUNT_TARGET = "SELECT STEPID, SCHEMANAME, TABLENAME, ROWCOUNT  FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = FALSE AND STEPID >= %s "
              packageimport          = "IMPORT COLUMNS TABLE SYSLOADER.TRANSFORMATIONMODEL \n"
              packageimport          =  packageimport + 'OWNER                 1 DEFAULT      NULL SCHEMANAME           2 DEFAULT      NULL TABLENAME                 3 DEFAULT      NULL \nDATASTREAMNAME      4 DEFAULT      NULL \n'
              PACKAGEVALIDATE        = "SELECT   T1.PACKAGEGUID , T1.TRANSFORMATION, T1.OWNER, T1.TABLENAME , T1.DATASTREAMHEADERBLOCK, T1.DATASTREAMHEADERPOS , T1.DATASTREAMDATABLOCK, T1.DATASTREAMDATAPOS , T1.START  FROM   SYSLOADER.TRANSFORMATIONSTEPS T1, SYSLOADER.TRANSFORMATIONSTEPS T2 "
              DB_SCHEMAS_TARGET    =("""SELECT  SCHEMANAME                              FROM DOMAIN.SCHEMAS WHERE OWNER LIKE '%s' """
                                     """AND SCHEMANAME NOT LIKE 'OMS' """
                                     """AND SCHEMANAME NOT LIKE 'DOMAIN' """
                                     """AND SCHEMANAME NOT LIKE 'SYSINFO'""")
           if version      <  (7, 6, 0):
              PACKAGEMAXSTEP_TARGET  = "SELECT MAX(STEPID)                         FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE "
              PACKAGEROWCOUNT_TARGET = "SELECT STEPID, OWNER, TABLENAME, ROWCOUNT  FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = TRUE "
              STEPIDROWCOUNT_TARGET  = "SELECT STEPID, OWNER, TABLENAME, ROWCOUNT  FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = TRUE  AND STEPID  = %s "
              STEPIDSROWCOUNT_TARGET = "SELECT STEPID, OWNER, TABLENAME, ROWCOUNT  FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID  = x'%s' AND EXCLUDE = FALSE AND RESTART = FALSE AND STEPID >= %s "
              packageimport          = "IMPORT COLUMNS TABLE  SYSDBA.TRANSFORMATIONMODEL \n"
              packageimport          =  packageimport + 'OWNER                 1 DEFAULT      NULL                                          TABLENAME                 3 DEFAULT      NULL \nDATASTREAMNAME      4 DEFAULT      NULL \n'
              PACKAGEVALIDATE        = "SELECT   T1.PACKAGEGUID , T1.TRANSFORMATION, T1.OWNER, T1.TABLENAME , T1.DATASTREAMHEADERBLOCK, T1.DATASTREAMHEADERPOS , T1.DATASTREAMDATABLOCK, T1.DATASTREAMDATAPOS , T1.START  FROM   SYSDBA.TRANSFORMATIONSTEPS T1, SYSDBA.TRANSFORMATIONSTEPS T2 "
           '''
           packageimport             =  packageimport + "SEPARATOR  '%s'  \n" % re.split('/',FIELDCOMPRESSED)[1]
           packageimport             =  packageimport + "DELIMITER  '%s'  \n" % re.split('/',FIELDCOMPRESSED)[2]
           packageimport             =  packageimport + "NULL       '%s'  \n" % FIELDNULLVALUE
           packageimport             =  packageimport + "DECIMAL    '%s'  \n" % FIELDDECIMALVALUE
           packageimport             =  packageimport + "BOOLEAN    '%s'  \n" % FIELDBOOLVALUE
           packageimport             =  packageimport + "TIMESTAMP   %s   \n" % FIELDTIMESTAMPVALUE
           packageimport             =  packageimport + "DATE        %s   \n" % FIELDATEVALUE
           packageimport             =  packageimport + "TIME        %s   \n" % FIELDTIMEVALUE
           '''           
           packageimport             =  packageimport + 'DATASTREAMHEADERBLOCK 5 DEFAULT      NULL DATASTREAMHEADERPOS  6 DEFAULT      NULL \nDATASTREAMTRAILERBLOCK  7 DEFAULT      NULL DATASTREAMTRAILERPOS  8 DEFAULT      NULL  DATASTREAMDATABLOCK  9 DEFAULT      NULL DATASTREAMDATAPOS   10  DEFAULT    NULL \n'
           packageimport             =  packageimport + '\"EXCLUDE\"          11 DEFAULT      NULL \n'
           packageimport             =  packageimport + 'PACKAGEGUID          12 DEFAULT      NULL TASKID              13 DEFAULT      NULL \nSTEPID                 14 DEFAULT      NULL SERVERNODE           15 DEFAULT      NULL \n'
           packageimport             =  packageimport + '\"SERVERDB\"         16 DEFAULT      NULL USERNAME            17 DEFAULT      NULL \n\"CODETYPE\"           18 DEFAULT      NULL CODEPAGEID           19 DEFAULT      NULL \n'
           packageimport             =  packageimport + '\"STORAGEDEVICE\"    20 DEFAULT      NULL TRANSACTIONSIZE     21 DEFAULT      NULL TRANSFORMATION           22 DEFAULT      NULL \nPART               23 DEFAULT      NULL TABLETYPE            24 DEFAULT      NULL \n'
           packageimport             =  packageimport + 'CATALOGSTREAMNAME    25 DEFAULT      NULL CATALOGSTREAMTYPE   26 DEFAULT      NULL \nCATALOGSTREAMEBID      27 DEFAULT      NULL CATALOGSTREAMFORMAT  28 DEFAULT      NULL DATASTREAMTYPE       29 DEFAULT      NULL  DATASTREAMEBID     30  DEFAULT     NULL \n'
           packageimport             =  packageimport + 'DATASTREAMFORMAT     31 DEFAULT      NULL \"RESTART\"         32 DEFAULT      NULL \nRESTORE                33 DEFAULT      NULL PAGECOUNT            34 DEFAULT      NULL \n'
           packageimport             =  packageimport + 'ROWCOUNT             35 DEFAULT      NULL STARTTIME           36 DEFAULT      NULL \nENDTIME                37 DEFAULT      NULL REQUESTTIME          38 DEFAULT      NULL \n'
           packageimport             =  packageimport + 'RECEIVETIME          39 DEFAULT      NULL IOTIME              40 DEFAULT      NULL \nLOGTIME                41 DEFAULT      NULL '
           packageimport             =  packageimport + " INSTREAM FILE '%s'  CSV %s \n"
           PACKAGEIMPORT             =  packageimport
        if importmonitor != None: 
           resultset      = importmonitor.sql(MAXUSERTASKS)
           for row in resultset:
               maxusertasks = row[0]
           resultset    = importmonitor.sql(USEDUSERTASKS)
           for row in resultset:
               sessions = row[0]    
           if int(maxusertasks) - int(usedusertasks) + 1  < 2:
              print 'At least 2 free USERTASKS for Data Source Needed'
              rc = False
           resultset    = importmonitor.sql(CODE)
           for row in resultset:
               code = row[0]
               if code[0:2]            == 'NO':
                  DB_CODE_TARGET        = 'ASCII'
               else:
                  DB_CODE_TARGET        = 'UNICODE'
           resultset    = importmonitor.sql(DEFAULTCODE)
           for row in resultset:
               defaultcode = row[0]
               if defaultcode[0:5]     == 'ASCII':
                  DB_DEFAULTCODE_SOURCE = 'ASCII'
               if defaultcode[0:7]     == 'UNICODE':
                  DB_DEFAULTCODE_SOURCE = 'UNICODE'
    except dbm.CommunicationError, err :
           pass
    except dbm.DBMServError, err:
           pass
    except sql.CommunicationError , err:   
           print 'Connect failed, try again:' , err
    except sql.SQLError , err:
           print err
           raise sql.SQLError, err, sys.exc_traceback
    return rc

def  exist_dba(options, exportmonitor, importmonitor ):
     global DBA_SOURCE
     global DBA_TARGET
     exist_sourcedba = False
     exist_targetdba = False
     if exportmonitor == None:  
        exist_sourcedba = True
     if exportmonitor != None:  
        try:   
           resultset = exportmonitor.sql(EXISTSDBA)
           for row in resultset:
               if re.split(',',options.sourcedbuser)[0] == row[0] :
                  exist_sourcedba = True
        except sql.CommunicationError , err:   
            print 'Connect failed, try again:' , err
        except sql.SQLError , err:
            exist_sourcedba = False
     if importmonitor == None:
        exist_targetdba = True
     if importmonitor != None:         
        try:   
            resultset = importmonitor.sql(EXISTSDBA)
            for row in resultset:
                if re.split(',',options.targetdbuser)[0] == row[0] :
                   exist_targetdba = True
        except sql.CommunicationError , err:   
            print 'Connect failed, try again:' , err
        except sql.SQLError , err:
            exist_targetdba = False
     if exist_sourcedba == True and exist_targetdba == True:
        rc = True
        if options.packageref in ['DB']:
           DBA_SOURCE = options.sourcedbuser
           DBA_TARGET = options.targetdbuser
     else:
        rc = False      
     return rc    

def  exist_schema(options, exportmonitor, importmonitor, schemaname ):
     rc = True
     exist_sourceschema = True
     exist_targetschema = True
     if CASESENSITIVE == True or   (schemaname [0] == '"' and schemaname[len(schemaname)-1] == '"'):
        schemaname = string.replace(schemaname,'"','')
     else:
        schemaname = string.upper  (schemaname)
     if exportmonitor == None:
        exist_sourceschema = True  
     if options.sourcedbschema == '' and exportmonitor != None and DB_SCHEMAS_SOURCE == ''  :
        exist_sourceschema = True 
     if options.sourcedbschema == '' and exportmonitor != None and DB_SCHEMAS_SOURCE != '' and TESTLOADER == 'FALSE':  
        try:   
           resultset = exportmonitor.sql(EXISTSSCHEMA %schemaname )
           row       = resultset.first()
           if row != None or schemaname[0:3] == 'SYS':
               exist_sourceschema = True
           else:
               exist_sourceschema = False
        except sql.CommunicationError , err:   
            print 'Connect failed, try again:' , err
        except sql.SQLError, err:
            exist_sourceschema = False
            options.sourcedbschema = ''
        if options.sourcedbschema == '' and options.sourcedbschema != '':
           STEPID = STEPID+1
           while options.sourcedbschema in ['']:
                 schemacollection     = exportmonitor.sql(DB_SCHEMAS_SOURCE % ( re.split(',',options.sourcedbuser)[0] ))
                 schemas = []
                 if schemacollection != None:
                    for schema in schemacollection :
                        schemas.append(schema[0])
                 schemadict  = dict([(schemas.index(schema)+1,schema) for schema in schemas])  
                 if len(schemadict) == 1:
                     options.sourcedbschema = schemadict[1]
                 if len(schemadict)  > 1: 
                    print "\nStep %i: Choose a source database schema " % STEPID 
                    index = 0
                    for schema in schemadict:
                        index = index + 1
                        print ' [%i] %s ' % (index, schemadict[schema])
                    input_schema = ''
                    while input_schema == '' :
                          try :
                              input_schema  =  raw_input("\nStep %i: "  % STEPID )
                              if string.atoi(input_schema,10) >= 0 and string.atoi(input_schema,10) <= len (schemadict):
                                 options.sourcedbschema = schemadict[string.atoi(input_schema,10)]
                              else:
                                 input_schema = ''
                          except:
                              input_schema = ''
                              pass    
                              
     if importmonitor == None:
        exist_targetschema = True 
     if options.targetdbschema == '' and importmonitor != None and DB_SCHEMAS_TARGET == ''  :
        exist_targetschema = True 
     if options.targetdbschema == '' and importmonitor != None and DB_SCHEMAS_TARGET != '' and TESTLOADER == 'FALSE':  
        try:   
           resultset = importmonitor.sql(EXISTSSCHEMA %schemaname )
           row       = resultset.first()
           if row != None:
              exist_targetschema = True
           else:
              exist_targetschema = False
        except sql.CommunicationError, err:   
            print 'Connect failed, try again:' , err
        except sql.SQLError, err:
            options.targetdbschema = ''
            exist_targetschema = False
        if options.targetdbschema == '' and options.targetdbschema != '':  
           STEPID = STEPID+1
           while options.targetdbschema in ['']:
                 schemacollection     = importmonitor.sql(DB_SCHEMAS_TARGET % ( username ) )
                 schemas = []
                 if schemacollection != None:
                    for schema in schemacollection :
                        schemas.append(schema[0])
                    schemadict  = dict([(schemas.index(schema)+1,schema) for schema in schemas]) 
                    if len(schemadict) == 1:
                       options.targetdbschema = schemadict[1] 
                    if len(schemadict)  > 1:
                       print "\nStep %i: Choose a target database schema " % STEPID 
                       index = 0
                       for schema in schemadict:
                           index = index + 1
                           print ' [%i]:  %s ' % (index, schemadict[schema])
                       input = '-1'
                       while string.atoi(input,10) < 0 or string.atoi(input,10) > len (schemadict):
                             input  =  raw_input("\nStep %i: "  % STEPID )
                             options.targetdbschema = schemadict[string.atoi(input,10)]
     if exist_sourceschema == True and exist_targetschema == True:
        rc = True
     else:
        rc = False
     return rc

def  export_configuration (options, exportmonitor, importmonitor ):    
     try: 
        configuration = codecs.open(os.path.join(options.sourcepackagepath, '%s.config' % os.path.split(options.sourcepackagepath)[1] ), "wb+","utf-8")
        count  = 1
        schemaname = ''
        tablename  = ''
        schema = ''
        table  = ''
        if options.sourcedbschema       != '':
           if CASESENSITIVE == True or   (options.sourcedbschema[0] == '"' and options.sourcedbschema[len(options.sourcedbschema)-1] == '"'):
              schemaname = string.replace(options.sourcedbschema,'"','')
           else:
              schemaname = string.upper  (options.sourcedbschema)
           schema = "\"%s\"" % schemaname
        elif options.sourcedbuser != '':
           username     = re.split(','  , options.sourcedbuser)[0]
           userpassword = re.split(','  , options.sourcedbuser)[1]
           if (CASESENSITIVE == True and username != '') or (username != '' and username [0] != '"' and username[len(username)-1] != '"'):
              username  = string.upper  (username)
           elif username != '' :
              username  = string.replace(username,'"','')  
           schemaname   = username
           schema       = "\"%s\"" % schemaname
        table  = raw_input(' [%i] ' %count )
        while table not in   [''] :
              if re.search('[%_]',table) == None :
                 if len(re.split('\.',table)) == 1:
                    tablename  = table
                    if CASESENSITIVE == True or  (tablename[0] == '"' and tablename[len(tablename)-1] == '"'):
                       tablename = string.replace(tablename,'"','')
                    else:
                       tablename = string.upper(tablename)
                    table  = "\"%s\"" % tablename
                 if len(re.split('\.',table)) == 2: 
                    schemaname = re.split('\.',table)[0]
                    tablename  = re.split('\.',table)[1]
                    if CASESENSITIVE == True or   (schemaname [0] == '"' and schemaname[len(schemaname)-1] == '"'):
                       schemaname = string.replace(schemaname,'"','')
                    else:
                       schemaname = string.upper (schemaname)
                    if CASESENSITIVE == True or  (tablename [0] == '"' and tablename[len(tablename)-1] == '"'):
                       tablename = string.replace(tablename,'"','')
                    else:
                       tablename = string.upper(tablename)
                    schema = "\"%s\"" % schemaname
                    table  = "\"%s\"" % tablename
                 if exist_table( options, exportmonitor, importmonitor, schema, table) == True:
                    count = count + 1
                    configuration.write(u'%s,%s\n' %  (schema,table) )
                    table = raw_input(' [%i] ' %count )
                 else:
                    table = raw_input(' [%i] ' %count )
              if re.search('[%_]',table) != None :
                 if len(re.split('\.',table)) == 1:
                    tablename  = table
                    if CASESENSITIVE == True or  (tablename[0] == '"' and tablename[len(tablename)-1] == '"'):
                       tablename = string.replace(tablename,'"','')
                    else:
                       tablename = string.upper(tablename)
                    table  = "\"%s\"" % tablename
                 if len(re.split('\.',table)) == 2: 
                    schemaname = re.split('\.',table)[0]
                    tablename  = re.split('\.',table)[1]
                    if CASESENSITIVE == True or   (schemaname [0] == '"' and schemaname[len(schemaname)-1] == '"'):
                       schemaname = string.replace(schemaname,'"','')
                    else:
                       schemaname = string.upper(schemaname)
                    if CASESENSITIVE == True  or (tablename   [0] == '"' and tablename[len(tablename)-1] == '"'):
                       tablename = string.replace(tablename,'"','')
                    else:
                       tablename = string.upper(tablename)
                    schema = "\"%s\"" % schemaname
                    table  = "\"%s\"" % tablename
                 try:  
                    resultset = exportmonitor.sql(PACKAGETABLES % (schemaname,tablename))
                    for row in resultset:
                        print '     %s.%s' % (row[0],row[1])
                        configuration.write(u'%s,%s\n' %  (row[0],row[1]) )
                    count = count + 1
                    table = raw_input(' [%i] ' %count )
                 except sql.CommunicationError, err:   
                    table = ''
                    print 'Connect failed, try again:' , err
                 except sql.SQLError , err:
                    print 'Error in transport configuration %s ' % err
                    table = ''
        configuration.close() 
     except:
        print 'Error in transport configuration ' 
        
def  exist_table (options, exportmonitor, importmonitor, schemaname, table):
     exist_sourcetable = True
     exist_targettable = True
     if len(re.split('\.',table)) == 1:
        if CASESENSITIVE == True or   (schemaname [0] == '"' and schemaname[len(schemaname)-1] == '"'):
           schemaname = string.replace(schemaname,'"','')
        else:
           schemaname = string.upper(schemaname)
        schema = schemaname
        if CASESENSITIVE == True or (table [0] == '"' and table[len(table)-1] == '"'):
           table = table[1:len(table)-1]
        else:
           table = string.upper(table)
     if len(re.split('\.',table)) == 2: 
        schema     = re.split('\.',table)[0]
        tablename  = re.split('\.',table)[1]
        if CASESENSITIVE == True or (schema [0] == '"' and schema[len(schema)-1] == '"'):
           schema = schema[1:len(schema)-1]
        else:
           schema = string.upper(schema)
        if CASESENSITIVE == True or  (tablename [0] == '"' and tablename[len(tablename)-1] == '"'):
           tablename = tablename[1:len(tablename)-1]
        else:
           tablename = string.upper   (tablename)
        table = tablename
     if exportmonitor == None:
        exist_sourcetable = True    
     if exportmonitor != None and TESTLOADER == 'FALSE':  
        try:   
           exportmonitor.sql('EXISTS TABLE "%s"."%s" ' % (schema,table))
           exist_sourcetable = True
        except sql.CommunicationError, err:   
            print 'Connect failed, try again:' , err
        except sql.SQLError , err:
            if  err.errorCode == -4004: 
                exist_sourcetable = False
                print ' Source database table          : "%s"."%s"       %s' %(schema,table,'does not exist')
     if importmonitor == None:
        exist_targettable = True           
     if options.transporttype == 'GENERATE':
        exist_targettable = False
     if STREAMDATA_DUPLICATES    != 'REJECT DUPLICATES':
        exist_targettable = False
     if importmonitor != None :
        if options.transporttype != 'GENERATE' and STREAMDATA_DUPLICATES == 'REJECT DUPLICATES':         
           try: 
                importmonitor.sql('EXISTS TABLE "%s"."%s" ' % (options.targetdbschema,table))
                exist_targettable = True
                print ' Target database table          : "%s"."%s"       %s' %(options.targetdbschema,table,'does not exist')
           except sql.CommunicationError , err:   
                print 'Connect failed, try again:' , err
           except sql.SQLError , err:
                if err.errorCode == -4004: 
                   exist_targettable = False
     if options.transportdirection == 'TRANSPORT':
        if exist_sourcetable == True and exist_targettable == False:
           rc = True
        else:
           rc = False  
     if options.transportdirection == 'EXPORT' :
        if exist_sourcetable == True:
           rc = True
        else:
           rc = False  
     if options.transportdirection == 'IMPORT' :     
        if exist_targettable == False:
           rc = True
        else:
           rc = False     
     return rc  
     
if __name__ == '__main__':
    optlib.optMain2 (main, _options ())
    
       
 
