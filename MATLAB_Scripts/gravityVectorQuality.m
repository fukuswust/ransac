function [ Q ] = gravityVectorQuality( gravityVect )
%GRAVITYVECTORQUALITY Returns %error between gravity magnitude and 1g
%   Input
%       gravityVect - accelerometer values (3x1)
%   Output
%       Q - Quality rating
%   Description
%       What does this function do?
%
%   Change Log
%       12/16/2011 - John Gideon - Created Function

    mag = sqrt(sum(gravityVect.*gravityVect));
    Q = (abs((819/512)-mag)/(819/512));

end

