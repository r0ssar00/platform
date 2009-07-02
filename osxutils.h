/*
 *  osxutils.h
 *  XTagRev
 *
 *  Created by Kevin Ross on 20/01/09.
 *
 */
#ifndef _OSXUTILS_H
#define _OSXUTILS_H
#define MoreAssert(x) (true)
#define MoreAssertQ(x)
int SetFileComment (string spath, string scomment);
string GetFileComment(string spath);
static char *GetCommentParameter (char *arg);
static OSErr OSX_SetComment (FSSpec *fileSpec, const char *comment);
static void PrintVersion (void);
static void PrintHelp (void);
pascal OSErr MoreFESetComment(const FSSpecPtr pFSSpecPtr, const Str255 pCommentStr, const AEIdleUPP pIdleProcUPP);
pascal OSErr MoreFEGetComment(const FSSpecPtr pFSSpecPtr,Str255 pCommentStr,const AEIdleUPP pIdleProcUPP);
pascal OSStatus MoreAEOCreateObjSpecifierFromFSSpec(const FSSpecPtr pFSSpecPtr,AEDesc *pObjSpecifier);
pascal OSStatus MoreAEOCreateObjSpecifierFromFSRef(const FSRefPtr pFSRefPtr,AEDesc *pObjSpecifier);
pascal OSStatus MoreAEOCreateObjSpecifierFromCFURLRef(const CFURLRef pCFURLRef,AEDesc *pObjSpecifier);
pascal OSErr MoreAESendEventNoReturnValue (const AEIdleUPP pIdleProcUPP, const AppleEvent* pAppleEvent );
pascal OSErr MoreAEGetHandlerError(const AppleEvent* pAEReply);
pascal void MoreAEDisposeDesc(AEDesc* desc);
pascal void MoreAENullDesc(AEDesc* desc);
pascal OSErr MoreFEGetComment(const FSSpecPtr pFSSpecPtr,Str255 pCommentStr,const AEIdleUPP pIdleProcUPP);
pascal OSStatus MoreAESendEventReturnAEDesc(const AEIdleUPP    pIdleProcUPP, const AppleEvent  *pAppleEvent,const DescType    pDescType, AEDesc        *pAEDesc);
pascal OSStatus MoreAESendEventReturnPString(const AEIdleUPP pIdleProcUPP,const AppleEvent* pAppleEvent,Str255 pStr255);
pascal OSStatus MoreAESendEventReturnData(const AEIdleUPP    pIdleProcUPP,const AppleEvent  *pAppleEvent,DescType      pDesiredType,DescType*      pActualType,void*         pDataPtr,Size        pMaximumSize,Size         *pActualSize);
pascal OSErr MoreAEGetCFStringFromDescriptor(const AEDesc* pAEDesc, CFStringRef* pCFStringRef);
Boolean MyAEIdleCallback (EventRecord * theEvent,SInt32 * sleepTime,RgnHandle * mouseRgn);
int CreateAlias (const char *srcPath, const char *destPath);
static OSStatus FSMakePath(FSRef fileRef, UInt8 *path, UInt32 maxPathSize);
string ResolveAlias (string path);
short UnixIsFolder (const char *path);
static const OSType gFinderSignature = 'MACS';
#define		MAX_COMMENT_LENGTH	255
#endif
