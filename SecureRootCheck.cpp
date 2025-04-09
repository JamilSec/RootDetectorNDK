#include <jni.h>
#include <unistd.h>                // Para funciones como access(), getuid()
#include <sys/stat.h>              // Para stat()
#include <cstring>                 // Para strcmp, strstr
#include <fcntl.h>                 // Para operaciones de archivos
#include <sys/system_properties.h> // Para obtener propiedades del sistema
#include <cstdio>                  // Para fopen, fgets, FILE*
#include <cstdlib>                 // Para popen, pclose

namespace RootDetector {

    /**
     * Verifica la existencia de binarios comunes de 'su' en paths típicos
     * utilizados por herramientas de root como Magisk, SuperSU, KingRoot, etc.
     */
    bool checkSuBinary() {
        const char* suPaths[] = {
            "/data/local/bin/su",
            "/data/local/su",
            "/sbin/su",
            "/system/bin/su",
            "/system/bin/.ext/su",
            "/system/xbin/su",
            "/system/xbin/mu",
            "/system/xbin/ku",
            "/system/sd/xbin/su",
            "/system/usr/we-need-root/su",
            "/vendor/bin/su",
            "/su/bin/su",
            "/magisk/.core/bin/su"
        };

        for (auto path : suPaths) {
            if (access(path, F_OK) == 0) {
                return true; // Binario de su encontrado
            }
        }
        return false;
    }

    /**
     * Verifica si existen archivos o carpetas específicas relacionadas a Magisk,
     * KernelSU y otros gestores de root modernos.
     */
    bool checkMagiskFiles() {
        const char* magiskPaths[] = {
            "/sbin/.magisk",
            "/dev/magisk",
            "/dev/.magisk.unblock",
            "/data/adb/magisk",
            "/cache/.disable_magisk",
            "/data/adb/ksu",
            "/data/adb/ap",
            "/data/adb/apd"
        };

        for (auto path : magiskPaths) {
            struct stat st;
            if (stat(path, &st) == 0) {
                return true; // Ruta relacionada a Magisk o KSU encontrada
            }
        }
        return false;
    }

    /**
     * Revisa si la build del dispositivo fue firmada con test-keys,
     * lo cual es común en dispositivos con root o custom ROMs.
     */
    bool checkBuildTags() {
        char build_tags[PROP_VALUE_MAX];
        __system_property_get("ro.build.tags", build_tags);
        return std::strstr(build_tags, "test-keys") != nullptr;
    }

    /**
     * Verifica si el sistema está configurado como debuggable o inseguro,
     * lo cual puede indicar un entorno de pruebas o con root.
     */
    bool checkSystemProperties() {
        char debuggable[PROP_VALUE_MAX];
        char secure[PROP_VALUE_MAX];
        __system_property_get("ro.debuggable", debuggable);
        __system_property_get("ro.secure", secure);
        return (std::strcmp(debuggable, "1") == 0) || (std::strcmp(secure, "0") == 0);
    }

    /**
     * Analiza el archivo /proc/self/mounts para detectar montajes sospechosos
     * como los que realiza Magisk o KernelSU (usando mirror, core, etc.).
     */
    bool checkRootCloaking() {
        std::FILE* mounts = std::fopen("/proc/self/mounts", "r");
        if (mounts) {
            char line[1024];
            while (std::fgets(line, sizeof(line), mounts)) {
                if (std::strstr(line, "magisk") ||
                    std::strstr(line, "core/mirror") ||
                    std::strstr(line, "KSU")) {
                    std::fclose(mounts);
                    return true; // Se detectó una montura relacionada a root
                }
            }
            std::fclose(mounts);
        }
        return false;
    }

    /**
     * Ejecuta el comando `pm path <package>` y analiza la salida
     * para ver si la app en cuestión está instalada en el sistema.
     */
    bool checkPackage(const char* package) {
        char command[256];
        std::snprintf(command, sizeof(command), "pm path %s", package);
        std::FILE* fp = popen(command, "r");
        if (fp) {
            char output[256];
            while (std::fgets(output, sizeof(output), fp)) {
                if (std::strstr(output, "package:")) {
                    pclose(fp);
                    return true; // La app está instalada
                }
            }
            pclose(fp);
        }
        return false;
    }

    /**
     * Comprueba si existen aplicaciones de root populares instaladas
     * como Magisk, KingRoot, KernelSU, SuperSU, etc.
     */
    bool checkRootApps() {
        const char* rootApps[] = {
            "com.topjohnwu.magisk",
            "com.kingroot.kinguser",
            "com.kingo.root",
            "me.weishu.kernelsu",
            "me.weishu.superuser",
            "eu.chainfire.supersu"
        };

        for (auto app : rootApps) {
            if (checkPackage(app)) {
                return true; // App de root encontrada
            }
        }
        return false;
    }

    /**
     * Función principal: Ejecuta todas las verificaciones disponibles
     * para determinar si el dispositivo está rooteado.
     */
    bool isDeviceRooted() {
        return checkSuBinary() ||         // Archivos binarios su
               checkMagiskFiles() ||      // Archivos Magisk/KSU
               checkBuildTags() ||        // Firmas test-keys
               checkSystemProperties() || // Propiedades de sistema inseguras
               checkRootCloaking() ||     // Monturas ocultas
               checkRootApps();           // Apps de root instaladas
    }

}
