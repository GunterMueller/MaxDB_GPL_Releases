*   *ID* A01VIEW  DBCMD    changed on 1992-09-28-13.22.16 by ADMIN     *
 ***************************
 ***   A 0 1 V I E W     ***
 ***************************!
file connect ( kern test !
create table kern.tab1
  (f1 char(7) range not between 'a' and 'e',
   f2 fixed (5,2) not null, f3 char (3) 
   range  between  'AA' and 'ZZ' ) !
create table kern.tab2
 ( f1 char (7) range not between 'a' and 'e',
   f2 fixed (5,2) not null, f3 char (3)  not null
   range  between  'AA' and 'ZZ' ) !
COMMIT WORK  !
create view  kern.viewtab1  as select *
    from kern.tab2 where  (f2 = 1.0  and f3 > 'KK') or
       (f1 >'ea' ) !
create view  kern.viewtab2  as select *
    from kern.tab2 where  (f2 = 1.0  and f3 > 'KK') or
       (f1 >'c' ) !
create view  kern.viewt3  as select *
    from kern.tab2
    where (f2 between 1.0 and 2.0  and f3 like 'K_K') or
       (f1 sounds  'ea' ) !
create view  kern.viewt4  as select *
    from kern.tab2 where  (f2  not between 1.0 and 2.0
                and f3  not like 'K_K') or
       (f1 not sounds  'ea' ) !
create table kern.t5 (f1 char (8) ) !
create view kern.viewta
    as select * from kern.tab2  where f1 = user !
create view kern.viewt5 as select * from kern.t5
    where f1 = user or f1 <> user !
commit work !
file sel_view1 ( KERN VIEWTAB1 !
file sel_view1 ( KERN VIEWTAB2 !
file sel_view1 ( KERN VIEWT2 !
file sel_view1 ( KERN VIEWT22 !
file sel_view1 ( KERN VIEWT3 !
file sel_view1 ( KERN VIEWT4 !
file sel_view1 ( KERN VIEWT5 !
insert kern.viewt5 values ('PUBLIC') !
insert kern.viewt5 values ('FREIER') !
insert kern.viewt5 values ('PUBLIC') !
select * from kern.viewt5 !
fetch !
commit work !
create index "kern.t5.f1"
	on kern.t5( f1 ) !
file sel_tablestat ( KERN T5 !
file sel_tablestat ( KERN VIEWT5 !
select distinct owner, tablename, indexname, columnname
 from sysdd.indexstatistics 
where owner = 'KERN'
  and tablename = 'T5'
  and indexname = 'kern.t5.f1'!
fetch !
select substr(description,1,33) description,
       numeric_value,
       substr(value,1,6) "VALUE"
 from sysdd.indexstatistics 
where owner = 'KERN'
  and tablename = 'T5'
  and indexname = 'kern.t5.f1'!
fetch !
commit work release !
file connect ( sut sut !
create table person (name char (32), vorname char (15))!
create user neu password w resource !
grant select on person to neu!
commit release !
file connect ( neu w !
create view pers as select * from sut.person !
create table likes (drinker char (15), beer char (7))!
select name, beer from pers, likes
where pers.vorname = likes.drinker!
fetch!
drop table likes !
commit release !
file connect ( sut sut !
drop table person !
drop user neu !
commit release !
