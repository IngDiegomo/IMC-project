import subprocess

def takeAndClassifyPic():
    subprocess.call("/home/pi/MATLAB_ws/R2021a/C/Users/diego/Desktop/Trimestre_17_FINAL_YA_FJLHI_GJIOA_UFKHAG/P2/Software/Codigos_Matlab/raspiWebcamP2", shell = True)

def checkClassificationResults():
    f = open("/home/pi/MATLAB_ws/R2021a/C/Users/diego/Desktop/Trimestre_17_FINAL_YA_FJLHI_GJIOA_UFKHAG/P2/Software/Codigos_Matlab/results.txt")
    lines = f.readlines()
    label = lines[0]

    if label.find('ZR,NPKCa') != -1:
        return 0
    elif label.find('ZR,_PKCa') != -1:
        return 1
    elif label.find('ZR,N_KCa') != -1:
        return 2
    elif label.find('ZR,NP_Ca') != -1:
        return 3

    