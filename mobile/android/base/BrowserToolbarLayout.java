/* -*- Mode: Java; c-basic-offset: 4; tab-width: 20; indent-tabs-mode: nil; -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko;

import org.mozilla.gecko.gfx.LayerView;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.widget.LinearLayout;

public class BrowserToolbarLayout extends LinearLayout {
    private static final String LOGTAG = "GeckoToolbarLayout";

    public BrowserToolbarLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // If the motion event has occured below the toolbar (due to the scroll
        // offset), let it pass through to the page.
        if (event != null && event.getY() > getHeight() - getScrollY()) {
            return false;
        }

        return super.onTouchEvent(event);
    }

    @Override
    protected void onScrollChanged(int l, int t, int oldl, int oldt) {
        super.onScrollChanged(l, t, oldl, oldt);

        if (t != oldt) {
            refreshMargins();
        }
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);

        if (h != oldh) {
            // In the current UI, this is the only place we have need of
            // viewport margins (to stop the toolbar from obscuring fixed-pos
            // content).
            GeckoAppShell.sendEventToGecko(
                GeckoEvent.createBroadcastEvent("Viewport:FixedMarginsChanged",
                    "{ \"top\" : " + h + ", \"right\" : 0, \"bottom\" : 0, \"left\" : 0 }"));

            refreshMargins();
        }
    }

    public void refreshMargins() {
        LayerView view = GeckoApp.mAppContext.getLayerView();
        if (view != null) {
            view.getLayerClient().setFixedLayerMargins(0, getHeight() - getScrollY(), 0, 0);
        }
    }
}

