#!C:/Perl/bin/perl -w
$|++;

use Win32::API;
my @digits = ( 0xFB, 0x60, 0x2F, 0x6E, 0x74, 0x5e, 0x5F, 0x78, 0x7F, 0x7C );
my ( $outAddress, $inAddress, $cntlAddress, $in, $out );

sub initParallelPort {
 	$outAddress     = shift;
	$inAddress      = $outAddress + 1;
	$cntlAddress      = $outAddress + 2;
	$in = new Win32::API("inpout32","Inp32",['I'],'I')      unless($in);
	$out = new Win32::API("inpout32","Out32",['I','I'],'I') unless($out);
}

sub setParallel { $out->Call($outAddress, (shift)); }
sub getStatus   { return $in->Call($inAddress);     }

#   initParallelPort( 0x3BC );
   initParallelPort( 0x378 );
#   initParallelPort( 0x278 );
#    initParallelPort( 0x178 );

   my $i = 0;
   while ( 1 )
   {
       my $i;
       for ($i=0; $i < 10; $i++)
       {
	   select(undef, undef, undef, 2.0);
	   print "$i\n";
	   setParallel($digits[$i]);
       }
   }
