*   *ID* N99OP    DBCMD    changed on 1992-09-28-13.27.34 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1 fixed(5), spalte2 fixed(5) ) !
insert tab values ( 5 , 2 ) !
select 3++5 from tab !
fetch !
select 3-+5 from tab !
fetch !
select 3+-5 from tab !
fetch !
select 3- -5 from tab !
fetch !
select 3+++5 from tab !
fetch !
select 3+*5 from tab !
fetch !
select 3*+5 from tab !
fetch !
select 3**5 from tab !
fetch !
select 3mod+5 from tab !
fetch !
select 3 mod + 5 from tab !
fetch !
select 3div+5 from tab !
fetch !
select 3+mod5 from tab !
fetch !
select 3 + mod 5 from tab !
fetch !
select ++5 from tab !
fetch !
select - -5 from tab !
fetch !
select +-5 from tab !
fetch !
rollback work release !
