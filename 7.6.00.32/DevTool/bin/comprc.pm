#!/usr/bin/perl
# comprc.pm
#
# @(#)comprc      2002-02-01
#
# Compile different resource files
#
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#

package comprc;
use Env;
use File::Basename;
use File::Copy;
use ICopy;
use ToolLib;
use comprc;
use Exporter;

@ISA = ('Exporter');
@EXPORT = ('comprc','idl2Ispc','GenerateBuildNr');


sub comprc
{
   do "$TOOLVARS";
   if ( $@ ) { print "$@"; exit }

   ToolTrace("comprc called with @ARGV\n");
   $INSPCODE="NONE";
   $OPTDEBUG=0;
   $NOCOMPILE=0;
   $MULTI_OFFSET = 0;
   $IDL2ISPC=0;
   $COPY_ISPC_FILES = 0;
   $PREPROC = 0;
   $RC2RES = 0;
   @Copies=();
   @Headers=();
   @XMLFiles = ();
   @INCLUDES=();
   @OPTADD=();
   @COMPCFLAGS=();
   @OPTSUB=();
   @CFLAGS=();
   $NOVALIDATE = 0;
   $XML2ISPC = 0;
   $NOTDC = 0;
   $XML2DISCO=0; # PTS 1141199
   $M76=0;       # PTS 1141199
   $OSC=0;       # PTS 1141457
   $OID4BA=0;    # PTS 1141457
   $LCA=0;       # PTS 1141457

   # comprc wird von vmake aufgerufen (neue Variante)!

   $USAGE = "usage: comprc resource\n";

   if ( $OS eq "WIN32" ) {
       $midl = "midl";
   }
   else {
       $midl = "";
      $MACHINE = "";
   }

   while ($_ = $ARGV[0], /^-/) {
      shift @ARGV;
      last if /^--$/;

      if(/^-header=(.*)/) {
         $header = $1 ; next }

      if(/^-skeleton=(.*)/) {
         $skeleton = $1 ; next }

      if (/^-language/ )   { next }     # ignored

      if (/^-optdebug/ )   { $OPTDEBUG=1; next }

      if (/^-multi_offset/ )   { $MULTI_OFFSET=1; next }


      if(/^-copy=(.*)/) {
		 $copy = "$1";
		 @Copies = split(/,/, $copy);
		 next
	  }

	  if(/^-xmlfile=(.*)/) {
		 @XMLFiles = split(/,/, $1);
		 next
	  }

      if(/^-insensitive/) {
         $insensitive = "-i" ; next }

      if(/^-nomidl/) {
         $midl = "" ; next }
      # CTS IDLISPC
      if (/^-idl2ispc/)  { $IDL2ISPC=1; next	  }

      if (/^-xml2ispc/)  { $XML2ISPC=1; next	  }

      if (/^-xml2disco/)  { $XML2DISCO=1; next	  } # PTS 1141199

      if (/^-copy_ispc_files/)  { $COPY_ISPC_FILES=1; next	  }

      if(/^-keep_(.+)$/) { ${keep_.$1}=1; next }

      if(/^-k$/) { @KEEPFLG = "-k"; next }

      if(/-output=(.*)$/) { $output = "$1"; next}

# PTS 1110090 start

      if(/-min=(.*)$/) { $min = "$1"; next}

      if(/-max=(.*)$/) { $max = "$1"; next}

	  if(/-nocompile$/) { $NOCOMPILE = 1; next}
# PTS 1110090 end

	  if(/-preprocess$/) { $PREPROC = 1; next}

      if (/-D/)          # defines to RCFLAGS also
	  { push @RCFLAGS, $_;	  }

	  # compile rc files only to res-files
	  if (/-rc2res/)
	  { $RC2RES = 1, next}

	  if (/^-I(.*)$/)   { push @INCLUDES, "-I$1"; next }

      if(/^-classpath=(.*)/) {
         $classpath = $1 ; next }

      if(/^-CFLAGS=(.*)/) { push @COMPCFLAGS, (split ",", $1);  next }

      if (/^-OPT=(.*)$/) { @OPTIMIZE = getOptFlags($1); ; next }

      if (/^-OPTADD=(.*)$/) { @OPTADD = getOptFlags($1); next }

      if (/^-OPTSUB=(.*)$/) { @OPTSUB = getOptFlags($1); next }

      if (/^-except$/) { push @COMPCFLAGS, "-except" };

      if (/^-novalidate$/) { $NOVALIDATE = 1; next };

      if (/^-notdc$/) { $NOTDC = 1; next };

      if (/^-M76$/) { $M76 = 1; next }

      if (/^-OSC$/) { $OSC = 1; next }

      if (/^-OID4BA$/) { $OID4BA = 1; next }

      if (/^-LCA$/) { $LCA = 1; next }

      if(/^(-.*)/) { push @FLAGS, $1; $DEBUG = 1; next}

   } #while

   if ( ! defined $MACHINE ) {
     print "Error: MACHINE environment variable not set, $MACHINE=xxxx\n";
     exit 1;
   }

   # mix optimze flags
   push @OPTIMIZE, @OPTADD;
   foreach $optsup (@OPTSUB)
   {
	  my( $i, $maxi ) = ( 0, $#OPTIMIZE );
      while( $_ = $OPTIMIZE[ $i ], ( $i<=$maxi ) )
      {
         if ( $optsup eq $_)
         {
            splice @OPTIMIZE, $i, 1; next;
         }
         $i++;
      }
   }

   $SOURCE = shift @ARGV;
    # well known extensions rc, ico, dlg, def, mc, idl, lex, ycc
    ($RCBASE, $path,$EXT) = fileparse($SOURCE, '\..*');
   @RCFLAGS;
    $output = "$RCBASE.o" unless $output;

   if ( $EXT =~ /^\.mc$/ ) {
      system("$MC $SOURCE >nul 2>&1") == 0
         or die "Error while executing \"$MC $SOURCE\"\n".
         "message: $!\n";
      unlink("$WRK/incl/$RCBASE.h");
      copy("$RCBASE.h", "$WRK/incl/$RCBASE.h") ||
            warn "Can't copy $RCBASE.h to $WRK/incl/$RCBASE.h";
        system("$RC @RCFLAGS $RCBASE.rc") == 0
         or die "Error while executing \"$RC $RCBASE.rc\"\n".
         "message: $!\n";
	  unless ($RC2RES)
	  {
		  print "$CVTRES -nologo -MACHINE:$MACHINE -out:$output $RCBASE.res \n" if ($ENV{NOQUIET});
		  system("$CVTRES -nologo -MACHINE:$MACHINE -out:$output $RCBASE.res") == 0
			 or die "Error while executing \"$CVTRES -MACHINE:$MACHINE -out:$output $RCBASE.res\"\n".
			 "message: $!\n";
		  ToolTrace("comprc: resource object (message) for '$RCBASE' created.\n");
	  }
      if ( @KEEPFLG == 0 ) {
         unlink "$RCBASE.h", "$RCBASE.rc", <MSG*.bin>;
		 unlink "$RCBASE.res" unless ($RC2RES);
      }
   }
   elsif ( $EXT =~ /^\.(rc|dlg)$/ ) {
        GenerateBuildNr($SOURCE);
      system("$RC -r @RCFLAGS $SOURCE") == 0
         or die "Error while executing \"rc -r @RCFLAGS $RCBASE\"\n".
         "message: $!\n";
	  unless ($RC2RES)
	  {
		  print "$CVTRES -nologo -MACHINE:$MACHINE -out:$output $RCBASE.res\n" if ($ENV{NOQUIET});
		  system("$CVTRES -nologo -MACHINE:$MACHINE -out:$output $RCBASE.res") == 0
			 or die "Error while executing \"$CVTRES -MACHINE:$MACHINE -out:$output $RCBASE.res\"\n".
			 "message: $!\n";
		  ToolTrace("comprc: resource object (GUI) for '$RCBASE' created.\n");

		  if ( @KEEPFLG == 0 ) {
			 unlink "$RCBASE.res";
		  }
	  }
   }
   elsif ( $EXT =~ /^\.xml$/ ) {

      # PTS 1141199
      # exit (xml2Ispc()) if ( $XML2ISPC );
      if ( $XML2ISPC ) {
         exit (xml2Ispc());
      }
      elsif ( $XML2DISCO ) {
        exit (xml2disco());
      }

   }

   elsif ( $EXT =~ /^\.idl$/ ) {

      exit (idl2Ispc())  if ( $IDL2ISPC );


      foreach my $singlecopy (@Copies) {
	     # &gar  (PTS 1102509)
         ICopy::checkdir($singlecopy) ||
            die "Can't create directory for $singlecopy: $!\n";
         File::Copy::copy($SOURCE, $singlecopy) ||
            die "Can't copy $SOURCE to $single: $!\n"
      }
      if (( $midl ne "" ) && (!$nomidl))
      {
          system("$midl -h $RCBASE.h
         -tlb $RCBASE.tlb -proxy ${RCBASE}_p.c
         -dlldata ${RCBASE}_dlldata.c
         -iid ${RCBASE}_i.c
         $SOURCE @FLAGS" );
#  if (( $midl ne "" ) && (!$nomidl))
#   {
#     system("$midl -h $RCBASE.h
#     -tlb $RCBASE.tlb -proxy ${RCBASE}_p.c
#     -dlldata ${RCBASE}_dlldata.c
#     -iid ${RCBASE}_i.c
#     $SOURCE @FLAGS" );
         if ($INSPCODE ne "NONE")
         {
            print ("genInspecCode \"$RCBASE.tlb\" \"${RCBASE}Ispc\" \"$INSPCODE\"\n");

            if (defined $ENV{XCOMP} )
            {
               print ("Using 32 bit version of geninspeccode.exe for the Itanium cross compilation\n");

               if ( ! -f "$ENV{OWN}/../32Bit_Appl/geninspeccode.exe" )
               {
	              warn "$ENV{OWN}/../32Bit_Appl/geninspeccode.exe doesn't exist for Cross-Compilation!\n";
	              exit 1;
               }

               system("$ENV{OWN}/../32Bit_Appl/genInspecCode \"$RCBASE.tlb\" \"${RCBASE}Ispc\" \"$INSPCODE\"");
            }
            else
            {
               system("genInspecCode \"$RCBASE.tlb\" \"${RCBASE}Ispc\" \"$INSPCODE\"");
            }

            system("$CC /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400
                    /DREGISTER_PROXY_DLL /Fo${RCBASE}Ispc.o ${RCBASE}Ispc.cpp");
         }
# }
      }
      else
      {
         @ARGV=($SOURCE,"$RCBASE.h");
         print("convert_idl.pl ...\n");
            do "$TOOL/bin/convert_idl$TOOLEXT";
      }
      $header=~ tr[\\][/]; # PTS 1103045
      if ( $header ) {
         @headerlist = split(/,/, $header);
         foreach my $singleheader (@headerlist) {
         	if ("$RCBASE.h" !~ m!^$singleheader$!i )
         	{
	            ICopy::checkdir($singleheader) ||
	               die "Can't create directory for $singleheader: $!\n";
	            File::Copy::copy("$RCBASE.h", $singleheader) ||
	               die "Can't copy $RCBASE.h to $singleheader: $!\n"
	        }
         }
      }

      # touch dummy files
      system("$TOOL/Posix/touch ${RCBASE}_dlldata.c");
      system("$TOOL/Posix/touch ${RCBASE}_p.c");
      system("$TOOL/Posix/touch ${RCBASE}_i.c");
      system("$TOOL/Posix/touch ${RCBASE}.tlb");

      unlink "$SOURCE.o";
      File::Copy::copy("$SOURCE","$SOURCE.o") ||
            die "Can't copy $SOURCE to $SOURCE.o: $!\n";

	 # ICopy::checkdir("$WRK/incl/${RCBASE}.tlb");
	 # ToolLib::SrcCopy ("${RCBASE}.tlb", "$WRK/incl/") if (-f "${RCBASE}.tlb");
	 # ToolLib::SrcCopy ("${RCBASE}_i.c", "$WRK/incl/") if (-f "${RCBASE}_i.c");
	 # ToolLib::SrcCopy ("${RCBASE}_p.c", "$WRK/incl/") if (-f "${RCBASE}_p.c");
	 # ToolLib::SrcCopy ("${RCBASE}_dlldata.c", "$WRK/incl") if (-f "${RCBASE}_dlldata.c");


      if ( @KEEPFLG == 0 ) {
            unlink "$RCBASE.tlb" unless $keep_tlb;
            unlink "$RCBASE.h" unless $keep_h;
         unlink <$RCBASE*.c> unless $keep_c;
      }
   }
   # CTS DANIEL
   elsif ( $EXT =~ /^\.struct$/ )
   {
      print ("Calling struct2h ...");
   }
   elsif ( $EXT =~ /^\.y$/ )
   {

# Der folgende abschnitt ist fuer die uebersetzung von yacc-dateien verantwortlich.

# Das werkzeug BISON.EXE zum uebersetzen der yacc-dateien in c- oder c++-dateien akzeptiert auf NT
# nur dateien mit einer dateinamelaenge von 8 zeichen !!! Offensichtlich ist es hier immer noch
# eine dos-anwendung.
# Das uebersetzen ist aher in einen UNIX und einen NT abschnitt eingeteilt. Auf UNIX kann das kopieren
# der dateien auf dateien mit kuerzeren dateinamen entfallen.

# Bison erwartet eine datei 'bison.simple' (die eigentliche skelettdatei <dateiname>.skl;
# auf NT wird diese aufgrund der 8.3-notation auf bison.sim verkuerzt). Der name dieser datei kann aber
# ueber die environmentvariable BISON_SIMPLE veraendert werden, was in diesem script auch genutzt wird.
#
# -->  copy <dateiname>.skl, <dateiname_8zeichen>.skl
# -->  $ENV{BISON_SIMPLE} = "./$var1.skl";
#
# Dann, wegen der 8 zeichen
# -->  copy <dateiname>.y, <dateiname_8zeichen>.y

# Aufruf des werkzeugs BISON.EXE mit den erforderlichen parametern.

# BISON.EXE erzeugt eine headerdatei <dateiname>.h, eine c-datei <dateiname>.c und
# eine ausgabedatei <dateiname>.out (die beiden letzteren werden auf NT wieder auf die urspruenglichen
# dateinamen kopiert). Die c-datei wird auf eine cpp-datei kopiert, da wir hier mit klassen arbeiten.
# Die erzeugte headerdatei wird auf den generischen namen <dateiname>_gen.h kopiert, da diese datei
# von den flex-und yacc-dateien benoetigt wird und in diese per '#include <dateiname>_gen.h' anweisung eingebunden
# werden muss (der name muss also schon vor dem uebersetzungszeitpunkt bekannt sein).

# Nach dem kopieren werden die ueberfluessigen dateien geloescht. Die generierte headerdatei DARF NICHT geloescht
# werden, da sie fuer die generierung des scanners mittels flex gebraucht wird!

# Wenn die grammatik 'DEBUGGED' werden muss, dann ist dafuer die datei
# <dateiname_8zeichen>.y (NT) oder <dateiname>.y (UNIX)  im  %wrk%\<slow|quick|fast>\tmp zu nutzen.

      if ($OS eq "UNIX") {
         local @ARGV = ("$RCBASE.skl");
         do "$TOOL/bin/d2u$TOOLEXT";
      }

      if ( $OS eq "WIN32" )
      {
         $var1 = substr $RCBASE,0,8;

         copy "$RCBASE.skl", "$var1.skl";
         $ENV{BISON_SIMPLE} = "./$var1.skl";

         copy "$RCBASE.y", "$var1.y";

         system("bison -v -o $var1.c -d $var1.y");

         unless ( (stat "$var1.c")[7] > 0 )
         { 	die "error while executing $cmd : size of $var1.c is 0 byte\n"; }

         copy "$var1.c", "${RCBASE}.cpp";
         copy "$var1.h", "$WRK/incl/${RCBASE}_gen.h";
         copy "$var1.out", "${RCBASE}.out";
         unlink "$var1.out", "$var1.h", "$var1.c", "$var1.skl";
         if (!$DEBUG)
         {
            unlink "$var1.y";
         }
         else
         {
# im falle des debug-falles, kann diese datei geloescht werden, da $var1.y die datei ist, die von bison
# zum erstellen des parsers genutzt wird
			# unlinked by vmake
            # unlink "$RCBASE.y";
         }
      }
      else
      {
         $ENV{BISON_SIMPLE} = "./$RCBASE.skl";
         my $cmd = "$TOOL/gnu/bin/bison -v -o $RCBASE.c -d $RCBASE.y";
         unless (system($cmd) == 0)
         { die "error while executing $cmd : $!\n"; }
         unless ( (stat "$RCBASE.c")[7] > 0 )
         { 	die "error while executing $cmd : size of $RCBASE.c is 0 byte\n"; }

         copy "$RCBASE.c", "${RCBASE}.cpp";
         copy "$RCBASE.h", "$WRK/incl/${RCBASE}_gen.h";

         unlink "$RCBASE.out", "$RCBASE.h", "$RCBASE.c";
         if (!$DEBUG)
         {
            # &gar unlink "$RCBASE.y", "$RCBASE.skl";
            # $RCBASE.y and $RCBASE.skl (defined as demand )
            # unlink by vmake
         }
      }

      if ( $DEBUG )
      {
         copy "$RCBASE.cpp", "$RCBASE.dbg";
      }

      local @ARGV = (@FLAGS);
      push @ARGV, "-optdebug" if ($OPTDEBUG);
      push @ARGV, @COMPCFLAGS;
      push @ARGV, "${RCBASE}.cpp";
      do "$TOOL/bin/compc$TOOLEXT";

      if ( @KEEPFLG == 0 )
      {
         unlink "$RCBASE.cpp", "$RCBASE.out";
      }
    }
   elsif ( $EXT =~ /^\.l$/ )
   {
# Der folgende abschnitt ist fuer die uebersetzung von lex-dateien verantwortlich.

# In der kompilierungsbeschreibung ist sicherzustellen, dass die lex-dateien erst dann uebersetzt werden,
# wenn die entsprechenden yacc-dateien uebersetzt wurden, da flex dateien aus dem yacc-uebersetzungslauf
# benoetigt.

      $skeleton = "$RCBASE.skl" unless ($skeleton);

      if ($OS eq "UNIX") {
         local @ARGV = ("$RCBASE.skl");
         do "$TOOL/bin/d2u$TOOLEXT";
         my $cmd = "$TOOL/gnu/bin/flex -+ -i -S$skeleton -o$RCBASE.cpp $RCBASE.l";
         unless (system ($cmd) == 0) {
            die "error while executing $cmd : $!\n";
         }
      } else {
         system("flex -+ -i -S$skeleton -o$RCBASE.cpp $RCBASE.l");
      }

      if ( $DEBUG )
      {
         copy "$RCBASE.cpp", "$RCBASE.dbg"
      }

      local @ARGV = (@FLAGS);
      push @ARGV, "-optdebug" if ($OPTDEBUG);
      push @ARGV, @COMPCFLAGS;
      push @ARGV, "${RCBASE}.cpp";
      do "$TOOL/bin/compc$TOOLEXT";

      if ( @KEEPFLG == 0 )
      {
         unlink "$RCBASE.cpp";
      }
   }

   # START PTS 1108978, 15.01.2001
   elsif ( $EXT =~ /^\.g$/ )
   {
      # Der folgende Abschnitt ist für die Übersetzung von ANTLR Grammatiken.
      # Setze temporär den classpath auf das antlr-Tool.
      if ($classpath) { $ENV{CLASSPATH} = $classpath; }
      else { $ENV{CLASSPATH} = "$WRK/Java/antlr.jar"; }
      # Rufe das ANTLR-Tool auf.
      system("java antlr.Tool $RCBASE.g");

      # Übersetze das aus dem .g File generierte cpp File.
      local @ARGV = (@FLAGS);
      push @ARGV, "-language=x";
      push @ARGV, "-optdebug" if ($OPTDEBUG);
      push @ARGV, @COMPCFLAGS;
      push @ARGV, @INCLUDES;
      push @ARGV, "${RCBASE}.cpp";
      do "$TOOL/bin/compc$TOOLEXT";

      if ( $header ) {
         @headerlist = split(/,/, $header);
         foreach my $singleheader (@headerlist) {
            ICopy::checkdir($singleheader) ||
               die "Can't create directory for $singleheader: $!\n";
            File::Copy::copy(basename($singleheader), $singleheader) ||
               die "Can't copy $singlefile to $singleheader: $!\n"
         }
      }

      if ( @KEEPFLG == 0 )
      {
         unlink "$RCBASE.cpp";
         unlink "$RCBASE.hpp";
      }
   }
   # END PTS 1108978

   # gert g. and stefan b., 17.4.2000, .genmsg files contain errors and messages
   elsif( $EXT =~ /^\.genmsg$/ )
   {
     # PTS 1110090: $min option
     $min = "0" unless $min;

     if ( $OS eq "WIN32" )
     {
       # PTS 1110090: rangemin and rangemax
       system( "$ENV{TOOLSHELL} $TOOL/bin/genmsg.pl $RCBASE.genmsg -rangemin $min -rangemax $max @FLAGS" );
     }
     else
     {
       # PTS 1110090: rangemin and rangemax
       system( "$TOOL/bin/genmsg.pl $RCBASE.genmsg -rangemin $min -rangemax $max @FLAGS" );
     }

     if ( $header ) {
      @headerlist = split(/,/, $header);
      foreach my $singleheader (@headerlist) {
         ICopy::checkdir($singleheader) ||
            die "Can't create directory for $singleheader: $!\n";
         File::Copy::copy(basename($singleheader), $singleheader) ||
            die "Can't copy $singlefile to $singleheader: $!\n"
      }
     }
    }
   # jörg m. and frank h., 7.6.2000, .cid files contain configuration parameter identifier
   elsif( $EXT =~ /^\.gencid$/ )
   {
     if ( $OS eq "WIN32" )
     {
         system( "$PERL/bin/perl.exe $TOOL/bin/genmsg.pl -rawmode -difftarget $RCBASE.gencid" );
      }
      else
      {
         system( "$TOOL/bin/genmsg.pl -rawmode -difftarget $RCBASE.gencid" );
      }
     if ( $header ) {
      @headerlist = split(/,/, $header);
      foreach my $singleheader (@headerlist) {
         ICopy::checkdir($singleheader) ||
            die "Can't create directory for $singleheader: $!\n";
         File::Copy::copy(basename($singleheader), $singleheader) ||
            die "Can't copy $singlefile to $singleheader: $!\n"
      }
     }
    }
   else {
      die "comprc: Don't know how to make $SOURCE!\n";
   }
   return 0;
}

##################################################################################

sub GenerateBuildNr {
    local $source = shift;
    open(FILE_OUT, ">$source.tmp") || die "Can't open $source.tmp (output): $!\n";
    print FILE_OUT "#define LINKNUMBER \"$ENV{BUILDSTRING}\"\n";
    open(FILE_IN, "$source") || die "Can't open $source (input): $!\n";
    print FILE_OUT <FILE_IN>;
    close(FILE_IN);
    close(FILE_OUT);
    unlink $source;
    rename "$source.tmp", $source;
}


###################################################################################

sub idl2Ispc
{
	$rc=0;
	if ($PREPROC)
	{
	# call preprocessor

		local @ARGV = (@FLAGS);
        push @ARGV, "$SOURCE";
        do "$TOOL/bin/preprocess$TOOLEXT";
	}
	my @INCLUDES_FOR_IDL = ("-I$INSTROOT/incl", "-I$WRK/incl" );

	unshift @INCLUDES_FOR_IDL, "-I$ENV{LC_INC}" if ($ENV{LC_INC});
	unshift @INCLUDES_FOR_IDL, @INCLUDES;


	unlink ( "$RCBASE.log", "$RCBASE.xml");
	system("$TOOL/pgm/idl2xml -F$SOURCE  @INCLUDES_FOR_IDL @FLAGS")== 0
		or die "Error while executing \"idl2xml -F$SOURCE -I$INSTROOT/incl\"\n".
		"For errors look in $WRK/$VMAKE_VERSION/tmp/$RCBASE.log !\n";

	@IDL_CMD=("$TOOL/pgm/xml2ispc");
	push @IDL_CMD, "-m" if ($MULTI_OFFSET);
	push @IDL_CMD, "$RCBASE.xml";
	system(@IDL_CMD)== 0
		or die "Error while executing \"@IDL_CMD\"\n";

	# copy of headers
	ICopy::checkdir("$WRK/incl/${RCBASE}.h") ||
            die "Can't create directory for $WRK/incl/${RCBASE}.h: $!\n";
	ToolLib::SrcCopy ("${RCBASE}.h", "$WRK/incl/") if (-f "${RCBASE}.h");
	ToolLib::SrcCopy ("${RCBASE}_i.c", "$WRK/incl/") if (-f "${RCBASE}_i.c");
	if ( $COPY_ISPC_FILES )
	{
		ToolLib::SrcCopy ("${RCBASE}Ispc.cpp", "$WRK/incl/") if (-f "${RCBASE}Ispc.cpp");
		ToolLib::SrcCopy ("${RCBASE}Ispc.h", "$WRK/incl/") if (-f "${RCBASE}Ispc.h");
	}
	foreach my $singlecopy (@Copies)
	{
		ICopy::checkdir($singlecopy) ||
            die "Can't create directory for $singlecopy: $!\n";
		ToolLib::SrcCopy ("$SOURCE", "$singlecopy");
	}

	foreach my $xmlcopy (@XMLFiles)
	{
		$xmlcopy=~ tr[\\][/]; # PTS 1103045
		ICopy::checkdir($xmlcopy) ||
				die "Can't create directory for $xmlcopy: $!\n";

		ToolLib::SrcCopy ("$RCBASE.xml", "$xmlcopy")
			  if ( "$RCBASE.xml" !~ m!^$xmlcopy$!i  ); # PTS 1103045

	}


	if ($header)
	{
		@headerlist = split(/,/, $header);
		foreach my $singleheader (@headerlist)
		{
			$singleheader=~ tr[\\][/]; # PTS 1103045
			ICopy::checkdir($singleheader) ||
				die "Can't create directory for $singleheader: $!\n";

			ToolLib::SrcCopy ("$RCBASE.h", "$singleheader")
			  if ( "$RCBASE.h" !~ m!^$singleheader$!i  ); # PTS 1103045

			# if last character  = '/'  copy also other include file
			if ( $singleheader =~ /\/$/ )
			{
				ToolLib::SrcCopy ("${RCBASE}_i.c", "$singleheader") if (-f "${RCBASE}_i.c");
				if ( $COPY_ISPC_FILES )
				{
					ToolLib::SrcCopy ("${RCBASE}Ispc.cpp", "$singleheader") if (-f "${RCBASE}Ispc.cpp");
					ToolLib::SrcCopy ("${RCBASE}Ispc.h", "$singleheader") if (-f "${RCBASE}Ispc.h");
				}
			}
		}
	}


	unless ($NOCOMPILE)
	{
		local @ARGV = (@FLAGS);
		push @ARGV, "-optdebug" if ($OPTDEBUG);
		push @ARGV, @COMPCFLAGS;
		push @ARGV, "${RCBASE}Ispc.cpp";
		do "$TOOL/bin/compc$TOOLEXT";
	}

	if ( @KEEPFLG == 0 ) {
         unlink ("${RCBASE}_i.c", "$RCBASE.cmt", "$RCBASE.xml", "$RCBASE.log",
				"$RCBASE.h", "${RCBASE}Ispc.cpp", "${RCBASE}Ispc.h");
    }
	return 0;
}

###################################################################################

sub xml2Ispc
{
	my @INCLUDES_FOR_IDL = ("-I$INSTROOT/incl", "-I$WRK/incl" );
	unlink ( "${RCBASE}__mgd.xml");

	unshift @INCLUDES_FOR_IDL, "-I$ENV{LC_INC}" if ($ENV{LC_INC});
	#unshift @INCLUDES_FOR_IDL, @INCLUDES;


	system_output("$TOOL/pgm/XMLMerge -F$SOURCE  @INCLUDES_FOR_IDL")== 0
		or die "Error while executing \"$TOOL/pgm/XMLMerge -F$SOURCE  @INCLUDES_FOR_IDL @FLAGS\"\n";

	my $parser_path = defined ($ENV{'SDB_JPD'}) ? $ENV{'SDB_JPD'} : "$ENV{TOOL}/java/XMLValidator";

	my $merged_file = "${RCBASE}__mgd.xml";
	unless ($NOVALIDATE)
	{
		(-e "$parser_path/msv.jar") or die "Missing parser file (msv.jar) under $parser_path\n";
		system_output ("java -jar $parser_path/msv.jar $INSTROOT/incl/CppApplInterface.xsd $merged_file")== 0
			or die "Error while validating xml file\n(java -jar $parser_path/msv.jar $INSTROOT/incl/CppApplInterface.xsd $merged_file)\n";
	}

	my @IDL_CMD=("$TOOL/pgm/xml2ispc");
	push @IDL_CMD, "-m", "-f76";
	 ( $NOTDC ) and push @IDL_CMD, "-notdc";
	push @IDL_CMD, "$merged_file";
	#push @IDL_CMD, "DBPROC";
	system_output(@IDL_CMD)== 0
		or die "Error while executing \"@IDL_CMD\"\n";

	# copy of headers
	ICopy::checkdir("$WRK/incl/${RCBASE}.h") ||
            die "Can't create directory for $WRK/incl/${RCBASE}.h: $!\n";
	ToolLib::SrcCopy ("${RCBASE}.h", "$WRK/incl/") if (-f "${RCBASE}.h");
	ToolLib::SrcCopy ("${RCBASE}_i.c", "$WRK/incl/") if (-f "${RCBASE}_i.c");
	if ( $COPY_ISPC_FILES )
	{
		ToolLib::SrcCopy ("${RCBASE}Ispc.cpp", "$WRK/incl/") if (-f "${RCBASE}Ispc.cpp");
		ToolLib::SrcCopy ("${RCBASE}Ispc.h", "$WRK/incl/") if (-f "${RCBASE}Ispc.h");
	}
	foreach my $singlecopy (@Copies)
	{
		ICopy::checkdir($singlecopy) ||
            die "Can't create directory for $singlecopy: $!\n";
		ToolLib::SrcCopy ("$SOURCE", "$singlecopy");
	}

	foreach my $xmlcopy (@XMLFiles)
	{
		$xmlcopy=~ tr[\\][/]; # PTS 1103045
		ICopy::checkdir($xmlcopy) ||
				die "Can't create directory for $xmlcopy: $!\n";

		ToolLib::SrcCopy ("$RCBASE.xml", "$xmlcopy")
			  if ( "$RCBASE.xml" !~ m!^$xmlcopy$!i  ); # PTS 1103045

	}

	if ($header)
	{
		@headerlist = split(/,/, $header);
		foreach my $singleheader (@headerlist)
		{
			$singleheader=~ tr[\\][/]; # PTS 1103045
			ICopy::checkdir($singleheader) ||
				die "Can't create directory for $singleheader: $!\n";

			ToolLib::SrcCopy ("$RCBASE.h", "$singleheader")
			  if ( "$RCBASE.h" !~ m!^$singleheader$!i  ); # PTS 1103045

			# if last character  = '/'  copy also other include file
			if ( $singleheader =~ /\/$/ )
			{
				ToolLib::SrcCopy ("${RCBASE}_i.c", "$singleheader") if (-f "${RCBASE}_i.c");
				if ( $COPY_ISPC_FILES )
				{
					ToolLib::SrcCopy ("${RCBASE}Ispc.cpp", "$singleheader") if (-f "${RCBASE}Ispc.cpp");
					ToolLib::SrcCopy ("${RCBASE}Ispc.h", "$singleheader") if (-f "${RCBASE}Ispc.h");
				}
			}
		}
	}

	unless ($NOCOMPILE)
	{
		local @ARGV = (@FLAGS);
		push @ARGV, "-optdebug" if ($OPTDEBUG);
		push @ARGV, @COMPCFLAGS;
		push @ARGV, "${RCBASE}Ispc.cpp";
		do "$TOOL/bin/compc$TOOLEXT";
	}

	if ( @KEEPFLG == 0 ) {
         unlink ("${RCBASE}_i.c", "$RCBASE.cmt",
				"$RCBASE.h", "${RCBASE}Ispc.cpp", "${RCBASE}Ispc.h");
    }
    return (0);
}

###################################################################################

sub xml2disco # PTS 1141199
{

	unlink ( "${RCBASE}__mgd.xml");

	my @INCLUDES_FOR_XML = ("-I$INSTROOT/incl", "-I$WRK/incl" );
	unshift @INCLUDES_FOR_XML, "-I$ENV{LC_INC}" if ($ENV{LC_INC});

	system_output("$TOOL/pgm/XMLMerge -F$SOURCE  @INCLUDES_FOR_XML")== 0
		or die "Error while executing \"$TOOL/pgm/XMLMerge -F$SOURCE  @INCLUDES_FOR_XML @FLAGS\"\n";

	my $parser_path = defined ($ENV{'SDB_JPD'}) ? $ENV{'SDB_JPD'} : "$ENV{TOOL}/java/XMLValidator";

	my $merged_file = "${RCBASE}__mgd.xml";

	unless ($NOVALIDATE)
	{
		(-e "$parser_path/msv.jar") or die "Missing parser file (msv.jar) under $parser_path\n";
		system_output ("java -jar $parser_path/msv.jar $INSTROOT/incl/CppClassDecl.xsd $merged_file")== 0
			or die "Error while validating xml file\n(java -jar $parser_path/msv.jar $INSTROOT/incl/CppClassDecl.xsd $merged_file)\n";
	}

	my @XML_CMD=("$TOOL/pgm/XML2DiscCpp");
	push @XML_CMD, "-F$merged_file";
	if ( $M76 )
	{
		push @XML_CMD, "-M76";
	}
	if ( $OSC )
	{
		push @XML_CMD, "-OSC";
	}
	if ( $OID4BA )
	{
		push @XML_CMD, "-OID4BA";
	}
	if ( $LCA )
	{
		push @XML_CMD, "-LCA";
	}
	system_output(@XML_CMD)== 0
		or die "Error while executing \"@XML_CMD\"\n";

	# copy files
	ICopy::checkdir("$WRK/incl/${RCBASE}.h") ||
            die "Can't create directory for $WRK/incl/${RCBASE}.h: $!\n";
	ToolLib::SrcCopy ("${RCBASE}.h", "$WRK/incl/") if (-f "${RCBASE}.h");
	ToolLib::SrcCopy ("${RCBASE}_X2DC_PersClassDesc.h", "$WRK/incl/") if (-f "${RCBASE}_X2DC_PersClassDesc.h");
	ToolLib::SrcCopy ("${RCBASE}_X2DC_PersClassDesc.cpp", "$WRK/incl/") if (-f "${RCBASE}_X2DC_PersClassDesc.cpp");
	foreach my $singlecopy (@Copies)
	{
		ICopy::checkdir($singlecopy) ||
			die "Can't create directory for $singlecopy: $!\n";
		ToolLib::SrcCopy ("$SOURCE", "$singlecopy");
	}

	if ($header)
	{
		@headerlist = split(/,/, $header);
		foreach my $singleheader (@headerlist)
		{
			$singleheader=~ tr[\\][/]; # PTS 1103045
			ICopy::checkdir($singleheader) ||
				die "Can't create directory for $singleheader: $!\n";
			ToolLib::SrcCopy ("$RCBASE.h", "$singleheader")
			  if ( "$RCBASE.h" !~ m!^$singleheader$!i  ); # PTS 1103045

		}
	}

	unless ($NOCOMPILE)
	{
		local @ARGV = (@FLAGS);
		push @ARGV, "-optdebug" if ($OPTDEBUG);
		push @ARGV, @COMPCFLAGS;
		push @ARGV, "${RCBASE}_X2DC_PersClassDesc.cpp";
		do "$TOOL/bin/compc$TOOLEXT";
	}

	if ( @KEEPFLG == 0 ) {
		unlink ("$RCBASE.cmt", "${RCBASE}.h", "${RCBASE}_X2DC_PersClassDesc.h", "${RCBASE}_X2DC_PersClassDesc.cpp");
	}

	return (0);

}

sub getOptFlags
{
	my @OptArray =();
	my $OptString=shift;
	while ( $OptString =~ /(.*)\/(.*)/)
	{
		push @OptArray, $1;
		$OptString = $2;
	}
	push @OptArray, $OptString if ($OptString);
	return @OptArray;
}


sub system_output
{
	my @CMD = @_;
	my $exec_cmd = join " ", @CMD;
	if (defined $ENV{'NOQUIET'})
	{
		print "$exec_cmd\n";
	}
	return (system ("$exec_cmd"));
}
