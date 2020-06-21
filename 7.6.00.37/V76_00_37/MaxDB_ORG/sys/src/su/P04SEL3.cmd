*   *ID* P04SEL3  DBCMD    changed on 1992-09-28-13.27.46 by ADMIN     *
file connect ( kern test !
 *******************
 *                 *
 *  P 0 4 S E L 3  *
 *                 *
 *******************
*
 *******************************************************
 * In der ersten Abteilung werden Orderbedingungen
 * angegeben. Bei einer Orderbedingung ?uber das einzige
 * Schl?usselfeld erfolgt die Suche erst bei FETCH.
 * Ansonsten wird eine Ergebnismenge aufgebaut.
 ******************************************************!
select name, currsal, manager, startsal from emp
       order by name asc !
fetch !
select name, currsal, manager, startsal from emp
       order by name desc!
fetch !
select name, currsal, manager, startsal from emp
       order by manager asc !
fetch !
select name, currsal, manager, startsal from emp
       order by manager desc !
fetch !
select name, currsal, manager, startsal from emp
       order by startsal asc !
fetch !
select name, currsal, manager, startsal from emp
       order by startsal desc !
fetch !
 *******************************************************
 * Die Orderbedingung wird f?ur ein Feld angegeben,
 * das nicht Ausgabefeld ist.
 ******************************************************!
select name, currsal, manager, startsal from emp
       order by sex asc !
fetch !
select name, currsal, manager, startsal from emp
       order by deptno desc !
fetch !
select name, deptno, currsal, startsal from emp
       order by manager asc !
fetch !
select name, deptno, currsal, startsal from emp
       order by manager desc!
fetch !
 *******************************************************
 * Es werden mehrere Orderbedingungen angegeben
 ******************************************************!
select name, currsal, manager, startsal from emp
       order by manager asc, startsal desc !
fetch !
select name, currsal, manager, startsal from emp
       order by currsal asc, name desc !
fetch !
select name, currsal, manager, startsal from emp
       order by startsal desc, name asc !
fetch !
 *******************************************************
 * Eine Orderbedingung wird ?uber ein Nicht-Ausgabe-
 * Feld angegeben.
 ******************************************************!
select name, currsal, manager, startsal from emp
       order by startsal desc, sex  asc !
fetch !
select name, currsal, manager, startsal from emp
       order by sex desc, name asc !
fetch !
 *******************************************************
 * In der zweiten Abteilung wird ?uberpr?uft, ob die
 * DISTINCT-Angabe die erwartete Wirkung hat.
 ******************************************************!
select manager from emp !
fetch !
select distinct manager from emp order by manager !
fetch !
select distinct deptno, manager from emp order by deptno, manager !
fetch !
select distinct manager, startsal from emp order by manager, startsal !
fetch !
select manager from emp order by manager !
fetch !
 *******************************************************
 * Es wird ?uberpr?uft, ob DISTINCT zusammen mit
 * Orderbedingungen ?uber Ausgabe- oder Nicht-Ausgabe-
 * Felder funktioniert.
 ******************************************************!
select distinct manager from emp order by manager !
fetch !
select distinct manager, deptno from emp order by deptno asc, manager !
fetch !
select manager, deptno from emp order by deptno desc !
fetch !
select distinct manager from emp order by deptno desc !
fetch !
commit work release !
