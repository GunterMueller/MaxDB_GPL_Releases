*   *ID* N18LOCK3 DBCMD    changed on 1992-09-28-13.25.28 by ADMIN     *
 *** Richtige 'LOCK'.
 *** Vor diesem File muss 'N18LOCK1 VDNMAC' aufgerufen
 *** werden.
 *****************************************************!
file connect ( kern test !
lock table t1 in share mode !
lock  table  t2  in  share  mode !
lock (nowait) table t3 in share mode !
lock (  nowait  )  table  t4  in  share  mode  !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
lock table t1 in exclusive mode !
lock  table  t2  in  exclusive  mode !
lock (nowait  ) table t3 in exclusive mode !
lock  ( nowait ) table  t4  in  exclusive  mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
lock table t1 , t2 in share mode !
lock table t3, t4  ,t5 in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
lock ( nowait )  table t1 , t5 in exclusive mode !
lock ( nowait  )  table t2 , t3 , t4 in share mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
 ***********************************************************!
lock row t3 key t3_c1=1e00 in share mode !
lock row t3  key t3_c1 =+2E+00 in  share mode !
lock  row  t3 key t3_c1 = 3.0e0 in exclusive mode !
lock row t3 key t3_c1  =  0.4E+1   in exclusive mode !
lock (nowait) row t3 key t3_c1 =5 in share mode !
lock (nowait )  row t3  key t3_c1 =+0.6E01 in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
 ***********************************************************!
lock row t5 key t5_c1 = 6.0,t5_c2 =  0.3, t5_c3 ='Hans'
                                          in  share mode !
lock row t5  key t5_c1 =+7, t5_c2 = -88e0,  t5_c3='a++'
                                     in exclusive mode !
lock  (nowait )  row t5  key t5_c1=8,t5_c2=-0.0012,t5_c3='a b c d'
                                     in exclusive mode !
lock ( nowait )  row t5 key t5_c1 = + 9 , t5_c2= + .8,t5_c3=' Otto'
                                           in share mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
 ***********************************************************!
lock row t4 key t4_c1 = '00001'
     row t4 key t4_c1 = '00002' in share mode !
lock row t4 key t4_c1 = '00003'
     row t4 key t4_c1 = '00004' in exclusive mode !
lock row t4 key t4_c1 = '00005'
     row t5 key t5_c1=9,t5_c2=-12,t5_c3='1'
                                in exclusive mode !
lock row t5 key t5_c1 = 9, t5_c2=  - 12. ,t5_c3 = ' 2'
     row t4 key t4_c1 = '00006' in share  mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
 ***********************************************************!
lock (nowait) row t4 key t4_c1 = '1'  row t4 key t4_c1 = '2'
          row t4 key t4_c1 = '3'  row t4 key t4_c1 = '4'
          row t4 key t4_c1 = '5'  row t4 key t4_c1 = '6'
          row t4 key t4_c1 = '7'  row t4 key t4_c1 = '8'
          row t4 key t4_c1 = '9'  in share mode !
lock (nowait ) row t3 key t3_c1 =1   row t3 key t3_c1 = 2
          row t3 key t3_c1 =3   row t3 key t3_c1 = 4
          row t3 key t3_c1 =5   row t3 key t3_c1 = 6
          row t3 key t3_c1 =7
          row t4 key t4_c1 = '10' in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
 ***********************************************************!
lock table t1  in share mode  table t2 in exclusive mode !
lock table t4  in share mode  table t4 in exclusive mode !
lock row t3 key t3_c1=1 in share mode
     row  t3  key t3_c1  =2 in exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
 ***********************************************************!
lock    (    nowait    )       table t1
          row t5 key t5_c1 = 10, t5_c2 = 10, t5_c3 = '10'
          row t5 key t5_c1 = 11 ,t5_c2 = 11, t5_c3 = '11'
                      in share mode
          table t2, t3 row t4 key t4_c1 = '1'
          row t5 key t5_c1 =12, t5_c2 =12, t5_c3 ='12'
          row t4 key t4_c1 =  ' 2'  in  exclusive mode !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work !
pars_then_ex!
data !
lock    (    nowait    )       table t1
          row t5 key t5_c1 = :a, t5_c2 = :b, t5_c3 = :c
                                    in  exclusive mode !
n 3 10 n 3 10 c 7 '10'

pars_execute!
nodata !
select * from sysdd.locks where tablename <> 'DUAL' !
fetch !
rollback work release !
