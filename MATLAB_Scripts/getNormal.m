function [ normal ] = getNormal( P1, P2, P3 )
%FUNCTIONTEMPLATE Summary of this function goes here
%   Input
%       P1, P2, P3 - three points represented by 3x1 matrices.
%   Output
%       normal - the vector that is normal to those three points.
%   Description
%       What does this function do?
%
%   Change Log
%       11/01/2011 - John Gideon - Created Shell Template

    normal = cross(P1-P2, P1-P3);
    length = sqrt( normal(1)^2 + normal(2)^2 + normal(3)^2 );
    normal = normal / length; % Convert into unit vector
end