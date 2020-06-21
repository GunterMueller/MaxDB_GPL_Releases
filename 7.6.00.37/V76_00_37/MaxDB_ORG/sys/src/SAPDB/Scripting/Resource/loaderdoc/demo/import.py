#    coding: utf-8
#
#    module:       import.py
#
#    responsible : d033893
#
#    special area: dav
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

BENCHMARK    = 'true'
BENCHMAXROWS = 1000
TAB1MAXROWS  = 20
TAB2MAXROWS  = 1000
TAB3MAXROWS  = 20
TAB4MAXROWS  = 20
TAB5MAXROWS  = 20
TAB6MAXROWS  = 20
TAB7MAXROWS  = 20
TAB8MAXROWS  = 20


#system
import sys
import os
import time
import optlib
import socket
import math
#string
import re
import string
import tempfile
#sapdb
from sdb import dbm,loader,sql

def _options ():
    return [
    ('mode', 'mode'             , ':', 'PARAM'                 , 'specifies debug  mode'),                 # PARAM,INTER,DEBUG
    ('n'   , 'node'             , ':', socket.gethostname()    , 'specifies servernode'),
    ('r'   , 'instroot'         , ':', ''                      , 'specifies INSTROOT path'),
    ('d'   , 'dbname'           , ':', ''                      , 'specifies sapdb to create'),
    ('a'   , 'dbaUser'          , ':', 'DBA,DBA'               , 'specifies sapdb sysdba user'),
    ('i'   , 'init'             , ':', 'system'                , 'init sapdb'),                            # DELETE, SYSTEM, USER, CATALOG, DATA, DAV          
]

TESTPATH        = ''

def getEnvironment(options,environment):
    global TESTPATH
    TESTPATH      = os.path.split(os.environ['JTEST_ROOT'])[0]
    environment.append(TESTPATH)

def main (options, args):
    print "\nLoader Import Module:\n'%s'" % os.path.abspath(sys.argv[0])
    environment = []
    getEnvironment(options,environment)
    options.dbname = string.upper(options.dbname)
    options.mode   = string.upper(options.mode)
    options.init   = string.upper(options.init )

    session = connect( options.node, options.instroot, options.dbname, options.dbaUser )
    
    if  session <> None:
        #DELETE
        if options.init == 'DELETE' :
           print 'Cleaning  Databasesystem %s' % options.dbname
           dbauser = re.split  (',',options.dbaUser)[1] 
           try:
              sqlsession = sql.connect ( dbauser, dbauser, options.dbname, options.node ) 
           except sql.SQLError, err :
              raise sql.SQLError, err, sys.exc_traceback

           '''
           try:  
              sqlsession.sql ( "DELETE FROM SYSLOADER.TRANSFORMATIONMODEL where packageguid not like x''" )
           except sql.SQLError, err :
              raise sql.SQLError, err, sys.exc_traceback
           '''
           
           try:  
              sqlsession.sql ( 'commit' )
           except sql.SQLError, err :
              raise sql.SQLError, err, sys.exc_traceback

        #DAV    
        if options.init == 'WEBDAV'  and socket.gethostname()  == 'p66796' and options.dbname <> 'LO76T' and options.dbname <> 'TARGETDB'  :
           try:
              if options.mode == 'INTER':
                 input('Import    DAV Database ')
              if options.mode == 'PARAM':
                 print 'Import    DAV Database '
                 i = 1/0
           except:
              pass    
           davLoad (session, options, environment) 
        #DB
        else:
           try:
              if options.mode == 'INTER':
                 input('Import    Database ')
              if options.mode == 'PARAM':
                 print 'Import    Database '
                 i = 1/0
           except:
              pass    
           demoLoad (session, options, environment)
       
def davLoad(session, options, environment): 

    TESTPATH = environment[0]
    
    loaderCmd(session, ' USE USER %s %s '  % (re.split(',',options.dbaUser)[0], re.split(',',options.dbaUser)[1] ))
    
    dataLoad (session, """
             LOAD DATA TABLE "XML_ASSIGN_DC_IDX"
                             "DCID"       1 CHAR
                             "IDXID"      2 CHAR
             INFILE *
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
              INFILE  *
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
    INFILE  *
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
     
    #dav import     
    cmd = 'x_python %s -d %s -s %s -h %s -t %s' % ( os.path.join(TESTPATH,'sdb','dav' ,'demo','import.py' ), options.dbname, 
                                                    os.path.join(TESTPATH,'sdb','load','demo','sourcedb'  ), 
                                                   '%s:%s' % ( socket.gethostname(), '85'), '/webdav' )
    try:
        os.system (cmd)
    except OSError, err:
        print 'command failed:', err                                            
    
def demoLoad(session, options, environment) :

    TESTPATH = environment[0]
    
    loaderCmd(session, 'SQLMODE INTERNAL')
    #format
    loaderCmd(session, 'SET COMPRESSED       \'/,/\"/\'') 
    loaderCmd(session, 'SET LANGUAGE         \'ENG\' ') 
    #datatypes
    loaderCmd(session, 'SET CODETYPE         ASCII ')
    loaderCmd(session, 'SET CODEPAGE         ISO-8859-1 ')
    loaderCmd(session, 'SET BOOLEAN          \'TRUE/FALSE\' ')
    loaderCmd(session, 'SET DECIMAL          \'//./\'') 
    loaderCmd(session, 'SET NULL             \'?                   \'')
    loaderCmd(session, 'SET DATE             INTERNAL')
    loaderCmd(session, 'SET TIME             INTERNAL')
    loaderCmd(session, 'SET TIMESTAMP        INTERNAL')
    #loaderCmd(session, 'SET FORMAT           ISO')
    #transaction
    loaderCmd(session, 'AUTOCOMMIT ON')
    loaderCmd(session, 'SET TRANSACTION SIZE 1000')
    loaderCmd(session, 'SET MAXERRORCOUNT    100')
    #medium
    loaderCmd(session, 'SET BACKUPTOOL       NETWORKER ')
    #protocol
    loaderCmd(session, 'SET ODBCTRACE        OFF ')
                                        
    # CREATE USER
    if  options.init  == 'USER' or options.init == 'CATALOG' or options.init == 'DATA' :
        
        try:
           if options.mode == 'INTER':
              input('Import    Database       USERS')
           if options.mode == 'PARAM':
              print 'Import    Database       USERS'
              i = 1/0
        except:
           pass

        loaderCmd(session, 'USE USER DBA DBA')
        
        '''
        if  ExistsTable (session, 'TRANSFORMATIONMODEL'):
          
            dataLoad (session,
                      """
                      LOAD DATA TABLE SYSLOADER.TRANSFORMATIONMODEL   
                      OWNER                 1 DEFAULT      NULL TABLENAME            2 DEFAULT      NULL DATASTREAMNAME          3 DEFAULT      NULL 
                      DATASTREAMHEADERBLOCK 4 DEFAULT      NULL DATASTREAMHEADERPOS  5 DEFAULT      NULL DATASTREAMTRAILERBLOCK  6 DEFAULT      NULL DATASTREAMTRAILERPOS  7 DEFAULT      NULL  DATASTREAMDATABLOCK  8 DEFAULT      NULL DATASTREAMDATAPOS    9  DEFAULT    NULL 
                      \"EXCLUDE\"           10 DEFAULT     NULL 
                      PACKAGEGUID           11 DEFAULT     NULL TASKID              12 DEFAULT      NULL STEPID                 13 DEFAULT      NULL SERVERNODE           14 DEFAULT      NULL 
                      \"SERVERDB\"          15 DEFAULT     NULL USERNAME            16 DEFAULT      NULL \"CODETYPE\"           17 DEFAULT      NULL CODEPAGEID           18 DEFAULT      NULL 
                      \"STORAGEDEVICE\"     19 DEFAULT     NULL TRANSACTIONSIZE     20 DEFAULT      NULL TRANSFORMATION         21 DEFAULT      NULL PART                 22 DEFAULT      NULL  TABLETYPE            23 DEFAULT      NULL 
                      CATALOGSTREAMNAME     24 DEFAULT     NULL CATALOGSTREAMTYPE   25 DEFAULT      NULL CATALOGSTREAMEBID      26 DEFAULT      NULL CATALOGSTREAMFORMAT  27 DEFAULT      NULL  DATASTREAMTYPE       28 DEFAULT      NULL  DATASTREAMEBID     29 DEFAULT     NULL 
                      DATASTREAMFORMAT      30 DEFAULT     NULL \"RESTART\"         31 DEFAULT      NULL RESTORE                32 DEFAULT      NULL PAGECOUNT            33 DEFAULT      NULL 
                      ROWCOUNT              34 DEFAULT     NULL STARTTIME           35 DEFAULT      NULL ENDTIME                36 DEFAULT      NULL REQUESTTIME          37 DEFAULT      NULL 
                      RECEIVETIME           38 DEFAULT     NULL IOTIME              39 DEFAULT      NULL LOGTIME                40 DEFAULT      NULL 
                      INFILE *  COMPRESSED 
                      DECIMAL '/ /./' DATE INTERNAL TIME INTERNAL TIMESTAMP INTERNAL NULL '?                   ' BOOLEAN 'TRUE/FALSE' ""","""
"EDEMO","PHOTOS","SOURCEDB.data","0","0","0","431972","0","432996","FALSE","100000000000000000000000000000000000000000000000","1","1","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","14","20021206112910213000","20021206112915440000","0.0300000000","5.1480000000","0.0000000000","0.0100000000"
"EDEMO","REQUESTS","SOURCEDB.data","0","432996","0","435838","0","436862","FALSE","100000000000000000000000000000000000000000000000","1","2","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","6","20021206112915450000","20021206112915511000","0.0000000000","0.0000000000","0.0000000000","0.0100000000"
"SQLTRAVEL00","ACCOUNT","SOURCEDB.data","0","436862","0","438415","0","439439","FALSE","100000000000000000000000000000000000000000000000","1","3","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","28","20021206112915521000","20021206112915601000","0.0000000000","0.0000000000","0.0000000000","0.0200000000"
"SQLTRAVEL00","CITY","SOURCEDB.data","0","439439","0","441014","0","442038","FALSE","100000000000000000000000000000000000000000000000","1","15","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","25","20021206112915621000","20021206112915701000","0.0000000000","0.0000000000","0.0000000000","0.0200000000"
"EDEMO","CATEGORIES","SOURCEDB.data","0","442038","0","528899","0","529923","FALSE","100000000000000000000000000000000000000000000000","1","29","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","8","20021206112915711000","20021206112920938000","0.0000000000","5.1480000000","0.0000000000","0.0200000000"
"EDEMO","SUPPLIERS","SOURCEDB.data","0","529923","0","534800","0","535824","FALSE","100000000000000000000000000000000000000000000000","1","30","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","29","20021206112920948000","20021206112921008000","0.0100000000","0.0000000000","0.0000000000","0.0200000000"
"EDEMO","CUSTOMERS","SOURCEDB.data","0","535824","0","549174","0","550198","FALSE","100000000000000000000000000000000000000000000000","1","31","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","91","20021206112921018000","20021206112921089000","0.0000000000","0.0000000000","0.0000000000","0.0100000000"
"EDEMO","EMPLOYEES","SOURCEDB.data","0","550198","0","880876","0","881900","FALSE","100000000000000000000000000000000000000000000000","1","32","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","15","20021206112921099000","20021206112926556000","0.0100000000","5.3980000000","0.0000000000","0.0200000000"
"EDEMO","SHIPPERS","SOURCEDB.data","0","881900","0","882983","0","884007","FALSE","100000000000000000000000000000000000000000000000","1","33","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","3","20021206112926566000","20021206112926627000","0.0000000000","0.0000000000","0.0000000000","0.0000000000"
"SQLTRAVEL00","CUSTOMER","SOURCEDB.data","0","884007","0","885841","0","886865","FALSE","100000000000000000000000000000000000000000000000","1","34","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","15","20021206112926637000","20021206112926727000","0.0000000000","0.0000000000","0.0000000000","0.0200000000"
"SQLTRAVEL10","HOTEL","SOURCEDB.data","0","886865","0","890496","0","891520","FALSE","100000000000000000000000000000000000000000000000","1","48","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","15","20021206112926737000","20021206112926817000","0.0000000000","0.0000000000","0.0000000000","0.0200000000"
"EDEMO","PRODUCTS","SOURCEDB.data","0","891520","0","898933","0","899957","FALSE","100000000000000000000000000000000000000000000000","1","62","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","77","20021206112926827000","20021206112926887000","0.0000000000","0.0000000000","0.0000000000","0.0100000000"
"EDEMO","ORDERS","SOURCEDB.data","0","899957","0","1045100","0","1046124","FALSE","100000000000000000000000000000000000000000000000","1","63","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","1078","20021206112926907000","20021206112927007000","0.0300000000","0.0000000000","0.0000000000","0.0100000000"
"SQLTRAVEL10","ROOM","SOURCEDB.data","0","1046124","0","1048059","0","1049083","FALSE","100000000000000000000000000000000000000000000000","1","64","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","38","20021206112927017000","20021206112927097000","0.0000000000","0.0000000000","0.0000000000","0.0100000000"
"EDEMO","ORDERDETAILS","SOURCEDB.data","0","1049083","0","1164945","0","1165969","FALSE","100000000000000000000000000000000000000000000000","1","78","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","2820","20021206112927107000","20021206112927217000","0.0000000000","0.0000000000","0.0100000000","0.0100000000"
"SQLTRAVEL20","RESERVATION","SOURCEDB.data","0","1165969","0","1167426","0","1168450","FALSE","100000000000000000000000000000000000000000000000","1","79","LOCALHOST","SOURCEDB","DBA","ASCII","ISO-8859-1","NSR","1000","EXTRACT","ALL","TABLE","SOURCEDB.catalog","FILE","","DDL","FILE","","RECORDS","FALSE","FALSE","0","10","20021206112927227000","20021206112927307000","0.0000000000","0.0000000000","0.0000000000","0.0100000000"
""")
        '''
        if  BENCHMARK == 'true':     
            if not ExistsUser (session, 'BENCHMARK'):
               loaderCmd(session, 'CREATE USER BENCHMARK PASSWORD initial DBA NOT EXCLUSIVE')
           
        if not ExistsUser (session, 'EDEMO'):
           loaderCmd(session, 'CREATE USER MONA  PASSWORD initial DBA NOT EXCLUSIVE')   
        
        if not ExistsUser (session, 'EDEMO'):
           loaderCmd(session, 'CREATE USER EDEMO PASSWORD initial DBA NOT EXCLUSIVE')

        if not ExistsUser (session, 'SQLTRAVEL00'):
           loaderCmd(session, 'CREATE USER sqltravel00 PASSWORD initial DBA NOT EXCLUSIVE')

        if not ExistsUser (session, 'SQLTRAVEL10'):
           loaderCmd(session, 'CREATE USER sqltravel10 PASSWORD initial DBA NOT EXCLUSIVE')

        if not ExistsUser (session, 'SQLTRAVEL20'):
           loaderCmd(session, 'CREATE USER sqltravel20 PASSWORD initial DBA NOT EXCLUSIVE')


        loaderCmd(session, 'USE USER sqltravel00 initial')

        if not ExistsUser (session, 'SQLTRAVEL01'):
           loaderCmd(session, 'CREATE USER sqltravel01 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL02'):
           loaderCmd(session, 'CREATE USER sqltravel02 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL03'):
           loaderCmd(session, 'CREATE USER sqltravel03 PASSWORD initial RESOURCE')

        loaderCmd(session, 'USE USER sqltravel10 initial')

        if not ExistsUser (session, 'SQLTRAVEL11'):
           loaderCmd(session, 'CREATE USER sqltravel11 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL12'):
           loaderCmd(session, 'CREATE USER sqltravel12 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL13'):
           loaderCmd(session, 'CREATE USER sqltravel13 PASSWORD initial RESOURCE')

        loaderCmd(session, 'USE USER sqltravel20 initial')

        if not ExistsUser (session, 'SQLTRAVEL21'):
           loaderCmd(session, """CREATE USER sqltravel21 PASSWORD initial RESOURCE""")
        if not ExistsUser (session, 'SQLTRAVEL22'):
           loaderCmd(session, 'CREATE USER sqltravel22 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL23'):
           loaderCmd(session, 'CREATE USER sqltravel23 PASSWORD initial RESOURCE')


    #CREATE CATALOG
    if  options.init == 'CATALOG' or options.init == 'DATA' :
        
        try:
           if options.mode == 'INTER':
              input('Import    Database       CATALOG')
           if options.mode == 'PARAM':
              print 'Import    Database       CATALOG'
              i = 1/0
        except: 
           pass
           
        loaderCmd(session, 'USE USER mona        initial')   
        
        loaderCmd(session, """CREATE TABLE city ( 
        zip                 CHAR(5)     PRIMARY KEY CONSTRAINT zip_cons CHECK 
                            SUBSTR(zip,1,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,2,1) BETWEEN '0' AND '9' AND               
                            SUBSTR(zip,3,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,4,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,5,1) BETWEEN '0' AND '9',
        name                CHAR(20)    NOT NULL,
        state               CHAR(2)     NOT NULL)""")

        loaderCmd(session, """CREATE TABLE customer
        (cno                FIXED(4)    PRIMARY KEY CONSTRAINT cno_cons CHECK cno > 0,
        title               CHAR(7)     CONSTRAINT title_cons CHECK title IN ('Mr','Mrs','Company'),
        firstname           CHAR(10),
        name                CHAR(10)    NOT NULL,
        zip                 CHAR(5)     CONSTRAINT zip_cons CHECK 
                            SUBSTR(zip,1,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,2,1) BETWEEN '0' AND '9' AND               
                            SUBSTR(zip,3,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,4,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,5,1) BETWEEN '0' AND '9',
        address             CHAR (25)   NOT NULL,
        FOREIGN KEY customer_zip_in_city (zip) REFERENCES city ON DELETE RESTRICT)""")

        loaderCmd(session, """CREATE TABLE hotel (
        hno                 FIXED(4)    PRIMARY KEY CONSTRAINT hno_cons CHECK hno > 0,
        name                CHAR(15)    NOT NULL,
        zip                 CHAR(5)     CONSTRAINT zip_cons CHECK
                            SUBSTR(zip,1,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,2,1) BETWEEN '0' AND '9' AND               
                            SUBSTR(zip,3,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,4,1) BETWEEN '0' AND '9' AND
                            SUBSTR(zip,5,1) BETWEEN '0' AND '9',
        address             CHAR(25)    NOT NULL,
        info                LONG,
        FOREIGN KEY hotel_zip_in_city (zip) REFERENCES city ON DELETE RESTRICT)""")

        loaderCmd(session, """CREATE TABLE room
        (hno                FIXED(4)    CONSTRAINT hno_cons CHECK hno > 0,
        type               CHAR(6)     CONSTRAINT type_cons CHECK type IN ('single','double','suite'), PRIMARY KEY (hno,type),
        free               FIXED(3,0)  CONSTRAINT free_cons CHECK free >= 0,
        price              FIXED(6,2)  CONSTRAINT price_cons CHECK price BETWEEN 0.00 AND 5000.00,
        FOREIGN KEY room_hno_in_hotel (hno) REFERENCES hotel ON DELETE CASCADE)""")

        loaderCmd(session, """CREATE TABLE reservation (
        rno                FIXED(4)    PRIMARY KEY CONSTRAINT rno_cons CHECK rno > 0,
        cno                FIXED(4)    CONSTRAINT cno_cons CHECK cno > 0,
        hno                FIXED(4)    CONSTRAINT hno_cons CHECK hno > 0,
        type               CHAR(6)     CONSTRAINT type_cons CHECK type IN ('single','double','suite'),
        arrival            DATE        NOT NULL,
        departure          DATE        NOT NULL, CONSTRAINT staying CHECK departure > arrival,
        FOREIGN KEY reservation_cno_in_customer (cno)      REFERENCES customer ON DELETE CASCADE,
        FOREIGN KEY reservation_info_in_room    (hno,type) REFERENCES room     ON DELETE CASCADE)""")

        loaderCmd(session, """CREATE TABLE employee
        (hno                 FIXED(4),     
        eno                 FIXED(4),   PRIMARY KEY (hno,eno),
        title               CHAR(7)     CONSTRAINT title_cons CHECK title IN ('Mr','Mrs'),
        firstname           CHAR(10),
        name                CHAR(10)    NOT NULL,
        manager_eno         FIXED(4),
        FOREIGN KEY employee_hno_in_hotel (hno) REFERENCES hotel ON DELETE CASCADE)""")

        loaderCmd(session, """CREATE VIEW customer_addr (cno, title, name, zip, city, state, address)
        AS SELECT customer.cno, customer.title, customer.name, customer.zip,
                  city.name, city.state, customer.address
                  FROM   customer, city
                  WHERE  customer.zip = city.zip
                  WITH CHECK OPTION""")

        loaderCmd(session, """CREATE VIEW hotel_addr (hno, name, zip, city, state, address)
        AS SELECT hotel.hno, hotel.name, hotel.zip, city.name,
                    city.state, hotel.address
                    FROM   hotel, city
                    WHERE  hotel.zip = city.zip
                    WITH CHECK OPTION""")

        loaderCmd(session, """CREATE VIEW custom_hotel (customname, customcity, hotelname, hotelcity)
        AS SELECT   customer_addr.name, customer_addr.city, hotel_addr.name, hotel_addr.city
                    FROM  customer_addr,
                          hotel_addr,
                          reservation
                    WHERE customer_addr.cno = reservation.cno
                    AND   hotel_addr.hno    = reservation.hno""")
           
        loaderCmd(session, 'USE USER sqltravel00 initial')

        if not ExistsUser (session, 'SQLTRAVEL01'):
           loaderCmd(session, 'CREATE USER sqltravel01 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL02'):
           loaderCmd(session, 'CREATE USER sqltravel02 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL03'):
           loaderCmd(session, 'CREATE USER sqltravel03 PASSWORD initial RESOURCE')

        loaderCmd(session, 'USE USER sqltravel10 initial')

        if not ExistsUser (session, 'SQLTRAVEL11'):
           loaderCmd(session, 'CREATE USER sqltravel11 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL12'):
           loaderCmd(session, 'CREATE USER sqltravel12 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL13'):
           loaderCmd(session, 'CREATE USER sqltravel13 PASSWORD initial RESOURCE')

        loaderCmd(session, 'USE USER sqltravel20 initial')

        if not ExistsUser (session, 'SQLTRAVEL21'):
           loaderCmd(session, """CREATE USER sqltravel21 PASSWORD initial RESOURCE""")
        if not ExistsUser (session, 'SQLTRAVEL22'):
           loaderCmd(session, 'CREATE USER sqltravel22 PASSWORD initial RESOURCE')
        if not ExistsUser (session, 'SQLTRAVEL23'):
           loaderCmd(session, 'CREATE USER sqltravel23 PASSWORD initial RESOURCE')

        # CREATE TABLES
        loaderCmd(session, 'USE USER sqltravel00 initial')

        if ExistsDomain (session, 'CNO_DOM'):
           loaderCmd(session, 'DROP DOMAIN cno_dom')
        loaderCmd(session, """CREATE DOMAIN cno_dom FIXED (4)
            CONSTRAINT cno_dom BETWEEN 1 AND 9999""")

        if ExistsDomain (session, 'ZIP_DOM'):
           loaderCmd(session, 'DROP DOMAIN zip_dom')
        loaderCmd(session, """CREATE DOMAIN zip_dom CHAR (5) CONSTRAINT
        SUBSTR(zip_dom,1,1) between '1' AND '9' AND 
        SUBSTR(zip_dom,2,1) between '0' AND '9' AND 
        SUBSTR(zip_dom,3,1) between '0' AND '9' AND 
        SUBSTR(zip_dom,4,1) between '0' AND '9' AND 
        SUBSTR(zip_dom,5,1) between '0' AND '9' """)

        if ExistsTable (session, 'CITY'):
           loaderCmd(session, 'DROP TABLE city')
        loaderCmd(session, """ CREATE TABLE city
            (zip                zip_dom     KEY,
             name               CHAR(20)    NOT NULL,
             state              CHAR(2)     NOT NULL)""")

        if ExistsTable (session, 'CUSTOMER'):
           loaderCmd(session, 'DROP TABLE customer')
        loaderCmd(session, """CREATE TABLE customer
            (cno                cno_dom     KEY,
             title              CHAR (7)    CONSTRAINT title IN ('Mr', 'Mrs', 'Company'),
             firstname          CHAR (10)   ,
             name               CHAR (10)   NOT NULL,
             zip                zip_dom     ,
             address            CHAR (25)   NOT NULL,
             FOREIGN KEY (zip) REFERENCES sqltravel00.city ON DELETE RESTRICT)""")

        loaderCmd(session, """CREATE INDEX customer_index ON customer (name)""")

        loaderCmd(session, """CREATE VIEW customer_addr
            (c_no, c_title, c_name, c_firstname,
            c_zip, c_city, c_state, c_address)
            AS SELECT cno, title, customer.name, firstname,
            customer.zip, city.name, state,
            customer.address
            FROM   customer, city
            WHERE  customer.zip = city.zip
            WITH CHECK OPTION""")

        if ExistsTable (session, 'ACCOUNT'):
            loaderCmd(session, 'DROP TABLE account')

        loaderCmd(session,"""CREATE TABLE account
            (booking_day                           DATE      KEY,
            activities                             FIXED (6,2) )""")

        loaderCmd(session,"""GRANT ALL ON city TO
            sqltravel01,sqltravel02,sqltravel03,
            sqltravel11,sqltravel12,sqltravel13,
            sqltravel21,sqltravel22,sqltravel23""")

        loaderCmd(session,"""GRANT ALL ON city TO
            sqltravel10, sqltravel20 WITH GRANT OPTION""")

        loaderCmd(session,"""GRANT ALL ON customer TO
            sqltravel01,sqltravel02,sqltravel03,
            sqltravel11,sqltravel12,sqltravel13,
            sqltravel21,sqltravel22,sqltravel23""")

        loaderCmd(session,"""GRANT ALL ON customer TO
            sqltravel10,sqltravel20 WITH GRANT OPTION""")

        loaderCmd(session,"""GRANT ALL ON customer_addr TO
            sqltravel01,sqltravel02,sqltravel03,
            sqltravel11,sqltravel12,sqltravel13,
            sqltravel21,sqltravel22,sqltravel23""")

        loaderCmd(session,"""GRANT ALL ON customer_addr TO
            sqltravel10,sqltravel20 WITH GRANT OPTION""")

        loaderCmd(session,"""GRANT ALL ON account TO
            sqltravel01,sqltravel02,sqltravel03,
            sqltravel10,sqltravel11,sqltravel12,sqltravel13,
            sqltravel20,sqltravel21,sqltravel22,sqltravel23""")


        loaderCmd(session, 'USE USER sqltravel10 initial')

        if ExistsDomain (session, 'HNO_DOM'):
            loaderCmd(session, 'DROP DOMAIN hno_dom')
        loaderCmd(session, """CREATE DOMAIN hno_dom FIXED (4) CONSTRAINT hno_dom BETWEEN 1 AND 9999""")

        if ExistsDomain (session, 'ROOM_DOM'):
            loaderCmd(session, 'DROP DOMAIN room_dom')
        loaderCmd(session, """CREATE DOMAIN room_dom CHAR (6) CONSTRAINT room_dom IN ('single', 'double','suite')""")

        if ExistsTable (session, 'HOTEL'):
            loaderCmd(session, 'DROP TABLE hotel')

        loaderCmd(session,"""CREATE TABLE hotel
            (hno                hno_dom     KEY,
             name               CHAR (15)   NOT NULL,
             zip                sqltravel00.zip_dom     NOT NULL,
             address            CHAR (25)   NOT NULL,
             info               LONG        ,
             FOREIGN KEY (zip) REFERENCES sqltravel00.city ON DELETE RESTRICT)""")

        if ExistsTable (session, 'ROOM'):
            loaderCmd(session, 'DROP TABLE room')

        loaderCmd(session,"""CREATE TABLE room
            (hno                hno_dom     KEY,
             type               room_dom    KEY,
             free               FIXED(3,0)  CONSTRAINT free >= 0,
             price              FIXED(6,2)  CONSTRAINT price BETWEEN 0.00 AND 5000.00,
             FOREIGN KEY (hno) REFERENCES sqltravel10.hotel ON DELETE CASCADE)""")

        loaderCmd(session,"""CREATE VIEW hotel_addr (h_no, h_name, h_zip, h_city, h_state, h_address)
              AS SELECT hno, hotel.name, hotel.zip, city.name,
                        state, hotel.address
                        FROM   hotel, sqltravel00.city
                        WHERE  hotel.zip = city.zip
                        WITH CHECK OPTION""")

        loaderCmd(session,"""GRANT ALL ON hotel TO sqltravel01,sqltravel02,sqltravel03,
                          sqltravel11,sqltravel12,sqltravel13,
                          sqltravel21,sqltravel22,sqltravel23""")
                          
        #loaderCmd(session,"""GRANT SELECT,INSERT ON hotel TO PUBLIC""") #                     

        loaderCmd(session,"""GRANT ALL ON hotel TO sqltravel00,sqltravel20 WITH GRANT OPTION""")

        loaderCmd(session,"""GRANT ALL ON room TO sqltravel01,sqltravel02,sqltravel03,
                         sqltravel11,sqltravel12,sqltravel13,
                         sqltravel21,sqltravel22,sqltravel23""")

        loaderCmd(session,"""GRANT ALL ON room TO sqltravel00,sqltravel20 WITH GRANT OPTION""")

        loaderCmd(session,"""GRANT ALL ON hotel_addr TO sqltravel01,sqltravel02,sqltravel03,
                               sqltravel11,sqltravel12,sqltravel13,
                               sqltravel21,sqltravel22,sqltravel23""")

        loaderCmd(session,"""GRANT ALL ON hotel_addr TO sqltravel00,sqltravel20 WITH GRANT OPTION""")

        loaderCmd(session, 'USE USER sqltravel20 initial')

        if ExistsDomain (session, 'RNO_DOM'):
            loaderCmd(session, 'DROP DOMAIN rno_dom')
        loaderCmd(session, """CREATE DOMAIN rno_dom FIXED (4)
            CONSTRAINT rno_dom BETWEEN 1 AND 9999""")

        if ExistsTable (session, 'RESERVATION'):
            loaderCmd(session, 'DROP TABLE reservation')
        loaderCmd(session, """ CREATE TABLE reservation
            (rno                rno_dom     KEY,
             cno                sqltravel00.cno_dom     ,
             hno                sqltravel10.hno_dom     ,
             type               sqltravel10.room_dom    ,
             arrival            DATE        NOT NULL,
             departure          DATE        NOT NULL CONSTRAINT departure > arrival,
             FOREIGN KEY (cno) REFERENCES sqltravel00.customer ON DELETE CASCADE,
             FOREIGN KEY (hno,type) REFERENCES sqltravel10.room ON DELETE CASCADE)""")

        loaderCmd(session,"""GRANT ALL ON reservation TO
             sqltravel00,sqltravel01,sqltravel02,sqltravel03,
             sqltravel10,sqltravel11,sqltravel12,sqltravel13,
             sqltravel21,sqltravel22,sqltravel23""")

        loaderCmd(session,"""CREATE VIEW custom_hotel
            (customname, customcity, hotelname, hotelcity)
              AS SELECT c_name, c_city, h_name, h_city
              FROM  sqltravel00.customer_addr,
                    sqltravel10.hotel_addr,
                    reservation
              WHERE c_no = reservation.cno
              AND   h_no = reservation.hno""")

        loaderCmd(session,"""GRANT ALL ON custom_hotel TO
                sqltravel00,sqltravel01,sqltravel02,sqltravel03,
                sqltravel10,sqltravel11,sqltravel12,sqltravel13,
                sqltravel21,sqltravel22,sqltravel23""")

        loaderCmd(session,"""USE USER sqltravel00 initial""")

        loaderCmd(session,"""CREATE SYNONYM hotel        FOR sqltravel10.hotel""")
        loaderCmd(session,"""CREATE SYNONYM room         FOR sqltravel10.room""")
        loaderCmd(session,"""CREATE SYNONYM reservation  FOR sqltravel20.reservation""")
        loaderCmd(session,"""CREATE SYNONYM hotel_addr   FOR sqltravel10.hotel_addr""")
        loaderCmd(session,"""CREATE SYNONYM custom_hotel FOR sqltravel20.custom_hotel""")
        
        #loaderCmd(session,"""GRANT SELECT ON hotel TO PUBLIC """) #

        loaderCmd(session,"""USE USER sqltravel10 initial""")

        loaderCmd(session,"""CREATE SYNONYM city          FOR sqltravel00.city""")
        loaderCmd(session,"""CREATE SYNONYM customer      FOR sqltravel00.customer""")
        loaderCmd(session,"""CREATE SYNONYM account       FOR sqltravel00.account""")
        loaderCmd(session,"""CREATE SYNONYM reservation   FOR sqltravel20.reservation""")
        loaderCmd(session,"""CREATE SYNONYM customer_addr FOR sqltravel00.customer_addr""")
        loaderCmd(session,"""CREATE SYNONYM custom_hotel  FOR sqltravel20.custom_hotel""")



        loaderCmd(session,"""USE USER sqltravel20 initial""")

        loaderCmd(session,"""CREATE SYNONYM city          FOR sqltravel00.city""")
        loaderCmd(session,"""CREATE SYNONYM customer      FOR sqltravel00.customer""")
        loaderCmd(session,"""CREATE SYNONYM account       FOR sqltravel00.account""")
        loaderCmd(session,"""CREATE SYNONYM hotel         FOR sqltravel10.hotel""")
        loaderCmd(session,"""CREATE SYNONYM room          FOR sqltravel10.room""")
        loaderCmd(session,"""CREATE SYNONYM customer_addr FOR sqltravel00.customer_addr""")
        loaderCmd(session,"""CREATE SYNONYM hotel_addr    FOR sqltravel10.hotel_addr""")


        loaderCmd(session,"""USE USER EDEMO initial""")
        
        #select OWNER, SEQUENCE_NAME, MIN_VALUE, MAX_VALUE,LAST_NUMBER,INCREMENT_BY, CYCLE_FLAG, CACHE_SIZE, ORDER_FLAG from sequences where SEQUENCE_NAME ='SEQ'
        
        #if ExistsTable (session, 'ТТОЫРЫГГУФВЫАП'):
        #   loaderCmd(session, 'DROP TABLE  ТТОЫРЫГГУФВЫАП')
        #loaderCmd(session,"""CREATE TABLE  ТТОЫРЫГГУФВЫАП (
        #    CATEGORYID       INTEGER   ,
        #    CATEGORYNAME     VARCHAR   (16)     NOT NULL,
        #    DESCRIPTION      LONG          ,
        #    PICTURE          LONG      BYTE
        #    , PRIMARY KEY (CATEGORYID))""")

        if ExistsTable (session, 'CATEGORIES'):
           loaderCmd(session, 'DROP TABLE categories')
        loaderCmd(session,"""CREATE TABLE CATEGORIES (
            CATEGORYID       INTEGER   ,
            CATEGORYNAME     VARCHAR   (16)     NOT NULL,
            DESCRIPTION      LONG          ,
            PICTURE          LONG      BYTE
            , PRIMARY KEY (CATEGORYID))""")

        loaderCmd(session,"""CREATE UNIQUE INDEX CATEGORYNAME ON
            EDEMO.CATEGORIES (CATEGORYNAME ASC )""")

        if ExistsTable (session, 'CATEGORIES_A'):
           loaderCmd(session, 'DROP TABLE CATEGORIES_A')
        loaderCmd(session,"""CREATE TABLE CATEGORIES_A (
            CATEGORYID       INTEGER   ,
            CATEGORYNAME     VARCHAR(16) ASCII   NOT NULL,
            DESCRIPTION      LONG        ASCII,
            PICTURE          LONG        BYTE,
            PRIMARY KEY (CATEGORYID))""")

        loaderCmd(session,"""CREATE UNIQUE INDEX CATEGORYNAME ON
            EDEMO.CATEGORIES_A (CATEGORYNAME ASC )""")
        
        if ExistsTable (session, 'SUPPLIERS'):
            loaderCmd(session, 'DROP TABLE suppliers')
        loaderCmd(session,"""CREATE TABLE SUPPLIERS (
            SUPPLIERID        INTEGER   ,
            COMPANYNAME       VARCHAR   (40)     NOT NULL,
            CONTACTNAME       VARCHAR   (30)     ,
            CONTACTTITLE      VARCHAR   (32)     ,
            ADDRESS           VARCHAR   (60)     ,
            CITY              VARCHAR   (15)     ,
            REGION            VARCHAR   (15)     ,
            POSTALCODE        VARCHAR   (10)     ,
            COUNTRY           VARCHAR   (15)     ,
            PHONE             VARCHAR   (24)     ,
            FAX               VARCHAR   (24) 
            , PRIMARY KEY (SUPPLIERID))""")

        loaderCmd(session,"""CREATE INDEX CompanyName ON EDEMO.Suppliers (CompanyName ASC )""")

        if ExistsTable (session, 'CUSTOMERS'):
            loaderCmd(session, 'DROP TABLE Customers')
        loaderCmd(session,"""CREATE TABLE Customers (
            CustomerID        VARCHAR   (5)     ,
            CompanyName       VARCHAR   (40)     NOT NULL,
            ContactName       VARCHAR   (30)     ,
            ContactTitle      VARCHAR   (30)     ,
            Address           VARCHAR   (60)     ,
            City              VARCHAR   (15)     ,
            Region            VARCHAR   (15)     ,
            PostalCode        VARCHAR   (10)     ,
            Country           VARCHAR   (15)     ,
            Phone             VARCHAR   (24)     ,
            Fax               VARCHAR   (24) 
            , PRIMARY KEY (CustomerID))""")

        loaderCmd(session,"""CREATE INDEX CompanyName ON EDEMO.Customers (CompanyName ASC )""")
        loaderCmd(session,"""CREATE INDEX City ON EDEMO.Customers (City ASC )""")
        loaderCmd(session,"""CREATE INDEX Region ON EDEMO.Customers (Region ASC )""")

        if ExistsTable (session, 'EMPLOYEES'):
            loaderCmd(session, 'DROP TABLE Employees')
        loaderCmd(session,"""CREATE TABLE EDEMO.Employees (
            EmployeeID        INTEGER   ,
            LastName          VARCHAR   (20)     NOT NULL,
            FirstName         VARCHAR   (10)     NOT NULL,
            Title             VARCHAR   (30)     ,
            BirthDate         DATE    ,
            HireDate          DATE    ,
            Address           VARCHAR   (60)     ,
            City              VARCHAR   (15)     ,
            Region            VARCHAR   (15)     ,
            PostalCode        VARCHAR   (10)     ,
            Country           VARCHAR   (15)     ,
            HomePhone         VARCHAR   (24)     ,
            Extension         VARCHAR   (4)      ,
            Photo             LONG      BYTE     ,
            Notes             LONG          ,
            ReportsTo         INTEGER
            , PRIMARY KEY (EmployeeID))""")

        loaderCmd(session,"""CREATE INDEX LastName ON EDEMO.Employees (LastName ASC )""")


        if ExistsTable (session, 'SHIPPERS'):
            loaderCmd(session, 'DROP TABLE Shippers')
        loaderCmd(session,"""CREATE TABLE Shippers (
            ShipperID         INTEGER   ,
            Companyname       VARCHAR   (40)     NOT NULL
            , PRIMARY KEY (ShipperID))""")
            
        if ExistsTable (session, 'PRODUCTS'):
            loaderCmd(session, 'DROP TABLE PRODUCTS')
        loaderCmd(session,"""CREATE TABLE PRODUCTS (
            ProductID         INTEGER   ,
            SupplierID        INTEGER   ,
            CategoryID        INTEGER   ,
            Productname       VARCHAR   (40) ,
            QuantityPerUnit   VARCHAR   (23) ,
            UnitPrice         FLOAT     (8)  ,
            UnitsInStock      SMALLINT  ,
            UnitsOnOrder      SMALLINT  ,
            ReorderLevel      SMALLINT  ,
            Discontinued      BOOLEAN DEFAULT  FALSE,
            BarcodeID         CHAR      (10)   BYTE DEFAULT x'0086B22A318528A4D7' ,
            PRIMARY KEY (ProductID))""")

        loaderCmd(session,"""ALTER TABLE EDEMO.Products FOREIGN KEY CategoryProduct
            (CategoryID
            ) REFERENCES EDEMO.Categories
            (CategoryID
            ) ON DELETE RESTRICT""")



        loaderCmd(session,"""ALTER TABLE EDEMO.Products FOREIGN KEY SupplierProduct
            (SupplierID
            ) REFERENCES EDEMO.Suppliers
            (SupplierID
            ) ON DELETE RESTRICT""")


        loaderCmd(session,"""CREATE INDEX ProductName ON EDEMO.Products (Productname ASC )""")
        loaderCmd(session,"""CREATE INDEX CategoryID  ON EDEMO.Products (CategoryID ASC )""")
        loaderCmd(session,"""CREATE INDEX SupplierID  ON EDEMO.Products (SupplierID ASC )""")

        if ExistsTable (session, 'ORDERS'):
           loaderCmd(session, 'DROP TABLE ORDERS')
        loaderCmd(session,"""CREATE TABLE EDEMO.ORDERS (
            OrderID           INTEGER   ,
            CustomerID        VARCHAR   (5)     NOT NULL,
            EmployeeID        INTEGER   ,
            Shipname          VARCHAR   (40)     ,
            ShipAddress       VARCHAR   (60)     ,
            ShipCity          VARCHAR   (15)     ,
            ShipRegion        VARCHAR   (15)     ,
            ShipPostalCode    VARCHAR   (10)     ,
            ShipCountry       VARCHAR   (15)     ,
            ShipVia           INTEGER   ,
            OrderDate         DATE    ,
            RequiredDate      DATE    ,
            ShippedDate       DATE    ,
            Freight           FLOAT     (8)
            , PRIMARY KEY (OrderID
            ))""")

        loaderCmd(session,"""ALTER TABLE EDEMO.Orders FOREIGN KEY CustomerOrder
            (CustomerID
            ) REFERENCES EDEMO.Customers
            (CustomerID
            ) ON DELETE RESTRICT""")



        loaderCmd(session,"""ALTER TABLE EDEMO.Orders FOREIGN KEY EmployeeOrder
            (EmployeeID
            ) REFERENCES EDEMO.Employees
            (EmployeeID
            ) ON DELETE RESTRICT""")



        loaderCmd(session,"""ALTER TABLE EDEMO.Orders FOREIGN KEY ShipperOrder
            (ShipVia
            ) REFERENCES EDEMO.Shippers
            (ShipperID
            ) ON DELETE RESTRICT""")

        loaderCmd(session,"""CREATE INDEX OrderDate  ON EDEMO.Orders (OrderDate ASC )""")
        loaderCmd(session,"""CREATE INDEX CustomerID ON EDEMO.Orders (CustomerID ASC )""")
        loaderCmd(session,"""CREATE INDEX EmployeeID ON EDEMO.Orders (EmployeeID ASC )""")

        if ExistsTable (session, 'ORDERDETAILS'):
            loaderCmd(session, 'DROP TABLE ORDERDETAILS')
        loaderCmd(session,"""CREATE TABLE EDEMO.ORDERDETAILS (
            OrderID           INTEGER   ,
            ProductID         INTEGER   ,
            UnitPrice         FLOAT     (8) NOT NULL,
            Quantity          SMALLINT DEFAULT  1,
            Discount          FLOAT(4) DEFAULT  0.000E+00,
            CONSTRAINT Quantity CHECK Quantity>0,
            CONSTRAINT Discount CHECK Discount BETWEEN 0 AND 1
            , PRIMARY KEY (OrderID,ProductID))""")

        loaderCmd(session,"""ALTER TABLE EDEMO.OrderDetails FOREIGN KEY ProductOrderDetails
            (ProductID
            ) REFERENCES EDEMO.Products
            (ProductID
            ) ON DELETE RESTRICT""")



        loaderCmd(session,"""ALTER TABLE EDEMO.OrderDetails FOREIGN KEY OrderOrderDetail
            (OrderID
            ) REFERENCES EDEMO.Orders
            (OrderID
            ) ON DELETE CASCADE""")

        loaderCmd(session,"""CREATE INDEX ProductID ON EDEMO.OrderDetails (ProductID ASC )""")
        loaderCmd(session,"""COMMENT ON COLUMN OrderDetails.OrderID     IS 'Same as OrderID in Orders Table.'""")
        loaderCmd(session,"""COMMENT ON COLUMN OrderDetails.ProductID   IS 'Same as ProductID in Products table.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Products.ProductID       IS 'Number automatically assigned to new product.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Products.SupplierID      IS 'Same as SupplierID in Suppliers table.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Products.CategoryID      IS 'Same as CategoryID in Categoriers table.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Products.QuantityPerUnit IS '(e.g., 24-count case, 1-liter bottle).'""")
        loaderCmd(session,"""COMMENT ON COLUMN Products.ReorderLevel    IS 'Minimum units to maintain in stock.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Products.Discontinued    IS 'Yes means item no longer available.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Orders.OrderID           IS 'Unique order number from invoice.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Orders.CustomerID        IS 'Same as CustomerID in Customers table.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Orders.EmployeeID        IS 'Same as EmployeeID in Employees table.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Orders.ShipName          IS 'Name of person or company to receive the shipment.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Orders.ShipAddress       IS 'Street address only -- no post-office box allowed.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Orders.ShipRegion        IS 'State or Province'""")
        loaderCmd(session,"""COMMENT ON COLUMN Orders.ShipVia           IS 'Same as ShipperID in Shippers table.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Categories.CategoryID    IS 'Number automatically assigned to new category.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Categories.CategoryName  IS 'Name of food category.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Categories.Picture       IS 'A picture representing the food category.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Customers.CustomerID     IS 'Unique five-character code based on customer name.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Customers.Address        IS 'Street or post-office box.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Customers.Region         IS 'State or province.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Customers.Phone          IS 'Phone number includes country code or area code.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Customers.Fax            IS 'Phone number includes country code or area code.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Suppliers.SupplierID     IS 'Number automatically assigned to new supplier.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Suppliers.Address        IS 'Street or post-office box.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Suppliers.Region         IS 'State or province.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Suppliers.Phone          IS 'Phone number includes country code or area code.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Suppliers.Fax            IS 'Phone number includes country code or area code.'""")
        loaderCmd(session,"""COMMENT ON TABLE EMPLOYEES                 IS 'This table holds the data of all employees.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.EmployeeID     IS 'Number automatically assigned to new employees.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.Title          IS 'Employee''s title.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.LastName       IS 'This is a ''test'' comment for SAPDBLOADER.'""") 
        loaderCmd(session,"""COMMENT ON COLUMN Employees.Address        IS 'Street or post-office box.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.Region         IS 'State or province.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.HomePhone      IS 'Phone number includes country code or area code.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.Extension      IS 'Phone number includes country code or area code.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.Photo          IS 'Picture of employee.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.Notes          IS 'General information about employee''s background.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Employees.ReportsTo      IS 'Employee''s supervisor.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Shippers.ShipperID       IS 'Number automatically assigned to new shipper.'""")
        loaderCmd(session,"""COMMENT ON COLUMN Shippers.CompanyName     IS 'Name of shipping company.'""")


        loaderCmd(session,"""CREATE VIEW EDEMO.OrderSubtotals AS
            SELECT OrderID, Sum(UnitPrice * Quantity * (1 - Discount) * 100 / 100) AS Subtotal 
            FROM OrderDetails
            GROUP BY OrderID""")

        if ExistsTable (session, 'REQUESTS'):
            loaderCmd(session, 'DROP TABLE REQUESTS')
        loaderCmd(session,"""CREATE TABLE EDEMO.REQUESTS (
            Requestname VARCHAR (40)  NOT NULL,
            Request     LONG  NOT NULL,
            PRIMARY KEY (RequestName))""")
            
        if ExistsTable (session, 'PHOTOS'):
            loaderCmd(session, 'DROP TABLE PHOTOS')
        loaderCmd(session,"""CREATE TABLE EDEMO.PHOTOS (
            Name    CHAR (18)  NOT NULL,
            Photo    LONG BYTE)""")
            
        if ExistsTable (session, 'NL_TEST'):
           loaderCmd(session, 'DROP TABLE NL_TEST')
        loaderCmd(session,"""CREATE TABLE EDEMO.NL_TEST (
              Number       INTEGER,
              Description  CHAR(500),
              Prenumber    INTEGER,
              CHANGED      TIMESTAMP)""")
            
        loaderCmd(session, 'USE USER DBA DBA')
        
        if  BENCHMARK == 'true':     
            if  not ExistsUser (session, 'BENCHMARK'):
                loaderCmd(session, 'CREATE USER BENCHMARK PASSWORD initial DBA NOT EXCLUSIVE')
            loaderCmd(session, 'USE USER BENCHMARK initial')
                
            if ExistsTable (session, 'BENCH'):
               loaderCmd(session, 'DROP TABLE BENCH')             
            loaderCmd(session,"""CREATE TABLE BENCH(
                                KSEQ    Integer    NOT NULL,
                                K500K   Integer,
                                K250K   Integer,
                                K100K   Integer,
                                K40K    Integer,
                                K10K    Integer,
                                K1K     Integer,
                                K100    Integer,
                                K25     Integer,
                                K10     Integer,
                                K5      Integer,
                                K4      Integer,
                                K2      Integer,
                                S1      Char (8),
                                S2      Char (20),
                                S3      Char (20),
                                S4      Char (20),
                                S5      Char (20),
                                S6      Char (20),
                                S7      Char (20),
                                S8      Char (20),
                                PRIMARY KEY (KSEQ))""")
                
            if ExistsTable (session, 'TAB1'):
               loaderCmd(session, 'DROP TABLE TAB1')
            loaderCmd(session,"""CREATE TABLE TAB1(
                            COL1 VARCHAR(250) NOT NULL,
                            COL2 CHAR NOT NULL,
                            COL3 VARCHAR(250)  ,
                            COL4 INTEGER,
                            COL5 SMALLINT,
                            COL6 FIXED(38) NOT NULL,
                            COL7 FIXED(38,38) CHECK (COL7 > 0.01),
                            COL8 FIXED(20,10),
                            COL9 VARCHAR(1000),
                            COL10 DATE,
                            COL11 TIME,
                            COL12 TIMESTAMP,
                            COL13 FLOAT,
                            COL14 BOOLEAN,
                            COL15 BOOLEAN,
                            CONSTRAINT CONSTR1 PRIMARY KEY (COL4, COL5),
                            CONSTRAINT CONSTR2 UNIQUE (COL7, COL8),
                            CONSTRAINT CONSTR3 UNIQUE (COL13))""")
                                
            if ExistsTable (session, 'TAB2'):
               loaderCmd(session, 'DROP TABLE TAB2')
            loaderCmd(session,"""CREATE TABLE TAB2(
                        COL1 CHAR(1000),
                        COL2 CHAR(84),
                        CONSTRAINT CONSTR1 PRIMARY KEY (COL2))""")
                
            if ExistsTable (session, 'TAB3'):
               loaderCmd(session, 'DROP TABLE TAB3')
            loaderCmd(session,"""CREATE TABLE TAB3(
                                COL1 INT,
                                COL2 INT,
                                COL3 INT,
                                COL4 INT,
                                COL5 INT,
                                COL6 INT,
                                COL7 INT,
                                COL8 INT,
                                COL9 INT,
                                COL10 INT,
                                COL11 INT,
                                COL12 INT,
                                COL13 INT,
                                COL14 INT,
                                COL15 INT,
                                COL16 INT,
                                COL17 INT,
                                COL18 INT,
                                COL19 INT,
                                COL20 INT,
                                COL21 FIXED(38,10),
                                COL22 FIXED(38,10),
                                COL23 FIXED(38,10),
                                COL24 FIXED(38,10),
                                COL25 FIXED(38,10),
                                COL26 FIXED(38,10),
                                COL27 FIXED(38,10),
                                COL28 FIXED(38,10),
                                COL29 FIXED(38,10),
                                COL30 FIXED(38,10),
                                COL31 FIXED(38,10),
                                COL32 FIXED(38,10),
                                COL33 FIXED(38,10),
                                COL34 FIXED(38,10),
                                COL35 FIXED(38,10),
                                COL36 FIXED(38,10),
                                COL37 FIXED(38,10),
                                COL38 FIXED(38,10),
                                COL39 FIXED(38,10),
                                COL40 FIXED(38,10),
                                COL41 FLOAT,
                                COL42 FLOAT,
                                COL43 FLOAT,
                                COL44 FLOAT,
                                COL45 FLOAT,
                                COL46 FLOAT,
                                COL47 FLOAT,
                                COL48 FLOAT,
                                COL49 FLOAT,
                                COL50 FLOAT,
                                COL51 FLOAT,
                                COL52 FLOAT,
                                COL53 FLOAT,
                                COL54 FLOAT,
                                COL55 FLOAT,
                                COL56 FLOAT,
                                COL57 FLOAT,
                                COL58 FLOAT,
                                COL59 FLOAT,
                                COL60 FLOAT,
                                COL61 FIXED(38,2),
                                COL62 FIXED(38,2),
                                COL63 FIXED(38,2),
                                COL64 FIXED(38,2),
                                COL65 FIXED(38,2),
                                COL66 FIXED(38,2),
                                COL67 FIXED(38,2),
                                COL68 FIXED(38,2),
                                COL69 FIXED(38,2),
                                COL70 FIXED(38,2),
                                COL71 FIXED(38,2),
                                COL72 FIXED(38,2),
                                COL73 FIXED(38,2),
                                COL74 FIXED(38,2),
                                COL75 FIXED(38,2),
                                COL76 FIXED(38,2),
                                COL77 FIXED(38,2),
                                COL78 FIXED(38,2),
                                COL79 FIXED(38,2),
                                COL80 FIXED(38,2),
                                COL81 SMALLINT,
                                COL82 SMALLINT,
                                COL83 SMALLINT,
                                COL84 SMALLINT,
                                COL85 SMALLINT,
                                COL86 SMALLINT,
                                COL87 SMALLINT,
                                COL88 SMALLINT,
                                COL89 SMALLINT,
                                COL90 SMALLINT,
                                COL91 SMALLINT,
                                COL92 SMALLINT,
                                COL93 SMALLINT,
                                COL94 SMALLINT,
                                COL95 SMALLINT,
                                COL96 SMALLINT,
                                COL97 SMALLINT,
                                COL98 SMALLINT,
                                COL99 SMALLINT,
                                COL100 SMALLINT,
                                CONSTRAINT CONSTR1 PRIMARY KEY (COL2, COL22, COL42, COL62, COL82),
                                CONSTRAINT CONSTR3  UNIQUE (COL4, COL5, COL33, COL57, COL99, COL100))""")
                
            if ExistsTable (session, 'TAB4'):
               loaderCmd(session, 'DROP TABLE TAB4')
            loaderCmd(session,"""CREATE TABLE TAB4(
                                MIGRATIONTESTMIGRATIONTEST1 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST2 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST3 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST4 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST5 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST6 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST7 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST8 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST9 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST10 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST11 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST12 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST13 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST14 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST15 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST16 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST17 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST18 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST19 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST20 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST21 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST22 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST23 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST24 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST25 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST26 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST27 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST28 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST29 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST30 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST31 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST32 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST33 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST34 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST35 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST36 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST37 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST38 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST39 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST40 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST41 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST42 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST43 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST44 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST45 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST46 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST47 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST48 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST49 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST50 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST51 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST52 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST53 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST54 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST55 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST56 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST57 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST58 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST59 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST60 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST61 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST62 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST63 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST64 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST65 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST66 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST67 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST68 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST69 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST70 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST71 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST72 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST73 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST74 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST75 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST76 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST77 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST78 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST79 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST80 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST81 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST82 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST83 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST84 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST85 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST86 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST87 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST88 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST89 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST90 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST91 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST92 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST93 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST94 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST95 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST96 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST97 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST98 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST99 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST100 CHAR(40),
                                MIGRATIONTESTMIGRATIONTEST101 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST102 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST103 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST104 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST105 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST106 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST107 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST108 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST109 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST110 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST111 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST112 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST113 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST114 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST115 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST116 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST117 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST118 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST119 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST120 CHAR(10),
                                MIGRATIONTESTMIGRATIONTEST121 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST122 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST123 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST124 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST125 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST126 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST127 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST128 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST129 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST130 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST131 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST132 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST133 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST134 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST135 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST136 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST137 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST138 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST139 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST140 VARCHAR(10),
                                MIGRATIONTESTMIGRATIONTEST141 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST142 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST143 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST144 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST145 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST146 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST147 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST148 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST149 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST150 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST151 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST152 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST153 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST154 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST155 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST156 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST157 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST158 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST159 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST160 CHAR(20),
                                MIGRATIONTESTMIGRATIONTEST161 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST162 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST163 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST164 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST165 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST166 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST167 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST168 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST169 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST170 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST171 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST172 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST173 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST174 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST175 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST176 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST177 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST178 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST179 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST180 VARCHAR(20),
                                MIGRATIONTESTMIGRATIONTEST181 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST182 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST183 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST184 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST185 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST186 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST187 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST188 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST189 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST190 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST191 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST192 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST193 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST194 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST195 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST196 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST197 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST198 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST199 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST200 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST201 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST202 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST203 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST204 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST205 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST206 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST207 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST208 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST209 CHAR(21),
                                MIGRATIONTESTMIGRATIONTEST210 CHAR(21),
                                CONSTRAINT CONSTR1 PRIMARY KEY (MIGRATIONTESTMIGRATIONTEST1, MIGRATIONTESTMIGRATIONTEST2, MIGRATIONTESTMIGRATIONTEST3, MIGRATIONTESTMIGRATIONTEST4, MIGRATIONTESTMIGRATIONTEST5))""")
                
            if ExistsTable (session, 'TAB5'):
               loaderCmd(session, 'DROP TABLE TAB5')
            loaderCmd(session,"""CREATE TABLE TAB5(
                                COL1 VARCHAR(250) ,
                                COL2 CHAR DEFAULT 'a' NOT NULL,
                                COL3 CHAR(1000) ,
                                COL4 INTEGER DEFAULT 1234567890,
                                COL5 SMALLINT DEFAULT 12345,
                                COL6 FIXED(38)    DEFAULT 12345678901234567890123456789012345678,
                                COL7 FIXED(38,38) DEFAULT .12345678901234567890123456789012345678 CHECK (COL6 > 0.1),
                                COL8 FIXED(20,10) DEFAULT 123456789.01234567891,
                                COL9 DATE DEFAULT '20030123',
                                COL10 TIME DEFAULT '00143008',
                                COL11 TIMESTAMP DEFAULT '20030123143008456234',
                                COL12 FLOAT DEFAULT +9.9999999999999999999999999999999999999E+61,
                                COL13 BOOLEAN DEFAULT TRUE,
                                COL14 BOOLEAN DEFAULT FALSE)""")
                
            if ExistsTable (session, 'TAB6'):
               loaderCmd(session, 'DROP TABLE TAB6')
            loaderCmd(session,"""CREATE TABLE TAB6(
                                COL1 CHAR(20),
                                COL2 CHAR(10),
                                COL3 FIXED(10,2),
                                COL4 CHAR(1000),
                                CONSTRAINT CONSTR1 PRIMARY KEY (COL1, COL2))""")
                
            if ExistsTable (session, 'TAB7'):
               loaderCmd(session, 'DROP TABLE TAB7')
            loaderCmd(session,"""CREATE TABLE TAB7(
                                COL1 INT,
                                COL2 CHAR(20),
                                COL3 CHAR(10),
                                COL4 CHAR(100),
                                CONSTRAINT CONSTR1 FOREIGN KEY (COL2, COL3) REFERENCES TAB6 (COL1, COL2))""")
                
            if ExistsTable (session, 'TAB8'):
               loaderCmd(session, 'DROP TABLE TAB8')
            loaderCmd(session,"""CREATE TABLE TAB8(
                                COL1 CHAR (10),
                                COL2 FIXED(10,2),
                                COL3 BOOLEAN,
                                COL4 CHAR(1000))""")
                
            if ExistsTable (session, 'TAB9'):
               loaderCmd(session, 'DROP TABLE TAB9')
            loaderCmd(session,"""CREATE TABLE TAB9(
                                COL1 INT,
                                COL2 BOOLEAN,
                                COL3 CHAR(20),
                                COL4 CHAR(10),
                                COL5 FIXED(10,2),
                                COL6 CHAR(2000) ,
                                COL7 FIXED(38,2))""")
            loaderCmd(session,"""ALTER TABLE TAB4 COLUMN MIGRATIONTESTMIGRATIONTEST190 NOT NULL""")
            loaderCmd(session,"""ALTER TABLE TAB4 COLUMN MIGRATIONTESTMIGRATIONTEST190 ADD DEFAULT 'This is default 1.'""")
            loaderCmd(session,"""ALTER TABLE TAB4 COLUMN MIGRATIONTESTMIGRATIONTEST190 ALTER DEFAULT 'This is default 2.'""")
            loaderCmd(session,"""RENAME COLUMN TAB9.COL1 TO COL1A""")
            loaderCmd(session,"""RENAME COLUMN TAB9.COL2 TO COL2A""")
            loaderCmd(session,"""RENAME TABLE TAB9 TO TAB9A""")
            loaderCmd(session,"""RENAME TABLE TAB8 TO TAB8A""")
                
            if ExistsSequence (session, 'SEQ1') :
               loaderCmd(session, 'DROP SEQUENCE SEQ1')
            loaderCmd(session, 'CREATE SEQUENCE SEQ1 INCREMENT BY   2 START WITH   2 NOMAXVALUE     MINVALUE 2 NOCYCLE NOCACHE')
            if ExistsSequence (session, 'SEQ2') :
               loaderCmd(session, 'DROP SEQUENCE SEQ2')
            loaderCmd(session, 'CREATE SEQUENCE SEQ2 INCREMENT BY -10 START WITH 100   MAXVALUE 100 MINVALUE 0   CYCLE NOCACHE')
            if ExistsSequence (session, 'SEQ3') :
               loaderCmd(session, 'DROP SEQUENCE SEQ3')
            loaderCmd(session, 'CREATE SEQUENCE  SEQ3 MINVALUE 1    MAXVALUE 100000000 START  WITH 226  INCREMENT BY   1  NOCYCLE  CACHE 1 ORDER') 
            if ExistsSequence (session, 'SEQ4') :
               loaderCmd(session, 'DROP SEQUENCE SEQ4')
            loaderCmd(session, 'CREATE SEQUENCE  SEQ4 NOMINVALUE    MAXVALUE 10        START  WITH 10   INCREMENT BY -10  NOCYCLE  NOCACHE')
            if ExistsSequence (session, 'SEQ5') :
               loaderCmd(session, 'DROP SEQUENCE SEQ5')
            loaderCmd(session, 'CREATE SEQUENCE  SEQ5 MINVALUE 100  NOMAXVALUE         START  WITH 2235 INCREMENT BY   1  NOCYCLE  CACHE 1 ORDER')
            
            '''
            loaderCmd(session, 'GRANT SELECT ON  SEQ1 TO EDEMO WITH GRANT OPTION')
            loaderCmd(session, 'GRANT SELECT ON  SEQ2 TO EDEMO WITH GRANT OPTION')
            loaderCmd(session, 'GRANT SELECT ON  SEQ3 TO EDEMO WITH GRANT OPTION')
            loaderCmd(session, 'GRANT SELECT ON  SEQ4 TO EDEMO WITH GRANT OPTION')
            loaderCmd(session, 'GRANT SELECT ON  SEQ5 TO EDEMO WITH GRANT OPTION')
            '''
              
            if ExistsDBProcedure (session, 'DO_INSERTS_1') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_1')
            loaderCmd(session,"""
            CREATE DBPROC DO_INSERTS_1
                AS VAR I INTEGER;
                    SET I = 1;
                    WHILE I <= %s DO BEGIN
                        INSERT INTO BENCHMARK.TAB1 VALUES (
                            RFILL('tab1:', 'xyz', 250),
                            'Q',
                            'ruieruixoewurxureiwourtguirguhxihihfxhahxhafzxufiauxuz',
                            :I,
                            :I + 1,
                            :I * 1000000,
                            :I / 20 - 0.01,
                            :I * 100 + 0.1,
                            RFILL('tab1:', 'abcdefghijklmnopqrstuvwxyz', 1000),
                            '20030123',
                            '00143008',
                            '20030123143008456234',
                            :I * 1.23456789,
                            FALSE,
                            TRUE);
                            IF $RC <> 0 THEN
                                        STOP ($RC, $ERRMSG);
                        SET I = I + 1;
                    END;""" % TAB1MAXROWS)
                        
            if ExistsDBProcedure (session, 'DO_INSERTS_2') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_2')

            loaderCmd(session,"""
            CREATE DBPROC DO_INSERTS_2
                AS VAR I INTEGER;
                    SET I = 1;
                    WHILE I <= %s DO BEGIN
                        INSERT INTO BENCHMARK.TAB2 VALUES (
                            RFILL('', '1234567890', 1000),
                            CHR(:I));
                            IF $RC <> 0 THEN
                                        STOP ($RC, $ERRMSG);
                        SET I = I + 1;
                    END;""" % TAB2MAXROWS)

            if ExistsDBProcedure (session, 'DO_INSERTS_3') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_3')
            loaderCmd(session,"""
            CREATE DBPROC DO_INSERTS_3
                AS VAR I INTEGER;
                SET I = 1;
                WHILE I <= %s DO BEGIN
                    INSERT INTO BENCHMARK.TAB3 VALUES (
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                        :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10);
                        IF $RC <> 0 THEN
                                    STOP ($RC, $ERRMSG);
                    SET I = I + 1;
                END;""" % TAB3MAXROWS)
                
            if ExistsDBProcedure (session, 'DO_INSERTS_4') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_4')

            loaderCmd(session,"""
            CREATE DBPROC DO_INSERTS_4_0
                AS VAR I INTEGER;
                SET I = 1;
                INSERT INTO BENCHMARK.TAB4 VALUES (
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10);
                IF $RC <> 0 THEN
                            STOP ($RC, $ERRMSG);
                SET I = 2;
                INSERT INTO BENCHMARK.TAB4 VALUES (
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10);
                IF $RC <> 0 THEN
                            STOP ($RC, $ERRMSG);
                SET I = 3;
                INSERT INTO BENCHMARK.TAB4 VALUES (
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10);
                IF $RC <> 0 THEN
                            STOP ($RC, $ERRMSG);
                SET I = 4;
                INSERT INTO BENCHMARK.TAB4 VALUES (
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                    :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10);
                IF $RC <> 0 THEN
                            STOP ($RC, $ERRMSG);
                DELETE FROM BENCHMARK.TAB4;
                IF $RC <> 0 THEN
                            STOP ($RC, $ERRMSG);""")

            if ExistsDBProcedure (session, 'DO_INSERTS_4') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_4')
            loaderCmd(session,"""CREATE DBPROC DO_INSERTS_4
                AS VAR I INTEGER;
                    SET I = 1;
                    WHILE I <= %s DO BEGIN
                        INSERT INTO BENCHMARK.TAB4 VALUES (
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10,
                            :I, :I * 2, :I * 3, :I * 4, :I * 5, :I * 6, :I * 7, :I * 8, :I * 9,  :I * 10);
                            IF $RC <> 0 THEN
                                        STOP ($RC, $ERRMSG);
                        SET I = I + 1;
                    END;""" %TAB4MAXROWS)

            if ExistsDBProcedure (session, 'DO_INSERTS_5') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_5')
            loaderCmd(session,"""
            CREATE DBPROC DO_INSERTS_5
                AS VAR I INTEGER;
                    SET I = 1;
                    WHILE I <= %s DO BEGIN
                        INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;
                            IF $RC <> 0 THEN
                                        STOP ($RC, $ERRMSG);
                        SET I = I + 1;
                    END;""" % TAB5MAXROWS)

            if ExistsDBProcedure (session, 'DO_INSERTS_6') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_6')
            loaderCmd(session,"""
            CREATE DBPROC DO_INSERTS_6
                AS VAR I INTEGER;
                    SET I = 1;
                    WHILE I <= %s DO BEGIN
                        INSERT INTO BENCHMARK.TAB6 VALUES (
                            :I,
                            :I * 2,
                            :I + .01,
                            RFILL('', '1234567890', 1000));             
                            IF $RC <> 0 THEN
                                        STOP ($RC, $ERRMSG);
                        SET I = I + 1;
                    END;""" % TAB6MAXROWS)

            if ExistsDBProcedure (session, 'DO_INSERTS_7') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_7')
            loaderCmd(session,"""
            CREATE DBPROC DO_INSERTS_7
                AS VAR I INTEGER;
                    SET I = 1;
                    WHILE I <= %s DO BEGIN
                        INSERT INTO BENCHMARK.TAB7 VALUES (
                            :I - 1,
                            :I,
                            :I * 2,
                            RFILL('', '1234567890', 100));              
                            IF $RC <> 0 THEN
                                        STOP ($RC, $ERRMSG);
                        SET I = I + 1;
                    END;""" %TAB7MAXROWS)

            if ExistsDBProcedure (session, 'DO_INSERTS_8A9A') :
               loaderCmd(session, 'DROP DBPROCEDURE DO_INSERTS_8A9A')
            loaderCmd(session,"""
            CREATE DBPROC DO_INSERTS_8A9A
                AS VAR I INTEGER;
                    SET I = 1;
                    WHILE I <= %s DO BEGIN
                        INSERT INTO BENCHMARK.TAB8A VALUES (
                            :I,
                            BENCHMARK.SEQ1.NEXTVAL,
                            TRUE,
                            RFILL('', '1234567890', 1000));             
                            IF $RC <> 0 THEN
                                        STOP ($RC, $ERRMSG);
                        INSERT INTO BENCHMARK.TAB9A VALUES (
                            :I,
                            TRUE,
                            'Hello World!',
                            'lalala',
                            BENCHMARK.SEQ1.CURRVAL,
                            'ifsodpfiosodpfosdfksoifjdsioofjsoifujdsiofh',
                            BENCHMARK.SEQ2.NEXTVAL);
                            IF $RC <> 0 THEN
                                        STOP ($RC, $ERRMSG);
                        SET I = I + 1;
                    END;""" % TAB8MAXROWS)

#            if ExistsTrigger (session, 'INS_TAB6') :
#               loaderCmd(session, 'DROP TRIGGER INS_TAB6')

            trigger = 'CREATE TRIGGER INS_TAB6 FOR TAB6 AFTER INSERT EXECUTE (\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SE COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);\
                INSERT INTO BENCHMARK.TAB5 SET COL14 = FALSE;\
                IF $RC <> 0 THEN\
                        STOP ($RC, $ERRMSG);)'
                        
            #loaderCmd(session,trigger)
            
    #DATA    
    if  options.init == 'DATA' :
        
        try:
           if options.mode == 'INTER':
              input('Import    Database       DATA')
           if options.mode == 'PARAM': 
              print 'Import    Database       DATA'
              i = 1/0
        except:
           pass
           
        loaderCmd(session, 'USE USER mona initial')
        
        loaderCmd(session, """INSERT INTO city VALUES ('12203','Albany' ,'NY')""")
        loaderCmd(session, """INSERT INTO city VALUES ('60601','Chicago','IL')""")
        loaderCmd(session, """INSERT INTO city VALUES ('60615','Chicago','IL')""")
        loaderCmd(session, """INSERT INTO city VALUES ('45211','Cincinnati','OH')""")
        loaderCmd(session, """INSERT INTO city VALUES ('33575','Clearwater','FL')""")
        loaderCmd(session, """INSERT INTO city VALUES ('75243','Dallas','TX')""")
        loaderCmd(session, """INSERT INTO city VALUES ('32018','Daytona Beach','FL')""")
        loaderCmd(session, """INSERT INTO city VALUES ('33441','Deerfield Beach','FL')""")
        loaderCmd(session, """INSERT INTO city VALUES ('48226','Detroit','MI')""")
        loaderCmd(session, """INSERT INTO city VALUES ('90029','Hollywood','CA')""")
        loaderCmd(session, """INSERT INTO city VALUES ('92714','Irvine','CA')""")
        loaderCmd(session, """INSERT INTO city VALUES ('90804','Long Beach','CA')""")
        loaderCmd(session, """INSERT INTO city VALUES ('11788','Long Island','NY')""")
        loaderCmd(session, """INSERT INTO city VALUES ('90018','Los Angeles','CA')""")
        loaderCmd(session, """INSERT INTO city VALUES ('70112','New Orleans','LA')""")
        loaderCmd(session, """INSERT INTO city VALUES ('10580','New York','NY')""")
        loaderCmd(session, """INSERT INTO city VALUES ('10019','New York','NY')""")
        loaderCmd(session, """INSERT INTO city VALUES ('92262','Palm Springs','CA')""")
        loaderCmd(session, """INSERT INTO city VALUES ('97213','Portland','OR')""")
        loaderCmd(session, """INSERT INTO city VALUES ('60018','Rosemont','IL')""")
        loaderCmd(session, """INSERT INTO city VALUES ('95054','Santa Clara','CA')""")
        loaderCmd(session, """INSERT INTO city VALUES ('20903','Silver Spring','MD')""")
        loaderCmd(session, """INSERT INTO city VALUES ('20037','Washington','DC')""")
        loaderCmd(session, """INSERT INTO city VALUES ('20005','Washington','DC')""")
        loaderCmd(session, """INSERT INTO city VALUES ('20019','Washington','DC')""")
        
        loaderCmd(session, """INSERT INTO customer VALUES (3000,'Mrs','Jenny','Porter','10580','1340 N.Ash Street, #3')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3100,'Mr','Peter','Brown','48226','1001 34th Str., APT.3')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3200,'Company',NULL,'Datasoft','90018','486 Maple Str.')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3300,'Mrs','Rose','Brian','75243','500 Yellowstone Drive, #2')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3400,'Mrs','Mary','Griffith', '20005','3401 Elder Lane')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3500,'Mr','Martin','Randolph','60615','340 MAIN STREET, #7')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3600,'Mrs','Sally','Smith','75243','250 Curtis Street')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3700,'Mr','Mike','Jackson','45211','133 BROADWAY APT. 1')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3800,'Mrs','Rita','Doe','97213','2000 Humboldt Str., #6')""")
        loaderCmd(session, """INSERT INTO customer VALUES (3900,'Mr','George','Howe','75243','111 B Parkway, #23')""")
        loaderCmd(session, """INSERT INTO customer VALUES (4000,'Mr','Frank','Miller','95054','27 5th Str., 76')""")
        loaderCmd(session, """INSERT INTO customer VALUES (4100,'Mrs','Susan','Baker','90018','200 MAIN STREET, #94')""")
        loaderCmd(session, """INSERT INTO customer VALUES (4200,'Mr','Joseph','Peters','92714','700 S. Ash Str., APT.12')""")
        loaderCmd(session, """INSERT INTO customer VALUES (4300,'Company',NULL,'TOOLware','20019','410 Mariposa Str., #10')""")
        loaderCmd(session, """INSERT INTO customer VALUES (4400,'Mr','Antony','Jenkins','20903','55 A Parkway, #15')""")
        
        loaderCmd(session, """INSERT INTO hotel VALUES (10,'Congress','20005','155 Beechwood Str.','Sports and Games
        - - - - - - - - - - - -

        solaria in separate relaxation rooms
        sauna, free of charge, individual booking if you 
        prefer to sweat alone
        a playroom for our small guests
        bicycles for our health-conscious guests
        storeroom for bicycles in the house
        various activities and events every night
        broad selection of board games available
        indoor swimming pool and fitness center


        Teaching and Learning
        - - - - - - - - - - - -

        Enjoy it. In an absolutely quiet place. The person in charge is available 
        during the whole meeting. Consistently high standards providing the ideal 
        working environment for you to have a successful meeting.
        Telefax and copy machines, even for transparencies.
        Daylit rooms creatively arranged.
        Aroma lamps with blends of fragrances to improve 
        concentration and learning capabilities.
        Music in the meeting rooms whenever you like.

        Are you curious?  We are waiting for you!
        ')""")
        loaderCmd(session, """INSERT INTO hotel VALUES (30,'Regency','20037','477 17th Avenue','Our hotel is situated in the city centre in the direct vicinity of the shopping mall.
        Our rooms offer you the maximal facilities of a modern middleclass hotel.')""")
        loaderCmd(session, """INSERT INTO hotel VALUES (20,'Long Island','11788','1499 Grove Street',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (70,'Empire State','12203','65 Yellowstone Dr.',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (80,'Midtown','10019','12 Barnard Str.',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (40,'Eight Avenue','10019','112 8th Avenue',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (50,'Lake Michigan','60601','354 OAK Terrace',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (60,'Airport','60018','650 C Parkway','Welcome in the Airport Hotel.
        - - - - - - - - - - - - - - -
         
        The Airport Hotel is situated right in the city centre.
        Our hotel has spacious, comfortable rooms at attractive prices.
        All rooms are with bath/shower, toilet, radio, colour TV, 
        video, telephone, minibar and desk.')""")

        loaderCmd(session, """INSERT INTO hotel VALUES (90,'Sunshine','33575','200 Yellowstone Dr.','The Sunshine is located in the centre of the city.
        489 rooms, suites and facilities for the disabled, with 
        superb furnishings and genuine elegance are only some of the
        features which make this hotel something special.
        Individual service ensures personal comfort for all our guests 24 hours a day -
        more than just mere luxury.

        11 function rooms accommodating up to 300 persons are 
        available for all types of events.

        For relaxation and recreation there are a swimming pool with
        whirlpool, a sauna, steam bath and solarium.
        500 parking spaces are available in the hotel garage.')""")

        loaderCmd(session, """INSERT INTO hotel VALUES (100,'Beach','32018','1980 34th Str.',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (110,'Atlantic','33441','111 78th Str.',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (120,'Long Beach','90804','35 Broadway',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (150,'Indian Horse','92262','16 MAIN STREET',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (130,'Star','90029','13 Beechwood Place',NULL)""")
        loaderCmd(session, """INSERT INTO hotel VALUES (140,'River Boat','70112','788 MAIN STREET',NULL)""")
        
        loaderCmd(session, """INSERT INTO room VALUES (10,'single',20,135.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (10,'double',45,200.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (30,'single',12,45.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (30,'double',15,80.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (20,'single',10,70.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (20,'double',13,100.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (70,'single',4,115.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (70,'double',11,180.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (80,'single',15,90.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (80,'double',19,150.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (80,'suite',5,400.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (40,'single',20,85.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (40,'double',35,140.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (50,'single',50,105.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (50,'double',230,180.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (50,'suite',12,500.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (60,'single',10,120.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (60,'double',39,200.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (60,'suite',20,500.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (90,'single',45,90.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (90,'double',145,150.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (90,'suite',60,300.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (100,'single',11,60.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (100,'double',24,100.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (110,'single',2,70.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (110,'double',10,130.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (120,'single',34,80.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (120,'double',78,140.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (120,'suite',55,350.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (150,'single',44,100.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (150,'double',115,190.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (150,'suite',6,450.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (130,'single',89,160.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (130,'double',300,270.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (130,'suite',100,700.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (140,'single',10,125.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (140,'double',9,200.00)""")
        loaderCmd(session, """INSERT INTO room VALUES (140,'suite',78,600.00)""")
        
        loaderCmd(session, """SET FORMAT ISO""")
        
        loaderCmd(session, """INSERT INTO reservation VALUES (100,3000,80,'single','2004-11-13','2004-11-15')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (110,3000,100,'double','2004-12-24','2005-01-06')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (120,3200,50,'suite','2004-11-14','2004-11-18')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (130,3900,110,'single','2005-02-01','2005-02-03')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (150,3600,70,'double','2005-03-14','2005-03-24')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (140,4300,80,'double','2004-04-12','2004-04-30')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (160,4100,70,'single','2004-04-12','2004-04-15')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (170,4400,150,'suite','2004-09-01','2004-09-03')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (180,3100,120,'double','2004-12-23','2005-01-08')""")
        loaderCmd(session, """INSERT INTO reservation VALUES (190,4300,140,'double','2004-11-14','2004-11-17')""")
        
        loaderCmd(session, """SET FORMAT INTERNAL""")
        
        loaderCmd(session, """INSERT INTO employee VALUES (10, 1, 'Mrs','Martina','Bender',  NULL)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 2, 'Mr', 'Martin', 'Bender',  NULL)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 3, 'Mrs','Claudia','Randolph',1)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 4, 'Mr', 'Mark',   'Nober',   1)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 5, 'Mrs','Phyllis','Manger',  3)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 6, 'Mrs','Francis','Nick',    2)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 7, 'Mr', 'Peter',  'Carley',  2)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 8, 'Mr' ,'Ian',    'Wolf',    NULL)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 9, 'Mr', 'Bill',   'Tucker',  8)""")
        loaderCmd(session, """INSERT INTO employee VALUES (10, 10,'Mrs','Diana',  'Corner',  1)""")

        loaderCmd(session, 'USE USER sqltravel00 initial')

        dataLoad (session, """
            DATALOAD TABLE city
            zip       1
            name      2
            state     3
            INFILE *
            COMPRESSED
            SEPARATOR ','
            DELIMITER ''""","""
12203,Albany,NY
60601,Chicago,IL
60615,Chicago,IL
45211,Cincinnati,OH
33575,Clearwater,FL
75243,Dallas,TX
32018,DaytonaBeach,FL
33441,DeerfieldBeach,FL
48226,Detroit,MI
90029,Hollywood,CA
92714,Irvine,CA
90804,LongBeach,CA
11788,LongIsland,NY
90018,LosAngeles,CA
70112,NewOrleans,LA
10580,NewYork,NY
10019,NewYork,NY
92262,PalmSprings,CA
97213,Portland,OR
60018,Rosemont,IL
95054,SantaClara,CA
20903,SilverSpring,MD
20037,Washington,DC
20005,Washington,DC
20019,Washington,DC
""")

        dataLoad (session, """
            DATALOAD TABLE customer
             cno                1
             title              2
             firstname          3 DEFAULT NULL
             name               4
             zip                5
             address            6
            INFILE *
            NULL '-'""","""
"3000","Mrs","Jenny  ","Porter  ","10580","1340 N.Ash Street, #3"
"3100","Mr ","Peter  ","Brown   ","48226","1001 34th Str., APT.3"
"3200","Company","-","Datasoft","90018","486 Maple Str."
"3300","Mrs","Rose   ","Brian   ","75243","500 Yellowstone Drive, #2"
"3400","Mrs","Mary   ","Griffith","20005","3401 Elder Lane"
"3500","Mr ","Martin ","Randolph","60615","340 MAIN STREET, #7"
"3600","Mrs","Sally  ","Smith   ","75243","250 Curtis Street"
"3700","Mr ","Michael","Jackson ","45211","133 BROADWAY APT. 1"
"3800","Mrs","Rita   ","Doe     ","97213","2000 Humboldt Str., #6"
"3900","Mr ","George ","Howe    ","75243","111 B Parkway, #23"
"4000","Mr ","Frank  ","Miller  ","95054","27 5th Str., 76"
"4100","Mrs","Susan  ","Baker   ","90018","200 MAIN STREET, #94"
"4200","Mr ","Joseph ","Peters  ","92714","700 S. Ash Str., APT. 12"
"4300","Company","-","TOOLware","20019","410 Mariposa Str., # 10"
"4400","Mr ","Antony ","Jenkins ","20903","55 A Parkway, #15"
""")

        dataLoad (session, """
            DATALOAD TABLE account
             booking_day   1
             activities    2
            INFILE *""","""
"20030901","5000.00"
"20030902","-350.00"
"20030903","-350.00"
"20030904","4500.00"
"20030905","1500.00"
"20030906","-1000.00"
"20030907","-1000.00"
"20030908","1500.00"
"20030909","1000.00"
"20030910","6000.00"
"20030911","-100.00"
"20030912","2500.00"
"20030913","350.00"
"20030914","-2000.00"
"20030915","50.00"
"20030916","100.00"
"20030917","-1250.00"
"20030918","8000.00"
"20030919","-2000.00"
"20030920","500.00"
"20030921","-1100.00"
"20030922","1000.00"
"20030923","-200.00"
"20030924","10.00"
"20030925","540.00"
"20030926","-100.00"
"20030927","100.00"
"20030928","1000.00"
""")

        loaderCmd(session, 'USE USER sqltravel10 initial')

        dataLoad (session, """
            DATALOAD TABLE hotel
             hno     1
             name    2
             zip     3
             address 4
             info    5 DEFAULT NULL
            INFILE *
            NULL '-'
            LONGFILE Info ASCII""","""
"10","Congress     ","20005","155 Beechwood Str. "," """ + CreateFileName(environment, os.path.join('SQLTRAVEL10','HOTEL','SQLTRAVEL10_HOTEL_INFO.long001')) + """
"30","Regency      ","20037","477 17th Avenue    "," """ + CreateFileName(environment, os.path.join('SQLTRAVEL10','HOTEL','SQLTRAVEL10_HOTEL_INFO.long003')) + """
"20","Long Island  ","11788","1499 Grove Street  ","-"
"70","Empire State ","12203","65 Yellowstone Dr. ","-"
"80","Midtown      ","10019","12 Barnard Str.    ","-"
"40","Eight Avenue ","10019","112 8th Avenue     ","-"
"50","Lake Michigan","60601","354 OAK Terrace    ","-"
"60","Airport      ","60018","650 C Parkway      "," """ + CreateFileName(environment, os.path.join('SQLTRAVEL10','HOTEL','SQLTRAVEL10_HOTEL_INFO.long006')) + """
"90","Sunshine     ","33575","200 Yellowstone Dr."," """ + CreateFileName(environment, os.path.join('SQLTRAVEL10','HOTEL','SQLTRAVEL10_HOTEL_INFO.long009')) + """
"100","Beach       ","32018","1980 34th Str.     ","-"
"110","Atlantic    ","33441","111 78th Str.      ","-"
"120","Long Beach  ","90804","35 Broadway        ","-"
"150","Indian Horse","92262","16 MAIN STREET     ","-"
"130","Star        ","90029","13 Beechwood Place ","-"
"140","River Boat  ","70112","788 MAIN STREET    ","-"
""")

        dataLoad (session, """
            DATALOAD TABLE room
             hno           1
             type          2
             free          3
             price         4
            INFILE *""","""
"10","single","20","135.00"
"10","double","45","200.00"
"30","single","12","45.00"
"30","double","15","80.00"
"20","single","10","70.00"
"20","double","13","100.00"
"70","single","4","115.00"
"70","double","11","180.00"
"80","single","15","90.00"
"80","double","19","150.00"
"80","suite","5","400.00"
"40","single","20","85.00"
"40","double","35","140.00"
"50","single","50","105.00"
"50","double","230","180.00"
"50","suite","12","500.00"
"60","single","10","120.00"
"60","double","39","200.00"
"60","suite","20","500.00"
"90","single","45","90.00"
"90","double","145","150.00"
"90","suite","60","300.00"
"100","single","11","60.00"
"100","double","24","100.00"
"110","single","2","70.00"
"110","double","10","130.00"
"120","single","34","80.00"
"120","double","78","140.00"
"120","suite","55","350.00"
"150","single","44","100.00"
"150","double","115","190.00"
"150","suite ","6","450.00"
"130","single","89","160.00"
"130","double","300","270.00"
"130","suite","100","700.00"
"140","single","10","125.00"
"140","double","9","200.00"
"140","suite","78","600.00"
""")

        loaderCmd(session, 'USE USER sqltravel20 initial')

        dataLoad (session, """
            DATALOAD TABLE reservation
             rno           1
             cno           2
             hno           3
             type          4
             arrival       5
             departure     6
            INFILE *
            DATE 'YYYY-MM-DD'
            NULL '-'""","""
"100","3000","80","single","2003-11-13","2003-11-15"
"110","3000","100","double","2003-12-24","2004-01-06"
"120","3200","50","suite ","2003-11-14","2003-11-18"
"130","3900","110","single","2004-02-01","2004-02-03"
"150","3600","70","double","2004-03-14","2004-03-24"
"140","4300","80","double","2003-04-12","2003-04-30"
"160","4100","70","single","2003-04-12","2003-04-15"
"170","4400","150","suite ","2003-09-01","2003-09-03"
"180","3100","120","double","2003-12-23","2004-01-08"
"190","4300","140","double","2003-11-14","2003-11-17"
""")

        loaderCmd(session, 'USE USER edemo initial')

        dataLoad (session, """
            DATALOAD TABLE CATEGORIES
            CATEGORYID      1
            CATEGORYNAME    2
            DESCRIPTION     3 DEFAULT NULL
            PICTURE         4 DEFAULT NULL
            INFILE *
            NULL '-'
            LONGFILE DESCRIPTION ASCII""","""
"1","Beverages     "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_DESCRIPTION.long001')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long001')) + """ "
"2","Condiments    "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_DESCRIPTION.long002')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long002')) + """ "
"3","Confections   "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_DESCRIPTION.long003')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long003')) + """ "
"4","Dairy Products"," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_DESCRIPTION.long004')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long004')) + """ "
"5","GrainsCereals "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_DESCRIPTION.long005')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long005')) + """ "
"6","MeatPoultry   "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_DESCRIPTION.long006')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long006')) + """ "
"7","Produce       "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_DESCRIPTION.long007')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long007')) + """ "
"8","Seafood       "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_DESCRIPTION.long008')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long008')) + """ "
""")

        dataLoad (session, """
            DATALOAD TABLE CATEGORIES_A
            CATEGORYID      1
            CATEGORYNAME    2
            DESCRIPTION     3 DEFAULT NULL
            PICTURE         4 DEFAULT NULL
            INFILE *
            NULL '-'
            LONGFILE DESCRIPTION ASCII""","""
"1","Beverages     "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES_A','EDEMO_CATEGORIES_A_DESCRIPTION.long001')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long001')) + """ "
"2","Condiments    "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES_A','EDEMO_CATEGORIES_A_DESCRIPTION.long002')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long002')) + """ "
"3","Confections   "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES_A','EDEMO_CATEGORIES_A_DESCRIPTION.long003')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long003')) + """ "
"4","Dairy Products"," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES_A','EDEMO_CATEGORIES_A_DESCRIPTION.long004')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long004')) + """ "
"5","GrainsCereals "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES_A','EDEMO_CATEGORIES_A_DESCRIPTION.long005')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long005')) + """ "
"6","MeatPoultry   "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES_A','EDEMO_CATEGORIES_A_DESCRIPTION.long006')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long006')) + """ "
"7","Produce       "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES_A','EDEMO_CATEGORIES_A_DESCRIPTION.long007')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long007')) + """ "
"8","Seafood       "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES_A','EDEMO_CATEGORIES_A_DESCRIPTION.long008')) + """ "," """ + CreateFileName(environment, os.path.join('EDEMO','CATEGORIES','EDEMO_CATEGORIES_PICTURE.long008')) + """ "
""")

        dataLoad (session, """
            DATALOAD TABLE Suppliers
            SupplierID    1
            CompanyName   2
            ContactName   3 DEFAULT NULL
            ContactTitle  4 DEFAULT NULL
            Address       5 DEFAULT NULL
            City          6 DEFAULT NULL
            Region        7 DEFAULT NULL
            PostalCode    8 DEFAULT NULL
            Country       9 DEFAULT NULL
            Phone        10 DEFAULT NULL
            Fax          11 DEFAULT NULL
            INFILE """ + CreateFileName(environment, os.path.join('EDEMO','SUPPLIERS','EDEMO_SUPPLIERS.data')))

        dataLoad (session, """
            DATALOAD TABLE Customers
            CustomerID    1  CHAR
            CompanyName   2  CHAR
            ContactName   3  CHAR   DEFAULT NULL
            ContactTitle  4  CHAR   DEFAULT NULL
            Address       5  CHAR   DEFAULT NULL
            City          6  CHAR   DEFAULT NULL
            Region        7  CHAR   DEFAULT NULL
            PostalCode    8  CHAR   DEFAULT NULL
            Country       9  CHAR   DEFAULT NULL
            Phone         10 CHAR   DEFAULT NULL
            Fax           11 CHAR   DEFAULT NULL
            INFILE """ + CreateFileName(environment, os.path.join('EDEMO','CUSTOMERS','EDEMO_CUSTOMERS.data')))

        dataLoad (session, """
            DATALOAD TABLE Employees
            EmployeeID  1 CHAR
            LastName    2 CHAR
            FirstName   3 CHAR
            Title       4 CHAR DEFAULT NULL
            BirthDate   5 CHAR DEFAULT NULL
            HireDate    6 CHAR DEFAULT NULL
            Address     7 CHAR DEFAULT NULL
            City        8 CHAR DEFAULT NULL
            Region      9 CHAR DEFAULT NULL
            PostalCode 10 CHAR DEFAULT NULL
            Country    11 CHAR DEFAULT NULL
            HomePhone  12 CHAR DEFAULT NULL
            Extension  13 CHAR DEFAULT NULL
            Photo      14 CHAR DEFAULT NULL
            Notes      15 CHAR DEFAULT NULL
            ReportsTo  16 CHAR DEFAULT NULL
            INFILE *
            DATE 'MMDDYYYY'
            LONGFILE PHOTO """ + CreateFileName(environment, os.path.join('EDEMO','EMPLOYEES','EDEMO_EMPLOYEES_PHOTO.long'))  + """
            LONGFILE NOTES """ + CreateFileName(environment, os.path.join('EDEMO','EMPLOYEES','EDEMO_EMPLOYEES_NOTES.long'))  + "ASCII","""
"1","Davolio","Nancy","Sales Representative","12081948","03291991","507 - 20th Ave. E. Apt. 2A","Seattle","WA","98122","USA","(206) 555-9857","5467","1-21526","1-175","2"
"2","Fuller","Andrew","Vice President, Sales","02191942","07121991","908 W. Capital Way","Tacoma","WA","98401","USA","(206) 555-9482","3457","21527-43052","176-622","?"
"3","Leverling","Janet","Sales Representative","08301963","02271991","722 Moss Bay Blvd.","Kirkland","WA","98033","USA","(206) 555-3412","3355","43053-64578","623-861","2"
"4","Peacock","Margaret","Sales Representative","09191937","03301992","4110 Old Redmond Rd.","Redmond","WA","98052","USA","(206) 555-8122","5176","64579-86104","862-1079","2"
"5","Buchanan","Steven","Sales Manager","03041955","09131992","14 Garrett Hill","London","?","SW1 8JR","UK","(71) 555-4848","3453","86105-107630","1080-1522","2"
"6","Suyama","Michael","Sales Representative","07021963","09131992","Coventry House Miner Rd.","London","?","EC2 7JR","UK","(71) 555-7773","428","107631-129156","1523-1841","5"
"7","King","Robert","Sales Representative","05291960","11291992","Edgeham Hollow Winchester Way","London","?","RG1 9SP","UK","(71) 555-5598","465","129157-150682","1842-2126","5"
"8","Callahan","Laura","Inside Sales Coordinator","01091958","01301993","4726 - 11th Ave. N.E.","Seattle","WA","98105","USA","(206) 555-1189","2344","150683-172208","2127-2277","2"
"9","Dodsworth","Anne","Sales Representative","01271966","10121993","7 Houndstooth Rd.","London","?","WG2 7LT","UK","(71) 555-4444","452","172209-193734","2278-2372","5"
"10","Hellstern","Albert","Business Manager","03131960","03011993","13920 S.E. 40th Street","Bellevue","WA","98006","USA","(206) 555-4869","7559","193735-215260","2373-2926","2"
"11","Smith","Tim","Mail Clerk","06061973","01151993","30301 - 166th Ave. N.E.","Kent","WA","98042","USA","(206) 555-3857","6261","215261-236786","2927-3255","3"
"12","Patterson","Caroline","Receptionist","09111972","05151993","16 Maple Lane","Auburn","WA","98002","USA","(206) 555-3487","1411","236787-258312","3256-3550","3"
"13","Brid","Justin","Marketing Director","10081962","01011994","2 impasse du Soleil","Haguenau","Bas-Rhin","67500","France","88 83 83 16","377","258313-279838","3551-4018","2"
"14","Martin","Xavier","Marketing Associate","11301960","01151994","9 place de la Liberte","Schiltigheim","Bas-Rhin","67300","France","88 62 43 53","380","279839-301364","4019-4477","13"
"15","Pereira","Laurent","Advertising Specialist","12091965","02011994","7 rue Nationale","Strasbourg","Bas-Rhin","67000","France","88 01 01 68","376","301365-322890","4478-4650","?"
""")

        dataLoad (session, """
            DATALOAD TABLE Shippers
            ShipperID   1 
            CompanyName 2 
            INFILE *""","""
"1","Speedy Express"
"2","United Package"
"3","Federal Shipping"
""")

        dataLoad (session, """
            DATALOAD TABLE Products
            ProductID        1  CHAR
            SupplierID       2  CHAR        DEFAULT NULL
            CategoryID       3  CHAR        DEFAULT NULL
            Productname      4  CHAR        DEFAULT NULL
            QuantityPerUnit  5  CHAR        DEFAULT NULL
            UnitPrice        6  CHAR        DEFAULT NULL
            UnitsInStock     7  CHAR        DEFAULT NULL
            UnitsOnOrder     8  CHAR        DEFAULT NULL
            ReorderLevel     9  CHAR        DEFAULT NULL
            Discontinued     10 CHAR        DEFAULT NULL
            INFILE  """ + CreateFileName(environment, os.path.join('EDEMO','PRODUCTS','EDEMO_PRODUCTS.data')))
            
        dataLoad (session, """
            DATALOAD TABLE Orders
            OrderID          1 CHAR
            CustomerID       2 CHAR
            EmployeeID       3 CHAR DEFAULT NULL
            Shipname         4 CHAR DEFAULT NULL
            ShipAddress      5 CHAR DEFAULT NULL
            ShipCity         6 CHAR DEFAULT NULL
            ShipRegion       7 CHAR DEFAULT NULL
            ShipPostalCode   8 CHAR DEFAULT NULL
            ShipCountry      9 CHAR DEFAULT NULL
            ShipVia         10 CHAR DEFAULT NULL
            OrderDate       11 CHAR DEFAULT NULL
            RequiredDate    12 CHAR DEFAULT NULL
            ShippedDate     13 CHAR DEFAULT NULL
            Freight         14 CHAR DEFAULT NULL
            INFILE  """ + CreateFileName(environment, os.path.join('EDEMO','ORDERS','EDEMO_ORDERS.data')) + """
            DATE 'MM/DD/YYYY'""")

        dataLoad (session, """
            DATALOAD TABLE OrderDetails
            OrderID    1 CHAR
            ProductID  2 CHAR
            UnitPrice  3 CHAR
            Quantity   4 CHAR DEFAULT NULL
            Discount   5 CHAR DEFAULT NULL
            INFILE  """ + CreateFileName(environment, os.path.join('EDEMO','ORDERDETAILS','EDEMO_ORDERDETAILS.data')))

        dataLoad (session, """
            DATALOAD TABLE Requests
            Requestname  1
            Request      2
            INFILE *
            LONGFILE Request ASCII""","""
"Order Subtotals       "," """ + CreateFileName(environment, os.path.join('EDEMO','REQUESTS','EDEMO_REQUESTS_REQUEST.long001')) + """ "
"Products on Order     "," """ + CreateFileName(environment, os.path.join('EDEMO','REQUESTS','EDEMO_REQUESTS_REQUEST.long002')) + """ "
"Daily Order Totals    "," """ + CreateFileName(environment, os.path.join('EDEMO','REQUESTS','EDEMO_REQUESTS_REQUEST.long003')) + """ "
"Product List          "," """ + CreateFileName(environment, os.path.join('EDEMO','REQUESTS','EDEMO_REQUESTS_REQUEST.long004')) + """ "
"Products and Suppliers"," """ + CreateFileName(environment, os.path.join('EDEMO','REQUESTS','EDEMO_REQUESTS_REQUEST.long005')) + """ "
"Sales for 1993        "," """ + CreateFileName(environment, os.path.join('EDEMO','REQUESTS','EDEMO_REQUESTS_REQUEST.long006')) + """ "
""")

        dataLoad (session, """
            DATALOAD TABLE Photos
            Name   1 
            Photo  2 
            INFILE *""","""
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long001')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long002')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long003')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long004')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long005')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long006')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long007')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long008')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long009')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long010')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long011')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long012')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long013')) + """ "
"Photo: R. Meinecke"," """ + CreateFileName(environment, os.path.join('EDEMO','PHOTOS','EDEMO_PHOTOS_PHOTO.long014')) + """ "
""")

#        dataLoad (session, """
#              DATALOAD TABLE NL_TEST
#              "NUMBER"      1  CHAR DEFAULT NULL
#              Description   2  CHAR DEFAULT NULL
#              Prenumber     3  CHAR DEFAULT NULL
#              Changed       TIMESTAMP
#              INFILE  """ + CreateFileName(environment, os.path.join('EDEMO','NL_TEST','EDEMO_NL_TEST.data'))) 

        if  BENCHMARK == 'true':     
            loaderCmd(session,"""USE USER BENCHMARK initial""")
            
            try:
                sqlsession = sql.connect ( 'BENCHMARK', 'initial', options.dbname, options.node ) 
            except sql.SQLError, err :
                raise sql.SQLError, err, sys.exc_traceback 
                
            insert_values  = "INSERT INTO BENCH VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
            prepare_insert = sqlsession.prepare(insert_values)
            
            insert_array = [0,0,0,0,0,0,0,0,0,0,0,0,0,'AAAAAAAA','AAAAAAAAAAAAAAAAAAAA','AAAAAAAAAAAAAAAAAAAA','AAAAAAAAAAAAAAAAAAAA','AAAAAAAAAAAAAAAAAAAA','AAAAAAAAAAAAAAAAAAAA','AAAAAAAAAAAAAAAAAAAA','AAAAAAAAAAAAAAAAAAAA','AAAAAAAAAAAAAAAAAAAA']
            seed_array   = [500000,250000,100000,40000,10000,1000,100,25,10,5,4,2] 
            seed         = 1.0
            
            elapsed_insert = 0
            start_loop = time.time()
            i = 1
            while (i <= BENCHMAXROWS ) :
                insert_array[0] = i
                j = 1
                while (j <= 12) :
                    seed = math.fmod((16807.0 * seed), 2147483647.0)
                    insert_array[j] = int(math.fmod(seed, seed_array[j-1]) + 1)
                    j = j + 1
                i = i + 1
                
                start_insert = time.time()
                prepare_insert.execute(insert_array) 
                stop_insert  = time.time()
                elapsed_insert = elapsed_insert + ((stop_insert) - (start_insert))

            stop_loop = time.time()
            elapsed = stop_loop - start_loop
            #print "\tlooping   time %.3f seconds" % (elapsed)
            #print "\tinserting time %.3f seconds" % (elapsed_insert)
            sqlsession.commit()
            sqlsession.release ()
            
            loaderCmd(session,"CALL DO_INSERTS_1")
            loaderCmd(session,"CALL DO_INSERTS_2")
            loaderCmd(session,"CALL DO_INSERTS_3")
            loaderCmd(session,"CALL DO_INSERTS_4_0")
            loaderCmd(session,"CALL DO_INSERTS_4")
            loaderCmd(session,"CALL DO_INSERTS_5")
            loaderCmd(session,"CALL DO_INSERTS_5")
            loaderCmd(session,"CALL DO_INSERTS_6")
            loaderCmd(session,"CALL DO_INSERTS_7")
            loaderCmd(session,"CALL DO_INSERTS_8A9A")

    loaderCmd(session, 'COMMIT')

def CreateFileName (environment, filename):
    TESTPATH = environment[0]
    absFname = os.path.join(TESTPATH,'sdb','loader','demo','sourcedb', filename)
    return "'" + absFname + "'"
    
def dataLoad (session, loadcmd, data = None):
    if data != None:
        # write data to tmpfile
        tmpname = tempfile.mktemp ()
        stream = open (tmpname, 'w')
        stream.write (data)
        stream.close ()
        # patch load cmd
        match = re.search ('INFILE +[*]', loadcmd)
        if match != None:
            start, end = match.span ()
            loadcmd = loadcmd [:start] + ("INFILE '%s'" % tmpname) + loadcmd [end:]
    # execute command
    loaderCmd (session, loadcmd)
    # clean up
    if data != None:
       os.unlink (tmpname)                      
    
def loaderCmd (session, cmdStr):
    try:
        result = session.cmd (cmdStr)
    except loader.LoaderError, err:
        raise loader.LoaderError, err, sys.exc_traceback
    except:
        kind, val, traceback = sys.exc_info()
        raise kind, val, traceback

def ExistsUser (session, user):
    try:
        loaderCmd (session, ("select * from users where username = '%s'") % user)
        ok = 1
    except loader.LoaderError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a Loader error
    return ok

def ExistsDomain (session, domain):
    try:
        loaderCmd (session, ("select * from domains where owner = usergroup and domainname = '%s'") % domain)
        ok = 1
    except loader.LoaderError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a Loader error
    return ok

def ExistsTable (session, table):
    try:
        loaderCmd (session, ("select * from tables where owner = usergroup and tablename = '%s'") % table)
        ok = 1
    except loader.LoaderError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a Loader error
    return ok
    
def ExistsSequence (session, sequence):
    try:
        loaderCmd (session, ("select * from sequences where sequence_name = '%s'") % sequence)
        ok = 1
    except loader.LoaderError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a Loader error
    return ok
    
def ExistsDBProcedure (session, dbprocedure) :
    try:
        loaderCmd (session, ("select * from dbprocedures where dbprocname = '%s'") % dbprocedure)
        ok = 1
    except loader.LoaderError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a Loader error
    return ok
    
def ExistsTrigger (session, trigger) :
    try:
        loaderCmd (session, ("select * from triggers where triggername = '%s'") % trigger)
        ok = 1
    except loader.LoaderError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a Loader error
    return ok
    
def connect ( node, instroot, dbname, dbaUser ):
    session = None
    if  node == '':
        node = socket.gethostname()
    try:
        session = loader.Loader()
        cmd     = 'use user %s %s serverdb %s on %s' % ( re.split(',',dbaUser) [0], re.split(',',dbaUser) [1], dbname, node )
        session.cmd (cmd)
    except loader.CommunicationError, err :
        raise loader.CommunicationError, err, sys.exc_traceback
    except loader.LoaderError, err:
        raise loader.LoaderError, err, sys.exc_traceback
    except:
        kind, val, traceback = sys.exc_info()
        raise kind, val, traceback
    
    return session
 
if __name__ == '__main__':
    optlib.optMain2 (main, _options ())
    
    
