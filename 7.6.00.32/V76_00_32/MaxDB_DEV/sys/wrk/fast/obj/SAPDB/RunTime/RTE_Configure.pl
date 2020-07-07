#!/usr/bin/env perl
#  @author DanielD
#  @brief autoconf in perl
#
#    ========== licence begin  GPL
#    Copyright (c) 2004-2005 SAP AG
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

# the following must be added to ::SAPDB/RunTime.com
#RTE_Configure.pl ascii exec \
#    execscript=./RTE_Configure.pl \
#    extdep=$WRK/incl/SAPDB/RunTime/RTE_Configure.h

# extending RTE_Configure.pl:
#
# 1. add a new variable SDB_WHATEVER to the array @variables in main
#
# 2. to test: add $testVar = 'SDB_WHATEVER'; in main
#    this will test only this variable and write the result to stdout
#     (great for remimf)
#
# 3. the most common class is RTE_RunDetection:
#     new RTE_RunDetection ('SDB_WHATEVER',   # the variable name
#         "comment for SDB_WHATEVER",         # comment
#         $source,                            # short snippets can be written inline
#                                             # longer code should be put
#                                             # into package RTE_ConfigureSources
#         "ignoreErrors",                     # optional: errors in compile, link
#                                             #  or run result in empty/false variable
#         "link: -lwhatever",                 # optional: option passed to the linker
#         "restrict: $^O =~ /linux/",         # optional: test is not run if
#                                             #  expression evaluates to false
#         "target: header,vmake,concom",      # optional: make variable visible
#                                             #  to RTE_Configure.h, vmake concom
#                                             #  and/or mfconcom
#                                             #  default: header
#     )
#     The value of the variable is read from the file sdbtest.out.
#     with moste templates in RTE_Template, a variable FILE * result
#       can be used to write to that file.
#     If sdbtest.out is empty, the variable will not be defined.
#

use strict;
use Getopt::Std;

{
    # class: describes a platform feature
    package RTE_ConfigureVariable;

    sub new {
        my ($class, $name, $comment, @options) = @_;
        my $self = {};
        bless $self, $class;
        $self->{name} = $name;
        $self->{comment} = $comment;
        $self->{options} = \@options;
        $self->targetfilesFromOptions (@options);
        return $self;
    }

    sub targetfilesFromOptions {
        my ($self, @options) = @_;
        my ($targetSet) = 0;
        foreach my $option (@options) {
            if ($option =~ /target: *(.*)/) {
                my @targets = split /,/, $1;

                foreach my $target (@targets) {
                    if ($target eq 'header') {
                        $self->{targetsHeader} =1;
                    }
                    elsif ($target eq 'vmake') {
                        $self->{targetsVmake} =1;
                    }
                    elsif ($target eq 'concom') {
                        $self->{targetsConcom} =1;
                    }
                }
                $targetSet = 1;
            }
        }
        if (!$targetSet) {
            $self->{targetsHeader} =1;
        }
    }
    sub cdefine {
        my ($self) = @_;
        my $result = '';

        if (defined ($self->{comment})) {
            $result .= "/*\n * " . $self->{comment} . "\n */\n";
        }
        if (!defined ($self->{value}) || ($self->{value} eq '')) {
            $result .= "/* #undef " . $self->{name} . " */\n\n";
        }
        else {
            $result .= "#define " . $self->{name} . " " . $self->{value} . "\n\n";
        }
        return $result;
    }

    sub isApplicable {
        my ($self, $configure) = @_;
        my $options = $self->{options};
        foreach my $option (@$options) {
            if ($option =~ /restrict:(.*)/) {
                if (!eval ($1)) {
                    if ($Configure::Util::verbose) {
                        print "variable $self->{name} skipped because eval ('$1') -> false\n";
                    }
                    return undef;
                }
            }
            if ($option =~ /configprecondition:\s*(.*)/) {
                if (defined $configure) {
                    my $othervar = $configure->findVariable ($1);
                    if (! defined $othervar->{value}) {
                        if ($Configure::Util::verbose) {
                            print "variable $self->{name} skipped because configure var $1 not set\n";
                        }
                        return undef;
                    }
                }
            }
        }
        return 1;
    }

    sub  detect {
        my ($self) = @_;
        die "abstract method detect not defined";
    }

}

#----------------------------------------------------------------------#

{
    # class: detects feature by compiling and runnign a C program
    package RTE_RunDetection;
    @RTE_RunDetection::ISA = qw ( RTE_ConfigureVariable );
    sub new {
        my ($class, $name, $comment, $code, @options) = @_;
        my $self = RTE_ConfigureVariable::new ($class, $name, $comment, @options);
        $self->{code} = $code;
        return $self;
    }

    sub  detect {
        my ($self) = @_;
        if (defined $ENV{XCOMP}) {
            die "variable $self->{name} canot be detected when cross compiling, define in RTE_Configure_*.fixed instead";
        }
        $self->{value} = CCompile::testRun ($self->{code}, $self->{options});
    }
}

#----------------------------------------------------------------------#

{
    # class: test if a certain include file is available
    package RTE_IncludeDetection;
    @RTE_IncludeDetection::ISA = qw ( RTE_ConfigureVariable );
    sub new {
        my ($class, $name, $includefile, @options) = @_;
        my $comment = "true if $includefile can be included";
        my $self = RTE_ConfigureVariable::new ($class, $name, $comment, @options);
        $self->{includefile} = $includefile;
        return $self;
    }

    sub  detect {
        my ($self) = @_;
        my $includefile = $self->{includefile};
        my $code = <<EOF
#include <$includefile>
EOF
        ;
        if (CCompile::testCompile ($code, 0, $self->{options})) {
           $self->{value} = 1;
        }
        else {
           undef $self->{value};
        }
    }
}

#----------------------------------------------------------------------#

{
    # class: detects the type of int8 by testing several common names
    package RTE_Int8Detection;
    @RTE_Int8Detection::ISA = qw ( RTE_ConfigureVariable );

    sub new {
        my ($class, $name, $comment, @options) = @_;
        return RTE_ConfigureVariable::new ($class, $name, $comment, @options);
    }

    sub  detect {
        my ($self) = @_;
        my $int8type;
        foreach my $typedef ('__int64', 'long', 'long long') {
            my $code = RTE_Templates::simpleMain (<<EOF
    $typedef intvar;
    if (sizeof (intvar) == 8) {
        fprintf (result, "$typedef");
    }
EOF
            );
            eval {$int8type = CCompile::testRun ($code);};
            if (!$@ && ($int8type ne '')) {
                $self->{value} = $int8type;
                last;
            }
        }
        if ($int8type eq '') {
            die 'no valid int8 type found';
        }
    }

}

#----------------------------------------------------------------------#

{
    # class: detects the type of int8 by testing several common names
    package RTE_ConstPamMessageParameter;
    @RTE_ConstPamMessageParameter::ISA = qw ( RTE_ConfigureVariable );

    sub new {
        my ($class, $name, $comment, @options) = @_;
        return RTE_ConfigureVariable::new ($class, $name, $comment, @options);
    }

    sub  detect {
        my ($self) = @_;
        my $usefulModifier;
        foreach my $modifier ('const', '') {
            my $code = <<EOF
#include <security/pam_appl.h>

extern "C" int
RTE_PAM_conv ( int               num_msg,
           $modifier struct pam_message  **msg,
           struct pam_response **resp,
           void                 *appdata_ptr )
{
	return 0;
}

static struct pam_conv PAM_conversation = {
    &RTE_PAM_conv,      /* address of our interface function */
    0
};

int main ()
{
    return 0;
}
EOF
            ;
            if (CCompile::testCompile ($code)) {
                if ($modifier eq 'const') {
                    $self->{value} = 1;
                }
                last;
            }
        }
    }

}

#----------------------------------------------------------------------#

{
   package RTE_Templates;

   sub simpleMain {
      my ($code) = @_;
      my $resultfile = "sdbtest.out";;
      return <<EOF
#include <stdlib.h>
#include <stdio.h>

int main ()
{
    FILE * result = fopen ("$resultfile", "w");
    $code
    fclose (result);
    return 0;
}
EOF
      ;
   }
}

#----------------------------------------------------------------------#

{

    # class: keeps list of all the configure varaibles
    #        and writes header files
    #
    #        Could also write files that are used by
    #        concom (both vmake and pascal
    #
    #        Has provisions to sort variables into multiple header files

    package RTE_Configure;

    sub new {
        my ($class) = @_;
        my $self = {};
        bless $self;
        $self->{lists} = {};
        $self->addList ('ALL');
        # add additional names
        # if additional header files with subsets are useful
        return $self;
    }

    sub  addList {
        my ($self, $listname) = @_;
        my $lists = $self->{lists};
        $lists->{$listname} = [];
    }

    sub  addVar {
        my ($self, $variable, @listnames) = @_;
        push @listnames, 'ALL';
        foreach my $listname (@listnames) {
            my $list = $self->{lists}->{$listname};
            push @$list, $variable;
        }
    }

    sub findVariable {
        my ($self, $name) = @_;
        my $list = $self->{lists}->{ALL};
        foreach my $variable (@$list) {
            if ($name eq $variable->{name}) {
                return $variable;
            }
        }
    }

    sub genCHeader {
        my ($self) = @_;
        my $lists = $self->{lists};
        foreach my $listname (keys %$lists) {
            my $list = $lists->{$listname};
            my $filebase = ($listname eq 'ALL' ? 'RTE_Configure.h' : "$listname.h");
            open (CHEADER, ">". $ENV{WRK} . "/incl/SAPDB/RunTime/$filebase");
            print CHEADER<<EOF
/*
 * RTE_Config.h
 * this file is generated by:
 * imf RTE_Configure.pl
 */

#ifndef RTE_CONFIGURE_H
#define RTE_CONFIGURE_H

EOF
            ;
            foreach my $variable (@$list) {
                if ($variable->{targetsHeader}) {
                    print CHEADER $variable->cdefine ();
                }
            }
            print CHEADER<<EOF
#endif
EOF
            ;
            close CHEADER;
        }
    }

    sub genToolvars {
        my ($self) = @_;
        my @vmakeVars;
        my @concomVars;
        my $varlist = $self->{lists}->{'ALL'};
        foreach my $variable (@$varlist) {
            if ($variable->{value} ne '') {
                if ($variable->{targetsVmake}) {
                    push @vmakeVars, $variable;
                }
                if ($variable->{targetsConcom}) {
                    push @concomVars, $variable;
                }
            }
        }
        my $toolvarsFile = "$ENV{WRK}/config/toolvars_configure.pl";
        if (@vmakeVars && @concomVars) {
            open (TOOLVARS, ">$toolvarsFile");
            print TOOLVARS<<EOF
#
# toolvars_configure.pl
# this file is generated by:
# imf RTE_Configure.pl
#
EOF
            ;
            foreach my $variable (@vmakeVars) {
                print TOOLVARS '$ENV{' . $variable->{name} . '} = "'
                    . $variable->{value} . "\";\n";
            }
            if (@concomVars) {
                print TOOLVARS 'push @CONCOMOPTS';
                foreach my $variable (@concomVars) {
                    print TOOLVARS ', "-' . $variable->{name} . '=' . $variable->{value} . '"';
                }
                print TOOLVARS ";\n";
            }
            close TOOLVARS;
        }
        else {
            unlink $toolvarsFile;
        }
    }

    sub fullfillsMinimumRequirements {
        my ($self) = @_;
        my $errorCount = 0;
        if ($ENV{SAPDB_INTERNAL} && $^O =~ /linux/) {
            my $variable = $self->findVariable ('HAS_PAM_SUPPORT');
            if (!defined $variable->{value}) {
                print "error: RTE_Configure.pl: PAM SDK is required, but not found\n";
                print "error: RTE_Configure.pl: on SuSE: install pam-devel\n";
                # ++$errorCount;
            }
        }
        return $errorCount == 0;
    }
}

#----------------------------------------------------------------------#

{
    # package: compile and run C programs
    package CCompile;

    my $source = 'sdbtest.cpp';
    my $obj = 'sdbtest.o';
    my $executable = 'sdbtest.exe';
    my $resultfile = "sdbtest.out";

    #
    # compiles and runs a C++ program
    # returns: stdout of the run
    # exceptions: if compile/link fails or program exists with rc != 0
    #
    sub testRun {
        #
        # compile
        #
        my ($code, $options) = @_;
        my $ignoreErrors = grep /ignoreErrors/, @$options;
        my $linkok = testCompile ($code, 1, $options);
        if (!$linkok) {
            if ($ignoreErrors) {
                return undef;
            }
            else {
                die "Link step failed";
            }
        }
        #
        # run
        #
        my $runresult =  Configure::Util::vsystem ("./$executable");
        Configure::Util::removeFile ($executable);
        if ($runresult) {
            if ($ignoreErrors) {
                return undef;
            }
            else {
                die "run failed with $runresult";
            }
        }
        #
        # get output
        #
        open (RESULTFILE, "<$resultfile");
        my @resultlines = <RESULTFILE>;
        close (RESULTFILE);
        Configure::Util::removeFile ($resultfile);
        return join ("\n", @resultlines);
    }

    sub  testCompile {
        my ($code, $link, $options) = @_;
        open (CFILE, ">$source");
            print CFILE $code;
        close CFILE;
        my $compilerc = Configure::Util::vscript ("compc", $source);
        Configure::Util::removeFile ($source);
        if ($compilerc) {
            print "compile failed: $compilerc\n";
            return 0;
        }
        if (!$link) {
            print "no linking\n";
            Configure::Util::removeFile ($obj);
            return 1;
        }
        my @linkoptions;
        foreach my $option (@$options) {
            if ($option =~ /link:(.*)/) {
                push @linkoptions, $1;
            }
        }
        my $linkrc = Configure::Util::vscript ("linklnk", '-BuildNrFunction=', '-o', $executable, $obj, @linkoptions);
        Configure::Util::removeFile ($obj);
        return !$linkrc;
    }

}

#----------------------------------------------------------------------#

{
    # package: just to keep it out of the main namespace
    package Configure::Util;

    my $keepFiles = 0;
    my $verbose = 0;
    my $scriptdir = "$ENV{TOOL}/bin";
    if (-d "$scriptdir/vmake" ) {
        $scriptdir .= "/vmake";
    }


    sub removeFile {
        if (!$Configure::Util::keepFiles) {
            foreach my $fname (@_) {
                unlink $fname;
            }
        }
    }

    sub vsystem {
        if ($Configure::Util::verbose) {
            print "system (@_)";
        }
        my $rc = system (@_);
        if ($Configure::Util::verbose) {
            print " -> $rc\n";
        }
        return $rc;
    }

    sub vscript {
        my ($script, @args) = @_;
        vsystem ($ENV{TOOLSHELL}, "$scriptdir/$script.pl", @args);
    }

    sub readPredefinedVariables {
        my %fixedValues;
        if (! defined $ENV{XCOMP}) {
            return %fixedValues;
        }
        my $fixedFile = "$ENV{WRK}/obj/SAPDB/RunTime/RTE_Configure.fixed";
        if (-r $fixedFile) {
            open FIXEDFILE, "<$fixedFile";
            while (<FIXEDFILE>) {
                if (/#/) {
                    next;
                }
                if (/(.*)=(.*)/) {
                    $fixedValues{$1} = $2;
                }
            }
            close FIXEDFILE;
        }
        return %fixedValues;
    }

    sub detectVariableValue {
        my ($variable, $predefined) = @_;
        my $predefinedValue = $predefined->{$variable->{name}};
        if (defined $predefinedValue) {
            if (!$predefinedValue) {
                $predefinedValue = undef;
            }
            $variable->{value} = $predefinedValue;
            $variable->{comment} .= "\n * !!! Predefined for XCOMP cross compilation"
                                  . "\n * !!! in file RTE_Configure_*.fixed";
        }
        else {
            $variable->detect ();
        }
    }

    sub usage {
        print <<EOF
usage: RTE_Configure.pl [-v] [-V] [-k] [-T <variable>]

    creates \$WRK/incl/SAPDB/RunTime/RTE_Configure.h

    -v              verbose: show variables during detection phase
    -V              very verbose: -v + show system calls
    -k              keep files
    -T <variable>   detect only one variable and write result to stdout
EOF
    ;
    exit 0;
    }
}

#----------------------------------------------------------------------#

{
    # package: feature tests that are too long to keep them inline
    package RTE_ConfigureSources;

    $RTE_ConfigureSources::testNPTL = <<EOF
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_EXIT_WITHOUT_RESULT             (1)
#define PROGRAM_EXIT_LINUXTHREAD_BEHAVIOUR      (42)
#define PROGRAM_EXIT_STANDARD_PTHREAD_BEHAVIOUR (0)
pid_t mainProcessId = 0;
void *testThreadMain(void *arg)
{
        pid_t threadProcessId = getpid();
        FILE * resultfile = fopen ("sdbtest.out", "w");
        if ( threadProcessId == mainProcessId )
        {
                fclose (resultfile);
                exit(0);
        }
        else
        {
                fprintf (resultfile, "1");
                fclose (resultfile);
                exit(0);
        }
        return arg;
}

int main(int argc, char *argv[])
{
        int errCode = 0;
        pthread_attr_t attr;
        pthread_t testThread;
        void *joinValue = 0;
        mainProcessId = getpid();
        errCode = pthread_attr_init(&attr);
        if ( errCode != 0 )
        {
                exit(PROGRAM_EXIT_WITHOUT_RESULT);
        }
#ifdef PTHREAD_CREATE_UNDETACHED
        errCode = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_UNDETACHED);
#endif
#ifdef PTHREAD_CREATE_JOINABLE
        errCode = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
#endif
        if ( errCode != 0 )
        {
                exit(PROGRAM_EXIT_WITHOUT_RESULT);
        }
        errCode = pthread_create(&testThread, &attr, testThreadMain, &mainProcessId);
        if ( errCode != 0 )
        {
                exit(PROGRAM_EXIT_WITHOUT_RESULT);
        }
        errCode = pthread_join(testThread, &joinValue);
        if ( errCode != 0 )
        {
                exit(PROGRAM_EXIT_WITHOUT_RESULT);
        }
        return PROGRAM_EXIT_WITHOUT_RESULT;
}
EOF
}

#----------------------------------------------------------------------#

sub main {
    getopts ('hkvVT:');
    if ($main::opt_h) {
        Configure::Util::usage ();
    }
    $Configure::Util::keepFiles = $main::opt_k;
    $Configure::Util::verbose = $main::opt_V;
    my $testVar = $main::opt_T;
    my $verbose = $main::opt_v || $main::opt_V;

    my @variables = (
        new RTE_RunDetection ("SDB_BYTE_ORDER_MOST_SIGNIFICANT_BYTE_FIRST",
             "true if byte order is big endian 1 = [0001]\n"
            . " * not defined if byte order is little endian 1 = [1000]",
            RTE_Templates::simpleMain (<<EOF
    int i1 = 1;
    char * aschar = (char *) &i1;
    if (aschar [0] == 0) {
        fprintf (result, \"1\");
    }

EOF
        )
            ),
        new RTE_RunDetection ("SDB_BIT64",
             "true if 64bit address space is supported\n"
            . " * not defined if only 32bit address space is supported",
            RTE_Templates::simpleMain (<<EOF
    if (sizeof (void*) >= 8) {
        fprintf (result, \"1\");
    }

EOF
            )
            ),
        new RTE_Int8Detection ("SDB_INT8_TYPE",
            "int type for 8 bye integer"),

        new RTE_RunDetection ("SDB_THREAD_CREATES_PROCESS_ID",
            "true if old style Linux threads are used",
            $RTE_ConfigureSources::testNPTL,
            "link: -lpthread", 'restrict: $^O =~ /linux/'),
        new RTE_IncludeDetection ("HAS_PAM_SUPPORT", "security/pam_appl.h",
            'restrict: $ENV{OS} eq "UNIX"'),
        new RTE_ConstPamMessageParameter ("HAS_CONST_PAM_MESSAGE_PARAMETER",
            "parameter msg of PAM_conversation is const",
            "configprecondition: HAS_PAM_SUPPORT"),
        new RTE_IncludeDetection ("SDB_HAS_SYS_ATOMIC", "sys/atomic.h",
            'restrict: $ENV{OS} eq "UNIX"'),
    );

    my %predefinedValues = &Configure::Util::readPredefinedVariables;

    my $configure =  new RTE_Configure;

    #$Configure::Util::verbose = 1; # when -V can't be passed to remimf
    #$verbose = 1;                  # when -v can't be passed to remimf
    #$testVar = 'SDB_BYTE_ORDER_MOST_SIGNIFICANT_BYTE_FIRST';     # when -T can't be passed to remimf

    if ($testVar) {
        #
        # test only one variable
        #
        my $variableFound = 0;
        foreach my $variable (@variables) {
            if ($variable->{name} eq $testVar) {
                if ($variable->isApplicable ()) {
                    &Configure::Util::detectVariableValue ($variable, \%predefinedValues);
                    #$variable->detect ();
                }
                print $variable->cdefine ();
                $variableFound = 1;
            }
        }
        if (!$variableFound) {
            print "Variable $testVar not in list\n";
            exit 2;
        }
    }
    else {
        #
        # test all variables and write output
        #
        foreach my $variable (@variables) {
            if ($verbose) {
                print $variable->{name} . "\n";
            }
            if ($variable->isApplicable ($configure)) {
                &Configure::Util::detectVariableValue ($variable, \%predefinedValues);
                #$variable->detect ();
            }
            $configure->addVar ($variable);
        }
        if (!$configure->fullfillsMinimumRequirements ()) {
            die "Build environment does not meet all requirements";
        }
        $configure->genCHeader ();
        $configure->genToolvars ();
    }

}

&main ();

