function [ outArray ] = replaceValue( inArray, oldValue, newValue )
%REPLACEVALUE Takes an array and switches out one value for another
%   Inputs
%       inArray - General 2D input array
%       oldValue - Value to replace (CANNOT BE NaN)
%       newValue - Value with which to replace
%   Output
%       outArray - General 2D output array
%   Description
%       This function is used to take an input array and interchange all of
%       one value to another. Usually used to convert a certain value with
%       a NaN.
%       WARNING: CANNOT BE USED TO REPLACE NaN WITH ANOTHER NUMBER
%
%   Change Log
%       11/01/2011 - John Gideon - Created first implementation
%       12/19/2011 - John Gideon - Edited description

    outArray = inArray;
    for i = 1:size(inArray,1)
        for j = 1:size(inArray,2)
            if inArray(i,j) == oldValue
                outArray(i,j) = newValue;
            end
        end
    end
    
end