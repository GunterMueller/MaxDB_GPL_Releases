*   *ID* N31CONCA DBCMD    changed on 1992-09-28-13.26.06 by ADMIN     *
create table tab1 ( spalte1 char(20) ascii,
                    spalte2 char(20),
                    spalte3 char(12) ascii,
                    spalte4 char(1),
                    spalte5 fixed( 5,3),
                    spalte6 float(9) ) !
insert tab1 values ( 'Hans-Peter Klein', 'Dortmund', 'Kauderweg 12',
                    'a', 7.2, 8E12 ) !
insert tab1 values ( ' Maria Sonne', ' Buxtehude ', 'Lange Str. 5',
                    null, null, 54 ) !
insert tab1 values ( null, 'Frankfurt/Main', null , '1', 18, 1e-12 ) !
insert tab1 values ( 'Helga Krause', null, null, 'A', 4.333, null ) !
select * from tab1 !
fetch !
select 'a'&'b', '12.' &( ( 'E+12')), ('1 3' &'-31'), ' a'& 'b',
       'a ' & 'b', ( (  'a') & ' b'), 'a'& 'b ' , 'a  ' & '  b'
       from tab1 where spalte1 is null !
fetch !
select ( 'a'& 'b' ) & 'c', ' a '& ' bb '  &  ' c   '
       from tab1 where spalte1 is null !
fetch !
select 'a' & ' '&  'b' from tab1 where spalte1 is null !
fetch !
select '', 'a' & '' & 'b' from tab1 where spalte1 is null !
fetch !
select spalte1 & spalte3, tab1.spalte4 &  '44 4' ,
       '8a' & kern.tab1.spalte3, kern.tab1.spalte3 & tab1.spalte4
       from tab1 !
fetch !
select spalte1&spalte3, spalte2 &tab1.spalte3   from tab1 !
fetch !
select a & 'b' from tab1 !
select 'a' & b from tab1 !
select & 'b' from tab1 !
select 'a' & from tab1 !
select null & 'b' from tab1 !
fetch !
select 'b' & null from tab1 !
fetch !
select spalte1 & tab1.spalte2 & kern.tab1.spalte3 & spalte4
       from tab1 !
fetch !
select * from tab1 where spalte1 & spalte2 = ' Maria Sonne Buxtehude ' !
fetch !
select * from tab1 where spalte1 & spalte2 = ' Maria Sonne Buxtehude'  !
fetch !
select * from tab1 where ( ((spalte3) & spalte4) = '1' ) !
fetch !
select * from tab1 where spalte2 & spalte3 is null !
fetch !
select spalte1 & spalte5 from tab1 !
select spalte2 & spalte6 from tab1 !
select 'a' & spalte6 from tab1 where spalte4 = 'A' !
rollback work !
create table tab1 ( spalte time ) !
insert tab1 values ( '11111111' ) !
update tab1 set spalte = '23' & '23' & '23' !
rollback work !
