function [ zArray ] = depthToZ( depthArray )
%DEPTHTOZ Converts raw depth values from Kinect to z coordinates
%   Input
%       depthArray - Raw data array containing NaN where acquisition error
%   Output
%       zArray - Output array with z coordinates with resolution of 1mm
%   Description
%       Converts values from raw depth table to actual distances.
%       IDEA: Since we have a lot of repeated values, we could find an
%       efficient way to store and retrieve values instead of recalculating
%       them.
%
%   Change Log
%       11/01/2011 - John Gideon - Created Shell Script
    
    for i=1:480
        for j=1:640
            % Equation conveniently borrowed from
            % http://openkinect.org/wiki/Imaging_Information. Used basic
            % approximation to reduce calculation time
            depthArray(i,j) = 100/(-0.00307 * depthArray(i,j) + 3.33);
        end
    end
    
%     % Precalculate all possible values and place in array
%     for i=1:2048
%         % Equation conveniently borrowed from
%         % http://openkinect.org/wiki/Imaging_Information. Used basic
%         % approximation to reduce calculation time
%         precalculatedArray(i) = 100/(-0.00307 * i + 3.33);
%     end
%     
%     for i=1:480
%         for j=1:640
%             depthArray(i,j) = precalculatedArray(depthArray(i,j));
%         end
%     end
    
    zArray = depthArray;
end

