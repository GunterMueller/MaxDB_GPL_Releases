*   *ID* N33ANSI  DBCMD    changed on 1992-09-28-13.26.14 by ADMIN     *
file connect ( kern test sqlmode ansi !
create table abc
  ( a  integer  not null,
    b  integer  not null,
    c  integer ) !
declare res cursor for select a, count(*)
  from abc
  group by a !
fetch res into :a, :b !
close res !
declare res cursor for select a, count(a)
  from abc
  group by a !
fetch res into :a, :b !
close res !
declare res cursor for select count(*)
  from abc
  group by a !
fetch res into :a !
close res !
declare res cursor for select count(distinct a)
  from abc
  group by a !
fetch res into :a !
close res !
declare res cursor for select count(a)
  from abc
  group by a !
fetch res into :a !
close res !
declare res cursor for select count(*)
  from abc !
fetch res into :a !
close res !
declare res cursor for select count(distinct a)
  from abc !
fetch res into :a !
close res !
declare res cursor for select count(a)
  from abc !
fetch res into :a !
close res !
declare res cursor for select sum(a)
  from abc !
fetch res into :a !
close res !
declare res cursor for select sum(c)
  from abc !
fetch res into :a !
close res !
declare res cursor for select sum(a)
  from abc
  group by a !
fetch res into :a  !
rollback work release !
