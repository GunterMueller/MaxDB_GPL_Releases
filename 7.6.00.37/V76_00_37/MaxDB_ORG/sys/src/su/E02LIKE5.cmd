*   *ID* E02LIKE5 DBCMD    changed on 1992-09-28-13.23.39 by ADMIN     *
file connect ( kern test !
 *** test of buffer limit !
create table t ( a char (7))!
insert t set a = 'aaa'!
pars_then_ex !
data !
select * from t where chr(a) like :a1 and chr(a) like :a2
and chr(a) like :a3 and chr(a) like :a4 !
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'

select * from t where chr(a) like :a1 and chr(a) like :a2
and chr(a) like :a3 and chr(a) like :a4
and chr(a) like :a5 and chr(a) like :a6
and chr(a) like :a7 and chr(a) like :a8 !
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'

select * from t where chr(a) like :a1 and chr(a) like :a2
and chr(a) like :a3 and chr(a) like :a4
and chr(a) like :a5 and chr(a) like :a6
and chr(a) like :a7 and chr(a) like :a8
and chr(a) like :a9 and chr(a) like :a10
and chr(a) like :a11 and chr(a) like :a12
and chr(a) like :a13 and chr(a) like :a14
and chr(a) like :a15 !
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa'

select * from t where chr(a) like :a1 and chr(a) like :a2
and chr(a) like :a3 and chr(a) like :a4
and chr(a) like :a5 and chr(a) like :a6
and chr(a) like :a7 and chr(a) like :a8
and chr(a) like :a9 and chr(a) like :a10
and chr(a) like :a11 and chr(a) like :a12
and chr(a) like :a13 and chr(a) like :a14
and chr(a) like :a15 and chr(a) like :a16 !
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'

select * from t where chr(a) like :a1 and chr(a) like :a2
and chr(a) like :a3 and chr(a) like :a4
and chr(a) like :a5 and chr(a) like :a6
and chr(a) like :a7 and chr(a) like :a8
and chr(a) like :a9 and chr(a) like :a10
and chr(a) like :a11 and chr(a) like :a12
and chr(a) like :a13 and chr(a) like :a14
and chr(a) like :a15 and chr(a) like :a16 
and chr(a) like :a17 and chr(a) like :a18
and chr(a) like :a19 and chr(a) like :a20
and chr(a) like :a21 and chr(a) like :a22
and chr(a) like :a23 and chr(a) like :a24
and chr(a) like :a25 and chr(a) like :a26
and chr(a) like :a27 and chr(a) like :a28
and chr(a) like :a29 and chr(a) like :a30 !
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' c 254 'aaa' c 254 'aaa'
c 254 'aaa' c 254 'aaa' 

nodata !
pars_execute !
drop table t !
commit release !
