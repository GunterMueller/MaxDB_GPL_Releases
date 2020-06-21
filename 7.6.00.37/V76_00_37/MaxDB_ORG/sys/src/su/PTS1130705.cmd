connect kern test !

create table j ( i int key )!

pars_then_ex !

select count(*) into :a 
from j t1, j t2, j t3
where t1.i = 1 and 
      t2.i = 1 and 
      t3.i = 1 ! 
      
select count(*) into :a 
from j t1, j t2, j t3
where t1.i = 1 and 
      t2.i = 1 and 
      t3.i = 1 ! 
      
select count( i ) into :a 
from j t1 
where t1.i = 1 or t1.i = 2!

pars_execute !

commit work release !

