*   *ID* C00E120  DBCMD    changed on 1992-11-11-17.01.56 by KERN      *
 * interne Fehlernummer 120
 * fuehrte zu einen kb_stacktype_illegal !
**************** cn 27.06.90 !
file cleark !
file connect ( kern test !
CREATE TABLE "RN0014            "
("CO_ID             " CHAR (4) KEY,
 "AST_GRP       " CHAR (3) KEY,
 "BUSI_YR           " FIXED(4,0)        KEY,
 "AST_ID        " CHAR (6) KEY,
 "D_TYP_ID       " CHAR (2) KEY,
 "AST_CLA       " CHAR (1)
 DEFAULT            ' ',
 "UNIT         " FIXED(4,0)
 DEFAULT                0,
 "UNIT_DSPL    " FIXED(4,0)
 DEFAULT                0,
 "UNIT_TRAN    " FIXED(4,0)
 DEFAULT                0,
 "UNIT_ADD     " FIXED(4,0)
 DEFAULT                0,
 "VAL          " FIXED(13,2)
 DEFAULT                       0.00,
 "VAL_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "DEPR         " FIXED(13,2)
 DEFAULT                       0.00,
 "D_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "ADJ          " FIXED(13,2)
 DEFAULT                       0.00,
 "ADJ_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "DSPL         " FIXED(13,2)
 DEFAULT                       0.00,
 "DSPL_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "TRAN         " FIXED(13,2)
 DEFAULT                       0.00,
 "TRAN_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "ADD          " FIXED(13,2)
 DEFAULT                       0.00,
 "ADD_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_BAS_VAL      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_VAL          " FIXED(13,2)
 DEFAULT                       0.00,
 "D_BAS_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_VAL_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_IND          " CHAR (1)
 DEFAULT            ' ',
 "D_ADJ          " FIXED(13,2)
 DEFAULT                       0.00,
 "D_ADJ_HDM      " FIXED(13,2)
 DEFAULT                       0.00,
 "D_DL         " FIXED(13,2)
 DEFAULT                       0.00,
 "D_DL_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "D_TR         " FIXED(13,2)
 DEFAULT                       0.00,
 "D_TR_HDM     " FIXED(13,2)
 DEFAULT                       0.00,
 "D_END_DT       " DATE
 DEFAULT            '19000101',
 "LAST_D_DT      " DATE
 DEFAULT            '19000101',
 "DT_STA_DEPR       " DATE
 DEFAULT            '19000101') !
CREATE INDEX RN0014_X1 ON RN0014
    (CO_ID ASC,
     BUSI_YR ASC,
     AST_GRP ASC,
     D_TYP_ID ASC,
     AST_ID ASC) !
commit work !
select RN0014.AST_id, RN0014.AST_grp, RN0014.val,
b.AST_id, b.AST_grp, b.val, RN0014.val+b.val
from <rn0014   , rn0014 b>
where RN0014.co_id = '507V' and RN0014.busi_yr = 1990
and b.co_id =RN0014.co_id and b.busi_yr = RN0014.busi_yr
and RN0014.AST_grp in
('410','420','430','490','511','520','530','610','620','630')
and b.AST_grp in
('410','420','430','490','511','520','530','610','620','630')
and RN0014.D_typ_id = '01' and   b.D_typ_id = '01'
and RN0014.val < 142499 and RN0014.AST_id <> b.AST_id
and RN0014.dspl = 0  and b.dspl = 0
and (RN0014.val + b.val) > 142499
and (RN0014.val + b.val) < 142501 !
select RN0014.AST_id, RN0014.AST_grp, RN0014.val,
b.AST_id, b.AST_grp, b.val, RN0014.val+b.val
from <rn0014  b, rn0014>
where RN0014.co_id = '507V' and RN0014.busi_yr = 1990
and b.co_id =RN0014.co_id and b.busi_yr = RN0014.busi_yr
and RN0014.AST_grp in
('410','420','430','490','511','520','530','610','620','630')
and b.AST_grp in
('410','420','430','490','511','520','530','610','620','630')
and RN0014.D_typ_id = '01' and   b.D_typ_id = '01'
and RN0014.val < 142499 and RN0014.AST_id <> b.AST_id
and RN0014.dspl = 0  and b.dspl = 0
and (RN0014.val + b.val) > 142499
and (RN0014.val + b.val) < 142501 !
commit   release!
