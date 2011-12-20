function [ outPC ] = shrinkPointCloud( inPC, factor )
%SHRINKPOINTCLOUD Reduces the size of a point cloud
%   Inputs
%       inPC - Input 2D array of 3D points (i,j,3)
%       factor - Scaling factor (eg. 4 = 1/4 width of original)
%   Output
%       outPC - Input matrix with shrink operation applied
%   Description
%       More Detail
%
%   Change Log
%       12/16/2011 - John Gideon - Created Function

    outPC = NaN(floor(size(inPC,1)./factor), ...
        floor(size(inPC,2)./factor), 3);
    for i = 0:floor(size(inPC,1)./factor)-1
        for j = 0:floor(size(inPC,2)./factor)-1
            outPC(i+1,j+1,:) = inPC((i*factor)+1,(j*factor)+1,:);
        end        
    end

end

