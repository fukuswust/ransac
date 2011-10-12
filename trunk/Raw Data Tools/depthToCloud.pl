use Math::Trig;
use POSIX;

my $width = 640;
my $height = 480;

if (@ARGV != 2 && @ARGV != 1) {
	print "Usage: perl depthToCloud.pl <startingFile> <endingFile>\n";
	print "       perl depthToCloud.pl <file>\n";
	exit;
}

my $firstFile = $ARGV[0];
my $lastFile;
if (@ARGV == 2) {
	$lastFile = $ARGV[1];
} else {
	$lastFile = $firstFile;
}

open PATHFILE, "paths.txt" or die $!;
my @inPaths = <PATHFILE>;
close(PATHFILE);
chomp(@inPaths);

unless(-d $inPaths[3]) {
	mkdir $inPaths[3] or die $!;
}

for (my $fileIndex = $firstFile; $fileIndex < $lastFile+1; ++$fileIndex) {
	my $totalVerts = 0;
	my $tmpFileName = sprintf("%06d", $fileIndex);
	
	open  INFILE, "<$inPaths[0]/$tmpFileName.csv" or die $!; 
	my @inLines = <INFILE>;
	close(INFILE);
	chomp(@inLines);
	open  INFILE, "<$inPaths[2]/".$tmpFileName."_R.csv" or die $!; 
	my @inRLines = <INFILE>;
	close(INFILE);
	chomp(@inRLines);
	open  INFILE, "<$inPaths[2]/".$tmpFileName."_G.csv" or die $!; 
	my @inGLines = <INFILE>;
	close(INFILE);
	chomp(@inGLines);
	open  INFILE, "<$inPaths[2]/".$tmpFileName."_B.csv" or die $!; 
	my @inBLines = <INFILE>;
	close(INFILE);
	chomp(@inBLines);
	
	my @arrayR;
	my @arrayG;
	my @arrayB;
	for (my $j = 0; $j < $height; $j+=1) {
		my @tmpSplitR = split(/,/,$inRLines[$j]);
		my @tmpSplitG = split(/,/,$inGLines[$j]);
		my @tmpSplitB = split(/,/,$inBLines[$j]);
		for (my $i = 0; $i < $width; $i+=1) {
			$arrayR[$i+($width*$j)] = $tmpSplitR[$i]; 
			$arrayG[$i+($width*$j)] = $tmpSplitG[$i]; 
			$arrayB[$i+($width*$j)] = $tmpSplitB[$i]; 
		}
	}

	open  OUTFILE, ">$inPaths[3]/$tmpFileName.ply" or die $!;
	
	for (my $j = 0; $j < $height; $j+=1) {
		@splitRow = split(/,/,$inLines[$j]);
		for (my $i = 0; $i < $width; $i+=1) {
			$depth = $splitRow[$i];
			if ($depth != 2047) {
				my $z = 1.0 / (($depth * -0.0030711016) + 3.3309495161);
				#if ($z <= 1.5) {
					$totalVerts+=1;
				#}
			}
		}
	}

	print OUTFILE "ply\n";
	print OUTFILE "format ascii 1.0\n";
	print OUTFILE "comment Created by John Gideon\n";
	print OUTFILE "element vertex $totalVerts\n";
	print OUTFILE "property float x\n";
	print OUTFILE "property float y\n";
	print OUTFILE "property float z\n";
	print OUTFILE "property uchar red\n";
	print OUTFILE "property uchar green\n";
	print OUTFILE "property uchar blue\n";
	print OUTFILE "end_header\n";

	#DEPTH
	$fx_d = 5.9421434211923247e+02;
	$fy_d = 5.9104053696870778e+02;
	$cx_d = 3.3930780975300314e+02;
	$cy_d = 2.4273913761751615e+02;
	
	#RGB
	$fx_rgb = 5.2921508098293293e+02;
	$fy_rgb = 5.2556393630057437e+02;
	$cx_rgb = 3.2894272028759258e+02;
	$cy_rgb = 2.6748068171871557e+02;

	for (my $j = 0; $j < $height; $j+=1) {
		@splitRow = split(/,/,$inLines[$j]);

		for (my $i = 0; $i < $width; $i+=1) {
			$depth = $splitRow[$i];
			if ($depth != 2047) {
				my $z = 1.0 / (($depth * -0.0030711016) + 3.3309495161);
				#if ($z <= 1.5) {
					my $x = (($i - $cx_d) * $z) / $fx_d;
					my $y = (($j - $cy_d) * $z) / $fy_d;

					my $n_x = ((9.9984628826577793e-01)*$x) + ((-1.4779096108364480e-03)*$y) + ((1.7470421412464927e-02)*$z) + (1.9985242312092553e-02);
					my $n_y = ((1.2635359098409581e-03)*$x) + ((9.9992385683542895e-01)*$y) + ((1.2275341476520762e-02)*$z) + (-7.4423738761617583e-04);
					my $n_z = ((-1.7487233004436643e-02)*$x) + ((-1.2251380107679535e-02)*$y) + ((9.9977202419716948e-01)*$z) + (-1.0916736334336222e-02);

					my $c_x = floor(($n_x * $fx_rgb / $n_z) + $cx_rgb);
					my $c_y = floor(($n_y * $fy_rgb / $n_z) + $cy_rgb);

					if ($c_x >= 0 && $c_x < $width && $c_y >= 0 && $c_y < $height) {
						my $R = $arrayR[$c_x+($width*$c_y)];
						my $G = $arrayG[$c_x+($width*$c_y)];
						my $B = $arrayB[$c_x+($width*$c_y)];
						print OUTFILE "$x  $y  $z  $R  $G  $B\n";
					} else {
						print OUTFILE "$x  $y  $z  127  127  127\n";
					}
				#}
			}
		}
	}
	
	close OUTFILE;
	print "Imported Depth File: $tmpFileName.csv\n";		
}

