/*
Copyright 2014-2016 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef NatJ_ObjCRuntime_h
#define NatJ_ObjCRuntime_h

#include "NatJ.h"
#import <objc/NSObject.h>
#import <Foundation/NSObjCRuntime.h>

@class NSMutableDictionary;

@class ObjCInstanceContainer;

/**
 * Flags for NatJ class types
 */
typedef NS_OPTIONS(NSInteger, NatJClassType){
    NatJClassInheritedType = 0x1, NatJClassProxyType = 0x2,
    NatJClassHybridType = 0x4, NatJClassBindingType = 0x8,
};

/**
 * Dictionary containing Java construction infos
 *
 * These construction infos used when an inherited object created by native side
 * to create the respective Java object.
 */
extern NSMutableDictionary* gObjCAllocMap;

/**
 * Dictionary containing Java initialization infos
 *
 * These initialization infos used when a hybrid object's initialize class
 * method get invoked.
 */
extern NSMutableDictionary* gObjCHybridMap;

/**
 * Init target setter
 *
 * Sets the init target to given target in the current thread.
 *
 * @param target The target to set
 */
extern void setCurrentInitTargetOnCurrentThread(jobject target);

/**
 * Init target unsetter
 *
 * Unsets the init target in the current thread.
 */
extern void unsetCurrentInitTargetOnCurrentThread();

/**
 * Returns the NatJ class type of the specified class.
 *
 * @param The class we want to check
 */
extern NatJClassType getNatJClassType(Class);

/**
 * Is the class an inherited class?
 *
 * Determines whether the class is an inherited class generated by NatJ at
 * runtime.
 *
 * @param The class we want to check
 */
extern BOOL isInheritedClass(Class);

/**
 * Simplified version of isInheritedClass(), this only compares the flags.
 */
inline BOOL isInheritedNatJClass(NatJClassType t) {
  return (t & NatJClassInheritedType) == NatJClassInheritedType;
}

/**
 * Is the class a proxy class?
 *
 * Determines whether the class is a proxy class generated by NatJ at runtime.
 *
 * @param The class we want to check
 */
extern BOOL isProxyClass(Class);

/**
 * Simplified version of isProxyClass(), this only compares the flags.
 */
inline BOOL isProxyNatJClass(NatJClassType t) {
  return (t & NatJClassProxyType) == NatJClassProxyType;
}

/**
 * Is the class a hybrid class?
 *
 * Determines whether the class is a hybrid class.
 *
 * @param The class we want to check
 */
extern BOOL isHybridClass(Class);

/**
 * Simplified version of isHybridClass(), this only compares the flags.
 */
inline BOOL isHybridNatJClass(NatJClassType t) {
  return (t & (NatJClassInheritedType | NatJClassHybridType)) ==
         NatJClassHybridType;
}

/**
 * Is the class a binding class?
 *
 * Determines whether the class is a binding class.
 *
 * @param The class we want to check
 */
extern BOOL isBindingClass(Class);

/**
 * Simplified version of isBindingClass(), this only compares the flags.
 */
inline BOOL isBindingNatJClass(NatJClassType t) {
  return (t & NatJClassBindingType) == NatJClassBindingType;
}

/**
 * Cleanups after a deallocated Objective-C proxy
 *
 * @param env JNIEnv pointer for the current thread
 * @param The Java object
 */
extern void cleanupObjCProxy(JNIEnv*, jobject);

/**
 * Cleanups after an Objective-C block
 *
 * @param env JNIEnv pointer for the current thread
 * @param The Java object
 * @param The Java method object
 */
extern void cleanupObjCBlock(JNIEnv*, jobject, jobject);

/**
 * Sets associated instance container of binding object.
 */
extern void setBindingInstanceContainer(id, ObjCInstanceContainer*);

/**
 * Returns associated instance container of binding object.
 */
extern ObjCInstanceContainer* getBindingInstanceContainer(id);

/**
 * Sets associated instance container of non binding object.
 */
extern void setCustomInstanceContainer(id, ObjCInstanceContainer*);

/**
 * Returns associated instance container of non binding object.
 */
extern ObjCInstanceContainer* getCustomInstanceContainer(id);

/**
 * Returns associated instance container of Java block object,
 */
extern ObjCInstanceContainer* getJavaBlockInstanceContainer(id);

/**
 * The selector of the swizzled original initializer implementation.
 */
extern const SEL gObjCOriginalInitializeSelector;

/**
 * Structure containing every information needed for an Objective-C class
 * allocation.
 */
struct ObjCAllocInfo {
  /** The constrcution info for Java object construction */
  jobject constructionInfo;
};

/**
 * Structure containing every information needed for an Objective-C class
 * allocation for proxies with extension.
 */
struct ProxyExtensionInfo {
  /** The extension class */
  jclass extensionClass;
  
  /** The extension constructor */
  jmethodID extensionConstructor;
};

/**
 * Returns the alloc info for an Objective-C class.
 */
extern ObjCAllocInfo* getAllocInfoForClass(Class);

/**
 * Returns the extension info for a proxy Objective-C class.
 */
extern ProxyExtensionInfo* getExtensionInfoForProxyClass(Class);

/**
 * Returns the default variadic unboxing policy.
 */
extern int8_t getDefaultVariadicUnboxPolicy();

/**
 * Handles Objective-C startup
 *
 * If this rintume is not responsible for the given class nothing will happen
 * and false will be returned.
 */
extern bool handleObjCStartup(JNIEnv*, jclass);

/**
 * Returns callback from a block
 */
extern IMP getBlockCallback(id block);

/**
 * Returns the Objective-C runtime instance
 */
extern jobject getObjCRuntime();

/**
 * Return String with full stacktrace of java throwable object
 */
extern NSString* getExceptionStacktrace(JNIEnv*, jthrowable);

extern "C" {
/**
 * Initializes the ObjCRuntime, looks up Java classes and methods.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param instance CRuntime singleton object
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_initialize(JNIEnv* env, jclass clazz,
                                                      jobject instance);

/**
 * Process a Java class and register native methods.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param type The Java type we want to process
 * @return The Objective-C Class object
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_registerClass(JNIEnv* env,
                                                         jclass clazz,
                                                         jclass type);

/**
 * Retains an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C object we want to retain
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_retainObject(JNIEnv* env,
                                                        jclass clazz,
                                                        jlong ptr);

/**
 * Releases an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C object we want to release
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_releaseObject(JNIEnv* env,
                                                         jclass clazz,
                                                         jlong ptr);

/**
 * Autoreleases an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C object we want to autorelease
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_autoreleaseObject(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong ptr);

/**
 * Creates a new autorelease pool.
 *
 * <p>
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @return The newly created autorelease pool
 */
jlong Java_org_moe_natj_objc_ObjCRuntime_createAutoreleasePool(JNIEnv* env,
                                                                     jclass clazz);

/**
 * Releases an autorelease pool.
 *
 * <p>
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param pool The pool we want to release
 */
void Java_org_moe_natj_objc_ObjCRuntime_releaseAutoreleasePool(JNIEnv* env,
                                                                     jclass clazz,
                                                                     jlong peer);

/**
 * Constructs a Java string from an NSString instance.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param string The NSString instance we want to create Java string from
 * @return The Java string
 */
JNIEXPORT jstring JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createJavaString(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong string);

/**
 * Constructs a _NatJObjCCastProxy object for the specified peer.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param instance The instance of the proxy target
 * @param target The target ObjC class
 * @param transparent If this is true, then native side will see only the original peer
 * @return The _NatJObjCCastProxy instance peer
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createObjCCastProxy(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong instance,
                                                            jlong target,
                                                            jboolean transparent);

/**
 * Returns the peer of a _NatJObjCCastProxy object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param instance The instance of the proxy target
 * @return The peer of _NatJObjCCastProxy
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getObjCCastProxyPeer(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong instance);

/**
 * Constructs an NSString instance from a Java string.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param string The Java string we want to create NSString instance from
 * @return The native string
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createNativeString(JNIEnv* env,
                                                              jclass clazz,
                                                              jstring string);

/**
 * Constructs an Objective-C class from a Java class.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param type The Java class we want to create proxy for
 * @return The newly created Objective-C class
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createProxyClass(JNIEnv* env,
                                                            jclass clazz,
                                                            jclass type);

/**
 * Constructs an Objective-C class from a Java class and adds implementations specified by
 * extension.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param type The Java class we want to create proxy for
 * @param base Name of the base Objective-C class to be used as super class
 * @param extension The extension we want to use extra selectors from
 * @return The newly created Objective-C class
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createProxyClassWithExtension(JNIEnv* env,
                                                            jclass clazz,
                                                            jclass type,
                                                            jstring base,
                                                            jclass extension);

/**
 * Instantiates a proxy class for a Java instance.
 * Works only for classes we generate with
 * Java_org_moe_natj_objc_ObjCRuntime_createProxyClass().
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param clazzObject The Objective-C class object we want to instantiate
 * @param instance Java instance we want to create proxy object for
 * @return The Objective-C instance
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createProxyInstance(
        JNIEnv* env, jclass clazz, jlong clazzObject, jobject instance);

/**
 * Creates Native block construction data for a Java method
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param method Tha Java method
 * @return The native block construction data pointer
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createDataForNativeBlock(
        JNIEnv* env, jclass clazz, jobject method);
    
/**
 * Creates Native protocol proxy construction data for a Java interface
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param interface Tha Java interface
 * @return The native protocol proxy construction data pointer
 */
JNIEXPORT jobjectArray JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createDataForNativeProtocolProxy(
        JNIEnv* env, jclass clazz, jclass interface);

/**
 * Macros for declaring forwarders for native block calls
 */
#define TYPED_BLOCK_CALL_FORWARDER_IMPL(type, name)                      \
  JNIEXPORT type JNICALL                                                 \
      Java_org_moe_natj_objc_ObjCRuntime_forward##name##BlockCall( \
          JNIEnv* env, jclass clazz, jlong peer, jlong data,             \
          jobjectArray args);

TYPED_BLOCK_CALL_FORWARDER_IMPL(jboolean, Boolean)
TYPED_BLOCK_CALL_FORWARDER_IMPL(jbyte, Byte)
TYPED_BLOCK_CALL_FORWARDER_IMPL(jchar, Char)
TYPED_BLOCK_CALL_FORWARDER_IMPL(jshort, Short)
TYPED_BLOCK_CALL_FORWARDER_IMPL(jint, Int)
TYPED_BLOCK_CALL_FORWARDER_IMPL(jlong, Long)
TYPED_BLOCK_CALL_FORWARDER_IMPL(jfloat, Float)
TYPED_BLOCK_CALL_FORWARDER_IMPL(jdouble, Double)
TYPED_BLOCK_CALL_FORWARDER_IMPL(jobject, Object)

#undef TYPED_BLOCK_CALL_FORWARDER_IMPL

JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_forwardVoidBlockCall(
        JNIEnv* env, jclass clazz, jlong peer, jlong data, jobjectArray args);
    
/**
 * Macros for declaring forwarders for native protocol calls
 */
#define TYPED_PROTOCOL_CALL_FORWARDER_IMPL(type, name)                           \
  JNIEXPORT type JNICALL                                                         \
      Java_org_moe_natj_objc_ObjCRuntime_forward##name##ProtocolCall(      \
          JNIEnv* env, jclass clazz, jlong peer, jlong data, jobjectArray args);

TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jboolean, Boolean)
TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jbyte, Byte)
TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jchar, Char)
TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jshort, Short)
TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jint, Int)
TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jlong, Long)
TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jfloat, Float)
TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jdouble, Double)
TYPED_PROTOCOL_CALL_FORWARDER_IMPL(jobject, Object)

#undef TYPED_PROTOCOL_CALL_FORWARDER_IMPL

JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_forwardVoidProtocolCall(
        JNIEnv* env, jclass clazz, jlong peer, jlong data, jobjectArray args);

/**
 * Creates Java block construction data for a Java method
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param method Tha Java method
 * @return The Java block construction data pointer
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createDataForJavaBlock(
        JNIEnv* env, jclass clazz, jobject method);

/**
 * Constructs an Objective-C block from a Java method.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param instance The Java instance we want to create native callback for
 * @param data The data pointer created by a call to
 * Java_org_moe_natj_objc_ObjCRuntime_createDataForJavaBlock()
 * @return The native callback
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createNativeCallbackFromJavaInstance(
        JNIEnv* env, jclass clazz, jobject instance, jlong data);

/**
 * Constructs a Java instance from a given callback.
 * Works only with callbacks we create with
 * Java_org_moe_natj_objc_ObjCRuntime_createNativeCallbackFromJavaInstance().
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param callback The callback we want to create Java instance from
 * @return The Java instance
 */
JNIEXPORT jobject JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getInstanceForJavaBlock(
        JNIEnv* env, jclass clazz, jlong callback);

/**
 * Returns the Java instance associated with an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The Objective-C object we want to get Java instance from
 * @return The Java instance
 */
JNIEXPORT jobject JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getJavaReferenceOfBindingObject(
        JNIEnv* env, jclass clazz, jlong object);

/**
 * Sets the Java instance associated with an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The Objective-C object we want to set the associated Java
 * instance of
 * @param newRef The Java instance we want to associate with the Objective-C
 * object
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_setJavaReferenceOfBindingObject(
        JNIEnv* env, jclass clazz, jlong object, jobject newRef);

/**
 * Returns the custom Java instance associated with an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The Objective-C object we want to get Java instance from
 * @return The Java instance
 */
JNIEXPORT jobject JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getJavaReferenceOfCustomObject(
        JNIEnv* env, jclass clazz, jlong object);

/**
 * Sets the custom Java instance associated with an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The Objective-C object we want to set the associated Java
 * instance of
 * @param newRef The Java instance we want to associate with the Objective-C
 * object
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_setJavaReferenceOfCustomObject(
        JNIEnv* env, jclass clazz, jlong object, jobject newRef);

/**
 * Determines whether an Objective-C class is a runtime created inherited class.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C class object
 * @return Whether the given class is a runtime created inerited class
 */
JNIEXPORT jboolean JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_isKindOfInheritedClass(JNIEnv* env,
                                                                  jclass clazz,
                                                                  jlong ptr);

/**
 * Determines whether an Objective-C class is a runtime created proxy class.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C class object
 * @return Whether the given class is a runtime created proxy class
 */
JNIEXPORT jboolean JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_isKindOfProxyClass(JNIEnv* env,
                                                              jclass clazz,
                                                              jlong ptr);

/**
 * Determines whether an Objective-C class is a hybrid class.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C class object
 * @return Whether the given class is a runtime created hybrid class
 */
JNIEXPORT jboolean JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_isKindOfHybridClass(JNIEnv* env,
                                                               jclass clazz,
                                                               jlong ptr);

/**
 * Gets description of an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C object
 * @return The oject description
 */
JNIEXPORT jstring JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getObjectDescription(JNIEnv* env,
                                                                jclass clazz,
                                                                jlong ptr);

/**
 * Returns the Objective-C class of an Objective-C object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C object we want to get the class of
 * @return The class of the Objective-C object
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getObjectClass(JNIEnv* env,
                                                          jclass clazz,
                                                          jlong object);

/**
 * Returns the parent of an Objective-C class object.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param ptr The Objective-C class object we want to get the parent of
 * @return The parent class
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getClassParent(JNIEnv* env,
                                                          jclass clazz,
                                                          jlong object);

/**
 * Lookups and returns Objective-C class by its name.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param string The name of the Objective-C class we want to get
 * @return The Objective-C class
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getClassByName(JNIEnv* env,
                                                          jclass clazz,
                                                          jstring string);

/**
 * Determines whether the given object is a string.
 *
 * Also documented in ObjCRuntime.h
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The object we want to check
 * @return Whether the object is a string
 */
JNIEXPORT jboolean JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_isObjectString(JNIEnv* env,
                                                          jclass clazz,
                                                          jlong object);

/**
 * Determines whether the given object is a block.
 *
 * Also documented in ObjCRuntime.h
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The object we want to check
 * @return Whether the object is a block
 */
JNIEXPORT jboolean JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_isObjectBlock(JNIEnv* env,
                                                         jclass clazz,
                                                         jlong object);

/**
 * Determines whether the given object is a stack block.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The object we want to check
 * @return Whether the object is a stack block
 */
JNIEXPORT jboolean JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_isStackBlock(JNIEnv* env,
                                                        jclass clazz,
                                                        jlong object);

/**
 * Copies a native block.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The object we want to copy
 * @return The copied block
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_copyBlock(JNIEnv* env, jclass clazz,
                                                     jlong object);

/**
 * Returns the name of an Objective-C class.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param cls The Objective-C class object we want to get the name of
 * @return The Objective-C calss name
 */
JNIEXPORT jstring JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getClassName(JNIEnv* env,
                                                        jclass clazz,
                                                        jlong cls);

/**
 * Gets the target of an actual intializer method call of the current thread.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @return The init call target
 */
JNIEXPORT jobject JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getInitTargetOnCurrentThread(
        JNIEnv* env, jclass clazz);

/**
 * Registers a selector by its name
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param string The name of the selector we want to register
 * @return The SEL object
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_registerSelector(JNIEnv* env,
                                                            jclass clazz,
                                                            jstring string);

/**
 * Locks Objective-C object
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The Objective-C object we want to lock
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_lockObject(JNIEnv* env, jclass clazz,
                                                      jlong object);

/**
 * Unlocks Objective-C object
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The Objective-C object we want to unlock
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_unlockObject(JNIEnv* env,
                                                        jclass clazz,
                                                        jlong object);

/**
 * Gets Java type for hybrid Objective-C class
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The Objective-C object we want to get the relevant Java class
 * for
 * @return The relevant Java class
 */
JNIEXPORT jclass JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_getJavaTypeForHybridClass(
        JNIEnv* env, jclass clazz, jlong object);

/**
 * Stores a weak reference to an Objective-C object
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The Objective-C object we want to store weak reference for
 * @param object The address of the reference
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_storeWeak(JNIEnv* env, jclass clazz,
                                                     jlong object,
                                                     jlong location);

/**
 * Loads a weak reference to an Objective-C object
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The address of the reference
 * @return The referenced Objective-C object
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_loadWeak(JNIEnv* env, jclass clazz,
                                                    jlong location);

/**
 * Destroys a weak reference to an Objective-C object
 *
 * Also documented in ObjCRuntime.java
 *
 * @param env JNIEnv pointer for the current thread
 * @param clazz Java class of ObjCRuntime, used for nothing
 * @param object The address of the reference
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_destroyWeak(JNIEnv* env,
                                                       jclass clazz,
                                                       jlong location);

/**
 * Associates an object to an Objective-C object
 *
 * The Objective-C object will retain the object for itself and it will release
 * the object when it gets deallocated.
 *
 * Also documented in ObjCRuntime.java
 *
 * @param object The ObjCObject to which we want to associate the instance
 * @param instance The object we want to associate
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_associateObjCObject(JNIEnv* env,
                                                               jclass clazz,
                                                               jlong object,
                                                               jlong instance);

/**
 * Removes an object from an Objective-C object's association list
 *
 * If the object was associated with the Objective-C object then it will be
 * removed from the association list and released.
 *
 * Also documented in ObjCRuntime.h
 *
 * @param object The ObjCObject with which we want to dissociate the
 * instance
 * @param instance The object we want to dissociate
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_dissociateObjCObject(
        JNIEnv* env, jclass clazz, jlong object, jlong instance);

/**
 * Creates a new autorelease pool
 *
 * Also documented in ObjCRuntime.h
 *
 * @return The newly created autorelease pool
 */
JNIEXPORT jlong JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_createAutoReleasePool(
        JNIEnv* env, jclass clazz, jlong location);

/**
 * Releases an autorelease pool
 *
 * Also documented in ObjCRuntime.h
 *
 * @param pool The pool we want to release
 */
JNIEXPORT void JNICALL
    Java_org_moe_natj_objc_ObjCRuntime_releaseAutoReleasePool(JNIEnv* env,
                                                                  jclass clazz,
                                                                  jlong pool);
}

#endif
