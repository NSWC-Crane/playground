
clear
clc
% ZOOM 2000
filename600 = "C:\Data\JSSAP\sharpest\z2000\0600\image_z01998_f46229_e14987_i00.png";
filename700 = "C:\Data\JSSAP\sharpest\z2000\0700\image_z01998_f46270_e06064_i00.png";
filename800 = "C:\Data\JSSAP\sharpest\z2000\0800\image_z01999_f46270_e07221_i00.png";
filenamez35r600 ="C:\Data\JSSAP\z3500\600\image_z03497_f47153_e15004_i00.png";
filenamez2000r850 = "C:\Data\JSSAP\z2000\850\image_z01999_f46112_e05089_i01.png";

img600 = double(rgb2gray(imread(filename600)));
img700 = double(rgb2gray(imread(filename700)));
img800 = double(rgb2gray(imread(filename800)));
img6z35 = double(rgb2gray(imread(filenamez35r600)));
imgz2r850 = double(rgb2gray(imread(filenamez2000r850)));
img = imgz2r850;

% max change over 5 pixels
[img_h,imh_w] = size(img);
testM = zeros(imh_w,1);
intV = 5;
maxgrad = 0;

% Test each column for large gradient over intv
% Find it with highest pixel value (light color) first
% img_vline(rw,1) will be darker color (lower pixel value like 60)
% img_vline(rw+intv,1) will be lighter color (higher pixel value like 200)
for col = 1:imh_w 
    img_vline = img(:,col);
    for rw = 1:img_h-intV
        grad = img_vline(rw+intV,1)-img_vline(rw,1);
        if grad > maxgrad
            maxgrad = grad;
            maxgRw = rw;
            maxgCol = col;
            img_maxgCol = img(:,maxgCol); % maxRow will be darker (low pix val)
        end
    end
end

figure(16)
x=1:length(img_maxgCol);
plot(x,img_maxgCol);
xlabel("Pixel Number")
ylabel("Pixel Value")
grid on
ylim([0,255])
xlim([1,img_h])
title("Range 850 Zoom 2000")
xticks(1:5:img_h)

% Find blur in this column near maxgrad - max value before and after, 
% min value 
% After img_vline(rw,1), find max pixel value within 12 pixels after maxRw

if maxgRw+12 < img_h
    [mx,iMx] = max(img_maxgCol(maxgRw:maxgRw+12,1)); % Find max color and row
else
    [mx,iMx] = max(img_maxgCol(maxgRw:end,1));
end
% Row location in img_maxCol where max value of mx resides
mxRw = maxgRw + iMx -1;

% Find row for Min before and after 
srt = maxgRw-6;
[mn, iMn] = min(img_maxgCol(srt:srt+12,1));
mnRw = srt + iMn -1;
% Count blurred pixels between mxRw and mnRw in img_maxgCol
% Use an allowance to the pixel value: min+allow, max - allow
% Find this allowance between img_maxgCol(mnRw:mxRw)
allow = 5;
indMn = find(img_maxgCol(mnRw:mxRw) > mn + allow,1,"first");
indMx = find(img_maxgCol(mnRw:mxRw) < mx - allow,1,"last");
blurPix = indMx - indMn + 1;

