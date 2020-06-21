use ExtUtils::MakeMaker;

# Don't try to descend into Expat directory for testing

sub MY::test
{
  my $self = shift;

  my $hold = delete $self->{DIR};
  my $ret = $self->MM::test(@_);
  $self->{DIR} = $hold if defined($hold);
  $ret;
}

@extras = ();

push(@extras,
     CAPI => 'TRUE')
    if ($PERL_VERSION >= 5.005 and $OSNAME eq 'MSWin32'
	and $Config{archname} =~ /-object\b/i);

push(@extras,
     ABSTRACT_FROM => 'Parser.pm',
     AUTHOR        => 'Clark Cooper (coopercc@netheaven.com)')
    if ($ExtUtils::MakeMaker::Version >= 5.4301);
     
WriteMakefile(
              NAME	=> 'XML::Parser',
	      DIR	=> [qw(Expat)],
              dist      => {COMPRESS => 'gzip', SUFFIX => '.gz'},
              VERSION_FROM => 'Parser.pm',
	      @extras
             );

