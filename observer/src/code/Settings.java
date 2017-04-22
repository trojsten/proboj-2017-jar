package code;

import javafx.scene.input.KeyCodeCombination;

import java.awt.*;

/**
 * Created by siegrift on 12/26/16.
 * <p>
 * This class holds all settings that can be specified to observer,
 * we pass default values here (as option if no settings are provided).
 */
public class Settings {
  //TODO create System settings (unchangable)
  public static final String DEV_MODE = "developer";
  public static final String OBS_MODE = "observer";
  public static final String BOT_LOG_DELIMETER = "---";
  private String obsMode = DEV_MODE;
  private String obsDir = null;
  private Dimension resolution = new Dimension(800, 600);
  private int frameTime = 500;
  private boolean fullscreen = false;
  private boolean windowMaximized = false;
  private double frameStepRate = 1.2;
  private KeyCodeCombination pauseCombination = (KeyCodeCombination) KeyCodeCombination
      .keyCombination("Ctrl+Space");
  private KeyCodeCombination incSpeedCombination = (KeyCodeCombination) KeyCodeCombination
      .keyCombination("Ctrl+Add");
  private KeyCodeCombination decSpeedCombination = (KeyCodeCombination) KeyCodeCombination
      .keyCombination("Ctrl+Subtract");
  private boolean useSmoothing = true;
  private int maxConsoleMessages = 1500;
  private String title = "Observer";
  private int maxMessageLength = 2000;
  private boolean minimapEnabled = true;
  private int squareSize = 20;

  public int getSquareSize() {
    return squareSize;
  }

  public void setSquareSize(int squareSize) {
    this.squareSize = squareSize;
  }

  public boolean isMinimapEnabled() {
    return minimapEnabled;
  }

  public void setMinimapEnabled(boolean minimapEnabled) {
    this.minimapEnabled = minimapEnabled;
  }

  public String getTitle() {
    return title;
  }

  public void setTitle(String title) {
    this.title = title;
  }

  public void setUseSmoothing(boolean useSmoothing) {
    this.useSmoothing = useSmoothing;
  }

  public int getMaxConsoleMessages() {
    return maxConsoleMessages;
  }

  public void setMaxConsoleMessages(int maxConsoleMessages) {
    this.maxConsoleMessages = maxConsoleMessages;
  }

  public int getMaxMessageLength() {
    return maxMessageLength;
  }

  public void setMaxMessageLength(int maxMessageLength) {
    this.maxMessageLength = maxMessageLength;
  }

  public KeyCodeCombination getPauseCombination() {
    return pauseCombination;
  }

  public void setPauseCombination(String pauseCombination) {
    this.pauseCombination = (KeyCodeCombination) KeyCodeCombination
        .keyCombination(pauseCombination);
  }

  public KeyCodeCombination getIncSpeedCombination() {
    return incSpeedCombination;
  }

  public void setIncSpeedCombination(String incSpeedCombination) {
    this.incSpeedCombination = (KeyCodeCombination) KeyCodeCombination
        .keyCombination(incSpeedCombination);
  }

  public KeyCodeCombination getDecSpeedCombination() {
    return decSpeedCombination;
  }

  public void setDecSpeedCombination(String decSpeedCombination) {
    this.decSpeedCombination = (KeyCodeCombination) KeyCodeCombination
        .keyCombination(decSpeedCombination);
  }

  public void setFrameStepRate(double frameStepRate) {
    this.frameStepRate = frameStepRate;
  }


  public void setMode(String mode) {
    this.obsMode = mode;
  }

  public void setObsFileDir(String obsFileDir) {
    this.obsDir = obsFileDir;
  }

  public boolean inDevMode() {
    return obsMode.equals(DEV_MODE);
  }

  public String getObsDir() {
    return obsDir;
  }

  public void setResolution(Dimension resolution) {
    this.resolution = resolution;
  }

  public void setFrameTime(Integer frameTime) {
    this.frameTime = Math.min(Math.max(frameTime, 10), 5000);
  }

  public void setFullscreen(Boolean fullscreen) {
    this.fullscreen = fullscreen;
  }

  public void setMaximized(boolean maximized) {
    this.windowMaximized = maximized;
  }

  public boolean isWindowMaximized() {
    return windowMaximized;
  }

  public int getWindowWidth() {
    return (int) resolution.getWidth();
  }

  public int getWindowHeight() {
    return (int) resolution.getHeight();
  }

  public int getFrameTime() {
    return frameTime;
  }

  public double getFrameStepRate() {
    return frameStepRate;
  }
}
