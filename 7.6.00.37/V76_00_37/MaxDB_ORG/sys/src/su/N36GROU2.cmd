*   *ID* N36GROU2 DBCMD    changed on 1992-09-28-13.26.24 by ADMIN     *
select sum ( t1.t1xc2 + t2.t2xc1 ),
       avg ( t1xc4 * 5 * t2xc5 ),
       ( sum ( distinct kern.t1.t1xc4 / t2xc5 ) )
    from t1, t2
    group by t2xc1
    order by 1 desc, 3 asc !
fetch !
