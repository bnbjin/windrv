/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_testkmdf,
    0x81dfc4fd,0x8239,0x4719,0xbc,0x57,0x5c,0x85,0xc8,0x7f,0x05,0x66);
// {81dfc4fd-8239-4719-bc57-5c85c87f0566}
