#!/usr/bin/perl

# Simple script (for now) to run against the Memcached UDFs

use strict;
use Getopt::Long;
use DBI;
use Data::Dumper;
use Time::HiRes qw(gettimeofday tv_interval);

our $opt_debug;
our $opt_verbose;
our $opt_host;
our $opt_schema;
our $opt_user;
our $opt_password;
our $opt_schema;
our $opt_iterations;
our $opt_help;

GetOptions (
    'd|debug'       => \$opt_debug,
    'h|host=s'      => \$opt_host,
    'i|iterations=s'=> \$opt_iterations,
    'h|help'        => \$opt_help,
    'p|password=s'  => \$opt_password,
    's|schema=s'    => \$opt_schema,
    'u|user=s'      => \$opt_user,
    'v|verbose'     => \$opt_verbose,
) or usage();

usage() if $opt_help;

$opt_schema ||= 'test';
$opt_user   ||= 'root';
$opt_host   ||= 'localhost';
$opt_iterations ||= 1000;

my $dbh= DBI->connect("DBI:mysql:$opt_schema;mysql_socket=/tmp/mysql1.sock", $opt_user, $opt_password);

my $sth_set= $dbh->prepare("select memc_set(?, ?)");
my $sth_get= $dbh->prepare("select memc_get(?)");
my $sth_del= $dbh->prepare("select memc_delete(?)");
for (0 .. 1000) {
    my $key= 'key' . $_;
    my $value= 'value' . $_ x 10 ;
    my $t1= [gettimeofday()];
    print "Calling memc_set($key, $value);\n" if $opt_verbose;
    $sth_set->execute($key, $value);
    my $t2= [gettimeofday()];
    my $elapsed= tv_interval($t1, $t2);
    print "memc_set in $elapsed seconds.\n" if $opt_verbose;
    $t1= [gettimeofday()];
    print "Calling memc_get($key);\n" if $opt_verbose;
    $sth_get->execute($key);
    $t2= [gettimeofday()];
    $elapsed= tv_interval($t1, $t2);
    print "memc_get in $elapsed seconds.\n" if $opt_verbose;
    my $ret= $sth_get->fetch();
    print "returned: $ret->[0]\n" if $opt_verbose;
    print "Calling memc_delete($key, $value);\n" if $opt_verbose;
    $t1= [gettimeofday()];
    $sth_del->execute($key);
    $t2= [gettimeofday()];
    $elapsed= tv_interval($t1, $t2);
    print "deleted in $elapsed seconds\n" if $opt_verbose;
}


sub usage {
    my ($msg)= @_;
    print "ERROR: $msg\n\n";
    print <<EOUSAGE;
    d|debug             Debug flag 
    h|host=s            Host  
    i|iterations=s      Num Iterations of set, get, delete 
    p|password=s        Password 
    s|schema=s          Schema 
    u|user=s            DB Username 
    v|verbose           Verbosity 

EOUSAGE

    exit(0);
}
1;
