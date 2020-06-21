file connect ( kern test !
insert p1 (k , f1, f2, n1, n2) values ('aaa', 'bbb', 'ccc', 4, 5)!
insert p1 (k1, f1, f2, n1, n3) values ('aaa', 'bbb', 'ccc', 4, 5)!
insert p1 set k  = 'aaa', f1 = 'bbb'!
insert p1 set k1 = 'aaa', f3 = 'bbb'!
insert p1 (k1, f1, f2, n1, n2) values ('aaa', 'bbb', 'ccc', 4, 5, 6)!
insert p1 (k1, f1, f2, n1, n2) values ('aaa', 'bbb', 'ccc', 4)!
insert p1 (k1, f1, f1, n1, n2) values ('aaa', 'bbb', 'ccc', 4, 5)!
insert p1 set k1 = 'aaa', f1 = 'bbb', f2 = 'ccc'!
rollback release !
