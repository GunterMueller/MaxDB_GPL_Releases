*   *ID* FRCORSUB DBCMD    changed on 1992-09-28-13.23.49 by ADMIN     *
*
file connect ( kern test !
create table big0 ( eq1000 fixed(5)) !
insert big0 set eq1000 = 111 !
insert big0 set eq1000 = 112 !
insert big0 set eq1000 = 121 !
insert big0 set eq1000 = 211 !
insert big0 set eq1000 = 311 !
insert big0 set eq1000 = 131 !
insert big0 set eq1000 = 113 !
insert big0 set eq1000 = 114 !
insert big0 set eq1000 = 141 !
insert big0 set eq1000 = 411 !
create table big1 ( eq1000 fixed(5)) !
insert big1 set eq1000 = 111 !
insert big1 set eq1000 = 114 !
insert big1 set eq1000 = 121 !
insert big1 set eq1000 = 211 !
insert big1 set eq1000 = 311 !
insert big1 set eq1000 = 132 !
insert big1 set eq1000 = 113 !
insert big1 set eq1000 = 119 !
insert big1 set eq1000 = 141 !
insert big1 set eq1000 = 411 !
create table big2 ( eq1000 fixed(5)) !
insert big2 set eq1000 = 111 !
insert big2 set eq1000 = 112 !
insert big2 set eq1000 = 121 !
insert big2 set eq1000 = 211 !
insert big2 set eq1000 = 311 !
insert big2 set eq1000 = 131 !
insert big2 set eq1000 = 113 !
insert big2 set eq1000 = 114 !
insert big2 set eq1000 = 141 !
insert big2 set eq1000 = 411 !
commit work !
select distinct eq1000 from
   (select eq1000, 'FROMSELECT' from
       (select distinct eq1000 from big2 a
       where not exists
           (select distinct eq1000 from big1 b
           where a.eq1000 = b.eq1000))
    where eq1000 between 100 and 150 ) !
FETCH !
   (select eq1000, 'FROMSELECT' from
       (select distinct eq1000 from big2 a
       where not exists
           (select distinct eq1000 from big1 b
           where a.eq1000 = b.eq1000))
    where eq1000 between 100 and 150 ) !
FETCH !
select distinct eq1000 from
   (select distinct eq1000 from
       (select distinct eq1000 from big2 a
       where eq1000 between 100 and 150
       and not exists
           (select distinct eq1000 from big1 b
           where a.eq1000 = b.eq1000)) c
    where eq1000 between 100 and 150
    and not exists
        (select eq1000 from big0 d
         where c.eq1000 = d.eq1000)) e
where eq1000 between 100 and 150
and not exists
    (select eq1000 from big1 f
     where e.eq1000 = f.eq1000) !
FETCH !
       declare c cursor for 
       (select distinct eq1000 from big2 a
       where eq1000 between 100 and 150
       and not exists
           (select distinct eq1000 from big1 b
           where a.eq1000 = b.eq1000)) !
select distinct eq1000 from
   (select distinct eq1000 from  c
    where eq1000 between 100 and 150
    and not exists
        (select eq1000 from big0 d
         where c.eq1000 = d.eq1000)) e
where eq1000 between 100 and 150
and not exists
    (select eq1000 from big1 f
     where e.eq1000 = f.eq1000) !
FETCH !
select distinct eq1000 from
   (select distinct eq1000 from
       (select distinct eq1000 from big2 a
       where eq1000 between 100 and 150
       and not exists
           (select distinct eq1000 from big1 b
           where a.eq1000 = b.eq1000)) c
    where eq1000 between 100 and 150
    and not exists
        (select eq1000 from big0 d
         where c.eq1000 = d.eq1000)) e
where eq1000 between 100 and 150
and not exists
    (select eq1000 from big1 f
     where e.eq1000 = f.eq1000) !
fetch !
SELECT DISTINCT eq1000 FROM
   (SELECT DISTINCT eq1000 FROM
       (SELECT DISTINCT eq1000 FROM BIG2 A
       WHERE NOT EXISTS
           (SELECT DISTINCT eq1000 FROM BIG1 B
           WHERE A.eq1000 = B.eq1000)) C
    WHERE NOT EXISTS
        (SELECT eq1000 FROM BIG0 D
     WHERE D.eq1000 = C.eq1000)) E
WHERE eq1000 BETWEEN 100 AND 150
AND NOT EXISTS
    (SELECT eq1000 FROM BIG1 F
     WHERE E.eq1000 = F.eq1000) !
FETCH !
SELECT DISTINCT eq1000 FROM
   (SELECT DISTINCT eq1000 FROM
       (SELECT DISTINCT eq1000 FROM BIG2 A
                                                  ) C
    WHERE NOT EXISTS
        (SELECT eq1000 FROM BIG0 D
     WHERE D.eq1000 = C.eq1000)) E
WHERE eq1000 BETWEEN 100 AND 150
AND NOT EXISTS
    (SELECT eq1000 FROM BIG1 F
     WHERE E.eq1000 = F.eq1000) !
FETCH !
SELECT DISTINCT eq1000 FROM
   (SELECT DISTINCT eq1000 FROM
       (SELECT DISTINCT eq1000 FROM BIG2 A
                                                  ) C
     ) E
WHERE eq1000 BETWEEN 100 AND 150
AND NOT EXISTS
    (SELECT eq1000 FROM BIG1 F
     WHERE E.eq1000 = F.eq1000) !
FETCH !
SELECT DISTINCT eq1000 FROM
   (SELECT DISTINCT eq1000 FROM
       (SELECT DISTINCT eq1000 FROM BIG2 A
       WHERE NOT EXISTS
           (SELECT DISTINCT eq1000 FROM BIG1 B
           WHERE A.eq1000 = B.eq1000)) C
     ) E
WHERE eq1000 BETWEEN 100 AND 150
AND NOT EXISTS
    (SELECT eq1000 FROM BIG1 F
     WHERE E.eq1000 = F.eq1000) !
FETCH !
SELECT DISTINCT eq1000 FROM
   (SELECT DISTINCT eq1000 FROM
       (SELECT DISTINCT eq1000 FROM BIG2 A
       WHERE NOT EXISTS
           (SELECT DISTINCT eq1000 FROM BIG1 B
           WHERE A.eq1000 = B.eq1000)) C
     ) E !
FETCH !
rollback release !
