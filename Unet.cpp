#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <opencv2/core/ocl.hpp>
#include "tensorflow/lite/builtin_op_data.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/model.h"
#include <cmath>

using namespace cv;
using namespace std;
using namespace tflite;

int model_width;
int model_height;
int model_channels;

std::unique_ptr<Interpreter> interpreter;

//------------------------------------------------------------------------------------
struct RGB {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};
//------------------------------------------------------------------------------------
const RGB Colors[21] = {{127,127,127} ,  // 0 background
                        {  0,  0,255} ,  // 1 aeroplane
                        {  0,255,  0} ,  // 2 bicycle
                        {255,  0,  0} ,  // 3 bird
                        {255,  0,255} ,  // 4 boat
                        {  0,255,255} ,  // 5 bottle
                        {255,255,  0} ,  // 6 bus
                        {  0,  0,127} ,  // 7 car
                        {  0,127,  0} ,  // 8 cat
                        {127,  0,  0} ,  // 9 chair
                        {127,  0,127} ,  //10 cow
                        {  0,127,127} ,  //11 diningtable
                        {127,127,  0} ,  //12 dog
                        {127,127,255} ,  //13 horse
                        {127,255,127} ,  //14 motorbike
                        {255,127,127} ,  //15 person
                        {255,127,255} ,  //16 potted plant
                        {127,255,255} ,  //17 sheep
                        {255,255,127} ,  //18 sofa
                        {  0, 91,127} ,  //19 train
                        { 91,  0,127} }; //20 tv monitor
//-----------------------------------------------------------------------------------------------------------------------
void GetImageTFLite(float* out, Mat &src)
{
    int i;
    float f;
    uint8_t *in;
    static Mat image;
    int Len = model_height * model_height * model_channels;

    // copy image to input as input tensor
    cv::resize(src, image, Size(model_width,model_height),INTER_NEAREST);

    in=image.data;
    for(i=0;i<Len;i++){
        f     =in[i];
        out[i]=(f - 127.5f) / 127.5f;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void detect_from_video(Mat &src)
{
    int i,j,k,mi;
    float mx,v;
    float *data;
    RGB *rgb;
    static Mat image;
    static Mat frame(model_width,model_height,CV_8UC3);
    static Mat blend(src.cols   ,src.rows    ,CV_8UC3);

    GetImageTFLite(interpreter->typed_tensor<float>(interpreter->inputs()[0]), src);

    interpreter->Invoke();      // run your model

    // get max object per pixel
    data = interpreter->tensor(interpreter->outputs()[0])->data.f;
    rgb = (RGB *)frame.data;

    for(i=0;i<model_height;i++){
        for(j=0;j<model_width;j++){
            for(mi=-1,mx=0.0,k=0;k<21;k++){
                v = data[21*(i*model_width+j)+k];
                if(v>mx){ mi=k; mx=v; }
            }
            rgb[j+i*model_width] = Colors[mi];
        }
    }

    //merge output into frame
    cv::resize(frame, blend, Size(src.cols,src.rows),INTER_NEAREST);
    cv::addWeighted(src, 0.5, blend, 0.5, 0.0, src);
}
//-----------------------------------------------------------------------------------------------------------------------
int main(int argc,char ** argv)
{
    float f;
    float FPS[16];
    int i;
    int In;
    int Fcnt=0;
    Mat frame;
    chrono::steady_clock::time_point Tbegin, Tend;

    for(i=0;i<16;i++) FPS[i]=0.0;

    // Load model
    std::unique_ptr<FlatBufferModel> model = FlatBufferModel::BuildFromFile("deeplabv3_257_mv_gpu.tflite");

    // Build the interpreter
    ops::builtin::BuiltinOpResolver resolver;
    InterpreterBuilder(*model.get(), resolver)(&interpreter);

    interpreter->AllocateTensors();
    interpreter->SetAllowFp16PrecisionForFp32(true);
    interpreter->SetNumThreads(4);      //quad core

    // Get input dimension from the input tensor metadata
    // Assuming one input only
    In = interpreter->inputs()[0];
    model_height   = interpreter->tensor(In)->dims->data[1];
    model_width    = interpreter->tensor(In)->dims->data[2];
    model_channels = interpreter->tensor(In)->dims->data[3];
    cout << "height   : "<< model_height << endl;
    cout << "width    : "<< model_width << endl;
    cout << "channels : "<< model_channels << endl;

    VideoCapture cap("Highway.mp4");
    if (!cap.isOpened()) {
        cerr << "ERROR: Unable to open the camera" << endl;
        return 0;
    }

    cout << "Start grabbing, press ESC on Live window to terminate" << endl;

    while(1){
        //frame=imread("cat.jpg");  //need to refresh frame before dnn class detection
        cap >> frame;
        if (frame.empty()) {
            cerr << "End of movie" << endl;
            break;
        }

        detect_from_video(frame);

        Tend = chrono::steady_clock::now();
        //calculate frame rate
        f = chrono::duration_cast <chrono::milliseconds> (Tend - Tbegin).count();

        Tbegin = chrono::steady_clock::now();

        FPS[((Fcnt++)&0x0F)]=1000.0/f;
        for(f=0.0, i=0;i<16;i++){ f+=FPS[i]; }
        putText(frame, format("FPS %0.2f",f/16),Point(10,20),FONT_HERSHEY_SIMPLEX,0.6, Scalar(0, 0, 255));

        //show output
        imshow("RPi 4 - 1.85 GHz - 2 Mb RAM", frame);

        char esc = waitKey(5);
        if(esc == 27) break;
    }

    cout << "Closing the camera" << endl;

    // When everything done, release the video capture and write object
    cap.release();

    destroyAllWindows();
    cout << "Bye!" << endl;

    return 0;
}
