#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

# Inputfile for QueryRewrite_Test
#
# > Query definitions
#
# Queries must not have one of the hints "QUERYREWRITE_*" as this would 
# Definitions may range over more than one line und must stop with an '!' 
# Anything in a line after '#' will be ignore
# 
# Existing tables are T1 ... T10 with columns (K int primary key, A1 int, ..., An int)
#  where Tn has columns up to An
# Existing views are defined in QueryRewriteViews.def
#
# Tables as well as Views are created in schema QRW by default
#

# some select from tables Tn

select @ *, T2.*, T2.K, K from T2 !

select @ * from T2 order by A2 !

select @ A1, sum(A2) from T2 group by A1 !

select @ * from (
 select * from T1
) !

select @ * from (
 select * from T1 WHERE K < 100
) !

select @ * from (
 select * from T1
) WHERE K < 100 !

select @ * from (
 select * from T1 WHERE K < 100
) WHERE K > 50 !

select @ * from (
select K, A1 from T1 UNION ALL select K, A2 from T2
) WHERE K < 100 !

(select @ * from (select * from T1))
UNION ALL
(select * from (select * from T1)) !
 

select @ * from (
(select * from (select * from T1))
UNION
(select * from (select * from T1))
) !

select @ y1, y2, s, m, y3 from (
select b1 as y1, a1 as y2, k as y3, a1+b1 as s, (a1+b1)/2 as m from (
 select a1, k, a1 as b1 from t1 where k < 100
 UNION select a4, a3, a2 from t4 where a3 > 7
 ) as y WHERE k <> 27 AND b1 > 40 AND y.k > 10 AND y.b1 < 300
) as a4 where s > 2 and m < 700 !

select @ ma, av, mi, co from (
 select max(t3.a2) as ma, min(t2.a1) as mi, avg(t1.k) as av, count(*) as co from t1,t2,t3 
) where ma >100 and mi < 20 and co <> 7 !

select @ a1 as d1, k as s1 from t1 
union all 
select k as s2, a1 as d2 from t2 !

select @ a1 as d1, k as s1 from t1 
union 
select k as s2, a1 as d2 from t2 
union 
select k as s3, a2 as d3 from t2 !

select @ * from (select a1 as d1, k as s1 from t1 
union all 
select k as s2, a1 as d2 from t2 
union 
select k as s3, a2 as d3 from t2) where d1<>1 !

select @ max(a1) from t1 group by k !

select @ * from (select * from t1, t3) where a3 > 50 and a2 < 100 !

select @ * from (select k,t1.a1 from t1 UNION select k,t2.a2 from t2) where a1 < 4 !

select @ * from t1 order by k !
select @ k from t1 order by k !              
select @ a1 from t1 order by k !              
select @ k as b from t1 order by k !
select @ a1 as k from t1 order by k !       
select @ k+1 from t1 order by k+1 !       
select @ k+1 from t1 order by k !           
select @ k from t1 order by k+1 !           
select @ k+1 as x from t1 order by k+1 !   
select @ k+1 as x from t1 order by k !     
select @ k as x from t1 order by k+1 !   
select @ k+1 as x from t1 order by x+1 !
select @ k+1 as x from t1 order by x !       
select @ k as x from t1 order by x+1 !      

select @ a1 from t1 group by a1 !
select @ a1, avg(k) from t1 group by a1 !

select @ * from t1 union select k,a2 from t2 order by 1 !

select @ * from t1 for update of a1 !

select @ k from (select a1 as b, k from t1 where k=1)  where k<3 !

select @ a1 as b, k from t1 where k=1 and a1<3 !

select @ k from (select distinct k from t1 where k=1 group by k) where k<3 !

select @ c2 as d1, c1 as d2, r2.c1+c2 as s1 from 
 ( select r1.b as c1, r1.c as c2 from 
   ( select k as b, a1 as c from t1 ) as r1 
   where b=1 ) as r2 
 where r2.c1<7 !

select @ r1.b as c1, r1.c as c2 from ( select k as b, a1 as c from t1) as r1 where b=1 !
 
select @ * from ( select k as b, a1 as c from t1) as r1 where b=1 !

select @ * from ( select a3,a2 as x ,a1 as d from t4 ) as u1 where d=2 !


# some select from complex views

SELECT @ * FROM V1 as X WHERE X.a1<100 !

SELECT @ * FROM (
 SELECT * FROM V1
) WHERE a1>100 !

SELECT @ * FROM (
 SELECT a3 as z FROM T4 WHERE k<100 UNION SELECT * FROM V1
) WHERE z>100 !

SELECT @ * FROM T3 where a1 < ANY ( SELECT a1 FROM T6 ) !

SELECT @ * FROM T3 where a1 = ANY ( SELECT a1 FROM T6 ) !

SELECT @ * FROM T3 where a1 = ANY ( SELECT * FROM ( SELECT a1 FROM T6 ) ) !

SELECT @ * FROM T3 where a1 = ANY ( SELECT * FROM ( SELECT a1 FROM T6 UNION SELECT a2 FROM T5 ) ) !

SELECT @ * FROM T3 where a1 = ANY ( SELECT * FROM ( SELECT a1 FROM T6 UNION ALL SELECT a2 FROM T5 ) ) !

SELECT @ * FROM T3 where a1 = ANY ( SELECT * FROM ( SELECT a1 FROM T6 INTERSECT SELECT a2 FROM T5 ) ) !

SELECT @ * FROM T3 where a1 = ANY ( SELECT a1 FROM T6 UNION SELECT a2 FROM T5 ) !

SELECT @ * FROM T3 where a1 = ANY ( SELECT a1 FROM T6 UNION ALL SELECT a2 FROM T5 ) !

SELECT @ * FROM T3 where a1 = ANY ( SELECT a1 FROM T6 INTERSECT SELECT a2 FROM T5 ) !

SELECT @ * FROM T3 where a1 < ALL ( SELECT a1 FROM T6 ) !

SELECT @ * FROM T3 where a1 = ALL ( SELECT a1 FROM T6 ) !

SELECT @ * FROM T3 where (a1,a2) = ANY ( SELECT a3, a1 FROM T6 ) !

SELECT @ * FROM T3 where (a1,a2) = ALL ( SELECT a3, a1 FROM T6 ) !

SELECT @ * FROM T3 where a1 IN ( SELECT a1 FROM T6 ) !

SELECT @ * FROM T3 where a1 IN ( SELECT max(a1) FROM T6 ) !















