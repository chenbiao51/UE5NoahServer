/*
            This file is part of: 
                NoahFrame
            https://github.com/ketoo/NoahGameFrame

   Copyright 2009 - 2021 NoahFrame(NoahGameFrame)

   File creator: lvsheng.huang
   
   NoahFrame is open-source software and you can redistribute it and/or modify
   it under the terms of the License; besides, anyone who use this file/software must include this copyright announcement.

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


#ifndef NF_TS_SCRIPT_MODULE_H
#define NF_TS_SCRIPT_MODULE_H

#ifdef min
#undef min
#endif

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include "CppObjectMapper.h"
#include "V8Utils.h"
#include "JSModuleLoader.h"
#include "ObjectMapper.h"
#include "NFILogModule.h"
#include "ObjectCacheNode.h"
#include <unordered_map>
#include "PromiseRejectCallback.hpp"


#pragma warning(push, 0)
#include "libplatform/libplatform.h"
#include "v8.h"
#pragma warning(pop)

#include "V8InspectorImpl.h"

#pragma warning(push, 0)
#include "node.h"
#include "uv.h"
#pragma warning(pop)

#include "NFComm/NFPluginModule/NFINetModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIEventModule.h"
#include "NFComm/NFPluginModule/NFIScheduleModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFINetClientModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFJsPBModule.h"

namespace puerts
{

class JSError
{
public:
    std::string Message;

    JSError()
    {
    }
    explicit JSError(const std::string& m):Message(m)
    {
    }
};


class NFIJsScriptModule : public NFIModule
{
public:

};

class NFJsScriptModule : public NFIJsScriptModule,puerts::ICppObjectMapper
{
public:
    NFJsScriptModule(NFIPluginManager* p)
    {
        m_bIsExecute = true;
        pPluginManager = p;
    }

	virtual bool Awake();
	virtual bool Init();
    virtual bool Shut();
	virtual bool ReadyExecute();
	virtual bool Execute();

    virtual bool AfterInit();
    virtual bool BeforeShut();

private:

    struct  TickDelegateInfo
    {
        v8::Isolate *Isolate;
        v8::Global<v8::Context> DefaultContext;
        v8::Global<v8::Function> DefaultFunction;
        std::function<void(v8::Isolate*,v8::TryCatch*)> exceptionHandler;
        std::function<void(NFGUID*)> DelegateHandleCleaner;
        bool FunctionContinue;
        bool IsCalling;

        TickDelegateInfo(){
        }
    };
    std::map<NFGUID*,TickDelegateInfo*> TickerDelegateHandleMap; 

    struct EventInfo
    {
        int EventId ;
        v8::Isolate *Isolate;
        v8::Global<v8::Context> DefaultContext;
        v8::Global<v8::Function> DefaultFunction;
        std::function<void(v8::Isolate*,v8::TryCatch*)> exceptionHandler;
        std::function<void(NFGUID*)> DelegateHandleCleaner;
        bool IsCalling;
        EventInfo(){
        }
    };
protected:



	//return the group id
    EventInfo* FindDelegate(const NFGUID& self);
	bool EnterScene(const int sceneID, const int groupID);

	// bool DoEvent(const NFGUID& self, const int eventID, const NFDataList& arg);
    // bool AddEventCallBack(const NFGUID& self, const int eventID, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);
    // int OnEventCB(const NFGUID& self, const int eventID, const NFDataList& argVar);


	NFINT64 GetNowTime();
	NFGUID CreateId();
	NFINT64 APPId();
	NFINT64 APPType();






    void ImportProtoFile(const std::string& fileName);
    const std::string Encode(const std::string& msgTypeName, const v8::Local<v8::Value>& v8Value);
	v8::Local<v8::Value> Decode(const std::string& msgTypeName, const std::string& data);

    //hot fix
	void SetVersionCode(const std::string& logData);
	const std::string& GetVersionCode();


protected:
    template<typename T>
    bool AddEventInfoToMap(NFMap<T, NFMap<NFGUID, NFList<EventInfo*>>>& funcMap, const NFGUID& self, T key, EventInfo& eventInfo);

   
    int OnTimeOutHeartBeatCB(const NFGUID& self, const std::string& strHeartBeatName, const float time, const int count);
    

protected:
    NFIElementModule* m_pElementModule;
    NFIKernelModule* m_pKernelModule;
    NFIClassModule* m_pClassModule;
	NFIEventModule* m_pEventModule;
    NFIScheduleModule* m_pScheduleModule;
    NFINetClientModule* m_pNetClientModule;
    NFINetModule* m_pNetModule;
    std::shared_ptr<NFILogModule> m_pLogModule;
	NFIJsPBModule* m_pJsPBModule;
	
protected:
    int64_t mnTime;
    std::string strVersionCode;

    NFMap<int, NFMap<NFGUID, NFList<EventInfo*>>> mxJsEventCallBackFuncMap;
public:
    explicit NFJsScriptModule(const std::string& NFDataCfgPath ,const std::string& ScriptRoot);

    NFJsScriptModule(std::shared_ptr<puerts::IJSModuleLoader> InModuleLoader,int InPort,  std::function<void(const std::string&)> InOnSourceLoadedCallback, void* InExternalRuntime = nullptr, void* InExternalContext = nullptr);

    virtual ~NFJsScriptModule() override;

    void Start(const std::string& ModuleNameOrScript,  bool IsScript);

    bool IdleNotificationDeadline(double DeadlineInSeconds);

    void LowMemoryNotification();

    void RequestMinorGarbageCollectionForTesting() ;

    void RequestFullGarbageCollectionForTesting() ;

    void WaitDebugger(int64_t timeout) 
    {
#ifdef THREAD_SAFE
        v8::Locker Locker(MainIsolate);
#endif
        const auto startTime = NFGetTimeS();
        while (Inspector && !Inspector->Tick())
        {
            if (timeout > 0)
            {
                auto now = NFGetTimeS();
                if ((now - startTime) >= timeout)
                {
                    break;
                }
            }
        }
    }



    std::string CurrentStackTrace() ;

    void JsHotReload(std::string ModuleName, const std::string& JsSource);

    void ReloadModule(std::string ModuleName, const std::string& JsSource);

    void ReloadSource(const std::string& Path, const std::string& JsSource) ;

    std::function<void(const std::string&)> OnSourceLoadedCallback;

    void OnSourceLoaded(std::function<void(const std::string&)> Callback);

public:
    virtual void UnBindCppObject(JSClassDefinition* ClassDefinition, void* Ptr) override;

    virtual void BindCppObject(v8::Isolate* InIsolate, JSClassDefinition* ClassDefinition, void* Ptr, v8::Local<v8::Object> JSObject, bool PassByPointer) override;

    virtual v8::Local<v8::Value> FindOrAddCppObject( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, const void* TypeId, void* Ptr, bool PassByPointer) override;

    virtual bool IsInstanceOfCppObject(const void* TypeId, v8::Local<v8::Object> JsObject) override;


    v8::UniquePersistent<v8::Function> JsPromiseRejectCallback;

    V8_INLINE static NFJsScriptModule* Get(v8::Isolate* Isolate)
    {
        return static_cast<NFJsScriptModule*>(puerts::FV8Utils::IsolateData<ICppObjectMapper>(Isolate));
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

    void SetTimeout(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void SetFTickerDelegate(const v8::FunctionCallbackInfo<v8::Value>& Info, bool Continue);

    void ReportExecutionException( v8::Isolate* Isolate, v8::TryCatch* TryCatch, std::function<void(const JSError*)> CompletionHandler);

    void RemoveFTickerDelegateHandle(NFGUID* Handle);

    void SetInterval(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void ClearInterval(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void FindModule(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void DumpStatisticsLog(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void SetInspectorCallback(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void DispatchProtocolMessage(const v8::FunctionCallbackInfo<v8::Value>& Info);

    void DoEvent(const v8::FunctionCallbackInfo<v8::Value>& Info);
    void AddEventCallBack(const v8::FunctionCallbackInfo<v8::Value>& Info);
    void RemoveEventCallBack(const v8::FunctionCallbackInfo<v8::Value>& Info);
    int OnEventCB(const NFGUID& self, const int eventID, const NFDataList& argVar);
public:

private:

    std::shared_ptr<puerts::IJSModuleLoader> ModuleLoader;

    
    bool Started;

private:

    uv_loop_t NodeUVLoop;

    node::IsolateData* NodeIsolateData;

    node::Environment* NodeEnv;

    v8::Isolate* MainIsolate;

    v8::Global<v8::Context> DefaultContext;

    v8::Global<v8::Function> Require;

    v8::Global<v8::Function> ReloadJs;

    puerts::FCppObjectMapper CppObjectMapper;
    

    
    
    


    bool ExtensionMethodsMapInited = false;

    V8Inspector* Inspector;

    V8InspectorChannel* InspectorChannel;

    v8::Global<v8::Function> InspectorMessageHandler;

    v8::Global<v8::Map> ManualReleaseCallbackMap;

    typedef void (NFJsScriptModule::*V8MethodCallback)(const v8::FunctionCallbackInfo<v8::Value>& Info);

    template <V8MethodCallback callback>
    struct MethodBindingHelper
    {
        static void Bind(v8::Isolate* Isolate, v8::Local<v8::Context> Context, v8::Local<v8::Object> Obj, const char* Key,v8::Local<v8::External> This)
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
#endif

