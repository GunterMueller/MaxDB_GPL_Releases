#    coding: utf-8
#
#    module:       test_loader.py
#
#    responsible : d033893 
#
#    special area: sdb.loader
#
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

# 

try:
    #system
    import sys
    import time
    import os
    import imp
    import socket
    #shell
    import shutil
    #string
    import string
    import re
    import ConfigParser
    #sdb
    import optlib
    from   sdb    import dbm,loader,sql
    if sys.platform[:3] == 'win' : 
       import _winreg
except ImportError, err :
    print err
    print sys.path    

LOADERAPPLPATH  =  os.getcwd()
LOADERDATAPATH  =  os.getcwd()

#raw_input('')

loadersyntax = [
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                                                                                                                                     |',
'|                                        Loader Session Parameter                                                                     |',
'|                                                                                                                                     |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                                                                                                                                     |',
'|  Session Parameter      Parameter Value                                 Default Value                                               |',
'|                                                                                                                                     |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                                                                                                                                     |', 
'|  SET COMPRESSED          \'/<s>/<d>/\'                                   \  ,/"/\'                                                  |',
'|  SET BLOBMASK            \'###\'                                                                                                    |',
'|  SQLMODE                 [INTERNAL|ANSI|DB2|ORACLE]                      INTERNAL                                                   |', 
'|  SET LANGUAGE            [ENG|DEU]                                       \'ENG\'                                                    |', 
'|  SET CODETYPE            [ASCII|UCS2|UTF8]                               ASCII                                                      |',
'|  SET CODEPAGE            [http://www.unicode.org/Public/MAPPINGS/]       ISO-8859-1                                                 |',
'|  SET DATE                [INTERNAL|ISO|USA|EUR|JIS]                      ISO                                                        |',
'|  SET TIME                [INTERNAL|ISO|USA|EUR|JIS]                      ISO                                                        |',
'|  SET TIMESTAMP           [INTERNAL|ISO|USA|EUR|JIS]                      ISO                                                        |',
'|  SET DECIMAL             \'/[<t>]/<d>/\'                                 \'//./\'                                                   |',
'|  SET BOOLEAN                                                             \'TRUE/FALSE\'                                             |',
'|  SET NULL                                                                \'?\'                                                      |',
'|  SET TRANSACTION SIZE                                                    1000                                                       |',
'|  SET MAXERRORCOUNT                                                       100                                                        |',
'|  SET BACKUPTOOL          [TSM|NETWORKER|BACKINT]                         NETWORKER                                                  |', 
'|  SET ODBCTRACE           [ON|OFF]                                        OFF                                                        |',
'|                                                                                                                                     |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                                                                                                                                     |',
'|                                        Loader Transformation Parameter                                                              |',
'|                                                                                                                                     |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                    |                    |                 |                |               |                       |                |',
'| What should be     | Transformation of  |  Logging of     | Package of     | Medium of     | Format of             | Mapping of     |',
'| imported/exported ?| Models und Data    |  Transformation | Transformation | Staging Area  | Models and Data       | Char Codetypes |',
'|                    |                    |                 |                |               |                       |                |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|Catalog, Data and   |EXTRACT|LOAD        |[RESTORE]        | DB             |               |                       |                |',
'|Package of a        |CONFIGURATION       |                 |                | FILE          | DTL, XMI              | UCS2           |',
'|Database            |CATALOG             |                 |                | PIPE          | DDL, XMI              | UTF8           |',
'|                    |DATA                |                 |                | TAPE          | PAGES, RECORDS        | ASCII          |',
'|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              |                |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|Catalog, Data and   |EXTRACT|LOAD        |[RESTORE]        | USER           |               |                       |                |',
'|Package of a        |CONFIGURATION       |                 |                | FILE          | DTL, XMI              | UCS2           |',
'|Database User       |CATALOG             |                 |                | PIPE          | DDL, XMI              | UTF8           |',
'|                    |DATA                |                 |                | TAPE          | PAGES, RECORDS        | ASCII          |',
'|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              |                |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|Catalog, Data and   |EXTRACT|LOAD[FAST]| |[RESTORE]        | TABLE          |               |                       |                |',
'|Package of a        |CONFIGURATION       |                 |                | FILE          | DTL, XMI              | UCS2           |',
'|Database Table      |CATALOG             |                 |                | PIPE          | DDL, XMI              | UTF8           |',
'|                    |DATA                |                 |                | TAPE          | PAGES, RECORDS,       | ASCII          |',
'|                    |                    |                 |                |               | FORMATTED, COMPRESSED |                |',
'|                    |                    |                 |                |               | COMPRESSED            |                |',
'|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              |                |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|Data and Package    |EXTRACT|LOAD[FAST]  |                 | COLUMNS FIELDS |               |                       |                |',
'|of a qualified      |                    |                 |                | FILE          |                       | UCS2           |',
'|Database Columnset  |                    |                 |                | PIPE          |                       | UTF8           |',
'|                    |DATA                |                 |                | TAPE          | FORMATTED, COMPRESSED | ASCII          |',
'|                    |PACKAGE             |[RESTART]        |                |               | DTL, XMI              |                |',
'|                    |                    |                 |                |               |                       |                |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                                                                                                                                     |',
'|                                        Loader Migration Parameter                                                                   |',
'|                                                                                                                                     |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                                                                                                                                     |',
'| Option     Transformation Parameter                 Parameter Value                 Default Value             Varname               |',
'|                                                                                                                                     |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                                                                                                                                     |',
'| t          Type      of Transformation              TRANSPORT, EXPORT, IMPORT       TRANSPORT                 transformationtype    |',
'| p          Package   of Transformation              DB  , USER, TABLE               DB                        transformationpackage |',
'| P          Part      of Package                     DB  , USER, TABLE               DB                        transformationpart    |',
'| m          Medium    of Staging Area                PIPE, FILE, TAPE                FILE                      streammedium          |',
'| f          Format    of Models and Data             DDL, XML                        DDL,RECORDS               streamformat          |',
'|                                                     RECORDS, PAGES,                                                                 |',
'|                                                     COMPRESSED, FORMATTED                                                           |',
'| F                                                   PAGE , ROW                      ROW                       streamformat_target   |',
'| e          Mapping of Char Codetypes                ASCII, UCS2, UTF8, DYNAMIC      DYNAMIC                   streamcharencoding    |',
'| l          Mask    of LOB  Datatypes                COLUMN, COLUMNVALUE             COLUMN                    streamblob            |',
'| i          Logging of Transformation                START, RESTART                  START                     startposition         |',
'|                                                                                                                                     |',
'|                                                                                                                                     |',
'|                                                                                                                                     |',
'| nSRC       specifies source servernode                                              localhost                 sourcenode            |',
'| rSRC       specifies source instroot                                                os.environ[INSTROOT]      sourceinstroot        |',
'| iSRC       init      source database                SYSTEM,USER,CATALOG,DELETE      data                      sourceinit            |',
'| dSRC       specifies source database to create                                      sourcedb                  sourcedbname          |',
'| eSRC       specifies source catalog coding          ASCII, UNICODE                  ascii                     sourcecatalog         |',
'|                                                                                                                                     |',
'| nTRG       specifies target servernode                                              localhost                 targetnode            |',
'| rTRG       specifies target instroot                                                os.environ[INSTROOT]      targetinstroot        |',
'| iTRG       init      target database                SYSTEM,USER,CATALOG,DELETE      system                    targetinit            |',
'| dTRG       specifies target database to create                                      targetdb                  targetdbname          |',
'| eTRG       specifies target catalog coding          ASCII, UNICODE                  ascii                     targetcatalog         |',
'|                                                                                                                                     |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
'|                                                                                                                                     |',
'|                                        Loader Migration Parameters                                                                  |',
'|                                                                                                                                     |',
'|-------------------------------------------------------------------------------------------------------------------------------------|',
]

#package webdav : 1,2,3,4,5,
#medium  file   : 6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
#medium  pipe   : 58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,
#medium  tape   : 83,84,85 

parameters = [
#|Testcase          |Transformation                                |Datastream                                                      |Database                                                                                                            |Testenvironment                 
                                                                                                                                                                                                                                                                                                     
#|Test  |Type       |Package     |Part      |Streams               |Startpos |Medium |Format       Import |Encoding   |Lobmask      |Sourcedb                                               |Targetdb                                                    |Encoding                   |Validation            | Initialization            |Version
#|1     |2          |3           |4         |5                     |6        |7      |8            |9     |10         |11           |12             |13            |14            |15       |16              |17            |18           |19            |20           |21           |22       |23          |24           |25           |26                  |27 
																																																																				  
#data                                                            usecases:																																
'-T  1 -t TRANSPORT -p WEBDAV: 1 -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f PAGES      -F PAGE -c DYNAMIC -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v data    -V complete -iSRC data    -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T  2 -t TRANSPORT -p WEBDAV: 2 -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v data    -V complete -iSRC delete  -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T  3 -t TRANSPORT -p WEBDAV: 3 -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c UCS2    -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v data    -V complete -iSRC delete  -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T  4 -t TRANSPORT -p WEBDAV: 4 -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F ROW  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v data    -V complete -iSRC delete  -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T  5 -t TRANSPORT -p WEBDAV: 5 -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v data    -V complete -iSRC delete  -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
#metadata                                                        usecases : ascii2ascii																																 	 				 																							
'-T  6 -t TRANSPORT -p DB: 6     -P DB      -s CATALOG              -i START -m FILE -f DDL        -F DDL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V complete -iSRC catalog -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T  7 -t EXPORT    -p DB: 7     -P DB      -s CATALOG              -i START -m FILE -f XML        -F XML  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T  8 -t TRANSPORT -p DB: 8     -P USER    -s CATALOG              -i START -m FILE -f DDL        -F DDL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T  9 -t EXPORT    -p DB: 9     -P USER    -s CATALOG              -i START -m FILE -f XML        -F XML  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 10 -t TRANSPORT -p DB:10     -P TABLE   -s CATALOG              -i START -m FILE -f DDL        -F DDL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 11 -t EXPORT    -p DB:11     -P TABLE   -s CATALOG              -i START -m FILE -f XML        -F XML  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 12 -t TRANSPORT -p DB:12     -P TABLE   -s PACKAGE              -i START -m FILE -f PAGES      -F DTL  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 13 -t TRANSPORT -p DB:13     -P TABLE   -s PACKAGE              -i START -m FILE -f RECORDS    -F DTL  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 14 -t EXPORT    -p DB:14     -P TABLE   -s PACKAGE              -i START -m FILE -f FORMATTED  -F DTL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 15 -t EXPORT    -p DB:15     -P TABLE   -s PACKAGE              -i START -m FILE -f COMPRESSED -F DTL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
#data																																																																									 	 																								
'-T 16 -t TRANSPORT -p DB:16     -P DB      -s CATALOG,DATA,PACKAGE -i START -m FILE -f PAGES      -F PAGE -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC data    -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 17 -t TRANSPORT -p DB:17     -P DB      -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC data    -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 18 -t TRANSPORT -p DB:18     -P USER    -s CATALOG,DATA,PACKAGE -i START -m FILE -f PAGES      -F PAGE -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v data    -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 19 -t TRANSPORT -p DB:19     -P USER    -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v data    -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 20 -t TRANSPORT -p DB:20     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f PAGES      -F PAGE -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 21 -t TRANSPORT -p DB:21     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 22 -t TRANSPORT -p DB:22     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F ROW  -c ASCII   -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 23 -t TRANSPORT -p DB:23     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 24 -t TRANSPORT -p DB:24     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F ROW  -c ASCII   -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 25 -t TRANSPORT -p DB:25     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F PAGE -c ASCII   -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
#metadata                                                         usecases : ascii2unicode																															 	 				 																						
'-T 26 -t TRANSPORT -p DB:26     -P DB      -s CATALOG              -i START -m FILE -f DDL        -F DDL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v catalog -V complete -iSRC catalog -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 27 -t TRANSPORT -p DB:27     -P TABLE   -s PACKAGE              -i START -m FILE -f FORMATTED  -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 28 -t TRANSPORT -p DB:28     -P TABLE   -s PACKAGE              -i START -m FILE -f COMPRESSED -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
#data																																																																									 	 																									
'-T 29 -t TRANSPORT -p DB:29     -P DB      -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC data    -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 30 -t TRANSPORT -p DB:30     -P USER    -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 31 -t TRANSPORT -p DB:31     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 32 -t TRANSPORT -p DB:32     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F ROW  -c ASCII   -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 33 -t TRANSPORT -p DB:33     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 34 -t TRANSPORT -p DB:34     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F ROW  -c ASCII   -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 35 -t TRANSPORT -p DB:35     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 36 -t TRANSPORT -p DB:36     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F ROW  -c UTF8    -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 37 -t TRANSPORT -p DB:37     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F PAGE -c UTF8    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 38 -t TRANSPORT -p DB:38     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F ROW  -c UTF8    -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 39 -t TRANSPORT -p DB:39     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F PAGE -c UTF8    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
#metadata                                                         usecases : unicode2unicode																															 	 			 	 																						
'-T 40 -t TRANSPORT -p DB:40     -P DB      -s CATALOG              -i START -m FILE -f DDL        -F DDL  -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v catalog -V complete -iSRC catalog -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 41 -t TRANSPORT -p DB:41     -P TABLE   -s PACKAGE              -i START -m FILE -f PAGES      -F PAGE -c DYNAMIC -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v catalog -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 42 -t TRANSPORT -p DB:42     -P TABLE   -s PACKAGE              -i START -m FILE -f RECORDS    -F ROW  -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v catalog -V complete -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 43 -t TRANSPORT -p DB:43     -P TABLE   -s PACKAGE              -i START -m FILE -f FORMATTED  -F PAGE -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v catalog -V none     -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 44 -t TRANSPORT -p DB:44     -P TABLE   -s PACKAGE              -i START -m FILE -f COMPRESSED -F PAGE -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v catalog -V complete -iSRC delete  -iTRG delete  -rSRC sourceinstroot -rTRG targetinstroot',
#data																																																																								 	 																							
'-T 45 -t TRANSPORT -p DB:45     -P DB      -s CATALOG,DATA,PACKAGE -i START -m FILE -f PAGES      -F PAGE -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC data    -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 46 -t TRANSPORT -p DB:46     -P DB      -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c UCS2    -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 47 -t TRANSPORT -p DB:47     -P USER    -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c UCS2    -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 48 -t TRANSPORT -p DB:48     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f PAGES      -F PAGE -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 49 -t TRANSPORT -p DB:49     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f RECORDS    -F ROW  -c UCS2    -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 50 -t TRANSPORT -p DB:50     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F ROW  -c UCS2    -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 51 -t TRANSPORT -p DB:51     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F PAGE -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 52 -t TRANSPORT -p DB:52     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F ROW  -c UCS2    -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 53 -t TRANSPORT -p DB:53     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F PAGE -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 54 -t TRANSPORT -p DB:54     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F ROW  -c UTF8    -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 55 -t TRANSPORT -p DB:55     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f FORMATTED  -F PAGE -c UTF8    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 56 -t TRANSPORT -p DB:56     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F ROW  -c UTF8    -l COLUMNVALUE -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 57 -t TRANSPORT -p DB:57     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m FILE -f COMPRESSED -F PAGE -c UTF8    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
#metadata                                                         usecases : ascii2ascii																																 	 			 	 																							
'-T 58 -t TRANSPORT -p DB:58     -P DB      -s CATALOG              -i START -m PIPE -f DDL        -F DDL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V complete -iSRC catalog -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
#data                                                                																																					 	 			 	 																						
'-T 59 -t TRANSPORT -p DB:59     -P DB      -s CATALOG,DATA,PACKAGE -i START -m PIPE -f PAGES      -F PAGE -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC data    -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 60 -t TRANSPORT -p DB:60     -P DB      -s CATALOG,DATA,PACKAGE -i START -m PIPE -f RECORDS    -F ROW  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 61 -t TRANSPORT -p DB:61     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f RECORDS    -F ROW  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 62 -t TRANSPORT -p DB:62     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f FORMATTED  -F ROW  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 63 -t TRANSPORT -p DB:63     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f FORMATTED  -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 64 -t TRANSPORT -p DB:64     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f COMPRESSED -F ROW  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 65 -t TRANSPORT -p DB:65     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f COMPRESSED -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
#metadata                                                         usecases : ascii2unicode																															 	 				 																							
'-T 66 -t TRANSPORT -p DB:66     -P DB      -s CATALOG              -i START -m PIPE -f DDL        -F DDL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v catalog -V complete -iSRC catalog -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
#data                                                               																																					 	 			 	 																								
'-T 67 -t TRANSPORT -p DB:67     -P DB      -s CATALOG,DATA,PACKAGE -i START -m PIPE -f RECORDS    -F ROW  -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC data    -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 68 -t TRANSPORT -p DB:68     -P USER    -s CATALOG,DATA,PACKAGE -i START -m PIPE -f RECORDS    -F ROW  -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 69 -t TRANSPORT -p DB:69     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f RECORDS    -F ROW  -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 70 -t TRANSPORT -p DB:70     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f FORMATTED  -F ROW  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 71 -t TRANSPORT -p DB:71     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f FORMATTED  -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V none     -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 72 -t TRANSPORT -p DB:72     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f COMPRESSED -F ROW  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 73 -t TRANSPORT -p DB:73     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f COMPRESSED -F PAGE -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG unicode -v all     -V none     -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
#metadata                                                        usecases : unicode2unicode																															 	 				 																								
'-T 74 -t TRANSPORT -p DB:74     -P DB      -s CATALOG              -i START -m PIPE -f DDL        -F DDL  -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v catalog -V complete -iSRC catalog -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
#data                                                               																																					 	 			 	 																							
'-T 75 -t TRANSPORT -p DB:75     -P DB      -s CATALOG,DATA,PACKAGE -i START -m PIPE -f PAGES      -F PAGE -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC data    -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 76 -t TRANSPORT -p DB:76     -P DB      -s CATALOG,DATA,PACKAGE -i START -m PIPE -f RECORDS    -F ROW  -c UCS2    -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 77 -t TRANSPORT -p DB:77     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f RECORDS    -F ROW  -c UCS2    -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 78 -t TRANSPORT -p DB:78     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f FORMATTED  -F ROW  -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 79 -t TRANSPORT -p DB:79     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f FORMATTED  -F PAGE -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V none     -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 80 -t TRANSPORT -p DB:80     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f COMPRESSED -F ROW  -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 81 -t TRANSPORT -p DB:81     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f COMPRESSED -F PAGE -c UCS2    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V none     -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
'-T 82 -t TRANSPORT -p DB:82     -P TABLE   -s CATALOG,DATA,PACKAGE -i START -m PIPE -f COMPRESSED -F ROW  -c UTF8    -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC unicode -eTRG unicode -v all     -V complete -iSRC delete  -iTRG catalog -rSRC sourceinstroot -rTRG targetinstroot',
#metadata                                                        usecases: ascii2ascii																																 	 				 																								
'-T 83 -t TRANSPORT -p DB:83     -P DB      -s CATALOG              -i START -m TAPE -f DDL        -F DDL  -c ASCII   -l COLUMN      -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v catalog -V complete -iSRC catalog -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
#data                                                                																																					 	 			 	 																							
'-T 84 -t TRANSPORT -p DB:84     -P DB      -s CATALOG,DATA,PACKAGE -i START -m TAPE -f PAGES      -F PAGE -c DYNAMIC -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V complete -iSRC data    -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot',
'-T 85 -t TRANSPORT -p DB:85     -P DB      -s CATALOG,DATA,PACKAGE -i START -m TAPE -f RECORDS    -F ROW  -c ASCII   -l TABLE       -nSRC LOCALHOST -dSRC SOURCEDB -uSRC DBA,DBA -sSRC DBA -nTRG LOCALHOST -dTRG TARGETDB -uTRG DBA,DBA -sTRG DBA       -eSRC ascii   -eTRG ascii   -v all     -V sorted   -iSRC delete  -iTRG system  -rSRC sourceinstroot -rTRG targetinstroot' 
]																																												 													

def _options ():																																									
    return [
    #testcase
    ('T', 'testcase'               , ':', '16'                  , 'specifies use   case'  ),   
    ('mode', 'mode'                , ':', 'PARAM'               , 'specifies use   mode'  ),                    # PARAM,INTER,DEBUG
    ('v', 'validation'             , ':', ''                    , 'specifies validation scope'),                # 
    ('V', 'validationmode'         , ':', ''                    , 'specifies validation type' ),                # 
    #transport.par 
    #loader.cfg
    ('t', 'transformationtype'     , ':', ''                    , 'type      of transformation '),              # TRANSPORT, EXPORT, IMPORT
    ('p', 'transformationpackage'  , ':', ''                    , 'package   of transformation '),              # DB   , USER, TABLE, WEBDAV
    ('P', 'transformationpart'     , ':', ''                    , 'part      of transformation '),              # DB   , USER, TABLE, WEBDAV
    ('s', 'transformationstreams'  , ':', ''                    , 'streams   of transformation '),              # CATALOG,DATA,PACKAGE
    ('i', 'startposition'          , ':', ''                    , 'startposition transformation '),             # START , RESTART
    ('m', 'streammedium'           , ':', ''                    , 'streammedium of transformation '),           # PIPE , FILE, TAPE
    ('f', 'streamformat'           , ':', ''                    , 'streamformat of transformation '),           # RECORDS, PAGES, COMPRESSED, FORMATTED, FORMATTED BINARY, DDL, XML
    ('F', 'streamformat_target'    , ':', ''                    , 'streamformat of transformation '),           # DATA  , FAST
    ('c', 'streamcharencoding'     , ':', ''                    , 'streamcoding of transformation '),           # ASCII , UCS2, UTF8
    ('l', 'streamlob'              , ':', ''                    , 'lobmask      of transformation '),           # COLUMN, COLUMNVALUE
    ('L', 'loaderinstroot'         , ':', ''                    , 'specifies sdb  loader  independent path'),   # 
    #transport.cfg
    ('iSRC', 'sourceinit'          , ':', ''                    , 'init      sdb  source'),                     # DELETE, SYSTEM, USER, CATALOG, DATA 
    ('eSRC', 'sourcecharencoding'  , ':', ''                    , 'specifies sdb  source char encoding '),      # ASCII , UNICODE
    ('nSRC', 'sourcenode'          , ':', socket.gethostname()  , 'specifies source servernode'),
    ('rSRC', 'sourceinstroot'      , ':', ''                    , 'specifies sdb  source  instroot'), 
    ('jSRC', 'sourceindepdatapath' , ':', ''                    , 'specifies sdb  source  independent data path'), 
    ('kSRC', 'sourceindepprogpath' , ':', ''                    , 'specifies sdb  source  independent prog path'), 
    ('dSRC', 'sourcedbname'        , ':', ''                    , 'specifies source db to create'),
    ('uSRC', 'sourcedbuser'        , ':', ''                    , 'specifies source user'),
    ('sSRC', 'sourcedbschema'        , ':', ''                  , 'specifies source schema'),
    
    ('iTRG', 'targetinit'          , ':', ''                    , 'init      sdb  target'),                     # DELETE, SYSTEM, USER, CATALOG, DATA
    ('eTRG', 'targetcharencoding'  , ':', ''                    , 'specifies sdb  target char encoding '),      # ASCII , UNICODE
    ('nTRG', 'targetnode'          , ':', socket.gethostname()  , 'specifies target servernode '),
    ('rTRG', 'targetinstroot'      , ':', ''                    , 'specifies sdb  target  instroot'), 
    ('jTRG', 'targetindepdatapath' , ':', ''                    , 'specifies sdb  target  independent data path'), 
    ('kTRG', 'targetindepprogpath' , ':', ''                    , 'specifies sdb  target  independent prog path'), 
    ('dTRG', 'targetdbname'        , ':', ''                    , 'specifies target db to create'),
    ('uTRG', 'targetdbuser'        , ':', ''                    , 'specifies target user'),
    ('sTRG', 'targetdbschema'      , ':', ''                    , 'specifies target schema'),
] 

TESTPATH        = ''
LOADERPROGPATH  = ''
INDEPPROGPATH   = ''
DEPPROGPATH     = ''

def main (options, args):
    rc = 0
    print "\nLoader Test Module:\n'%s'" % os.path.abspath(sys.argv[0])
    environment = []
    os.putenv('TESTLOADER','TRUE')
    getEnvironment (options,environment)
    testcase = options.testcase
    if testcase == '0':
       #transformation resources
       options.sourceinit   = 'data'
       options.sourcedbname = 'SOURCEDB'
       options.targetinit   = 'catalog'
       options.targetdbname = 'TARGETDB'
       init_transformation_resources  (options,environment)
       #deinit_transformation_resources(options) 
    if testcase != '0':   
       for case in re.split(',',testcase):
           options.testcase = string.atoi(case,10)
           #source db
           if  check_options(options) == 0 :
               #execute use case
               '''
               print '\n# %i: EXECUTE TESTCASE \n ' % (options.testcase)
               #transformation attributes
               print '-T %s  '  % 'EXECUTE'
               print '-t %s  '  % options.transformationtype
               print '-p %s  '  % options.transformationpackage
               print '-P %s  '  % options.transformationpart
               print '-s %s  '  % options.transformationstreams
               print '-i %s  '  % options.startposition
               print '-m %s  '  % options.streammedium
               print '-f %s  '  % options.streamformat
               print '-F %s  '  % options.streamformat_target
               print '-c %s  '  % options.streamcharencoding 
               print '-l %s\n'  % options.streamlob
               '''
               #transformation resources
               init_transformation_resources(options,environment)
               #transport sourcedb to targetdb
               #     |Transformation datastream                                        |Source database                   |Target database                         
               if sys.platform[:3] == 'win':
                  cmd = '"%s" -T %s -t %s -p %s -P %s -s %s -m %s -f %s -F %s -c %s -l %s -nSRC %s -dSRC %s -uSRC %s -sSRC %s -nTRG %s -dTRG %s -uTRG %s -sTRG %s' % ( 
                  os.path.join( INDEPPROGPATH ,'bin','loadercli.exe'),
                  'EXECUTE', options.transformationtype,options.transformationpackage,options.transformationpart, options.transformationstreams,
                  options.streammedium, options.streamformat, options.streamformat_target, options.streamcharencoding, options.streamlob, 
                  options.sourcenode, options.sourcedbname, options.sourcedbuser, options.sourcedbschema, 
                  options.targetnode, options.targetdbname, options.targetdbuser, options.targetdbschema )
               if sys.platform[:3] != 'win':
                  cmd = '"%s" -T %s -t %s -p %s -P %s -s %s -m %s -f %s -F %s -c %s -l %s -nSRC %s -dSRC %s -uSRC %s -sSRC %s -nTRG %s -dTRG %s -uTRG %s -sTRG %s' % ( 
                  os.path.join( INDEPPROGPATH ,'bin','loadercli'),
                  'EXECUTE', options.transformationtype,options.transformationpackage,options.transformationpart, options.transformationstreams,
                  options.streammedium, options.streamformat, options.streamformat_target, options.streamcharencoding, options.streamlob, 
                  options.sourcenode, options.sourcedbname, options.sourcedbuser, options.sourcedbschema, 
                  options.targetnode, options.targetdbname, options.targetdbuser, options.targetdbschema )
               print '\nLoader Transport Module:\n %s ' % cmd
               try:
                  rc = os.system (cmd)
               except OSError, err:
                  print 'command failed:', err
               #transformation resources   
               deinit_transformation_resources(options) 
               
#environment
def getEnvironment(options,environment):
    global TESTPATH
    global LOADERPROGPATH
    global INDEPPROGPATH
    global DEPPROGPATH
    session = None
    
    # TESTPATH
    TESTPATH       = os.path.split(os.environ['JTEST_ROOT'])[0] 
    environment.append(TESTPATH)
    
    # LOADERPROGPATH
    if sys.platform[:3] == 'win' : 
       try:
           import _winreg
           LOADERPROGPATH = _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"APPDATA")[0]
       except ImportError, err :
           print err
           print sys.path
    if sys.platform[:3] != 'win' :
       LOADERPROGPATH = os.getenv('HOME')
    try:
        # INDEPPROGPATH
        session  = dbm.DBM()
        indepprogpath  = session.cmd('dbm_getpath INDEPPROGPATH ')
        INDEPPROGPATH  = re.split('\n',indepprogpath)[0]
        
        # DEPPROGPATH
        if options.targetinstroot not in ['','targetinstroot']: 
           DEPPROGPATH = options.targetinstroot
        #transport.cfg
        elif( options.targetinstroot in ['','targetinstroot'] ):
            if os.path.isfile(os.path.join(LOADERPROGPATH,'sdb','loader','config','transport.cfg')):
               try:
                   os.unlink (os.path.join(LOADERPROGPATH,'sdb','loader','config','transport.cfg'))
               except OSError, err:
                   print 'os.unlink failed: %s ' %( err )
               packages = []
               for package in re.split('\n'  ,session.cmd('inst_enum')):
                   packages.append(package)
               shutil.copy            ( os.path.join(TESTPATH,      'sdb','loader','test'  ,'transport.cfg'), 
                                        os.path.join(LOADERPROGPATH,'sdb','loader','config','transport.cfg'))
               TRANSPORTCONFIGFILE    = os.path.join(LOADERPROGPATH,'sdb','loader','config','transport.cfg')
               TRANSPORTCONFIGURATION = ConfigParser.ConfigParser()
               configfile             = file(TRANSPORTCONFIGFILE,'r')
               TRANSPORTCONFIGURATION.readfp(configfile)
               INSTROOT_SOURCE     = TRANSPORTCONFIGURATION.get('SOURCEDB','VERSION')
               INSTROOT_TARGET     = TRANSPORTCONFIGURATION.get('TARGETDB','VERSION')   
               configfile.close()
               index   = re.split(' ',INSTROOT_SOURCE)[1]
               index   = string.replace(index,' ','')  
               index   = string.replace(index,'[','')  
               index   = string.replace(index,']','')  
               INSTROOT_SOURCE = string.replace(INSTROOT_SOURCE,' '             ,'')  
               INSTROOT_SOURCE = string.replace(INSTROOT_SOURCE,'['+ index + ']','')  
               INSTROOT_SOURCE = re.split(','  ,INSTROOT_SOURCE)[string.atoi(index,10)]
               index   = re.split(' ',INSTROOT_TARGET)[1]
               index   = string.replace(index,' ','')  
               index   = string.replace(index,'[','')  
               index   = string.replace(index,']','')  
               INSTROOT_TARGET = string.replace(INSTROOT_TARGET,' '             ,'')  
               INSTROOT_TARGET = string.replace(INSTROOT_TARGET,'['+ index + ']','')  
               INSTROOT_TARGET = re.split(','  ,INSTROOT_TARGET)[string.atoi(index,10)]
               for package in packages:
                   if package[0:9]   == INSTROOT_SOURCE:
                      options.sourceinstroot  =  re.split('    ', package )[1]
               for package in packages:
                   if package[0:9]   == INSTROOT_TARGET:
                      options.targetinstroot  =  re.split('    ', package )[1]
               DEPPROGPATH =  options.targetinstroot
    except:
        print 'test'

def init_transformation_resources (options,environment):
    init_transformation_databases (options,environment)
    init_transformation_files     (options)
    
def deinit_transformation_resources(options):
    
    deinit_transformation_files    (options)  

    #transformation attributes
    options.transformationtype    = ''
    options.transformationpackage = ''
    options.transformationpart    = ''
    options.streammedium          = ''   
    options.streamformat          = ''    
    options.streamformat_target   = ''    
    options.streamcharencoding    = ''    
    options.streamlob             = ''    
    options.startposition         = ''    

    #sourcedb
    options.sourceinit            = ''
    options.sourcecharencoding    = ''   
    options.sourcenode            = ''  
    options.sourcedbname          = ''
    #targetdb   
    options.targetinit            = '' 
    options.targetcharencoding    = ''
    options.targetnode            = ''
    options.targetdbname          = ''          
    
def init_transformation_databases (options,environment):
    input = None
    rc    = 0
    TESTPATH = environment[0]
    while input not in ["y","n"]:   
          if options.mode == 'INTER':
             input = raw_input('Init      Transformation Databases ? [y]/[n] : ')
          if options.mode == 'PARAM':
             print             'Init      Transformation Databases '
             input  = "y"
          if input == "n":
             try:
                 session = dbm.DBM ('',options.sourcedbname, options.sourceinstroot, 'DBM,DBM' )
                 sourceindepdatapath = session.cmd('dbm_getpath INDEPDATAPATH ')
                 sourceindepprogpath = session.cmd('dbm_getpath INDEPPROGPATH ')
                 options.sourceindepdatapath = re.split('\n',sourceindepdatapath)[0]
                 options.sourceindepprogpath = re.split('\n',sourceindepprogpath)[0]
             except dbm.CommunicationError, err :
                 raise dbm.CommunicationError, err, sys.exc_traceback
             except dbm.DBMServError, err:
                 raise dbm.DBMServError, err , sys.exc_traceback 
             except:
                  kind, val, traceback = sys.exc_info()
                  raise kind, val, traceback
             try:
                 session = dbm.DBM ('',options.targetdbname, options.targetinstroot, 'DBM,DBM' )
                 targetindepdatapath = session.cmd('dbm_getpath INDEPDATAPATH ')
                 targetindepprogpath = session.cmd('dbm_getpath INDEPPROGPATH ')
                 options.targetindepdatapath = re.split('\n',targetindepdatapath)[0]
                 options.targetindepprogpath = re.split('\n',targetindepprogpath)[0]
             except dbm.CommunicationError, err :
                 raise dbm.CommunicationError, err, sys.exc_traceback
             except dbm.DBMServError, err:
                 raise dbm.DBMServError, err , sys.exc_traceback 
             except:
                  kind, val, traceback = sys.exc_info()
                  raise kind, val, traceback
          if input == "y":
             #sourcedb
             if options.sourceinit <> 'DELETE' and options.sourcedbname == 'SOURCEDB' :
                print 'Install   Databasesystem %s ' % options.sourcedbname
                cmd = 'x_python "%s" -mode %s -r "%s" -d %s -e %s '         % (os.path.join (TESTPATH,'sdb','dbm','demo','installdb.py' ) , options.mode ,options.sourceinstroot,options.sourcedbname,options.sourcecharencoding)
                try:
                    rc = os.system (cmd)
                except OSError, err:
                    print 'command failed:', err
             try:
                 session = dbm.DBM ('',options.sourcedbname, options.sourceinstroot, 'DBM,DBM' )
                 sourceindepdatapath = session.cmd('dbm_getpath INDEPDATAPATH ')
                 sourceindepprogpath = session.cmd('dbm_getpath INDEPPROGPATH ')
                 options.sourceindepdatapath = re.split('\n',sourceindepdatapath)[0]
                 options.sourceindepprogpath = re.split('\n',sourceindepprogpath)[0]
             except dbm.CommunicationError, err :
                 raise dbm.CommunicationError, err, sys.exc_traceback
             except dbm.DBMServError, err:
                 raise dbm.DBMServError, err , sys.exc_traceback 
             except:
                  kind, val, traceback = sys.exc_info()
                  raise kind, val, traceback
             #db
             if options.transformationpackage <> 'WEBDAV'  :          
                print 'Import    Database       %s ' % options.sourcedbname
                cmd = 'x_python "%s" -mode %s -r "%s" -d %s -i %s'           % (os.path.join (TESTPATH,'sdb','loader','demo','import.py' ), options.mode,DEPPROGPATH ,options.sourcedbname,options.sourceinit)
                try:
                    rc = os.system (cmd)
                except OSError, err:
                    print 'command failed:', err    
             #dav
             if options.transformationpackage == 'WEBDAV'  :    
                print 'Import    DAV Database   %s ' % options.sourcedbname
                cmd = 'x_python "%s" -mode %s -r "%s" -d %s -i %s -h %s:85'  % (os.path.join (TESTPATH,'sdb','dav'   ,'demo','import.py' ), options.mode,options.targetinstroot,options.sourcedbname,options.sourceinit,socket.gethostname())
                try:
                    rc = os.system (cmd)
                except OSError, err:
                    print 'command failed:', err        
                
             #targetdb
             if options.targetinit <> 'DELETE' and options.targetdbname == 'TARGETDB' :
                print 'Install   Databasesystem %s \n'  % options.targetdbname 
                cmd = 'x_python "%s" -mode %s -r "%s" -d %s -e %s '          % (os.path.join (TESTPATH,'sdb','dbm','demo','installdb.py' ), options.mode,options.targetinstroot,options.targetdbname,options.targetcharencoding)
                try:
                    rc = os.system (cmd)
                except OSError, err:
                    print 'command failed:', err
                   
             try:
                 session = dbm.DBM ('',options.targetdbname, options.targetinstroot, 'DBM,DBM' )
                 targetindepdatapath = session.cmd('dbm_getpath INDEPDATAPATH ')
                 targetindepprogpath = session.cmd('dbm_getpath INDEPPROGPATH ')
                 options.targetindepdatapath = re.split('\n',targetindepdatapath)[0]
                 options.targetindepprogpath = re.split('\n',targetindepprogpath)[0]
             except dbm.CommunicationError, err :
                 raise dbm.CommunicationError, err, sys.exc_traceback
             except dbm.DBMServError, err:
                 raise dbm.DBMServError, err , sys.exc_traceback 
             except:
                  kind, val, traceback = sys.exc_info()
                  raise kind, val, traceback
                           
             #db
             if options.transformationpackage <> 'WEBDAV'  :      
                print 'Import    Database       %s '  % options.targetdbname 
                cmd = 'x_python "%s" -mode %s -r "%s" -d %s -i %s'           % (os.path.join (TESTPATH,'sdb','loader','demo','import.py' ), options.mode,DEPPROGPATH, options.targetdbname,options.targetinit)
                try:
                    rc = os.system (cmd)
                except OSError, err:
                    print 'command failed:', err  
             #dav
             if options.transformationpackage == 'WEBDAV'  :        
                print '\nImport    DAV Database   %s'   % options.targetdbname
                cmd = 'x_python "%s" -mode %s -r "%s" -d %s -i %s -h %s:85'  % (os.path.join (TESTPATH,'sdb','dav'   ,'demo','import.py' ), options.mode,options.targetinstroot,options.targetdbname,options.targetinit,socket.gethostname())
                try:
                    rc = os.system (cmd)
                except OSError, err:
                    print 'command failed:', err   
    return rc
           
def init_transformation_files     (options): 
    input = None
    global LOADERDATAPATH
    global LOADERAPPLPATH
    sourceloaderdatapath  = None
    targetloaderdatapath  = None
    sourcetapedatapath    = None
    targettapedatapath    = None
    if sys.platform[:3] == 'win': 
       import _winreg
       #LOADERDATAPATH = os.path.join (_winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"HOMEDRIVE")[0], _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"HOMEPATH")[0])
       LOADERDATAPATH  = _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders"),"Personal")[0]
       LOADERAPPLPATH  = _winreg.QueryValueEx(_winreg.OpenKey(_winreg.HKEY_CURRENT_USER ,"Volatile Environment"),"APPDATA")[0] 
    if sys.platform[:3] != 'win': 
       LOADERDATAPATH = os.environ['HOME']
       LOADERAPPLPATH = os.environ['HOME']
    
    sourceloaderdatapath = os.path.join(LOADERDATAPATH, 'sdb','loader','data', 'source', options.sourcenode, options.sourcedbname)
    targetloaderdatapath = os.path.join(LOADERDATAPATH, 'sdb','loader','data', 'target', options.targetnode, options.targetdbname)
    sourcetapedatapath   = os.path.join(LOADERDATAPATH, 'sdb','tape'  , options.sourcenode, options.sourcedbname)
    targettapedatapath   = os.path.join(LOADERDATAPATH, 'sdb','tape'  , options.targetnode, options.targetdbname)

    while input not in ["y","n"]:   
          if options.mode == 'INTER':
             input = raw_input('Init      Transformation Files ?     [y]/[n] : ')
          if options.mode == 'PARAM':
             print             'Init      Transformation Files '
             input  = "y"
          if input == "n": 
              pass          
        
          if input == "y": 
              #source and target loader protocol file             
              try :
                  os.chdir (os.path.join (LOADERAPPLPATH, 'sdb', 'loader', 'log' ))
                  curdir = os.getcwd()
                  for file in os.listdir(curdir) :
                      if os.path.isfile(file) and  ( string.find(file,'loader') == 0 or string.find(file,'%s_loader' % options.testcase ) == 0 or string.find(file,'%s_install' % options.testcase ) == 0) :
                         try:
                            os.unlink(file)
                         except OSError, err:
                            print 'os.unlink (%s) failed: %s ' %( file ,err )
              except OSError, (errno, err ):
                  if errno <> 17 :
                     print 'command failed %s ' % ( err )
              
              #backup file     
              try:
                  os.makedirs (sourcetapedatapath)
                  os.makedirs (targettapedatapath) 
              except OSError, (errno, err ):
                  if errno <> 17 :
                     print 'os.makedirs (%s) failed: %s ' % (sourcetapedatapath, err )
                     
              if options.streammedium == 'TAPE' and socket.gethostname() == 'p66796':            
                 try :
                     os.chdir ("C:\\WINNT\\Temp\\")
                 except OSError, err:
                     print 'command failed:', err 
                  
                 curdir = os.getcwd() 
                 print 'Empty     Directory %s' % curdir     
                 for file in os.listdir(curdir) :
                     if ( os.path.isfile(file) and ( 
                          string.find(file,'temp')  == 0 )):
                          try:
                              os.unlink(file)
                          except OSError, err:
                              print 'command failed:', err 
                    
def deinit_transformation_files (options):
    # install protocol  
    if sys.platform[:3] == 'win':                
       try :
           os.chdir (os.path.join (LOADERAPPLPATH, 'sdb','loader','log'))
       except OSError, (errno, err ):
           print 'os.chdir %s : %s ' % (os.path.join (LOADERAPPLPATH, 'sdb','loader','log') , err  )
       for file in os.listdir(os.curdir) :        
           if os.path.isfile(file) and  file[:7] == 'install' :
              time.sleep(1) 
              file_install = string.replace(file, 'install', '%s_install' % options.testcase ) 
              try:
                  os.rename(file,file_install)
              except OSError, err:
                  print 'os.rename(%s) failed: %s ' % ( file , err )
       #loader protocol
       try :
           os.chdir (os.path.join (LOADERAPPLPATH, 'sdb','loader','log'))
       except OSError, (errno, err ):
           print 'os.chdir %s : %s ' % (os.path.join (LOADERAPPLPATH, 'sdb','loader','log'), err  )
       for file in os.listdir(os.curdir) :
           if os.path.isfile(file) and file[:6] == 'loader':
              time.sleep(1) 
              file_target = string.replace(file, 'loader', '%s_loader' % options.testcase ) 
              try:
                  os.rename(file,file_target)
              except OSError, err:
                  print 'os.rename(%s) failed: %s ' % ( file , err )
    if sys.platform[:3] != 'win':
       try :
           os.chdir (os.path.join (LOADERAPPLPATH, '.sdb','loader','log'))
       except OSError, (errno, err ):
           print 'os.chdir %s : %s ' % (os.path.join (LOADERAPPLPATH, '.sdb','loader','log') , err  )
       for file in os.listdir(os.curdir) :        
           if os.path.isfile(file) and  file[:7] == 'install' :
              time.sleep(1) 
              file_install = string.replace(file, 'install', '%s_install' % options.testcase ) 
              try:
                  os.rename(file,file_install)
              except OSError, err:
                  print 'os.rename(%s) failed: %s ' % ( file , err )
       #loader protocol
       try :
           os.chdir (os.path.join (LOADERAPPLPATH, '.sdb','loader','log'))
       except OSError, (errno, err ):
           print 'os.chdir %s : %s ' % (os.path.join (LOADERAPPLPATH, '.sdb','loader','log'), err  )
       for file in os.listdir(os.curdir) :
           if os.path.isfile(file) and file[:6] == 'loader':
              time.sleep(1) 
              file_target = string.replace(file, 'loader', '%s_loader' % options.testcase ) 
              try:
                  os.rename(file,file_target)
              except OSError, err:
                  print 'os.rename(%s) failed: %s ' % ( file , err )
              
def check_options(options):
    rc = 0
    if options.testcase != '0':
       #testcase
       if options.testcase              == '':    
          options.testcase               = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [1] [1:]))
       #transformation 
       if options.transformationtype    == '':
          options.transformationtype     = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [2] [1:]))  
       else:
          options.transformationtype     = string.upper(options.transformationtype)
       if options.transformationpackage == '':
          options.transformationpackage  = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [3] [1:]))
       if options.transformationpart    == '':
          options.transformationpart     = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [4] [1:]))       
       if options.transformationstreams == '':
          options.transformationstreams  = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [5] [1:]))              
       if options.startposition         == '':    
          options.startposition          = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [6] [1:]))       
       #stream   
       if options.streammedium          == '':   
          options.streammedium           = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [7] [1:]))
       if options.streamformat          == '':    
          options.streamformat           = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [8] [1:]))
       if options.streamformat_target   == '':    
          options.streamformat_target    = string.upper(string.strip(re.split('-',parameters[options.testcase-1]) [9] [1:]))
       if options.streamcharencoding   == '':    
          options.streamcharencoding     = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[10] [1:]))
       if options.streamlob             == '':    
          options.streamlob              = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[11] [1:]))
       #database
       if options.sourcenode            == 'localhost' or options.sourcenode == '':  
          options.sourcenode             = socket.gethostname()
       if options.sourcedbname          == '':
          options.sourcedbname           = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[13] [4:]))
       if options.sourcedbuser          == '':
          options.sourcedbuser           = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[14] [4:]))
       if options.sourcedbschema        == '':
          options.sourcedbschema         = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[15] [4:]))   
       if options.targetnode            == 'localhost' or options.targetnode == '':
          options.targetnode             = socket.gethostname()
       if options.targetdbname          == '':
          options.targetdbname           = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[17] [4:]))
       if options.targetdbuser          == '':
          options.targetdbuser           = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[18] [4:]))
       if options.targetdbschema        == '':
          options.targetdbschema         = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[19] [4:]))
       #testenvironment
       if options.sourcecharencoding    == '': 
          options.sourcecharencoding     = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[20] [4:])) 
       if options.targetcharencoding    == '':
          options.targetcharencoding     = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[21] [4:]))                     
       if options.validation            == '':    
          options.validation             = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[22] [1:]))
       if options.validationmode        == '':    
          options.validationmode         = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[23] [1:]))                    
       if options.sourceinit            == '':
          options.sourceinit             = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[24] [4:]))
       else:
          options.sourceinit             = string.upper(options.sourceinit)
       if options.targetinit            == '': 
          options.targetinit             = string.upper(string.strip(re.split('-',parameters[options.testcase-1])[25] [4:]))
       else:
          options.targetinit             = string.upper(options.targetinit)
       if options.sourceinstroot        == '' or options.sourceinstroot == 'sourceinstroot' :
          options.sourceinstroot         = ''
       else:
          options.sourceinstroot         = '"%s"' % string.strip(re.split('-',parameters[options.testcase-1])[26] [4:])
       if options.targetinstroot        == '' or options.targetinstroot == 'targetinstroot' : 
          options.targetinstroot         = ''
       else:
          options.targetinstroot         = '"%s"' % string.strip(re.split('-',parameters[options.testcase-1])[27] [4:])
    return rc   
              
def check_parameter(options):
    rc    = 0 
    if( check_range        (options) <> 0 or
        check_parameter    (options) <> 0 or
        check_configuration(options) <> 0   ):
        rc = 1
    return rc            
           
def check_range    (options):
    rc = 0
    if options.testcase > 85:
       print 'Index out of range.' 
       rc = 1
    return rc  

def check_configuration (options):
    rc = 0
    if options.transformationpackage == 'WEBDAV':
       if(socket.gethostname() == 'P48126' or   #markus
          socket.gethostname() == 'P48142' or 
          socket.gethostname() == 'p66796' or 
          socket.gethostname() == 'P76209' or
          socket.gethostname() == 'P59953'   ) :
          rc = 0
       else:        
          print 'transformationpackage DAV: test restricted on server' 
          rc = 1
    if options.streammedium == 'TAPE' :
       if(socket.gethostname() == 'P47579' or   #tilo
          socket.gethostname() == 'p66796' or
          socket.gethostname() == 'P76209'   ) :
          rc = 0
       else:
           print 'medium TAPE: test restricted on server' 
           rc = 1
    if options.streammedium == 'PIPE':
       if sys.platform == 'win32':
          rc = 0
       else: 
          print 'medium PIPE: test restricted on platform' 
          rc = 1
    return rc
       
if __name__ == '__main__':
   optlib.optMain2 (main, _options () )
