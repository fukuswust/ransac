function [ outPointCloud ] = addGreyscaleToPointCloud( inPointCloud, inColorData )
%ADDGREYSCALETOPOINTCLOUD 
% http://nicolas.burrus.name/index.php/Research/KinectCalibration
%   Input
%       inPointCloud - Input point cloud containing 3 planes (x, y, z)
%       rColorData - Input r color data.
%       gColorData - Input g color data.
%       bColorData - Input b color data.
%   Output
%       outPointCloud - Output point cloud containing 4 planes (x, y, z,
%       greyscale value).
%   Description
%       Adds a fourth plane to the point cloud that
%       includes greyscale value of that point - this value is determined by
%       taking some sort of average of the points in the area around the desired
%       point. This is performed for each color, and then the three colors are
%       combined (either average, or the sum).
%
%   Change Log
%       12/19/2011 - Tom Dickman - Created initial script
%       12/21/2011 - Tom Dickman - Script now adds 4th layer with greyscale
%       value (sum of the r, g, and b values).
    
    % Prealocate outPointCloud - copy contents from inPointCloud, and add a
    % layer
    outPointCloud = inPointCloud;
    outPointCloud(:,:,4) = zeros(size(inPointCloud,1), size(inPointCloud,2), 1);
    
    % Declare the constants we will need. These values are from:
    % http://nicolas.burrus.name/index.php/Research/KinectCalibration
    fx_rgb =  5.2921508098293293e+02;
    fy_rgb =  5.2556393630057437e+02;
    cx_rgb =  3.2894272028759258e+02;
    cy_rgb =  2.6748068171871557e+02;
    
    R = [ 9.9984628826577793e-01 1.2635359098409581e-03 ...
        -1.7487233004436643e-02; -1.4779096108364480e-03 ...
        9.9992385683542895e-01 -1.2251380107679535e-02; ...
        1.7470421412464927e-02 1.2275341476520762e-02 ...
        9.9977202419716948e-01 ];
    T = [ 1.9985242312092553e-02; ...
        -7.4423738761617583e-04; ...
        -1.0916736334336222e-02 ];
    
    % Determine x and y points on rgb matrices that correspond to each
    % point cloud point.
    %   Loop through inPointCloud
    for i=1:size(inPointCloud,1)
        for j=1:size(inPointCloud,2)
            if (~isnan(inPointCloud(i,j,3)))
                % Create P3D
                P3D = R*[inPointCloud(i,j,1); inPointCloud(i,j,2); inPointCloud(i,j,3)]+T;
                nx = P3D(1);
                ny = P3D(2);
                nz = P3D(3);

                % Determine i and j coordinates on color image for this point.
                fj = (nx * fx_rgb / nz) + cx_rgb;
                fi = (ny * fy_rgb / nz) + cy_rgb;

                % Combine r, g, and b values for this point (add for now)
                outPointCloud(i, j, 4) = (inColorData(floor(fi), floor(fj), 1) ...
                    + inColorData(floor(fi), floor(fj), 2) ...
                    + inColorData(floor(fi), floor(fj), 3));
            end
        end
    end
    