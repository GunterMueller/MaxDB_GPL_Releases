#!/usr/bin/perl
#
# @(#)sqlmess   1997-11-18
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

use Env;

chdir("$TMP") || die "Can't change to directory $TMP: $!";

if ($RELVER eq "R61"){
    open(FILE_IN, "<$DBROOT/env/SQL.dem") || die "Can't open $DBROOT/env/SQL.dem (input): $!";
    open(FILE_OUT, ">SQLtmp.dem") || die "Can't open SQLtmp.dem (output): $!";
    while(<FILE_IN>) {
        next if /^\+\+/;
        print FILE_OUT $_;
    }
    close(FILE_IN);
    close(FILE_OUT);

    open(FILE_IN, "<$DBROOT/env/SQL.enm") || die "Can't open $DBROOT/env/SQL.enm (input): $!";
    open(FILE_OUT, ">SQLtmp.enm") || die "Can't open SQLtmp.enm (output): $!";
    while(<FILE_IN>) {
        next if /^\+\+/;
        print FILE_OUT $_;
    }
    close(FILE_IN);
    close(FILE_OUT);

    open(FILE_IN, "<$TMP/vpx01.dm") || die "Can't open $TMP/vpx01.dm (input): $!";
    open(FILE_OUT, ">>SQLtmp.dem") || die "Can't open SQLtmp.dem (output): $!";
    while(<FILE_IN>) {
        next if /^\+\+/;
        print FILE_OUT $_;
    }
    close(FILE_OUT); close(FILE_IN);

    open(FILE_IN, "<$TMP/vpx01.em") || die "Can't open $TMP/vpx01.em (input): $!";
    open(FILE_OUT, ">>SQLtmp.enm") || die "Can't open SQLtmp.enm (output): $!";
    while(<FILE_IN>) {
        next if /^\+\+/;
        print FILE_OUT $_;
    }
    close(FILE_IN);
    close(FILE_OUT);

    system("$TOOL/Posix/sort -n +1 -2 SQLtmp.dem > $DBROOT/env/SQLMESS.dem");
    system("$TOOL/Posix/sort -n +1 -2 SQLtmp.enm > $DBROOT/env/SQLMESS.enm");
}
else {
    if ( -f "$DBROOT/env/de/SQLM.de" )
    {
        open(FILE_IN, "<$DBROOT/env/de/SQLM.de") ||
            die "Can't open $DBROOT/env/de/SQLM.de (input): $!";
        open(FILE_OUT, ">SQLtmp.dem") ||
            die "Can't open SQLtmp.dem (output): $!";
        while(<FILE_IN>) {
            next if /^\+\+/;
            print FILE_OUT $_;
            $dSQLMlinecount++;
        }
        close(FILE_IN);
    }
    if (-f "$DBROOT/env/de/PRECOMM.de" )
    {
        open(FILE_IN, "<$DBROOT/env/de/PRECOMM.de") ||
            die "Can't open $DBROOT/env/de/PRECOMM.de (input): $!";
        while(<FILE_IN>) {
            next if /^\+\+/;
            print FILE_OUT $_;
        }
        close(FILE_OUT); close(FILE_IN);
    }

    open(FILE_IN, "<$DBROOT/env/en/SQLM.en") || die "Can't open $DBROOT/env/en/SQLM.en (input): $!";
    open(FILE_OUT, ">SQLtmp.enm") || die "Can't open SQLtmp.enm (output): $!";
    while(<FILE_IN>) {
        next if /^\+\+/;
        print FILE_OUT $_;
        $eSQLMlinecount++;
    }
    close(FILE_IN);
    open(FILE_IN, "<$DBROOT/env/en/PRECOMM.en") || die "Can't open $DBROOT/env/en/PRECOMM.en (input): $!";
    while(<FILE_IN>) {
        next if /^\+\+/;
        print FILE_OUT $_;
    }
    close(FILE_OUT); close(FILE_IN);

##################################################################################################################
#                                                                                                                #
#     Japanese messages no longer supported                                                                      #
#                                                                                                                #
#     if ($RELVER eq "R62"){                                                                                     #
#         open(FILE_IN, "<$DBROOT/env/jp/SQLM.jp") || die "Can't open $DBROOT/env/jp/SQLM.jp (input): $!";       #
#         open(FILE_OUT, ">SQLtmp.jpm") || die "Can't open SQLtmp.jp (output): $!";                              #
#         while(<FILE_IN>) {                                                                                     #
#             next if /^\+\+/;                                                                                   #
#             print FILE_OUT $_;                                                                                 #
#             $jSQLMlinecount++;                                                                                 #
#         }                                                                                                      #
#         close(FILE_IN);                                                                                        #
#         open(FILE_IN, "<$DBROOT/env/jp/PRECOMM.jp") || die "Can't open $DBROOT/env/jp/PRECOMM.jp (input): $!"; #
#         while(<FILE_IN>) {                                                                                     #
#             next if /^\+\+/;                                                                                   #
#             print FILE_OUT $_;                                                                                 #
#         }                                                                                                      #
#         close(FILE_OUT); close(FILE_IN);                                                                       #
#                                                                                                                #
#         }                                                                                                      #
##################################################################################################################

    if ( -f "SQLtmp.dem" )
    {
        system("$TOOL/Posix/sort -n +1 -2 SQLtmp.dem > $DBROOT/env/de/SQLMESS.de");
    }
    system("$TOOL/Posix/sort -n +1 -2 SQLtmp.enm > $DBROOT/env/en/SQLMESS.en");

#######################################################################################
#     Japanese messages no longer supported                                           #
#                                                                                     #
#     if ($RELVER eq "R62"){                                                          #
#         system("$TOOL/Posix/sort -n +1 -2 SQLtmp.jpm > $DBROOT/env/jp/SQLMESS.jp"); #
#         }                                                                           #
#######################################################################################

    if ( -f "$DBROOT/env/de/SQLMESS.de" )
    {
        open(FILE_IN, "<$DBROOT/env/de/SQLMESS.de") || die "Can't open $DBROOT/env/de/SQLMESS.de (input): $!";
        while(<FILE_IN>) { $dSQLMESSlinecount++ }
        close(FILE_IN);
    }

    open(FILE_IN, "<$DBROOT/env/en/SQLMESS.en") || die "Can't open $DBROOT/env/en/SQLMESS.en (input): $!";
    while(<FILE_IN>) { $eSQLMESSlinecount++ }
    close(FILE_IN);

##################################################################################################################
#     Japanese messages no longer supported                                                                      #
#                                                                                                                #
#     if ($RELVER eq "R62"){                                                                                     #
#         open(FILE_IN, "<$DBROOT/env/jp/SQLMESS.jp") || die "Can't open $DBROOT/env/jp/SQLMESS.jp (input): $!"; #
#         while(<FILE_IN>) { $jSQLMESSlinecount++ }                                                              #
#         close(FILE_IN);                                                                                        #
#         }                                                                                                      #
##################################################################################################################

    #if ( $dSQLMESSlinecount > $dSQLMlinecount ) {
    #  unlink "$DBROOT/env/de/SQLM.de";
    #  unlink "$DBROOT/env/de/PRECOMM.de";
    #}
    #if ( $eSQLMESSlinecount > $eSQLMlinecount ) {
    #  unlink "$DBROOT/env/en/SQLM.en";
    #  unlink "$DBROOT/env/en/PRECOMM.en";
    #}
    #if ( $jSQLMESSlinecount > $jSQLMlinecount ) {
    #  unlink "$DBROOT/env/jp/SQLM.jp";
    #  unlink "$DBROOT/env/jp/PRECOMM.jp";
    #}
}

unlink <SQLtmp.??m>;

