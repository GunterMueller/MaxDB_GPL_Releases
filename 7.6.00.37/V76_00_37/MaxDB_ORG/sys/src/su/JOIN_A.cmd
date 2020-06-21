senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* A03INSER.vdnts
file connect ( sut sut ! 
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table kern.t1 (f1 fixed(4) key,f2 fixed(4) key,f3 float(6) key,
            f4 char (6), f5 char (12)) !
create view kern.s1 as select * from kern.t1
       where f2 >= 20  and f3 <= 1.e6 and f4 > 'f' with check option !
create synonym  syns1 for kern.s1 !
commit work !
insert kern.t1 values (1, 10, 1.1e1,'satz1', 'feld5' )!
insert kern.t1 values (2, 22, 1.1e2,'satz2', 'feld5' )!
insert kern.t1 values (3, 33, 1.1e3,'satz3', 'feld5' )!
insert kern.t1 values (4, 44, 1.1e4,'satz4', 'feld5' )!
insert kern.t1 values (5, 55, 1.1e5,'satz5', 'feld5' )!
insert kern.t1 values (6, 66, 1.1e6,'satz6', 'feld5' )!
commit work !
update kern.t1 set f4 = 'feld4' !
commit work !
declare erg1 cursor for 
select * from kern.t1 where f1 > 2 and f1 < 5 for reuse !
fetch erg1 !
declare erg2 cursor for 
select syn.f1 s1f1, f2 s1f2 , syn.f3 s1f3 , f4 s1f4 ,
      syn.f5 s1f5  from syns1  syn for reuse !
fetch erg2 !
declare erg4 cursor for 
select * from erg1, erg2
     where  erg1.f1 = erg2.s1f1  !
fetch erg4 !
drop synonym syns1!
drop table t1!
commit work !
CREATE TABLE T ( A CHAR (4) )!
INSERT T SET A = 'aa'!
INSERT T SET A = 'ff'!
INSERT T SET A = 'fe'!
INSERT T SET A = 'qq'!
CREATE TABLE T1 LIKE T !
create table t2 (a2 char (6))!
insert t2 set a2 = 'w'!
insert t1 select t.a from t, t2 where rowno < 3 !
select * from t1 !
fetch !
rollback release!
file connect ( kern test !
create table t ( a int, b char(8), c char(8))!
create table source ( a int, b int)!
insert source values (12,13)!
insert source values (13,14)!
commit!
* innser select a little bit more complex; has to work !
insert t values ((select s1.a * s2.b from source s1, source s2
                  where s1.a = s2.b), 'aaa', 'c1')!
* more than one result not allowed !                  
insert t values ((select s1.a + s2.b from source s1, source s2),
                    'aaa', 'c2')!
select * from t !
fetch !
rollback !
drop table t !
drop table source !
commit release !

file connect ( sut sut ! 
drop user kern !
commit work release !

*****************************************************************************
* A04SELEC.vdnts
file connect ( sut sut ! 
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE TABLE kern.T1 ( F1 CHAR (3), F2 FIXED(5),F3 FLOAT(6) )!
CREATE TABLE kern.T2 ( F1 CHAR (3), F2 FIXED(5), F3 FLOAT(6) )!
CREATE TABLE kern.T3 ( T1 CHAR (3), T2 FIXED(5), T3 FLOAT(6) )!
INSERT kern.T1 VALUES ('a', 1, 40 ) !
INSERT kern.T2 VALUES ('a', 1, 40 ) !
INSERT kern.T3 VALUES ('a', 1, 40 ) !
INSERT kern.T1 VALUES ('b', 2, 50 ) !
INSERT kern.T2 VALUES ('b', 2, 50 ) !
INSERT kern.T3 VALUES ('b', 2, 50 ) !
COMMIT WORK !
SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
WHERE T1.F1 = kern.T3.T1 !
FETCH !
SELECT T1.*,kern.T2.*  FROM kern.T2, kern.T1
   WHERE T1.F1=kern.T2.F1 !
FETCH !
declare e cursor for SELECT T1.*, kern.T3.*  FROM kern.T3 , kern.T1
WHERE T1.F1 = kern.T3.T1 !
FETCH E !
COMMIT WORK RELEASE !
file connect ( kern test sqlmode oracle !
create table t ( a char (8) primary key, b char (20), c char (12))!
insert into t values ('aaa', 'bbbaaa', 'cccaaa')!
insert into t values ('bbb', 'bbbbbb', 'cccbbb')!
insert into t values ('ccc', 'bbbccc', 'cccccc')!
insert into t values ('ddd', 'bbbddd', 'cccddd')!
insert into t values ('eee', 'bbbeee', 'ccceee')!
commit work release!
file connect ( kern test sqlmode oracle !
pars_then_ex !
data !
mselect t.*, t1.a d into :a, :b, :c, :d
   from t, t t1 where t.a = :a and t.a = t1.a !
32767
4
c 8 'ccc'
c 8 'fff'
c 8 'eee'
c 8 'mmm'

mselect t.*, t1.a d into :a, :b, :c, :d
   from t, t t1 where t.a = :a and t.a = t1.a !
32767
2
c 8 'fff'
c 8 'mmm'

nodata !
pars_execute !
drop table t !
commit work release !
file connect ( sut sut !
create user orauser password orauser dba !
commit release !
file connect (orauser orauser sqlmode oracle !
create table Item (
  TITLE	      VARCHAR (700)	,
  QUERY	      VARCHAR (  4)	,
  TYPE        NUMBER  ( 18)	,
  UPDATEDATE  DATE,
  CREATEDATE  DATE,
  A1  VARCHAR (100)	,
  A2  VARCHAR (100)	,
  A4  VARCHAR (100)	,
  A5  VARCHAR (100)	,
  A6  NUMBER  ( 18)	,
  A9  VARCHAR (100)	,
  A10 VARCHAR (100)	,
  A11 VARCHAR (100)	,
  A12 VARCHAR (100)	,
  A13 VARCHAR (100)	,
  A15 VARCHAR (100)	,
  A16 VARCHAR (100)	,
  A17 VARCHAR (100)	,
  A18 VARCHAR (100)	,
  A19 VARCHAR (100)	,
  A20 VARCHAR (100)	,
  A21 VARCHAR (100)	,
  A22 VARCHAR (100)	,
  A24 VARCHAR (128)	,
  itemid number(18))!
Insert into Item (a5, a6, itemid) values ( 840, 840, 333)!

create table Item_tl (
  ITEM_ITEMID  NUMBER(18),
  LANG         VARCHAR (4), 
    PRIMARY KEY (ITEM_ITEMID, LANG),
  CTX_DESC long,
  A3  VARCHAR (700),
  A7  VARCHAR (100),
  A8  LONG,
  A14 VARCHAR (100))!
Insert into Item_tl 
  (ITEM_ITEMID, LANG, A3  , A7  , A14  , ctx_desc)
  values (333 , 'US', 'a3', 'a7', 'a14', 'das ist der long-wert')!

create table Category_Item (
  ITEMID NUMBER	(18),
  CATID	 NUMBER	(18), 
    primary key (itemid, catid))!
Insert into Category_Item values (333, 123)!
create table  SDFSDF_POLICY(A5	VARCHAR	(700))!
insert into SDFSDF_POLICY values ('840')!

CREATE VIEW SDFSDF_POLICY_VL
AS SELECT ITEM.ITEMID, ITEM_TL.CTX_DESC
FROM ITEM, ITEM_TL, CATEGORY_ITEM
WHERE ITEM_TL.ITEM_ITEMID=ITEM.ITEMID AND 
       ITEM_TL.LANG ='US' AND 
       CATEGORY_ITEM.ITEMID=ITEM.ITEMID AND 
       ITEM_TL.ITEM_ITEMID=CATEGORY_ITEM.ITEMID
AND (A5
       IN (SELECT A5 FROM SDFSDF_POLICY))!
create table SDFSDF_CAT(CATID	NUMBER	(18))!
INSERT INTO SDFSDF_CAT ( catid)  
  SELECT distinct Category_Item.catid
  FROM SDFSDF_POLICY_VL, Category_Item 
  WHERE Category_Item.itemid=SDFSDF_POLICY_VL.itemid !
select * from SDFSDF_CAT !
fetch into :a !
commit release!
file connect ( sut sut !
create table session ( session fixed (10), language varchar (32)) !
grant all on session to public !
commit release !

senderid ODB !
file connect ( orauser orauser sqlmode oracle!
create table category (
CATID      number (18,0),
NAME       VARCHAR (700) not null,
TYPE       number (18,0) not null,
LANG       VARCHAR (4),
"KEY"      VARCHAR (700) not null,
TITLE      VARCHAR (200),
UPDATEDATE date,
"GRAPHIC"  VARCHAR (256),
CREATEDATE date,
PRIMARY KEY (CATID, LANG))!
insert into Category values 
 (0, 'Bleistift', 2, 'US', 'Büro','Stand-Alone XML'
  ,'2000-08-24 00:08:12.337000',null,'2000-08-19 00:18:12.337000')!
insert into Category values 
 (1, 'Bleistift', 2, 'DE', 'Büro','Stand-Alone XML'
 ,'2000-08-24 00:08:12.337000',null,'2000-08-19 00:18:12.337000')!
 * PTS 1107340 !
CREATE VIEW Category_vl
   AS SELECT X.*
        FROM Category X, SUT.SESSION S, domain.CONNECTPARAMETERS P
       WHERE P.SESSION = S.SESSION AND S.LANGUAGE = X.LANG !
commit release !

senderid ODB !
file connect ( orauser orauser sqlmode oracle !
insert into session select session, 'US' 
   from domain.CONNECTPARAMETERS !
pars_then_ex !
DECLARE CUR_61 CURSOR FOR SELECT MAX( updatedate ) 
        FROM <Category X, SUT.SESSION S, domain.CONNECTPARAMETERS P>
       WHERE P.SESSION = S.SESSION AND S.LANGUAGE = X.LANG !
fetch CUR_61 into :a !
DECLARE CUR_62 CURSOR FOR SELECT MAX( updatedate ) FROM Category_vl !
fetch CUR_62 into :a !
pars_execute !
commit release !
senderid STANDARD !
file connect ( orauser orauser !
create table descriptor (
constraint d_pk primary key (descid, lang), 
DESCID        FIXED (18),
CATID         FIXED (18) not null,
NAME          VARCHAR (700),
TYPE          FIXED (18) not null,
HIDDEN        FIXED (18), 
"SEQUENCE"    FIXED (18),
"LANG"        VARCHAR (4) ascii,
"KEY"         VARCHAR (700),
TITLE         VARCHAR (200),
SEARCHABLE    FIXED (18),
UPDATEDATE    TIMESTAMP,
DEFAULTVALUE  VARCHAR (700),
MULTI_VALUE_TYPE FIXED (18),
MULTI_VALUE_KEY  VARCHAR (256),
REQUIRED         FIXED (18), 
REFINABLE        FIXED (18),
CREATEDATE       TIMESTAMP,
NAME_IN_SEARCH   FIXED (18)) !
 
insert into descriptor (descid, catid, type, lang)
values (000,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (002,      0, 2, 'D')!
insert into descriptor (descid, catid, type, lang)
values (002,      0, 2, 'US')!
insert into descriptor (descid, catid, type, lang)
values (004,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (004,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (006,      0, 2, 'D')!
insert into descriptor (descid, catid, type, lang)
values (007,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (008,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (008,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (010,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (010,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (012,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (013,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (013,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (015,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (015,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (017,      0, 2, 'US')!
insert into descriptor (descid, catid, type, lang)
values (018,      0, 2, 'D')!
insert into descriptor (descid, catid, type, lang)
values (019,      0, 9, 'D')!
insert into descriptor (descid, catid, type, lang)
values (020,      0, 9, 'US')!
insert into descriptor (descid, catid, type, lang)
values (021,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (022,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (022,      0, 2, 'US')!
insert into descriptor (descid, catid, type, lang)
values (024,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (025,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (025,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (027,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (028,      0, 6, 'D')!
insert into descriptor (descid, catid, type, lang)
values (029,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (029,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (031,      0, 6, 'US')!
insert into descriptor (descid, catid, type, lang)
values (032,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (032,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (034,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (034,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (036,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (036,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (038,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (038,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (040,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (040,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (042,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (042,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (044,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (044,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (046,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (047,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (048,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (048,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (050,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (050,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (052,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (052,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (054,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (054,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (056,      0, 2, 'US')!
insert into descriptor (descid, catid, type, lang)
values (057,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (058,      0, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (059,      0, 2, 'D')!
insert into descriptor (descid, catid, type, lang)
values (060,      0, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (061, 100001, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (061, 100001, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (063, 100001, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (063, 100001, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (065, 100005, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (065, 100005, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (067, 100007, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (067, 100007, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (069, 100007, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (069, 100007, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (071, 100007, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (071, 100007, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (073, 100009, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (073, 100009, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (075, 100009, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (075, 100009, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (077, 100009, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (077, 100009, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (079, 100013, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (079, 100013, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (081, 100015, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (081, 100015, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (083, 100015, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (083, 100015, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (085, 100015, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (085, 100015, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (087, 100017, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (087, 100017, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (089, 100019, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (089, 100019, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (091, 100019, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (092, 100019, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (093, 100021, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (093, 100021, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (095, 100021, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (095, 100021, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (097, 100023, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (098, 100023, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (099, 100023, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (099, 100023, 0, 'US')!
commit !
insert into descriptor (descid, catid, type, lang)
values (100, 100025, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (100, 100025, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (101, 100029, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (101, 100029, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (102, 100029, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (102, 100029, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (103, 100031, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (103, 100031, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (104, 100033, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (104, 100033, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (110, 100035, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (110, 100035, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (111, 100035, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (111, 100035, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (112, 100035, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (112, 100035, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (113, 100037, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (113, 100037, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (114, 100037, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (114, 100037, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (120, 100037, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (120, 100037, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (121, 100039, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (121, 100039, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (122, 100039, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (122, 100039, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (123, 100039, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (123, 100039, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (130, 100039, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (130, 100039, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (131, 100039, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (131, 100039, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (133, 100039, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (133, 100039, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (135, 100039, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (135, 100039, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (136, 100039, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (136, 100039, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (140, 100041, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (140, 100041, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (141, 100047, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (141, 100047, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (142, 100047, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (142, 100047, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (143, 100051, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (143, 100051, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (144, 100051, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (144, 100051, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (150, 100051, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (150, 100051, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (151, 100053, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (151, 100053, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (152, 100053, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (152, 100053, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (153, 100055, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (153, 100055, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (155, 100055, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (155, 100055, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (160, 100057, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (160, 100057, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (162, 100057, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (162, 100057, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (164, 100057, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (164, 100057, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (165, 100057, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (165, 100057, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (167, 100059, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (167, 100059, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (170, 100059, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (170, 100059, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (171, 100059, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (171, 100059, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (172, 100063, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (172, 100063, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (173, 100063, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (173, 100063, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (174, 100065, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (174, 100065, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (175, 100065, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (175, 100065, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (176, 100067, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (176, 100067, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (177, 100067, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (177, 100067, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (178, 100069, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (178, 100069, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (179, 100069, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (179, 100069, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (180, 100069, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (180, 100069, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (181, 100071, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (181, 100071, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (182, 100071, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (182, 100071, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (183, 100073, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (183, 100073, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (184, 100073, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (184, 100073, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (185, 100075, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (185, 100075, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (186, 100075, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (186, 100075, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (187, 100075, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (187, 100075, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (188, 100075, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (188, 100075, 0, 'US')!
insert into descriptor (descid, catid, type, lang)
values (189, 100075, 0, 'D')!
insert into descriptor (descid, catid, type, lang)
values (189, 100075, 0, 'US')!
commit ! 
update descriptor set name = 
'der ist hier egal, soll nur den Record breit machen'!
commit !
update descriptor set name = name || ' ' || name || ' ' || name !
commit !
CREATE VIEW DESCRIPTOR_VL
   AS SELECT X.*
        FROM DESCRIPTOR X, SUT.SESSION S, domain.CONNECTPARAMETERS P
       WHERE P.SESSION = S.SESSION AND S.LANGUAGE = X.LANG !
commit release !

file connect ( orauser orauser sqlmode oracle !
pars_then_ex !
insert into session select session, 'US' 
   from domain.CONNECTPARAMETERS !
create index IDX_DESCRIPTOR_CATID on descriptor (CATID) !
create index IDX_DESCRIPTOR_KEY on descriptor (KEY) !
create index IDX_DESCRIPTOR_ID_NAME_TYPE 
   on descriptor (CATID, NAME, TYPE) !
create index IDX_DESCRIPTOR_ID_KEY_TYPE 
   on descriptor (CATID, KEY, TYPE) !
commit !
SELECT  CATID, TYPE,LANG FROM DESCRIPTOR_VL WHERE LANG = 'US'
  order by 2 !
fetch into :a, :b, :c !
select count(*) from DESCRIPTOR_VL !
fetch into :a !
select count(*) from DESCRIPTOR_VL where type > 0!
fetch into :a !
select count(type) from DESCRIPTOR_VL where type > 0!
fetch into :a !
select count (catid) from DESCRIPTOR_VL where type > 0!
fetch into :a !
pars_execute!
drop table descriptor!
commit release !

file connect ( orauser orauser !
create table Descriptor (
  type       fixed (3),
  title      char (20),
  createdate timestamp,
  lang       char (3))!
insert into Descriptor values (0, 'tom', timestamp, 'US')!
insert into Descriptor values (1, 'bob', timestamp, 'US')!
CREATE VIEW DESCRIPTOR_VL
   AS SELECT X.*
        FROM DESCRIPTOR X, SUT.SESSION S, domain.CONNECTPARAMETERS P
       WHERE P.SESSION = S.SESSION AND S.LANGUAGE = X.LANG !
commit release !

SENDERID ODB !
file connect ( orauser orauser sqlmode oracle !
pars_then_ex !
insert into session select session, 'US' 
   from domain.CONNECTPARAMETERS !

SELECT Count(*) FROM Descriptor_vl WHERE Descriptor_vl.type >'-1' AND
   Descriptor_vl.title ='bob' AND   Descriptor_vl.createdate >=
   TO_DATE('2000-05-10 17:20:07','YYYY-MM-DD HH24:MI:SS') !
fetch into :a !
pars_execute !
commit release !

file connect ( sut sut !
drop user orauser !
commit release !
* PTS 1122678-80 !
file connect ( kern test !
select t.dummy from dual t, dual t1 !
select t.dummy from dual as t, dual t1 !
select t1.dummy from dual t, dual as t1 !
senderid STANDARD !
rollback release !

*****************************************************************************
* A13MAX.vdnts
file connect ( kern test !
commit work !
CREATE TABLE maxrecord  (
key1   char (2) ascii  key,
key2   char (2) ascii  key,
key3   char (2) ascii  key,
key4   char (2) ascii  key,
key5   char (2) ascii  key,
key6   char (2) ascii  key,
key7   char (2) ascii  key,
key8   char (2) ascii  key,
key9   char (2) ascii  key,
key10  char (2) ascii  key,
g1field1  float (5),
g1field2  float (5),
g1field3  float (5),
g1field4  float (5),
g1field5  float (5),
g1field6  float (5),
g1field7  float (5),
g1field8  float (5),
g1field9  float (5),
g1field10 float (5),
g2field1  char (5) ascii,
g2field2  char (5) ascii,
g2field3  char (5) ascii,
g2field4  char (5) ascii,
g2field5  char (5) ascii,
g2field6  char (5) ascii,
g2field7  char (5) ascii,
g2field8  char (5) ascii,
g2field9  char (5) ascii,
g2field10 char (5) ascii,
txt1   char (20) ascii,
txt2   char (20) ascii,
txt3   char (20) ascii,
txt4   char (20) ascii,
txt5   char (20) ascii,
txt6   char (20) ascii,
txt7   char (20) ascii,
txt8   char (20) ascii,
txt9   char (20) ascii,
txt10  char (20) ascii,
num1   float (18),
num2   float (18),
num3   float (18),
num4   float (18),
num5   float (18),
num6   float (18),
num7   float (18),
num8   float (18),
num9   float (18),
num10  float (18),
f1  fixed (3),
f2  fixed (3),
f3  fixed (3),
f4  fixed (3),
f5  fixed (3),
f6  fixed (3),
f7  fixed (3),
f8  fixed (3),
f9  fixed (3),
f10 fixed (3))!
commit work !
ALTER  TABLE maxrecord  ADD(
n2g1f1  float (5),
n2g1f2  float (5),
n2g1f3  float (5),
n2g1f4  float (5),
n2g1f5  float (5),
n2g1f6  float (5),
n2g1f7  float (5),
n2g1f8  float (5),
n2g1f9  float (5),
n2g1f10 float (5),
n2g2f1  char (5) ascii,
n2g2f2  char (5) ascii,
n2g2f3  char (5) ascii,
n2g2f4  char (5) ascii,
n2g2f5  char (5) ascii,
n2g2f6  char (5) ascii,
n2g2f7  char (5) ascii,
n2g2f8  char (5) ascii,
n2g2f9  char (5) ascii,
n2g2f10 char (5) ascii,
n2txt1   char (2) ascii,
n2txt2   char (2) ascii,
n2txt3   char (2) ascii,
n2txt4   char (2) ascii,
n2txt5   char (2) ascii,
n2txt6   char (2) ascii,
n2txt7   char (2) ascii,
n2txt8   char (2) ascii,
n2txt9   char (2) ascii,
n2txt10  char (2) ascii,
n2num1   float (8),
n2num2   float (8),
n2num3   float (8),
n2num4   float (8),
n2num5   float (8),
n2num6   float (8),
n2num7   float (8),
n2num8   float (8),
n2num9   float (8),
n2num10  float (8),
n2f1  fixed (3),
n2f2  fixed (3),
n2f3  fixed (3),
n2f4  fixed (3),
n2f5  fixed (3),
n2f6  fixed (3),
n2f7  fixed (3),
n2f8  fixed (3),
n2f9  fixed (3),
n2f10 fixed (3))!
commit work !
ALTER  TABLE maxrecord  ADD(
a110g1f1  float (5),
a110g1f2  float (5),
a110g1f3  float (5),
a110g1f4  float (5),
a110g1f5  float (5),
a110g1f6  float (5),
a110g1f7  float (5),
a110g1f8  float (5),
a110g1f9  float (5),
a110g1f10 float (5),
a110g2f1  char (5) ascii,
a110g2f2  char (5) ascii,
a110g2f3  char (5) ascii,
a110g2f4  char (5) ascii,
a110g2f5  char (5) ascii,
a110g2f6  char (5) ascii,
a110g2f7  char (5) ascii,
a110g2f8  char (5) ascii,
a110g2f9  char (5) ascii,
a110g2f10 char (5) ascii,
a110txt1   char (4) ascii,
a110txt2   char (4) ascii,
a110txt3   char (4) ascii,
a110txt4   char (4) ascii,
a110txt5   char (4) ascii,
a110txt6   char (4) ascii,
a110txt7   char (4) ascii,
a110txt8   char (4) ascii,
a110txt9   char (4) ascii,
a110txt10  char (4) ascii,
a110num1   float (4),
a110num2   float (4),
a110num3   float (4),
a110num4   float (4),
a110num5   float (4),
a110num6   float (4),
a110num7   float (4),
a110num8   float (4),
a110num9   float (4),
a110num10  float (4),
a110f1  fixed (3),
a110f2  fixed (3),
a110f3  fixed (3),
a110f4  fixed (3),
a110f5  fixed (3),
a110f6  fixed (3),
a110f7  fixed (3),
a110f8  fixed (3),
a110f9  fixed (3),
a110f10 fixed (3))!
commit work !
ALTER  TABLE maxrecord  ADD(
a160g1f1  float (5),
a160g1f2  float (5),
a160g1f3  float (5),
a160g1f4  float (5),
a160g1f5  float (5),
a160g1f6  float (5),
a160g1f7  float (5),
a160g1f8  float (5),
a160g1f9  float (5),
a160g1f10 float (5),
a160g2f1  char (5) ascii,
a160g2f2  char (5) ascii,
a160g2f3  char (5) ascii,
a160g2f4  char (5) ascii,
a160g2f5  char (5) ascii,
a160g2f6  char (5) ascii,
a160g2f7  char (5) ascii,
a160g2f8  char (5) ascii,
a160g2f9  char (5) ascii,
a160g2f10 char (5) ascii,
a160txt1   char (4) ascii,
a160txt2   char (4) ascii,
a160txt3   char (4) ascii,
a160txt4   char (4) ascii,
a160txt5   char (4) ascii,
a160txt6   char (4) ascii,
a160txt7   char (4) ascii,
a160txt8   char (4) ascii,
a160txt9   char (4) ascii,
a160txt10  char (4) ascii,
a160num1   float (8),
a160num2   float (8),
a160num3   float (8),
a160num4   float (8),
a160num5   float (8),
a160num6   float (8),
a160num7   float (8),
a160num8   float (8),
a160num9   float (8),
a160num10  float (8),
a160f1  fixed (3),
a160f2  fixed (3),
a160f3  fixed (3),
a160f4  fixed (3),
a160f5  fixed (3),
a160f6  fixed (3),
a160f7  fixed (3),
a160f8  fixed (3),
a160f9  fixed (3),
a160f10 fixed (3))!
commit work !
ALTER  TABLE maxrecord  ADD(
b210g1f1  float (5),
b210g1f2  float (5),
b210g1f3  float (5),
b210g1f4  float (5),
b210g1f5  float (5),
b210g1f6  float (5),
b210g1f7  float (5),
b210g1f8  float (5),
b210g1f9  float (5),
b210g1f10 float (5),
b210g2f1  char (5) ascii,
b210g2f2  char (5) ascii,
b210g2f3  char (5) ascii,
b210g2f4  char (5) ascii,
b210g2f5  char (5) ascii,
b210g2f6  char (5) ascii,
b210g2f7  char (5) ascii,
b210g2f8  char (5) ascii,
b210g2f9  char (5) ascii,
b210g2f10 char (5) ascii,
b210txt1   char (2) ascii,
b210txt2   char (2) ascii,
b210txt3   char (2) ascii,
b210txt4   char (2) ascii,
b210txt5   char (2) ascii,
b210txt6   char (2) ascii,
b210txt7   char (2) ascii,
b210txt8   char (2) ascii,
b210txt9   char (2) ascii,
b210txt10  char (2) ascii,
b210num1   float (18),
b210num2   float (18),
b210num3   float (18),
b210num4   float (18),
b210num5   float (18),
b210num6   float (18),
b210num7   float (18),
b210num8   float (18),
b210num9   float (18),
b210num10  float (18),
b210f1  char (57) ascii,
b210f2  char (57) ascii,
b210f3  char (57) ascii,
b210f4  char (57) ascii)!
commit work release!
file connect ( kern test !
commit work   !
CREATE TABLE MAXrec1     (
key1   char (2) ascii key,
key2   char (2) ascii key,
key3   char (2) ascii key,
key4   char (2) ascii key,
key5   char (2) ascii key,
key6   char (2) ascii key,
key7   char (2) ascii key,
key8   char (2) ascii key,
key9   char (2) ascii key,
key10  char (2) ascii key,
g1field1  float (5),
g1field2  float (5),
g1field3  float (5),
g1field4  float (5),
g1field5  float (5),
g1field6  float (5),
g1field7  float (5),
g1field8  float (5),
g1field9  float (5),
g1field10 float (5),
g2field1  char (5) ascii,
g2field2  char (5) ascii,
g2field3  char (5) ascii,
g2field4  char (5) ascii,
g2field5  char (5) ascii,
g2field6  char (5) ascii,
g2field7  char (5) ascii,
g2field8  char (5) ascii,
g2field9  char (5) ascii,
g2field10 char (5) ascii,
txt1   char (20) ascii,
txt2   char (20) ascii,
txt3   char (20) ascii,
txt4   char (20) ascii,
txt5   char (20) ascii,
txt6   char (20) ascii,
txt7   char (20) ascii,
txt8   char (20) ascii,
txt9   char (20) ascii,
txt10  char (20) ascii,
num1   float (18),
num2   float (18),
num3   float (18),
num4   float (18),
num5   float (18),
num6   float (18),
num7   float (18),
num8   float (18),
num9   float (18),
num10  float (18),
f1  fixed (3),
f2  fixed (3),
f3  fixed (3),
f4  fixed (3),
f5  fixed (3),
f6  fixed (3),
f7  fixed (3),
f8  fixed (3),
f9  fixed (3),
f10 fixed (3))!
commit work  !
ALTER  TABLE maxrec1  ADD(
n2g1f1  float (5),
n2g1f2  float (5),
n2g1f3  float (5),
n2g1f4  float (5),
n2g1f5  float (5),
n2g1f6  float (5),
n2g1f7  float (5),
n2g1f8  float (5),
n2g1f9  float (5),
n2g1f10 float (5),
n2g2f1  char (5) ascii,
n2g2f2  char (5) ascii,
n2g2f3  char (5) ascii,
n2g2f4  char (5) ascii,
n2g2f5  char (5) ascii,
n2g2f6  char (5) ascii,
n2g2f7  char (5) ascii,
n2g2f8  char (5) ascii,
n2g2f9  char (5) ascii,
n2g2f10 char (5) ascii,
n2txt1   char (2) ascii,
n2txt2   char (2) ascii,
n2txt3   char (2) ascii,
n2txt4   char (2) ascii,
n2txt5   char (2) ascii,
n2txt6   char (2) ascii,
n2txt7   char (2) ascii,
n2txt8   char (2) ascii,
n2txt9   char (2) ascii,
n2txt10  char (2) ascii,
n2num1   float (8),
n2num2   float (8),
n2num3   float (8),
n2num4   float (8),
n2num5   float (8),
n2num6   float (8),
n2num7   float (8),
n2num8   float (8),
n2num9   float (8),
n2num10  float (8),
n2f1  fixed (3),
n2f2  fixed (3),
n2f3  fixed (3),
n2f4  fixed (3),
n2f5  fixed (3),
n2f6  fixed (3),
n2f7  fixed (3),
n2f8  fixed (3),
n2f9  fixed (3),
n2f10 fixed (3))!
commit work  !
ALTER  TABLE maxrec1  ADD(
a110g1f1  float (5),
a110g1f2  float (5),
a110g1f3  float (5),
a110g1f4  float (5),
a110g1f5  float (5),
a110g1f6  float (5),
a110g1f7  float (5),
a110g1f8  float (5),
a110g1f9  float (5),
a110g1f10 float (5),
a110g2f1  char (5) ascii,
a110g2f2  char (5) ascii,
a110g2f3  char (5) ascii,
a110g2f4  char (5) ascii,
a110g2f5  char (5) ascii,
a110g2f6  char (5) ascii,
a110g2f7  char (5) ascii,
a110g2f8  char (5) ascii,
a110g2f9  char (5) ascii,
a110g2f10 char (5) ascii,
a110txt1   char (4) ascii,
a110txt2   char (4) ascii,
a110txt3   char (4) ascii,
a110txt4   char (4) ascii,
a110txt5   char (4) ascii,
a110txt6   char (4) ascii,
a110txt7   char (4) ascii,
a110txt8   char (4) ascii,
a110txt9   char (4) ascii,
a110txt10  char (4) ascii,
a110num1   float (4),
a110num2   float (4),
a110num3   float (4),
a110num4   float (4),
a110num5   float (4),
a110num6   float (4),
a110num7   float (4),
a110num8   float (4),
a110num9   float (4),
a110num10  float (4),
a110f1  fixed (3),
a110f2  fixed (3),
a110f3  fixed (3),
a110f4  fixed (3),
a110f5  fixed (3),
a110f6  fixed (3),
a110f7  fixed (3),
a110f8  fixed (3),
a110f9  fixed (3),
a110f10 fixed (3))!
commit work  !
ALTER  TABLE maxrec1  ADD(
a160g1f1  float (5),
a160g1f2  float (5),
a160g1f3  float (5),
a160g1f4  float (5),
a160g1f5  float (5),
a160g1f6  float (5),
a160g1f7  float (5),
a160g1f8  float (5),
a160g1f9  float (5),
a160g1f10 float (5),
a160g2f1  char (5) ascii,
a160g2f2  char (5) ascii,
a160g2f3  char (5) ascii,
a160g2f4  char (5) ascii,
a160g2f5  char (5) ascii,
a160g2f6  char (5) ascii,
a160g2f7  char (5) ascii,
a160g2f8  char (5) ascii,
a160g2f9  char (5) ascii,
a160g2f10 char (5) ascii,
a160txt1   char (4) ascii,
a160txt2   char (4) ascii,
a160txt3   char (4) ascii,
a160txt4   char (4) ascii,
a160txt5   char (4) ascii,
a160txt6   char (4) ascii,
a160txt7   char (4) ascii,
a160txt8   char (4) ascii,
a160txt9   char (4) ascii,
a160txt10  char (4) ascii,
a160num1   float (8),
a160num2   float (8),
a160num3   float (8),
a160num4   float (8),
a160num5   float (8),
a160num6   float (8),
a160num7   float (8),
a160num8   float (8),
a160num9   float (8),
a160num10  float (8),
a160f1  fixed (3),
a160f2  fixed (3),
a160f3  fixed (3),
a160f4  fixed (3),
a160f5  fixed (3),
a160f6  fixed (3),
a160f7  fixed (3),
a160f8  fixed (3),
a160f9  fixed (3),
a160f10 fixed (3))!
commit work  !
ALTER  TABLE maxrec1  ADD(
b210g1f1  float (5),
b210g1f2  float (5),
b210g1f3  float (5),
b210g1f4  float (5),
b210g1f5  float (5),
b210g1f6  float (5),
b210g1f7  float (5),
b210g1f8  float (5),
b210g1f9  float (5),
b210g1f10 float (5),
b210g2f1  char (5) ascii,
b210g2f2  char (5) ascii,
b210g2f3  char (5) ascii,
b210g2f4  char (5) ascii,
b210g2f5  char (5) ascii,
b210g2f6  char (5) ascii,
b210g2f7  char (5) ascii,
b210g2f8  char (5) ascii,
b210g2f9  char (5) ascii,
b210g2f10 char (5) ascii,
b210txt1   char (2) ascii,
b210txt2   char (2) ascii,
b210txt3   char (2) ascii,
b210txt4   char (2) ascii,
b210txt5   char (2) ascii,
b210txt6   char (2) ascii,
b210txt7   char (2) ascii,
b210txt8   char (2) ascii,
b210txt9   char (2) ascii,
b210txt10  char (2) ascii,
b210num1   float (18),
b210num2   float (18),
b210num3   float (18),
b210num4   float (18),
b210num5   float (18),
b210num6   float (18),
b210num7   float (18),
b210num8   float (18),
b210num9   float (18),
b210num10  float (18),
b210f1  char (57) ascii,
b210f2  char (57) ascii,
b210f3  char (57) ascii,
b210f4  char (57) ascii)!
commit work release!
file connect ( kern test !
insert maxrecord  values (
'k1' , 'k2', 'k3'  ,'k4'  ,'k5'  , 'k6' , 'k7' , 'k8' , 'k9' , 'k0',
11   , 12  , 13    , 14   , 15   , 16   , 17   , 18   , 19   , 20  ,
'f21','f22','f23'  ,'f24' ,'f25' , 'f26', 'f27', 'f28', 'f29','f30',
't31','t32','t33'  ,'t34' ,'t35' , 't36', 't37', 't38', 't39','t40',
41   , 42  , 43    , 44   , 45   , 46   , 47   , 48   , 49   , 50  ,
51   , 52  , 53    , 54   , 55   , 56   , 57   , 58   , 59   , 60  ,
61   , 62  , 63    , 64   , 65   , 66   , 67   , 68   , 69   , 70  ,
't71','t72','t73'  ,'t74' ,'t75' , 't76', 't77', 't78', 't79','t80',
 '81', '82', '83'  , '84' , '85' ,  '86',  '87',  '88',  '89', '90',
91   , 92  , 93    , 94   , 95   , 96   , 97   , 98   , 99   , 100 ,
101  , 102 , 103   , 104  , 105  , 106  , 107  , 108  , 109  , 110 ,
111  , 112 , 113   , 114  , 115  , 116  , 117  , 118  , 119  , 120 ,
'121','122','123'  ,'124' ,'125' , '126', '127', '128', '129','130',
'131','132','133'  ,'134' ,'135' , '136', '137', '138', '139','140',
141  , 142 , 143   , 144  , 145  , 146  , 147  , 148  , 149  , 150 ,
151  , 152 , 153   , 154  , 155  , 156  , 157  , 158  , 159  , 160 ,
161  , 162 , 163   , 164  , 165  , 166  , 167  , 168  , 169  , 170 ,
'171','172','173'  ,'174' ,'175' , '176', '177', '178', '179','180',
'181','182','183'  ,'184' ,'185' , '186', '187', '188', '189','190',
191  , 192 , 193   , 194  , 195  , 196  , 197  , 198  , 199  , 200 ,
201  , 202 , 203   , 204  , 205  , 206  , 207  , 208  , 209  , 210 ,
211  , 212 , 213   , 214  , 215  , 216  , 217  , 218  , 219  , 220 ,
'221','222','223'  ,'224' ,'225' , '226', '227', '228', '229','230',
 '31', '32', '33'  , '34' , '35' ,  '36',  '37',  '38',  '39', '40',
241  , 242 , 243   , 244  , 245  , 246  , 247  , 248  , 249  , 250 ,
'feld251', 'feld252', 'feld253', 'feld254' ) !
insert maxrecord  values (
'a1' , 'a2', 'a3'  ,'a4'  ,'a5'  , 'a6' , 'a7' , 'a8' , 'a9' , 'a0',
11   , 12  , 13    , 14   , 15   , 16   , 17   , 18   , 19   , 20  ,
'f21','f22','f23'  ,'f24' ,'f25' , 'f26', 'f27', 'f28', 'f29','f30',
't31','t32','t33'  ,'t34' ,'t35' , 't36', 't37', 't38', 't39','t40',
41   , 42  , 43    , 44   , 45   , 46   , 47   , 48   , 49   , 50  ,
51   , 52  , 53    , 54   , 55   , 56   , 57   , 58   , 59   , 60  ,
61   , 62  , 63    , 64   , 65   , 66   , 67   , 68   , 69   , 70  ,
't71','t72','t73'  ,'t74' ,'t75' , 't76', 't77', 't78', 't79','t80',
 '81', '82', '83'  , '84' , '85' ,  '86',  '87',  '88',  '89', '90',
91   , 92  , 93    , 94   , 95   , 96   , 97   , 98   , 99   , 100 ,
101  , 102 , 103   , 104  , 105  , 106  , 107  , 108  , 109  , 110 ,
111  , 112 , 113   , 114  , 115  , 116  , 117  , 118  , 119  , 120 ,
'121','122','123'  ,'124' ,'125' , '126', '127', '128', '129','130',
'131','132','133'  ,'134' ,'135' , '136', '137', '138', '139','140',
141  , 142 , 143   , 144  , 145  , 146  , 147  , 148  , 149  , 150 ,
151  , 152 , 153   , 154  , 155  , 156  , 157  , 158  , 159  , 160 ,
161  , 162 , 163   , 164  , 165  , 166  , 167  , 168  , 169  , 170 ,
'171','172','173'  ,'174' ,'175' , '176', '177', '178', '179','180',
'181','182','183'  ,'184' ,'185' , '186', '187', '188', '189','190',
191  , 192 , 193   , 194  , 195  , 196  , 197  , 198  , 199  , 200 ,
201  , 202 , 203   , 204  , 205  , 206  , 207  , 208  , 209  , 210 ,
211  , 212 , 213   , 214  , 215  , 216  , 217  , 218  , 219  , 220 ,
'221','222','223'  ,'224' ,'225' , '226', '227', '228', '229','230',
 '31', '32', '33'  , '34' , '35' ,  '36',  '37',  '38',  '39', '40',
241  , 242 , 243   , 244  , 245  , 246  , 247  , 248  , 249  , 250 ,
'feld251', 'feld252', 'feld253', 'feld254' ) !
insert maxrecord  values (
'd1' , 'd2', 'd3'  ,'d4'  ,'d5'  , 'd6' , 'd7' , 'd8' , 'd9' , 'd0',
11   , 12  , 13    , 14   , 15   , 16   , 17   , 18   , 19   , 20  ,
'f21','f22','f23'  ,'f24' ,'f25' , 'f26', 'f27', 'f28', 'f29','f30',
't31','t32','t33'  ,'t34' ,'t35' , 't36', 't37', 't38', 't39','t40',
41   , 42  , 43    , 44   , 45   , 46   , 47   , 48   , 49   , 50  ,
51   , 52  , 53    , 54   , 55   , 56   , 57   , 58   , 59   , 60  ,
61   , 62  , 63    , 64   , 65   , 66   , 67   , 68   , 69   , 70  ,
't71','t72','t73'  ,'t74' ,'t75' , 't76', 't77', 't78', 't79','t80',
 '81', '82', '83'  , '84' , '85' ,  '86',  '87',  '88',  '89', '90',
91   , 92  , 93    , 94   , 95   , 96   , 97   , 98   , 99   , 100 ,
101  , 102 , 103   , 104  , 105  , 106  , 107  , 108  , 109  , 110 ,
111  , 112 , 113   , 114  , 115  , 116  , 117  , 118  , 119  , 120 ,
'121','122','123'  ,'124' ,'125' , '126', '127', '128', '129','130',
'131','132','133'  ,'134' ,'135' , '136', '137', '138', '139','140',
141  , 142 , 143   , 144  , 145  , 146  , 147  , 148  , 149  , 150 ,
151  , 152 , 153   , 154  , 155  , 156  , 157  , 158  , 159  , 160 ,
161  , 162 , 163   , 164  , 165  , 166  , 167  , 168  , 169  , 170 ,
'171','172','173'  ,'174' ,'175' , '176', '177', '178', '179','180',
'181','182','183'  ,'184' ,'185' , '186', '187', '188', '189','190',
191  , 192 , 193   , 194  , 195  , 196  , 197  , 198  , 199  , 200 ,
201  , 202 , 203   , 204  , 205  , 206  , 207  , 208  , 209  , 210 ,
211  , 212 , 213   , 214  , 215  , 216  , 217  , 218  , 219  , 220 ,
'221','222','223'  ,'224' ,'225' , '226', '227', '228', '229','230',
 '31', '32', '33'  , '34' , '35' ,  '36',  '37',  '38',  '39', '40',
241  , 242 , 243   , 244  , 245  , 246  , 247  , 248  , 249  , 250 ,
'feld251', 'feld252', 'feld253', 'feld254' ) !
insert maxrecord  values (
's1' , 's2', 's3'  ,'s4'  ,'s5'  , 's6' , 's7' , 's8' , 's9' , 's0',
11   , 12  , 13    , 14   , 15   , 16   , 17   , 18   , 19   , 20  ,
'f21','f22','f23'  ,'f24' ,'f25' , 'f26', 'f27', 'f28', 'f29','f30',
't31','t32','t33'  ,'t34' ,'t35' , 't36', 't37', 't38', 't39','t40',
41   , 42  , 43    , 44   , 45   , 46   , 47   , 48   , 49   , 50  ,
51   , 52  , 53    , 54   , 55   , 56   , 57   , 58   , 59   , 60  ,
61   , 62  , 63    , 64   , 65   , 66   , 67   , 68   , 69   , 70  ,
't71','t72','t73'  ,'t74' ,'t75' , 't76', 't77', 't78', 't79','t80',
 '81', '82', '83'  , '84' , '85' ,  '86',  '87',  '88',  '89', '90',
91   , 92  , 93    , 94   , 95   , 96   , 97   , 98   , 99   , 100 ,
101  , 102 , 103   , 104  , 105  , 106  , 107  , 108  , 109  , 110 ,
111  , 112 , 113   , 114  , 115  , 116  , 117  , 118  , 119  , 120 ,
'121','122','123'  ,'124' ,'125' , '126', '127', '128', '129','130',
'131','132','133'  ,'134' ,'135' , '136', '137', '138', '139','140',
141  , 142 , 143   , 144  , 145  , 146  , 147  , 148  , 149  , 150 ,
151  , 152 , 153   , 154  , 155  , 156  , 157  , 158  , 159  , 160 ,
161  , 162 , 163   , 164  , 165  , 166  , 167  , 168  , 169  , 170 ,
'171','172','173'  ,'174' ,'175' , '176', '177', '178', '179','180',
'181','182','183'  ,'184' ,'185' , '186', '187', '188', '189','190',
191  , 192 , 193   , 194  , 195  , 196  , 197  , 198  , 199  , 200 ,
201  , 202 , 203   , 204  , 205  , 206  , 207  , 208  , 209  , 210 ,
211  , 212 , 213   , 214  , 215  , 216  , 217  , 218  , 219  , 220 ,
'221','222','223'  ,'224' ,'225' , '226', '227', '228', '229','230',
 '31', '32', '33'  , '34' , '35' ,  '36',  '37',  '38',  '39', '40',
241  , 242 , 243   , 244  , 245  , 246  , 247  , 248  , 249  , 250 ,
'feld251', 'feld252', 'feld253', 'feld254' ) !
commit work release !
file connect ( kern test !
insert maxrec1  values (
'k1' , 'k2', 'k3'  ,'k4'  ,'k5'  , 'k6' , 'k7' , 'k8' , 'k9' , 'k0',
11   , 12  , 13    , 14   , 15   , 16   , 17   , 18   , 19   , 20  ,
'f21','f22','f23'  ,'f24' ,'f25' , 'f26', 'f27', 'f28', 'f29','f30',
't31','t32','t33'  ,'t34' ,'t35' , 't36', 't37', 't38', 't39','t40',
41   , 42  , 43    , 44   , 45   , 46   , 47   , 48   , 49   , 50  ,
51   , 52  , 53    , 54   , 55   , 56   , 57   , 58   , 59   , 60  ,
61   , 62  , 63    , 64   , 65   , 66   , 67   , 68   , 69   , 70  ,
't71','t72','t73'  ,'t74' ,'t75' , 't76', 't77', 't78', 't79','t80',
 '81', '82', '83'  , '84' , '85' ,  '86',  '87',  '88',  '89', '90',
91   , 92  , 93    , 94   , 95   , 96   , 97   , 98   , 99   , 100 ,
101  , 102 , 103   , 104  , 105  , 106  , 107  , 108  , 109  , 110 ,
111  , 112 , 113   , 114  , 115  , 116  , 117  , 118  , 119  , 120 ,
'121','122','123'  ,'124' ,'125' , '126', '127', '128', '129','130',
'131','132','133'  ,'134' ,'135' , '136', '137', '138', '139','140',
141  , 142 , 143   , 144  , 145  , 146  , 147  , 148  , 149  , 150 ,
151  , 152 , 153   , 154  , 155  , 156  , 157  , 158  , 159  , 160 ,
161  , 162 , 163   , 164  , 165  , 166  , 167  , 168  , 169  , 170 ,
'171','172','173'  ,'174' ,'175' , '176', '177', '178', '179','180',
'181','182','183'  ,'184' ,'185' , '186', '187', '188', '189','190',
191  , 192 , 193   , 194  , 195  , 196  , 197  , 198  , 199  , 200 ,
201  , 202 , 203   , 204  , 205  , 206  , 207  , 208  , 209  , 210 ,
211  , 212 , 213   , 214  , 215  , 216  , 217  , 218  , 219  , 220 ,
'221','222','223'  ,'224' ,'225' , '226', '227', '228', '229','230',
 '31', '32', '33'  , '34' , '35' ,  '36',  '37',  '38',  '39', '40',
241  , 242 , 243   , 244  , 245  , 246  , 247  , 248  , 249  , 250 ,
'feld251', 'feld252', 'feld253', 'feld254' ) !
insert maxrec1  values (
'a1' , 'a2', 'a3'  ,'a4'  ,'a5'  , 'a6' , 'a7' , 'a8' , 'a9' , 'a0',
11   , 12  , 13    , 14   , 15   , 16   , 17   , 18   , 19   , 20  ,
'f21','f22','f23'  ,'f24' ,'f25' , 'f26', 'f27', 'f28', 'f29','f30',
't31','t32','t33'  ,'t34' ,'t35' , 't36', 't37', 't38', 't39','t40',
41   , 42  , 43    , 44   , 45   , 46   , 47   , 48   , 49   , 50  ,
51   , 52  , 53    , 54   , 55   , 56   , 57   , 58   , 59   , 60  ,
61   , 62  , 63    , 64   , 65   , 66   , 67   , 68   , 69   , 70  ,
't71','t72','t73'  ,'t74' ,'t75' , 't76', 't77', 't78', 't79','t80',
 '81', '82', '83'  , '84' , '85' ,  '86',  '87',  '88',  '89', '90',
91   , 92  , 93    , 94   , 95   , 96   , 97   , 98   , 99   , 100 ,
101  , 102 , 103   , 104  , 105  , 106  , 107  , 108  , 109  , 110 ,
111  , 112 , 113   , 114  , 115  , 116  , 117  , 118  , 119  , 120 ,
'121','122','123'  ,'124' ,'125' , '126', '127', '128', '129','130',
'131','132','133'  ,'134' ,'135' , '136', '137', '138', '139','140',
141  , 142 , 143   , 144  , 145  , 146  , 147  , 148  , 149  , 150 ,
151  , 152 , 153   , 154  , 155  , 156  , 157  , 158  , 159  , 160 ,
161  , 162 , 163   , 164  , 165  , 166  , 167  , 168  , 169  , 170 ,
'171','172','173'  ,'174' ,'175' , '176', '177', '178', '179','180',
'181','182','183'  ,'184' ,'185' , '186', '187', '188', '189','190',
191  , 192 , 193   , 194  , 195  , 196  , 197  , 198  , 199  , 200 ,
201  , 202 , 203   , 204  , 205  , 206  , 207  , 208  , 209  , 210 ,
211  , 212 , 213   , 214  , 215  , 216  , 217  , 218  , 219  , 220 ,
'221','222','223'  ,'224' ,'225' , '226', '227', '228', '229','230',
 '31', '32', '33'  , '34' , '35' ,  '36',  '37',  '38',  '39', '40',
241  , 242 , 243   , 244  , 245  , 246  , 247  , 248  , 249  , 250 ,
'feld251', 'feld252', 'feld253', 'feld254' ) !
insert maxrec1  values (
'd1' , 'd2', 'd3'  ,'d4'  ,'d5'  , 'd6' , 'd7' , 'd8' , 'd9' , 'd0',
11   , 12  , 13    , 14   , 15   , 16   , 17   , 18   , 19   , 20  ,
'f21','f22','f23'  ,'f24' ,'f25' , 'f26', 'f27', 'f28', 'f29','f30',
't31','t32','t33'  ,'t34' ,'t35' , 't36', 't37', 't38', 't39','t40',
41   , 42  , 43    , 44   , 45   , 46   , 47   , 48   , 49   , 50  ,
51   , 52  , 53    , 54   , 55   , 56   , 57   , 58   , 59   , 60  ,
61   , 62  , 63    , 64   , 65   , 66   , 67   , 68   , 69   , 70  ,
't71','t72','t73'  ,'t74' ,'t75' , 't76', 't77', 't78', 't79','t80',
 '81', '82', '83'  , '84' , '85' ,  '86',  '87',  '88',  '89', '90',
91   , 92  , 93    , 94   , 95   , 96   , 97   , 98   , 99   , 100 ,
101  , 102 , 103   , 104  , 105  , 106  , 107  , 108  , 109  , 110 ,
111  , 112 , 113   , 114  , 115  , 116  , 117  , 118  , 119  , 120 ,
'121','122','123'  ,'124' ,'125' , '126', '127', '128', '129','130',
'131','132','133'  ,'134' ,'135' , '136', '137', '138', '139','140',
141  , 142 , 143   , 144  , 145  , 146  , 147  , 148  , 149  , 150 ,
151  , 152 , 153   , 154  , 155  , 156  , 157  , 158  , 159  , 160 ,
161  , 162 , 163   , 164  , 165  , 166  , 167  , 168  , 169  , 170 ,
'171','172','173'  ,'174' ,'175' , '176', '177', '178', '179','180',
'181','182','183'  ,'184' ,'185' , '186', '187', '188', '189','190',
191  , 192 , 193   , 194  , 195  , 196  , 197  , 198  , 199  , 200 ,
201  , 202 , 203   , 204  , 205  , 206  , 207  , 208  , 209  , 210 ,
211  , 212 , 213   , 214  , 215  , 216  , 217  , 218  , 219  , 220 ,
'221','222','223'  ,'224' ,'225' , '226', '227', '228', '229','230',
 '31', '32', '33'  , '34' , '35' ,  '36',  '37',  '38',  '39', '40',
241  , 242 , 243   , 244  , 245  , 246  , 247  , 248  , 249  , 250 ,
'feld251', 'feld252', 'feld253', 'feld254' ) !
insert maxrec1  values (
's1' , 's2', 's3'  ,'s4'  ,'s5'  , 's6' , 's7' , 's8' , 's9' , 's0',
11   , 12  , 13    , 14   , 15   , 16   , 17   , 18   , 19   , 20  ,
'f21','f22','f23'  ,'f24' ,'f25' , 'f26', 'f27', 'f28', 'f29','f30',
't31','t32','t33'  ,'t34' ,'t35' , 't36', 't37', 't38', 't39','t40',
41   , 42  , 43    , 44   , 45   , 46   , 47   , 48   , 49   , 50  ,
51   , 52  , 53    , 54   , 55   , 56   , 57   , 58   , 59   , 60  ,
61   , 62  , 63    , 64   , 65   , 66   , 67   , 68   , 69   , 70  ,
't71','t72','t73'  ,'t74' ,'t75' , 't76', 't77', 't78', 't79','t80',
 '81', '82', '83'  , '84' , '85' ,  '86',  '87',  '88',  '89', '90',
91   , 92  , 93    , 94   , 95   , 96   , 97   , 98   , 99   , 100 ,
101  , 102 , 103   , 104  , 105  , 106  , 107  , 108  , 109  , 110 ,
111  , 112 , 113   , 114  , 115  , 116  , 117  , 118  , 119  , 120 ,
'121','122','123'  ,'124' ,'125' , '126', '127', '128', '129','130',
'131','132','133'  ,'134' ,'135' , '136', '137', '138', '139','140',
141  , 142 , 143   , 144  , 145  , 146  , 147  , 148  , 149  , 150 ,
151  , 152 , 153   , 154  , 155  , 156  , 157  , 158  , 159  , 160 ,
161  , 162 , 163   , 164  , 165  , 166  , 167  , 168  , 169  , 170 ,
'171','172','173'  ,'174' ,'175' , '176', '177', '178', '179','180',
'181','182','183'  ,'184' ,'185' , '186', '187', '188', '189','190',
191  , 192 , 193   , 194  , 195  , 196  , 197  , 198  , 199  , 200 ,
201  , 202 , 203   , 204  , 205  , 206  , 207  , 208  , 209  , 210 ,
211  , 212 , 213   , 214  , 215  , 216  , 217  , 218  , 219  , 220 ,
'221','222','223'  ,'224' ,'225' , '226', '227', '228', '229','230',
 '31', '32', '33'  , '34' , '35' ,  '36',  '37',  '38',  '39', '40',
241  , 242 , 243   , 244  , 245  , 246  , 247  , 248  , 249  , 250 ,
'feld251', 'feld252', 'feld253', 'feld254' ) !
commit work release !
file connect ( kern test !
SELECT MAXREC1.KEY1, MAXREC1.KEY2, 
       MAXREC1.KEY3, MAXREC1.KEY4  
   FROM maxrecord, MAXREC1   WHERE
       MAXREC1.KEY1 <= 's'  AND  MAXREC1.A110F1 = maxrecord.A110F1  !
FETCH !
SELECT MAXREC1.KEY1, MAXREC1.KEY2, 
       MAXREC1.KEY3, MAXREC1.KEY4  
   FROM maxrecord, MAXREC1   WHERE
  maxrecord. KEY10 >= 's'  AND
  maxrec1. KEY1 <= 's'  AND  maxrec1.A110F1 = maxrecord.A110F1  !
FETCH !
SELECT MAXREC1.*               FROM maxrecord, MAXREC1   WHERE
  maxrecord. KEY10 >= 's'  AND
  MAXREC1. KEY1 <= 's'  AND  MAXREC1.A110F1 = maxrecord.A110F1  !
SELECT MAXREC1.* FROM MAXREC1, maxrecord  WHERE
MAXREC1.KEY1 = 'a1' AND maxrecord.KEY1 = MAXREC1.KEY1 !
FETCH !
commit work release!
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* A16PARS.vdnts
file connect ( sut sut ! 
create user kern password test dba not exclusive !
commit work release !
file CONNECT ( KERN TEST !
create table tab1 ( spalte1 char(5) key,
                    spalte2 char(5),
                    spalte3 char(5),
                    spalte4 fixed(10),
                    spalte5 fixed(10) ) !
create table tab2 ( col1 char(5) key,
                    col2 char(5),
                    col3 char(5),
                    col4 fixed(10),
                    col5 fixed(10) ) !
create index "tab2.col2"
	on tab2( col2 ) !
pars_then_ex !
data !
select col2, col4, spalte4 from tab2, tab1
    where col2 between :a and :b and col4 = spalte4!
c 5 '3' c 5 '4'

nodata !
pars_execute !
rollback release !
file CONNECT ( KERN TEST !
create table t ( a char (12)) !
create index "t.a"
	on t( a ) !
insert t set a = 'ddd'!
create table t1 ( b char (16))!
insert t1 set b = 'ddd'!
declare erg cursor for select max(b) from t, t1 where t.a = 'sss'!
fetch erg!
close erg!
parsing!
declare erg cursor for select max(b) from t, t1 where t.a = :a!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
fetch erg into :a!
close erg!
insert t set a = 'sss'!
declare erg cursor for select max(b) from t, t1 where t.a = 'sss'!
fetch erg!
close erg!
parsing!
declare erg cursor for select max(b) from t, t1 where t.a = :a!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
fetch erg into :a!
close erg!
delete t where a = 'sss'!
declare erg cursor for select b from t, t1 where t.a = 'sss'!
close erg!
parsing!
declare erg cursor for select b from t, t1 where t.a = :a!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
close erg!
rollback release !
pars_execute !
file CONNECT ( KERN TEST !
CREATE TABLE T ( A CHAR (7) ) !
CREATE TABLE T1 ( B CHAR (8) ) !
INSERT T SET A = 'aaa'!
INSERT T1 SET B = 'sss'!
SELECT * FROM T, T1 WHERE A LIKE 'a%' !
PARS_THEN_EX!
DATA !
SELECT * FROM T, T1 WHERE A LIKE :A   !
c 7 'a%'

NODATA !
INSERT T SET A = 'abc'!
*
INSERT T SET A = 'cvd'!
*
DATA !
SELECT * FROM T, T1 WHERE A LIKE :A   !
c 7 'a%'

NODATA !
FETCH INTO :A, :B!
pars_execute!
CREATE TABLE T2 ( A CHAR (7) ascii) !
INSERT T2 SET A = 'aaa'!
SELECT * FROM T2, T1 WHERE A LIKE 'a%' !
PARS_THEN_EX!
DATA !
SELECT * FROM T2, T1 WHERE A LIKE :A   !
c 7 'a%'

NODATA !
INSERT T2 SET A = 'abc'!
INSERT T2 SET A = 'cvd'!
DATA !
SELECT * FROM T2, T1 WHERE A LIKE :A   !
c 7 'a%'

NODATA !
FETCH INTO :A, :B!
pars_execute !
rollback release !
file CONNECT ( KERN TEST !
create table t ( a char (6)) !
create index "t.a"
	on t( a ) !
insert t set a = 'ddd'!
create table t1 ( b char (8))!
insert t1 set b = 'ddd'!
declare erg cursor for select max(b) from t, t1 where t.a = 'sss'!
fetch erg!
close erg!
parsing!
declare erg cursor for select max(b) from t, t1 where t.a = :b!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
fetch erg into :a!
close erg!
declare erg cursor for select b from t,t1 where t.a = 'sss'!
close erg!
parsing!
parse declare erg cursor for select b from t, t1 where t.a = :a!
data !
executing !
select !
1
c 6 'sss'

nodata !
pars_then_ex !
close erg!
pars_execute !
drop table t!
drop table t1!
commit release !
connect sut identified by sut !
drop user kern !
commit work release !

*****************************************************************************
* A17SEL.vdnts
file connect ( sut sut ! 
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( a char (8), b1 char (7))!
insert t1 set a = 'aaa', b1 = 'bbb'!
create table t2 (a fixed (3), b2  fixed (5))!
insert t2 set a = 234, b2 = 12345 !
 * das geht komischerweise in echt Oracle 8 nicht !
select t1.*, t2.b2 from t1, t2 order by a !
fetch into :a, :b, :c!
 * das muß ja nun gehen !
select t1.a, t2.b2 from t1,t2 order by a !
fetch into :a, :b !  
 * das sollte ja nun wirklich gehen !
select * from t1, t2 order by t1.a !
fetch into :a, :b, :c, :d !
rollback release !
file connect ( kern test !
CREATE TABLE T ( A CHAR (4) KEY, B CHAR (7), C FIXED (3))!
INSERT T VALUES ('AAA', 'ERTYUIO', 1)!
INSERT T VALUES ('AAB', 'ERTYUIO', 4)!
INSERT T VALUES ('CCC', 'Q'      , 1)!
SELECT T1.A, T2.A, T1.B, T1.C FROM T T1, T T2 ORDER BY 4, 1, 2 !
FETCH !
SELECT * FROM T T1, T T2 ORDER BY T1.C !
FETCH !
SELECT T1.A, T2.A, T1.B FROM T T1, T T2 ORDER BY T1.C, 1, 2 !
FETCH !
drop table t !
COMMIT RELEASE !
file connect ( kern test !
CREATE TABLE RVP006 (OBJ_NR FIXED(6) KEY,
BEN_NAME CHAR(8) KEY,
BK_BEZ CHAR(60)  KEY)!
CREATE TABLE RVP011 (AUFG_BEZ_OBJ CHAR(60) KEY,
OBJ_NR FIXED (6) NOT NULL,
AUFG_ART CHAR(16) NOT NULL)!
COMMIT !
PARS_THEN_EX !
DATA !
DECLARE DUMMY2 CURSOR FOR       
SELECT DISTINCT RVP011.AUFG_BEZ_OBJ, AUFG_ART
FROM RVP006, RVP011
WHERE RVP006.OBJ_NR = RVP011.OBJ_NR
AND RVP006.BEN_NAME = :A
ORDER BY RVP011.AUFG_ART, AUFG_BEZ_OBJ !
c 8 '0d1196'

nodata !
fetch dummy2 into :a, :b!
pars_execute!
drop table rvp006 !
drop table rvp011 !
commit work release !
file connect ( kern test !
create table t ( a char(5))!
create table t1 (b char (7) ascii key)!
insert t set a = 'aaa'!
insert t set a = 'bbb'!
insert t set a = 'ccc'!
insert t set a = 'ddd'!
insert t1 set b = 'aaa'!
insert t1 set b = 'ccc'!
select * from t, t1 where t.a = t1.b!
fetch!
select * from t, t1 where t1.b = t.a!
fetch!
create table t2 (b char (7) ascii key,
                 c fixed (8) key, d char (7) ascii)!
insert t2 values ('bbb', 4, 'bbb')!
insert t2 values ('bbb', 9, 'ddd')!
insert t2 values ('ccc', 9, 'eee')!
insert t2 values ('ddd', 9, 'ddd')!
select * from t, t2 where t.a = t2.b!
fetch !
select * from t, t2 where t2.b = t.a!
fetch !
select * from t, t2 where t.a = t2.d!
fetch !
select * from t, t2 where t2.d = t.a!
fetch !
create index "t2.d"
	on t2( d ) !
select * from t, t2 where t.a = t2.d!
fetch !
select * from t, t2 where t2.d = t.a!
fetch !
drop table t !
drop table t1 !
drop table t2 !
commit release !
file connect ( kern test !
create table aa (
aadstnr    char (5 ) key,
aadat      char (6 ) key,
aalfdnr    char (5 ) key,
aaswa      char (12) ,
aaadat     char (6 ) ,
aabkza     char (7 ) ,
aastatkn   char (1 ) ,
aaanzvv    char (3 ) ,
aagst      char (3 ) ,
aaost      char (3 ) ,
aaostw     char (4 ) ,
aaostm     char (4 ) ,
aaaehz     char (4 ))!
create table ab (
abswb      char (12) key,
abbkzb     char (7 ) ,
abregsb1   char (3 ) ,
abortknb   char (3 ))!
insert aa set aadstnr = '12345', aadat = '050687', aalfdnr = '1',
  aaswa = 'abcdef', aabkza = '52301', aastatkn = 'O', aaanzvv = '002',
  aaostm = '1', aaost = '2' !
insert aa set aadstnr = '12346', aadat = '050687', aalfdnr = '1',
  aaswa = 'abcdef', aabkza = '52301', aastatkn = 'O', aaanzvv = '014',
  aaostm = '1', aaost = '2' !
insert ab set abswb = 'abcdef', abbkzb = '52301',
  abregsb1 = 'A05'!
insert ab set abswb = 'fegrhf', abbkzb = '52301',
  abregsb1 = 'B05'!
commit!
select aa.aadstnr, aadat, aastatkn, aaost, aaostm, aaanzvv, ab.abregsb1
  from aa, ab
  where
        (aa.aaost <> ' ' and aa.aaostm < aa.aaost)
    and aa.aaanzvv >= '005'
    and aa.aastatkn = 'O'
    and ab.abregsb1 = 'A05'
    and aa.aaswa = ab.abswb and aa.aabkza = ab.abbkzb
  order by aa.aaanzvv !
fetch!
select aa.aadstnr, aadat, aastatkn, aaost, aaostm, aaanzvv, ab.abregsb1
  from aa, ab
  where
        aa.aaanzvv >= '005'
    and aa.aastatkn = 'O'
    and (aa.aaost <> ' ' and aa.aaostm < aa.aaost)
    and ab.abregsb1 = 'A05'
    and aa.aaswa = ab.abswb and aa.aabkza = ab.abbkzb
  order by aa.aaanzvv !
fetch!
drop table aa !
drop table ab !
commit release !
file connect ( kern test !
create table t ( a char (6))!
create table t1 ( b char (5))!
insert t set a = 'aaa'!
insert t1 set b = 'aaa'!
pars_then_ex !
declare erg cursor for select a&b from t1, t!
fetch erg!
close erg!
declare erg cursor for select a&b from t, t1 !
fetch erg!
pars_execute!
drop table t !
drop table t1 !
commit release !
file connect ( kern test !
create table t ( a char (8))!
insert t set a = 'aaa'!
create table t1 (a1 char (8))!
insert t1 set a1 = 'f'!
select * from t, t1 where a like 'aaa' and
(a = a1 or a1 = 'f')
order by a !
fetch1 pos(1) into :a, :b !
rollback release!
file connect ( kern test !
create table t ( a fixed (5),
                 b fixed (5),
                 c fixed (5),
                 d fixed (5))!
insert t set a = 1, b = 0, c = 3, d = 0!
create index i1 on t (a, b)!
rollback release !
file connect ( sut sut !
create user u1 password u1 dba !
commit release !
file connect ( kern test !
create user u password u resource !
create table t ( a char (8) key,
  b char (8))!
insert t set a = 'aaa', b = 'bbb'!
grant all on t to u, u1 !
commit release !

file connect ( u u !
create table t ( a char (8) key,
  b char (8))!
insert t set a = 'aaa', b = 'bbb'!
select * from t, kern.t where t.a = 'aaa'!
select * from t, kern.t where kern.t.a = 'aaa'!
fetch !
select * from t, kern.t where superdba.t.a = 'aaa'!
fetch !
grant all on t to u1 !
commit release !
file connect ( u1 u1 !
create table t ( a2 char (8))!
insert t set a2 = 'aaa'!
select * from t, u.t where u1.t.a2 = 'aaa'!
select * from t, u.t where u.t.a = 'aaa'!
fetch!
select * from t , u.t where u1.t.a2 = 'aaa'!
fetch!
select * from t , kern.t where t.a2 = 'aaa'!
fetch!
select * from t , kern.t where kern.t.a = 'aaa'!
fetch!
select * from t , kern.t where u1.t.a2 = 'aaa'!
fetch!
drop table t !
create view t as select u.t.a from  t, u.t !
drop view t !
create view t as select a from  u.t !
commit release !
file connect ( sut sut !
drop user u1 !
commit release !
file connect ( kern test !
drop table t !
drop user u !
commit release !
file CONNECT (kern test!
CREATE TABLE TABX
( K FIXED(5),
  I FIXED(6),
  N FIXED(7),
  PRIMARY KEY( K,N ))!
INSERT INTO TABX VALUES( 1, 1, 1)!
CREATE TABLE TABY
( K FIXED(8),
  I FIXED(9),
  N FIXED(10),
  PRIMARY KEY( K,N ))!
INSERT INTO TABY VALUES( 1, 1, 1)!
CREATE VIEW C1
AS SELECT T2.K,T1.I,T1.N FROM TABX T1, TABY T2
WHERE T1.K = 1 OR T2.N = 1 !
SELECT * FROM C1!
FETCH!
PARSING!
DECLARE ERG CURSOR FOR  
SELECT T1.K COL1, T1.N COL2 FROM TABX T1
WHERE T1.K
IN (SELECT T3.N FROM C1 T3 ) FOR REUSE!
EXECUTING!
DATA!
SELECT     X!
1

pars_then_ex !
fetch erg into :a, :b, :c !
nodata !
pars_execute!
rollback release!
file connect ( kern test !
create table t ( a char (3),  b char (8), primary key (e,b),
c varchar (10), d varchar (10), e varchar (12))!
insert into t values ('001', 'b0 ', 'ccc', 'ddd',  'SUPER')!
insert into t values ('001', 'b1 ', 'ccc', 'ddd',  'NICHT-SUPER')!
insert into t values ('001', 'b2 ', 'ccc', 'ddd',  'SUPER')!
create table t1 ( a char (8))!
insert into t1 values ('unsinn')!
pars_execute !
create index i1 on t (e) !
pars_then_ex !
data !
select * from t,t1 where t.a = :a and not e = :e !
c 3 '001'
c 12 'SUPER'

nodata !
fetch into :a, :b, :c, :d, :e, :f!
pars_execute !
rollback !
create table t ( a char (3),  b char (8), primary key (e,b),
c varchar (10), d varchar (10), e int)!
insert into t values ('001', 'b0 ', 'ccc', 'ddd',  0)!
insert into t values ('001', 'b1 ', 'ccc', 'ddd',  1)!
insert into t values ('001', 'b2 ', 'ccc', 'ddd',  0)!
create table t1 ( a char (8))!
insert into t1 values ('unsinn')!
create index i1 on t (e) !
select * from t,t1 where t.a = '001' and not 3*e = 0 !
fetch!
rollback release !

file connect ( sut sut !
drop user kern!
commit release !

*****************************************************************************
