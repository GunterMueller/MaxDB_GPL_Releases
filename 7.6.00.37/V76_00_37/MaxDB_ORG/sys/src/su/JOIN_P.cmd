senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* PTS.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

connect kern test !
CREATE TABLE "WAGGONS"            (
"WAGGON_NUMMER" CHAR(20) ASCII ,
"HOLZNR" INTEGER 
DEFAULT 0,
"BAHNHOFAB" TIMESTAMP ,
"VERMESSEN" TIMESTAMP ,
"STEINHORST" TIMESTAMP ,
"BOX" TIMESTAMP ,
"BHFADELEBSENAN" TIMESTAMP ,
"WEGENER" TIMESTAMP ,
"BEMERKUNG" CHAR(50) ASCII ,
"BAHNHOFSNR" INTEGER 
DEFAULT 99999,
"CAFM" FLOAT(8) 
DEFAULT 0.0000000E+00,
"ERLEDIGT" SMALLINT 
DEFAULT 0,
"FUHRMANN_NR" INTEGER 
DEFAULT 99999,
"BAHNHOF" CHAR(40) ASCII ,
"WAGGONSNR" CHAR(12) ASCII ,
"LOESCHZEIT" TIMESTAMP ,
"LKW" FIXED(5,0) 
DEFAULT 0
, PRIMARY KEY ("WAGGONSNR"
)) !

insert into waggons set waggon_nummer = '1', waggonsnr = '1' !
insert into waggons set waggon_nummer = '2', waggonsnr = '2' !
insert into waggons set waggon_nummer = '3', waggonsnr = '3' !

CREATE TABLE "PUFFER"             (
"STUECK" SMALLINT 
DEFAULT 0,
"LOESCHZEIT" TIMESTAMP ,
"AUFTRAGSNR" CHAR(12) ASCII ,
"WAGGONSNR" CHAR(12) ASCII ,
"PUFFERNR" CHAR(12) ASCII 
, PRIMARY KEY ("PUFFERNR"
)) !

insert into puffer set Puffernr = '2', waggonsnr = '2' !
insert into puffer set Puffernr = '3' !
insert into puffer set Puffernr = '4' !

commit !

explain select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<waggons,puffer> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !

select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<waggons, puffer> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !
fetch !

explain select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<puffer, waggons> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !

select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<puffer, waggons> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !
fetch !

CREATE INDEX        "WAGGONSNR"          ON "PUFFER"            
("WAGGONSNR" ASC ) !

explain select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<waggons,puffer> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !

select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<waggons, puffer> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !
fetch !

explain select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<puffer, waggons> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !

select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<puffer, waggons> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !
fetch !

drop table waggons !
drop table puffer !
commit work release !

connect kern test !

create table Tab1 ( k00  int, k01 int) !
create table Tab2 ( k00  int, k01 int) !

insert into Tab1 values ( 1, 1 )!
insert into Tab1 values ( 2, 2 )!
insert into Tab2 values ( 1, 1 )!
insert into Tab2 values ( 2, 2 )!

* earlier error was kb-stack type illegal 
select * from Tab1, Tab2 
where not(Tab1.k01 = 1 and Tab2.k00 = 1) !
commit work release !

connect kern test !
Create Table Name_Space (
    Id         Char(24)    Byte,
    Name_Space Char(50)      ,
    Primary Key (Name_Space),
    Constraint Name_Space_X Unique (Id)
)!
*//
Insert into Name_Space Set
  Id = X'000000000000000000000000000000000000000000000001',
  Name_Space = 'DAV:'!
*//
*////////////////////////////////////////////////////
*//
Create Table Property_Management (
   Id                Char(24)   Byte,
   Name_Space_Id     Char(24)   Byte,
   Name_Prefix       Char(50)      ,
   Name              Char(50)     ,
   Primary Key (Name_Space_Id, Name_Prefix)
)!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000001',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'displayname'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000002',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'resourcetype'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000003',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'getcontenttype'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000004',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'getcontentlength'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000005',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'getlastmodified'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000006',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockdiscovery'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000006',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockid'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000007',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockowner'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000008',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'locktype'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000009',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockscope'!
*//
Insert into Property_Management Set
  Id = X'00000000000000000000000000000000000000000000000A',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'locktimeout'!
*//
Insert into Property_Management Set
  Id = X'00000000000000000000000000000000000000000000000B',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockdepth'!
*//
Insert into Property_Management Set
  Id = X'00000000000000000000000000000000000000000000000C',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'supportedlock'!
*//
*//Select * from Property_Management
*//
*////////////////////////////////////////////////////
*//
Create Table Property (
   CId                  Char(24)  Byte,

   Property_Id          Char(24)  Byte,
   Property_Short_Value Char(900)     ,
   Property_Long_Value  Long      Byte
)!
*//
Create Index Property_X1 On Property (Property_Id, CId)!
*//
Create Index Property_X2 On Property (Cid DESC)!
*//
*///////////////////////////////////////////////////
*// Initial values for ROOT
*///////////////////////////////////////////////////
*//
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000001',
  Property_Short_Value = '/'!
*//
*// resourcetype for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000002',
  Property_Short_Value = 'collection'!
*//
*// getcontentype for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000003',
  Property_Short_Value = 'sapdbwww/directory'!
*//
*// getcontentlength for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000004',
  Property_Short_Value = '0'!
*//
*// getlastmodified for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000005',
  Property_Short_Value = TIMESTAMP!
*//
*// lockdiscovery for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000006',
  Property_Short_Value = '000000000000000000000000000000000000000000000000'!
*//
*// supportedlock for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'00000000000000000000000000000000000000000000000C',
  Property_Short_Value = 'exclusive'!


* before PTS1110719 this produces 8 results, one row with 'DAV' and 'lockid'
SELECT NS.Name_Space, PM.Name_Prefix, 
       P.Property_Short_Value, P.Property_Id   
FROM Property P, Property_Management PM, Name_Space NS         
WHERE 
P.Property_Id = PM.Id AND PM.Name_Space_Id = NS.Id AND
NOT (NS.Name_Space = 'DAV:' AND PM.Name_Prefix = 'lockid') !
fetch!

* before PTS1110719 this produces the correct result set of 7 rows
SELECT NS.Name_Space, PM.Name_Prefix, 
       P.Property_Short_Value, P.Property_Id   
FROM Property P, Property_Management PM, Name_Space NS         
WHERE 
NOT (NS.Name_Space = 'DAV:' AND PM.Name_Prefix = 'lockid') AND
P.Property_Id = PM.Id AND PM.Name_Space_Id = NS.Id !
fetch!
commit work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* TEST 'ONE PHASE HANDLING'
*
* check if count(*) delivers result 0 for an empty join result
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
senderid DBM !
file connect ( kern test !

create table j1 (i1 int)!
create table j2 (i2 int)!

pars_execute!
commit work release!
file connect ( sut sut !
SET PARAMETER OPTIMIZE_JOIN_ONEPHASE = 'YES' !
commit work release!
file connect ( kern test !
select count(*) into :a from j1, j2 where i1 = i2!
select count(*) from j1, j2 where i1 = i2!
fetch !
pars_then_ex!
select count(*) into :a from j1, j2 where i1 = i2!
select count(*) from j1, j2 where i1 = i2!
fetch !
pars_execute!
commit work release!
file connect ( sut sut !
SET PARAMETER OPTIMIZE_JOIN_ONEPHASE = 'NO' !
commit work release!
file connect ( kern test !
select count(*) into :a from j1, j2 where i1 = i2!
select count(*) from j1, j2 where i1 = i2!
fetch !
pars_then_ex!
select count(*) into :a from j1, j2 where i1 = i2!
select count(*) from j1, j2 where i1 = i2!
fetch !
pars_execute!
commit work release!
file connect ( sut sut !
SET PARAMETER OPTIMIZE_JOIN_ONEPHASE = 'YES' !
commit work release!
file connect ( kern test !
rollback work release!
senderid XCI !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* PTS http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130364
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test sqlmode oracle !
create table table1 ( 
d_start date, column1 number (9,0), 
e_key number (9,0) primary key)!
create table table2 (a_key number(9,0) primary key)!
parsing!
SELECT * FROM TABLE1 b  inner join TABLE2 a on (b.e_key=a.a_key)
WHERE  b.D_START=to_DATE(?, 'dd/mm/yyyy') AND 
 b.column1=to_number(?) AND
 b.e_key=to_number(?) !
executing !
data !
select !
1
c 254 '26/05/2004'
c 254 '1'
c 254 '200027'

pars_execute !
nodata !

rollback work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* PTS http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1131347
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

connect kern test !
create table table1 (ukfield int key, fkfield int)!
insert table1 values (1,1)!
create table table2 (ukfield int key)!
insert table2 values (1)!

explain SELECT *
FROM table1 t1, table2 t2
WHERE ( t1.FKfield = t2.UKfield(+) ) 
  AND (t1.UKfield = 0 AND t1.UKfield = 1)!

SELECT *
FROM table1 t1, table2 t2
WHERE ( t1.FKfield = t2.UKfield(+) ) 
  AND (t1.UKfield = 0 AND t1.UKfield = 1)!

rollback work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* PTS 1138197
file connect ( sut sut !
create table tab1 ( c1 char(10), c2 char(1) default 'z'
, primary key(c1,c2))!
insert into tab1(c1) values('aaaaaaaaaa')!
insert into tab1(c1) values('bbbbb')!

create table tab2 ( c1 char(5), c2 char(1) default 'x'
, primary key(c1,c2))!
create index i1 on tab2(c1)!
insert into tab2(c1) values('aaaaa')!
insert into tab2(c1) values('bbbbb')!

explain select tab1.c1 from <tab1, tab2> 
where tab1.c1 = tab2.c1 !
select tab1.c1 from <tab1, tab2> 
where tab1.c1 = tab2.c1 !

rollback work release!

file connect ( sut sut !
commit work release !

*****************************************************************************
