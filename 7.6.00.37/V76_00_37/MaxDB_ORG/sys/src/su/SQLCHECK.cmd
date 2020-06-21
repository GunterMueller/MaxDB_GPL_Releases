file connect ( kern test sqlmode &1 !
create table checktab1 ( a char (4))!
parsing !
insert into checktab1 values (:a INDICATOR :ind)! 
pars_execute!
commit work !
create table checktab2 (a float (5) )!
insert into checktab2 values (8.0) !
insert into checktab2 values (7.0) !
select a into :a from checktab2 where a = any (2+4, 2*4) !
insert into hextable values (x'1234') !
commit work release !
file connect ( kern test !
drop table checktab1 !
drop table checktab2 !
commit release !
