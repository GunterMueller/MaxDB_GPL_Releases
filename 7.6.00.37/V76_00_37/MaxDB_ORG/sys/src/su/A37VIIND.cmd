*   *ID* A37VIIND DBCMD    changed on 1992-09-28-13.22.51 by ADMIN     *
file connect ( kern test !
file sel_table !
create index "maxrecord.g1field1"
	on maxrecord( g1field1 ) !
create index "maxrecord.g2field1"
	on maxrecord( g2field1 ) !
create index i1 on maxrecord (text1,text2,text3)!
create index i2 on maxrecord (text2,text3,text4)!
commit work release !
