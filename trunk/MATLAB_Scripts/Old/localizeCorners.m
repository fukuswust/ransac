function [ cornerMaxesC cornerMaxesI cornerMaxesJ ] = localizeCorners( inMtrx, cornerPeaksC, cornerPeaksI, cornerPeaksJ )

cornerMaxesC = zeros(2,1);
cornerMaxesI = zeros(2,1);
cornerMaxesJ = zeros(2,1);
outOn = 1;
for index = 1:size(cornerPeaksC,1)
    I = ((cornerPeaksI(index)-1)*4);
    J = ((cornerPeaksJ(index)-1)*4);
    
    if ((I-5 >= 1) && (I+10 <= size(inMtrx,1)) && (J-5 >= 1) && (J+10 <= size(inMtrx,2)))
        subMtrx = inMtrx(I-5:I+10,J-5:J+10);
        [cornerMaxC cornerMaxI cornerMaxJ] = localizeCorner(subMtrx);
        if cornerMaxC > 0
            cornerMaxesC(outOn) = cornerMaxC;
            cornerMaxesI(outOn) = I-4+cornerMaxI;
            cornerMaxesJ(outOn) = J-4+cornerMaxJ;
            outOn = outOn + 1;
        end
    end
end

end

