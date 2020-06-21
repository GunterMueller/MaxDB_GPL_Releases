# specials.com

# patch ptoc.h on the fly
patch_ptoc_h.pl ascii exec unpack=no concom=no ->$WRK/tools/

# dummyfile ptoc.h file for patching 
ptocdummy.h ascii unpack=no concom=no \
            ->$TOOL/incl/ptoc.h execscript=$WRK/tools/patch_ptoc_h.pl \
            dep=patch_ptoc_h.pl

truncate_ptoc_header.pl ascii concom=no noversion unpack=no exec ->$WRK/tools/ suppress_uncond

ptoc_base_h ->$TOOL/incl/ptoc_base.h ascii concom=no unpack=no noversion <-$TOOL/incl/ptoc.h \
       dep=:specials/ptoc/truncate_ptoc_header.pl execscript=$WRK/tools/truncate_ptoc_header.pl suppress_uncond
