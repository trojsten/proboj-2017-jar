package util;

import javafx.scene.Node;

import java.util.HashMap;
import java.util.HashSet;

/**
 * Created by siegrift on 1/28/17.
 */
public class ObserverObjects {
    private HashMap<String, Node> acessibleObjects;
    private HashSet<Node> allObjects;

    public ObserverObjects() {
        acessibleObjects = new HashMap<>();
        allObjects = new HashSet<>();
    }

    public void add(Node node) {
        allObjects.add(node);
    }

    public void addAccessible(String key, Node node) {
        acessibleObjects.put(key, node);
        add(node);
    }

    public Node getAcessible(String key) {
        return acessibleObjects.get(key);
    }

    public void removeAccessible(String key) {
        Node n = getAcessible(key);
        acessibleObjects.remove(key);
        allObjects.remove(n);
    }

    public void removeAll() {
        allObjects.clear();
        acessibleObjects.clear();
    }
}
