*   *ID* N08PRIV4 DBCMD    changed on 1992-09-28-13.25.15 by ADMIN     *
file connect ( sut sut !
create user sowa password w dba!
commit release !
file connect ( kern test !
create table tpriv (f1 char(10), f2 char(10), f3 char(10),
u1 char(10), u2 char (2))!
grant select, insert, delete on tpriv to sowa!
grant update (u1, u2) on tpriv to sowa!
commit release !
file connect ( sowa w !
update kern.tpriv set u1 = 'aaa' where f1 = '1'!
commit release !
file connect ( kern test !
drop table tpriv!
commit release !
file connect ( sut sut !
drop user sowa !
commit release !
