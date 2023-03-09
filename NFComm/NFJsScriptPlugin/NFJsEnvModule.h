#ifndef NF_JS_ENV_MODULE_H
#define NF_JS_ENV_MODULE_H

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

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include "Dependencies/LuaIntf/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NFILogModule.h"


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


class NFIJsEnvModule : public NFIModule
{

};

class NFJsEnvModule : public NFIJsEnvModule,ICppObjectMapper
{
public:
    NFJsEnvModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

	virtual bool Awake();
	virtual bool Init();
    virtual bool Shut();
	virtual bool ReadyExecute();
	virtual bool Execute();

    virtual bool AfterInit();
    virtual bool BeforeShut();

	virtual void ImportProtoFile(const std::string& strFile);

protected:
	void SetLuaState(lua_State* pState);
	void PrintMessage(const google::protobuf::Message& messag, const bool bEncode);

	LuaIntf::LuaRef Decode(const std::string& strMsgTypeName, const std::string& strData);
	const std::string Encode(const std::string& strMsgTypeName, const LuaIntf::LuaRef& luaTable);

	friend class NFLuaScriptModule;

private:
	LuaIntf::LuaRef MessageToTbl(const google::protobuf::Message& message) const;

	LuaIntf::LuaRef GetField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field) const;
	LuaIntf::LuaRef GetRepeatedField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field) const;
	LuaIntf::LuaRef GetRepeatedFieldElement(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, int index) const;


	///////////////
	const bool TblToMessage(const LuaIntf::LuaRef& luaTable, google::protobuf::Message& message);
	
	void SetField(google::protobuf::Message& message, const std::string& sField, const LuaIntf::LuaRef& luaValue);
	void SetRepeatedField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const LuaIntf::LuaRef& luaTable);
	void SetRepeatedMapField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const LuaIntf::LuaRef& luaTable);
	void AddToRepeatedField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const LuaIntf::LuaRef& luaValue);
	void AddToMapField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const LuaIntf::LuaRef& key, const LuaIntf::LuaRef& val);
	int GetEnumValue(google::protobuf::Message& message, const LuaIntf::LuaRef& luaValue, const google::protobuf::FieldDescriptor* field) const;

protected:
	NFILogModule* m_pLogModule;

    int64_t mnTime;
    std::string strVersionCode;
	lua_State* m_pLuaState;

	NFMultiFileErrorCollector mErrorCollector;
	google::protobuf::compiler::DiskSourceTree mSourceTree;
	google::protobuf::compiler::Importer* m_pImporter;
	google::protobuf::DynamicMessageFactory* m_pFactory;


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

#endif