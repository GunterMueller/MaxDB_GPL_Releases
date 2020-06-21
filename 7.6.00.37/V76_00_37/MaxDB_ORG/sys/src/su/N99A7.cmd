*   *ID* N99A7    DBCMD    changed on 1992-09-28-13.27.22 by ADMIN     *
file connect ( sut sut !
create usergroup g1 standard !
create user user1 password w usergroup g1 !
commit release !
file connect ( kern test !
create table tab1 ( s1 char(64) key,
                    s2 char(64) ascii  key ) !
grant all on tab1 to g1 !
commit work release !
file connect ( user1 w !
insert kern.tab1 values ( user, usergroup ) !
commit work release !
file connect ( sut sut !
drop usergroup g1 !
commit release !
file connect ( kern test !
drop table kern.tab1 !
commit work release !
