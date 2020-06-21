*   *ID* N01PRIV9 DBCMD    changed on 1992-09-28-13.25.02 by ADMIN     *
file connect (sut sut !
create user d1 password w dba !
commit work release !
file connect (d1 w !
create user d1_r1 password w resource !
create user d1_p1 password w !
file sel_table !
file sel_priv_granted ( D1 !
commit work release !
file connect (d1_r1 w !
file sel_table !
file sel_priv_granted ( D1_R1 !
create table d1_r1 ( spalte char (3) ) !
file sel_table !
file sel_priv_granted ( D1_R1 !
grant insert on d1_r1.d1_r1 to d1_p1, d1_p1 !
file sel_priv_granted ( D1_P1 !
grant insert on d1_r1 to d1 !
commit work release !
file connect (d1 w !
insert into d1_r1.d1_r1 set spalte = 'aaa' !
rollback work release !
file connect (sut sut !
create user p1 password w !
create user p2 password w !
create user p3 password w !
commit release !
file connect ( kern test !
create table kern.tabelle ( spalte fixed(5) ) !
grant insert, delete, select on kern.tabelle to p1, p2, p3 !
revoke select on kern.tabelle, delete on kern.tabelle from p1 !
revoke insert on kern.tabelle from p1, p2 !
revoke select on kern.tabelle from p2, p3 !
revoke insert on kern.tabelle from p3, p3 !
revoke delete on kern.tabelle from p3, p3 !
file sel_priv_granted ( P3 !
grant select ( spalte ) on kern.tabelle to p2 !
revoke all on kern.tabelle from p2 !
file sel_priv_granted ( P2 !
rollback work release !
file connect (sut sut !
drop user p1 !
drop user p2 !
drop user p3 !
commit release !
