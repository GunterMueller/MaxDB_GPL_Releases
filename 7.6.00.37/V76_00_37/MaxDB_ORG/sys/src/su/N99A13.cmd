*   *ID* N99A13   DBCMD    changed on 1992-09-28-13.27.12 by ADMIN     *
file connect ( sut sut !
create domain dom fixed(5) !
create user d password w dba !
create user r password w resource !
create user p password w !
commit work release !
file connect ( p w !
file sel_domain !
drop domain dom !
rollback work release !
file connect ( r w !
file sel_domain !
drop domain dom !
rollback work release !
file connect ( d w !
file sel_domain !
drop domain dom !
rollback work release !
file connect ( sut sut !
drop domain dom !
commit work release !
