file connect ( hu hu sqlmode ansi !
PARS_THEN_EX !
      DECLARE CC CURSOR
            FOR SELECT EMPNAME,PNUM,HOURS
                FROM   STAFF,WORKS
                WHERE  STAFF.EMPNUM = WORKS.EMPNUM
            UNION
                SELECT EMPNAME,PNUM,HOURS
                FROM   STAFF,WORKS
                WHERE  NOT EXISTS
                      (SELECT HOURS 
                       FROM   WORKS
                       WHERE  STAFF.EMPNUM = WORKS.EMPNUM) order by 1,2,3!
FETCH cc INTO :a, :b, :c !
pars_execute !
commit work release !
