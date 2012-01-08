function [ outPolarPC ] = cartesianToPolar( inPC )
%CARTESIANTOPOLAR Converts from cartesian coordinates to polar
%   Input
%       inPC - Point cloud with cartesian coordinates
%           (x, y, z)
%   Output
%       outPolarPC - Point cloud with polar coordinates
%           (height, direction, distance)
%   Description
%       The input point cloud is in the <x,y,z> coordinate system. The
%       function then determines the polar coordinates
%       <height,direction,distance>. With the floor oriented in the -y
%       direction this will put the walls at the maximum distance
%       coordiante for each direction.
%           height = y
%           dir = atan2(z,x)
%           dis = sqrt(x^2 + z^2)
%
%   Change Log
%       12/17/2011 - John Gideon - Created Script
%       12/19/2011 - Tom Dickman - Changed name to cartesianToPolar
%       12/19/2011 - John Gideon - Added description
%       01/05/2012 - John Gideon - Added support for color

    outPolarPC = zeros(size(inPC,1), size(inPC,2), size(inPC,3));
    for i = 1:size(inPC,1)
        for j = 1:size(inPC,2)
            x = inPC(i,j,1);
            y = inPC(i,j,2);
            z = inPC(i,j,3);
            
            outPolarPC(i,j,1) = y;
            outPolarPC(i,j,2) = atan2(z,x);
            outPolarPC(i,j,3) = sqrt(x.*x + z.*z);
            if size(inPC,3) == 4 
                outPolarPC(i,j,4) = inPC(i,j,4);
            end
        end
    end

end

