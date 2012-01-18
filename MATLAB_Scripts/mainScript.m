%% Initial Constants
frameName = '000012';

%% Loop
lastFile = 32;
tdView = cell(lastFile+1,1);
for fileOn = 0:33
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
tdView{fileOn+1} = maxDisList;

end


%% Fit Line to the Right Wall
% yaw = determineRightWall(maxDisList);
% scatter(maxDisList(:,1), maxDisList(:,2), 30, 'filled')
% line([-200 200], [((yaw(1)*-200)+yaw(2)) ((yaw(1)*200)+yaw(2))])
% axis equal
% saveas(gcf,[dataPath '/MATLAB_output/33_' frameName '.bmp']) 

%% Fit Lines to Walls
lines = fitWallsToLines(maxDisList);
scatter(maxDisList(:,1), maxDisList(:,2), 30, 'filled')
t = 2;
line([-20 20], [((lines(t,1)*-20)+lines(t,2)) ...
    ((lines(t,1)*20)+lines(t,2))])
axis equal
saveas(gcf,[dataPath '/MATLAB_output/34_' frameName '.bmp'])

%% Segment Walls
wPointCloud = segmentWalls(pPointCloud, maxDisListPolar);
tmp = printImage(wPointCloud(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/4_' frameName '.bmp']);

%% Extract Height Slices
[slices source] = extractHeightSlice(pPointCloud, 125, 80, 10);
tmp = printImage(source(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/5_' frameName '.bmp']);

%% Print XZ Plot
pointColor = zeros(size(slices,1),3);
for i = 1:size(slices,1)
    if isnan(slices(i,3))
        pointColor(i,1) = 1;
        pointColor(i,2) = 0;
        pointColor(i,3) = 0;
    else
        pointColor(i,1) = slices(i,3);
        pointColor(i,2) = slices(i,3);
        pointColor(i,3) = slices(i,3);
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

%% ICP Initial Conditions
frame1 = 19;

%% ICP Loop
for frame1 = 1:lastFile

%% Show Nearest Neighbors
frameName = sprintf('%.6u',frame1);
matches = nearestNeighbor(tdView{frame1}, tdView{frame1+1});
scatter(tdView{frame1}(:,1), tdView{frame1}(:,2), 30, 'filled', 'blue')
hold on
scatter(tdView{frame1+1}(:,1), tdView{frame1+1}(:,2), 30, 'filled', 'red')
for i = 1:40
    if ~isnan(matches(i))
        line([tdView{frame1}(i,1) tdView{frame1+1}(matches(i),1)], ...
            [tdView{frame1}(i,2) tdView{frame1+1}(matches(i),2)])
    end
end
axis equal
hold off
saveas(gcf,[dataPath '/MATLAB_output/7_' frameName '.bmp'])

%% Setup Matricies for ICP
clear T, M;
M1 = tdView{frame1+1}';
j = 1;
for i=1:size(M1,2)
    if ~isnan(M1(1,i))
        M(1,j) = M1(1,i);
        M(2,j) = M1(2,i);
        j = j+1;
    end
end
T1 = tdView{frame1}';
j = 1;
for i=1:size(T1,2)
    if ~isnan(T1(1,i))
        T(1,j) = T1(1,i);
        T(2,j) = T1(2,i);
        j = j+1;
    end
end

%% Perform ICP
Tr_fit = icpMex(M,T,eye(3),-1,'point_to_plane');
T_fit  = Tr_fit(1:2,1:2)*T + Tr_fit(1:2,3)*ones(1,size(T,2));

ms=8; lw=2; fs=16;
plot(M(1,:),M(2,:),'or','MarkerSize',ms,'LineWidth',lw);
hold on;
plot(T(1,:),T(2,:),'sg','MarkerSize',ms,'LineWidth',lw);
plot(T_fit(1,:),T_fit(2,:),'xb','MarkerSize',ms,'LineWidth',lw);
legend('End Points','Start Points','Fitted','Location','South');
set(gca,'FontSize',fs);
axis equal
hold off;
saveas(gcf,[dataPath '/MATLAB_output/8_' frameName '.bmp'])

end