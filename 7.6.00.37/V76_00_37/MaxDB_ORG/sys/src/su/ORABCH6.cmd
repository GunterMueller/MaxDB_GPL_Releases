
* Stephan Scholz, 7.1. 94 ! 
 * PAGE 143 !
select name, above, below, empty from math !
fetch into :a,:b,:c,:d !

 * PAGE 144 ! 
select name, above, below, empty,
       above + below "PLUS",
       above - below "MINUS",
       above * below "TIMES",
       above / below "Divided"
from math
where name = 'HIGH DECIMAL' !
fetch into :a, :b, :c, :d, :e, :f, :g, :h !

select name, above, below, empty,
       above + empty "PLUS",
       above - empty "MINUS",
       above * empty "TIMES",
       above / empty "DIVIDED"
from math 
where name = 'HIGH DECIMAL' !
fetch into :a, :b, :c, :d, :e, :f, :g, :h !

 * PAGE 145 !
select client, weight from shipping !
fetch into :a, :b !

select client, nvl (weight, 43) from shipping !
fetch into :a, :b !

 * PAGE 150 !
select name, above, below,
       round(above, 2),
       round(below, 2),
       trunc(above, 2),
       trunc(below, 2)
from math!
fetch into :a, :b, :c, :d, :e, :f, :g !

       
select name, above, below,
       round(above, 0),
       round(below, 0),
       trunc(above, 0),
       trunc(below, 0)
from math!
fetch into :a, :b, :c, :d, :e, :f, :g !

 * PAGE 151 !
select name, above, below,
       round(above,-1),
       round(below,-1),
       trunc(above,-1),
       trunc(below,-1)
from math!
fetch into :a, :b, :c, :d, :e, :f, :g !

select * from comfort !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i !

select avg(noon), count(noon), max(noon), min(noon), sum(noon)
from comfort
where city = 'SAN FRANCISCO' !
fetch into :a, :b, :c, :d, :e !

 * PAGE 156 !
select city, day, noon-midnight
from comfort
where city = 'KEENE' !
fetch into :a, :b, :c !

select avg(noon-midnight)
from comfort
where city = 'KEENE' !
fetch into :a !

select avg(abs(noon-midnight))
from comfort
where city = 'KEENE' !
fetch into :a !

 * PAGE 157 !
 * ORACLE Error: ORA-0978 !
select sum(avg(noon)) from comfort!

 * PAGE 158 !
select max(noon) - min(noon)
from comfort
where city = 'SAN FRANCISCO' !
fetch into :a !

select city, avg(noon), max(noon), min(noon),
       max(noon) - min(noon) "Swing"
from comfort
group by city !
fetch into :a, :b, :c, :d, :e !

 * PAGE 159 !
select max(noon), avg(noon), min(noon), stddev(noon),
       variance(noon)
from comfort
where city = 'KEENE' !
fetch into :a, :b, :c, :d, :e !

select count(distinct city), count(city), count(*)
from comfort !
fetch into :a, :b, :c !

 * PAGE 160 !
select firstname, age from birthday !
fetch into :a, :b !

select avg(distinct age) "Average", 
       sum(distinct age) "Total"
from birthday !
fetch into :a, :b !

 * PAGE 161 !
 * see also page 155 !
select * from comfort !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i !

select city, day, greatest(midnight, noon) "High",
       least(midnight, noon) "Low"
from comfort !
fetch into :a, :b, :c, :d !

 * PAGE 162 !
 * ORACLE Error: ORA-0937 !
select city, day, max(noon)
from comfort !

select city, day, noon
from comfort
where noon = (select max(noon) from comfort) !
fetch into :a, :b, :c !

 * PAGE 163 !
select city, day, midnight
from comfort
where midnight = (select min(midnight) from comfort) !
fetch into :a, :b, :c !

select city, day, noon
from comfort
where noon = (select max(noon) from comfort) or
      noon = (select min(noon) from comfort) !
fetch into :a, :b, :c !

 * PAGE 164 !
select 2/2/4 from dual !
fetch into :a !

select 2/(2/4) from dual !
fetch into :a !

select * from newspaper
where section = 'B' and page = 1 or page = 2 !
fetch into :a, :b, :c !

 * PAGE 165 !
select * from newspaper
where page = 1 or page = 2 and section = 'B' !
fetch into :a, :b, :c !

select * from newspaper
where section = 'B' and (page = 1 or page = 2) !
fetch into :a, :b, :c !

commit work !
