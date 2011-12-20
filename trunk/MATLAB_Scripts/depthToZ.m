function [ zArray ] = depthToZ( depthArray )
%DEPTHTOZ Converts raw depth values from Kinect to z coordinates
%   Input
%       depthArray - Raw data array containing NaN where acquisition error
%   Output
%       zArray - Output array with z coordinates with resolution of 1mm
%   Description
%       Converts values from raw depth to actual distances (z). The
%       conversion is approximated by the following inverse equation: 
%           100/(-0.00307 * depth + 3.33);
%       IDEA: Since we have a lot of repeated values, we could find an
%       efficient way to store and retrieve values instead of recalculating
%       them.
%       COUNTER: Memory access times on the LUT may actually make this
%       method slower.
%
%   Change Log
%       11/01/2011 - John Gideon - Created Shell Script
%       11/05/2011 - Tom Dickman - Added intial code for script - two versions
%        available. One that precalculates all values, and one that calculates
%        all values.
%       12/19/2011 - John Gideon - Edited description

    zArray = zeros(size(depthArray,1), size(depthArray,2));
    for i=1:480
        for j=1:640
            % http://openkinect.org/wiki/Imaging_Information. Used basic
            % approximation to reduce calculation time
            zArray(i,j) = 100/(-0.00307 * depthArray(i,j) + 3.33);
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
%             zArray(i,j) = precalculatedArray(depthArray(i,j));
%         end
%     end
end

