/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include "JSModuleLoader.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Algo/Reverse.h"
#if (ENGINE_MAJOR_VERSION >= 5)
#include "HAL/PlatformFileManager.h"
#else
#include "HAL/PlatformFilemanager.h"
#endif

namespace puerts
{

static void StringSplit(const std::string& str,const char split, std::vector<string>& res)
{
    if(str=="") return;
    //
    std::string strs = str+split;
    size_t pos = strs.find(split);
    //
    while(pos!= strs.npos)
    {
        std::string temp = strs.substr(0,pos);
        res.push_back(temp);
        //
        strs = strs.substr(pos+1,strs.size());
        pos = strs.find(split);
    }
}

static void StringSplit(const std::string& str,const std::string& split, std::vector<string>& res)
{
    if(str=="") return;
    //
    std::string strs = str+split;
    size_t pos = strs.find(split);
    int step = split.size();
    //
    while(pos!= strs.npos)
    {
        std::string temp = strs.substr(0,pos);
        res.push_back(temp);
        //
        strs = strs.substr(pos+step,strs.size());
        pos = strs.find(split);
    }
}

static std::string Join(const std::vector<std::string>& strings,const std::string& separator)
{
    if(strings.empty())
    {
        return "";
    }
    std::string result = strings[0];
    for(std::size_t i=1;i<strings.size();++i)
    {
        result +=separator + strings[i];
    }
    return result;
}


static std::string PathNormalize(const std::string& PathIn)
{
    std::vector<std::string> PathFrags;
    StringSplit(PathIn,"/",PathFrags);
    std::reverse(PathFrags.begin(),PathFrags.end());
    std::vector<std::string> NewPathFrags;
    bool FromRoot = PathIn.find("/")==0;
    while (PathFrags.size() > 0)
    {
        std::string E = PathFrags.pop_back();
        if (E != "" && E != ".")
        {
            if (E == ".." && NewPathFrags.size() > 0 && NewPathFrags[NewPathFrags.size()-1] != "..")
            {
                NewPathFrags.pop_back();
            }
            else
            {
                NewPathFrags.push_back(E);
            }
        }
    }
    if (FromRoot)
    {
        return "/" + Join(NewPathFrags, "/");
    }
    else
    {
        return Join(NewPathFrags,"/");
    }
}

bool DefaultJSModuleLoader::CheckExists(const std::string& PathIn, std::string& Path, std::string& AbsolutePath)
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    std::string NormalizedPath = PathNormalize(PathIn);
    if (PlatformFile.FileExists(*NormalizedPath))
    {
        AbsolutePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*NormalizedPath);
        Path = NormalizedPath;
        return true;
    }

    return false;
}

bool DefaultJSModuleLoader::SearchModuleInDir(const std::string& Dir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath)
{
    if (FPaths::GetExtension(RequiredModule) == TEXT(""))
    {
        return SearchModuleWithExtInDir(Dir, RequiredModule + ".js", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule + ".mjs", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule / "index.js", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule / "package.json", Path, AbsolutePath);
    }
    else
    {
        return SearchModuleWithExtInDir(Dir, RequiredModule, Path, AbsolutePath);
    }
}

bool DefaultJSModuleLoader::SearchModuleWithExtInDir(const std::string& Dir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath)
{
    return CheckExists(Dir / RequiredModule, Path, AbsolutePath) ||
           (!Dir.EndsWith(TEXT("node_modules")) && CheckExists(Dir / TEXT("node_modules") / RequiredModule, Path, AbsolutePath));
}

bool DefaultJSModuleLoader::Search(const std::string& RequiredDir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath)
{
    if (SearchModuleInDir(RequiredDir, RequiredModule, Path, AbsolutePath))
    {
        return true;
    }
    if (RequiredDir != TEXT("") && !RequiredModule.GetCharArray().Contains('/') && !RequiredModule.EndsWith(TEXT(".js")) &&
        !RequiredModule.EndsWith(TEXT(".mjs")))
    {
        // 调用require的文件所在的目录往上找
        TArray<FString> pathFrags;
        RequiredDir.ParseIntoArray(pathFrags, TEXT("/"));
        pathFrags.Pop();    // has try in "if (SearchModuleInDir(RequiredDir, RequiredModule, Path, AbsolutePath))"
        while (pathFrags.Num() > 0)
        {
            if (!pathFrags.Last().Equals(TEXT("node_modules")))
            {
                if (SearchModuleInDir(std::string::Join(pathFrags, TEXT("/")), RequiredModule, Path, AbsolutePath))
                {
                    return true;
                }
            }
            pathFrags.Pop();
        }
    }

    return SearchModuleInDir(FPaths::ProjectContentDir() / ScriptRoot, RequiredModule, Path, AbsolutePath) ||
           (ScriptRoot != TEXT("JavaScript") &&
               SearchModuleInDir(FPaths::ProjectContentDir() / TEXT("JavaScript"), RequiredModule, Path, AbsolutePath));
}

bool DefaultJSModuleLoader::Load(const std::string& Path, std::string& Content)
{
     std::ifstream ifs(Path);
     if(ifs)
     {
        std::stringstream buffer;
        buffer<<ifs.rdbuf();
        std::string filecontent(buffer.str());
        Content = *filecontent;
        ifs.close();
        return true;
     }
     else
     {
        std::cout << "no find====>"<<Path << std::endl;
     }
     
     return false;
     
}

std::string& DefaultJSModuleLoader::GetScriptRoot()
{
    return ScriptRoot;
}

}    // namespace puerts
