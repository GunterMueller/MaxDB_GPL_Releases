file connect ( sut sut !
create user orauser password orauser dba !
commit release !

file connect (orauser orauser sqlmode oracle !

* PTS 1106650 !

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

commit !

* PTS 1106647 !

lineinfo off !
 * im Unicode-Umfeld muss der Key unter 1024 gedrückt werden,
 * bei Nicht-Unicode brauchen wir ihn aber mit 300, also
 * muss das erste create table im Nicht-Unicode-Umfeld Fehler liefern,
 * bei Unicode das zweite !
create table t (
 fixcol number (2),
 varcol char (40),
 c250_1 char (250) ,
 l2000_1 char (2000),
 l2000_2 char (2000),
 c250_2 char (250) UNICODE,
 primary key (c250_1,c250_2 )) !
create table t (
 fixcol number (2),
 varcol char (40),
 c250_1 char (250) ,
 l2000_1 char (2000),
 l2000_2 char (2000),
 c250_2 char (300) ASCII,
 primary key (c250_1,c250_2 )) !
insert into t values (0,'Admin','c1 a', 'l1', 'l2', 'sapcomb')!
insert into t values (0,'Admin','c1 b', 'l1', 'l2', 'sapcomb')!
insert into t values (0,'Admin','c1 c', 'l1', 'l2', 'sapcomb')!
insert into t values (0,'Admin','c1 d', 'l1', 'l2', 'sapcomb')!
insert into t values (0,'Admin','c1 e', 'l1', 'l2', 'sapcomb')!
insert into t values (0,'Admin','c1 f', 'l1', 'l2', 'sapcomb')!
insert into t values (0,'Admin','c1 g', 'l1', 'l2', 'sapcomb')!
insert into t values (0,'Admin','c1 h', 'l1', 'l2', 'sapcomb')!
commit !
select c250_1,l2000_1,l2000_2,c250_2  from t 
where c250_2 = 'sapcomb'
  and fixcol = 0
  and varcol = 'Admin'
order by c250_1 for update !
mfetch into :a, :b, :c, :d!
10
lineinfo on !
commit release !
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

* PTS 1107409 !

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
* values (000, 100077, 0, 'D')!
* values (000, 100077, 0, 'US')!
* values (000, 100077, 0, 'D')!
* values (000, 100077, 0, 'US')!
* values (000, 100077, 0, 'D')!
* values (000, 100077, 0, 'US')!
* values (000, 100079, 0, 'D')!
* values (000, 100079, 0, 'US')!
* values (000, 100079, 0, 'D')!
* values (000, 100079, 0, 'US')!
* values (000, 100079, 0, 'D')!
* values (000, 100079, 0, 'US')!
* values (000, 100079, 0, 'D')!
* values (000, 100079, 0, 'US')!
* values (000, 100081, 0, 'D')!
* values (000, 100081, 0, 'US')!
* values (000, 100081, 0, 'D')!
* values (000, 100081, 0, 'US')!
* values (000, 100081, 0, 'D')!
* values (000, 100081, 0, 'US')!
* values (000, 100081, 0, 'D')!
* values (000, 100081, 0, 'US')!
* values (000, 100081, 0, 'D')!
* values (000, 100081, 0, 'US')!
* values (000, 100083, 0, 'D')!
* values (000, 100083, 0, 'US')!
* values (000, 100083, 0, 'D')!
* values (000, 100083, 0, 'US')!
* values (000, 100087, 0, 'D')!
* values (000, 100087, 0, 'US')!
* values (000, 100087, 0, 'D')!
* values (000, 100087, 0, 'US')!
* values (000, 100087, 0, 'D')!
* values (000, 100087, 0, 'US')!
* values (000, 100089, 0, 'D')!
* values (000, 100089, 0, 'US')!
* values (000, 100089, 0, 'D')!
* values (000, 100089, 0, 'US')!
* values (000, 100089, 0, 'D')!
* values (000, 100089, 0, 'US')!
* values (000, 100089, 0, 'D')!
* values (000, 100089, 0, 'US')!
* values (000, 100091, 0, 'D')!
* values (000, 100091, 0, 'US')!
* values (000, 100093, 0, 'D')!
* values (000, 100093, 0, 'US')!
* values (000, 100093, 0, 'D')!
* values (000, 100093, 0, 'US')!
* values (000, 100095, 0, 'D')!
* values (000, 100095, 0, 'US')!
* values (000, 100095, 0, 'D')!
* values (000, 100095, 0, 'US')!
* values (000, 100095, 0, 'D')!
* values (000, 100095, 0, 'US')!
* values (000, 100097, 0, 'D')!
* values (000, 100097, 0, 'US')!
* values (000, 100099, 0, 'D')!
* values (000, 100099, 0, 'US')!
* values (000, 100099, 0, 'D')!
* values (000, 100099, 0, 'US')!
* values (000, 100099, 0, 'D')!
* values (000, 100099, 0, 'US')!
* values (000, 100099, 0, 'D')!
* values (000, 100099, 0, 'US')!
* values (000, 100099, 0, 'D')!
* values (000, 100099, 0, 'US')!
* values (000, 100101, 0, 'D')!
* values (000, 100101, 0, 'US')!
* values (000, 100101, 0, 'D')!
* values (000, 100101, 0, 'US')!
* values (000, 100107, 0, 'D')!
* values (000, 100107, 0, 'US')!
* values (000, 100107, 0, 'D')!
* values (000, 100107, 0, 'US')!
* values (000, 100109, 0, 'D')!
* values (000, 100109, 0, 'US')!
* values (000, 100109, 0, 'D')!
* values (000, 100109, 0, 'US')!
* values (000, 100109, 0, 'D')!
* values (000, 100109, 0, 'US')!
* values (000, 100111, 0, 'D')!
* values (000, 100111, 0, 'US')!
* values (000, 100113, 0, 'D')!
* values (000, 100113, 0, 'US')!
* values (000, 100113, 0, 'D')!
* values (000, 100113, 0, 'US')!
* values (000, 100113, 0, 'D')!
* values (000, 100113, 0, 'US')!
* values (000, 100115, 0, 'D')!
* values (000, 100115, 0, 'US')!
* values (000, 100115, 0, 'D')!
* values (000, 100115, 0, 'US')!
* values (000, 100115, 0, 'D')!
* values (000, 100115, 0, 'US')!
* values (000, 100115, 0, 'D')!
* values (000, 100115, 0, 'US')!
* values (000, 100117, 0, 'D')!
* values (000, 100117, 0, 'US')!
* values (000, 100117, 0, 'D')!
* values (000, 100117, 0, 'US')!
* values (000, 100117, 0, 'D')!
* values (000, 100117, 0, 'US')!
* values (000, 100117, 0, 'D')!
* values (000, 100117, 0, 'US')!
* values (000, 100119, 0, 'D')!
* values (000, 100119, 0, 'US')!
* values (000, 100119, 0, 'D')!
* values (000, 100119, 0, 'US')!
* values (000, 100119, 0, 'D')!
* values (000, 100119, 0, 'US')!
* values (000, 100121, 0, 'D')!
* values (000, 100121, 0, 'US')!
* values (000, 100121, 0, 'D')!
* values (000, 100121, 0, 'US')!
* values (000, 100121, 0, 'D')!
* values (000, 100121, 0, 'US')!
* values (000, 100121, 0, 'D')!
* values (000, 100121, 0, 'US')!
* values (000, 100123, 0, 'D')!
* values (000, 100123, 0, 'US')!
* values (000, 100123, 0, 'D')!
* values (000, 100123, 0, 'US')!
* values (000, 100123, 0, 'D')!
* values (000, 100123, 0, 'US')!
* values (000, 100123, 0, 'D')!
* values (000, 100123, 0, 'US')!
* values (000, 100125, 0, 'D')!
* values (000, 100125, 0, 'US')!
* values (000, 100127, 0, 'D')!
* values (000, 100127, 0, 'US')!
* values (000, 100127, 0, 'D')!
* values (000, 100127, 0, 'US')!
* values (000, 100127, 0, 'D')!
* values (000, 100127, 0, 'US')!
* values (000, 100127, 0, 'D')!
* values (000, 100127, 0, 'US')!
* values (000, 100129, 0, 'D')!
* values (000, 100129, 0, 'US')!
* values (000, 100129, 0, 'D')!
* values (000, 100129, 0, 'US')!
* values (000, 100133, 0, 'D')!
* values (000, 100133, 0, 'US')!
* values (000, 100137, 0, 'D')!
* values (000, 100137, 0, 'US')!
* values (000, 100137, 0, 'D')!
* values (000, 100137, 0, 'US')!
* values (000, 100139, 0, 'D')!
* values (000, 100139, 0, 'US')!
* values (000, 100139, 0, 'D')!
* values (000, 100139, 0, 'US')!
* values (000, 100139, 0, 'D')!
* values (000, 100139, 0, 'US')!
* values (000, 100143, 0, 'D')!
* values (000, 100143, 0, 'US')!
* values (000, 100143, 0, 'D')!
* values (000, 100143, 0, 'US')!
* values (000, 100143, 0, 'D')!
* values (000, 100143, 0, 'US')!
* values (000, 100145, 0, 'D')!
* values (000, 100145, 0, 'US')!
* values (000, 100147, 0, 'D')!
* values (000, 100147, 0, 'US')!
* values (000, 100149, 0, 'D')!
* values (000, 100149, 0, 'US')!
* values (000, 100153, 0, 'D')!
* values (000, 100153, 0, 'US')!
* values (000, 100157, 0, 'D')!
* values (000, 100157, 0, 'US')!
* values (000, 100157, 0, 'D')!
* values (000, 100157, 0, 'US')!
* values (000, 100157, 0, 'D')!
* values (000, 100157, 0, 'US')!
* values (000, 100157, 0, 'D')!
* values (000, 100157, 0, 'US')!
* values (000, 100159, 0, 'D')!
* values (000, 100159, 0, 'US')!
* values (000, 100161, 0, 'D')!
* values (000, 100161, 0, 'US')!
* values (000, 100163, 0, 'D')!
* values (000, 100163, 0, 'US')!
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



* PTS 1106589 !

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

SELECT Count(*) FROM Descriptor WHERE Descriptor.type >'-1' AND   
Descriptor.title ='bob' AND   Descriptor.createdate >=
TO_DATE('2000-05-10 17:20:07','YYYY-MM-DD HH24:MI:SS') !
fetch into :a !

SELECT Count(*) FROM Descriptor_vl WHERE Descriptor_vl.type >'-1' AND
   Descriptor_vl.title ='bob' AND   Descriptor_vl.createdate >=
   TO_DATE('2000-05-10 17:20:07','YYYY-MM-DD HH24:MI:SS') !
fetch into :a !
pars_execute !
commit release !

file connect (sut sut !
drop user orauser !
commit release !
