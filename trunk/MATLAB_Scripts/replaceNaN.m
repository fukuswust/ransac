function retImage = replaceNaN( inImage,repValue )
    retImage = zeros(size(inImage,1),size(inImage,2));
    for i = 1:size(inImage,1)
        for j = 1:size(inImage,2)
            if isnan(inImage(i,j))
                retImage(i,j) = repValue;
            else
                retImage(i,j) = inImage(i,j);
            end
        end
    end
end

