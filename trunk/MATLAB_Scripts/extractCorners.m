function [detectedCornerR detectedCornerI detectedCornerJ] = extractCorners(inImage, maxCorners)
%% Corner
    inImage = double(inImage);
    cornerMag = double(zeros(size(inImage,1),size(inImage,2)));
    Dx = imageApplyDx(inImage);
    Dy = imageApplyDy(inImage);
    
%% Dx
    Dxx = int16(Dx) .* int16(Dx);
    Dyy = int16(Dy) .* int16(Dy);
    Dxy = int16(Dx) .* int16(Dy);
    
    gDxx = gaussianBlurImage(Dxx);
    gDyy = gaussianBlurImage(Dyy);
    gDxy = gaussianBlurImage(Dxy);
  
%% Get Corner Magnitudes
    for i = 4:size(inImage,1)-3
        for j = 4:size(inImage,2)-3            
            C = double([gDxx(i,j) gDxy(i,j)
                        gDxy(i,j) gDyy(i,j)]);
            R = det(C) - (.05*(trace(C)^2));
            if R > 50000
                cornerMag(i,j) = R;
            else 
                cornerMag(i,j) = 0;
            end                
        end
    end
    
    imshow(normalizeImage(cornerMag))
    
%% Narrow to Regional Maximums
detectedCornerR = zeros(maxCorners,1);
detectedCornerI = zeros(maxCorners,1);
detectedCornerJ = zeros(maxCorners,1);
corner_peaks = imregionalmax(cornerMag);

%% Determine top <maxCorners>
for i = 4:size(inImage,1)-3
    for j = 4:size(inImage,2)-3  
        if corner_peaks(i,j) == 1
            R = cornerMag(i,j);
            [M minIndex] = min(detectedCornerR);
            if (detectedCornerR(minIndex) < R)
                detectedCornerR(minIndex) = R;
                detectedCornerI(minIndex) = i;
                detectedCornerJ(minIndex) = j;
            end
        end
    end
end

end

