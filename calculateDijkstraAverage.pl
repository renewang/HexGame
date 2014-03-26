#!/opt/local/bin/perl
use strict;
use warnings;
use IO::File;

my @files;
my @means;
my @stds;

if (-f $ARGV[0]){
	push(@files, $ARGV[0]);
}elsif(-d $ARGV[0]){
	opendir DIR, $ARGV[0] or die $!;
	while (my $file = readdir(DIR)){
		if($file =~ /mean$/){
			push(@files, $file);
		}
	}	
	closedir(DIR);
}
print "filename\t";
foreach my $file (sort(@files)){
	my $sum = 0, my $cnt = 0, my $sumofsquare = 0;
	my $filename = $ARGV[0].$file;
	open FILE, $filename or die $!;
	while(my $line=<FILE>){
		my @result = split(',',$line);
		$result[0] =~ s/^\s+//g;
		my $elem = $result[0];
		$sum += $elem;
		$sumofsquare += $elem*$elem;
		$cnt++;
	}
	my $mean = $sum/$cnt;
	my $std = sqrt($sumofsquare/$cnt - $mean*$mean);
	push(@means, $mean);
	push(@stds, $std);
       	print "$file\t";
	#print "mean = $mean\n";
        #print "std = $std\n";
        close(FILE)
}
print "\nmean\t";
foreach my $mean (@means){
	print "$mean\t";
}
print "\nstd\t";
foreach my $std (@stds){
	print "$std\t";
}
print "\n";
