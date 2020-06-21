*   *ID* N00T0803 DBCMD    changed on 1992-09-28-13.24.54 by ADMIN     *
 ***  8.4  <rename table statement> !
file connect ( sut sut !
create user d3 password d3xw dba !
create user r3 password r3xw resource !
create user p3 password p3xw !
commit release !
file connect ( kern test !
create table t1 ( t1xc1 fixed(5) key, t1xc2 char(10) ) !
insert t1 values ( 1, 'Hans' ) !
grant all on t1 to d3 !
rename table t1 to renamext1 !
commit work release !
file connect ( d3 d3xw !
insert kern.renamext1 set t1xc1 = 2 !
select * from kern.renamext1 !
fetch !
commit work release !
file connect ( r3 r3xw !
create table r3xt1 ( r3xt1xc1 char(10) , r3xt1xc2 date ) !
insert r3xt1 values ( 'HAMBURG', '19651025' ) !
grant all on r3xt1 to d3 !
create view view1 ( r3xt1xc1 )
   as select r3xt1xc1 from r3xt1
      where r3xt1xc1 between 'B' and 'K'
   with check option !
grant select on view1 to p3 !
create index "r3xt1.r3xt1xc1"
	on r3xt1( r3xt1xc1 asc ) !
create unique index indexn1 on r3xt1 ( r3xt1xc2 ) !
create synonym synxr3xt1 for r3xt1 !
commit work release !
file connect ( d3 d3xw !
create synonym s1 for r3.r3xt1 !
commit work release !
file connect ( r3 r3xw !
rename table r3xt1 to renxr3xt1 !
commit work !
file sel_table !
file sel_cols_tab ( RENXR3XT1 !
file sel_index_tab ( RENXR3XT1 !
file sel_priv_granted ( P3 !
file sel_priv_on ( RENXR3XT1 !
file sel_synonym !
declare sel_synonym cursor for 
 select substr(synonymname,1,18) synonymname,
        substr(tableowner,1,18)  owner,
        substr(tablename,1,18)   tablename
   from domain.synonyms 
  where synonymname = 'SYNXR3XT1'  !
fetch sel_synonym into :a,:b,:c !
close sel_synonym !
file sel_view ( VIEW1 !
insert renxr3xt1 values ( 'AACHEN' , '19651102' ) !
commit work release !
file connect ( d3 d3xw !
update r3.renxr3xt1 set r3xt1xc1 = 'HAMELN'
   where r3xt1xc1 = 'HAMBURG' !
file sel_synonym !
insert s1  values ( 'KOELN', '19651103'  ) !
commit work release !
file connect ( p3 p3xw !
file sel_table !
select r3.view1.r3xt1xc1 from r3.view1 !
fetch !
commit work release !
file connect ( d3 d3xw !
create table d3xt1 ( d3xt1xc1 fixed( 5) ) !
insert d3xt1 values ( 15 ) !
rename table d3xt1 to renamexd3xt1 !
grant all on renamexd3xt1 to r3, p3 !
create index "renamexd3xt1.d3xt1xc1"
	on renamexd3xt1( d3xt1xc1 ) !
create view view2 as select * from renamexd3xt1
   where d3xt1xc1 < 1000 with check option !
create synonym s2 for renamexd3xt1 !
rollback work !
file sel_table !
file sel_synonym !
file sel_priv_on !
commit work release !
