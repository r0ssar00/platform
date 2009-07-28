/**
 * @file osx.h
 * @license GPLv2
 * @author Sveinbjorn Thordarson <sveinbt@hi.is>, modified heavily by Kevin Ross <r0ssar00@gmail.com>
 * @details Copyright (C) 2003 Sveinbjorn Thordarson <sveinbt@hi.is>.  Modified under terms of GPLv2.
 */
#ifndef _OSX_H
#define _OSX_H
#define MoreAssert(x) ( true )
#define MoreAssertQ(x)

#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>
// Comment Section
int SetFileComment(std::string spath, std::string scomment);
std::string GetFileComment(std::string spath);
static OSErr OSX_SetComment(FSSpec *fileSpec, const char *comment);
pascal OSErr MoreFESetComment(const FSSpecPtr pFSSpecPtr, const Str255 pCommentStr, const AEIdleUPP pIdleProcUPP);
pascal OSErr MoreFEGetComment(const FSSpecPtr pFSSpecPtr, Str255 pCommentStr, const AEIdleUPP pIdleProcUPP);
Boolean MyAEIdleCallback(EventRecord *theEvent, SInt32 *sleepTime, RgnHandle *mouseRgn);
pascal OSStatus MoreAEOCreateObjSpecifierFromFSSpec(const FSSpecPtr pFSSpecPtr, AEDesc *pObjSpecifier);
pascal OSStatus MoreAEOCreateObjSpecifierFromFSRef(const FSRefPtr pFSRefPtr, AEDesc *pObjSpecifier);
pascal OSStatus MoreAEOCreateObjSpecifierFromCFURLRef(const CFURLRef pCFURLRef, AEDesc *pObjSpecifier);
pascal OSStatus MoreAESendEventReturnPString(const AEIdleUPP pIdleProcUPP, const AppleEvent *pAppleEvent, Str255 pStr255);
pascal OSStatus MoreAESendEventReturnData(const AEIdleUPP pIdleProcUPP, const AppleEvent *pAppleEvent, DescType pDesiredType, DescType *pActualType, void *pDataPtr, Size pMaximumSize, Size *pActualSize);
pascal OSErr MoreAESendEventNoReturnValue(const AEIdleUPP pIdleProcUPP, const AppleEvent *pAppleEvent );
pascal OSErr MoreAEGetHandlerError(const AppleEvent *pAEReply);
pascal void MoreAEDisposeDesc(AEDesc *desc);
pascal void MoreAENullDesc(AEDesc *desc);
static const OSType gFinderSignature = 'MACS';

// Spotlight section
std::list<std::string> QuerySpotlight(std::string arg1, bool throttle = false);
std::list<std::string> QueryComment(std::string arg1, bool throttle = false);
#endif