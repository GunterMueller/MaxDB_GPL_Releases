*   *ID* AK02T1   DBCMD    changed on 1992-09-28-13.22.07 by ADMIN     *
CREATE TABLE kern.alT1 ( F1 CHAR (4) ascii   key
   defAULT 'aaa' RANGE IN ( 'aaa' ) ,
   f4 char (6) range not in ('a', 'f', 'c', 'r', 'e', 't' ),
   F2 FIXED (3,1 ) not null ,
   F21 FIXED (5,1 ) not null range in (-512, 500, -500,512, -22, -191),
   F3 char  (4 ) not null RANGE BETWEEN 'xx'  AND 'zzz'  ) !
file sel_cols_1tab ( KERN ALT1 !
alter table kern.alt1
 add neu1 char(12) range in ('qqq','www') !
alter table  kern.alt1 add (
  neu2 char (12) default 'uuu' range between 'sss'and 'www' ,
  neu3 char (4) )  !
alter table  kern.alt1 add (
  neu2 char (12)  range between 'sss'and 'www' ,
  neu3 char (4) not null )  !
alter table  kern.alt1 add (
  neu2 char (12) range between 'sss'and 'www' ,
  neu3 char (4) )  !
alter table  kern.alt1 drop   f2, f21    !
alter table  kern.alt1 alter constraint f3 check f3 in ('xx', 'sss')  !
file sel_cols_1tab ( KERN ALT1 !
create view
  kern.valt1 as select * from kern.alt1 where f3 <> 'xx' !
create view  kern.valt2 as select * from kern.alt1
  where neu2 < 'ttt' !
create view  kern.valt3 as select * from kern.valt1
  where neu2 < 'ttt' !
create synonym svalt1  for   kern.valt1 !
create synonym svalt2  for   kern.valt2 !
create synonym salt1   for   kern.alt1  !
file sel_synonym !
file sel_view1 ( KERN VALT1 !
file sel_view1 ( KERN VALT2 !
file sel_cols_1tab ( KERN ALT1 !
drop view  kern.valt2  !
file sel_synonym !
file sel_view1 ( KERN VALT1 !
file sel_view1 ( KERN VALT2 !
file sel_cols_1tab ( KERN ALT1 !
drop table kern.alt1  !
file sel_synonym !
file sel_view1 ( KERN VALT1 !
file sel_view1 ( KERN VALT2 !
file sel_cols_1tab ( KERN ALT1 !
commit work release !
