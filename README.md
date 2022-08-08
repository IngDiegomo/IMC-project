# IMC-project
Archive of all the programs used in a smart hydroponics project of mechatronics students of the INTEC by Diego Morales and John Ureña.

## Folder contents
### Matlab Codes
1. creatLgraphUsingConnectios: Supporting function that creates a Layer Graph object of a neural network in Matlab Deep Learning Toolbox
2. freezeWeights: Supporting function to freeze the weights of the first layers of the neural network for transfer learning.
3. findLayersToReplace: Supporting function to find learnable  layers on a neural netwokr for transfer learning.
4. resnet101TransferLearning: Train a CNN with resnet101 architecture using transfer learning.
5. raspiWebcamP2: This function loads the CNN and the camera module of the raspberry pi, takes a picture, classifies it and saves the results on a .txt file.
6. deployCnnOnRpi: This function deploys the function raspiWebcam2 as a standalone executable on the raspberry pi.
7. experimentLime (wip): Applies the Lime analisys on different images using the trained network.
