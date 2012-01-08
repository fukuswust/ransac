function [ outPC ] = segmentFloor( inPC )
%SEGMENTFLOOR Summary of this function goes here
%   Input
%       inPC - Point cloud oriented with -y in direction of gravity
%   Output
%       outPC - Segmented floor point cloud
%   Description
%       What does this function do?
%
%   Change Log
%       12/16/2011 - John Gideon - Created Function
%       01/05/2012 - John Gideon - Added support for color

    outPC = NaN(size(inPC,1), size(inPC,2), size(inPC,3)); 
    minValue = min(min(inPC(:,:,2)));
    for i = 1:size(inPC,1)
        for j = 1:size(inPC,2)
            if (inPC(i,j,2) < minValue+25)
                outPC(i,j,:) = inPC(i,j,:);
            end
        end
    end
    
end

