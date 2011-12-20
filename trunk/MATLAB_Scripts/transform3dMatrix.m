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

    outM = zeros(size(inM,1), size(inM,2), 3);
    for i = 1:size(inM,1)
        for j = 1:size(inM,2)
            outM(i,j,:) = [inM(i,j,1) inM(i,j,2) inM(i,j,3)]*transM;
        end
    end

end

