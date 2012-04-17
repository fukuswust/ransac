%% Initial Constants
frameName = '000027';

%% Loop
lastFile = 33;
tdView = cell(lastFile+1,1);
for fileOn = 0:lastFile
    frameName = sprintf('%.6u',fileOn);

prevTransform = [1 2 3 4 5 6];

%% Import Data
inDepthData = csvread([dataPath '/depthData/' frameName '.csv']);
inRColorData = csvread([dataPath '/colorData/' frameName '_R.csv']);
inGColorData = csvread([dataPath '/colorData/' frameName '_G.csv']);
inBColorData = csvread([dataPath '/colorData/' frameName '_B.csv']);
inAccelData = transpose(csvread([dataPath '/accelData/' frameName '.csv']));

%% Combine Color Values
colorData(:,:,1) = inRColorData(:,:)./256;
colorData(:,:,2) = inGColorData(:,:)./256;
colorData(:,:,3) = inBColorData(:,:)./256;
image(colorData);
axis equal
%imwrite(colorData,[dataPath '/colorImages/' frameName '.bmp']);

%% Shrink Color Data
sColorData = shrinkPointCloud(colorData,4);
image(sColorData);
axis equal

%% Shrink Depth Data and Replace NaN
% Shrink Matrix
sDepthData = shrinkMatrix(inDepthData, 16);
% Interchange 0's for NaN
nanDepthData = replaceValue(sDepthData,2047,NaN);
printImage(nanDepthData);
axis equal

%% Convert Depth to Point Cloud
% Convert Depth to Z
zData = depthToZ(nanDepthData);
% Convert Z Array to Point Cloud
pointCloud = zToPointCloud(zData);
printImage(pointCloud(:,:,3));
axis equal

%% Add Color to Point Cloud
cPointCloud = addGreyscaleToPointCloud(pointCloud, colorData);
printImage(cPointCloud(:,:,4)); %All floor should be black
axis equal

%% Determine Quality of Gravity Vector
gravityVectorQuality(inAccelData)
tmp = printImage(pointCloud(:,:,2));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/0_' frameName '.bmp']);

%% Align to Gravity
upVect = [0 1 0];
gravityRotate = getRotationMatrix(inAccelData, upVect);
gPointCloud = transform3dMatrix(cPointCloud, gravityRotate);
tmp = printImage(gPointCloud(:,:,2)); %All floor should be black
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/1_' frameName '.bmp']);

%% Segment Floor
fPointCloud = segmentFloor(gPointCloud);
tmp = printImage(fPointCloud(:,:,2)); %All floor should be black
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/2_' frameName '.bmp']);
floorHeight = min(min(gPointCloud(:,:,1)));
fprintf('Height: %f cm\n', floorHeight);

%% Segment Mesh
totFloor = 0;
for i = 1:30
    for j = 1:40
        if ~isnan(fPointCloud(i,j,1))
            totFloor = totFloor + 1;
            floorPoints(totFloor,1:3) = permute(fPointCloud(i,j,1:3),[1 3 2]);
        end
    end
end
scatter3(floorPoints(:,1),floorPoints(:,3),floorPoints(:,2));
axis equal

%% Get First Plane Extimate
sFloorPoints = sort(floorPoints,2);
floorModeY = mode(floor(sFloorPoints(:,2)));
totMFloor = 0;
for i = 1:totFloor
    if abs(floorPoints(i,2)-floorModeY) < 5
        totMFloor = totMFloor + 1;
        mFloorPoints(totMFloor,1:3) = floorPoints(i,1:3);
    end
end
scatter3(mFloorPoints(:,1),mFloorPoints(:,2),mFloorPoints(:,3));
axis equal

%% Regress Plane
x = mFloorPoints(:,1);
z = mFloorPoints(:,2);
y = mFloorPoints(:,3);
M = [sum(x.^2) sum(x.*y) sum(x);
     sum(x.*y) sum(y.^2) sum(y);
     sum(x)    sum(y)    totMFloor];
A = [sum(x.*z); sum(y.*z); sum(z)];
V = M\A;
align = [-V(1)/V(3) 1/V(3) -V(2)/V(3)];
align = align/sqrt(sum(align.^2));
if align(2) < 0
    align = -align;
end
upVect = [0 1 0];
alignRotate = getRotationMatrix(align, upVect);
faPointCloud = transform3dMatrix(gPointCloud, alignRotate);
x = faPointCloud(:,:,1);
y = faPointCloud(:,:,2);
z = faPointCloud(:,:,3);
scatter(x(:),y(:));
%printImage(faPointCloud(:,:,2)); %All floor should be black
axis equal


%% Convert to Polar
pPointCloud = cartesianToPolar(gPointCloud);
tmp = printImage(pPointCloud(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/3_' frameName '.bmp']);

%% Get Max Distances for Each Direction Slice
maxDisListPolar = getMaxDisPerDir(pPointCloud,40);
minDir = min(min(pPointCloud(:,:,2)));
fprintf('Min Dir: %f rad\n', minDir);
maxDir = max(max(pPointCloud(:,:,2)));
fprintf('Max Dir: %f rad\n', maxDir);
dirSpan = maxDir - minDir;
fprintf('Dir Span: %f rad (%f deg)\n', dirSpan, (dirSpan/pi())*180);

%% Change MaxDisPerDir List to Cartesian
maxDisList = polarToCartesian2D(maxDisListPolar);
maxDisList(:,2) = -maxDisList(:,2);
scatter(maxDisList(:,1), maxDisList(:,2), 30, 'filled')
axis equal
%saveas(gcf,[dataPath '/MATLAB_output/32_' frameName '.bmp']) 
%tdView{fileOn+1} = maxDisList;

%% Segment Walls
[wPointCloud outScanP] = segmentWalls(pPointCloud, maxDisListPolar);
tmp = printImage(wPointCloud(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/4_' frameName '.bmp']);

%% Top-Down 
outScan = polarToCartesian2D(outScanP);
outScan(:,2) = -outScan(:,2);
scatter(outScan(:,1),outScan(:,2));
axis([-200 200 0 500]);
axis equal 

%% Initialize
param = zeros(8,3);
id = kron([1 2 3 4 5 6 7 8],ones(1,5));
numSec = 8;

%% Regress Sections
for rep = 1:5
scatter(outScan(:,1),outScan(:,2));
axis([-300 300 0 600]);
% Create Sums
n = zeros(1,numSec);
sumX = zeros(1,numSec);
sumY = zeros(1,numSec);
sumXsquared = zeros(1,numSec);
sumYsquared = zeros(1,numSec);
sumXY = zeros(1,numSec);
for i = 1:40
    if ~isnan(outScan(i,1))
        n(id(i)) = n(id(i)) + 1;
        sumX(id(i)) = sumX(id(i)) + outScan(i,1);
        sumY(id(i)) = sumY(id(i)) + outScan(i,2);
        sumXsquared(id(i)) = sumXsquared(id(i)) + outScan(i,1) * outScan(i,1);
        sumYsquared(id(i)) = sumYsquared(id(i)) + outScan(i,2) * outScan(i,2);
        sumXY(id(i)) = sumXY(id(i)) + outScan(i,1) * outScan(i,2);
    end
end

% Regress
for secOn = 1:8
    if n(secOn) > 1
        param(secOn,1) = ( n(secOn) * sumXY(secOn) - ...
            sumY(secOn) * sumX(secOn)) / ( n(secOn) * ...
            sumXsquared(secOn) - sumX(secOn) * sumX(secOn));
        param(secOn,2) = (sumY(secOn) - param(secOn,1) * sumX(secOn)) / ...
            n(secOn);   
        sx = param(secOn,1) * ( sumXY(secOn) - sumX(secOn) * ...
            sumY(secOn) / n(secOn) );
        sy2 = sumYsquared(secOn) - sumY(secOn) * sumY(secOn) / n(secOn);
        sy = sy2 - sx;
        if n(secOn) == 2
            param(secOn,3) = 0;
        else
            param(secOn,3) = sqrt(sy / (n(secOn) - 2));
        end
        pt1x = -300;
        pt1y = pt1x*param(secOn,1) + param(secOn,2);
        pt2x = 300;
        pt2y = pt2x*param(secOn,1) + param(secOn,2);
        line([pt1x pt2x], [pt1y pt2y]);
    elseif n(secOn) == 1
        param(secOn,1) = sumY(secOn)./sumX(secOn);
        param(secOn,2) = 0;
        param(secOn,3) = 0;
    else 
        param(secOn,1) = NaN;
        param(secOn,2) = NaN;
        param(secOn,3) = NaN;
    end
end

% Join Lines
if rep > 1
    for l1 = 1:8
        for l2 = l1+1:8
            if n(l1) > 1 && n(l2) > 0
                jn = n(l1) + n(l2);
                jsumX = sumX(l1) + sumX(l2);
                jsumY = sumY(l1) + sumY(l2);
                jsumXsquared = sumXsquared(l1) + sumXsquared(l2);
                jsumYsquared = sumYsquared(l1) + sumYsquared(l2);
                jsumXY = sumXY(l1) + sumXY(l2);
                jm = ( jn * jsumXY - ...
                    jsumY * jsumX) / ( jn * ...
                    jsumXsquared - jsumX * jsumX);
                jb = (jsumY - jm * jsumX) / ...
                    jn;   
                jsx = jm * ( jsumXY - jsumX * ...
                    jsumY / jn );
                jsy2 = jsumYsquared - jsumY * jsumY / jn;
                jsy = jsy2 - jsx;
                jerr = sqrt(jsy / (jn - 2));
                if n(l2) > 1
                    if jerr*jn < param(l1,3)*n(l1) + param(l2,3)*n(l2) + n(l2)*10
                        id = replaceValue(id,l1,l2);
                        n(l1) = 0;
                        param(l1,:) = [NaN NaN NaN];
                        n(l2) = jn;
                        sumX(l2) = jsumX;
                        sumY(l2) = jsumY;
                        sumXsquared(l2) = jsumXsquared;
                        sumYsquared(l2) = jsumYsquared;
                        sumXY(l2) = jsumXY;
                        param(l2,:) = [jm jb jerr];
                    end
                else
                    if jerr < param(l1,3) + 0.5
                        id = replaceValue(id,l1,l2);
                        n(l1) = 0;
                        param(l1,:) = [NaN NaN NaN];
                        n(l2) = jn;
                        sumX(l2) = jsumX;
                        sumY(l2) = jsumY;
                        sumXsquared(l2) = jsumXsquared;
                        sumYsquared(l2) = jsumYsquared;
                        sumXY(l2) = jsumXY;
                        param(l2,:) = [jm jb jerr];
                    end
                end
            end
        end
    end
end

% Draw Intermediate
% scatter(outScan(:,1),outScan(:,2));
% axis([-300 300 0 600]);
% for secOn = 1:8
%     if n(secOn) > 1
%         pt1x = -300;
%         pt1y = pt1x*param(secOn,1) + param(secOn,2);
%         pt2x = 300;
%         pt2y = pt2x*param(secOn,1) + param(secOn,2);
%         line([pt1x pt2x], [pt1y pt2y]);
%     end
% end

% Distance to line
%y = mx + b   --->   Ax+By+C=0
for i = 1:40
    if ~isnan(outScan(i,1))
        dist = nan(1,8);
        for secOn = 1:8
            if n(secOn) > 0
                A = param(secOn,1);
                B = -1;
                C = param(secOn,2);
                dist(secOn) = abs(A*outScan(i,1) + B*outScan(i,2) + C)/ ...
                    sqrt(A.^2 + B.^2);
            else
                dist(secOn) = inf;
            end
        end
        [minV minI] = min(dist);
        id(i) = minI;
    end
end

end

for secOn = 1:8
    if n(secOn) < 4
        param(secOn,:) = [NaN NaN NaN];
    end
end
%saveas(gcf,[dataPath '/MATLAB_output/reg_' frameName '.bmp']) 

%% Rotate Data
[mv mi] = min(param(:,3));
estYaw = atan(param(mi,1));
rotDataP(:,:) = [outScanP(:,1)+estYaw outScanP(:,2)];
rotData = polarToCartesian2D(rotDataP);
scatter(rotData(:,1),rotData(:,2),'filled');
axis equal

%% Find Yaw
clear dir;
clear b;
numVals = 0;
for li = 1:40
    for lj = li-5:li+5
        if li~=lj && lj > 0 && lj <= 40
        if ~isnan(outScan(li,1)) && ~isnan(outScan(lj,1))
            numVals = numVals + 1;
            m = (outScan(li,2)-outScan(lj,2))/ ...
                (outScan(li,1)-outScan(lj,1));
            dir(numVals) = mod(atan(m),pi()/2);
            b(numVals) = outScan(li,2) - (m*outScan(li,1));
        end
        end
    end
end
hist(dir,45);
axis([0 pi()/2 0 40])
[histAm histLoc] = hist(dir,45);
[mv mi] = max(histAm);
newYaw = histLoc(mi);

%% Rotate Data
rotDataP(:,:) = [outScanP(:,1)+newYaw outScanP(:,2)];
rotData = polarToCartesian2D(rotDataP);
scatter(rotData(:,1),rotData(:,2),'filled');
axis equal
%saveas(gcf,[dataPath '/MATLAB_output/rot_' frameName '.bmp']) 

%% Determine X,Z Histograms
hist(rotData(:,2),16)

%% Use Median Filter
clear xzPts;
numPts = 0;
for i = 1:40
    if ~isnan(rotData(i,1))
        numPts = numPts+1;
        xzPts(numPts,:) = rotData(i,:);
    end
end
avgPts = zeros(numPts-4,2);
for i = 3:numPts-2
    subset = xzPts(i-2:i+2,:);
    subsetX = sortrows(subset,1);
    subsetZ = sortrows(subset,2);
    avgPts(i-2,:) = [subsetX(3,1) subsetZ(3,2)];
end
numPts = numPts-4;
scatter(avgPts(:,1), avgPts(:,2));
axis equal;
saveas(gcf,[dataPath '/MATLAB_output/rotAvg_' frameName '.bmp']) 

%% Detect Lines Init
id = zeros(numPts,1);
for i = 1:numPts
    id(i) = floor((i-1)/5)+1;
end

%% Detect Lines
for rep = 1:5

avgZ = zeros(8,1);
n = zeros(8,1);
for i = 1:numPts
    mID = id(i);
    n(mID) = n(mID) + 1;
    avgZ(mID) = avgZ(mID) + avgPts(i,2);
end
for mID = 1:8
    avgZ(mID) = avgZ(mID) / n(mID);
end

if (rep > 2) 
    for l1 = 1:8
        for l2 = l1+1:8
            if (abs(avgZ(l1) - avgZ(l2)) < 10)
                n(l2) = n(l1) + n(l2);
                n(l1) = 0;
            end
        end
    end
end

for i = 1:numPts
    minV = 999999;
    minI = -1;
    for mID = 1:8
        if n(mID) > 0
            if abs(avgZ(mID) - avgPts(i,2)) < minV
                minV = abs(avgZ(mID) - avgPts(i,2));
                minI = mID;
            end
        end
    end
    id(i) = minI;
end

end

minX = ones(8,1).*999999;
maxX = ones(8,1).*-999999;
for i = 1:numPts
    mID = id(i);
    if avgPts(i,1) > maxX(mID)
        maxX(mID) = avgPts(i,1);
    end
    if avgPts(i,1) < minX(mID)
        minX(mID) = avgPts(i,1);
    end
end

for mID = 1:8
     if maxX(mID) - minX(mID) < 50
         n(mID) = 0;
     end
end

minX = ones(8,1).*999999;
maxX = ones(8,1).*-999999;
for i = 1:numPts
    mID = id(i);
    if avgPts(i,1) > maxX(mID)
        maxX(mID) = avgPts(i,1);
    end
    if avgPts(i,1) < minX(mID)
        minX(mID) = avgPts(i,1);
    end
end

%%

for mID = 1:8
     if maxX(mID) - minX(mID) < 50
         n(mID) = 0;
     end
end

scatter(avgPts(:,1), avgPts(:,2));
axis equal;
for l1 = 1:8
    if n(l1) > 3
        line([-100 150],[avgZ(l1) avgZ(l1)]);
    end
end
saveas(gcf,[dataPath '/MATLAB_output/z_' frameName '.bmp']) 

%%
end

%% Extract Height Slices
[slices source] = extractHeightSlice(pPointCloud, 125, 80, 10);
tmp = printImage(source(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/5_' frameName '.bmp']);

%% Print XZ Plot
pointColor = zeros(size(slices,1),3);
for i = 1:size(slices,1)
    if isnan(slices(i,3))
        pointColor(i,1) = 0;
        pointColor(i,2) = 0;
        pointColor(i,3) = 0;
    else
        pointColor(i,1) = 0;
        pointColor(i,2) = 0;
        pointColor(i,3) = 0;
    end
end

scatter(slices(:,1), slices(:,2), 30, pointColor , 'filled')
axis equal
saveas(gcf,[dataPath '/MATLAB_output/6_' frameName '.bmp']) 

%% Convert to Cartesian
cwPointCloud = polarToCartesian(wPointCloud);
tmp = printImage(cwPointCloud(:,:,3));
axis equal

%% Export Point Cloud to Meshlab Format
exportMeshlab(fPointCloud, [dataPath '/pointClouds/' frameName '_fa.ply']);



%% Load Slice Data
firstSliceFile = 0;
lastSliceFile = 2500;
indexOn = 1;
for fileOn=firstSliceFile:3:lastSliceFile
    frameName = sprintf('%.6u',fileOn);
    fileName = [dataPath '/../sliceData/' frameName '.csv'];
    if exist(fileName, 'file')
        inWallSlice = csvread(fileName);
        newIndex = 1;
        for i=1:40
            if inWallSlice(i,1) ~= -999999.0 ...
                    && inWallSlice(i,2) <= 400
                pTdView{indexOn}(newIndex,1) = -inWallSlice(i,1);
                pTdView{indexOn}(newIndex,2) = inWallSlice(i,2);
                newIndex = newIndex + 1;
            end
        end
        indexOn = indexOn + 1;
    end
end
numData = indexOn - 1;

%% Set Range
curMinRange = 1;
curMaxRange = numData;

%% Convert to Cartesian
for frame = curMinRange:curMaxRange
    cTdView{frame} = polarToCartesian2D(pTdView{frame});
    scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'blue')
    hold on
    frameName = sprintf('%.6u',frame);
    title(['Frame: ' frameName])
    axis([-250 250 0 500])
    drawnow
    hold off
    %saveas(gcf,[dataPath '/MATLAB_output/9_' frameName '.bmp'])
end

%% 
frame = 123;
cTdView{frame} = polarToCartesian2D(pTdView{frame});
scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'blue')
hold on
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
axis([-250 250 0 500])
drawnow
hold off

%% Compare 2 Frames
sep = 1;
inSep = 0;
for frame = 8
    scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'blue')
    hold on
    scatter(cTdView{frame+sep}(:,1), cTdView{frame+sep}(:,2), 30, 'filled', 'red')
    for i = 1:size(cTdView{frame},1)
        if i+inSep > 1 && i+inSep < size(cTdView{frame+sep},1)
        line([cTdView{frame}(i,1) cTdView{frame+sep}(i+inSep,1)], ...
                    [cTdView{frame}(i,2) cTdView{frame+sep}(i+inSep,2)])
        end
    end
    axis([-300 300 0 600])
    hold off
end

%% Determine MSE
sep = 1;
for frame = 8
    inSep = 1;
    nextPos = pTdView{frame+sep};
    trans0 = pTdView{frame};
    trans1 = apply2dTransform(trans0,0,0,0);
    trans2 = apply2dTransform(trans1,0,0,0);
    
    
    determineError(trans2,nextPos,inSep)
    scatter(trans2(:,1), trans2(:,2), 30, 'filled', 'blue')
    hold on
    scatter(nextPos(:,1), nextPos(:,2), 30, 'filled', 'red')
    for i = 1:size(pTdView{frame},1)
        if i+inSep > 1 && i+inSep < size(nextPos,1)
        line([trans2(i,1) nextPos(i+inSep,1)], ...
                    [trans2(i,2) nextPos(i+inSep,2)])
        end
    end
    %axis([-300 300 0 600])
    %axis([-pi() pi() 0 600])
    hold off
end















%% Find Wall Corners
for frame = curMinRange:curMaxRange
[tc fc] = findWallCorners(cTdView{frame}, pTdView{frame});
scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'black')
hold on
scatter(cTdView{frame}(tc,1), cTdView{frame}(tc,2), 30, 'filled', 'green')
scatter(cTdView{frame}(fc,1), cTdView{frame}(fc,2), 30, 'filled', 'red')
hold off
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
axis([-250 250 0 500])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/9_' frameName '.bmp'])
end

%% Show Polar Graph
for frame = curMinRange:curMaxRange
curMaxDis{frame} = 0;
for i=1:size(pTdView{frame},1)
    if ~isnan(pTdView{frame}(i,1))
        if pTdView{frame}(i,2) > curMaxDis{frame}
            curMaxDis{frame} = pTdView{frame}(i,2);
            curMaxDir{frame} = pTdView{frame}(i,1);
        end
    end
end
scatter(pTdView{frame}(:,1), pTdView{frame}(:,2), 30, 'filled', 'blue')
hold on
scatter(curMaxDir{frame}, curMaxDis{frame}, 30, 'filled', 'red')
hold off
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
axis([0.5 2.5 0 500])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])
end

%% Align to Corner
for frame = curMinRange:curMaxRange
cornerX = curMaxDis{frame}*cos(curMaxDir{frame});
cornerZ = curMaxDis{frame}*sin(curMaxDir{frame});
alignedTd{frame}(1:40,1) = cTdView{frame}(:,1) - cornerX;
alignedTd{frame}(1:40,2) = cTdView{frame}(:,2) - cornerZ;
scatter(alignedTd{frame}(:,1), alignedTd{frame}(:,2), 30, 'filled', 'blue')
axis([-300 300 -300 300])
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])
end

%% Find Polar Rotation from Corner Origin
for frame = curMinRange:curMaxRange
pAlignedTd{frame} = cartesianToPolar2D(alignedTd{frame});
curMaxDis{frame} = 0;
for i=1:40
    if ~isnan(pAlignedTd{frame}(i,1))
        if pAlignedTd{frame}(i,2) > curMaxDis{frame}
            curMaxDis{frame} = pAlignedTd{frame}(i,2);
            curMaxDir{frame} = pAlignedTd{frame}(i,1);
        end
    end
end
scatter(pAlignedTd{frame}(:,1), pAlignedTd{frame}(:,2), 30, 'filled', 'blue')
line([curMaxDir{frame} curMaxDir{frame}],[0 curMaxDis{frame}], 'color', 'red')
axis([-pi() pi() 0 600])
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/11_' frameName '.bmp'])
end

%% Find Polar Rotation
for frame = curMinRange:curMaxRange
finalRot{frame} = apply2dTransform(alignedTd{frame}, -curMaxDir{frame}, 0, 0);
scatter(finalRot{frame}(:,1), finalRot{frame}(:,2), 30, 'filled', 'blue')
axis([-300 300 -300 300])
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])
end

%% Show Nearest Neighbors
frameName = sprintf('%.6u',frame1);
matches = nearestNeighbor(cTdView1, cTdView2);
scatter(cTdView1(:,1), cTdView1(:,2), 30, 'filled', 'blue')
hold on
scatter(cTdView2(:,1), cTdView2(:,2), 30, 'filled', 'red')
for i = 1:40
    if ~isnan(matches(i))
        line([cTdView1(i,1) cTdView2(matches(i),1)], ...
            [cTdView1(i,2) cTdView2(matches(i),2)])
    end
end
axis([-300 300 0 600])
hold off
saveas(gcf,[dataPath '/MATLAB_output/7_' frameName '.bmp'])