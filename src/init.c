#include <git2.h>
#include <Rinternals.h>

SEXP R_init(){
  #if LIBGIT2_VER_MAJOR == 0 && LIBGIT2_VER_MINOR < 22
  git_threads_init();
  #endif
  return R_NilValue;
}
