*   *ID* SUBVIEW1 DBCMD    changed on 1992-09-28-13.28.14 by ADMIN     *
file connect ( kern test !
create view v1 as select emp.name, emp.deptno, 
                         fam.address from emp, fam !
* join view in correlation column
* two buffers for the results of first select !
create view v2 as select * from v1
    where deptno > 65 and
    exists ( select deptno from dept where v1.deptno = deptno) !
select * from v2 !
fetch !
* join view using more than one table in one expression !
create view v3 (mul) as select emp.deptno * fam.deptno
      from emp, fam !
create view v4 as select * from v3 where exists
      (select locname from dept where v3.mul > deptno)!
select * from v4 !
fetch!
* complicated select, (bool1 has to be false, no new_stpos ) !
create view v5 (a,b,c,d,e,f,g,h,i,j,k,l,m,n)
               as select * from emp, fam, dept
   where emp.deptno * fam.deptno > dept.deptno and
         emp.name = fam.name and
         emp.deptno = any ( select deptno from dept
                               where emp.name > dept.locname )!
select * from v5 !
fetch !
commit release !
