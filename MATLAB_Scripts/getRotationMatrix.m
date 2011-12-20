function [ R ] = getRotationMatrix( A, B )
%GETROTATIONMATRIX Determines rotation matric from two vectors
%   Input
%       A - Start vector (3x1)
%       B - End vector (3x1)
%   Output
%       R - Rotation matrix (3x3)
%   Description
%       What does this function do?
%
%   Change Log
%       12/16/2011 - John Gideon - Created Function

    magA = sqrt(sum(A.*A));
    magB = sqrt(sum(B.*B));
    uA = A./magA;
    uB = B./magB;
    c = dot(uA,uB);
    s = sqrt(1-(c*c));
    t = 1 - c;
    RotAxis = cross(uA,uB);
    magRotAxis = sqrt(sum(RotAxis.*RotAxis));
    uRotAxis = RotAxis./magRotAxis;
    x = uRotAxis(1);
    y = uRotAxis(2);
    z = uRotAxis(3);   
    
    xy = x.*y;
    xz = x.*z;
    yz = y.*z;
    
    M1 = [x*x xy xz; xy y*y yz; xz yz z*z];
    M2 = [0 -z y; z 0 -x; -y x 0];
    
    R = (c.*eye(3))+(t.*M1)+(s.*M2);
end

