function exportMeshlab( pointCloud, filePath )
%EXPORTMESHLAB Converts Matlab point cloud to Meshlab format
%   Input
%       pointCloud - 3D array of point cloud coordinates
%       filePath - Path to export Meshlab file
%   Description
%       This function takes a Matlab array formatted point cloud and
%       formats it to Mashlab formatted data
%
%   Change Log
%       11/05/2011 - John Gideon - Created Shell Script
%       11/09/2011 - Benjamin Zerhusen - Updated Code to perform file
%                    creation

totNum = 0;
for i = 1:size(pointCloud,1)
    for j = 1:size(pointCloud,2)
        if(~isnan(pointCloud(i,j,3)));
            totNum = totNum + 1;
        end
    end
end

fid = fopen(filePath,'w');

%Write header
fprintf(fid,'ply\n');
fprintf(fid,'format ascii 1.0\n');
fprintf(fid,'element vertex %i\n',totNum);
fprintf(fid,'property float x\n');
fprintf(fid,'property float y\n');
fprintf(fid,'property float z\n');
fprintf(fid,'end_header\n');

%Loop to write data points to file
for i = 1:size(pointCloud,1)
    for j = 1:size(pointCloud,2)
        if(~isnan(pointCloud(i,j,3)))
            fprintf(fid,'%f ',pointCloud(i,j,1));
            fprintf(fid,'%f ',pointCloud(i,j,2));
            fprintf(fid,'%f \n',pointCloud(i,j,3));
        end
    end
end
    
fclose(fid);

str = sprintf('File %s Was Successfully Written', filePath);
disp(str);

%OUTPUT FILE FORMAT - 140160 is the # of total points in the file
% ply
% format ascii 1.0
% element vertex 140160
% property float x
% property float y
% property float z
% end_header
% 134.65549466758  100.991621000685  210.380200398184
% 136.009884901754  102.327186132981  213.030131216233
% 136.484809725935  103.007403566744  214.379768981634
% ...
% -1.56124336446591  -9.46503789707457  56.4655763233901
% -1.65882107474503  -9.46503789707457  56.4655763233901

end

