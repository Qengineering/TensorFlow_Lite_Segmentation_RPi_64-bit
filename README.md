![output image](https://qengineering.eu/images/SDcard16GB_tiny.jpg) Find this example on our [SD-image](https://github.com/Qengineering/RPi-image)
# TensorFlow_Lite_Segmentation_RPi_64-bit
![output image]( https://qengineering.eu/images/Unet_64.jpg )<br/>
## TensorFlow Lite Segmentation running on bare Raspberry Pi 4 with 64-bit OS
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)<br/><br/>

A fast C++ implementation of TensorFlow Lite Unet on a bare Raspberry Pi 4.
Once overclocked to 1850 MHz, the app runs at 7.2 FPS!

https://arxiv.org/abs/1606.00915 <br/>
Training set: VOC2017 <br/>
Size: 257x257 <br/>
Frame rate Unet Lite : 4.0 FPS (RPi 4 @ 1900 MHz - 32 bits OS) <br/>
Frame rate Unet Lite : 7.2 FPS (RPi 4 @ 1875 MHz - 64 bits OS) <br/>
<br/>
Special made for a bare Raspberry Pi see: https://qengineering.eu/install-tensorflow-2-lite-on-raspberry-pi-4.html <br/>
<br/>
To extract and run the network in Code::Blocks <br/>
$ mkdir *MyDir* <br/>
$ cd *MyDir* <br/>
$ wget https://github.com/Qengineering/TensorFlow_Lite_Segmentation_RPi_64-bit/archive/refs/heads/master.zip <br/>
$ unzip -j master.zip <br/>
Remove master.zip and README.md as they are no longer needed. <br/> 
$ rm master.zip <br/>
$ rm README.md <br/> <br/>
Your *MyDir* folder must now look like this: <br/> 
cat.jpg.mp4 <br/>
deeplabv3_257_mv_gpu.tflite <br/>
TestUnet.cpb <br/>
Unet.cpp<br/>
 <br/>
Run TestUnet.cpb withCode::Blocks. Remember, you also need a working OpenCV 4 on your Raspberry. <br/>
I fact you can run this example on any aarch64 Linux system. <br/>

See the movie at: https://www.youtube.com/watch?v=Kh9DLMgCIIE


