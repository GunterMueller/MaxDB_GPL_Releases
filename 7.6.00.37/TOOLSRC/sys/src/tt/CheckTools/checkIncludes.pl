#!/usr/bin/perl
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

use Env;
use Cwd;
use Getopt::Std;

$INC_STARTDIR="";
$LC_STARTDIR="";
%INCLUDES;
%FULLINCLUDES;
%LC_INCLUDES;
%DUPPLICATE_INCLUDES;

if ( ! getopts('l:') ) { print "Usage:checkIncludes.pl [-l <location of livecache headers>] [<directory>]\n"; exit }

if ( $#ARGV == -1 )
{
   $INC_STARTDIR=cwd();
}
else
{
   $INC_STARTDIR=shift;
}

##################
# Hash Includes 
##################

print ("Hash includes in $INC_STARTDIR ...\n");
findIncludes(\%INCLUDES, "", $INC_STARTDIR, \%FULLINCLUDES); 
if ($opt_l)
{
   $LC_STARTDIR=$opt_l;
   print ("Hash inlcudes in $LC_STARTDIR ...\n");
   findIncludes(\%LC_INCLUDES, "", $opt_l) if ( $opt_l);
}

foreach $include ( keys %DUPPLICATE_INCLUDES )
{
   print ("found $include in $DUPPLICATE_INCLUDES{$include}\n");
}

foreach $include ( keys %LC_INCLUDES )
{
   print ("WARNING: found lc include '$include' in $INCLUDES{$include}\n") 
       if (defined $INCLUDES{$include});
}

#####################
# scan sourcefiles 
#####################
scanDirectory(".");



sub findIncludes
{
   local ($r_inchash,$dir,$startdir,$r_fullinchash)=@_;
   local @allfiles=();
   opendir(DIR_IN, "$startdir/$dir") or
       die "Can't read directory '$startdir/$dir\n" ;
   @allfiles=readdir (DIR_IN);
   closedir(DIR_IN);
   foreach $file (@allfiles)
   {
      next if ($file =~ /^(\.\.|\.)$/);
      findIncludes($r_inchash, $dir eq "" ? "$file" : "$dir/$file",
         $startdir, $r_fullinchash) 
            if ( -d "$startdir/$dir/$file");
      if ($file =~/.*\.(h|hpp)$/)
      {
         if ( defined $$r_inchash{$file})
         {
#           print ("WARNING: found $file in ");
#	    print $dir eq "" ? "." : "$dir";
#           print " and in $$r_inchash{$file}\n";
            $$r_inchash{$file}.=$dir eq "" ? ",." : ",$dir";
            $DUPPLICATE_INCLUDES{$file}=$$r_inchash{$file};
         } 
         else
         {
            $$r_inchash{$file}=$dir eq "" ? "." : "$dir";
         }
         $$r_fullinchash{$dir eq "" ? "$file" : "$dir/$file"}=1 
                       if (defined $r_fullinchash); 
      }
   } 
}

sub scanDirectory
{
   local $dir=shift;
   local @allfiles=();
   opendir(DIR_IN, "$INC_STARTDIR/$dir") or
       die "Can't read directory '$INC_STARTDIR/$dir\n" ;
   @allfiles=readdir (DIR_IN);
   closedir(DIR_IN);
   foreach $file (@allfiles)
   {
      next if ($file =~ /^(\.\.|\.)$/);
      scanDirectory("$dir/$file") if ( -d "$INC_STARTDIR/$dir/$file");
      scanFile("$dir/$file") if ($file =~/.*\.(h|hpp|cpp|c)$/);
   }
}

sub scanFile
{
   local $file=shift;
   $printFilename=0;
   $linenumber = 0;
   open (FILE_IN, "<$INC_STARTDIR/$file") or 
           die "Can't open $INC_STARTDIR/$file\n";
   while ($line = <FILE_IN>)
   {
        $linenumber++;
        printErrorMessage("$1 for includes is deadly:\n$line\n" ,
             $linenumber,$file,\$printFilename)
                if ($line =~ /^\s*#\s*include\s*[<"](\.|\..).*[>"]/) ;
        
        if ($line =~ /^\s*#\s*include\s*<.*[\\\/]+([^\\\/].*)>/)
        {
           # look for own include <xyz/x.h>
           printErrorMessage("don't use #include <...> for own headers:\n$line",
             $linenumber,$file,\$printFilename) 
              if (defined $INCLUDES{$1} || defined $FULLINCLUDES{$1});
        }
	elsif ($line =~ /^\s*#\s*include\s*<(.*)>/) # check
        {
           # look for own include <x.h>
           printErrorMessage("don't use #include <...> for own headers:\n$line" ,
             $linenumber,$file,\$printFilename) 
              if (defined $INCLUDES{$1} || defined $FULLINCLUDES{$1});
        }
        elsif ($line =~ /^\s*#\s*include\s*"(.*[\\\/]+)([^\\\/].*)"/)
        {
           # look for own include "xyz/x.h" without full name 
           if (defined $INCLUDES{$2})
           {
              printErrorMessage("write the \"full\" path for includes:\n$line" ,
                $linenumber,$file,\$printFilename)
                 unless  ( defined $FULLINCLUDES{"$1$2"});
           }
           else
           {
             # look for system includes with "xyz/x.h"
             printErrorMessage("found unknown include with \"...\" :\n$line" ,
                $linenumber,$file,\$printFilename)
           }
        }
        elsif ($line =~ /^\s*#\s*include\s*"(.*)"/)
        {
           if (defined $INCLUDES{$1})
           {
           # look for own include "x.h" without full name 
              printErrorMessage("write the \"full\" path for includes:\n$line" ,
                $linenumber,$file,\$printFilename)
                  unless (defined $FULLINCLUDES{"$1"});
           }
           else
           {
           # look for system includes with "x.h" 
             printErrorMessage("found unknown include with \"...\" :\n$line" ,
                $linenumber,$file,\$printFilename) 
           }
        }
        

   }
}

sub printErrorMessage
{
   local ($message,$line,$file,$r_printfile)=@_;
   unless ($$r_printfile)
   {
      print ("\nFILE: $file:\n");
      $$r_printfile=1;
   }
   print "$line:$message\n";
}
   

