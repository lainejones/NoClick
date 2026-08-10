/*
 * NoClick — stop the empty-drive click on Amiga floppy drives.
 *
 * Sets the OS-official TDPF_NOCLICK bit in trackdisk.device's public
 * unit flags (available since Kickstart 2.0 / V36). No hardware pokes,
 * no patches; trackdisk itself then steps the head "outward past 0"
 * for its disk-change checks, which is silent.
 *
 * Usage:  NoClick [OFF] [UNIT n]
 *   (no args)  disable clicking on all present drives DF0:-DF3:
 *   OFF        re-enable clicking
 *   UNIT n     act on a single unit only
 */

#include <exec/types.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <devices/trackdisk.h>
#include <proto/exec.h>
#include <proto/dos.h>

static const char ver[] = "$VER: NoClick 1.0 (9.8.2026)";

#define TEMPLATE "OFF/S,UNIT/N"

extern struct ExecBase *SysBase;

int main(void)
{
    struct RDArgs *rda;
    LONG args[2] = { 0, 0 };
    LONG off, first, last, unit, found = 0;
    int rc = RETURN_OK;

    if (SysBase->LibNode.lib_Version < 36) {
        Printf("NoClick needs Kickstart 2.0 or newer.\n");
        return RETURN_FAIL;
    }

    rda = ReadArgs(TEMPLATE, args, NULL);
    if (!rda) {
        PrintFault(IoErr(), "NoClick");
        return RETURN_ERROR;
    }
    off = args[0];
    if (args[1]) {
        first = last = *(LONG *)args[1];
        if (first < 0 || first > 3) {
            Printf("UNIT must be 0-3.\n");
            FreeArgs(rda);
            return RETURN_ERROR;
        }
    } else {
        first = 0;
        last = 3;
    }

    for (unit = first; unit <= last; unit++) {
        struct MsgPort *port = CreateMsgPort();
        struct IOExtTD *io;

        if (!port)
            break;
        io = (struct IOExtTD *)CreateIORequest(port, sizeof(struct IOExtTD));
        if (io) {
            if (OpenDevice(TD_NAME, unit, (struct IORequest *)io, 0) == 0) {
                struct TDU_PublicUnit *pu =
                    (struct TDU_PublicUnit *)io->iotd_Req.io_Unit;

                if (off)
                    pu->tdu_PubFlags &= ~TDPF_NOCLICK;
                else
                    pu->tdu_PubFlags |= TDPF_NOCLICK;

                Printf("DF%ld: clicking %s\n", unit,
                       off ? (LONG)"on" : (LONG)"off");
                found++;
                CloseDevice((struct IORequest *)io);
            }
            DeleteIORequest((struct IORequest *)io);
        }
        DeleteMsgPort(port);
    }

    if (!found) {
        Printf("No trackdisk units found.\n");
        rc = RETURN_WARN;
    }

    FreeArgs(rda);
    return rc;
}
