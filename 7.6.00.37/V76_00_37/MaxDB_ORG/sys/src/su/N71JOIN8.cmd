file connect ( kern test !
create table credit (
proty char  ( 3  ) key default ' ',
freda fixed ( 8  ) key default 0 ,
proje char  ( 10 ) key default ' ',
auftr fixed ( 6  ) key default 0 ,
eirec fixed ( 6  ) key default 0 ,
banlz char  ( 8 ) not null default ' ',
banl1 char  ( 8 ) not null default ' ',
banko char  ( 10 ) not null default ' ',
bank1 char  ( 10 ) not null default ' ',
betra fixed ( 12,2  ) not null default 0 ,
anrec char  ( 15 ) not null default ' ',
datum fixed ( 8  ) not null default 0 ,
ldate fixed ( 8  ) not null default 0 ,
ueber char  ( 1  ) not null default ' ',
rueck char  ( 1  ) not null default ' ',
bfrei char  ( 1  ) not null default ' ',
kurz1 char  ( 35 ) not null default ' ',
kurz2 char  ( 35 ) not null default ' ',
kurz3 char  ( 35 ) not null default ' ',
bemer char  ( 35 ) not null default ' '
) !
insert credit set proty = 'BAU', 
                          freda = 19941121, 
                          proje = 'projekt5', 
                          auftr = 3, 
                          eirec = 0 ,
                          ueber = 'J',
                          rueck = 'N',
                          bfrei = 'J' !
insert credit set proty = 'BAU', 
                          freda = 19941121, 
                          proje = 'projekt5', 
                          auftr = 0, 
                          eirec = 3, 
                          ueber = 'N',
                          rueck = 'J',
                          bfrei = 'J' !
insert credit set proty = 'BAU', 
                          freda = 19941121, 
                          proje = 'projekt5', 
                          auftr = 3, 
                          eirec = 3, 
                          ueber = 'N',
                          rueck = 'N',
                          bfrei = 'J' !
create table rechnung (
proje char  ( 10 ) key default ' ',
auftr fixed ( 6  ) key default 0 ,
eirec fixed ( 6  ) key default 0 ,
durch char  ( 1  ) not null default ' ',
klass char  ( 2  ) not null default ' ',
klawe fixed ( 2  ) not null default 0 ,
anrec char  ( 15 ) not null default ' ',
datum fixed ( 8  ) not null default 0 ,
eigda fixed ( 8  ) not null default 0 ,
faeda fixed ( 8  ) not null default 0 ,
freda fixed ( 8  ) not null default 0 ,
mwstk fixed ( 2  ) not null default 0 ,
mwst0 fixed ( 4,2 ) not null default 0 ,
recun fixed ( 12,2 ) not null default 0 ,
recub fixed ( 12,2 ) not null default 0 ,
recan fixed ( 12,2 ) not null default 0 ,
recab fixed ( 12,2 ) not null default 0 ,
schlk char  ( 1  ) not null default ' ',
nettg char  ( 1  ) not null default ' ',
zahan fixed ( 12,2 ) not null default 0 ,
zahab fixed ( 12,2 ) not null default 0 ,
mannr char  ( 7  ) not null default '0',
unter char  ( 7  ) not null default ' ',
storn char  ( 1  ) not null default ' ',
ueber char  ( 1  ) not null default ' ',
ldate fixed ( 8  ) not null default 0 ,
skont fixed ( 4,2 ) not null default 0 ,
kurz1 char  ( 35 ) not null default ' '
) !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt1 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt2 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt3 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt4 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt5 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt6 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt7 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt8 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt9 ', 
                        auftr = 3,
                        eirec = 3 !
insert rechnung set proje = 'projekt10', 
                        auftr = 0,
                        eirec = 0 !
insert rechnung set proje = 'projekt10', 
                        auftr = 0,
                        eirec = 1 !
insert rechnung set proje = 'projekt10', 
                        auftr = 1,
                        eirec = 0 !
insert rechnung set proje = 'projekt10', 
                        auftr = 1,
                        eirec = 1 !
insert rechnung set proje = 'projekt10', 
                        auftr = 1,
                        eirec = 2 !
insert rechnung set proje = 'projekt10', 
                        auftr = 2,
                        eirec = 0 !
insert rechnung set proje = 'projekt10', 
                        auftr = 0,
                        eirec = 2 !
insert rechnung set proje = 'projekt10', 
                        auftr = 3,
                        eirec = 0 !
insert rechnung set proje = 'projekt10', 
                        auftr = 0,
                        eirec = 3 !
insert rechnung set proje = 'projekt10', 
                        auftr = 3,
                        eirec = 3 !
commit !
explain declare zz_9221 cursor for select all 
credit.proty,
credit.freda,
credit.proje,
credit.auftr,
credit.eirec,
credit.kurz1,
credit.kurz2,
credit.kurz3,
credit.banko,
credit.banlz,
credit.datum,
credit.anrec,
credit.betra,
credit.bemer,
credit.rueck,
credit.ueber
from <credit , rechnung>
where
        credit.proty = 'BAU'
and     credit.proje = rechnung.proje
and     credit.auftr = rechnung.auftr
and     credit.eirec = rechnung.eirec
and     rechnung.mannr = '0'
and (   (       credit.ueber = 'J'
        and     credit.bfrei = 'J'
        and     credit.ldate = 0
        )
    or          credit.rueck = 'J'
    or  (       credit.ueber = 'N'
        and     credit.rueck = 'N'
        and     credit.freda = 19941121
        and     credit.ldate = 0
        )
    )
order by        credit.ueber,
                credit.rueck,
                credit.proty,
                credit.freda,
                credit.proje,
                credit.auftr,
                credit.eirec !
declare zz_9221 cursor for select all 
credit.proty,
credit.freda,
credit.proje,
credit.auftr,
credit.eirec,
credit.kurz1,
credit.kurz2,
credit.kurz3,
credit.banko,
credit.banlz,
credit.datum,
credit.anrec,
credit.betra,
credit.bemer,
credit.rueck,
credit.ueber
from <credit , rechnung>
where
        credit.proty = 'BAU'
and     credit.proje = rechnung.proje
and     credit.auftr = rechnung.auftr
and     credit.eirec = rechnung.eirec
and     rechnung.mannr = '0'
and (   (       credit.ueber = 'J'
        and     credit.bfrei = 'J'
        and     credit.ldate = 0
        )
    or          credit.rueck = 'J'
    or  (       credit.ueber = 'N'
        and     credit.rueck = 'N'
        and     credit.freda = 19941121
        and     credit.ldate = 0
        )
    )
order by        credit.ueber,
                credit.rueck,
                credit.proty,
                credit.freda,
                credit.proje,
                credit.auftr,
                credit.eirec !
fetch zz_9221 !
declare zz_9221 cursor for select all 
credit.proty,
credit.freda,
credit.proje,
credit.auftr,
credit.eirec,
credit.kurz1,
credit.kurz2,
credit.kurz3,
credit.banko,
credit.banlz,
credit.datum,
credit.anrec,
credit.betra,
credit.bemer,
credit.rueck,
credit.ueber
from <credit , rechnung>
where
        credit.proty = 'BAU'
and     credit.proje = rechnung.proje
and     credit.auftr = rechnung.auftr
and     credit.eirec = rechnung.eirec
and     rechnung.mannr = '0'
and (
                credit.rueck = 'J'
    or
        (       credit.ueber = 'J'
        and     credit.bfrei = 'J'
        and     credit.ldate = 0
        )
    or  (       credit.ueber = 'N'
        and     credit.rueck = 'N'
        and     credit.freda = 19941121
        and     credit.ldate = 0
        )
    )
order by        credit.ueber,
                credit.rueck,
                credit.proty,
                credit.freda,
                credit.proje,
                credit.auftr,
                credit.eirec !
fetch zz_9221 !
declare zz_9221 cursor for select all 
credit.proty,
credit.freda,
credit.proje,
credit.auftr,
credit.eirec,
credit.kurz1,
credit.kurz2,
credit.kurz3,
credit.banko,
credit.banlz,
credit.datum,
credit.anrec,
credit.betra,
credit.bemer,
credit.rueck,
credit.ueber
from <credit , rechnung>
where
        credit.proty = 'BAU'
and     credit.proje = rechnung.proje
and     credit.auftr = rechnung.auftr
and     credit.eirec = rechnung.eirec
and     rechnung.mannr = '0'
and (
        (       credit.ueber = 'N'
        and     credit.rueck = 'N'
        and     credit.freda = 19941121
        and     credit.ldate = 0
        )
or
       (       credit.ueber = 'J'
        and     credit.bfrei = 'J'
        and     credit.ldate = 0
        )
    or          credit.rueck = 'J'
    )
order by        credit.ueber,
                credit.rueck,
                credit.proty,
                credit.freda,
                credit.proje,
                credit.auftr,
                credit.eirec !
fetch zz_9221 !
declare zz_9221 cursor for select all 
credit.proty,
credit.freda,
credit.proje,
credit.auftr,
credit.eirec,
credit.kurz1,
credit.kurz2,
credit.kurz3,
credit.banko,
credit.banlz,
credit.datum,
credit.anrec,
credit.betra,
credit.bemer,
credit.rueck,
credit.ueber
from <credit , rechnung>
where
        credit.proty = 'BAU'
and     credit.proje = rechnung.proje
and     credit.auftr = rechnung.auftr
and     credit.eirec = rechnung.eirec
and     rechnung.mannr = '0'
and (
        (       credit.ueber = 'N'
        and     credit.rueck = 'N'
        and     credit.freda = 19941121
        and     credit.ldate = 0
        )
    or          credit.rueck = 'J'
or
       (       credit.ueber = 'J'
        and     credit.bfrei = 'J'
        and     credit.ldate = 0
        )
    )
order by        credit.ueber,
                credit.rueck,
                credit.proty,
                credit.freda,
                credit.proje,
                credit.auftr,
                credit.eirec !
fetch zz_9221 !
declare zz_9221 cursor for select all 
credit.proty,
credit.freda,
credit.proje,
credit.auftr,
credit.eirec,
credit.kurz1,
credit.kurz2,
credit.kurz3,
credit.banko,
credit.banlz,
credit.datum,
credit.anrec,
credit.betra,
credit.bemer,
credit.rueck,
credit.ueber
from <credit , rechnung>
where
        credit.proty = 'BAU'
and     credit.proje = rechnung.proje
and     credit.auftr = rechnung.auftr
and     credit.eirec = rechnung.eirec
and     rechnung.mannr = '0'
and (
       (       credit.ueber = 'J'
        and     credit.bfrei = 'J'
        and     credit.ldate = 0
        )
or
        (       credit.ueber = 'N'
        and     credit.rueck = 'N'
        and     credit.freda = 19941121
        and     credit.ldate = 0
        )
    or          credit.rueck = 'J'
    )
order by        credit.ueber,
                credit.rueck,
                credit.proty,
                credit.freda,
                credit.proje,
                credit.auftr,
                credit.eirec !
fetch zz_9221 !
declare zz_9221 cursor for select all 
credit.proty,
credit.freda,
credit.proje,
credit.auftr,
credit.eirec,
credit.kurz1,
credit.kurz2,
credit.kurz3,
credit.banko,
credit.banlz,
credit.datum,
credit.anrec,
credit.betra,
credit.bemer,
credit.rueck,
credit.ueber
from <credit , rechnung>
where
        credit.proty = 'BAU'
and     credit.proje = rechnung.proje
and     credit.auftr = rechnung.auftr
and     credit.eirec = rechnung.eirec
and     rechnung.mannr = '0'
and (
               credit.rueck = 'J'
or
        (       credit.ueber = 'N'
        and     credit.rueck = 'N'
        and     credit.freda = 19941121
        and     credit.ldate = 0
        )
or
       (       credit.ueber = 'J'
        and     credit.bfrei = 'J'
        and     credit.ldate = 0
        )
    )
order by        credit.ueber,
                credit.rueck,
                credit.proty,
                credit.freda,
                credit.proje,
                credit.auftr,
                credit.eirec !
fetch zz_9221 !
commit work release !
