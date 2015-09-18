#include <jni.h>
void calljava(int);
void  init1(JNIEnv *, jobject); 
int jniRegisterNativeMethods(JNIEnv*,const char*,const JNINativeMethod*,int);
