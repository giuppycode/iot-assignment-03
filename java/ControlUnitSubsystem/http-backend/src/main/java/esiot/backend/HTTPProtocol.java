package esiot.backend;

import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.client.WebClient;

public class HTTPProtocol {
    private final WebClient client;
    private final int port;

    public HTTPProtocol(WebClient client, int port) {
        this.client = client;
        this.port = port;
    }

    public void sendData(float value, String place) {
        JsonObject item = new JsonObject()
                .put("value", value)
                .put("place", place);

        client.post(port, "localhost", "/api/data")
                .sendJson(item)
                .onSuccess(res -> System.out.println("[HTTP] Dato salvato ok"))
                .onFailure(err -> System.out.println("[HTTP] Errore: " + err.getMessage()));
    }

    public void sendMode(String mode) {
        JsonObject body = new JsonObject().put("mode", mode);
        client.post(port, "localhost", "/api/status")
                .sendJson(body)
                .onSuccess(r -> System.out.println("[HTTP] Mode synced to DataService: " + mode))
                .onFailure(err -> System.out.println("[HTTP] Mode sync error: " + err.getMessage()));
    }

    public void sendValve(int percent) {
        JsonObject body = new JsonObject().put("percent", percent);
        client.post(port, "localhost", "/api/valve")
                .sendJson(body)
                .onSuccess(r -> System.out.println("[HTTP] Valve " + percent + "% saved"))
                .onFailure(err -> System.out.println("[HTTP] Valve save error: " + err.getMessage()));
    }

    public void getStatus(Callback<String> callback) {
        client.get(port, "localhost", "/api/status")
                .send()
                .onSuccess(res -> {
                    String mode = res.bodyAsJsonObject().getString("mode");
                    callback.onSuccess(mode);
                })
                .onFailure(err -> callback.onFailure(err.getMessage()));
    }

    public void getData(Callback<JsonArray> callback) {
        client.get(port, "localhost", "/api/data")
                .send()
                .onSuccess(res -> callback.onSuccess(res.bodyAsJsonArray()))
                .onFailure(err -> callback.onFailure(err.getMessage()));
    }

    public interface Callback<T> {
        void onSuccess(T result);
        void onFailure(String error);
    }
}
