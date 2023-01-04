/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#pragma once

#include <vector>
#include <string>
#include <sstream>

#pragma warning(push, 0)
#include "libplatform/libplatform.h"
#include "v8.h"
#pragma warning(pop)

#include "DataTransfer.h"

namespace puerts
{
enum ArgType
{
    EArgInt32,
    EArgNumber,
    EArgString,
    EArgExternal,
    EArgFunction,
    EArgObject
};

class FV8Utils
{
public:



    FORCEINLINE static void ThrowException(v8::Isolate* Isolate, const std::string& Message)
    {
        ThrowException(Isolate, Message.c_str());
    }

    FORCEINLINE static void ThrowException(v8::Isolate* Isolate, const char* Message)
    {
        auto ExceptionStr = v8::String::NewFromUtf8(Isolate, Message, v8::NewStringType::kNormal).ToLocalChecked();
        Isolate->ThrowException(v8::Exception::Error(ExceptionStr));
    }

    FORCEINLINE static void* GetPointer(v8::Local<v8::Context>& Context, v8::Local<v8::Value> Value, int Index = 0)
    {
        if (Value.IsEmpty() || !Value->IsObject() || Value->IsUndefined() || Value->IsNull())
        {
            return nullptr;
        }
        auto Object = Value->ToObject(Context).ToLocalChecked();
        return GetPointerFast<void>(Object, Index);
    }

    FORCEINLINE static void* GetPointer(v8::Local<v8::Object> Object, int Index = 0)
    {
        if (Object.IsEmpty() || Object->IsUndefined() || Object->IsNull())
        {
            return nullptr;
        }
        return GetPointerFast<void>(Object, Index);
    }

    template <typename T>
    FORCEINLINE static T* GetPointerFast(v8::Local<v8::Object> Object, int Index = 0)
    {
        return DataTransfer::GetPointerFast<T>(Object, Index);
    }

    FORCEINLINE static bool IsReleasedPtr(void* Ptr)
    {
        return RELEASED_UOBJECT_MEMBER == Ptr;
    }

    FORCEINLINE static v8::Local<v8::String> InternalString(v8::Isolate* Isolate, const std::string& String)
    {
        return v8::String::NewFromUtf8(Isolate, String.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
    }

    FORCEINLINE static v8::Local<v8::String> InternalString(v8::Isolate* Isolate, const char* String)
    {
        return v8::String::NewFromUtf8(Isolate, String, v8::NewStringType::kNormal).ToLocalChecked();
    }

    FORCEINLINE static std::string ToFString(v8::Isolate* Isolate, v8::Local<v8::Value> Value)
    {
        v8::String::Utf8Value   utf8(Isolate, Value);
        std::string str(*utf8);
        return str;
    }

    FORCEINLINE static v8::Local<v8::String> ToV8String(v8::Isolate* Isolate, const std::string& String)
    {
        return v8::String::NewFromUtf8(Isolate, String.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
    }

    FORCEINLINE static v8::Local<v8::String> ToV8String(v8::Isolate* Isolate, const char* String)
    {
        return v8::String::NewFromUtf8(Isolate, String, v8::NewStringType::kNormal).ToLocalChecked();
    }

    static v8::Local<v8::String> ToV8StringFromFileContent(v8::Isolate* Isolate, const std::vector<uint8_t>& FileContent)
    {
        const uint8_t* Buffer = FileContent.data();
        int Size = FileContent.size();

        if (Size >= 2 && !(Size & 1) && ((Buffer[0] == 0xff && Buffer[1] == 0xfe) || (Buffer[0] == 0xfe && Buffer[1] == 0xff)))
        {
            std::string Content(FileContent.begin(),FileContent.end());
            //FFileHelper::BufferToString(Content, Buffer, Size);
            return ToV8String(Isolate, Content);
        }
        else
        {
            if (Size >= 3 && Buffer[0] == 0xef && Buffer[1] == 0xbb && Buffer[2] == 0xbf)
            {
                // Skip over UTF-8 BOM if there is one
                Buffer += 3;
                Size -= 3;
            }
            return v8::String::NewFromUtf8(Isolate, (const char*) Buffer, v8::NewStringType::kNormal, Size).ToLocalChecked();
        }
    }

    template <typename T>
    FORCEINLINE static T* IsolateData(v8::Isolate* Isolate)
    {
        return DataTransfer::IsolateData<T>(Isolate);
    }

    FORCEINLINE static bool CheckArgumentLength(const v8::FunctionCallbackInfo<v8::Value>& Info, int32_t Length)
    {
        if (Info.Length() < Length)
        {
            std::ostringstream oss ;
            oss <<  "Bad parameters, the function expect "<< Length << ", but  "<< Info.Length()<<"provided.";
            std::string info = oss.str();
            ThrowException(Info.GetIsolate(),info);
            return false;
        }
        return true;
    }

    FORCEINLINE static std::string TryCatchToString(v8::Isolate* Isolate, v8::TryCatch* TryCatch)
    {
        v8::Isolate::Scope IsolateScope(Isolate);
        v8::HandleScope HandleScope(Isolate);
        v8::String::Utf8Value Exception(Isolate, TryCatch->Exception());
        std::string ExceptionStr(*Exception);
        v8::Local<v8::Message> Message = TryCatch->Message();
        if (Message.IsEmpty())
        {
            // 如果没有提供更详细的信息，直接输出Exception
            return ExceptionStr;
        }
        else
        {
            v8::Local<v8::Context> Context(Isolate->GetCurrentContext());

            // 输出 (filename):(line number): (message).
            v8::String::Utf8Value FileName(Isolate, Message->GetScriptResourceName());
            int LineNum = Message->GetLineNumber(Context).FromJust();
            std::string FileNameStr(*FileName);
            std::string LineNumStr = std::to_string(LineNum);
            std::string FileInfoStr;
            FileInfoStr.append(FileNameStr).append(":").append(LineNumStr).append(": ").append(ExceptionStr);

            std::string FinalReport;
            FinalReport.append(FileInfoStr).append("\n");

            // 输出调用栈信息
            v8::Local<v8::Value> StackTrace;
            if (TryCatch->StackTrace(Context).ToLocal(&StackTrace))
            {
                v8::String::Utf8Value StackTraceVal(Isolate, StackTrace);
                std::string StackTraceStr(*StackTraceVal);
                FinalReport.append("\n").append(StackTraceStr);
            }
            return FinalReport;
        }
    }

    FORCEINLINE static bool CheckArgument(const v8::FunctionCallbackInfo<v8::Value>& Info, const std::vector<ArgType>& TypesExpect)
    {
        if (Info.Length() < TypesExpect.size())
        {
            std::ostringstream oss ;
            oss <<  "Bad parameters, the function expect "<< TypesExpect.size() << ", but  "<< Info.Length()<<"provided.";
            std::string info = oss.str();
            ThrowException(Info.GetIsolate(), info);
            return false;
        }

        for (int i = 0; i < TypesExpect.size(); ++i)
        {
            switch (TypesExpect[i])
            {
                case EArgInt32:
                    if (!Info[i]->IsInt32())
                    {
                        std::ostringstream oss ;
                        oss <<  "Bad parameters #"<< i << ", expect a int32.";
                        std::string info = oss.str();
                        ThrowException(Info.GetIsolate(), info);
                        return false;
                    }
                    else
                    {
                        break;
                    }
                case EArgNumber:
                    if (!Info[i]->IsNumber())
                    {
                        std::ostringstream oss ;
                        oss <<  "Bad parameters #"<< i << ", expect a int32.";
                        std::string info = oss.str();
                        ThrowException(Info.GetIsolate(), info);
                        return false;
                    }
                    else
                    {
                        break;
                    }
                case EArgString:
                    if (!Info[i]->IsString())
                    {
                        std::ostringstream oss ;
                        oss <<  "Bad parameters #"<< i << ", expect a string.";
                        std::string info = oss.str();
                        ThrowException(Info.GetIsolate(), info);
                        return false;
                    }
                    else
                    {
                        break;
                    }
                case EArgExternal:
                    if (!Info[i]->IsExternal())
                    {
                        std::ostringstream oss ;
                        oss <<  "Bad parameters #"<< i << ", expect an external.";
                        std::string info = oss.str();
                        ThrowException(Info.GetIsolate(), info);
                        return false;
                    }
                    else
                    {
                        break;
                    }
                case EArgFunction:
                    if (!Info[i]->IsFunction())
                    {
                        std::ostringstream oss ;
                        oss <<  "Bad parameters #"<< i << ", expect a function.";
                        std::string info = oss.str();
                        ThrowException(Info.GetIsolate(), info);
                        return false;
                    }
                    else
                    {
                        break;
                    }
                case EArgObject:
                    if (!Info[i]->IsObject())
                    {
                        std::ostringstream oss ;
                        oss <<  "Bad parameters #"<< i << ", expect a object.";
                        std::string info = oss.str();
                        ThrowException(Info.GetIsolate(), info);
                        return false;
                    }
                    else
                    {
                        break;
                    }
                default:
                    break;
            }
        }

        return true;
    }
};
}    // namespace puerts

#define CHECK_V8_ARGS_LEN(Length)                     \
    if (!FV8Utils::CheckArgumentLength(Info, Length)) \
    {                                                 \
        return;                                       \
    }

#define CHECK_V8_ARGS(...)                                 \
    static std::vector<ArgType> ArgExpect = {__VA_ARGS__}; \
    if (!FV8Utils::CheckArgument(Info, ArgExpect))         \
    {                                                      \
        return;                                            \
    }
