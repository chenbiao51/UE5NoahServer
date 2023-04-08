/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#pragma once

#if USING_IN_UNREAL_ENGINE
#include "CoreMinimal.h"
#include "UObject/Package.h"
#else
#include "JSClassRegister.h"
#endif

#pragma warning(push, 0)
#include "v8.h"
#pragma warning(pop)

#if !defined(MAPPER_ISOLATE_DATA_POS)
#define MAPPER_ISOLATE_DATA_POS 0
#endif

#define RELEASED_UOBJECT ((UObject*) 12)
#define RELEASED_UOBJECT_MEMBER ((void*) 12)

namespace puerts
{
template <typename T, typename FT, typename = void>
struct TOuterLinker
{
    V8_INLINE static void Link(v8::Local<v8::Context> Context, v8::Local<v8::Value> Outer, v8::Local<v8::Value> Inner)
    {
    }
};

V8_INLINE void LinkOuterImpl(v8::Local<v8::Context> Context, v8::Local<v8::Value> Outer, v8::Local<v8::Value> Inner)
{
#ifdef WITH_OUTER_LINK
    Inner.As<v8::Object>()->Set(Context, 0, Outer);
#endif
}


class JSENV_API DataTransfer
{
public:
    FORCEINLINE static void* MakeAddressWithHighPartOfTwo(void* Address1, void* Address2)
    {
        UPTRINT High = reinterpret_cast<UPTRINT>(Address1) & (((UPTRINT) -1) << (sizeof(UPTRINT) / 2));    //清除低位
        UPTRINT Low = (reinterpret_cast<UPTRINT>(Address2) >> (sizeof(UPTRINT) / 2)) &
                      ~(((UPTRINT) -1) << (sizeof(UPTRINT) / 2));    //右移，然后清除高位
        return reinterpret_cast<void*>(High | Low);
    }

    FORCEINLINE static void SplitAddressToHighPartOfTwo(const void* Address, UPTRINT& High, UPTRINT& Low)
    {
        High = reinterpret_cast<UPTRINT>(Address) & (((UPTRINT) -1) << (sizeof(UPTRINT) / 2));    //清除低位
        Low = reinterpret_cast<UPTRINT>(Address) << (sizeof(UPTRINT) / 2);
    }

    template <typename T>
    FORCEINLINE static T* GetPointerFast(v8::Local<v8::Object> Object, int Index = 0)
    {
        if (Object->InternalFieldCount() > (Index * 2 + 1))
        {
            return static_cast<T*>(MakeAddressWithHighPartOfTwo(
                Object->GetAlignedPointerFromInternalField(Index * 2), Object->GetAlignedPointerFromInternalField(Index * 2 + 1)));
        }
        else
        {
            return nullptr;
        }
    }

    //替代 Object->SetAlignedPointerInInternalField(Index, Ptr);
    FORCEINLINE static void SetPointer(v8::Isolate* Isolate, v8::Local<v8::Object> Object, const void* Ptr, int Index)
    {
        // Object->SetInternalField(Index, v8::External::New(Isolate, Ptr));
        // Object->SetAlignedPointerInInternalField(Index, Ptr);
        UPTRINT High;
        UPTRINT Low;
        SplitAddressToHighPartOfTwo(Ptr, High, Low);
        Object->SetAlignedPointerInInternalField(Index * 2, reinterpret_cast<void*>(High));
        Object->SetAlignedPointerInInternalField(Index * 2 + 1, reinterpret_cast<void*>(Low));
    }

    template <typename T>
    FORCEINLINE static T* IsolateData(v8::Isolate* Isolate)
    {
        return static_cast<T*>(Isolate->GetData(MAPPER_ISOLATE_DATA_POS));
    }

    static v8::Local<v8::Value> FindOrAddCData(v8::Isolate* Isolate, v8::Local<v8::Context> Context, const void* TypeId, const void* Ptr, bool PassByPointer);

    template <typename T>
    static bool IsInstanceOf(v8::Isolate* Isolate, v8::Local<v8::Object> JsObject)
    {
        IsInstanceOf(Isolate,&typeid(T),JsObject);
    };

    static bool IsInstanceOf(v8::Isolate* Isolate, const void* TypeId, v8::Local<v8::Object> JsObject);

    static v8::Local<v8::Value> UnRef(v8::Isolate* Isolate, const v8::Local<v8::Value>& Value);

    static void UpdateRef(v8::Isolate* Isolate, v8::Local<v8::Value> Outer, const v8::Local<v8::Value>& Value);


    template <typename T1, typename T2>
    FORCEINLINE static void LinkOuter(v8::Local<v8::Context> Context, v8::Local<v8::Value> Outer, v8::Local<v8::Value> Inner)
    {
        TOuterLinker<T1, T2>::Link(Context, Outer, Inner);
    }
};
}    // namespace puerts
