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
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "JSModuleLoader.h"


namespace puerts
{

bool EndsWith(const std::string& str ,const std::string& suffix ,bool ignoreCase=false)
{
    if(ignoreCase)
    {
        return boost::iends_with(str,suffix);
    }
    else
    {
        return boost::ends_with(str,suffix);
    }
    
}

static std::string GetExtension(const std::string& InPath,bool bIncludeDot)
{
    const size_t DotPos = InPath.find_last_of(".");
    if(DotPos!=std::string::npos){
        return InPath.substr(DotPos+(bIncludeDot?0:1));
    }
    return "";
}

static void StringSplit(const std::string& str,const char split, std::vector<std::string>& res)
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

static void StringSplit(const std::string& str,const std::string& split, std::vector<std::string>& res)
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
        std::string E = PathFrags.back();
        PathFrags.pop_back();
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
    std::string NormalizedPath = PathNormalize(PathIn);
    std::ifstream File(NormalizedPath);
    if(File.good())
    {
        Path = NormalizedPath;
        AbsolutePath = boost::filesystem::absolute(NormalizedPath).string();
        return true;
    }
    return false;
}

bool DefaultJSModuleLoader::SearchModuleInDir(const std::string& Dir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath)
{
    if (GetExtension(RequiredModule,false) == "")
    {
        return SearchModuleWithExtInDir(Dir, RequiredModule + ".js", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule + ".mjs", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule + "/index.js", Path, AbsolutePath) ||
               SearchModuleWithExtInDir(Dir, RequiredModule + "/package.json", Path, AbsolutePath);
    }
    else
    {
        return SearchModuleWithExtInDir(Dir, RequiredModule, Path, AbsolutePath);
    }
}

bool DefaultJSModuleLoader::SearchModuleWithExtInDir(const std::string& Dir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath)
{
    return CheckExists(Dir+RequiredModule, Path, AbsolutePath) || (!EndsWith(Dir,"node_modules",false) && CheckExists(Dir+"/node_modules/"+RequiredModule, Path, AbsolutePath));
}

bool DefaultJSModuleLoader::Search(const std::string& RequiredDir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath)
{
    if (SearchModuleInDir(RequiredDir, RequiredModule, Path, AbsolutePath))
    {
        return true;
    }
    if (RequiredDir != "" && !(RequiredModule.find('/')!=std::string::npos) && !EndsWith(RequiredModule,".js",true) &&!EndsWith(RequiredModule,".mjs",true))
    {
        // 调用require的文件所在的目录往上找
        std::vector<std::string> pathFrags;
        StringSplit(RequiredDir,"/",pathFrags);
        pathFrags.pop_back();    // has try in "if (SearchModuleInDir(RequiredDir, RequiredModule, Path, AbsolutePath))"
        while (pathFrags.size() > 0)
        {
            if (pathFrags.back()!="node_modules")
            {
                if (SearchModuleInDir(Join(pathFrags, "/"), RequiredModule, Path, AbsolutePath))
                {
                    return true;
                }
            }
            pathFrags.pop_back();
        }
    }

    return SearchModuleInDir(NFDataCfgPath +"/"+ScriptRoot, RequiredModule, Path, AbsolutePath) ||
           (ScriptRoot !="JavaScript" && SearchModuleInDir(NFDataCfgPath +"/JavaScript", RequiredModule, Path, AbsolutePath));
}

bool DefaultJSModuleLoader::Load(const std::string& Path, std::string& Content)
{
     std::ifstream ifs(Path);
     if(ifs)
     {
        std::stringstream buffer;
        buffer<<ifs.rdbuf();
        std::string filecontent(buffer.str());
        Content = filecontent;
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
