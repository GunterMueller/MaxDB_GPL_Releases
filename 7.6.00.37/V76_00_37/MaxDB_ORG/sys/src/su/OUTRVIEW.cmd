*   *ID* OUTRVIEW DBCMD    changed on 1992-09-28-13.27.39 by ADMIN     *
create view v as select ta.*, tb.* from <ta, tb> where a1    =    b1 !
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
create view v as select ta.*, tb.* from <tb, ta> where a1    =    b1 !
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
create view v as select ta.*, tb.* from ta, tb !
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
create view v as select ta.*, tb.* from <ta, tb> where a1 (+)=b1 (+)!
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
create view v as select ta.*, tb.* from <tb, ta> where a1 (+)=b1 (+)!
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
create view v as select ta.*, tb.* from <ta, tb> where a1 (+)=    b1 !
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
create view v as select ta.*, tb.* from <tb, ta> where a1 (+)=    b1 !
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
create view v as select ta.*, tb.* from <ta, tb> where a1    =b1 (+)!
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
create view v as select ta.*, tb.* from <tb, ta> where a1    =b1 (+)!
explain select * from v !
select * from v !
fetch !
drop view v !
commit work !
