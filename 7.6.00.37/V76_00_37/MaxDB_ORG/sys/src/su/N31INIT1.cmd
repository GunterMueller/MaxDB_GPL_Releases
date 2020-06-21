*   *ID* N31INIT1 DBCMD    changed on 1992-09-28-13.26.08 by ADMIN     *
create table tab ( spalte1 char(20),
                   spalte2 char(20) ascii,
                   spalte3 char(30),
                   spalte4 fixed(5,1),
                   spalte5 float(10) ) !
insert tab values ( ' Hans-Peter Klein ', 'Newvalue', 'Frankfurt/Main',
                    32, 9E+12 ) !
insert tab values ( null, ' 32 * .1e12  ', 'GROSS-GERAU', null, null ) !
insert tab values ( 'Paul Lincke', null, 'Paul-Lincke-Ufer 1a',
                     22.3, -27.3 ) !
insert tab values ( NULL, 'Chars  ', null, 2.2, 1E-12 ) !
select * from tab !
fetch !
commit work !
