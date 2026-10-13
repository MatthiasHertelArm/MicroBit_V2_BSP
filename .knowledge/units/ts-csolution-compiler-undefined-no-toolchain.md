---
id: ts-csolution-compiler-undefined-no-toolchain
type: troubleshooting
title: "VS Code CMSIS build: compiler undefined / no compiler registered although GCC is installed through vcpkg"
summary: "The CMSIS Solution extension registers only the tools of the activated Arm Environment Manager configuration; without vcpkg-configuration.json in the workspace root no compiler is registered and cbuild stops. Add the configuration at the workspace root."
domain: project
status: draft
tool: csolution
severity: error
symptoms:
  - "error csolution: compiler undefined, use '--toolchain' option or add 'compiler: <value>' to yml input"
  - "warning csolution: no compiler registered. Add path to compiler 'bin' directory with environment variable <name>_TOOLCHAIN_<major>_<minor>_<patch>"
symptom_regex: "compiler undefined, use '--toolchain' option"
applies_to:
  tool: [csolution, cbuild, cmsis-solution, vcpkg]
source: own observation 2026-10-13 with arm.cmsis-csolution 1.72.0 and CMSIS-Toolbox 2.15.0
license: Apache-2.0
confidence: high
---

## Symptom

`cmsis_action build` fails after 0.3 s with the two messages above. `Examples/<x>/.cmsis/tools-environment.yml`,
written by the extension, lists pyOCD, the bundled CMSIS-Toolbox and GDB but `variables: {}` and no compiler.
`~/.vcpkg/artifacts/.../compilers.arm.arm.none.eabi.gcc/<version>` exists.

## Cause

The Arm Environment Manager activates the `vcpkg-configuration.json` found in the *workspace root*. The
example's own `vcpkg-configuration.json` in a sub-folder is not seen when the repository root is the workspace,
so no `GCC_TOOLCHAIN_<major>_<minor>_<patch>` variable reaches cbuild. With `select-compiler:` listing several
compilers and none registered, csolution cannot pick one.

## Fix

Add a `vcpkg-configuration.json` at the workspace root (copy of the example's: cmsis-toolbox, arm-none-eabi-gcc,
cmake, ninja). The Environment Manager activates it within about half a minute; `.cmsis/tools-environment.yml` then
shows `GCC_TOOLCHAIN_14_3_1: .../bin` and the extension appends `compiler: GCC` to the csolution (remove that line
again before shipping an example with `select-compiler:`). A second message "selectable values can be found in
cbuild-idx.yml" disappears after the extension has written the compiler selection; just build again.
