*   *ID* AK01T3   DBCMD    changed on 1992-09-28-13.22.04 by ADMIN     *
monitor init !
CREATE TABLE kern.T1 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ,
   f4 char (6) range not in ('a', 'f', 'c', 'r', 'e', 't' ),
   F2 FIXED (3,1 ) not null ,
   F21 FIXED (5,1 ) not null range in (-512, 500, -500,512, -22, -191),
   F3 char  (4 ) not null RANGE BETWEEN 'xx'  AND 'zzz'  ) !
file sel_cols_1tab ( KERN T1 !
CREATE TABLE kern.k1 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ) !
CREATE TABLE kern.sysk1 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ) !
CREATE TABLE kern.k21 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ) !
CREATE TABLE kern.syskk1 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ) !
file sel_monitor_all ( CACHES PAGES !
file sel_table !
file sel_tab_owner ( KERN !
