*   *ID* N15TAB3  DBCMD    changed on 1992-09-28-13.25.19 by ADMIN     *
 *** Falsche 'CREATE TABLE' mit 'FIXED' und 'RANGE NOT BETWEEN' . !
create table t1 ( t1_c1 fixed(1) range not         3 and 4 ) !
create table t1 ( t1_c1 fixed(1) rangenot between  3 and 4 ) !
create table t1 ( t1_c1 fixed(1) range notbetween  3 and 4 ) !
create table t1 ( t1_c1 fixed(1) range not,between 3 and 4 ) !
create table t1 ( t1_c1 fixed(1) range not between 'NULL' and 5  ) !
create table t1 ( t1_c1 fixed(1) range not between  NULL  and 5  ) !
create table t1 ( t1_c1 fixed(1) range not between     5  and 22 ) !
create table t1 ( t1_c1 fixed(1) range not between     5  and 3  ) !
create table t1a ( t1a_c1 fixed(1) range not between  -9    and 9  ) !
create table t1 ( t1_c1 fixed(1) range not between    -5  and -6 ) !
create table t1 ( t1_c1 fixed(1) range not between     0  and -5 ) !
create table t1 ( t1_c1 fixed(1) range not between     2  and -5 ) !
create table t2 ( t2_c1 fixed(1,1) range not between   .3 and .2 ) !
create table t2 ( t2_c1 fixed(1,1) range not between  0.3 and 0  ) !
create table t2a ( t2a_c1 fixed(1,1) range not between -0.9 and 0.9) !
create table t2 ( t2_c1 fixed(1,1) range not between -0.2 and -.3) !
create table t2 ( t2_c1 fixed(1,1) range not between -0.0 and -.2) !
create table t2 ( t2_c1 fixed(1,1) range not between 0.2 and -0.2) !
create table t4 ( t4_c1 fixed(18)
   range not between  123456789012345678 and -123456789012345678 ) !
create table t4a ( t4a_c1 fixed(18)
   range not between -999999999999999999 and +999999999999999999 )  !
