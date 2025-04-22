- Use this in case you want  a preset.
```json
{
  "version": 4,
  "cmakeMinimumRequired": {
    "major": 4,
    "minor": 0,
    "patch": 0
  },
  "configurePresets": [
    {
      "name": "x64-Debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/out/build/${presetName}",
      "installDir": "${sourceDir}/out/install/${presetName}",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_C_COMPILER": "cl",
        "CMAKE_CXX_COMPILER": "cl",
        "ILOG_BUILD_EXAMPLES" : "True",
        "CMAKE_TOOLCHAIN_FILE": "C:/Users/COHOTECH/scoop/apps/vcpkg/current/scripts/buildsystems/vcpkg.cmake"
      },
      "environment": {
        "VSCMD_START_DIR": "${sourceDir}"
      }
    }
  ]
}
```
