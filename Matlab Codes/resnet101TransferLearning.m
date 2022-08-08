clc
clear
%% Load and Separate Data
imds = imageDatastore('Set Reducido Comprimido', ...
    'IncludeSubfolders',true, ...
    'LabelSource','foldernames');
[imdsTrain,imdsTest,imdsValidation] = splitEachLabel(imds,0.7,0.15,0.15,'randomized');

%% Display some images
numTrainImages = numel(imdsTrain.Labels);
idx = randperm(numTrainImages,16);
figure
for i = 1:16
    subplot(4,4,i)
    I = readimage(imdsTrain,idx(i));
    imshow(I)
end
%% Load net
numClasses = numel(categories(imdsTrain.Labels));
net = resnet101;
inputSize = net.Layers(1).InputSize;
%% Replace layers
% Convert DAGNetwork object to LayerGraph object
lgraph = layerGraph(net);
% Replace the last few layers
[learnableLayer,classLayer] = findLayersToReplace(lgraph);
if isa(learnableLayer, 'nnet.cnn.layer.FullyConnectedLayer' )
    newLearnableLayer = fullyConnectedLayer(numClasses, ... 
        'Name' , 'new_fc' , ... 
        'WeightLearnRateFactor' ,20, ... 
        'BiasLearnRateFactor' ,20);
    
elseif isa(learnableLayer, 'nnet.cnn.layer.Convolution2DLayer' )
    newLearnableLayer = convolution2dLayer(1,numClasses, ... 
        'Name' , 'new_conv' , ... 
        'WeightLearnRateFactor' ,20, ... 
        'BiasLearnRateFactor' ,20);
end

lgraph = replaceLayer(lgraph,learnableLayer.Name,newLearnableLayer);
        
newClassLayer = classificationLayer( 'Name' , 'new_classoutput' );
lgraph = replaceLayer(lgraph,classLayer.Name,newClassLayer);
        
% Freeze first layers
layers = lgraph.Layers;
connections = lgraph.Connections;
layers(1:10) = freezeWeights(layers(1:10));
% Re-connect all the layers in the original order 
% by using the support function createLgraphUsingConnections
lgraph = createLgraphUsingConnections(layers,connections);
%% Data augmentation
pixelRange = [-30 30];
scaleRange = [0.9 1.1];
imageAugmenter = imageDataAugmenter( ...
    'RandXReflection',true, ...
    'RandXTranslation',pixelRange, ...
    'RandYTranslation',pixelRange, ...
    'RandXScale',scaleRange, ...
    'RandYScale',scaleRange);
augimdsTrain = augmentedImageDatastore(inputSize(1:2),imdsTrain, ...
    'DataAugmentation',imageAugmenter);
augimdsValidation = augmentedImageDatastore(inputSize(1:2),...
    imdsValidation);
%% Training options
miniBatchSize = 9;
valFrequency = floor(numel(augimdsTrain.Files)/miniBatchSize);
options = trainingOptions('sgdm', ...
    'MiniBatchSize',miniBatchSize, ...
    'MaxEpochs',30, ...
    'InitialLearnRate',1e-4, ...
    'Shuffle','every-epoch', ...
    'ValidationData',augimdsValidation, ...
    'ValidationFrequency',valFrequency, ...
    'Verbose',false, ...
    'Plots','training-progress');
%% Train network 
netTransfer = trainNetwork(augimdsTrain,lgraph,options);

%% Evaluation
[YPred,probs] = classify(netTransfer,augimdsValidation);
valAccuracy = mean(YPred == imdsValidation.Labels)

valError = 1 - mean(YPred == imdsValidation.Labels)

%% Test
augimdsTest = augmentedImageDatastore(inputSize(1:2),...
    imdsTest);
[YPred,probs] = classify(netTransfer,augimdsTest);
testAccuracy = mean(YPred == imdsTest.Labels)

testError = 1 - mean(YPred == imdsTest.Labels)

NTest = numel(imdsTest.Labels);
testErrorSE = sqrt(testError*(1-testError)/NTest)
testError95CI = [testError - 1.96*testErrorSE, testError + 1.96*testErrorSE]

%% Confusion matrix
figure('Units','normalized','Position',[0.2 0.2 0.4 0.4]);
cm = confusionchart(imdsTest.Labels,YPred);
cm.Title = 'Confusion Matrix for Test Data';
cm.ColumnSummary = 'column-normalized';
cm.RowSummary = 'row-normalized';

%% Display predictions
idx = randperm(numel(imdsTest.Files),4);
figure
for i = 1:4
    subplot(2,2,i)
    I = readimage(imdsTest,idx(i));
    imshow(I)
    label = YPred(idx(i));
    realLabel = imdsTest.Labels(idx(i));
    title(string(label) + ", " + num2str(100*max(probs(idx(i),:)),3) + "%" +...
        ", "+"Real label:"+string(realLabel));
 end
%% Save workspace
save("CNNWorkspace")
