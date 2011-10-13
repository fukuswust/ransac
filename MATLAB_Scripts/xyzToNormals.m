function [ outGradient ] = xyzToNormals( inXYZ, diff )

outGradient = zeros(size(inXYZ,1)-(2*diff),size(inXYZ,2)-(2*diff),3,3);
dir1 = zeros(3,1);
dir2 = zeros(3,1);

for i = diff+1:size(inXYZ,1)-diff
    for j = diff+1:size(inXYZ,2)-diff
        dir1(1) = inXYZ(i+diff,j,1) - inXYZ(i-diff,j,1);
        dir1(2) = inXYZ(i+diff,j,2) - inXYZ(i-diff,j,2);
        dir1(3) = inXYZ(i+diff,j,3) - inXYZ(i-diff,j,3);
        
        dir2(1) = inXYZ(i,j+diff,1) - inXYZ(i,j-diff,1);
        dir2(2) = inXYZ(i,j+diff,2) - inXYZ(i,j-diff,2);
        dir2(3) = inXYZ(i,j+diff,3) - inXYZ(i,j-diff,3);
        
        normal = cross(dir1, dir2);
        
        uNormal = normal./sqrt(sum([normal(1) normal(2) normal(3)].^2));
        
        outGradient(i-diff,j-diff,1) = uNormal(1);
        outGradient(i-diff,j-diff,2) = uNormal(2);
        outGradient(i-diff,j-diff,3) = uNormal(3);
    end
end

