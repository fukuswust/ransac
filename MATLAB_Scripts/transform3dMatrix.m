function [ outM ] = transform3dMatrix( inM, transM )
%TRANSFORM3DMATRIX Applies transformation matrix to 2D array of 3D points
%   Input
%       inM - Input 2D array of 3D points (i,j,3)
%       transM - Transformation matrix to apply (3x3)
%   Output
%       outM - Input matrix with transformation applied
%   Description
%       This function simply applies a matrix multiplication to every
%       vector within a 2D array of vectors, as shown in the following
%       equation:
%          outM(i,j,:) = inM(i,j,:)*transM
%
%   Change Log
%       12/16/2011 - John Gideon - Created Function
%       12/17/2011 - John Gideon - Cleanup
%       12/19/2011 - John Gideon - Edited description
%       01/05/2012 - John Gideon - Added support for color

    outM = zeros(size(inM,1), size(inM,2), size(inM,3));
    for i = 1:size(inM,1)
        for j = 1:size(inM,2)
            outM(i,j,1:3) = [inM(i,j,1) inM(i,j,2) inM(i,j,3)]*transM;
            if size(inM,3) == 4
                outM(i,j,4) = inM(i,j,4);
            end
        end
    end

end

