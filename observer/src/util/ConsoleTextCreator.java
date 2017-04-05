package util;

import code.Player;
import code.Settings;
import javafx.scene.Node;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.text.Text;
import javafx.scene.text.TextFlow;

import java.util.ArrayDeque;
import java.util.Deque;

/**
 * Created by siegrift on 1/30/17.
 */
public class ConsoleTextCreator {
    private final Settings settings;
    private final Player player;
    private final Deque<Character> buffer;


    public ConsoleTextCreator(Settings settings, Player player) {
        this.settings = settings;
        this.player = player;
        buffer = new ArrayDeque<>(settings.getMaxMessageLength());
        // use space for nicer input (should be ok with performance)
        buffer.add(' ');
    }

    public Text createBotPlainText() {
        return new Text(getMessageFromBuffer());
    }

    private String getMessageFromBuffer() {
        StringBuilder b = new StringBuilder();
        for (Character ch : buffer) {
            b.append(ch);
        }
        return b.toString();
    }

    public Node createBotInfo() {
        // TODO this may cause preforamnce issues
        TextFlow flow = new TextFlow();
        Text info = new Text(getBotInfoName());
        flow.setBackground(new Background(new BackgroundFill(new TransparentColorCreator(player.getColor(), 0.5).create(), null, null)));
        flow.getChildren().add(info);
        return flow;
    }

    private String getBotInfoName() {
        return String.format("[%s]:", player.getName());
    }

    public ConsoleTextCreator appendMessage(String line) {
        for (int i = 0; i < line.length(); i++) {
            while (buffer.size() >= settings.getMaxMessageLength())buffer.poll();
            buffer.add(line.charAt(i));
        }
        return this;
    }

    // use 1 because of the space in the beggining
    public boolean empty() {
        return buffer.size() == 1;
    }
}