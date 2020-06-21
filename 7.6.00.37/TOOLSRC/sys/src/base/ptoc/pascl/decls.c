/* @(#)pascl/decls.c

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

 */

/* decls.c */

/* globals */

#include <stdio.h>
#include <string.h>
#include "../common/globals.ci"
#include "../gram/manifest"
#include "../common/stab.h"
#include "../libpc/h00vars.h"
#include "../pascl/sflags.def"
#include "../common/machdep.h"

# define type_size(x)	STab [x].TypeInfo.SSize

extern int asm_esc;


TypeSize (index)
	int index;
{
	int tsize;
	int tdim;
	int talign;
	int offset;
	SYMTAB *p;
	SYMTAB *q;
	SYMTAB *r;
	int qflag = control ['q'];
	int nflag = control ['n'];

	p = &STab [index];
	if (p->TypeInfo.SSize == -1) {
		switch (p->SType) {
			case ShortType:
				p->TypeInfo.SAlign = ShortAlignment;
				p->TypeInfo.SSize = ShortSize;
				break;
			case UshortType:
				p->TypeInfo.SAlign = UshortAlignment;
				p->TypeInfo.SSize = UshortSize;
				break;
			case LongType:
				p->TypeInfo.SAlign = LongAlignment;
				p->TypeInfo.SSize = LongSize;
				break;
			case UlongType:
				p->TypeInfo.SAlign = UlongAlignment;
				p->TypeInfo.SSize = UlongSize;
				break;
			case IntegerType:
				p->TypeInfo.SAlign = IntegerAlignment;
				p->TypeInfo.SSize = IntegerSize;
				break;
			case UnsignedType:
				p->TypeInfo.SAlign = UnsignedAlignment;
				p->TypeInfo.SSize = UnsignedSize;
				break;
			case BooleanType:
				if (qflag) {
					p->TypeInfo.SAlign = CharAlignment;
					p->TypeInfo.SSize = CharSize;
				}
				else {
					p->TypeInfo.SAlign = IntegerAlignment;
					p->TypeInfo.SSize = IntegerSize;
				}
				break;
			case CharType:
				p->TypeInfo.SAlign = CharAlignment;
				p->TypeInfo.SSize = CharSize;
				break;
			case UcharType:
				p->TypeInfo.SAlign = UcharAlignment;
				p->TypeInfo.SSize = UcharSize;
				break;
			case CharacterType:
				p->TypeInfo.SAlign = CharacterAlignment;
				p->TypeInfo.SSize = CharacterSize;
				break;
			case EnumerativeType:
				if (qflag) {
					if (p->SHighRange < 0x100) {
						p->TypeInfo.SAlign = CharAlignment;
						p->TypeInfo.SSize = CharSize;
					}
					else if (p->SHighRange < 0x10000) {
						p->TypeInfo.SAlign = ShortAlignment;
						p->TypeInfo.SSize = ShortSize;
					}
					else {
						p->TypeInfo.SAlign = IntegerAlignment;
						p->TypeInfo.SSize = IntegerSize;
					}
				}
				else {
					p->TypeInfo.SAlign = IntegerAlignment;
					p->TypeInfo.SSize = IntegerSize;
				}
				break;
			case FloatType:
				p->TypeInfo.SAlign = FloatAlignment;
				p->TypeInfo.SSize = FloatSize;
				break;
			case DoubleType:
				p->TypeInfo.SAlign = /* DoubleAlignment */ IntegerAlignment;
				p->TypeInfo.SSize = DoubleSize;
				break;
			case PointerType:
				p->TypeInfo.SAlign = PointerAlignment;
				p->TypeInfo.SSize = PointerSize;
				break;
			case ArrayType:
				q = &STab [p->SIndex];
				r = &STab [p->SElement];
				talign = r->TypeInfo.SAlign;
				p->TypeInfo.SAlign = talign;
				tsize = type_size (p->SElement);
				SETOFF (tsize, talign);
				tdim = q->SHighRange - q->SLowRange + 1;
				if ((tdim > 0) && (MAXSIZE / tsize) > tdim) {
					tsize = tdim * tsize;
					p->TypeInfo.SSize = tsize;
				}
				else {
					if (q->SHighRange == -1 &&
					q->SLowRange == 0) {
						p->TypeInfo.SSize = 0;
					} else {
/*  						cerror ("size too big");  */
						uerror ("size too big");
						p->TypeInfo.SSize = 0;
					}
				}
				break;
			case BlockType:
				p->TypeInfo.SAlign = p->SElemAlign;
				p->TypeInfo.SSize = p->SBlockLen * UcharSize;
				break;
			case StringType:
				p->TypeInfo.SAlign = CharacterAlignment;
				p->TypeInfo.SSize = p->SLength * CharacterSize;
				break;
			case SetType:
				q = &STab [p->SBase];
				tsize = q->SHighRange - q->SLowRange + 1;
				if (nflag) {
					talign = (tsize <= CharSize) ?
						CharAlignment : ShortAlignment;
				}
				else {
#ifdef PYRAMID_DOUBTFUL
					talign = IntegerAlignment;
#else
					talign = IntegerSize;
#endif
				}
				p->TypeInfo.SAlign = talign;
				SETOFF (tsize, talign);
				if (MAXSIZE > tsize) {
					p->TypeInfo.SSize = tsize;
				}
				else {
					cerror ("size too big");
				}
				break;
			case RecordType:
				offset = define_fields (
					p->SFirstField,
					0,
					&talign);
				if (p->SFirstVariant != -1) {
					offset = define_variant (
						p->SFirstVariant,
						offset);
					talign = RecordAlignment;
				}
				else {
					talign = qflag ?
						talign : RecordAlignment;
				}
				if (!nflag) {
					SETOFF (offset, talign);
				}
				p->TypeInfo.SAlign = talign;
				p->TypeInfo.SSize = offset;
				break;
			case FileType:
				q = &STab [p->SComponent];
				p->TypeInfo.SAlign = FileAlignment;
				tsize = (sizeof (struct iorechd) + BUFSIZ) * 8 +
					type_size (p->SComponent);
				SETOFF (tsize, FileAlignment);
				p->TypeInfo.SSize = tsize;
				break;
			default:
				q = &STab [p->SType];
				p->TypeInfo.SAlign = q->TypeInfo.SAlign;
				p->TypeInfo.SSize = type_size (p->SType);
				break;
		}
	}
 	return(p->TypeInfo.SSize);			/* DK 89/11/08 */
}


int
define_fields (field_list, base_offset, max_align)
	int	field_list;
	int	base_offset;
	int	*max_align;
{
	int	i;
	int	tsize;
	int	offset;
	int	talign;
	int	alignment;
	SYMTAB	*p;
	SYMTAB	*q;

	i = field_list;
	offset = base_offset;
	alignment = CharAlignment;
	while (i != -1) {
		p = &STab [i];
		q = &STab [p->SType];
		tsize = type_size (p->SType);
		talign = p->TypeInfo.SAlign = q->TypeInfo.SAlign;
		SETOFF (offset, talign);
		p->ElementInfo.SSize = tsize;
		p->ElementInfo.SOffset = offset;
		if ((MAXSIZE - offset) > tsize) {
			offset = offset + tsize;
		}
		else {
			cerror ("size too big");
		}
		if (talign > alignment) {
			alignment = talign;
		}
		i = p->SNextField;
	}
	*max_align = alignment;
	return offset;
}


int
define_variant (variant, base_offset)
	int	variant;
	int	base_offset;
{
	int	i;
	int	max_offset;
	int	max_alignment;
	int	offset;
	SYMTAB	*p;

	max_offset = base_offset;
	SETOFF (base_offset, RecordAlignment);
	i = variant;
	while ( i != -1) {
		p = &STab [i];
		offset = define_fields (p->SFirstField, base_offset, &max_alignment);
		offset = define_variant (p->SFirstVariant, offset);
		if (offset > max_offset) {
			max_offset = offset;
		}
		i = STab [i].SNextVariant;
	}
	return max_offset;
}


int eval_name (syminx)
	int syminx;
{
	STRING buf;
	char *p;
	int index;
	int name;

	p = buf;
#ifdef PYRAMID_DOUBTFUL
	*p++ = '_';
#endif
	name = STab [syminx].SName;
	GetString (name, p);
	p += GetStringLength (name);
        index = STab [syminx].SParent;
	while (index != -1) {
		*p++ = BUILDNAME_CHAR;				/* DK 26.08.88 */
		name = STab [index].SName;
		GetString (name, p);
		p += GetStringLength (name);
        	index = STab [index].SParent;
	};
/*	EF4: 01-11-86 WB
 *      if (control ['s'] && !GetFlag (syminx, ExternalFlag)) {
 *                      *p++ = '.';
 *      }
 */
	return EnterString (buf, p - buf);
}


int eval_lab (lab)
	int lab;
{
	STRING buf;
	sprintf (buf, GLOBALLAB_NAME, lab);			/* DK 26.08.88 */	
	return EnterString (buf, strlen (buf));
}

