*   *ID* N50L3    DBCMD    changed on 1992-09-28-13.26.49 by ADMIN     *
 * getestet werden Links in Verbindung mit Views
 ****************************************************!
file connect ( kern test !
create table t1 ( t1xc1 fixed(5) key,
                  t1xc2 char(5) key,
                  t1xc3 fixed(5),
                  t1xc4 char(5),
                  t1Xc5 fixed(5) not null,
                  t1xc6 char(5) not null,
                  t1xc7 fixed(5),
                  t1xc8 char(5) ) !
create table t2 ( t2xc1 fixed(5) key,
                  t2xc2 char(5) key,
                  t2xc3 fixed(5),
                  t2xc4 char(5),
                  t2Xc5 fixed(5) not null,
                  t2xc6 char(5) not null,
                  t2xc7 fixed(5) default 1,
                  t2xc8 char(5) default 'AAAA' ) !
create table t3 ( t3xc1 fixed(5) key,
                  t3xc2 char(5) key,
                  t3xc3 fixed(5),
                  t3xc4 char(5),
                  t3Xc5 fixed(5) not null,
                  t3xc6 char(5) not null,
                  t3xc7 fixed(5) default 1,
                  t3xc8 char(5) default 'AAAA' ) !
commit work !
 ***************************************************!
insert t1 values
   ( 1, 'AAAA', 2, 'DDDD', 3, 'BBBB', 2, 'AAAA' ) !
insert t1 values
   ( 1, 'CCCC', 2, 'EEEE', 4, 'BBBB', 2, null ) !
insert t1 values
   ( 4, 'AAAA', 6, 'CCCC', 3, 'BBBB', 1, 'AAAA' ) !
insert t1 values
   ( 2, 'BBBB', 2, 'DDDD', 1, 'AAAA', 8, 'FFFF' ) !
insert t1 values
   ( 3, 'FFFF', null, 'CCCC', 3, 'BBBB', 2, 'AAAA' ) !
insert t1 values
   ( 5, 'AAAA', 2, 'FFFF', 3, 'BBBB', 2, 'CCCC' ) !
insert t1 values
   ( 1, 'FFFF', 6, 'AAAA', 3, 'FFFF', 2, 'AAAA' ) !
insert t1 values
   ( 8, 'DDDD', null, null, 8, 'BBBB', null, 'FFFF' ) !
insert t1 values
   ( 6, 'AAAA', 2, 'DDDD', 3, 'AAAA', 2, 'FFFF' ) !
insert t1 values
   ( 3, 'BBBB', 7, 'DDDD', 3, 'CCCC', 2, 'AAAA' ) !
select * from t1 !
fetch !
commit work !
 ***************************************************!
create view v1xt1 as select t1xc1, t1xc2 from t1 !
create view v1xt2 as select t2xc1, t2xc2, t2xc5, t2xc6 from t2
   with check option !
alter table v1xt2 foreign key l12r1 ( t2xc5, t2xc6 )
   references t1 !
alter table t2 foreign key l12r1 ( t2xc5, t2xc6 )
   references v1xt1 !
create synonym syn1 for v1xt1 !
create synonym syn2 for v1xt2 !
alter table syn2 foreign key l12r1 ( t2xc5, t2xc6 )
   references t1 !
alter table t2 foreign key l12r1 ( t2xc5, t2xc6 )
   references syn1 !
alter table t2 foreign key l12r1 ( t2xc5, t2xc6 )
   references t1 !
commit work !
insert v1xt2 values ( 2, 'DDDD', 5, 'AAAA' ) !
insert v1xt2 values ( 4, 'FFFF', 7, 'BBBB' ) !
delete v1xt1 where t1xc1 <= 2 !
delete v1xt1 where t1xc1 in ( 5, 8 ) !
delete v1xt1 where t1xc1 in ( 5 ) !
select * from t1 !
fetch !
select * from t2 !
fetch !
commit work !
create view v1xt3 as select t3xc3, t3xc4, t3xc5, t3xc6 from t3
   with check option !
alter table t3 foreign key l13c1 (t3xc3, t3xc4 )
   references t1 on delete cascade !
commit work !
insert t3 values ( 2, 'FFFF', 1, 'DDDD', 3, 'BBBB', 8, 'AAAA' ) !
insert t3 values ( 5, 'BBBB', 5, 'AAAA', 5, 'CCCC', 3, 'BBBB' ) !
insert t3 values ( 2, 'BBBB', 6, 'AAAA', 4, 'BBBB', 2, 'FFFF' ) !
insert t3 values ( 8, 'CCCC', 6, null  , 4, 'BBBB', 2, 'FFFF' ) !
delete v1xt1 where t1xc1 in ( 5, 6 ) !
delete v1xt1 where t1xc1 in ( 6 ) !
select * from t1 !
fetch !
select * from t2 !
fetch !
select * from t3 !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
* PTS 1000593 !
CREATE TABLE T ( A CHAR (80) KEY, B CHAR (3) KEY, 
C FIXED(2) KEY, IND INTEGER)!
CREATE UNIQUE INDEX "T.IND"
	on T( IND ) !
INSERT T VALUES (user, 'b', 3, 2)!
CREATE TABLE T1 (B CHAR (73) , C FIXED(2) PRIMARY KEY, IND INTEGER, 
 FOREIGN KEY  (IND) REFERENCES T (IND) ON DELETE CASCADE)!
CREATE VIEW V AS SELECT * FROM T WHERE a = USER!
parsing !
INSERT T VALUES (user, 'b', 3, 2)!
pars_execute!
subtrans begin!
executing !
insert !
1
pars_then_ex !
DELETE V WHERE IND = 2!
pars_execute!
select count(*) into :a from t !
drop table t !
drop table t1!
commit work release !
