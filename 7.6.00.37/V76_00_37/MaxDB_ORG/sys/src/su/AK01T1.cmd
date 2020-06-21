*   *ID* AK01T1   DBCMD    changed on 1992-09-28-13.22.03 by ADMIN     *
CREATE TABLE kern.T1 ( F1 CHAR (4) ascii   key
   defAULT 'sss' RANGE IN ( 'aaa' ) ,
   f4 char (6) range not in ('a', 'f', 'c', 'r', 'e', 't' ),
   F2 FIXED (3,1 ) not null ,
   F3 char  (4 ) not null RANGE BETWEEN 'xx'  AND 'zzz'  ) !
CREATE TABLE kern.T11 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ,
   f4 char (6) range not in ('a', 'f', 'c', 'r', 'e', 't' ),
   F2 FIXED (3,1 ) not null ,
   F3 char  (4 ) not null RANGE BETWEEN 'xx'  AND 'zzz'  ) !
file sel_cols_1tab ( KERN T11 !
create table kern.t2 like kern.t1 !
file sel_cols_1tab ( KERN T2 !
CREATE TABLE kern.T3
 ( F1 float(4) DEFAULT 4.55  RANGE IN ( 1,2,3,4 ) ,
    F2 FIXED (3,1 ) RANGE BETWEEN 1 AND 3 ) !
file sel_cols_1tab ( KERN T3 !
CREATE TABLE kern.T31 ( F1 float(4) DEFAULT 3  RANGE IN ( 1,2,3,4 ) ,
    F2 FIXED (3,1 ) RANGE BETWEEN 1 AND 3 ) !
file sel_cols_1tab ( KERN T31 !
rollback work   !
