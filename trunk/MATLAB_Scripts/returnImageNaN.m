function [ outImage ] = returnImageNaN( inArray )

I = normalizeImage(inArray);
[r g b] = deal(I);
for i = 1:size(inArray,1)
    for j = 1:size(inArray,2)
        if (isnan(inArray(i,j)))
            r(i,j) = 0;
            g(i,j) = 0;
            b(i,j) = 255;
        end
    end
end
outImage = cat(3,r,g,b);

end

