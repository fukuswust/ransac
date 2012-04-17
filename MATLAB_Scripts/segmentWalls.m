function [ outPolarPC outScan ] = segmentWalls( inPolarPC, maxDistList )
%SEGMENTWALLS Finds walls in a point cloud
%   Input
%       inPolarPC - Input 2D array of 3D points (polar)
%       maxDistList - Input list of max distances for each direction
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
    factor = 40/(maxDir - minDir + 0.00001);
    minHeight = min(min(inPolarPC(:,:,1)));
            
    outPolarPC = NaN(size(inPolarPC,1), size(inPolarPC,2), size(inPolarPC,3));
    histAccl = zeros(40,20);
    for i = 1:size(inPolarPC,1)
        for j = 1:size(inPolarPC,2)
            if (~isnan(inPolarPC(i,j,2)))
                if (inPolarPC(i,j,1) > minHeight+100)
                    dirOn = floor((inPolarPC(i,j,2)-minDir).*factor)+1;
                    diff = floor(maxDistList(dirOn,2)-inPolarPC(i,j,3))+1;
                    if (diff < 20)
                        outPolarPC(i,j,:) = inPolarPC(i,j,:);
                        histAccl(dirOn,diff) = histAccl(dirOn,diff) + 1;
                    end
                end
            end
        end
    end
    
    %% Find Histogram Peaks and average around
    outScan = nan(40,2);
    for dirOn = 1:40 
        [maxV maxI] = max(histAccl(dirOn,:));
        if (maxV > 2)
            outScan(dirOn,1) = ((dirOn-1)./factor)+minDir;
            outScan(dirOn,2) = maxDistList(dirOn,2)+maxI-1;
        end
    end
    
end

