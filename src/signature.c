#include <git2.h>
#include <Rinternals.h>
#include "utils.h"

void fin_git_signature(SEXP ptr){
  if(!R_ExternalPtrAddr(ptr)) return;
  git_signature_free(R_ExternalPtrAddr(ptr));
  R_ClearExternalPtr(ptr);
}

SEXP R_git_signature_now(SEXP name, SEXP email) {
  git_signature *sig = NULL;
  assert(git_signature_now(&sig, translateCharUTF8(asChar(name)), translateCharUTF8(asChar(email))));
  SEXP ptr = PROTECT(R_MakeExternalPtr(sig, R_NilValue, R_NilValue));
  R_RegisterCFinalizerEx(ptr, fin_git_signature, 1);
  setAttrib(ptr, R_ClassSymbol, mkString("git_signature"));
  UNPROTECT(1);
  return ptr;
}

SEXP R_signature_to_list(SEXP ptr){
  git_signature* sig = R_ExternalPtrAddr(ptr);
  if(!sig)
    error("Object has been disposed.");
  SEXP list = PROTECT(allocVector(VECSXP, 3));
  SET_VECTOR_ELT(list, 0, mkString(sig->name));
  SET_VECTOR_ELT(list, 1, mkString(sig->email));
  SET_VECTOR_ELT(list, 2, ScalarInteger(sig->when.time));
  UNPROTECT(1);
  return list;
}
