#include <ntifs.h>
#include <ntstrsafe.h>
#include "tools.h"
#include "ssdt.h"



ULONG_PTR goldFunc = 0;

typedef NTSTATUS (NTAPI *OpenProcessProc)(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PCLIENT_ID ClientId
);

NTSTATUS NTAPI MyOpenProcess(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PCLIENT_ID ClientId
)
{
	DbgPrintEx(77, 0, "[db]:%s\r\n",__FUNCTION__);
	OpenProcessProc func = (OpenProcessProc)goldFunc;

	return func(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
	//返回到原来的函数
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	SsdtSetHook("ZwOpenProcess", goldFunc); //还原回自己的正常函数
	SsdtDestory();//卸载内存

	//延时
	LARGE_INTEGER inTime = {0};
	inTime.QuadPart = -10000 * 3000;
	////防止调用MyOpenProcess返回调用func原函数导致内存已经释放导致找不到内存而蓝屏
	KeDelayExecutionThread(KernelMode, FALSE, &inTime);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	DbgBreakPoint();
	SsdtInit();
	
	goldFunc = SsdtSetHook("ZwOpenProcess", MyOpenProcess);//完成替换自己的函数
	//goldFunc为原来的正常函数

	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}