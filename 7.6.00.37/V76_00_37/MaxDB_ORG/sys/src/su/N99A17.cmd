*   *ID* N99A17   DBCMD    changed on 1992-09-28-13.27.14 by ADMIN     *
file connect ( kern test !
create table ct0 (code fixed(3) key,
   symbol        char (3),
   code_iso      char (3),
   rndg_value    float(7),
   exch_unit     fixed(4))!
insert ct0 values (3, 's', 'iso', 7, 4)!
create table ct1 (usage_area fixed (2) key,
                  code       fixed (3) key,
   pos_price     fixed(1),
   pos_amounts   fixed(1))!
insert ct1 values (1, 3, 0, 9)!
create table ct2 (usage_area fixed (2) key,
                  code       fixed (3) key,
                  rate_date  char  (8) key,
   rate_counter  float(10),
   rate_denomi   float(10))!
insert ct2 values (1, 3, '19892808', 10, 9)!
data !
pars_then_ex !
select
   ct1.usage_area,
   ct2.code,
   ct2.rate_date,
   ct0.symbol,
   ct0.code_iso,
   ct0.rndg_value,
   ct0.exch_unit,
   ct2.rate_counter,
   ct2.rate_denomi
from ct0, ct1, ct2
where ct0.code = :code
  and ct1.usage_area = :ua
  and ct2.usage_area = ct1.usage_area
  and ct1.code = ct0.code
  and ct2.code = ct0.code
  and ct2.rate_date <= :rd !
n 3 3 n 2 1 c 8 '19990101'

nodata !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i, :j, :k !
pars_execute !
rollback release !
