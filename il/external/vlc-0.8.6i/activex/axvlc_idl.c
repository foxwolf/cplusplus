

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Apr 18 17:29:06 2007
 */
/* Compiler settings for axvlc.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_AXVLC,0xDF2BBE39,0x40A8,0x433b,0xA2,0x79,0x07,0x3F,0x48,0xDA,0x94,0xB6);


MIDL_DEFINE_GUID(IID, IID_IVLCControl,0xC2FA41D0,0xB113,0x476e,0xAC,0x8C,0x9B,0xD1,0x49,0x99,0xC1,0xC1);


MIDL_DEFINE_GUID(IID, IID_IVLCAudio,0x9E0BD17B,0x2D3C,0x4656,0xB9,0x4D,0x03,0x08,0x4F,0x3F,0xD9,0xD4);


MIDL_DEFINE_GUID(IID, IID_IVLCInput,0x49E0DBD1,0x9440,0x466C,0x9C,0x97,0x95,0xC6,0x71,0x90,0xC6,0x03);


MIDL_DEFINE_GUID(IID, IID_IVLCLog,0x8E3BC3D9,0x62E9,0x48FB,0x8A,0x6D,0x99,0x3F,0x9A,0xBC,0x4A,0x0A);


MIDL_DEFINE_GUID(IID, IID_IVLCMessage,0x9ED00AFA,0x7BCD,0x4FFF,0x8D,0x48,0x7D,0xD4,0xDB,0x2C,0x80,0x0D);


MIDL_DEFINE_GUID(IID, IID_IVLCMessageIterator,0x15179CD8,0xCC12,0x4242,0xA5,0x8E,0xE4,0x12,0x21,0x7F,0xF3,0x43);


MIDL_DEFINE_GUID(IID, IID_IVLCMessages,0x6C5CE55D,0x2D6C,0x4AAD,0x82,0x99,0xC6,0x2D,0x23,0x71,0xF1,0x06);


MIDL_DEFINE_GUID(IID, IID_IVLCPlaylist,0x54613049,0x40BF,0x4035,0x9E,0x70,0x0A,0x93,0x12,0xC0,0x18,0x8D);


MIDL_DEFINE_GUID(IID, IID_IVLCVideo,0x0AAEDF0B,0xD333,0x4B27,0xA0,0xC6,0xBB,0xF3,0x14,0x13,0xA4,0x2E);


MIDL_DEFINE_GUID(IID, IID_IVLCControl2,0x2D719729,0x5333,0x406C,0xBF,0x12,0x8D,0xE7,0x87,0xFD,0x65,0xE3);


MIDL_DEFINE_GUID(IID, DIID_DVLCEvents,0xDF48072F,0x5EF8,0x434e,0x9B,0x40,0xE2,0xF3,0xAE,0x75,0x9B,0x5F);


MIDL_DEFINE_GUID(IID, IID_IVLCPlaylistItems,0xFD37FE32,0x82BC,0x4A25,0xB0,0x56,0x31,0x5F,0x4D,0xBB,0x19,0x4D);


MIDL_DEFINE_GUID(CLSID, CLSID_VLCPlugin,0xE23FE9C6,0x778E,0x49D4,0xB5,0x37,0x38,0xFC,0xDE,0x48,0x87,0xD8);


MIDL_DEFINE_GUID(CLSID, CLSID_VLCPlugin2,0x9BE31822,0xFDAD,0x461B,0xAD,0x51,0xBE,0x1D,0x1C,0x15,0x99,0x21);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

