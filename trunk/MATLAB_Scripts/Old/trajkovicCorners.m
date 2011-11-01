function [ outMtrx ] = trajkovicCorners( inMtrx, thresh )

outMtrx = double(zeros(size(inMtrx,1)-2,size(inMtrx,2)-2));
for i = 2:size(inMtrx,1)-1
    for j = 2:size(inMtrx,2)-1 
        C  = inMtrx(i,j);
        A  = inMtrx(i,j+1);
        Ap = inMtrx(i,j-1);
        B  = inMtrx(i-1,j+1);
        Bp = inMtrx(i+1,j-1);
        D  = inMtrx(i-1,j);
        Dp = inMtrx(i+1,j);
        E  = inMtrx(i-1,j-1);
        Ep = inMtrx(i+1,j+1);
        rA = ((A-C)^2)+((Ap-C)^2);
        rB = ((B-C)^2)+((Bp-C)^2);
        rD = ((D-C)^2)+((Dp-C)^2);
        rE = ((E-C)^2)+((Ep-C)^2);
        Out = min([rA rB rD rE]);
        if Out >= thresh
            outMtrx(i-1,j-1) = Out;
        else
            outMtrx(i-1,j-1) = 0;
        end
    end
end

end

