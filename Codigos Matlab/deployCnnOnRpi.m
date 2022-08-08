%% Set up
r = raspi;
board = targetHardware('Raspberry Pi')
% verify DeviceAddress, Username and Password
% edit using board.DeviceAddress if neccesary
%% Set Target Language and ARM compute
board.CoderConfig.TargetLang = 'C++'
dlcfg = coder.DeepLearningConfig('arm-compute');
dlcfg.ArmArchitecture = 'armv7';
%% Check arm version
r.system('strings $ARM_COMPUTELIB/lib/libarm_compute.so | grep arm_compute_versio | cut -d\  -f 1')
%% Set arm version
dlcfg.ArmComputeVersion = '20.02.1';
%% Config deep learning
board.CoderConfig.DeepLearningConfig = dlcfg
%% Deploy Code
deploy(board,'raspiWebcamP2')
