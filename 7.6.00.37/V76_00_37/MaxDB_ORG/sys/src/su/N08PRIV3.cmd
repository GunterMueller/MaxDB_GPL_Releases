*   *ID* N08PRIV3 DBCMD    changed on 1992-09-28-13.25.15 by ADMIN     *
file connect (sut sut !
create user r1 password w resource !
create user r2 password w resource !
create user r3 password w resource !
create user r4 password w resource !
commit work release !
file connect (r1 w !
create table tab1 ( spalte fixed(5) ) !
grant all on tab1 to public !
commit work release !
file connect (r2 w !
create table tab2 ( spalte fixed(5) ) !
grant insert on tab2 to public !
commit work release !
file connect (r3  w!
create table tab3 ( spalte fixed(5) ) !
grant delete on tab3 to r2, r4 !
commit work release !
file connect (r1 w !
file sel_priv_granted ( PUBLIC !
file sel_priv_on_tab ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( R3 !
file sel_priv_granted ( R4 !
commit work release !
file connect (r2 w !
file sel_priv_granted ( PUBLIC !
file sel_priv_granted ( R1 !
file sel_priv_on !
file sel_priv_granted ( R3 !
file sel_priv_granted ( R4 !
commit work release !
file connect (r3 w !
file sel_priv_granted ( PUBLIC !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_on !
file sel_priv_granted ( R4 !
commit work release !
file connect (r4 w !
file sel_priv_granted ( PUBLIC !
file sel_priv_granted ( R1 !
file sel_priv_granted ( R2 !
file sel_priv_granted ( R3 !
file sel_priv_on !
commit work release !
