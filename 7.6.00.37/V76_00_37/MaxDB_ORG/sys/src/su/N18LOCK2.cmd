*   *ID* N18LOCK2 DBCMD    changed on 1992-09-28-13.25.27 by ADMIN     *
 *** Fehlerhafte 'LOCK' wegen fehlerhafter Syntax.
 *** Vor diesem File muss 'N18LOCK1 VDNMAC' aufgerufen
 *** werden.
 ***********************************************************!
file connect ( kern test !
lock t1 in share mode !
lock table t1 in !
lock table t1 in share  !
lock table t1 in exclusive !
locktable t1 in exclusive mode !
locktest table t1 in exclusive mode !
lockrow t3 key t3_c1 = -12 in share mode !
lock table t1 inshare mode !
lock table t1 inexclusive mode !
lock table t1 in sharemode !
lock table t1 in exclusivemode !
lock table t1 in exclusiv mode !
 *** fehlerhafte 'LOCK' wegen fehlerhafte Semantik.
 ***********************************************************!
lock row t1 key t1_c1  = 'abc' in share mode !
lock row t1 key spalte = 'abc' in share mode !
lock row t4 key t4_c2  = 5.5   in share mode !
lock table t6 in exclusive mode !
lock row t4 key t4_c1 = 'Paul', t4_c2 = 5 in share mode !
lock row t5 key t5_c1 = +33 in exclusive mode !
lock row t5 key t5_c1 = +33 , t5_c2 = 33 in exclusive mode !
lock row t5 key t5_c1 = -33 , t5_c2 = 33 , t5_c3 = '1'
                                         in exclusive mode !
lock table t2 in exclusive mode table t3 in share mode !
lock row t3 key t3_c1 = 5 table t4 in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work release !
