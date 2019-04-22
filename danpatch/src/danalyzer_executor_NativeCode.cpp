#include <jni.h>
#include <stdio.h>
#include "danalyzer_executor_NativeCode.h"

// These are definitions pulled from Value.java file in the danalyzer project
static const int VALTYPE_BLN   = 0x0001;
static const int VALTYPE_CHR   = 0x0002;
static const int VALTYPE_INT8  = 0x0004;
static const int VALTYPE_INT16 = 0x0008;
static const int VALTYPE_INT32 = 0x0010;
static const int VALTYPE_INT64 = 0x0020;
static const int VALTYPE_FLT   = 0x0040;
static const int VALTYPE_DBL   = 0x0080;
static const int VALTYPE_STR   = 0x0100;
static const int VALTYPE_REF   = 0x0200;
static const int VALTYPE_ARY   = 0x2000;
static const int VALTYPE_MARY  = 0x4000;
static const int VALTYPE_SYM   = 0x8000;

  JNIEXPORT jobjectArray JNICALL Java_danalyzer_executor_NativeCode_newArrayNative
    (JNIEnv *jni, jclass clz, jint size, jint type) {

    // get access to the Value class & its fields
    jclass valueClass = jni->FindClass("danalyzer/executor/Value");
    if (NULL == valueClass) {
      printf("[[newArrayNative: 'Value' class not found]]\n");
      return NULL;
    }
    jfieldID fidType  = jni->GetFieldID(valueClass, "type", "I");
    if (NULL == fidType) {
      printf("[[newArrayNative: field 'type' not found in Value class]]\n");
      return NULL;
    }
    jfieldID fidValue = jni->GetFieldID(valueClass, "value", "Ljava/lang/Object;");
    if (NULL == fidValue) {
      printf("[[newArrayNative: field 'value' not found in Value class]]\n");
      return NULL;
    }

    // create an Object of the specified type
    jobject objItem = NULL;
    jclass itemClass;
    switch (type) {
      case VALTYPE_CHR:
      case VALTYPE_BLN:
      case VALTYPE_INT8:
      case VALTYPE_INT16:
      case VALTYPE_INT32:
        itemClass = jni->FindClass("Ljava/lang/Integer;");
        if (NULL == itemClass) {
          printf("[[newArrayNative: 'Integer' class not found]]\n");
          return NULL;
        }
        objItem = jni->AllocObject(itemClass);
        break;
      case VALTYPE_INT64:
        itemClass = jni->FindClass("Ljava/lang/Long;");
        if (NULL == itemClass) {
          printf("[[newArrayNative: 'Long' class not found]]\n");
          return NULL;
        }
        objItem = jni->AllocObject(itemClass);
        break;
      case VALTYPE_FLT:
        itemClass = jni->FindClass("Ljava/lang/Float;");
        if (NULL == itemClass) {
          printf("[[newArrayNative: 'Float' class not found]]\n");
          return NULL;
        }
        objItem = jni->AllocObject(itemClass);
        break;
      case VALTYPE_DBL:
        itemClass = jni->FindClass("Ljava/lang/Double;");
        if (NULL == itemClass) {
          printf("[[newArrayNative: 'Double' class not found]]\n");
          return NULL;
        }
        objItem = jni->AllocObject(itemClass);
        break;
      case VALTYPE_STR:
        itemClass = jni->FindClass("Ljava/lang/String;");
        if (NULL == itemClass) {
          printf("[[newArrayNative: 'String' class not found]]\n");
          return NULL;
        }
        objItem = jni->AllocObject(itemClass);
        break;
      default:
        break;
    }

    // create the Value item to fill the array with
    jobject objValue = jni->AllocObject(valueClass);
    if (NULL == objValue) {
      printf("[[newArrayNative: Failed to allocate Value item]]\n");
      return NULL;
    }
    jni->SetObjectField(objValue, fidValue, objItem);
    jni->SetIntField(objValue, fidType, type);

    // allocate the array of Values
    jobjectArray array = jni->NewObjectArray(size, valueClass, NULL);
    //printf("[[newArrayNative: array: %p]]\n", array);
    if (NULL == array) {
      printf("[[newArrayNative: Failed to allocate Value array]]\n");
      return NULL;
    }

    // now set each array entry to the specified Value type
    for (int ix = 0; ix < size; ix++) {
      jni->SetObjectArrayElement(array, ix, objValue);
    }

    //printf("[[newArrayNative: created array of length: %d, type %d]]\n", size, type);
    return (jobjectArray) array;
  }