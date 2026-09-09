/*
 * BigClock - shows the system clock (HH:MM:SS AM/PM) at 72pt in a
 * standard Mac OS window, with the date below in a smaller font.
 *
 * Classic Toolbox (non-Carbon). Builds with Retro68 for 68k or PPC.
 * Time comes from GetDateTime() every pass, so it tracks the system
 * clock (and anything that adjusts it, e.g. Network Time / NTP).
 */

#include <MacTypes.h>
#include <Quickdraw.h>
#include <Fonts.h>
#include <Windows.h>
#include <Events.h>
#include <Menus.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Devices.h>
#include <ToolUtils.h>
#include <OSUtils.h>
#include <string.h>
#include <stdio.h>

#define kAppleMenu 128
#define kFileMenu  129
#define kAboutItem 1
#define kQuitItem  1
#define kAboutAlert 128

#ifndef BIGCLOCK_VERSION
#define BIGCLOCK_VERSION "0.0.0"
#endif

#define WIN_W 560
#define WIN_H 170
#define TIME_SIZE 72
#define DATE_SIZE 18

static WindowPtr gWindow;
static GWorldPtr gOffscreen = NULL;   /* back buffer to avoid flicker */
static short gTimeFont, gDateFont;
static unsigned long gLastSecs = 0;

static const char *kDays[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
static const char *kMonths[] = {
    "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"
};

/* C string -> Pascal string */
static void CToP(const char *src, Str255 dst)
{
    size_t n = strlen(src);
    if (n > 255) n = 255;
    dst[0] = (unsigned char)n;
    memcpy(dst + 1, src, n);
}

static short FontOrDefault(const char *name, short fallback)
{
    Str255 pname;
    short id = 0;
    CToP(name, pname);
    GetFNum(pname, &id);
    return id ? id : fallback;
}

static void DrawCentered(const char *text, short font, short size, short baseline)
{
    Str255 p;
    short w;
    CToP(text, p);
    TextFont(font);
    TextSize(size);
    TextFace(0);
    w = StringWidth(p);
    MoveTo((WIN_W - w) / 2, baseline);
    DrawString(p);
}

/* Draw the current time/date into whatever port is current. */
static void RenderClock(void)
{
    unsigned long secs;
    DateTimeRec dt;
    char timeStr[16], dateStr[64];
    short hour12;
    Rect r;

    GetDateTime(&secs);
    SecondsToDate(secs, &dt);

    hour12 = dt.hour % 12;
    if (hour12 == 0) hour12 = 12;
    sprintf(timeStr, "%d:%02d:%02d %s", hour12, dt.minute, dt.second,
            dt.hour < 12 ? "AM" : "PM");
    sprintf(dateStr, "%s, %s %d, %d", kDays[dt.dayOfWeek - 1],
            kMonths[dt.month - 1], dt.day, dt.year);

    SetRect(&r, 0, 0, WIN_W, WIN_H);
    EraseRect(&r);
    DrawCentered(timeStr, gTimeFont, TIME_SIZE, 95);
    DrawCentered(dateStr, gDateFont, DATE_SIZE, 140);

    gLastSecs = secs;
}

/* Render offscreen, then blit to the window in one CopyBits so the
   user never sees the erase step. Falls back to direct drawing if the
   GWorld couldn't be allocated. */
static void DrawClock(void)
{
    Rect r;
    SetRect(&r, 0, 0, WIN_W, WIN_H);

    if (gOffscreen) {
        CGrafPtr savePort;
        GDHandle saveDev;
        PixMapHandle pm = GetGWorldPixMap(gOffscreen);

        GetGWorld(&savePort, &saveDev);
        LockPixels(pm);
        SetGWorld(gOffscreen, NULL);
        RenderClock();
        SetGWorld(savePort, saveDev);

        SetPort(gWindow);
        CopyBits((BitMap *)*pm, &gWindow->portBits, &r, &r, srcCopy, NULL);
        UnlockPixels(pm);
    } else {
        SetPort(gWindow);
        RenderClock();
    }
}

static void SetUpMenus(void)
{
    MenuHandle m;

    m = NewMenu(kAppleMenu, "\p\024");          /* \024 = Apple logo glyph */
    AppendMenu(m, "\pAbout BigClock\311;(-");    /* \311 = ellipsis */
    AppendResMenu(m, 'DRVR');
    InsertMenu(m, 0);

    m = NewMenu(kFileMenu, "\pFile");
    AppendMenu(m, "\pQuit/Q");
    InsertMenu(m, 0);

    DrawMenuBar();
}

static void DoMenu(long choice)
{
    short menu = (short)(choice >> 16), item = (short)(choice & 0xFFFF);
    Str255 name;

    switch (menu) {
    case kAppleMenu:
        if (item == kAboutItem) {
            Str255 ver;
            CToP(BIGCLOCK_VERSION, ver);
            ParamText(ver, "\p", "\p", "\p");
            NoteAlert(kAboutAlert, NULL);
        } else {
            GetMenuItemText(GetMenuHandle(kAppleMenu), item, name);
            OpenDeskAcc(name);
        }
        break;
    case kFileMenu:
        if (item == kQuitItem) ExitToShell();
        break;
    }
    HiliteMenu(0);
}

static void DoMouseDown(EventRecord *ev)
{
    WindowPtr win;
    short part = FindWindow(ev->where, &win);
    Rect dragBounds;

    switch (part) {
    case inDrag:
        dragBounds = qd.screenBits.bounds;
        InsetRect(&dragBounds, 4, 4);
        DragWindow(win, ev->where, &dragBounds);
        break;
    case inGoAway:
        if (TrackGoAway(win, ev->where))
            ExitToShell();
        break;
    case inContent:
        if (win != FrontWindow()) SelectWindow(win);
        break;
    case inMenuBar:
        DoMenu(MenuSelect(ev->where));
        break;
    }
}

int main(void)
{
    Rect bounds;
    EventRecord ev;
    unsigned long now;

    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(NULL);
    InitCursor();
    FlushEvents(everyEvent, 0);
    SetUpMenus();

    /* Chicago is the classic look; Charcoal is the OS 8/9 system font.
       Change the first arg to "Charcoal" if you prefer that one. */
    gTimeFont = FontOrDefault("Chicago", systemFont);
    gDateFont = FontOrDefault("Geneva", applFont);

    SetRect(&bounds, 60, 60, 60 + WIN_W, 60 + WIN_H);
    gWindow = NewCWindow(NULL, &bounds, "\pBigClock", true,
                         noGrowDocProc, (WindowPtr)-1L, true, 0);
    SetPort(gWindow);

    /* Offscreen buffer matching the screen depth (depth 0 = same as screen). */
    SetRect(&bounds, 0, 0, WIN_W, WIN_H);
    if (NewGWorld(&gOffscreen, 0, &bounds, NULL, NULL, 0) != noErr)
        gOffscreen = NULL;

    DrawClock();

    for (;;) {
        /* Sleep up to 10 ticks (~1/6 s) so we redraw promptly on the
           second boundary without hogging the CPU. */
        if (WaitNextEvent(everyEvent, &ev, 10, NULL)) {
            switch (ev.what) {
            case mouseDown:
                DoMouseDown(&ev);
                break;
            case keyDown:
            case autoKey:
                if (ev.modifiers & cmdKey)
                    DoMenu(MenuKey((char)(ev.message & charCodeMask)));
                break;
            case updateEvt:
                BeginUpdate((WindowPtr)ev.message);
                DrawClock();
                EndUpdate((WindowPtr)ev.message);
                break;
            }
        }
        GetDateTime(&now);
        if (now != gLastSecs)
            DrawClock();
    }
    return 0;
}
