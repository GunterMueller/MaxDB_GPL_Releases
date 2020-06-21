*   *ID* P04SEL2  DBCMD    changed on 1992-09-28-13.27.45 by ADMIN     *
file connect ( kern test !
 *******************
 *                 *
 *  P 0 4 S E L 2  *
 *                 *
 *******************
*
 *******************************************************
 * Es werden Bedingungen ?uber die Nicht-Key-Felder
 * angegeben.
 ******************************************************!
select name, deptno, startsal, sex from emp
       where deptno = 40 !
fetch !
select name, deptno, startsal, sex from emp
       where deptno = 40 and
                sex = 'female' !
fetch !
select name, deptno, manager, sex from emp
       where     sex = 'male' and
             manager = 'George Taylor' !
fetch !
select name, manager, startsal, sex from emp
       where     sex = 'male' and
             manager = 'Fred Astaire' !
fetch !
select name, deptno, startsal, sex from emp
       where     sex = 'male' and
             manager = 'Fred Astaire' and
              deptno = 70 !
fetch !
select name, deptno, currsal, sex from emp
       where     sex = 'female' and
             manager = 'Fred Gerard' and
              deptno = 50 !
fetch !
select name, deptno, startsal, sex from emp
       where deptno in (40) !
fetch !
select name, deptno, currsal, sex from emp
       where deptno in (10, 40, 70) !
fetch !
select name, currsal, manager, sex from emp
       where deptno in (30, 70) !
fetch !
select name, deptno, startsal, sex from emp
       where deptno >= 50 and
             deptno <= 70 !
fetch !
select currsal, manager, startsal, sex from emp
       where deptno >  10 and
             deptno <= 99 !
fetch !
select name, deptno, currsal, sex from emp
       where deptno >= 0 and
             deptno <= 45 !
fetch !
select currsal, manager, startsal, sex from emp
       where deptno between 1 and 45 !
fetch !
select name, deptno, startsal, sex from emp
       where deptno between 35 and 36 !
fetch !
commit work release !
