#!/usr/bin/perl
##
###############################################################################
#  Brief:  The script demos the usage of TypeInfo.pm.
#
#  Usage:  readlog.pl [OPTIONS] [<funcname> ...];
#
#  OPTIONS
#    --struct,  -s <struct_name>
#        Specify the struct name to be parsed. 
#        Note that struct should be defined in a ${struct_name}.h header.
#    --binary,  -b <binary_file_name>
#        Specify the binary file name to be analyzed.
#    --include, -i<include_path>
#        Specify the search include path.
#    --help,    -h
#        print this help.
#
#  EAMPLES
#   - perl readlog.pl -s Employee -b Employee.bin
#   - perl readlog.pl -s Employee -b Employee.bin -i. -i../
#
#  (C) Copyright reserved by Nokia Siemens Networks. 2013. 
###############################################################################
BEGIN
{
    push @INC, "./";
    push @INC, "../";
};

use warnings;
use strict;
use TypeInfo; #use API: read_defs, print_data
use Getopt::Long qw/:config no_ignore_case gnu_getopt/;

my $opt_help;
my $opt_binary_file;
my $opt_struct_name;
my @opt_inc_dirs;

GetOptions("help|h"     => \$opt_help,
           "binary|b=s" => \$opt_binary_file,
           "struct|s=s"=>  \$opt_struct_name,
           "include|i=s"=> \@opt_inc_dirs
          ) or die "Unknown option";

#------------------------------------------------------------------------------
# (1) Show the help of this script.
#------------------------------------------------------------------------------
if($opt_help)
{
    &print_help();
    exit 0;
}

#------------------------------------------------------------------------------
# (2) Main cotrol flow of the script.
#    (2.1) Get all header files from specify directory.
#    (2.2) Retrieve the type definitions from header files.
#    (2.3) Generate the readable output for all inputs.
#------------------------------------------------------------------------------
if($opt_struct_name && $opt_binary_file)
{
    # (2.1) Get all header files from specify directory.
    my @files;
    my @default_dirs = (".");
    my @dirs = (@default_dirs, @opt_inc_dirs);
   
    foreach(@dirs)
    {
        if(opendir(DIR, $_))
        {
            @files = grep(/\.h$/,readdir(DIR));
            close DIR;
        }
        else
        {
            print "Could not open the file: $_ $! !\n";
            exit 1;
        }        
    }
    
    # (2.2) Retrieve the type definitions from header files.
    my $type_defs = retrieve_type_definitions(\@files, \@opt_inc_dirs);
    
    # (2.3) Generate the readable output for all inputs.
    print_single_output($opt_struct_name, $opt_binary_file, $type_defs);
    
    exit 0;
}
else
{
    # (2.4) Print the help if illegal parameters input.
    &print_help();
    exit 0;
}

## Sub Procedure: retrieve_type_definitions
#------------------------------------------------------------------------------
#   Print the readable output to the output_file.
#
#  \Output\
#   - type_defs:  type definitions returned
#------------------------------------------------------------------------------
sub retrieve_type_definitions
{    
    my ($files, $dirs) = @_;
    
    #  Specify current directory as the default
    my @type_dirs = (".");
    my @type_files = (); # None as default
    
    my @all_dirs = (@type_dirs, @$dirs); 
    my @all_files = (@type_files, @$files);
    my %opts; # Not used
       
    return read_defs(\@all_files, \@all_dirs, \%opts); 
}

## Sub Procedure: print_single_output
#------------------------------------------------------------------------------
#   Print the readable output to the output_file.
#
#  \Input\
#   - bin_file   :  the binary file name.
#   - struct_name:  struct name
#   - output_file:  output file name
#   - type_defs  :  type definitions.
#  \Output\
#   - output_file:  file created on current directory.    
#------------------------------------------------------------------------------
sub print_single_output
{
    my ($struct_name, $bin_file, $type_defs) = @_;
     
    my @bytes = convert_bin2arr($bin_file);
    my $lines = print_data(\@bytes, $struct_name, $type_defs);
    
    if($lines)
    {
        foreach (@$lines){ print $_;}
    }
    else
    {
        print "Please check the input! ! \n";
    }
}

## Sub Procedure: convert_bin2arr
#------------------------------------------------------------------------------
#   An adapter to read memory dump binary file and convert it into a byte array.
#
#  \Input\:
#   - file:     the binary file name.
#  \Output\:  
#   - array:    the numberic bytes array returned.
#------------------------------------------------------------------------------
sub convert_bin2arr
{
    my ($file) = @_;
    my $buffer;
    
    if(open(FILE, $file))
    {
        binmode FILE;
        my $line = read(FILE, $buffer, 65535);
        my @array = unpack('C*', $buffer);
        close FILE;        
        return @array;
    }
    else
    {
        print "Could not open the file: ${file} $! !\n";
        exit 1;
    }
}

## Sub Procedure: print_help
#------------------------------------------------------------------------------
# Show the help.
#------------------------------------------------------------------------------
sub print_help
{
    print 
"******************************************************************************
$0 [OPTIONS] [<funcname> ...];

The script demos the usage of TypeInfo.pm.

OPTIONS
    --message, -m <message_name>
        Specify the message name to be parsed.
    --binary,  -b <binary_file_name>
        Specify the binary file name to be analyzed.
    --include, -i<include_path>
        Specify the search include path.
    --help,    -h
        print this help.

EAMPLES
- perl readlog.pl -s Employee -b Employee.bin
- perl readlog.pl -s Employee -b Employee.bin -i. -i../
*****************************************************************************";
}    
