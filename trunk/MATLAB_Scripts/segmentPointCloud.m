function [ segments ] = segmentPointCloud( pointCloud, normals )
%SEGMENTPOINTCLOUD Divides the point cloud into discrete segments by normal
%   Inputs
%       pointCloud - Point cloud for the raw data consisting of a 3D 
%       array of coordinates
%       normals - 3D array of output normal directions (point relative to
%       <0,0,0>
%   Output
%       segments - List of point cloud segments represented as 4D array
%   Description
%       What does this function do?
%
%   Change Log
%       11/16/2011 - John Gideon - Created Shell Script

    segments(:,:,:,1) = zeros(size(pointCloud,1),size(pointCloud,2),...
        size(pointCloud,3));
    segments(:,:,:,2) = zeros(size(pointCloud,1),size(pointCloud,2),...
        size(pointCloud,3));
    for i = 1:size(pointCloud,1)
        for j = 1:size(pointCloud,2)
            
        end
    end

end

