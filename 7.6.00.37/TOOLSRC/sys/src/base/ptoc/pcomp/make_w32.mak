#  @(#)pcomp/make_w32.mak		2.0.5  1996-09-03  SQL DBS
#

!include ..\Makedefs.w32

INCL    = -I..\common
INKL    = -I..\include
DEFS    = -DOPTIMIZING -DFLEX -DNO_ENUM
CFLAGS  = $(CCFLAGS) $(DEFS) $(INCL) $(INKL) $(DEBUG) $(OPTIONS)
LFLAGS	= $(LDFLAGS) $(OPTIONS)
PFX	=

!include make_w32.fil


.SUFFIXES :	.y .l .c .h .obj .lib
.y.c :
.l.c :
.c.obj :
	$(CC) $(CFLAGS) -c $<
.c.lib :


pgenc.exe:  Version.obj $(OBJECTS) $(PGRAMOBJ) $(OPTOBJ) ..\libpc\standard\ptocnts.lib
	$(link) $(LFLAGS) Version.obj \
		$(OBJECTS) $(PGRAMOBJ) $(OPTOBJ) \
		..\libpc\standard\ptocnts.lib -out:pgenc.exe user32.lib

install: pgenc.exe
	copy pgenc.exe $(TOOL)\lib\pgenc.exe

clean:
	del pgenc.exe *.obj *.pdb

Version.obj: Version.c
	$(CC) $(CFLAGS) -c Version.c

y.tab.obj: y.tab.c
	$(CC) $(CFLAGS) -c y.tab.c

# END OF MAKEHEAD

_pascl_i.obj: _pascl_i.c
	$(CC) -c $(CFLAGS) _pascl_i.c

atox.obj: ..\pascl\atox.c
	$(CC) -c $(CFLAGS) ..\pascl\atox.c

decls.obj: ..\pascl\decls.c
	$(CC) -c $(CFLAGS) ..\pascl\decls.c

init.obj: init.c
	$(CC) -c $(CFLAGS) init.c

initialize.obj: ..\pascl\initialize.c
	$(CC) -c $(CFLAGS) ..\pascl\initialize.c

p1m2.obj: p1m2.c
	$(CC) -c $(CFLAGS) p1m2.c

pascalalloc.obj: ..\pascl\pascalalloc.c
	$(CC) -c $(CFLAGS) ..\pascl\pascalalloc.c

pcomp.obj: pcomp.c
	$(CC) -c $(CFLAGS) pcomp.c

putilities.obj: ..\pascl\putilities.c
	$(CC) -c $(CFLAGS) ..\pascl\putilities.c

sem.obj: sem.c
	$(CC) -c $(CFLAGS) sem.c

semutil.obj: semutil.c
	$(CC) -c $(CFLAGS) semutil.c

symtab.obj: symtab.c
	$(CC) -c $(CFLAGS) symtab.c

sysprocs.obj: sysprocs.c
	$(CC) -c $(CFLAGS) sysprocs.c

_common_i.obj: _common_i.c
	$(CC) -c $(CFLAGS) _common_i.c

_shifts.obj: ..\common\_shifts.c
	$(CC) -c $(CFLAGS) ..\common\_shifts.c

commonalloc.obj: ..\common\commonalloc.c
	$(CC) -c $(CFLAGS) ..\common\commonalloc.c

csg.obj: ..\common\csg.c
	$(CC) -c $(CFLAGS) ..\common\csg.c

dumpsym.obj: ..\common\dumpsym.c
	$(CC) -c $(CFLAGS) ..\common\dumpsym.c

dumptree.obj: dumptree.c
	$(CC) -c $(CFLAGS) dumptree.c

fold.obj: ..\common\fold.c
	$(CC) -c $(CFLAGS) ..\common\fold.c

stab.obj: ..\common\stab.c
	$(CC) -c $(CFLAGS) ..\common\stab.c

utilities.obj: utilities.c
	$(CC) -c $(CFLAGS) utilities.c

genc.obj: genc.c
	$(CC) -c $(CFLAGS) genc.c

genc_util.obj: ..\common\genc_util.c
	$(CC) -c $(CFLAGS) ..\common\genc_util.c

genc_typ.obj: genc_typ.c
	$(CC) -c $(CFLAGS) genc_typ.c

util.obj: ..\common\util.c
	$(CC) -c $(CFLAGS) ..\common\util.c

dump_hdg.obj: dump_hdg.c
	$(CC) -c $(CFLAGS) dump_hdg.c

_optimizer_i.obj: _optimizer_i.c
	$(CC) -c $(CFLAGS) _optimizer_i.c

kludge2.obj: ..\optimizer\kludge2.c
	$(CC) -c $(CFLAGS) ..\optimizer\kludge2.c

optdriver.obj: optdriver.c
	$(CC) -c $(CFLAGS) optdriver.c

optm1.obj: optm1.c
	$(CC) -c $(CFLAGS) optm1.c

optm15.obj: optm15.c
	$(CC) -c $(CFLAGS) optm15.c

optm2.obj: optm2.c
	$(CC) -c $(CFLAGS) optm2.c

optm3.obj: optm3.c
	$(CC) -c $(CFLAGS) optm3.c

optm5.obj: optm5.c
	$(CC) -c $(CFLAGS) optm5.c

optm6.obj: optm6.c
	$(CC) -c $(CFLAGS) optm6.c

optm7.obj: optm7.c
	$(CC) -c $(CFLAGS) optm7.c

optm8.obj: optm8.c
	$(CC) -c $(CFLAGS) optm8.c

optm9.obj: optm9.c
	$(CC) -c $(CFLAGS) optm9.c

set_util.obj: ..\optimizer\set_util.c
	$(CC) -c $(CFLAGS) ..\optimizer\set_util.c

temp_util.obj: temp_util.c
	$(CC) -c $(CFLAGS) temp_util.c

yycopy.obj: ..\pgram\yycopy.c
	$(CC) -c $(CFLAGS) ..\pgram\yycopy.c

yycosts.obj: ..\pgram\yycosts.c
	$(CC) -c $(CFLAGS) ..\pgram\yycosts.c

yyerror.obj: ..\pgram\yyerror.c
	$(CC) -c $(CFLAGS) ..\pgram\yyerror.c

yyget.obj: ..\pgram\yyget.c
	$(CC) -c $(CFLAGS) ..\pgram\yyget.c

yylex.obj: ..\pgram\yylex.c
	$(CC) -c $(CFLAGS) ..\pgram\yylex.c

yyoptions.obj: ..\pgram\yyoptions.c
	$(CC) -c $(CFLAGS) ..\pgram\yyoptions.c

yypanic.obj: ..\pgram\yypanic.c
	$(CC) -c $(CFLAGS) ..\pgram\yypanic.c

yyparse.obj: ..\pgram\yyparse.c
	$(CC) -c $(CFLAGS) ..\pgram\yyparse.c

yyprint.obj: ..\pgram\yyprint.c
	$(CC) -c $(CFLAGS) ..\pgram\yyprint.c

yyput.obj: ..\pgram\yyput.c
	$(CC) -c $(CFLAGS) ..\pgram\yyput.c

yyrecover.obj: ..\pgram\yyrecover.c
	$(CC) -c $(CFLAGS) ..\pgram\yyrecover.c

yyseman.obj: ..\pgram\yyseman.c
	$(CC) -c $(CFLAGS) ..\pgram\yyseman.c

subr.obj: ..\pgram\subr.c
	$(CC) -c $(CFLAGS) ..\pgram\subr.c

error.obj: ..\pgram\error.c
	$(CC) -c $(CFLAGS) ..\pgram\error.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

y.tab.obj: ..\pgram\0.h ..\libpc\whoami.h
y.tab.obj:
y.tab.obj: ..\common\csg.def ..\common\hdg.def
y.tab.obj: ..\common\globals.ci ..\common\_strings.ch ..\common\symtab.ci
y.tab.obj: ..\common\trees.ci ..\common\csg.i ..\common\opcode.i
y.tab.obj: ..\common\stab.h ..\pascl\pascal_opc.i ..\pascl\globals1.ci
y.tab.obj: ..\pascl\glob1const.ci ..\pascl\glob1type.ci ..\pascl\glob1var.ci
y.tab.obj: ..\pascl\symtab.ch ..\pascl\pascalalloc.h ..\pascl\sflags.def
y.tab.obj: ..\pgram\yy.h ..\pcomp\y.tab.h ..\pgram\f.decls.h
y.tab.obj: ..\common\machdep.h
_pascl_i.obj: ..\common\globals.i ..\common\_strings.h ..\common\symtab.i
_pascl_i.obj: ..\common\trees.i ..\pascl\globals1.i ..\pascl\glob1const.i
_pascl_i.obj: ..\pascl\glob1type.i ..\pascl\glob1var.i ..\pascl\symtab.h
_pascl_i.obj: ..\pascl\sysprocs.h ..\pascl\scanner.h ..\pascl\p1m2.h
_pascl_i.obj: ..\pascl\_pascl_i.h
atox.obj:
decls.obj:
decls.obj: ..\common\globals.ci ..\common\_strings.ch ..\common\symtab.ci
decls.obj: ..\common\trees.ci ..\gram\manifest ..\common\stab.h
decls.obj: ..\libpc\h00vars.h ..\libpc\whoami.h ..\pascl\sflags.def
decls.obj: ..\common\machdep.h
init.obj: ..\common\csg.def ..\common\globals.i ..\common\_strings.h
init.obj: ..\common\symtab.i ..\common\trees.i ..\common\csg.i
init.obj: ..\common\opcode.i ..\common\_shifts.h ..\common\utilities.h
init.obj: ..\pascl\globals1.i ..\pascl\glob1const.i ..\pascl\glob1type.i
init.obj: ..\pascl\glob1var.i ..\pascl\p1m2.h ..\pascl\init.h
initialize.obj:
initialize.obj: ..\common\globals.ci
initialize.obj: ..\common\_strings.ch ..\common\symtab.ci ..\common\trees.ci
initialize.obj: ..\common\stab.h ..\pascl\sflags.def ..\pascl\symtab.ch
initialize.obj: ..\common\opcode.i ..\common\csg.def ..\common\csg.i
initialize.obj: ..\common\hdg.def ..\gram\data.h ..\pascl\globals1.ci
initialize.obj: ..\pascl\glob1const.ci ..\pascl\glob1type.ci
initialize.obj: ..\pascl\glob1var.ci
p1m2.obj: ..\common\globals.i ..\common\_strings.h ..\common\symtab.i
p1m2.obj: ..\common\trees.i ..\common\stab.h ..\pascl\globals1.i
p1m2.obj: ..\pascl\glob1const.i ..\pascl\glob1type.i ..\pascl\glob1var.i
p1m2.obj: ..\pascl\scanner.h ..\pascl\pcexit.h ..\pascl\p1m2.h
pascalalloc.obj: ..\common\stab.h
pcomp.obj: ..\common\csg.def ..\common\globals.i ..\common\_strings.h
pcomp.obj: ..\common\symtab.i ..\common\trees.i ..\common\opcode.i
pcomp.obj: ..\common\_common_i.h ..\common\stab.h ..\common\utilities.h
pcomp.obj: ..\common\dumpsym.h ..\common\dumptree.h ..\pascl\globals1.i
pcomp.obj: ..\pascl\glob1const.i ..\pascl\glob1type.i ..\pascl\glob1var.i
pcomp.obj: ..\pascl\_pascl_i.h ..\pascl\scanner.h ..\pascl\yyparse.h
pcomp.obj: ..\pascl\symtab.h ..\pascl\decls.h ..\pascl\p1m2.h ..\pascl\pcexit.h
pcomp.obj: ..\gram\codeservice.h ..\pcomp\tokens.i ..\common\machdep.h
putilities.obj:
putilities.obj: ..\pgram\0.h ..\libpc\whoami.h
putilities.obj:
sem.obj: ..\common\csg.def ..\common\hdg.def ..\pascl\sflags.def
sem.obj: ..\common\globals.i ..\common\_strings.h ..\common\symtab.i
sem.obj: ..\common\trees.i ..\common\csg.i ..\common\opcode.i
sem.obj: ..\common\_shifts.h ..\common\stab.h ..\common\csg.h ..\common\fold.h
sem.obj: ..\common\utilities.h ..\pascl\globals1.i ..\pascl\glob1const.i
sem.obj: ..\pascl\glob1type.i ..\pascl\glob1var.i ..\pascl\pascal_opc.i
sem.obj: ..\pascl\symtab.h ..\pascl\sysprocs.h ..\pascl\decls.h
sem.obj: ..\pascl\semutil.h ..\pascl\init.h ..\pascl\pascalalloc.h
sem.obj: ..\pascl\p1m2.h ..\pascl\sem.h
semutil.obj: ..\common\csg.def ..\common\hdg.def ..\pascl\sflags.def
semutil.obj: ..\common\globals.i ..\common\_strings.h ..\common\symtab.i
semutil.obj: ..\common\trees.i ..\common\opcode.i ..\common\csg.h
semutil.obj: ..\common\csg.i ..\common\fold.h ..\common\stab.h
semutil.obj: ..\common\utilities.h ..\common\_shifts.h ..\pascl\globals1.i
semutil.obj: ..\pascl\glob1const.i ..\pascl\glob1type.i ..\pascl\glob1var.i
semutil.obj: ..\pascl\symtab.h ..\pascl\p1m2.h ..\pascl\initialize.h
semutil.obj: ..\pascl\semutil.h
symtab.obj: ..\common\machdep.h ..\common\csg.def ..\common\hdg.def
symtab.obj: ..\pascl\sflags.def ..\common\globals.i ..\common\_strings.h
symtab.obj: ..\common\symtab.i ..\common\trees.i ..\common\fold.h
symtab.obj: ..\common\stab.h ..\common\_shifts.h ..\common\utilities.h
symtab.obj: ..\pascl\globals1.i ..\pascl\glob1const.i ..\pascl\glob1type.i
symtab.obj: ..\pascl\glob1var.i ..\pascl\init.h ..\pascl\p1m2.h
symtab.obj: ..\pascl\semutil.h ..\pascl\sysprocs.h ..\pascl\decls.h
symtab.obj: ..\pascl\pascalalloc.h ..\pascl\symtab.h
sysprocs.obj: ..\common\machdep.h ..\common\csg.def ..\common\hdg.def
sysprocs.obj: ..\pascl\sflags.def ..\common\globals.i ..\common\_strings.h
sysprocs.obj: ..\common\symtab.i ..\common\trees.i ..\common\csg.i
sysprocs.obj: ..\common\opcode.i ..\common\_shifts.h ..\common\stab.h
sysprocs.obj: ..\common\csg.h ..\common\utilities.h ..\common\fold.h
sysprocs.obj: ..\pascl\globals1.i ..\pascl\glob1const.i ..\pascl\glob1type.i
sysprocs.obj: ..\pascl\glob1var.i ..\pascl\symtab.h ..\pascl\sem.h
sysprocs.obj: ..\pascl\semutil.h ..\pascl\initialize.h ..\pascl\decls.h
sysprocs.obj: ..\pascl\p1m2.h ..\pascl\pascal_opc.i ..\pascl\sysprocs.h
_common_i.obj: ..\common\globals.i ..\common\_strings.h ..\common\symtab.i
_common_i.obj: ..\common\trees.i ..\common\stab.h ..\common\utilities.h
_common_i.obj: ..\common\_common_i.h
commonalloc.obj: ..\common\globals.ci
commonalloc.obj: ..\common\_strings.ch ..\common\symtab.ci ..\common\trees.ci
commonalloc.obj: ..\common\strtab.h
csg.obj: ..\common\csg.def ..\common\globals.ci ..\common\_strings.ch
csg.obj: ..\common\symtab.ci ..\common\trees.ci ..\common\csg.i
csg.obj: ..\common\opcode.i
dumpsym.obj:
dumpsym.obj: ..\common\stab.h ..\common\symtab.ci
fold.obj: ..\common\globals.ci ..\common\_strings.ch ..\common\symtab.ci
fold.obj: ..\common\trees.ci ..\common\csg.def ..\common\hdg.def
fold.obj: ..\common\csg.i ..\common\opcode.i
stab.obj: ..\common\stab.h
utilities.obj: ..\common\csg.def ..\common\globals.i ..\common\_strings.h
utilities.obj: ..\common\symtab.i ..\common\trees.i ..\common\csg.i
utilities.obj: ..\common\opcode.i ..\common\_shifts.h ..\common\csg.h
utilities.obj: ..\common\utilities.h
genc.obj: ..\common\csg.def ..\common\hdg.def ..\common\globals.i
genc.obj: ..\common\_strings.h ..\common\symtab.i ..\common\trees.i
genc.obj: ..\common\csg.i ..\common\opcode.i ..\common\strtab.h
genc.obj: ..\common\utilities.h ..\common\_shifts.h ..\common\dumptree.h
genc_util.obj:
genc_util.obj: ..\common\opcode.i
genc_util.obj: ..\common\globals.ci ..\common\_strings.ch ..\common\symtab.ci
genc_util.obj: ..\common\trees.ci ..\common\stab.h ..\libpc\h00vars.h
genc_util.obj: ..\libpc\whoami.h
genc_typ.obj: ..\common\csg.def ..\common\globals.i ..\common\_strings.h
genc_typ.obj: ..\common\symtab.i ..\common\trees.i ..\common\csg.i
genc_typ.obj: ..\common\opcode.i ..\common\strtab.h ..\common\utilities.h
genc_typ.obj: ..\common\_shifts.h ..\common\dumptree.h
util.obj:
util.obj:
dump_hdg.obj: ..\common\csg.def ..\common\globals.i ..\common\_strings.h
dump_hdg.obj: ..\common\symtab.i ..\common\trees.i ..\common\csg.i
dump_hdg.obj: ..\common\opcode.i ..\common\strtab.h ..\common\utilities.h
dump_hdg.obj: ..\common\_shifts.h ..\common\dumptree.h
_optimizer_i.obj: ..\common\globals.i ..\common\_strings.h ..\common\symtab.i
_optimizer_i.obj: ..\common\trees.i ..\optimizer\gl_decls.i ..\common\opcode.i
_optimizer_i.obj: ..\optimizer\gl_consts.i ..\optimizer\gl_types.i
_optimizer_i.obj: ..\optimizer\gl_vars.i ..\optimizer\optdriver.h
_optimizer_i.obj: ..\optimizer\optm2.h ..\optimizer\_optimizer_i.h
optdriver.obj: ..\common\csg.def ..\common\globals.i ..\common\_strings.h
optdriver.obj: ..\common\symtab.i ..\common\trees.i ..\common\csg.i
optdriver.obj: ..\common\_shifts.h ..\common\utilities.h ..\common\dumptree.h
optdriver.obj: ..\optimizer\gl_decls.i ..\common\opcode.i
optdriver.obj: ..\optimizer\gl_consts.i ..\optimizer\gl_types.i
optdriver.obj: ..\optimizer\gl_vars.i ..\optimizer\set_util.h
optdriver.obj: ..\optimizer\temp_util.h ..\optimizer\alloc_comdef.i
optdriver.obj: ..\optimizer\optm1.h ..\optimizer\optm9.h
optdriver.obj: ..\optimizer\optdriver.h
optm1.obj: ..\common\csg.def ..\common\hdg.def ..\common\globals.i
optm1.obj: ..\common\_strings.h ..\common\symtab.i ..\common\trees.i
optm1.obj: ..\optimizer\gl_decls.i ..\common\opcode.i ..\optimizer\gl_consts.i
optm1.obj: ..\optimizer\gl_types.i ..\optimizer\gl_vars.i ..\common\csg.i
optm1.obj: ..\common\csg.h ..\common\_shifts.h ..\common\utilities.h
optm1.obj: ..\optimizer\set_util.h ..\optimizer\temp_util.h
optm1.obj: ..\optimizer\optm1.h
optm15.obj: ..\common\csg.def ..\optimizer\more_csg.def
optm15.obj: ..\optimizer\opt_symtab.def ..\common\globals.i
optm15.obj: ..\common\_strings.h ..\common\symtab.i ..\common\trees.i
optm15.obj: ..\common\csg.i ..\common\_shifts.h ..\common\csg.h
optm15.obj: ..\common\utilities.h ..\optimizer\gl_decls.i ..\common\opcode.i
optm15.obj: ..\optimizer\gl_consts.i ..\optimizer\gl_types.i
optm15.obj: ..\optimizer\gl_vars.i ..\optimizer\optm1.h ..\optimizer\optm2.h
optm15.obj: ..\optimizer\optm5.h ..\optimizer\optm12.h ..\optimizer\set_util.h
optm15.obj: ..\optimizer\temp_util.h ..\optimizer\optm15.h
optm2.obj: ..\common\csg.def ..\optimizer\more_csg.def ..\common\globals.i
optm2.obj: ..\common\_strings.h ..\common\symtab.i ..\common\trees.i
optm2.obj: ..\common\csg.i ..\common\_shifts.h ..\common\utilities.h
optm2.obj: ..\optimizer\gl_decls.i ..\common\opcode.i ..\optimizer\gl_consts.i
optm2.obj: ..\optimizer\gl_types.i ..\optimizer\gl_vars.i ..\optimizer\optm1.h
optm2.obj: ..\optimizer\set_util.h ..\optimizer\optm2.h
optm3.obj: ..\common\csg.def ..\optimizer\more_csg.def ..\common\globals.i
optm3.obj: ..\common\_strings.h ..\common\symtab.i ..\common\trees.i
optm3.obj: ..\common\csg.i ..\common\_shifts.h ..\optimizer\gl_decls.i
optm3.obj: ..\common\opcode.i ..\optimizer\gl_consts.i ..\optimizer\gl_types.i
optm3.obj: ..\optimizer\gl_vars.i ..\optimizer\optm1.h ..\optimizer\optm3.h
optm5.obj: ..\common\csg.def ..\optimizer\more_csg.def ..\common\globals.i
optm5.obj: ..\common\_strings.h ..\common\symtab.i ..\common\trees.i
optm5.obj: ..\common\csg.i ..\common\_shifts.h ..\common\utilities.h
optm5.obj: ..\optimizer\gl_decls.i ..\common\opcode.i ..\optimizer\gl_consts.i
optm5.obj: ..\optimizer\gl_types.i ..\optimizer\gl_vars.i ..\optimizer\optm1.h
optm5.obj: ..\optimizer\optm2.h ..\optimizer\optm3.h ..\optimizer\set_util.h
optm5.obj: ..\optimizer\optm5.h
optm6.obj: ..\common\csg.def ..\common\hdg.def ..\optimizer\more_csg.def
optm6.obj: ..\common\globals.i ..\common\_strings.h ..\common\symtab.i
optm6.obj: ..\common\trees.i ..\common\csg.i ..\common\csg.h
optm6.obj: ..\common\_shifts.h ..\common\utilities.h ..\optimizer\gl_decls.i
optm6.obj: ..\common\opcode.i ..\optimizer\gl_consts.i ..\optimizer\gl_types.i
optm6.obj: ..\optimizer\gl_vars.i ..\optimizer\optm1.h ..\optimizer\optm2.h
optm6.obj: ..\optimizer\optm5.h ..\optimizer\set_util.h
optm6.obj: ..\optimizer\temp_util.h ..\optimizer\optm6.h
optm7.obj: ..\common\csg.def ..\common\globals.i ..\common\_strings.h
optm7.obj: ..\common\symtab.i ..\common\trees.i ..\common\csg.i
optm7.obj: ..\common\_shifts.h ..\optimizer\gl_decls.i ..\common\opcode.i
optm7.obj: ..\optimizer\gl_consts.i ..\optimizer\gl_types.i
optm7.obj: ..\optimizer\gl_vars.i ..\common\utilities.h ..\optimizer\optm7.h
optm8.obj: ..\common\csg.def ..\common\globals.i ..\common\_strings.h
optm8.obj: ..\common\symtab.i ..\common\trees.i ..\common\csg.i ..\common\csg.h
optm8.obj: ..\common\_shifts.h ..\optimizer\gl_decls.i ..\common\opcode.i
optm8.obj: ..\optimizer\gl_consts.i ..\optimizer\gl_types.i
optm8.obj: ..\optimizer\gl_vars.i ..\optimizer\optm5.h ..\optimizer\optm8.h
optm9.obj: ..\common\csg.def ..\optimizer\more_csg.def ..\common\globals.i
optm9.obj: ..\common\_strings.h ..\common\symtab.i ..\common\trees.i
optm9.obj: ..\common\csg.i ..\common\fold.h ..\common\_shifts.h
optm9.obj: ..\optimizer\gl_decls.i ..\common\opcode.i ..\optimizer\gl_consts.i
optm9.obj: ..\optimizer\gl_types.i ..\optimizer\gl_vars.i ..\optimizer\optm1.h
optm9.obj: ..\optimizer\optm2.h ..\optimizer\optm6.h ..\optimizer\optm7.h
optm9.obj: ..\optimizer\optm8.h ..\optimizer\optm11.h ..\optimizer\optm12.h
optm9.obj: ..\optimizer\optm15.h ..\optimizer\temp_util.h ..\optimizer\optm9.h
set_util.obj: ..\common\symtab.ci ..\common\_strings.ch
temp_util.obj: ..\common\globals.i ..\common\_strings.h ..\common\symtab.i
temp_util.obj: ..\common\trees.i ..\common\utilities.h ..\common\csg.i
temp_util.obj: ..\optimizer\gl_decls.i ..\common\opcode.i
temp_util.obj: ..\optimizer\gl_consts.i ..\optimizer\gl_types.i
temp_util.obj: ..\optimizer\gl_vars.i ..\optimizer\set_util.h
temp_util.obj: ..\optimizer\alloc_comdef.i ..\optimizer\temp_util.h
yycopy.obj: ..\pgram\0.h ..\libpc\whoami.h
yycopy.obj:
yycopy.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yycosts.obj: ..\pgram\0.h ..\libpc\whoami.h
yycosts.obj:
yycosts.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yyerror.obj: ..\pgram\0.h ..\libpc\whoami.h
yyerror.obj:
yyerror.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yyerror.obj: ..\common\globals.ci ..\common\_strings.ch ..\common\symtab.ci
yyerror.obj: ..\common\trees.ci
yyget.obj: ..\pgram\0.h ..\libpc\whoami.h
yyget.obj:
yyget.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yyget.obj: ..\common\globals.ci ..\common\_strings.ch ..\common\symtab.ci
yyget.obj: ..\common\trees.ci ..\common\opcode.i ..\pascl\globals1.ci
yyget.obj: ..\pascl\glob1const.ci ..\pascl\glob1type.ci ..\pascl\glob1var.ci
yyget.obj: ..\common\stab.h
yylex.obj: ..\pgram\0.h ..\libpc\whoami.h
yylex.obj:
yylex.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yylex.obj: ..\pcomp\tokenstrings.c ..\common\opcode.i ..\common\globals.ci
yylex.obj: ..\common\_strings.ch ..\common\symtab.ci ..\common\trees.ci
yylex.obj: ..\common\stab.h ..\pascl\globals1.ci ..\pascl\glob1const.ci
yylex.obj: ..\pascl\glob1type.ci ..\pascl\glob1var.ci ..\pascl\pascalalloc.h
yyoptions.obj: ..\pgram\0.h ..\libpc\whoami.h
yyoptions.obj:
yyoptions.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yypanic.obj: ..\pgram\0.h ..\libpc\whoami.h
yypanic.obj:
yypanic.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yyparse.obj: ..\pgram\0.h ..\libpc\whoami.h
yyparse.obj:
yyparse.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yyparse.obj: ..\common\globals.ci ..\common\_strings.ch ..\common\symtab.ci
yyparse.obj: ..\common\trees.ci ..\pascl\globals1.ci ..\pascl\glob1const.ci
yyparse.obj: ..\pascl\glob1type.ci ..\pascl\glob1var.ci
yyprint.obj: ..\pgram\0.h ..\libpc\whoami.h
yyprint.obj:
yyprint.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yyprint.obj: ..\common\stab.h
yyput.obj: ..\pgram\0.h ..\libpc\whoami.h
yyput.obj:
yyput.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yyrecover.obj: ..\pgram\0.h ..\libpc\whoami.h
yyrecover.obj:
yyrecover.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
yyseman.obj: ..\pgram\0.h ..\libpc\whoami.h
yyseman.obj:
yyseman.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
subr.obj: ..\pgram\0.h ..\libpc\whoami.h
subr.obj:
subr.obj:
error.obj: ..\pgram\0.h ..\libpc\whoami.h
error.obj:
error.obj: ..\pgram\yy.h ..\pcomp\y.tab.h
error.obj: ..\common\globals.ci ..\common\_strings.ch ..\common\symtab.ci
error.obj: ..\common\trees.ci
