function [ outList ] = randomFromList( inList )
%FUNCTIONTEMPLATE Summary of this function goes here
%   Input
%       inList - A list at least 1 wide.
%   Output
%       outList - A random item from the list.
%   Description
%       Returns a random item from a list.
%
%   Change Log
%       01/11/2012 - Tom Dickman - Created inital script

    randomNumber = floor(rand()*size(inList,1) + 1);
    outList = inList(randomNumber, :);
end
