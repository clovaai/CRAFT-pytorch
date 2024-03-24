#!/bin/bash
if [ ! -f craft_mlt_25k.pth ];then
    gdown https://drive.google.com/uc?id=1Jk4eGD7crsqCCg9C9VjCLkMN3ze8kutZ
fi
if [ ! -f craft_ic15_20k.pth ];then
    gdown https://drive.google.com/uc?id=1i2R7UIUqmkUtF0jv_3MXTqmQ_9wuAnLf
fi
if [ ! -f craft_refiner_CTW1500.pth ];then
    gdown https://drive.google.com/uc?id=1XSaFwBkOaFOdtk4Ane3DFyJGPRw6v5bO
fi

