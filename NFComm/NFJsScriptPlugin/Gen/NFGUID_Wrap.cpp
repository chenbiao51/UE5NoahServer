
#include "V8Utils.h"
#include "JSClassRegister.h"
#include "CppObjectMapper.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"

using namespace puerts;

static void * _NFGUIDNew_(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    if(Info.Length()==0)
    {
        NFGUID* Obj = new NFGUID();
        return Obj;
    }

    if(Info.Length()==2)
    {
        if(Info[0]->IsNumber() && Info[1]->IsNumber())
        {
            NFINT64 Arg0 = Info[0]->ToNumber(Context).ToLocalChecked()->Value();
            NFINT64 Arg1 = Info[1]->ToNumber(Context).ToLocalChecked()->Value();
            NFGUID* Obj = new NFGUID(Arg0,Arg1);
            return Obj;
        }
    }

    FV8Utils::ThrowException(Isolate,"Invalid argument!");
    return nullptr;

}

static void _NFGUIDDelete_(void * Ptr)
{
    NFGUID* Self = static_cast<NFGUID*>(Ptr);
    delete Self;
}

static void _NFGUIDnData64Get_(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();

    auto Self = puerts::DataTransfer::GetPointerFast<NFGUID>(Info.Holder());
    if(!Self){
        FV8Utils::ThrowException(Isolate,"[NFGUID::nData64] Attempt to access aNULL self pointer");
        return;
    }

    auto V8Result = v8::Number::New(Isolate,Self->nData64);
    puerts::DataTransfer::LinkOuter<NFGUID,NFINT64>(Context,Info.Holder(),V8Result);
    Info.GetReturnValue().Set(V8Result);
}

static void _NFGUIDnData64Set_(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();

    auto Self = puerts::DataTransfer::GetPointerFast<NFGUID>(Info.Holder());
    if(!Self){
        FV8Utils::ThrowException(Isolate,"[NFGUID::nData64] Attempt to access aNULL self pointer");
        return;
    }

    auto Value = Info[0];
    Self->nData64 = Value->ToNumber(Context).ToLocalChecked()->Value();
}

static void _NFGUIDnHead64Get_(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();

    auto Self = puerts::DataTransfer::GetPointerFast<NFGUID>(Info.Holder());
    if(!Self){
        FV8Utils::ThrowException(Isolate,"[NFGUID::nData64] Attempt to access aNULL self pointer");
        return;
    }

    auto V8Result = v8::Number::New(Isolate,Self->nHead64);
    puerts::DataTransfer::LinkOuter<NFGUID,NFINT64>(Context,Info.Holder(),V8Result);
    Info.GetReturnValue().Set(V8Result);
}

static void _NFGUIDnHead64Set_(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();

    auto Self = puerts::DataTransfer::GetPointerFast<NFGUID>(Info.Holder());
    if(!Self){
        FV8Utils::ThrowException(Isolate,"[NFGUID::nHead64] Attempt to access aNULL self pointer");
        return;
    }

    auto Value = Info[0];
    Self->nHead64 = Value->ToNumber(Context).ToLocalChecked()->Value();
}



struct AutoRegisterForNFGUID
{
    AutoRegisterForNFGUID()
    {
        puerts::JSClassDefinition Def = JSClassEmptyDefinition;

        static puerts::JSPropertyInfo Properties[] = {
            {"nData64",_NFGUIDnData64Get_,_NFGUIDnData64Set_},
            {"nHead64",_NFGUIDnHead64Get_,_NFGUIDnHead64Set_},
            {0,0,0}
        };
        static puerts::JSFunctionInfo Methods[] = {};
        static puerts::JSFunctionInfo Functions[] = {};

        Def.UETypeName = "NFGUID";
        Def.Initialize = _NFGUIDNew_;
        Def.Finalize = _NFGUIDDelete_;
        Def.Properties = Properties;
        Def.Methods = Methods;
        Def.Functions = Functions;
        puerts::RegisterJSClass(Def);
    }
};


AutoRegisterForNFGUID _AutoRegisterForNFGUID_;