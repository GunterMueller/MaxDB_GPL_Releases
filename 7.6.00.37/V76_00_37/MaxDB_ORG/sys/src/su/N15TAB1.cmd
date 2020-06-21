*   *ID* N15TAB1  DBCMD    changed on 1992-09-28-13.25.18 by ADMIN     *
 *** Falsche 'CREATE TABLE' mit 'FIXED'. !
create table t1 (t1_c1fixed(5))         !
create table t1 (t1_c1 fixed(0))        !
create table t1 ( t1_c1 fixed(0))       !
create table t1 ( t1_c1 fixed (0))      !
create table t1 ( t1_c1 fixed ( 0))     !
create table t1 ( t1_c1 fixed ( 0 ))    !
create table t1 ( t1_c1 FIXED ( 0 ) )   !
create table t1  ( t1_c1 fixed (39) )   !
create table t1 ( t1_c1 fixed (0,5) )   !
create table t1 ( t1_c1 fixed (39,5) )  !
create table t1 ( t1_c1 fixed (1,2) )   !
create table t1(t1_c1 fiXED ( 37,38))   !
create table t1 ( t1_c1 FIXed (38,39) ) !
create table t1 ( t1_c1 FIXED ( 5.5) )  !
create table t1 ( t1_c1 fixed ( 5_5 ))  !
create table t1 ( t1_c1 fixed ( 5 5 ))  !
create table t1 ( t1_c1 fixed (a) )     !
create table t1 ( t1_c1 FI_XED (5) )    !
create table t1 ( t1_c1 fixed_(5) )     !
create table t2   t2_c1 fixed (5) )     !
create table t2 ( t2_c1 fixed (5)       !
create table t2 ( t2_c1 fixed (5),)     !
create table t2 ( t2_c1 fixed (5) ,)    !
create table t3 KEY (t3_c1 fixed(5,18)) !
create table t3 (t3_c1 fixed(5,18)KEY ) !
create table t3 (t3_c1 fixed (1,18) key)!
create table t3 ( t3_c1 fixed(101) key )!
create table t3 ( t3_c1 fixed(10 key )  !
create table t3  t3_c1 fixed (7) key )  !
create table t4 ( t4_c1 fixed (5) NOTNULL )   !
create table t4 ( t4_c1 fixed (5) not_null )  !
create table t4 ( t4_c1 fixed (5) not null_)  !
create table t4 ( t4_c1 fixed (5) not null,)  !
create table t5     ( t5_c1 fixed (12) ,    ( t5_c2 fixed (6) )   !
create table t5     ( t5_c1 fixed (12) , key( t5_c2 fixed (6) )   !
create table t5     ( t5_c1 fixed (12) key    t5_c2 fixed (6) )   !
create table t5     ( t5_c1 fixed (12),  t5_c2 fixed (6) key  )   !
create table t5     ( t5_c1 fixed (12)  key,( t5_c2 fixed (6) )   !
 **  warning invalid end  !
create table t5ok   ( t5_c1 fixed (12))key  ( t5_c2 fixed (6) )   !
