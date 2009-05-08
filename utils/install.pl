#!/usr/bin/perl

# Simple script (for now) to run against the Memcached UDFs

use strict;
use Getopt::Long;
use DBI;
use Data::Dumper;
use Carp qw(croak);

our $opt_debug;
our $opt_verbose;
our $opt_host;
our $opt_schema;
our $opt_user;
our $opt_password;
our $opt_silent;
our $opt_help;

GetOptions (
    'd|debug'       => \$opt_debug,
    'h|host=s'      => \$opt_host,
    'h|help'        => \$opt_help,
    'p|password=s'  => \$opt_password,
    's|silent'      => \$opt_silent,
    'u|user=s'      => \$opt_user,
    'v|verbose'     => \$opt_verbose,
) or usage();

usage() if $opt_help;

my $funclist = {
    memc_servers_set            => 'INT',
    memc_set                    => 'INT',
    memc_set_by_key             => 'INT',
    memc_add                    => 'INT',
    memc_add_by_key             => 'INT',
    memc_cas                    => 'INT',
    memc_cas_by_key             => 'INT',
    memc_get                    => 'STRING',
    memc_get_by_key             => 'STRING',
    memc_delete                 => 'INT',
    memc_delete_by_key          => 'INT',
    memc_append                 => 'INT',
    memc_append_by_key          => 'INT',
    memc_prepend                => 'INT',
    memc_prepend_by_key         => 'INT',
    memc_increment              => 'INT',
    memc_decrement              => 'INT',
    memc_replace                => 'INT',
    memc_replace_by_key         => 'INT',
    memc_servers_behavior_set   => 'INT',
    memc_behavior_get           => 'STRING',
    memc_servers_behavior_get   => 'STRING',
    memc_behavior_set           => 'INT',
    memc_list_behaviors         => 'STRING',
    memc_list_hash_types        => 'STRING',
    memc_list_distribution_types => 'STRING',
    memc_libmemcached_version   => 'STRING',
    memc_udf_version            => 'STRING',
    memc_server_count           => 'INT',
    memc_stats                  => 'STRING',
    memc_stat_get_value         => 'STRING',
    memc_stat_get_keys          => 'STRING',
};

my $types = { 
    0 => 'STRING',
    2 => 'INT',
};


$opt_schema ||= 'test';
$opt_user   ||= 'root';
$opt_host   ||= 'localhost';
my $existing_functions;

my $dbh= DBI->connect("DBI:mysql:$opt_schema", $opt_user, $opt_password) 
    or croak "Unable to connect! $DBI::errstr\n";

my $sth= $dbh->prepare('select name,ret from mysql.func') 
    or croak "Unable to fetch functions! $DBI::errstr\n";
$sth->execute();

my $ref= $sth->fetchall_arrayref();
$sth->finish();
map {$existing_functions->{$_->[0]} = $types->{$_->[1]}} @$ref; 

for my $func(keys %$funclist) {
    my $ans= '';
    unless ($existing_functions->{$func}) {
        print "function $func doesn't exist.";
        unless ($opt_silent) {
            print " Create? [Y|n]\n";
            $ans= <STDIN>;
            chomp($ans);
        } 
        else {
            $ans= 'Y';
        }
        if ($ans eq 'Y' or $ans eq 'y') {
            my $create= 'CREATE FUNCTION ' . $func . 
                ' RETURNS ' . $funclist->{$func} .
                " SONAME 'libmemcached_functions_mysql.so'"; 
            print "Running: $create\n" unless $opt_silent;
            $sth= $dbh->prepare($create);
            $sth->execute() or croak "Error: $DBI::errstr\n";
        }

    }
    else {
        print "function $func does exist\n";
    }
}

sub usage {
    my ($msg)= @_;
    print "ERROR: $msg\n\n";
    print <<EOUSAGE;
    d|debug             Debug flag 
    h|host=s            Host  
    p|password=s        Password 
    s|silent=s          Silent - please don't ask me, just do it! 
    u|user=s            DB Username 
    v|verbose           Verbosity 

EOUSAGE

    exit(0);
}
1;
