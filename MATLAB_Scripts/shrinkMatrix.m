function [ outM ] = shrinkMatrix( inM, factor )
%SHRINKMATRIX Reduces the size of a matrix
%   Inputs
%       inM - Input 2D array (height,width)
%       factor - Scaling factor (eg. 4 = 1/4 width of original)
%   Output
%       outM - Input matrix with shrink operation applied
%   Description
%       Takes a 2D array and scales it in size by an amount of 1/<factor>
%       in the width and height directions. This is accomplished by
%       incrementing in both directions by "factor". This should result in
%       a total decrease of data by 1/<factor>^2.
%
%   Change Log
%       12/19/2011 - John Gideon - Created Function and added description

    outM = NaN(floor(size(inM,1)./factor), ...
        floor(size(inM,2)./factor));
    for i = 0:floor(size(inM,1)./factor)-1
        for j = 0:floor(size(inM,2)./factor)-1
            outM(i+1,j+1) = inM((i*factor)+1,(j*factor)+1);
        end        
    end

end
