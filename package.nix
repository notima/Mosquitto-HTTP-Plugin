# The plugin as a Nix package: lib/plugin.so.
#
# Built against the headers of the mosquitto package it is given, instead of the
# vendored mosquitto sub-module, so that it always matches the broker that loads it.
# The Makefile's include paths into the sub-module are then simply missing, which the
# compiler ignores.
{
  lib,
  stdenv,
  mosquitto,
  uthash,
}:

stdenv.mkDerivation {
  pname = "mosquitto-http-plugin";
  version = "0-unstable";

  src = lib.fileset.toSource {
    root = ./.;
    fileset = lib.fileset.unions [ ./Makefile ./src ];
  };

  buildInputs = [ mosquitto uthash ];

  buildFlags = [ "compile" ];

  installPhase = ''
    runHook preInstall
    install -Dm755 bin/plugin.so $out/lib/plugin.so
    runHook postInstall
  '';

  meta = {
    description = "Mosquitto plugin for authentication and access control through an HTTP API";
    platforms = lib.platforms.linux;
  };
}
