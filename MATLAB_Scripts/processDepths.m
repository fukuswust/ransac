%% Import Data Files
inData1 = csvread([dataPath '/depthData/000015.csv']);
%inData2 = csvread([dataPath '/depthData/000005.csv']);

%% Insert NaN where 2047
nanData1 = replaceValue(inData1,2047,NaN);
%nanData2 = replaceValue(inData2,2047,NaN);

%% Shrink B/W Image
smallBwImage = shrinkNaNMatrix(nanData1,4);
imshow (normalizeImage(replaceValue(smallBwImage,NaN,2047)));

%% Gaussian Blur the Small B/W Image
gbSmallBwImage = gaussianBlurImage(double(smallBwImage));
imshow (normalizeImage(replaceValue(gbSmallBwImage,NaN,2047)));

%% Convert to X,Y,Z
xyzValues1 = depthToCartesian(nanData1);

%% Determine Normals
planeNormals1 = xyzToNormals(xyzValues1, 10);

%% Save Normals
imwrite(returnImageNaN(planeNormals1(:,:,1)),'Nx1.bmp');
imwrite(returnImageNaN(planeNormals1(:,:,2)),'Ny1.bmp');
imwrite(returnImageNaN(planeNormals1(:,:,3)),'Nz1.bmp');

%% Save Z Image
imwrite(returnImageNaN(xyzValues1(:,:,1)),'x1.bmp');
imwrite(returnImageNaN(xyzValues1(:,:,2)),'y1.bmp');
imwrite(returnImageNaN(xyzValues1(:,:,3)),'z1.bmp');
% imwrite(returnImageNaN(xyzValues2(:,:,1)),'x2.bmp');
% imwrite(returnImageNaN(xyzValues2(:,:,2)),'y2.bmp');
% imwrite(returnImageNaN(xyzValues2(:,:,3)),'z2.bmp');


%% Import Testing Image
inColorR1 = double(TestImage1(:,:,1));
inColorG1 = double(TestImage1(:,:,2));
inColorB1 = double(TestImage1(:,:,3));
inColorR2 = double(TestImage2(:,:,1));
inColorG2 = double(TestImage2(:,:,2));
inColorB2 = double(TestImage2(:,:,3));

%% Import Color Data
inColorR1 = csvread('../colorData/000026_R.csv');
inColorG1 = csvread('../colorData/000026_G.csv');
inColorB1 = csvread('../colorData/000026_B.csv');
inColorR2 = csvread('../colorData/000027_R.csv');
inColorG2 = csvread('../colorData/000027_G.csv');
inColorB2 = csvread('../colorData/000027_B.csv');

%% Make B/W Image
inColor1 = normalizeImage(inColorR1+inColorG1+inColorB1);
inColor2 = normalizeImage(inColorR2+inColorG2+inColorB2);

%% Draw Input
small2 = int8(gaussianBlurImage(shrinkMatrix(inColor2,4)));
imshow(small2);
inColor2 = small2;

%% Test
%[detectedCornerR1 detectedCornerI1 detectedCornerJ1] = extractCorners(inColor1, 32);
[detectedCornerR2 detectedCornerI2 detectedCornerJ2] = extractCorners(inColor2, 32);

%% Draw One Result
%featurePlot(inColor1, detectedCornerI1, detectedCornerJ1)
featurePlot(inColor2, detectedCornerI2, detectedCornerJ2)

%% Draw Results
subplot(1,2,1), featurePlot(inColor1, detectedCornerI1, detectedCornerJ1);
subplot(1,2,2), featurePlot(inColor2, detectedCornerI2, detectedCornerJ2);