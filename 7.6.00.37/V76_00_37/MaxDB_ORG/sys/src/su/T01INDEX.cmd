*   *ID* T01INDEX DBCMD    changed on 1992-09-28-13.28.30 by ADMIN     *
 ***************************
 ***   T 0 1 I N D E X   ***
 ***************************!
CREATE INDEX "emp.manager"
	on emp( manager ) !
CREATE INDEX "fam.status"
	on fam( status ) !
CREATE INDEX "dept.locname"
	on dept( locname ) !
commit work !
