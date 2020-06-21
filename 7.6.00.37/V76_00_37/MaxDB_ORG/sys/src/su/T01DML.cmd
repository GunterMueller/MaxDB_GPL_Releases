*   *ID* T01DML   DBCMD    changed on 1992-09-28-13.28.28 by ADMIN     *
 ***************************
 ***   T 0 1 D M L       ***
 ***************************!
LOCK TABLE fam ROW emp KEY  name = 'Jerry Denny'  IN EXCLUSIVE MODE!
UPDATE OF emp
  SET manager = 'Jack Black',
      currsal = 15000
  KEY name = 'Jerry Denny' !
DELETE FROM fam KEY name = 'Paul Brown' !
commit work !
LOCK TABLE emp IN EXCLUSIVE MODE !
DELETE FROM emp KEY name = 'Fred Gerard'  !
DELETE FROM emp KEY name = 'Norma Wilson' !
DELETE FROM emp KEY name = 'Paul Brown'   !
ROLLBACK WORK !
LOCK TABLE fam ROW emp KEY  name = 'Jerry Denny'
       IN EXCLUSIVE MODE !
UPDATE OF emp
  SET manager = 'Paul Brown',
      currsal = 14000
  KEY name = 'Jerry Denny' !
INSERT INTO fam
  SET name = 'Paul Brown',
      deptno = 70,
      status = 'married',
      depend = 4,
      address = '271 Oxnard St., Lodi' !
commit work !
