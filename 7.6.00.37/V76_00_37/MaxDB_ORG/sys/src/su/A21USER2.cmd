*   *ID* A21USER2 DBCMD    changed on 1992-09-28-13.21.28 by ADMIN     *
file connect ( sut sut !
drop user u1 !
data !
create user u2 resource !
p 'PWU2'

nodata !
file sel_user1 ( U2 !
commit release !
*
file connect ( u2 pwu2 !
alter password pwu2 to apwu2 !
commit release !
*
file connect ( u2 apwu2 !
commit release !
*
file connect ( sut sut !
data!
alter password u2 !
p 'PWU2'

nodata !
commit release !
*
file connect ( u2 pwu2 !
alter password pwu2 to apwu2 !
commit release !
*
file connect ( u2 apwu2 !
data !
alter password !
p 'APWU2'
p 'PWU2'

nodata !
commit release !
*
file connect ( u2 pwu2 !
alter password pwu2 to apwu2 !
commit release !
*
file connect ( sut sut !
drop user u2 !
commit release !
*
