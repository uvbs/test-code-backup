========================================================================
    活动模板库：Section4Demo1 项目概述
========================================================================

应用程序向导已为您创建了此 Section4Demo1 项目，您可以在此基础之上
编写自己的动态链接库 (DLL)。


此文件概述了组成项目的每个文件
的内容。

Section4Demo1.vcproj
    这是使用应用程序向导生成的 VC++ 项目的主项目文件。 
    它包含有关生成该文件所使用的 Visual C++ 版本的信息，以及
    有关在应用程序向导中选择的平台、配置和项目功能
    的信息。

Section4Demo1.idl
    此文件包含在您的项目中定义的类型库、接口
    和 coclass 的 IDL 定义。
    此文件将由 MIDL 编译器处理以生成：
        C++ 接口定义和 GUID 声明                 (Section4Demo1.h)
        GUID 定义                                (Section4Demo1_i.c)
        一个类型库                               (Section4Demo1.tlb)
        封送代码                                 (Section4Demo1_p.c 和 dlldata.c)

Section4Demo1.h
    此文件包含在 Section4Demo1.idl 中定义的项的 C++ 接口定义
    和 GUID 声明。它将在编译过程中由 MIDL 重新生成。

Section4Demo1.cpp
    此文件包含对象映射和 DLL 导出的实现。

Section4Demo1.rc
    这是程序使用的所有 Microsoft Windows 资源的清
    单。

Section4Demo1.def
    此模块定义文件为链接器提供有关 DLL 所要求的
    导出的信息。它包含用于以下内容的导出：
        DllGetClassObject  
        DllCanUnloadNow    
        GetProxyDllInfo    
        DllRegisterServer	
        DllUnregisterServer

/////////////////////////////////////////////////////////////////////////////
其他标准文件：

StdAfx.h，StdAfx.cpp
    这些文件用于生成名为 Section4Demo1.pch 的预编译头 (PCH) 文件
    和名为 StdAfx.obj 的预编译类型文件。

Resource.h
    这是用于定义资源 ID 的标准头文件。

/////////////////////////////////////////////////////////////////////////////
代理/存根 (stub) DLL 项目和模块定义文件：

Section4Demo1ps.vcproj
    此文件是用于生成代理/存根 (stub) DLL 的项目文件（若有必要）。
	主项目中的 IDL 文件必须至少包含一个接口，并且
	在生成代理/存根 (stub) DLL 之前必须先编译 IDL 文件。	此进程生成
	dlldata.c、Section4Demo1_i.c 和 Section4Demo1_p.c，它们。
	是生成代理/存根 (stub) DLL 所必需的。

Section4Demo1ps.def
    此模块定义文件为链接器提供有关代理/存根 (stub) 所要求的
    导出的信息。

/////////////////////////////////////////////////////////////////////////////
