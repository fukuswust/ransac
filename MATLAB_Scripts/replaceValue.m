function [ outArray ] = replaceValue( inArray, oldValue, newValue )
%REPLACEVALUE Takes an array and switches out one value for another
%   Inputs
%       inArray - General input array
%       oldValue - Value to replace
%       newValue - Value with which to replace
%   Output
%       outArray - General output array
%   Description
%       This function is used to take an input array and interchange all of
%       one value to another.
%
%   Change Log
%       11/01/2011 - John Gideon - Created first implementation

    outArray = inArray;
    for i = 1:size(inArray,1)
        for j = 1:size(inArray,2)
            if inArray(i,j) == oldValue
                outArray(i,j) = newValue;
            end
        end
    end
    
end