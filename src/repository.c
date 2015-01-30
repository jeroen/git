#include <git2.h>
#include <Rinternals.h>
#include "utils.h"

void fin_git_repository(SEXP ptr){
  if(!R_ExternalPtrAddr(ptr)) return;
  git_repository_free(R_ExternalPtrAddr(ptr));
  R_ClearExternalPtr(ptr);
}

SEXP repo2ptr(git_repository *repo){
  SEXP ptr = PROTECT(R_MakeExternalPtr(repo, R_NilValue, R_NilValue));
  R_RegisterCFinalizerEx(ptr, fin_git_repository, 1);
  setAttrib(ptr, R_ClassSymbol, mkString("git_repository"));
  UNPROTECT(1);
  return ptr;
}

int check_interrupt(){
  R_CheckUserInterrupt();
  return 0;
}

SEXP R_git_clone(SEXP url, SEXP path, SEXP branch){

  /* init stuff */
  git_repository *repo = NULL;
  git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;

  /* will this lead to mem leak? not sure */
  #if LIBGIT2_VER_MAJOR > 0 || LIBGIT2_VER_MINOR >= 20
  clone_opts.remote_callbacks.transfer_progress = check_interrupt;
  #endif

  /* dont verify certificate on windows */
  #ifdef _WIN32
  clone_opts.ignore_cert_errors = 1;
  #endif

  /* specify branch to checkout */
  if(branch != R_NilValue)
    clone_opts.checkout_branch = translateCharUTF8(asChar(branch));

  /* try to clone */
  assert(git_clone(&repo, CHAR(asChar(url)), CHAR(asChar(path)), &clone_opts));

  /* check success */
  if (!repo) {
    error("Failed to clone repository.");
  }

  return repo2ptr(repo);
}

SEXP R_git_repository_info(SEXP ptr){
  git_repository *repo = R_ExternalPtrAddr(ptr);
  if(!repo)
    error("Object has been disposed.");
  git_reference *head = NULL;
  assert(git_repository_head(&head, repo));

  git_strarray ref_list;
  assert(git_reference_list(&ref_list, repo));
  SEXP refs = PROTECT(allocVector(STRSXP, ref_list.count));
  for(int i = 0; i < ref_list.count; i++){
    SET_STRING_ELT(refs, i, mkChar(ref_list.strings[i]));
  }

  SEXP list = PROTECT(allocVector(VECSXP, 4));
  SET_VECTOR_ELT(list, 0, mkString(git_repository_workdir(repo)));
  SET_VECTOR_ELT(list, 1, mkString(git_reference_name(head)));
  SET_VECTOR_ELT(list, 2, mkString(git_reference_shorthand(head)));
  SET_VECTOR_ELT(list, 3, refs);
  UNPROTECT(2);

  return list;
}

SEXP R_git_init(SEXP path, SEXP bare){
  git_repository *repo = NULL;
  assert(git_repository_init(&repo, translateCharUTF8(asChar(path)), asLogical(bare)));
  return repo2ptr(repo);
}

SEXP R_git_repository_open(SEXP path){
  git_repository *repo = NULL;
  assert(git_repository_open(&repo, translateCharUTF8(asChar(path))));
  return repo2ptr(repo);
}

SEXP R_git_checkout(SEXP ptr, SEXP ref, SEXP force){
  git_repository *repo = R_ExternalPtrAddr(ptr);
  if(!repo)
    error("Object has been disposed.");

  /* checkout */
  #if LIBGIT2_VER_MAJOR > 0 || LIBGIT2_VER_MINOR >= 21
  git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
  #else
  git_checkout_opts checkout_opts = GIT_CHECKOUT_OPTS_INIT;
  #endif

  if(asLogical(force)){
    checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;
  } else {
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;
  }

  const char *refstring = translateCharUTF8(asChar(ref));
  if(!git_reference_is_valid_name(refstring)) {
    git_reference *newref;
    assert(git_reference_dwim(&newref, repo, refstring));
    refstring = git_reference_name(newref);

    /* does not work: create local branch from remote */
    /*
    if(git_reference_is_remote(newref)){
      Rprintf("is remote\n");
      git_commit *newcommit;
      git_oid oid;
      assert(git_reference_name_to_id(&oid, repo, git_reference_name(newref)));
      assert(git_commit_lookup(&newcommit, repo, &oid));
      assert(git_branch_create(NULL, repo, refstring, newcommit, 0, NULL, NULL));
    } else {
      Rprintf("is local\n");
      refstring = git_reference_name(newref);
    }
    */
  }

  #if LIBGIT2_VER_MAJOR > 0 || LIBGIT2_VER_MINOR >= 21
  assert(git_repository_set_head(repo, refstring, NULL, NULL));
  #else
  assert(git_repository_set_head(repo, refstring));
  #endif

  assert(git_checkout_head(repo, &checkout_opts));
  assert(git_repository_set_namespace(repo, "foo"));
  return mkString(refstring);
}
