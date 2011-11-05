function [ pointCloud ] = zToPointCloud( zArray )
%ZTOPOINTCLOUD Summary of this function goes here
%   Input
%       zArray - Array with z coordinates with resolution of 1mm
%   Output
%       pointCloud - Local created point cloud for the raw data consisting
%       of a 3D array of coordinates
%   Description
%       What does this function do?
%
%   Change Log
%       11/02/2011 - John Gideon - Created Shell Script

    pointCloud(:,:,1) = zeros(size(zArray,1),size(zArray,2));
    pointCloud(:,:,2) = zeros(size(zArray,1),size(zArray,2));
    pointCloud(:,:,3) = zArray;
    for i = 1:size(zArray,1)
        for j = 1:size(zArray,2)
            %Equation goes here
            pointCloud(i,j,1) = i;
            pointCloud(i,j,2) = j;
            pointCloud(i,j,3) = zArray(i,j);
        end
    end

end