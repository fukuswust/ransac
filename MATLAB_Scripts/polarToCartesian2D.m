function [ outCartesianPC ] = polarToCartesian2D( inPC )
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
%       01/11/2012 - John Gideon - Created Script

    outCartesianPC = zeros(size(inPC,1), size(inPC,2));
    for i = 1:size(inPC,1)
        theta = inPC(i,1);
        r = inPC(i,2);

        % X
        outCartesianPC(i,1) = r*cos(theta);
        % Z - out / in
        outCartesianPC(i,2) = r*sin(theta);

        if size(inPC,2) == 3 
            outCartesianPC(i,3) = inPC(i,3);
        end
    end
end