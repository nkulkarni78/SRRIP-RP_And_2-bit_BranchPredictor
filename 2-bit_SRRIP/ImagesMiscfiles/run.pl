#!/usr/bin/perl

@parsec_benchmarks = ('blackscholes', 'bodytrack', 'canneal', 'fluidanimate', 'streamcluster', 'swaptions', 'x264');

@spec_benchmarks = ('mcf','hmmer', 'sjeng', 'libquantum', 'xalan', 'milc', 'cactusADM', 'leslie3d', 'namd', 'soplex', 'calculix', 'lbm');

@modes = ('SRRIP');

foreach my $mode (@modes) {
  print "################################## \n";
  print "Running benchmarks with $mode \n";
  print "##################################\n";

  foreach my $bench (@parsec_benchmarks) {
    print "hw4runscript PARSEC $bench $mode\n";
    my $cmd = "sh hw4runscript PARSEC $bench $mode";
    system ($cmd);
  }

  foreach my $bench (@spec_benchmarks) {
    print "hw4runscript SPEC $bench $mode\n";
    my $cmd = "sh hw4runscript SPEC $bench $mode";
    system ($cmd);
  }
}
