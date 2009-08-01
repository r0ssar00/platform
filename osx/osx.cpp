/**
 * @file osx.cpp
 * @license GPLv2
 * @author Sveinbjorn Thordarson <sveinbt@hi.is>, modified heavily by Kevin Ross <r0ssar00@gmail.com>
 * @details Copyright (C) 2003 Sveinbjorn Thordarson <sveinbt@hi.is>.  Modified under terms of GPLv2.
 */
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>
#include <string>
#include <list>
#include "osx.h"


std::string GetFileComment(std::string spath) {
	const char *path = spath.c_str();
	OSErr err = noErr;
	FSRef fileRef;
	FSSpec fileSpec;
	unsigned char *comment;
	char cStrCmt[ 255 ] = "\0";
	AEIdleUPP inIdleProc = NewAEIdleUPP(&MyAEIdleCallback);

	// see if the file in question exists and we can write it
	if (access(path, R_OK | F_OK) == - 1) {
		return "";
	}

	// get file reference from path
	err = FSPathMakeRef( (const UInt8 *) path, &fileRef, NULL);
	if (err != noErr) {
		return "";
	}

	// retrieve filespec from file ref
	err = FSGetCatalogInfo(&fileRef, NULL, NULL, NULL, &fileSpec, NULL);
	if (err != noErr) {
		return "";
	}

	// call the apple event routine. I'm not going to pretend I understand what's going on
	// in all those horribly kludgy functions, but at least it works.
	err = MoreFEGetComment(&fileSpec, comment, inIdleProc);
	if (err) {
		return "";
	}
	// convert pascal string to c string
	// strncpy((char *)&cStrCmt, (unsigned char *)&comment+1, comment[0]);
	// comment=(unsigned char *)p2cstr((unsigned char *)&cStrCmt);
	CopyPascalStringToC(comment, cStrCmt);

	// if there is no comment, we don't print out anything
	if (!strlen( (char *) &cStrCmt) )
		return "";

	// print out the comment
	if (!false)
		return std::string( (const char *) &cStrCmt);
	else
		return std::string( (const char *) &cStrCmt);
}
int SetFileComment(std::string spath, std::string scomment) {
	const char *path = spath.c_str();
	const char *comment = scomment.c_str();
	OSErr err = noErr;
	FSRef fileRef;
	FSSpec fileSpec;

	// see if the file in question exists and we can write it
	if (access(path, R_OK | W_OK | F_OK) == - 1) {
		return 1;
	}

	// get file reference from path
	err = FSPathMakeRef( (const UInt8 *) path, &fileRef, NULL);
	if (err != noErr) {
		return 1;
	}

	// retrieve filespec from file ref
	err = FSGetCatalogInfo(&fileRef, NULL, NULL, NULL, &fileSpec, NULL);
	if (err != noErr) {
		return 1;
	}
	// being by setting MacOS X Finder Comment
	Str255 pCommentStr;
	CopyCStringToPascal(comment, pCommentStr);
	err = MoreFESetComment(&fileSpec, pCommentStr, NULL);
	if (err != noErr) {
		return 1;
	}
	return 0;
}

pascal OSErr MoreFEGetComment(const FSSpecPtr pFSSpecPtr, Str255 pCommentStr, const AEIdleUPP pIdleProcUPP){
	AppleEvent tAppleEvent = {
		typeNull, NULL
	};                                         // If you always init AEDescs, it's always safe to dispose of them.
	AEDesc tAEDesc = {
		typeNull, NULL
	};
	OSErr anErr = noErr;

	if (NULL == pIdleProcUPP) { // the idle proc is required
		return paramErr;
	}
	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr, &tAEDesc);
	if (anErr) {
		return paramErr;
	}
	if (noErr == anErr) {
		AEBuildError tAEBuildError;

		anErr = AEBuildAppleEvent(
		        kAECoreSuite, kAEGetData,
		        typeApplSignature, &gFinderSignature, sizeof( OSType ),
		        kAutoGenerateReturnID, kAnyTransactionID,
		        &tAppleEvent, &tAEBuildError,
		        "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)}", &tAEDesc);

		// always dispose of AEDescs when you are finished with them
		(void) MoreAEDisposeDesc(&tAEDesc);

		if (noErr == anErr) {
			// Send the event.
			anErr = MoreAESendEventReturnPString(pIdleProcUPP, &tAppleEvent, pCommentStr);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}
pascal OSErr MoreFESetComment(const FSSpecPtr pFSSpecPtr, const Str255 pCommentStr, const AEIdleUPP pIdleProcUPP){
	AppleEvent tAppleEvent = {
		typeNull, nil
	};                                              // If you always init AEDescs, it's always safe to dispose of them.
	AEBuildError tAEBuildError;
	AEDesc tAEDesc = {
		typeNull, nil
	};
	OSErr anErr = noErr;

	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr, &tAEDesc);
	if (noErr == anErr) {
		char *dataPtr = NewPtr(pCommentStr[ 0 ]);

		CopyPascalStringToC(pCommentStr, dataPtr);
		anErr = AEBuildAppleEvent(
		        kAECoreSuite, kAESetData,
		        typeApplSignature, &gFinderSignature, sizeof( OSType ),
		        kAutoGenerateReturnID, kAnyTransactionID,
		        &tAppleEvent, &tAEBuildError,
		        "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)},data:'TEXT'(@)",
		        &tAEDesc, dataPtr);

		DisposePtr(dataPtr);

		if (noErr == anErr) {
			// Send the event. In this case we don't care about the reply
			anErr = MoreAESendEventNoReturnValue(pIdleProcUPP, &tAppleEvent);
			(void) MoreAEDisposeDesc(&tAppleEvent); // always dispose of AEDescs when you are finished with them
		}
	}
	return anErr;
}

pascal OSStatus MoreAEOCreateObjSpecifierFromFSSpec(const FSSpecPtr pFSSpecPtr, AEDesc *pObjSpecifier){
	OSErr anErr = paramErr;

	if (nil != pFSSpecPtr) {
		FSRef tFSRef;

		anErr = FSpMakeFSRef(pFSSpecPtr, &tFSRef);
		if (noErr == anErr) {
			anErr = MoreAEOCreateObjSpecifierFromFSRef(&tFSRef, pObjSpecifier);
		}
	}
	return anErr;
}
pascal OSStatus MoreAEOCreateObjSpecifierFromFSRef(const FSRefPtr pFSRefPtr, AEDesc *pObjSpecifier){
	OSErr anErr = paramErr;

	if (nil != pFSRefPtr) {
		CFURLRef tCFURLRef = CFURLCreateFromFSRef(kCFAllocatorDefault, pFSRefPtr);

		if (nil != tCFURLRef) {
			anErr = MoreAEOCreateObjSpecifierFromCFURLRef(tCFURLRef, pObjSpecifier);
			CFRelease(tCFURLRef);
		} else
			anErr = coreFoundationUnknownErr;
	}
	return anErr;
}
pascal OSStatus MoreAEOCreateObjSpecifierFromCFURLRef(const CFURLRef pCFURLRef, AEDesc *pObjSpecifier){
	OSErr anErr = paramErr;

	if (nil != pCFURLRef) {
		Boolean isDirectory = CFURLHasDirectoryPath(pCFURLRef);
		CFStringRef tCFStringRef = CFURLCopyFileSystemPath(pCFURLRef, kCFURLHFSPathStyle);
		AEDesc containerDesc = {
			typeNull, NULL
		};
		AEDesc nameDesc = {
			typeNull, NULL
		};
		UniCharPtr buf = nil;

		if (nil != tCFStringRef) {
			Size bufSize = ( CFStringGetLength(tCFStringRef) + ( isDirectory ? 1 : 0 ) ) * sizeof( UniChar );

			buf = (UniCharPtr) NewPtr(bufSize);

			if ( ( anErr = MemError() ) == noErr) {
				CFStringGetCharacters(tCFStringRef, CFRangeMake(0, bufSize / 2), buf);
				if (isDirectory) ( buf )[ ( bufSize - 1 ) / 2 ] = (UniChar) 0x003A;
			}
		} else
			anErr = coreFoundationUnknownErr;

		if (anErr == noErr)
			anErr = AECreateDesc(typeUnicodeText, buf, GetPtrSize( (Ptr) buf), &nameDesc);
		if (anErr == noErr)
			if (isDirectory) {
				anErr = CreateObjSpecifier(cFolder, &containerDesc, formName, &nameDesc, false, pObjSpecifier);
			} else {
				anErr = CreateObjSpecifier(cFile, &containerDesc, formName, &nameDesc, false, pObjSpecifier);
			}

		MoreAEDisposeDesc(&nameDesc);

		if (buf)
			DisposePtr( (Ptr) buf);
	}
	return anErr;
}
pascal OSStatus MoreAESendEventReturnPString(const AEIdleUPP pIdleProcUPP, const AppleEvent *pAppleEvent, Str255 pStr255) {
	DescType actualType;
	Size actualSize;
	OSStatus anError;

	anError = MoreAESendEventReturnData(pIdleProcUPP, pAppleEvent, typePString,
	                                    &actualType, pStr255, sizeof( Str255 ), &actualSize);

	if (errAECoercionFail == anError) {
		anError = MoreAESendEventReturnData(pIdleProcUPP, pAppleEvent, typeChar,
		                                    &actualType, ( Ptr ) & pStr255[ 1 ], sizeof( Str255 ), &actualSize);
		if (actualSize < 256)
			pStr255[ 0 ] = (UInt8) actualSize;
		else
			anError = errAECoercionFail;
	}
	return anError;
}
pascal OSStatus MoreAESendEventReturnData(const AEIdleUPP pIdleProcUPP, const AppleEvent *pAppleEvent, DescType pDesiredType, DescType *pActualType, void *pDataPtr, Size pMaximumSize, Size *pActualSize) {
	OSStatus anError = noErr;

	// No idle function is an error, since we are expected to return a value
	if (pIdleProcUPP == NULL)
		anError = paramErr;
	else {
		AppleEvent theReply = {
			typeNull, NULL
		};
		AESendMode sendMode = kAEWaitReply;

		anError = AESend(pAppleEvent, &theReply, sendMode, kAENormalPriority, kNoTimeOut, pIdleProcUPP, NULL);
		// [ Don't dispose of the event, it's not ours ]
		if (noErr == anError) {
			anError = MoreAEGetHandlerError(&theReply);

			if (!anError && theReply.descriptorType != typeNull) {
				anError = AEGetParamPtr(&theReply, keyDirectObject, pDesiredType,
				                        pActualType, pDataPtr, pMaximumSize, pActualSize);
			}
			MoreAEDisposeDesc(&theReply);
		}
	}
	return anError;
}
pascal OSErr MoreAESendEventNoReturnValue(const AEIdleUPP pIdleProcUPP, const AppleEvent *pAppleEvent ){
	OSErr anErr = noErr;
	AppleEvent theReply = {
		typeNull, nil
	};
	AESendMode sendMode;

	if (nil == pIdleProcUPP)
		sendMode = kAENoReply;
	else
		sendMode = kAEWaitReply;

	anErr = AESend( pAppleEvent, &theReply, sendMode, kAENormalPriority, kNoTimeOut, pIdleProcUPP, nil );
	if ( ( noErr == anErr ) && ( kAEWaitReply == sendMode ) )
		anErr = MoreAEGetHandlerError(&theReply);

	MoreAEDisposeDesc( &theReply );

	return anErr;
}
pascal OSErr MoreAEGetHandlerError(const AppleEvent *pAEReply){
	OSErr anErr = noErr;
	OSErr handlerErr;

	DescType actualType;
	long actualSize;

	if ( pAEReply->descriptorType != typeNull ) {   // there's a reply, so there may be an error
		OSErr getErrErr = noErr;

		getErrErr = AEGetParamPtr( pAEReply, keyErrorNumber, typeShortInteger, &actualType,
		                           &handlerErr, sizeof( OSErr ), &actualSize );

		if ( getErrErr != errAEDescNotFound ) { // found an errorNumber parameter
			anErr = handlerErr;                                     // so return it's value
		}
	}
	return anErr;
}
pascal void MoreAEDisposeDesc(AEDesc *desc){
	OSStatus junk;

	MoreAssertQ(desc != nil);

	junk = AEDisposeDesc(desc);
	MoreAssertQ(junk == noErr);

	MoreAENullDesc(desc);
}
pascal void MoreAENullDesc(AEDesc *desc){
	MoreAssertQ(desc != nil);

	desc->descriptorType = typeNull;
	desc->dataHandle = nil;
}
Boolean MyAEIdleCallback(EventRecord *theEvent, SInt32 *sleepTime, RgnHandle *mouseRgn) {

	return 0;
}


std::list<std::string> QuerySpotlight(std::string arg1, bool throttle) {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	std::list<std::string> out;
	int count;
	NSString *Tag = [NSString stringWithUTF8String:arg1.c_str()];
	MDQueryRef query = MDQueryCreate(kCFAllocatorDefault,(CFStringRef)Tag,NULL,NULL);
	if (query == NULL) {
		return out;
	}
	Boolean ok = MDQueryExecute(query, kMDQuerySynchronous);
	if (!ok) {
		CFRelease(query);
		return out;
	}
	count = MDQueryGetResultCount(query);
	for (int i = 0; i < count; i++) {
		if (throttle) {
			usleep(10000);
		}
		MDItemRef item = (MDItemRef)MDQueryGetResultAtIndex(query, i);
		out.push_back(([(NSString *)MDItemCopyAttribute(item, kMDItemPath) UTF8String]));
	}
	CFRelease(query);
	// from NSString to string: string a = string([Tag UTF8String]);
	[pool drain];
	return out;
};
std::list<std::string> QueryComment(std::string arg1, bool throttle) {
	return QuerySpotlight(std::string(("kMDItemFinderComment == \"*@") + arg1 + ";*\""),throttle);
};