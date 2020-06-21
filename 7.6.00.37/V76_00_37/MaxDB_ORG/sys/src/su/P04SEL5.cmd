*   *ID* P04SEL5  DBCMD    changed on 1992-09-28-13.27.47 by ADMIN     *
file connect ( kern test !
select name, deptno from emp!
fetch !
declare erg1 cursor for
select name, deptno from emp order by deptno !
fetch erg1 !
declare erg2 cursor for
select name, deptno from erg1
           where deptno >= 50 !
fetch erg2!
close erg1 !
close erg2 !
declare erg1 cursor for
select manager, name, currsal, deptno from emp for reuse!
fetch erg1 !
declare erg1a cursor for
select * from erg1!
fetch erg1a !
close erg1a !
declare erg2 cursor for
select deptno, name, manager from erg1
           where deptno >= 50 for reuse!
fetch erg2!
declare erg2a cursor for
select * from erg2!
fetch erg2a !
close erg2a !
declare erg3 cursor for
select deptno, name, manager from erg1
     order by deptno!
fetch erg3!
close erg3 !
declare erg4 cursor for
select name, manager, deptno from erg1 order by deptno!
fetch erg4!
declare erg5 cursor for
select name from erg4 !
fetch erg5!
close erg4 !
close erg5 !
declare erg10 cursor for
select manager, name, deptno, currsal from emp
where deptno = 50 and  currsal > 10000 for reuse!
fetch erg10 !
declare erg11 cursor for
select name, currsal, deptno from erg10!
fetch erg11!
declare erg20 cursor for
select manager, name, deptno, currsal from emp
where deptno >= 50 and (currsal > 14000 or name < 'Pat') for reuse!
fetch erg20!
declare erg21 cursor for
select name, currsal, deptno from erg20!
fetch erg21!
declare erg30 cursor for
select manager, name, deptno, currsal from emp
where deptno = 50 or currsal > 14000 for reuse!
fetch erg30 !
declare erg31 cursor for
select name, currsal, deptno from erg30 !
fetch erg31!
declare erg41 cursor for
select manager, name, deptno, currsal from erg1
where deptno = 50 and  currsal > 14000!
fetch erg41!
declare erg42 cursor for
select manager, name, deptno, currsal from erg1
where deptno >= 50 and (currsal > 14000 or name < 'Pat') !
fetch erg42!
declare erg43 cursor for
select manager, name, deptno, currsal from erg1
where deptno = 50 or currsal > 14000!
fetch erg43!
declare erg1041 cursor for
select manager, name, deptno, currsal from erg10
where deptno = 50 and  currsal > 14000!
fetch erg1041!
declare erg1042 cursor for
select manager, name, deptno, currsal from erg10
where deptno >= 50 and deptno < 70 and
(currsal > 14000 or name < 'Pat') !
fetch erg1042!
declare erg1043 cursor for
select manager, name, deptno, currsal from erg10
where deptno = 50 or currsal > 14000 or name < 'Martin'!
fetch erg1043!
close erg10 !
close erg1041 !
close erg1042 !
close erg1043 !
declare erg2041 cursor for
select manager, name, deptno, currsal from erg20
where deptno = 50 and  currsal > 14000!
fetch erg2041!
declare erg2042 cursor for
select manager, name, deptno, currsal from erg20
where deptno >= 50 and (currsal > 14000 or name < 'Pat') !
fetch erg2042!
declare erg2043 cursor for
select manager, name, deptno, currsal from erg20
where deptno = 50 or currsal > 14000!
fetch erg2043!
close erg20 !
close erg2041 !
close erg2042 !
close erg2043 !
declare erg3041 cursor for
select manager, name, deptno, currsal from erg30
where deptno = 50 and  currsal > 14000!
fetch erg3041!
declare erg3042 cursor for
select manager, name, deptno, currsal from erg30
where deptno >= 50 and (currsal > 14000 or name < 'Pat') !
fetch erg3042!
declare erg3043 cursor for
select manager, name, deptno, currsal from erg30
where deptno = 50 or currsal > 14000!
fetch erg3043!
close erg30 !
close erg3041 !
close erg3042 !
close erg3043 !
commit work release !
