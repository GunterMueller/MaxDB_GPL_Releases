*   *ID* A21USER  DBCMD    changed on 1992-09-28-13.22.50 by ADMIN     *
file connect ( sut sut !
create user udba1 password w dba permlimit 10  !
create user udba1 password w dba templimit 20 !
create user udba1 password w dba !
create user udba2 password w dba !
commit work release !
file connect ( udba1 w !
create table t1 (s1 char(5) )!
insert into t1 values ('aaaaa')!
create table t2 (s1 char(5) )!
insert into t2 values ('aaaaa')!
create table t11 (s1 char(5) , s2 char(5))!
insert into t11 values ('aaaaa','bbbbb')!
select * from sysdba.userstatistics where 
  username = 'UDBA1' !
fetch !
create user ures2 password w resource !
commit work release !
file connect ( udba2 w !
create table t1 (bb char(5) , aa char(5) )!
insert into t1 values ('aaaaa','bbbbb')!
create table t2 (s2 char(5) , s1 char(5) )!
insert into t2 values ('aaaaa','ccccc')!
create index "t1.bb"
	on t1( bb ) !
create index "t1.aa"
	on t1( aa ) !
create index "t2.s2"
	on t2( s2 ) !
create index "t2.s1"
	on t2( s1 ) !
create table t22 (p1 char(5), p2 char(5))!
insert into t22 values ('ddddd','fffff')!
select * from sysdba.userstatistics where 
  username = 'UDBA2' !
fetch !
commit work release !
file connect ( ures2 w !
create table t1 (s1 char(5), s2 char(5))!
insert into t1 values ('aaaaa','bbbbb')!
select * from sysdba.userstatistics !
fetch !
commit work release !
file connect ( sut sut !
select * from sysdba.userstatistics where 
  username <> 'SUT' !
fetch !
commit work release !
file connect ( sut sut !
create user elke password os resource costlimit 600 !
commit release !
file connect ( elke os !
create table t ( a char (8))!
insert t set a = 'aaa'!
create table t1 ( b char (8))!
insert t1 set b = 'aaa'!
explain select * from t, t1 where t.a = t1.b !
        select * from t, t1 where t.a = t1.b !
        select * from t, t1 where t.a = t1.b !
commit release !
