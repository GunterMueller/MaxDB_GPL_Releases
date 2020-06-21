file connect ( kern test !
create table rechnung (
rechjahr char(4) key,
rechnr   fixed (5) key,
va            char (3) not null,
jahr          char (4) not null,
monat         fixed (4) not null,
rhythmus      char (1) not null range in ('w', 'm') ,
abrechklasse char (3) not null range in
            ('fix', 'opt', 'ccl', 'kum', 'HIT', 'sfx',
             'rfa', 'rab', 'mfl', 'mfr', 'zpf', 'zpo') ,
streckenklasse char (1)  not null range in ('K', 'M', 'L') ,
rp_typ        char (1) not null range in ('B', 'G') ,
waehrung      char (5) not null,
rechn_erstell_nr fixed (4) not null,
rech_status char (1) not null range in ('O', 'F') ,
datum_erstellt date not null,
benutzer_erstellt char (8) not null default ' ' )!
pars_then_ex! 
data !
declare cursor_rechnung cursor for 
select rechjahr, rechnr  , va           , jahr         ,
monat        , rhythmus     , abrechklasse ,
streckenklasse, rp_typ       , waehrung     ,
rechn_erstell_nr,
rech_status,
datum_erstellt,
benutzer_erstellt
from rechnung
where (abrechklasse = :a or
       abrechklasse = :b)
  and (rechn_erstell_nr = 9999 or
       datum_erstellt  >= :c )
  and va                = :d
order by rechjahr, rechnr !
c 3 'fix'
c 3 'opt'
c 10 '19910212'
c 3 'abc'

nodata!
fetch cursor_rechnung into :a, :b, :c, :d, :e, :f, :g !
close cursor_rechnung !
pars_execute!
create index re_jahr_akl_va on rechnung (jahr,
abrechklasse, va, streckenklasse,
waehrung, monat, rechn_erstell_nr) !
pars_then_ex! 
data !
declare cursor_rechnung cursor for 
select rechjahr, rechnr  , va           , jahr         ,
monat        , rhythmus     , abrechklasse ,
streckenklasse, rp_typ       , waehrung     ,
rechn_erstell_nr,
rech_status,
datum_erstellt,
benutzer_erstellt
from rechnung
where (abrechklasse = :a or
       abrechklasse = :b)
  and (rechn_erstell_nr = 9999 or
       datum_erstellt  >= :c )
  and va                = :v
order by rechjahr, rechnr !
c 3 'fix'
c 3 'opt'
c 10 '19910212'
c 3 'abc'

nodata!
fetch cursor_rechnung into :a, :b, :c, :d, :e, :f, :g !
close cursor_rechnung !
pars_execute!
rollback release !
