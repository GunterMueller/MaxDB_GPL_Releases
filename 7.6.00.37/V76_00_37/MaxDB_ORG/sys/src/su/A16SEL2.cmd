*   *ID* A16SEL2  DBCMD    changed on 1992-09-28-13.22.38 by ADMIN     *
file CONNECT ( KERN TEST !
create table t ( spalte1 char(8) key,
                 spalte2 fixed(6) key,
                 spalte3 char(6),
                 spalte4 char(6) ) !
create index i1 on t ( spalte1, spalte4 ) !
insert t values ( '00000400', 2, 'aaa', '000005' ) !
rollback work release !
* PTS 1125272-5 !
file connect ( kern test !
parsing !
SELECT 1 + ? from DUAL!
executing !
data !
select !
1 
n 38 7

pars_then_ex !
fetch !
parsing !
parameter !
SELECT 1+? from DUAL!
c 10

executing !
data !
select !
1 
n 38 7

no_parameter!
pars_then_ex !
fetch !

parsing !
SELECT ?  from DUAL!
executing !
data !
select !
1 
n 38 7

pars_then_ex !
fetch !
parsing !
parameter !
SELECT ? from DUAL!
c 10

executing !
data !
select !
1 
c 10 '7'

no_parameter!
pars_then_ex !
fetch !
pars_execute !
rollback release !
