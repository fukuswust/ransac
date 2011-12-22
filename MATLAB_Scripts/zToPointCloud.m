function [ pointCloud ] = zToPointCloud( zArray )
%ZTOPOINTCLOUD Convert array of depths to point cloud
%   Input
%       zArray - Array with z coordinates with resolution of 1mm
%   Output
%       pointCloud - Local created point cloud for the raw data consisting
%       of a 3D array of coordinates
%   Description
%       Converts and array of z value at i,j locations into an x,y,z point
%       cloud. This is approximated by the following equations:
%           x = ((i - cx_d) * z) / -fx_d
%           y = ((j - cy_d) * z) / fy_d
%           z = -z
%       NOTE: The above equations assumed a height of 480 and a width of
%       640. This has been made more general for algorithm development.
%
%   Change Log
%       11/02/2011 - John Gideon - Created Shell Script
%       11/09/2011 - John Gideon - Initial equation input and tested
%       12/16/2011 - John Gideon - Clean up and y=-y z=-z
%       12/19/2011 - John Gideon - Added description

    fx_d = 5.9421434211923247e+02;
	fy_d = 5.9104053696870778e+02;
	cx_d = 3.3930780975300314e+02;
	cy_d = 2.4273913761751615e+02;
    
    MinDistance = -10;
	DepthScaleFactor = .0021;
    
    factor = 640/size(zArray,2);
    
    pointCloud(:,:,:) = zeros(size(zArray,1),size(zArray,2),3);
    for j = 1:size(zArray,1)
        for i = 1:size(zArray,2)
            z = zArray(j,i);
            %x = ((  ((i-1)*factor) - cx_d) * z) / (-fx_d);
            %y = ((  ((j-1)*factor) - cy_d) * z) / fy_d;   
            
		 x = ( ((i-1)*factor) - 320) * (z + MinDistance) * -DepthScaleFactor ;
		 y = ( ((j-1)*factor) - 240) * (z + MinDistance) * DepthScaleFactor ;
            
            pointCloud(j,i,:) = [x y z];
        end
    end

end