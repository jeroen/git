#include <git2.h>
#include <Rinternals.h>

SEXP R_init(){
  git_threads_init();
  return R_NilValue;
}
