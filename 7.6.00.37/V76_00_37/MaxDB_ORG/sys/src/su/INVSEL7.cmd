*   *ID* INVSEL7  DBCMD    changed on 1992-09-28-13.23.56 by ADMIN     *
declare erg cursor for select distinct uk1,uk2,uk3  from u &1!
fetch erg!
declare erg cursor for select distinct u1  from u &1!
fetch erg!
declare erg cursor for select distinct u2  from u &1!
fetch erg!
declare erg cursor for select distinct u3  from u &1!
fetch erg!
declare erg cursor for select distinct u4  from u &1!
fetch erg!
declare erg cursor for select distinct u5  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1  from u &1!
fetch erg!
declare erg cursor for select distinct u3,u4  from u &1!
fetch erg!
declare erg cursor for select distinct u4,u3  from u &1!
fetch erg!
declare erg cursor for select distinct u3,u5  from u &1!
fetch erg!
declare erg cursor for select distinct u5,u3  from u &1!
fetch erg!
declare erg cursor for select distinct u4,u5  from u &1!
fetch erg!
declare erg cursor for select distinct u5,u4  from u &1!
fetch erg!
declare erg cursor for select distinct u3,u4,u5  from u &1!
fetch erg!
declare erg cursor for select distinct u5,u4,u3  from u &1!
fetch erg!
declare erg cursor for select distinct u5,u3,u4  from u &1!
fetch erg!
declare erg cursor for select distinct u4,u5,u3  from u &1!
fetch erg!
declare erg cursor for select distinct u1,uk1  from u &1!
fetch erg!
declare erg cursor for select distinct u1,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct u2,uk1  from u &1!
fetch erg!
declare erg cursor for select distinct u2,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk1  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1,uk1  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk1,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1,uk1,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk2,uk1  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1,uk2,uk1  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk1,uk2,uk3  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1,uk1,uk2,uk3  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk1,uk3,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1,uk1,uk3,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk3,uk2,uk1  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1,uk3,uk2,uk1  from u &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk2,uk1,uk3  from u &1!
fetch erg!
declare erg cursor for select distinct u2,u1,uk2,uk1,uk3  from u &1!
fetch erg!
declare erg cursor for select distinct uk2,uk1,u3,u4,u5  from u &1!
fetch erg!
declare erg cursor for select distinct uk2,uk1,u4,u5  from u &1!
fetch erg!
declare erg cursor for select distinct uk2,u5,uk1,u3,uk3,u4  from u &1!
fetch erg!
declare erg cursor for select distinct uk2,uk1,u3,uk3,u4  from u &1!
fetch erg!
declare erg cursor for select distinct uk2,u5,u3,uk3,u4  from u &1!
fetch erg!
commit!
declare erg cursor for select distinct u1  from u order by u1 &1!
fetch erg!
declare erg cursor for select distinct u2  from u order by u2 &1!
fetch erg!
declare erg cursor for select distinct u3  from u order by u3 &1!
fetch erg!
declare erg cursor for select distinct u4  from u order by u4 &1!
fetch erg!
declare erg cursor for select distinct u5  from u order by u5 &1!
fetch erg!
declare erg cursor for select distinct u5  from u order by u5 desc &1!
fetch erg!
declare erg cursor for 
select distinct u1,u2  from u order by u1,u2 &1!
fetch erg!
declare erg cursor for 
select distinct u2,u1  from u order by u1,u2 &1!
fetch erg!
declare erg cursor for 
select distinct u1,u2  from u order by u2,u1 &1!
fetch erg!
declare erg cursor for 
select distinct u2,u1  from u order by u2,u1 &1!
fetch erg!
declare erg cursor for 
select distinct u1,u2  from u order by u1 desc,u2 &1!
fetch erg!
declare erg cursor for 
select distinct u2,u1  from u order by u1,u2 desc &1!
fetch erg!
declare erg cursor for 
select distinct u1,u2  from u where u1 = 'Z' &1!
fetch erg!
declare erg cursor for 
select distinct u1,u2  from u where u1 like 'Z' &1!
fetch erg!
declare erg cursor for select distinct u1,u2  from u 
where u2 in (9,7,5,3,1,0) &1!
fetch erg!
declare erg cursor for select distinct u1,u2  from u 
where u2 between 0 and 9 &1!
fetch erg!
declare erg cursor for 
select distinct u5  from u where u5 is null &1!
fetch erg!
declare erg cursor for 
select distinct u5  from u where u5 is not null &1!
fetch erg!
declare erg cursor for select distinct u1,u2  from u where u1 = 'Z' 
order by u1 &1!
fetch erg!
declare erg cursor for select distinct u1,u2  from u where u1 like 'Z' 
order by u1 &1!
fetch erg!
declare erg cursor for select distinct u1,u2  from u 
where u2 in (9,7,5,3,1,0) order by u1,u2 &1!
fetch erg!
declare erg cursor for select distinct u1,u2  from u 
where u2 between 0 and 9 order by u1,u2 &1!
fetch erg!
declare erg cursor for select distinct u5  from u where u5 is null 
order by u5 &1!
fetch erg!
declare erg cursor for select distinct u5  from u where u5 is not null 
order by u5 &1!
fetch erg!
commit!
declare erg cursor for select distinct u1,uk1  from u order by u1 &1!
fetch erg!
declare erg cursor for select distinct u1,uk2  from u 
where u1 in ('Z', 'A') AND uk2 > 0 &1!
fetch erg!
declare erg cursor for select distinct u1,uk3  from u 
where u1 = 'A' AND uk3 = 'A' &1!
fetch erg!
declare erg cursor for 
select distinct u1,uk1  from u order by uk1 &1!
fetch erg!
declare erg cursor for select distinct u1,uk2  from u order by u2 &1!
fetch erg!
declare erg cursor for select distinct u1,uk3  from u 
where u2 < 100 &1!
fetch erg!
declare erg cursor for select distinct u5,uk1  from u 
where u5 < 1000 &1!
fetch erg!
declare erg cursor for select distinct u5,uk2  from u 
where u5 < 100 &1!
fetch erg!
declare erg cursor for select distinct u5,uk3  from u 
where uk3 < 'C' &1!
fetch erg!
declare erg cursor for select distinct uk1,u1  from u 
where uk2 > 0 &1!
fetch erg!
declare erg cursor for select distinct uk3,u1  from u 
where uk1 between 'A' AND 'Z' &1!
fetch erg!
declare erg cursor for select distinct uk1,u5  from u 
where u5 is null order by u5 &1!
fetch erg!
declare erg cursor for select distinct uk2,u5  from u 
where u5 is not null order by u5 &1!
fetch erg!
commit!
declare erg cursor for select distinct substr(u1,1),u2,uk1,uk2  
from u &1!
fetch erg!
declare erg cursor for select distinct uk2,substr(uk1,1),u1,u2  
from u &1!
fetch erg!
declare erg cursor for select distinct u1,uk1,uk3  from u 
where uk2 = 9 &1!
fetch erg!
declare erg cursor for select distinct uk3,uk1,u1  from u 
where uk2 = 0 &1!
fetch erg!
declare erg cursor for select distinct u1,uk3,uk2  from u 
order by u1 &1!
fetch erg!
declare erg cursor for select distinct uk2,uk3,u1  from u 
order by u1,uk2 &1!
fetch erg!
declare erg cursor for select distinct uk2,uk1,u2*10,u1  from u 
where u2 between 0 AND 9 &1!
fetch erg!
declare erg cursor for select distinct u2/10,uk3,uk1,u1  from u 
where u2 >= 0 &1!
fetch erg!
declare erg cursor for select distinct uk1&uk3,u2,u1  from u 
where u2 <= 9 &1!
fetch erg!
declare erg cursor for select distinct u1,u2,uk3,chr(uk2)  from u 
where u2 = 0 &1!
fetch erg!
declare erg cursor for select distinct u1,uk2,uk3,chr(u2)  from u 
where u2 = 9 &1!
fetch erg!
declare erg cursor for select distinct u5,u4+uk2,uk1,u3  from u &1!
fetch erg!
declare erg cursor for select distinct u5,uk1,uk2-u4  from u &1!
fetch erg!
declare erg cursor for select distinct uk3&u1&uk1,uk2  from u &1!
fetch erg!
declare erg cursor for select distinct chr(uk2)&uk1&u1&uk3&u2  
from u &1!
fetch erg!
declare erg cursor for select distinct uk2,u1,uk1,uk3  from u 
where u1 = uk1 &1!
fetch erg!
declare erg cursor for select distinct u2,uk3,uk1,u1,uk2  from u 
where u1 < uk1 &1!
fetch erg!
declare erg cursor for select distinct u1,uk3,uk2,uk1  from u 
where uk1 = uk3 &1!
fetch erg!
declare erg cursor for select distinct uk1,uk2,uk3,u1  from u 
where u1 not in ('Z', 'M', 'A') &1!
fetch erg!
declare erg cursor for select distinct u5,u4*u4,uk1,uk2,uk3  
from u &1!
fetch erg!
declare erg cursor for select distinct u5,u3,uk2/uk2,uk1,u4,uk3  
from u &1!
fetch erg!
declare erg cursor for select distinct uk2*u4,uk3,uk1,u3,u5  
from u &1!
fetch erg!
declare erg cursor for select distinct u5,uk1,uk3,trunc(u4,0) div uk2  
from u &1!
fetch erg!
declare erg cursor for select distinct u5,uk1,trunc(u4,0) mod uk2,uk3  
from u &1!
fetch erg!
declare erg cursor for select distinct uk2,uk3,u4*1,uk1,u3,u5  
from u &1!
fetch erg!
declare erg cursor for select distinct u5,uk3,uk2,uk1  from u 
where u5 is null &1!
fetch erg!
declare erg cursor for select distinct uk2,uk3,u5,uk1  from u 
where u5 is not null &1!
fetch erg!
