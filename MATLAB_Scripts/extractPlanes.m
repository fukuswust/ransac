function [ planes ] = extractPlanes( segments )
%EXTRACTPLANES Determined planes from the segmented point cloud
%   Input
%       segments - List of point cloud segments represented as 4D array
%   Output
%       planes - List of plane structures
%   Description
%       What does this function do?
%
%   Change Log
%       11/16/2011 - John Gideon - Created Shell Script

    planes(size(segments,4)).normal = [0 7 5];
    planes(size(segments,4)).num = 5;
    for segOn = 1:size(segments,4);
        planes(segOn).normal = [segOn 1 2];
        planes(segOn).num = segOn;
    end

end

