*   *ID* N31LENGT DBCMD    changed on 1992-09-28-13.26.08 by ADMIN     *
select length ( 'Paul' ), LENGTH (' Paul'), length('  Paul') ,
       ( length ('Paul ') ) , length (  'Paul  ') ,length('Pa ul') ,
       length ('Pa  ul') , length(' Pa ul ')
       from tab where spalte2 is null !
fetch !
select length ( spalte1), length(tab.spalte2) ,
       length(kern.tab.spalte3 )
       from tab !
fetch !
select ( LENGTH ( USER ) ) from tab where spalte2 is null !
fetch !
select length(date)* 3 from tab where spalte2 is null !
fetch !
select length ( time ) from tab where spalte2 is null !
fetch !
select * from tab where length(spalte1) = 0 !
fetch !
select * from tab where (( length ( spalte2)) >= 10) !
fetch !
select * from tab where length ( kern.tab.spalte2 ) < 10 !
fetch !
select  * from tab where LENGTH ( tab.spalte1 ) <length(spalte2) !
fetch !
select * from tab where length( spalte1  ) > 10 !
fetch !
select * from taB WHERE length( spalte1 ) is null !
select * from tab  where length ( spalte1 ) < 0 !
select * from tab where 3 * length(spalte1) + 5 = 38 !
fetch !
select length ( 3 ) from tab !
select length ( spalte4 ) from tab !
select length ( '' ) from tab where spalte2 is null !
fetch !
select length ( ' ' ) from tab where spalte2 is null !
fetch !
select length ( '  ' ) from tab where spalte2 is null !
fetch !
select length ( spalte1) + length( spalte2 ) from tab !
fetch !
commit work !
create table t1 ( c1 char(10) ) !
create table t2 ( c2 fixed(2) ) !
insert t1 set c1 = 'a' !
insert t2 set c2 = 1 !
select * from t1, t2 where t2.c2 = length (t1.c1 ) !
fetch !
rollback work !
