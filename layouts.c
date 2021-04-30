/* See LICENSE file for copyright and license details. */

#include "layouts.h"
#include "instantwm.h"
#include "push.h"
#include "util.h"

#define OUTERGAP m->outergap*(m->enablegap && (!m->smartgap || n != 1))
#define INNERGAP m->innergap*m->enablegap

void
bstack(Monitor *m) {
	int w, h, mh, mx, tx, ty, tw, framecount, rw, rh, rx, ry;
	unsigned int i, n;
	Client *c;

	if (animated && clientcount() > 4)
		framecount = 4;
	else
		framecount = 7;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;

	rh = m->wh - 2*OUTERGAP;
	rw = m->ww - 2*OUTERGAP*m->mw/m->mh;
	ry = m->wy + OUTERGAP;
	rx = m->wx + OUTERGAP*m->mw/m->mh;

	if (n > m->nmaster) {
		mh = m->nmaster ? m->mfact * rh - INNERGAP / 2 : 0;
		ty = ry + mh;
	} else {
		mh = rh;
		ty = ry;
	}
	for (i = mx = tx = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			w = (rw - mx + INNERGAP) / (MIN(n, m->nmaster) - i) - INNERGAP;
			animateclient(c, rx + mx, ry, w - (2 * c->bw), mh - (2 * c->bw), framecount, 0);
			mx += WIDTH(c) + INNERGAP;
		} else {
			h = rh - mh;
			tw = (rw - tx + INNERGAP) / (n - i) - INNERGAP;
			animateclient(c, tx + rx, ty + INNERGAP * (m->nmaster != 0), tw - (2 * c->bw), h - (2 * c->bw) - INNERGAP * (m->nmaster != 0), framecount, 0);
			tx += WIDTH(c) + INNERGAP;
		}
	}
}


/*
 * Different ids for snapping positions
 *
 *    ##################################
 *    # 8             1              2 # 
 *    #                                # 
 *    #                                # 
 *    #                                # 
 *    # 7             9              3 # 
 *    #                                # 
 *    #                                # 
 *    # 6             5              4 # 
 *    ##################################
 *
 * */

void floatl(Monitor *m) {
    Client *c;
    int animatestore;
    animatestore = animated;
    animated = 0;
    for(c = m->clients; c; c = c->next) {
        if (!(ISVISIBLE(c)))
            continue;
        if (c->snapstatus)
            applysnap(c, m);
    }
    restack(selmon);
    if (selmon->sel)
        XRaiseWindow(dpy, selmon->sel->win);
    if (animatestore)
        animated = 1;
}



void
bstackhoriz(Monitor *m) {
	int w, mh, mx, tx, ty, th, framecount, rw, rh, rx, ry;
	unsigned int i, n;
	Client *c;

	if (animated && clientcount() > 4)
		framecount = 4;
	else
		framecount = 7;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;

	rh = m->wh - 2*OUTERGAP;
	rw = m->ww - 2*OUTERGAP*m->mw/m->mh;
	ry = m->wy + OUTERGAP;
	rx = m->wx + OUTERGAP*m->mw/m->mh;

	if (n > m->nmaster) {
		mh = m->nmaster ? m->mfact * rh - INNERGAP / 2 : 0;
		ty = mh + INNERGAP * (m->nmaster != 0);
	} else {
		th = mh = rh;
	}
	for (i = mx = 0, tx = rx, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			w = (rw - mx + INNERGAP) / (MIN(n, m->nmaster) - i) - INNERGAP;
			animateclient(c, rx + mx, ry, w - (2 * c->bw), mh - (2 * c->bw), framecount, 0);
			mx += WIDTH(c) + INNERGAP;
		} else {
			th = (rh - ty + INNERGAP) / (n - i) - INNERGAP;
			animateclient(c, tx, ty + ry, rw - (2 * c->bw), th - (2 * c->bw), framecount, 0);
			ty += HEIGHT(c) + INNERGAP;
		}
	}
}

void
deck(Monitor *m)
{
	int dn, rw, rh, rx, ry;
	unsigned int i, n, h, mw, my;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if(n == 0)
		return;

	dn = n - m->nmaster;
	if(dn > 0) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "D %d", dn);

	rh = m->wh - 2*OUTERGAP;
	rw = m->ww - 2*OUTERGAP*m->mw/m->mh;
	ry = m->wy + OUTERGAP;
	rx = m->wx + OUTERGAP*m->mw/m->mh;

	if(n > m->nmaster)
		mw = m->nmaster ? rw * m->mfact : 0;
	else
		mw = rw;
	for(i = my = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if(i < m->nmaster) {
			h = (rh - my) / (MIN(n, m->nmaster) - i);
			resize(c, rx, ry + my, mw - (2*c->bw), h - (2*c->bw), False);
			my += HEIGHT(c);
		}
		else
			resize(c, rx + mw, ry, rw - mw - (2*c->bw), rh - (2*c->bw), False);
}

void
grid(Monitor *m) {
	int rows, framecount, rw, rh, rx, ry;
	unsigned int i, n, cols;
	Client *c;

	if (animated && clientcount() > 5)
		framecount = 3;
	else
		framecount = 6;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if(n == 0)
		return;

	rh = m->wh - 2*OUTERGAP;
	rw = m->ww - 2*OUTERGAP*m->mw/m->mh;
	ry = m->wy + OUTERGAP;
	rx = m->wx + OUTERGAP*m->mw/m->mh;

	/* grid dimensions */
	for(rows = 0; rows <= n/2; rows++)
		if(rows*rows >= n)
			break;
	cols = (rows && (rows - 1) * rows >= n) ? rows - 1 : rows;

	/* window geoms (cell height/width) */
	int ch = rh / (rows ? rows : 1);
	int cw = rw / (cols ? cols : 1);
	for(i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		unsigned int cx = rx + (i / rows) * cw;
		unsigned int cy = ry + (i % rows) * ch;
		/* adjust height/width of last row/column's windows */
		int ah = ((i + 1) % rows == 0) ? rh - ch * rows : 0;
		unsigned int aw = (i >= rows * (cols - 1)) ? rw - cw * cols : 0;
		animateclient(c, cx, cy, cw - 2 * c->bw + aw, ch - 2 * c->bw + ah, framecount, 0);
		i++;
	}
}

// overlay all clients on top of each other
void
monocle(Monitor *m)
{
	int rw, rh, rx, ry;
	unsigned int n;
	Client *c;

	if (animated && selmon->sel)
		XRaiseWindow(dpy, selmon->sel->win);

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;

	rh = m->wh - 2*OUTERGAP*!m->smartgap;
	rw = m->ww - 2*OUTERGAP*m->mw/m->mh*!m->smartgap;
	ry = m->wy + OUTERGAP*!m->smartgap;
	rx = m->wx + OUTERGAP*m->mw/m->mh*!m->smartgap;

	if (n > 0) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[%1u]", n);
	for (c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		if (animated && c == selmon->sel) {
			animateclient(c, rx, ry, rw - 2 * c->bw, rh - 2 * c->bw, 7, 0);
			continue;
		}
			
		resize(c, rx, ry, rw - 2 * c->bw, rh - 2 * c->bw, 0);
	}

}

void
focusstack2(const Arg *arg)
{
	Client *nextVisibleClient = findVisibleClient(selmon->sel->next) ?: findVisibleClient(selmon->clients);

	if (nextVisibleClient) {
		if (nextVisibleClient->mon != selmon)
			selmon = nextVisibleClient->mon;
		detachstack(nextVisibleClient);
		attachstack(nextVisibleClient);
		selmon->sel = nextVisibleClient;
	}
}

void
overviewlayout(Monitor *m)
{
	int n;
	int gridwidth;
	unsigned int colwidth;
	unsigned int lineheight;
	int tmpx;
	int tmpy;
	Client *c;
	XWindowChanges wc;
	n = allclientcount();

	if (n == 0)
		return;

	gridwidth = 1;

	while ((gridwidth * gridwidth) < n) {
		gridwidth++;
	}

	tmpx = selmon->mx;
	tmpy = selmon->my + (selmon->showbar ? bh : 0);
	lineheight = selmon->wh / gridwidth;
	colwidth = selmon->ww / gridwidth;
	wc.stack_mode = Above;
	wc.sibling = m->barwin;

	for(c = m->clients; c; c = c->next) {
        if (HIDDEN(c))
            continue;
		if (c == selmon->overlay)
			continue;
		if (c->isfloating)
			savefloating(c);
		resize(c,tmpx, tmpy, c->w, c->h, 0);

		XConfigureWindow(dpy, c->win, CWSibling|CWStackMode, &wc);
		wc.sibling = c->win;
		if (tmpx + colwidth < selmon->mx + selmon->ww) {
			tmpx += colwidth;
		} else {
			tmpx = selmon->mx;
			tmpy += lineheight;
		}
	}
	XSync(dpy, False);
}

void
tcl(Monitor * m)
{
	int x, y, h, w, mw, sw, bdw;
	unsigned int i, n;
	Client * c;

	for (n = 0, c = nexttiled(m->clients); c;
			c = nexttiled(c->next), n++);

	if (n == 0)
		return;

	c = nexttiled(m->clients);

	mw = m->mfact * m->ww;
	sw = (m->ww - mw) / 2;
	bdw = (2 * c->bw);
	resize(c,
			n < 3 ? m->wx : m->wx + sw,
			m->wy,
			n == 1 ? m->ww - bdw : mw - bdw,
			m->wh - bdw,
			False);

	if (--n == 0)
		return;

	w = (m->ww - mw) / ((n > 1) + 1);
	c = nexttiled(c->next);

	if (n > 1)
	{
		x = m->wx + ((n > 1) ? mw + sw : mw);
		y = m->wy;
		h = m->wh / (n / 2);

		if (h < bh)
			h = m->wh;

		for (i = 0; c && i < n / 2; c = nexttiled(c->next), i++)
		{
			resize(c,
					x,
					y,
					w - bdw,
					(i + 1 == n / 2) ? m->wy + m->wh - y - bdw : h - bdw,
					False);

			if (h != m->wh)
				y = c->y + HEIGHT(c);
		}
	}

	x = (n + 1 / 2) == 1 ? mw : m->wx;
	y = m->wy;
	h = m->wh / ((n + 1) / 2);

	if (h < bh)
		h = m->wh;

	int rw = w - bdw;

	for (i = 0; c; c = nexttiled(c->next), i++)
	{
		int rh = (i + 1 == (n + 1) / 2) ? m->wy + m->wh - y - bdw : h - bdw;
		resize(c, x, y, rw,	rh, 0);

		if (h != m->wh)
			y = c->y + HEIGHT(c);
	}
}

void
tile(Monitor *m)
{
	unsigned int i, n, h, mw, my, ty, framecount, tmpanim, rw, rh, rx, ry;
	Client *c;

	if (animated && clientcount() > 5)
		framecount = 4;
	else
		framecount = 7;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;

	rh = m->wh - 2*OUTERGAP;
	rw = m->ww - 2*OUTERGAP*m->mw/m->mh;
	ry = m->wy + OUTERGAP;
	rx = m->wx + OUTERGAP*m->mw/m->mh;

	if (n > m->nmaster)
		mw = m->nmaster ? rw * m->mfact - INNERGAP / 2 : 0;
	else {
		mw = rw;
		if (n > 1 && n < m->nmaster) {
			m->nmaster = n;
			tile(m);
			return;
		}
	}
	for (i = my = ty = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			// client is in the master
			h = (rh - my + INNERGAP) / (MIN(n, m->nmaster) - i) - INNERGAP;

            if (n == 2) {
                tmpanim = animated;
                animated = 0;
			animateclient(c, rx, ry + my, mw - (2*c->bw), h - (2*c->bw), framecount, 0);
                animated = tmpanim;
            } else {
			animateclient(c, rx, ry + my, mw - (2*c->bw), h - (2*c->bw), framecount, 0);
			if (m->nmaster == 1 && n > 1) {
				mw = c->w + c->bw * 2;
			}
            }
			if (my + HEIGHT(c) < rh)
				my += HEIGHT(c) + INNERGAP;
		} else {
			// client is in the stack
			h = (rh - ty + INNERGAP) / (n - i) - INNERGAP;
            animateclient(c, rx + mw + INNERGAP * (m->nmaster != 0), ry + ty, rw - mw - (2*c->bw) - INNERGAP * (m->nmaster != 0), h - (2*c->bw), framecount, 0);
			if (ty + HEIGHT(c) < rh)
				ty += HEIGHT(c) + INNERGAP;
		}
}
