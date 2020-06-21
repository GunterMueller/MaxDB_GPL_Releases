*   *ID* N34AVG1  DBCMD    changed on 1992-09-28-13.26.16 by ADMIN     *
select avg(5), ( ( avg ( -3.2) )), (AVG (-10/3) )from tab !
fetch !
select ( avg (all 2*3 )), avg ( distinct (2/3)) from tab !
fetch !
select avg (spalte2 ), avg(tab.spalte3),AVG  (  kern.tab.spalte4)
    from tab !
fetch !
select ( avg(  all tab.spalte2)), avg( DISTINCT   tab.spalte2) from tab !
fetch !
select avg ( spalte4), avg( all (kern.tab.spalte3 ))
    from tab where tab.spalte1 is  null or tab.spalte4  is null !
fetch !
select avg(spalte2), avg(distinct ( tab.spalte2 ))
    from tab where spalte2 is null !
fetch !
select all  avg ( spalte2+ spalte3 )  from tab !
fetch !
select distinct  avg ( all spalte2+spalte3 )
    from tab where spalte2<>82.3 !
fetch !
select distinct (  avg ( tab.spalte2 +  1 * tab.spalte3)) from tab !
fetch !
declare erg1 cursor for 
select (avg(5)), avg (spalte2) from tab !
fetch erg1 !
declare erg2 cursor for 
select distinct avg ( all spalte4 ) from tab where spalte4<0 !
fetch erg2 !
declare erg3 cursor for 
select all  avg ( spalte2 +kern.tab.spalte3 +spalte4),
            avg ( spalte2 +1*tab.spalte3)
    from tab where spalte4 >= 0 !
fetch  erg3 !
select avg(spalte2) average from tab !
fetch !
declare erg4 cursor for 
select all avg(spalte2), avg(tab.spalte2) averag from tab !
fetch  erg4 !
select distinct avg(5) average1, Avg( 5*(spalte4 )) average2
   from tab !
fetch !
select distinct avg( distinct spalte2 ) from tab !
fetch !
select spalte2, avg( spalte2) from tab !
select avg(spalte2), * from tab !
select 2*3, avg ( spalte2 ) from tab !
select avg ( all avg(spalte2)) from tab !
select avg ( avg(5)) from tab !
select avg(spalte1 ) from tab !
select avg ( a ) from tab !
select (avg(spalte2), avg(spalte2 )) from tab !
select * from tab where avg(5) > 0 !
commit work !
* PTS 1131651-2 !
file connect ( kern test !
create table t10 (k char(10) , i int)!
insert into t10 values ('a', 1)!
insert into t10 values ('a', 5)!
select k,sum(i) from t10 group by k order by sum(i)!
fetch!
select k,avg(i) from t10 group by k order by avg(i)!
fetch!
select k from t10 group by k order by sum(i)!
fetch!
select k from t10 group by k order by avg(i)!
fetch!
select k from t10 group by k order by sum(i)+1!
fetch!
select k from t10 group by k order by avg(i)+1!
fetch!
rollback release !
