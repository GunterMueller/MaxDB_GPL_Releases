*   *ID* UNTEST4  DBCMD    changed on 1992-09-28-13.28.52 by ADMIN     *
declare erg cursor for select a from t1 where a in
        (select a from t1) !
fetch erg !
*
select a from t1 where a in
        (select a from t1)
&1 &2
select b from t2 !
fetch !
*
select * from erg
&1 &2
select b from t2 !
fetch !
*
select b from t2
&1 &2
select a from t1 where a in
        (select a from t1) !
fetch !
*
select b from t2
&1 &2
select * from erg !
fetch !
