{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs}:
  
  let 
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
    libs = with pkgs; [
        pkg-config
        clang_15
        adwaita-qt6
        libGL
        libGLU
        gvfs
        glib
        cmake
        cmake-format
        xorg.libX11.dev
        glfw
      ];
  in {
    devShell.x86_64-linux = pkgs.mkShell {
      packages = [pkgs.glxinfo];
      buildInputs = libs;
      LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath libs;
      shellHook = ''
        # clang 15 is the default compiler version on QtCreator on MacOS, so using that here to be consistent with their behavior
        export CC=${pkgs.clang_15}/bin/clang
        export CXX=${pkgs.clang_15}/bin/clang++
        export CMAKE_PREFIX_PATH=${pkgs.glfw}:$CMAKE_PREFIX_PATH
        export QT_STYLE_OVERRIDE=adwaita-dark
        export QT_QPA_PLATFORMTHEME=gtk3
      '';
    };
  };
}