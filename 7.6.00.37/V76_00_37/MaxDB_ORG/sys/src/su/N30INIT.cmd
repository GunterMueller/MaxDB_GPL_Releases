*   *ID* N30INIT  DBCMD    changed on 1992-09-28-13.26.03 by ADMIN     *
file CONNECT ( KERN TEST !
create table tab ( spalte1 fixed(5,2),
                   spalte2 float(7),
                   spalte3 float(18),
                   spalte4 char(10) ,
                   spalte5 char(10) ascii ) !
insert tab values ( 32, 12, null, 'Paul', null ) !
insert tab values ( 281.35, 129.7e32,
                    1.2345678901234567e-12, null, '7aA?-' ) !
insert tab values ( null, null, -234.5E-8, '123', '-123' ) !
insert tab values ( - 0.32, -18.7, -3.e62, '-1.2E12 ', ' 6.78 ' ) !
insert tab values ( 0, 0, 0, '000', '+6.72E-32' ) !
select * from tab !
fetch !
commit work !
