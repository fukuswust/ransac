function [ cornerMaxC cornerMaxI cornerMaxJ ] = localizeCorner( inMtrx )
    
    inMtrx = double(inMtrx);
    Dx = imageApplyDx(inMtrx);
    Dy = imageApplyDy(inMtrx);
    
    Dxx = int16(Dx) .* int16(Dx);
    Dyy = int16(Dy) .* int16(Dy);
    Dxy = int16(Dx) .* int16(Dy);
    
    gDxx = gaussianBlurImage(Dxx);
    gDyy = gaussianBlurImage(Dyy);
    gDxy = gaussianBlurImage(Dxy);

    cornerMaxC = 0;
    cornerMag = zeros(10);
    for i = 2:size(gDxx,1)-1
        for j = 2:size(gDxx,2)-1           
            C = double([gDxx(i,j) gDxy(i,j)
                        gDxy(i,j) gDyy(i,j)]);
            cornerMag(i,j) = det(C) - (.05*(trace(C)^2));
            if (cornerMag(i,j) > cornerMaxC) 
                cornerMaxC = cornerMag(i,j);
                cornerMaxI = i;
                cornerMaxJ = j;
            end
        end
    end
    
    dxx = Dx(cornerMaxI+1,cornerMaxJ+1);
    dyy = Dy(cornerMaxI+1,cornerMaxJ+1);
    
    m1 = sqrt((double(dyy)^2)+(double(dxx)^2));
    dirX1 = ((dxx*3)/m1);
    dirY1 = ((dyy*3)/m1);
    aX1 = round(max([-3 min([3 dirX1])]));
    aY1 = round(max([-3 min([3 dirY1])]));
    aX3 = -aX1;
    aY3 = -aY1;
    aX2 = -aY1;
    aY2 = aX1;
    aX4 = -aX2;
    aY4 = -aY2;   
    
    cornerMaxI = cornerMaxI+2;
    cornerMaxJ = cornerMaxJ+2;
    
    cm = inMtrx(cornerMaxI,cornerMaxJ);
    sm = abs(inMtrx(cornerMaxI+aX1,cornerMaxJ+aY1)-inMtrx(cornerMaxI+aX3,cornerMaxJ+aY3));
    dm = abs(inMtrx(cornerMaxI+aX2,cornerMaxJ+aY2)-inMtrx(cornerMaxI+aX4,cornerMaxJ+aY4));
    
    scDiff1 = abs(inMtrx(cornerMaxI+aX1,cornerMaxJ+aY1) - cm);
    scDiff2 = abs(inMtrx(cornerMaxI+aX3,cornerMaxJ+aY3) - cm);
    scDiff = max([scDiff1 scDiff2]);
    
    if (sm > 30)
        cornerMaxC = 0;
    else
        if (scDiff < 20)
            cornerMaxC = 0;
        else
            if (dm < 40)
                cornerMaxC = 0;
            end
        end
    end
    
%     I = normalizeImage(inMtrx);
%     [r g b] = deal(I);
%     r(cornerMaxI,cornerMaxJ) = 255;
%     g(cornerMaxI,cornerMaxJ) = 0;
%     b(cornerMaxI,cornerMaxJ) = 0;
%     r(cornerMaxI+aX1,cornerMaxJ+aY1) = 0;
%     g(cornerMaxI+aX1,cornerMaxJ+aY1) = 0;
%     b(cornerMaxI+aX1,cornerMaxJ+aY1) = 255;
%     r(cornerMaxI+aX2,cornerMaxJ+aY2) = 0;
%     g(cornerMaxI+aX2,cornerMaxJ+aY2) = 255;
%     b(cornerMaxI+aX2,cornerMaxJ+aY2) = 0;    
%     r(cornerMaxI+aX3,cornerMaxJ+aY3) = 0;
%     g(cornerMaxI+aX3,cornerMaxJ+aY3) = 255;
%     b(cornerMaxI+aX3,cornerMaxJ+aY3) = 0;
%     r(cornerMaxI+aX4,cornerMaxJ+aY4) = 0;
%     g(cornerMaxI+aX4,cornerMaxJ+aY4) = 255;
%     b(cornerMaxI+aX4,cornerMaxJ+aY4) = 0;
%     RGB = cat(3,r,g,b);
%     imshow(RGB);
end

