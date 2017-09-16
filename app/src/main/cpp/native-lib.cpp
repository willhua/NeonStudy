#include <jni.h>
#include <string>
#include <arm_neon.h>
#include <math.h>
#include <android/log.h>

#define log(...) __android_log_print(ANDROID_LOG_DEBUG, "LYH", __VA_ARGS__)

const int len = 3264 * 2448;
float data1[len];
float data2[len];
float result1[len];
float result2[len];
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



void *func1(void *param)
{
    float *r = &result1[(int)param];
    float *d1 = &data1[(int)param];
    float *d2 = &data2[(int)param];
    for (int i = 0; i < len / 4; ++i) {
        r[i] = d1[i] * d2[i];
    }
    return 0;
}

void *func2(void *param)
{
    float *r = &result2[(int)param];
    float *d1 = &data1[(int)param];
    float *d2 = &data2[(int)param];
    for (int i = 0; i < len / 4; i += 4) {
        float32x4_t vec1, vec2, vec;
        vec1 = vld1q_dup_f32(d1);
        vec2 = vld1q_dup_f32(d2);
        vec = vmulq_f32(vec1, vec2);
        vst1q_f32(r + i, vec);
    }
    return 0;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_jx_willhua_neonstudy_MainActivity_native1(
        JNIEnv *env,
        jobject /* this */) {

#if false

    pthread_t pt2[4];
    log("neon start");
    for (int i = 0; i < 4; ++i) {
        int param = i * len / 4;
        pthread_create(&pt2[i], NULL, func2, (void*)param);
    }
    for (int i = 0; i < 4; ++i) {
        pthread_join(pt2[i], 0);
    }
    log("neon end");

#endif

#if true

    pthread_t pt1[4];
    log("c++ start");
    for (int i = 0; i < 4; ++i) {
        int param = i * len / 4;
        pthread_create(&pt1[i], NULL, func1, (void*)param);
    }
    for (int i = 0; i < 4; ++i) {
        pthread_join(pt1[i], 0);
    }
    log("c++ end");

#endif


}



extern "C"
JNIEXPORT void JNICALL
Java_com_jx_willhua_neonstudy_MainActivity_native0(
        JNIEnv *env,
        jobject /* this */) {
    log("native start");

    getData();
    log("c++ start");
    for (int i = 0; i < len; ++i) {
        //result[i / 4] = data1[i] * data2[i] + data1[i+1] * data2[i+1] +data1[i+2] * data2[i+2] +data1[i+3] * data2[i+3];
        result1[i] = data1[i] * data2[i];
    }
    log("c++ end");
    float *p = result2;
    log("neon start");
    for (int i = 0; i < len; i += 4) {
        float32x4_t vec1, vec2, r;
        vec1 = vld1q_dup_f32(data1);
        vec2 = vld1q_dup_f32(data2);
        r = vmulq_f32(vec1, vec2);
        vst1q_f32(p+=4, r);
    }
    log("neon end");

}



