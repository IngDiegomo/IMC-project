function raspiWebcamP2()
%#codegen
% Create raspi & webcam obj
raspiObj = raspi();
cam      = webcam(raspiObj,1);



% Initialize CNN and the input size
net        = coder.loadDeepLearningNetwork('netP2.mat');
inputSize  = [224, 224, 3];          %net.Layers(1).InputSize;

system(raspiObj, 'raspistill -o /home/pi/P2/imgSnapshots/resultado.jpg','sudo');

fprintf('Taking picture.\n');

% Take picture, resize and classify
img = snapshot(cam);
imgSizeAdjusted = imresize(img,inputSize(1:2));
[label,score] = net.classify(imgSizeAdjusted);
maxScore = max(score);
labelStr = cellstr(label);

% Texts to save
textToSave= sprintf('Label : %s \nScore : %f \nOther scores:\n%f \n%f \n%f \n%f'...
    ,labelStr{:},maxScore,score(1),score(2),score(3),score(4));

% Create files with results
fid = fopen( 'results.txt','w' );
fprintf(fid,"%s",textToSave);
fclose(fid);


end

