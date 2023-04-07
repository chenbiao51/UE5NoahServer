#include "JSClassRegister.h"


static void * _TDataNew_(const v8::FunctionCallbackInfo<v8::Value>& Info)
{

}

static void _TDataDelete_(void * Ptr)
{

}

struct AutoRegisterForTData
{
    AutoRegisterForTData()
    {
        puerts::JSClassDefinition Def = JSClassEmptyDefinition;

        static puerts::JSPropertyInfo Properties[] = {};
        static puerts::JSFunctionInfo Methods[] = {};
        static puerts::JSFunctionInfo Functions[] = {};

        Def.UETypeName = "TData";
        Def.Initialize = _TDataNew_;
        Def.Finalize = _TDataDelete_;
        Def.Properties = Properties;
        Def.Methods = Methods;
        Def.Functions = Functions;
        puerts::RegisterJSClass(Def);

    }
};


AutoRegisterForTData _AutoRegisterForTData_;