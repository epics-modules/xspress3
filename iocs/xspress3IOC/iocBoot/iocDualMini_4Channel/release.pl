# FILENAME...	release.pl
#
# USAGE... This PERL script is used in conjunction with a start_epics_xxx
#          script to setup environment variables for medm.  It defaults to
#          csh output, but a switch enables bash output (second form below).
#
# ORIGINAL AUTHOR: Ron Sluiter
# 
# SYNOPSIS...	perl release.pl (<ioctop> directory)
#               perl -s release.pl -form=bash (<ioctop> directory)  
#
#
#
# MODIFICATION LOG...
# 03/25/04 rls Support for GATEWAY environment variable.
# 04/08/04 rls Bug fix for spaces between macro and '=' sign; e.g. MPF = /home/mpf.
# 01/25/08 rls Support "include" entries without a macro; e.g. "include /home/ioc/configure/MASTER_RELEASE"
# 01/29/08 rls Bug fix; "($macro) =" line is wrong.
# 04/06/11 daa Add bash output format support.
# 08/18/15 mdm Allowed for use with AD modules, requires full file name/path now

#
#Version:	$Revision: 1.8.1 $
#Modified By:	$Author: matthewmoore $
#Last Modified:	$Date: 2015-08-18 $
#
# 

use Env;

if ($ENV{GATEWAY} ne "")
{
    # Add GATEWAY to macro list.
    $applications{GATEWAY} = $ENV{GATEWAY};
}

$top = $ARGV[0];

$format = 0;
if ($form eq "bash")
{
    $format = 1;
}

$applications{TOP} = $top;

@files =();
push(@files,"$top");

foreach $file (@files)
{
    if (-r "$file")
    {
	open(IN, "$file") or die "Cannot open $file\n";
	while ($line = <IN>)
	{
	    next if ( $line =~ /\s*#/ );
	    chomp($line);
	    $_ = $line;
	    #test for "include" command
	    ($prefix,$post) = /(.*)\s* (.*)/;
	    if ($prefix eq "include")
	    {
		($prefix,$macro,$post) = /(.*)\s* \s*\$\((.*)\)(.*)/;
		if ($macro eq "")
		{
		    #true if no macro is present
		    #the following looks for
		    #prefix = post
		    ($prefix,$post) = /(.*)\s* \s*(.*)/;
		}
		else
		{
		    $base = $applications{$macro};
		    if ($base eq "")
		    {
			#print "error: $macro was not previously defined\n";
		    }
		    else
		    {
			$post = $base . $post;
		    }
		}
		push(@files,"$post")
	    }
	    else
	    {
		#the following looks for
		#prefix = $(macro)post
		($prefix,$macro,$post) = /(.*)\s*=\s*\$\((.*)\)(.*)/;
		if ($macro eq "")
		{
		    #true ifno macro is present
		    #the following looks for
		    #prefix = post
		    ($prefix,$post) = /(.*)\s*=\s*(.*)/;
		}
		else
		{
		    $base = $applications{$macro};
		    if ($base eq "")
		    {
			#print "error: $macro was not previously defined\n";
		    }
		    else
		    {
			$post = $base . $post;
		    }
		}

		$prefix =~ s/^\s+|\s+$//g; # strip leading and trailing whitespace.

		$applications{$prefix} = $post;
		if ( -d "$post")
		{
                    if ($format == 1)
                    {
                        print "$prefix=$post\n";
                    }
                    else
                    {
                        print "set $prefix = $post\n";
                    }
		}
	    }
	}
	close IN;
    }
}

