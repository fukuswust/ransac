function [ slice sliceSource ] = extractHeightSlice( inPolarPC, height, ...
    outSize, thresh )
%EXTRACTHEIGHTSLICE Get 2D XZ matrix from height
%   Input
%       inPolarPC - 3D point cloud with polar coordinates
%          (height, direction, distance)
%       height - Height to extract slice from point cloud
%       outSize - Resolution of output slice
%       thresh - Threshold of minimum allowable difference between height
%   Output
%       slice - 1D matrix of size outSize of cartesian coordinates at
%           height (X,Z)
%       sliceSource - Input overlayed with NaNs where slice taken from
%   Description
%       What does this function do?
%
%   Change Log
%       01/04/2012 - John Gideon - Created Shell Function
%       01/05/2012 - John Gideon - Added threshold, color, and source

    minHeight = min(min(inPolarPC(:,:,1)));
    height = height + minHeight;
    minDir = min(min(inPolarPC(:,:,2)));
    maxDir = max(max(inPolarPC(:,:,2)));
    maxDir = maxDir + 0.000001;
    factor = outSize/(maxDir - minDir);
    slice = NaN(outSize,size(inPolarPC,3)-1);
    heightDiffList = inf(outSize,1);
    sliceLoc = NaN(outSize,2);
    
    minDis = min(min(inPolarPC(:,:,3)));
    
    for i = 1:size(inPolarPC,1)
        for j = 1:size(inPolarPC,2)
            if (~isnan(inPolarPC(i,j,2)))
                dirOn = floor((inPolarPC(i,j,2)-minDir).*factor)+1;
                heightDiff = abs(inPolarPC(i,j,1)-height);
                if (heightDiff < heightDiffList(dirOn) && ...
                        heightDiff < thresh)
                    heightDiffList(dirOn) = heightDiff;
                    dir = inPolarPC(i,j,2);
                    dis = inPolarPC(i,j,3);
                    slice(dirOn,1) = dis*cos(dir);
                    slice(dirOn,2) = dis*sin(dir);
                    if size(inPolarPC,3) == 4 
                        slice(dirOn,3) = inPolarPC(i,j,4);
                    end
                    sliceLoc(dirOn,1) = i;
                    sliceLoc(dirOn,2) = j;
                end
            else
                inPolarPC(i,j,3) = minDis;
            end
        end
    end
    
    sliceSource = inPolarPC;
    for i = 1:outSize
        if (~isnan(sliceLoc(i,1)))
            sliceSource(sliceLoc(i,1),sliceLoc(i,2),1) = NaN;
            sliceSource(sliceLoc(i,1),sliceLoc(i,2),2) = NaN;
            sliceSource(sliceLoc(i,1),sliceLoc(i,2),3) = NaN;
        end
    end    
end

