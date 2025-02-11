//Identifies the type of information that will be written to the minidump file by the MiniDumpWriteDump function.
//标识由MiniDumpWriteDump函数写入到转储文件的信息类型
typedef enum _MINIDUMP_TYPE {
	MiniDumpNormal                         = 0x00000000,  
	//包括仅仅需要捕获过程中的所有现有线程的堆栈跟踪信息。	

	MiniDumpWithDataSegs                   = 0x00000001,  
	//包括所有已加载的模块的数据段。这将导致在包括全局变量，它可以使小型转储文件的显着变大。每个模块控制，使用ModuleWriteDataSeg的的​​枚举值MODULE_WRITE_FLAGS。
	//如果指定这个标志，minidump会包括进程装载的所有可执行模块的可写数据段。如果我们希望查看全局变量的值，有不希望被MiniDumpWithFullMemory困扰，就可以使用MiniDumpWithDataSegs	

	MiniDumpWithFullMemory                 = 0x00000002,
	//包括所有访问内存中的进程。的原始存储器中的数据的结尾处有，使最初的结构可以被直接映射没有原料存储器信息。此选项可能会导致非常大的文件。
	//如果指定了这个标志，minidump会包含进程地址空间中所有可读页面的内容。我们可以看到应用程序分配的所有内存，这使我们有很多的调试方法。
	// 可以查看存储在栈上、堆上、模块数据段的所有数据。甚至还可以看到线程和进程环境块(Process Environment Block和Thread Environment Bolck, PEB和TEB)的数据。
	// 这些没有公开的数据结构可以给我们的调试提供无价的帮助。
	//使用这个标记的唯一问题是会使minidump变得很大，至少有几MByte。另外，minidump的内容里面包含了冗余信息，所有可执行模块的代码段都包含在了里面。
	// 但是很多时候，我们很容易从其他地方获得可执行代码。让我们一起来看看MINIDUMP_TYPE，是否能够找到更好的选项。
	// 
	MiniDumpWithHandleData                 = 0x00000004,
	//果指定这个标志，minidump会包括故障时刻进程故障表里面的所有句柄。可以用WinDbg的!handle来显示这些信息。
	//这个标志对于minidump大小的影响取决于进程句柄表中的句柄数量。 

	MiniDumpFilterMemory                   = 0x00000008,
	//如果指定这个标志，栈内存的内容会在保存之前进行过滤。只有重建调用栈需要的数据才会被保留。其他数据会被写成0。也就是说，调用栈可以被重建，但是所有局部变量和函数参数的值都是0。
	// 这个标志不影响minidump的大小。它只是没有改变保存的内存数量，只是把其中一部分用0覆盖了。
	// 同时，这个标志只影响线程栈占用内存的内容。其他内存（比如堆）不受影响。如果使用了MiniDumpWithFullMemory，这个标志就不起作用了。
	
	MiniDumpScanMemory                     = 0x00000010,
	//这个标志可以帮助我们节约minidump占用的空间。它会把调试不需要的可执行模块去掉。这个标志会和MiniDumpCallback函数紧密合作。
	// 因此，我们首先看一下这个函数，然后回头讨论MiniDumpScanMemory。

	MiniDumpWithUnloadedModules            = 0x00000020,
	MiniDumpWithIndirectlyReferencedMemory = 0x00000040,
	//如果指定这个标志，MiniDumpWriteDump检查线程栈内存中的每一个指针。这些指针可能指向线程地址空间的其他可读内存页。
	// 一旦发现这样的指针，程序会读取指针附近1024字节的内容存到minidump中（指针前的256字节和指针后的768字节）。
	
	MiniDumpFilterModulePaths              = 0x00000080,
	//这个标志控制模块信息中是否包括模块路径(参考MiniDumpNormal的说明)。如果指定这个标记，模块路径会从dump中删除，只保留模块的名字。
	// 按照文档说明，它也可以帮助从minidump中删除可能涉及隐私的信息（例如有些时候模块的路径会包含用户名）。
	// 由于模块路径数量不多，这个标志对minidump的大小影响不大。对调试的影响也不大。我们经常需要告诉调试器匹配的可执行程序保存的位置。

	MiniDumpWithProcessThreadData          = 0x00000100,
	//有些时候我们需要查看线程和进程环境块的内容（PEB和TEB）。假设minidump包括了这些块占用的内存，就可以通过WinDbg的!peb和!teb命令来查看。
	// 这正是MiniDumpWithProcessThreadData所提供的数据。当使用这个标志时，minidump会包含PEB和TEB占据的内存页。
	// 同时，也包括了另外一些它们也用的内存页(例如，环境变量和进程参数保存的位置，通过TlsAlloc分配的TLS空间)。
	// 遗憾的是，有一些PEB和TEB引用的内存被忽略了，
	// 例如，通过__declspec(thread)分配的线程TLS数据。如果确实需要，就不得不使用MiniDumpWithFullMemory或者MiniDumpWithPrivateReadWriteMemory来获得。

	MiniDumpWithPrivateReadWriteMemory     = 0x00000200,
	//如果指定这个标志，minidump会包括所有可读和可写的私有内存页的内容。这使我们可以察看栈、堆甚至TLS的数据。PEB和TEB也包括在里面。
	//这时候，minidump没有包括共享内存也的内容。也就是说，我们不能查看内存映射文件的内容。同样，可执行模块的代码和数据段也没有包括进来。
	// 不包括代码段意味着dump没有占用不需要的空间。但是，我们也没有办法查看全局变量的值。
	//无论如何，通过组合其他一些选项，MiniDumpWithPrivateReadWriteMemory是一个非常有用的选项。我们会在后面看到。
	
	MiniDumpWithoutOptionalData            = 0x00000400,
	//我们已经看过的所有MINIDUMP_TYPE标记都是想minidump中添加一些数据。也有一些标志作用相反，它们从minidump中去除一些不必要的数据。
	// MiniDumpWithoutOptionalData就是其中一个。他可以用来减小保存在dump中的内存的内容。
	// 当指定这个标志时，只有MiniDumpNormal指定的内存会被保存。
	// 其他内存相关的标志(MiniDumpWithFullMemory, MiniDumpWithPrivateReadWriteMemory, MiniDumpWithIndirectlyReferencedMemory)即使指定，也是无效的。
	// 同时，他不影响这些标志的行为：MiniDumpWithProcessThreadData, MiniDumpWithThreadInfo, MiniDumpWithHandleData, MiniDumpWithDataSegs,
	// MiniDumpWithCodeSegs, MiniDumpWithFullMemoryInfo


	MiniDumpWithFullMemoryInfo             = 0x00000800,
	//如果希望检查整个继承的虚拟内存布局，我们可以使用MiniDumpWithFullMemoryInfo标志。
	// 如果指定它，mindump会包含进程虚拟内存布局的完整信息。可以通过WinDbg的!vadump和!vprot命令查看。
	// 这个标志对minidump大小的影响取决于虚拟内存布局-每个有相似特性的页面区域（参考VirtualQuery函数说明）会增加48字节。

	MiniDumpWithThreadInfo                 = 0x00001000,
	//MiniDumpWithThreadInfo可以帮助收集进程中线程的附加信息。对于每一个线程，会提供下列信息：
	//线程时间 (创建时间，执行用户代码和内核代码的时间)
	//入口地址
	//相关性
	//WinDbg中，可以通过.ttime命令查看线程时间。 

	MiniDumpWithCodeSegs                   = 0x00002000,
	//如果指定这个标志，mindump会包括所有进程装载的可执行模块的代码段。就像MiniDumpWithDataSegs，minidump大小会有明显增长。
	// 在文章的后半部分，我会演示增么样定制MiniDumpWriteDump，保证只包含必要的代码段
	
	MiniDumpWithoutAuxiliaryState          = 0x00004000,
	MiniDumpWithFullAuxiliaryState         = 0x00008000,
	MiniDumpWithPrivateWriteCopyMemory     = 0x00010000,
	MiniDumpIgnoreInaccessibleMemory       = 0x00020000,
	MiniDumpWithTokenInformation           = 0x00040000,//添加过滤分流相关数据。DbgHelp 6.1之前的版本不支持这个值。

	MiniDumpValidTypeFlags                 = 0x0007ffff,//指示标志是有效的。
} MINIDUMP_TYPE;
