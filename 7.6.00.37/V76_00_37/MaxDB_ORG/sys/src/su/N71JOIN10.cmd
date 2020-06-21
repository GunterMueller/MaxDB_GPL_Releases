file CONNECT ( KERN TEST SQLMODE ORACLE!
CREATE TABLE bestand_verf
(
    c_id                     INTEGER                  ,
    anlage_user              CHARACTER(15)            ,
    anlage_termin            CHARACTER(16)            ,
    aend_user                CHARACTER(15)            ,
    aend_termin              CHARACTER(16)            ,
    auftr_nr                 CHARACTER(20)            NOT NULL,
    auftr_art                CHARACTER(4)             NOT NULL,
    stab_auftr_art           CHARACTER(4)             ,
    split_nr                 CHARACTER(4)             NOT NULL,
    avo_nr                   CHARACTER(10)            NOT NULL,
    avo_art                  CHARACTER(4)             ,
    avo_split_nr             CHARACTER(4)             ,
    stab_avo_art             CHARACTER(4)             ,
    la_split_nr              CHARACTER(4)             NOT NULL,
    pos_nr                   CHARACTER(10)            NOT NULL,
    pos_split_nr             CHARACTER(4)             NOT NULL,
    lb_nr                    CHARACTER(20)            NOT NULL,
    verf_menge               DOUBLE PRECISION         ,
    teile_nr                 CHARACTER(20)            ,
    aend_idx                 CHARACTER(6)             ,
    chargen_nr               CHARACTER(20)            
) TABLESPACE ts_data !

GRANT ALL ON bestand_verf TO PUBLIC !

CREATE UNIQUE INDEX bestand_verf_idx ON bestand_verf
(
    auftr_nr,
    auftr_art,
    split_nr,
    avo_nr,
    la_split_nr,
    pos_nr,
    pos_split_nr,
    lb_nr
) TABLESPACE ts_idx !

CREATE TABLE lag_einh
(
    c_id                     INTEGER                  ,
    anlage_user              CHARACTER(15)            ,
    anlage_termin            CHARACTER(16)            ,
    aend_user                CHARACTER(15)            ,
    aend_termin              CHARACTER(16)            ,
    le_nr                    CHARACTER(12)            NOT NULL,
    liegt_auf_le_nr          CHARACTER(12)            ,
    basis_le_nr              CHARACTER(12)            ,
    nv_le_nr                 CHARACTER(12)            ,
    thm_typ                  CHARACTER(4)             ,
    stab_thm_typ             CHARACTER(4)             ,
    permanent_kez            CHARACTER(4)             ,
    le_nr_ext_kez            CHARACTER(4)             ,
    thm_ok_grad              DOUBLE PRECISION         ,
    bedingt_verwend_kez      CHARACTER(4)             ,
    stab_jnv_kez             CHARACTER(4)             ,
    lb_nr                    CHARACTER(20)            ,
    lo_nr                    CHARACTER(9)             ,
    teile_nr                 CHARACTER(20)            ,
    aend_idx                 CHARACTER(6)             ,
    chargen_nr               CHARACTER(20)            ,
    sn                       CHARACTER(25)            ,
    b_chargen_nr             CHARACTER(20)            ,
    chargen_rein_kez         CHARACTER(4)             ,
    frei_menge               DOUBLE PRECISION         ,
    res_menge                DOUBLE PRECISION         ,
    trp_nr                   CHARACTER(10)            ,
    we_a_nr                  CHARACTER(20)            ,
    we_a_art                 CHARACTER(4)             ,
    we_pos_nr                CHARACTER(10)            ,
    we_split_nr              CHARACTER(4)             ,
    auftr_nr                 CHARACTER(20)            ,
    auftr_art                CHARACTER(4)             ,
    stab_auftr_art           CHARACTER(4)             ,
    split_nr                 CHARACTER(4)             ,
    avo_nr                   CHARACTER(10)            ,
    avo_art                  CHARACTER(4)             ,
    stab_avo_art             CHARACTER(4)             ,
    avo_split_nr             CHARACTER(4)             ,
    la_split_nr              CHARACTER(4)             ,
    pos_nr                   CHARACTER(10)            ,
    pos_split_nr             CHARACTER(4)             ,
    ist_gew                  DOUBLE PRECISION         ,
    ist_hoehe                DOUBLE PRECISION         ,
    ist_laenge               DOUBLE PRECISION         ,
    ist_breite               DOUBLE PRECISION         ,
    ladelisten_nr            CHARACTER(20)            ,
    element_nr               CHARACTER(20)            ,
    element_ebene            CHARACTER(4)             ,
    stab_hier_ebene          CHARACTER(4)             ,
    mandant_teile            CHARACTER(20)            ,
    mandant_le               CHARACTER(20)            ,
    liefer_nr                CHARACTER(20)            ,
    liefersch_nr             CHARACTER(20)            ,
    wert                     DOUBLE PRECISION         ,
    sperre                   CHARACTER(4)             ,
    abc_ken                  CHARACTER(4)             ,
    stab_abc_ken             CHARACTER(4)             ,
    gefahr_ken               CHARACTER(4)             ,
    stab_gefahr_ken          CHARACTER(4)             ,
    xyz_ken                  CHARACTER(4)             ,
    stab_xyz_ken             CHARACTER(4)             ,
    techn_ken                CHARACTER(4)             ,
    stab_techn_ken           CHARACTER(4)             ,
    einlager_termin          CHARACTER(16)            ,
    kons_termin              CHARACTER(16)            ,
    qa_termin                CHARACTER(16)            ,
    mhd_termin               CHARACTER(16)            ,
    we_termin                CHARACTER(16)            ,
    inv_termin               CHARACTER(16)            ,
    inv_user                 CHARACTER(15)            ,
    inv_ta                   CHARACTER(31)            ,
    kom_nr                   CHARACTER(20)            ,
    zaehl_liste              CHARACTER(20)            
) TABLESPACE ts_data !

GRANT ALL ON lag_einh TO PUBLIC !

CREATE UNIQUE INDEX lag_einh_idx ON lag_einh
(
    le_nr
) TABLESPACE ts_idx !

CREATE VIEW v_bestand_verf_kumuliert as
 SELECT  teile_nr,
   aend_idx,
   chargen_nr,
   lb_nr,
   sum(bsvf.verf_menge)verf_menge
 FROM  bestand_verf bsvf
 GROUP BY teile_nr,
   aend_idx,
   chargen_nr,
   lb_nr !

CREATE VIEW bestand_kum as
 SELECT 101000 c_id, '1995010100000000' anlage_termin, 'FELIX' anlage_user, 
 '1995010100000000' aend_termin, 'FELIX' aend_user, le.lb_nr, le.teile_nr,
 le.aend_idx, le.chargen_nr, sum(le.frei_menge) frei_menge,
 sum(bsvf.verf_menge)/count(*) verf_menge, sum(le.res_menge) res_menge,
 0 gesp_menge
 FROM lag_einh le, v_bestand_verf_kumuliert bsvf
 WHERE le.teile_nr = bsvf.teile_nr AND
 le.aend_idx = bsvf.aend_idx AND
 le.chargen_nr = bsvf.chargen_nr AND
 le.lb_nr = bsvf.lb_nr AND
 le.sperre = '0' 
 GROUP BY le.teile_nr,
 le.aend_idx,
 le.chargen_nr,
 le.lb_nr
 UNION
 SELECT 101000 c_id, '1995010100000000' anlage_termin, 'FELIX' anlage_user,
 '1995010100000000' aend_termin, 'FELIX' aend_user, le.lb_nr, le.teile_nr,
 le.aend_idx, le.chargen_nr, sum(le.frei_menge) frei_menge, 0 verf_menge,
 sum(le.res_menge) res_menge, 0 gesp_menge
 FROM lag_einh le
 WHERE le.sperre = '0' AND
 not exists (SELECT bsv.teile_nr
 FROM bestand_verf bsv
 WHERE bsv.teile_nr = le.teile_nr AND 
 bsv.aend_idx = le.aend_idx AND
 bsv.chargen_nr = le.chargen_nr AND
 bsv.lb_nr = le.lb_nr)
 GROUP BY le.teile_nr,
 le.aend_idx,
 le.chargen_nr,
 le.lb_nr
 UNION
 SELECT 101000 c_id, '1995010100000000' anlage_termin, 'FELIX' anlage_user,
 '1995010100000000' aend_termin, 'FELIX' aend_user, le.lb_nr, le.teile_nr,
 le.aend_idx, le.chargen_nr, 0 frei_menge,
 sum(bsvf.verf_menge)/count(*) verf_menge, 0 res_menge,
 sum(le.frei_menge) + sum(le.res_menge) gesp_menge
 FROM lag_einh le, v_bestand_verf_kumuliert bsvf
 WHERE le.teile_nr = bsvf.teile_nr AND
 le.aend_idx = bsvf.aend_idx AND
 le.chargen_nr = bsvf.chargen_nr AND
 le.lb_nr = bsvf.lb_nr AND
 le.sperre = '1' 
 GROUP BY le.teile_nr,
 le.aend_idx,
 le.chargen_nr,
 le.lb_nr
 UNION
 SELECT 101000 c_id, '1995010100000000' anlage_termin, 'FELIX' anlage_user,
 '1995010100000000' aend_termin, 'FELIX' aend_user, le.lb_nr, le.teile_nr,
 le.aend_idx, le.chargen_nr, 0 frei_menge, 0 verf_menge, 0 res_menge,
 sum(le.frei_menge) + sum(le.res_menge) gesp_menge
 FROM lag_einh le
 WHERE le.sperre = '1' AND
 not exists (SELECT bsv.teile_nr
 FROM bestand_verf bsv
 WHERE bsv.teile_nr = le.teile_nr
 AND bsv.aend_idx = le.aend_idx
 AND bsv.chargen_nr = le.chargen_nr
 AND bsv.lb_nr = le.lb_nr)
 GROUP BY le.teile_nr,
 le.aend_idx,
 le.chargen_nr,
 le.lb_nr !

file sel_view ( BESTAND_KUM !

select * from bestand_kum !

fetch into :a !

CREATE VIEW bestand as
 SELECT  101000 c_id,
   '1995010100000000' anlage_termin,
   'FELIX' anlage_user,
   '1995010100000000' aend_termin,
   'FELIX' aend_user,
   lb_nr,
   teile_nr,
   aend_idx,
   chargen_nr,     
   sum(frei_menge) frei_menge,
   sum(verf_menge)/count(*) verf_menge,
   sum(res_menge) res_menge,
   sum(gesp_menge) gesp_menge
 FROM  bestand_kum
 GROUP BY teile_nr,
   aend_idx,
   chargen_nr,
   lb_nr!
    
GRANT ALL ON bestand TO PUBLIC !


CREATE VIEW V1 as
 SELECT  101000 c_id,
   '1995010100000000' anlage_termin,
   'FELIX' anlage_user,
   '1995010100000000' aend_termin,
   'FELIX' aend_user,
   le.lb_nr,
   le.teile_nr,
   le.aend_idx,
   le.chargen_nr,     
   sum(le.frei_menge) frei_menge,
   sum(bsvf.verf_menge)/count(*) verf_menge,
   sum(le.res_menge) res_menge,
   0 gesp_menge
 FROM  lag_einh le, v_bestand_verf_kumuliert bsvf
 WHERE  le.teile_nr  =  bsvf.teile_nr AND
   le.aend_idx  =  bsvf.aend_idx AND
   le.chargen_nr  =  bsvf.chargen_nr AND
   le.lb_nr  =  bsvf.lb_nr AND
   le.sperre =  '0' 
 GROUP BY le.teile_nr,
   le.aend_idx,
   le.chargen_nr,
   le.lb_nr !
CREATE VIEW V2 as
 SELECT  101000 c_id,
   '1995010100000000' anlage_termin,
   'FELIX' anlage_user,
   '1995010100000000' aend_termin,
   'FELIX' aend_user,
   le.lb_nr,
   le.teile_nr,
   le.aend_idx,
   le.chargen_nr,     
   sum(le.frei_menge) frei_menge,
   0 verf_menge,
   sum(le.res_menge) res_menge,
   0 gesp_menge
 FROM  lag_einh le
 WHERE  le.sperre = '0' AND
   not exists (SELECT bsv.teile_nr
     FROM  bestand_verf bsv
     WHERE bsv.teile_nr = teile_nr  AND 
           bsv.aend_idx = aend_idx  AND
           bsv.chargen_nr = chargen_nr AND
           bsv.lb_nr = lb_nr)
 GROUP BY le.teile_nr,
   le.aend_idx,
   le.chargen_nr,
   le.lb_nr!
CREATE VIEW V3 as
 SELECT  101000  c_id,
   '1995010100000000' anlage_termin,
   'FELIX' anlage_user,
   '1995010100000000' aend_termin,
   'FELIX' aend_user,
   le.lb_nr,
   le.teile_nr,
   le.aend_idx,
   le.chargen_nr,     
   0 frei_menge,
   sum(bsvf.verf_menge)/count(*) verf_menge,
   0 res_menge,
   sum(le.frei_menge) + sum(le.res_menge) gesp_menge
 FROM  lag_einh le, v_bestand_verf_kumuliert bsvf
 WHERE  le.teile_nr = bsvf.teile_nr AND
   le.aend_idx = bsvf.aend_idx AND
   le.chargen_nr  = bsvf.chargen_nr AND
   le.lb_nr  = bsvf.lb_nr AND
   le.sperre =  '1' 
 GROUP BY le.teile_nr,
   le.aend_idx,
   le.chargen_nr,
   le.lb_nr!
CREATE VIEW V4 as
 SELECT  101000  c_id,
   '1995010100000000' anlage_termin,
   'FELIX' anlage_user,
   '1995010100000000' aend_termin,
   'FELIX' aend_user,
   le.lb_nr,
   le.teile_nr,
   le.aend_idx,
   le.chargen_nr,     
   0 frei_menge,
   0 verf_menge,
   0 res_menge,
   sum(le.frei_menge) + sum(le.res_menge) gesp_menge
 FROM  lag_einh le
 WHERE  le.sperre = '1' AND
   not exists (SELECT bsv.teile_nr
     FROM bestand_verf bsv
     WHERE bsv.teile_nr = teile_nr
     AND   bsv.aend_idx = aend_idx
     AND   bsv.chargen_nr = chargen_nr
     AND   bsv.lb_nr = lb_nr)
 GROUP BY le.teile_nr,
   le.aend_idx,
   le.chargen_nr,
   le.lb_nr !
CREATE VIEW V AS
SELECT * FROM V1
UNION
SELECT * FROM V2
UNION
SELECT * FROM V3
UNION
SELECT * FROM V4 !
select * from v !
fetch into :a !
drop TABLE bestand_verf!
drop TABLE lag_einh!
commit work release !
