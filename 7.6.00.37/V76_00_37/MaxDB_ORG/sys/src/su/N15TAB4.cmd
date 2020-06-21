*   *ID* N15TAB4  DBCMD    changed on 1992-09-28-13.25.20 by ADMIN     *
 *** Falsche 'CREATE TABLE' mit 'FIXED' und 'RANGE IN' . !
create table t1 ( t1_c1 fixed(1) range (3) )         !
create table t1 ( t1_c1 fixed(1) rangein (3) )       !
create table t1c ( t1c_c1 fixed(1) range in(3) )       !
create table t1 ( t1_c1 fixed(1) range in )          !
create table t1 ( t1_c1 fixed(1) range in ( ) )      !
create table t1 ( t1_c1 fixed(1) range in ( 'a' ) )  !
create table t1 ( t1_c1 fixed(1) range in (+))       !
create table t1 ( t1_c1 fixed(1) range in ( 11 ) )   !
create table t1 ( t1_c1 fixed(1) range in ( -3, 11)) !
create table t1 ( t1_c1 fixed(1) range in ( 11 ,3) ) !
create table t1 ( t1_c1 fixed(1) range in
   (-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10) )!
create table t1 ( t1_c1 fixed(1) range in (0.1) )    !
create table t2 ( t2_c1 fixed(1,1) range in (1)  )   !
create table t2 ( t2_c1 fixed(1,1) range in (0.1,1)) !
create table t2 ( t2_c1 fixed(1,1) range in ( 0.01)) !
create table t3 ( t3_c1 fixed(18,17)
   range in ( -.123456789012345678 ) )               !
rollback !
create table t3 ( t3_c1 fixed(18,17) range in (22.)) !
create table t3 ( t3_c1 fixed (18,17 )
   range in ( 1.3 , 22 ) )                           !
create table t3 ( t3_c1 fixed (18,17) range in (2E+33))!
