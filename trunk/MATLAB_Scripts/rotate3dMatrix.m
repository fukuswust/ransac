function [ outM ] = rotate3dMatrix( inM, rotM )
%ROTATE3DMATRIX Applies rotation matrix to 2D array of 3D points
%   Input
%       inM - Input 2D array of 3D points (i,j,3)
%       rotM - Rotation matrix to apply (3x3)
%   Output
%       outM - Input matrix with rotation applied
%   Description
%       outM(i,j,:) = inM(i,j,:)*rotM
%
%   Change Log
%       12/16/2011 - John Gideon - Created Function
%       12/17/2011 - John Gideon - Cleanup

    outM = zeros(size(inM,1), size(inM,2), 3);
    for i = 1:size(inM,1)
        for j = 1:size(inM,2)
            outM(i,j,:) = [inM(i,j,1) inM(i,j,2) inM(i,j,3)]*rotM;
        end
    end

end

