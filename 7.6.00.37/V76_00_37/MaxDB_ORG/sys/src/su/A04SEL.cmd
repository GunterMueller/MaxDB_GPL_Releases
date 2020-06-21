*   *ID* A04SEL   DBCMD    changed on 1992-09-28-13.22.24 by ADMIN     *
 ***************************
 ***   A 0 4 S E L       ***
 ***************************!
connect kern IDENTIFIED BY :A !
TEST
select distinct k1,f1,f2,n1,n2  into  :v1,:v2 ,:v3,:v4,:v5
     from kern.p1 where   n1 = 1000  !
select distinct k1,f1,f2,n1,n2
     from kern.p1 where   n1 = 1000
     order by k1 !
fetch !
select    k1,f1,f2,n1,n2  into :v1, :v2, :v3,:v4,:v5
     from kern.p1 where  k1 = 'aaa'   !
declare erg cursor for select k1,f1,f2,n1,n2 
     from kern.p1 where  n1 = 1000 and n2 = 33.2 !
fetch erg into :v1, :v2, :v3, :v4, :v5 !
declare erg1 cursor for 
select k1,f1,f2,n1,n2
     from kern.p1  !
fetch erg1!
select      k1,f1,f2,n1,n2  into :v1 ,:v2 ,:v3,:v4,:v5
     from kern.p1 order by f1   !
select      k1,f1,f2,n1,n2
     from kern.p1 order by f1   !
fetch  into :v1, :v2, :v3, :v4, :v5 !
declare erg3 cursor for 
select k1,f1,f2,n1,n2 into  :v1,:v2 ,:v3,:v4,:v5
     from kern.p1 where   k1 = 'aaa' !
select  (k1,f1,f2,n1,n2) into  :v1,:v2 ,:v3,:v4,:v5
     from kern.p1 where    k1 = 'aaa' and    f1 > :'satz2' !
pars_then_ex !
data !
select       k1,f1,f2,n1,n2  into  :v1,:v2 ,:v3,:v4,:v5
     from kern.p1 where    k1 = :v6  and    n2 <= :v7 !
c 3 'aaa'  n 4,2  11.6

select       k1,f1,f2,n1,n2  into  :v1,:v2 ,:v3,:v4,:v5
     from kern.p1 where    k1 = :v6  and    f1 > :v7 !
c 3 'aaa'  c 5  'satz3'

select       k1,f1,f2,n1,n2  into  :v1,:v2 ,:v3,:v4,:v5
     from kern.p1 where    k1 = :v6  and    f1 <= :v7 !
c 3 'aaa'  c 5  'satz3'

nodata !
pars_execute !
close       erg1 !
declare erg1 cursor for 
select k1, f1, f2, n1, n2 from kern.p1 !
fetch erg1 !
commit work release !
