function [ cornerMaxesC cornerMaxesI cornerMaxesJ ] = detectLCorners( bwImage )

%% Shrink B/W Image
smallBwImage = int8(shrinkMatrix(bwImage,4));

%% Gaussian Blur the Small B/W Image
gbSmallBwImage = gaussianBlurImage(double(smallBwImage));

%% Trajkovic Operation
trajMtrx = trajkovicCorners(gbSmallBwImage, 150);

%% Find Local Maxima
[cornerPeaksC cornerPeaksI cornerPeaksJ] = limitMaxima(trajMtrx, -1);

%% Localize Corners
[cornerMaxesC cornerMaxesI cornerMaxesJ] = localizeCorners(bwImage, cornerPeaksC, cornerPeaksI+2, cornerPeaksJ+2);
cornerMaxesI = cornerMaxesI-2;
cornerMaxesJ = cornerMaxesJ-2;

end

