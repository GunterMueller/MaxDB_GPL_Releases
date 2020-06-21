*   *ID* A11SYS   DBCMD    changed on 1992-09-28-13.22.29 by ADMIN     *
file connect ( sut sut !
create user  aaaa  password w dba !
create user bbbb  password w resource !
create user cccc  password w !
create user dddd  password w dba !
create user  aaee password w !
commit release !
file connect ( kern test !
create table kern.tab1
   (a1 char (8) byte, a2 char (8) ascii, a3 char (8) )!
create table kern.tab2
   (b1 char (8) byte, b2 char (8) ascii, b3 char (8))!
grant  insert, delete on kern.tab1 to aaee !
grant  all on kern.tab1 to aaaa !
grant  insert, delete on kern.tab2 to bbbb, cccc !
grant  select, update on kern.tab1 to cccc, dddd , public!
commit work !
file sel_priv_granted ( AAAA !
file sel_priv_granted ( AAEE !
file sel_priv_granted ( BBBB !
file sel_priv_granted ( CCCC !
file sel_priv_granted ( DDDD !
file sel_priv_granted ( PUBLIC !
file sel_priv_on !
commit work release !
file connect ( aaaa w !
file sel_priv_on !
commit work release !
file connect ( bbbb w !
file sel_priv_on !
commit work release !
file connect ( cccc w !
file sel_priv_on !
commit work release !
file connect ( dddd w !
file sel_priv_on !
commit work release !
file connect ( aaee w !
file sel_priv_on !
commit work release !
