*   *ID* N08PRIV2 DBCMD    changed on 1992-09-28-13.25.14 by ADMIN     *
 * Es wird getestet, ob die Privilegien geloescht werden, wenn der
 * priviligierte Benutzer geloescht wird !
file connect (kern test !
create table tab ( spalte fixed(5) ) !
create user p1 password w !
grant all on tab to p1 !
drop user p1 !
create user p1 password w !
commit work release !
file connect (p1 w !
file sel_table !
commit work release !
