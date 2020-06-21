#!/usr/bin/env python
import sys
import repman
import tempfile
import re
import os

def getDbArgs (args):

    import getopt
    import string

    try:
        optlist, args = getopt.getopt(args, 'u:d:')

        opts = {}
        for option in optlist:
            opts[option[0]] = option[1]

        user = opts['-u']
        pos = string.find (user, ',')
        if pos == -1:
            fatalError ('wrong format, use -u <user>,<pwd>')
        pwd = user [pos + 1:]
        user = user [:pos]

        db   = opts['-d']

        return user, pwd, db

    except getopt.error, errtext:
        print errtext

def CreateFileName (filename):
    absFname = os.path.join (os.environ ['TOOL'], 'bin', 'demodb', filename)
    return "'" + absFname + "'"

def connect (username, pwd, db, node):
    session = repman.RepMan (node, db)
    if node == '':
        session.cmd ('use user %s %s serverdb %s' % (username, pwd, db))
    else:
        session.cmd ('use user %s %s serverdb %s on %s' % (username, pwd, db, node ))
    return session

def repmanCmd (session, cmdStr):
    try:
        result = session.cmd (cmdStr)
    except repman.RepManServError, err:
        raise repman.RepManServError, err, sys.exc_traceback
    except:
        kind, val, traceback = sys.exc_info()
        raise kind, val, traceback

def ExistsUser (session, user):
    try:
        repmanCmd (session, ("select * from users where username = '%s'") % user)
        ok = 1
    except repman.RepManServError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a ReplMan error
    return ok

def ExistsDomain (session, domain):
    try:
        repmanCmd (session, ("select * from domains where owner = usergroup and domainname = '%s'") % domain)
        ok = 1
    except repman.RepManServError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a ReplMan error
    return ok

def ExistsTable (session, table):
    try:
        repmanCmd (session, ("select * from tables where owner = usergroup and tablename = '%s'") % table)
        ok = 1
    except repman.RepManServError, err:
        if (err.sqlCode == 100):
            ok = None
        else:
            raise err  # this is a ReplMan error
    return ok

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
    repmanCmd (session, loadcmd)
    # clean up
#    if data != None:
#        os.unlink (tmpname)

if __name__ == '__main__':

    user, pwd, db = getDbArgs(sys.argv[1:])

    session = connect(user, pwd, db, '')

    # CREATE USER

    if not ExistsUser (session, 'EDEMO'):
        repmanCmd(session, 'CREATE USER EDEMO PASSWORD EDEMO DBA NOT EXCLUSIVE')

    if not ExistsUser (session, 'SQLTRAVEL00'):
        repmanCmd(session, 'CREATE USER sqltravel00 PASSWORD travel00 DBA NOT EXCLUSIVE')

    if not ExistsUser (session, 'SQLTRAVEL10'):
        repmanCmd(session, 'CREATE USER sqltravel10 PASSWORD travel10 DBA NOT EXCLUSIVE')

    if not ExistsUser (session, 'SQLTRAVEL20'):
        repmanCmd(session, 'CREATE USER sqltravel20 PASSWORD travel20 DBA NOT EXCLUSIVE')


    repmanCmd(session, 'USE USER sqltravel00 travel00')

    if not ExistsUser (session, 'SQLTRAVEL01'):
        repmanCmd(session, 'CREATE USER sqltravel01 PASSWORD travel01 RESOURCE')
    if not ExistsUser (session, 'SQLTRAVEL02'):
        repmanCmd(session, 'CREATE USER sqltravel02 PASSWORD travel02 RESOURCE')
    if not ExistsUser (session, 'SQLTRAVEL03'):
        repmanCmd(session, 'CREATE USER sqltravel03 PASSWORD travel03 RESOURCE')

    repmanCmd(session, 'USE USER sqltravel10 travel10')

    if not ExistsUser (session, 'SQLTRAVEL11'):
        repmanCmd(session, 'CREATE USER sqltravel11 PASSWORD travel11 RESOURCE')
    if not ExistsUser (session, 'SQLTRAVEL12'):
        repmanCmd(session, 'CREATE USER sqltravel12 PASSWORD travel12 RESOURCE')
    if not ExistsUser (session, 'SQLTRAVEL13'):
        repmanCmd(session, 'CREATE USER sqltravel13 PASSWORD travel13 RESOURCE')

    repmanCmd(session, 'USE USER sqltravel20 travel20')

    if not ExistsUser (session, 'SQLTRAVEL21'):
        repmanCmd(session, """CREATE USER sqltravel21
        PASSWORD travel21 RESOURCE""")
    if not ExistsUser (session, 'SQLTRAVEL22'):
        repmanCmd(session, 'CREATE USER sqltravel22 PASSWORD travel22 RESOURCE')
    if not ExistsUser (session, 'SQLTRAVEL23'):
        repmanCmd(session, 'CREATE USER sqltravel23 PASSWORD travel23 RESOURCE')


    # CREATE TABLES

    repmanCmd(session, 'USE USER sqltravel00 travel00')

    if ExistsDomain (session, 'CNO_DOM'):
        repmanCmd(session, 'DROP DOMAIN cno_dom')
    repmanCmd(session, """CREATE DOMAIN cno_dom FIXED (4)
        CONSTRAINT cno_dom BETWEEN 1 AND 9999""")

    if ExistsDomain (session, 'ZIP_DOM'):
        repmanCmd(session, 'DROP DOMAIN zip_dom')
    repmanCmd(session, """CREATE DOMAIN zip_dom CHAR (5) CONSTRAINT 
    SUBSTR(zip_dom,1,1) between '1' AND '9' AND 
    SUBSTR(zip_dom,2,1) between '0' AND '9' AND 
    SUBSTR(zip_dom,3,1) between '0' AND '9' AND 
    SUBSTR(zip_dom,4,1) between '0' AND '9' AND 
    SUBSTR(zip_dom,5,1) between '0' AND '9' """)

    if ExistsTable (session, 'CITY'):
        repmanCmd(session, 'DROP TABLE city')
    repmanCmd(session, """ CREATE TABLE city
        (zip                zip_dom     KEY,
         name               CHAR(20)    NOT NULL,
         state              CHAR(2)     NOT NULL)""")

    if ExistsTable (session, 'CUSTOMER'):
        repmanCmd(session, 'DROP TABLE customer')
    repmanCmd(session, """CREATE TABLE customer
        (cno                cno_dom     KEY,
         title              CHAR (7)    CONSTRAINT title
                                        IN ('Mr', 'Mrs', 'Company'),
         firstname          CHAR (10)   ,
         name               CHAR (10)   NOT NULL,
         zip                zip_dom     ,
         address            CHAR (25)   NOT NULL,
         FOREIGN KEY (zip) REFERENCES sqltravel00.city ON DELETE RESTRICT)""")

    repmanCmd(session, """CREATE INDEX customer_index ON customer (name)""")

    repmanCmd(session, """CREATE VIEW customer_addr
        (c_no, c_title, c_name, c_firstname,
        c_zip, c_city, c_state, c_address)
        AS SELECT cno, title, customer.name, firstname,
        customer.zip, city.name, state,
        customer.address
        FROM   customer, city
        WHERE  customer.zip = city.zip
        WITH CHECK OPTION""")



    if ExistsTable (session, 'ACCOUNT'):
        repmanCmd(session, 'DROP TABLE account')

    repmanCmd(session,"""CREATE TABLE account
        (booking_day        DATE      KEY,
        activities         FIXED (6,2) )""")



    repmanCmd(session,"""GRANT ALL ON city TO
        sqltravel01,sqltravel02,sqltravel03,
        sqltravel11,sqltravel12,sqltravel13,
        sqltravel21,sqltravel22,sqltravel23""")

    repmanCmd(session,"""GRANT ALL ON city TO
        sqltravel10, sqltravel20 WITH GRANT OPTION""")

    repmanCmd(session,"""GRANT ALL ON customer TO
        sqltravel01,sqltravel02,sqltravel03,
        sqltravel11,sqltravel12,sqltravel13,
        sqltravel21,sqltravel22,sqltravel23""")

    repmanCmd(session,"""GRANT ALL ON customer TO
        sqltravel10,sqltravel20 WITH GRANT OPTION""")

    repmanCmd(session,"""GRANT ALL ON customer_addr TO
        sqltravel01,sqltravel02,sqltravel03,
        sqltravel11,sqltravel12,sqltravel13,
        sqltravel21,sqltravel22,sqltravel23""")

    repmanCmd(session,"""GRANT ALL ON customer_addr TO
        sqltravel10,sqltravel20 WITH GRANT OPTION""")

    repmanCmd(session,"""GRANT ALL ON account TO
        sqltravel01,sqltravel02,sqltravel03,
        sqltravel10,sqltravel11,sqltravel12,sqltravel13,
        sqltravel20,sqltravel21,sqltravel22,sqltravel23""")



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
"19960901","5000.00"
"19960902","-350.00"
"19960903","-350.00"
"19960904","4500.00"
"19960905","1500.00"
"19960906","-1000.00"
"19960907","-1000.00"
"19960908","1500.00"
"19960909","1000.00"
"19960910","6000.00"
"19960911","-100.00"
"19960912","2500.00"
"19960913","350.00"
"19960914","-2000.00"
"19960915","50.00"
"19960916","100.00"
"19960917","-1250.00"
"19960918","8000.00"
"19960919","-2000.00"
"19960920","500.00"
"19960921","-1100.00"
"19960922","1000.00"
"19960923","-200.00"
"19960924","10.00"
"19960925","540.00"
"19960926","-100.00"
"19960927","100.00"
"19960928","1000.00"
""")

    repmanCmd(session, 'USE USER sqltravel10 travel10')

    if ExistsDomain (session, 'HNO_DOM'):
        repmanCmd(session, 'DROP DOMAIN hno_dom')
    repmanCmd(session, """CREATE DOMAIN hno_dom FIXED (4)
        CONSTRAINT hno_dom BETWEEN 1 AND 9999""")

    if ExistsDomain (session, 'ROOM_DOM'):
        repmanCmd(session, 'DROP DOMAIN room_dom')
    repmanCmd(session, """CREATE DOMAIN room_dom CHAR (6)
        CONSTRAINT room_dom IN ('single', 'double','suite')""")

    if ExistsTable (session, 'HOTEL'):
        repmanCmd(session, 'DROP TABLE hotel')

    repmanCmd(session,"""CREATE TABLE hotel
        (hno                hno_dom     KEY,
         name               CHAR (15)   NOT NULL,
         zip                sqltravel00.zip_dom     NOT NULL,
         address            CHAR (25)   NOT NULL,
         info               LONG,
         FOREIGN KEY (zip) REFERENCES sqltravel00.city ON DELETE RESTRICT)""")

    if ExistsTable (session, 'ROOM'):
        repmanCmd(session, 'DROP TABLE room')

    repmanCmd(session,"""CREATE TABLE room
        (hno                hno_dom     KEY,
         roomtype           room_dom    KEY,
         max_free           FIXED(3,0)  CONSTRAINT max_free >= 0,
         price              FIXED(6,2)  CONSTRAINT price BETWEEN 0.00 AND 5000.00,
         FOREIGN KEY (hno) REFERENCES sqltravel10.hotel ON DELETE CASCADE)""")

    repmanCmd(session,"""CREATE VIEW hotel_addr (h_no, h_name, h_zip, h_city, h_state, h_address)
          AS SELECT hno, hotel.name, hotel.zip, city.name,
                    state, hotel.address
                    FROM   hotel, sqltravel00.city
                    WHERE  hotel.zip = city.zip
                    WITH CHECK OPTION""")

    repmanCmd(session,"""GRANT ALL ON hotel TO sqltravel01,sqltravel02,sqltravel03,
                      sqltravel11,sqltravel12,sqltravel13,
                      sqltravel21,sqltravel22,sqltravel23""")

    repmanCmd(session,"""GRANT ALL ON hotel TO sqltravel00,sqltravel20 WITH GRANT OPTION""")

    repmanCmd(session,"""GRANT ALL ON room TO sqltravel01,sqltravel02,sqltravel03,
                     sqltravel11,sqltravel12,sqltravel13,
                     sqltravel21,sqltravel22,sqltravel23""")

    repmanCmd(session,"""GRANT ALL ON room TO sqltravel00,sqltravel20 WITH GRANT OPTION""")

    repmanCmd(session,"""GRANT ALL ON hotel_addr TO sqltravel01,sqltravel02,sqltravel03,
                           sqltravel11,sqltravel12,sqltravel13,
                           sqltravel21,sqltravel22,sqltravel23""")

    repmanCmd(session,"""GRANT ALL ON hotel_addr TO sqltravel00,sqltravel20 WITH GRANT OPTION""")

    dataLoad (session, """
        DATALOAD TABLE hotel
         hno     1
         name    2
         zip     3
         address 4
         info    5 DEFAULT NULL
        INFILE *
        NULL '-'
        LONGFILE INFO """ + CreateFileName('EHOTEL.DSC') + """ ASCII""","""
"10","Congress     ","20005","155 Beechwood Str. ","1-900"
"30","Regency      ","20037","477 17th Avenue    ","901-1064"
"20","Long Island  ","11788","1499 Grove Street  ","-"
"70","Empire State ","12203","65 Yellowstone Dr. ","-"
"80","Midtown      ","10019","12 Barnard Str.    ","-"
"40","Eight Avenue ","10019","112 8th Avenue     ","-"
"50","Lake Michigan","60601","354 OAK Terrace    ","-"
"60","Airport      ","60018","650 C Parkway      ","1065-1360"
"90","Sunshine     ","33575","200 Yellowstone Dr.","1361-1953"
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
         roomtype      2
         max_free      3
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

    repmanCmd(session, 'USE USER sqltravel20 travel20')

    if ExistsDomain (session, 'RNO_DOM'):
        repmanCmd(session, 'DROP DOMAIN rno_dom')
    repmanCmd(session, """CREATE DOMAIN rno_dom FIXED (4)
        CONSTRAINT rno_dom BETWEEN 1 AND 9999""")

    if ExistsTable (session, 'RESERVATION'):
        repmanCmd(session, 'DROP TABLE reservation')
    repmanCmd(session, """ CREATE TABLE reservation
        (rno                rno_dom     KEY,
         cno                sqltravel00.cno_dom     ,
         hno                sqltravel10.hno_dom     ,
         roomtype           sqltravel10.room_dom    ,
         arrival            DATE        NOT NULL,
         departure          DATE        NOT NULL CONSTRAINT departure > arrival,
         FOREIGN KEY (cno) REFERENCES sqltravel00.customer ON DELETE CASCADE,
         FOREIGN KEY (hno,roomtype) REFERENCES sqltravel10.room ON DELETE CASCADE)""")

    repmanCmd(session,"""GRANT ALL ON reservation TO
         sqltravel00,sqltravel01,sqltravel02,sqltravel03,
         sqltravel10,sqltravel11,sqltravel12,sqltravel13,
         sqltravel21,sqltravel22,sqltravel23""")

    repmanCmd(session,"""CREATE VIEW custom_hotel
        (customname, customcity, hotelname, hotelcity)
          AS SELECT c_name, c_city, h_name, h_city
          FROM  sqltravel00.customer_addr,
                sqltravel10.hotel_addr,
                reservation
          WHERE c_no = reservation.cno
          AND   h_no = reservation.hno""")

    repmanCmd(session,"""GRANT ALL ON custom_hotel TO
            sqltravel00,sqltravel01,sqltravel02,sqltravel03,
            sqltravel10,sqltravel11,sqltravel12,sqltravel13,
            sqltravel21,sqltravel22,sqltravel23""")

    dataLoad (session, """
        DATALOAD TABLE reservation
         rno           1
         cno           2
         hno           3
         roomtype      4
         arrival       5
         departure     6
        INFILE *
        DATE 'YY-MM-DD'
        NULL '-'""","""
"100","3000","80","single","95-11-13","95-11-15"
"110","3000","100","double","95-12-24","96-01-06"
"120","3200","50","suite ","95-11-14","95-11-18"
"130","3900","110","single","96-02-01","96-02-03"
"150","3600","70","double","96-03-14","96-03-24"
"140","4300","80","double","95-04-12","95-04-30"
"160","4100","70","single","95-04-12","95-04-15"
"170","4400","150","suite ","95-09-01","95-09-03"
"180","3100","120","double","95-12-23","96-01-08"
"190","4300","140","double","95-11-14","95-11-17"
""")

    repmanCmd(session,"""USE USER sqltravel00 travel00""")

    repmanCmd(session,"""CREATE SYNONYM hotel FOR sqltravel10.hotel""")
    repmanCmd(session,"""CREATE SYNONYM room FOR sqltravel10.room""")
    repmanCmd(session,"""CREATE SYNONYM reservation FOR sqltravel20.reservation""")
    repmanCmd(session,"""CREATE SYNONYM hotel_addr FOR sqltravel10.hotel_addr""")
    repmanCmd(session,"""CREATE SYNONYM custom_hotel FOR sqltravel20.custom_hotel""")



    repmanCmd(session,"""USE USER sqltravel10 travel10""")

    repmanCmd(session,"""CREATE SYNONYM city FOR sqltravel00.city""")
    repmanCmd(session,"""CREATE SYNONYM customer FOR sqltravel00.customer""")
    repmanCmd(session,"""CREATE SYNONYM account FOR sqltravel00.account""")
    repmanCmd(session,"""CREATE SYNONYM reservation FOR sqltravel20.reservation""")
    repmanCmd(session,"""CREATE SYNONYM customer_addr FOR sqltravel00.customer_addr""")
    repmanCmd(session,"""CREATE SYNONYM custom_hotel FOR sqltravel20.custom_hotel""")



    repmanCmd(session,"""USE USER sqltravel20 travel20""")

    repmanCmd(session,"""CREATE SYNONYM city FOR sqltravel00.city""")
    repmanCmd(session,"""CREATE SYNONYM customer FOR sqltravel00.customer""")
    repmanCmd(session,"""CREATE SYNONYM account FOR sqltravel00.account""")
    repmanCmd(session,"""CREATE SYNONYM hotel FOR sqltravel10.hotel""")
    repmanCmd(session,"""CREATE SYNONYM room FOR sqltravel10.room""")
    repmanCmd(session,"""CREATE SYNONYM customer_addr FOR sqltravel00.customer_addr""")
    repmanCmd(session,"""CREATE SYNONYM hotel_addr FOR sqltravel10.hotel_addr""")




    repmanCmd(session,"""USE USER EDEMO EDEMO""")

    if ExistsTable (session, 'CATEGORIES'):
        repmanCmd(session, 'DROP TABLE categories')
    repmanCmd(session,"""CREATE TABLE CATEGORIES (
        CATEGORYID       INTEGER,
        CATEGORYNAME     VARCHAR(16) NOT NULL,
        DESCRIPTION      LONG,
        PICTURE          LONG BYTE
        , PRIMARY KEY (CATEGORYID))""")

    dataLoad (session, """
        DATALOAD TABLE CATEGORIES
        CATEGORYID      1
        CATEGORYNAME    2
        DESCRIPTION     3 DEFAULT NULL
        PICTURE         4 DEFAULT NULL
        INFILE *
        NULL '-'
        LONGFILE DESCRIPTION """ + CreateFileName('DESCRIPTION.LONG') + """ ASCII
        LONGFILE PICTURE """ + CreateFileName('PICTURE.LONG'),"""
"1","Beverages     ","  1- 41","    1-10678"
"2","Condiments    "," 42-100","10679-21356"
"3","Confections   ","101-130","21357-32034"
"4","Dairy Products","131-137","32035-42712"
"5","GrainsCereals ","138-172","42713-53390"
"6","MeatPoultry   ","173-186","53391-64068"
"7","Produce       ","187-211","64069-74746"
"8","Seafood       ","212-227","74747-85424"
""")

    repmanCmd(session,"""CREATE UNIQUE INDEX CATEGORYNAME ON
        EDEMO.CATEGORIES (CATEGORYNAME ASC )""")

    if ExistsTable (session, 'SUPPLIERS'):
        repmanCmd(session, 'DROP TABLE suppliers')
    repmanCmd(session,"""CREATE TABLE SUPPLIERS (
        SUPPLIERID        INTEGER,
        COMPANYNAME       VARCHAR (40) NOT NULL,
        CONTACTNAME       VARCHAR (30),
        CONTACTTITLE      VARCHAR (32),
        ADDRESS           VARCHAR (60),
        CITY              VARCHAR (15),
        REGION            VARCHAR (15),
        POSTALCODE        VARCHAR (10),
        COUNTRY           VARCHAR (15),
        PHONE             VARCHAR (24),
        FAX               VARCHAR (24)
        , PRIMARY KEY (SUPPLIERID))""")

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
        INFILE """ + CreateFileName('SUPPLIER.DAT'))

    repmanCmd(session,"""CREATE INDEX CompanyName ON EDEMO.Suppliers (CompanyName ASC )""")

    if ExistsTable (session, 'CUSTOMERS'):
        repmanCmd(session, 'DROP TABLE Customers')
    repmanCmd(session,"""CREATE TABLE Customers (
        CustomerID        VARCHAR   (5),
        CompanyName       VARCHAR   (40) NOT NULL,
        ContactName       VARCHAR   (30),
        ContactTitle      VARCHAR   (30),
        Address           VARCHAR   (60),
        City              VARCHAR   (15),
        Region            VARCHAR   (15),
        PostalCode        VARCHAR   (10),
        Country           VARCHAR   (15),
        Phone             VARCHAR   (24),
        Fax               VARCHAR   (24)
        , PRIMARY KEY (CustomerID))""")

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
        INFILE """ + CreateFileName('CUSTOMER.DAT'))

    repmanCmd(session,"""CREATE INDEX CompanyName ON EDEMO.Customers (CompanyName ASC )""")
    repmanCmd(session,"""CREATE INDEX City ON EDEMO.Customers (City ASC )""")
    repmanCmd(session,"""CREATE INDEX Region ON EDEMO.Customers (Region ASC )""")



    if ExistsTable (session, 'EMPLOYEES'):
        repmanCmd(session, 'DROP TABLE Employees')
    repmanCmd(session,"""CREATE TABLE EDEMO.Employees (
        EmployeeID        INTEGER,
        LastName          VARCHAR   (20) NOT NULL,
        FirstName         VARCHAR   (10) NOT NULL,
        Title             VARCHAR   (30),
        BirthDate         DATE,
        HireDate          DATE,
        Address           VARCHAR   (60),
        City              VARCHAR   (15),
        Region            VARCHAR   (15),
        PostalCode        VARCHAR   (10),
        Country           VARCHAR   (15),
        HomePhone         VARCHAR   (24),
        Extension         VARCHAR   (4),
        Photo             LONG BYTE,
        Notes             LONG,
        ReportsTo         INTEGER
        , PRIMARY KEY (EmployeeID))""")

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
        LONGFILE PHOTO """ + CreateFileName('PHOTO.DAT') + """
        LONGFILE NOTES """ + CreateFileName('NOTE.DAT') + """ ASCII""","""
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
"14","Martin","Xavier","Marketing Associate","11301960","01151994","9 place de la Liberté","Schiltigheim","Bas-Rhin","67300","France","88 62 43 53","380","279839-301364","4019-4477","13"
"15","Pereira","Laurent","Advertising Specialist","12091965","02011994","7 rue Nationale","Strasbourg","Bas-Rhin","67000","France","88 01 01 68","376","301365-322890","4478-4650","?"
""")

    repmanCmd(session,"""CREATE INDEX LastName ON EDEMO.Employees (LastName ASC )""")


    if ExistsTable (session, 'SHIPPERS'):
        repmanCmd(session, 'DROP TABLE Shippers')
    repmanCmd(session,"""CREATE TABLE Shippers (
        ShipperID         INTEGER,
        Companyname       VARCHAR   (40) NOT NULL
        , PRIMARY KEY (ShipperID))""")
        
    dataLoad (session, """
        DATALOAD TABLE Shippers
        ShipperID   1 
        CompanyName 2 
        INFILE *""","""
"1","Speedy Express"
"2","United Package"
"3","Federal Shipping"
""")


    if ExistsTable (session, 'PRODUCTS'):
        repmanCmd(session, 'DROP TABLE PRODUCTS')
    repmanCmd(session,"""CREATE TABLE PRODUCTS (
        ProductID         INTEGER,
        SupplierID        INTEGER,
        CategoryID        INTEGER,
        Productname       VARCHAR (40),
        QuantityPerUnit   VARCHAR (23),
        UnitPrice         FLOAT (8),
        UnitsInStock      SMALLINT,
        UnitsOnOrder      SMALLINT,
        ReorderLevel      SMALLINT,
        Discontinued      BOOLEAN DEFAULT FALSE,
        BarcodeID         CHAR (10) BYTE,
        PRIMARY KEY (ProductID))""")

    dataLoad (session, """
        DATALOAD TABLE Products
        ProductID        1 CHAR
        SupplierID       2 CHAR         DEFAULT NULL
        CategoryID       3 CHAR         DEFAULT NULL
        Productname      4 CHAR         DEFAULT NULL
        QuantityPerUnit 5 CHAR         DEFAULT NULL
        UnitPrice        6 CHAR         DEFAULT NULL
        UnitsInStock    7 CHAR         DEFAULT NULL
        UnitsOnOrder    8 CHAR         DEFAULT NULL
        ReorderLevel     9 CHAR         DEFAULT NULL
        Discontinued     10 CHAR         DEFAULT NULL
        INFILE  """ + CreateFileName('PRODUCTS.DAT'))
        


    repmanCmd(session,"""ALTER TABLE EDEMO.Products FOREIGN KEY CategoryProduct
        (CategoryID) REFERENCES EDEMO.Categories (CategoryID) ON DELETE RESTRICT""")



    repmanCmd(session,"""ALTER TABLE EDEMO.Products FOREIGN KEY SupplierProduct
        (SupplierID
        ) REFERENCES EDEMO.Suppliers
        (SupplierID
        ) ON DELETE RESTRICT""")


    repmanCmd(session,"""CREATE INDEX ProductName ON EDEMO.Products (Productname ASC )""")
    repmanCmd(session,"""CREATE INDEX CategoryID ON EDEMO.Products (CategoryID ASC )""")
    repmanCmd(session,"""CREATE INDEX SupplierID ON EDEMO.Products (SupplierID ASC )""")

    if ExistsTable (session, 'ORDERS'):
        repmanCmd(session, 'DROP TABLE ORDERS')
    repmanCmd(session,"""CREATE TABLE EDEMO.ORDERS (
        OrderID           INTEGER,
        CustomerID        VARCHAR (5) NOT NULL,
        EmployeeID        INTEGER,
        Shipname          VARCHAR (40),
        ShipAddress       VARCHAR (60),
        ShipCity          VARCHAR (15),
        ShipRegion        VARCHAR (15),
        ShipPostalCode    VARCHAR (10),
        ShipCountry       VARCHAR (15),
        ShipVia           INTEGER,
        OrderDate         DATE,
        RequiredDate      DATE,
        ShippedDate       DATE,
        Freight           FLOAT (8)
        , PRIMARY KEY (OrderID))""")

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
        INFILE  """ + CreateFileName('ORDERS.DAT') + """
        DATE 'MM/DD/YYYY'""")


    repmanCmd(session,"""ALTER TABLE EDEMO.Orders FOREIGN KEY CustomerOrder
        (CustomerID) REFERENCES EDEMO.Customers
        (CustomerID) ON DELETE RESTRICT""")



    repmanCmd(session,"""ALTER TABLE EDEMO.Orders FOREIGN KEY EmployeeOrder
        (EmployeeID
        ) REFERENCES EDEMO.Employees
        (EmployeeID
        ) ON DELETE RESTRICT""")



    repmanCmd(session,"""ALTER TABLE EDEMO.Orders FOREIGN KEY ShipperOrder
        (ShipVia
        ) REFERENCES EDEMO.Shippers
        (ShipperID
        ) ON DELETE RESTRICT""")

    repmanCmd(session,"""CREATE INDEX OrderDate ON EDEMO.Orders (OrderDate ASC )""")
    repmanCmd(session,"""CREATE INDEX CustomerID ON EDEMO.Orders (CustomerID ASC )""")
    repmanCmd(session,"""CREATE INDEX EmployeeID ON EDEMO.Orders (EmployeeID ASC )""")

    if ExistsTable (session, 'ORDERDETAILS'):
        repmanCmd(session, 'DROP TABLE ORDERDETAILS')
    repmanCmd(session,"""CREATE TABLE EDEMO.ORDERDETAILS (
        OrderID           INTEGER,
        ProductID         INTEGER,
        UnitPrice         FLOAT (8) NOT NULL,
        Quantity          SMALLINT DEFAULT  1,
        Discount          FLOAT(4) DEFAULT  0.000E+00,
        CONSTRAINT Quantity CHECK Quantity>0,
        CONSTRAINT Discount CHECK Discount BETWEEN 0 AND 1
        , PRIMARY KEY (OrderID,ProductID))""")

    dataLoad (session, """
        DATALOAD TABLE OrderDetails
        OrderID   1 CHAR
        ProductID 2 CHAR
        UnitPrice 3 CHAR
        Quantity   4 CHAR DEFAULT NULL
        Discount   5 CHAR DEFAULT NULL
        INFILE  """ + CreateFileName('ODETAILS.DAT')) 



    repmanCmd(session,"""ALTER TABLE EDEMO.OrderDetails FOREIGN KEY ProductOrderDetails
        (ProductID
        ) REFERENCES EDEMO.Products
        (ProductID
        ) ON DELETE RESTRICT""")



    repmanCmd(session,"""ALTER TABLE EDEMO.OrderDetails FOREIGN KEY OrderOrderDetail
        (OrderID
        ) REFERENCES EDEMO.Orders
        (OrderID
        ) ON DELETE CASCADE""")

    repmanCmd(session,"""CREATE INDEX ProductID ON EDEMO.OrderDetails (ProductID ASC )""")
    repmanCmd(session,"""COMMENT ON COLUMN OrderDetails.OrderID IS 'Same as OrderID in Orders Table.'""")
    repmanCmd(session,"""COMMENT ON COLUMN OrderDetails.ProductID IS 'Same as ProductID in Products table.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Products.ProductID IS 'Number automatically assigned to new product.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Products.SupplierID IS 'Same as SupplierID in Suppliers table.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Products.CategoryID IS 'Same as CategoryID in Categoriers table.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Products.QuantityPerUnit IS '(e.g., 24-count case, 1-liter bottle).'""")
    repmanCmd(session,"""COMMENT ON COLUMN Products.ReorderLevel IS 'Minimum units to maintain in stock.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Products.Discontinued IS 'Yes means item no longer available.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Orders.OrderID IS 'Unique order number from invoice.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Orders.CustomerID IS 'Same as CustomerID in Customers table.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Orders.EmployeeID IS 'Same as EmployeeID in Employees table.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Orders.ShipName IS 'Name of person or company to receive the shipment.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Orders.ShipAddress IS 'Street address only -- no post-office box allowed.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Orders.ShipRegion IS 'State or Province'""")
    repmanCmd(session,"""COMMENT ON COLUMN Orders.ShipVia IS 'Same as ShipperID in Shippers table.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Categories.CategoryID IS 'Number automatically assigned to new category.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Categories.CategoryName IS 'Name of food category.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Categories.Picture IS 'A picture representing the food category.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Customers.CustomerID IS 'Unique five-character code based on customer name.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Customers.Address IS 'Street or post-office box.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Customers.Region IS 'State or province.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Customers.Phone IS 'Phone number includes country code or area code.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Customers.Fax IS 'Phone number includes country code or area code.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Suppliers.SupplierID IS 'Number automatically assigned to new supplier.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Suppliers.Address IS 'Street or post-office box.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Suppliers.Region IS 'State or province.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Suppliers.Phone IS 'Phone number includes country code or area code.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Suppliers.Fax IS 'Phone number includes country code or area code.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.EmployeeID IS 'Number automatically assigned to new employees.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.Title IS 'Employee''s title.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.Address IS 'Street or post-office box.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.Region IS 'State or province.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.HomePhone IS 'Phone number includes country code or area code.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.Extension IS 'Phone number includes country code or area code.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.Photo IS 'Picture of employee.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.Notes IS 'General information about employee''s background.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Employees.ReportsTo IS 'Employee''s supervisor.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Shippers.ShipperID IS 'Number automatically assigned to new shipper.'""")
    repmanCmd(session,"""COMMENT ON COLUMN Shippers.CompanyName IS 'Name of shipping company.'""")


    repmanCmd(session,"""CREATE VIEW EDEMO.OrderSubtotals AS
        SELECT OrderID, Subtotal = Sum(UnitPrice * Quantity * (1 - Discount) * 100 / 100)
        FROM OrderDetails
        GROUP BY OrderID""")

    if ExistsTable (session, 'REQUESTS'):
        repmanCmd(session, 'DROP TABLE REQUESTS')
    repmanCmd(session,"""CREATE TABLE EDEMO.REQUESTS (
        Requestname VARCHAR (40) NOT NULL,
        Request     LONG NOT NULL,
        PRIMARY KEY (RequestName))""")
        
    dataLoad (session, """
        DATALOAD TABLE Requests
        Requestname  1
        Request      2
        INFILE *
        LONGFILE REQUEST """ + CreateFileName('REQUEST.LONG') + """ ASCII""","""
"Daily Order Totals    ","   1-202"
"Order Subtotals       "," 203-337"
"Product List          "," 338-676"
"Products and Suppliers"," 677-1036"
"Products on Order     ","1037-1274"
"Sales for 1993        ","1275-1684"
""")

    if ExistsTable (session, 'PHOTOS'):
        repmanCmd(session, 'DROP TABLE PHOTOS')
    repmanCmd(session,"""CREATE TABLE EDEMO.PHOTOS (
        Name    CHAR (18) NOT NULL,
        Photo    LONG BYTE)""")

    dataLoad (session, """
        DATALOAD TABLE Photos
        Name   1 
        Photo  2 
        INFILE *""","""
"Photo: R. Meinecke"," """ + CreateFileName('PHOT01.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT02.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT03.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT04.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT05.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT06.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT07.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT08.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT09.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT10.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT11.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT12.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT13.BMP') + """ "
"Photo: R. Meinecke"," """ + CreateFileName('PHOT14.BMP') + """ "
""")

