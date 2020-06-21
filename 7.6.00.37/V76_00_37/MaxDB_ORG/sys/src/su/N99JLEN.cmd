*   *ID* N99JLEN  DBCMD    changed on 1992-09-28-13.27.26 by ADMIN     *
file connect ( kern test !
 create table t1
       (nr        fixed (4),
        dummy1   char(200),
        dummy2   char(100),
        dummy3   char(200),
        dummy4   char(200),
        dummy5   char(200)) !
 create table t2
       (nr2       fixed (4),
        dmy1   char(200),
        dmy2   char(100),
        dmy3   char(200),
        dmy4   char(200),
        dmy5   char(200)) !
select *
from t1, t2
where ((t1.nr < 10) AND
(t1.dummy1 = t2.dmy1) AND
(t1.dummy2 = t2.dmy2)) !
rollback work release !
