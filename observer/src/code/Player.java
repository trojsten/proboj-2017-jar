package code;

import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.scene.paint.Color;

/**
 * Created by siegrift on 12/25/16.
 * <p>
 * This class holds properties of player, edit this if you want for player to store additional data
 */
public class Player {
    private SimpleStringProperty name;
    private SimpleIntegerProperty score;
    private int iron;

    private Color color;

    public Player(String name, Color color, int iron) {
        this.name = new SimpleStringProperty(name);
        score = new SimpleIntegerProperty(0);
        this.color = color;
        this.iron = iron;
    }

    /**
     * You have to define these property methods, otherwise table won't be updated
     * http://stackoverflow.com/questions/10912690/autoupdating-rows-in-tableview-from-model
     */
    public SimpleStringProperty nameProperty(){
        return name;
    }

    public SimpleIntegerProperty scoreProperty() {
        return score;
    }

    public void setScore(int score) {
        this.score.set(score);
    }

    public String getName() {
        return name.get();
    }

    public int getScore() {
        return score.get();
    }

    public Color getColor() {
        return color;
    }

    public int getIron() {
        return iron;
    }

    public void setIron(int iron) {
        this.iron = iron;
    }
}
