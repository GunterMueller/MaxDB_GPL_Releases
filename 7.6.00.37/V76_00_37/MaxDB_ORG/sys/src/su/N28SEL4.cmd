*   *ID* N28SEL4  DBCMD    changed on 1992-09-28-13.25.54 by ADMIN     *
 *** SELECT FIRST und SELECT LAST ueber multiplen Index und
 *** WHERE Bedingung !
file connect ( kern test !
create table tab5 ( spalte1 fixed(3) key,
                    spalte2 fixed(3) key,
                    spalte3 fixed(3) key,
                    spalte4 fixed(3) key ) !
insert tab5 values ( 5,4,3,2 ) !
insert tab5 values ( 2,3,4,5 ) !
insert tab5 values ( 1,1,1,1 ) !
insert tab5 values ( 8,6,4,6 ) !
insert tab5 values ( 9,9,9,9 ) !
insert tab5 values ( 4,2,7,8 ) !
create index indextab5 on tab5 ( spalte4, spalte1 ) !
rollback work release !
