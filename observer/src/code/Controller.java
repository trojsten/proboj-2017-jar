package code;

import javafx.application.Platform;
import javafx.scene.Node;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.text.Text;
import javafx.scene.text.TextFlow;

public class Controller {
    public TextFlow consoleArea;
    public TextField commandLine;
    public TableView<TableColumn<String, String>> table;
    public GridPane gridPane;
    public Pane pane;
    public ScrollPane scrollPaneConsoleArea;

    public void commandLineTyped(KeyEvent event) {
        //we are on FX application thread, no need to call Platform.runLater
        if (event.getCode() == KeyCode.ENTER){
            String command = commandLine.getText();
            Text text = new Text(command + "\n");

            //TODO make command line
            switch (command) {
                case "clear":
                    consoleArea.getChildren().clear();
                    break;
                default:
                    consoleArea.getChildren().add(text);
            }
            commandLine.clear();
        }
    }

    public void addTextToConsole(Node t){
        Platform.runLater(()->{
            while (consoleArea.getChildren().size() > 1000)consoleArea.getChildren().remove(0);
            consoleArea.getChildren().add(t);
        });
    }

    public void addTextToConsole(String t){
        addTextToConsole(new Text(t));
    }

    public void addErrorText(String s) {
        Text text = new Text(s);
        text.setFill(Color.RED);
        addTextToConsole(text);
    }

    public void initialize() {
        consoleArea.setCache(true);
    }
}

