/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#include "DataTransfer.h"
#include "ObjectMapper.h"
#if USING_IN_UNREAL_ENGINE
#include "V8Utils.h"
#endif

namespace puerts
{
v8::Local<v8::Value> DataTransfer::FindOrAddCData(
    v8::Isolate* Isolate, v8::Local<v8::Context> Context, const void* TypeId, const void* Ptr, bool PassByPointer)
{
    return IsolateData<ICppObjectMapper>(Isolate)->FindOrAddCppObject(
        Isolate, Context, TypeId, const_cast<void*>(Ptr), PassByPointer);
}


bool DataTransfer::IsInstanceOf(v8::Isolate* Isolate, const void* TypeId, v8::Local<v8::Object> JsObject)
{
    return IsolateData<ICppObjectMapper>(Isolate)->IsInstanceOfCppObject(TypeId, JsObject);
}

v8::Local<v8::Value> DataTransfer::UnRef(v8::Isolate* Isolate, const v8::Local<v8::Value>& Value)
{
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::EscapableHandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    v8::Local<v8::Value> ReturnValue = Value->ToObject(Context).ToLocalChecked()->Get(Context, 0).ToLocalChecked();

    return HandleScope.Escape(ReturnValue);
}

void DataTransfer::UpdateRef(v8::Isolate* Isolate, v8::Local<v8::Value> Outer, const v8::Local<v8::Value>& Value)
{
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::EscapableHandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    auto Ret = Outer->ToObject(Context).ToLocalChecked()->Set(Context, 0, Value);
}


}    // namespace puerts
