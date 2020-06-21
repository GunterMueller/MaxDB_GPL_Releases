senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* BOOL.vdnts
file connect ( sut sut !
CREATE TABLE J1 ( B1 BOOLEAN, T1 SMALLINT ) !
CREATE TABLE J2 ( B2 BOOLEAN, T2 SMALLINT ) !
INSERT INTO J1 VALUES ( TRUE , 1 ) !
INSERT INTO J1 VALUES ( FALSE, 2 ) !
INSERT INTO J2 VALUES ( TRUE , 2 ) !
INSERT INTO J2 VALUES ( FALSE, 1 ) !
SELECT B1, T1, T2, B2  FROM J1, J2 WHERE B1 = B2 !
FETCH!

CREATE TABLE c1 (nr1 smallint, ort char(10), b1 boolean) !
CREATE TABLE c2 (nr2 smallint, preis int, b2 boolean ) !
INSERT INTO c1 VALUES ( 1, 'Berlin', TRUE) !
INSERT INTO c1 VALUES ( 2, 'Berlin', TRUE) !
INSERT INTO c1 VALUES ( 3, 'Berlin', TRUE) !
INSERT INTO c1 VALUES ( 4, 'Berlin', FALSE) !
INSERT INTO c1 VALUES ( 5, 'Berlin', FALSE) !
INSERT INTO c1 VALUES ( 6, 'Hamburg', FALSE ) !
INSERT INTO c1 VALUES ( 7, 'Hamburg', FALSE ) !
INSERT INTO c1 VALUES ( 8, 'Hamburg', FALSE ) !
INSERT INTO c1 VALUES ( 9, 'Mainz', NULL ) !
INSERT INTO c1 VALUES ( 10, 'Mainz', TRUE ) !
INSERT INTO c1 VALUES ( 11, 'Mainz', FALSE ) !
INSERT INTO c2 VALUES ( 1, 100, TRUE ) !
INSERT INTO c2 VALUES ( 2, 200, FALSE ) !
INSERT INTO c2 VALUES ( 3, 300, NULL ) !
INSERT INTO c2 VALUES ( 4, 400, TRUE ) !
INSERT INTO c2 VALUES ( 5, 500, FALSE ) !
INSERT INTO c2 VALUES ( 6, 600, NULL ) !
INSERT INTO c2 VALUES ( 7, 700, TRUE ) !
INSERT INTO c2 VALUES ( 8, 800, FALSE ) !
INSERT INTO c2 VALUES ( 9, 900, NULL ) !
INSERT INTO c2 VALUES ( 10, 1000, TRUE ) !
INSERT INTO c2 VALUES ( 11, 1100, FALSE ) !
SELECT b2, ort
FROM   c1 x, c2
WHERE  x.nr1 = nr2
  AND  c2.nr2 < ( SELECT count(b1)
                 FROM   c1, c2
                 WHERE  c1.nr1 = c2.nr2
                   AND  c1.ort = x.ort) !
FETCH !
SELECT b2, ort
FROM   c1 x, c2
WHERE  x.nr1 = nr2
  AND  c2.nr2 < ( SELECT count(b2)
                 FROM   c2
                 WHERE  x.b1 = c2.b2)!
FETCH !
SELECT * 
FROM   c1 x
WHERE  b1 = (SELECT b1
             FROM   c1
             WHERE  c1.nr1 = x.nr1) !
FETCH !
ROLLBACK WORK RELEASE !

*****************************************************************************
* BURRY.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE DOMAIN DM_ADDRESSES       CHAR(20)!
CREATE DOMAIN DM_BINS            CHAR(5)!
CREATE DOMAIN DM_B_IDS      CHAR(4)!
CREATE DOMAIN DM_CITIES          CHAR(14)!
CREATE DOMAIN DM_CLASSIFICATIONS CHAR(2)!
COMMIT WORK!
CREATE DOMAIN DM_CREDITS         CHAR(2)
RANGE IN ('A1','A2','A3','B1','B2','B3','C1','C2','C3','C7')!
CREATE DOMAIN DM_C_IDS    CHAR(6)!
CREATE DOMAIN DM_DATES           DATE!
CREATE DOMAIN DM_DESCRIPTIONS    CHAR(30)!
CREATE DOMAIN DM_EMPLOYEE_IDS    CHAR(4)!
CREATE DOMAIN DM_GRO_IDS       CHAR(2)!
CREATE DOMAIN DM_IV_IDS     CHAR(4)!
CREATE DOMAIN DM_M_IDS    CHAR(5)!
CREATE DOMAIN DM_MONIES          FIXED(9,2)
RANGE BETWEEN  0.00  AND  999999.99!
COMMIT WORK!
CREATE DOMAIN DM_NAMES           CHAR(20)!
CREATE DOMAIN DM_PO_IDS          CHAR(6)!
CREATE DOMAIN DM_PR_IDS     CHAR(9)!
CREATE DOMAIN DM_QUANTITIES      FIXED(5,0)
RANGE BETWEEN  0  AND  99999!
CREATE DOMAIN DM_R_IDS    INTEGER!
CREATE DOMAIN DM_REGION_IDS      CHAR(3)!
CREATE DOMAIN DM_ROUTES          CHAR(2)
RANGE IN ('00','01','02','03','04','05','06','07','08','09')!
CREATE DOMAIN DM_STATES          CHAR(2)
RANGE IN ('AL','AK','AZ','AR','CA','CO','CT','DE','FL','GA',
          'HI','ID','IL','IN','IA','KS','KY','LA','ME','MD',
          'MA','MI','MN','MS','MO','MT','NE','NV','NH','NJ',
          'NM','NY','NC','ND','OH','OK','OR','PA','RI','SC',
          'SD','TN','TX','UT','VT','VA','WA','WV','WI','WY',
          'DC')!
CREATE DOMAIN DM_SU_IDS    CHAR(6)!
CREATE DOMAIN DM_ZIPCODES        CHAR(5)
RANGE BETWEEN  '00000' AND '99999'!
COMMIT WORK!
* TABLES
CREATE TABLE kern.BRANCH
( B_NO          DM_B_IDS      KEY
, B_NAME        DM_NAMES               NOT NULL
, B_ADDR        DM_ADDRESSES           NOT NULL
, B_CITY        DM_CITIES              NOT NULL
, B_STATE       DM_STATES              NOT NULL
, B_ZIPCODE     DM_ZIPCODES            NOT NULL
, B_REGION      DM_REGION_IDS          NOT NULL
, B_ROUTE   DM_ROUTES
, B_SLSQUO   DM_MONIES
, B_STFQUO   DM_QUANTITIES
, B_DIRECT  LONG )!
CREATE TABLE kern.CUSTOMER
( C_NO        DM_C_IDS    KEY
, C_NAME      DM_NAMES               NOT NULL
, C_ADDR      DM_ADDRESSES           NOT NULL
, C_CITY      DM_CITIES              NOT NULL
, C_STATE     DM_STATES              NOT NULL
, C_ZIPCODE   DM_ZIPCODES            NOT NULL
, C_BRANCH    DM_B_IDS          NOT NULL
, C_CLASS     DM_CLASSIFICATIONS
, C_CR_CODE   DM_CREDITS
, C_CR_LIM    DM_MONIES              NOT NULL )!
CREATE TABLE kern.GROUPTAB
( GRO_CODE         DM_GRO_IDS       KEY
, GRO_DESC         DM_DESCRIPTIONS        NOT NULL )!
CREATE TABLE kern.INVLINE
( I_INVOICE    DM_IV_IDS     KEY
, I_PROD       DM_PR_IDS     KEY
, I_QNTY       DM_QUANTITIES          NOT NULL
, I_PRICE      DM_MONIES              NOT NULL )!
COMMIT WORK!
CREATE TABLE kern.INVOICE
( IV_NO         DM_IV_IDS     KEY
, IV_SLSPRS     DM_EMPLOYEE_IDS        NOT NULL
, IV_TOTAL      DM_MONIES              NOT NULL
, IV_BRANCH     DM_B_IDS          NOT NULL
, IV_DATE       DM_DATES               NOT NULL
, IV_CUSTOM   DM_C_IDS )!
CREATE TABLE kern.MANIFEST
( M_NO        DM_M_IDS    KEY
, M_TOTAL     DM_MONIES              NOT NULL
, M_BRANCH    DM_B_IDS          NOT NULL
, M_DATE      DM_DATES )!
CREATE TABLE kern.MANLINE
( ML_MANIF      DM_M_IDS    KEY
, ML_PROD       DM_PR_IDS     KEY
, ML_QNTY       DM_QUANTITIES          NOT NULL
, ML_VALUE      DM_MONIES              NOT NULL )!
COMMIT WORK!
CREATE TABLE kern.PO
( PO_NO              DM_PO_IDS          KEY
, PO_TOTAL           DM_MONIES              NOT NULL
, PO_SUPPL        DM_SU_IDS        NOT NULL
, PO_DATE            DM_DATES               NOT NULL
, PO_DEL_DT        DM_DATES               NOT NULL )!
CREATE TABLE kern.POLINE
( PL_PO          DM_PO_IDS          KEY
, PL_PROD        DM_PR_IDS     KEY
, PL_QNTY        DM_QUANTITIES          NOT NULL
, PL_COST        DM_MONIES              NOT NULL )!
CREATE TABLE kern.PRODUCT
( PR_CODE       DM_PR_IDS     KEY
, PR_DESC       DM_DESCRIPTIONS        NOT NULL
, PR_WH_QNT    DM_QUANTITIES          NOT NULL
, PR_PRICE      DM_MONIES
, PR_GROUP      DM_GRO_IDS           NOT NULL )!
CREATE TABLE kern.REGION
( REGION_NO          DM_REGION_IDS KEY
, REG_NAME        DM_NAMES               NOT NULL )!
CREATE TABLE kern.STOCK
( ST_BRANCH       DM_B_IDS      KEY
, ST_PROD         DM_PR_IDS     KEY
, ST_QNTY         DM_QUANTITIES          NOT NULL
, ST_BIN_LC      DM_BINS
, ST_YT_SAL    DM_MONIES              NOT NULL )!
CREATE TABLE kern.STRUCTURE
( STR_ASSM     DM_PR_IDS     KEY
, STR_COMP     DM_PR_IDS     KEY
, STR_QNTY     DM_QUANTITIES          NOT NULL )!
CREATE TABLE kern.SUPPLIER
( SU_NO        DM_SU_IDS    KEY
, SU_NAME      DM_NAMES               NOT NULL
, SU_ADDR      DM_ADDRESSES           NOT NULL
, SU_CITY      DM_CITIES              NOT NULL
, SU_STATE     DM_STATES              NOT NULL
, SU_ZIPCOD   DM_ZIPCODES            NOT NULL )!
CREATE TABLE kern.VS_NO
( VSN_SUPPL     DM_SU_IDS    KEY
, VSN_PROD         DM_PR_IDS     KEY
, VSN_PNO      DM_DESCRIPTIONS        NOT NULL
, VSN_PCOST    DM_MONIES              NOT NULL )!
COMMIT WORK!
alter table kern.branch FOREIGN KEY ASSIGNED_TO ( B_REGION )
REFERENCES kern.REGION ON DELETE RESTRICT!
alter table kern.customer FOREIGN KEY OPENED_ACCOUNT_AT ( C_BRANCH )
REFERENCES kern.BRANCH ON DELETE RESTRICT!
alter table kern.invline FOREIGN KEY CONTAINED_ON ( I_PROD)
REFERENCES kern.PRODUCT ON DELETE RESTRICT!
alter table kern.invline FOREIGN KEY SOLD_ON ( I_INVOICE )
REFERENCES kern.INVOICE ON DELETE CASCADE!
COMMIT WORK!
alter table kern.invoice FOREIGN KEY CUT_AT ( IV_BRANCH )
REFERENCES kern.BRANCH ON DELETE RESTRICT!
alter table kern.invoice FOREIGN KEY CUT_FOR ( IV_CUSTOM )
REFERENCES kern.CUSTOMER ON DELETE RESTRICT!
alter table kern.manifest FOREIGN KEY DISTRIBUTED_TO ( M_BRANCH )
REFERENCES kern.BRANCH ON DELETE RESTRICT!
alter table kern.MANLINE FOREIGN KEY INCLUDED ( ML_PROD)
REFERENCES kern.PRODUCT ON DELETE RESTRICT!
COMMIT WORK!
alter table kern.MANLINE FOREIGN KEY SHIPPED_ON ( ml_manif )
REFERENCES kern.MANIFEST ON DELETE CASCADE!
alter table kern.PO FOREIGN KEY PLACED_WITH ( PO_SUPPL )
REFERENCES kern.SUPPLIER ON DELETE RESTRICT!
alter table kern.POLINE FOREIGN KEY BOUGHT_ON ( PL_PO )
REFERENCES kern.PO ON DELETE RESTRICT!
alter table kern.POLINE FOREIGN KEY CONTAINED ( PL_PROD)
REFERENCES kern.PRODUCT ON DELETE CASCADE!
COMMIT WORK!
alter table kern.PRODUCT FOREIGN KEY ORGANIZED_BY ( PR_GROUP )
REFERENCES kern.GROUPTAB ON DELETE RESTRICT!
alter table kern.STOCK FOREIGN KEY CONTAINS ( ST_PROD)
REFERENCES kern.PRODUCT ON DELETE RESTRICT!
alter table kern.STOCK FOREIGN KEY STORED_AT ( ST_BRANCH )
REFERENCES kern.BRANCH ON DELETE RESTRICT!
COMMIT WORK!
alter table kern.STRUCTURE FOREIGN KEY MADE_OF ( STR_ASSM )
REFERENCES kern.PRODUCT ON DELETE RESTRICT!
alter table kern.STRUCTURE FOREIGN KEY USED_IN ( STR_COMP )
REFERENCES kern.PRODUCT ON DELETE RESTRICT!
alter table kern.VS_NO FOREIGN KEY CAN_BUY_FROM ( VSN_SUPPL )
REFERENCES kern.SUPPLIER ON DELETE RESTRICT!
alter table kern.VS_NO FOREIGN KEY OFFERS ( VSN_PROD)
REFERENCES kern.PRODUCT ON DELETE RESTRICT!
COMMIT WORK!
* BASE VIEWS
CREATE VIEW kern.B_BV      AS SELECT * FROM kern.BRANCH!
CREATE VIEW kern.C_BV    AS SELECT * FROM kern.CUSTOMER!
CREATE VIEW kern.GROUP_BV       AS SELECT * FROM kern.GROUPTAB!
CREATE VIEW kern.I_BV     AS SELECT * FROM kern.INVLINE!
CREATE VIEW kern.IV_BV     AS SELECT * FROM kern.INVOICE!
COMMIT WORK!
CREATE VIEW kern.M_BV    AS SELECT * FROM kern.MANIFEST!
CREATE VIEW kern.ML_BV     AS SELECT * FROM kern.MANLINE!
CREATE VIEW kern.PO_BV          AS SELECT * FROM kern.PO!
CREATE VIEW kern.PL_BV      AS SELECT * FROM kern.POLINE!
COMMIT WORK!
CREATE VIEW kern.PR_BV     AS SELECT * FROM kern.PRODUCT!
CREATE VIEW kern.REGION_BV      AS SELECT * FROM kern.REGION!
CREATE VIEW kern.ST_BV       AS SELECT * FROM kern.STOCK!
CREATE VIEW kern.STR_BV   AS SELECT * FROM kern.STRUCTURE!
COMMIT WORK!
CREATE VIEW kern.SU_BV    AS SELECT * FROM kern.SUPPLIER!
CREATE VIEW kern.VSN_BV       AS SELECT * FROM kern.VS_NO!
* DERIVED VIEWS
CREATE VIEW kern.PUR_ASSM_STRUCTURE AS
     SELECT STR_ASSM pr_c_ASSM,
            ASSM.PR_DESC pr_d_ASSM,
            STR_COMP pr_c_COMP,
            ASSM.PR_DESC pr_d_COMP,
            STR_QNTY QNT_IN_AS
     FROM   kern.STR_BV, kern.PR_BV ASSM, kern.PR_BV COMP
     WHERE  ASSM.PR_CODE = STR_ASSM
       AND  COMP.PR_CODE = STR_COMP!
COMMIT WORK!
CREATE VIEW kern.PUR_PO_HISTORY AS
     SELECT PO_NO,
            PL_PO,
            PO_DATE,
            PO_DEL_DT,
            PO_SUPPL SU_NO,
            PL_PROD PR_CODE,
            PL_QNTY,
            PL_COST PL_P_COST,
            PO_TOTAL
     FROM   kern.PO_BV, kern.PL_BV
     WHERE  PL_PO        = PO_NO!
* WITH CHECK OPTION!
CREATE VIEW kern.PUR_PO_VALIDATE AS
     SELECT PO_NO,
            PO_DATE,
            PO_DEL_DT,
            SU_NO,
            SU_NAME,
            SU_ADDR,
            SU_CITY,
            SU_STATE,
            SU_ZIPCOD,
            PL_PROD PR_CODE,
            VSN_PNO SU_P_NO,
            PL_QNTY,
            PL_COST,
            PO_TOTAL
     FROM   kern.PO_BV, kern.SU_BV, kern.PL_BV, kern.VSN_BV
     WHERE  SU_NO    = PO_SUPPL
       AND  PL_PO      = PO_NO
       AND  VSN_SUPPL = PO_SUPPL
       AND  VSN_PROD     = PL_PROD!
CREATE VIEW kern.PUR_PROD_MAINT AS
     SELECT PR_CODE,   PR_DESC, PR_GROUP,
            GRO_CODE,     GRO_DESC,   PR_PRICE,
            PR_WH_QNT
     FROM   kern.PR_BV, kern.GROUP_BV
     WHERE  GRO_CODE = PR_GROUP!
* WITH CHECK OPTION!
CREATE VIEW kern.PUR_ST_MAINT AS
     SELECT PR_CODE, PR_DESC, PR_GROUP,
            GRO_CODE,   GRO_DESC,   PR_WH_QNT
     FROM   kern.PR_BV, kern.GROUP_BV
     WHERE  GRO_CODE = PR_GROUP!
* WITH CHECK OPTION!
CREATE VIEW kern.P_S_MAINT AS
     SELECT SU_NO,
            VSN_SUPPL,
            SU_NAME,
            SU_ADDR,
            SU_CITY,
            SU_STATE,
            SU_ZIPCOD,
            PR_CODE,
            VSN_PROD,
            PR_DESC,
            VSN_PNO SU_P_NO, 
            VSN_PCOST SU_P_COST
     FROM   kern.SU_BV, kern.VSN_BV, kern.PR_BV
     WHERE  VSN_SUPPL     = SU_NO
       AND  PR_CODE       = VSN_PROD!
CREATE VIEW kern.P_S_PROD AS
     SELECT SU_NO,
            SU_NAME,
            SU_ADDR,
            SU_CITY,
            SU_STATE,
            SU_ZIPCOD,
            VSN_PROD PR_CODE,
            PR_DESC,
            VSN_PNO SU_P_NO,
            VSN_PCOST SU_P_COST
     FROM   kern.SU_BV, kern.VSN_BV, kern.PR_BV
     WHERE  VSN_SUPPL     = SU_NO
       AND  PR_CODE       = VSN_PROD!
CREATE VIEW kern.PUR_WHERE_USED AS
     SELECT STR_COMP pr_c_COMP,
            COMP.PR_DESC pr_d_COMP,
            STR_ASSM pr_c_ASSM,
            ASSM.PR_DESC pr_d_ASSM,
            STR_QNTY QNT_IN_AS
     FROM   kern.STR_BV, kern.PR_BV ASSM, kern.PR_BV COMP
     WHERE  ASSM.PR_CODE = STR_ASSM
       AND  COMP.PR_CODE = STR_COMP!
CREATE VIEW kern.DMRECVG AS
     SELECT M_NO,    M_DATE,  B_NO,
            B_NAME,    B_ADDR,    B_CITY,
            B_STATE,   B_ZIPCODE, PR_CODE,
            PR_DESC,   ML_QNTY,   ML_VALUE,
            M_TOTAL
     FROM   kern.B_BV, kern.M_BV,
            kern.ML_BV, kern.PR_BV
     WHERE  M_BRANCH  = B_NO
       AND  ml_manif = M_NO
       AND  PR_CODE     = ML_PROD!
CREATE VIEW kern.DMSHIP AS
     SELECT M_NO,    M_DATE,  B_NO,
            B_NAME,    B_ADDR,    B_CITY,
            B_STATE,   B_ZIPCODE, PR_CODE,
            PR_DESC,   PR_GROUP,  GRO_DESC,
            ML_QNTY,   ML_VALUE,  M_TOTAL
     FROM   kern.B_BV, kern.M_BV,
            kern.ML_BV, kern.PR_BV,
            kern.GROUP_BV
     WHERE  M_BRANCH  = B_NO
       AND  ml_manif = M_NO
       AND  PR_CODE     = ML_PROD
       AND  GRO_CODE       = PR_GROUP!
CREATE VIEW kern.DIS_REQ_HISTORY AS
     SELECT M_NO,
            ml_manif,
            M_DATE,
            M_BRANCH B_NO,
            ML_PROD PR_CODE,
            ML_QNTY,
            ML_VALUE,
            M_TOTAL
     FROM   kern.M_BV, kern.ML_BV
     WHERE  ml_manif = M_NO!
COMMIT WORK!
CREATE VIEW kern.SAL_DELINQ_INVC AS
     SELECT IV_NO,
            IV_DATE,
            IV_BRANCH B_NO,
            IV_TOTAL,
            C_NO,
            C_NAME,
            C_ADDR,
            C_CITY,
            C_STATE,
            C_ZIPCODE
     FROM   kern.IV_BV, kern.C_BV
     WHERE  C_NO      = IV_CUSTOM!
CREATE VIEW kern.S_I_HIST AS
     SELECT IV_NO,
            I_INVOICE,
            IV_DATE,
            IV_CUSTOM C_NO,
            IV_BRANCH B_NO,
            IV_SLSPRS,
            I_PROD PR_CODE,
            I_QNTY,
            I_PRICE,
            IV_TOTAL
     FROM   kern.IV_BV, kern.I_BV
     WHERE  I_INVOICE  = IV_NO!
* WITH CHECK OPTION!
COMMIT WORK!
CREATE VIEW kern.S_I_VALID AS
     SELECT IV_NO,
            IV_DATE,
            IV_BRANCH B_NO,
            C_NO,
            C_NAME,
            IV_TOTAL
     FROM   kern.IV_BV, kern.C_BV
     WHERE  C_NO = IV_CUSTOM!
CREATE VIEW kern.SAL_PROD AS
     SELECT REGION_NO,
            REG_NAME,
            B_NO,
            B_NAME,
            PR_CODE,
            PR_DESC,
            PR_PRICE,
            ST_YT_SAL AMT_SD_YT
     FROM   kern.REGION_BV, kern.B_BV,
            kern.ST_BV, kern.PR_BV
     WHERE  B_REGION = REGION_NO
       AND  ST_BRANCH  = B_NO
       AND  PR_CODE  = ST_PROD!
COMMIT WORK!
insert kern.region values ('555','NEW ENGLAND')!
insert kern.region values ('444','MID-ATLANTIC')!
insert kern.region values ('222','SOUTH-EASTERN')!
insert kern.region values ('333','CENTRAL PLAINS')!
insert kern.region values ('111','GREAT LAKES')!
insert kern.region values ('000','MAIN WAREHOUSE')!
insert kern.region values ('666','CALIFORNIA')!
insert kern.region values ('777','TEXAS')!
insert kern.region values ('888','ROCKY MOUNTAINS')!
insert kern.region values ('999','NORTH-WESTERN')!
COMMIT WORK!
INSERT INTO kern.BRANCH VALUES (
'0000','DUMMY','DEFAULT BRANCH','CINCINNATI','OH','45211','000',
'00',00000,00,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2154','S CANYON','9104 GRAND BLVD.','DENVER','CO','80224','888',
'08',73000,13,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2144','FT WORTH','4560 KELLOGG AVE.','FORT WORTH','TX','75423','777',
'07',63000,09,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2134','IRVINE','9114 ST VIVIAN AVE','IRVINE','CA','92714','666',
'06',66000,09,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2135','BEECHER','16131 YUCATAN PLACE','LOS ANGELES','CA','92278',
'666','06',74000,11,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2141','DAYTONA','2 REGAL COURT','HOUSTON','TX','77079','777',
'07',74000,14,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2136','WESTWOOD','8217 JOHN WOODIN RD.','LOS ANGELES','CA','92209',
'666','06',57000,08,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2153','MEYER SQ','5617 MESA DRIVE','ALBUQUERQUE','NM','89230','888',
'08',61000,09,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2164','FLOSSMER','10655 4TH AVE N,E,','SEATTLE','WA','98004','999',
'09',70000,11,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2142','FAIRTON','245 J CARPENTER WAY','LOS CALENAS','TX','75343','777',
'07',68000,11,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2161','BELLVIEW','2365 KAPULTA MAU AVE','HONOLULU','HI','99821','999',
'09',82000,17,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2152','TEMPE','100 TEMPE-PHOENIX PK','TEMPE','AZ','85210','888',
'08',64000,10,NULL)!
INSERT INTO kern.BRANCH VALUES (
'2137','GAYLORD','509 OAKLAND BRDG RD','SAN FRANCISCO','CA','94105',
'666','06',69000,12,NULL)!
INSERT INTO kern.BRANCH VALUES (
'1272','ROCKAWAY','373 SUNRISE HIGHWAY','LNYBROOK L.I.','NY','11518',
'555','05',65000,10,NULL)!
INSERT INTO kern.BRANCH VALUES (
'1263','FULTON','4200 ARMORY AVE.','WASHINGTON','DC','22031','444',
'04',55000,08,NULL)!
COMMIT WORK!
*
insert kern.manifest values ('52221',01509.90,'2154','19860204')!
insert kern.manifest values ('04999',00959.50,'0000','19860308')!
insert kern.manifest values ('61141',00572.50,'2144','19860506')!
insert kern.manifest values ('90008',03008.70,'2134','19860204')!
insert kern.manifest values ('33391',01173.50,'2141','19870806')!
insert kern.manifest values ('12449',01970.00,'2153','19861110')!
insert kern.manifest values ('12441',01872.00,'2142','19861205')!
insert kern.manifest values ('91143',00397.15,'2161','19861210')!
insert kern.manifest values ('32993',01773.00,'2137','19860506')!
insert kern.manifest values ('48133',00959.75,'2136','19860201')!
COMMIT WORK!
INSERT INTO kern.CUSTOMER VALUES (
'000000','DUMMY (DEFAULT CUST)','ANY PLACE IN','NORTHERN','KY',
'41042','0000','ZZ','C7',0000)!
INSERT INTO kern.CUSTOMER VALUES (
'D11774','HARVEY FRIED','1123 ELMWOOD PIKE','OMAHA','NE',
'68106','2154','P3','A1',0250)!
INSERT INTO kern.CUSTOMER VALUES (
'F80081','TOM LANGDON','950 W. CARLTON ST.','MEMPHIS','TN',
'38117','2153','B4','A1',0500)!
INSERT INTO kern.CUSTOMER VALUES (
'P10099','BEN JOHNSON','HURSTBOURNE DR.','LOUISVILLE','KY',
'40201','2136','C7','A1',1000)!
INSERT INTO kern.CUSTOMER VALUES (
'S70703','TIM MARTIN','400 HARRINGTON BLVD','BUFFALO','NY',
'14240','2136','Z3','A1',0250)!
INSERT INTO kern.CUSTOMER VALUES (
'Y77793','JANE HESSLER','PO BOX 7444','DULUTH','MN',
'55802','2152','Q8','A1',0150)!
INSERT INTO kern.CUSTOMER VALUES (
'C74119','HENRY GIBSON','14 INDUSTRY DR.','DENVER','CO',
'80202','2161','C1','A1',1000)!
INSERT INTO kern.CUSTOMER VALUES (
'Z95551','PAUL ELLIOT','899 TEMPEST DR.','BRATTLEBORO','VT',
'05301','1263','C6','A1',0150)!
INSERT INTO kern.CUSTOMER VALUES (
'T78916','BRIAN DAVIS','1818 W.PAXTON','CENTERVILLE','AZ',
'86022','1263','D4','A1',0250)!
INSERT INTO kern.CUSTOMER VALUES (
'T44302','RON FLIGHT','4117 INDUSTRY LN.','CHICAGO','IL',
'60609','2154','Q4','A1',0500)!
INSERT INTO kern.CUSTOMER VALUES (
'T50118','AL BENJAMIN','500 FIST LANE','PHILADELPHIA ','PA',
'19104','2136','R7','A1',0750)!
INSERT INTO kern.CUSTOMER VALUES (
'H24899','SCOTT SIMPSON','14 INDUSTRIAL DR.','PITTSBURG','PA',
'15219','2142','X4','A1',0150)!
INSERT INTO kern.CUSTOMER VALUES (
'Z15504','TED LANGE','1009 ROUNDLAKE RD.','NEW YORK','NY',
'10038','2137','R6','A1',0250)!
INSERT INTO kern.CUSTOMER VALUES (
'Q88443','KAREN JONES','1000 MERCER','LEXINGTON','KY',
'40507','2134','J3','A1',0750)!
INSERT INTO kern.CUSTOMER VALUES (
'X00070','ANDREW FREEMAN','10 W.BROAD ST.','HAMPTON','IL',
'60631','2144','D5','A1',0500)!
COMMIT WORK!
**
insert kern.invoice values (
'1120','6302',00205.92,'0000','19860611','T78916')!
insert kern.invoice values (
'1157','5209',00011.94,'2154','19860711','T44302')!
insert kern.invoice values (
'1187','4105',00023.18,'2144','19861008','T50118')!
insert kern.invoice values (
'1140','4311',00075.00,'2134','19861209','H24899')!
insert kern.invoice values (
'2004','5401',00030.13,'2135','19860206','Z15504')!
insert kern.invoice values (
'1165','3602',00087.15,'2141','19861005','Q88443')!
insert kern.invoice values (
'2005','6405',00099.64,'2136','19860104','Z15504')!
insert kern.invoice values (
'1189','4105',00300.46,'2153','19861110','Z95551')!
insert kern.invoice values (
'1005','3407',00317.95,'2164','19861106','C74119')!
insert kern.invoice values (
'1025','7403',00039.00,'2164','19860201','Y77793')!
COMMIT WORK!
*
insert kern.GROUPTAB values ('SE','ALARMS AND SECURITY')!
insert kern.GROUPTAB values ('AV','AUDIO AND AUDIO ACCESSORIES')!
insert kern.GROUPTAB values ('TO','TOOLS AND HARDWARE')!
insert kern.GROUPTAB values ('PT','ELECTRONIC KITS AND PARTS')!
insert kern.GROUPTAB values ('VI','VIDEO AND VIDEO ACCESSORIES')!
insert kern.GROUPTAB values ('TW','TYPEWRITERS AND SUPPLIES')!
COMMIT WORK!
*
insert kern.product values (
'TI-3270-M','TI COLOUR MONITOR',00015,0199.95,'VI')!
insert kern.product values (
'TI-9948-V','TI VOICE SYNTHESIZER',00007,0069.95,'VI')!
insert kern.product values (
'TI-0064-K','TI 64K MEMORY CARD',00073,0239.95,'VI')!
insert kern.product values (
'TI-0128-K','TI 128K FLOPPIES',01912,0009.95,'VI')!
insert kern.product values (
'TI-9940-D','TI DISK DRIVE',00040,0139.95,'VI')!
insert kern.product values (
'TI-0058-J','TI JOY STICK',00398,0029.95,'VI')!
insert kern.product values (
'TI-0058-W','TI JOY WHEEL',00207,0029.95,'VI')!
insert kern.product values (
'TI-1001-C','TI POWER CORD',00092,0019.95,'VI')!
insert kern.product values (
'TI-1002-C','TI JOY/WHEEL CORD',00203,0019.95,'VI')!
insert kern.product values (
'TI-1003-C','TI EXPANSION BOX CRD',00086,0019.95,'VI')!
insert kern.product values (
'TI-1004-C','TI TAPE RECORDER CRD',00015,0019.95,'VI')!
COMMIT WORK!
INSERT INTO kern.SUPPLIER VALUES (
'734817','TIMSON INDUSTRIES','PO BOX 53624','DALLAS','TX',
'75227')!
INSERT INTO kern.SUPPLIER VALUES (
'734818','TIMSON INDUSTRIES','PO BOX 61048','DENVER','CO',
'80223')!
INSERT INTO kern.SUPPLIER VALUES (
'734819','TIMSON INDUSTRIES','PO BOX G114','BOSTON','MA',
'02167')!
INSERT INTO kern.SUPPLIER VALUES (
'700142','WESTERN ELECTRONICS ','119 W. FOURTH ST.','DETROIT','MI',
'48229')!
INSERT INTO kern.SUPPLIER VALUES (
'931013','JOHANSEN AND ASSOC.','4830 WARRING WAY','CINCINNATI','OH',
'45211')!
INSERT INTO kern.SUPPLIER VALUES (
'038940','CURTIS MANUFACTURING','222 E. ROLLY DR.','LOS ANGELES','CA',
'90044')!
INSERT INTO kern.SUPPLIER VALUES (
'973164','NELSON COMPANY','4317 SILVER MAPLE','TAMPA','FL',
'33608')!
INSERT INTO kern.SUPPLIER VALUES (
'078441','ANDREW INDUSTRIES','144 INDUSTRY DR.','BALTIMORE','MD',
'21205')!
INSERT INTO kern.SUPPLIER VALUES (
'049991','HARRISON ELECTRONICS','933 BENSON AVE.','RENO','NV',
'89508')!
INSERT INTO kern.SUPPLIER VALUES (
'147011','DOBSON ELECTRIC','148 BROWNWOOD BLVD.','DALLAS','TX',
'75203')!
INSERT INTO kern.SUPPLIER VALUES (
'637363','GLO-WORM ELECTRONICS','444 THREE MILE PLAZA','HARRISBURG','PA',
'17215')!
COMMIT WORK!
**
insert kern.stock values ('2154','TI-3270-M',0148,'03001',00096)!
insert kern.stock values ('2154','TI-9948-V',0244,'03002',00088)!
insert kern.stock values ('2154','TI-9940-D',0306,'03006',00013)!
insert kern.stock values ('2144','TI-9948-V',0407,'03005',00014)!
insert kern.stock values ('2144','TI-0064-K',0539,'03004',00079)!
insert kern.stock values ('2153','TI-0128-K',0605,'03003',00010)!
insert kern.stock values ('2137','TI-9940-D',0710,'03012',00020)!
insert kern.stock values ('1272','TI-0058-J',0813,'03011',00026)!
insert kern.stock values ('1263','TI-0058-J',0937,'03010',00074)!
insert kern.stock values ('0000','TI-0058-W',0112,'03009',00024)!
insert kern.stock values ('1263','TI-0058-W',0111,'03008',00022)!
insert kern.stock values ('1263','TI-9940-D',0210,'03007',00020)!
COMMIT WORK!
**
insert kern.manline values ('52221','TI-3270-M',175,0222.25)!
insert kern.maNLINe values ('04999','TI-3270-M',450,0162.00)!
insert kern.manline values ('61141','TI-9948-V',003,1797.00)!
insert kern.manline values ('90008','TI-9948-V',003,0004.47)!
insert kern.manline values ('33391','TI-9940-D',003,0014.97)!
insert kern.manline values ('12449','TI-9940-D',003,0036.42)!
insert kern.manline values ('12441','TI-0128-K',020,0199.40)!
insert kern.manline values ('91143','TI-1001-C',040,0199.60)!
insert kern.manline values ('32993','TI-1001-C',225,0081.00)!
insert kern.manline values ('48133','TI-1002-C',075,0172.50)!
COMMIT WORK!
**
Insert kern.vs_no values ('734817','TI-3270-M','CU3740',000.31)!
insert kern.vs_no values ('734818','TI-3270-M','CU5954',007.52)!
insert kern.vs_no values ('734819','TI-9948-V','CU8549',008.11)!
insert kern.vs_no values ('700142','TI-0064-K','CU1145',067.20)!
insert kern.vs_no values ('931013','TI-9940-D','CU8167',003.72)!
insert kern.vs_no values ('038940','TI-9940-D','CU0763',009.42)!
insert kern.vs_no values ('973164','TI-0058-W','CU2061',000.22)!
insert kern.vs_no values ('147011','TI-0058-W','CU3358',002.08)!
insert kern.vs_no values ('078441','TI-0058-J','CU4656',000.23)!
insert kern.vs_no values ('049991','TI-1001-C','CU7786',001.49)!
insert kern.vs_no values ('637363','TI-1001-C','CU0381',008.06)!
COMMIT WORK!
**
insert kern.invline values ('1120','TI-3270-M',001,0014.99)!
insert kern.invline values ('1120','TI-0064-K',050,0001.62)!
insert kern.invline values ('1157','TI-9948-V',005,0002.49)!
insert kern.invline values ('1157','TI-0128-K',001,0001.98)!
insert kern.invline values ('1187','TI-9940-D',005,0002.50)!
insert kern.invline values ('1187','TI-0058-J',002,0002.95)!
insert kern.invline values ('1157','TI-0058-J',001,0002.26)!
insert kern.invline values ('1140','TI-1001-C',001,0009.97)!
insert kern.invline values ('1165','TI-1003-C',002,0004.50)!
insert kern.invline values ('2005','TI-1002-C',001,0006.00)!
insert kern.invline values ('1025','TI-1004-C',001,0299.95)!
COMMIT WORK!
**
insert kern.po values ('152270',00391.00,'734818','19861001','19870104')!
insert kern.po values ('022271',07598.57,'734817','19860202','19860303')!
insert kern.po values ('043222',16604.00,'734817','19861002','19860203')!
insert kern.po values ('052483',04994.00,'734817','19861002','19860604')!
insert kern.po values ('661454',04388.00,'734818','19860702','19860903')!
insert kern.po values ('401375',00834.00,'734817','19861206','19861206')!
insert kern.po values ('740226',01369.50,'734817','19861210','19860611')!
insert kern.po values ('082277',00837.60,'734818','19870405','19870607')!
insert kern.po values ('059282',00623.70,'734817','19871101','19870202')!
insert kern.po values ('401079',06917.00,'734817','19860907','19861108')!
COMMIT WORK!
**
insert kern.poline values ('152270','TI-9948-V',010,00039.10)!
insert kern.poline values ('152270','TI-0064-K',021,00043.17)!
insert kern.poline values ('152270','TI-0128-K',005,00157.42)!
insert kern.poline values ('022271','TI-9940-D',010,00196.09)!
insert kern.poline values ('022271','TI-0128-K',800,00004.93)!
insert kern.poline values ('043222','TI-9940-D',100,00041.79)!
insert kern.poline values ('043222','TI-0058-W',200,00015.18)!
insert kern.poline values ('152270','TI-9940-D',100,00093.89)!
insert kern.poline values ('052483','TI-1001-C',200,00012.34)!
insert kern.poline values ('052483','TI-1002-C',200,00012.63)!
insert kern.poline values ('661454','TI-1003-C',100,00043.88)!
COMMIT WORK!
**
INSERT kern.STRUCTURE values ('TI-1001-C','TI-1001-C',02)!
insert kern.structure values ('TI-0058-W','TI-1001-C',03)!
insert kern.structure values ('TI-1001-C','TI-0058-W',02)!
insert kern.structure values ('TI-0058-J','TI-0058-J',02)!
insert kern.structure values ('TI-0128-K','TI-0128-K',01)!
insert kern.structure values ('TI-0128-K','TI-9940-D',01)!
insert kern.structure values ('TI-0128-K','TI-9948-V',02)!
insert kern.structure values ('TI-3270-M','TI-0064-K',01)!
insert kern.structure values ('TI-0064-K','TI-3270-M',01)!
insert kern.structure values ('TI-0058-J','TI-3270-M',02)!
insert kern.structure values ('TI-3270-M','TI-3270-M',01)!
insert kern.structure values ('TI-9940-D','TI-3270-M',01)!
COMMIT WORK!
*
* DERIVED VIEWS
*
SELECT * FROM kern.PUR_ASSM_STRUCTURE !
FETCH !

SELECT * FROM kern.PUR_PO_HISTORY !
FETCH !

SELECT * FROM kern.PUR_PO_VALIDATE !
FETCH !

SELECT * FROM kern.PUR_PROD_MAINT !
FETCH !

SELECT * FROM kern.PUR_ST_MAINT !
FETCH !

SELECT * FROM kern.P_S_MAINT ORDER BY su_no!
FETCH !

SELECT * FROM kern.P_S_PROD !
FETCH !

SELECT * FROM kern.pur_where_used ORDER BY pr_c_comp, pr_c_assm !
FETCH !

SELECT * FROM kern.DMRECVG !
FETCH !

SELECT * FROM kern.DMSHIP !
FETCH !

SELECT * FROM kern.DIS_REQ_HISTORY !
FETCH !

SELECT * FROM kern.SAL_CREDIT_MAINT !
FETCH !

SELECT * FROM kern.SAL_DELINQ_INVC !
FETCH !

SELECT * FROM kern.S_I_HIST !
FETCH !

SELECT * FROM kern.S_I_VALID !
FETCH !

SELECT * FROM kern.SAL_PROD !
FETCH !

COMMIT WORK !
select su_no, su_name, po_suppl, po_no from po, supplier
where po_suppl = su_no!
fetch!
*
select su_no, su_name, po_suppl, po_no from po, supplier
where po_suppl=su_no(+)!
fetch!
*
select su_no, su_name, po_suppl, po_no from po, supplier
where po_suppl(+)=su_no!
fetch!
*
select su_no, su_name, po_suppl, po_no from po, supplier
where po_suppl(+)=su_no(+)!
fetch!
*
fetch!
*
select * from manifest,manline
where m_no=ml_manif(+)!
*
fetch!
*
select * from manifest,manline
where m_total=ml_value(+)!
*
fetch!
*
select * from manifest,manline
where m_total=ml_value(+)
 and  m_no=ml_manif(+)!
*
fetch!
*
select * from manifest,manline
where m_total=ml_value(+)
 or   m_no=ml_manif(+)!
*
fetch!
*
select * from manifest,manline,poline
where m_total=ml_value(+)
 and  m_no=ml_manif(+)
 and  ml_value(+)=pl_cost!
*
select m_no,m_total
from manifest
union
select ml_manif,ml_value
from manline
order by 1, 2!
*
fetch!
*
select m_no,m_total
from manifest
except
select ml_manif,ml_value
from manline
order by 1!
*
fetch!
*
select su_no,po_suppl from supplier,po
where po_suppl=su_no(+)
union
select su_no,po_suppl from supplier,po
where po_suppl(+)=su_no
order by 1, 2 !
*
fetch!
*
select su_no,po_suppl from supplier,po
where po_suppl=su_no(+)
intersect
select su_no,po_suppl from supplier,po
where po_suppl(+)=su_no
order by 1 !
*
fetch!
*
select su_no,po_suppl from supplier,po
where po_suppl (+)=     su_no
intersect
select su_no,po_suppl from supplier,po
where po_suppl     =su_no(+)
order by 1 !
*
fetch!
*
select su_no,po_suppl from supplier,po
where po_suppl=su_no(+)
except
select su_no,po_suppl from supplier,po
where po_suppl(+)=su_no!
*
select su_no,po_suppl from supplier,po
where po_suppl(+)=su_no
except
select su_no,po_suppl from supplier,po
where po_suppl=su_no(+)
order by 2, 1!
*
fetch!
DROP DOMAIN DM_ADDRESSES !
DROP DOMAIN DM_BINS !
DROP DOMAIN DM_B_IDS !
DROP DOMAIN DM_CITIES !
DROP DOMAIN DM_CLASSIFICATIONS !
DROP DOMAIN DM_CREDITS !
DROP DOMAIN DM_C_IDS !
DROP DOMAIN DM_DATES !
commit work !
DROP DOMAIN DM_DESCRIPTIONS !
DROP DOMAIN DM_EMPLOYEE_IDS !
DROP DOMAIN DM_GRO_IDS !
DROP DOMAIN DM_IV_IDS !
DROP DOMAIN DM_M_IDS !
DROP DOMAIN DM_MONIES !
DROP DOMAIN DM_NAMES !
commit work !
DROP DOMAIN DM_PO_IDS !
DROP DOMAIN DM_PR_IDS !
DROP DOMAIN DM_QUANTITIES !
DROP DOMAIN DM_R_IDS !
DROP DOMAIN DM_REGION_IDS !
commit work !
DROP DOMAIN DM_ROUTES !
DROP DOMAIN DM_STATES !
DROP DOMAIN DM_SU_IDS !
DROP DOMAIN DM_ZIPCODES !
*
DROP TABLE kern.BRANCH !
DROP TABLE kern.CUSTOMER !
commit work !
DROP TABLE kern.GROUPTAB !
DROP TABLE kern.INVLINE !
commit work !
DROP TABLE kern.INVOICE !
DROP TABLE kern.MANIFEST !
commit work !
DROP TABLE kern.MANLINE !
DROP TABLE kern.PO !
commit work !
DROP TABLE kern.POLINE !
DROP TABLE kern.PRODUCT!
commit work !
DROP TABLE kern.REGION !
commit work !
DROP TABLE kern.STOCK !
DROP TABLE kern.STRUCTURE !
commit work !
DROP TABLE kern.SUPPLIER !
DROP TABLE kern.VS_NO !
commit work !
*
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
