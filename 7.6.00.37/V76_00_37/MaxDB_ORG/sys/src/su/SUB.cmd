*   *ID* SUB      DBCMD    changed on 1992-09-28-13.28.12 by ADMIN     *
file connect ( kern test !
create view v1 as select emp.name, emp.deptno, fam.address 
                     from emp, fam !
* join view in correlation column
* two buffers for the results of first select !
select * from v1
    where deptno > 65 and
    exists ( select deptno from dept where v1.deptno = deptno) !
fetch !
* join view using more than one table in one expression !
create view v2 (mul) as select emp.deptno * fam.deptno
      from emp, fam !
select * from v2 where exists
      (select locname from dept where v2.mul > deptno)!
fetch!
* complicated select, (bool1 has to be false, no new_stpos ) !
select * from emp, fam, dept
   where emp.deptno * fam.deptno > dept.deptno and
         emp.name = fam.name and
         emp.deptno = any ( select deptno from dept
                               where emp.name > dept.locname )!
fetch !
commit release !
