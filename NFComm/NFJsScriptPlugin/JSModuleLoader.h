/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#pragma once

#include <string>

namespace puerts
{
class IJSModuleLoader
{
public:
    virtual bool Search(const std::string& nfdatacfg,const std::string& RequiredDir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath) = 0;

    virtual bool Load(const std::string& Path, std::string& Content) = 0;

    virtual std::string& GetScriptRoot() = 0;

    virtual ~IJSModuleLoader()
    {
    }
};

class  DefaultJSModuleLoader : public IJSModuleLoader
{
public:
    explicit DefaultJSModuleLoader(const std::string& InScriptRoot) : ScriptRoot(InScriptRoot)
    {
    }

    virtual bool Search(const std::string& nfdatacfg,const std::string& RequiredDir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath) override;

    virtual bool Load(const std::string& Path, std::string& Content) override;

    virtual std::string& GetScriptRoot() override;

    virtual bool CheckExists(const std::string& PathIn, std::string& Path, std::string& AbsolutePath);

    virtual bool SearchModuleInDir(const std::string& Dir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath);

    virtual bool SearchModuleWithExtInDir(const std::string& Dir, const std::string& RequiredModule, std::string& Path, std::string& AbsolutePath);

    std::string ScriptRoot;
};

}    // namespace puerts
