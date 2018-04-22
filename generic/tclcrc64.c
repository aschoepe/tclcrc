/*
    Copyright (C) 2018 Alexander Schoepe, Bochum, DE, <schoepe@users.sourceforge.net>
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


static int Crc64ecma_Cmd( ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[] ) {
  unsigned char *data;
  int dataLen;
  uint64_t initValue = 0ll, crc64Value;

  if (objc < 2 || objc > 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "string ?initValue?");
    return TCL_ERROR;
  }

  if (objc == 3) {
    if (Tcl_GetWideIntFromObj(interp, objv[2], (long *) &initValue) == TCL_ERROR) {
      Tcl_SetObjResult(interp, Tcl_ObjPrintf("wrong # initValue unsigned integer required"));
      return TCL_ERROR;
    }
  }
    
  data = Tcl_GetByteArrayFromObj(objv[1], &dataLen);
  crc64Value = lzma_crc64(data, (uint64_t) dataLen, initValue);

  Tcl_SetObjResult(interp, Tcl_NewWideIntObj(crc64Value));
  return TCL_OK;
}


int Tclcrc_Init(Tcl_Interp *interp) {
  if (Tcl_InitStubs(interp, "8.6", 0) == NULL) {
    return TCL_ERROR;
  }
  if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
    return TCL_ERROR;
  }
  Tcl_CreateObjCommand(interp, "crc64ecma", (Tcl_ObjCmdProc *) Crc64ecma_Cmd, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

  return TCL_OK;
}
