*   *ID* N99A11   DBCMD    changed on 1992-09-28-13.27.11 by ADMIN     *
file connect ( sut sut !
create user otto password os costwarning 2 !
commit release !

file connect ( kern test !
create table t ( a char (250))!
insert t set a = 'aaa'!
grant all on t to otto !
commit release !

file connect ( otto os !
!* The cost value with build of result is too expensive for otto!
explain select /*+buildresult*/ * from kern.t where a like 'a%' !
select /*+buildresult*/ * from kern.t where a like 'a%' !

!* without build of result result otto can afford this query!
explain select * from kern.t where a like 'a%'!
select * from kern.t where a like 'a%'!
rollback work release !
