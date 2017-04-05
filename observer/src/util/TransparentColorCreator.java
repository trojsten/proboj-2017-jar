package util;

import javafx.scene.paint.Color;

/**
 * Created by siegrift on 1/30/17.
 */
public class TransparentColorCreator {
    private final Color color;

    public TransparentColorCreator(Color color, double opacity) {
        this.color = new Color(color.getRed(), color.getGreen(), color.getBlue(), opacity);
    }

    public Color create() {
        return color;
    }
}
