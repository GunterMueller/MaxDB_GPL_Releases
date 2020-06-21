*   *ID* AK01T5   DBCMD    changed on 1992-09-28-13.22.05 by ADMIN     *
CREATE TABLE kern.T1 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ,
   f4 char (6) range not in ('a', 'f', 'c', 'r', 'e', 't' ),
   F2 FIXED (3,1 ) not null ,
   F21 FIXED (5,1 ) not null range in (-512, 500, -500,512, -22, -191),
   F3 char  (4 ) not null RANGE BETWEEN 'xx'  AND 'zzz'  ) !
create index  "kern.t1.f4"
	on kern.t1( f4  asc ) !
create unique index  "kern.t1.f2"
	on kern.t1( f2 desc ) !
create unique index  indname1 on kern.t1
        ( f4 asc, f2, f3 desc ) !
