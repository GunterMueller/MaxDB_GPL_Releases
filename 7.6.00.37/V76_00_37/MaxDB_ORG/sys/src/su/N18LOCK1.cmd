*   *ID* N18LOCK1 DBCMD    changed on 1992-09-28-13.25.27 by ADMIN     *
file connect ( kern test !
create table t1 ( t1_c1 char(3) ) !
create table t2 ( t2_c1 char(3) range in ('aaa','bbb') ) !
create table t3 ( t3_c1 float(3) key ) !
create table t4 ( t4_c1 char(5) key ,
                  t4_c2 fixed(5,5) not null ) !
create table t5 ( t5_c1 fixed(3) key range between 5 and 604,
                  t5_c2 float(3) key ,
                  t5_c3 char(7) key ,
                  t5_c4 char(7) not null ,
                  t5_c5 char(7) ) !
commit work release !
