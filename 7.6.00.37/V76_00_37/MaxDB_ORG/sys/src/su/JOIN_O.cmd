senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* ODBC.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

file connect ( kern test !
create table j1 (j1 int, c1 char)!
create table j2 (j2 int, c2 char)!
insert into j1 values (1, 'a')!
insert into j1 values (2, 'a')!
insert into j1 values (3, 'a')!
insert into j2 values (3, 'b')!
insert into j2 values (4, 'b')!
insert into j2 values (5, 'b')!
select * from j1, j2 where j1    = j2 !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1(+) = j2 !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1    = j2(+) !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1(+) = j2(+) !
fetch into :a, :b, :c, :d !
drop table j1 !
drop table j2 !
commit work release!
senderid ODB !
file connect ( kern test ! 
create table j1 (j1 int, c1 char)!
create table j2 (j2 int, c2 char)!
insert into j1 values (1, 'a')!
insert into j1 values (2, 'a')!
insert into j1 values (3, 'a')!
insert into j2 values (3, 'b')!
insert into j2 values (4, 'b')!
insert into j2 values (5, 'b')!
select * from j1, j2 where j1    = j2 !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1(+) = j2 !
fetch into :a, :b, :c, :d !
select * from { oj j1 left  outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1    = j2(+) !
fetch into :a, :b, :c, :d !
select * from { oj j1 right outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1(+) = j2(+) !
fetch into :a, :b, :c, :d !
select * from { oj j1 full  outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
drop table j1 !
drop table j2 !
commit work release!
senderid ODB !
file connect ( kern test sqlmode oracle ! 
create table j1 (j1 int, c1 char)!
create table j2 (j2 int, c2 char)!
insert into j1 values (1, 'a')!
insert into j1 values (2, 'a')!
insert into j1 values (3, 'a')!
insert into j2 values (3, 'b')!
insert into j2 values (4, 'b')!
insert into j2 values (5, 'b')!
select * from j1, j2 where j1    = j2 !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1(+) = j2 !
fetch into :a, :b, :c, :d !
select * from { oj j1 left  outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1    = j2(+) !
fetch into :a, :b, :c, :d !
select * from { oj j1 right outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1(+) = j2(+) !
fetch into :a, :b, :c, :d !
select * from { oj j1 full  outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
drop table j1 !
drop table j2 !
senderid XCI !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* ORABOOK.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test sqlmode oracle!
create table ADDRESS ( LastName  char(25),
                       FirstName char(25),
                       Street    char(50),
                       City      char(25),
                       State     char(2),
                       Zip       number,
                       Phone     char(12),
                       Ext       char(5))!
create table LOCATION ( City       char(25),
                        Country    char(25),
                        Continent  char(25),
                        Latitude   number,
                        Nor_South  char(1),
                        Longitude  number,
                        EastWest   char(1))!
create table WEATHER  ( City        char(11),
                        degree      number,
                        Humidity    number,
                        Condition   char(9))!
commit work release !
file connect ( kern test sqlmode oracle !
insert into ADDRESS values (
'BAILEY','WILLIAM', null, null, null, null, '213-293-0223', null) !
insert into ADDRESS values (
'ADAMS','JACK', null, null, null, null, '415-453-7530', null) !
insert into ADDRESS values (
'SEP','FECICIA', null, null, null, null, '214-522-8383', null) !
insert into ADDRESS values (
'DE MEDICI','LEFTY', null, null, null, null, '312-736-1166', null) !
insert into ADDRESS values (
'DEMIURGE','FRANK', null, null, null, null, '707-767-8900', null) !
insert into ADDRESS values (
'CASEY','WILLIS', null, null, null, null, '312-684-1414', null) !
insert into ADDRESS values (
'ZACK','JACK', null, null, null, null, '415-620-6842', null) !
insert into ADDRESS values (
'YARROW','MARY', null, null, null, 949414302, '415-787-2178', null) !
insert into ADDRESS values (
'WERSCHKY','ARNY', null, null, null, null, '415-235-7387', null) !
insert into ADDRESS values (
'BRANT','GLEN', null, null, null, null, '415-526-7512', null) !
insert into ADDRESS values (
'EDGAR','THEODORE', null, null, null, null, '415-525-6252', null) !
insert into ADDRESS values (
'HARDIN','HUGGY', null, null, null, null, '617-566-0125', null) !
insert into ADDRESS values (
'HILD','PHIL', null, null, null, null, '603-934-2242', null) !
insert into ADDRESS values (
'LOEBEL','FRANK', null, null, null, null, '202-456-1414', null) !
insert into ADDRESS values (
'MOORE','MARY', null, null, null, 601262460, '718-857-1638', null) !
insert into ADDRESS values (
'SZEP','FELICIA', null, null, null, null, '214-522-8383', null) !
insert into ADDRESS values (
'ZIMMERMAN','FRED', null, null, null, null, '503-234-7491', null) !
commit work!
insert into LOCATION values (
'ATHENS','GREECE','EUROPE',37.58,'N',23.43,'E')!
insert into LOCATION values (
'CHICAGO','UNITED STATES','NORTH AMERICA',41.53,'N',87.38,'W')!
insert into LOCATION values (
'CONAKRY','GUINEA','AFRICA',9.31,'N',13.43,'W')!
insert into LOCATION values (
'LIMA','PERU','SOUTH AMERICA',12.03,'S',77.03,'W')!
insert into LOCATION values (
'MADRAS','INDIA','INDIA',13.05,'N',80.17,'E')!
insert into LOCATION values (
'MANCHESTER','ENGLAND','EUROPE',53.30,'N',2.15,'W')!
insert into LOCATION values (
'MOSCOW','U.S.S.R.','EUROPE',55.45,'N',37.35,'E')!
insert into LOCATION values (
'PARIS','FRANCE','EUROPE',48.52,'N',2.20,'E')!
insert into LOCATION values (
'SHENYANG','CHINA','CHINA',41.48,'N',123.27,'E')!
insert into LOCATION values (
'ROME','ITALY','EUROPE',41.54,'N',12.29,'E')!
insert into LOCATION values (
'TOKYO','JAPAN','EUROPE',35.42,'N',139.46,'E')!
insert into LOCATION values (
'SYDNEY','AUSTRALIA','AUSTRALIA',33.52,'S',151.13,'E')!
insert into LOCATION values (
'SPARTA','GREECE','EUROPE',37.05,'N',22.27,'E')!
insert into LOCATION values (
'MADRID','SPAIN','EUROPE',40.24,'N',3.41,'W')!
commit work !

insert into WEATHER values ('LIMA',45,79,'RAIN')!
insert into WEATHER values ('PARIS',81,62,'CLOUDY')!
insert into WEATHER values ('MANCHESTER',66,98,'FOG')!
insert into WEATHER values ('ATHENS',97,89,'SUNNY')!
insert into WEATHER values ('CHICAGO',66,88,'RAIN')!
insert into WEATHER values ('SYDNEY',29,12,'SNOW')!
insert into WEATHER values ('SPARTA',74,63,'CLOUDY')!
commit work !

select weather.city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from weather, location
 where weather.city = location.city 
 order by weather.city!
fetch into :a,:b,:c,:d,:e,:f,:g!
create view invasion as
select weather.city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from weather, location
 where weather.city = location.city !
select city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from invasion !
fetch into :a,:b,:c,:d,:e,:f,:g!
drop view invasion !
create view invasion as
select weather.city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from weather, location
 where weather.city = location.city
   and country = 'GREECE'!
select city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from invasion !
fetch into :a,:b,:c,:d,:e,:f,:g!
commit work !
select a.lastname, a.firstname, a.phone
  from address a, address b
 where a.lastname != b.lastname
   and soundex(a.lastname) = soundex(b.lastname) !
fetch into :a,:b,:c !
commit work !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* ORADATE.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test sqlmode oracle!
create table t (s0 char(35) default ' ', s1 date ) !
insert into t (s1) values ('29-FEb-68') !
insert into t      values (' *++ Current date', sysdate) !
insert into t (s1) values (null) !
insert into t (s1) values ('29-Aug-65') !
insert into t (s1) values ('09-Jan-65') !
insert into t (s1) values ('09-Feb-65') !
insert into t (s1) values ('09-Mar-65') !
insert into t (s1) values ('09-Apr-65') !
insert into t (s1) values ('09-May-65') !
insert into t (s1) values ('09-Jun-65') !
insert into t (s1) values ('09-Jul-65') !
insert into t (s1) values ('09-Aug-65') !
insert into t (s1) values ('09-Sep-65') !
insert into t (s1) values ('29-Oct-65') !
insert into t (s1) values ('29-Nov-65') !
insert into t (s1) values ('29-Dec-65') !
insert into t (s1) values ('23-Feb-70') !
insert into t (s1) values ('23-Feb-71') !
insert into t (s1) values ('23-Feb-72') !
insert into t (s1) values ('23-Feb-73') !
commit work release !
file connect ( kern test !
create table stats$lockactivity (
snap_id char (8),
indx    fixed (6),
"COUNT" fixed (8))!
insert into stats$lockactivity values ('abcdefgh', 6, 3) !
commit release !
file connect ( kern test sqlmode oracle !
CREATE VIEW "STATS$PINGACTIVITY" ("STARTSNAP","END_SNAP",
"PING_RELS","PING_REQS") AS SELECT B.SNAP_ID, E.SNAP_ID ,
SUM(DECODE(B.INDX, 1, E.COUNT - B.COUNT, 4, E.COUNT - B.COUNT, 0)) 
PING_RELS ,
SUM(DECODE( B.INDX, 6, E.COUNT - B.COUNT, 0)) PING_REQS 
FROM STATS$LOCKACTIVITY B ,STATS$LOCKACTIVITY E 
WHERE B.INDX = E.INDX GROUP BY B.SNAP_ID, E.SNAP_ID !
select * from stats$pingactivity !
fetch into :a, :b, :c, :d !
drop table stats$lockactivity!
commit release !

file connect ( kern test !
create table bug1_1 ( id varchar(5), c2 varchar(5) , primary key (id) )!
create table bug1_2 ( id varchar(5), bug1_1_ref varchar(5) , 
c3 varchar(5), primary key (id) )!

insert into bug1_1 values( '1', '1')!
insert into bug1_1 values( '2', '2')!
insert into bug1_2 values( '1', '1', 'x')!

select 
 b1.id id1, b1.c2, b2.id id2, b2.bug1_1_ref , b2.c3
from 
 bug1_1 b1, bug1_2 b2
where 
 b2.c3 in ( select c3 from bug1_2) and
 b1.id = b2.bug1_1_ref (+) !

select 
 b1.id id1, b1.c2, b2.id id2, b2.bug1_1_ref , b2.c3
from 
 bug1_1 b1, bug1_2 b2
where 
 b1.id = b2.bug1_1_ref (+) and 
 b2.c3 in ( select c3 from bug1_2)!

commit release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
