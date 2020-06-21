&1 !
select * from t where rowno <= 5 !
fetch!
mfetch first !
10
fetch1 last!
fetch1 pos (6)!
fetch1 pos (3)!
select * from t,t1 where a = a1 and rowno <= 5 !
fetch!
mfetch first !
10
fetch1 last!
fetch1 pos (6)!
fetch1 pos (3)!
select * from t where rowno <= 5 order by b !
fetch!
mfetch first !
10
fetch1 last!
fetch1 pos (6)!
fetch1 pos (3)!
select max(a), b from t where rowno <= 5 group by b ! 
fetch!
mfetch first !
10
select sum(b), max(a1), a from t,t1 
where rowno <= 5
group by a !
fetch!
declare c cursor for 
  with recursive px (px_a, px_b) as
  (select a, b from t where b in (1,3)
   union all
   select a, b from t, px
       where px_b * 2 = t.b
  )
select *
from px where rowno <= 3 order by px_a !
fetch c !
select * from t t1 
 where rowno <= 5 
   and b = any (select b from t
                 where t1.a = t.a)!
fetch!                  
select distinct b mod 4 from t 
  where rowno <= 3 order by 1 !
fetch!
select top 5 * from t where rowno <= 5 !
select top 1 * from t !
fetch!
declare c cursor for select top 1 * from t for reuse !
fetch c!
select top 5 * from t !
fetch!
mfetch first !
10
select top 5 * from t order by b !
fetch!
mfetch first !
10
mfetch first !
5
mfetch !
5
select top 5 * from t,t1 where a = a1 order by b !
fetch!
mfetch first !
10
select top 5 max(a), b from t group by b ! 
fetch!
mfetch first !
10
select top 5 sum(b), max(a1), a from t,t1 
group by a !
fetch!
select top 5 * from t union all select * from t1 !
fetch !
select top 5 * from t union all select * from t1 order by 1 !
fetch !
declare c cursor for 
  with recursive px (px_a, px_b) as
  (select a, b from t where b in (1,3)
   union all
   select a, b from t, px
       where px_b * 2 = t.b
  )
select top 3 *
from px order by px_a !
fetch c !
select top 5 * from t t1 
 where b = any (select b from t
                 where t1.a = t.a)!
fetch!                  
select distinct top 3 b mod 4 from t 
  order by 1 !
fetch!
select * from t where rowno <= 5 limit 5 !
select * from t limit 1!
fetch!
declare c cursor for select * from t for reuse limit 1 !
fetch c!
mfetch pos (1) !
3
select * from t limit 5 !
fetch!
fetch1 last !
fetch1 prev !
fetch1 prev !
fetch1 pos (6)!
fetch1 pos (3)!
select * from t order by b limit 5 !
fetch!
mfetch first !
10
select max(a), b from t group by b limit 5 ! 
fetch!
mfetch first !
10
select sum(b), max(a1), a from t,t1 
group by a limit 5 !
fetch!
select sum(b), max(a1), a from t,t1 
group by a limit 2 3 !
fetch!
select * from t limit 2 3 !
fetch!
fetch1 last !
fetch1 prev !
fetch1 prev !
fetch first !
fetch !
mfetch first !
10
fetch1 pos (6)!
fetch1 pos (3)!
select * from t union all select * from t1 limit 5 !
fetch !
select * from t union all select * from t1 order by 1 limit 5 !
fetch !
select * from t,t1 where a = a1 limit 2 3 !
fetch!
mfetch first !
10
mfetch first !
3
mfetch !
3
select * from t order by b limit 2 3 !
fetch!
fetch1 last !
fetch1 prev !
fetch1 prev !
fetch first !
fetch !
fetch1 pos (6)!
fetch1 pos (3)!
select max(a), b from t group by b limit 2 3 ! 
fetch!
declare c cursor for 
  with recursive px (px_a, px_b) as
  (select a, b from t where b in (1,3)
   union all
   select a, b from t, px
       where px_b * 2 = t.b
  )
select *
from px order by px_a limit 4 !
fetch c !
declare c cursor for 
  with recursive px (px_a, px_b) as
  (select a, b from t where b in (1,3)
   union all
   select a, b from t, px
       where px_b * 2 = t.b
  )
select *
from px order by px_a limit 1 3 !
fetch c !
select * from t t1 
 where b = any (select b from t
                 where t1.a = t.a)
 order by a limit 2 3 !                 
fetch!                  
select distinct b mod 4 from t 
  order by 1 limit 3 !
fetch!

pars_execute !