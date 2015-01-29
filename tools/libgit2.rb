require "formula"

class Libgit2 < Formula
  homepage "https://libgit2.github.com/"
  url "https://github.com/libgit2/libgit2/archive/v0.21.3.tar.gz"
  sha1 "d116cb15f76edf2283c85da40389e4fecc8d5aeb"

  head "https://github.com/libgit2/libgit2.git"

  depends_on "cmake" => :build
  depends_on "libssh2" => :optional
  depends_on "openssl"

  def install
    args = std_cmake_args
    args << "-DBUILD_TESTS=NO"
    args << "-DBUILD_SHARED_LIBS=OFF"

    mkdir "build" do
      system "cmake", "..", *args
      system "make", "install"
    end
  end
end
