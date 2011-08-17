#!C:/Perl/bin/perl.exe -w 
use LWP::UserAgent; 
use HTTP::Request; 
use HTTP::Response; 
use URI::Heuristic;
use File::Basename;

   my $url = $ARGV[0];
   my $newurl = URI::Heuristic::uf_urlstr($url);
   my $ua = LWP::UserAgent->new(); 
      $ua->agent("Schmozilla/v9.14 Platinum");
   my $req = HTTP::Request->new(GET => $newurl); 
      $req->referer("http://www.midnightlibrary.com");
   my $response = $ua->request($req);
   open(WGETFILE, ">".basename($url) );
   print WGETFILE $response->content();
   close WGETFILE;
