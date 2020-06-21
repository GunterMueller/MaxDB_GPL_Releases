#
# @(#)scriptex		1999-10-18
#
# D. Dittmar, SAP AG
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


package scriptex;


########################################################
#
# Function: script
#
# description: executes a script file.
#
#    Examines the first line to look for the name of
#    the script interpreter
#
########################################################

sub script {
    my (@args) = @_;

    $scriptName = @args [0];
    $scriptEngine = &getScriptEngine ($scriptName);
    #print "executing with $scriptEngine\n";
    $rc = system ("$scriptEngine @args");
    return $rc;
}

########################################################
#
# local Function: getScriptEngine
#
# description: extracts script engine name from first line
#
########################################################

sub getScriptEngine {
    my ($fname) = @_;
    my ($scriptFile) = ("scriptFile");
    my ($result);

    open $scriptFile, "<$fname";
    $line1 = <$scriptFile>;
    close $scriptFile;

    if ($line1 =~ /^#! *(.*)$/) {
        $result = $1;
    }
    else {
        die "script $fname must start with #!";
    }
    return $result;
}

1;

