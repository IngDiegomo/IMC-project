function rgbImage = readResizedImg(fileName,imgSize)
%#codegen
% Copyright 2020 The MathWorks, Inc.

assert(all(size(fileName) <= 50));
assert(isa(fileName,'char'));
assert(all(size(imgSize) == [1,3]));

if coder.target('Rtw')
    %Initialize output image
    rgbImage = zeros(imgSize,'uint8');
    
    %Declare variables
    coder.cinclude('<opencv2/opencv.hpp>');
    imgOpenCV     = coder.opaque('cv::Mat'); %#ok<*NASGU>
    tmpImgOpenCV  = coder.opaque('cv::Mat');
    imgSizeOpenCV = coder.opaque('cv::Size');
    imgFormat     = coder.opaque('int','CV_8UC3');
    
    %Read the image
    imgOpenCV     = coder.ceval('cv::imread',fileName,1);
    imgColSize    = coder.opaque('int','imgOpenCV.cols');
    imgRowSize    = coder.opaque('int','imgOpenCV.rows');
    if ((double(imgColSize) == 0) || (double(imgRowSize) == 0))
        outputStr = sprintf('Error while reading the input file %s. \nTry a different input image. \n',fileName);
        fprintf('%s\n',outputStr);
        coder.ceval('exit',0);
    end
    imgSizeOpenCV = coder.ceval('cv::Size',imgSize(1),imgSize(2));
    tmpImgOpenCV  = coder.ceval('cv::Mat',imgSizeOpenCV,imgFormat);
    
    %Resize the image
    coder.ceval('cv::resize',imgOpenCV,tmpImgOpenCV,imgSizeOpenCV);
    
    %Convert BGR to RGB (OpenCV to MATLAB format)
    pixValueB = coder.opaque('uint8_t');
    pixValueG = coder.opaque('uint8_t');
    pixValueR = coder.opaque('uint8_t');
    pixPtr    = coder.opaque('uint8_t*');
    for i = 1:imgSize(1)
        for j = 1:imgSize(2)
            pixPtr = coder.ceval('tmpImgOpenCV.ptr',i-1,j-1);
            
            pixValueB = coder.ceval('(uint8_t)*',pixPtr);
            coder.ceval('++',pixPtr);
            pixValueG = coder.ceval('(uint8_t)*',pixPtr);
            coder.ceval('++',pixPtr);
            pixValueR = coder.ceval('(uint8_t)*',pixPtr);
            
            rgbImage(i,j,1) = cast(pixValueR,'uint8');
            rgbImage(i,j,2) = cast(pixValueG,'uint8');
            rgbImage(i,j,3) = cast(pixValueB,'uint8');
        end
    end
    opencv_linkflags = '`pkg-config --cflags --libs opencv`';
    coder.updateBuildInfo('addLinkFlags',opencv_linkflags);
    
else
    img = imread(fileName);
    rgbImage = imresize(img,imgSize(1:2));
end
end