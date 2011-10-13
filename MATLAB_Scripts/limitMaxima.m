function [ detectedCornerC detectedCornerI detectedCornerJ ] = limitMaxima( inMtrx, maxCorners )

corner_peaks = imregionalmax(inMtrx);

if (maxCorners == -1)
    detectedCornerC = zeros(2, 1);
    detectedCornerI = zeros(2, 1);
    detectedCornerJ = zeros(2, 1);
    minIndex = 1;
    for i = 1:size(inMtrx,1)
        for j = 1:size(inMtrx,2)
            if corner_peaks(i,j) == 1
                C = inMtrx(i,j);
                detectedCornerC(minIndex) = C;
                detectedCornerI(minIndex) = i;
                detectedCornerJ(minIndex) = j;
                minIndex = minIndex + 1;
            end
        end
    end
else
    detectedCornerC = zeros(maxCorners, 1);
    detectedCornerI = zeros(maxCorners, 1);
    detectedCornerJ = zeros(maxCorners, 1);
    for i = 1:size(inMtrx,1)
        for j = 1:size(inMtrx,2)
            if corner_peaks(i,j) == 1
                C = inMtrx(i,j);
                [M minIndex] = min(detectedCornerC);
                if (detectedCornerC(minIndex) < C) 
                    detectedCornerC(minIndex) = C;
                    detectedCornerI(minIndex) = i;
                    detectedCornerJ(minIndex) = j;
                end
            end
        end
    end
end

end

