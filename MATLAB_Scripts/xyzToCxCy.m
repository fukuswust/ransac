function [ CxCyValues ] = xyzToCxCy( xyzValues )
    R = [9.9984628826577793e-01 -1.4779096108364480e-03 1.7470421412464927e-02
         1.2635359098409581e-03 9.9992385683542895e-01 1.2275341476520762e-02
         -1.7487233004436643e-02 -1.2251380107679535e-02 9.9977202419716948e-01];
    T = [1.9985242312092553e-02; -7.4423738761617583e-04; -1.0916736334336222e-02];
	fx_rgb = 5.2921508098293293e+02;
	fy_rgb = 5.2556393630057437e+02;
	cx_rgb = 3.2894272028759258e+02;
	cy_rgb = 2.6748068171871557e+02;

    CxCyValues = zeros(size(xyzValues,1),size(xyzValues,2));
    for i = 1:size(xyzValues,1)
        for j = 1:size(xyzValues,2)
            tmpOn = [xyzValues(i,j,1); xyzValues(i,j,2); xyzValues(i,j,3)];
            tmpOnP = (R*tmpOn) + T;
			CxCyValues(i,j,1) = (tmpOnP(1) * fx_rgb / tmpOnP(3)) + cx_rgb;
			CxCyValues(i,j,2) = (tmpOnP(2) * fy_rgb / tmpOnP(3)) + cy_rgb;
        end
    end
end

