function [ outPC ] = cartesianToPolar2D( inPC )
%POLARTOCARTESIAN Converts from polar coordinates to cartesian
%   Input
%       inPC - List of polar coordinates (2D)
%           (direction, distance)
%   Output
%       outCartesianPC - List of cartesian coordinates (2D)
%           (x, z)
%   Description
%       The input point cloud is in the <direction,distance> polar 
%       coordinate system. The function then determines the cartesian  
%       coordinates <x,z> using trigonometry.
%           x = dis*cos(dir)
%           z = dis*sin(dir)
%
%   Change Log
%       01/17/2012 - John Gideon - Created Script

    outPC = zeros(size(inPC,1), size(inPC,2));
    for i = 1:size(inPC,1)
        x = inPC(i,1);
        z = inPC(i,2);
        dis = sqrt((x*x)+(z*z));
        dir = atan2(z,x);

        outPC(i,1) = dir;
        outPC(i,2) = dis;

        if size(inPC,2) == 3 
            outPC(i,3) = inPC(i,3);
        end
    end
end

