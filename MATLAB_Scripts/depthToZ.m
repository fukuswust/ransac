function [ zArray ] = depthToZ( depthArray )
%DEPTHTOZ Converts raw depth values from Kinect to z coordinates
%   Input
%       depthArray - Raw data array containing NaN where acquisition error
%   Output
%       zArray - Output array with z coordinates with resolution of 1mm
%   Description
%       What does this function do?
%
%   Change Log
%       11/01/2011 - John Gideon - Created Shell Script

    zArray = depthArray;

end

