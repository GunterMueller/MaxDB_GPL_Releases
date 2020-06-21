*   *ID* DECODE   DBCMD    changed on 1992-09-28-13.23.19 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select decode ( 32) from t1 !
declare c cursor for select decode ( 32,3) from t1 !
declare c cursor for select decode ( 32,3,4) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( NULL, 3, 4) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32, NULL, 4) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32, 3, NULL) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32, 3,4,32,NULL) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32,'3',4) from t1 !
declare c cursor for select decode ( 32,3,4,1) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32,3,4,32,2) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32,3,4,1,2,3) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32,32,'4') from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32,3,'4',1) from t1 !
declare c cursor for select decode ( 32,3,'4','1') from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32,3,'4',1,2) from t1 !
declare c cursor for select decode ( 32,3,'4',1,'2') from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32,3,'4',1,'2',3) from t1 !
declare c cursor for select decode ( 1,3,'4',1,'2','3') from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( 32, 'aaa',3 ) from t1 !
declare c cursor for select decode ( user, 32 ,5) from t1 !
declare c cursor for select decode ( &2, 32,6 ) from t1 !
declare c cursor for select decode ( 32, time,6 ) from t1 !
declare c cursor for select decode ( time, s1 ,6) from t1 !
declare c cursor for select decode ( &2, time,2 ) from t1 !
declare c cursor for select decode ( user, 32 ,'as') from t1 !
declare c cursor for select decode ( 32, user ) from t1 !
declare c cursor for select decode ( s1, s2, s3 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( s1, s2, s3 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( s3,s1 ) from t1 !
declare c cursor for select decode ( (s1+5), s2*3, 3/s3 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( s1-s2, kern.t1.s2 / t1.s3, s1+s3 )
     from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( s1, 32, 5e30 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( s1, 32/0, s3 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( s3, NULL,90, s3) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( s3, NULL,90, -9) from t1 !
fetch c into :a !
close c !
declare c cursor for select decode ( c1, c2, c3 ) from t2 !
fetch c into :a !
close c !
declare c cursor for select decode ( c2,c1 ) from t2 !
fetch c into :a !
close c !
declare c cursor for select decode ( 1,h1,h2,'invalid') from t3 !
fetch c into :a !
close c !
declare c cursor for select decode ( 321,321,h2) from t3 !
fetch c into :a !
close c !
declare c cursor for select decode ( '32','32',h1) from t3 !
fetch c into :a !
close c !
declare c cursor for select decode ( '32','32',h1),
       decode ('as','as',h2)
from t3 !
fetch c into :a,:b !
close c !
declare c cursor for select decode ( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
from t3 where h1 = 1 !
fetch c into :a !
close c !
declare c cursor for select decode (  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                11, 12, 13, 14, 15, 16, 17, 18, 19, 20
) from t3 !
fetch c into :a !
close c !
declare c cursor for select decode (  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                21, 22, 23, 24, 25, 26, 27, 28, 29, 30
) from t3 !
fetch c into :a !
close c !
declare c cursor for select decode (  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                31, 32, 33, 34, 35, 36, 37, 38, 39, 40
) from t3 !
fetch c into :a !
close c !
declare c cursor for select decode (  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                41, 42, 43, 44, 45, 46, 47, 48, 49, 50
) from t3 !
fetch c into :a !
close c !
CREATE TABLE DATTEST ( A CHAR (8))!
INSERT into DATTEST VALUES ( 'aaa' )!
INSERT into DATTEST VALUES ( 'bbb' )!
declare c cursor for SELECT A, DECODE (A, 'aaa', 'ja', A) FROM DATTEST !
FETCH C into :a, :b !
declare c cursor for SELECT A, DECODE (A, 'aaa', 'ja', KERN.DATTEST.A) 
           FROM DATTEST !
FETCH C into :a, :b !
pars_then_ex !
data !
select decode (a, 'aaa' , :a, :b) from dattest !
c 254 'x23456789012345678901234567890'
c 254 'y234567890y234567890y234567890'

nodata !
fetch into :a !
parsing!
nodata!
parameter !
select decode (a, 'aaa' , :a, 3) from dattest !
n 8 

executing !
data !
select !
1
n 8 6

pars_then_ex !
nodata !
no_parameter!
fetch into :a!
pars_execute!
drop table dattest !
commit work release !
