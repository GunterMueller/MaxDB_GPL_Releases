*   *ID* A13REC1  DBCMD    changed on 1992-09-28-13.22.34 by ADMIN     *
 ***************************
 ***   A 1 3 R E C 1     ***
 ***************************!
file connect ( kern test !
commit work   !
CREATE TABLE MAXrec1     (
key1   char (2) ascii key,
key2   char (2) ascii key,
key3   char (2) ascii key,
key4   char (2) ascii key,
key5   char (2) ascii key,
key6   char (2) ascii key,
key7   char (2) ascii key,
key8   char (2) ascii key,
key9   char (2) ascii key,
key10  char (2) ascii key,
g1field1  float (5),
g1field2  float (5),
g1field3  float (5),
g1field4  float (5),
g1field5  float (5),
g1field6  float (5),
g1field7  float (5),
g1field8  float (5),
g1field9  float (5),
g1field10 float (5),
g2field1  char (5) ascii,
g2field2  char (5) ascii,
g2field3  char (5) ascii,
g2field4  char (5) ascii,
g2field5  char (5) ascii,
g2field6  char (5) ascii,
g2field7  char (5) ascii,
g2field8  char (5) ascii,
g2field9  char (5) ascii,
g2field10 char (5) ascii,
txt1   char (20) ascii,
txt2   char (20) ascii,
txt3   char (20) ascii,
txt4   char (20) ascii,
txt5   char (20) ascii,
txt6   char (20) ascii,
txt7   char (20) ascii,
txt8   char (20) ascii,
txt9   char (20) ascii,
txt10  char (20) ascii,
num1   float (18),
num2   float (18),
num3   float (18),
num4   float (18),
num5   float (18),
num6   float (18),
num7   float (18),
num8   float (18),
num9   float (18),
num10  float (18),
f1  fixed (3),
f2  fixed (3),
f3  fixed (3),
f4  fixed (3),
f5  fixed (3),
f6  fixed (3),
f7  fixed (3),
f8  fixed (3),
f9  fixed (3),
f10 fixed (3))!
commit work  !
ALTER  TABLE maxrec1  ADD(
n2g1f1  float (5),
n2g1f2  float (5),
n2g1f3  float (5),
n2g1f4  float (5),
n2g1f5  float (5),
n2g1f6  float (5),
n2g1f7  float (5),
n2g1f8  float (5),
n2g1f9  float (5),
n2g1f10 float (5),
n2g2f1  char (5) ascii,
n2g2f2  char (5) ascii,
n2g2f3  char (5) ascii,
n2g2f4  char (5) ascii,
n2g2f5  char (5) ascii,
n2g2f6  char (5) ascii,
n2g2f7  char (5) ascii,
n2g2f8  char (5) ascii,
n2g2f9  char (5) ascii,
n2g2f10 char (5) ascii,
n2txt1   char (2) ascii,
n2txt2   char (2) ascii,
n2txt3   char (2) ascii,
n2txt4   char (2) ascii,
n2txt5   char (2) ascii,
n2txt6   char (2) ascii,
n2txt7   char (2) ascii,
n2txt8   char (2) ascii,
n2txt9   char (2) ascii,
n2txt10  char (2) ascii,
n2num1   float (8),
n2num2   float (8),
n2num3   float (8),
n2num4   float (8),
n2num5   float (8),
n2num6   float (8),
n2num7   float (8),
n2num8   float (8),
n2num9   float (8),
n2num10  float (8),
n2f1  fixed (3),
n2f2  fixed (3),
n2f3  fixed (3),
n2f4  fixed (3),
n2f5  fixed (3),
n2f6  fixed (3),
n2f7  fixed (3),
n2f8  fixed (3),
n2f9  fixed (3),
n2f10 fixed (3))!
commit work  !
ALTER  TABLE maxrec1  ADD(
a110g1f1  float (5),
a110g1f2  float (5),
a110g1f3  float (5),
a110g1f4  float (5),
a110g1f5  float (5),
a110g1f6  float (5),
a110g1f7  float (5),
a110g1f8  float (5),
a110g1f9  float (5),
a110g1f10 float (5),
a110g2f1  char (5) ascii,
a110g2f2  char (5) ascii,
a110g2f3  char (5) ascii,
a110g2f4  char (5) ascii,
a110g2f5  char (5) ascii,
a110g2f6  char (5) ascii,
a110g2f7  char (5) ascii,
a110g2f8  char (5) ascii,
a110g2f9  char (5) ascii,
a110g2f10 char (5) ascii,
a110txt1   char (4) ascii,
a110txt2   char (4) ascii,
a110txt3   char (4) ascii,
a110txt4   char (4) ascii,
a110txt5   char (4) ascii,
a110txt6   char (4) ascii,
a110txt7   char (4) ascii,
a110txt8   char (4) ascii,
a110txt9   char (4) ascii,
a110txt10  char (4) ascii,
a110num1   float (4),
a110num2   float (4),
a110num3   float (4),
a110num4   float (4),
a110num5   float (4),
a110num6   float (4),
a110num7   float (4),
a110num8   float (4),
a110num9   float (4),
a110num10  float (4),
a110f1  fixed (3),
a110f2  fixed (3),
a110f3  fixed (3),
a110f4  fixed (3),
a110f5  fixed (3),
a110f6  fixed (3),
a110f7  fixed (3),
a110f8  fixed (3),
a110f9  fixed (3),
a110f10 fixed (3))!
commit work  !
ALTER  TABLE maxrec1  ADD(
a160g1f1  float (5),
a160g1f2  float (5),
a160g1f3  float (5),
a160g1f4  float (5),
a160g1f5  float (5),
a160g1f6  float (5),
a160g1f7  float (5),
a160g1f8  float (5),
a160g1f9  float (5),
a160g1f10 float (5),
a160g2f1  char (5) ascii,
a160g2f2  char (5) ascii,
a160g2f3  char (5) ascii,
a160g2f4  char (5) ascii,
a160g2f5  char (5) ascii,
a160g2f6  char (5) ascii,
a160g2f7  char (5) ascii,
a160g2f8  char (5) ascii,
a160g2f9  char (5) ascii,
a160g2f10 char (5) ascii,
a160txt1   char (4) ascii,
a160txt2   char (4) ascii,
a160txt3   char (4) ascii,
a160txt4   char (4) ascii,
a160txt5   char (4) ascii,
a160txt6   char (4) ascii,
a160txt7   char (4) ascii,
a160txt8   char (4) ascii,
a160txt9   char (4) ascii,
a160txt10  char (4) ascii,
a160num1   float (8),
a160num2   float (8),
a160num3   float (8),
a160num4   float (8),
a160num5   float (8),
a160num6   float (8),
a160num7   float (8),
a160num8   float (8),
a160num9   float (8),
a160num10  float (8),
a160f1  fixed (3),
a160f2  fixed (3),
a160f3  fixed (3),
a160f4  fixed (3),
a160f5  fixed (3),
a160f6  fixed (3),
a160f7  fixed (3),
a160f8  fixed (3),
a160f9  fixed (3),
a160f10 fixed (3))!
commit work  !
ALTER  TABLE maxrec1  ADD(
b210g1f1  float (5),
b210g1f2  float (5),
b210g1f3  float (5),
b210g1f4  float (5),
b210g1f5  float (5),
b210g1f6  float (5),
b210g1f7  float (5),
b210g1f8  float (5),
b210g1f9  float (5),
b210g1f10 float (5),
b210g2f1  char (5) ascii,
b210g2f2  char (5) ascii,
b210g2f3  char (5) ascii,
b210g2f4  char (5) ascii,
b210g2f5  char (5) ascii,
b210g2f6  char (5) ascii,
b210g2f7  char (5) ascii,
b210g2f8  char (5) ascii,
b210g2f9  char (5) ascii,
b210g2f10 char (5) ascii,
b210txt1   char (2) ascii,
b210txt2   char (2) ascii,
b210txt3   char (2) ascii,
b210txt4   char (2) ascii,
b210txt5   char (2) ascii,
b210txt6   char (2) ascii,
b210txt7   char (2) ascii,
b210txt8   char (2) ascii,
b210txt9   char (2) ascii,
b210txt10  char (2) ascii,
b210num1   float (18),
b210num2   float (18),
b210num3   float (18),
b210num4   float (18),
b210num5   float (18),
b210num6   float (18),
b210num7   float (18),
b210num8   float (18),
b210num9   float (18),
b210num10  float (18),
b210f1  char (57) ascii,
b210f2  char (57) ascii,
b210f3  char (57) ascii,
b210f4  char (57) ascii)!
commit work  !
commit work release!
