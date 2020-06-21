*   *ID* NVL      DBCMD    changed on 1992-09-28-13.24.42 by ADMIN     *
file connect ( kern test sqlmode &1 !
declare c cursor for select nvl ( (s1+5), s2*3 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select nvl ( null,s1 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select nvl ( s1, 32 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select nvl ( s1-s2, kern.t1.s2 / t1.s3, s1+s3 )
    from t1 !
fetch c into :a !
close c !
declare c cursor for select nvl ( s1, s2 ) from t1 !
fetch c into :a !
close c !
declare c cursor for select s1, s2, s3 from t1 where nvl( s1, s2) = 4 !
fetch c into :a,:b, :c !
close c !
declare c cursor for select nvl ( s1, s2, s3 ) from t1 !
declare c cursor for select nvl ( 32, 'aaa' ) from t1 !
declare c cursor for select nvl ( (s1+5), s2*3, 3/s3 ) from t1 !
declare c cursor for select nvl ( sysdate, 32 ) from t1 !
declare c cursor for select nvl ( 32, time ) from t1 !
declare c cursor for select nvl ( time, s1 ) from t1 !
declare c cursor for select nvl ( sysdate, time ) from t1 !
declare c cursor for select nvl ( user, 32 ) from t1 !
declare c cursor for select nvl ( 32, user ) from t1 !
commit work release !
