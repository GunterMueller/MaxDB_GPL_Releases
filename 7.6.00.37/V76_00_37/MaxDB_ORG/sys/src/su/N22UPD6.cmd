*   *ID* N22UPD6  DBCMD    changed on 1992-09-28-13.25.34 by ADMIN     *
file connect ( kern test !
create table t ( a fixed (7))!
create unique index "t.a"
	on t( a ) !
insert t set a = 3!
insert t set a = 1!
update t set a = a + 2 where a = 1!
update t set a = 2!
select * from t!
fetch !
update t set a = null!
select * from t!
fetch !
rollback work release !
