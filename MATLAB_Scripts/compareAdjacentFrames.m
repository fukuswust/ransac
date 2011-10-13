dataPath = 'C:\Kinect Data\FreeFormTest';

for imageIndexOn = 27

%% Import Data
inColorR1 = csvread([dataPath '/colorData/' sprintf('%.6u',imageIndexOn) '_R.csv']);
inColorG1 = csvread([dataPath '/colorData/' sprintf('%.6u',imageIndexOn) '_G.csv']);
inColorB1 = csvread([dataPath '/colorData/' sprintf('%.6u',imageIndexOn) '_B.csv']);
inDepth   = csvread([dataPath '/depthData/' sprintf('%.6u',imageIndexOn) '.csv']);

%% Make Full B/W Image
bwImage1 = normalizeImage(inColorR1+inColorG1+inColorB1);
bwImage1(1,:) = bwImage1(2,:);
bwImage1(:,1) = bwImage1(:,2);
bwImage1(size(bwImage1,1),:) = bwImage1(size(bwImage1,1)-1,:);
bwImage1(:,size(bwImage1,2)) = bwImage1(:,size(bwImage1,2)-1);

%% Test Alg
[cornerMaxesC1 cornerMaxesI1 cornerMaxesJ1] = detectLCorners(bwImage1);

I = normalizeImage(bwImage1);
[r g b] = deal(I);
for indexOn = 1:size(cornerMaxesI1,1)
    I = cornerMaxesI1(indexOn);
    J = cornerMaxesJ1(indexOn);
    r(I-1:I+1,J-1:J+1) = 255;
    g(I-1:I+1,J-1:J+1) = 0;
    b(I-1:I+1,J-1:J+1) = 0;
end
RGB = cat(3,r,g,b);
imshow(RGB);

% imwrite(RGB,[dataPath '/outCornerTrack/' sprintf('%.6u',imageIndexOn) '.bmp']);

%% Convert Depth Array to x,y,z,cx,cy
nanDepth = replaceValue(inDepth,2047,NaN);
xyzValues = depthToCartesian(nanDepth);
CxCyValues = xyzToCxCy(xyzValues);

imshow((normalizeImage(replaceNaN(nanDepth,min(min(nanDepth))))));
%fimshow(RGB);
for wait=1:1000
    k = waitforbuttonpress;
    point1 = get(gca,'CurrentPoint');  %button down detected
    j = int16(point1(1,1));
    i = int16(point1(1,2));
    fprintf('I: %u  J: %u  X: %i  Y: %i  Z: %i  CX: %u  CY: %u\n', i, j, int16(100*xyzValues(i,j,1)), int16(100*xyzValues(i,j,2)), int16(100*xyzValues(i,j,3)), int16(CxCyValues(i,j,1)), int16(CxCyValues(i,j,2)))
end






end