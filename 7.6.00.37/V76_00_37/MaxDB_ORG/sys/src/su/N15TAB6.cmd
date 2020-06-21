*   *ID* N15TAB6  DBCMD    changed on 1992-09-28-13.25.21 by ADMIN     *
create table t0 ( t1_c1 fixed (3,2) key not null ) !
create table t1 ( t1_c1 fixed (3,2) not null key ) !
create table t1 ( t1_c1 fixed (3,2) key range between
       8 and +8 not null ) !
create table t1 ( t1_c1 fixed (3,2) not null range not
       between 8. and +8.3 key ) !
create table t1 ( t1_c1 fixed (13,5) key range in
       ( 8.7 , 22222 ) not null ) !
create table t1 ( t1_c1 fixed ( 12 , 11) key range  not
       in ( 7) not null ) !
