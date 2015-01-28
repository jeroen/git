#include <git2.h>
#include <Rinternals.h>
#include "utils.h"

void fin_git_repository(SEXP ptr){
  if(!R_ExternalPtrAddr(ptr)) return;
  git_repository_free(R_ExternalPtrAddr(ptr));
  R_ClearExternalPtr(ptr);
}

int check_interrupt(){
  R_CheckUserInterrupt();
  return 0;
}

SEXP R_git_clone(SEXP url, SEXP path, SEXP branch){

  /* init stuff */
  git_repository *cloned_repo = NULL;
  git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;

  /* will this lead to mem leak? not sure */
  clone_opts.remote_callbacks.transfer_progress = check_interrupt;
  clone_opts.ignore_cert_errors = 1;

  /* specify branch to checkout */
  if(branch != R_NilValue)
    clone_opts.checkout_branch = translateCharUTF8(asChar(branch));

  /* try to clone */
  assert(git_clone(&cloned_repo, CHAR(asChar(url)), CHAR(asChar(path)), &clone_opts));

  /* check success */
  if (!cloned_repo) {
    error("Failed to clone repository.");
  }

  SEXP ptr = PROTECT(R_MakeExternalPtr(cloned_repo, R_NilValue, R_NilValue));
  R_RegisterCFinalizerEx(ptr, fin_git_repository, 1);
  setAttrib(ptr, R_ClassSymbol, mkString("git_repository"));
  UNPROTECT(1);
  return ptr;
}

SEXP R_git_repository_info(SEXP ptr){
  git_repository *repo = R_ExternalPtrAddr(ptr);
  if(!repo)
    error("Object has been disposed.");
  git_reference *head = NULL;
  assert(git_repository_head(&head, repo));

  git_branch_iterator *iter = NULL;
  git_reference *ref = NULL;
  git_branch_t type;

  /* iterate once just to count */
  int len = 0;
  git_branch_iterator_new(&iter, repo, GIT_BRANCH_ALL);
  while(GIT_ITEROVER != git_branch_next(&ref, &type, iter)) {
    len++;
  }
  git_branch_iterator_free(iter);

  /* iterate again */
  SEXP branches = PROTECT(allocVector(STRSXP, len));
  git_branch_iterator_new(&iter, repo, GIT_BRANCH_ALL);
  for(int i = 0; GIT_ITEROVER != git_branch_next(&ref, &type, iter); i++) {
    SET_STRING_ELT(branches, i, mkChar(git_reference_name(ref)));
  }
  git_branch_iterator_free(iter);

  SEXP list = PROTECT(allocVector(VECSXP, 4));
  SET_VECTOR_ELT(list, 0, mkString(git_repository_workdir(repo)));
  SET_VECTOR_ELT(list, 1, mkString(git_reference_name(head)));
  SET_VECTOR_ELT(list, 2, mkString(git_reference_shorthand(head)));
  SET_VECTOR_ELT(list, 3, branches);
  UNPROTECT(2);

  git_reference_free(ref);
  return list;
}
