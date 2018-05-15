#pragma once

#define DEVNAME_NT L"\\Device\\dev_thread_ioctl"
#define DEVNAME_DOS L"\\DosDevices\\dev_thread_ioctl" 
#define INTERFACE_CLASS_GUID L"{ 78A1C341 - 4539 - 11d3 - B88D - 00C04FAD5171 }"
#define DEVNAME_APP "\\\\.\\dev_thread_ioctl"

#define TI_IOCTL_CREATE_THREAD        CTL_CODE (FILE_DEVICE_UNKNOWN, (0x800 + 2), METHOD_BUFFERED, FILE_ANY_ACCESS)