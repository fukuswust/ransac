function [ outPolarPC ] = segmentWalls( inPolarPC )
%SEGMENTWALLS Finds walls in a point cloud
%   Input
%       inPolarPC - Input 2D array of 3D points (polar)
%   Output
%       outPolarPC - Segmented array of wall points (polar)
%   Description
%       outM(i,j,:) = inM(i,j,:)*rotM
%
%   Change Log
%       12/17/2011 - John Gideon - Created Function
%       01/05/2012 - John Gideon - Added support for color

    minDir = min(min(inPolarPC(:,:,2)));
    maxDir = max(max(inPolarPC(:,:,2)));
    factor = 40/(maxDir - minDir);
    maxDistList = zeros(40); %Make more general
    
    for i = 1:size(inPolarPC,1)
        for j = 1:size(inPolarPC,2)
            if (~isnan(inPolarPC(i,j,2)))
                dirOn = floor((inPolarPC(i,j,2)-minDir).*factor)+1;
                if (inPolarPC(i,j,3) > maxDistList(dirOn))
                    maxDistList(dirOn) = inPolarPC(i,j,3);
                end
            end
        end
    end
            
    outPolarPC = NaN(size(inPolarPC,1), size(inPolarPC,2), size(inPolarPC,3));
    for i = 1:size(inPolarPC,1)
        for j = 1:size(inPolarPC,2)
            if (~isnan(inPolarPC(i,j,2)))
                dirOn = floor((inPolarPC(i,j,2)-minDir).*factor)+1;
                if (inPolarPC(i,j,3) > maxDistList(dirOn)-20)
                    outPolarPC(i,j,:) = inPolarPC(i,j,:);
                end
            end
        end
    end
    
end

