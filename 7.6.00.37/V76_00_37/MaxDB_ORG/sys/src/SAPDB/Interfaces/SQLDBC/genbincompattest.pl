#!perl

#foreach $N (keys %ENV) {
#    print $N . "\n";
#}
#die "test";

use English;
use File::Path;

if($ARGV[0] eq 'create') {
  exit(create_test());
}


$OUTPUTPATH  = "$ENV{'WRK'}/src/SAPDB/Interfaces/SQLDBC/tests";
$OUTFILENAME = "$ENV{'WRK'}/src/SAPDB/Interfaces/SQLDBC/tests/testbincompat.cpp";

File::Path::mkpath($OUTPUTPATH, 0, 0777) if ( ! -d $OUTPUTPATH);

open OUTFILE, ">$OUTFILENAME";


$SRCFILE="$ENV{'SRC'}/SAPDB/Interfaces/SQLDBC/SQLDBC.h";

$INFILENAME="$ENV{'WRK'}/incl/SAPDB/Interfaces/SQLDBC/SQLDBC.h";
$VARNAMECOUNTER = 1;

#print "INPUT FILE : $INFILENAME \n";

$SRCMD5 = get_md5($SRCFILE);

print OUTFILE <<"ENDE";
#define SQLDBC_H_MD5 $SRCMD5
#ifdef VMAKE
#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#else
#include "SQLDBC.h" /*nocheck*/
#endif

using namespace SQLDBC;

int main(int argc, char **argv)
{
ENDE
#---------------------------------------------------------------------------

$FILECONTENT = load_file($INFILENAME);
$STRIPPED_CONTENT="";

while(length($FILECONTENT) > 0) {
    if($STATE == 0) {
        if($FILECONTENT =~/\/\*/) {
            $STATE = 1;
            $FILECONTENT = $';
            $STRIPPED_CONTENT = $STRIPPED_CONTENT . $PREMATCH;
            next;
        } else {
            $STRIPPED_CONTENT = $STRIPPED_CONTENT . substr($FILECONTENT, 0, 1);
            $FILECONTENT = substr($FILECONTENT, 1);
            next;
        }
    } elsif($STATE == 1) {
        if($FILECONTENT =~ /^\*\//) {
            $FILECONTENT = $';
            $STATE = 0;
            next;
        } else {
            $FILECONTENT = substr($FILECONTENT, 1);
        }
    }
}

@LINES = split("\n", $STRIPPED_CONTENT);
$STRIPPED_CONTENT = "";
$IGNORE = 0;

foreach $line (@LINES) {

    # need to catch newer methods too
    if($line =~/#ifdef SQLDBC_FEATURE_TRACEOPTION/) {
        next;
    }
    elsif($line =~/^#ifdef/) {
        $IGNORE = 1;
        next;
    } elsif($line =~/^#else/) {
        $IGNORE = 0;
        next;
    } elsif($line =~/^#endif/) {
        $IGNORE = 0;
        next;
    } elsif($line =~/^#define/) {
        next;
    } elsif($line =~/^#undef/) {
        next;
    } elsif($line =~/^#ifndef __cplusplus/) {
        $IGNORE = 1;
        next;
    } elsif($line =~/^#ifndef/) {
        next;
    } elsif($line =~/^#include/) {
        next;
    } else {
        if($IGNORE == 0) {
            $STRIPPED_CONTENT .= $line . " ";
        }
    }
}

$token = next_token();

while($token ne '### END OF INPUT ###') {
    push @TOKENS, $token;
    # print OUTFILE "TOKEN |" . $token . "|\n";
    $token = next_token();
}

$NUMTOKENS = $#TOKENS;


# want not pick something back at end
push @TOKENS, "";
push @TOKENS, "";
push @TOKENS, "";
push @TOKENS, "";
push @TOKENS, "";

$INDEX = 0;

while($INDEX <= $NUMTOKENS) {
    parse_next("");
}

sub parse_next
{
    #print "parse_next \n";

    my $stop_indicator = shift;
    if($TOKENS[$INDEX] eq $stop_indicator) {
        return 1;
    }

    if($TOKENS[$INDEX] eq 'class') {
        $CLASS_ELEMENT_PROTECTION = 0;
        parse_class();
    }
    elsif($TOKENS[$INDEX] eq 'struct') {
        $CLASS_ELEMENT_PROTECTION = 1;
        parse_class();
    }
    elsif($TOKENS[$INDEX] eq 'namespace') {
        parse_namespace();
    }
    elsif($TOKENS[$INDEX] eq 'static') {
        skip_until(';');
    }
    else {
        parse_global_declaration();
    }

    return 0;
}

#---------------------------------------------------------------------------
sub skip_until
{
    my $skipvalue = shift;

    while($TOKENS[$INDEX] ne $skipvalue)  {
        # print "SKIP " . $TOKENS[$INDEX] . "\n";
        die_if_at_end();
        ++$INDEX;
    }
    ++$INDEX;
}

#---------------------------------------------------------------------------
sub die_if_at_end
{
    if($INDEX > $NUMTOKENS) { die "Unexpected end of file."; }
}


#---------------------------------------------------------------------------
sub parse_class
{
    # print "parse_class \n";

    if($TOKENS[$INDEX + 2] eq ';') {
        # print "CLASS DECL ONLY " . $TOKENS[$INDEX + 1] . "\n";
        $INDEX += 3;
        return;
    }

    my $classname = $TOKENS[$INDEX + 1];
    # print "CLASS NAME " . $TOKENS[$INDEX + 1] . "\n";

    push @NAME_SCOPE, $classname;

    skip_until('{');

    while($INDEX <= $NUMTOKENS) {
        if(parse_next_class_element($classname) == 1) {
            ++$INDEX;
            ++$INDEX;
            pop @NAME_SCOPE;
            return;
        }
    }
}

$CLASS_ELEMENT_PROTECTION = 0;

#---------------------------------------------------------------------------
sub parse_next_class_element
{
    my $classname = shift;

    if($TOKENS[$INDEX] eq '}') {
        return 1;
    }

    if($TOKENS[$INDEX] eq 'public') {
        if($TOKENS[$INDEX + 1] ne ':') {
            die "Expected : after public.";
        }
        $INDEX += 2;
        $CLASS_ELEMENT_PROTECTION = 1;
        return 0;
    }
    elsif($TOKENS[$INDEX] eq 'private' ||
          $TOKENS[$INDEX] eq 'protected') {
        if($TOKENS[$INDEX + 1] ne ':') {
            die "Expected : after" . $TOKENS[$INDEX] . ".";
        }
        $INDEX += 2;
        $CLASS_ELEMENT_PROTECTION = 0;
        return 0;
    }
    elsif($TOKENS[$INDEX] eq $classname &&
          $TOKENS[$INDEX + 1] eq '(') {
        parse_constructor($classname);
        return 0;
    } elsif($TOKENS[$INDEX] eq '~' ||
        ($TOKENS[$INDEX] eq 'virtual' && $TOKENS[$INDEX] eq '~')) {
        parse_destructor($classname);
        return 0;
    } else {
        parse_declaration($classname);
    }

}

sub parse_constructor
{
    if($CLASS_ELEMENT_PROTECTION == 0) {
        skip_until(";");
        return;
    }

    my $simple_classname = shift;

    my $classname = make_full_classname($simple_classname);
    my $varname = "var_" . $VARNAMECOUNTER;
    $VARNAMECOUNTER++;

    my $outstring  = "    {\n";
       $outstring .= "        " . $classname . "* " . $varname . " = new " . $classname . "(";

    skip_until("(");

    $arguments = parse_and_fill_argument_list($simple_classname);

    if($arguments eq "SKIP DECLARATION") {
        return;
    }

    $outstring .= $arguments;
    $outstring .= ");\n";
    $outstring .= "        delete " . $varname . ";\n";
    $outstring .= "    }\n";

    print OUTFILE $outstring;
}

sub make_full_classname
{
    my $result = "";
    foreach $s (@NAME_SCOPE) {
        $result = $result . "::" . $s;
    }
    return $result;
}

sub parse_and_fill_argument_list
{
    my $fulldecl;
    my $arguments = "";
    my $classname = shift;
    while($TOKENS[$INDEX] ne ")") {
        $fulldecl = "";
        while($TOKENS[$INDEX] ne ',' &&
              $TOKENS[$INDEX] ne ')') {
              if(length($fulldecl) != 0) {
                    $fulldecl .= ' ';
              }
              $fulldecl .= $TOKENS[$INDEX];
              ++$INDEX;
              die_if_at_end();
        }
############################################################################
# Insert matchings for new argument types of constructors here.
############################################################################
        if($fulldecl =~ /const SQLDBC_ConnectProperties &/) {
            $arguments .= "::SQLDBC::SQLDBC_ConnectProperties()";
        }
        elsif($fulldecl =~/SAPDBMem_IRawAllocator &/) {
            # SKIP INTERNAL METHOD
            if($classname eq "SQLDBC_ConnectProperties") {
                return "SKIP DECLARATION";
            }
            $arguments .= "RTEMem_Allocator::Instance()";
        }
        elsif($fulldecl =~/SQLDBC_IRuntime \*/) {
            $arguments .= "::SQLDBC::GetClientRuntime(0, 0)";
        }
        else {
            die "Unhandled constructor argument type $fulldecl ";
        }

        if($TOKENS[$INDEX] eq ',') {
            $arguments .= ", ";
            ++$INDEX;
        }
    }
    return $arguments;
}

sub parse_destructor
{
    my $classname = shift;
    skip_until(";");
}

sub filter_type
{
    my $name = shift;
    if($name eq 'ParameterMode') {
        return "SQLDBC_ParameterMetaData::ParameterMode";
    }
    elsif($name eq "ParameterNullBehavior") {
        return "SQLDBC_ParameterMetaData::ParameterNullBehavior";
    }
    elsif($name eq "ColumnNullBehavior") {
        return "SQLDBC_ResultSetMetaData::ColumnNullBehavior";
    }
    elsif($name eq "ResultSetType") {
        return "SQLDBC_Statement::ResultSetType";
    }
    elsif($name eq "ConcurrencyType") {
        return "SQLDBC_Statement::ConcurrencyType";
    }
    else {
        return $name;
    }
}

sub parse_declaration
{
    my $classname = shift;

    if($TOKENS[$INDEX] eq 'inline') {
        skip_until("}");
        if($TOKENS[$INDEX] eq ';') {
            $INDEX ++;
        }
        return;
    }

    if($CLASS_ELEMENT_PROTECTION == 0) {
        skip_until(";");
        return;
    }

    my @DECL_TOKENS;
    my @TYPE_TOKENS;
    my @RUMP_TOKENS;
    my $brace_found = 0;
    my $methodname = "";
    while($TOKENS[$INDEX] ne ";") {
        if($TOKENS[$INDEX] eq '(') {
            @TYPE_TOKENS = @DECL_TOKENS;
            $brace_found = 1;
        }
        push @DECL_TOKENS, $TOKENS[$INDEX];
        if($brace_found == 1) {
            push @RUMP_TOKENS, $TOKENS[$INDEX];
        }
        ++$INDEX;
    }
    # Skip non-functions.
    if($brace_found == 0) {
        ++$INDEX;
        return;
    }

    my $methodname = $TYPE_TOKENS[$#TYPE_TOKENS];
    my $operator_found = 0;
    if($TYPE_TOKENS[0] eq 'operator') {
        $operator_found = 1;
        $methodname = "";
        for($i=0; $i<=$#TYPE_TOKENS; ++$i) {
            $methodname .= $TYPE_TOKENS[$i];
            if(i != $#TYPE_TOKENS) {
                $methodname .= " ";
            }
        }
    }

    my $varname = "var_" . $VARNAMECOUNTER;
    ++$VARNAMECOUNTER;



    my $output = "    ";
    for($i = $operator_found; $i<$#TYPE_TOKENS + $operator_found; ++$i) {
        $output .= filter_type($TYPE_TOKENS[$i]);
        $output .= " ";
    }

    # print "METHOD " . $methodname . "\n";

    $output .= "(";
    my $full_classname = make_full_classname();
    $output .= $full_classname . "::* ";
    $output .= $varname;
    $output .= ")";
    for($i = 0; $i<=$#RUMP_TOKENS; ++$i) {
        if($RUMP_TOKENS[$i] eq '(') {
            $output .= "(";
            next;
        }
        if($RUMP_TOKENS[$i + 1] eq '=') {
            $i += 2;
            next;
        }
        if($RUMP_TOKENS[$i + 1] ne "," &&
           $RUMP_TOKENS[$i + 1] ne ")") {
            $output .= filter_type($RUMP_TOKENS[$i]);
            $output .= " ";
        }
    }

    $output .= " =  &" . $full_classname . "::" . $methodname . ";\n";

    print OUTFILE $output;

    ++$INDEX;
    return;
};

sub parse_global_declaration
{
    skip_until(";");
}

#---------------------------------------------------------------------------
sub parse_namespace
{
    if($TOKENS[$INDEX + 2] eq ';') {
        $INDEX += 3;
        return;
    }
    push @NAME_SCOPE, $TOKENS[$INDEX + 1];
    # print "NAMESPACE NAME " . $TOKENS[$INDEX + 1] . "\n";
    if($TOKENS[$INDEX + 2] ne '{') {
        die "Expected { after namespace";
    }
    $INDEX += 3;
    while($INDEX <= $NUMTOKENS) {
        if(parse_next("}") == 1) {
            ++$INDEX;
            pop @NAME_SCOPE;
            if($TOKENS[$INDEX] eq ';') {
                ++$INDEX;
            }
            return;
        }
    }
    return;
}

#---------------------------------------------------------------------------
print OUTFILE <<"ENDE";
    SQLDBC_IRuntime * (*glob1)(char *, const SQLDBC_Int4) = &GetClientRuntime;
    SQLDBC_IRuntime * (*glob2)(char *, const SQLDBC_Int4) = &GetSingleThreadClientRuntime;
    return 0;
}
ENDE
close OUTFILE;

sub load_file
{
    my $filename = shift;
    open INFILE, "<$filename";
    #my $sep = $/;
    local $/;
    $result = <INFILE>;
    #$/ = $sep;
    close INFILE;
    return $result;
}

sub next_token
{
    if($STRIPPED_CONTENT =~ /^(\s+)/) {
        $STRIPPED_CONTENT = $POSTMATCH;
    }

    if($STRIPPED_CONTENT =~ /^(\w+)/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        if($1 eq 'SQLDBC_DLLEXPORT') {
            return next_token();
        }
        return $1;
    }
    if($STRIPPED_CONTENT =~ /^;/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return ";";
    }
    if($STRIPPED_CONTENT =~ /^\*/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return "*";
    }
    if($STRIPPED_CONTENT =~ /^\&/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return "&";
    }
    if($STRIPPED_CONTENT =~ /^=/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return "=";
    }
    if($STRIPPED_CONTENT =~ /^::/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return "::";
    }
    if($STRIPPED_CONTENT =~ /^{/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return "{";
    }
    if($STRIPPED_CONTENT =~ /^}/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return "}";
    }
    if($STRIPPED_CONTENT =~ /^:/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return ":";
    }
    if($STRIPPED_CONTENT =~ /^\(/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return "(";
    }
    if($STRIPPED_CONTENT =~ /^\)/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return ")";
    }
    if($STRIPPED_CONTENT =~ /^~/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return "~";
    }
    if($STRIPPED_CONTENT =~ /^,/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return ",";
    }
    if($STRIPPED_CONTENT =~ /^(\"[^\"]+\")/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return $1;
    }
    if($STRIPPED_CONTENT =~ /^([0-9]+)/) {
        $STRIPPED_CONTENT = $POSTMATCH;
        return $1;
    }
    if(length($STRIPPED_CONTENT) == 0) {
        return '### END OF INPUT ###';
    }
    die "Unexpected: " . substr($STRIPPED_CONTENT, 0, 100) . " \n";
}

sub get_md5
{
    my $filename = shift;
    my $command = "md5sum " . $filename . "|";
    my $md5sum = undef;
    open MD5PIPE, $command;
    while(<MD5PIPE>) {
        if(/([0-9a-f]+)/) {
            $md5sum = $1;
        }
    }
    close MD5PIPE;
    if(!defined $md5sum) {
        return "undefined_md5";
        # die "Cannot determine MD5 of " . $filename;
    }
    return $md5sum;
}


#----------------------------------------------------------------------
# Use for actually creating the binary compatibility test.
#----------------------------------------------------------------------
sub create_test
{
    print "------------------------------------------------------------\n";
    print "Creating binary compatibility test.\n";
    print "------------------------------------------------------------\n";

    $OWN = $ARGV[1];
    $SRCDIR = $OWN . "/sys/src";

    $changelist = check_sqldbc_version();

    print "Current changelist of SQLDBC.h is " . $changelist ."\n";

    if(exists_testsource($changelist)) {
        print "Test source exists already, only updating the source.\n";
        check_current_test($changelist);
        copy_test($changelist);
    } else {
        print "Create new test from scratch.\n";
        create_new_test($changelist);
    }

    return 0;
}

sub check_sqldbc_version
{
    my $command = "p4 fstat " . $SRCDIR . "/SAPDB/Interfaces/SQLDBC/SQLDBC.h";
    my $headrev = undef;
    my $haverev = undef;
    my $headchange = undef;
    open P4PIPE, $command . "|";
    while(<P4PIPE>) {
        # print $_;
        if(/headRev ([0-9]+)/) {
            $headrev = $1;
        }
        if(/haveRev ([0-9]+)/) {
            $haverev = $1;
        }
        if(/headChange ([0-9]+)/) {
            $headchange = $1;
        }
    }
    close P4PIPE;

    if(!defined $headrev ||
       !defined $haverev ||
       !defined $headchange) {
       die "Perforce status information for SQLDBC.h not found.";
    }

    # print "Head revision is " . $headrev;

    if($headrev == $haverev) {
        return $headchange;
    } else {
        return get_current_sqldbc_changelist($haverev, $headrev);
    }
}

sub get_current_sqldbc_changelist
{
    my $revision = shift;
    my $currentrevision = shift;
    my $change = undef;
    my $command = "p4 changes " . $SRCDIR . "/SAPDB/Interfaces/SQLDBC/SQLDBC.h";
    open P4PIPE, $command . "|";
    while(<P4PIPE>) {
        if($revision != $currentrevision) {
            --$currentrevision;
            next;
        } else {
            /Change ([0-9]+)/;
            $change = $1;
            last;
        }
    }
    close P4PIPE;
    if(!defined $change) {
        die "Cannot determine the change list for revision " . $revision . " of SQLDBC.h";
    }
    return $change;
}

sub exists_testsource
{
    my $changelist = shift;
    my $filename = $SRCDIR . "SAPDB/Interfaces/SQLDBC/tests/testbincompat_ " .$changelist .".cpp";
    if(-x $filename) {
        return 1;
    } else {
        return 0;
    }
}

sub get_test_md5
{
    open TESTSOURCE, "<" . $OWN . "/sys/wrk/src/SAPDB/Interfaces/SQLDBC/tests/testbincompat.cpp"
        or die "Cannot open generated binary compatibility test.";

    my $md5sum = undef;
    while(<TESTSOURCE>) {
        if(/#define SQLDBC_H_MD5 ([0-9a-f]+)/) {
            $md5sum = $1;
            last;
        }
    }
    close TESTSOURCE;
    if(!defined $md5sum) {
        die "Binary test does not contain the MD5 of SQLDBC.h";
    }
    return $md5sum;
}


sub create_new_test
{
    my $changelist = shift;

    check_current_test($changelist);
    copy_test($changelist);
    add_description($changelist);
    add_maketarget($changelist);
    add_target($changelist);
}

sub check_current_test
{
    my $changelist = shift;
    my $sqldbc_md5 = get_md5($SRCDIR . "/SAPDB/Interfaces/SQLDBC/SQLDBC.h");
    my $test_md5 = get_test_md5();

    if($sqldbc_md5 ne $test_md5) {
        die "MD5 checksums of SQLDBC.h and the generated tests differ, rebuild target";
    }
    return;
}


sub copy_test
{
    my $changelist = shift;

    my $command = 'copy ' . $OWN
        . '\sys\wrk\src\SAPDB\Interfaces\SQLDBC\tests\testbincompat.cpp '
        . $OWN
        . '\sys\src\SAPDB\Interfaces\SQLDBC\tests\testbincompat_' . $changelist . ".cpp";
    if(system($command)) {
        die "Cannot copy test to destination.";
    }

    my $p4command = "p4 add " . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/testbincompat_" . $changelist . ".cpp";
    system($p4command);
    return;
}

sub add_description
{
    my $changelist = shift;

    my $p4editcmd = "p4 edit " . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/tests.com";
    if(system($p4editcmd)) {
        die "Cannot edit test description file.";
    }

    open I, "<" . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/tests.com";
    open O, ">" . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/tests.com.new";

    while(<I>) {
        if(/#!testbincompat/) {
            print O $_;
            print O "testbincompat_" . $changelist . '.cpp ascii ->$(OWN)/test/sqldbc/testbincompat_' . $changelist . ".cpp \n";
        } else {
            print O $_;
        }
    }

    close I;
    close O;

    $movecommand = "move /Y "
        . $OWN . '\sys\src\SAPDB\Interfaces\SQLDBC\tests\tests.com.new '
        . $OWN . '\sys\src\SAPDB\Interfaces\SQLDBC\tests\tests.com';

    if(system($movecommand)) {
        die "Cannot move patched description file.";
    }

}

sub add_maketarget
{
    my $changelist = shift;

    my $p4editcmd = "p4 edit " . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/bincompat.mk";
    if(system($p4editcmd)) {
        die "Cannot edit test description file.";
    }

    open I, "<" . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/bincompat.mk";
    open O, ">" . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/bincompat.mk.new";
    $mode = 0;
    while(<I>) {

        if($mode == 0) {
            if(/#testbincompat-program/) {
#                print O "  testbincompat_" . $changelist . '$(EXEEXT) ' . "\\\n";
                print O $_;
                $mode = 1;
                next;
            } elsif(/#testbincompat-object/) {
#                print O "  testbincompat_" . $changelist . '$(OBJEXT) ' . "\\\n";
                print O $_;
                $mode = 2;
                next;
            }
            if(/#testbincompat-rule/) {
                print O "testbincompat_" . $changelist . '$(OBJEXT) : testbincompat_' . $changelist . ".cpp\n";
                print O "\t" . ' $(CPP_COMPILE_PREFIX) testbincompat_' . $changelist . '.cpp $(CPP_COMPILE_SUFFIX)' . "\n\n";
                print O "testbincompat_" . $changelist . '$(EXEEXT) : testbincompat_' . $changelist . '$(OBJEXT)' . "\n";
                print O "\t" . '$(CPP_LINK_PREFIX) testbincompat_' . $changelist . '$(OBJEXT) $(CPP_LINK_SUFFIX)' . "\n\n";
                print O $_;
            } else {
                print O $_;
            }
        } elsif($mode == 1) {
            chomp;
            if(/\\$/) {
                print O $_ . "\n";
            } else {
                print O "  testbincompat_" . $changelist . '$(EXEEXT) ' . "\\\n";
                print O $_ . "\n";
                $mode = 0;
            }
        } elsif($mode == 2) {
            chomp;
            if(/\\$/) {
                print O $_ . "\n";
            } else {
                print O "  testbincompat_" . $changelist . '$(OBJEXT) ' . "\\\n";
                print O $_ . "\n";
                $mode = 0;
            }
        }

    }

    close I;
    close O;

    $movecommand = "move /Y "
        . $OWN . '\sys\src\SAPDB\Interfaces\SQLDBC\tests\bincompat.mk.new '
        . $OWN . '\sys\src\SAPDB\Interfaces\SQLDBC\tests\bincompat.mk';

    if(system($movecommand)) {
        die "Cannot move patched make file.";
    }

}

sub add_target
{
    my $changelist = shift;

    my $p4editcmd = "p4 edit " . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/bincompat.mac";
    if(system($p4editcmd)) {
        die "Cannot edit bincompat.mac.";
    }

    open I, "<" . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/bincompat.mac";
    open O, ">" . $OWN . "/sys/src/SAPDB/Interfaces/SQLDBC/tests/bincompat.mac.new";

    while(<I>) {
        if(/#testbincompat/) {
            print O $_;
            print O "testbincompat_" . $changelist . ".cpp ascii \n";
        } else {
            print O $_;
        }
    }

    close I;
    close O;

    $movecommand = "move /Y "
        . $OWN . '\sys\src\SAPDB\Interfaces\SQLDBC\tests\bincompat.mac.new '
        . $OWN . '\sys\src\SAPDB\Interfaces\SQLDBC\tests\bincompat.mac';

    if(system($movecommand)) {
        die "Cannot move patched bincompat.mac file.";
    }

}

