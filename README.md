# NoClick

Stops the empty-drive "click" on Amiga floppy drives — the ~2.5-second tick a
drive makes when no disk is inserted.

Unlike the classic 1.x-era utilities that patch trackdisk or poke the hardware,
NoClick uses the **OS-official mechanism**: since Kickstart 2.0 (V36),
`trackdisk.device` exposes a public per-unit flag (`TDPF_NOCLICK` in
`tdu_PubFlags`). With the flag set, trackdisk performs its disk-change polling
by stepping the head *outward past track 0* — a step pulse the mechanism can't
act on, so it makes no sound. Disk-change detection keeps working normally.

No patches, no interrupts, no background process: NoClick sets the flag and
exits. 2,700 bytes.

## Requirements

- Kickstart 2.0 (V36) or newer. On 1.3 the flag doesn't exist: NoClick just
  says it needs 2.0 and exits with return code 20. Use one of the old
  hardware-poking utilities there instead.

## Usage

```
NoClick [OFF] [UNIT n]
```

| Invocation        | Effect                                          |
|-------------------|-------------------------------------------------|
| `NoClick`         | silence all present drives (DF0:–DF3:)          |
| `NoClick OFF`     | turn clicking back on                           |
| `NoClick UNIT 0`  | act on a single unit only (0–3)                 |

It reports each drive it finds (`DF0: clicking off`) and skips absent units.
Return codes: 0 = OK, 5 = no trackdisk units found, 10 = bad argument,
20 = Kickstart older than 2.0 (nothing changed).

## Installation

The release archive unpacks to a `NoClick` drawer (with `NoClick.info` beside
it) holding the `NoClick` program and this `README.md`. NoClick is a Shell
command, so the program has no icon of its own.

Use the **`.lha`**: a `.zip` can't carry AmigaDOS protection bits, so if you
unpacked a `.zip`, run `protect NoClick +e` in the drawer first.

1. Copy the binary to `C:` (from a Shell in the `NoClick` drawer):
   ```
   copy NoClick C:
   protect C:NoClick +e
   ```
2. Add it to the end of `S:User-Startup` so it runs every boot:
   ```
   echo >>S:User-Startup "C:NoClick >NIL:"
   ```
3. Run `C:NoClick` once by hand (or reboot). The clicking stops immediately.

The flag resets on every reboot (it lives in trackdisk's unit structure, not
on disk), which is why the User-Startup line is needed.

If you're replacing an old WBStartup-style NoClick commodity, remember to
remove it (and its `.info`) from `SYS:WBStartup` so both don't run.

### Getting the binary onto a networked Amiga

Any way you like — but if the Amiga has a TCP stack and `wget`, a quick
transfer from a PC on the LAN:

```
# on the PC, in the directory holding NoClick:
python -m http.server 8765
```
```
; on the Amiga:
wget -O C:NoClick http://<pc-ip>:8765/NoClick
protect C:NoClick rwed
```

(`protect` is needed because wget doesn't set the execute bit.)

## Building

Cross-compile with [amiga-gcc](https://github.com/bebbo/amiga-gcc) (Bebbo):

```
m68k-amigaos-gcc -O2 -noixemul -Wall -fomit-frame-pointer -s -o out/NoClick src/noclick.c
python3 tools/mkpkg.py      # stage out/release/NoClick/ + NoClick.info
```

## License

Public domain / do what you like.
