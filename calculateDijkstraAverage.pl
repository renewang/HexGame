#!//opt/local/bin/perl
use IO::File;

open FILE, $ARGV[0];
my $sum = 0, $cnt = 0, $sumofsquare = 0;
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
print "mean = $mean\n";
print "std = $std\n";
