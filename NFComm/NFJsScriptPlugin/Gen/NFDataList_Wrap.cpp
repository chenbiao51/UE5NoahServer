#include "JSClassRegister.h"


static void * _NFDataListNew_(const v8::FunctionCallbackInfo<v8::Value>& Info)
{

}

static void _NFDataListDelete_(void * Ptr)
{

}

struct AutoRegisterForNFDataList
{
    AutoRegisterForNFDataList()
    {
        puerts::JSClassDefinition Def = JSClassEmptyDefinition;
        static puerts::JSPropertyInfo Properties[] = {};
        static puerts::JSFunctionInfo Methods[] = {};
        static puerts::JSFunctionInfo Functions[] = {};

        Def.UETypeName = "NFDataList";
        Def.Initialize = _NFDataListNew_;
        Def.Finalize = _NFDataListDelete_;
        Def.Properties = Properties;
        Def.Methods = Methods;
        Def.Functions = Functions;
        puerts::RegisterJSClass(Def);
    }
};


AutoRegisterForNFDataList _AutoRegisterForNFDataList_;