{
  description = "laser - another ls alternative but configurable with lua!";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };

        pname = "laser";
        version = "1.6.0";

        src = pkgs.fetchFromGitHub {
          owner = "jmattaa";
          repo = pname;
          rev = "v1.6.0";
          sha256 = "09fmjmilbfr0q6l72y1i131bizsp3kc563npmydn920m4kahbpfa";
        };

      in
      {
        packages.${pname} = pkgs.stdenv.mkDerivation {
          inherit pname version src;

          nativeBuildInputs = [ pkgs.cmake ];
          buildInputs = [ pkgs.lua pkgs.libgit2 ];

          cmakeFlags = [
            "-DLUA_SCRIPTS_INSTALL_PATH=${placeholder "out"}/lsr"
          ];

          installPhase = ''
            runHook preInstall
            cmake --install . --prefix=$out
            runHook postInstall
          '';

          postInstall = ''
            mkdir -p $out/share/bash-completion/completions
            mkdir -p $out/share/zsh/site-functions
            mkdir -p $out/share/fish/vendor_completions.d
            $out/bin/lsr --completions bash > $out/share/bash-completion/completions/lsr
            $out/bin/lsr --completions zsh > $out/share/zsh/site-functions/_lsr
            $out/bin/lsr --completions fish > $out/share/fish/vendor_completions.d/lsr.fish
          '';
        };

        defaultPackage = self.packages.${system}.${pname};

        apps.default = flake-utils.lib.mkApp {
          drv = self.packages.${system}.${pname};
          name = "lsr";
        };

        devShells.default = pkgs.mkShell {
          nativeBuildInputs = [ pkgs.cmake ];
          buildInputs = [ pkgs.lua pkgs.libgit2 ];
        };
      });
}
