function [ ] = featurePlot(inImage, detectedCornerI, detectedCornerJ)
    I = normalizeImage(inImage);
    [r g b] = deal(I);
    for indexOn = 1:size(detectedCornerI,1)
        I = detectedCornerI(indexOn);
        J = detectedCornerJ(indexOn);
        r(I-1:I+1,J-1:J+1) = 255;
        g(I-1:I+1,J-1:J+1) = 0;
        b(I-1:I+1,J-1:J+1) = 0;
    end
    RGB = cat(3,r,g,b);
    imshow(RGB);
end

