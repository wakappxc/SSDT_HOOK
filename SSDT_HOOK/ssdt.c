#include "ssdt.h"
#include "tools.h"
#include <intrin.h>

PUCHAR gMapNtdll = NULL;


//导出服务表12356
extern PSsdtTable KeServiceDescriptorTable;

ULONG wpOff()
{
	ULONG cr0 = __readcr0();
	_disable();//关中断
	__writecr0(cr0 & (~0x10000));
	return cr0;
}

VOID wpOn(ULONG value)
{
	__writecr0(value);
	_enable();
}

BOOLEAN SsdtInit()
{
	if (gMapNtdll) return TRUE;
	PWCH path = GetSystemRootNtdllPath();//获取路径的地址
	gMapNtdll = MapOfViewFile(path);
	ExFreePool(path);
	return TRUE;
}

VOID SsdtDestory()
{
	if (!gMapNtdll) return;
	UmMapOfViewFile(gMapNtdll);
	gMapNtdll = NULL;
}

PSsdtTable SsdtGet()
{
	return (PSsdtTable)((PUCHAR)KeServiceDescriptorTable + 0x40);
}

ULONG SsdtGetFunctionIndex(char* funName)
{
	PUCHAR func = (PUCHAR)ExportTableFuncByName(gMapNtdll, funName);
	if (!func) return -1;
	return *(PULONG)(func + 1);  //看汇编，程序开头加1就直接是索引号
}

ULONG_PTR SsdtSetHook(char* funName, ULONG_PTR newFunction)
{
	PSsdtTable ssdtTable = SsdtGet();

	ULONG index = SsdtGetFunctionIndex(funName);

	if (index == -1) return 0;

	ULONG function = ssdtTable->ssdt.funcTable[index];//64位在这里不一样，需要修改

	ULONG _cr0 = wpOff();

	ssdtTable->ssdt.funcTable[index] = newFunction;//替换成自己的函数MyOpenProcess

	wpOn(_cr0);

	return function;

}

