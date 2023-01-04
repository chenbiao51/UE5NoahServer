/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */
#include <vector>
#include "JsEnvImpl.h"
#include "JsEnvModule.h"
#include "DynamicDelegateProxy.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "StructWrapper.h"
#include "DelegateWrapper.h"
#include "V8Utils.h"
#if !defined(ENGINE_INDEPENDENT_JSENV)
#include "Engine/Engine.h"
#endif
#include "ObjectMapper.h"
#include "JSLogger.h"
#include "TickerDelegateWrapper.h"
#include "Async/Async.h"
#if !defined(ENGINE_INDEPENDENT_JSENV)
#include "JSGeneratedClass.h"
#include "JSAnimGeneratedClass.h"
#include "JSWidgetGeneratedClass.h"
#include "JSGeneratedFunction.h"
#endif
#include "JSClassRegister.h"
#include "PromiseRejectCallback.hpp"
#if !defined(ENGINE_INDEPENDENT_JSENV)
#include "TypeScriptObject.h"
#include "TypeScriptGeneratedClass.h"
#include "Engine/UserDefinedEnum.h"
#endif
#include "ContainerMeta.h"

#pragma warning(push, 0)
#include "libplatform/libplatform.h"
#include "v8.h"
#pragma warning(pop)

#include "V8InspectorImpl.h"

#if !defined(WITH_NODEJS)

#if V8_MAJOR_VERSION < 8 && !defined(WITH_QUICKJS)

#if PLATFORM_WINDOWS
#include "Blob/Win64/NativesBlob.h"
#include "Blob/Win64/SnapshotBlob.h"
#elif PLATFORM_ANDROID_ARM
#include "Blob/Android/armv7a/NativesBlob.h"
#include "Blob/Android/armv7a/SnapshotBlob.h"
#elif PLATFORM_ANDROID_ARM64
#include "Blob/Android/arm64/NativesBlob.h"
#include "Blob/Android/arm64/SnapshotBlob.h"
#elif PLATFORM_MAC
#include "Blob/macOS/NativesBlob.h"
#include "Blob/macOS/SnapshotBlob.h"
#elif PLATFORM_IOS
#include "Blob/iOS/arm64/NativesBlob.h"
#include "Blob/iOS/arm64/SnapshotBlob.h"
#elif PLATFORM_LINUX
#include "Blob/Linux/NativesBlob.h"
#include "Blob/Linux/SnapshotBlob.h"
#endif

#else

#if PLATFORM_WINDOWS
#include "Blob/Win64MD/SnapshotBlob.h"
#elif PLATFORM_ANDROID_ARM
#include "Blob/Android/armv7a/SnapshotBlob.h"
#elif PLATFORM_ANDROID_ARM64
#include "Blob/Android/arm64/SnapshotBlob.h"
#elif PLATFORM_MAC
#include "Blob/macOS/SnapshotBlob.h"
#elif PLATFORM_IOS
#include "Blob/iOS/arm64/SnapshotBlob.h"
#elif PLATFORM_LINUX
#include "Blob/Linux/SnapshotBlob.h"
#endif

#endif

#else
#if PLATFORM_WINDOWS
#include <windows.h>
#elif PLATFORM_LINUX
#include <sys/epoll.h>
#elif PLATFORM_MAC
#include <sys/select.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#endif
#endif

#include <functional>
#include <string>
#include "NFComm/NFPluginModule/NFILogModule.h"

namespace puerts
{


FJsEnvImpl::FJsEnvImpl(const std::string& ScriptRoot): FJsEnvImpl( std::make_shared<DefaultJSModuleLoader>(ScriptRoot), std::make_shared<FDefaultLogger>(), -1, nullptr, nullptr, nullptr)
{
}

static void ToCString(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    auto Isolate = Info.GetIsolate();
    if (!Info[0]->IsString())
    {
        FV8Utils::ThrowException(Isolate, "expect a string");
        return;
    }

    v8::Local<v8::String> Str = Info[0]->ToString(Isolate->GetCurrentContext()).ToLocalChecked();

    const size_t Length = Str->Utf8Length(Isolate);
    v8::Local<v8::ArrayBuffer> Ab = v8::ArrayBuffer::New(Info.GetIsolate(), Length + 1);
#if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
    char* Buff = static_cast<char*>(v8::ArrayBuffer_Get_Data(Ab));
#else
    char* Buff = static_cast<char*>(Ab->GetContents().Data());
#endif
    Str->WriteUtf8(Isolate, Buff);
    Buff[Length] = '\0';
    Info.GetReturnValue().Set(Ab);
}

static void ToCPtrArray(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    const size_t Length = sizeof(void*) * Info.Length();

    v8::Local<v8::ArrayBuffer> Ret = v8::ArrayBuffer::New(Info.GetIsolate(), Length);
#if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
    void** Buff = static_cast<void**>(v8::ArrayBuffer_Get_Data(Ret));
#else
    void** Buff = static_cast<void**>(Ret->GetContents().Data());
#endif

    for (int i = 0; i < Info.Length(); i++)
    {
        auto Val = Info[i];
        void* Ptr = nullptr;
        if (Val->IsArrayBufferView())
        {
            v8::Local<v8::ArrayBufferView> BuffView = Val.As<v8::ArrayBufferView>();
            auto Ab = BuffView->Buffer();
#if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
            Ptr = static_cast<char*>(v8::ArrayBuffer_Get_Data(Ab)) + BuffView->ByteOffset();
#else
            Ptr = static_cast<char*>(Ab->GetContents().Data()) + BuffView->ByteOffset();
#endif
        }
        else if (Val->IsArrayBuffer())
        {
            auto Ab = v8::Local<v8::ArrayBuffer>::Cast(Val);
#if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
            Ptr = static_cast<char*>(v8::ArrayBuffer_Get_Data(Ab));
#else
            Ptr = static_cast<char*>(Ab->GetContents().Data());
#endif
        }
        Buff[i] = Ptr;
    }
    Info.GetReturnValue().Set(Ret);
}


void FJsEnvImpl::StartPolling()
{
    uv_async_init(&NodeUVLoop, &DummyUVHandle, nullptr);
    uv_sem_init(&PollingSem, 0);
    uv_thread_create(
        &PollingThread,
        [](void* arg)
        {
            auto* self = static_cast<FJsEnvImpl*>(arg);
            while (true)
            {
                uv_sem_wait(&self->PollingSem);

                if (self->PollingClosed)
                    break;

                self->PollEvents();

                if (self->PollingClosed)
                    break;

                self->LastJob = FFunctionGraphTask::CreateAndDispatchWhenReady(
                    [self]() { self->UvRunOnce(); }, TStatId{}, nullptr, ENamedThreads::GameThread);
            }
        },
        this);


    Epoll = epoll_create(1);
    int backend_fd = uv_backend_fd(&NodeUVLoop);
    struct epoll_event ev = {0};
    ev.events = EPOLLIN;
    ev.data.fd = backend_fd;
    epoll_ctl(Epoll, EPOLL_CTL_ADD, backend_fd, &ev);
    NodeUVLoop.data = this;
    NodeUVLoop.on_watcher_queue_updated = OnWatcherQueueChanged;


    UvRunOnce();
}

void FJsEnvImpl::UvRunOnce()
{
    auto Isolate = MainIsolate;
#ifdef THREAD_SAFE
    v8::Locker Locker(Isolate);
#endif
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = v8::Local<v8::Context>::New(Isolate, DefaultContext);
    v8::Context::Scope ContextScope(Context);

    // TODO: catch uv_run可以让脚本错误不至于进程退出，但这不知道会不会对node有什么副作用
    v8::TryCatch TryCatch(Isolate);

    uv_run(&NodeUVLoop, UV_RUN_NOWAIT);
    if (TryCatch.HasCaught())
    {
        Logger->Error(FString::Printf(TEXT("uv_run throw: %s"), *FV8Utils::TryCatchToString(Isolate, &TryCatch)));
    }
    else
    {
        static_cast<node::MultiIsolatePlatform*>(IJsEnvModule::Get().GetV8Platform())->DrainTasks(Isolate);
    }

    LastJob = nullptr;

    // Tell the Polling thread to continue.
    uv_sem_post(&PollingSem);
}

void FJsEnvImpl::PollEvents()
{
    int timeout = uv_backend_timeout(&NodeUVLoop);
    timeout = (timeout > 100 || timeout < 0) ? 100 : timeout;

    // Wait for new libuv events.
    int r;
    do
    {
        struct epoll_event ev;
        r = epoll_wait(Epoll, &ev, 1, timeout);
    } while (r == -1 && errno == EINTR);
}

void FJsEnvImpl::OnWatcherQueueChanged(uv_loop_t* loop)
{
    FJsEnvImpl* self = static_cast<FJsEnvImpl*>(loop->data);
    self->WakeupPollingThread();
}

void FJsEnvImpl::WakeupPollingThread()
{
    uv_async_send(&DummyUVHandle);
}

void FJsEnvImpl::StopPolling()
{
    PollingClosed = true;

    uv_sem_post(&PollingSem);

    WakeupPollingThread();

    uv_thread_join(&PollingThread);

    if (LastJob)
    {
        LastJob->Wait();
    }

    uv_sem_destroy(&PollingSem);
}


FJsEnvImpl::FJsEnvImpl(std::shared_ptr<IJSModuleLoader> InModuleLoader, std::shared_ptr<ILogger> InLogger, int InDebugPort,std::function<void(const std::string&)> InOnSourceLoadedCallback, void* InExternalRuntime, void* InExternalContext)
{
    GUObjectArray.AddUObjectDeleteListener(static_cast<FUObjectArray::FUObjectDeleteListener*>(this));

    Started = false;
    Inspector = nullptr;
    InspectorChannel = nullptr;

    ModuleLoader = std::move(InModuleLoader);
    Logger = InLogger;
    OnSourceLoadedCallback = InOnSourceLoadedCallback;

    int Argc = 1;
    const char* Argv[] = {"puerts"};
    std::vector<std::string> Args(Argv, Argv + Argc);
    std::vector<std::string> ExecArgs;
    std::vector<std::string> Errors;

    const int Ret = uv_loop_init(&NodeUVLoop);
    if (Ret != 0)
    {
        Logger->Error(FString::Printf(TEXT("Failed to initialize loop: %s\n"), UTF8_TO_TCHAR(uv_err_name(Ret))));
        return;
    }

    CreateParams.array_buffer_allocator = nullptr;
    NodeArrayBufferAllocator = node::ArrayBufferAllocator::Create();

    auto Platform = static_cast<node::MultiIsolatePlatform*>(IJsEnvModule::Get().GetV8Platform());
    MainIsolate = node::NewIsolate(NodeArrayBufferAllocator.get(), &NodeUVLoop, Platform);

    auto Isolate = MainIsolate;
#ifdef THREAD_SAFE
    v8::Locker Locker(Isolate);
    UserObjectRetainer.Isolate = Isolate;
    SysObjectRetainer.Isolate = Isolate;

    Isolate->SetData(0, static_cast<IObjectMapper*>(this));    //直接传this会有问题，强转后地址会变

    // v8::Locker locker(Isolate);
    // difference from embedding example, if lock, blow check fail:
    // Utils::ApiCheck(
    //! v8::Locker::IsActive() ||
    //    internal_isolate->thread_manager()->IsLockedByCurrentThread() ||
    //    internal_isolate->serializer_enabled(),
    //"HandleScope::HandleScope",
    //"Entering the V8 API without proper locking in place");

    v8::Isolate::Scope Isolatescope(Isolate);

    NodeIsolateData =
        node::CreateIsolateData(Isolate, &NodeUVLoop, Platform, NodeArrayBufferAllocator.get());    // node::FreeIsolateData

    v8::HandleScope HandleScope(Isolate);

    v8::Local<v8::Context> Context = node::NewContext(Isolate);

#endif

    DefaultContext.Reset(Isolate, Context);

    v8::Context::Scope ContextScope(Context);


    // kDefaultFlags = kOwnsProcessState | kOwnsInspector, if kOwnsInspector set, inspector_agent.cc:681
    // CHECK_EQ(start_io_thread_async_initialized.exchange(true), false) fail!
    NodeEnv = CreateEnvironment(NodeIsolateData, Context, Args, ExecArgs, node::EnvironmentFlags::kOwnsProcessState);

    v8::MaybeLocal<v8::Value> LoadenvRet = node::LoadEnvironment(NodeEnv,
        "const publicRequire ="
        "  require('module').createRequire(process.cwd() + '/');"
        "globalThis.require = publicRequire;");

    if (LoadenvRet.IsEmpty())    // There has been a JS exception.
    {
        return;
    }

    // the same as raw v8
    Isolate->SetMicrotasksPolicy(v8::MicrotasksPolicy::kAuto);

    StartPolling();


    v8::Local<v8::Object> Global = Context->Global();

    v8::Local<v8::Object> PuertsObj = v8::Object::New(Isolate);
    Global->Set(Context, FV8Utils::InternalString(Isolate, "puerts"), PuertsObj).Check();

    auto This = v8::External::New(Isolate, this);

    MethodBindingHelper<&FJsEnvImpl::EvalScript>::Bind(Isolate, Context, Global, "__tgjsEvalScript", This);

    MethodBindingHelper<&FJsEnvImpl::Log>::Bind(Isolate, Context, Global, "__tgjsLog", This);

    MethodBindingHelper<&FJsEnvImpl::SearchModule>::Bind(Isolate, Context, Global, "__tgjsSearchModule", This);

    MethodBindingHelper<&FJsEnvImpl::LoadModule>::Bind(Isolate, Context, Global, "__tgjsLoadModule", This);

    MethodBindingHelper<&FJsEnvImpl::LoadCppType>::Bind(Isolate, Context, Global, "__tgjsLoadCDataType", This);

    MethodBindingHelper<&FJsEnvImpl::FindModule>::Bind(Isolate, Context, Global, "__tgjsFindModule", This);

    MethodBindingHelper<&FJsEnvImpl::SetInspectorCallback>::Bind(Isolate, Context, Global, "__tgjsSetInspectorCallback", This);

    MethodBindingHelper<&FJsEnvImpl::DispatchProtocolMessage>::Bind(
        Isolate, Context, Global, "__tgjsDispatchProtocolMessage", This);

    Isolate->SetPromiseRejectCallback(&PromiseRejectCallback<FJsEnvImpl>);
    Global->Set(Context, FV8Utils::ToV8String(Isolate, "__tgjsSetPromiseRejectCallback"),v8::FunctionTemplate::New(Isolate, &SetPromiseRejectCallback<FJsEnvImpl>)->GetFunction(Context).ToLocalChecked()).Check();

    //#if !defined(WITH_NODEJS)
    MethodBindingHelper<&FJsEnvImpl::SetTimeout>::Bind(Isolate, Context, Global, "setTimeout", This);

    MethodBindingHelper<&FJsEnvImpl::ClearInterval>::Bind(Isolate, Context, Global, "clearTimeout", This);

    MethodBindingHelper<&FJsEnvImpl::SetInterval>::Bind(Isolate, Context, Global, "setInterval", This);

    MethodBindingHelper<&FJsEnvImpl::ClearInterval>::Bind(Isolate, Context, Global, "clearInterval", This);
    //#endif

    MethodBindingHelper<&FJsEnvImpl::DumpStatisticsLog>::Bind(Isolate, Context, Global, "dumpStatisticsLog", This);

    PuertsObj
        ->Set(Context, FV8Utils::ToV8String(Isolate, "toCString"),
            v8::FunctionTemplate::New(Isolate, ToCString)->GetFunction(Context).ToLocalChecked())
        .Check();

    PuertsObj
        ->Set(Context, FV8Utils::ToV8String(Isolate, "toCPtrArray"),
            v8::FunctionTemplate::New(Isolate, ToCPtrArray)->GetFunction(Context).ToLocalChecked())
        .Check();

    MethodBindingHelper<&FJsEnvImpl::ReleaseManualReleaseDelegate>::Bind(Isolate, Context, PuertsObj, "releaseManualReleaseDelegate", This);

    CppObjectMapper.Initialize(Isolate, Context);

    DelegateTemplate = v8::UniquePersistent<v8::FunctionTemplate>(Isolate, FDelegateWrapper::ToFunctionTemplate(Isolate));

    MulticastDelegateTemplate =
        v8::UniquePersistent<v8::FunctionTemplate>(Isolate, FMulticastDelegateWrapper::ToFunctionTemplate(Isolate));

    SoftObjectPtrTemplate = v8::UniquePersistent<v8::FunctionTemplate>(Isolate, FSoftObjectWrapper::ToFunctionTemplate(Isolate));

    DynamicInvoker = MakeShared<DynamicInvokerImpl, ESPMode::ThreadSafe>(this);
    MixinInvoker = DynamicInvoker;
#if !defined(ENGINE_INDEPENDENT_JSENV)
    TsDynamicInvoker = MakeShared<TsDynamicInvokerImpl, ESPMode::ThreadSafe>(this);
#endif

    Inspector = CreateV8Inspector(InDebugPort, &Context);

    ExecuteModule("puerts/first_run.js");
    ExecuteModule("puerts/log.js");
    ExecuteModule("puerts/modular.js");
    ExecuteModule("puerts/uelazyload.js");
    ExecuteModule("puerts/events.js");
    ExecuteModule("puerts/promises.js");
    ExecuteModule("puerts/argv.js");
    ExecuteModule("puerts/jit_stub.js");
    ExecuteModule("puerts/hot_reload.js");

    Require.Reset(Isolate, PuertsObj->Get(Context, FV8Utils::ToV8String(Isolate, "__require")).ToLocalChecked().As<v8::Function>());

    ReloadJs.Reset(Isolate, PuertsObj->Get(Context, FV8Utils::ToV8String(Isolate, "__reload")).ToLocalChecked().As<v8::Function>());

    DelegateProxiesCheckerHandler =FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FJsEnvImpl::CheckDelegateProxies), 1);

    ManualReleaseCallbackMap.Reset(Isolate, v8::Map::New(Isolate));

    UserObjectRetainer.SetName(TEXT("Puerts_UserObjectRetainer"));
    SysObjectRetainer.SetName(TEXT("Puerts_SysObjectRetainer"));

#ifdef SINGLE_THREAD_VERIFY
    BoundThreadId = std::this_thread::get_id();
#endif
}

// #lizard forgives
FJsEnvImpl::~FJsEnvImpl()
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogError("Access by illegal thread!");
        return;
    }
#endif

    StopPolling();
    for (int i = 0; i < ManualReleaseCallbackList.size(); i++)
    {
        if (ManualReleaseCallbackList[i].IsValid())
        {
            ManualReleaseCallbackList[i].Get()->JsFunction.Reset();
        }
    }
    ManualReleaseCallbackMap.Reset();
    InspectorMessageHandler.Reset();
    Require.Reset();
    ReloadJs.Reset();
    JsPromiseRejectCallback.Reset();

    FTicker::GetCoreTicker().RemoveTicker(DelegateProxiesCheckerHandler);

    {
        auto Isolate = MainIsolate;
#ifdef THREAD_SAFE
        v8::Locker Locker(Isolate);
#endif
        v8::Isolate::Scope IsolateScope(Isolate);
        v8::HandleScope HandleScope(Isolate);

        ClassToTemplateMap.Empty();

        CppObjectMapper.UnInitialize(Isolate);

        ObjectMap.Empty();

        GeneratedObjectMap.Empty();

        StructCache.Empty();

        ContainerCache.Empty();

        for (auto Iter = DelegateMap.begin(); Iter != DelegateMap.end(); Iter++)
        {
            Iter->second.JSObject.Reset();
            if (Iter->second.Proxy.IsValid())
            {
                Iter->second.Proxy->JsFunction.Reset();
            }
            for (auto ProxyIter = Iter->second.Proxys.CreateIterator(); ProxyIter; ++ProxyIter)
            {
                if (!(*ProxyIter).IsValid())
                {
                    continue;
                }
                (*ProxyIter)->JsFunction.Reset();
            }
            if (!Iter->second.PassByPointer)
            {
                delete ((FScriptDelegate*) Iter->first);
            }
        }

        TsFunctionMap.Empty();
        MixinFunctionMap.Empty();

#if !defined(ENGINE_INDEPENDENT_JSENV)
        TsDynamicInvoker.Reset();
        BindInfoMap.Empty();
#endif

        for (auto& Pair : TickerDelegateHandleMap)
        {
            FTicker::GetCoreTicker().RemoveTicker(*(Pair.first));
            delete Pair.first;
            delete Pair.second;
        }
        TickerDelegateHandleMap.clear();

        node::EmitExit(NodeEnv);
        node::Stop(NodeEnv);
        node::FreeEnvironment(NodeEnv);
        node::FreeIsolateData(NodeIsolateData);

        auto Platform = static_cast<node::MultiIsolatePlatform*>(IJsEnvModule::Get().GetV8Platform());
        Platform->UnregisterIsolate(Isolate);

        if (InspectorChannel)
        {
            delete InspectorChannel;
            InspectorChannel = nullptr;
        }

        if (Inspector)
        {
            delete Inspector;
            Inspector = nullptr;
        }

        DynamicInvoker.Reset();
        MixinInvoker.Reset();

        SoftObjectPtrTemplate.Reset();
        MulticastDelegateTemplate.Reset();
        DelegateTemplate.Reset();
        FixSizeArrayTemplate.Reset();
        MapTemplate.Reset();
        SetTemplate.Reset();
        ArrayTemplate.Reset();
    }

    DefaultContext.Reset();
    MainIsolate->Dispose();
    MainIsolate = nullptr;
    delete CreateParams.array_buffer_allocator;

    GUObjectArray.RemoveUObjectDeleteListener(static_cast<FUObjectArray::FUObjectDeleteListener*>(this));

    // quickjs will call UnBind in vm dispose, so cleanup move to here
#if !WITH_BACKING_STORE_AUTO_FREE && !defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
    for (auto& KV : ScriptStructFinalizeInfoMap)
    {
        FScriptStructWrapper::Free(KV.Value.Struct, KV.Value.Finalize, KV.Key);
    }
#endif
}

void FJsEnvImpl::InitExtensionMethodsMap()
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogError("Access by illegal thread!");
        return;
    }
#endif
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif
#if !defined(ENGINE_INDEPENDENT_JSENV)
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* Class = *It;
        if (Class->IsChildOf<UExtensionMethods>() && Class->IsNative())
        {
            for (TFieldIterator<UFunction> FuncIt(Class, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
            {
                UFunction* Function = *FuncIt;

                if (Function->HasAnyFunctionFlags(FUNC_Static))
                {
                    TFieldIterator<PropertyMacro> ParamIt(Function);
                    if (ParamIt &&
                        ((ParamIt->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm))    // has at least one param
                    {
                        UStruct* Struct = nullptr;
                        if (auto ObjectPropertyBase = CastFieldMacro<ObjectPropertyBaseMacro>(*ParamIt))
                        {
                            Struct = ObjectPropertyBase->PropertyClass;
                        }
                        else if (auto StructProperty = CastFieldMacro<StructPropertyMacro>(*ParamIt))
                        {
                            Struct = StructProperty->Struct;
                        }
                        if (Struct)
                        {
                            if (ExtensionMethodsMap.find(Struct) == ExtensionMethodsMap.end())
                            {
                                ExtensionMethodsMap[Struct] = std::vector<UFunction*>();
                            }
                            auto Iter = ExtensionMethodsMap.find(Struct);

                            if (std::find(Iter->second.begin(), Iter->second.end(), Function) == Iter->second.end())
                            {
                                Iter->second.push_back(Function);
                            }
                        }
                    }
                }
            }
        }
    }
#endif
    ExtensionMethodsMapInited = true;
}


bool FJsEnvImpl::IdleNotificationDeadline(double DeadlineInSeconds)
{
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif

    return MainIsolate->IdleNotificationDeadline(DeadlineInSeconds);

}

void FJsEnvImpl::LowMemoryNotification()
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogError("Access by illegal thread!");
        return;
    }
#endif
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif
    MainIsolate->LowMemoryNotification();
}

void FJsEnvImpl::RequestMinorGarbageCollectionForTesting()
{
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif
    MainIsolate->RequestGarbageCollectionForTesting(v8::Isolate::kMinorGarbageCollection);

}

void FJsEnvImpl::RequestFullGarbageCollectionForTesting()
{
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif

    MainIsolate->RequestGarbageCollectionForTesting(v8::Isolate::kFullGarbageCollection);

}


void FJsEnvImpl::JsHotReload(FName ModuleName, const FString& JsSource)
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogError("Access by illegal thread!");
        return;
    }
#endif
    auto Isolate = MainIsolate;
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = DefaultContext.Get(Isolate);
    v8::Context::Scope ContextScope(Context);
    auto LocalReloadJs = ReloadJs.Get(Isolate);

    FString OutPath, OutDebugPath;

    if (ModuleLoader->Search(TEXT(""), ModuleName.ToString(), OutPath, OutDebugPath))
    {
        OutPath = FPaths::ConvertRelativePathToFull(OutPath);
        Logger->Info(FString::Printf(TEXT("reload js module [%s]"), *OutPath));
        v8::TryCatch TryCatch(Isolate);
        v8::Handle<v8::Value> Args[] = {FV8Utils::ToV8String(Isolate, ModuleName), FV8Utils::ToV8String(Isolate, OutPath),
            FV8Utils::ToV8String(Isolate, JsSource)};

        auto MaybeRet = LocalReloadJs->Call(Context, v8::Undefined(Isolate), 3, Args);

        if (TryCatch.HasCaught())
        {
            Logger->Error(FString::Printf(TEXT("reload module exception %s"), *FV8Utils::TryCatchToString(Isolate, &TryCatch)));
        }
    }
    else
    {
        Logger->Warn(FString::Printf(TEXT("not find js module [%s]"), *ModuleName.ToString()));
        return;
    }
}

void FJsEnvImpl::ReloadModule(FName ModuleName, const FString& JsSource)
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogError("Access by illegal thread!");
        return;
    }
#endif
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif
    // Logger->Info(FString::Printf(TEXT("start reload js module [%s]"), *ModuleName.ToString()));
    JsHotReload(ModuleName, JsSource);
}

void FJsEnvImpl::ReloadSource(const std::string& Path, const std::string& JsSource)
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogInfo("Access by illegal thread!");
        return;
    }
#endif
    auto Isolate = MainIsolate;
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = DefaultContext.Get(Isolate);
    v8::Context::Scope ContextScope(Context);
    auto LocalReloadJs = ReloadJs.Get(Isolate);

    std::ostringstream stream;
	stream << "reload js [" << *Path <<"]";
	m_pLogModule->LogInfo(stream);
    v8::TryCatch TryCatch(Isolate);
    v8::Handle<v8::Value> Args[] = {v8::Undefined(Isolate), FV8Utils::ToV8String(Isolate, Path), FV8Utils::ToV8String(Isolate, JsSource.c_str())};

    auto MaybeRet = LocalReloadJs->Call(Context, v8::Undefined(Isolate), 3, Args);

    if (TryCatch.HasCaught())
    {
        std::ostringstream streama;
		streama << "reload module exception " << FV8Utils::TryCatchToString(Isolate, &TryCatch));
		m_pLogModule->LogError(streama);
    }
}

void FJsEnvImpl::OnSourceLoaded(std::function<void(const FString&)> Callback)
{
    OnSourceLoadedCallback = Callback;
}

std::string FJsEnvImpl::CurrentStackTrace()
{
    return "";
}


v8::Local<v8::Value> FJsEnvImpl::FindOrAddCppObject(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, const void* TypeId, void* Ptr, bool PassByPointer)
{
    return CppObjectMapper.FindOrAddCppObject(Isolate, Context, TypeId, Ptr, PassByPointer);
}

v8::Local<v8::Value> FJsEnvImpl::FindOrAddDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, UObject* Owner, PropertyMacro* Property, void* DelegatePtr, bool PassByPointer)
{
    check(DelegatePtr);    // must not null

    if (PassByPointer)
    {
        auto Iter = DelegateMap.find(DelegatePtr);
        if (Iter != DelegateMap.end())
        {
            if (Iter->second.Owner.IsValid())
            {
                return Iter->second.JSObject.Get(Isolate);
            }
            else
            {
                ClearDelegate(Isolate, Context, DelegatePtr);
            }
        }
    }
    else
    {
        if (CastFieldMacro<DelegatePropertyMacro>(Property))
        {
            auto NewDelegatePtr = new FScriptDelegate;
            *NewDelegatePtr = *static_cast<FScriptDelegate*>(DelegatePtr);
            DelegatePtr = NewDelegatePtr;
        }
        else    // do not support MulticastDelegate
        {
            return v8::Undefined(Isolate);
        }
    }

    {
        // UE_LOG(LogTemp, Warning, TEXT("FindOrAddDelegate -- new %s"), *Property->GetName());
        auto Constructor = (Property->IsA<DelegatePropertyMacro>() ? DelegateTemplate : MulticastDelegateTemplate)
                               .Get(Isolate)
                               ->GetFunction(Context)
                               .ToLocalChecked();
        auto JSObject = Constructor->NewInstance(Context).ToLocalChecked();
        DataTransfer::SetPointer(Isolate, JSObject, DelegatePtr, 0);
        auto ReturnVal = JSObject->Set(Context, 0, v8::Map::New(Isolate));
        UFunction* Function = nullptr;
        DelegatePropertyMacro* DelegateProperty = CastFieldMacro<DelegatePropertyMacro>(Property);
        MulticastDelegatePropertyMacro* MulticastDelegateProperty = CastFieldMacro<MulticastDelegatePropertyMacro>(Property);
        if (DelegateProperty)
        {
            Function = DelegateProperty->SignatureFunction;
        }
        else if (MulticastDelegateProperty)
        {
            Function = MulticastDelegateProperty->SignatureFunction;
        }
        DelegateMap[DelegatePtr] = {v8::UniquePersistent<v8::Object>(Isolate, JSObject), TWeakObjectPtr<UObject>(Owner),
            DelegateProperty, MulticastDelegateProperty, Function, PassByPointer, nullptr};
        return JSObject;
    }
}

v8::Local<v8::Value> FJsEnvImpl::CreateArray(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, FPropertyTranslator* Property, void* ArrayPtr)
{
    auto Array = FixSizeArrayTemplate.Get(Isolate)->GetFunction(Context).ToLocalChecked()->NewInstance(Context).ToLocalChecked();
    DataTransfer::SetPointer(Isolate, Array, ArrayPtr, 0);
    DataTransfer::SetPointer(Isolate, Array, Property, 1);
    return Array;
}

void FJsEnvImpl::InvokeDelegateCallback(UDynamicDelegateProxy* Proxy, void* Params)
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogError("Access by illegal thread!");
        return;
    }
#endif
    auto SignatureFunction = Proxy->SignatureFunction;
    auto Iter = JsCallbackPrototypeMap.find(SignatureFunction.Get());
    if (Iter == JsCallbackPrototypeMap.end())
    {
        if (!SignatureFunction.IsValid())
        {
		    m_pLogModule->LogWarn("invalid SignatureFunction!");
            return;
        }
        JsCallbackPrototypeMap[SignatureFunction.Get()] = std::make_unique<FFunctionTranslator>(SignatureFunction.Get(), true);
        Iter = JsCallbackPrototypeMap.find(SignatureFunction.Get());
    }
    else
    {
        if (!SignatureFunction.IsValid())
        {
            JsCallbackPrototypeMap.erase(Iter);
            m_pLogModule->LogWarn("invalid SignatureFunction!");
            return;
        }

        // 非 Editor 模式，函数签名地址可能会变且内存可能复用，不检查可能会访问到旧的非法地址。
        if (!Iter->second->IsValid())
        {
            JsCallbackPrototypeMap[SignatureFunction.Get()] = std::make_unique<FFunctionTranslator>(SignatureFunction.Get(), true);
            Iter = JsCallbackPrototypeMap.find(SignatureFunction.Get());
        }
    }

    auto Isolate = MainIsolate;
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = DefaultContext.Get(Isolate);
    v8::Context::Scope ContextScope(Context);

    v8::TryCatch TryCatch(Isolate);

    Iter->second->CallJs(Isolate, Context, Proxy->JsFunction.Get(Isolate), Context->Global(), Params);

    if (TryCatch.HasCaught())
    {
        Logger->Error(FString::Printf(TEXT("js callback exception %s"), *FV8Utils::TryCatchToString(Isolate, &TryCatch)));
    }
}



void FJsEnvImpl::ExecuteDelegate( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, const v8::FunctionCallbackInfo<v8::Value>& Info, void* DelegatePtr)
{
    auto Iter = DelegateMap.find(DelegatePtr);
    if (Iter == DelegateMap.end())
    {
        FV8Utils::ThrowException(Isolate, "can not find the delegate!");
    }
    auto SignatureFunction = Iter->second.SignatureFunction;
    if (JsCallbackPrototypeMap.find(SignatureFunction) == JsCallbackPrototypeMap.end())
    {
        JsCallbackPrototypeMap[SignatureFunction] = std::make_unique<FFunctionTranslator>(SignatureFunction, true);
    }

    if (Iter->second.DelegateProperty)
    {
        JsCallbackPrototypeMap[SignatureFunction]->Call(Isolate, Context, Info,
            [ScriptDelegate = static_cast<FScriptDelegate*>(DelegatePtr)](void* Params)
            { ScriptDelegate->ProcessDelegate<UObject>(Params); });
    }
    else
    {
        JsCallbackPrototypeMap[SignatureFunction]->Call(Isolate, Context, Info,
            [MulticastScriptDelegate = static_cast<FMulticastScriptDelegate*>(DelegatePtr)](void* Params)
            { MulticastScriptDelegate->ProcessMulticastDelegate<UObject>(Params); });
    }
}

static FName NAME_Fire("Fire");

bool FJsEnvImpl::AddToDelegate( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr, v8::Local<v8::Function> JsFunction)
{
    // UE_LOG(LogTemp, Warning, TEXT("add delegate proxy"));
    auto Iter = DelegateMap.find(DelegatePtr);
    if (Iter == DelegateMap.end())
    {
        return false;
    }
    if (!Iter->second.Owner.IsValid())
    {
        Logger->Warn("try to bind a delegate with invalid owner!");
        ClearDelegate(Isolate, Context, DelegatePtr);
        if (!Iter->second.PassByPointer)
        {
            delete ((FScriptDelegate*) Iter->first);
        }
        DelegateMap.erase(Iter);
        return false;
    }
    if (Iter->second.Proxy.IsValid())
    {
        ClearDelegate(Isolate, Context, DelegatePtr);
    }
    auto JSObject = Iter->second.JSObject.Get(Isolate);
    auto Map = v8::Local<v8::Map>::Cast(JSObject->Get(Context, 0).ToLocalChecked());
    auto MaybeProxy = Map->Get(Context, JsFunction);
    UDynamicDelegateProxy* DelegateProxy = nullptr;
    if (MaybeProxy.IsEmpty() || !MaybeProxy.ToLocalChecked()->IsExternal())
    {
        // UE_LOG(LogTemp, Warning, TEXT("new delegate proxy"));
        DelegateProxy = NewObject<UDynamicDelegateProxy>();
#ifdef THREAD_SAFE
        DelegateProxy->Isolate = Isolate;
#endif
        DelegateProxy->Owner = Iter->second.Owner;
        DelegateProxy->SignatureFunction = Iter->second.SignatureFunction;
        DelegateProxy->DynamicInvoker = DynamicInvoker;
        DelegateProxy->JsFunction = v8::UniquePersistent<v8::Function>(Isolate, JsFunction);

        SysObjectRetainer.Retain(DelegateProxy);
        auto ReturnVal = Map->Set(Context, JsFunction, v8::External::New(Context->GetIsolate(), DelegateProxy));
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("find delegate proxy"));
        DelegateProxy =
            Cast<UDynamicDelegateProxy>(static_cast<UObject*>(v8::Local<v8::External>::Cast(MaybeProxy.ToLocalChecked())->Value()));
    }

    FScriptDelegate Delegate;
    Delegate.BindUFunction(DelegateProxy, NAME_Fire);

    if (Iter->second.DelegateProperty)
    {
        // UE_LOG(LogTemp, Warning, TEXT("bind to delegate"));
        Iter->second.Proxy = DelegateProxy;
        *(static_cast<FScriptDelegate*>(DelegatePtr)) = Delegate;
    }
    else if (Iter->second.MulticastDelegateProperty)
    {
        // UE_LOG(LogTemp, Warning, TEXT("add to multicast delegate, proxy: %p to:%p"), DelegateProxy, DelegatePtr);
        Iter->second.Proxys.Add(DelegateProxy);
#if ENGINE_MINOR_VERSION >= 23 || ENGINE_MAJOR_VERSION > 4
        if (Iter->second.MulticastDelegateProperty->IsA<MulticastSparseDelegatePropertyMacro>())
        {
            Iter->second.MulticastDelegateProperty->AddDelegate(MoveTemp(Delegate), Iter->second.Owner.Get(), DelegatePtr);
        }
        else
#endif
        {
            static_cast<FMulticastScriptDelegate*>(DelegatePtr)->AddUnique(Delegate);
        }
    }
    return true;
}

PropertyMacro* FJsEnvImpl::FindDelegateProperty(void* DelegatePtr)
{
    auto Iter = DelegateMap.find(DelegatePtr);
    if (Iter == DelegateMap.end())
    {
        return nullptr;
    }
    return Iter->second.DelegateProperty ? (PropertyMacro*) Iter->second.DelegateProperty
                                         : (PropertyMacro*) Iter->second.MulticastDelegateProperty;
}

FScriptDelegate FJsEnvImpl::NewManualReleaseDelegate( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, v8::Local<v8::Function> JsFunction, UFunction* SignatureFunction)
{
    auto CallbacksMap = ManualReleaseCallbackMap.Get(Isolate);
    auto MaybeProxy = CallbacksMap->Get(Context, JsFunction);
    UDynamicDelegateProxy* DelegateProxy = nullptr;
    if (MaybeProxy.IsEmpty() || !MaybeProxy.ToLocalChecked()->IsExternal())
    {
        DelegateProxy = NewObject<UDynamicDelegateProxy>();
#ifdef THREAD_SAFE
        DelegateProxy->Isolate = Isolate;
#endif
        DelegateProxy->Owner = DelegateProxy;
        DelegateProxy->SignatureFunction = SignatureFunction;
        DelegateProxy->DynamicInvoker = DynamicInvoker;
        DelegateProxy->JsFunction = v8::UniquePersistent<v8::Function>(Isolate, JsFunction);

        SysObjectRetainer.Retain(DelegateProxy);
        __USE(CallbacksMap->Set(Context, JsFunction, v8::External::New(Context->GetIsolate(), DelegateProxy)));

        ManualReleaseCallbackList.push_back(DelegateProxy);
    }
    else
    {
        DelegateProxy =Cast<UDynamicDelegateProxy>(static_cast<UObject*>(v8::Local<v8::External>::Cast(MaybeProxy.ToLocalChecked())->Value()));
    }

    FScriptDelegate Delegate;
    Delegate.BindUFunction(DelegateProxy, NAME_Fire);
    return Delegate;
}

void FJsEnvImpl::ReleaseManualReleaseDelegate(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgFunction);

    auto CallbacksMap = ManualReleaseCallbackMap.Get(Isolate);
    auto MaybeProxy = CallbacksMap->Get(Context, Info[0]);
    if (!MaybeProxy.IsEmpty() && MaybeProxy.ToLocalChecked()->IsExternal())
    {
        __USE(CallbacksMap->Delete(Context, Info[0]));
        auto DelegateProxy =
            Cast<UDynamicDelegateProxy>(static_cast<UObject*>(v8::Local<v8::External>::Cast(MaybeProxy.ToLocalChecked())->Value()));
        for (auto it = ManualReleaseCallbackList.begin(); it != ManualReleaseCallbackList.end();)
        {
            if (!it->IsValid())
            {
                it = ManualReleaseCallbackList.erase(it);
            }
            else if (it->Get() == DelegateProxy)
            {
                DelegateProxy->JsFunction.Reset();
                it = ManualReleaseCallbackList.erase(it);
                SysObjectRetainer.Release(DelegateProxy);
            }
            else
            {
                ++it;
            }
        }
    }
}

bool FJsEnvImpl::RemoveFromDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr, v8::Local<v8::Function> JsFunction)
{
    auto Iter = DelegateMap.find(DelegatePtr);
    if (Iter == DelegateMap.end())
    {
        return false;
    }

    FScriptDelegate Delegate;

    if (Iter->second.DelegateProperty)
    {
        return ClearDelegate(Isolate, Context, DelegatePtr);
    }
    else if (Iter->second.MulticastDelegateProperty)
    {
        auto JSObject = Iter->second.JSObject.Get(Isolate);
        auto Map = v8::Local<v8::Map>::Cast(JSObject->Get(Context, 0).ToLocalChecked());
        auto MaybeValue = Map->Get(Context, JsFunction);

        if (MaybeValue.IsEmpty())
        {
            return false;
        }

        auto MaybeProxy = MaybeValue.ToLocalChecked();
        if (!MaybeProxy->IsExternal())
        {
            return false;
        }

        auto DelegateProxy = Cast<UDynamicDelegateProxy>(static_cast<UObject*>(v8::Local<v8::External>::Cast(MaybeProxy)->Value()));

        Delegate.BindUFunction(DelegateProxy, NAME_Fire);

#if ENGINE_MINOR_VERSION >= 23 || ENGINE_MAJOR_VERSION > 4
        if (Iter->second.MulticastDelegateProperty->IsA<MulticastSparseDelegatePropertyMacro>())
        {
            Iter->second.MulticastDelegateProperty->RemoveDelegate(Delegate, Iter->second.Owner.Get(), DelegatePtr);
        }
        else
#endif
        {
            static_cast<FMulticastScriptDelegate*>(DelegatePtr)->Remove(Delegate);
        }

        auto ReturnVal = Map->Delete(Context, JsFunction);

        Iter->second.Proxys.Remove(DelegateProxy);
        SysObjectRetainer.Release(DelegateProxy);
        DelegateProxy->JsFunction.Reset();
    }

    return true;
}

bool FJsEnvImpl::ClearDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr)
{
    auto Iter = DelegateMap.find(DelegatePtr);
    if (Iter == DelegateMap.end())
    {
        return false;
    }

    auto JSObject = Iter->second.JSObject.Get(Isolate);
    auto Map = v8::Local<v8::Map>::Cast(JSObject->Get(Context, 0).ToLocalChecked());
    Map->Clear();

    if (Iter->second.DelegateProperty)
    {
        if (Iter->second.Proxy.IsValid())
        {
            if (Iter->second.Owner.IsValid())
            {
                FScriptDelegate Delegate;
                *(static_cast<FScriptDelegate*>(DelegatePtr)) = Delegate;
            }

            SysObjectRetainer.Release(Iter->second.Proxy.Get());
            Iter->second.Proxy->JsFunction.Reset();
            Iter->second.Proxy.Reset();
        }
    }
    else if (Iter->second.MulticastDelegateProperty)
    {
        if (Iter->second.Owner.IsValid())
        {
#if ENGINE_MINOR_VERSION >= 23 || ENGINE_MAJOR_VERSION > 4
            if (Iter->second.MulticastDelegateProperty->IsA<MulticastSparseDelegatePropertyMacro>())
            {
                Iter->second.MulticastDelegateProperty->ClearDelegate(Iter->second.Owner.Get(), DelegatePtr);
            }
            else
#endif
            {
                static_cast<FMulticastScriptDelegate*>(DelegatePtr)->Clear();
            }
        }

        for (auto ProxyIter = Iter->second.Proxys.CreateIterator(); ProxyIter; ++ProxyIter)
        {
            if (!(*ProxyIter).IsValid())
            {
                continue;
            }
            (*ProxyIter)->JsFunction.Reset();
            SysObjectRetainer.Release((*ProxyIter).Get());
        }
        Iter->second.Proxys.Empty();
    }

    return true;
}

bool FJsEnvImpl::CheckDelegateProxies(float Tick)
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogError("Access by illegal thread!");
        return;
    }
#endif
    auto Isolate = MainIsolate;
#ifdef THREAD_SAFE
    v8::Locker Locker(Isolate);
#endif

    std::vector<void*> PendingToRemove;
    for (auto& KV : DelegateMap)
    {
        if (!KV.second.Owner.IsValid())
        {
            PendingToRemove.push_back(KV.first);
        }
    }

    if (PendingToRemove.size() > 0)
    {
        v8::Isolate::Scope IsolateScope(Isolate);
        v8::HandleScope HandleScope(Isolate);
        v8::Local<v8::Context> Context = DefaultContext.Get(Isolate);
        v8::Context::Scope ContextScope(Context);
        for (int i = 0; i < PendingToRemove.size(); ++i)
        {
            ClearDelegate(Isolate, Context, PendingToRemove[i]);
            if (!DelegateMap[PendingToRemove[i]].PassByPointer)
            {
                delete ((FScriptDelegate*) PendingToRemove[i]);
            }
            DelegateMap.erase(PendingToRemove[i]);
        }
    }

    // Collecting invalid function translators to remove.
    std::vector<UFunction*> PendingToRemoveJsCallbacks;
    for (auto& KV : JsCallbackPrototypeMap)
    {
        if ((nullptr == KV.first) || (!KV.second->IsValid()))
        {
            PendingToRemoveJsCallbacks.push_back(KV.first);
        }
    }

    if (PendingToRemoveJsCallbacks.size() > 0)
    {
        for (int32 i = 0; i < PendingToRemoveJsCallbacks.size(); i++)
        {
            JsCallbackPrototypeMap.erase(PendingToRemoveJsCallbacks[i]);
        }
    }

    return true;
}

FPropertyTranslator* FJsEnvImpl::GetContainerPropertyTranslator(PropertyMacro* Property)
{
    auto Iter = ContainerPropertyMap.find(Property);
    // TODO: 如果脚本一直持有蓝图里头的Map，还是有可能有问题的，需要统筹考虑一套机制解决这类问题
    if (Iter == ContainerPropertyMap.end() || !Iter->second.PropertyWeakPtr.IsValid())
    {
        ContainerPropertyInfo Temp{Property, FPropertyTranslator::Create(Property)};
        ContainerPropertyMap[Property] = std::move(Temp);
#if ENGINE_MINOR_VERSION < 25 && ENGINE_MAJOR_VERSION < 5
        if (!Property->IsNative())
        {
            SysObjectRetainer.Retain(Property);
        }
#endif
        return ContainerPropertyMap[Property].PropertyTranslator.get();
    }
    else
    {
        return Iter->second.PropertyTranslator.get();
    }
}

v8::Local<v8::Value> FJsEnvImpl::FindOrAddContainer(v8::Isolate* Isolate, v8::Local<v8::Context>& Context,v8::Local<v8::Function> Constructor, PropertyMacro* Property1, PropertyMacro* Property2, void* Ptr, bool PassByPointer)
{
    check(Ptr);    // must not null

    auto PersistentValuePtr = ContainerCache.Find(Ptr);
    if (PersistentValuePtr)
    {
        return v8::Local<v8::Value>::New(Isolate, *PersistentValuePtr);
    }

    auto BindTo = v8::External::New(Context->GetIsolate(), Ptr);
    v8::Handle<v8::Value> Args[] = {BindTo, v8::Boolean::New(Isolate, PassByPointer)};
    auto Result = Constructor->NewInstance(Context, 2, Args).ToLocalChecked();
    DataTransfer::SetPointer(Isolate, Result, GetContainerPropertyTranslator(Property1), 1);
    if (Property2)
    {
        DataTransfer::SetPointer(Isolate, Result, GetContainerPropertyTranslator(Property2), 2);
    } 
    return Result;
}


void FJsEnvImpl::BindCppObject(v8::Isolate* InIsolate, JSClassDefinition* ClassDefinition, void* Ptr, v8::Local<v8::Object> JSObject, bool PassByPointer)
{
    CppObjectMapper.BindCppObject(InIsolate, ClassDefinition, Ptr, JSObject, PassByPointer);
}



void FJsEnvImpl::UnBindCppObject(JSClassDefinition* ClassDefinition, void* Ptr)
{
    CppObjectMapper.UnBindCppObject(ClassDefinition, Ptr);
}

void FJsEnvImpl::BindContainer(void* Ptr, v8::Local<v8::Object> JSObject, void (*Callback)(const v8::WeakCallbackInfo<void>& data))
{
    DataTransfer::SetPointer(MainIsolate, JSObject, Ptr, 0);
    ContainerCache.Emplace(Ptr, v8::UniquePersistent<v8::Value>(MainIsolate, JSObject));
    ContainerCache[Ptr].SetWeak<void>(nullptr, Callback, v8::WeakCallbackType::kInternalFields);
}

void FJsEnvImpl::UnBindContainer(void* Ptr)
{
    ContainerCache.Remove(Ptr);
}

bool FJsEnvImpl::IsInstanceOfCppObject(const void* TypeId, v8::Local<v8::Object> JsObject)
{
    return CppObjectMapper.IsInstanceOfCppObject(TypeId, JsObject);
}

void FJsEnvImpl::LoadCppType(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    CppObjectMapper.LoadCppType(Info);
}

bool FJsEnvImpl::GetContainerTypeProperty(v8::Local<v8::Context> Context, v8::Local<v8::Value> Value, PropertyMacro** PropertyPtr)
{
    if (Value->IsInt32())
    {
        int Type = Value->Int32Value(Context).ToChecked();
        if (Type >= MaxBuiltinType)
        {
            *PropertyPtr = nullptr;
            return false;
        }
        *PropertyPtr = ContainerMeta.GetBuiltinProperty((BuiltinType) Type);
        return true;
    }
    else if (auto Field = Cast<UField>(FV8Utils::GetUObject(Context, Value)))
    {
        *PropertyPtr = ContainerMeta.GetObjectProperty(Field);
        return *PropertyPtr != nullptr;
    }
    else
    {
        *PropertyPtr = nullptr;
        return false;
    }
}



void FJsEnvImpl::Start(const std::string& ModuleNameOrScript, const std::vector<std::pair<std::string, NFIModule*>>& Arguments, bool IsScript)
{
#ifdef SINGLE_THREAD_VERIFY
    if(BoundThreadId != std::this_thread::get_id())
    {
        m_pLogModule->LogError("Access by illegal thread!");
        return;
    }
#endif
    if (Started)
    {
        Logger->Error("Started yet!");
        return;
    }

    auto Isolate = MainIsolate;
#ifdef THREAD_SAFE
    v8::Locker Locker(Isolate);
#endif
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = v8::Local<v8::Context>::New(Isolate, DefaultContext);
    v8::Context::Scope ContextScope(Context);

    auto MaybeTGameTGJS = Context->Global()->Get(Context, FV8Utils::ToV8String(Isolate, "puerts"));

    if (MaybeTGameTGJS.IsEmpty() || !MaybeTGameTGJS.ToLocalChecked()->IsObject())
    {
        Logger->Error("global.puerts not found!");
        return;
    }

    auto TGJS = MaybeTGameTGJS.ToLocalChecked()->ToObject(Context).ToLocalChecked();

    auto MaybeArgv = TGJS->Get(Context, FV8Utils::ToV8String(Isolate, "argv"));

    if (MaybeArgv.IsEmpty() || !MaybeArgv.ToLocalChecked()->IsObject())
    {
        Logger->Error("global.puerts.argv not found!");
        return;
    }

    auto Argv = MaybeArgv.ToLocalChecked()->ToObject(Context).ToLocalChecked();

    auto MaybeArgvAdd = Argv->Get(Context, FV8Utils::ToV8String(Isolate, "add"));

    if (MaybeArgvAdd.IsEmpty() || !MaybeArgvAdd.ToLocalChecked()->IsFunction())
    {
        Logger->Error("global.puerts.argv.add not found!");
        return;
    }

    auto ArgvAdd = MaybeArgvAdd.ToLocalChecked().As<v8::Function>();

    for (int i = 0; i < Arguments.size(); i++)
    {
        auto Object = Arguments[i].Value;
        v8::Local<v8::Value> Args[2] = {FV8Utils::ToV8String(Isolate, Arguments[i].Key), FindOrAdd(Isolate, Context, Object->GetClass(), Object)};
        auto Result = ArgvAdd->Call(Context, Argv, 2, Args);
    }

    if (IsScript)
    {
        v8::ScriptOrigin Origin(FV8Utils::ToV8String(Isolate, "chunk"));
        v8::Local<v8::String> Source = FV8Utils::ToV8String(Isolate, ModuleNameOrScript);
        v8::TryCatch TryCatch(Isolate);

        auto CompiledScript = v8::Script::Compile(Context, Source, &Origin);
        if (CompiledScript.IsEmpty())
        {
            Logger->Error(FV8Utils::TryCatchToString(Isolate, &TryCatch));
            return;
        }
        auto ReturnVal = CompiledScript.ToLocalChecked()->Run(Context);
        if (TryCatch.HasCaught())
        {
            Logger->Error(FV8Utils::TryCatchToString(Isolate, &TryCatch));
        }
    }
    else
    {
        v8::TryCatch TryCatch(Isolate);
        v8::Local<v8::Value> Args[] = {FV8Utils::ToV8String(Isolate, ModuleNameOrScript)};
        __USE(Require.Get(Isolate)->Call(Context, v8::Undefined(Isolate), 1, Args));
        if (TryCatch.HasCaught())
        {
            Logger->Error(FV8Utils::TryCatchToString(Isolate, &TryCatch));
        }
    }
    Started = true;
}

bool FJsEnvImpl::LoadFile(const std::string& RequiringDir, const std::string& ModuleName, std::string& OutPath, std::string& OutDebugPath,std::string& Data, std::string& ErrInfo)
{
    if (ModuleLoader->Search(RequiringDir, ModuleName, OutPath, OutDebugPath))
    {
        if (!ModuleLoader->Load(OutPath, Data))
        {
            std::ostringstream stream;
	        stream <<"can not load ["<< *ModuleName <<"]";
		    ErrInfo = stream.str();
	        //m_pLogModule->LogInfo(stream.str());
            return false;
        }
    }
    else
    {
        std::ostringstream stream;
	    stream <<"can not load ["<< *ModuleName <<"]";
        ErrInfo = stream.str();
		//ErrInfo = stream.str();
        return false;
    }
    return true;
}

void FJsEnvImpl::ExecuteModule(const std::string& ModuleName, std::function<std::string(const std::string&, const std::string&)> Preprocessor)
{
    std::string OutPath;
    std::string DebugPath;
    std::string Data;

    std::string ErrInfo;
    if (!LoadFile(TEXT(""), ModuleName, OutPath, DebugPath, Data, ErrInfo))
    {
        m_pLogModule->LogError(ErrInfo);
        return;
    }

    auto Isolate = MainIsolate;
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = v8::Local<v8::Context>::New(Isolate, DefaultContext);
    v8::Context::Scope ContextScope(Context);
    
    v8::Local<v8::String> Source;
    if (Preprocessor)
    {
        std::string Script = Data;
        if (Preprocessor)
        {
            Script = Preprocessor(Script, OutPath);
        }
            
        Source = FV8Utils::ToV8String(Isolate, Script);
    }
    else
    {
        Source = FV8Utils::ToV8String(Isolate, Data);
    }
    std::string FormattedScriptUrl = DebugPath;
    v8::Local<v8::String> Name = FV8Utils::ToV8String(Isolate, FormattedScriptUrl);
    v8::ScriptOrigin Origin(Name);
    v8::TryCatch TryCatch(Isolate);

    auto CompiledScript = v8::Script::Compile(Context, Source, &Origin);
    if (CompiledScript.IsEmpty())
    {
        m_pLogModule->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
        return;
    }
    auto ReturnVal = CompiledScript.ToLocalChecked()->Run(Context);
    if (TryCatch.HasCaught())
    {
        m_pLogModule->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
        return;
    }
}

void FJsEnvImpl::EvalScript(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgString, EArgString);

    v8::Local<v8::String> Source = Info[0]->ToString(Context).ToLocalChecked();

    v8::String::Utf8Value UrlArg(Isolate, Info[1]);
    std::string ScriptUrl = std::string(*UrlArg);

    std::string FormattedScriptUrl = ScriptUrl;
    v8::Local<v8::String> Name = FV8Utils::ToV8String(Isolate, FormattedScriptUrl);
    v8::ScriptOrigin Origin(Name);
    auto Script = v8::Script::Compile(Context, Source, &Origin);
    if (Script.IsEmpty())
    {
        return;
    }
    auto Result = Script.ToLocalChecked()->Run(Context);
    if (Result.IsEmpty())
    {
        return;
    }
    Info.GetReturnValue().Set(Result.ToLocalChecked());

    if (OnSourceLoadedCallback)
    {
        OnSourceLoadedCallback(FormattedScriptUrl);
    }
}

void FJsEnvImpl::Log(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgInt32, EArgString);

    auto Level = Info[0]->Int32Value(Context).ToChecked();

    std::string Msg = FV8Utils::ToFString(Isolate, Info[1]);
    switch (Level)
    {
        case 1:
            m_pLogModule->LogInfo(Msg);
            break;
        case 2:
            m_pLogModule->LogWarn(Msg);
            break;
        case 3:
            m_pLogModule->LogError(Msg);
            break;
        default:
            m_pLogModule->LogInfo(Msg);
            break;
    }
}

void FJsEnvImpl::SearchModule(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();

    CHECK_V8_ARGS(EArgString, EArgString);

    std::string ModuleName = FV8Utils::ToFString(Isolate, Info[0]);
    std::string RequiringDir = FV8Utils::ToFString(Isolate, Info[1]);
    std::string OutPath;
    std::string OutDebugPath;

    if (ModuleLoader->Search(RequiringDir, ModuleName, OutPath, OutDebugPath))
    {
        auto Result = v8::Array::New(Isolate);
        Result->Set(Context, 0, FV8Utils::ToV8String(Isolate, OutPath)).Check();
        Result->Set(Context, 1, FV8Utils::ToV8String(Isolate, OutDebugPath)).Check();
        Info.GetReturnValue().Set(Result);
    }
}

void FJsEnvImpl::LoadModule(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgString);

    std::string Path = FV8Utils::ToFString(Isolate, Info[0]);
    std::string Data;
    if (!ModuleLoader->Load(Path, Data))
    {
        FV8Utils::ThrowException(Isolate, "can not load module");
        return;
    }

    Info.GetReturnValue().Set(FV8Utils::ToV8String(Isolate, Data));
}

void FJsEnvImpl::SetTimeout(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgFunction, EArgNumber);

    SetFTickerDelegate(Info, false);
}

void FJsEnvImpl::SetFTickerDelegate(const v8::FunctionCallbackInfo<v8::Value>& Info, bool Continue)
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    std::function<void(const JSError*, std::shared_ptr<ILogger>&)> ExceptionLog =
        [](const JSError* Exception, std::shared_ptr<ILogger>& InLogger)
    {
        FString Message = FString::Printf(TEXT("JS Execution Exception: %s"), *(Exception->Message));
        InLogger->Warn(Message);
    };
    std::function<void(const JSError*)> ExceptionLogWrapper = std::bind(ExceptionLog, _1, Logger);
    std::function<void(v8::Isolate*, v8::TryCatch*)> ExecutionExceptionHandler =
        std::bind(&FJsEnvImpl::ReportExecutionException, this, _1, _2, ExceptionLogWrapper);
    std::function<void(FDelegateHandle*)> DelegateHandleCleaner = std::bind(&FJsEnvImpl::RemoveFTickerDelegateHandle, this, _1);

    FTickerDelegateWrapper* DelegateWrapper = new FTickerDelegateWrapper(Continue);
    DelegateWrapper->Init(Info, ExecutionExceptionHandler, DelegateHandleCleaner);
#ifdef SINGLE_THREAD_VERIFY
    DelegateWrapper->BoundThreadId = BoundThreadId;
#endif
    FTickerDelegate Delegate = FTickerDelegate::CreateRaw(DelegateWrapper, &FTickerDelegateWrapper::CallFunction);

    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    float Millisecond = Info[1]->NumberValue(Context).ToChecked();
    float Delay = Millisecond / 1000.f;

    // TODO - 如果实现多线程，这里应该加锁阻止定时回调的执行，直到DelegateWrapper设置好handle
    FDelegateHandle* DelegateHandle = new FDelegateHandle(FTicker::GetCoreTicker().AddTicker(Delegate, Delay));
    DelegateWrapper->SetDelegateHandle(DelegateHandle);
    TickerDelegateHandleMap[DelegateHandle] = DelegateWrapper;

    Info.GetReturnValue().Set(v8::External::New(Info.GetIsolate(), DelegateHandle));
}

void FJsEnvImpl::ReportExecutionException(
    v8::Isolate* Isolate, v8::TryCatch* TryCatch, std::function<void(const JSError*)> CompletionHandler)
{
    const JSError Error(FV8Utils::TryCatchToString(Isolate, TryCatch));
    if (CompletionHandler)
    {
        CompletionHandler(&Error);
    }
}

void FJsEnvImpl::RemoveFTickerDelegateHandle(FDelegateHandle* Handle)
{
    // TODO - 如果实现多线程，FTicker所在主线程和当前线程释放handle可能有竞争
    auto Iterator = std::find_if(
        TickerDelegateHandleMap.begin(), TickerDelegateHandleMap.end(), [&](auto& Pair) { return Pair.first == Handle; });
    if (Iterator != TickerDelegateHandleMap.end())
    {
        // call clearTimeout in setTimeout callback
        if (Iterator->second->IsCalling)
        {
            Iterator->second->FunctionContinue = false;
            return;
        }
        FTicker::GetCoreTicker().RemoveTicker(*(Iterator->first));
        delete Iterator->first;
        delete Iterator->second;
        TickerDelegateHandleMap.erase(Iterator);
    }
}

void FJsEnvImpl::ClearInterval(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    // todo - mocha 7.0.1，当reporter为JSON，调用clearTimeout时，可能不传值，或传Null、Undefined过来。暂将其忽略
    if (Info.Length() == 0)
    {
        Logger->Warn(TEXT("Calling ClearInterval with 0 argument."));
    }
    else if (Info[0]->IsNullOrUndefined())
    {
        // 屏蔽这条只在mocha中出现的警告
        // Logger->Warn(TEXT("Calling ClearInterval with a Null or Undefined."));
    }
    else
    {
        CHECK_V8_ARGS(EArgExternal);
        v8::Local<v8::External> Arg = v8::Local<v8::External>::Cast(Info[0]);
        FDelegateHandle* Handle = static_cast<FDelegateHandle*>(Arg->Value());
        RemoveFTickerDelegateHandle(Handle);
    }
}

void FJsEnvImpl::SetInterval(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgFunction, EArgNumber);

    SetFTickerDelegate(Info, true);
}


void FJsEnvImpl::FindModule(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope Isolatescope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgString);

    std::string Name = *(v8::String::Utf8Value(Isolate, Info[0]));

    auto Func = FindAddonRegisterFunc(Name);

    if (Func)
    {
        auto Exports = v8::Object::New(Isolate);
        Func(Context, Exports);
        Info.GetReturnValue().Set(Exports);
    }
}

}