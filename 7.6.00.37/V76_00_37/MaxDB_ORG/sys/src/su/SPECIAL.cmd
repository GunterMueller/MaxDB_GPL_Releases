*   *ID* SPECIAL  DBCMD    changed on 1992-09-28-13.21.52 by ADMIN     *
file connect ( kern test sqlmode &2 !
declare c cursor for select &1 ( 32, 'aaa' ) from t1 !
declare c cursor for select &1 ( user, 32 ) from t1 !
declare c cursor for select &1 ( &3, 32 ) from t1 !
declare c cursor for select &1 ( 32, time ) from t1 !
declare c cursor for select &1 ( time, s1 ) from t1 !
declare c cursor for select &1 ( &3, time ) from t1 !
declare c cursor for select &1 ( user, 32 ) from t1 !
declare c cursor for select &1 ( 32, user ) from t1 !
declare c cursor for select &1 ( 1,2,3,4,5,6) from t1 where s1 = 5 !
fetch c into :a !
close c !
declare c cursor for select &1 ( 41,52,23,334,85,96)
from t1 where s1 = 5 !
fetch c into :a !
close c !
declare c cursor for select &1 ( 'a','b','c','d','e')
from t1 where s1 = 5!
fetch c into :a !
close c !
declare c cursor for select &1 ( 'A','b','c','D','E')
from t1 where s1 = 5!
fetch c into :a !
close c !
declare c cursor for select &1 ( 'A','B','C','D','E')
from t1 where s1 = 5!
fetch c into :a !
close c !
declare c cursor for select &1
( 'Golf','Astra','190','318','Escort')
    from t1 where s1 = 5 !
fetch c into :a !
close c !
declare c cursor for select &1 ( s1, s2, s3 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select - &1 ( s1, s2, s3 ) from t1
    where &1 ( s1, s2, s3) > 0 !
fetch c into :a !
close c !
declare c cursor for select &1 ( s3,s1 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( (s1+5), s2*3, 3/s3 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1
     ( s1-s2, kern.t1.s2 / t1.s3, s1+s3 )
     from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( s1, 32, 5e30 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( s1, 32/0, s3 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( s1, 12.34, 12.345) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( 12.34, s1, 12.345) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( 12.34, 12.345, s1) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( 12.3456789012345678, s1) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( s1, 12.3456789012345678) from t1 !
fetch c into :a !
close c !
declare c cursor for select &1 ( c1, c2, c3 ) from t2 !
fetch c into :a !
close c !
declare c cursor for select &1 ( c1, c2, c3 ) from t2
    where &1 ( c1, c2, c3) > 'big' !
fetch c into :a !
close c !
declare c cursor for select &1 ( c2,c1 ) from t2 !
fetch c into :a !
close c !
commit work release !
