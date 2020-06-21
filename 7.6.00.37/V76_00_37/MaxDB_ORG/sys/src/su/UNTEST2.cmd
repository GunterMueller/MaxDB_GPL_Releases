*   *ID* UNTEST2  DBCMD    changed on 1992-09-28-13.28.51 by ADMIN     *
* uniontst  ********
select * from t1
&1 &3
select * from t2
&2 &4
select * from t1 !
fetch !
*
(select * from t1
&1 &3
select * from t2)
&2 &4
select * from t1 !
fetch !
*
select * from t1
&1 &3
(select * from t2
&2 &4
select * from t1) !
fetch !
