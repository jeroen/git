class Libgit2 < Formula
  homepage "https://libgit2.github.com/"
  url "https://github.com/libgit2/libgit2/archive/v0.22.0.tar.gz"
  sha1 "a37dc29511422eec9828e129ad057e77ca962c5e"

  head "https://github.com/libgit2/libgit2.git"

  option :universal

  depends_on "cmake" => :build
  depends_on "libssh2" => :optional

  def install
    args = std_cmake_args
    args << "-DBUILD_CLAR=NO" # Don't build tests.
    args << "-DBUILD_SHARED_LIBS=OFF"

    if build.universal?
      ENV.universal_binary
      args << "-DCMAKE_OSX_ARCHITECTURES=#{Hardware::CPU.universal_archs.as_cmake_arch_flags}"
    end

    mkdir "build" do
      system "cmake", "..", *args
      system "make", "install"
    end
  end
end
