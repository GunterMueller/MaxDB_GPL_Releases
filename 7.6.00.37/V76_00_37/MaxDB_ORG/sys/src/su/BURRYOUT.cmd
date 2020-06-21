*   *ID* BURRYOUT DBCMD    changed on 1992-09-28-13.22.53 by ADMIN     *
select * from po !
fetch!
*
select * from supplier !
fetch!
*
select su_no, su_name, po_suppl, po_no from po, supplier
where po_suppl = su_no!
fetch!
*
select su_no, su_name, po_suppl, po_no from po, supplier
where po_suppl=su_no(+)!
fetch!
*
select su_no, su_name, po_suppl, po_no from po, supplier
where po_suppl(+)=su_no!
fetch!
*
select su_no, su_name, po_suppl, po_no from po, supplier
where po_suppl(+)=su_no(+)!
fetch!
*
select su_no from supplier
union
select po_suppl from po
order by 1 !
*
fetch!
*
select su_no from supplier
union
select po_suppl from po
except
select su_no from supplier
intersect
select po_suppl from po
order by 1 !
*
fetch!
*
select su_no from supplier
intersect
select po_suppl from po!
*
fetch!
*
select su_no from supplier
except
select po_suppl from po
order by 1 !
*
fetch!
*
select * from manifest,manline
where m_no=ml_manif(+)!
*
fetch!
*
select * from manifest,manline
where m_total=ml_value(+)!
*
fetch!
*
select * from manifest,manline
where m_total=ml_value(+)
 and  m_no=ml_manif(+)!
*
fetch!
*
select * from manifest,manline
where m_total=ml_value(+)
 or   m_no=ml_manif(+)!
*
fetch!
*
select * from manifest,manline,poline
where m_total=ml_value(+)
 and  m_no=ml_manif(+)
 and  ml_value(+)=pl_cost!
*
select m_no,m_total
from manifest
union
select ml_manif,ml_value
from manline
order by 1, 2!
*
fetch!
*
select m_no,m_total
from manifest
except
select ml_manif,ml_value
from manline
order by 1!
*
fetch!
*
select m_no
from manifest
intersect
select ml_manif
from manline
order by 1!
*
fetch!
*
select ml_manif from manline
where ml_value=any
(select ml_value from manline
 where ml_value > 150)
except
select ml_manif from manline intersect
select m_no from manifest
where m_total=any
(select m_total from manifest
 where m_total > 1000)
order by 1 !
*
fetch!
*
select ml_manif from manline
where ml_value > 150
except
select ml_manif from manline intersect
select m_no from manifest
where m_total > 1000
order by 1 !
*
fetch!
*
select ml_manif from manline
where ml_value > 150
union
select ml_manif from manline intersect
select m_no from manifest
where m_total > 1000
order by 1 !
*
fetch!
*
select ml_manif from manline
where ml_value > 150
intersect
select ml_manif from manline intersect
select m_no from manifest
where m_total > 1000
order by 1 !
*
fetch!
*
select su_no,po_suppl from supplier,po
where po_suppl=su_no(+)
union
select su_no,po_suppl from supplier,po
where po_suppl(+)=su_no
order by 1, 2 !
*
fetch!
*
select su_no,po_suppl from supplier,po
where po_suppl=su_no(+)
intersect
select su_no,po_suppl from supplier,po
where po_suppl(+)=su_no
order by 1 !
*
fetch!
*
select su_no,po_suppl from supplier,po
where po_suppl (+)=     su_no
intersect
select su_no,po_suppl from supplier,po
where po_suppl     =su_no(+)
order by 1 !
*
fetch!
*
select su_no,po_suppl from supplier,po
where po_suppl=su_no(+)
except
select su_no,po_suppl from supplier,po
where po_suppl(+)=su_no!
*
select su_no,po_suppl from supplier,po
where po_suppl(+)=su_no
except
select su_no,po_suppl from supplier,po
where po_suppl=su_no(+)
order by 2, 1!
*
fetch!
