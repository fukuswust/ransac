function [ matches ] = nearestNeighbor( pc1, pc2 )
%FUNCTIONTEMPLATE Summary of this function goes here
%   Input
%       pc1 - First point cloud - stored
%       pc2 - Second point cloud
%   Output
%       matches - Explanation
%   Description
%       What does this function do?
%
%   Change Log
%       11/01/2011 - John Gideon - Created Shell Template
    % Initialize matches initially to all NaN
    matches = NaN(40,1);
    
    % Set minDistance to some large value
    closestPoint = NaN;
    minDistance = 999999;

    % Loop through pc1 - return the point closest to each point
    for n=1:size(pc1, 1)
        % Find closest point, and place index in matches array
        xi = pc1(n,1);
        yi = pc1(n,2);
        
        % Find distances to all other point in other array, and save
        % minimum to minDistance
        for i=1:size(pc2, 1)
            xf = pc2(i,1);
            yf = pc2(i,2);
            % Don't need to take sqrt of distance - we only care about
            % finding the lowest value, not the actual distance.
            distance = (xf-xi)^2 + (yf-yi)^2;
            if distance < minDistance
                minDistance = distance;
                closestPoint = i;
            end
        end
        matches(n) = closestPoint;
        % Reset minDistance to some large value
        closestPoint = NaN;
        minDistance = 999999;
    end
end

