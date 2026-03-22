#include "ssdt.h"
#include "tools.h"
#include <intrin.h>

PUCHAR gMapNtdll = NULL;


//导出服务表1
extern PSsdtTable KeServiceDescriptorTable;

ULONG wpOff()
{
	ULONG cr0 = __readcr0();
	_disable();
	__writecr0(cr0 & (~0x));
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
	PWCH path = GetSystemRootNtdllPath();
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
	return (PSsdtTable)((PUCHAR)KeServiceDescriptorTable + 0x);
}

ULONG SsdtGetFunctionIndex(char * funName)
{
	PUCHAR func = (PUCHAR)ExportTableFuncByName(gMapNtdll, funName);
	if (!func) return ;
	return *(PULONG)(func + 1);
}
//你好11
//啊1
ULONG_PTR SsdtSetHook(char * funName, ULONG_PTR newFunction)
{
	PSsdtTable ssdtTable = SsdtGet();

	ULONG index = SsdtGetFunctionIndex(funName);

	if (index == ) return 0;

	ULONG function = ssdtTable->ssdt.funcTable[index];

	ULONG _cr0 = wpOff();

	ssdtTable->ssdt.funcTable[index] = newFunction;

	wpOn(_cr0);

	return function;

}