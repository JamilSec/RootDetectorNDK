# SecureRootCheck.cpp

**SecureRootCheck.cpp** es una librería escrita en C++ (NDK) diseñada para detectar si un dispositivo Android está rooteado. Este sistema ofrece múltiples verificaciones de integridad del sistema para identificar entornos comprometidos, herramientas de rooteo (como Magisk, KingRoot, SuperSU, etc.), configuraciones de sistema inseguras y modificaciones del sistema de archivos.

## 🚀 Características

- Detección de binarios `su` en ubicaciones comunes.
- Verificación de archivos relacionados con Magisk y KernelSU.
- Comprobación de `build tags` con `test-keys`.
- Evaluación de propiedades del sistema (`ro.debuggable`, `ro.secure`).
- Análisis de montajes sospechosos en `/proc/self/mounts`.
- Detección de apps root populares instaladas en el sistema.
- Listo para integración en proyectos Android a través de JNI.

## 🛠️ Integración con Android (JNI)

Para integrar **SecureRootCheck.cpp** con tu aplicación Android usando JNI, añade la siguiente función a tu archivo nativo:

```cpp
extern "C" JNIEXPORT jboolean JNICALL
Java_<package-name>_MainActivity_isRooted(JNIEnv* env, jobject thiz) {
    return RootDetector::isDeviceRooted();
}
```

Asegúrate de que el nombre del paquete y clase coincidan con tu proyecto Android (`com.orbi.test.MainActivity` en este ejemplo).

## 📁 Estructura del Proyecto

```plaintext
SecureRootCheck.cpp
├── RootDetector namespace
│   ├── checkSuBinary()
│   ├── checkMagiskFiles()
│   ├── checkBuildTags()
│   ├── checkSystemProperties()
│   ├── checkRootCloaking()
│   ├── checkRootApps()
│   └── isDeviceRooted()
└── JNI Interface
    └── Java_com_orbi_test_MainActivity_isRooted()
```

## 📦 Requisitos

- Android NDK
- CMake o ndk-build
- API Level 21+

---

> **Nota**: Esta librería no garantiza una detección infalible en todos los dispositivos, pero ofrece un conjunto robusto de verificaciones para mejorar la seguridad contra entornos modificados.
