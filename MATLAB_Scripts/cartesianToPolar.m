function [ outPolarPC ] = cartesianToPolar( inPC )
%CARTESIANTOPOLAR Converts from cartesian coordinates to polar
%   Input
%       inPC - Point cloud with cartesian coordinates
%   Output
%       outPolarPC - Point cloud with polar coordinates
%           (height, direction, distance)
%   Description
%       What does this function do?
%
%   Change Log
%       12/17/2011 - John Gideon - Created Script
%       12/19/2011 - Tom Dickman - Changed name to cartesianToPolar

    outPolarPC = zeros(size(inPC,1), size(inPC,2), 3);
    for i = 1:size(inPC,1)
        for j = 1:size(inPC,2)
            x = inPC(i,j,1);
            y = inPC(i,j,2);
            z = inPC(i,j,3);
            
            outPolarPC(i,j,1) = y;
            outPolarPC(i,j,2) = atan2(z,x);
            outPolarPC(i,j,3) = sqrt(x.*x + z.*z);
        end
    end

end

