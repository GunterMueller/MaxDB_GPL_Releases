/* @(#)common/opcode.i


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

/* opcode.i */

# define FirstOp	1
# define NopOp		1		/* node */

/* # define StartOp	2		/* spec */

# define LnoOp		3		/* fake */
# define LoadOp		4		/* UnaryExp */

/* # define LoadxOp	5 */

# define StoreOp	6		/* LongStmnt */

/* # define StorexOp	7 */

# define StorepOp	8		/* UnaryExp */
# define StorepadrOp	9		/* UnaryExp */

/* # define LoadxadrOp	10 */

# define LitOp		12		/* node */
# define LoadadrOp	13		/* UnaryExp */
# define LoadstrOp	14		/* node */
# define AddOp		19		/* BinaryExp */
# define SubtOp		20		/* BinaryExp */
# define MultOp		21		/* BinaryExp */
# define DivOp		22		/* BinaryExp */
# define RangeOp	23		/* UnaryExp */
# define UminusOp	24		/* UnaryExp */
# define OrOp		25		/* BinaryExp */
# define XorOp		26		/* BinaryExp */
# define AndOp		27		/* BinaryExp */
# define NotOp		28		/* UnaryExp */
# define ShlOp		29		/* BinaryExp */
# define ShrOp		30		/* BinaryExp */
# define EqOp		31		/* BinaryExp */
# define LtOp		32		/* BinaryExp */
# define GtOp		33		/* BinaryExp */
# define LeOp		34		/* BinaryExp */
# define GeOp		35		/* BinaryExp */
# define NeOp		36		/* BinaryExp */
# define CallOp		40		/* LongStmnt */

/* # define ScallOp	41 */

# define RtrnOp		42		/* spec */
# define RtrnxOp	43		/* LongStmnt */

/* # define SaveOp	45 */
/* # define ExitOp	47 */
/* # define LngthOp	50 */
/* # define ElmntOp	51 */
/* # define AppendOp	52 */
/* # define PrintOp	56 */
/* # define StorefOp	57 */

# define ProcOp		58		/* Proc */

/* # define IntToCharOp	59 */
/* # define CharToIntOp	60 */

# define Sys0Op		62		/* LongStmnt */

/* # define Sys1Op	63 */
/* # define Sys2Op	64 */
/* # define Sys3Op	73 */
/* # define Sys4Op	74 */
/* # define DescOp	65 */

# define LitSizeOp	69		/* node */
# define LitOffsetOp	70		/* node */

/* # define LoadtOp	71 */
/* # define StoretOp	72 */

# define AddiOp		76		/* UnaryExp */
# define SubtiOp	77		/* UnaryExp */
# define AddixOp	82		/* UnaryExp */
# define SubtixOp	83		/* UnaryExp */
# define MultxOp	84		/* UnaryExp */

/* new opcodes */

# define ContinueOp	86		/* spec */
# define GotoOp		87		/* spec */
# define ModOp		88		/* BinaryExp */
# define BAndOp		89		/* BinaryExp */
# define BOrOp		90		/* BinaryExp */
# define BXorOp		91		/* BinaryExp */
# define BNotOp		92		/* UnaryExp */
# define ConvertOp	93		/* UnaryExp */

# define IdOp		94		/* node */
# define SubscriptOp	95		/* BinaryExp */
# define ElementOp	96		/* BinaryExp */
# define DerefOp	97		/* UnaryExp */

# define LabelOp	98		/* spec */

# define CaseOp		99		/* Case */
# define DefaultOp	100		/* Case */

# define BreakOp	101		/* spec */

# define PrefixOp	102		/* LongStmnt */
# define PostfixOp	103		/* LongStmnt */
# define ModuloOp	104		/* BinaryExp */
# define FileOp		105		/* spec */
# define AsmOp		106		/* spec */
# define RepeatOp	107		/* BinaryExp */
# define ComplexOp	108		/* BinaryExp */
# define StaticLinkOp	109		/* node */
# define RegOp       	110		/* node */
# define EntryOp	111		/* spec */
# define StoreadrOp	112		/* LongStmnt */
# define ListOp		113		/* ExpList */
# define CommaOp	114		/* ExpList */
# define PushOp		115		/* spec */
# define PopOp		116		/* spec */
# define PushAdrOp	117		/* spec */
# define PopAdrOp	118		/* spec */
# define BeginOp	119		/* spec */
# define EndOp		120		/* spec */

# define LastOp		120
