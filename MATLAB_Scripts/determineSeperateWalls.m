function [ yaw ] = determineSeperateWalls( maxDisList )
%DETERMINESEPERATEWALLS Summary of this function goes here
%   Input
%       maxDisList - 1D List of cartesian 2D points for walls
%   Output
%       yaw - Slope and intercept of line fitting the wall [m b]
%   Description
%       What does this function do?
%
%   Change Log
%       01/11/2012 - John Gideon - Created Function
%       01/12/2012 - John Gideon - Added manual regression math

    maxZ = -999999;
    indexMaxZ = -1;
    for i=1:size(maxDisList,1)
        if (maxDisList(i,2) > maxZ) 
            maxZ = maxDisList(i,2);
            indexMaxZ = i;
        end
    end
    
    if (indexMaxZ + 7 > size(maxDisList,1))
        j = size(maxDisList,1);
    else
        j = indexMaxZ + 7;
    end
    
    % http://people.hofstra.edu/stefan_waner/realworld/
    % calctopic1/regression.html
    sumX = 0;
    sumZ = 0;
    sumXZ = 0;
    sumX2 = 0;
    
    n = j - indexMaxZ + 1;
    for i=indexMaxZ:j
        sumX = sumX + maxDisList(i,1);
        sumZ = sumZ + maxDisList(i,2);
        sumXZ = sumXZ + (maxDisList(i,1) * maxDisList(i,2));
        sumX2 = sumX2 + (maxDisList(i,1) * maxDisList(i,1));
    end
    
    yaw(1) = ((n * sumXZ) - (sumX * sumZ)) / ((n * sumX2) - (sumX * sumX));
    yaw(2) = (sumZ - (yaw(1) * sumX)) / n;
end

