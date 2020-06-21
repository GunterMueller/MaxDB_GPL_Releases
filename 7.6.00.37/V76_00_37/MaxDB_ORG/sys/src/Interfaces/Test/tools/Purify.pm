#!/usr/bin/perl
# purify
#
# @(#)purify  6.2  2001-08-10
#
# Purifies precompiler and ODBC test programs
# assumes that Purify is set in current path
#
# TTK
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2001-2005 SAP AG
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

package Purify;

use Env;

sub WriteResultHeader;
sub WriteResultFooter;
sub EvaluateSingleResults;
sub WriteResultValgrindHeader;
sub WriteResultValgrindFooter;
sub EvaluateSingleValgrindResults;
sub Rename;

sub Rename
{
    local($type) = $_[0];

&if $OSSPEC in [ WIN32 ]
    $testdir = "$TEST\\pc\\wrk\\";
&else
    $testdir = "$TEST/pc/wrk/";
&endif

    $oldname = $testdir."purify.html";
    $newname = $testdir."purify".$type.".html";

    rename ($oldname, $newname);

}

sub EvaluateSingleResults
{
# gathers results of a single purify run:
# for each test program two new files are generated: one for errors only and one for warnings only.

    local($exe) = $_[0];

    $HTMLWarnTagBegin="\<b\>\<span style=\"background-color: #F0F099\">";
    $HTMLWarnTagEnd="\</span\>\</b\>";
    $HTMLErrorTagBegin="\<b\>\<span style=\"background-color: #F09999\">";
    $HTMLErrorTagEnd="\</span\>\</b\>";

&if $OSSPEC in [ WIN32 ]
    $testdir = "$TEST\\pc\\wrk\\";
&else
    $testdir = "$TEST/pc/wrk/";
&endif

    $formatstring       = "%s<A href=\"%s.txt\">%s</A>:\t <A href=\"%s.err\">%d Errors</A> \t <A href=\"%s.warn\">%d Warnings</A> %s\n";
    $formatstringnoerr  = "%s<A href=\"%s.txt\">%s</A>:\t %d Errors \t <A href=\"%s.warn\">%d Warnings</A> %s\n";
    $formatstringnowarn = "%s<A href=\"%s.txt\">%s</A>:\t %d Errors \t %d Warnings %s\n";
    $formatstringerrnw  = "%s<A href=\"%s.txt\">%s</A>:\t <A href=\"%s.err\">%d Errors</A> \t %d Warnings %s\n";


    open (PURIFYRES, ">>".$testdir."purify.html");

    $error = 0;
    $warn = 0;
    $writeerr = 0;
    $writewarn = 0;
    open (TESTRES, $testdir.$exe.".txt");
    open (TESTRESERR, ">".$testdir.$exe.".err");
    open (TESTRESWARN, ">".$testdir.$exe.".warn");
    while (<TESTRES>)  {
      if (/^\[E/) {
        $writeerr = 1;
        $writewarn = 0;
        $error = $error + 1;
      }
      if (/^\[W/) {
        $writewarn = 1;
        $writeerr = 0;
        $warn = $warn + 1;
      }
      if (/^\[I/) {
        $writeerr = 0;
        $writewarn = 0;
      }
      
      if ($writeerr != 0)  {
        printf TESTRESERR ("%s", $_);
      }
      if ($writewarn != 0)  {
        printf TESTRESWARN ("%s", $_);
      }
    }
    close (TESTRES);
    close (TESTRESOUTERR);
    close (TESTRESOUTWARN);
    $out = $exe;
    for ($i=length($exe); $i<12; $i++)  {
      $exe = $exe . " ";
    }
    $begintag = "";
    $endtag = "";
    if ($warn > 0)  {
      $begintag = $HTMLWarnTagBegin;
      $endtag   = $HTMLWarnTagEnd;
    }
    if ($error > 0)  {
      $begintag = $HTMLErrorTagBegin;
      $endtag   = $HTMLErrorTagEnd;
    }
    
    if ($warn > 0)  {
      if ($error > 0)  {
        printf PURIFYRES ($formatstring,      $begintag, $out, $exe, $out, $error, $out, $warn, $endtag);
      }
      else {
        printf PURIFYRES ($formatstringnoerr, $begintag, $out, $exe, $error, $out, $warn, $endtag);
      }
    }
    else {
      if ($error > 0)  {
        printf PURIFYRES ($formatstringerrnw,  $begintag, $out, $exe, $out, $error, $warn, $endtag);
      }
      else {
        printf PURIFYRES ($formatstringnowarn, $begintag, $out, $exe, $error, $warn, $endtag);
      }
    }
    close (PURIFYRES);
}

sub WriteResultHeader
{
# creates result file, writes HTML-preamble, starting time and some user info

    local($kind) = $_[0];

&if $OSSPEC in [ WIN32 ]
    $testdir = "$TEST\\pc\\wrk\\";
&else
    $testdir = "$TEST/pc/wrk/";
&endif

    open (PURIFYRES, ">".$testdir."purify.html");

    print PURIFYRES "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html\"> <meta http-equiv=\"expires\" content=\"28800\"> </head><body ><PRE>\n";

    print PURIFYRES "\<h4\>Purify results. Start: ".localtime(time())."\</h4\>\n\n";
    print PURIFYRES "The hyper links point to:\n\<ul\>";
    print PURIFYRES "\<li\>All information gathered by Purify\</li\>";
    print PURIFYRES "\<li\>Errors found by Purify (background color of line is red)<\/li\>";
    print PURIFYRES "\<li\>Warnings found by Purify (background color of line is yellow)</li></ul>\n\n\n";

    close (PURIFYRES);
}

sub WriteResultFooter
{
# appends to result file ending time and finishes HTML-document

&if $OSSPEC in [ WIN32 ]
    $testdir = "$TEST\\pc\\wrk\\";
&else
    $testdir = "$TEST/pc/wrk/";
&endif

    open (PURIFYRES, ">>".$testdir."purify.html");

    print PURIFYRES "\n\n\n\<h4\>Purify results. Stop: ".localtime(time())."\</h4\>\n\n";
    print PURIFYRES "\</body\> \</html\>";

    close (PURIFYRES);
}


sub WriteResultValgrindFooter
{
# appends to result file ending time and finishes HTML-document

&if $OSSPEC in [ WIN32 ]
    $testdir = "$TEST\\pc\\wrk\\";
&else
    $testdir = "$TEST/pc/wrk/";
&endif

    open (PURIFYRES, ">>".$testdir."purify.html");

    print PURIFYRES "\n\n\n\<h4\>Valgrind results. Stop: ".localtime(time())."\</h4\>\n\n";
    print PURIFYRES "\</body\> \</html\>";

    close (PURIFYRES);
}


sub WriteResultValgrindHeader
{
# creates result file, writes HTML-preamble, starting time and some user info

    local($kind) = $_[0];

&if $OSSPEC in [ WIN32 ]
    $testdir = "$TEST\\pc\\wrk\\";
&else
    $testdir = "$TEST/pc/wrk/";
&endif

    open (PURIFYRES, ">".$testdir."purify.html");

    print PURIFYRES "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html\"> <meta http-equiv=\"expires\" content=\"28800\"> </head><body ><PRE>\n";

    print PURIFYRES "\<h4\>Valgrind results. Start: ".localtime(time())."\</h4\>\n";
    print PURIFYRES "Programs with errors are labeled red and linked to the error protocol.\n\n\n";

    close (PURIFYRES);
}

sub EvaluateSingleValgrindResults
{
# for each test program: separates output of testprogram and valgrind, checks for errors

    local($exe) = $_[0];

    $HTMLErrorTagBegin="\<b\>\<span style=\"background-color: #F09999\">";
    $HTMLErrorTagEnd="\</span\>\</b\>";

&if $OSSPEC in [ WIN32 ]
    $testdir = "$TEST\\pc\\wrk\\";
&else
    $testdir = "$TEST/pc/wrk/";
&endif

    $formatstringerror = "%s<A href=\"%s\">%s</A>%s\n";
    $formatstringnoerror = "%s\n";


    open (PURIFYRES, ">>".$testdir."purify.html");

    $error = 1;
    open (TESTRES, $testdir.$exe.".val");
    while (<TESTRES>)  {
      if (/ERROR SUMMARY: 0 errors/) {
      print $_;
        $error = 0;
      }
    }
    close (TESTRES);

    $out = $exe.".val";

    if ($error > 0) {
        printf PURIFYRES ($formatstringerror,  $HTMLErrorTagBegin, $out, $exe, $HTMLErrorTagEnd);
    } else {
        printf PURIFYRES ($formatstringnoerror,  $exe);
    }

    close (PURIFYRES);
}


1;
