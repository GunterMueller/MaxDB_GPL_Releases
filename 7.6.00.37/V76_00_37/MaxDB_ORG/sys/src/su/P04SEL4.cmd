*   *ID* P04SEL4  DBCMD    changed on 1992-09-28-13.27.47 by ADMIN     *
file connect ( kern test !
 *******************
 *                 *
 *  P 0 4 S E L 4  *
 *                 *
 *******************
*
 *******************************************************
 * Es werden Suchbedingungen angegeben, deren
 * Bearbeitung durch die aufgebauten Indices erleichtert
 * wird.
 ******************************************************!
create index "emp.deptno"
	on emp( deptno ) !
create index "emp.manager"
	on emp( manager ) !
create index "emp.sex"
	on emp( sex ) !
select name, deptno, manager, startsal, sex from emp
       where deptno = 40 !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno = 40 and
                sex = 'female' !
fetch !
select name, deptno, manager, startsal, sex from emp
       where     sex = 'male' and
             manager = 'George Taylor' !
fetch !
select name, deptno, manager, startsal, sex from emp
       where     sex = 'male' and
             manager = 'Fred Astaire' !
fetch !
select name, deptno, manager, startsal, sex from emp
       where     sex = 'male' and
             manager = 'Fred Astaire' and
              deptno = 70 !
fetch !
select name, deptno, manager, startsal, sex from emp
       where     sex = 'female' and
             manager = 'Fred Gerard' and
              deptno = 50 !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno in (40) !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno in (10, 40, 70) !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno in (30, 70) !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno >= 50 and
             deptno <= 70 !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno >  10 and
             deptno <= 99 !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno >= 0 and
             deptno <= 45 !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno between 1 and 45 !
fetch !
select name, deptno, manager, startsal, sex from emp
       where deptno between 35 and 36 !
fetch !
drop index "emp.deptno" !
drop index "emp.manager" !
drop index "emp.sex" !
select name, deptno from emp
       where deptno = 50 or
             deptno = 70 !
fetch !
select name, deptno from emp
       where deptno = 30 or
             deptno = 40 !
fetch !
commit work !
create index "emp.deptno"
	on emp( deptno ) !
commit work release !
connect kern identified by test isolation level  0 !
select name, deptno from emp
       where deptno = 50 or
             deptno = 70 !
fetch !
select name, deptno from emp
       where deptno = 30 or
             deptno = 40 !
fetch !
select name, deptno from emp
       where deptno = 40 and
            (sex = 'male' or sex = 'female')!
fetch !
commit work !
create index "emp.sex"
	on emp( sex ) !
commit work !
create index "emp.manager"
	on emp( manager ) !
commit work !
select name from emp
       where deptno  > 50 and
                sex <> 'male' !
fetch !
select name from emp
       where deptno  > 40 and
             deptno  < 50 and
                sex <> 'male' !
fetch !
select name from emp
       where deptno  > 80 and
             manager = 'Fred Gerard' !
fetch !
select name from emp
       where deptno between 40 and 50 and
                sex    <>   'male' !
fetch !
select name from emp
       where deptno between 40 and 50 and
                sex    in   ('male' ) !
fetch !
select name from emp
       where deptno in (40) and
                sex in ('male') !
fetch !
select name from emp
       where deptno in (40, 45) and
                sex in ('male') !
fetch !
select name from emp
       where deptno in (40) and
                sex in ('a', 'b', 'c')  !
fetch !
select name from emp
       where deptno in (40) and
                sex  = 'male' and
            manager  = 'Fred Gerard' !
fetch !
select name from emp
       where deptno = 40 and
            manager > 'Alice' and
                sex = 'female' !
fetch !
drop index "emp.deptno" !
drop index "emp.manager" !
drop index "emp.sex" !
commit work release !
