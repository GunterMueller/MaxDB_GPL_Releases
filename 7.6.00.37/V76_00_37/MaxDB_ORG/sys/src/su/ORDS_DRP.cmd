*   *ID* ORDS-DRP CMD      changed on 1994-02-09-11.29.57 by KRISCHAN  *

file CONNECT ( kern test SQLMODE ORACLE!

DROP VIEW     bauk!
DROP SYNONYM  teil!
DROP TABLE    st_liste!
DROP TABLE    artikel!
DROP SEQUENCE teilenummer!

COMMIT WORK RELEASE!

file CONNECT ( kern2 test2 SQLMODE ORACLE!
DROP TABLE t2!
COMMIT WORK RELEASE!

file CONNECT ( sut sut!
DROP USER kern2!
COMMIT WORK RELEASE!

