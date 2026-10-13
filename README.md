# BBC micro:bit V2 Board Support Pack

[![Build pack](https://github.com/MatthiasHertelArm/MicroBit_V2_BSP/actions/workflows/pack.yml/badge.svg)](https://github.com/MatthiasHertelArm/MicroBit_V2_BSP/actions/workflows/pack.yml)
[![Build examples](https://github.com/MatthiasHertelArm/MicroBit_V2_BSP/actions/workflows/build.yml/badge.svg)](https://github.com/MatthiasHertelArm/MicroBit_V2_BSP/actions/workflows/build.yml)

Source of the CMSIS-Pack `MicroBit::MicroBit_V2_BSP`: board support for the BBC micro:bit V2 on top of the
Nordic `nRF_DeviceFamilyPack`. See [Documents/OVERVIEW.md](Documents/OVERVIEW.md) for the content.

Releases: packs are attached to the [GitHub releases](https://github.com/MatthiasHertelArm/MicroBit_V2_BSP/releases);
install one with `cpackget add https://github.com/MatthiasHertelArm/MicroBit_V2_BSP/releases/download/v0.1.0/MicroBit.MicroBit_V2_BSP.0.1.0.pack`.

## Repository structure

| Directory / File                 | Content
|:---------------------------------|:---------------------------------------------------------------
| `MicroBit.MicroBit_V2_BSP.pdsc`  | Pack description (board, VIO component, example)
| `Board/`, `Board/Include/`       | CMSIS-Driver VIO and the Board Support drivers (`microbit_v2.h`)
| `Examples/Blinky/`               | Blinky example (VIO) in csolution format
| `Examples/BoardTest/`            | All peripherals with serial output and commands
| `Documents/`                     | Pack overview and the board schematic (CC BY-SA 4.0)
| `knowledge/`                     | Knowledge units for the CMSIS Developer Assistant knowledge base
| `gen_pack.sh`                    | Pack generation script ([gen-pack](https://github.com/Open-CMSIS-Pack/gen-pack) library)
| `vcpkg-configuration.json`       | Tool environment (CMSIS-Toolbox, GCC, CMake, Ninja) for the Arm Environment Manager in VS Code

## Generating the pack

Prerequisites: bash, curl, zip (or 7z), GNU coreutils (`brew install coreutils` on macOS), and `packchk` from the
[CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox) on the `PATH`.

```sh
./gen_pack.sh
```

The script validates the pack description with `packchk` (against the Nordic DFP and ARM::CMSIS) and writes
`output/MicroBit.MicroBit_V2_BSP.<version>.pack`. Install it with
`cpackget add output/MicroBit.MicroBit_V2_BSP.<version>.pack`.

## Continuous integration

- `.github/workflows/pack.yml` generates the pack with the
  [gen-pack action](https://github.com/Open-CMSIS-Pack/gen-pack-action) on every push and pull request and, for a
  tag `v<version>`, publishes a GitHub release with the `.pack` attached. The `<url>` in the pack description must
  point to that release folder.
- `.github/workflows/build.yml` installs the tools from `vcpkg-configuration.json`, generates and installs the pack
  and builds the Blinky and BoardTest examples with GCC.

Both workflows install `cmsis-knowledge` from the private repository
[cmsis-developer-assistant-tools](https://github.com/MatthiasHertelArm/cmsis-developer-assistant-tools) (pinned
commit) with the repository secret `CDA_TOOLS_TOKEN`, a fine-grained personal access token with read access to that
repository's contents; `gen_pack.sh` runs `cmsis-knowledge build --strict --fetch .` before packaging.

To release: set the version and date in the `<releases>` section of the pdsc and in the `<url>`, commit, then
`git tag v<version> && git push --tags`.

## Using the pack

```yaml
solution:
  packs:
    - pack: MicroBit::MicroBit_V2_BSP
    - pack: NordicSemiconductor::nRF_DeviceFamilyPack
    - pack: ARM::CMSIS
  target-types:
    - type: microbit-v2
      board: MicroBit::microbit-v2
```

Then add the components `CMSIS:CORE`, `Device:Startup` (from the Nordic DFP) and either `CMSIS Driver:VIO:Board`
or the `Board Support:*` components (`GPIO` plus the peripherals you need). For GCC, use the linker script and
regions header from `Examples/Blinky/RTE/Device/nRF52833_xxAA/` (see the notes in the overview).

## Debugging

The on-board DAPLink probe is a CMSIS-DAP probe (SWD). The Blinky example is configured for the
Arm CMSIS Debugger with pyOCD (`CMSIS-DAP@pyOCD`). Programming by drag-and-drop of the generated
`.hex` file to the `MICROBIT` drive also works.

## License

Apache-2.0, see [LICENSE](LICENSE).
