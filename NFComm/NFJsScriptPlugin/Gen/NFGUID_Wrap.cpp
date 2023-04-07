#include "JSClassRegister.h"

static void * _NFGUIDNew_(const v8::FunctionCallbackInfo<v8::Value>& Info)
{

}

static void _NFGUIDDelete_(void * Ptr)
{

}


struct AutoRegisterForNFGUID
{
    AutoRegisterForNFGUID()
    {
        puerts::JSClassDefinition Def = JSClassEmptyDefinition;

        static puerts::JSPropertyInfo Properties[] = {};
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