*   *ID* SQLMTST2 DBCMD    changed on 1992-09-28-13.28.10 by ADMIN     *
declare c cursor for select * from t3
union
select * from t1
&1
select * from t2 !
fetch c into :a !
close c !
*
declare c cursor for (select * from t3
union
select * from t1)
&1
select * from t2 !
fetch c into :a !
close c !
*
declare c cursor for select * from t3
union
select * from t1
&1 all
select * from t2 !
fetch c into :a !
close c !
*
