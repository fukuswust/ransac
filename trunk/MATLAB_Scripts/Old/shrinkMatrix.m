function [ outMtrx ] = shrinkMatrix( inMtrx, ratio )

outMtrx = zeros(floor(size(inMtrx,1)/ratio),floor(size(inMtrx,2)/ratio));
for i = 1:(floor(size(inMtrx,1)/ratio))
    for j = 1:(floor(size(inMtrx,2)/ratio))
        inI = (((i-1)*ratio)+1):(i*ratio);
        inJ = (((j-1)*ratio)+1):(j*ratio);
        outMtrx(i,j) = mean(mean(inMtrx(inI,inJ)));
    end
end

end

