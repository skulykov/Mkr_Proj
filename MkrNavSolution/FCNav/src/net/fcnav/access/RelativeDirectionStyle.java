package net.fcnav.access;

import net.fcnav.plus.R;
import android.content.Context;

public enum RelativeDirectionStyle {

    SIDEWISE(R.string.direction_style_sidewise),
    CLOCKWISE(R.string.direction_style_clockwise);

    private final int key;

    RelativeDirectionStyle(int key) {
        this.key = key;
    }

    public String toHumanString(Context ctx) {
        return ctx.getResources().getString(key);
    }

}
