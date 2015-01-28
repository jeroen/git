#include <git2.h>
#include <Rinternals.h>

void assert(int out){
  if (out) {
    const git_error *err = giterr_last();
    if (err)
      error("ERROR %d: %s\n", err->klass, err->message);
    else
      error("ERROR %d: no detailed info\n", out);
  }
}
