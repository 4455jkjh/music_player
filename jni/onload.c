#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#define LOG_TAG "jni"
 
 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
 
 
jmethodID mid;
jclass objclass;
jobject mobj;
JavaVM *m_vm;
 
 int register_audio(JNIEnv *env) ;
//初始化的时候会调进来一次，在这个方法里持有jvm的引用
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    m_vm=vm;
    JNIEnv* env = NULL;
    jint result = -1;
    if(m_vm){
        LOGD("m_vm init success");
    }else{
        LOGD("m_vm init failed");
    }
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK){
        return result;
    }
	register_audio(env);
    return JNI_VERSION_1_6;
}
JNIEnv* getJNIEnv(){
    JNIEnv* env = NULL;
    jint result = -1;
    if ((*m_vm)->GetEnv(m_vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK){
        int status = (*m_vm)->AttachCurrentThread(m_vm, &env, 0);
        if (status < 0){
            return NULL;
        }
    }
    return env;
}

void calljava(int i){ 
    JNIEnv *evn=getJNIEnv();
    (*evn)->CallVoidMethod(evn, mobj, mid,i);
}
void init1(JNIEnv *evn, jobject object){
LOGD("call start");
 
 
    //在子线程中不能这样用
    //jclass tclass = (*evn)->FindClass(evn, "com/example/ndktest/CallbackTest");
 
    //这种写法可以用在子线程中
    objclass=(*evn)->GetObjectClass(evn, object);
    mid = (*evn)->GetMethodID(evn, objclass, "callback", "(I)V");
 
    //JNI 函数参数中 jobject 或者它的子类，其参数都是 local reference。Local reference 只在这个 JNI函数中有效，JNI函数返回后，引用的对象就被释放，它的生命周期就结束了。若要留着日后使用，则需根据这个 local reference 创建 global reference。Global reference 不会被系统自动释放，它仅当被程序明确调用 DeleteGlobalReference 时才被回收。（JNI多线程机制）
    mobj=(*evn)->NewGlobalRef(evn, object);
}
 int jniRegisterNativeMethods(JNIEnv* env,
                             const char* className,
                             const JNINativeMethod* gMethods,
                             int numMethods)
{
    jclass clazz;

    __android_log_print(ANDROID_LOG_INFO, "ffmpegaudio", "Registering %s natives\n", className);
    clazz = (*env)->FindClass(env,className);
    if (clazz == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "ffmpegaudio", "Native registration unable to find class '%s'\n", className);
        return -1;
    }
    if ((*env)->RegisterNatives(env,clazz, gMethods, numMethods) < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "ffmpegaudio", "RegisterNatives failed for '%s'\n", className);
        return -1;
    }
    return 0;
}
