# IMC-project
Archive of all the programs used in a smart hydroponics project of mechatronics students of the INTEC by Diego Morales and John Ureña.

## Folder contents
### Matlab Codes
1. creatLgraphUsingConnectios: Supporting function that creates a Layer Graph object of a neural network in Matlab Deep Learning Toolbox
2. freezeWeights: Supporting function to freeze the weights of the first layers of the neural network for transfer learning.
3. findLayersToReplace: Supporting function to find learnable  layers on a neural netwokr for transfer learning.
4. resnet101TransferLearning: Train a CNN with resnet101 architecture using transfer learning.
5. raspiWebcamP2: This function loads the CNN and the camera module of the raspberry pi, takes a picture, classifies it and saves the results on a .txt file.
6. deployCnnOnRpi: This function deploys the function raspiWebcam2 or raspiStaticPicP2 as a standalone executable on the raspberry pi.
7. experimentLime (wip): Applies the Lime analisys on different images using the trained network.
8. raspiStaticPicP2: This function loads the CNN, classifies a static picture saved on the raspberry pi and saves the results on a .txt file.
9. readResizedImg: Reads and resizes a static image to a specified input size.

### Python Codes
1. arduinoComms: This file has all the functions used to communicate to the arduino via serial.
2. datasetInteraction: This file has all the functions used to interact with the dataset, modify it or send it via TCP server.
3. fileInteractions: This file has all the functions used to interact with files, images and classification results saved on the raspberry pi.
4. interrupts: This file contains the routine executed when an interrupt occurs due to the pressing of the emergency button.
5. iPadComms: This file contains all the functions used to communicate to the iPad via TCP server.
6. routines: This file contains all the routines that the project performs while functioning.
7. main: Main python file of the project.

All the python files with "test" on their name were used to test a specific feature befor writing the code.

### Arduino Codes
