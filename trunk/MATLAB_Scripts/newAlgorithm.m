%% Import Color Data
inColorR = csvread('../colorData/000026_R.csv');
inColorG = csvread('../colorData/000026_G.csv');
inColorB = csvread('../colorData/000026_B.csv');

%% Make Full B/W Image
bwImage = normalizeImage(inColorR+inColorG+inColorB);
bwImage(1,:) = bwImage(2,:);
bwImage(:,1) = bwImage(:,2);
bwImage(size(bwImage,1),:) = bwImage(size(bwImage,1)-1,:);
bwImage(:,size(bwImage,2)) = bwImage(:,size(bwImage,2)-1);

%% Shrink B/W Image
smallBwImage = int8(shrinkMatrix(bwImage,4));

%% Gaussian Blur the Small B/W Image
gbSmallBwImage = gaussianBlurImage(double(smallBwImage));

%% Trajkovic Operation
trajMtrx = trajkovicCorners(gbSmallBwImage, 150);

%% Find Local Maxima
[cornerPeaksC cornerPeaksI cornerPeaksJ] = limitMaxima(trajMtrx, 64);

%% Draw Local Maxima
featurePlot(smallBwImage, cornerPeaksI+2, cornerPeaksJ+2);

%% Localize Corners
[cornerMaxesC cornerMaxesI cornerMaxesJ] = localizeCorners(bwImage, cornerPeaksC, cornerPeaksI+2, cornerPeaksJ+2);

%% Draw Local Maxima
featurePlot(bwImage, cornerMaxesI-2, cornerMaxesJ-2);

%% Test Alg
[cornerMaxesC cornerMaxesI cornerMaxesJ] = detectLCorners(bwImage);
featurePlot(bwImage, cornerMaxesI, cornerMaxesJ);
