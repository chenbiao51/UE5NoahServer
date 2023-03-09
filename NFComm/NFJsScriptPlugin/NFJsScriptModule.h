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
#include "JsEnv.h"
#include "DynamicDelegateProxy.h"
#include "StructWrapper.h"
#include "CppObjectMapper.h"
#include "V8Utils.h"

#include "ObjectMapper.h"
#include "NFILogModule.h"
#include "TickerDelegateWrapper.h"
#include "ContainerMeta.h"
#include "ObjectCacheNode.h"
#include <unordered_map>



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

class NFIJsScriptModule : public NFIModule
{
public:

};

class NFJsScriptModule : public NFIJsScriptModule
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


protected:
	void RegisterModule(const std::string& tableName, const LuaIntf::LuaRef& luaTable);

	//FOR KERNEL MODULE
	NFGUID CreateObject(const NFGUID& self, const int sceneID, const int groupID, const std::string& className, const std::string& objectIndex, const NFDataList& arg);
	bool ExistObject(const NFGUID& self);
	bool DestroyObject(const NFGUID & self);

	//return the group id
	bool EnterScene(const int sceneID, const int groupID);
	bool DoEvent(const NFGUID& self, const int eventID, const NFDataList& arg);

	bool FindProperty(const NFGUID& self, const std::string& propertyName);

	bool SetPropertyInt(const NFGUID& self, const std::string& propertyName, const NFINT64 propValue);
	bool SetPropertyFloat(const NFGUID& self, const std::string& propertyName, const double propValue);
	bool SetPropertyString(const NFGUID& self, const std::string& propertyName, const std::string& propValue);
	bool SetPropertyObject(const NFGUID& self, const std::string& propertyName, const NFGUID& propValue);
	bool SetPropertyVector2(const NFGUID& self, const std::string& propertyName, const NFVector2& propValue);
	bool SetPropertyVector3(const NFGUID& self, const std::string& propertyName, const NFVector3& propValue);

	NFINT64 GetPropertyInt(const NFGUID& self, const std::string& propertyName);
	int GetPropertyInt32(const NFGUID& self, const std::string& propertyName);	//equal to (int)GetPropertyInt(...), to remove C4244 warning
	double GetPropertyFloat(const NFGUID& self, const std::string& propertyName);
	std::string GetPropertyString(const NFGUID& self, const std::string& propertyName);
	NFGUID GetPropertyObject(const NFGUID& self, const std::string& propertyName);
	NFVector2 GetPropertyVector2(const NFGUID& self, const std::string& propertyName);
	NFVector3 GetPropertyVector3(const NFGUID& self, const std::string& propertyName);

	bool AddPropertyCallBack(const NFGUID& self, std::string& propertyName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);
    bool AddRecordCallBack(const NFGUID& self, std::string& recordName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);
    bool AddEventCallBack(const NFGUID& self, const int eventID, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);
	bool AddSchedule(const NFGUID& self, std::string& strHeartBeatName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc, const float time, const int count);
	bool AddModuleSchedule(std::string& strHeartBeatName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc, const float time, const int count);

	int AddRow(const NFGUID& self, std::string& recordName, const NFDataList& var);
	bool RemRow(const NFGUID& self, std::string& recordName, const int row);

	bool SetRecordInt(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag, const NFINT64 value);
	bool SetRecordFloat(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag, const double value);
	bool SetRecordString(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag, const std::string& value);
	bool SetRecordObject(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag, const NFGUID& value);
	bool SetRecordVector2(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag, const NFVector2& value);
	bool SetRecordVector3(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag, const NFVector3& value);

	NFINT64 GetRecordInt(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag);
	double GetRecordFloat(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag);
	std::string GetRecordString(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag);
	NFGUID GetRecordObject(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag);
	NFVector2 GetRecordVector2(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag);
	NFVector3 GetRecordVector3(const NFGUID& self, const std::string& recordName, const int row, const std::string& colTag);

	NFINT64 GetNowTime();
	NFGUID CreateId();
	NFINT64 APPId();
	NFINT64 APPType();

	//FOR ELEMENT MODULE
	bool ExistElementObject(const std::string& configName);
	std::vector<std::string> GetEleList(const std::string& className);

	NFINT64 GetElePropertyInt(const std::string& configName, const std::string& propertyName);
	double GetElePropertyFloat(const std::string& configName, const std::string& propertyName);
	std::string GetElePropertyString(const std::string& configName, const std::string& propertyName);
	NFVector2 GetElePropertyVector2(const std::string& configName, const std::string& propertyName);
	NFVector3 GetElePropertyVector3(const std::string& configName, const std::string& propertyName);

	//FOR NET MODULE
	//as server
	void RemoveCallBackAsServer(const int msgID);
	void AddMsgCallBackAsServer(const int msgID, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);

	//as client
	void RemoveMsgCallBackAsClient(const NF_SERVER_TYPES serverType, const int msgID);
	void AddMsgCallBackAsClient(const NF_SERVER_TYPES serverType, const int msgID, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);

/*
	void RemoveHttpCallBack(const std::string& path);
	void AddHttpCallBack(const std::string& path, const int httpType, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);
*/

    void ImportProtoFile(const std::string& fileName);
    const std::string Encode(const std::string& msgTypeName, const LuaIntf::LuaRef& luaTable);
	LuaIntf::LuaRef Decode(const std::string& msgTypeName, const std::string& data);

	void SendToServerByServerID(const int serverID, const uint16_t msgID, const std::string& data);
	void SendToServerBySuit(const NF_SERVER_TYPES eType, const uint16_t msgID, const std::string& data, const std::string& hash);
	void SendToAllServerByServerType(const NF_SERVER_TYPES eType, const uint16_t msgID, const std::string& data);

    //for net module
	void SendMsgToClientByFD(const NFSOCK fd, const uint16_t msgID, const std::string& data);

	void SendMsgToPlayer(const NFGUID player, const uint16_t msgID, const std::string& data);
	void SendToAllPlayer(const uint16_t msgID, const std::string& data);
    void SendToGroupPlayer(const uint16_t msgID, const std::string& data);

	//for log
	void LogInfo(const std::string& logData);
	void LogError(const std::string& logData);
	void LogWarning(const std::string& logData);
	void LogDebug(const std::string& logData);

    //hot fix
	void SetVersionCode(const std::string& logData);
	const std::string& GetVersionCode();

	//FOR CLASS MODULE
    bool AddClassCallBack(std::string& className, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);

protected:
    template<typename T>
    bool AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, const NFGUID& self, T key, std::string& luaFunc);

    template<typename T>
    bool AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, T key, std::string& luaFunc);

    int OnLuaPropertyCB(const NFGUID& self, const std::string& propertyName, const NFData& oldVar, const NFData& newVar, const NFINT64 reason);
    int OnLuaRecordCB(const NFGUID& self, const RECORD_EVENT_DATA& eventData, const NFData& oldVar, const NFData& newVar);
    int OnLuaHeartBeatCB(const NFGUID& self, const std::string& strHeartBeatName, const float time, const int count);

    int OnLuaEventCB(const NFGUID& self, const int eventID, const NFDataList& argVar);

    int OnClassEventCB(const NFGUID& self, const std::string& className, const CLASS_OBJECT_EVENT classEvent, const NFDataList& var);
    
	void OnScriptReload();

	void OnNetMsgCallBackAsServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len);
	void OnNetMsgCallBackAsClientForMasterServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len);
	void OnNetMsgCallBackAsClientForWorldServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len);
	void OnNetMsgCallBackAsClientForGameServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len);

protected:
    bool Register();
	std::string FindFuncName(const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc);


protected:
    NFIElementModule* m_pElementModule;
    NFIKernelModule* m_pKernelModule;
    NFIClassModule* m_pClassModule;
	NFIEventModule* m_pEventModule;
    NFIScheduleModule* m_pScheduleModule;
    NFINetClientModule* m_pNetClientModule;
    NFINetModule* m_pNetModule;
    NFILogModule* m_pLogModule;
	NFIJsPBModule* m_pJsPBModule;
	
protected:
    int64_t mnTime;
    std::string strVersionCode;
    LuaIntf::LuaContext mLuaContext;

	std::map<std::string, LuaIntf::LuaRef> mxTableName;

    NFMap<std::string, NFMap<NFGUID, NFList<std::string>>> mxLuaPropertyCallBackFuncMap;
    NFMap<std::string, NFMap<NFGUID, NFList<std::string>>> mxLuaRecordCallBackFuncMap;
    NFMap<int, NFMap<NFGUID, NFList<std::string>>> mxLuaEventCallBackFuncMap;
    NFMap<std::string, NFMap<NFGUID, NFList<std::string>>> mxLuaHeartBeatCallBackFuncMap;

    NFMap<std::string, NFList<std::string>> mxClassEventFuncMap;


	NFMap<int, NFList<std::string>> mxNetMsgCallBackFuncMapAsServer;
	NFMap<NF_SERVER_TYPES, NFMap<int, NFList<std::string>>> mxNetMsgCallBackFuncMapAsClient;


public:
    explicit FJsEnvImpl(const std::string& NFDataCfgPath ,const std::string& ScriptRoot);

    FJsEnvImpl(std::shared_ptr<IJSModuleLoader> InModuleLoader, std::shared_ptr<NFILogModule> InLogger, int InPort,  std::function<void(const std::string&)> InOnSourceLoadedCallback, void* InExternalRuntime = nullptr, void* InExternalContext = nullptr);

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

private:
    puerts::FObjectRetainer UserObjectRetainer;

    puerts::FObjectRetainer SysObjectRetainer;

    std::shared_ptr<IJSModuleLoader> ModuleLoader;

    std::shared_ptr<NFILogModule> Logger;

    bool Started;

private:

    uv_loop_t NodeUVLoop;

    node::IsolateData* NodeIsolateData;

    node::Environment* NodeEnv;

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


    typedef void (FJsEnvImpl::*V8MethodCallback)(const v8::FunctionCallbackInfo<v8::Value>& Info);

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

#endif
