*   *ID* C00E248  DBCMD    changed on 1992-09-28-13.23.08 by ADMIN     *
file connect ( kern test !
*=============
create table sni_nl
 (k1 char (10) key,
  f1 char (10) , f2 char (10), f3 char (10), f4 char (10))!
insert sni_nl values ('1', '1' , NULL, NULL, '0' )!
insert sni_nl values ('2', '2' , '2' , NULL, '0' )!
insert sni_nl values ('3', NULL, '3' , NULL, '3' )!
insert sni_nl values ('4', NULL, '0' , '4' , NULL)!
insert sni_nl values ('5', NULL, '0' , '5' , '0' )!
insert sni_nl values ('6', NULL, '0' , '6' , '6' )!
insert sni_nl values ('7', '7' , '0' , NULL, '0' )!
insert sni_nl values ('8', '8' , '8' , '8' , '8' )!
*==============
select * from sni_nl!
fetch!
*==============
select * from sni_nl where
 (f1 is not NULL  and
    (f2 is NULL or '1' <= f2 ))
 or
 (f3 is not NULL  and
    (f4 is NULL or '1' <= f4 ))!
*==============
fetch!
*==============
select * from sni_nl where
 (f1 is not NULL  and
    (f2 is NULL or '1' <= f2 ))
 and
 (f3 is not NULL  and
    (f4 is NULL or '1' <= f4 ))!
*==============
fetch!
*==============
select * from sni_nl where
    ((f2 is NULL or '1' <= f2 ) and
       f1 is not NULL)
 or
    ((f4 is NULL or '1' <= f4 ) and
       f3 is not NULL)!
*==============
fetch!
*==============
select * from sni_nl where
    ((f2 is NULL or '1' <= f2 ) and
       f1 is not NULL)
 and
    ((f4 is NULL or '1' <= f4 ) and
       f3 is not NULL)!
*==============
fetch!
*==============
drop table sni_nl !
commit work release !
