{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs}:
  
  let 
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
    libs = with pkgs; [
        pkg-config
        gcc
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
        export CC=${pkgs.gcc}/bin/gcc
        export CXX=${pkgs.gcc}/bin/g++
        export QT_STYLE_OVERRIDE=adwaita-dark
        export QT_QPA_PLATFORMTHEME=gtk3
      '';
    };
  };
}