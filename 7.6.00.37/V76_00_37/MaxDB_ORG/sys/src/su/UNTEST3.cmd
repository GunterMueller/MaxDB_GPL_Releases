*   *ID* UNTEST3  DBCMD    changed on 1992-09-28-13.28.52 by ADMIN     *
* uniontst  ********
select * from t1
&1 &4
select * from t2
&2 &3
select * from t1 !
fetch !
*
(select * from t1
&1 &4
select * from t2)
&2 &3
select * from t1 !
fetch !
*
select * from t1
&1 &4
(select * from t2
&2 &3
select * from t1) !
fetch !
