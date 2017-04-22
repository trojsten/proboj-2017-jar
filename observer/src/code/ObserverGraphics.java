package code;

import java.util.HashMap;
import java.util.Vector;
import javafx.application.Platform;
import javafx.geometry.Bounds;
import javafx.geometry.VPos;
import javafx.scene.Node;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.image.WritableImage;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.shape.Shape;
import javafx.scene.text.Font;
import javafx.scene.text.FontSmoothingType;
import javafx.scene.text.Text;
import javafx.scene.text.TextAlignment;
import javafx.scene.text.TextBoundsType;
import util.ObserverObjects;

/**
 * Created by siegrift on 12/24/16.
 * <p>
 * You shouldn't edit this class, it just serves as a template class.
 * Call these methods after initializing this class with controller.
 */

public class ObserverGraphics {

  public Controller controller;
  private Pane pane;
  private ObserverObjects observerObjects;
  private Color color;
  private Vector<Node> children = new Vector<>();
  private HashMap<String, Object> saved = new HashMap<>();

  ObserverGraphics(Controller controller) {
    observerObjects = new ObserverObjects();
    this.controller = controller;
    this.pane = controller.pane;
    this.color = Color.BLACK;
  }

  public void setColor(Color color) {
    this.color = color;
  }


  public void setCanvasSize(double width, double height) {
    // setting pref size won't trigger scroll pane to be shown
    pane.setMinSize(width, height);
  }

  public double getWidth() {
    return pane.getMinWidth();
  }

  public double getHeight() {
    return pane.getMinHeight();
  }

  public Node getObserverObject(String key) {
    return observerObjects.getAcessible(key);
  }

  public void fillRect(Rectangle rect, Color c, String key) {
    if (key != null) {
      observerObjects.addAccessible(key, rect);
    }
    rect.setFill(c);
    children.add(rect);
  }

  public void fillRect(Rectangle rect, Color c) {
    fillRect(rect, c, null);
  }

  public void fillRect(Rectangle rect) {
    fillRect(rect, color);
  }

  public void drawBoundedText(String s, Rectangle bounds, Color c) {
    Text text = new Text(s);
    text.setFontSmoothingType(FontSmoothingType.LCD);
    text.setTextAlignment(TextAlignment.CENTER);
    text.setTextOrigin(VPos.CENTER);
    text.setFill(c);
    // TODO change
    text.setFont(new Font("Verdana", 100));
    text.setBoundsType(TextBoundsType.LOGICAL_VERTICAL_CENTER);
    Bounds textBounds = text.getBoundsInLocal();
    // TODO use equal scaling or separate
    double scale = Math.min(bounds.getWidth() / textBounds.getWidth(),
        bounds.getHeight() / (text.getLayoutBounds().getHeight()));
    text.setScaleX(bounds.getWidth() / textBounds.getWidth());
    text.setScaleY(bounds.getHeight() / textBounds.getHeight());
    // we have to move it to (x,y) pos, image is centered, however during scaling, layout bounds change and we have to subtract them
    text.setLayoutX(
        bounds.getX() + bounds.getWidth() / 2 - text.getLayoutBounds().getWidth() / 2);
    text.setLayoutY(bounds.getY() + bounds.getHeight() / 2);
    children.add(text);
  }

  public void fillShape(Shape cc, Color c, String key) {
    if (key != null) {
      observerObjects.addAccessible(key, cc);
    }
    cc.setFill(c);
    children.add(cc);
  }

  public void dispatch(Runnable r) {
    Platform.runLater(r);
  }

  public void addNode(Node t) {
      children.add(t);
  }

  public Image loadImage(String path, double width, double height) {
    return new Image(path, width, height, false, true);
  }

  public void drawImage(Image image, Rectangle bounds, String key) {
    ImageView view = new ImageView(image);
    view.relocate(bounds.getX(), bounds.getY());
    if (key != null) {
      observerObjects.addAccessible(key, view);
    }
    children.add(view);
  }

  public void clear() {
    dispatch(() -> {
      pane.getChildren().clear();
    });
  }

  public void repaint() {
    dispatch(() -> {
      synchronized (children) {
        pane.getChildren().setAll(children);
        children.clear();
      }
    });
  }

  public void save(Image image, String key) {
    saved.put(key, image);
  }

  public Object fromSaved(String key) {
    return saved.get(key);
  }

  public void drawImage(Image image, double x, double y) {
    drawImage(image, new Rectangle(x, y, image.getWidth(), image.getHeight()), null);
  }

  public Image getSnapshot() {
    WritableImage img = new WritableImage((int)getWidth(), (int)getHeight());
    dispatch(() -> {
      synchronized (children) {
        pane.getChildren().setAll(children);
        children.clear();
        pane.snapshot(null, img);
      }
    });
    return img;
  }
}
