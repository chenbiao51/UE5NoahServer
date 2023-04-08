#include "V8Utils.h"
#include "JSClassRegister.h"
#include "CppObjectMapper.h"
#include "NFComm/NFCore/NFVector2.hpp"
#include "NFComm/NFPluginModule/NFIKernelModule.h"

using namespace puerts;

static void *_NFDataNew_(const v8::FunctionCallbackInfo<v8::Value> &Info)
{
    v8::Isolate *Isolate = Info.GetIsolate();
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    if (Info.Length() == 0)
    {
        NFData *Obj = new NFData();
        return Obj;
    }

    if (Info.Length() == 1)
    {
        if (Info[0]->IsNumber())
        {
            NFINT64 Arg0 = Info[0]->ToNumber(Context).ToLocalChecked()->Value();
            NFData *Obj = new NFData(Arg0);
            return Obj;
        }
        else if (Info[0]->IsBoolean())
        {
            bool Arg0 = Info[0]->ToBoolean(Isolate)->Value();
            NFData *Obj = new NFData(Arg0);
            return Obj;
        }
        else if (Info[0]->IsObject())
        {
            if (puerts::DataTransfer::IsInstanceOf(Isolate, &typeid(NFVector2), Info[0]->ToObject(Context).ToLocalChecked()))
            {
                const NFVector2* Arg0 = puerts::DataTransfer::GetPointerFast<NFVector2>(Info[0]->ToObject(Context).ToLocalChecked());
                NFData *Obj = new NFData(Arg0);
                return Obj;
            }
            else if (puerts::DataTransfer::IsInstanceOf(Isolate, &typeid(NFVector3), Info[0]->ToObject(Context).ToLocalChecked()))
            {
                const NFVector3* Arg0 = puerts::DataTransfer::GetPointerFast<NFVector3>(Info[0]->ToObject(Context).ToLocalChecked());
                NFData *Obj = new NFData(Arg0);
                return Obj;
            }
            else if(puerts::DataTransfer::IsInstanceOf<NFGUID>(Isolate, Info[0]->ToObject(Context).ToLocalChecked()))
            {
                const NFGUID* Arg0 = puerts::DataTransfer::GetPointerFast<NFGUID>(Info[0]->ToObject(Context).ToLocalChecked());
                NFData *Obj = new NFData(Arg0);
                return Obj;
            }
            else if(puerts::DataTransfer::IsInstanceOf<NFData>(Isolate, Info[0]->ToObject(Context).ToLocalChecked()))
            {
                const NFData* Arg0 = puerts::DataTransfer::GetPointerFast<NFData>(Info[0]->ToObject(Context).ToLocalChecked());
                NFData *Obj = new NFData(Arg0);
                return Obj;
            }
        }
    }

    FV8Utils::ThrowException(Isolate, "Invalid argument!");
    return nullptr;
}

static void _NFDataDelete_(void *Ptr)
{
    NFData* Self = static_cast<NFData*>(Ptr);
    delete Self;
}

struct AutoRegisterForNFData
{
    AutoRegisterForNFData()
    {
        puerts::JSClassDefinition Def = JSClassEmptyDefinition;

        static puerts::JSPropertyInfo Properties[] = {};
        static puerts::JSFunctionInfo Methods[] = {};
        static puerts::JSFunctionInfo Functions[] = {};

        Def.UETypeName = "NFData";
        Def.Initialize = _NFDataNew_;
        Def.Finalize = _NFDataDelete_;
        Def.Properties = Properties;
        Def.Methods = Methods;
        Def.Functions = Functions;
        puerts::RegisterJSClass(Def);
    }
};

AutoRegisterForNFData _AutoRegisterForNFData_;