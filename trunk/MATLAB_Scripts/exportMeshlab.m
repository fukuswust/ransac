function exportMeshlab( pointCloud, filePath )
%EXPORTMESHLAB Summary of this function goes here
%   Input
%       pointCloud - 3D array of point cloud coordinates
%       filePath - Path to export Meshlab file
%   Description
%       What does this function do?
%
%   Change Log
%       11/05/2011 - John Gideon - Created Shell Script

    for i = 1:size(zArray,1)
        for j = 1:size(zArray,2)
            x = pointCloud(i,j,1);
            y = pointCloud(i,j,2);
            z = pointCloud(i,j,3);
            filePath;
        end
    end
    
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

