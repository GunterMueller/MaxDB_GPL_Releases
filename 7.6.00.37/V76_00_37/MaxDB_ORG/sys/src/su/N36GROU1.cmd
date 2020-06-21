*   *ID* N36GROU1 DBCMD    changed on 1992-09-28-13.26.23 by ADMIN     *
select sum ( t1.t1xc2 ) , sum ( t2.t2xc1 ) from t1, t2 !
fetch !
select sum ( t1xc2 ) , sum ( t2xc1 ) from t1, t2 !
fetch !
select sum( t1.t1xc2 ), sum ( t2.t2xc1 ),sum ( t3.t3xc3 ) from t1,t2,t3 !
fetch !
select sum ( t1xc2 ) from t1, t2 !
fetch !
select count (*), count ( distinct kern.t2.t2xc2 ),
       count ( all ( t3xc5)) from t1,t2,t3 !
fetch !
select avg ( distinct t2.t2xc1 ) ,
       count ( distinct t2.t2xc1 ) ,
       avg ( all kern.t1.t1xc5 ) ,
       sum ( all t1xc5 ) ,
       count ( all ( t1xc5 ) ) from t1,t2 !
fetch !
select sum ( ref.t1xc4 ) , AVG ( ref.t1xc4) from t1,t1 ref !
fetch !
select sum ( t1xc4 ), avg ( t1xc4), t1xc1, count(t1xc1)
    from t1,t2 group by t1xc1 !
fetch !
select sum ( t1xc4 ), sum ( t2xc4)
    from t1, t2 ref
    group by ref.t2xc1
    having ref.t2xc1 > 10
       and sum ( ref.t2xc4) is not null !
fetch !
select avg ( t1xc2), avg ( distinct t1xc5 ) , t1.t1xc5
    from t1, t2
    group by t1xc1 , kern.t1.t1xc5
    having sum ( t1xc5 ) > 0
    order by t1xc5, 2 !
fetch !
select sum( t1xc2) , count ( t1xc2) , avg (t1xc2)
    from t1,t3 group by t1xc1, t3.t3xc4 !
fetch !
