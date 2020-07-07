/*



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end





*/
#if !defined (__Error_INCLUDED)
#define __Error_INCLUDED

#define e_NEW_FAILED               1
#define e_WRONG_LIST_OID           2
#define e_WRONG_VAR_OID            3
#define e_WRONG_VAR_OBJ_CONTENT    4
#define e_CONS_VIEW_ERRO_OID_FOUND 5
#define e_ABORTED                  6
#define e_INSERTED_KEY_NOT_FOUND   7
#define e_SELF_VERIFY_FAILED       8
#define e_VERIFY_FAILED            9
#define e_WRONG_VAR_OBJECT         10
#define e_WRONG_LIST_SUM           11
#define e_WRONG_SUM                12
#define e_DIFF_ITER_KEY            13
#define e_DIFF_ITER_LIST_COUNT     14
#define e_WRONG_PARAM              15
#define e_SQL_ERROR                16
#define e_NO_VERSION_FOUND         17
#define e_SCHEMA_DROPPED           18
#define e_DROP_IN_PROGRESS         19
#define e_TOO_NEW_OBJECT           20
#define e_CLEANUP_NOT_CALLED       21
#define e_PACKED_ERROR             22
#define e_WRONG_KEY_RANGE_ITER     23
#define e_INVALID_TIMESTAMP        24
#define e_ALLOCATOR_ERROR          25
#define e_ALREADY_IN_SCHEMA_DIR    26
#define e_ALREADY_REGISTERED       27
#define e_NOT_REGISTERED           28
#define e_OBJECT_NOT_LOCKED        29

#define SQL_ERROR_HISTORY_NOT_FOUND(X) ((-28815 == X) /*|| (-28804 == X)*/ || (-28811 == X))

void ThrowError(int e);

void ThrowError(int e, int val1, int val2);

#endif