#' @useDynLib git R_git_clone
#' @export
git_clone <- function(url, path, branch = NULL){
  stopifnot(is.character(url))
  stopifnot(is.character(path))
  stopifnot(is.null(branch) || is.character(branch))
  path <- normalizePath(path.expand(path), mustWork = FALSE)
  .Call(R_git_clone, url, path, branch)
}

#' @useDynLib git R_git_init
#' @export
git_init <- function(path, bare = FALSE){
  stopifnot(is.character(path))
  path <- normalizePath(path.expand(path), mustWork = FALSE)
  .Call(R_git_init, path, bare)
}

#' @useDynLib git R_git_repository_open
#' @export
git_open <- function(path, bare = FALSE){
  stopifnot(is.character(path))
  path <- normalizePath(path.expand(path), mustWork = FALSE)
  .Call(R_git_repository_open, path, bare)
}

#' @export
#' @useDynLib git R_git_checkout
git_checkout <- function(repo, ref, force = FALSE){
  stopifnot(is(repo, "git_repository"))
  .Call(R_git_checkout, repo, ref, force)
}

#' @useDynLib git R_git_repository_info
git_repository_info <- function(repo){
  stopifnot(is(repo, "git_repository"))
  out <- .Call(R_git_repository_info, repo)
  names(out) <- c("workdir", "head_name", "head_shorthand", "references")
  return(out)
}

#' @export
print.git_repository <- function(x, ...){
  out <- git_repository_info(x)
  cat("git_repository:\n - Working dir:", out$workdir, "\n - Branches:", out$references, "\n - Head:", out$head_shorthand, "\n");
}

#' @useDynLib git R_git_signature_now
#' @export
git_signature_now <- function(name, email){
  stopifnot(is.character(name))
  stopifnot(is.character(email))
  .Call(R_git_signature_now, name, email)
}

#' @useDynLib git R_signature_to_list
signature_to_list <- function(sig){
  out <- .Call(R_signature_to_list, sig)
  names(out) <- c("name", "email", "when")
  class(out[[3]]) <- c("POSIXct", "POSIXt")
  out
}

#' @export
print.git_signature <- function(x, ...){
  out <- signature_to_list(x)
  cat("git_signature:\n - Name:", out$name, "\n - Email:", out$email, "\n - When:", format(out$when), "\n")
}

#' @useDynLib git R_init
init <- function(){
  .Call(R_init)
}
