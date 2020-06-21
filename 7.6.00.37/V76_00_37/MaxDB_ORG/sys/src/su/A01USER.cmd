*   *ID* A01USER  DBCMD    changed on 1992-09-28-13.22.15 by ADMIN     *
 ***************************
 ***   A 0 1 U S E R     ***
 ***************************!
connect sut
IDENTIFIED BY :A !
SUT
create user  freier password w dba location nixdorf !
create user  freier password w dba !
create user  freier password w resource !
create user  munz password w  resource  location  berlin !
create user  munz password w resource  location  nixdorf!
create user  munz password w resource  !
create user  lamb password w !
commit  work  !

file sel_user !
drop user freier !
drop user frank !

file sel_user !
commit work release !
