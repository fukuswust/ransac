function [ pointCloud ] = zToPointCloud( zArray )
%ZTOPOINTCLOUD Convert array of depths to point cloud
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
%       11/09/2011 - John Gideon - Initial equation input and tested
    
    fx_d = 5.9421434211923247e+02;
	fy_d = 5.9104053696870778e+02;
	cx_d = 3.3930780975300314e+02;
	cy_d = 2.4273913761751615e+02;
    
    pointCloud(:,:,1) = zeros(size(zArray,1),size(zArray,2));
    pointCloud(:,:,2) = zeros(size(zArray,1),size(zArray,2));
    pointCloud(:,:,3) = zArray;
    for i = 1:size(zArray,1)
        for j = 1:size(zArray,2)
            %Equation goes here
            z = zArray(i,j);
            pointCloud(i,j,1) = ((j - cx_d) * z) / fx_d;
            pointCloud(i,j,2) = ((i - cy_d) * z) / fy_d;
            pointCloud(i,j,3) = z;
        end
    end

end