function [ list ] = makeList( pointCloud )
%FUNCTIONTEMPLATE Summary of this function goes here
%   Input
%       pointCloud - A point cloud
%   Output
%       list - A list of point cloud values that are not NaN.
%   Description
%       Returns a list of all point cloud coordinates that are values
%
%   Change Log
%       01/11/2012 - Tom Dickman - Created inital script

    % Preallocate array - maximum length
    list(:,:) = NaN(size(pointCloud,1)*size(pointCloud,2), 3);
    
    % Copy all values over to new array
    counter = 0;
    for i=1:size(pointCloud,1)
        for j=1:size(pointCloud,2)
            if ~isnan(pointCloud(i, j, 1))
                counter = counter + 1;
                list(counter, :) = pointCloud(i, j, :);
            end
        end
    end
    % Recreate proper list length
    list((counter+1):size(list,1),:) = [];
end
