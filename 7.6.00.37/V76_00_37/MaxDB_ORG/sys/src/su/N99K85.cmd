*   *ID* N99K85   DBCMD    changed on 1992-09-28-13.27.32 by ADMIN     *
file connect ( sut sut !
create user d1 password w dba !
commit work release !
file connect ( d1 w !
create table d1xt1 ( d1xt1xc1 fixed(5) ) !
grant all on d1xt1 to public !
commit work release !
file connect ( sut sut !
select * from d1.d1xt1 !
commit work release !
