function [ planeParameters ] = ransacSegmentation( inPointCloud )
%FUNCTIONTEMPLATE Summary of this function goes here
%   Input
%       inPointCloud - Input point cloud containing 3 planes (x, y, z)
%   Output
%       output_args - Explanation
%   Description
%       What does this function do?
%
%   Change Log
%       11/01/2011 - John Gideon - Created Shell Template
%       1/10/2012  - Ben Zerhusen - Added code for RANSAC wrapper script

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% set RANSAC options
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
options.epsilon = 1e-6;
options.P_inlier = 0.99;
options.sigma = 0.20;
options.est_fun = @estimate_plane;
options.man_fun = @error_plane;
options.mode = 'MSAC';
options.Ps = [];
options.notify_iters = [];
options.min_iters = 5;
options.fix_seed = false;
options.reestimate = true;
options.stabilize = false;

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Format to necessary data array dimensions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
k = 1;
for i=1:size(inPointCloud,1)
    for j=1:size(inPointCloud,2)
        if (~isnan(inPointCloud(i,j,3)))
            ransacPointCloud(1,k) = inPointCloud(i,j,1);
            ransacPointCloud(2,k) = inPointCloud(i,j,2);
            ransacPointCloud(3,k) = inPointCloud(i,j,3);
            k = k + 1;
        end
    end
end

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% RANSAC
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% run RANSAC
[results, options] = RANSAC(ransacPointCloud, options);

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Plane function creation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% syms x y z;
% planefunction = results.Theta(1)*x + results.Theta(2)*y + results.Theta(3)*z + results.Theta(4)
% zplane = solve(planefunction, z);

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Results Visualization
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% figure;
% hold on
% ind = results.CS;
% plot3(ransacPointCloud(1, ind), ransacPointCloud(2, ind), ransacPointCloud(3, ind), 'g+')
% plot3(ransacPointCloud(1, ~ind), ransacPointCloud(2, ~ind), ransacPointCloud(3, ~ind), 'r+')
% ezmesh(zplane, [-200, 100, -105, 125])
% xlabel('x')
% ylabel('y')
% zlabel('z')
% title('RANSAC results for 3D plane estimation')
% %legend('Estimated Iniliers', 'Estimated Outliers')
% axis equal tight
% view(3)

planeParameters = results.Theta;

    %take in the x,y,z point cloud data
    %return the normal for the floor plane and the midpoint of the data
    
    
    
end

