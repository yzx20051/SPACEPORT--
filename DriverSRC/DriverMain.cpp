﻿#include "stdafx.h"

UNICODE_STRING RegPath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\ΉӨPΣƧΛЯ");//rtblok
typedef NTSTATUS(*HookControl_t)(void* a1, void* a2);
HookControl_t OriginalPtr;
PVOID SharedBuffer = 0;
UINT SharedPid = 0;
ULONG64 NewMaggicCode = DEFAULT_MAGGICCODE;

NTSTATUS HookControl(PDEVICE_OBJECT device, PIRP irp) {
	static bool retry = false;
	retry:
	PEPROCESS process = NULL;
	NTSTATUS status = PsLookupProcessByProcessId((HANDLE)SharedPid, &process);
	if (NT_SUCCESS(status) && process) {
		retry = false;
		REQUEST_DATA data;
		SIZE_T outSize = 0; 
		if (NT_SUCCESS(MmCopyVirtualMemory(process, (void*)SharedBuffer, PsGetCurrentProcess(), &data, (SIZE_T)sizeof(REQUEST_DATA), KernelMode, &outSize))) {
			if (*data.MaggicCode == NewMaggicCode) {
				if (NewMaggicCode == DEFAULT_MAGGICCODE) { //reset maggicode to fuck with bad devs ;)
					ULONG time = 0;
					KeQuerySystemTime(&time);
					NewMaggicCode = RtlRandomEx(&time) % MAXULONG64;
					*data.MaggicCode = NewMaggicCode;
					Utils::Registry::WriteRegistry(RegPath, RTL_CONSTANT_STRING(L"tคkคŞhi"), &NewMaggicCode, REG_QWORD, 8);//emre
			
				}
				switch (data.Type)
				{
					CallbackHandler(WRITE);
					CallbackHandler(READ);
					CallbackHandler(PROTECT);
					CallbackHandler(ALLOC);
					CallbackHandler(FREE);
					CallbackHandler(Alloce);
					CallbackHandler(MODULE);
					CallbackHandler(MAINBASE);
				case THREADCALL: {
					break;
				}
				case 99:
					break;
				default:
					break;
				}
				ObfDereferenceObject(process);
				IofCompleteRequest(irp, 0);
				return 0;
			}
		}
		ObfDereferenceObject(process);
	}
	else {
		SharedBuffer = (PVOID)Utils::Registry::ReadRegistry<LONG64>(RegPath, RTL_CONSTANT_STRING(L"໓Şຟfງ"));//ufc
		SharedPid = (UINT)Utils::Registry::ReadRegistry<LONG64>(RegPath, RTL_CONSTANT_STRING(L"ᏬᏇᏬ"));//ld
		if (!retry) {
			retry = true;
			goto retry;
		}
	}
	return OriginalPtr(device, irp);
}

INT64 driver_main() {


	PDRIVER_OBJECT DriverObject = nullptr;
	UNICODE_STRING DriverObjectName = RTL_CONSTANT_STRING(L"\\Driver\\SPACEPORT");
	ObReferenceObjectByName(&DriverObjectName, (ULONG)OBJ_CASE_INSENSITIVE, (PACCESS_STATE)0, (ACCESS_MASK)0, *IoDriverObjectType, KernelMode, (PVOID)0, (PVOID*)&DriverObject);
	if (DriverObject) {
		*(PVOID*)&OriginalPtr = InterlockedExchangePointer((void**)&DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS], HookControl);
		SharedBuffer = (PVOID)Utils::Registry::ReadRegistry<LONG64>(RegPath, RTL_CONSTANT_STRING(L"໓Şຟfງ"));//ufc
		SharedPid = (UINT)Utils::Registry::ReadRegistry<LONG64>(RegPath, RTL_CONSTANT_STRING(L"ᏬᏇᏬ"));	//ld
		return 0;
	}
	return 1;
}

NTSTATUS DriversMaain(PVOID lpBaseAddress, DWORD32 dwSize) {
	driver_main();
	return -1;
}