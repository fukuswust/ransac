function [ outImage ] = printImage( inMatrix )
%PRINTIMAGE Takes matrix and prints normalized image to screen
%   Input
%       input_args - Any 2D matrix
%   Output
%       outImage - Normalized greyscale matrix with red for NaN
%   Description
%       This function normalizes the input matrix to values between 0 and
%       255. It then prints to the screen in greyscale except for NaN
%       values, which it draw in red.
%
%   Change Log
%       12/13/2011 - John Gideon - Created Script
%       12/16/2011 - John Gideon - Added red for NaN
%       12/19/2011 - John Gideon - Added Description

    outImage = zeros(size(inMatrix,1),size(inMatrix,2), 3);
    imMin = min(min(inMatrix));
    imMax = max(max(inMatrix));
    display(sprintf('Normalize Min: %d', imMin))
    display(sprintf('Normalize Max: %d', imMax))
    multFactor = 1/(imMax-imMin);
    for i = 1:size(inMatrix,1)
        for j = 1:size(inMatrix,2)
            if isnan(inMatrix(i,j))
                outImage(i,j,:) = [1 0 0]; %red
            else
                level = (inMatrix(i,j)-imMin)*multFactor;
                outImage(i,j,:) = [level level level]; %grey
            end
        end
    end
    image(outImage)
end