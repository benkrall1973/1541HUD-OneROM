# 1541HUD passive Monitor plugin (OneROM v0.8.1)

This is the Fire-24G UB4 user plugin, currently capture version **0.8.1.32**.
It captures 1541 activity through PIO0 state machine 3 and DMA channel 11,
then writes decoded events to the Monitor mailbox. The qualified image uses
the project passive core overlay (`HUD1541_PASSIVE_UB4`) and Monitor USB
0.8.1.5. Do not substitute a normal ROM-serving core.

Use the complete role build wrapper from the repository root in WSL/Linux:

```sh
export TOOLCHAIN=/path/to/arm-toolchain/bin
export ONEROM_CLI=/path/to/linux/onerom
bash firmware-monitor/Build-Monitor-Fire24G-V0.8.1.sh
```

The wrapper exports the supported source, verifies the checked-in host fixes,
applies the passive firmware overlay,
builds the role core and both custom Monitor plugins, composes the complete
image, and inspects it. For a combined Control/Monitor verification build that
also builds its own Linux CLI, follow the
[clean source-build procedure](../../../../docs/CLEAN_SOURCE_BUILD.md).

The standalone user-plugin output is
`build/1541-OneROM-1541HUD-Monitor-v0.8.1.32.bin`; it is not a complete image
to flash directly. After the wrapper prepares its core work tree, a plugin-only
rebuild from this directory can use:

```sh
make TOOLCHAIN="$TOOLCHAIN" ONEROM_ROOT=../../../.work/official-v0.8.1/one-rom-v0.8.1 -j2
```

See the [build and installation guide](../../../../docs/FIRE24G_V81_BUILD_AND_INSTALL.md),
[engineering dossier](../../../../docs/FIRE24G_V81_ENGINEERING_DOSSIER.md),
and [release-image qualification](../../../../docs/PUBLIC_FIRE24G_RELEASES.md).
