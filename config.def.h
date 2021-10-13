/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const int gappx     = 6;                 /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh            = 22;        /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const char *fonts[]          = { "JetBrainsMono Nerd Font:size=10", "Font Awesome 5 Free Solid:size=10", "Font Awesome 5 Free Regular:size=10", "Font Awesome 5 Free Brands:size=10" };
static const char dmenufont[]       = "JetBrainsMono Nerd Font:size=10";
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char col_gray1[]       = "#282c34";
static const char col_gray2[]       = "#353b45";
static const char col_gray3[]       = "#abb2bf";
static const char col_gray4[]       = "#b6bdca";
static const char col_cyan[]        = "#c678dd";
static const char *colors[][3]      = {
	    /*               fg         bg         border   */
	    [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	    [SchemeSel]  = { col_gray4, col_gray2,  col_gray4  },
	    [SchemeTabInactive]  = { col_gray3, col_gray1,  col_gray2 },
		[SchemeTabActive]  = { col_gray3, col_gray1,  col_gray2 }

};

/* tagging */
static const char *tags[] = { "", "", "", "", "ﱮ", "", "", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class              instance    title       tags mask     isfloating   monitor  ignoretransient*/
	{ "Gimp",       		NULL,       NULL,       0,            1,           -1, 0},
	{ "1Password",  		NULL,       NULL,       0,            1,           -1, 0 },
	{ "stalonetray",        NULL,       NULL,       ~0,            1,           -1, 0 },
	{ "trayer",             NULL,       NULL,       ~0,            1,           -1, 0 },
	{ "Firefox",            NULL,       NULL,       1 << 1,       0,           -1, 0 },
	{ "Microsoft Teams - Preview",  NULL,       NULL,       1 << 3,       0,           -1, 0 },
	{ "Slack",              NULL,       NULL,       1 << 3,       0,           -1, 0 },
	{ "jetbrains-idea",     NULL,       NULL,       1 << 2,       0,           -1, 1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#include "fibonacci.c"

/* Bartabgroups properties */
#define BARTAB_BORDERS 1       // 0 = off, 1 = on
#define BARTAB_BOTTOMBORDER 1  // 0 = off, 1 = on
#define BARTAB_TAGSINDICATOR 1 // 0 = off, 1 = on if >1 client/view tag, 2 = always on
#define BARTAB_TAGSPX 5        // # pixels for tag grid boxes
#define BARTAB_TAGSROWS 3      // # rows in tag grid (9 tags, e.g. 3x3)
static void (*bartabmonfns[])(Monitor *) = { monocle /* , customlayoutfn */ };
static void (*bartabfloatfns[])(Monitor *) = { NULL /* , customlayoutfn */ };

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
 	{ "[@]",      spiral },
 	{ "[\\]",     dwindle },
    { "[D]",      deck },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "rofi", "-modi", "window,run,ssh,drun", "-show-icons", "-show", "drun", NULL };
static const char *quitcmd[] = { "rofi", "-show", "power-menu", "-modi", "power-menu:rofi-power-menu" };
static const char *termcmd[]  = { "kitty", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "kitty", "--name", scratchpadname, NULL };
static const char *volumeupcmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%", NULL };
static const char *volumedowncmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%", NULL };
static const char *volumemutecmd[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };

#include "shift-tools.c"
#include "focusurgent.c"
#include <X11/XF86keysym.h>

#include "movestack.c"
static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_o, shiftviewclients,    { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,	shiftview,         { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,	shiftview,         { .i = -1 } },
	{ MODKEY|ControlMask,           XK_i, shiftviewclients,    { .i = -1 } },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,                       XK_s,      togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY|ShiftMask,		XK_h,      shiftboth,      { .i = -1 }	},
	{ MODKEY|ControlMask,		XK_h,      shiftswaptags,  { .i = -1 }	},
	{ MODKEY|ControlMask,		XK_l,      shiftswaptags,  { .i = +1 }	},
	{ MODKEY|ShiftMask,             XK_l,      shiftboth,      { .i = +1 }	},
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	//{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} },
	//{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,  cyclelayout,    {.i = +1 } },
	//{ MODKEY|ControlMask,		XK_comma,  cyclelayout,    {.i = -1 } },
	//{ MODKEY|ControlMask,           XK_period, cyclelayout,  {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_u,      focusurgent,    {0} },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -5 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +5 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 
	{ MODKEY|ShiftMask,             XK_e,      spawn,          {.v = quitcmd } },
	{ 0, 							XF86XK_AudioLowerVolume, spawn, {.v = volumedowncmd }  },
	{ 0, 							XF86XK_AudioRaiseVolume, spawn, {.v = volumeupcmd }  },
	{ 0, 							XF86XK_AudioMute, 		 spawn, {.v = volumemutecmd }  } 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
