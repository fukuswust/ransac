function [ outMtrx ] = shrinkNaNMatrix( inMtrx, ratio )

outMtrx = double(zeros(floor(size(inMtrx,1)/ratio),floor(size(inMtrx,2)/ratio)));
for i = 1:(floor(size(inMtrx,1)/ratio))
    for j = 1:(floor(size(inMtrx,2)/ratio))
        count = 0;
        total = 0;
        for inI = (((i-1)*ratio)+1):(i*ratio);
            for inJ = (((j-1)*ratio)+1):(j*ratio);
                tmpOn = inMtrx(inI,inJ);
                if (~isnan(tmpOn))
                    total = total + tmpOn;
                    count = count + 1;
                end
            end
            if (count >= (ratio^2)/2) 
                outMtrx(i,j) = total/count;
            else
                outMtrx(i,j) = NaN;
            end
        end
    end
end

end

