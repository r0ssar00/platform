/*
 *  osxutils.cpp
 *  XTagRev
 *
 *  Created by Kevin Ross on 20/01/09.
 *
 */
using namespace std;
#include <string>
#include <list>
#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <cstdio>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sysexits.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>
#include "osxutils.h"
#include "platform.h"
string GetFileComment (string spath)
{
	const char * path = spath.c_str();
	OSErr	err = noErr;
    FSRef	fileRef;
    FSSpec	fileSpec;
	unsigned char * comment;
	char	cStrCmt[255] = "\0";
	AEIdleUPP inIdleProc = NewAEIdleUPP(&MyAEIdleCallback);
	
    ///////////// Make sure we can do as we please :) /////////////
	
	//see if the file in question exists and we can write it
	if (access(path, R_OK|F_OK) == -1)
	{
		return "";
	}
	
	//get file reference from path
	err = FSPathMakeRef((const UInt8 *)path, &fileRef, NULL);
	if (err != noErr)
	{
		return "";
	}
	
	//retrieve filespec from file ref
	err = FSGetCatalogInfo (&fileRef, NULL, NULL, NULL, &fileSpec, NULL);
	if (err != noErr)
	{
		return "";
	}
    
    
    ///////////// oK, now we can go about getting the comment /////////////
	
	// call the apple event routine. I'm not going to pretend I understand what's going on
	// in all those horribly kludgy functions, but at least it works.
	err = MoreFEGetComment(&fileSpec, comment, inIdleProc);
	if (err)
	{
		return "";
	}
	//convert pascal string to c string
	//strncpy((char *)&cStrCmt, (unsigned char *)&comment+1, comment[0]);
	comment=(unsigned char *)p2cstr((unsigned char *)&cStrCmt);
	
	//if there is no comment, we don't print out anything
	if (!strlen((char *)&cStrCmt))
		return "";
	
	//print out the comment
	if (!false)
		return string((const char *)&cStrCmt);
	else
		return string((const char *)&cStrCmt);
}
int SetFileComment (string spath, string scomment)
{
	const char *path = spath.c_str();
	const char *comment = scomment.c_str();
    OSErr	err = noErr;
    FSRef	fileRef;
    FSSpec	fileSpec;
	
    ///////////// Make sure we can do as we please :) /////////////
	
	//see if the file in question exists and we can write it
	if (access(path, R_OK|W_OK|F_OK) == -1)
	{
		return 1;
	}
	
	//get file reference from path
	err = FSPathMakeRef((const UInt8 *)path, &fileRef, NULL);
	if (err != noErr)
	{
		return 1;
	}
	
	//retrieve filespec from file ref
	err = FSGetCatalogInfo (&fileRef, NULL, NULL, NULL, &fileSpec, NULL);
	if (err != noErr)
	{
		return 1;
	}
    
    
    ///////////// oK, now we can go about setting the comment /////////////
    
    
	//being by setting MacOS X Finder Comment
	err = OSX_SetComment (&fileSpec, comment);
	if (err != noErr)
	{
		return 1;
	}
	return 0;
}
OSErr OSX_SetComment (FSSpec *fileSpec, const char *comment)
{
    OSErr	err = noErr;
    Str255	pCommentStr;
	
    if (strlen(comment) > MAX_COMMENT_LENGTH)
        return true;
	
    CopyCStringToPascal(comment,pCommentStr);
	
    err = MoreFESetComment(fileSpec, pCommentStr, NULL);
    
    return(err);
}

Boolean MyAEIdleCallback (
						  EventRecord * theEvent,
						  SInt32 * sleepTime,
						  RgnHandle * mouseRgn)
{
	
	return 0;
}

pascal OSErr MoreFEGetComment(const FSSpecPtr pFSSpecPtr,Str255 pCommentStr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};  //  If you always init AEDescs, it's always safe to dispose of them.
	AEDesc tAEDesc = {typeNull,NULL};
	OSErr anErr = noErr;
	
	if (NULL == pIdleProcUPP)  // the idle proc is required
	{
		return paramErr;
	}
	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&tAEDesc);
	if (anErr)
	{
		return paramErr;
	}
	if (noErr == anErr)
	{
		AEBuildError  tAEBuildError;
		
		anErr = AEBuildAppleEvent(
								  kAECoreSuite,kAEGetData,
								  typeApplSignature,&gFinderSignature,sizeof(OSType),
								  kAutoGenerateReturnID,kAnyTransactionID,
								  &tAppleEvent,&tAEBuildError,
								  "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)}",&tAEDesc);
		
        // always dispose of AEDescs when you are finished with them
        (void) MoreAEDisposeDesc(&tAEDesc);
		
		if (noErr == anErr)
		{  
			//  Send the event.
			anErr = MoreAESendEventReturnPString(pIdleProcUPP,&tAppleEvent,pCommentStr);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
		}
	}
	return anErr;
}  // end MoreFEGetComment
#if TARGET_API_MAC_CARBON
pascal OSErr MoreFEGetCommentCFString(const FSRefPtr pFSRefPtr, CFStringRef* pCommentStr,const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,NULL};  //  If you always init AEDescs, it's always safe to dispose of them.
	AEDesc tAEDesc = {typeNull,NULL};
	OSErr anErr = noErr;
	
	if (NULL == pIdleProcUPP)  // the idle proc is required
		return paramErr;
	
	
	anErr = MoreAEOCreateObjSpecifierFromFSRef(pFSRefPtr,&tAEDesc);
	
	if (noErr == anErr)
	{
		AEBuildError  tAEBuildError;
		
		anErr = AEBuildAppleEvent(
								  kAECoreSuite,kAEGetData,
								  typeApplSignature,&gFinderSignature,sizeof(OSType),
								  kAutoGenerateReturnID,kAnyTransactionID,
								  &tAppleEvent,&tAEBuildError,
								  "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)}",&tAEDesc);
		
        // always dispose of AEDescs when you are finished with them
        (void) MoreAEDisposeDesc(&tAEDesc);
		
		if (noErr == anErr)
		{  
#if 0  // Set this true to printf the Apple Event before you send it.
			Handle strHdl;
			anErr = AEPrintDescToHandle(&tAppleEvent,&strHdl);
			if (noErr == anErr)
			{
				char  nul  = '\0';
				PtrAndHand(&nul,strHdl,1);
				DisposeHandle(strHdl);
			}
#endif
			//  Send the event.
			anErr = MoreAESendEventReturnAEDesc(pIdleProcUPP,&tAppleEvent,typeUnicodeText,&tAEDesc);
			// always dispose of AEDescs when you are finished with them
			(void) MoreAEDisposeDesc(&tAppleEvent);
			if (noErr == anErr)
			{
				anErr = MoreAEGetCFStringFromDescriptor(&tAEDesc,pCommentStr);
				// always dispose of AEDescs when you are finished with them
				(void) MoreAEDisposeDesc(&tAEDesc);
			}
		}
	}
	return anErr;
}  // end MoreFEGetCommentCFString
#endif
pascal OSStatus MoreAESendEventReturnAEDesc(
											const AEIdleUPP    pIdleProcUPP,
											const AppleEvent  *pAppleEvent,
											const DescType    pDescType,
											AEDesc        *pAEDesc)
{
	OSStatus anError = noErr;
	
	//  No idle function is an error, since we are expected to return a value
	if (pIdleProcUPP == NULL)
		anError = paramErr;
	else
	{
		AppleEvent theReply = {typeNull,NULL};
		AESendMode sendMode = kAEWaitReply;
		
		anError = AESend(pAppleEvent, &theReply, sendMode, kAENormalPriority, kNoTimeOut, pIdleProcUPP, NULL);
		//  [ Don't dispose of the event, it's not ours ]
		if (noErr == anError)
		{
			anError = MoreAEGetHandlerError(&theReply);
			
			if (!anError && theReply.descriptorType != typeNull)
			{
				anError = AEGetParamDesc(&theReply, keyDirectObject, pDescType, pAEDesc);
			}
			MoreAEDisposeDesc(&theReply);
		}
	}
	return anError;
}  // MoreAESendEventReturnAEDesc

pascal OSStatus MoreAESendEventReturnPString(
											 const AEIdleUPP pIdleProcUPP,
											 const AppleEvent* pAppleEvent,
											 Str255 pStr255)
{
	DescType      actualType;
	Size         actualSize;
	OSStatus      anError;
	
	anError = MoreAESendEventReturnData(pIdleProcUPP,pAppleEvent,typePString,
										&actualType,pStr255,sizeof(Str255),&actualSize);
	
	if (errAECoercionFail == anError)
	{
		anError =  MoreAESendEventReturnData(pIdleProcUPP,pAppleEvent,typeChar,
											 &actualType,(Ptr) &pStr255[1],sizeof(Str255),&actualSize);
		if (actualSize < 256)
			pStr255[0] = (UInt8) actualSize;
		else
			anError = errAECoercionFail;
	}
	return anError;
}  // MoreAESendEventReturnPString



pascal OSStatus MoreAESendEventReturnData(
										  const AEIdleUPP    pIdleProcUPP,
										  const AppleEvent  *pAppleEvent,
										  DescType      pDesiredType,
										  DescType*      pActualType,
										  void*         pDataPtr,
										  Size        pMaximumSize,
										  Size         *pActualSize)
{
	OSStatus anError = noErr;
	
	//  No idle function is an error, since we are expected to return a value
	if (pIdleProcUPP == NULL)
		anError = paramErr;
	else
	{
		AppleEvent theReply = {typeNull,NULL};
		AESendMode sendMode = kAEWaitReply;
		
		anError = AESend(pAppleEvent, &theReply, sendMode, kAENormalPriority, kNoTimeOut, pIdleProcUPP, NULL);
		//  [ Don't dispose of the event, it's not ours ]
		if (noErr == anError)
		{
			anError = MoreAEGetHandlerError(&theReply);
			
			if (!anError && theReply.descriptorType != typeNull)
			{
				anError = AEGetParamPtr(&theReply, keyDirectObject, pDesiredType,
										pActualType, pDataPtr, pMaximumSize, pActualSize);
			}
			MoreAEDisposeDesc(&theReply);
		}
	}
	return anError;
}  // MoreAESendEventReturnData
pascal OSErr MoreAEGetCFStringFromDescriptor(const AEDesc* pAEDesc, CFStringRef* pCFStringRef)
{
	AEDesc    uniAEDesc = {typeNull, NULL};
	OSErr    anErr;
	
	if (NULL == pCFStringRef)
		return paramErr;
	
	anErr = AECoerceDesc(pAEDesc, typeUnicodeText, &uniAEDesc);
	if (noErr == anErr)
	{
		if (typeUnicodeText == uniAEDesc.descriptorType)
		{
			Size bufSize = AEGetDescDataSize(&uniAEDesc);
			Ptr buf = NewPtr(bufSize);
			
			if ((noErr == (anErr = MemError())) && (NULL != buf))
			{
				anErr = AEGetDescData(&uniAEDesc, buf, bufSize);
				if (noErr == anErr)
					*pCFStringRef = CFStringCreateWithCharacters(kCFAllocatorDefault, (UniChar*) buf, bufSize / (Size) sizeof(UniChar));
				
				DisposePtr(buf);
			}
		}
		MoreAEDisposeDesc(&uniAEDesc);
	}
	return (anErr);
}//end MoreAEGetCFStringFromDescriptor

///////////////////////////////////////////////////////////////////
// Check out the string passed as parameter with the -c option
// Make sure it's within reasonable bounds etc.
///////////////////////////////////////////////////////////////////
static char *GetCommentParameter (char *arg)
{
    static char 	comment[200];
    
    if (strlen(arg) > MAX_COMMENT_LENGTH)
    {
        return((char*)' ');
    }
    strcpy((char *)&comment, arg);
    return((char *)&comment);
}

pascal OSErr MoreFESetComment(const FSSpecPtr pFSSpecPtr, const Str255 pCommentStr, const AEIdleUPP pIdleProcUPP)
{
	AppleEvent tAppleEvent = {typeNull,nil};	//	If you always init AEDescs, it's always safe to dispose of them.
	AEBuildError	tAEBuildError;
	AEDesc 			tAEDesc = {typeNull,nil};
	OSErr anErr = noErr;
	
	anErr = MoreAEOCreateObjSpecifierFromFSSpec(pFSSpecPtr,&tAEDesc);
	if (noErr == anErr)
	{
		char* dataPtr = NewPtr(pCommentStr[0]);
		
		CopyPascalStringToC(pCommentStr,dataPtr);
		anErr = AEBuildAppleEvent(
								  kAECoreSuite,kAESetData,
								  typeApplSignature,&gFinderSignature,sizeof(OSType),
								  kAutoGenerateReturnID,kAnyTransactionID,
								  &tAppleEvent,&tAEBuildError,
								  "'----':obj {form:prop,want:type(prop),seld:type(comt),from:(@)},data:'TEXT'(@)",
								  &tAEDesc,dataPtr);
		
		DisposePtr(dataPtr);
		
		if (noErr == anErr)
		{	
			//	Send the event. In this case we don't care about the reply
			anErr = MoreAESendEventNoReturnValue(pIdleProcUPP,&tAppleEvent);
			(void) MoreAEDisposeDesc(&tAppleEvent);	// always dispose of AEDescs when you are finished with them
		}
	}
	return anErr;
}	// end MoreFESetComment

pascal OSStatus MoreAEOCreateObjSpecifierFromFSSpec(const FSSpecPtr pFSSpecPtr,AEDesc *pObjSpecifier)
{
	OSErr 		anErr = paramErr;
	
	if (nil != pFSSpecPtr)
	{
		FSRef tFSRef;
		
		anErr = FSpMakeFSRef(pFSSpecPtr,&tFSRef);
		if (noErr == anErr)
		{
			anErr = MoreAEOCreateObjSpecifierFromFSRef(&tFSRef,pObjSpecifier);
		}
	}
	return anErr;
}//end MoreAEOCreateObjSpecifierFromFSSpec

pascal OSStatus MoreAEOCreateObjSpecifierFromFSRef(const FSRefPtr pFSRefPtr,AEDesc *pObjSpecifier)
{
	OSErr 		anErr = paramErr;
	
	if (nil != pFSRefPtr)
	{
		CFURLRef tCFURLRef = CFURLCreateFromFSRef(kCFAllocatorDefault,pFSRefPtr);
		
		if (nil != tCFURLRef)
		{
			anErr = MoreAEOCreateObjSpecifierFromCFURLRef(tCFURLRef,pObjSpecifier);
			CFRelease(tCFURLRef);
		}
		else
			anErr = coreFoundationUnknownErr;
	}
	return anErr;
}

pascal OSStatus MoreAEOCreateObjSpecifierFromCFURLRef(const CFURLRef pCFURLRef,AEDesc *pObjSpecifier)
{
	OSErr 		anErr = paramErr;
	
	if (nil != pCFURLRef)
	{
		Boolean 		isDirectory = CFURLHasDirectoryPath(pCFURLRef);
		CFStringRef		tCFStringRef = CFURLCopyFileSystemPath(pCFURLRef, kCFURLHFSPathStyle);
		AEDesc 			containerDesc = {typeNull, NULL};
		AEDesc 			nameDesc = {typeNull, NULL};
		UniCharPtr		buf = nil;
		
		if (nil != tCFStringRef)
		{
			Size	bufSize = (CFStringGetLength(tCFStringRef) + (isDirectory ? 1 : 0)) * sizeof(UniChar);
			
			buf = (UniCharPtr) NewPtr(bufSize);
			
			if ((anErr = MemError()) == noErr)
			{
				CFStringGetCharacters(tCFStringRef, CFRangeMake(0,bufSize/2), buf);
				if (isDirectory) (buf)[(bufSize-1)/2] = (UniChar) 0x003A;				
			}
		} else
			anErr = coreFoundationUnknownErr;
		
		if (anErr == noErr)
			anErr = AECreateDesc(typeUnicodeText, buf, GetPtrSize((Ptr) buf), &nameDesc);
		if (anErr == noErr)
			if (isDirectory) {
				anErr = CreateObjSpecifier(cFolder,&containerDesc,formName,&nameDesc,false,pObjSpecifier);
			} else {
				anErr = CreateObjSpecifier(cFile,&containerDesc,formName,&nameDesc,false,pObjSpecifier);
			}
		
		MoreAEDisposeDesc(&nameDesc);
		
		if (buf)
			DisposePtr((Ptr)buf);
	}
	return anErr;
}//end MoreAEOCreateObjSpecifierFromCFURLRef
pascal	OSErr	MoreAESendEventNoReturnValue (const AEIdleUPP pIdleProcUPP, const AppleEvent* pAppleEvent )
{
	OSErr		anErr = noErr;
	AppleEvent	theReply = {typeNull,nil};
	AESendMode	sendMode;
	
	if (nil == pIdleProcUPP)
		sendMode = kAENoReply;
	else
		sendMode = kAEWaitReply;
	
	anErr = AESend( pAppleEvent, &theReply, sendMode, kAENormalPriority, kNoTimeOut, pIdleProcUPP, nil );
	if ((noErr == anErr) && (kAEWaitReply == sendMode))
		anErr = MoreAEGetHandlerError(&theReply);
	
	MoreAEDisposeDesc( &theReply );
	
	return anErr;
}

pascal	OSErr	MoreAEGetHandlerError(const AppleEvent* pAEReply)
{
	OSErr		anErr = noErr;
	OSErr		handlerErr;
	
	DescType	actualType;
	long		actualSize;
	
	if ( pAEReply->descriptorType != typeNull )	// there's a reply, so there may be an error
	{
		OSErr	getErrErr = noErr;
		
		getErrErr = AEGetParamPtr( pAEReply, keyErrorNumber, typeShortInteger, &actualType,
								  &handlerErr, sizeof( OSErr ), &actualSize );
		
		if ( getErrErr != errAEDescNotFound )	// found an errorNumber parameter
		{
			anErr = handlerErr;					// so return it's value
		}
	}
	return anErr;
}//end MoreAEGetHandlerError

pascal void MoreAEDisposeDesc(AEDesc* desc)
{
	OSStatus junk;
	
	MoreAssertQ(desc != nil);
	
	junk = AEDisposeDesc(desc);
	MoreAssertQ(junk == noErr);
	
	MoreAENullDesc(desc);
}//end MoreAEDisposeDesc

pascal void MoreAENullDesc(AEDesc* desc)
{
	MoreAssertQ(desc != nil);
	
	desc->descriptorType = typeNull;
	desc->dataHandle     = nil;
}//end MoreAENullDesc


int CreateAlias (const char *srcPath, const char *destPath) {
    OSErr		err;
    FSSpec		sourceFSSpec, destFSSpec;
    FSRef		srcRef, destRef;
    OSType		srcFileType = (OSType)NULL;
	OSType		srcCreatorType = (OSType)NULL;
    FInfo		srcFinderInfo, destFinderInfo;
    int			fd;
    SInt16		rsrcRefNum;
    IconRef				srcIconRef;
    IconFamilyHandle	srcIconFamily;
    SInt16				theLabel;
    AliasHandle		alias;
    short			isSrcFolder;
    isSrcFolder = UnixIsFolder(srcPath);
    if (isSrcFolder == -1) {
        return(EX_IOERR);
    }
	err = FSPathMakeRef((const UInt8 *)srcPath, &srcRef, NULL);
	if (err != noErr) {
		return(EX_IOERR);
	}
	fd = open(destPath, O_CREAT | O_TRUNC | O_WRONLY);
	if (fd == -1) {
		return(EX_CANTCREAT);
	}
	close(fd);
	err = FSPathMakeRef((const UInt8 *)destPath, &destRef, NULL);
	if (err != noErr) {
		return(EX_IOERR);
	}
	err = FSGetCatalogInfo (&srcRef, NULL, NULL, NULL, &sourceFSSpec, NULL);
	if (err != noErr) {
		return(EX_IOERR);
	}
	err = FSGetCatalogInfo (&destRef, NULL, NULL, NULL, &destFSSpec, NULL);
	if (err != noErr) {
		return(EX_IOERR);
	}
	unlink(destPath);
	if (!isSrcFolder) {
		err = FSpGetFInfo (&sourceFSSpec, &srcFinderInfo);
		if (err != noErr) {
			return(EX_IOERR);
		}
		srcFileType = srcFinderInfo.fdType;
		srcCreatorType = srcFinderInfo.fdCreator;
	}
	err = GetIconRefFromFile (&sourceFSSpec, &srcIconRef, &theLabel);
	IconRefToIconFamily (srcIconRef, kSelectorAllAvailableData, &srcIconFamily);
	if (true) {
		FSpCreateResFile(&destFSSpec, 'TEMP', 'TEMP', smSystemScript);
		if ((err = ResError()) != noErr) {
			return(EX_CANTCREAT);
		}
		err = NewAlias(&destFSSpec, &sourceFSSpec, &alias);
		if (err != noErr) {
			return(EX_CANTCREAT);
		}
		rsrcRefNum = FSpOpenResFile(&destFSSpec, fsRdWrPerm);
		if (rsrcRefNum == -1)  { 
			err = ResError(); 
			return(EX_IOERR);
		}
		UseResFile(rsrcRefNum);
		AddResource((Handle) alias, rAliasType, 0, destFSSpec.name);
		if ((err = ResError()) != noErr) {
			return(EX_IOERR);
		}
		//write the custom icon data
		AddResource( (Handle)srcIconFamily, kIconFamilyType, kCustomIconResource, (const unsigned char*)"\p");
		CloseResFile(rsrcRefNum);
	} else {
		FSNewAliasMinimal (&srcRef, &alias);
		if (alias == NULL) {
			return(EX_IOERR);
		}
		if (isSrcFolder)
			FSpCreateResFile(&destFSSpec, 'MACS', 'fdrp', -1);
		else
			FSpCreateResFile(&destFSSpec, '    ', '    ', -1);
		
		rsrcRefNum = FSpOpenResFile (&destFSSpec, 3);
		AddResource ((Handle)alias, 'alis', 0, NULL);
		AddResource( (Handle)srcIconFamily, kIconFamilyType, kCustomIconResource,  (const unsigned char*)"\p");
		CloseResFile(rsrcRefNum);				
	}
	err = FSpGetFInfo (&destFSSpec, &destFinderInfo);
	if (err != noErr) {
		return(EX_IOERR);
	}
	destFinderInfo.fdFlags = destFinderInfo.fdFlags | 0x8000 | kHasCustomIcon;
	
	if (!isSrcFolder && !true) {
		destFinderInfo.fdType = srcFileType;
		destFinderInfo.fdCreator = srcCreatorType;
	}
	err = FSpSetFInfo (&destFSSpec, &destFinderInfo);
	if (err != noErr) {
		return(EX_IOERR);
	}
	return 0;
}

string ResolveAlias (string path)
{	
	FSRef * fileRef;
	FSPathMakeRef((unsigned char *)path.c_str(), fileRef, NULL);
    OSErr	err = noErr;
    static char	srcPath[2048];
    Boolean	isAlias, isFolder;
	FSRef	aliasRef;
    
    //make sure we're dealing with an alias
    err = FSIsAliasFile (fileRef, &isAlias, &isFolder);
    if (err != noErr)
    {
        return "";
    }
    if (!isAlias)
    {
        return "";
    }
    
    //resolve alias --> get file reference to file
    err = FSResolveAliasFile (fileRef, TRUE, &isFolder, &isAlias);
    if (err != noErr)
    {
        return "";
    }
    
    //get path to file that alias points to
    err = FSMakePath(*fileRef, (UInt8 *)&srcPath, strlen(srcPath));
    if (err != noErr)
	{
		return "";
    }
	return string(srcPath);
}
static OSStatus FSMakePath(FSRef fileRef, UInt8 *path, UInt32 maxPathSize)
{
    OSStatus result;
	
    result = FSRefMakePath(&fileRef, path, 2000);
	
    return ( result );
}
short UnixIsFolder (const char *path) {
    struct stat filestat;
    short err;
    err = stat(path, &filestat);
    if (err == -1)
        return err;
	
    if(S_ISREG(filestat.st_mode) != 1)
        return true;
    else 
        return false;
}
