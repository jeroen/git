# Static link against libgit2 builds
if(!file.exists("../windows/libgit2-0.21.5/include/git2.h")){
  setInternet2()
  download.file("http://jeroenooms.github.io/git/libgit2-0.21.5.zip", "lib.zip", quiet = TRUE)
  dir.create("../windows", showWarnings = FALSE)
  unzip("lib.zip", exdir = "../windows")
  unlink("lib.zip")
}
