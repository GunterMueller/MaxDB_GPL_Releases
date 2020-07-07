#!/usr/bin/perl
#
# @(#)DBTools		1998-11-16
#
# G. Großmann, SAP AG
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

package DBTools;
use Carp;
use FileHandle;
require "sutvars$ENV{TOOLEXT}";
@EXPORT_OK = qw(start_db stop_db stopdump_db GetParam PutParam);

##################################################################
# Funktion : start_db
#
# Argumente :
#   1. Datenbankname
#   in Release 6.1 gilt:
#   2. der zu startende Kern
#   in anderen Releases:
#   2. Optionen für 'x_start' fast,quick,slow
#   3. colduser,password
#
# Rückgabe : keine
#
sub start_db
{
  my $db = shift;
  if ($ENV{'RELVER'} eq "R61")
  {
    my $dbvers = shift;
    system("x_start $dbvers $db > $NULL") == 0
           or croak "Error while executing \"x_start $dbvers $db > $NULL\"\n"."message: $!\n";
  }
  else
  {
    my $dbvers = shift;
    if ( $ENV{'RELVER'} ge "R72")
    {
       my $user = shift;
       $user = $StdColdUser if !defined($user);
       my $node = shift;
       my $remoteroot = shift;
       if ( $remoteroot and $node ) {
           system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $db -u $user db_clear > $NULL");
           DBMRetryCmd("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $db -u $user start $dbvers > $NULL");
       } else {
           system("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $db -u $user db_clear > $NULL");
           if ($ENV{'PURIFY'}) {
               die "no PURIFY support on Unix";
           }
           else {
               DBMRetryCmd("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $db -u $user start $dbvers > $NULL");
           }
       }
    }
    else
    {
       system("x_start $dbvers $db > $NULL") == 0
           or croak "Error while executing \"x_start $dbvers $db > $NULL\"\n"."message: $!\n";
    }
  }
}

##################################################################
# Funktion : stopdump_db
#
# Argumente :
#   1. Datenbankname
#   2. colduser,password
#
# Rückgabe : keine
#
sub stopdump_db
{
  my $db         = shift;
  my $user       = shift;
  my $node       = shift;
  my $remoteroot = shift;

  $user = $StdColdUser if !defined($user);
  if ($ENV{'RELVER'} ge "R72"  )
  {
      if ( $remoteroot and $node ) {
        system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $db -u $user db_stop -dump >$NULL 2>&1");
      } else {
        system("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $db -u $user db_stop -dump >$NULL 2>&1");
      }
  }
  else
  {
    system("x_stop $db dump >$NULL 2>&1");
  }

}

##################################################################
# Funktion : stop_db
#
# Argumente :
#   1. Datenbankname
#   2. colduser,password
#
# Rückgabe : keine
#
sub stop_db
{
  my $db   = shift;
  my $user = shift;
  my $node       = shift;
  my $remoteroot = shift;

  $user = $StdColdUser if !defined($user);
  if ($ENV{'RELVER'} ge "R72"  )
  {
      if ( $remoteroot and $node ) {
        system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $db -u $user stop >$NULL 2>&1");
      } else {
        system("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $db -u $user stop >$NULL 2>&1");
      }
  }
  else
  {
    system("x_stop $db >$NULL 2>&1");
  }

}

#################################################################
# Funktion : PutParam
#
# Argumente :
#   1. Datenbankname
#   2. der zu setzende Parameter
#   3. der wert des zu setzenden Parameters
#   4. colduser,password
#
# Rückgabe : keine
#
sub PutParam
{
    my ($Database, $Variable, $Value, $user, $node, $remoteroot) = @_;
    if ($ENV{RELVER} lt "R72" )
    {
        system("$ENV{INSTROOT}/pgm/putparam $Database $Variable $Value") == 0
            or croak "Error while executing \"$ENV{INSTROOT}/pgm/putparam $Database $Variable $Value\"\n".
            "message: $!\n";
    }
    else
    {
        $user = $StdColdUser if !defined($user);
      if ( $remoteroot and $node ) {
        system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $Database -u $user param_directput $Variable $Value") == 0
	   or croak "Error while executing param_directput\nfor more information see $DbmcliProt\n";
      } else {
        system("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $Database -u $user param_directput $Variable $Value") == 0
	   or croak "Error while executing param_directput\nfor more information see $DbmcliProt\n";
      }
    }
}


#################################################################
# Funktion : GetParam
#
# Argumente :
#   1. Datenbankname
#   2. der zu erfragende Parameter
#   3. colduser,password
#
# Rückgabe : der Wert des Parameters
#
sub GetParam
{
    my ($Database, $Variable, $user, $node, $remoteroot) = @_;
    local ($Value, $Status);
    if ($ENV{RELVER} lt "R72" )
    {
        open(PIPE_IN, "$ENV{INSTROOT}/pgm/getparam $Database $Variable |") ||
            croak "Error while executing \"$ENV{INSTROOT}/pgm/getparam $Database $Variable\"\n".
            "message: $!\n";
        chomp($Value = <PIPE_IN>);
        close(PIPE_IN);
    }
    else
    {
        $user = $StdColdUser if !defined($user);
        if ( $remoteroot and $node ) {
            open(PIPE_IN, "dbmcli -n $node -R $remoteroot -d $Database -u $user param_directget $Variable |") ||
                croak "Error while executing \"$ENV{INSTROOT}/pgm/dbmcli -d $Database -u $user param_directget $Variable\"\n".
                "message: $!\n";
        } else {
            open(PIPE_IN, "dbmcli -d $Database -u $user param_directget $Variable |") ||
                croak "Error while executing \"$ENV{INSTROOT}/pgm/dbmcli -d $Database -u $user param_directget $Variable\"\n".
                "message: $!\n";
        }
        chomp($Status = <PIPE_IN>);
        chomp($Value = <PIPE_IN>);
        close(PIPE_IN);
        die "Can't get parameter $Variable!\n$Value\n" if $Status !~ /OK/;
        ($Value) = $Value =~ /.+\s+(.*)/;
    }
    return $Value;
}


#################################################################
# Funktion : BCheck
#
# Argumente :
#   1. Datenbankname
#   2. colduser,password
#
# Rückgabe : keine
#
sub BCheck
{
    my ($Database, $user, $node, $remoteroot) = @_;

    if ( $ENV{'RELVER'} ge "R72")
    {
	local $fh = new FileHandle;
	if ( ! $fh->open("Check_Param.dat", "w") )
	{
	    die "Can't open Check_Param.dat (output) :$!\n";
	}
	else
	{
	    $fh->print("param_startsession\n");
	    $fh->print("param_checkall\n");
	    $fh->print("param_commitsession\n");
	}
	$fh->close;

    if ( $remoteroot and $node ) {
    	system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $Database -u $user -i Check_Param.dat") == 0
	        or croak "Error while executing BCHECK\nfor more information see $DbmcliProt\n";
    } else {
	    system("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $Database -u $user -i Check_Param.dat") == 0
	        or croak "Error while executing BCHECK\nfor more information see $DbmcliProt\n";
    }

    unlink ("Check_Param.dat");

    } else {

	system("x_param -u $user -d $Database BCHECK") == 0
            or croak "Error while executing BCHECK\n";

    }
}

#################################################################
# Funktion : BInit
#
# Argumente :
#   1. Datenbankname
#   2. colduser,password
#   3. os user,password
#   4. instance_type
#   5. node
#   6. remote dbroot
#
# Rückgabe : keine
#
sub BInit
{
    my ($Database, $user, $osuser, $instance, $node, $remoteroot) = @_;

    if ( $ENV{'RELVER'} ge "R72")
    {
	local $fh = new FileHandle;


    # drop db
	if (defined $node) {
        system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $Database -u $user db_drop");
    } else {
        system("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $Database -u $user db_drop");
    }

    # create db
	my $rc;
	if (defined $osuser) {
	    if (defined $node) {
	        DBMRetryCmd ("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot db_create $Database $user $osuser");
	    } else {
	        DBMRetryCmd ("dbmcli -o $DbmcliOutput -t $DbmcliProt -R $INSTROOT db_create $Database $user $osuser");
        }
	} else {
	    DBMRetryCmd ("dbmcli -s -o $DbmcliOutput -t $DbmcliProt -R $INSTROOT db_create $Database $user");
	}

    # create new PARAM file
	if ( ! $fh->open("Init_Param.dat", "w") )
	{
	    die "Can't open Init_Param.dat (output) :$!\n";
	}
	else
	{
	    $fh->print("param_startsession\n");
   	    $fh->print("param_init $instance\n");
	    $fh->print("param_checkall\n");
	    $fh->print("param_commitsession\n");
	}
	$fh->close;
    if ( $node ) {
	    system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $Database -u $user -i Init_Param.dat") == 0
	        or croak "Error while executing init_param\nfor more information see $DbmcliProt";
    } else {
    	system("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $Database -u $user -i Init_Param.dat") == 0
	        or croak "Error while executing init_param\nfor more information see $DbmcliProt";
    }

	unlink ("Init_Param.dat");

    # at win32 systems register other kernel versions
	if ( $^O =~ /mswin32/i) {
		if (($ENV{'RELVER'} lt "R74") || (($ENV{'RELVER'} eq "R74") && ( $ENV{'CORRECTION_LEVEL'} < 4 ))) {
	        if ( $node ) {
			    system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $Database -u $user db_register -quick");
			   	system("dbmcli -o $DbmcliOutput -t $DbmcliProt -n $node -R $remoteroot -d $Database -u $user db_register -slow");
		    } else {
			    if ( -f "$ENV{INSTROOT}/pgm/quickknl.exe" ) {
				    DBMRetryCmd("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $Database -u $user db_register -quick");
	    		}
		    	if ( -f "$ENV{INSTROOT}/pgm/slowknl.exe" ) {
			    	DBMRetryCmd("dbmcli -o $DbmcliOutput -t $DbmcliProt -d $Database -u $user db_register -slow");
	    		}
	        }
	     }
     }

} else {

	system("x_param -u $user -d $Database BINIT");

	if ($RELVER ne "R61"){
		if ( $^O =~ /mswin32/i) {
			if ( -f "$ENV{INSTROOT}/pgm/slowknl.exe" ) {
				system("xservice -d $Database -u -s");
			}
			if ( -f "$ENV{INSTROOT}/pgm/quickknl.exe" ) {
				system("xservice -d $Database -u -q");
			}
			if ( -f "$ENV{INSTROOT}/pgm/kernel.exe" ) {
				system("xservice -d $Database -u");
			}
		}
	}
}
}

#################################################################
# Funktion : DBMRetryCmd
#
# Argumente :
#   1. cmd
#
# Rückgabe : keine
#
sub DBMRetryCmd ()
{
    my ($cmd) = @_;

    system("$cmd") == 0
        or system("$cmd") == 0
        or system("$cmd") == 0
        or croak "Error while executing \"$cmd\"\nfor more information see $DbmcliProt";
}

#################################################################
# Funktion : kernelexe
#
# returns: the full path to the kernel executable
#

sub kernelexe {
    my ($dbvers) = @_;
    my ($basexe);
    if ($dbvers eq "-fast") {
        $baseexe = "kernel";
    }
    if ($dbvers eq "-quick") {
        $baseexe = "quickknl";
    }
    if ($dbvers eq "-slow") {
        $baseexe = "slowknl";
    }
    return "$ENV{INSTROOT}/pgm/$baseexe" . "_reloc.exe";
}

#################################################################
# Funktion : WaitForAdminState
#
# Arguments:
#      1. the full dbmcli command
#
sub WaitForAdminState {
    my ($dbmcli) = @_;
    my ($targetTime) = time + 120;

    while (time < $targetTime) {
        my $remaining = $targetTime - time;
        open (PIPE, $dbmcli . " |") or die;
        while (<PIPE>) {
            if (/.*ADMIN/) {
                return 1;
            }
        }
        close PIPE;
        sleep 1;
    }
    return 0;
}

#################################################################
# Funktion : GetDiagFile
#
# Argumente :
#   1. Name der Ausgabedatei
#
# Rückgabe : keine
#
sub GetDiagFile
{
    my ($Database, $user, $node, $remoteroot, $diagfile) = @_;

    if ( $remoteroot and $node ) {
    	system("dbmgetf -n $node -R $remoteroot -d $Database -u $user -k KNLDIAG -q >$diagfile") == 0
       or croak "Error while getting knldiag:\n $!\n";
    } else {
    	system("dbmgetf -d $Database -u $user -k KNLDIAG -q >$diagfile") == 0
       or croak "Error while getting knldiag:\n $!\n";
    }
}


