/*
    Copyright (C) 2018-2026 Alexander Schoepe, Bochum, DE, <alx.tcl(at)sowaswie.de>
    Copyright (C) 2018 Joerg Mehring, Bochum, DE <j.mehring@sesam-gmbh.de>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the project nor the names of its contributors may be used
       to endorse or promote products derived from this software without specific
       prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT
    SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

 */


#include <tcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc64.h"
#include "manifest.h"


static int Crc64ecma_Cmd( ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[] ) {
  unsigned char *data;
  Tcl_Size dataLen;
  Tcl_WideInt initValue = 0ll;
  uint64_t crc64Value;

  if (objc < 2 || objc > 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "string ?initValue?");
    return TCL_ERROR;
  }

  if (objc == 3) {
    if (Tcl_GetWideIntFromObj(interp, objv[2], &initValue) == TCL_ERROR) {
      Tcl_SetObjResult(interp, Tcl_ObjPrintf("wrong # initValue unsigned integer required"));
      return TCL_ERROR;
    }
  }

  data = Tcl_GetByteArrayFromObj(objv[1], &dataLen);
  crc64Value = lzma_crc64(data, (uint64_t) dataLen, (uint64_t) initValue);

  Tcl_SetObjResult(interp, Tcl_NewWideIntObj((Tcl_WideInt) crc64Value));
  return TCL_OK;
}


/*
 *-----------------------------------------------------------------------------
 *
 * Tclcrc_Manifest --
 *
 *  Returns build manifest information as a Tcl dictionary.
 *
 * Results:
 *  A standard Tcl result.
 *  Interp result is set to a dictionary containing manifest data.
 *
 *-----------------------------------------------------------------------------
 */

static int
Tclcrc_Manifest(
  void *dummy,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *const objv[])
{
  static const char vTag[] = "@(#)tclcrc.c v" RELEASE_VERSION " " MANIFEST_VERSION " " MANIFEST_DATE " (BSD 3 License) Alexander Schoepe, Bochum, DE";
  (void)vTag;

  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, "");
    return TCL_ERROR;
  }

  Tcl_Obj *dictPtr = Tcl_NewDictObj();

  Tcl_DictObjPut(interp, dictPtr,
    Tcl_NewStringObj("version", -1),
    Tcl_NewStringObj(RELEASE_VERSION, -1));

  Tcl_DictObjPut(interp, dictPtr,
    Tcl_NewStringObj("date", -1),
    Tcl_NewStringObj(MANIFEST_DATE, -1));

  Tcl_DictObjPut(interp, dictPtr,
    Tcl_NewStringObj("check-in", -1),
    Tcl_NewStringObj(MANIFEST_VERSION, -1));

  Tcl_DictObjPut(interp, dictPtr,
    Tcl_NewStringObj("build-hash", -1),
    Tcl_NewStringObj(FOSSIL_BUILD_HASH, -1));

  Tcl_DictObjPut(interp, dictPtr,
    Tcl_NewStringObj("uuid", -1),
    Tcl_NewStringObj(MANIFEST_UUID, -1));

  Tcl_SetObjResult(interp, dictPtr);
  return TCL_OK;
}


#ifndef STRINGIFY
#  define STRINGIFY(x) STRINGIFY1(x)
#  define STRINGIFY1(x) #x
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
DLLEXPORT int
Tclcrc_Init(Tcl_Interp *interp) {
   Tcl_CmdInfo info;

  // Support any Tcl version from 8.5.0 to 9.x.x, the upper bound is exclusiv.
  if (Tcl_InitStubs(interp, "8.5-10", 0) == NULL) {
    return TCL_ERROR;
  }

  // Build Info Command - command to return build info for package
  if (Tcl_GetCommandInfo(interp, "::tcl::build-info", &info)) {
    Tcl_CreateObjCommand(interp, "::tclcrc::build-info",
      info.objProc, (void *)(
		    PACKAGE_VERSION "+" STRINGIFY(MANIFEST_UUID)
#if defined(__clang__) && defined(__clang_major__)
			    ".clang-" STRINGIFY(__clang_major__)
#if __clang_minor__ < 10
			    "0"
#endif
			    STRINGIFY(__clang_minor__)
#endif
#if defined(__cplusplus) && !defined(__OBJC__)
			    ".cplusplus"
#endif
#ifndef NDEBUG
			    ".debug"
#endif
#if !defined(__clang__) && !defined(__INTEL_COMPILER) && defined(__GNUC__)
			    ".gcc-" STRINGIFY(__GNUC__)
#if __GNUC_MINOR__ < 10
			    "0"
#endif
			    STRINGIFY(__GNUC_MINOR__)
#endif
#ifdef __INTEL_COMPILER
			    ".icc-" STRINGIFY(__INTEL_COMPILER)
#endif
#ifdef TCL_MEM_DEBUG
			    ".memdebug"
#endif
#if defined(_MSC_VER)
			    ".msvc-" STRINGIFY(_MSC_VER)
#endif
#ifdef USE_NMAKE
			    ".nmake"
#endif
#ifndef TCL_CFG_OPTIMIZED
			    ".no-optimize"
#endif
#ifdef __OBJC__
			    ".objective-c"
#if defined(__cplusplus)
			    "plusplus"
#endif
#endif
#ifdef TCL_CFG_PROFILED
			    ".profile"
#endif
#ifdef PURIFY
			    ".purify"
#endif
#ifdef STATIC_BUILD
			    ".static"
#endif
		), NULL);
  }

  Tcl_CreateObjCommand(interp, "crc64ecma", (Tcl_ObjCmdProc *) Crc64ecma_Cmd, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateObjCommand(interp, "Tclcrc::manifest", Tclcrc_Manifest, (void *)NULL, (Tcl_CmdDeleteProc *)NULL);

  if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
    return TCL_ERROR;
  }

  return TCL_OK;
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */