# Post-build: copia firmware.bin a firmware_bin/ con nombre por version y variante,
# listo para subir al cPanel (OTA remoto).
#   esp32dev          -> firmware_bin/Interfase_102.bin
#   esp32dev_display  -> firmware_bin/Interfase_102.1.bin
import os
import shutil

Import("env")


def _version_y_variante():
    version = "?"
    variante = ""
    for d in env.get("CPPDEFINES", []):
        nombre, valor = (d[0], d[1]) if isinstance(d, (list, tuple)) else (d, None)
        if nombre == "FW_VERSION":
            version = str(valor)
        elif nombre == "CON_DISPLAY":
            variante = ".1"
    return version, variante


def copiar_bin(source, target, env):
    version, variante = _version_y_variante()
    origen = str(target[0])
    destino_dir = os.path.join(env.subst("$PROJECT_DIR"), "firmware_bin")
    os.makedirs(destino_dir, exist_ok=True)
    destino = os.path.join(destino_dir, "Interfase_%s%s.bin" % (version, variante))
    shutil.copyfile(origen, destino)
    print("[copiar_bin] %s -> %s" % (origen, destino))


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", copiar_bin)
