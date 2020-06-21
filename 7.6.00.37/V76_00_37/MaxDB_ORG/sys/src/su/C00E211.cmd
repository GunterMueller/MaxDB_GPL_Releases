*   *ID* C00E211  DBCMD    changed on 1992-09-28-13.23.06 by ADMIN     *
file cleark !
file connect ( kern test !
* Kern-Fehlerprotokoll 211 !
create table tab1 (
cd_station fixed (3,0) key,
bdg_geg_station date range between '19900101' and '21001231',
srt_station char (1) ascii not null,
edg_geg_station date range between '19900101' and '21001231',
naam_station char (16) ascii not null,
nr_infopagina fixed (4) range between 1 and 9999,
kma_aftrek_grensp fixed (2) range between 0 and 20,
oms_zones char (19) ascii)!
insert tab1 values (2, '19910331', 'N', '19910818', 'abc', 567,4,'qw')!
create table tab2 (cd_station fixed (3) key range not in (1),
bdg_st_ap_afstand date key range between '19900101' and '21001231',
edg_st_ap_afstand date key range between '19900101' and '21001231',
nr_aa_p_1 fixed (3) not null,
kma_1e_a1 fixed(3) not null,
kma_2e_a1 fixed(3) not null,
nr_aa_p_2 fixed (3),
kma_1e_a2 fixed(3),
kma_2e_a2 fixed(3) )!
insert tab2 values (2, '19910522', '19910525', 3, 1, 2, null, null,null)!
create view cvi as select a.cd_station, a.bdg_geg_station,
a.edg_geg_station, b.nr_aa_p_1,
b.kma_1e_a1, b.kma_2e_a1, b.nr_aa_p_2, b.kma_1e_a2,
b.kma_2e_a2, b.bdg_st_ap_afstand, b.edg_st_ap_afstand
from tab1 a, tab2 b
where a.cd_station = b.cd_station and
a.srt_station = 'N' !
select cd_station, bdg_geg_station from cvi
order by nr_aa_p_1 !
select cd_station, nr_aa_p_1, bdg_geg_station from cvi
order by nr_aa_p_1 !
select a.cd_station, bdg_geg_station
from <tab1 a, tab2 b>
where a.cd_station = b.cd_station and
a.srt_station = 'N'
order by nr_aa_p_1 !
select b.cd_station, bdg_geg_station
from <tab2 b, tab1 a>
where a.cd_station = b.cd_station and
a.srt_station = 'N'
order by nr_aa_p_1 !
rollback release !
