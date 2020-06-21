*   *ID* N00T0801 DBCMD    changed on 1992-09-28-13.24.53 by ADMIN     *
 ***  8.1  <create table statement>
 ***  8.2  <drop table statement>   !
file connect ( kern test !
create table t1
  ( t1xc1 fixed (15,5) key,
    t1xc2 char(10) ascii key default 'Berlin',
    t1xc3 char(10) byte not null,
    t1xc4 fixed(5) not null default 12345,
    t1xc5 character(250) not null
          range not between 'aaa' and 'kkkkk',
    t1xc6 float(10) default 2E-3 range between -5E15 and +0.7E08 )!
create table kern.t2
  ( t2xc1 char(1) key range in ( 'a', 'z', '9' ),
    t2xc2 char(8) byte default X'9494949494949494',
    t2xc4 char(5) ) !
create table t3 ( t3xc1 date, t3xc2 time ) !
create table t4 ( t4xc1 integer key,
                  t4xc2 smallint not null,
                  t4xc3 decimal(8,3) range not in ( -15.2, 10, 15 ),
                  t4xc4 float default -13E12,
                  t4xc5 varchar(18) ) !
create table liketable like kern.t2 !
create domain dom char(15) default 'Hamburg' !
create table t5 ( t5xc1 dom not null ) !
commit work release !
file connect ( kern test sqlmode db2 !
create table t6 ( t5xc1 char(10) for bit data,
                  t5xc2 char(10) for bit data not null,
                  t5xc3 char(3)  for bit data ) !
rollback work release !
file connect ( kern test !
create table t6 ( t5xc1 char(10) byte key,
                  t5xc2 char(10) byte not null,
                  t5xc3 char(3)  byte default x'000001') !
file sel_cols_tab ( T6 !
commit work !
drop table t1 !
drop table t2 !
drop table kern.t3 !
drop table t4 !
drop domain dom !
drop table t5 !
drop table t6 !
drop table kern.liketable !
commit work release !
