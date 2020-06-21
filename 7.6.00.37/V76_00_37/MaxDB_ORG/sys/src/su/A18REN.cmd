*   *ID* A18REN   DBCMD    changed on 1992-09-28-13.22.49 by ADMIN     *
file connect ( kern test !
create table sdbat1 (s1 char(5) key, s2 char(5), s3 char(5), 
  s4 integer, s5 integer)!
insert sdbat1 values ('aaaaa','s2001','s3001',10,20)!
create view vsdba1 as select s1,s2,s3 from sdbat1 where s4 = 10 !
create view vsdba2 as select s1,s2 from vsdba1 where
s3 = 's3001'!
create view vsdba3 as select s1 from vsdba2 where
s2 = 's2001'!
create index "sdbat1.s2"
	on sdbat1( s2 ) !
create index "sdbat1.s3"
	on sdbat1( s3 ) !
create index "sdbat1.s4"
	on sdbat1( s4 ) !
create index i1 on sdbat1 (s2,s3)!
create index i2 on sdbat1 (s4,s5)!
create view vsdba4 (a,b,c) as select s3, s4, s5 from sdbat1 !
create view vsdba5 (x,y) as select b, c from vsdba4 where
a = 's3001'!
select * from sdbat1 !
fetch !
select * from vsdba1 !
fetch !
select * from vsdba2 !
fetch !
select * from vsdba3 !
fetch !
select * from vsdba4 !
fetch !
select * from vsdba5 !
fetch !
rename table sdbat1 to topdbat1 !
file sel_view ( VSDBA1 !
select c_definition into :def from sysdd.views
  where owner = usergroup and tablename = 'VSDBA1'!
file sel_view ( VSDBA4 !
select c_definition into :def from sysdd.views
  where owner = usergroup and tablename = 'VSDBA4'!
select * from topdbat1 !
fetch !
select * from vsdba1 !
fetch !
select * from vsdba2 !
fetch !
select * from vsdba3 !
fetch !
select * from vsdba4 !
fetch !
select * from vsdba5 !
fetch !
commit work release !
