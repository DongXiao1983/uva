#read_tokenpackage TypeInfo;

# This module can read C code and extract type definitions.
# It can also apply raw binary data to a type definition which it has
# read the definition of in the C code and print it out in text,
# field by field.

# Explanation to some special terms used inside of this module:
#
#     Complex type
#         A type which is defined through a block. This is currently
#         a 'struct', 'union', or 'enum'.
#
#     Abstract type
#         A type which can consist of more than one sub type.
#         This is currently a 'struct' or a 'union'.
#
#     Unresolved type
#         A type which gets its name when it has no known definition.
#         Observe that an unresolved type currently continues to be
#         unresolved also if its definition later becomes known.
#
#     Declaration
#         The combination of a type and a type name. The type might or
#         might not contain a definition of the type.

require Exporter;
@ISA = qw/Exporter/;
@EXPORT = qw//;
@EXPORT_OK = qw//;

use strict;
use warnings;
#use Data::Dumper;

my %qualifier = (
    static => 1, # Actually for storage, but we want to ignore it anyway
    const => 1,
    signed => 1,
    unsigned => 1,
    far => 1,
    extern => 1,
    volatile => 1,
);

my %glob_read_files;
my $alignment = 4;

sub read_defs
{
    my ($files, $dirs, $opts) = @_;
    
    $alignment = $opts->{alignment} if defined $opts->{alignment};

    # typedef
    my %defs;

    # struct, union
    my %sdefs;

    # enum
    my %edefs;

    my @ppl;
    my %symtab;

    # The %glob_read_files hash contains a list over all files which
    # have already been processed. This is used to avoid parsing a
    # file multiple times, which saves processing time.
    for my $rf (keys %glob_read_files)
    {
        $glob_read_files{$rf} = undef;
    }

    # The list of files which must be read in order to find all
    # definitions is now processed one file at a time. But several
    # files are typically read for each of these files since they,
    # directly or indirectly, include other files.
    for (my $i = 0; $i < @$files; ++$i)
    {
        my $file = $files->[$i];

        # The file might have been included, directly or indirectly,
        # by a previously read file.
        next if $glob_read_files{$file};

        # The file will now be read from disk and preprocessed.
        my $lines = &read_file(\%symtab, $file, $dirs);

        # For debugging purposes, an option can be used to
        # output the result of the preprocessor.
        push @ppl, $lines and next if $opts->{preprocessor};

        # The types built in by the compiler are now added to the list
        # of types. (Perhaps this could be done before the loop?)
        &append_builtin_types(\%defs);

        # Extract the different type of definitions from
        # the preprocessed file(s).
        &parse_defs(\%defs, \%sdefs, \%edefs, \%symtab, $lines);
    }
    return \@ppl if $opts->{preprocessor};
    $defs{struct} = \%sdefs;
    $defs{enum} = \%edefs;
    $defs{if} = \%symtab; # Use a reserved word not to overwrite a valid entry
    return \%defs;
}

sub read_file
{
    my ($symtab, $filename, $dirs) = @_;
    my $found;
    my $file = $filename;
    
    if (!open F, "<$filename")
    {
        for my $dir (@$dirs)
        {
            if (open F, "<$dir/$filename")
            {
                $file = "$dir/$filename";
                $found = 1;
                last;
            }
        }
    }
    else
    {
        $found = 1;
    }

    # This file seemed to be abscent at some point, but the compilation
    # worked nicely despite that. Perhaps it was in some strange location.
    # It seems like it can nowadays be found in the CSL include directory,
    # but surprisingly enough only under the CCS installation directory,
    # not under the design delivery copy of CSL.
    # None of the types which might appear in this file are currently used
    # by anyone using the logging system, so it is safe for now just to
    # keep this line here.
    return [] if !$found and $filename eq "csl_irq.h";
    die "Could not open $filename" unless $found;

    $glob_read_files{$filename} = 1;
    my @file = <F>;
    close F;
    chomp @file;
    for (my $i = 0; $i < @file; ++$i)
    {
        $file[$i] =~ s/\r//g;
    }

    my $lines = &strip_comments(\@file, $file);
    $lines = &wrap_lines($lines);
    $lines = &preprocessor($symtab, $lines, $dirs);
    return $lines;
}

sub preprocessor
{
    my ($symtab, $lines, $dirs) = @_;
    my @lines;
    my $throw_away = 0;
    for (my $i = 0; $i < @$lines; ++$i)
    {
        my $data = $lines->[$i];
        my $line = $data->{str};
        if ($line =~ m/^#\s*include\s+[<"]([^>]+)[>"]\s*$/)
        {
            if (!$throw_away)
            {
                if (!$glob_read_files{$1})
                {
                    my $data = &read_file($symtab, $1, $dirs);
                    push @lines, @$data;
                }
            }
        }
        elsif ($line =~ m/^#\s*define\s+(\w+)(\([^)]*\))?\s*(.*)$/)
        {
            $symtab->{$1} = $3 if !$throw_away;
        }
        elsif ($line =~ m/^#\s*if\s+0\s*$/)
        {
            ++$throw_away;
        }
        elsif ($line =~ m/^#\s*(if|else|end)\w*/)
        {
            # Everything #if and #end is kept, except for
            # any #else line. Moreover, contents between #if 0 and #end
            # is actually thrown away, se above.
            --$throw_away if $throw_away && $1 eq "end";
        }
        elsif (!$throw_away)
        {
            # Perform macro substitution on the current line.
            $line = &substitute($line, $symtab);
            $data->{str} = $line;
            push @lines, $data;
        }
    }
    return \@lines;
}

sub substitute
{
    my ($line, $symtab) = @_;
    my $prev;
    do
    {
        $prev = $line;
        # Try to replace anything that looks like a name with the
        # value of the symbol in the symtab.
        $line =~ s/(\w+)(\([^)]*\))?/&lookup($1, $2, $symtab)/eg;
    }
    while ($line ne $prev);
    return $line;
}

# The function searches the $symtab for any occurence of $str.
# If found, the value of the symbol is returned. Any arguments
# are currently discarded (which is obviously not the correct
# behaviour, but easy to implement and there is currently no
# feature that is known to rely on this).
# Returns $str if no symbol was found.
sub lookup
{
    my ($str, $arg, $symtab) = @_;
    my $tmp = $symtab->{$str};
    $str .= $arg if defined $arg;
    my $ret = defined $tmp? $tmp: $str;
    return $ret;
}

sub read_token
{
    my ($pos, $lines, $peek) = @_;

    $pos->{idx} = 0 unless defined $pos->{idx};
    my $i = $pos->{idx};
    while ((!defined $pos->{lst} || !@{$pos->{lst}}) && $i < @$lines)
    {
        # Pick the next line to be parsed.
        my $line = $lines->[$i++]->{str};
        $line =~ s/[\t\r]/ /g;

        # Separate tokens by making sure that there is one or more space
        # characters between them. A token is either a character defined
        # by the character group $c, or a sequence of any other characters.
        # To understand this definition, it can help to note that type
        # definitions depends on reserved words, names, numbers and
        # special characters. Each special character is considered
        # a token in its own and each name or number is a token in itself.
        my $c = "[^-a-zA-Z0-9_ \t]";
        for (my $i = 0; $i < 2; ++$i)
        {
            $line =~ s/(\S)($c)/$1 $2/g;   
            $line =~ s/($c)(\S)/$1 $2/g;
        }

        # Split tokens on space characters.
        # An empty string is not considered to be a token.
        $line =~ s/\s\s+/ /g; #
        $line =~ s/^\s+//;
        $line =~ s/\s+$//;
        my @lst = split / /, $line;
        $pos->{lst} = \@lst;
    }
    $pos->{idx} = $i;

    # Return the first character in the list. Also remove it unless
    # this is a peek operation.
    my $elem;
    if ($peek)
    {
        my $tmp = $pos->{lst};
        $elem = $tmp->[0];
    }
    else
    {
        $elem = shift @{$pos->{lst}};
    }

    return $elem;
}

sub unread_token
{
    my ($pos, $token) = @_;
    unshift @{$pos->{lst}}, $token;
}

sub peek_token
{
    return &read_token(@_, 1);
}

sub file_info
{
    my ($pos, $lines) = @_;
    my $line = $lines->[$pos->{idx}];
    return "$line->{file_name}:$line->{num}";
}

# For bitfields, each field has its size defined after a ':'.
sub read_size
{
    my ($pos, $lines) = @_;
    my $peek = &peek_token($pos, $lines);
    if (defined $peek and $peek eq ":")
    {
        &read_token($pos, $lines);
        return &read_token($pos, $lines);
    }
    return undef;
}

sub parse_qualified_name
{
    my ($pos, $lines) = @_;

    # Check if the "variable" is a pointer. This check is not part of the
    # type checking since they are rather associated with the name, as in:
    #     int *a, b; where 'a' is a pointer, but 'b' is not.
    my $ptr = 0;
    while (&peek_token($pos, $lines) eq "*")
    {
        ++$ptr;
        &read_token($pos, $lines);
    }

    # Read the name.
    my $name = &read_token($pos, $lines);
    $name = undef unless $name =~ /^[0-9a-zA-Z_]+$/;

    # This can also be an array, or a part of a bit field, so check that here.
    my ($arrlen, $size);
    if (defined $name)
    {
        $arrlen = &read_array($pos, $lines);
        $size = &read_size($pos, $lines);
        for (;;)
        {
            my $peek = &peek_token($pos, $lines);
            last if !defined $peek or $peek =~ /^[,;]$/;
            &read_token($pos, $lines);
        }
    }
    return ($ptr, $name, $arrlen, $size);
}

# Parses declarations which starts with "struct", "union" or "enum".
# Note that this first token has already been read and is given as the
# argument $complex_type to the function.
sub parse_complex
{
    my ($complex_type, $defs, $sdefs, $edefs, $pos, $lines) = @_;
    my $udefs; # Until the real one is added
    my %xdefs = (
        struct => $sdefs,
        union => $udefs,
        enum => $edefs,
    );
    my $struct_type_name = &read_token($pos, $lines);
    if ($struct_type_name eq "{")
    {
        # No name was given for this declaration, but the type is defined here.
        # So it looks something like:
        #     struct { ...
        $struct_type_name = "";
    }
    else
    {
        if (&peek_token($pos, $lines) eq "{")
        {
            # This indicates that the type is specified right here. Example:
            #     union signals { ...
            &read_token($pos, $lines);
        }
        else
        {
            # It looks like a type was declared here, but not defined.
            # The following should be a variable name (and any pointer
            # or array indicators). Example:
            #     enum bool valid ...
            my ($ptr, $name, $arrlen) = &parse_qualified_name($pos, $lines);

            # We have hopefully read a proper name and if so and the following
            # token is a semicolon, then we have successfully parsed this
            # declaration.
            my $peek = &peek_token($pos, $lines);
            if (defined $name &&
                $name =~ /^\w+$/ &&
                defined $peek &&
                $peek eq ";")
            {
                # This declaration has now been parsed successfully.
                &read_token($pos, $lines);
                my $type;

                # The definition of the type can have been made before,
                # so try to look it up now. It can be resolved later anyway,
                # but it is fast and easy to do it now, if possible.
                if (defined $sdefs->{$struct_type_name})
                {
                    $type = $sdefs->{$struct_type_name};
                }
                else
                {
                    # No previous definition was found, but there are
                    # occasions where this happens in valid C code.
                    # Example where 'mystruct_s' is not defined where it
                    # is used on the first line:
                    #     typedef struct mystruct_s mystruct_t;
                    #     struct mystruct_s { ... };
                    # So mark this type name as unresolved instead.
                    my %unres;
                    $unres{unresolved} = 1;
                    $unres{real_name} = $struct_type_name;
                    $unres{name_space} = $xdefs{$complex_type};
                    $unres{abstract} = 1;
                    $unres{ptr} = $ptr;
                    $type = \%unres;
                }

                # If this is actually an array, then the found type is
                # actually an array. The array element type is the one
                # which we just found.
                if ($arrlen)
                {
                    my %type;
                    $type{array} = 1;
                    $type{type} = $type;
                    $type{num_elems} = $arrlen;
                    $type = \%type;
                }
                return ($type, $name);
            }

            # Why was there no semicolon at the end of the presumed
            # declaration? As long as we know of no valid C code that
            # does not have a semicolon here, then just ignore what was
            # just read.
            return undef;
        }
    }

    # The last token was an opening brace, so what follows is the content
    # of a declaration block. The syntax of a 'union' block is a subset of
    # the syntax for a 'struct' block, but an 'enum' block is very different.
    my %type;
    my @tmp;
    $type{fields} = \@tmp;
    if ($complex_type eq "enum")
    {
        &parse_enum_block(\%type, $pos, $lines);
    }
    else
    {
        # This is for 'struct' and 'union' blocks.
        &parse_decl_block(\%type, $defs, $sdefs, $edefs, $pos, $lines);
    }

    # Any required padding between fields is now inserted.
    # Note that any padding in sub types must already have been applied.
    &insert_padding(\@tmp) if $complex_type eq "struct";

    my $struct_name = "";
    my $arrlen;

    # Finish reading the block, if not already finished with it.
    my $peek = &peek_token($pos, $lines);
    &read_token($pos, $lines) if defined $peek and $peek eq "}";

    # The name and any pointer and/or array indicators should follow.
    (my $ptr, $struct_name, $arrlen) = &parse_qualified_name($pos, $lines);

    # The declaration should end with a semicolon.
    $peek = &peek_token($pos, $lines);
    &read_token($pos, $lines) if defined $peek and $peek eq ";";

    if ($struct_type_name)
    {
        # It is not possible in the general case to know which definition
        # to use if there is more than one. We pick the first one, but it
        # could just as well have been the last one. The reason for
        # a type to be defined multiple times is usually that it was done
        # by mistake, but it is also possible that preprocessor directives
        # are used to select the correct one. This module does not fully
        # implement the preprocessor directives, so we can get into this
        # situation (and select the wrong one) because of that.
#        print STDERR "Struct type $struct_type_name already defined.\n"
#            if defined $sdefs->{$struct_type_name};
        $sdefs->{$struct_type_name}
            = \%type unless defined $sdefs->{$struct_type_name};
    }

    $type{name} = $struct_name;
    $type{type_name} = $struct_type_name eq ""?
                           undef:
                           "struct $struct_type_name";
    if ($complex_type eq "enum")
    {
        $type{enum} = 1;
        $type{length} = 32;
    }
    else
    {
        $type{abstract} = 1;
    }
    
    if ($complex_type eq "union")
    {
        $type{union} = 1 ;
        $type{type_name} = $struct_type_name eq ""?
                               undef:
                               "union $struct_type_name";        
    }
    $type{ptr} = $ptr;

    my $type = \%type;
    if (defined $arrlen)
    {
        my %arr;
        $arr{array} = 1;
        $arr{type} = \%type;
        $arr{num_elems} = $arrlen;
        $type = \%arr;
    }

    return ($type, $struct_name);
}

# Takes a list of fields as argument. Returns a list with the same
# fields, but with any appropriate padding fields inbetween. The first
# field is assumed to be aligned to the maximum alignment size.
sub insert_padding
{
    my ($lst) = @_;
    my $max_size = 8;
    my $offset = 0;
    for (my $i = 0; $i < @$lst; ++$i)
    {
        my $type = $lst->[$i];
        my $size = &type_size($type);
        my $elem_size = $size;

        # An array with (small) elements can be aligned just as an element,
        # so the total array size does not affect its alignment.
        if (defined $type->{type} && $type->{type}->{array})
        {
            $elem_size = &type_size($type->{type}->{type});
        }

        # The padding is defined by the offset to the first available
        # memory, and the size of the element which should be placed there.
        my $pad = &get_pad($offset, $elem_size);

        # The padding will be undefined if no padding field should be inserted.
        if (defined $pad)
        {
            my @tail = splice @$lst, $i;
            push @$lst, $pad;
            push @$lst, @tail;
            ++$i;
            $offset += $pad->{type}->{length};
        }
        $max_size = $size if $size > $max_size;
        $offset += $size;
    }

    # If the current type will be put into an array, then the next element
    # must always be aligned to the alignment border. We can implicitly do
    # that alignment already here by extending the current type to the next
    # even alignment border for this type, through padding. The advantage
    # is the implementation simplicity.
    #
    # But... One might argue that this padding should not be part of this
    # element since it could be used in another context, as a part of a
    # struct, where the following field could be smaller and thus
    # be able to use the space which we now allocate for padding.
    # This argument is probably correct and thus this padding is probably
    # incorrectly done, but tests would be required to determine if this
    # is actually the case.
    my $pad = &get_pad($offset, $max_size);
    push @$lst, $pad if defined $pad;
}

sub type_size
{
    my ($type) = @_;

    # Basic types always define the 'length' field, but no other type does.
    return $type->{length} if defined $type->{length};

    # typedef's simply refers directly to a subtype, which should
    # instead be used to determine the size.
    if (defined $type->{type})
    {
        my $sz = &type_size($type->{type});
        $sz *= $type->{num_elems} if $type->{array};
        return $sz;
    }

    # Abstract types contain a list of fields
    # for which the sizes must be accounted.
    if ($type->{abstract} && defined $type->{fields})
    {
        my $sz = 0;
        my $fields = $type->{fields};
        for my $field (@$fields)
        {
            if ($type->{union})
            {
                # A union takes the size of its largest field.
                my $tmp = &type_size($field);
                $sz = $tmp if $tmp > $sz;
            }
            else
            {
                # The field sizes of a struct should all be summed.
                $sz += &type_size($field);
            }
        }
        return $sz;
    }

    return 0;
}

sub get_pad
{
    my ($offset, $next_size) = @_;

    # Smallest alignment possible since the script currently does not
    # have support for bit fields.
    my $align = 8;

    # The $alignment option can change the maximum alignment size down
    # to a byte if it is set to 1, thus effectively removing all aligment.
    $align = 16 if $next_size > 8 && $alignment >= 2;

    # A maximum alignment of 16 bits is actually used in some situations.
    $align = 32 if $next_size >= 32 && $alignment >= 4;

    # Now calculate the distance from the offset to the next alignment border.
    my $pad = 0;
    $pad = $align - ($offset % $align) if $offset % $align;

    if ($pad)
    {
        # Create a special pad type which can be inserted in a field list.
        my %pad;
        $pad{pad} = 1;
        $pad{name} = "<pad:$pad>";
        my %padtype;
        $padtype{length} = $pad;
        $padtype{name} = "<padtype>";
        $pad{type} = \%padtype;
        return \%pad;
    }

    # The offset is already on an alignment border, so no padding is needed.
    return undef;
}

sub parse_enum_block
{
    my ($type, $pos, $lines) = @_;
    my $tmp;
    my $val = 0;
    for (;;)
    {
        $tmp = &peek_token($pos, $lines);
        last if !defined $tmp or $tmp eq "}";

        # Expect to find a name.
        my $name = &read_token($pos, $lines);

        # Each name may have a value assigned to it.
        if (&peek_token($pos, $lines) eq "=")
        {
            &read_token($pos, $lines);
            $tmp = "";
            # There can be a constant expression on the right hand side of
            # the assignment, so read everything except comma and end of
            # block and assume that it is part of this expression.
            while ((defined &peek_token($pos, $lines)) and
                   &peek_token($pos, $lines) !~ m/^[\},]$/)
            {
                $tmp .= &read_token($pos, $lines);
            }
            $tmp =~ s/(0x[0-9a-fA-F]{8})u/$1/g;
            $val = eval($tmp);
        }
        &read_token($pos, $lines) if &peek_token($pos, $lines) eq ",";

        # Append the enum value.
        my %field;
        $field{name} = $name;
        $field{value} = $val;
        push @{$type->{fields}}, \%field;
        ++$val;
    }
}

sub parse_decl_block
{
    my ($types, $defs, $sdefs, $edefs, $pos, $lines) = @_;
    my $tmp;

    # A declaration block (struct or union definition block) consists of
    # a number of declarations, so read all declarations that can be found
    # until the end of the block is found.
    for (;;)
    {
        $tmp = &peek_token($pos, $lines);
        last if !defined $tmp or $tmp eq "}";

        # Recursive call for a declaration found within a block,
        # which was part of another declaration.
        my ($type, $name, $ptr) = &parse_decl($defs, $sdefs, $edefs,
                                              $pos, $lines);
        my %field;
        $field{name} = $name;
        $field{type} = $type;
        $field{ptr} = $ptr;
        push @{$types->{fields}}, \%field;
    }
}

sub eval_c_expr
{
    my ($expr) = @_;
    $expr =~ s/(0x[0-9a-fA-F]{8})u/$1/g;
    $expr =~ s/\([a-zA-Z][a-zA-Z0-9_]*\)//g;
    my $val = eval($expr);
    return $val;
}

sub read_array
{
    my ($pos, $lines) = @_;
    my $len;
    my $peek = &peek_token($pos, $lines);
    if (defined $peek and $peek eq "[")
    {
        &read_token($pos, $lines);
        my $len_str = "";
        my $token;
        do
        {
            $token = &read_token($pos, $lines);
            $len_str .= $token unless $token eq "]";
        }
        while ($token ne "]");
        $len = &eval_c_expr($len_str);
        if (!defined $len)
        {
            my $file_info = &file_info($pos, $lines);
#            print "Could not resolve array length $len_str, $file_info\n";
        }
    }
    return $len;
}

my $prev_type_name;

sub parse_decl
{
    my ($defs, $sdefs, $edefs, $pos, $lines) = @_;
    my $type;
    my $name;
    my $arr_len;
    my $field_size;
    my $ptr = 0;
    while (defined (my $t = &peek_token($pos, $lines)) && !defined $name)
    {
        # Ignore words (like const, static etc) which does not affect
        # type contents and size.
        &read_token($pos, $lines) and next if $qualifier{$t};

        if (defined $type)
        {
            if ($t =~ /^[,:;]$/)
            {
                &unread_token($pos, $type);
                $type = "int";
            }

            # The name is now expected. Notion for it being a pointer or
            # an array is also parsed here.
            ($ptr, $name, $arr_len, $field_size)
                = &parse_qualified_name($pos, $lines);
        }
        else
        {
            # The first token of a type could be either a type name,
            # such as int or float, or it could be a typedef'd name.
            # Such a name should directly be treated as the type name.
            #
            # I could also be the start of a type declaration in itself,
            # such as:
            #     struct { int a; union {int a, char b;} data}
            $t = &read_token($pos, $lines);
            $type = $t unless defined $type;
            if ($type eq "long" and &peek_token($pos, $lines) eq "long")
            {
                $type = "long long";
                &read_token($pos, $lines);
            }

            my %complex = (
                struct => 1,
                union => 1,
                enum => 1,
            );
            if ($complex{$type})
            {
                # This might be a type declaration in itself. But it could
                # still be a fairly simple type, such as:
                #     struct mytype_s
                my @tmp = &parse_complex($type, $defs, $sdefs, $edefs,
                                         $pos, $lines);
                return @tmp;
            }
        }
    }

    my $peek = &peek_token($pos, $lines);
    &read_token($pos, $lines) if defined $peek and $peek eq ";";

    # Several variables of the same type are sometimes declared
    # at the same time, like:
    #     int a, b, c;
    # We can easily support this by storing the previous type name and
    # using it when we encounter a comma as the type name.
    my $t = $defs->{$type};
    if (!defined $t and $type eq ",")
    {
        $type = $prev_type_name;
        $t = $defs->{$type} if defined $type;
    }

    $prev_type_name = $type;
    if ($arr_len)
    {
        my %t;
        $t{array} = 1;
        $t{type} = $t;
        $t{num_elems} = $arr_len;
        $t{ptr} = $ptr;
        $t = \%t;
        $ptr = 0; # The field is not itself a pointer, but an array of pointers.
    }

    my $line = $lines->[$pos->{idx}];
    my $file_info = &file_info($pos, $lines);
    die "Unknown type $type at $file_info" unless defined $t;

    return ($t, $name, $ptr);
}

sub parse_defs
{
    my ($defs, $sdefs, $edefs, $symtab, $lines) = @_;
    my %pos;

    while (defined (my $t = &peek_token(\%pos, $lines)))
    {
        if ($t eq "typedef")
        {
            &read_token(\%pos, $lines);

            # A 'typedef' word is followed by a type and then a type name.
            # This combination of a type and a name is called a
            # declaration in this module, although their meaning can be
            # a bit different in other situations.
            my ($type, $name)
                = &parse_decl($defs, $sdefs, $edefs, \%pos, $lines);
            $type->{type_name} = $name unless defined $type->{type_name};

            if (defined $type and defined $name)
            {
#                die "Redefined type $name." if defined $defs->{$name};

                # Store the type under its new type name.
                $defs->{$name} = $type unless defined $defs->{$name};
            }
        }
        elsif ($t =~ /^(struct|enum)$/)
        {
            # The parse_decl function will store any struct or enum
            # types (a struct definition can define any number of
            # such types and parse_decl is called recursively, so
            # it seems practical to store these definitions along
            # the way instead of storing them all after the call).
            my ($type, $name)
                = &parse_decl($defs, $sdefs, $edefs, \%pos, $lines);
        }
        else
        {
            # C Code can consist of much more intricate things than this
            # module is capable of understanding, so just ignore any
            # unrecognized token since it is unlikely to affect how the
            # different types are defined.
            &read_token(\%pos, $lines);
        }
    }
}

sub find_type_size
{
    my ($type, $defs) = @_;
    my $def = $defs->{$type};
    if (defined $type)
    {
        return &type_size($def);
    }
    return undef;
}

# Prints the $data as mapped onto the specified $type, given the
# previously scanned definitions in $defs.
sub print_data
{
    my ($data, $type, $defs) = @_;

    # First check so that the specified types has a known definition.
    my $def = $defs->{$type};
    if (!defined $def && $type =~ /^struct\s+(.+)/)
    {
        $def = $defs->{struct}->{$1};
    }
    return undef unless defined $def;

    my @txt;
    &print_def_data(\@txt, 0, $data, $def);
    return \@txt;
}

# Prints the $data as mapped onto the specified $type, given the
# previously scanned definitions in $defs, with start address.
sub print_data_addr
{
    my ($data, $type, $defs, $start_addr) = @_;

    # First check so that the specified types has a known definition.
    my $def = $defs->{$type};
    if (!defined $def && $type =~ /^struct\s+(.+)/)
    {
        $def = $defs->{struct}->{$1};
    }
    return undef unless defined $def;

    my @txt;
    my $idx = &print_def_data(\@txt, $start_addr, $data, $def);
    return (\@txt, $idx);
}

sub print_basic
{
    my ($txt, $idx, $data, $type, $lead, $first_lead) = @_;

    # Basic types are just a value. Float types are currently not
    # supported properly. Something which looks like a character
    # is printed also as a character, if the character is printable.
    if ($type->{name} eq "char")
    {
        push @$txt,
             sprintf("${first_lead}%d, 0x%x", $data->[$idx], $data->[$idx]);
        my $c = chr($data->[$idx]);
        my $str = "";
        $str = sprintf(", '%c'", $data->[$idx]) if $c =~ /[[:print:]]/;
        $str .= "\n";
        push @$txt, $str;
    }
    else
    {
        my $val = &read_num($idx, $data, $type->{length});
        push @$txt, sprintf("${first_lead}%d, 0x%x\n", $val, $val);
    }
    return $idx + $type->{length} / 8;
}

sub print_abstract
{
    my ($txt, $idx, $data, $type, $lead, $first_lead) = @_;
    my $fields = $type->{fields};
    my $name = defined $type->{type_name}? $type->{type_name}: "<anonymous>";
    
    push @$txt, "${first_lead}$name {\n";
    my $start_idx = $idx;
    my $max_idx = $idx;
    for (my $i = 0; $i < @$fields; ++$i)
    {
        # The same data must be matched over and over again
        # for each field in a union.
        $idx = $start_idx if $type->{union};

        my $field = $fields->[$i];
        my $ptr = defined $field->{ptr}? $field->{ptr}: 0;
        my $ptr_str = "";
        for (; $ptr; --$ptr)
        {
            $ptr_str .= "*";
        }
        push @$txt, "${lead}  $field->{name} = $ptr_str";
        if ($field->{ptr})
        {
            my $num = &read_num($idx, $data, 32);
            $idx = $idx += 32 / 8;
            push @$txt, sprintf("${first_lead}0x%08x, %d\n", $num, $num);
        }
        else
        {
            # Recursive call for each field.
            $idx = &print_def_data($txt,
                                   $idx,
                                   $data,
                                   $field->{type},
                                   $lead . "  ",
                                   "");
        }
        $max_idx = $idx if $idx > $max_idx;
    }
    push @$txt, "${lead}}\n";
    return $type->{union}? $max_idx: $idx;
}

# Returns the number which is written at location $idx in $data
# and has the given $size.
sub read_num
{
    my ($idx, $data, $size) = @_;
    my $val;
    for (my $i = 0; $i < $size / 8; ++$i)
    {
        $val |= $data->[$i + $idx] << ($i * 8)
            if defined $data->[$i + $idx];
    }
    return $val;
}

sub print_enum
{
    my ($txt, $idx, $data, $type, $lead, $first_lead) = @_;
    my $fields = $type->{fields};
    my $name = defined $type->{type_name}? $type->{type_name}: "<anonymous>";
    my $value = &read_num($idx, $data, 32);

    # Go through the enum values and see if there is one which match
    # the number which we read for this field.
    my $val_name;
    for (my $i = 0; $i < @$fields; ++$i)
    {
        my $field = $fields->[$i];
        $val_name = $field->{name} if $field->{value} == $value;
    }

    # The value is printed in both decimal and hex.
    my $val_str = sprintf("$value, 0x%x", $value);
    my $tmp = defined $val_name? "$val_name ($val_str)": "$val_str";
    push @$txt, "${first_lead}$tmp\n";
    return $idx + 32 / 8;
}

sub print_array
{
    my ($txt, $idx, $data, $type, $lead, $first_lead) = @_;

    # An array is just a number of types in a sequence.
    # Any inter-element padding has already been compensated for
    # in the elements. See the end of &insert_padding().
    push @$txt, "${first_lead}\[\n";
    for (my $i = 0; $idx < @$data && $i < $type->{num_elems}; ++$i)
    {
        push @$txt, "${lead}  [$i] =";
        if ($type->{ptr})
        {
            my $num = &read_num($idx, $data, 32);
            $idx += 32 / 8;
            push @$txt, sprintf(" 0x%08x, %d\n", $num, $num);
        }
        else
        {
            $idx = &print_def_data($txt,
                                   $idx,
                                   $data,
                                   $type->{type},
                                   $lead . "  ",
                                   " ");
        }
    }
    push @$txt, "${lead}]\n";
    return $idx;
}

sub print_unresolved
{
    my ($txt, $idx, $data, $type, $lead, $first_lead) = @_;

    # Look up the data type, which is assumed to have ben read after
    # the declaration of this type name was read.
    my $defs = $type->{name_space};
    my $real_name = $type->{real_name};
    my $real_type = $defs->{$real_name};

    if ($type->{ptr})
    {
        my $num = &read_num($idx, $data, 32);
        push @$txt, sprintf("${first_lead}0x08%x, %d", $num, $num);
        $idx += 32 / 8;
    }
    else
    {
        $idx = &print_def_data($txt,
                               $idx,
                               $data,
                               $real_type,
                               $lead,
                               $first_lead . "$type->{type_name} = ");
    }
    return $idx;
}

# Append strings to the @$txt array while converting data from $data
# at offset $idx for the defined $type. The text should be indented
# by the string in $lead, except for the first line, which should be
# indented by the string $first_lead.
sub print_def_data
{
    my ($txt, $idx, $data, $type, $lead, $first_lead) = @_;

    # A recursion base case for when we run out of data.
    return $idx if $idx >= @$data;

    $lead = "" unless defined $lead;
    $first_lead = $lead unless defined $first_lead;

    if ($type->{enum})
    {
        $idx = &print_enum($txt, $idx, $data, $type, $lead, $first_lead);
    }
    elsif ($type->{length})
    {
        $idx = &print_basic($txt, $idx, $data, $type, $lead, $first_lead);
    }
    elsif ($type->{abstract})
    {
        if ($type->{unresolved})
        {
            $idx = &print_unresolved($txt, $idx, $data, $type,
                                     $lead, $first_lead);
        }
        else
        {
            $idx = &print_abstract($txt, $idx, $data, $type,
                                   $lead, $first_lead);
        }
    }
    elsif ($type->{array})
    {
        $idx = &print_array($txt, $idx, $data, $type, $lead, $first_lead);
    }
    return $idx;
}

sub strip_comments
{
    my ($file, $name) = @_;
    my @lines;
    for (my $i = 0; $i < @$file; ++$i)
    {
        my $line = $file->[$i];
        $line =~ s/\/\*(.*?)\*\///g;

        # Strip C++ comments
        if ($line !~ /\/\*/)
        {
            $line =~ s/(\/\/.*)//;
        }

        # Strip multiple line C comments
        while ($line =~ /\/\*/)
        {
            while ($line !~ m/\*\// && $i < @$file)
            {
                ++$i;
                $line .= $file->[$i];
            }
            $line =~ s/\/\*(.*?)\*\///g;
        }

        # Strip single line C comments
        $line =~ s/\/\*(.*?)\*\///;

        # File name and number can be used later for error messages
        # which tells where some sort of problem occured. It is easy
        # to save the information per line since lines in one file can
        # be separated by lines from other files (due to #include
        # statements).
        my %line;
        $line{file_name} = $name;
        $line{num} = $i + 1;
        $line{str} = $line;
        push @lines, \%line;
    }
    return \@lines;
}

# Lines ending with backslash and then a newline character should
# have the next line put at the end of the current line.
sub wrap_lines
{
    my ($lines) = @_;
    my @lines;
    for (my $i = 0; $i < @$lines; ++$i)
    {
        my $first = $lines->[$i];
        my $line = $first->{str};
        while ($line =~ m/\\$/ && $i < @$lines)
        {
            ++$i;
            $line =~ s/\\$//;
            $line .= $lines->[$i]->{str};
        }
        my %line;
        for my $key (keys %$first)
        {
            $line{$key} = $first->{$key};
        }
        $line{str} = $line;
        push @lines, \%line;
    }
    return \@lines;
}

sub append_builtin_types
{
    my ($defs) = @_;

    $defs->{char} = {name => "char", length => 8};
    $defs->{short} = {name => "short", length => 16};
    $defs->{int} = {name => "int", length => 32};
    $defs->{size_t} = {name => "size_t", length => 32};
    $defs->{ssize_t} = {name => "ssize_t", length => 32};
    $defs->{long} = {name => "long", length => 32};
    $defs->{"long long"} = {name => "long long", length => 64};
    $defs->{float} = {name => "float", length => 32};
    $defs->{double} = {name => "double", length => 32};
    $defs->{void} = {name => "void", length => 0};

    $defs->{bool} = {name => "bool", length => 8};
    $defs->{__int64} = {name => "__int64", length => 32};
    $defs->{__WCHAR_T_TYPE__} = {name => "__WCHAR_T_TYPE__", length => 8};
    $defs->{__SIZE_T_TYPE__} = {name => "__SIZE_T_TYPE__", length => 32};
    $defs->{__PTRDIFF_T_TYPE__} = {name => "__PTRDIFF_T_TYPE__", length => 32};
}

1;
