*   *ID* N99K32   DBCMD    changed on 1992-09-28-13.27.28 by ADMIN     *
file connect ( kern test !
create table tab1 ( spalte11 char(10) key ,
                    spalte12 fixed(5,2),
                    spalte13 float(5) ) !
insert tab1 values ( 'HANS'    , 22.52 , null ) !
insert tab1 values ( 'PAUL'    ,   0   ,  3E2 ) !
insert tab1 values ( 'JOACHIM' , 17.1  ,   0  ) !
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
create table tab2 ( spalte21 fixed(4),
                    spalte22 char(32) ,
                    spalte23 float(4) ) !
insert tab2 values ( 300   , 'HANS'   , null ) !
insert tab2 values ( 22.00 , 'FRIEDA' , 0    ) !
insert tab2 values ( 12    , 'JOACHIM', 0    ) !
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++!
create table tab3 ( spalte31 fixed(4) ,
                    spalte32 char(32) ,
                    spalte33 float(4) ) !
insert tab3 values ( 1000, 'HANS'    , null ) !
insert tab3 values ( 2000, 'VOLKER' , 100  ) !
insert tab3 values ( 3000, 'ANDREAS' , 200  ) !
insert tab3 values ( 4000, 'SIEGFRIED' , 300  ) !
commit!
 * nur 16 benannte Ergebnismengen moeglich +++++++++++++++++!
declare erg1  cursor for select * from tab1 !
declare erg2  cursor for select * from tab2 !
declare erg3  cursor for select * from tab3 !
declare erg4  cursor for select * from tab1 !
declare erg5  cursor for select * from tab2 !
declare erg6  cursor for select * from tab3 !
declare erg7  cursor for select * from tab1 !
declare erg8  cursor for select * from tab2 !
declare erg9  cursor for select * from tab3 !
declare erg10 cursor for select * from tab1 !
declare erg11 cursor for select * from tab2 !
declare erg12 cursor for select * from tab3 !
declare erg13 cursor for select * from tab1 !
declare erg14 cursor for select * from tab2 !
declare erg15 cursor for select * from tab3 !
declare erg16 cursor for select * from tab1 !
declare erg17 cursor for select * from tab2 !
declare erg1 cursor for
select * from tab1 !
declare erg10 cursor for
select * from tab2 !
declare erg17 cursor for
select * from tab3 !
 * die mit DECLARE erzeugten Ergebnismengen werden +++++++++
 * durch ROLLBACK in Zukunft wieder freigegeben    +++++++++!
rollback !
declare erg17 cursor for
select * from tab3 !
rollback release !
file connect ( kern test !
 *    wird der Katalog richtig aufgeraeumt         +++++++++!
declare erg cursor for select * from tab1!
declare erg cursor for select * from tab1!
close erg!
declare erg cursor for select * from tab1!
close erg!
 *    Absturz in rest_select wenn SETMEM           +++++++++!
(SELECT * FROM TAB2 UNION SELECT * FROM TAB3)
where spalte21 is not null !
fetch!
 * SELECT + DECLARE - Tests                        +++++++++!
select * from tab2 union select * from tab3 order by 1!
declare erg cursor for
select * from tab2 union select * from tab3 order by 1!
fetch!
fetch erg !
close erg!
declare erg cursor for select * from tab2 union select * from tab3 !
close erg!
declare erg cursor for (SELECT * FROM TAB2 UNION SELECT * FROM TAB3)
where spalte21 is not null !
select * from tab1 !
(select * from tab1) !
(((select * from tab1))) !
select * from tab2 union select * from tab3 !
(select * from tab2 union select * from tab3) !
(((select * from tab2 union select * from tab3))) !
(((select * from tab2) union (select * from tab3))) !
(((select * from tab2)) union ((select * from tab3))) !
(((select * from tab2))) union (((select * from tab3))) !

select distinct * from tab1 !
(select distinct * from tab1) !
(((select distinct * from tab1))) !
select distinct * from tab2 union select distinct * from tab3 !
(select distinct * from tab2 union select * from tab3) !
(((select distinct * from tab2 union select * from tab3))) !
(((select distinct * from tab2) union (select * from tab3))) !
(((select distinct * from tab2)) union ((select * from tab3))) !
(((select distinct * from tab2))) union (((select * from tab3))) !
(SELECT * FROM tab3, (SELECT * FROM TAB2, ((SELECT * FROM TAB3))),
 ((SELECT * FROM TAB3))) !

declare erg cursor for
select * from tab1 !
close erg !
declare erg cursor for
(select * from tab1) !
close erg !
declare erg cursor for
(((select * from tab1))) !
close erg !
declare erg cursor for
select * from tab2 union select * from tab3 !
close erg !
declare erg cursor for
(select * from tab2 union select * from tab3) !
close erg !
declare erg cursor for
(((select * from tab2 union select * from tab3))) !
close erg !
declare erg cursor for
(((select * from tab2) union (select * from tab3))) !
close erg !
declare erg cursor for
(((select * from tab2)) union ((select * from tab3))) !
close erg !
declare erg cursor for
(((select * from tab2))) union (((select * from tab3))) !
close erg !

declare erg cursor for
select distinct * from tab1 !
close erg !
declare erg cursor for
(select distinct * from tab1) !
close erg !
declare erg cursor for
(((select distinct * from tab1))) !
close erg !
declare erg cursor for
select distinct * from tab2 union select distinct * from tab3 !
close erg !
declare erg cursor for
(select distinct * from tab2 union select * from tab3) !
close erg !
declare erg cursor for
(((select distinct * from tab2 union select * from tab3))) !
close erg !
declare erg cursor for
(((select distinct * from tab2) union (select * from tab3))) !
close erg !
declare erg cursor for
(((select distinct * from tab2)) union ((select * from tab3))) !
close erg !
declare erg cursor for
(((select distinct * from tab2))) union (((select * from tab3))) !
close erg !
declare erg cursor for
(SELECT * FROM tab3, (SELECT * FROM TAB2, ((SELECT * FROM TAB3))),
 ((SELECT * FROM TAB3))) !
close erg !

declare erg cursor for
SELECT tab2.spalte21, tab2.spalte22, tab2.spalte23 FROM TAB2,
(SELECT * FROM tab2) !
close erg !
declare erg cursor for
SELECT * FROM tab2,
(SELECT spalte21, spalte22, spalte23 FROM TAB2) !
close erg !
declare erg cursor for
SELECT * FROM tab2,
(SELECT * FROM TAB2) !
close erg !
declare erg cursor for
SELECT tab2.spalte21, tab2.spalte22, tab2.spalte23 FROM TAB2,
(SELECT spalte21, spalte22, spalte23 FROM TAB2) !
close erg !

(SELECT SPALTE22 FROM TAB2
union SELECT SPALTE11 FROM TAB1) !
fetch !
declare erg cursor for
(SELECT SPALTE22 FROM TAB2
union SELECT SPALTE11 FROM TAB1) !
close erg !
SELECT * FROM TAB3 WHERE SPALTE32 in
(SELECT SPALTE22 FROM TAB2
union SELECT SPALTE11 FROM TAB1) !
fetch !
declare erg cursor for
SELECT * FROM TAB3 WHERE SPALTE32 in
(SELECT SPALTE22 FROM TAB2
union SELECT SPALTE11 FROM TAB1) !
close erg !
 * INSERT-SELECT  - Tests                          +++++++++!
create table tab4 like tab3 !
rename column tab4.spalte31 to spalte41 !
rename column tab4.spalte32 to spalte42 !
rename column tab4.spalte33 to spalte43 !
commit !
select * from tab4 !
fetch !
insert tab4 (spalte42) select spalte32 from tab3 union
select spalte22 from tab2!
select * from tab4 !
fetch !
rollback !
select * from tab4 !
fetch !

drop table tab1 !
drop table tab2 !
drop table tab3 !
drop table tab4 !
commit work release !
