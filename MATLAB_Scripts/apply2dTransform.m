function [ outPC ] = apply2dTransform( inPC, rot, tx, tz )
%APPLY2DTRANSFORM Summary of this function goes here
%   Input
%       input_args - Explanation
%   Output
%       output_args - Explanation
%   Description
%       What does this function do?
%
%   Change Log
%       11/01/2011 - John Gideon - Created Shell Template

    outPC = NaN(size(inPC,1),size(inPC,2));
    for i = 1:size(inPC,1)
        tmpX = inPC(i,1) + tx;
        tmpZ = inPC(i,2) + tz;
        
        dir = atan2(tmpZ, tmpX);
        dis = sqrt((tmpX*tmpX)+(tmpZ*tmpZ));
        
        outPC(i,1) = dis*cos(dir+rot);
        outPC(i,2) = dis*sin(dir+rot);
    end
    
    

end

