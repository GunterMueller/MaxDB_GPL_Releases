*   *ID* A05INIT  DBCMD    changed on 1992-09-28-13.22.26 by ADMIN     *
 ***************************
 ***   A 0 5 I N I T     ***
 ***************************!
file connect ( sut sut !
create user  freier password w dba !
create user  freier password ap dba !
create user  freier password w resource !
create user  munz  password w resource  location  berlin !
create user  munz  password w resource  location  nixdorf!
create user  munz  password rudi resource  !
create user  lamb password w !
commit  work !
file sel_user !
create user publik password pw dba !
commit work release !
file connect ( publik pw !
create table  publik.p1  (f1 char (12) key) !
create table publik.p2  (f1 char (12) key,
                         f2 char (4) , f3 fixed (6,2)) !
create table publik.p3 (f1 char (12),
                        f2 char (4) , f3 fixed (6,2)) !
create table  publik.t1 (f1 char (12),
                        f2 char (4), f3 fixed (6,2)) !
commit work release !
file connect ( publik pw !
grant    insert  on publik.p1 to freier !
grant   insert, delete  on publik.p1 to lamb, munz !
grant   insert, delete  on publik.p2, select on publik.p1 to lamb, munz !
grant   select (f1, f2), update (f3, f2),
 delete  on publik.p2 to lamb   !
grant   select (f1, f2), update (f3, f2),
 delete  on publik.p3 to lamb, freier!
grant  all on publik.p2, all on publik.t1  to freier !
commit work release !
file connect ( publik pw !
file sel_priv_grant_on ( P1 !
file sel_priv_grant_on ( P2 !
file sel_priv_grant_on ( P3 !
file sel_priv_granted ( FREIER !
file sel_priv_granted ( MUNZ !
file sel_priv_granted ( LAMB !
commit work release !
