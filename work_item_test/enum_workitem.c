#include <wdm.h>
#include "enum_workitem.h"

#define _ANONYMOUS_UNION
#define _ANONYMOUS_STRUCT

#pragma warning(push)
#pragma warning(disable:4201)

typedef struct _NT_TIB {
	struct _EXCEPTION_REGISTRATION_RECORD *ExceptionList;
	PVOID StackBase;
	PVOID StackLimit;
	PVOID SubSystemTib;
	_ANONYMOUS_UNION union {
		PVOID FiberData;
		ULONG Version;
	} DUMMYUNIONNAME;
	PVOID ArbitraryUserPointer;
	struct _NT_TIB *Self;
} NT_TIB, *PNT_TIB;

typedef struct _KPCR
{
	_ANONYMOUS_UNION union
	{
		NT_TIB NtTib;
		_ANONYMOUS_STRUCT struct
		{
			union _KGDTENTRY64 *GdtBase;
			struct _KTSS64 *TssBase;
			ULONG64 UserRsp;
			struct _KPCR *Self;
			struct _KPRCB *CurrentPrcb;
			PKSPIN_LOCK_QUEUE LockArray;
			PVOID Used_Self;
		};
	};
	union _KIDTENTRY64 *IdtBase;
	ULONG64 Unused[2];
	KIRQL Irql;
	UCHAR SecondLevelCacheAssociativity;
	UCHAR ObsoleteNumber;
	UCHAR Fill0;
	ULONG Unused0[3];
	USHORT MajorVersion;
	USHORT MinorVersion;
	ULONG StallScaleFactor;
	PVOID Unused1[3];
	ULONG KernelReserved[15];
	ULONG SecondLevelCacheSize;
	ULONG HalReserved[16];
	ULONG Unused2;
	PVOID KdVersionBlock;
	PVOID Unused3;
	ULONG PcrAlign1[24];
} KPCR, *PKPCR;

#pragma warning(pop)

void
EnumWorkItem()
{
	/*
	1. 获取KPCR，获取KPCRB
	2. 获取_EX_WORK_QUEUE
	3. 遍历列表
	*/
}