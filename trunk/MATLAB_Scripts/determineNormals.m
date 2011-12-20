function [ normals ] = determineNormals( pointCloud )
%DETERMINENORMALS Calculate the surface normals
%   Input
%       pointCloud - Point cloud for the raw data consisting of a 3D 
%       array of coordinates
%   Output
%       normals - 3D array of output normal directions (point relative to
%       <0,0,0>
%   Description
%       What does this function do?
%
%   Change Log
%       11/16/2011 - John Gideon - Created Shell Template

    normals = zeros(size(pointCloud,1),size(pointCloud,2),3);
    for i = 1:size(pointCloud,1)
        for j = 10:size(pointCloud,2)
            %Equation goes here
            normals(i,j,1) = pointCloud(i,j,1)-pointCloud(i,j-9,1);
            normals(i,j,2) = pointCloud(i,j,2)-pointCloud(i,j-9,2);
            normals(i,j,3) = pointCloud(i,j,3)-pointCloud(i,j-9,3);
        end
    end
end

