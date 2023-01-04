/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#pragma once
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include "JsEnv.h"
#include "DynamicDelegateProxy.h"
#include "StructWrapper.h"
#include "CppObjectMapper.h"
#include "V8Utils.h"
#if !defined(ENGINE_INDEPENDENT_JSENV)
#include "Engine/Engine.h"
#endif
#include "ObjectMapper.h"
#include "JSLogger.h"
#include "TickerDelegateWrapper.h"
#if !defined(ENGINE_INDEPENDENT_JSENV)
#include "TypeScriptGeneratedClass.h"
#endif
#include "ContainerMeta.h"
#include "ObjectCacheNode.h"
#include <unordered_map>

#if ENGINE_MINOR_VERSION >= 25 || ENGINE_MAJOR_VERSION > 4
#include "UObject/WeakFieldPtr.h"
#endif

#pragma warning(push, 0)
#include "libplatform/libplatform.h"
#include "v8.h"
#pragma warning(pop)

#include "V8InspectorImpl.h"

#if defined(WITH_NODEJS)
#pragma warning(push, 0)
#include "node.h"
#include "uv.h"
#pragma warning(pop)
#endif

#if V8_MAJOR_VERSION < 8 || defined(WITH_QUICKJS) || defined(WITH_NODEJS) || (WITH_EDITOR && !defined(FORCE_USE_STATIC_V8_LIB))
#define WITH_BACKING_STORE_AUTO_FREE 0
#else
#define WITH_BACKING_STORE_AUTO_FREE 1
#endif
namespace puerts
{

class JSError
{
public:
    std::string Message;

    JSError()
    {
    }

    explicit JSError(const std::string& m) : Message(m)
    {
    }
};

class FJsEnvImpl : public IJsEnv, IObjectMapper
{
public:
    explicit FJsEnvImpl(const std::string& ScriptRoot);

    FJsEnvImpl(std::shared_ptr<IJSModuleLoader> InModuleLoader, std::shared_ptr<ILogger> InLogger, int InPort,  std::function<void(const std::string&)> InOnSourceLoadedCallback, void* InExternalRuntime = nullptr, void* InExternalContext = nullptr);

    virtual ~FJsEnvImpl() override;

    virtual void Start(const std::string& ModuleNameOrScript, const std::vector<std::pair<std::string, NFIModule*>>& Arguments, bool IsScript) override;

    virtual bool IdleNotificationDeadline(double DeadlineInSeconds) override;

    virtual void LowMemoryNotification() override;

    virtual void RequestMinorGarbageCollectionForTesting() override;

    virtual void RequestFullGarbageCollectionForTesting() override;

    virtual void WaitDebugger(double timeout) override
    {
#ifdef THREAD_SAFE
        v8::Locker Locker(MainIsolate);
#endif
        const auto startTime = FDateTime::Now();
        while (Inspector && !Inspector->Tick())
        {
            if (timeout > 0)
            {
                auto now = FDateTime::Now();
                if ((now - startTime).GetTotalSeconds() >= timeout)
                {
                    break;
                }
            }
        }
    }



    virtual std::string CurrentStackTrace() override;

    virtual void InitExtensionMethodsMap() override;

    void JsHotReload(std::string ModuleName, const std::string& JsSource);

    virtual void ReloadModule(std::string ModuleName, const std::string& JsSource) override;

    virtual void ReloadSource(const std::string& Path, const std::string& JsSource) override;

    std::function<void(const std::string&)> OnSourceLoadedCallback;

    virtual void OnSourceLoaded(std::function<void(const std::string&)> Callback) override;

public:
    virtual void UnBindCppObject(JSClassDefinition* ClassDefinition, void* Ptr) override;

    virtual void BindCppObject(v8::Isolate* InIsolate, JSClassDefinition* ClassDefinition, void* Ptr, v8::Local<v8::Object> JSObject, bool PassByPointer) override;

    virtual v8::Local<v8::Value> FindOrAddCppObject( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, const void* TypeId, void* Ptr, bool PassByPointer) override;

    virtual void BindContainer( void* Ptr, v8::Local<v8::Object> JSObject, void (*Callback)(const v8::WeakCallbackInfo<void>& data)) override;

    virtual void UnBindContainer(void* Ptr) override;

    virtual v8::Local<v8::Value> FindOrAddContainer(v8::Isolate* Isolate, v8::Local<v8::Context>& Context,  v8::Local<v8::Function> Constructor, PropertyMacro* Property1, PropertyMacro* Property2, void* Ptr, bool PassByPointer);

    virtual v8::Local<v8::Value> FindOrAddDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, UObject* Owner, PropertyMacro* Property, void* DelegatePtr, bool PassByPointer) override;

    virtual bool AddToDelegate( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr, v8::Local<v8::Function> JsFunction) override;

    virtual PropertyMacro* FindDelegateProperty(void* DelegatePtr) override;

    virtual FScriptDelegate NewManualReleaseDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, v8::Local<v8::Function> JsFunction, UFunction* SignatureFunction) override;

    void ReleaseManualReleaseDelegate(const v8::FunctionCallbackInfo<v8::Value>& Info);

    virtual bool RemoveFromDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr, v8::Local<v8::Function> JsFunction) override;

    virtual bool ClearDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr) override;

    virtual void ExecuteDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, const v8::FunctionCallbackInfo<v8::Value>& Info, void* DelegatePtr) override;


    virtual bool IsInstanceOfCppObject(const void* TypeId, v8::Local<v8::Object> JsObject) override;


    bool CheckDelegateProxies(float Tick);

    virtual v8::Local<v8::Value> CreateArray(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, FPropertyTranslator* Property, void* ArrayPtr) override;

    void InvokeDelegateCallback(UDynamicDelegateProxy* Proxy, void* Params);

    v8::UniquePersistent<v8::Function> JsPromiseRejectCallback;

    V8_INLINE static FJsEnvImpl* Get(v8::Isolate* Isolate)
    {
        return static_cast<FJsEnvImpl*>(FV8Utils::IsolateData<IObjectMapper>(Isolate));
    }

public:

private:
    bool LoadFile(const std::string& RequiringDir, const std::string& ModuleName, std::string& OutPath, std::string& OutDebugPath,std::string& Data, std::string& ErrInfo);

    void ExecuteModule(const std::string& ModuleName, std::function<std::string(const std::string&, const std::string&)> Preprocessor = nullptr);

    void EvalScript(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void Log(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void SearchModule(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void LoadModule(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void LoadCppType(const v8::FunctionCallbackInfo<v8::Value>& Info);

    bool GetContainerTypeProperty(v8::Local<v8::Context> Context, v8::Local<v8::Value> Value, PropertyMacro** PropertyPtr);

    v8::Local<v8::FunctionTemplate> GetTemplateOfClass(UStruct* Class, bool& Existed);

    FPropertyTranslator* GetContainerPropertyTranslator(PropertyMacro* Property);

    void SetTimeout(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void SetFTickerDelegate(const v8::FunctionCallbackInfo<v8::Value>& Info, bool Continue);

    void ReportExecutionException( v8::Isolate* Isolate, v8::TryCatch* TryCatch, std::function<void(const JSError*)> CompletionHandler);

    void RemoveFTickerDelegateHandle(FDelegateHandle* Handle);

    void SetInterval(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void ClearInterval(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void FindModule(const v8::FunctionCallbackInfo<v8::Value>& Info);

public:
#if !defined(ENGINE_INDEPENDENT_JSENV)
    TSharedPtr<ITsDynamicInvoker, ESPMode::ThreadSafe> TsDynamicInvoker;

    TSharedPtr<IDynamicInvoker, ESPMode::ThreadSafe> MixinInvoker;
#endif
private:
    puerts::FObjectRetainer UserObjectRetainer;

    puerts::FObjectRetainer SysObjectRetainer;

    std::shared_ptr<IJSModuleLoader> ModuleLoader;

    std::shared_ptr<ILogger> Logger;

    bool Started;

private:
    v8::Isolate::CreateParams CreateParams;


    uv_loop_t NodeUVLoop;

    std::unique_ptr<node::ArrayBufferAllocator> NodeArrayBufferAllocator;

    node::IsolateData* NodeIsolateData;

    node::Environment* NodeEnv;

    uv_thread_t PollingThread;

    uv_sem_t PollingSem;

    uv_async_t DummyUVHandle;

    bool PollingClosed = false;

    FGraphEventRef LastJob;

#if PLATFORM_LINUX
    int Epoll;
#endif

    void StartPolling();

    void UvRunOnce();

    void PollEvents();

    static void OnWatcherQueueChanged(uv_loop_t* loop);

    void WakeupPollingThread();

    void StopPolling();


    v8::Isolate* MainIsolate;

    v8::Global<v8::Context> DefaultContext;

    v8::Global<v8::Function> Require;

    v8::Global<v8::Function> ReloadJs;

    TMap<UStruct*, v8::UniquePersistent<v8::FunctionTemplate>> ClassToTemplateMap;

    TMap<FString, std::shared_ptr<FStructWrapper>> TypeReflectionMap;

    TMap<UObject*, v8::UniquePersistent<v8::Value>> ObjectMap;
    TMap<const class UObjectBase*, v8::UniquePersistent<v8::Value>> GeneratedObjectMap;

    TMap<void*, FObjectCacheNode> StructCache;

    TMap<void*, v8::UniquePersistent<v8::Value>> ContainerCache;

    FCppObjectMapper CppObjectMapper;



    v8::UniquePersistent<v8::FunctionTemplate> FixSizeArrayTemplate;

    struct ContainerPropertyInfo
    {
#if ENGINE_MINOR_VERSION < 25 && ENGINE_MAJOR_VERSION < 5
        TWeakObjectPtr<PropertyMacro> PropertyWeakPtr;
#else
        TWeakFieldPtr<PropertyMacro> PropertyWeakPtr;
#endif
        std::unique_ptr<FPropertyTranslator> PropertyTranslator;
    };

    std::map<PropertyMacro*, ContainerPropertyInfo> ContainerPropertyMap;

    std::map<UFunction*, std::unique_ptr<FFunctionTranslator>> JsCallbackPrototypeMap;

    std::map<UStruct*, std::unique_ptr<ObjectMerger>> ObjectMergers;

    struct DelegateObjectInfo
    {
        v8::UniquePersistent<v8::Object> JSObject;    // function to proxy save here
        TWeakObjectPtr<UObject> Owner;                //可用于自动清理
        DelegatePropertyMacro* DelegateProperty;
        MulticastDelegatePropertyMacro* MulticastDelegateProperty;
        UFunction* SignatureFunction;
        bool PassByPointer;
        TWeakObjectPtr<UDynamicDelegateProxy> Proxy;           // for delegate
        TSet<TWeakObjectPtr<UDynamicDelegateProxy>> Proxys;    // for MulticastDelegate
    };

    struct TsFunctionInfo
    {
        v8::UniquePersistent<v8::Function> JsFunction;

        std::unique_ptr<puerts::FFunctionTranslator> FunctionTranslator;
    };

    
    TSharedPtr<DynamicInvokerImpl, ESPMode::ThreadSafe> DynamicInvoker;


    v8::UniquePersistent<v8::FunctionTemplate> DelegateTemplate;

    v8::UniquePersistent<v8::FunctionTemplate> MulticastDelegateTemplate;

    v8::UniquePersistent<v8::FunctionTemplate> SoftObjectPtrTemplate;

    std::map<void*, DelegateObjectInfo> DelegateMap;

    std::map<UFunction*, TsFunctionInfo> TsFunctionMap;

    TMap<UFunction*, v8::UniquePersistent<v8::Function>> MixinFunctionMap;

    std::map<UStruct*, std::vector<UFunction*>> ExtensionMethodsMap;

    bool ExtensionMethodsMapInited = false;

    std::map<FDelegateHandle*, FTickerDelegateWrapper*> TickerDelegateHandleMap;

    FDelegateHandle DelegateProxiesCheckerHandler;

    V8Inspector* Inspector;

    V8InspectorChannel* InspectorChannel;

    v8::Global<v8::Function> InspectorMessageHandler;

    FContainerMeta ContainerMeta;

    v8::Global<v8::Map> ManualReleaseCallbackMap;

    std::vector<TWeakObjectPtr<UDynamicDelegateProxy>> ManualReleaseCallbackList;

#ifdef SINGLE_THREAD_VERIFY
    std::thread::id BoundThreadId;
#endif

    typedef void (FJsEnvImpl::*V8MethodCallback)(const v8::FunctionCallbackInfo<v8::Value>& Info);

    template <V8MethodCallback callback>
    struct MethodBindingHelper
    {
        static void Bind(v8::Isolate* Isolate, v8::Local<v8::Context> Context, v8::Local<v8::Object> Obj, const char* Key,
            v8::Local<v8::External> This)
        {
            Obj->Set(Context, FV8Utils::ToV8String(Isolate, Key),
                   v8::FunctionTemplate::New(
                       Isolate,
                       [](const v8::FunctionCallbackInfo<v8::Value>& Info)
                       {
                           auto Self = static_cast<FJsEnvImpl*>((v8::Local<v8::External>::Cast(Info.Data()))->Value());
                           (Self->*callback)(Info);
                       },
                       This)
                       ->GetFunction(Context)
                       .ToLocalChecked())
                .Check();
        }
    };
};

}