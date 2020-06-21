*   *ID* AK01T4   DBCMD    changed on 1992-09-28-13.22.05 by ADMIN     *
monitor init !
CREATE TABLE kern.T1 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ,
   f4 char (6) range not in ('a', 'f', 'c', 'r', 'e', 't' ),
   F2 FIXED (3,1 ) not null ,
   F21 FIXED (5,1 ) not null range in (-512, 500, -500,512, -22, -191),
   F3 char  (4 ) not null RANGE BETWEEN 'xx'  AND 'zzz'  ) !
CREATE TABLE kern.T2 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ,
   f4 char (6) range not in ('a', 'f', 'c', 'r', 'e', 't' ),
   F2 FIXED (3,1 ) not null ,
   F21 FIXED (5,1 ) not null range in (-512, 500, -500,512, -22, -191),
   F3 char  (4 ) not null RANGE BETWEEN 'xx'  AND 'zzz'  ) !
file sel_cols_1tab ( KERN T1 !
file sel_table !
 create index  "kern.t1.f4"
	on kern.t1( f4  asc ) !
 create unique index  "kern.t1.f2"
	on kern.t1( f2 desc ) !
 create unique index  indname1 on kern.t1
        ( f4 asc, f2, f3 desc ) !
 create index  "kern.f3"
	on sut( f3 desc ) !
 drop index "kern.t1.f4" !
file sel_index_tab ( T1 !
file sel_index_1tab ( KERN T1 !
 alter table kern.t2 foreign key name (f1 ) references kern.t1 !
 create synonym synt1 for kern.t1 !
 create synonym synt2 for kern.t1 !
 create synonym synt3 for kern.t2 !
 create synonym p1syn for kern.t2 !
 drop synonym synt1 !
file sel_synonym !
create domain domname char (8)
      default 'aaa'
      range in ('aaa', 'bbb', 'ddddd' ) !
create domain domname1 char (8)
      default 'aaa'
      range not between 'b' and 'z' !
create domain domname2 char (8) not null
      default 'aaa'
      range not between 'b' and 'z' !
create domain domname2 char (8) key
      default 'aaa'
      range not between 'b' and 'z' !
file sel_domain !
file sel_domain !
rollback work !
