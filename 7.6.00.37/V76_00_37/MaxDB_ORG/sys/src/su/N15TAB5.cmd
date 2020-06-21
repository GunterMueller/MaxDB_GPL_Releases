*   *ID* N15TAB5  DBCMD    changed on 1992-09-28-13.25.20 by ADMIN     *
 *** Falsche 'CREATE TABLE' mit 'FIXED' und 'RANGE NOT IN' .!
create table t1 ( t1_c1 fixed(1) rangenot in (3) )         !
create table t1 ( t1_c1 fixed(1) range notin ( 3) )        !
create table t1 ( t1_c1 fixed(1) range,not in (3) )        !
create table t1d ( t1d_c1 fixed(1) range not in(3) )       !
create table t1 ( t1_c1 fixed(1) range not in  (22) )      !
create table t1 ( t1_c1 fixed(1) range not in (7, -22 ))   !
create table t2 ( t2_c1 fixed(1) range not in (0.3) )      !
