#include <jni.h>
#include <string>
#include <arm_neon.h>
#include <math.h>
#include <android/log.h>

#define log(...) __android_log_print(ANDROID_LOG_DEBUG, "LYH", __VA_ARGS__)

const int len = 3264 * 2448;
float data1[len];
float data2[len];
float result[len];
std::string path1 = "/sdcard/float_flie1.data";
std::string path2 = "/sdcard/float_flie2.data";

void getData()
{
    FILE *stream = fopen(path1.c_str(), "rb");
    if(stream == NULL)
    {
        for (int i = 0; i < len; ++i) {
            data1[i] = sqrtf(i) / i + i / sqrt(i) / 1000.0 ;
            data2[i] = sqrtf(i) / i + i / sqrt(i) / 100.0 ;
        }
        stream = fopen(path1.c_str(), "wb");
        fwrite(data1, sizeof(float), len, stream);
        fflush(stream);
        fclose(stream);
        stream = fopen(path2.c_str(), "wb");
        fwrite(data2, sizeof(float), len, stream);
        fflush(stream);
        fclose(stream);
    } else{
        fread(data1, sizeof(float), len, stream);
        fclose(stream);
        stream = fopen(path2.c_str(), "rb");
        fread(data2, sizeof(float), len, stream);
        fclose(stream);
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_jx_willhua_neonstudy_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}



extern "C"
JNIEXPORT jstring JNICALL
Java_com_jx_willhua_neonstudy_MainActivity_native0(
        JNIEnv *env,
        jobject /* this */) {
    getData();
    log("c++ start");
    for (int i = 0; i < len; ++i) {
        //result[i / 4] = data1[i] * data2[i] + data1[i+1] * data2[i+1] +data1[i+2] * data2[i+2] +data1[i+3] * data2[i+3];
        result[i] = data1[i] * data2[i];
    }
    log("c++ end");


    log("neon start");
    for (int i = 0; i < len; i += 4) {
        float32x4_t vec1, vec2, r;
        vec1 = vld1q_dup_f32(data1);
        vec2 = vld1q_dup_f32(data2);
        r = vmulq_f32(vec1, vec2);
        vst1q_f32(result, r);
    }
    log("neon end");

}



