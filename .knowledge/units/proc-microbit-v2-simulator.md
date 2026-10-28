---
id: proc-microbit-v2-simulator
type: procedure
title: "Run and debug micro:bit V2 projects without hardware in the mbsim simulator"
summary: "Download mbsim, a simulator that works with the BBC micro:bit V2, from its GitHub releases and add a target-type with the Arm-FVP debug adapter, so Load & Debug, the UART and the board inputs work without a board."
domain: debug
status: draft
applies_to:
  board: [microbit-v2]
  tool: [cmsis-debugger, fvp]
triggers:
  - debug without a micro:bit
  - no board connected
  - simulate the micro:bit V2
  - run the BSP examples in a simulator
  - test micro:bit firmware in CI
preconditions:
  - a csolution with board MicroBit::microbit-v2 that builds (see proc-microbit-v2-bring-up)
stop_conditions:
  - "mbsim: <value>: No such file or directory: a value in args: was taken for an image; write mbsim options as -C sim.<name>=<value>"
  - "Address already in use for the GDB server: another mbsim, pyOCD or FVP holds port 3333; stop it, or give the target-set another gdbserver port"
  - "macOS refuses to open mbsim: clear the quarantine attribute (step 1)"
references: [proc-microbit-v2-bring-up, concept-microbit-v2-bsp-components]
source: mbsim 0.3.2 (README, docs/getting-started.md, docs/decisions/0004-cmsis-debugger-integration.md) and the FVP.adapter.json template of CMSIS Solution 1.72.0; steps verified on macOS 2026-10-18 with 0.3.0
license: Apache-2.0
---

mbsim (github.com/MatthiasHertel80/mbsim, up to 0.3.1 MatthiasHertelArm/microbit-v2-sim) simulates the board
this pack describes: the Cortex-M4F, the nRF52833 peripherals the BSP drivers use, and the board parts (LED
matrix, buttons, touch logo, LSM303AGR, microphone, speaker, UART to the interface MCU). Blinky and BoardTest run
unmodified. It is a GDB server for the Arm CMSIS Debugger and shows the board in a window, where the inputs are
operated.
Not modelled: RADIO, NFCT and USBD (their registers read as zero, with a warning).

## Steps

1. **Download.** From github.com/MatthiasHertel80/mbsim/releases take the archive for the host:
   `mbsim-<version>-<host>.tar.gz` with `<host>` = `linux-x64`, `linux-arm64`, `macos-arm64` or `macos-x64`,
   or `mbsim-<version>-windows-x64.zip` / `windows-arm64.zip`; `SHA256SUMS` lists the checksums. For example:

   ```sh
   curl -LO https://github.com/MatthiasHertel80/mbsim/releases/download/v0.3.2/mbsim-v0.3.2-macos-arm64.tar.gz
   curl -LO https://github.com/MatthiasHertel80/mbsim/releases/download/v0.3.2/SHA256SUMS
   shasum -a 256 -c SHA256SUMS --ignore-missing      # Linux: sha256sum -c SHA256SUMS --ignore-missing
   tar -xzf mbsim-v0.3.2-macos-arm64.tar.gz
   ```

   Unpack to a folder without spaces in its path. The archive holds the folder `mbsim-<version>-<host>` with
   `mbsim` (`mbsim.exe`), the README, the licence and the adapter files in `cmsis/`.
   - macOS: the binary is not signed. When it was downloaded with a browser, macOS refuses to open it until
     `xattr -d com.apple.quarantine mbsim` clears the quarantine attribute; `curl` does not set it.
   - Linux: the window needs X11 or Wayland with libxkbcommon; sound and the host microphone need libasound2.
   - Take 0.3.1 or later: the archives of 0.3.0 were built without the board window.
   - Check: `mbsim --version` prints `mbsim 0.3.2`.

2. **Add a target-type** to the csolution, next to the one for the board. It uses the `Arm-FVP` debug adapter
   that CMSIS Solution already has, so nothing is installed into VS Code:

   ```yaml
     target-types:
       - type: microbit-v2-sim
         board: MicroBit::microbit-v2
         target-set:
           - set:
             debugger:
               name: Arm-FVP
               model: /Users/me/tools/mbsim/mbsim     # full path; C:/tools/mbsim/mbsim.exe on Windows
               args: -C sim.uart_port=4444
             images:
               - project-context: BoardTest.Debug
   ```

   mbsim options are FVP parameters, `-C sim.<name>=<value>` in `args:` (several separated by spaces), or lines
   `sim.<name>=<value>` in a `config-file:`:

   | Parameter | Effect |
   |:--|:--|
   | `uart_port=4444` | target UART on TCP port 4444 |
   | `headless=1` | no window (also `disable-visualisation=1`) |
   | `speed=2`, `speed=max` | simulation speed; default 1 = real time |
   | `mic=1` | start with the host microphone feeding the board's microphone (0.2.0 or later) |
   | `persist=1` | keep running when the debugger disconnects |
   | `semihosting-enable=1` | serve semihosting calls |
   | `shutdown_on_eot=1` | exit when the firmware sends 0x04 (test runs) |
   | `out_file=-` | target UART on stdout |

   Other mbsim command-line options do not work in `args:`: the template passes `args:` before `-a <image>`,
   and mbsim, like an FVP, reads only `-C`, `-f`, `-a` and a few flags there.

3. **Build and debug.** Select the target type `microbit-v2-sim` in the CMSIS view and build: the image goes to
   `out/<project>/microbit-v2-sim/<build-type>/`. *Load & Debug* starts mbsim with the image, shows the board
   window and stops at `main` (the template sets `tbreak main`). *Run* starts it without a debugger.
   *Load* does nothing for this adapter: mbsim loads the images when it starts. Breakpoints, stepping,
   watchpoints, memory, registers and the peripheral view work as on the board.
   The CMSIS Developer Assistant does the same with `cmsis_action build` and `cmsis_action load_and_debug`,
   both with `target: microbit-v2-sim`.

4. **UART.** With `uart_port=4444`, open the VS Code Serial Monitor in TCP mode on `127.0.0.1`, port `4444`.
   The window's UART panel shows the same output and sends text to the target. The assistant's `serial_*`
   tools open device paths only and do not reach the TCP port.

5. **Inputs, in the window.** Keys `A`, `B` (or the mouse) press the buttons, `Space` both, `T` touches the logo,
   `R` resets, `F` shows the back. Right-drag tilts the board. The side panel shakes it and sets heading,
   temperature, a test tone, a clap and the host microphone. The first time the microphone is used, macOS asks
   for access for the app that started mbsim (VS Code); Windows needs microphone access for desktop apps
   (Settings › Privacy & security › Microphone). Hovering over an LED shows its brightness; hovering over
   ring 0, 1 or 2 shows what the nRF52833 drives on the pin.

## Limits of this setup

- The Arm-FVP template turns off the second debugger connection, so Live Watch stays off. For Live Watch install
  the adapter `mbsim` that ships in the archive (`mbsim register-cmsis-adapter` shows what it adds;
  `--apply --backup-dir DIR` adds it) and use `debugger: name: mbsim` instead of `Arm-FVP`. Up to 0.3.1 the
  adapter was called `microbit-v2-sim`; that name still works.
- Without VS Code, for tests in CI: `mbsim BoardTest.elf --headless --speed max --scenario test.yaml` drives
  inputs and checks UART text, LED patterns and speaker tones in simulated time; the exit code is 1 when an
  expectation fails. The scenario format is in the mbsim README.
