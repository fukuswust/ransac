function [ maxDistList ] = getMaxDisPerDir( inPolarPC, numOut )
%GETMAXDISPERDIR Determines the maximum distance for each direction
%   Input
%       inPolarPC - Input 2D array of 3D points (polar)
%       numOut - Desired number of output points 
%   Output
%       maxDistList - Output list of max distances for each direction
%   Description
%       ABC
%
%   Change Log
%       01/11/2012 - John Gideon - Created Function

    minDir = min(min(inPolarPC(:,:,2)));
    maxDir = max(max(inPolarPC(:,:,2)));
    minHeight = min(min(inPolarPC(:,:,1)));
    factor = numOut/(maxDir - minDir + 0.00001);
    maxDistList = NaN(numOut,2); %Make more general
    
    for i = 1:size(inPolarPC,1)
        for j = 1:size(inPolarPC,2)
            if (~isnan(inPolarPC(i,j,2)))
                if (inPolarPC(i,j,1) > minHeight+100)
                    dirOn = floor((inPolarPC(i,j,2)-minDir).*factor)+1;
                    if (isnan(maxDistList(dirOn,1)))
                        maxDistList(dirOn,1) = inPolarPC(i,j,2);
                        maxDistList(dirOn,2) = inPolarPC(i,j,3);
                    elseif (inPolarPC(i,j,3) > maxDistList(dirOn,2))
                        maxDistList(dirOn,1) = inPolarPC(i,j,2);
                        maxDistList(dirOn,2) = inPolarPC(i,j,3);
                    end
                end
            end
        end
    end

end

