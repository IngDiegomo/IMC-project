clc
clear
%% Load data
load('CNNWorkspace')
%% Set up 
inputSize = netTransfer.Layers(1).InputSize(1:2);
classes = netTransfer.Layers(end).Classes;
imgN = imread("pruebaN.jpg");
imgN = imresize(imgN,inputSize);
imgP = imread("pruebaP.jpg");
imgP = imresize(imgP,inputSize);
imgK = imread("pruebaK.jpg");
imgK = imresize(imgK,inputSize);
imgSana = imread("pruebaSana.jpg");
imgSana = imresize(imgSana,inputSize);
%% Classify
[YPredN,scoresN] = classify(netTransfer,imgN);
[~,topIdx] = maxk(scoresN, 3);
topScoresN = scoresN(topIdx);
topClassesN = classes(topIdx);

[YPredP,scoresP] = classify(netTransfer,imgP);
[~,topIdx] = maxk(scoresP, 3);
topScoresP = scoresP(topIdx);
topClassesP = classes(topIdx);

[YPredK,scoresK] = classify(netTransfer,imgK);
[~,topIdx] = maxk(scoresK, 3);
topScoresK = scoresK(topIdx);
topClassesK = classes(topIdx);


[YPredSana,scoresSana] = classify(netTransfer,imgSana);
[~,topIdx] = maxk(scoresSana, 3);
topScoresSana = scoresSana(topIdx);
topClassesSana = classes(topIdx);
%% Lime N
mapN = imageLIME(netTransfer,imgN,YPredN);

figure
imshow(imgN,'InitialMagnification',150)
hold on
imagesc(mapN,'AlphaData',0.5)
colormap jet
colorbar

title(sprintf("Image LIME (%s)", ...
    YPredN))
hold off



%% Lime P



%% Lime K



%% Lime Sana



