function [xyzArray] = depthToCartesian(depthArray)
%DEPTHTOCARTESIAN Summary of this function goes here
%   Detailed explanation goes here
    xyzArray = zeros(size(depthArray,1),size(depthArray,2),3);
    fx_d = 5.9421434211923247e+02;
	fy_d = 5.9104053696870778e+02;
	cx_d = 3.3930780975300314e+02;
	cy_d = 2.4273913761751615e+02;
    for i = 1:size(depthArray,1)
        for j = 1:size(depthArray,2)
            z = 1.0 / ((depthArray(i,j) * -0.0030711016) + 3.3309495161);
            xyzArray(i,j,1) = ((j - cx_d) * z) / fx_d;
            xyzArray(i,j,2) = ((i - cy_d) * z) / fy_d;
            xyzArray(i,j,3) = z;
        end
    end
end

