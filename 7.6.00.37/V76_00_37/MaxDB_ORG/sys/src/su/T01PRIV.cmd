*   *ID* T01PRIV  DBCMD    changed on 1992-09-28-13.28.31 by ADMIN     *
 ***************************
 ***   T 0 1 P R I V     ***
 ***************************!
file sel_tab_owner ( KERN !
file sel_view ( DIVORCED !
file sel_view ( MARRIED !
file sel_view ( SINGLE !
file sel_view ( RICH_EMP !
CREATE USER personal PASSWORD geheim RESOURCE !
GRANT INSERT, SELECT ON divorced TO personal  !
GRANT SELECT (name, manager), UPDATE (name,manager) ON emp TO personal !
COMMIT WORK RELEASE !
file connect ( personal geheim !
file sel_tab_owner ( KERN !
LOCK TABLE kern.emp IN EXCLUSIVE MODE  !
UPDATE OF kern.emp
  SET currsal = 20000
  KEY name = 'Pat Mingo'  !
ROLLBACK WORK !
SELECT Name, status, depend, address
  FROM kern.divorced !
FETCH !
CLOSE !
commit work !
DROP USER personal !
commit work !
