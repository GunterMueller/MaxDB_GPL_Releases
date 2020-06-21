*   *ID* P04SEL1  DBCMD    changed on 1992-09-28-13.27.45 by ADMIN     *
file connect ( kern test !
 *******************
 *                 *
 *  P 0 4 S E L 1  *
 *                 *
 *******************
*
 *******************************************************
 * es werden in diesem Testfile Selects auf einen
 * Satztyp ?uberpr?uft.
 *******************************************************
 * In der ersten Abteilung handelt es sich um einfache
 * SELECTs ohne Qualifikation und ohne Orderangabe.
 * Es werden aber unterschiedliche Ausgabe-Feldnummern-
 * Reihenfolgen verwendet.
 * Die ?Uberpr?ufung geschieht mit Keys fester und
 * variabler L?ange.
 ******************************************************!
select name from emp !
fetch !
select manager from emp!
fetch !
select currsal from emp !
fetch !
select name, name from emp !
fetch !
select name, sex from emp !
fetch !
select startsal, name from emp!
fetch !
select currsal, startsal from emp !
fetch !
select manager, deptno, manager from emp!
fetch !
select * from emp !
fetch !
select name from emp1 !
fetch !
select manager from emp1!
fetch !
select currsal from emp1 !
fetch !
select name, name from emp1 !
fetch !
select name, sex from emp1 !
fetch !
select startsal, name from emp1!
fetch !
select currsal, startsal from emp1 !
fetch !
select manager, deptno, manager from emp1!
fetch !
select * from emp1 !
fetch !
 *******************************************************
 * In der zweiten Abteilung werden Bedingungen ?uber
 * das Schl?usselfeld angegeben.
 *******************************************************
 * Gleichheits- und IN-Bedingungen ?uber den Schl?ussel
 * f?uhren zu einem Aufbau der Ergebnismenge, die
 * naturgem?a?z sehr klein ist.
 ******************************************************!
select name from emp where name = 'Norma Wilson'!
fetch !
select name from emp where name = 'Martha'!
fetch !
select name from emp1 where name = 'Norma'!
fetch !
select name from emp1 where name = 'Martha'!
fetch !
select name from emp
            where name in ('Norma Wilson',
                           'Martha',
                           'Fred Gerard')!
fetch !
select name from emp
            where name in ('Alice Teir',
                           'Martha')!
fetch !
select name from emp1
            where name in ('Norma',
                           'Martha',
                           'Fred')!
fetch !
select name from emp1
            where name in ('Alice',
                           'Martha')!
fetch !
 *******************************************************
 * Es werden Bedingungen ?uberpr?uft, die die Unter-
 * und/oder Obergrenze f?ur den Key angeben.
 * Das f?uhrt nicht zu einem Aufbau einer Ergebnismenge.
 ******************************************************!
select name from emp where name >= 'Alice Teir' !
fetch !
select name from emp where name > 'Alice Teir' !
fetch !
select name from emp where name < 'Otto' !
fetch !
select name from emp where name <= 'Norma Wilson' !
fetch !
select name from emp1 where name >= 'Alice' !
fetch !
select name from emp1 where name > 'Alice' !
fetch !
select name from emp1 where name < 'Otto' !
fetch !
select name from emp1 where name <= 'Norma' !
fetch !
select name from emp where name > 'Alice Teir'
                       and name < 'Martha' !
fetch !
select name from emp
              where name >= 'Alice Teir'
                and name <= 'Norma Wilson'!
fetch !
select name from emp1 where name > 'Alice'
                        and name < 'Martha' !
fetch !
select name from emp1
              where name >= 'Alice'
                and name <= 'Norma'!
fetch !
select name from emp
            where name between 'Alice Teir'
                           and 'Norma Wilson'!
fetch !
select name from emp
            where name between 'Bertha'
                           and 'John' !
fetch !
select name from emp1
            where name between 'Alice'
                           and 'Norma'!
fetch !
select name from emp1
            where name between 'Bertha'
                           and 'John' !
fetch !
 *******************************************************
 * Es werden Bedingungen angegeben, die ein
 * sequentielles Suchen von zerokey bis fullkey
 * hervorrufen. Die Suche erfolgt aber erst bei FETCH.
 ******************************************************!
select name from emp
            where name not in ('Fred Gerard')!
fetch !
select name from emp
            where name not in ('Martha',
                               'Fred Gerard')!
fetch !
select name from emp1
            where name not in ('Fred')!
fetch !
select name from emp1
            where name not in ('Martha',
                               'Fred')!
fetch !
select name from emp
            where name not between 'Fred Gerard'
                               and 'Norma Wilson'!
fetch !
select name from emp1
            where name not between 'Fred'
                               and 'Norma'!
fetch !
commit work release !
