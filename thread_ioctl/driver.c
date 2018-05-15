#include <ntifs.h>
#include <wdm.h>
#include "shared.h"
#include "driver.h"


NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING DevNameStrNt = RTL_CONSTANT_STRING(DEVNAME_NT);
	UNICODE_STRING DevNameStrDos = RTL_CONSTANT_STRING(DEVNAME_DOS);
	PDEVICE_OBJECT DevObj = NULL;
	BOOLEAN SymLinkCreated = FALSE;

	UNREFERENCED_PARAMETER(RegistryPath);

	/* Create a Device */
	status = IoCreateDevice(
				DriverObject,
				sizeof(DEV_EXT),
				&DevNameStrNt,
				FILE_DEVICE_UNKNOWN,
				0,
				FALSE,
				&DevObj);
	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Creating device failed with 0x%08X.\n", status);
		goto L_Cleanup;
	}

	/* init dispatcher */
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispDevCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispDevClose;
	DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DispDevCleanup;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispDevCtl;
	DriverObject->DriverUnload = DriverUnload;

	/* Create SymLink */
	status = IoCreateSymbolicLink(&DevNameStrDos, &DevNameStrNt);
	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Creating symbolic link failed with 0x%08X.\n", status);
		goto L_Cleanup;
	}
	SymLinkCreated = TRUE;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Init Driver thread_ioctl succeeded.\n");

L_Cleanup:
	
	if (!NT_SUCCESS(status))
	{
		if (DriverObject->DeviceObject)
		{
			for (PDEVICE_OBJECT DevObjTmp = DriverObject->DeviceObject;
				NULL != DevObjTmp;
				DevObjTmp = DevObjTmp->NextDevice)
			{
				IoDeleteDevice(DevObjTmp);
			}
		}

		if (SymLinkCreated)
			IoDeleteSymbolicLink(&DevNameStrDos);
	}

	return status;
}

NTSTATUS
DispDevCreate(
	_Inout_  PDEVICE_OBJECT DeviceObject,
	_Inout_  PIRP Irp
	)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS
DispDevClose(
	_Inout_  PDEVICE_OBJECT DeviceObject,
	_Inout_  PIRP Irp
	)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS
DispDevCleanup(
	_Inout_  PDEVICE_OBJECT DeviceObject,
	_Inout_  PIRP Irp
	)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS
DispDevCtl(
	_Inout_  PDEVICE_OBJECT DeviceObject,
	_Inout_  PIRP Irp
	)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION CurIrpStackLoc = NULL;
	PVOID IoCtlInBuf = NULL;
	ULONG IoCtlInBufLen = 0;
	ULONG IoCtlCode = 0;
	HANDLE thread = NULL;
	HANDLE Pid = NULL;
	HANDLE ProcHandle = NULL;
	
	CurIrpStackLoc = IoGetCurrentIrpStackLocation(Irp);
	IoCtlCode = CurIrpStackLoc->Parameters.DeviceIoControl.IoControlCode;
	IoCtlInBuf = Irp->AssociatedIrp.SystemBuffer;
	IoCtlInBufLen = CurIrpStackLoc->Parameters.DeviceIoControl.InputBufferLength;

	Pid = *(PHANDLE)IoCtlInBuf;

	status = OpenProc(Pid, &ProcHandle);
	if (!NT_SUCCESS(status))
	{
		goto L_Exit;
	}

	status = PsCreateSystemThread(&thread, 
				GENERIC_ALL, 
				NULL, 
				ProcHandle, 
				NULL, 
				thr_proc, 
				NULL);
	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Create system thread failed with 0x%016X.\n", status);
		goto L_Exit;
	}

L_Exit:
	
	if (ProcHandle)
		ZwClose(ProcHandle);

	if (thread)
		ZwClose(thread);

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

VOID
DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING DevSymlinkName = RTL_CONSTANT_STRING(DEVNAME_DOS);

	IoDeleteSymbolicLink(&DevSymlinkName);

	if (DriverObject->DeviceObject)
	{
		for (PDEVICE_OBJECT DevObj = DriverObject->DeviceObject;
			NULL != DevObj;
			DevObj = DevObj->NextDevice)
		{
			IoDeleteDevice(DevObj);
		}
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[thread_ioctl] unload.\n");
}

VOID
thr_proc(_In_ PVOID StartContext)
{
	UNREFERENCED_PARAMETER(StartContext);

	LARGE_INTEGER li;

	li.QuadPart = 60 * -10 * 1000 * 1000; // 60s

	(void)KeDelayExecutionThread(KernelMode, FALSE, &li);

	(void)PsTerminateSystemThread(STATUS_SUCCESS);
}

NTSTATUS
OpenProc(
	_In_  HANDLE Pid, 
	_Out_ HANDLE *ProcHandle
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS proc = NULL;
	HANDLE proc_handle = NULL;

	if (!ProcHandle)
		return STATUS_INVALID_PARAMETER;

	*ProcHandle = NULL;

	status = PsLookupProcessByProcessId(Pid, &proc);
	if (!NT_SUCCESS(status))
	{
		goto L_Exit;
	}

	status = ObOpenObjectByPointer(proc,
									0,
									NULL,
									PROCESS_ALL_ACCESS,
									*PsProcessType,
									KernelMode,
									&proc_handle);
	if (!NT_SUCCESS(status))
	{
		goto L_Exit;
	}
	
	*ProcHandle = proc_handle;
	proc_handle = NULL;

L_Exit:
	
	if (!NT_SUCCESS(status))
	{
		if (proc_handle)
			ZwClose(proc_handle);
	}

	if (proc)
		ObDereferenceObject(proc);

	return status;
}