function [ outCartesianPC ] = polarToCartesian( inPC )
%POLARTOCARTESIAN Converts from polar coordinates to cartesian
%   Input
%       inPC - Point cloud with polar coordinates
%   Output
%       outPolarPC - Point cloud with cartesian coordinates
%           (height, direction, distance)
%   Description
%       What does this function do?
%
%   Change Log
%       12/19/2011 - Tom Dickman - Created Script

    outCartesianPC = zeros(size(inPC,1), size(inPC,2), 3);
    for i = 1:size(inPC,1)
        for j = 1:size(inPC,2)
            height = inPC(i,j,1);
            theta = inPC(i,j,2);
            r = inPC(i,j,3);
            
            % X
            outCartesianPC(i,j,1) = r*cos(theta);
            % Y - height
            outCartesianPC(i,j,2) = height;
            % Z - out / in
            outCartesianPC(i,j,3) = r*sin(theta);
        end
    end
end