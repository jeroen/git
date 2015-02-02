require "formula"

class Libgit2 < Formula
  homepage "https://libgit2.github.com/"
  url "https://github.com/libgit2/libgit2/archive/v0.22.1.tar.gz"
  sha1 "20223ca392bb8f5d891cd9944eec62c6a6e21dd5"

  keg_only "dont link global libs"
  head "https://github.com/libgit2/libgit2.git"

  depends_on "cmake" => :build
  depends_on "libssh2" => :optional
  depends_on "openssl"

  def install
    args = std_cmake_args
    args << "-DBUILD_TESTS=NO"
    args << "-DBUILD_SHARED_LIBS=OFF"
    args << "-DBUILD_CLAR=OFF"

    mkdir "build" do
      system "cmake", "..", *args
      system "make", "install"
    end
  end
end
