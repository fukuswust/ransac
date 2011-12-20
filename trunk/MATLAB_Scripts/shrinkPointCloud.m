function [ outPC ] = shrinkPointCloud( inPC, factor )
%SHRINKPOINTCLOUD Reduces the size of a point cloud
%   Inputs
%       inPC - Input 2D array of 3D points (height,width,3)
%       factor - Scaling factor (eg. 4 = 1/4 width and height of original)
%   Output
%       outPC - Input matrix with shrink operation applied
%   Description
%       Takes a 2D array of 3D points and scales it in size by an amount 
%       of 1/<factor> in the width and height directions. This is 
%       accomplished by incrementing in both directions by "factor". This 
%       should result in a total decrease of data by 1/<factor>^2.
%
%   Change Log
%       12/16/2011 - John Gideon - Created Function
%       12/19/2011 - John Gideon - Added description

    outPC = NaN(floor(size(inPC,1)./factor), ...
        floor(size(inPC,2)./factor), 3);
    for i = 0:floor(size(inPC,1)./factor)-1
        for j = 0:floor(size(inPC,2)./factor)-1
            outPC(i+1,j+1,:) = inPC((i*factor)+1,(j*factor)+1,:);
        end        
    end

end

