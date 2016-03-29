#!/usr/bin/perl -w
#
# ==========================================================================
#
# ZoneMinder ONVIF Control Protocol Module
# Copyright (C) 2014  Jan M. Hochstein
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# ==========================================================================
#
# This module contains the implementation of the ONVIF capability prober
#

use Getopt::Std;

my $verbose = "";

sub probe {
    # Execute probes using SOAP 1.1 and 1.2 messages
    # Duplicate URLs are filtered
    my %data;
    my @output = (`zmonvif-probe11 $verbose`, `zmonvif-probe12 $verbose`);
    chomp @output;

    foreach my $line (@output)
    {
	my ($url) = split /,/, $line;
	if (not exists $data{$url}) {
	    print "$line\n";
	    $data{$url} = 1;
	}
    }
}



# ========================================================================
# options processing

$Getopt::Std::STANDARD_HELP_VERSION = 1;

our ($opt_v);

my $OPTIONS = "v";

sub HELP_MESSAGE
{
  my ($fh, $pkg, $ver, $opts) = @_;
  print $fh "Usage: " . __FILE__ . " [-v] probe \n";
  print $fh "       " . __FILE__ . " [-v] <command> <device URI> <soap version> <user> <password>\n";
  print $fh  <<EOF
  Commands are:
    probe     - scan for devices on the local network and list them
    profiles  - print the device's supported stream configurations
    info      - print some of the device's configuration settings
  Common parameters:
    -v        - increase verbosity
  Device access parameters (for all commands but 'probe'):
    device URL    - the ONVIF Device service URL
    soap version  - SOAP version (1.1 or 1.2)
    user          - username of a user with access to the device
    password      - password for the user
EOF
}

# ========================================================================
# MAIN

if(!getopts($OPTIONS)) {
  HELP_MESSAGE(\*STDOUT);
  exit(1);
}

if(defined $opt_v) {
  $verbose = "-v";
}

my $action = shift;

if(!defined $action) {
  HELP_MESSAGE(\*STDOUT);
  exit(1);
}

if($action eq "probe") {
    probe();
}
else {
# all other actions need URI and credentials
  my $url_svc_device = shift;
  my $soap_version = shift;
  my $username = shift;
  my $password = shift;

  die "Required parameter is missing" if not defined $password;

  if($action eq "profiles") {
    exec "zmonvif-profiles12 $verbose $url_svc_device $username $password";
  }
  elsif($action eq "info") {
    exec "zmonvif-info12 $verbose $url_svc_device $username $password";
  }
  elsif($action eq "move") {
    die "move action is not implemented";
  }
  elsif($action eq "metadata") {
    die "metadata action is not implemented";
  }
  else {
    print("Error: Unknown command\"$action\"");
    exit(1);
  }
}
