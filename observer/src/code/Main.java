package code;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonElement;
import com.google.gson.JsonPrimitive;
import com.google.gson.reflect.TypeToken;
import java.awt.Dimension;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

public class Main extends Application {

  @Override
  public void start(Stage primaryStage) throws Exception {
    Settings settings = new Settings();
    loadSettings(settings);

    FXMLLoader loader;
    if (settings.inDevMode()) {
      loader = new FXMLLoader(getClass().getResource("developer.fxml"));
    } else {
      loader = new FXMLLoader(getClass().getResource("observer.fxml"));
    }
    Parent root = loader.load();
    Controller controller = loader.getController();
    controller.setSettings(settings);
    controller.initialize();

    int width = settings.getWindowWidth();
    int height = settings.getWindowHeight();
    Scene scene = new Scene(root, width, height);
    primaryStage.getIcons().add(new Image("res/icon.png"));
    primaryStage.setScene(scene);
    primaryStage.setOnCloseRequest(event -> System.exit(0));

    Path currentPath = Paths.get(System.getProperty("user.dir"));
    Path path = Paths.get(currentPath.toString(), "styles", "style.css");
    if (Files.exists(path)) {
      root.getStylesheets().add(path.toUri().toURL().toExternalForm());
    }
    if (settings.isWindowMaximized()) {
      primaryStage.setMaximized(true);
    }
    primaryStage.show();

    ObserverGraphics graphics = new ObserverGraphics(controller);
    ObserverLogic loop = new ObserverLogic(primaryStage, graphics, controller, settings);
  }

  private void loadSettings(Settings settings) {
    Map<String, JsonElement> namedOptions = new HashMap<>();
    ArrayList<String> other = new ArrayList<>();
    Path currentPath = Paths.get(System.getProperty("user.dir"));
    Path path = Paths.get(currentPath.toString(), "global.json");
    //load json
    try {
      Gson gson = new GsonBuilder().setPrettyPrinting().create();
      BufferedReader reader = new BufferedReader(new FileReader(path.toFile()));
      namedOptions = gson.fromJson(reader, new TypeToken<Map<String, JsonElement>>() {
      }.getType());
      //map.forEach((x,y)-> System.out.println("key : " + x + " , value : " + y));
    } catch (FileNotFoundException ex) {
      System.err.printf("Couldn't find: %s, using fixed settings.\n", currentPath.relativize(path));
    }
    //load command line arg
    for (String arg : this.getParameters().getRaw()) {
      if (arg.startsWith("--")) {
        String[] op = arg.substring(2).split("=");
        namedOptions.put(op[0], new JsonPrimitive(op[1]));
      } else {
        other.add(arg);
      }
    }

    namedOptions.forEach((key, value) -> {
      try {
        switch (key) {
          case "mode":
            settings.setMode(value.getAsString());
            break;
          case "resolution":
            settings.setResolution(new Gson().fromJson(value, Dimension.class));
            break;
          case "frameTime":
            settings.setFrameTime(value.getAsInt());
            break;
          case "maximize":
            settings.setMaximized(value.getAsBoolean());
            break;
          case "fullscreen":
            settings.setFullscreen(value.getAsBoolean());
            break;
          case "frameTimeStep":
            settings.setFrameStepRate(value.getAsDouble());
            break;
          case "incSpeed":
            settings.setIncSpeedCombination(value.getAsString());
            break;
          case "decSpeed":
            settings.setDecSpeedCombination(value.getAsString());
            break;
          case "pause":
            settings.setPauseCombination(value.getAsString());
            break;
          case "useSmoothing":
            settings.setUseSmoothing(value.getAsBoolean());
            break;
          case "maxConsoleMessages":
            settings.setMaxConsoleMessages(value.getAsInt());
            break;
          case "maxMessageLength":
            settings.setMaxMessageLength(value.getAsInt());
            break;
          case "title":
            settings.setTitle(value.getAsString());
            break;
          case "minimap":
            settings.setMinimapEnabled(value.getAsBoolean());
            break;
          case "size":
            settings.setSquareSize(value.getAsInt());
            break;
          default:
            System.err.printf("Ignoring unknown option: %s\n", key);
        }
      } catch (Exception ex) {
        System.err.printf("Error while setting '%s' to '%s'\n", key, value);
      }
    });
    //TODO use only first, but keep others, maybe useful later?
    if (other.size() != 0) {
      settings.setObsFileDir(other.get(0));
    }
  }

  public static void main(String[] args) {
    launch(args);
  }
}
