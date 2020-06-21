*   *ID* T01SEL2  DBCMD    changed on 1992-09-28-13.28.33 by ADMIN     *
 ***************************
 ***   T 0 1 S E L 2     ***
 ***************************!
declare res cursor for 
select manager.name man_name, manager.currsal man_sal,
       emp.name emp_name, emp.currsal emp_sal
  FROM emp, emp  manager
  WHERE manager.sex = 'female'
            AND manager.name = emp.manager
            AND emp.sex = 'male'  !
FETCH res!
CLOSE res!
declare res cursor for 
select emp.name emp_name, emp.currsal emp_sal,
       manager.name man_name, manager.currsal man_sal
   FROM emp, emp  manager
   WHERE emp.manager = manager.name
             AND emp.currsal > manager.currsal  !
FETCH res!
CLOSE res!
declare res cursor for 
select emp.name,fam.status,dept.locname,fam.address
  FROM emp, fam, dept
  WHERE emp.sex = 'female'
            AND emp.name = fam.name
            AND fam.status = 'single'
            AND fam.deptno = dept.deptno  
  ORDER BY NAME!
FETCH res!
CLOSE res !
