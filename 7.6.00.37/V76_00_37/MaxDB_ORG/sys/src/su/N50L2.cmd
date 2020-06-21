*   *ID* N50L2    DBCMD    changed on 1992-09-28-13.26.48 by ADMIN     *
 *** gestetet wird Katalog-Select
 *** nach RENAME TABLE ... und RENAME COLUMN ...
 *******************************************************!
file connect ( kern test !
create table t1 ( t1xc1 fixed(5) key,
                  t1xc2 char(5) key,
                  t1xc3 fixed(5),
                  t1xc4 char(5),
                  t1Xc5 fixed(5) not null,
                  t1xc6 char(5) not null,
                  t1xc7 fixed(5) default 1,
                  t1xc8 char(5) default 'AAAA' ) !
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
create table t4 ( t4xc1 fixed(5) key,
                  t4xc2 char(5) key,
                  t4xc3 fixed(5),
                  t4xc4 char(5),
                  t4Xc5 fixed(5) not null,
                  t4xc6 char(5) not null,
                  t4xc7 fixed(5) default 1,
                  t4xc8 char(5) default 'AAAA' ) !
commit work !
alter table t2 foreign key l12r1 ( t2xc3, t2xc4 )
   references t1 !
alter table t3 foreign key l23c1 ( t3xc5, t3xc6 )
   references t2 on delete cascade !
alter table t4 foreign key l34d1 ( t4xc7, t4xc8 )
   references t3 on delete set default !
commit work !
rename table t1 to rt1 !
rename table t2 to rt2 !
rename table t4 to rt4 !
rename column rt1.t1xc1 to rt1xc1 !
rename column rt1.t1xc2 to rt1xc2 !
rename column rt2.t2xc3 to rt2xc3 !
rename column rt2.t2xc4 to rt2xc4 !
rename column rt2.t2xc5 to rt2xc5 !
rename column rt2.t2xc6 to rt2xc6 !
rename column rt4.t4xc7 to rt4xc7 !
rename column rt4.t4xc8 to rt4xc8 !
commit work !
file sel_foreignkey ( KERN T2 !
file sel_foreignkey ( KERN RT2 !
file sel_references ( KERN RT2 !
file sel_foreignkey ( KERN RT4 !
commit work !
insert rt1 values ( 2, 'AAAA', 3, 'CCCC', 4, 'FFFF', 2, 'AAAA' ) !
insert rt1 values ( 5, 'BBBB', 3, null, 1, 'CCCC', 1, 'BBBB' ) !
insert rt2 values ( 4, 'FFFF', null, 'DDDD', 2, 'AAAA', 2, 'CCCC' ) !
insert rt2 values ( 3, 'BBBB', 5, 'BBBB', 4, 'DDDD', 2, 'AAAA' ) !
insert t3 set t3xc1 = 2,
              t3xc2 = 'AAAA',
              t3xc3 = 7,
              t3xc4 = 'BBBB',
              t3xc5 = 4,
              t3xc6 = 'FFFF',
              t3xc7 = 8,
              t3xc8 = 'CCCC' !
insert t3 ( t3xc1, t3xc2, t3xc3, t3xc4, t3xc5, t3xc6 )
   values ( 8, 'CCCC', 2, 'CCCC', 3, 'BBBB' ) !
insert rt4 set  t4xc1 = 4,
                t4xc2 = 'FFFF',
                t4xc3 = 3,
                t4xc4 = 'AAAA',
                t4xc5 = 2,
                t4xc6 = 'BBBB',
               rt4xc7 = 8,
               rt4xc8 = 'CCCC' !
insert rt4 ( t4xc1, t4xc2, t4xc3, t4xc4, t4xc5, t4xc6, rt4xc7 )
   values ( 2, 'BBBB', 6, 'BBBB', 4, 'DDDD', 2 ) !
commit work !
delete rt1 where rt1xc1 = 5 !
delete rt2 where t2xc1 = 4 !
select * from rt1 !
fetch !
select * from rt2 !
fetch !
select * from t3 !
fetch !
select * from rt4 !
fetch !
alter table rt2 drop foreign key l12r1 !
alter table t3 drop foreign key l23c1 !
alter table rt4 drop foreign key l34d1 !
drop table rt1 !
drop table rt2 !
drop table t3 !
drop table rt4 !
commit work release !
