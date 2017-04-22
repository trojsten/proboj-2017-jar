package code;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.net.MalformedURLException;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;
import javafx.animation.Animation;
import javafx.animation.AnimationTimer;
import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.Parent;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.image.Image;
import javafx.scene.image.PixelReader;
import javafx.scene.image.WritableImage;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;
import javafx.stage.Stage;
import javafx.util.Duration;
import util.ConsoleTextCreator;

/**
 * Created by siegrift on 12/24/16.
 */
public class ObserverLogic implements Runnable {

  private final Stage root;
  private final Parent sceneRoot;
  ObserverGraphics obs;
  Controller controller;
  Settings settings;

  Scanner obsReader;
  BufferedReader[] logReaders;

  //Fraction, used to get the fraction of current frame progression
  SimpleDoubleProperty frac = new SimpleDoubleProperty(0);
  ReentrantLock animationLock;
  Condition timelineFinished;
  ReentrantLock consoleLock = new ReentrantLock();
  Condition roundFinished = consoleLock.newCondition();
  Timeline timeline = new Timeline();
  AnimationTimer timer;
  ArrayList<CanvasAnimation> animations = new ArrayList<>();

  ReentrantLock pauseLock;
  Condition gamePausedCondition;

  private boolean gameRunning = true;
  private int roundNumber = 1;
  private boolean gamePaused = false;


  ObserverLogic(Stage root, ObserverGraphics graphics, Controller controller, Settings settings) {
    this.root = root;
    this.sceneRoot = root.getScene().getRoot();
    obs = graphics;
    this.controller = controller;
    this.settings = settings;
    try {
      new Thread(this).start();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  int totalPlayers;
  Player[] players;

  int rows, cols;

  @Override
  public void run() {
    try {
      initialize();
      while (gameRunning) {
        obs.dispatch(() -> root.setTitle(String.format("%s [Kolo %d]", settings.getTitle(), roundNumber)));
        while (gamePaused) {
          pauseLock.lock();
          while (gamePaused) {
            gamePausedCondition.await();
          }
          // we don't have to unlock here, because no thread is waiting for this monitor
          pauseLock.unlock();
        }
        long beforeRender = System.currentTimeMillis();
        createTimeline();
        long t = System.currentTimeMillis();
        process();
        //System.out.println("PROCCESS: " + (System.currentTimeMillis() - t));
        //t = System.currentTimeMillis();
        obs.repaint();
        //System.out.println("REPAINT: " + (System.currentTimeMillis() - t));
        playAnimations();

        long renderTime = (System.currentTimeMillis() - beforeRender);
        long sleepTime = settings.getFrameTime() - renderTime;
        // System.out.println(sleepTime);
        if (sleepTime < 0) {
          System.err.println("Warning: Sleep time negative!");
        }
        //System.out.println("SPIM: "+ Math.max(sleepTime, 50));
        Thread.sleep(Math.max(sleepTime, 50));
        roundNumber++;
        consoleLock.lock();
        roundFinished.signal();
        consoleLock.unlock();
      }
      obs.dispatch(() -> root.setTitle(String.format("%s [Koniec]", settings.getTitle())));
      gameFinished();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  private void gameFinished() {
    // otvor graf a zomri po 5 sec
    Image graph = null;
    try {
      graph = new Image(Paths.get(settings.getObsDir(), "graph.png").toUri().toURL().toString());
    } catch (MalformedURLException e) {
      e.printStackTrace();
    }
    obs.drawImage(graph, new Rectangle(0,0), null);
    obs.repaint();
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      e.printStackTrace();
    }
    System.exit(0);
  }

  private void createTimeline() throws InterruptedException {
    long curTime = System.nanoTime();
    //if current time is still running wait till it ends
    if (animationLock != null) {
      animationLock.lock();
      while (timeline.getStatus() == Animation.Status.RUNNING) {
        timelineFinished.await();
      }
      // we don't need to unlock, because there is no thread waiting for this monitor
      animationLock.unlock();
    }
    animations = new ArrayList<>();
    long waitTime = Math.min(settings.getFrameTime(), (System.nanoTime() - curTime) / 1000000);
    //shorten the animation byt the time we had to wait for the last animation to finish
    timeline = new Timeline(
        new KeyFrame(Duration.ZERO, new KeyValue(frac, 0)),
        new KeyFrame(Duration.millis(settings.getFrameTime() - waitTime), new KeyValue(frac, 1))
    );
    animationLock = new ReentrantLock();
    timelineFinished = animationLock.newCondition();

    timeline.play();
    //we manually force to stop animation, which can cause animation not to be renderer to full state
    //we force it to update, when timeline finishes
    timeline.setOnFinished(e -> {
      if (animationLock != null) {
        animationLock.lock();
        if (timer != null) {
          timer.stop();
        }
        //set fraction to 1 as it is the last val
        frac.set(1);
        Platform.runLater(() -> {
          synchronized (animations) {
            for (CanvasAnimation a : animations) {
              a.animate();
            }
          }
        });
        timelineFinished.signal();
        animationLock.unlock();
      }
    });
  }

  private void playAnimations() {
    timer = new AnimationTimer() {
      @Override
      public void handle(long now) {
        synchronized (animations) {
          for (CanvasAnimation a : animations) {
            // TODO this may not be in an runLater method
            Platform.runLater(() -> {
              a.animate();
            });
          }
        }
      }
    };
    timer.start();
  }

  /**
   * This is the method where update the game, and tell program what to paint
   */
  private final int TRAVA = 0, KAMEN = 1, VODA = 2, LAB = 3, LAB_SPAWN = 5, MESTO = 4;
  private int SIZE; // initialized in header

  private void process() throws Exception {
    if (obsReader.hasNextInt() == false) {
      sendGameFinishedRequest();
      return;
    }
    int zbyt;
    // round, dimensions, num players
    zbyt = obsReader.nextInt();
    zbyt = obsReader.nextInt();
    zbyt = obsReader.nextInt();
    zbyt = obsReader.nextInt();
    for (int i = 0; i < totalPlayers; i++) {
      players[i].setIron(obsReader.nextInt());
    }
    //num players
    zbyt = obsReader.nextInt();
    for (int i = 0; i < totalPlayers; i++) {
      players[i].setScore(obsReader.nextInt());
    }

    obs.drawImage((Image) obs.fromSaved("terrain"), new Rectangle(0,0), null);
    // rows
    zbyt = obsReader.nextInt();
    for (int i = 0; i < rows; i++) {
      // cols
      zbyt = obsReader.nextInt();
      for (int j = 0; j < cols; j++) {
        int owner = obsReader.nextInt();
        int robot = obsReader.nextInt();
        if (owner != -1) {
          Color c = players[owner].getColor();
          Color fill = new Color(c.getRed(), c.getGreen(), c.getBlue(), 0.7);
          obs.fillRect(new Rectangle(i * SIZE, j * SIZE, SIZE, SIZE), fill);
        }
        if (robot != 0) {
          // smaller radius is better
          Circle sh = new Circle(i * SIZE + SIZE / 2, j * SIZE + SIZE / 2, SIZE / 4);
          obs.fillShape(sh, Color.BLACK, null);
          /*double X = sh.getCenterX();
          animations.add(() -> {
            sh.setCenterX(X - SIZE*frac.getValue());
          });*/
          int w = SIZE / 3;
          obs.drawBoundedText(String.valueOf(robot),
              new Rectangle(i * SIZE, j * SIZE, 2 * w, 2 * w), Color.RED);
        }
      }
    }
  }

  private void initialize() throws Exception {
    if (settings.getObsDir() == null) {
      System.err.println("Observation dir not specified, showing only graphics!");
      gameRunning = false;
      return;
    }
    System.err.println("Opening: " + Paths.get(settings.getObsDir(), "observation").toString());
    //open reader
    obsReader = new Scanner(new BufferedReader(
        new FileReader((Paths.get(settings.getObsDir(), "observation").toFile()))));
    // TODO remove
    SIZE = settings.getSquareSize();
    initializeTextures();
    readHeader();
    initializeTable();
    if (settings.isMinimapEnabled()) {
      initializeMinimap();
    }
    if (settings.inDevMode()) {
      initializeBotLogs();
    }
    addKeyEventListeners(sceneRoot);
  }

  private void initializeMinimap() {
    new Thread(() -> {
      // TODO use settings
      double maxV = Math.max(obs.getWidth(), obs.getHeight());
      double scale = 250 /  maxV;
      double W = obs.getWidth() * scale, H = obs.getHeight() * scale;
      controller.minimap = new Canvas(W, H);
      obs.dispatch(() -> {
        controller.minimapWrapper.getChildren().add(controller.minimap);
      });

      while (true) {
        obs.dispatch(() -> {
          Image im = controller.pane.snapshot(null, null);
          PixelReader reader = im.getPixelReader();
          int w = Math.max((int) obs.getWidth(), 1);
          int h = Math.max((int) obs.getHeight(), 1);
          WritableImage newImage = new WritableImage(reader, 0, 0, w, h);
          GraphicsContext g = controller.minimap.getGraphicsContext2D();
          g.clearRect(0,0, W, H);
          g.drawImage(newImage, 0, 0, W, H);
        });
        // TODO sync with fps
        try {
          Thread.sleep(500);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }
      }
  }).start();
}

  private void initializeTextures() {
    obs.save(obs.loadImage("res/grass.jpg", SIZE, SIZE), "grass");
    obs.save(obs.loadImage("res/rock.jpg", SIZE, SIZE), "rock");
    obs.save(obs.loadImage("res/sea2.jpg", SIZE, SIZE), "sea");
    obs.save(obs.loadImage("res/gold.png", SIZE, SIZE), "ironMine");
    obs.save(obs.loadImage("res/city.png", SIZE, SIZE), "spawn");
  }

  private void addKeyEventListeners(Parent node) {
    // it's enough to set event only here, no need for recursion
    node.setOnKeyPressed(event -> {
      if (settings.getIncSpeedCombination().match(event)) {
        settings.setFrameTime((int) (settings.getFrameTime() / settings.getFrameStepRate()));
      } else if (settings.getDecSpeedCombination().match(event)) {
        settings.setFrameTime((int) (settings.getFrameTime() * settings.getFrameStepRate()));
      } else if (settings.getPauseCombination().match(event)) {
        gamePaused = !gamePaused;
        if (gamePaused) {
          pauseLock = new ReentrantLock();
          gamePausedCondition = pauseLock.newCondition();
        } else {
          pauseLock.lock();
          gamePausedCondition.signal();
          pauseLock.unlock();
        }
      }
    });
  }

  /**
   * Edit this method to read the initial content of observation file
   */
  private void readHeader() throws Exception {
    totalPlayers = obsReader.nextInt();
    rows = obsReader.nextInt();
    cols = obsReader.nextInt();
    players = new Player[totalPlayers];
    // this number?
    int zbyt = obsReader.nextInt();
    for (int i = 0; i < rows; i++) {
      // this number?
      zbyt = obsReader.nextInt();
      for (int j = 0; j < cols; j++) {
        int type = obsReader.nextInt();
        switch (type) {
          case TRAVA:
            obs.drawImage((Image) obs.fromSaved("grass"), i * SIZE, j * SIZE);
            break;
          case KAMEN:
            obs.drawImage((Image) obs.fromSaved("rock"), i * SIZE, j * SIZE);
            break;
          case VODA:
            obs.drawImage((Image) obs.fromSaved("sea"), i * SIZE, j * SIZE);
            break;
          case LAB:
          case LAB_SPAWN:
            obs.drawImage((Image) obs.fromSaved("grass"), i * SIZE, j * SIZE);
            obs.drawImage((Image) obs.fromSaved("spawn"), i * SIZE, j * SIZE);
            break;
          case MESTO:
            obs.drawImage((Image) obs.fromSaved("grass"), i * SIZE, j * SIZE);
            obs.drawImage((Image) obs.fromSaved("ironMine"), i * SIZE, j * SIZE);
            break;
          default:
            System.out.println("ERROR: NEPOZNANE POLICKO" + type);
        }
      }
    }
    for (int i = 0; i < totalPlayers; i++) {
      String name = obsReader.next();
      double r, g, b, a;
      r = obsReader.nextDouble();
      g = obsReader.nextDouble();
      b = obsReader.nextDouble();
      a = obsReader.nextDouble();
      players[i] = new Player(name, new Color(r, g, b, a), 0);
    }
    obs.setCanvasSize(rows * SIZE, cols * SIZE);
    obs.save(obs.getSnapshot(), "terrain");
    System.err.println(rows + " " + cols);
  }

  private void initializeTable() {
    TableView table = controller.table;
    TableColumn name = (TableColumn) table.getColumns().get(0);
    TableColumn score = (TableColumn) table.getColumns().get(1);
    ObservableList<Player> data = FXCollections.observableArrayList(players);
    Platform.runLater(() -> {
      name.setCellValueFactory(new PropertyValueFactory<Player, String>("name"));
      score.setCellValueFactory(new PropertyValueFactory<Player, String>("score"));
      table.setItems(data);
    });
  }

  /**
   * Bot logs will be files with 'playername'.log
   */
  private void initializeBotLogs() {
    logReaders = new BufferedReader[totalPlayers];
    for (int i = 0; i < totalPlayers; i++) {
      String filename = players[i].getName() + ".log";
      System.err.printf("BOTLOG %d: %s\n", i, filename);
      try {
        logReaders[i] = new BufferedReader(
            new FileReader(Paths.get(settings.getObsDir(), filename).toFile()));
      } catch (FileNotFoundException e) {
        controller.addErrorText(String.format("Neviem najst %s!\n", filename));
      }
    }
    readBotLogsInBackground();
  }

  private void readBotLogsInBackground() {
    new Thread(() -> {
      try {
        // bot logs round number is not the same as roundNumber
        int botLogRoundNumber = 1;
        while (!readersEmpty()) {
          int prevRound = roundNumber;
          consoleLock.lock();
          while (prevRound == roundNumber) {
            roundFinished.await();
          }
          consoleLock.unlock();

          controller.addTextToConsole(String.format("-----KOLO %d-----\n", botLogRoundNumber));
          for (int i = 0; i < totalPlayers; i++) {
            if (logReaders[i] == null) {
              continue;
            }
            String line = logReaders[i].readLine();
            ConsoleTextCreator creator = new ConsoleTextCreator(settings, players[i]);
            while (line != null && !line.equals(settings.BOT_LOG_DELIMETER)) {
              creator.appendMessage(line).appendMessage("\n");
              line = logReaders[i].readLine();
            }
            if (creator.empty() == false) {
              controller.addTextToConsole(creator.createBotInfo());
              controller.addTextToConsole(creator.createBotPlainText());
            }
          }
          botLogRoundNumber++;
          Thread.sleep(5);
        }
        controller.addTextToConsole(String.format("-----KONIEC BOT LOGOV-----\n"));
      } catch (IOException e) {
        e.printStackTrace();
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }).start();
  }

  private boolean readersEmpty() throws IOException {
    for (BufferedReader reader : logReaders) {
      if (reader != null && reader.ready() == true) {
        return false;
      }
    }
    return true;
  }

  private void sendGameFinishedRequest() {
    gameRunning = false;
  }
}
