function [ transform ] = comparePlanes( planes, prevPlanes )
%COMPAREPLANES Compare two sets of planes and determine transform
%   Input
%       planes - List of point cloud segments represented as 4D array
%   Output
%       planes - List of plane structures
%   Description
%       What does this function do?
%
%   Change Log
%       11/16/2011 - John Gideon - Created Shell Script

        correlates = 1;
        if (correlates)
            transform = [1 2 3 4 5 6];
        else
            transform = [NaN NaN NaN NaN NaN NaN];
        end
        
end

