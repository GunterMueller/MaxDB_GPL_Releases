*   *ID* AK01T6   DBCMD    changed on 1992-09-28-13.22.06 by ADMIN     *
monitor init !
create table kern.te1 (a char (4) key, b char(11) key,
   c char (12), d char (5) not null, e char (7)) !
CREATE TABLE kern.t19 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ,
   f4 char (6) range not in ('a', 'f', 'c', 'r', 'e', 't' ),
   F2 FIXED (3,1 ) not null ,
   F21 FIXED (5,1 ) not null range in (-512, 500, -500,512, -22, -191),
   f3 char  (4 ) not null RANGE BETWEEN 'xx'  AND 'zzz'  ) !
 create synonym synt19 for kern.t19 !
 create synonym synt2 for kern.t19 !
 drop synonym synt19 !
file sel_synonym !
create domain domname4 char (8)
      default 'aaa'
      range in ('aaa', 'bbb', 'ddddd' ) !
create domain domname5 char (8)
      default 'aaa'
      range not between 'b' and 'z' !
create domain char8  char (8)  not null
      default 'aaa'
      range in ('aaa', 'bbb', 'ddddd' ) !
create domain char8  char (8)
      default 'aaa'
      range in ('aaa', 'bbb', 'ddddd' ) !
create domain domname2 char (4)  ascii
      default 'aaa'
      range not between 'b' and 'z' !
create domain domname3 fixed (4,1) not null
      range not between '1' and '7' !
create domain domname3 fixed (4,1)
      range not between  1  and  7  !
create domain domname6 fixed (4,1) key
      range not between  1  and  7  !
file sel_domain !
file sel_domaindef ( DOMNAME2 !
file sel_domaindef ( DOMNAME1 !
create table kern.t29 (f1 char8, f2 domname3,
     f3 char (4) default 'aaa' ) !
file sel_cols_1tab ( KERN T29 !
create table kern.t30 (f1 char8 key , f2 domname2 key,
     f3 char (4) default 'aaa' ) !
file sel_cols_1tab ( KERN T30 !
create table kern.t31 (f1 domname2 key,
                         f2 domname2 default 'aaa',
                         f3 domname2 not null,
     f4 char (4) default 'aaa' ) !
create table kern.t31 (f1 domname2 key,
                         f2 domname2 range between 'aa' and 'zz',
                         f3 domname2 not null,
     f4 char (4) default 'aaa' ) !
create table kern.t31 (f1 domname2 key,
                         f2 domname2,
                         f3 domname2 not null,
     f4 char (4) default 'aaa' ) !
file sel_cols_1tab ( KERN T31 !
drop domain domname5 !
drop domain do1 !
grant privileges !
file sel_monitor_all ( CACHES PAGES !
commit work !
